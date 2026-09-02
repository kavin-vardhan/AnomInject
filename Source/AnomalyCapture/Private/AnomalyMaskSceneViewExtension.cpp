#include "AnomalyMaskSceneViewExtension.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"
#include "AnomalyStencilTag.h"

#include "SceneView.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "PixelShaderUtils.h"
#include "SceneTextureParameters.h"
#include "HAL/IConsoleManager.h"

class FViewInfo;

#include "ScreenPass.h"
#include "PostProcess/PostProcessMaterial.h"
#include "SceneRendering.h"

#include "AnomalyVisibleMaskShader.h"
#include "AnomalyMaskReduceShader.h"

static FScreenPassTexture FinalizeMaskAfterPassOutput(FRDGBuilder& GraphBuilder, const FSceneView& View,
	const FPostProcessMaterialInputs& Inputs, const FScreenPassTexture& SceneColor)
{
	FScreenPassRenderTarget Output = Inputs.OverrideOutput;
	if (!Output.IsValid() || !SceneColor.IsValid())
	{
		return SceneColor;
	}
	checkSlow(View.bIsViewInfo);
	const FViewInfo& ViewInfo = static_cast<const FViewInfo&>(View);
	AddDrawTexturePass(GraphBuilder, ViewInfo, SceneColor, Output);
	return MoveTemp(Output);
}

FAnomalyMaskSceneViewExtension::FAnomalyMaskSceneViewExtension(const FAutoRegister& AutoRegister)
	: FSceneViewExtensionBase(AutoRegister)
{
}

bool FAnomalyMaskSceneViewExtension::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	FScopeLock Lock(&StateCS);
	return PendingArms.Num() > 0;
}

void FAnomalyMaskSceneViewExtension::ArmMask(uint64 RequestId, bool bWantPixels)
{
	FScopeLock Lock(&StateCS);
	PendingArms.Add(RequestId);
	PendingArmWantsPixels.Add(bWantPixels ? 1 : 0);
}

void FAnomalyMaskSceneViewExtension::SetAssignedTags(const TSet<uint8>& InAssignedTags)
{
	FScopeLock Lock(&StateCS);
	AssignedTags = InAssignedTags;
}

void FAnomalyMaskSceneViewExtension::SetReduceMode(EAnomalyMaskReduceMode InMode)
{
	FScopeLock Lock(&StateCS);
	ReduceMode = InMode;
}

int32 FAnomalyMaskSceneViewExtension::NumPendingArms() const
{
	FScopeLock Lock(&StateCS);
	return PendingArms.Num();
}

void FAnomalyMaskSceneViewExtension::Reset()
{
	{
		FScopeLock Lock(&StateCS);
		PendingArms.Reset();
		PendingArmWantsPixels.Reset();
		AssignedTags.Reset();
	}
	{
		FScopeLock Lock(&ResultsCS);
		Results.Reset();
	}
}

void FAnomalyMaskSceneViewExtension::SubscribeToPostProcessingPass(EPostProcessingPass Pass,
	FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	if (Pass == EPostProcessingPass::Tonemap)
	{
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(
			this, &FAnomalyMaskSceneViewExtension::AfterTonemap_RenderThread));
	}
}

