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

void FAnomalyAsyncWriter::Enqueue(FJob&& Job)
{
	Pending.Increment();

	TSharedRef<FAnomalyAsyncWriter, ESPMode::ThreadSafe> Self = AsShared();
	Async(EAsyncExecution::ThreadPool, [Self, MovedJob = MoveTemp(Job)]() mutable
	{
		Self->Run(MovedJob);
		Self->Pending.Decrement();
	});
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

	bool bResampled = false;
	const bool bOk = AnomalyLabel::EncodeAndWriteFrame(Job.OutputDir, Job.OutFormat, Job.RawBytes,
		Job.SrcFormat, Job.BytesPerPixel, Job.Width, Job.Height, Job.OutWidth, Job.OutHeight,
		Job.ImageRelPath, Job.Record, JsonlCS, Job.bWriteLabels, bResampled);

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

void FAnomalyAsyncWriter::FlushPending(double TimeoutSeconds)
{
	const double Start = FPlatformTime::Seconds();
	while (Pending.GetValue() > 0)
	{
		if (FPlatformTime::Seconds() - Start > TimeoutSeconds)
		{
			UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture(async): writer flush timed out with %d job(s) still pending."),
				Pending.GetValue());
			break;
		}
		FPlatformProcess::Sleep(0.002f);
	}
}

void FAnomalyAsyncWriter::ResetCounters()
{
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
