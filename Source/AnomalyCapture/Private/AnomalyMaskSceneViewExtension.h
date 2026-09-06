#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "SceneViewExtension.h"
#include "RHIGPUReadback.h"
#include "AnomalyMaskTypes.h"
#include "AnomalyFrameCapturer.h"

struct FScreenPassTexture;
struct FPostProcessMaterialInputs;
class FRDGBuilder;
class FSceneView;

enum class EAnomalyMaskReduceMode : uint8
{
	Gpu = 0,
	Cpu = 1,
	Both = 2
};

inline const TCHAR* LexToStringAnomalyMaskReduceMode(EAnomalyMaskReduceMode Mode)
{
	switch (Mode)
	{
	case EAnomalyMaskReduceMode::Cpu:  return TEXT("cpu");
	case EAnomalyMaskReduceMode::Both: return TEXT("both");
	default:                           return TEXT("gpu");
	}
}

class FAnomalyMaskSceneViewExtension : public FSceneViewExtensionBase
{
public:
	FAnomalyMaskSceneViewExtension(const FAutoRegister& AutoRegister, UWorld* InWorld, FViewport* InViewport);

	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SubscribeToPostProcessingPass(EPostProcessingPass Pass,
		FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;
	virtual int32 GetPriority() const override { return -1; }

protected:
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;

public:
	void ArmMask(uint64 RequestId, bool bWantPixels = false);
	void SetAssignedTags(const TSet<uint8>& InAssignedTags);
	void SetReduceMode(EAnomalyMaskReduceMode InMode);
	void EnqueueDrain(bool bFinal = false);
	bool TakeMaskResult(uint64 RequestId, FAnomalyMaskResult& Out, bool bRemove = true);
	int32 NumPendingArms() const;
	void Reset();

private:
	FScreenPassTexture AfterTonemap_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View,
		const FPostProcessMaterialInputs& Inputs);
	void Drain_RenderThread(bool bFinal);

	struct FMaskInFlight
	{
		uint64 RequestId = 0;
		uint32 RenderFrame = 0;
		uint32 Generation = 0;
		TSet<uint8> Assigned;
		TArray<uint64> RequestIds;
		TArray<uint8> WantsPixels;
		TUniquePtr<FRHIGPUTextureReadback> Readback;
		TUniquePtr<FRHIGPUBufferReadback> BufferReadback;
		EAnomalyMaskReduceMode Mode = EAnomalyMaskReduceMode::Gpu;
		FIntPoint ViewRectSize = FIntPoint::ZeroValue;
		int32 CustomDepthModeAtPass = -1;
		FIntPoint CustomStencilExtent = FIntPoint::ZeroValue;
		bool bWantPixels = false;
	};

	mutable FCriticalSection StateCS;
	TArray<uint64> PendingArms;
	TMap<uint64, uint64> PendingGameFrames;
	struct FPublishedBatch
	{
		TArray<uint64> Ids;
		TArray<uint8> WantsPixels;
		TSet<uint8> Assigned;
		uint32 Generation = 0;
	};
	TMap<uint32, FPublishedBatch> Published;
	uint32 Generation = 0;
	TWeakObjectPtr<UWorld> CaptureWorld;
	FViewport* CaptureViewport = nullptr;
	TArray<uint8> PendingArmWantsPixels;
	TSet<uint8> AssignedTags;
	EAnomalyMaskReduceMode ReduceMode = EAnomalyMaskReduceMode::Gpu;
	float DepthBias = 1.0e-5f;

	TArray<FMaskInFlight> InFlight;

	mutable FCriticalSection ResultsCS;
	TMap<uint64, FAnomalyMaskResult> Results;

	FThreadSafeCounter GuardDrops;
};

#endif
