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

	const uint64 GameFrame = GFrameCounter;
	const bool bWanted = Cap->IsWanted(GameFrame);
	AnomalySveKeyRing::PublishKey(InViewFamily.FrameNumber, GameFrame, bWanted);
	Cap->TraceWantPublish(InViewFamily.FrameNumber, GameFrame, bWanted);
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
		return SceneColor;
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
		return SceneColor;
	}

	if (!Entry.bWanted)
	{
		return SceneColor;
	}

	FRDGTextureRef Texture = SceneColor.Texture;
	const FIntRect Rect = SceneColor.ViewRect;
	if (!Texture || Rect.Width() <= 0 || Rect.Height() <= 0)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(sve): empty scene-colour rect for frame id=%llu — skipped."), Entry.GameFrameCounter);
		return SceneColor;
	}

	TUniquePtr<FRHIGPUTextureReadback> Readback = MakeUnique<FRHIGPUTextureReadback>(TEXT("AnomalySveColorReadback"));

	AddEnqueueCopyPass(GraphBuilder, Readback.Get(), Texture,
		FResolveRect(Rect.Min.X, Rect.Min.Y, Rect.Max.X, Rect.Max.Y));

	Cap->SubmitInFlight_RenderThread(Entry.GameFrameCounter, Rect, Texture->Desc.Format, MoveTemp(Readback));

	return SceneColor;
}

#endif