FScreenPassTexture FAnomalyMaskSceneViewExtension::AfterTonemap_RenderThread(FRDGBuilder& GraphBuilder,
	const FSceneView& View, const FPostProcessMaterialInputs& Inputs)
{
	const FScreenPassTexture SceneColor = Inputs.GetInput(EPostProcessMaterialInput::SceneColor);

	if (View.bIsSceneCapture || View.bIsReflectionCapture || View.bIsPlanarReflection || !SceneColor.IsValid())
	{
		return FinalizeMaskAfterPassOutput(GraphBuilder, View, Inputs, SceneColor);
	}

	uint64 RequestId = 0;
	float Bias = 1.0e-5f;
	bool bWantPixels = false;
	TArray<uint64> ServedIds;
	TArray<uint8> ServedWantsPixels;
	EAnomalyMaskReduceMode Mode = EAnomalyMaskReduceMode::Gpu;
	{
		FScopeLock Lock(&StateCS);
		if (PendingArms.Num() == 0)
		{
			return FinalizeMaskAfterPassOutput(GraphBuilder, View, Inputs, SceneColor);
		}
		ServedIds = MoveTemp(PendingArms);
		ServedWantsPixels = MoveTemp(PendingArmWantsPixels);
		PendingArms.Reset();
		PendingArmWantsPixels.Reset();
		ServedWantsPixels.SetNumZeroed(ServedIds.Num());
		RequestId = ServedIds[0];
		for (uint8 W : ServedWantsPixels)
		{
			bWantPixels |= (W != 0);
		}
		Bias = DepthBias;
		Mode = ReduceMode;
	}

	const FIntRect ViewRect = SceneColor.ViewRect;
	const FIntPoint Size = ViewRect.Size();
	if (Size.X <= 0 || Size.Y <= 0)
	{
		return FinalizeMaskAfterPassOutput(GraphBuilder, View, Inputs, SceneColor);
	}

	const FRDGTextureDesc MaskDesc = FRDGTextureDesc::Create2D(
		Size, PF_R8_UINT, FClearValueBinding::Black, TexCreate_RenderTargetable | TexCreate_ShaderResource);
	FRDGTextureRef MaskRT = GraphBuilder.CreateTexture(MaskDesc, TEXT("AnomalyVisibleMask"));

	FAnomalyVisibleMaskPS::FParameters* P = GraphBuilder.AllocParameters<FAnomalyVisibleMaskPS::FParameters>();
	P->SceneTextures = Inputs.SceneTextures;
	P->DepthBias = Bias;
	P->ReservedBase = (uint32)AnomalyStencilTag::ReservedStencilBase;
	P->ViewRectMin = ViewRect.Min;
	P->RenderTargets[0] = FRenderTargetBinding(MaskRT, ERenderTargetLoadAction::EClear);

	const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.GetFeatureLevel());
	TShaderMapRef<FAnomalyVisibleMaskPS> PixelShader(ShaderMap);
	FPixelShaderUtils::AddFullscreenPass(GraphBuilder, ShaderMap, RDG_EVENT_NAME("AnomalyVisibleMask"),
		PixelShader, P, FIntRect(0, 0, Size.X, Size.Y));

	FMaskInFlight Item;
	Item.RequestId = RequestId;
	Item.Mode = Mode;
	Item.ViewRectSize = Size;

	if (Mode != EAnomalyMaskReduceMode::Cpu)
	{
		FRDGBufferRef TableBuffer = GraphBuilder.CreateBuffer(
			FRDGBufferDesc::CreateBufferDesc(sizeof(uint32), 1280), TEXT("AnomalyMaskReduceTable"));
		FRDGBufferUAVRef TableUAV = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(TableBuffer, PF_R32_UINT));
		AddClearUAVPass(GraphBuilder, TableUAV, 0u);

		FAnomalyMaskReduceCS::FParameters* CSP = GraphBuilder.AllocParameters<FAnomalyMaskReduceCS::FParameters>();
		CSP->MaskTexture = MaskRT;
		CSP->OutTable = TableUAV;
		CSP->MaskSize = Size;

		TShaderMapRef<FAnomalyMaskReduceCS> ComputeShader(ShaderMap);
		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("AnomalyMaskReduce"),
			ComputeShader, CSP, FComputeShaderUtils::GetGroupCount(Size, 8));

		TUniquePtr<FRHIGPUBufferReadback> BufferReadback =
			MakeUnique<FRHIGPUBufferReadback>(TEXT("AnomalyMaskReduceReadback"));
		AddEnqueueCopyPass(GraphBuilder, BufferReadback.Get(), TableBuffer, 1280u * sizeof(uint32));
		Item.BufferReadback = MoveTemp(BufferReadback);
	}

	Item.bWantPixels = bWantPixels;
	Item.RequestIds = ServedIds;
	Item.WantsPixels = ServedWantsPixels;
	if (Mode != EAnomalyMaskReduceMode::Gpu || bWantPixels)
	{
		TUniquePtr<FRHIGPUTextureReadback> Readback = MakeUnique<FRHIGPUTextureReadback>(TEXT("AnomalyMaskReadback"));
		AddEnqueueCopyPass(GraphBuilder, Readback.Get(), MaskRT);
		Item.Readback = MoveTemp(Readback);
	}

	int32 ModeAtPass = -1;
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth")))
	{
		ModeAtPass = CVar->GetInt();
	}

	FIntPoint StencilExtent = FIntPoint::ZeroValue;
	if (Inputs.SceneTextures.SceneTextures)
	{
		const auto SceneTexParams = Inputs.SceneTextures.SceneTextures->GetParameters();
		if (SceneTexParams->CustomStencilTexture && SceneTexParams->CustomStencilTexture->Desc.Texture)
		{
			StencilExtent = SceneTexParams->CustomStencilTexture->Desc.Texture->Desc.Extent;
		}
	}

	FString ServedList;
	for (uint64 Id : ServedIds)
	{
		ServedList += FString::Printf(TEXT(" %llu"), Id);
	}
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(mask): M23 PASS id=%llu servedArms=%d ids=[%s ] rCustomDepth_renderThread=%d ")
		TEXT("customStencilExtent=%dx%d viewRect=%dx%d overrideOutput=%d (m43: ONE render per frame serves ")
		TEXT("EVERY pending arm - the RT's content depends only on which actors are tagged at render time, ")
		TEXT("and each consumer filters by its own tag set, so one render is the same answer delivered to ")
		TEXT("each asker. Every arm is therefore served on the NEXT render after it was armed, and no ")
		TEXT("consumer can be starved by another's cadence. StencilDummy is 1x1: extent 1x1 means custom ")
		TEXT("depth was NOT produced; overrideOutput=1 means the engine designated THIS callback the ")
		TEXT("chain's final writer for the frame)"),
		RequestId, ServedIds.Num(), *ServedList, ModeAtPass, StencilExtent.X, StencilExtent.Y,
		Size.X, Size.Y, Inputs.OverrideOutput.IsValid() ? 1 : 0);

	Item.CustomDepthModeAtPass = ModeAtPass;
	Item.CustomStencilExtent = StencilExtent;
	InFlight.Add(MoveTemp(Item));

	return FinalizeMaskAfterPassOutput(GraphBuilder, View, Inputs, SceneColor);
}

