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
		int32 OutWidth = 0;
		int32 OutHeight = 0;
		FString ImageRelPath;
		FString Record;
		bool bPositive = false;
		bool bWriteLabels = true;
		bool bGrayMask = false;
	};

	void Enqueue(FJob&& Job);
	void FlushPending(double TimeoutSeconds);
	void ResetCounters();

	int32 GetFramesWritten() const { return FramesWritten.GetValue(); }
	int32 GetPositiveWritten() const { return PositiveWritten.GetValue(); }
	int32 GetPending() const { return Pending.GetValue(); }
	int32 GetDropped() const { return Dropped.GetValue(); }

	int32 GetMasksWritten() const { return MasksWritten.GetValue(); }
	int32 GetMasksDropped() const { return MasksDropped.GetValue(); }
	int32 GetResamplesPerformed() const { return ResamplesPerformed.GetValue(); }
	int32 GetDimMismatches() const { return DimMismatches.GetValue(); }
	void GetFirstWrittenSize(int32& OutW, int32& OutH) const;

private:
	void Run(FJob& Job);
	void NoteWrittenSize(int32 W, int32 H, const FString& ImageRelPath);

	FThreadSafeCounter FramesWritten;
	FThreadSafeCounter PositiveWritten;
	FThreadSafeCounter Pending;
	FThreadSafeCounter Dropped;
	FThreadSafeCounter ResamplesPerformed;
	FThreadSafeCounter DimMismatches;
	FThreadSafeCounter MasksWritten;
	FThreadSafeCounter MasksDropped;
	FCriticalSection JsonlCS;

	mutable FCriticalSection DimCS;
	int32 FirstWrittenW = 0;
	int32 FirstWrittenH = 0;
};

#endif
