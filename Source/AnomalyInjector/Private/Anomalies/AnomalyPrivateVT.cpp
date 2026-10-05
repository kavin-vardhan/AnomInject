#include "Anomalies/AnomalyPrivateVT.h"
#include "Engine/Texture2D.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyVTPageCheckShader.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MaterialShared.h"
#include "HAL/PlatformProcess.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "ShaderParameterStruct.h"
#include "EngineModule.h"
#include "HAL/PlatformTime.h"
#include "HAL/ThreadSafeCounter.h"
#include "PixelFormat.h"
#include "RendererInterface.h"
#include "RenderingThread.h"
#include "RHIGPUReadback.h"
#include "RHI.h"
#include "RHICommandList.h"
#include "RHIResources.h"
#include "TextureResource.h"
#include "Templates/Atomic.h"
#include "VirtualTexturing.h"

struct FAnomalyVTPrefetch
{
	struct FReadback
	{
		TUniquePtr<FRHIGPUBufferReadback> Reader;
		uint32 Levels[4] = {0, 0, 0, 0};
		uint32 Layers = 0;
		uint32 Width = 0;
		uint32 Height = 0;
	};
	TArray<FReadback> Readbacks;
	TAtomic<int32> Status{0};
	TAtomic<int32> Attempts{0};
	TAtomic<int32> Pages{0};
	TAtomic<bool> InFlight{false};
	bool bRootReady = false;
	IAllocatedVirtualTexture* Allocation = nullptr;
	FString Failure;
	TArray<uint32> ProducerBiases;
	double Started = FPlatformTime::Seconds();
};

namespace AnomalyPrivateVTDetail
{
	FThreadSafeCounter ResourceCount;
	FThreadSafeCounter ProducerCount;

	class FAnomalyPrivateVTResource final : public FVirtualTexture2DResource
	{
	public:
		FAnomalyPrivateVTResource(const UTexture2D* Source, FVirtualTextureBuiltData* Data, int32 FirstMip)
			: FVirtualTexture2DResource(Source, Data, FirstMip)
		{
			ResourceCount.Increment();
		}

		virtual ~FAnomalyPrivateVTResource() override
		{
			ResourceCount.Decrement();
		}

		virtual void InitRHI() override
		{
			FVirtualTexture2DResource::InitRHI();
			bCountedProducer = GetProducerHandle().IsValid();
			if (bCountedProducer) { ProducerCount.Increment(); }
		}

		virtual void ReleaseRHI() override
		{
			FVirtualTexture2DResource::ReleaseRHI();
			if (bCountedProducer) { ProducerCount.Decrement(); }
			bCountedProducer = false;
		}

#if WITH_EDITOR
		virtual void InitializeEditorResources(IVirtualTexture* InVirtualTexture) override {}
#endif

	private:
		bool bCountedProducer = false;
	};
}

