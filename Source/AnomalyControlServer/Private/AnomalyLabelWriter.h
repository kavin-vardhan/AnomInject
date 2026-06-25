#pragma once

#include "CoreMinimal.h"
#include "AnomalyPreviewCapture.h"

class UWorld;
struct FAnomalyViewInfo;

namespace AnomalyLabel
{
	static constexpr int32 SchemaVersion = 1;

	bool CaptureLabeledShot(UWorld* World, const FString& OutputDir, AnomalyPreview::EImageFormat Format,
		const FAnomalyViewInfo& ProjectionView,
		FString& OutImagePath, FString& OutSidecarPath, int32& OutNumLabels, bool bLog = true);

	struct FRunManifest
	{
		int32 Seed = 0;
		int32 SettleFrames = 0;
		int32 ViewLagFrames = 0;
		int32 PreFrames = 0;
		int32 PositiveFrames = 0;
		int32 PostFrames = 0;
		int32 BurstCount = 0;
		int32 ViewportW = 0;
		int32 ViewportH = 0;
		FString Format;
		uint64 StartFrame = 0;
		FString StartTimeUtc;
	};

	bool WriteRunManifest(const FString& RunDir, const FRunManifest& Manifest);

	bool WriteRunSummary(const FString& RunDir, int32 TotalFrames, int32 PositiveFrames, int32 BurstsDone,
		int32 ZeroMatchBursts, uint64 EndFrame);
}
