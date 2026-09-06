#include "AnomalyAsyncWriter.h"

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

bool FAnomalyAsyncWriter::Enqueue(FJob&& Job)
{
	bool bLaunch = false;
	const int64 Bytes = (int64)Job.RawBytes.Num() + Job.MaskBytes.Num();
	const double Deadline = FPlatformTime::Seconds() + 10.0;
	for (;;)
	{
		FScopeLock Lock(&QueueCS);
		if (Pending.GetValue() >= 8 || PendingBytes + Bytes > 256ll * 1024 * 1024)
		{
			// Encoding has no game-thread dependency. Apply backpressure rather than
			// dropping valid frames whenever the encoder briefly falls behind.
			if (Bytes <= 256ll * 1024 * 1024 && FPlatformTime::Seconds() < Deadline)
			{
				Lock.Unlock();
				FPlatformProcess::Sleep(0.002f);
				continue;
			}
			if (Job.bGrayMask) { MasksDropped.Increment(); } else { Dropped.Increment(); }
			UE_LOG(LogAnomalyCapture, Error, TEXT("Capture: writer capacity exceeded for session_index=%d; session is incomplete."), Job.SessionIndex);
			return false;
		}
		PendingBytes += Bytes;
		Pending.Increment();
		Queue.Add(MoveTemp(Job));
		bLaunch = !bWorkerRunning;
		bWorkerRunning = true;
		break;
	}
	if (bLaunch)
	{
		TSharedRef<FAnomalyAsyncWriter, ESPMode::ThreadSafe> Self = AsShared();
		Async(EAsyncExecution::ThreadPool, [Self]() { Self->Work(); });
	}
	return true;
}

void FAnomalyAsyncWriter::Work()
{
	for (;;)
	{
		FJob Job;
		{
			FScopeLock Lock(&QueueCS);
			if (Queue.IsEmpty()) { bWorkerRunning = false; return; }
			Job = MoveTemp(Queue[0]);
			Queue.RemoveAt(0);
		}
		const int64 Bytes = (int64)Job.RawBytes.Num() + Job.MaskBytes.Num();
		Run(Job);
		{
			FScopeLock Lock(&QueueCS);
			PendingBytes -= Bytes;
			Pending.Decrement();
		}
	}
}

TSet<int32> FAnomalyAsyncWriter::GetCommittedFrames() const
{
	FScopeLock Lock(&QueueCS);
	return CommittedFrames;
}

void FAnomalyAsyncWriter::Run(FJob& Job)
{
	if (Job.bGrayMask)
	{
		TArray<uint8> Png;
		const bool bEncoded = AnomalyPreview::EncodeGray8Png(Job.RawBytes, Job.Width, Job.Height, Png);
		const FString FullPath = FPaths::Combine(Job.OutputDir, Job.ImageRelPath);
		if (bEncoded)
		{
			IFileManager::Get().MakeDirectory(*FPaths::GetPath(FullPath), true);
		}
		if (bEncoded && FFileHelper::SaveArrayToFile(Png, *FullPath))
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
		return;
	}

	// A positive mask reference is committed only after its PNG has been saved.
	if (!Job.MaskRelPath.IsEmpty())
	{
		TArray<uint8> Png;
		const FString Path = FPaths::Combine(Job.OutputDir, Job.MaskRelPath);
		IFileManager::Get().MakeDirectory(*FPaths::GetPath(Path), true);
		if (!AnomalyPreview::EncodeGray8Png(Job.MaskBytes, Job.Width, Job.Height, Png)
			|| !FFileHelper::SaveArrayToFile(Png, *Path))
		{
			MasksDropped.Increment();
			Dropped.Increment();
			return;
		}
	}

	bool bResampled = false;
	const bool bOk = AnomalyLabel::EncodeAndWriteFrame(Job.OutputDir, Job.OutFormat, Job.RawBytes,
		Job.SrcFormat, Job.BytesPerPixel, Job.Width, Job.Height, Job.OutWidth, Job.OutHeight,
		Job.ImageRelPath, Job.Record, JsonlCS, Job.bWriteLabels, bResampled);

	if (bOk)
	{
		if (!Job.MaskRelPath.IsEmpty()) { MasksWritten.Increment(); }
		{ FScopeLock Lock(&QueueCS); CommittedFrames.Add(Job.SessionIndex); }
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
	}
	else
	{
		Dropped.Increment();
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
	while (Pending.GetValue() > 0)
	{
		if (TimeoutSeconds >= 0.0 && FPlatformTime::Seconds() - Start > TimeoutSeconds)
		{
			UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture(async): writer flush timed out with %d job(s) still pending."),
				Pending.GetValue());
			return false;
		}
		FPlatformProcess::Sleep(0.002f);
	}
	return true;
}

void FAnomalyAsyncWriter::ResetCounters()
{
	check(Pending.GetValue() == 0);
	{ FScopeLock Lock(&QueueCS); CommittedFrames.Reset(); }
	FramesWritten.Reset();
	PositiveWritten.Reset();
	Dropped.Reset();
	ResamplesPerformed.Reset();
	DimMismatches.Reset();
	MasksWritten.Reset();
	MasksDropped.Reset();
	FScopeLock Lock(&DimCS);
	FirstWrittenW = 0;
	FirstWrittenH = 0;
}

#endif
