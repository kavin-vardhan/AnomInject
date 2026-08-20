#pragma once

#include "CoreMinimal.h"
#include "PixelFormat.h"
#include "AnomalyPreviewCapture.h"
#include "AnomalyViewport.h"
#include "AnomalyAutoInjectorSubsystem.h"

class UWorld;

namespace AnomalyLabel
{
	static constexpr int32 SchemaVersion = 1;

	struct FCaptureSnapshot
	{
		uint64 FrameCounter = 0;
		int32  SessionIndex = 0;
		double TimeSeconds = 0.0;
		double WallSeconds = 0.0;
		float  NearClip = 0.0f;
		FAnomalyViewInfo View;
		TArray<FAutoLiveFireInfo> Fires;
		TArray<uint8>   FireHidden;
		TArray<FVector> FirePos;
	};

	void ConvertTightToBGRA(EPixelFormat Format, int32 BytesPerPixel, const TArray<uint8>& RawBytes,
		int32 W, int32 H, TArray<FColor>& OutPixels);

	bool CaptureLabeledShot(UWorld* World, const FString& OutputDir, AnomalyPreview::EImageFormat Format,
		const FAnomalyViewInfo& ProjectionView, const FString& ImageRelName, int32 SessionIndex,
		double WallSeconds, FString& OutImagePath, FString& OutSidecarPath, int32& OutNumLabels, bool bLog = true,
		bool bWriteLabels = true);

	FString BuildLabelRecordForSnapshot(const FCaptureSnapshot& Snapshot, int32 Width, int32 Height,
		const FString& ImageName, int32& OutNumLabels);

	bool EncodeAndWriteFrame(const FString& OutputDir, AnomalyPreview::EImageFormat OutFormat,
		const TArray<uint8>& RawBytes, EPixelFormat SrcFormat, int32 BytesPerPixel, int32 Width, int32 Height,
		const FString& ImageRelPath, const FString& Record, FCriticalSection& JsonlLock, bool bWriteLabels = true);

	struct FRunManifest
	{
		int32 Seed = 0;
		int32 SettleFrames = 0;
		int32 ViewLagFrames = 0;
		int32 PreFrames = 0;
		int32 PositiveFrames = 0;
		int32 PostFrames = 0;
		int32 BurstCount = 0;
		int32 FrameCap = 0;
		FString SessionId;
		int32 ViewportW = 0;
		int32 ViewportH = 0;
		FString Format;
		uint64 StartFrame = 0;
		FString StartTimeUtc;
		FString Mode;
		FString TargetAnomaly;
		FString TargetActor;
		int32 TargetFps = 30;
		bool bPaced = true;
	};

	bool WriteRunManifest(const FString& RunDir, const FRunManifest& Manifest);

	struct FRingTelemetry
	{
		int32 Published = 0;
		int32 Consumed = 0;
		int32 Missed = 0;
		int32 Wrapped = 0;
		int32 Corrupted = 0;
	};

	bool WriteRunSummary(const FString& RunDir, int32 TotalFrames, int32 PositiveFrames, int32 BurstsDone,
		int32 ZeroMatchBursts, uint64 EndFrame,
		int32 TargetFps, double SustainedWallFps, double SpeedRatio, double StampedFps, bool bPaced, bool bDeliveryMode,
		const FString& ContentClock, int32 NonManifestedEvents, const FString& CapturePath,
		const FRingTelemetry* Ring = nullptr,
		int32 MaskProbeArms = 0, int32 MaskResidualDiscards = 0, int32 MaskNoPassDiscards = 0);


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

	struct FProvenanceRecord
	{
		FString AnomalyId;
		FString Target;
		int32 AnchorIndex = 0;
		float CoveragePct = -1.0f;
		int32 OcclusionSamplesPassed = 0;
		int32 OcclusionSamplesTotal = 0;
		float PollDistance = -1.0f;
		bool bValid = false;
	};

	bool WriteSelectionProvenance(const FString& RunDir, const TArray<FProvenanceRecord>& Records);

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
		FString AnomalyType;
		FString AnomalySubtype;
		TArray<int32> FrameIndices;
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
	};

	bool WriteSessionAnnotation(const FString& RunDir, const FSessionAnnotation& Annotation);
}
