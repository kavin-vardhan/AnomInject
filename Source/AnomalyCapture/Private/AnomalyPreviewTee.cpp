#include "AnomalyPreviewTee.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"
#include "AnomalyFrameCapturer.h"
#include "AnomalyLabelWriter.h"
#include "AnomalyPreviewCapture.h"
#include "Async/Async.h"
#include "HAL/PlatformTime.h"

namespace
{
	constexpr int32 GPreviewJpegQuality = 60;
}

FAnomalyPreviewTee::FAnomalyPreviewTee()
{
	Capturer = MakeShared<FAnomalyFrameCapturer, ESPMode::ThreadSafe>();
	Capturer->SetLayoutPathName(TEXT("preview-tee"));
	Capturer->RegisterBackbufferHook();
	Slot = MakeShared<FSlot, ESPMode::ThreadSafe>();
}

FAnomalyPreviewTee::~FAnomalyPreviewTee()
{
	if (Capturer.IsValid())
	{
		Capturer->UnregisterBackbufferHook();
		Capturer.Reset();
	}
	Slot.Reset();
}

bool FAnomalyPreviewTee::IsBusy() const
{
	if (Outstanding > 0)
	{
		return true;
	}
	return Slot.IsValid() && Slot->EncodesInFlight.GetValue() > 0;
}

void FAnomalyPreviewTee::Arm(SWindow* TargetWindow, const FIntRect& CaptureRect, uint32 Epoch)
{
	if (!Capturer.IsValid() || !TargetWindow)
	{
		return;
	}
	const uint64 Id = NextRequestId++;
	ArmEpochs.Add(Id, Epoch);
	Capturer->ArmForCapture(Id, TargetWindow, CaptureRect);
	++Outstanding;
	ArmWallSeconds = FPlatformTime::Seconds();
}

void FAnomalyPreviewTee::Pump(bool bSuppressed)
{
	if (!Capturer.IsValid() || Outstanding <= 0)
	{
		return;
	}

	Capturer->EnqueueDrain();

	FAnomalyCapturedFrame Frame;
	while (Capturer->PopCompleted(Frame))
	{
		if (!ArmEpochs.Contains(Frame.RequestId)) { continue; }
		Outstanding = FMath::Max(0, Outstanding - 1);

		uint32 Epoch = 0;
		if (const uint32* Found = ArmEpochs.Find(Frame.RequestId))
		{
			Epoch = *Found;
			ArmEpochs.Remove(Frame.RequestId);
		}

		if (bSuppressed)
		{
			continue;
		}

		if (!bLoggedFormat)
		{
			bLoggedFormat = true;
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Preview(tee): first backbuffer frame (fmt=%d, bpp=%d, rect=%dx%d) — JPEG encode off the render thread."),
				(int32)Frame.Format, Frame.BytesPerPixel, Frame.Width, Frame.Height);
		}

		TSharedPtr<FSlot, ESPMode::ThreadSafe> SlotRef = Slot;
		if (!SlotRef.IsValid())
		{
			continue;
		}

		SlotRef->EncodesInFlight.Increment();
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask,
			[SlotRef, Raw = MoveTemp(Frame.RawBytes), Fmt = Frame.Format, Bpp = Frame.BytesPerPixel,
			 W = Frame.Width, H = Frame.Height, Epoch]()
			{
				TArray<FColor> Pixels;
				AnomalyLabel::ConvertTightToBGRA(Fmt, Bpp, Raw, W, H, Pixels);

				TArray<uint8> Jpeg;
				if (AnomalyPreview::EncodePixels(AnomalyPreview::EImageFormat::JPEG, Pixels, W, H, Jpeg, GPreviewJpegQuality))
				{
					FScopeLock Lock(&SlotRef->CS);
					SlotRef->Jpeg = MoveTemp(Jpeg);
					SlotRef->W = W;
					SlotRef->H = H;
					SlotRef->Epoch = Epoch;
					SlotRef->bHasFrame = true;
				}
				SlotRef->EncodesInFlight.Decrement();
			});
	}
	if (Outstanding > 0 && FPlatformTime::Seconds() - ArmWallSeconds > 2.0)
	{
		Capturer->CancelPendingRequests();
		ArmEpochs.Reset();
		Outstanding = 0;
	}

}

bool FAnomalyPreviewTee::PollJpeg(TArray<uint8>& OutJpeg, int32& OutW, int32& OutH, uint32& OutEpoch)
{
	if (!Slot.IsValid())
	{
		return false;
	}
	FScopeLock Lock(&Slot->CS);
	if (!Slot->bHasFrame)
	{
		return false;
	}
	OutJpeg = MoveTemp(Slot->Jpeg);
	OutW = Slot->W;
	OutH = Slot->H;
	OutEpoch = Slot->Epoch;
	Slot->Jpeg.Reset();
	Slot->bHasFrame = false;
	return true;
}

void FAnomalyPreviewTee::DiscardReady()
{
	if (!Slot.IsValid())
	{
		return;
	}
	FScopeLock Lock(&Slot->CS);
	Slot->Jpeg.Reset();
	Slot->bHasFrame = false;
}

#endif