bool UAnomalyPrivateVT::Initialize(UTexture2D* InSource, int32 InDrop, FString& OutReason)
{
	if (!InSource || !InSource->IsCurrentlyVirtualTextured() || InDrop < 0)
	{
		OutReason = TEXT("virtual_texture:source_type");
		return false;
	}
	FTexturePlatformData* Data = InSource->GetPlatformData();
	const FTextureResource* SourceResource = InSource->GetResource();
	const FVirtualTexture2DResource* VT = SourceResource ? SourceResource->GetVirtualTexture2DResource() : nullptr;
	if (!Data || !Data->VTData || !VT || !SourceResource->IsInitialized() || InSource->HasPendingInitOrStreaming(false))
	{
		OutReason = TEXT("virtual_texture:source_not_ready");
		return false;
	}
	const int32 SourceFirstMip = InSource->GetNumMips() - static_cast<int32>(VT->GetNumMips());
	const int32 MaximumDrop = FMath::Min(FMath::CeilLogTwo(FMath::Max(VT->GetNumTilesX(), VT->GetNumTilesY())),
		FMath::FloorLog2(FMath::Max(1u, VT->GetSizeX() / FMath::Max(1u, VT->GetTileSize()))));
	if (SourceFirstMip < 0 || InDrop > MaximumDrop || VT->GetNumLayers() == 0 || VT->GetTileSize() == 0)
	{
		OutReason = TEXT("virtual_texture:invalid_layout");
		return false;
	}
	SourcePin.Reset(InSource);
	BorrowedData = Data->VTData;
	FirstMip = SourceFirstMip;
	Drop = InDrop;
	Width = FMath::Max(1, static_cast<int32>(VT->GetSizeX()) >> Drop);
	Height = FMath::Max(1, static_cast<int32>(VT->GetSizeY()) >> Drop);
	TileSize = VT->GetTileSize();
	AddressU = InSource->AddressX;
	AddressV = InSource->AddressY;
	SRGB = InSource->SRGB;
	Filter = InSource->Filter;
	LODGroup = InSource->LODGroup;
	CompressionSettings = InSource->CompressionSettings;
	VirtualTextureStreaming = true;
	NeverStream = true;
	UpdateResource();
	FRenderCommandFence Initialized;
	Initialized.BeginFence();
	Initialized.Wait();
	const FTextureResource* CreatedResource = GetResource();
	const FVirtualTexture2DResource* PrivateVT = CreatedResource ? CreatedResource->GetVirtualTexture2DResource() : nullptr;
	if (!PrivateVT || !PrivateVT->GetProducerHandle().IsValid() || PrivateVT->GetProducerHandle() == VT->GetProducerHandle() || PrivateVT->GetSizeX() != static_cast<uint32>(Width)
		|| PrivateVT->GetSizeY() != static_cast<uint32>(Height) || PrivateVT->GetNumLayers() != VT->GetNumLayers()
		|| PrivateVT->GetTileSize() != VT->GetTileSize() || PrivateVT->GetBorderSize() != VT->GetBorderSize())
	{
		OutReason = TEXT("virtual_texture:private_layout_mismatch");
		ReleasePrivate();
		return false;
	}
	for (uint32 Layer = 0; Layer < VT->GetNumLayers(); ++Layer)
	{
		if (PrivateVT->GetFormat(Layer) != VT->GetFormat(Layer))
		{
			OutReason = TEXT("virtual_texture:private_format_mismatch");
			ReleasePrivate();
			return false;
		}
	}
	return true;
}

FTextureResource* UAnomalyPrivateVT::CreateResource()
{
	return SourcePin.IsValid() && BorrowedData
		? new AnomalyPrivateVTDetail::FAnomalyPrivateVTResource(SourcePin.Get(), BorrowedData, FirstMip + Drop) : nullptr;
}

bool UAnomalyPrivateVT::IsCurrentlyVirtualTextured() const
{
	const FTextureResource* CurrentResource = GetResource();
	return CurrentResource && CurrentResource->GetVirtualTexture2DResource();
}

void UAnomalyPrivateVT::ReleasePrivate()
{
	if (GetResource())
	{
		ReleaseResource();
		FRenderCommandFence Released;
		Released.BeginFence();
		Released.Wait();
	}
	Prefetch.Reset();
	BorrowedData = nullptr;
	SourcePin.Reset();
}

void UAnomalyPrivateVT::BeginDestroy()
{
	ReleasePrivate();
	Super::BeginDestroy();
}

int32 UAnomalyPrivateVT::LiveResources() { return AnomalyPrivateVTDetail::ResourceCount.GetValue(); }
int32 UAnomalyPrivateVT::LiveProducers() { return AnomalyPrivateVTDetail::ProducerCount.GetValue(); }


namespace AnomalyPrivateVTDetail
{
	void Refuse(FAnomalyVTPrefetch& State, const TCHAR* Reason)
	{
		State.Failure = Reason;
		State.Status.Store(2);
		State.InFlight.Store(false);
	}

