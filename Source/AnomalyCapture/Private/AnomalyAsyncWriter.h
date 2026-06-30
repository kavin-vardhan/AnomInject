#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "HAL/ThreadSafeCounter.h"
#include "PixelFormat.h"
#include "AnomalyPreviewCapture.h"

// Off-game-thread frame writer. The game thread only ENQUEUES a job (cheap move of the raw pixel
// bytes + the already-built label record); a thread-pool task does the heavy CPU work — format
// convert + PNG/JPEG encode + image file write + labels.jsonl append. This is the fix for the
// per-frame game-thread stall (encode+write were synchronous on the game thread). The labels.jsonl
// append is serialized by a shared lock; counters are atomic and mirrored back on the game thread.
class FAnomalyAsyncWriter : public TSharedFromThis<FAnomalyAsyncWriter, ESPMode::ThreadSafe>
{
public:
	struct FJob
	{
		FString OutputDir;
		AnomalyPreview::EImageFormat OutFormat = AnomalyPreview::EImageFormat::PNG;
		TArray<uint8> RawBytes;        // tight (stride-removed) source pixels, native format
		EPixelFormat SrcFormat = PF_Unknown;
		int32 BytesPerPixel = 0;
		int32 Width = 0;
		int32 Height = 0;
		uint64 FrameIndex = 0;
		FString Record;                // label record JSON, built on the game thread (UObject-safe)
		bool bPositive = false;
	};

	void Enqueue(FJob&& Job);
	void FlushPending(double TimeoutSeconds);
	void ResetCounters();

	int32 GetFramesWritten() const { return FramesWritten.GetValue(); }
	int32 GetPositiveWritten() const { return PositiveWritten.GetValue(); }
	int32 GetPending() const { return Pending.GetValue(); }
	int32 GetDropped() const { return Dropped.GetValue(); }

private:
	void Run(FJob& Job);

	FThreadSafeCounter FramesWritten;
	FThreadSafeCounter PositiveWritten;
	FThreadSafeCounter Pending;
	FThreadSafeCounter Dropped;
	FCriticalSection JsonlCS;
};

#endif // ANOMALY_CAPTURE
