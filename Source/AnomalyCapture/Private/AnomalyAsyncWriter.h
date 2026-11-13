#pragma once

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#if __has_include("Templates/SharedPointerFwd.h")
#include "Templates/SharedPointerFwd.h"
#endif
#include "Templates/SharedPointerInternals.h"

#if ANOMALY_CAPTURE

#include "HAL/ThreadSafeCounter.h"
#include "PixelFormat.h"
#include "AnomalyPreviewCapture.h"
#include "AnomalyChangeStage.h"

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
		bool bSettle = false;
		int32 SessionIndex = -1;
		bool bWriteLabels = true;
		bool bGrayMask = false;
		FAnomalyChangeReceiptPtr ChangeReceipt;
		FAnomalyChangeMaskPtr FrozenMask;
	};

	void Enqueue(FJob&& Job);
	bool FlushPending(double TimeoutSeconds);
	void ResetCounters();
	bool WaitForCapacity(int64 IncomingBytes, int64 ByteCap, int32 CountFloor);
	void TakeFailedSessions(TArray<int32>& OutSessions);
	void GetInFlightSessions(TArray<int32>& OutSessions) const;
	int64 GetPendingBytes() const;
	int64 GetPendingBytesPeak() const;
	int32 GetBackpressureWaits() const { return BackpressureWaits.GetValue(); }
	double GetBackpressureSeconds() const;
	int32 GetLabelRowsFailed() const { return LabelRowsFailed.GetValue(); }

	int32 GetFramesWritten() const { return FramesWritten.GetValue(); }
	int32 GetPositiveWritten() const { return PositiveWritten.GetValue(); }
	int32 GetSettleWritten() const { return SettleWritten.GetValue(); }
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
	FThreadSafeCounter SettleWritten;
	FThreadSafeCounter Pending;
	FThreadSafeCounter Dropped;
	FThreadSafeCounter ResamplesPerformed;
	FThreadSafeCounter DimMismatches;
	FThreadSafeCounter MasksWritten;
	FThreadSafeCounter MasksDropped;
	FThreadSafeCounter BackpressureWaits;
	FThreadSafeCounter LabelRowsFailed;
	FCriticalSection JsonlCS;

	mutable FCriticalSection TrackCS;
	TSet<int32> InFlightSessions;
	TArray<int32> FailedSessions;
	int64 PendingBytes = 0;
	int64 PendingBytesPeak = 0;
	double BackpressureSeconds = 0.0;

	mutable FCriticalSection DimCS;
	int32 FirstWrittenW = 0;
	int32 FirstWrittenH = 0;
};

#endif