	bool ValidateAllocation(IAllocatedVirtualTexture* Allocation, const TArray<FVirtualTexture2DResource*>& Expected,
		FAnomalyVTPrefetch& State)
	{
		if (!Allocation || Allocation->GetNumTextureLayers() != static_cast<uint32>(Expected.Num())
			|| (Expected.Num() > 1 && Allocation->GetDescription().bShareDuplicateLayers) || Allocation->GetDimensions() != 2
			|| Allocation->GetNumPageTableTextures() != static_cast<uint32>(FMath::DivideAndRoundUp(Expected.Num(), 4))
			|| Allocation->GetDescription().IndirectionTextureSize != 0 || Allocation->GetDescription().AdaptiveLevelBias != 0)
		{
			Refuse(State, TEXT("virtual_texture:combined_mapping_layout"));
			return false;
		}
		if (State.Allocation && State.Allocation != Allocation)
		{
			Refuse(State, TEXT("virtual_texture:combined_allocation_changed"));
			return false;
		}
		State.Allocation = Allocation;
		State.ProducerBiases.Reset();
		for (int32 Layer = 0; Layer < Expected.Num(); ++Layer)
		{
			const FVirtualTexture2DResource* VT = Expected[Layer];
			if (!VT || VT->GetNumLayers() != 1 || !VT->GetProducerHandle().IsValid()
				|| Allocation->GetProducerHandle(Layer) != VT->GetProducerHandle()
				|| Allocation->GetDescription().ProducerLayerIndex[Layer] != 0
				|| VT->GetTileSize() != Allocation->GetVirtualTileSize() || VT->GetBorderSize() != Allocation->GetTileBorderSize())
			{
				Refuse(State, TEXT("virtual_texture:combined_producer_mismatch"));
				return false;
			}
			const FIntPoint Blocks = VT->GetSizeInBlocks();
			if (Blocks.X <= 0 || Blocks.Y <= 0 || static_cast<uint32>(Blocks.X) != Allocation->GetWidthInBlocks()
				|| static_cast<uint32>(Blocks.Y) != Allocation->GetHeightInBlocks())
			{
				Refuse(State, TEXT("virtual_texture:combined_block_layout"));
				return false;
			}
			const uint32 X = FMath::DivideAndRoundUp(VT->GetNumTilesX(), static_cast<uint32>(Blocks.X));
			const uint32 Y = FMath::DivideAndRoundUp(VT->GetNumTilesY(), static_cast<uint32>(Blocks.Y));
			const uint32 CommonX = Allocation->GetBlockWidthInTiles();
			const uint32 CommonY = Allocation->GetBlockHeightInTiles();
			if (X == 0 || Y == 0 || CommonX < X || CommonY < Y)
			{
				Refuse(State, TEXT("virtual_texture:combined_mip_bias"));
				return false;
			}
			const uint32 BiasX = FMath::CeilLogTwo(CommonX / X);
			const uint32 BiasY = FMath::CeilLogTwo(CommonY / Y);
			if (BiasX != BiasY || (X << BiasX) != CommonX || (Y << BiasY) != CommonY
				|| BiasX > Allocation->GetMaxLevel())
			{
				Refuse(State, TEXT("virtual_texture:combined_mip_bias"));
				return false;
			}
			State.ProducerBiases.Add(BiasX);
		}
		return true;
	}

