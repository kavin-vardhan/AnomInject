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

	// A frame's label state captured at SUBMIT time (game thread). For the async path the GPU
	// readback resolves on a LATER frame, so the label must be stamped with the submit frame's
	// state, not the resolve frame's. The sync path fills this inline at the capture tick.
	struct FCaptureSnapshot
	{
		uint64 FrameCounter = 0;
		double TimeSeconds = 0.0;
		FAnomalyViewInfo View;
		TArray<FAutoLiveFireInfo> Fires;
	};

	// Synchronous path (legacy / fallback): grabs the game viewport (ReadPixels) + projects + writes,
	// all on the calling tick. ProjectionView supplies the (optionally lagged) projection view.
	bool CaptureLabeledShot(UWorld* World, const FString& OutputDir, AnomalyPreview::EImageFormat Format,
		const FAnomalyViewInfo& ProjectionView,
		FString& OutImagePath, FString& OutSidecarPath, int32& OutNumLabels, bool bLog = true);

	// Async path, GAME THREAD: build the JSONL label record string from the submit-time snapshot.
	// This is the part that touches UObjects (projects the target actor's bounds), so it must stay
	// on the game thread; it is light (a projection + JSON serialize). ImageName must match the file
	// the worker writes (frame_<FrameCounter>.<ext>).
	FString BuildLabelRecordForSnapshot(const FCaptureSnapshot& Snapshot, int32 Width, int32 Height,
		const FString& ImageName, int32& OutNumLabels);

	// Async path, WORKER THREAD: convert the tight native-format pixels -> BGRA, encode (PNG/JPEG),
	// write frame_<FrameIndex>.<ext>, and append the pre-built Record to labels.jsonl under JsonlLock.
	// Touches NO UObjects -> safe off the game thread.
	bool EncodeAndWriteFrame(const FString& OutputDir, AnomalyPreview::EImageFormat OutFormat,
		const TArray<uint8>& RawBytes, EPixelFormat SrcFormat, int32 BytesPerPixel, int32 Width, int32 Height,
		uint64 FrameIndex, const FString& Record, FCriticalSection& JsonlLock);

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
