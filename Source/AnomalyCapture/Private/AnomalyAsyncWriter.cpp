#include "AnomalyAsyncWriter.h"
#if __has_include("Templates/SharedPointerFwd.h")
#include "Templates/SharedPointerFwd.h"
#endif
#include "Templates/SharedPointerInternals.h"

#if ANOMALY_CAPTURE

#include "AnomalyLabelWriter.h"
#include "AnomalyCaptureLog.h"
#include "AnomalyWriterProgress.h"

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
	uint32 JobGeneration = 0;
	{
		FScopeLock Lock(&TrackCS);
		JobGeneration = Generation;
		PendingBytes += Bytes;
		PendingBytesPeak = FMath::Max(PendingBytesPeak, PendingBytes);
		++PendingThisRun;
		if (TrackedSession >= 0)
		{
			InFlightSessions.Add(TrackedSession);
		}
	}
	Pending.Increment();

	TSharedRef<FAnomalyAsyncWriter, ESPMode::ThreadSafe> Self = AsShared();
	Async(EAsyncExecution::ThreadPool, [Self, Bytes, TrackedSession, JobGeneration, MovedJob = MoveTemp(Job)]() mutable
	{
		Self->Run(MovedJob, JobGeneration);
		{
			FScopeLock Lock(&Self->TrackCS);
			Self->PendingBytes -= Bytes;
			Self->LastCompletionSeconds = FPlatformTime::Seconds();
			if (AnomalyWriterProgress::CountsForRun(JobGeneration, Self->Generation))
			{
				--Self->PendingThisRun;
				if (TrackedSession >= 0)
				{
					Self->InFlightSessions.Remove(TrackedSession);
				}
			}
			else
			{
				++Self->LateCompletions;
			}
		}
		Self->Pending.Decrement();
	});
}

bool FAnomalyAsyncWriter::CountsForCurrentRun(uint32 JobGeneration) const
{
	FScopeLock Lock(&TrackCS);
	return AnomalyWriterProgress::CountsForRun(JobGeneration, Generation);
}

bool FAnomalyAsyncWriter::IsStalled() const
{
	FScopeLock Lock(&TrackCS);
	return bStalled;
}

int32 FAnomalyAsyncWriter::GetPendingThisRun() const
{
	FScopeLock Lock(&TrackCS);
	return PendingThisRun;
}

int32 FAnomalyAsyncWriter::GetLateCompletions() const
{
	FScopeLock Lock(&TrackCS);
	return LateCompletions;
}

void FAnomalyAsyncWriter::MarkStalled(const TCHAR* Where, double WaitStart)
{
	double LastCompletion = 0.0;
	int32 InRun = 0;
	{
		FScopeLock Lock(&TrackCS);
		if (bStalled)
		{
			return;
		}
		bStalled = true;
		LastCompletion = LastCompletionSeconds;
		InRun = PendingThisRun;
	}
	const double Now = FPlatformTime::Seconds();
	UE_LOG(LogAnomalyCapture, Error,
		TEXT("Capture(m56): WRITER STALLED during %s - no writer job has completed for %.0f s (last completion %.1f s ago, wait ")
		TEXT("began %.1f s ago) with %d job(s) of this run and %d job(s) in total in flight. The capture run stops; every frame ")
		TEXT("of this run that is not written counts as writer_timeout in run_summary.frames_dropped, and run_summary.writer_stalled ")
		TEXT("is true. Files that land after the summary is written are not part of the run."),
		Where, AnomalyWriterProgress::StallWindowSeconds, LastCompletion > 0.0 ? Now - LastCompletion : -1.0, Now - WaitStart,
		InRun, Pending.GetValue());
}

bool FAnomalyAsyncWriter::WaitForCapacity(int64 IncomingBytes, int64 ByteCap, int32 CountFloor)
{
	const double Start = FPlatformTime::Seconds();
	bool bWaited = false;
	double LastLog = Start;
	if (IsStalled())
	{
		return false;
	}
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
				TEXT("the capture waits (fixed timestep: this costs wall time, never a frame) (%.1f s so far). The wait has ")
				TEXT("no total deadline: it ends as a stall only if no writer job completes for %.0f s."),
				Pending.GetValue(), GetPendingBytes(), ByteCap, Now - Start, AnomalyWriterProgress::StallWindowSeconds);
		}
		double LastCompletion = 0.0;
		{
			FScopeLock Lock(&TrackCS);
			LastCompletion = LastCompletionSeconds;
		}
		if (AnomalyWriterProgress::IsStalled(Now, Start, LastCompletion, false, AnomalyWriterProgress::StallWindowSeconds))
		{
			MarkStalled(TEXT("writer back-pressure"), Start);
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

void FAnomalyAsyncWriter::Run(FJob& Job, uint32 JobGeneration)
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
		const bool bCountsMask = CountsForCurrentRun(JobGeneration);
		if (bSaved)
		{
			if (bCountsMask) { MasksWritten.Increment(); }
		}
		else
		{
			if (bCountsMask) { MasksDropped.Increment(); }
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
	const bool bCounts = CountsForCurrentRun(JobGeneration);
	if (!bCounts)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(m56): a writer job of an earlier run finished late ('%s', session_index %d, ok=%d). It is not part of ")
			TEXT("the current run and its outcome touches none of this run's counts or lists."),
			*Job.ImageRelPath, Job.SessionIndex, bOk ? 1 : 0);
	}
	if (bLabelFailed && bCounts)
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

	if (!bCounts)
	{
		return;
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

bool FAnomalyAsyncWriter::FlushPending(double StallWindowSeconds)
{
	const double Start = FPlatformTime::Seconds();
	double LastLog = Start;
	while (GetPendingThisRun() > 0)
	{
		const double Now = FPlatformTime::Seconds();
		double LastCompletion = 0.0;
		bool bAlreadyStalled = false;
		{
			FScopeLock Lock(&TrackCS);
			LastCompletion = LastCompletionSeconds;
			bAlreadyStalled = bStalled;
		}
		if (AnomalyWriterProgress::IsStalled(Now, Start, LastCompletion, bAlreadyStalled, StallWindowSeconds))
		{
			MarkStalled(TEXT("the writer flush"), Start);
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(async): writer flush gave up with %d job(s) of this run still pending (no completion for %.0f s). ")
				TEXT("Every unfinished frame is counted as writer_timeout in run_summary.frames_dropped."),
				GetPendingThisRun(), StallWindowSeconds);
			return false;
		}
		if (Now - LastLog >= 2.0)
		{
			LastLog = Now;
			UE_LOG(LogAnomalyCapture, Log, TEXT("Capture(async): writer flush - %d job(s) still pending after %.1f s."),
				GetPendingThisRun(), Now - Start);
		}
		FPlatformProcess::Sleep(0.002f);
	}
	return true;
}

void FAnomalyAsyncWriter::ResetCounters()
{
	int32 Leftover = 0;
	{
		FScopeLock TrackLock(&TrackCS);
		Leftover = PendingThisRun;
		++Generation;
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
		FailedSessions.Reset();
		InFlightSessions.Reset();
		PendingThisRun = 0;
		LateCompletions = 0;
		bStalled = false;
		PendingBytesPeak = PendingBytes;
		BackpressureSeconds = 0.0;
	}
	if (Leftover > 0)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(m56): %d writer job(s) of an earlier run are still in flight. They finish in the background and never ")
			TEXT("touch this run's counts or lists (each job carries its run generation)."),
			Leftover);
	}
	FScopeLock Lock(&DimCS);
	FirstWrittenW = 0;
	FirstWrittenH = 0;
}

#endif
