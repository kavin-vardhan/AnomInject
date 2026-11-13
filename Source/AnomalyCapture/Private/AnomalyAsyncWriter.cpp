#include "AnomalyAsyncWriter.h"
#if __has_include("Templates/SharedPointerFwd.h")
#include "Templates/SharedPointerFwd.h"
#endif
#include "Templates/SharedPointerInternals.h"

#if ANOMALY_CAPTURE

#include "AnomalyLabelWriter.h"
#include "AnomalyCaptureLog.h"

#include "AnomalyPreviewCapture.h"

#include "Async/Async.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformTime.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Crc.h"

void FAnomalyAsyncWriter::Enqueue(FJob&& Job)
{
	const int64 Bytes = Job.FrozenMask.IsValid() ? (int64)Job.FrozenMask->Num() : (int64)Job.RawBytes.Num();
	const int32 TrackedSession = Job.bGrayMask ? -1 : Job.SessionIndex;
	{
		FScopeLock Lock(&TrackCS);
		PendingBytes += Bytes;
		PendingBytesPeak = FMath::Max(PendingBytesPeak, PendingBytes);
		if (TrackedSession >= 0)
		{
			InFlightSessions.Add(TrackedSession);
		}
	}
	Pending.Increment();

	TSharedRef<FAnomalyAsyncWriter, ESPMode::ThreadSafe> Self = AsShared();
	Async(EAsyncExecution::ThreadPool, [Self, Bytes, TrackedSession, MovedJob = MoveTemp(Job)]() mutable
	{
		Self->Run(MovedJob);
		{
			FScopeLock Lock(&Self->TrackCS);
			Self->PendingBytes -= Bytes;
			if (TrackedSession >= 0)
			{
				Self->InFlightSessions.Remove(TrackedSession);
			}
		}
		Self->Pending.Decrement();
	});
}

bool FAnomalyAsyncWriter::WaitForCapacity(int64 IncomingBytes, int64 ByteCap, int32 CountFloor)
{
	const double Start = FPlatformTime::Seconds();
	bool bWaited = false;
	double LastLog = Start;
	while (Pending.GetValue() >= CountFloor && GetPendingBytes() + IncomingBytes > ByteCap)
	{
		bWaited = true;
		FPlatformProcess::Sleep(0.001f);
		const double Now = FPlatformTime::Seconds();
		if (Now - LastLog >= 2.0)
		{
			LastLog = Now;
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(m56): writer back-pressure - %d job(s), %lld byte(s) in flight against a cap of %lld byte(s); ")
				TEXT("the capture waits (fixed timestep: this costs wall time, never a frame) (%.1f s so far)."),
				Pending.GetValue(), GetPendingBytes(), ByteCap, Now - Start);
		}
		if (Now - Start > 120.0)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(m56): writer back-pressure exceeded 120 s with %d job(s) in flight; enqueueing anyway so the ")
				TEXT("run cannot hang. The frame is still written; memory use may exceed the cap."),
				Pending.GetValue());
			break;
		}
	}
	if (bWaited)
	{
		BackpressureWaits.Increment();
		FScopeLock Lock(&TrackCS);
		BackpressureSeconds += FPlatformTime::Seconds() - Start;
	}
	return bWaited;
}

void FAnomalyAsyncWriter::TakeFailedSessions(TArray<int32>& OutSessions)
{
	FScopeLock Lock(&TrackCS);
	OutSessions.Append(FailedSessions);
	FailedSessions.Reset();
}

void FAnomalyAsyncWriter::GetInFlightSessions(TArray<int32>& OutSessions) const
{
	FScopeLock Lock(&TrackCS);
	OutSessions = InFlightSessions.Array();
	OutSessions.Sort();
}

int64 FAnomalyAsyncWriter::GetPendingBytes() const
{
	FScopeLock Lock(&TrackCS);
	return PendingBytes;
}

