#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "HAL/ThreadSafeCounter.h"
#include "PixelFormat.h"
#include "AnomalyPreviewCapture.h"

class FAnomalyAsyncWriter : public TSharedFromThis<FAnomalyAsyncWriter, ESPMode::ThreadSafe>
{
public:
	struct FJob
	{
		FString OutputDir;
		AnomalyPreview::EImageFormat OutFormat = AnomalyPreview::EImageFormat::PNG;
		TArray<uint8> RawBytes;
		EPixelFormat SrcFormat = PF_Unknown;
		int32 BytesPerPixel = 0;
		int32 Width = 0;
		int32 Height = 0;
		FString ImageRelPath;
		FString Record;
		bool bPositive = false;
		bool bWriteLabels = true;
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

#endif