	void PollAllocation(FRHICommandListImmediate& RHICmdList, IAllocatedVirtualTexture* Allocation,
		const TArray<FVirtualTexture2DResource*>& Expected, FAnomalyVTPrefetch& State, bool bRequestPages)
	{
		if (!ValidateAllocation(Allocation, Expected, State)) { return; }
		uint32 Pages = 0;
		for (uint32 Level = 0; Level <= Allocation->GetMaxLevel(); ++Level)
		{
			Pages += FMath::Max(1u, Allocation->GetWidthInTiles() >> Level)
				* FMath::Max(1u, Allocation->GetHeightInTiles() >> Level) * Allocation->GetNumTextureLayers();
		}
		State.Pages.Store(Pages);
		if (Pages > 512u) { Refuse(State, TEXT("virtual_texture:prefetch_page_cap")); return; }
		bool bReady = State.Readbacks.Num() > 0;
		for (FAnomalyVTPrefetch::FReadback& Row : State.Readbacks)
		{
			if (!Row.Reader->IsReady()) { State.InFlight.Store(false); return; }
		}
		for (FAnomalyVTPrefetch::FReadback& Row : State.Readbacks)
		{
			const uint32* Data = static_cast<const uint32*>(Row.Reader->Lock(Row.Width * Row.Height * 4u * sizeof(uint32)));
			bReady &= Data != nullptr;
			if (Data)
			{
				for (uint32 Pixel = 0; Pixel < Row.Width * Row.Height; ++Pixel)
				{
					for (uint32 Layer = 0; Layer < Row.Layers; ++Layer)
					{
						const uint32 Value = Data[Pixel * 4u + Layer];
						bReady &= (Value >> 4u) != 0u && (Value & 15u) == Row.Levels[Layer];
					}
				}
				Row.Reader->Unlock();
			}
		}
		State.Readbacks.Reset();
		if (bReady)
		{
			if (State.bRootReady) { State.Status.Store(1); State.InFlight.Store(false); return; }
			State.bRootReady = true;
		}
		State.Attempts.AddExchange(1);
		IRendererModule& Renderer = GetRendererModule();
		const FVector2D Size(Allocation->GetWidthInPixels(), Allocation->GetHeightInPixels());
		for (uint32 Level = State.bRootReady ? 0 : Allocation->GetMaxLevel(); Level <= Allocation->GetMaxLevel(); ++Level)
		{
			if (bRequestPages) { Renderer.RequestVirtualTextureTilesForRegion(Allocation, Size, FVector2D::ZeroVector, Size, FVector2D::ZeroVector, FVector2D(1, 1), Level); }
		}
		Renderer.LoadPendingVirtualTextureTiles(RHICmdList, GMaxRHIFeatureLevel);
		RHICmdList.ImmediateFlush(EImmediateFlushType::FlushRHIThread);
#if ANOMALY_SHADERS
		FRDGBuilder Graph(RHICmdList);
		for (uint32 Table = 0; Table < Allocation->GetNumPageTableTextures(); ++Table)
		{
			FRHITexture* TableRHI = Allocation->GetPageTableTexture(Table);
			if (TableRHI && TableRHI->GetTextureReference()) { TableRHI = TableRHI->GetTextureReference()->GetReferencedTexture(); }
			if (!TableRHI) { Refuse(State, TEXT("virtual_texture:prefetch_table_lost")); return; }
			FRDGTextureRef TableTexture = Graph.RegisterExternalTexture(CreateRenderTarget(TableRHI, TEXT("AnomalyVTPageCheck")));
			for (uint32 Level = 0; Level <= Allocation->GetMaxLevel(); ++Level)
			{
				FAnomalyVTPrefetch::FReadback& Row = State.Readbacks.AddDefaulted_GetRef();
				Row.Layers = FMath::Min(4u, Allocation->GetNumTextureLayers() - Table * 4u);
				Row.Width = FMath::Max(1u, Allocation->GetWidthInTiles() >> Level);
				Row.Height = FMath::Max(1u, Allocation->GetHeightInTiles() >> Level);
				for (uint32 Layer = 0; Layer < Row.Layers; ++Layer)
				{
					Row.Levels[Layer] = State.bRootReady ? FMath::Max(Level, State.ProducerBiases[Table * 4u + Layer]) : Allocation->GetMaxLevel();
				}
				FRDGBufferRef Buffer = Graph.CreateBuffer(FRDGBufferDesc::CreateBufferDesc(sizeof(uint32), Row.Width * Row.Height * 4u), TEXT("AnomalyVTPageEntries"));
				FAnomalyVTPageCheckCS::FParameters* Params = Graph.AllocParameters<FAnomalyVTPageCheckCS::FParameters>();
				Params->PageTable = Graph.CreateSRV(FRDGTextureSRVDesc::Create(TableTexture));
				Params->Entries = Graph.CreateUAV(FRDGBufferUAVDesc(Buffer, PF_R32_UINT));
				Params->PageOrigin = FIntPoint(Allocation->GetVirtualPageX() >> Level, Allocation->GetVirtualPageY() >> Level);
				Params->PageSize = FIntPoint(Row.Width, Row.Height);
				Params->PageLevel = Level;
				TShaderMapRef<FAnomalyVTPageCheckCS> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
				FComputeShaderUtils::AddPass(Graph, RDG_EVENT_NAME("AnomalyVTPageCheck"), Shader, Params, FComputeShaderUtils::GetGroupCount(Params->PageSize, 8));
				Row.Reader = MakeUnique<FRHIGPUBufferReadback>(TEXT("AnomalyVTPageCheck"));
				AddEnqueueCopyPass(Graph, Row.Reader.Get(), Buffer, Row.Width * Row.Height * 4u * sizeof(uint32));
			}
		}
		Graph.Execute();
		RHICmdList.SubmitCommandsHint();
		State.InFlight.Store(false);
#else
		Refuse(State, TEXT("virtual_texture:prefetch_shader_unavailable"));
#endif
	}
}