int64 FAnomalyAsyncWriter::GetPendingBytesPeak() const
{
	FScopeLock Lock(&TrackCS);
	return PendingBytesPeak;
}

double FAnomalyAsyncWriter::GetBackpressureSeconds() const
{
	FScopeLock Lock(&TrackCS);
	return BackpressureSeconds;
}

void FAnomalyAsyncWriter::Run(FJob& Job)
{
	const auto Receipt = Job.ChangeReceipt;
	auto Change = Receipt.IsValid() && Receipt->Issue.IsValid() ? Receipt->Issue->Stage.Pin() : nullptr;
	if (Job.bGrayMask)
	{
		TArray<uint8> Png;
		const TArray<uint8>& Gray = Job.FrozenMask.IsValid() ? *Job.FrozenMask : Job.RawBytes;
		if (Change.IsValid() && Change->GetGate() == 14 && Job.FrozenMask.IsValid())
		{
			UE_LOG(LogAnomalyCapture, Log, TEXT("Capture(m55): FROZEN-MASK si=%d bytes=%d crc32=%08x"),
				Receipt->Issue->SessionIndex, Gray.Num(), FCrc::MemCrc32(Gray.GetData(), Gray.Num()));
		}
		const bool bForcedFailure = Change.IsValid() && Change->Gate(5, Receipt->Issue->SessionIndex);
		const bool bEncoded = !bForcedFailure && AnomalyPreview::EncodeGray8Png(Gray, Job.Width, Job.Height, Png);
		const FString FullPath = FPaths::Combine(Job.OutputDir, Job.ImageRelPath);
		if (bEncoded)
		{
			IFileManager::Get().MakeDirectory(*FPaths::GetPath(FullPath), true);
		}
		const bool bSaved = bEncoded && FFileHelper::SaveArrayToFile(Png, *FullPath);
		if (bSaved)
		{
			MasksWritten.Increment();
		}
		else
		{
			MasksDropped.Increment();
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(m43): TARGET MASK WRITE FAILED for '%s' (%dx%d, encoded=%d). The labels row for ")
				TEXT("this frame names a file that does not exist; target_mask_frames_unavailable counts it."),
				*Job.ImageRelPath, Job.Width, Job.Height, bEncoded ? 1 : 0);
		}
		if (Change.IsValid()) { Change->CompleteMask({ Receipt, bSaved, true, TEXT("mask") }); }
		return;
	}

	bool bResampled = false;
	bool bLabelFailed = false;
	FAnomalyChangeColourPtr Canonical;
	const bool bForcedFailure = Change.IsValid() && Change->Gate(3, Receipt->Issue->SessionIndex);
	const bool bOk = !bForcedFailure && AnomalyLabel::EncodeAndWriteFrame(Job.OutputDir, Job.OutFormat, Job.RawBytes,
		Job.SrcFormat, Job.BytesPerPixel, Job.Width, Job.Height, Job.OutWidth, Job.OutHeight,
		Job.ImageRelPath, Job.Record, JsonlCS, Job.bWriteLabels, bResampled, Change.IsValid() ? &Canonical : nullptr,
		&bLabelFailed);
	if (bLabelFailed)
	{
		LabelRowsFailed.Increment();
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(m56): LABEL ROW APPEND FAILED for '%s' - the image was written but its labels.jsonl row was not. ")
			TEXT("run_summary.label_rows_failed counts it."),
			*Job.ImageRelPath);
	}
	if (Change.IsValid())
	{
		const bool bKnownFormat = Job.SrcFormat == PF_B8G8R8A8 || Job.SrcFormat == PF_R8G8B8A8
			|| Job.SrcFormat == PF_A2B10G10R10 || Job.SrcFormat == PF_FloatRGBA;
		const bool bSupported = bKnownFormat && Job.OutFormat == AnomalyPreview::EImageFormat::PNG
			&& Job.Width == Job.OutWidth && Job.Height == Job.OutHeight;
		Change->Colour({ Receipt, bOk, false, TEXT("writer") }, Canonical, bSupported);
	}

	if (bOk)
	{
		FramesWritten.Increment();
		if (bResampled)
		{
			ResamplesPerformed.Increment();
		}
		NoteWrittenSize(Job.OutWidth, Job.OutHeight, Job.ImageRelPath);
		if (Job.bPositive)
		{
			PositiveWritten.Increment();
		}
		if (Job.bSettle)
		{
			SettleWritten.Increment();
		}
	}
	else
	{
		Dropped.Increment();
		if (Job.SessionIndex >= 0)
		{
			FScopeLock Lock(&TrackCS);
			FailedSessions.Add(Job.SessionIndex);
		}
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(m56): FRAME WRITE FAILED for '%s' (session_index %d) - the file does not exist; the frame is ")
			TEXT("removed from annotation.json and counted in run_summary.frames_dropped with cause write_failed."),
			*Job.ImageRelPath, Job.SessionIndex);
	}
}

