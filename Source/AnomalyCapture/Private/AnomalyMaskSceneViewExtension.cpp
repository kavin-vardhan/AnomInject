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

class FViewInfo;

#include "ScreenPass.h"
#include "PostProcess/PostProcessMaterial.h"

class FAnomalyVisibleMaskPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FAnomalyVisibleMaskPS);
	SHADER_USE_PARAMETER_STRUCT(FAnomalyVisibleMaskPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FSceneTextureShaderParameters, SceneTextures)
		SHADER_PARAMETER(float, DepthBias)
		SHADER_PARAMETER(uint32, ReservedBase)
		SHADER_PARAMETER(FIntPoint, ViewRectMin)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters&) { return true; }
};

IMPLEMENT_GLOBAL_SHADER(FAnomalyVisibleMaskPS, "/Plugin/AnomalyInjector/Private/AnomalyVisibleMask.usf", "MainPS", SF_Pixel);


FAnomalyMaskSceneViewExtension::FAnomalyMaskSceneViewExtension(const FAutoRegister& AutoRegister)
	: FSceneViewExtensionBase(AutoRegister)
{
}

bool FAnomalyMaskSceneViewExtension::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	FScopeLock Lock(&StateCS);
	return PendingArms.Num() > 0;
}

void FAnomalyMaskSceneViewExtension::ArmMask(uint64 RequestId)
{
	FScopeLock Lock(&StateCS);
	PendingArms.Add(RequestId);
}

void FAnomalyMaskSceneViewExtension::SetAssignedTags(const TSet<uint8>& InAssignedTags)
{
	FScopeLock Lock(&StateCS);
	AssignedTags = InAssignedTags;
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
		return SceneColor;
	}

	uint64 RequestId = 0;
	float Bias = 1.0e-5f;
	{
		FScopeLock Lock(&StateCS);
		if (PendingArms.Num() == 0)
		{
			return SceneColor;
		}
		RequestId = PendingArms[0];
		PendingArms.RemoveAt(0);
		Bias = DepthBias;
	}

	const FIntRect ViewRect = SceneColor.ViewRect;
	const FIntPoint Size = ViewRect.Size();
	if (Size.X <= 0 || Size.Y <= 0)
	{
		return SceneColor;
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

	TUniquePtr<FRHIGPUTextureReadback> Readback = MakeUnique<FRHIGPUTextureReadback>(TEXT("AnomalyMaskReadback"));
	AddEnqueueCopyPass(GraphBuilder, Readback.Get(), MaskRT);

	FMaskInFlight Item;
	Item.RequestId = RequestId;
	Item.Readback = MoveTemp(Readback);
	Item.ViewRectSize = Size;
	InFlight.Add(MoveTemp(Item));

	return SceneColor;
}

void FAnomalyMaskSceneViewExtension::EnqueueDrain()
{
	TWeakPtr<FAnomalyMaskSceneViewExtension, ESPMode::ThreadSafe> WeakSelf =
		StaticCastSharedRef<FAnomalyMaskSceneViewExtension>(AsShared());
	ENQUEUE_RENDER_COMMAND(AnomalyMaskDrain)(
		[WeakSelf](FRHICommandListImmediate&)
		{
			if (TSharedPtr<FAnomalyMaskSceneViewExtension, ESPMode::ThreadSafe> Self = WeakSelf.Pin())
			{
				Self->Drain_RenderThread();
			}
		});
}

void FAnomalyMaskSceneViewExtension::Drain_RenderThread()
{
	TSet<uint8> LocalAssigned;
	{
		FScopeLock Lock(&StateCS);
		LocalAssigned = AssignedTags;
	}

	for (int32 i = InFlight.Num() - 1; i >= 0; --i)
	{
		FMaskInFlight& Item = InFlight[i];
		if (!Item.Readback.IsValid() || !Item.Readback->IsReady())
		{
			continue;
		}

		int32 RowPitchInPixels = 0;
		int32 BufferHeight = 0;
		void* Src = Item.Readback->Lock(RowPitchInPixels, &BufferHeight);
		if (Src && RowPitchInPixels > 0)
		{
			const int32 W = Item.ViewRectSize.X;
			const int32 H = Item.ViewRectSize.Y;
			const uint8* Base = static_cast<const uint8*>(Src);

			int32 Counts[256] = {};
			int32 MinXs[256];
			int32 MinYs[256];
			int32 MaxXs[256];
			int32 MaxYs[256];
			for (int32 t = 0; t < 256; ++t)
			{
				MinXs[t] = MAX_int32; MinYs[t] = MAX_int32;
				MaxXs[t] = MIN_int32; MaxYs[t] = MIN_int32;
			}

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
					++Counts[V];
					if (x < MinXs[V]) { MinXs[V] = x; }
					if (y < MinYs[V]) { MinYs[V] = y; }
					if (x > MaxXs[V]) { MaxXs[V] = x; }
					if (y > MaxYs[V]) { MaxYs[V] = y; }
				}
			}

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
				}
			}

			Item.Readback->Unlock();

			{
				FScopeLock Lock(&ResultsCS);
				Results.Add(Item.RequestId, MoveTemp(Result));
			}
		}
		else if (Src)
		{
			Item.Readback->Unlock();
		}

		InFlight.RemoveAt(i);
	}
}

bool FAnomalyMaskSceneViewExtension::TakeMaskResult(uint64 RequestId, FAnomalyMaskResult& Out)
{
	FScopeLock Lock(&ResultsCS);
	if (FAnomalyMaskResult* R = Results.Find(RequestId))
	{
		Out = MoveTemp(*R);
		Results.Remove(RequestId);
		return true;
	}
	return false;
}

#endif
