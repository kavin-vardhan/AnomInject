#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "SceneViewExtension.h"

class FAnomalySveCapturer;

class FAnomalySceneViewExtension : public FSceneViewExtensionBase
{
public:
	FAnomalySceneViewExtension(const FAutoRegister& AutoRegister,
		const TSharedPtr<FAnomalySveCapturer, ESPMode::ThreadSafe>& InCapturer, UWorld* InWorld, FViewport* InViewport);

	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;

	virtual void SubscribeToPostProcessingPass(EPostProcessingPass Pass,
		FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;

protected:
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;

private:
	FScreenPassTexture AfterPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View,
		const FPostProcessMaterialInputs& Inputs);

	TWeakPtr<FAnomalySveCapturer, ESPMode::ThreadSafe> Capturer;
	int32 ExtentClampDrops = 0;
	TWeakObjectPtr<UWorld> CaptureWorld;
	FViewport* CaptureViewport = nullptr;
};

#endif
