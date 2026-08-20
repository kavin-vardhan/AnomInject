#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "SceneViewExtension.h"
#include "RHIGPUReadback.h"
#include "AnomalyMaskTypes.h"

struct FScreenPassTexture;
struct FPostProcessMaterialInputs;
class FRDGBuilder;
class FSceneView;

class FAnomalyMaskSceneViewExtension : public FSceneViewExtensionBase
{
public:
	FAnomalyMaskSceneViewExtension(const FAutoRegister& AutoRegister);

	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SubscribeToPostProcessingPass(EPostProcessingPass Pass,
		FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;
	virtual int32 GetPriority() const override { return -1; }

protected:
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;

public:
	void ArmMask(uint64 RequestId);
	void SetAssignedTags(const TSet<uint8>& InAssignedTags);
	void EnqueueDrain();
	bool TakeMaskResult(uint64 RequestId, FAnomalyMaskResult& Out);
	int32 NumPendingArms() const;
	void Reset();

private:
	FScreenPassTexture AfterTonemap_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View,
		const FPostProcessMaterialInputs& Inputs);
	void Drain_RenderThread();

	struct FMaskInFlight
	{
		uint64 RequestId = 0;
		TUniquePtr<FRHIGPUTextureReadback> Readback;
		FIntPoint ViewRectSize = FIntPoint::ZeroValue;
		int32 CustomDepthModeAtPass = -1;
		FIntPoint CustomStencilExtent = FIntPoint::ZeroValue;
	};

	mutable FCriticalSection StateCS;
	TArray<uint64> PendingArms;
	TSet<uint8> AssignedTags;
	float DepthBias = 1.0e-5f;

	TArray<FMaskInFlight> InFlight;

	mutable FCriticalSection ResultsCS;
	TMap<uint64, FAnomalyMaskResult> Results;
};

#endif
