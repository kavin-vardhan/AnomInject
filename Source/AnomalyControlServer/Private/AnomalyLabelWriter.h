// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnomalyPreviewCapture.h"   // AnomalyPreview::EImageFormat

class UWorld;
struct FAnomalyViewInfo;             // AnomalyViewport.h (passed by const-ref; the caller resolves it)

/**
 * AnomalyLabelWriter (m7 capture/labeling, Stage 1) — turns one captured game-viewport frame + the
 * auto-injector's ground-truth live fires into an ML-friendly labeled sample on disk.
 *
 * The label is the injector's OWN ground truth (L1): we fired the anomaly, so we know which id is live on
 * which actor. Per fire, the 2D bounding box is a DIRECT projection of the fired actor's persisted 3D bounds
 * (AnomalyViewport::ProjectActorBoundsToScreenRect) — it works even when the anomaly has HIDDEN the actor
 * (missing_object / flicker), where the box marks "where the hole is" (the correct missing-object label).
 *
 * Alignment (L3 / Q6): the live-fire snapshot and the synchronous viewport capture happen in the SAME
 * game-thread call, so both reflect one frame; image and sidecar are stamped with one GFrameCounter.
 *
 * This is the Stage-1 single-shot path (IAI.Capture.Shot). Stage 2's UAnomalyCaptureSubsystem reuses the
 * same record builder for burst runs. Compiled with the rest of the control-server module
 * (ANOMALY_CONTROL_SERVER) — dataset capture is a dev/research activity, never a retail Shipping build.
 */
namespace AnomalyLabel
{
	/** Schema version of the on-disk record + manifest (additive-stable: bump to evolve, never repurpose). */
	static constexpr int32 SchemaVersion = 1;

	/**
	 * Capture ONE labeled frame on the calling game thread: snapshot the auto-injector's live fires, resolve
	 * the active view, capture the game viewport (Format; native resolution), project each fired actor's
	 * bounds to a pixel bbox, write the image (frame_<GFrameCounter>.<ext>) into OutputDir, and APPEND one
	 * JSONL label record to OutputDir/labels.jsonl. Creates OutputDir if needed.
	 *
	 * ProjectionView: the view to project the bbox with — the caller supplies the view that MATCHES the
	 * captured pixels. For a moving burst run that is the view from L frames ago (the render trails the game
	 * thread by L; see the capture subsystem), NOT the current game-thread view; projecting with the current
	 * view offsets the box by ~L frames of camera motion. An invalid view (bValid=false) => every bbox is
	 * bbox_valid=false, but the frame + temporal label are still written.
	 *
	 * bLog: log a per-shot line (true for the manual IAI.Capture.Shot; false for burst frames to avoid spam).
	 *
	 * Returns false (writing nothing) on no world / capture / encode / IO failure. On success fills
	 * OutImagePath + OutSidecarPath and OutNumLabels (the count of fires that produced a valid on-screen bbox).
	 */
	bool CaptureLabeledShot(UWorld* World, const FString& OutputDir, AnomalyPreview::EImageFormat Format,
		const FAnomalyViewInfo& ProjectionView,
		FString& OutImagePath, FString& OutSidecarPath, int32& OutNumLabels, bool bLog = true);

	/** The run-level manifest (run.json), written at run-START by the capture subsystem. Describes the burst
	 *  schedule + format + viewport so a dataset consumer can interpret labels.jsonl without guessing. */
	struct FRunManifest
	{
		int32 Seed = 0;
		int32 SettleFrames = 0;      // K (skipped at both boundaries)
		int32 ViewLagFrames = 0;     // L (project the bbox with the view this many frames back — render lag)
		int32 PreFrames = 0;         // M lead-in negatives
		int32 PositiveFrames = 0;    // P captured per burst while the fire is live
		int32 PostFrames = 0;        // M negatives after each burst (= next burst's pre-roll, shared)
		int32 BurstCount = 0;        // 0 = loop until Stop
		int32 ViewportW = 0;
		int32 ViewportH = 0;
		FString Format;              // "png" | "jpeg"
		uint64 StartFrame = 0;       // GFrameCounter at run start
		FString StartTimeUtc;        // ISO-8601
	};

	/** Write run.json into RunDir from the manifest. Returns false on IO failure. */
	bool WriteRunManifest(const FString& RunDir, const FRunManifest& Manifest);

	/** Append a small completion summary to run.json's sibling run_summary.json at Stop (nice-to-have). */
	bool WriteRunSummary(const FString& RunDir, int32 TotalFrames, int32 PositiveFrames, int32 BurstsDone,
		int32 ZeroMatchBursts, uint64 EndFrame);
}
