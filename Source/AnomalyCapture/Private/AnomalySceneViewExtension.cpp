#include "AnomalySceneViewExtension.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"
#include "AnomalySveCapturer.h"
#include "AnomalySveKeyRing.h"

#include "CoreGlobals.h"
#include "RHIGPUReadback.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "SceneView.h"

class FViewInfo;

#include "PostProcess/PostProcessMaterial.h"
#include "ScreenPass.h"
#include "SceneRendering.h"

static FScreenPassTexture FinalizeSveAfterPassOutput(FRDGBuilder& GraphBuilder, const FSceneView& View,
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

FAnomalySceneViewExtension::FAnomalySceneViewExtension(const FAutoRegister& AutoRegister,
	const TSharedPtr<FAnomalySveCapturer, ESPMode::ThreadSafe>& InCapturer)
	: FSceneViewExtensionBase(AutoRegister)
	, Capturer(InCapturer)
{
}

bool FAnomalySceneViewExtension::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	TSharedPtr<FAnomalySveCapturer, ESPMode::ThreadSafe> Cap = Capturer.Pin();
	return Cap.IsValid() && Cap->IsActive();
}

void FAnomalySceneViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	TSharedPtr<FAnomalySveCapturer, ESPMode::ThreadSafe> Cap = Capturer.Pin();
	if (!Cap.IsValid())
	{
		return;
	}

	if (InViewFamily.Views.Num() == 0 || !InViewFamily.Views[0]
		|| InViewFamily.Views[0]->bIsSceneCapture || InViewFamily.Views[0]->bIsReflectionCapture)
	{
		Cap->NoteIneligibleFamily();
		return;
	}

	uint64 RequestId = 0;
	const bool bWanted = Cap->ConsumeWantedForPublish(InViewFamily.FrameNumber, RequestId);
	AnomalySveKeyRing::PublishKey(InViewFamily.FrameNumber, RequestId, bWanted);
}

void FAnomalySceneViewExtension::SubscribeToPostProcessingPass(EPostProcessingPass Pass,
	FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	if (Pass == EPostProcessingPass::VisualizeDepthOfField)
	{
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateLambda(
			[this](FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs) -> FScreenPassTexture
			{
				return AfterPass_RenderThread(GraphBuilder, View, Inputs);
			}));
	}
}

FScreenPassTexture FAnomalySceneViewExtension::AfterPass_RenderThread(FRDGBuilder& GraphBuilder,
	const FSceneView& View, const FPostProcessMaterialInputs& Inputs)
{
	const FScreenPassTexture SceneColor = Inputs.GetInput(EPostProcessMaterialInput::SceneColor);

	TSharedPtr<FAnomalySveCapturer, ESPMode::ThreadSafe> Cap = Capturer.Pin();
	if (!Cap.IsValid() || !SceneColor.IsValid() || View.bIsSceneCapture || View.bIsReflectionCapture)
	{
		return FinalizeSveAfterPassOutput(GraphBuilder, View, Inputs, SceneColor);
	}

	const uint32 FamilyFrame = View.Family ? View.Family->FrameNumber : 0;

	AnomalySveKeyRing::FKeyEntry Entry;
	if (!AnomalySveKeyRing::LookupKey(FamilyFrame, Entry))
	{
		const AnomalySveKeyRing::FCounters Counters = AnomalySveKeyRing::GetCounters();
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(sve): NO KEY for view-family frame %u — frame DROPPED, never labelled by guess ")
			TEXT("(published=%d consumed=%d missed=%d wrapped=%d, forceMiss=%d)."),
			FamilyFrame, Counters.Published, Counters.Consumed, Counters.Missed, Counters.Wrapped,
			AnomalySveKeyRing::IsForceMiss() ? 1 : 0);
		return FinalizeSveAfterPassOutput(GraphBuilder, View, Inputs, SceneColor);
	}

	if (!Entry.bWanted)
	{
		return FinalizeSveAfterPassOutput(GraphBuilder, View, Inputs, SceneColor);
	}

	FRDGTextureRef Texture = SceneColor.Texture;
	const FIntRect Rect = SceneColor.ViewRect;
	if (!Texture || Rect.Width() <= 0 || Rect.Height() <= 0)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(sve): empty scene-colour rect for frame id=%llu — skipped."), Entry.RequestId);
		return FinalizeSveAfterPassOutput(GraphBuilder, View, Inputs, SceneColor);
	}

	const FIntPoint SourceExtent = Texture->Desc.Extent;
	if (Rect.Min.X < 0 || Rect.Min.Y < 0 || Rect.Max.X > SourceExtent.X || Rect.Max.Y > SourceExtent.Y)
	{
		++ExtentClampDrops;
		UE_LOG(LogAnomalyCapture, Error,
			TEXT("Capture(sve): EXTENT-CLAMP FIRED — FRAME DROPPED, NOT WRITTEN (clamp drop %d this run). ")
			TEXT("id=%llu viewRect=(%d,%d)-(%d,%d) is NOT INSIDE sceneColour extent %dx%d. Capturing the ")
			TEXT("clamped region instead would silently deliver a DIFFERENT picture than the label describes, ")
			TEXT("so the frame is dropped. This means the view rect and the scene-colour texture disagree ")
			TEXT("about their coordinate space on this host — report these numbers, they are the discriminator."),
			ExtentClampDrops, Entry.RequestId, Rect.Min.X, Rect.Min.Y, Rect.Max.X, Rect.Max.Y,
			SourceExtent.X, SourceExtent.Y);
		return FinalizeSveAfterPassOutput(GraphBuilder, View, Inputs, SceneColor);
	}

	const int32 W = Rect.Width();
	const int32 H = Rect.Height();

	const FRDGTextureDesc OwnDesc = FRDGTextureDesc::Create2D(
		FIntPoint(W, H), Texture->Desc.Format, FClearValueBinding::None,
		TexCreate_ShaderResource | TexCreate_RenderTargetable);
	FRDGTextureRef OwnTexture = GraphBuilder.CreateTexture(OwnDesc, TEXT("AnomalySveColorSubRect"));

	AddCopyTexturePass(GraphBuilder, Texture, OwnTexture,
		FIntPoint(Rect.Min.X, Rect.Min.Y), FIntPoint::ZeroValue, FIntPoint(W, H));

	TUniquePtr<FRHIGPUTextureReadback> Readback = MakeUnique<FRHIGPUTextureReadback>(TEXT("AnomalySveColorReadback"));

	AddEnqueueCopyPass(GraphBuilder, Readback.Get(), OwnTexture);

	Cap->SubmitInFlight_RenderThread(Entry.RequestId, Rect, SourceExtent, Texture->Desc.Format,
		MoveTemp(Readback));

	return FinalizeSveAfterPassOutput(GraphBuilder, View, Inputs, SceneColor);
}

#endif
