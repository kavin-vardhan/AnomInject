// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include "HAL/ThreadSafeBool.h"
#include "RHIResources.h"   // FTexture2DRHIRef (5.1 has no RHIFwd.h; it aliases FTextureRHIRef)

class SWindow;

/**
 * FAnomalyPreviewCapture (Slice 0 spike) — grabs the game backbuffer via the render-thread delegate
 * FSlateRenderer::OnBackBufferReadyToPresent (verified present on 5.1, SlateRenderer.h:300), reads it
 * back to CPU FColor, and hands the newest frame to the game thread on demand.
 *
 * RATIFIED preview path: OnBackBufferReadyToPresent (no disk writes, true presented frame) +
 * ImageWrapper JPEG. JPEG encoding happens on the GAME thread (TakeEncodedJpeg) to keep the render
 * thread doing only the (synchronous) readback. Throttle the request rate from the game thread.
 *
 * Known spike cost: ReadSurfaceData is a synchronous GPU readback (a render-thread stall). Acceptable at
 * the ~2 fps spike cadence; a later "real" path can use async/staged readback. Window targeting (only
 * capturing the game viewport's backbuffer, not editor windows) is THE correctness item the owner
 * smoke-test validates. If the RHI readback proves intractable, the documented fallback is
 * UGameViewportClient::OnScreenshotCaptured (game-thread, but writes a PNG to disk).
 */
class FAnomalyPreviewCapture
{
public:
	~FAnomalyPreviewCapture();

	/** Register the render-thread backbuffer delegate. Safe no-op if Slate isn't initialized. */
	void Start();

	/** Unregister the delegate and flush rendering so no in-flight callback touches us. Idempotent. */
	void Stop();

	bool IsRunning() const { return bRunning; }

	/** Game thread: request that the next matching present be grabbed. InTargetWindow filters to the game
	 *  viewport's window (pass an invalid weak ptr to accept any window). */
	void RequestCapture(TWeakPtr<SWindow> InTargetWindow);

	/** Game thread: if a raw frame was grabbed since the last call, encode it to JPEG and return true. */
	bool TakeEncodedJpeg(TArray<uint8>& OutJpeg, int32& OutWidth, int32& OutHeight, int32 Quality = 60);

private:
	void OnBackBufferReady(SWindow& Window, const FTexture2DRHIRef& BackBuffer); // RENDER THREAD

	FDelegateHandle Handle;
	bool bRunning = false;

	// render -> game handoff: single newest-wins slot under a lock.
	FCriticalSection Mutex;
	TArray<FColor> PendingPixels;
	FIntPoint PendingSize = FIntPoint::ZeroValue;
	bool bHasPending = false;

	FThreadSafeBool bCaptureRequested;
	TWeakPtr<SWindow> TargetWindow;
};
