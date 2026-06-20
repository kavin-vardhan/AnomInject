// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UWorld;

/**
 * AnomalyPreview (Slice 1 + m7 capture/labeling) — game-thread capture of the GAME viewport.
 *
 * Uses FViewport::ReadPixels on World->GetGameViewport()->Viewport, which reads the game render target
 * ONLY (no editor chrome) in both docked and separate-window PIE, and in a packaged build — superseding
 * the Slice-0 backbuffer path (which captured the whole editor window in docked PIE) and the
 * screenshot-delegate fallback (which wrote a PNG to disk per frame). No disk writes here (the label
 * writer owns disk I/O). Native viewport resolution — NO downscale (downscaling is a later dataset-prep step).
 *
 * Cost / latency (gotcha — m7 A1): ReadPixels is a SYNCHRONOUS readback (FlushRenderingCommands) that
 * reads the LAST render-thread-completed frame of the game viewport RT. With r.OneFrameThreadLag (default
 * ON) the render thread trails the game thread, so a game-thread state mutation in tick N is NOT in a frame
 * captured during tick N — it appears ~1 frame later. Callers that mutate then immediately capture must
 * settle (the burst state machine does). Call at a MODEST cadence to bound the synchronous stall; the
 * render-thread async readback (OnBackBufferReadyToPresent + game-window filter, or a staged GPU readback)
 * is the documented DEFERRED upgrade for higher fps (and is REQUIRED before framerate-bug anomalies enter
 * the pool — the flush would corrupt the very framerate label). Engine + ImageWrapper only — no RHI/RenderCore/Slate.
 */
namespace AnomalyPreview
{
	/** Output image encoding. PNG = lossless (default for the ML dataset — no compression artifacts a
	 *  bug-detector could mislearn as corruption); JPEG = smaller (the dashboard preview path). */
	enum class EImageFormat : uint8 { PNG, JPEG };

	/** Capture the current game viewport as raw BGRA8 (FColor) pixels — no encode, no disk. Returns false if
	 *  there is no game viewport or the readback failed. OutWidth/OutHeight are the native viewport pixel size. */
	bool CaptureGameViewportRaw(UWorld* World, TArray<FColor>& OutPixels, int32& OutWidth, int32& OutHeight);

	/** Capture + encode the current game viewport to PNG or JPEG. Quality applies to JPEG only (PNG is
	 *  lossless). Returns false on no viewport / readback failure / encode failure. */
	bool CaptureGameViewportEncoded(UWorld* World, EImageFormat Format, TArray<uint8>& OutBytes,
		int32& OutWidth, int32& OutHeight, int32 JpegQuality = 90);

	/** Capture + JPEG-encode the current game viewport (back-compat thin wrapper over CaptureGameViewportEncoded;
	 *  the dashboard preview path). */
	bool CaptureGameViewportJpeg(UWorld* World, TArray<uint8>& OutJpeg, int32& OutWidth, int32& OutHeight, int32 Quality = 60);
}
