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

	bool WriteRunSummary(const FString& RunDir, int32 TotalFrames, int32 PositiveFrames, int32 BurstsDone,
		int32 ZeroMatchBursts, uint64 EndFrame,
		int32 TargetFps, double SustainedWallFps, double SpeedRatio, double StampedFps, bool bPaced, bool bDeliveryMode);


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
	};

	struct FSessionEvent
	{
		FString AnomalyType;
		FString AnomalySubtype;
		TArray<int32> FrameIndices;
		double CoverageRatio = 0.0;

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
	};

	struct FSessionAnnotation
	{
		FString SessionId;
		FSessionVideo Video;
		TArray<FSessionEvent> Events;
	};

	bool WriteSessionAnnotation(const FString& RunDir, const FSessionAnnotation& Annotation);
}