void FAnomalyAsyncWriter::NoteWrittenSize(int32 W, int32 H, const FString& ImageRelPath)
{
	int32 KnownW = 0;
	int32 KnownH = 0;
	{
		FScopeLock Lock(&DimCS);
		if (FirstWrittenW <= 0 || FirstWrittenH <= 0)
		{
			FirstWrittenW = W;
			FirstWrittenH = H;
			return;
		}
		KnownW = FirstWrittenW;
		KnownH = FirstWrittenH;
	}

	if (KnownW != W || KnownH != H)
	{
		DimMismatches.Increment();
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(m28): FRAME DIMENSIONS CHANGED MID-RUN - '%s' was written at %dx%d but the FIRST written ")
			TEXT("frame of this session was %dx%d. annotation.json video.resolution reports the FIRST frame's pair, ")
			TEXT("so it does NOT describe this frame. The session's frames are not all one size and any consumer ")
			TEXT("that assumes they are - the mp4 encode included - is now wrong for at least one frame."),
			*ImageRelPath, W, H, KnownW, KnownH);
	}
}

void FAnomalyAsyncWriter::GetFirstWrittenSize(int32& OutW, int32& OutH) const
{
	FScopeLock Lock(&DimCS);
	OutW = FirstWrittenW;
	OutH = FirstWrittenH;
}

bool FAnomalyAsyncWriter::FlushPending(double TimeoutSeconds)
{
	const double Start = FPlatformTime::Seconds();
	double LastLog = Start;
	while (Pending.GetValue() > 0)
	{
		const double Now = FPlatformTime::Seconds();
		if (Now - Start > TimeoutSeconds)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(async): writer flush hit its %.0f s hard cap with %d job(s) still pending. Every unfinished frame ")
				TEXT("is counted as writer_timeout in run_summary.frames_dropped."),
				TimeoutSeconds, Pending.GetValue());
			return false;
		}
		if (Now - LastLog >= 2.0)
		{
			LastLog = Now;
			UE_LOG(LogAnomalyCapture, Log, TEXT("Capture(async): writer flush - %d job(s) still pending after %.1f s."),
				Pending.GetValue(), Now - Start);
		}
		FPlatformProcess::Sleep(0.002f);
	}
	return true;
}

void FAnomalyAsyncWriter::ResetCounters()
{
	FramesWritten.Reset();
	PositiveWritten.Reset();
	SettleWritten.Reset();
	Dropped.Reset();
	ResamplesPerformed.Reset();
	DimMismatches.Reset();
	MasksWritten.Reset();
	MasksDropped.Reset();
	BackpressureWaits.Reset();
	LabelRowsFailed.Reset();
	{
		FScopeLock TrackLock(&TrackCS);
		FailedSessions.Reset();
		PendingBytesPeak = PendingBytes;
		BackpressureSeconds = 0.0;
	}
	FScopeLock Lock(&DimCS);
	FirstWrittenW = 0;
	FirstWrittenH = 0;
}

#endif
