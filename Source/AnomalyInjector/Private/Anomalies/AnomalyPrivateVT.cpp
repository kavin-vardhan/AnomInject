#include "Anomalies/AnomalyPrivateVT.h"
#include "Engine/Texture2D.h"
#include "EngineModule.h"
#include "HAL/PlatformTime.h"
#include "HAL/ThreadSafeCounter.h"
#include "PixelFormat.h"
#include "RendererInterface.h"
#include "RenderingThread.h"
#include "RHIGPUReadback.h"
#include "RHICommandList.h"
#include "TextureResource.h"
#include "Templates/Atomic.h"
#include "VirtualTexturing.h"

struct FAnomalyVTPrefetch
{
	struct FReadback
	{
		TUniquePtr<FRHIGPUTextureReadback> Reader;
		FTextureRHIRef Copy;
		uint32 Level = 0;
		uint32 Layers = 0;
		uint32 Width = 0;
		uint32 Height = 0;
		bool b16Bit = false;
	};
	TArray<FReadback> Readbacks;
	TAtomic<int32> Status{0};
	TAtomic<int32> Attempts{0};
	TAtomic<int32> Pages{0};
	TAtomic<bool> InFlight{false};
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
	const int32 MaximumDrop = FMath::CeilLogTwo(FMath::Max(VT->GetNumTilesX(), VT->GetNumTilesY()));
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

bool UAnomalyPrivateVT::PollPrefetch(FString& OutReason)
{
	if (!Prefetch.IsValid()) { Prefetch = MakeShared<FAnomalyVTPrefetch, ESPMode::ThreadSafe>(); }
	if (Prefetch->Status.Load() == 1) { return true; }
	if (Prefetch->Status.Load() == 2 || Prefetch->Attempts.Load() >= 120 || FPlatformTime::Seconds() - Prefetch->Started > 10.0)
	{
		OutReason = TEXT("virtual_texture:prefetch_incomplete");
		return false;
	}
	if (Prefetch->InFlight.Load()) { return false; }
	FTextureResource* CurrentResource = GetResource();
	FVirtualTexture2DResource* PrivateVT = CurrentResource ? CurrentResource->GetVirtualTexture2DResource() : nullptr;
	if (!PrivateVT)
	{
		OutReason = TEXT("virtual_texture:prefetch_resource_lost");
		return false;
	}
	Prefetch->InFlight.Store(true);
	const TSharedPtr<FAnomalyVTPrefetch, ESPMode::ThreadSafe> State = Prefetch;
	bool bRequestPages = true;
#if !UE_BUILD_SHIPPING
	bRequestPages = !bSkipRequests;
#endif
	ENQUEUE_RENDER_COMMAND(AnomalyVTPrefetch)([PrivateVT, State, bRequestPages](FRHICommandListImmediate& RHICmdList)
	{
		bool bReady = State->Readbacks.Num() > 0;
		for (FAnomalyVTPrefetch::FReadback& Row : State->Readbacks)
		{
			if (!Row.Reader->IsReady())
			{
				State->InFlight.Store(false);
				return;
			}
		}
		for (FAnomalyVTPrefetch::FReadback& Row : State->Readbacks)
		{
			int32 Pitch = 0;
			const void* Data = Row.Reader->Lock(Pitch);
			bReady &= Data != nullptr && Pitch >= static_cast<int32>(Row.Width);
			if (Data && Pitch >= static_cast<int32>(Row.Width))
			{
				const uint32 Stride = GPixelFormats[Row.Copy->GetFormat()].BlockBytes / (Row.b16Bit ? 2u : 4u);
				for (uint32 Y = 0; Y < Row.Height; ++Y)
				{
					for (uint32 X = 0; X < Row.Width; ++X)
					{
						for (uint32 Layer = 0; Layer < Row.Layers; ++Layer)
						{
							const uint32 Index = (Y * Pitch + X) * Stride + Layer;
							const uint32 Value = Row.b16Bit ? static_cast<const uint16*>(Data)[Index] : static_cast<const uint32*>(Data)[Index];
							bReady &= (Value >> 4u) != 0u && (Value & 15u) == Row.Level;
						}
					}
				}
			}
			Row.Reader->Unlock();
		}
		State->Readbacks.Reset();
		if (bReady)
		{
			State->Status.Store(1);
			State->InFlight.Store(false);
			return;
		}
		IAllocatedVirtualTexture* Allocation = PrivateVT->AcquireAllocatedVT();
		uint32 Pages = 0;
		for (uint32 Level = 0; Level <= Allocation->GetMaxLevel(); ++Level)
		{
			Pages += FMath::Max(1u, Allocation->GetWidthInTiles() >> Level) * FMath::Max(1u, Allocation->GetHeightInTiles() >> Level) * Allocation->GetNumTextureLayers();
		}
		if (Pages > 512u)
		{
			State->Status.Store(2);
			State->InFlight.Store(false);
			return;
		}
		State->Pages.Store(Pages);
		State->Attempts.AddExchange(1);
		IRendererModule& Renderer = GetRendererModule();
		const FVector2D Size(Allocation->GetWidthInPixels(), Allocation->GetHeightInPixels());
		for (uint32 Level = 0; Level <= Allocation->GetMaxLevel(); ++Level)
		{
			if (bRequestPages) { Renderer.RequestVirtualTextureTilesForRegion(Allocation, Size, FVector2D::ZeroVector, Size, FVector2D::ZeroVector, FVector2D(1, 1), Level); }
		}
		Renderer.LoadPendingVirtualTextureTiles(RHICmdList, GMaxRHIFeatureLevel);
		for (uint32 Table = 0; Table < Allocation->GetNumPageTableTextures(); ++Table)
		{
			FRHITexture* PageTable = Allocation->GetPageTableTexture(Table);
			if (!PageTable)
			{
				State->Status.Store(2);
				State->InFlight.Store(false);
				return;
			}
			for (uint32 Level = 0; Level <= Allocation->GetMaxLevel(); ++Level)
			{
				FAnomalyVTPrefetch::FReadback& Row = State->Readbacks.AddDefaulted_GetRef();
				Row.Level = Level;
				Row.Layers = FMath::Min(4u, Allocation->GetNumTextureLayers() - Table * 4u);
				Row.Width = FMath::Max(1u, Allocation->GetWidthInTiles() >> Level);
				Row.Height = FMath::Max(1u, Allocation->GetHeightInTiles() >> Level);
				Row.b16Bit = Allocation->GetPageTableFormat() == EVTPageTableFormat::UInt16;
				const FRHITextureCreateDesc Desc = FRHITextureCreateDesc::Create2D(TEXT("AnomalyVTPageCheck"), Row.Width, Row.Height, PageTable->GetFormat())
					.SetFlags(ETextureCreateFlags::ShaderResource).SetInitialState(ERHIAccess::CopyDest);
				Row.Copy = RHICreateTexture(Desc);
				FRHICopyTextureInfo CopyInfo;
				CopyInfo.SourceMipIndex = Level;
				CopyInfo.SourcePosition = FIntVector(Allocation->GetVirtualPageX() >> Level, Allocation->GetVirtualPageY() >> Level, 0);
				CopyInfo.Size = FIntVector(Row.Width, Row.Height, 1);
				RHICmdList.Transition(FRHITransitionInfo(PageTable, ERHIAccess::Unknown, ERHIAccess::CopySrc));
				RHICmdList.CopyTexture(PageTable, Row.Copy, CopyInfo);
				RHICmdList.Transition(FRHITransitionInfo(PageTable, ERHIAccess::CopySrc, ERHIAccess::SRVMask));
				RHICmdList.Transition(FRHITransitionInfo(Row.Copy, ERHIAccess::CopyDest, ERHIAccess::CopySrc));
				Row.Reader = MakeUnique<FRHIGPUTextureReadback>(TEXT("AnomalyVTPageCheck"));
				Row.Reader->EnqueueCopy(RHICmdList, Row.Copy);
			}
		}
		RHICmdList.SubmitCommandsHint();
		State->InFlight.Store(false);
	});
	return false;
}

int32 UAnomalyPrivateVT::PrefetchAttempts() const { return Prefetch.IsValid() ? Prefetch->Attempts.Load() : 0; }
int32 UAnomalyPrivateVT::PrefetchPages() const { return Prefetch.IsValid() ? Prefetch->Pages.Load() : 0; }