bool UAnomalyPrivateVT::PollPrefetch(FString& OutReason)
{
	if (!Prefetch.IsValid()) { Prefetch = MakeShared<FAnomalyVTPrefetch, ESPMode::ThreadSafe>(); }
	if (Prefetch->Status.Load() == 1) { return true; }
	if (Prefetch->Status.Load() == 2 || Prefetch->Attempts.Load() >= 120 || FPlatformTime::Seconds() - Prefetch->Started > 10.0)
	{
		OutReason = Prefetch->Failure.IsEmpty() ? TEXT("virtual_texture:prefetch_incomplete") : Prefetch->Failure;
		return false;
	}
	if (Prefetch->InFlight.Load()) { return false; }
	FTextureResource* CurrentResource = GetResource();
	FVirtualTexture2DResource* PrivateVT = CurrentResource ? CurrentResource->GetVirtualTexture2DResource() : nullptr;
	if (!PrivateVT) { OutReason = TEXT("virtual_texture:prefetch_resource_lost"); return false; }
	Prefetch->InFlight.Store(true);
	const TSharedPtr<FAnomalyVTPrefetch, ESPMode::ThreadSafe> State = Prefetch;
	bool bRequestPages = true;
#if !UE_BUILD_SHIPPING
	bRequestPages = !bSkipRequests;
#endif
	ENQUEUE_RENDER_COMMAND(AnomalyVTPrefetch)([PrivateVT, State, bRequestPages](FRHICommandListImmediate& RHICmdList)
	{
		TArray<FVirtualTexture2DResource*> Expected;
		for (uint32 Layer = 0; Layer < PrivateVT->GetNumLayers(); ++Layer) { Expected.Add(PrivateVT); }
		AnomalyPrivateVTDetail::PollAllocation(RHICmdList, PrivateVT->AcquireAllocatedVT(), Expected, *State, bRequestPages);
	});
	return false;
}

