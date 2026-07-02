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
		uint64 FrameCounter = 0;      // GFrameCounter at submit time (engine frame; kept for debug / cross-thread key)
		int32  SessionIndex = 0;      // session-local 0-based frame ordinal (matches Actual_Frames/frame_%05d)
		double TimeSeconds = 0.0;
		float  NearClip = 0.0f;       // view near-clip (GNearClippingPlane) at submit time
		FAnomalyViewInfo View;
		TArray<FAutoLiveFireInfo> Fires;
		// Submit-time per-fire state, index-aligned with Fires (the async readback resolves later, so the
		// fast-toggling hidden flag + the actor position MUST be sampled here, not at completion).
		TArray<uint8>   FireHidden;   // 1 = actor hidden this frame (blink off-phase / missing_object)
		TArray<FVector> FirePos;      // actor world location at submit time
	};

	// Synchronous path (legacy / fallback): grabs the game viewport (ReadPixels) + projects + writes,
	// all on the calling tick. ProjectionView supplies the (optionally lagged) projection view.
	// ImageRelName is the image path relative to OutputDir (session runs pass "Actual_Frames/frame_%05d.<ext>";
	// the manual single-shot passes a flat "frame_<GFrameCounter>.<ext>"). SessionIndex is recorded as
	// session_index in the label record.
	bool CaptureLabeledShot(UWorld* World, const FString& OutputDir, AnomalyPreview::EImageFormat Format,
		const FAnomalyViewInfo& ProjectionView, const FString& ImageRelName, int32 SessionIndex,
		FString& OutImagePath, FString& OutSidecarPath, int32& OutNumLabels, bool bLog = true);

	// Async path, GAME THREAD: build the JSONL label record string from the submit-time snapshot.
	// This is the part that touches UObjects (projects the target actor's bounds), so it must stay
	// on the game thread; it is light (a projection + JSON serialize). ImageName must match the file
	// the worker writes (frame_<FrameCounter>.<ext>).
	FString BuildLabelRecordForSnapshot(const FCaptureSnapshot& Snapshot, int32 Width, int32 Height,
		const FString& ImageName, int32& OutNumLabels);

	// Async path, WORKER THREAD: convert the tight native-format pixels -> BGRA, encode (PNG/JPEG),
	// write the image at OutputDir/ImageRelPath (parent dirs created), and append the pre-built Record to
	// labels.jsonl under JsonlLock. ImageRelPath MUST match the "image" field baked into Record.
	// Touches NO UObjects -> safe off the game thread.
	bool EncodeAndWriteFrame(const FString& OutputDir, AnomalyPreview::EImageFormat OutFormat,
		const TArray<uint8>& RawBytes, EPixelFormat SrcFormat, int32 BytesPerPixel, int32 Width, int32 Height,
		const FString& ImageRelPath, const FString& Record, FCriticalSection& JsonlLock);

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
	};

	bool WriteRunManifest(const FString& RunDir, const FRunManifest& Manifest);

	bool WriteRunSummary(const FString& RunDir, int32 TotalFrames, int32 PositiveFrames, int32 BurstsDone,
		int32 ZeroMatchBursts, uint64 EndFrame);

	// ---- Native multi-anomaly session annotation (annotation.json) ----------------------------------
	// A superset of the client's per-clip schema: one shared video envelope + an anomalies[] array whose
	// each element mirrors exactly one client clip (the slicer, Stage 5, cuts these into singular clips).

	struct FSessionVideo
	{
		FString FramesDir;           // "Actual_Frames"
		FString VideoPath;           // "Video_Clip/<session>.mp4" (host ffmpeg produces it, Stage 3)
		int32 ResolutionW = 0;
		int32 ResolutionH = 0;
		double Fps = 30.0;           // MEASURED session rate (world-time; may be fractional) = mp4 playback fps
		int32 TotalFrames = 0;
	};

	struct FSessionNode
	{
		FString Name;
		FString Path;                // AActor::GetPathName()
		FVector GlobalPosition = FVector::ZeroVector;  // GetActorLocation() at injection moment
	};

	// One per fire (id, target, start_frame). Mirrors one client clip.
	struct FSessionEvent
	{
		FString AnomalyType;         // client vocab, e.g. "blink"
		FString AnomalySubtype;      // e.g. "disappear_reappear" / "flicker" (derived from the toggle pattern)
		FString SourceId;            // internal id, e.g. "blinking" (dropped by the slicer)
		TArray<int32> AffectedFrames; // session-local indices where the fire was live AND bbox_valid
		double CoverageRatio = 0.0;  // mean projected-bbox-area / frame-area over AffectedFrames (approx)

		TArray<FSessionNode> Nodes;
		int32 PrimaryIndex = 0;

		// camera-at-event (client-required: path/position/near/far; rotation/fov/aspect are bonus)
		FString CamPath;
		FVector CamPosition = FVector::ZeroVector;
		FRotator CamRotation = FRotator::ZeroRotator;
		float CamFovDeg = 0.0f;
		float CamAspect = 0.0f;
		float CamNear = 0.0f;
		float CamFar = 0.0f;

		// engine-at-event (ticks_msec is client-required + reused verbatim in the clip filename)
		int64 TicksMsec = 0;
		FString EngineName;
		FString EngineVersion;
		FString EngineProject;

		// diagnostic (also drives AnomalySubtype): visibility transitions observed over the hold.
		// HiddenFrameList = session-indices where the actor was actually render-hidden (the OUT frames the
		// slicer needs — the client clip's affected_frames are these, not our full live/bbox_valid span).
		int32 VisibleFrames = 0;
		int32 HiddenFrames = 0;
		int32 Transitions = 0;
		TArray<int32> HiddenFrameList;
	};

	struct FSessionAnnotation
	{
		FString SchemaVersion = TEXT("iai-session-1");
		FString SessionId;
		FSessionVideo Video;
		TArray<FSessionEvent> Events;
	};

	bool WriteSessionAnnotation(const FString& RunDir, const FSessionAnnotation& Annotation);
}
