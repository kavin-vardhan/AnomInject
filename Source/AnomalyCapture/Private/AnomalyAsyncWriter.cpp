#include "AnomalyAsyncWriter.h"

#if ANOMALY_CAPTURE

#include "AnomalyLabelWriter.h"
#include "AnomalyCaptureLog.h"

#include "Async/Async.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformTime.h"

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
	const bool bOk = AnomalyLabel::EncodeAndWriteFrame(Job.OutputDir, Job.OutFormat, Job.RawBytes,
		Job.SrcFormat, Job.BytesPerPixel, Job.Width, Job.Height, Job.ImageRelPath, Job.Record, JsonlCS);

	if (bOk)
	{
		FramesWritten.Increment();
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
}

#endif
