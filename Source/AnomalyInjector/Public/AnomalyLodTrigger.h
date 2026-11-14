#pragma once

#include "CoreMinimal.h"
#include "AnomalyLod.h"
#include "AnomalyLodTriggerRule.h"

class UWorld;
class AActor;
class UMeshComponent;
class UStaticMeshComponent;

namespace AnomalyLodTrigger
{
	struct FMotion
	{
		bool bValid = false;
		FVector CameraNow = FVector::ZeroVector;
		FVector CameraOld = FVector::ZeroVector;
		float YawNow = 0.0f;
		float YawRateDegPerS = 0.0f;
		float Seconds = 0.0f;
		float FrameSeconds = 0.0f;
		int32 FramesBack = 0;
	};

	struct FArmEvaluation
	{
		bool bArmable = false;
		AnomalyLodTriggerRule::EMode Mode = AnomalyLodTriggerRule::EMode::Auto;
		FString Refusal;
		int32 HeldLod = -1;
		float ReleaseScreenSize = -1.0f;
		float LookAheadScreenSize = -1.0f;
		int32 FramesAhead = -1;
		float ClosingSpeedCmPerS = 0.0f;
		float AngleOutsideFrustumDeg = -1.0f;
		AnomalyLod::FEngineLodPrediction Prediction;
	};

	enum class ECaptureRenderRecord : uint8 { Unknown = 0, Available = 1, Unavailable = 2 };

	ANOMALYINJECTOR_API void SetCaptureRenderRecord(ECaptureRenderRecord State);

	ANOMALYINJECTOR_API ECaptureRenderRecord GetCaptureRenderRecord();

	ANOMALYINJECTOR_API AnomalyLodTriggerRule::FSettings GetSettings();

	ANOMALYINJECTOR_API void RecordViewSample(UWorld* World);

	ANOMALYINJECTOR_API bool GetMotion(UWorld* World, FMotion& Out);

	ANOMALYINJECTOR_API const TCHAR* GetStaticRefusal(const UMeshComponent* Component);

	ANOMALYINJECTOR_API float GetComponentCoveragePct(UWorld* World, const UStaticMeshComponent* Component, bool* bOutOnScreen);

	ANOMALYINJECTOR_API float GetDistanceToBoundsCm(const FVector& Camera, const UStaticMeshComponent* Component);

	ANOMALYINJECTOR_API bool IsShadowPossiblyInView(UWorld* World, const UStaticMeshComponent* Component);

	ANOMALYINJECTOR_API FArmEvaluation EvaluateArm(UWorld* World, const AnomalyLod::FLodViewScope& Scope,
		UStaticMeshComponent* Component, AnomalyLodTriggerRule::EMode Requested, const AnomalyLodTriggerRule::FSettings& Settings);

	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> CollectArmableActors(UWorld* World, TMap<FString, int32>& OutRefusals);
}
