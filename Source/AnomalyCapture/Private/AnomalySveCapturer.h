#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "AnomalyFrameCapturer.h"

#include "HAL/ThreadSafeCounter.h"
#include "PixelFormat.h"
#include "RHIGPUReadback.h"

class FAnomalySveCapturer : public TSharedFromThis<FAnomalySveCapturer, ESPMode::ThreadSafe>
{
public:
	void SetActive(bool bInActive);
	bool IsActive() const;

	void MarkWanted(uint64 GameFrameCounter);
	bool IsWanted(uint64 GameFrameCounter) const;

	void SubmitInFlight_RenderThread(uint64 RequestId, const FIntRect& Rect, EPixelFormat Format,
		TUniquePtr<FRHIGPUTextureReadback>&& Readback);

	void EnqueueDrain();
	bool PopCompleted(FAnomalyCapturedFrame& Out);
	int32 NumPendingApprox() const;

	void Reset();

private:
	void Drain_RenderThread();

	struct FInFlight
	{
		uint64 RequestId = 0;
		TUniquePtr<FRHIGPUTextureReadback> Readback;
		FIntRect Rect;
		EPixelFormat Format = PF_Unknown;
	};

	mutable FCriticalSection StateCS;
	TSet<uint64> WantedFrames;
	FThreadSafeCounter ActiveFlag;

	TArray<FInFlight> InFlight;

	mutable FCriticalSection CompletedCS;
	TArray<FAnomalyCapturedFrame> Completed;
};

#endif
