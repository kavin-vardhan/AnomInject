#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "RHIGPUReadback.h"
#include "PixelFormat.h"

class SWindow;

// A finished, CPU-side captured frame: the raw (stride-removed) native-format pixels of the game
// viewport rect, top-left origin. The format convert + encode happen later on a worker thread, so
// the render thread does only the mandatory lock-copy-out here (kept minimal -> no game stall).
struct FAnomalyCapturedFrame
{
	uint64 RequestId = 0;   // == submit GFrameCounter; the label frame_index + the snapshot match key
	int32 Width = 0;
	int32 Height = 0;
	EPixelFormat Format = PF_Unknown;
	int32 BytesPerPixel = 0;
	TArray<uint8> RawBytes;
};

// Grab-source-agnostic async frame capture. Stage 1 source = the post-Slate composited backbuffer
// (OnBackBufferReadyToPresent), clipped to the game-viewport rect (FFrameGrabber pattern) so the
// captured frame is exactly what the player sees, GAME UI included, and never the editor chrome.
// The game thread ARMS a capture (one per captured frame) with the target window + viewport rect
// resolved game-side; the render-thread present hook enqueues a staged FRHIGPUTextureReadback for
// the matching window; a per-tick render command drains ready readbacks on a LATER frame. No
// synchronous flush during a run -> framerate not corrupted. (At Stage 3 the stencil/depth SVE
// feeds this same ring for the mask, joined by RequestId.)
class FAnomalyFrameCapturer : public TSharedFromThis<FAnomalyFrameCapturer, ESPMode::ThreadSafe>
{
public:
	FAnomalyFrameCapturer() = default;
	~FAnomalyFrameCapturer();

	// Game-thread API.
	void RegisterBackbufferHook();
	void UnregisterBackbufferHook();
	void ArmForCapture(uint64 RequestId, SWindow* TargetWindow, const FIntRect& CaptureRect);
	void EnqueueDrain();
	bool PopCompleted(FAnomalyCapturedFrame& Out);
	int32 NumPendingApprox() const;

private:
	void OnBackBufferReadyToPresent_RenderThread(SWindow& SlateWindow, const FTexture2DRHIRef& BackBuffer);
	void Drain_RenderThread();

	struct FArm
	{
		uint64 RequestId = 0;
		SWindow* Window = nullptr;   // identity-compare only, never dereferenced off the game thread
		FIntRect Rect;
	};

	struct FInFlight
	{
		uint64 RequestId = 0;
		TUniquePtr<FRHIGPUTextureReadback> Readback;
		FIntRect Rect;
		EPixelFormat Format = PF_Unknown;
	};

	// Arm queue (game -> render). FIFO of submit ids awaiting the next present of their window.
	mutable FCriticalSection StateCS;
	TArray<FArm> PendingArms;

	// In-flight readbacks: created in the present hook, drained in the render command -- both on the
	// render thread, so no lock is needed for this list.
	TArray<FInFlight> InFlight;

	// Completed frames (render -> game).
	mutable FCriticalSection CompletedCS;
	TArray<FAnomalyCapturedFrame> Completed;

	FDelegateHandle BackBufferHandle;
};

#endif // ANOMALY_CAPTURE