void FAnomalyMaskSceneViewExtension::EnqueueDrain(bool bFinal)
{
	TWeakPtr<FAnomalyMaskSceneViewExtension, ESPMode::ThreadSafe> WeakSelf =
		StaticCastSharedRef<FAnomalyMaskSceneViewExtension>(AsShared());
	ENQUEUE_RENDER_COMMAND(AnomalyMaskDrain)(
		[WeakSelf, bFinal](FRHICommandListImmediate&)
		{
			if (TSharedPtr<FAnomalyMaskSceneViewExtension, ESPMode::ThreadSafe> Self = WeakSelf.Pin())
			{
				Self->Drain_RenderThread(bFinal);
			}
		});
}

void FAnomalyMaskSceneViewExtension::Drain_RenderThread(bool bFinal)
{
	TSet<uint8> LocalAssigned;
	{
		FScopeLock Lock(&StateCS);
		LocalAssigned = AssignedTags;
	}

	for (int32 i = InFlight.Num() - 1; i >= 0; --i)
	{
		FMaskInFlight& Item = InFlight[i];
		const bool bNeedCpu = Item.Mode != EAnomalyMaskReduceMode::Gpu;
		const bool bNeedGpu = Item.Mode != EAnomalyMaskReduceMode::Cpu;
		const bool bNeedSurface = bNeedCpu || Item.bWantPixels;
		if (bNeedSurface && (!Item.Readback.IsValid() || !Item.Readback->IsReady()))
		{
			continue;
		}
		if (bNeedGpu && (!Item.BufferReadback.IsValid() || !Item.BufferReadback->IsReady()))
		{
			continue;
		}

		const int32 W = Item.ViewRectSize.X;
		const int32 H = Item.ViewRectSize.Y;

		int32 CpuCounts[256] = {};
		int32 CpuMinXs[256];
		int32 CpuMinYs[256];
		int32 CpuMaxXs[256];
		int32 CpuMaxYs[256];
		int32 GpuCounts[256] = {};
		int32 GpuMinXs[256];
		int32 GpuMinYs[256];
		int32 GpuMaxXs[256];
		int32 GpuMaxYs[256];
		for (int32 t = 0; t < 256; ++t)
		{
			CpuMinXs[t] = MAX_int32; CpuMinYs[t] = MAX_int32;
			CpuMaxXs[t] = MIN_int32; CpuMaxYs[t] = MIN_int32;
			GpuMinXs[t] = MAX_int32; GpuMinYs[t] = MAX_int32;
			GpuMaxXs[t] = MIN_int32; GpuMaxYs[t] = MIN_int32;
		}

		bool bHaveCpu = false;
		bool bHaveGpu = false;
		TArray<uint8> TightPixels;

		if (bNeedSurface)
		{
			int32 RowPitchInPixels = 0;
			int32 BufferHeight = 0;
			void* Src = Item.Readback->Lock(RowPitchInPixels, &BufferHeight);
			if (Src && RowPitchInPixels > 0
				&& AnomalyReadback::CheckDrainBounds(TEXT("mask"), Item.RequestId, FIntRect(0, 0, W, H),
					W, H, RowPitchInPixels, BufferHeight, GuardDrops))
			{
				const uint8* Base = static_cast<const uint8*>(Src);
				if (Item.bWantPixels)
				{
					TightPixels.SetNumUninitialized((int32)((int64)W * (int64)H));
					for (int32 y = 0; y < H; ++y)
					{
						FMemory::Memcpy(TightPixels.GetData() + (int64)y * W,
							Base + (int64)y * RowPitchInPixels, (SIZE_T)W);
					}
				}
				if (bNeedCpu)
				{
					for (int32 y = 0; y < H; ++y)
					{
						const uint8* Row = Base + (int64)y * RowPitchInPixels;
						for (int32 x = 0; x < W; ++x)
						{
							const uint8 V = Row[x];
							if (V == 0)
							{
								continue;
							}
							++CpuCounts[V];
							if (x < CpuMinXs[V]) { CpuMinXs[V] = x; }
							if (y < CpuMinYs[V]) { CpuMinYs[V] = y; }
							if (x > CpuMaxXs[V]) { CpuMaxXs[V] = x; }
							if (y > CpuMaxYs[V]) { CpuMaxYs[V] = y; }
						}
					}
					bHaveCpu = true;
				}
			}
			if (Src)
			{
				Item.Readback->Unlock();
			}
		}

		if (bNeedGpu)
		{
			const void* BufSrc = Item.BufferReadback->Lock(1280u * sizeof(uint32));
			if (BufSrc)
			{
				const uint32* T = static_cast<const uint32*>(BufSrc);
				for (int32 t = 0; t < 256; ++t)
				{
					const uint32 C = T[t * 5 + 0];
					GpuCounts[t] = (int32)C;
					if (C > 0)
					{
						GpuMinXs[t] = (int32)~T[t * 5 + 1];
						GpuMinYs[t] = (int32)~T[t * 5 + 2];
						GpuMaxXs[t] = (int32)T[t * 5 + 3];
						GpuMaxYs[t] = (int32)T[t * 5 + 4];
					}
				}
				bHaveGpu = true;
				Item.BufferReadback->Unlock();
			}
		}

		if (Item.Mode == EAnomalyMaskReduceMode::Both && bHaveCpu && bHaveGpu)
		{
			int32 DiffTag = -1;
			const TCHAR* DiffField = TEXT("");
			int32 DiffCpu = 0;
			int32 DiffGpu = 0;
			for (int32 t = 0; t < 256 && DiffTag < 0; ++t)
			{
				if (CpuCounts[t] != GpuCounts[t])
				{
					DiffTag = t; DiffField = TEXT("count"); DiffCpu = CpuCounts[t]; DiffGpu = GpuCounts[t];
				}
				else if (CpuCounts[t] > 0)
				{
					if (CpuMinXs[t] != GpuMinXs[t]) { DiffTag = t; DiffField = TEXT("minx"); DiffCpu = CpuMinXs[t]; DiffGpu = GpuMinXs[t]; }
					else if (CpuMinYs[t] != GpuMinYs[t]) { DiffTag = t; DiffField = TEXT("miny"); DiffCpu = CpuMinYs[t]; DiffGpu = GpuMinYs[t]; }
					else if (CpuMaxXs[t] != GpuMaxXs[t]) { DiffTag = t; DiffField = TEXT("maxx"); DiffCpu = CpuMaxXs[t]; DiffGpu = GpuMaxXs[t]; }
					else if (CpuMaxYs[t] != GpuMaxYs[t]) { DiffTag = t; DiffField = TEXT("maxy"); DiffCpu = CpuMaxYs[t]; DiffGpu = GpuMaxYs[t]; }
				}
			}
			if (DiffTag < 0)
			{
				UE_LOG(LogAnomalyCapture, Log, TEXT("MASK-REDUCE COMPARE id=%llu IDENTICAL"), Item.RequestId);
			}
			else
			{
				UE_LOG(LogAnomalyCapture, Error,
					TEXT("MASK-REDUCE COMPARE id=%llu FIRST-DIFF tag=%d field=%s cpu=%d gpu=%d"),
					Item.RequestId, DiffTag, DiffField, DiffCpu, DiffGpu);
			}
		}

		const bool bUseGpu = Item.Mode != EAnomalyMaskReduceMode::Cpu;
		const bool bHaveResult = bUseGpu ? bHaveGpu : bHaveCpu;
		if (bHaveResult)
		{
			const int32* Counts = bUseGpu ? GpuCounts : CpuCounts;
			const int32* MinXs = bUseGpu ? GpuMinXs : CpuMinXs;
			const int32* MinYs = bUseGpu ? GpuMinYs : CpuMinYs;
			const int32* MaxXs = bUseGpu ? GpuMaxXs : CpuMaxXs;
			const int32* MaxYs = bUseGpu ? GpuMaxYs : CpuMaxYs;

			FAnomalyMaskResult Result;
			Result.ViewRectSize = Item.ViewRectSize;
			for (int32 t = AnomalyStencilTag::ReservedStencilBase; t <= AnomalyStencilTag::ReservedStencilMax; ++t)
			{
				if (Counts[t] <= 0)
				{
					continue;
				}
				const uint8 Tag = (uint8)t;
				FAnomalyMaskTagResult R;
				R.Count = Counts[t];
				R.MinX = MinXs[t]; R.MinY = MinYs[t];
				R.MaxX = MaxXs[t]; R.MaxY = MaxYs[t];
				Result.TagResults.Add(Tag, R);
				Result.TotalMaskedPixels += Counts[t];

				if (!LocalAssigned.Contains(Tag) && !Result.bSawUnassignedReservedTag)
				{
					Result.bSawUnassignedReservedTag = true;
					Result.FirstUnassignedTag = Tag;
					Result.UnassignedTagCount = Counts[t];
				}
			}

			Result.CustomDepthModeAtPass = Item.CustomDepthModeAtPass;
			Result.CustomStencilExtent = Item.CustomStencilExtent;

			const int32 ViewPixels = W * H;
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(mask): M23 REDUCE id=%llu mode=%d stencilExtent=%dx%d totalMasked=%d ")
				TEXT("unassignedTag=%d unassignedCount=%d viewPixels=%d unassignedPctOfFrame=%.2f reduce=%s"),
				Item.RequestId, Item.CustomDepthModeAtPass, Item.CustomStencilExtent.X,
				Item.CustomStencilExtent.Y, Result.TotalMaskedPixels,
				(int32)Result.FirstUnassignedTag, Result.UnassignedTagCount, ViewPixels,
				ViewPixels > 0 ? (100.0 * (double)Result.UnassignedTagCount / (double)ViewPixels) : -1.0,
				LexToStringAnomalyMaskReduceMode(Item.Mode));

			{
				FScopeLock Lock(&ResultsCS);
				if (Item.RequestIds.Num() == 0)
				{
					Result.MaskPixels = MoveTemp(TightPixels);
					Results.Add(Item.RequestId, MoveTemp(Result));
				}
				else
				{
					int32 PixelOwner = INDEX_NONE;
					for (int32 k = 0; k < Item.RequestIds.Num(); ++k)
					{
						if (Item.WantsPixels.IsValidIndex(k) && Item.WantsPixels[k] != 0)
						{
							PixelOwner = k;
							break;
						}
					}
					for (int32 k = 0; k < Item.RequestIds.Num(); ++k)
					{
						FAnomalyMaskResult Copy = Result;
						if (k == PixelOwner)
						{
							Copy.MaskPixels = MoveTemp(TightPixels);
						}
						Results.Add(Item.RequestIds[k], MoveTemp(Copy));
					}
				}
			}
		}

		InFlight.RemoveAt(i);
	}

	if (bFinal && InFlight.Num() > 0)
	{
		FString Ids;
		for (const FMaskInFlight& Item : InFlight)
		{
			Ids += FString::Printf(TEXT(" %llu"), Item.RequestId);
		}
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(mask): M34 LOST-IN-FLIGHT lostInFlight=%d requestIds=[%s ] - these armed frames' ")
			TEXT("readbacks were still not ready at the run's FINAL bounded drain (one EnqueueDrain + ")
			TEXT("FlushRenderingCommands; poll, never wait). Each is a LOST MEASUREMENT: its arm resolves ")
			TEXT("nothing and reads as resolved < arms on the M26S1 EVENT line. The veto consumes only ")
			TEXT("COMPLETE results - an event whose armed frames were all lost stays NOT_MEASURED, which ")
			TEXT("ADMITS. Expected count on this bench: ZERO (readback latency is one render frame and the ")
			TEXT("drain tail precedes FinishRun)."),
			InFlight.Num(), *Ids);
	}
}

bool FAnomalyMaskSceneViewExtension::TakeMaskResult(uint64 RequestId, FAnomalyMaskResult& Out, bool bRemove)
{
	FScopeLock Lock(&ResultsCS);
	if (FAnomalyMaskResult* R = Results.Find(RequestId))
	{
		if (bRemove)
		{
			Out = MoveTemp(*R);
			Results.Remove(RequestId);
		}
		else
		{
			Out = *R;
		}
		return true;
	}
	return false;
}

#endif
