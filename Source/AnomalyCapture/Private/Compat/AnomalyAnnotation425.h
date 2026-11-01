#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"
namespace AnomalyAnnotation425
{
struct FSessionTransition { TArray<int32> Frames; TMap<FString,int32> Reasons; };
	struct FSessionVideo
	{
		FString FramesDir;
		FString VideoPath;
		int32 ResolutionW = 0;
		int32 ResolutionH = 0;
		double Fps = 30.0;
		int32 TargetFps = 30;
		int32 TotalFrames = 0;
	};

	struct FSessionNode
	{
		FString Name;
		FString Path;
		FVector GlobalPosition = FVector::ZeroVector;
		FString AssetName;
		FString ComponentClass;
		FVector BoundsOrigin = FVector::ZeroVector;
		FVector BoundsExtent = FVector::ZeroVector;
	};

	struct FSessionEvent
	{
		FString EventId;
		FString AnomalyType;
		FString AnomalySubtype;
		FString StuckMipRoute;
		TArray<int32> FrameIndices;
		TArray<int32> InjectedFrameIndices;
		int32 ObservableFrameCount = 0;
		int32 UnmeasuredFrameCount = 0;
		bool bObservabilityMeasured = true;
		FString BboxSource = TEXT("projected");
		bool bManifested = true;
		double CoverageRatio = 0.0;
		float CoveragePct = -1.0f;

		TArray<FSessionNode> Nodes;
		int32 PrimaryIndex = 0;

		FString CamPath;
		FVector CamPosition = FVector::ZeroVector;
		FRotator CamRotation = FRotator::ZeroRotator;
		float CamFovDeg = 0.0f;
		float CamAspect = 0.0f;
		float CamNear = 0.0f;
		float CamFar = 0.0f;

		int64 TicksMsec = 0;
		FString EngineName;
		FString EngineVersion;
		FString EngineProject;

		bool bMaskProvided = false;
	};

	struct FSessionAnnotation
	{
		FString SessionId;
		FSessionVideo Video;
		TArray<FSessionEvent> Events;
		TMap<FString, FSessionTransition> Transitions;
	};

	bool WriteSessionAnnotation(const FString& RunDir, const FSessionAnnotation& Annotation);
}
#endif