bool AnomalyPrefetchVTMaterial(UMaterialInstanceDynamic* Material, const TArray<UAnomalyPrivateVT*>& Targets,
	ERHIFeatureLevel::Type FeatureLevel, FString& OutReason)
{
	if (!Material || Targets.Num() == 0) { return true; }
	FMaterialResource* Resource = Material->GetMaterialResource(FeatureLevel);
	const FMaterialShaderMap* ShaderMap = Resource ? Resource->GetGameThreadShaderMap() : nullptr;
	FMaterialRenderProxy* Proxy = Material->GetRenderProxy();
	if (!ShaderMap || !Proxy) { OutReason = TEXT("virtual_texture:combined_material_lost"); return false; }
	const FUniformExpressionSet& Set = ShaderMap->GetUniformExpressionSet();
	const TConstArrayView<FMaterialVirtualTextureStack> Stacks = Set.GetVTStacks();
	for (int32 StackIndex = 0; StackIndex < Stacks.Num(); ++StackIndex)
	{
		const FMaterialVirtualTextureStack& Stack = Stacks[StackIndex];
		if (Stack.IsPreallocatedStack()) { OutReason = TEXT("virtual_texture:stack_preallocated"); return false; }
		TArray<FVirtualTexture2DResource*> Expected;
		for (uint32 Layer = 0; Layer < Stack.GetNumLayers(); ++Layer)
		{
			const int32 Index = Stack.LayerUniformExpressionIndices[Layer];
			if (Index < 0 || Index >= Set.GetNumTextures(EMaterialTextureParameterType::Virtual))
			{
				OutReason = TEXT("virtual_texture:stack_layer_missing"); return false;
			}
			const FMaterialTextureParameterInfo& Parameter = Set.GetTextureParameter(EMaterialTextureParameterType::Virtual, Index);
			UTexture* Bound = nullptr;
			if (Parameter.GetParameterName().IsNone() || !Material->GetTextureParameterValue(FHashedMaterialParameterInfo(Parameter.ParameterInfo), Bound, true))
			{
				OutReason = TEXT("virtual_texture:stack_unnamed"); return false;
			}
			UAnomalyPrivateVT* Private = Cast<UAnomalyPrivateVT>(Bound);
			if (!Private || !Targets.Contains(Private) || !Private->GetResource())
			{
				OutReason = TEXT("virtual_texture:stack_layer_unreplaced"); return false;
			}
			Expected.Add(Private->GetResource()->GetVirtualTexture2DResource());
		}
		const TSharedPtr<FAnomalyVTPrefetch, ESPMode::ThreadSafe> State = MakeShared<FAnomalyVTPrefetch, ESPMode::ThreadSafe>();
		while (State->Status.Load() == 0 && State->Attempts.Load() < 120 && FPlatformTime::Seconds() - State->Started <= 10.0)
		{
			ENQUEUE_RENDER_COMMAND(AnomalyVTCombinedPrefetch)([Proxy, FeatureLevel, ShaderMap, StackIndex, Expected, State](FRHICommandListImmediate& RHICmdList)
			{
				Proxy->UpdateUniformExpressionCacheIfNeeded(FeatureLevel);
				FUniformExpressionCacheAsyncUpdateScope::WaitForTask();
				const FUniformExpressionCache& Cache = Proxy->UniformExpressionCache[FeatureLevel];
				if (!Cache.bUpToDate || Cache.CachedUniformExpressionShaderMap != ShaderMap || !Cache.AllocatedVTs.IsValidIndex(StackIndex))
				{
					AnomalyPrivateVTDetail::Refuse(*State, TEXT("virtual_texture:combined_cache_mismatch")); return;
				}
				AnomalyPrivateVTDetail::PollAllocation(RHICmdList, Cache.AllocatedVTs[StackIndex], Expected, *State, true);
			});
			FlushRenderingCommands();
			if (State->Status.Load() == 0) { FPlatformProcess::SleepNoStats(0.001f); }
		}
		const bool bReady = State->Status.Load() == 1;
		FString Biases;
		for (int32 Layer = 0; Layer < State->ProducerBiases.Num(); ++Layer)
		{
			Biases += FString::Printf(TEXT("%s%d:%u:%d:%d"), Layer ? TEXT(",") : TEXT(""), Layer, State->ProducerBiases[Layer], Layer / 4, Layer % 4);
		}
		UE_LOG(LogAnomaly, Log, TEXT("stuck_low_mip: VT_COMBINED stack=%d layers=%d ready=%d pages=%d attempts=%d wait_ms=%.3f layer_bias_table_channel=%s"),
			StackIndex, Expected.Num(), bReady ? 1 : 0, State->Pages.Load(), State->Attempts.Load(), (FPlatformTime::Seconds() - State->Started) * 1000.0, *Biases);
		if (!bReady)
		{
			OutReason = State->Failure.IsEmpty() ? TEXT("virtual_texture:combined_prefetch_incomplete") : State->Failure;
			return false;
		}
	}
	return true;
}

int32 UAnomalyPrivateVT::PrefetchAttempts() const { return Prefetch.IsValid() ? Prefetch->Attempts.Load() : 0; }
int32 UAnomalyPrivateVT::PrefetchPages() const { return Prefetch.IsValid() ? Prefetch->Pages.Load() : 0; }
