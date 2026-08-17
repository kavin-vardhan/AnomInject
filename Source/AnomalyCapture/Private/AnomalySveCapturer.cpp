#include "AnomalySveCapturer.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"

#include "CoreGlobals.h"
#include "Misc/ScopeLock.h"
#include "RHICommandList.h"
#include "RenderingThread.h"

void FAnomalySveCapturer::SetActive(bool bInActive)
{
	ActiveFlag.Set(bInActive ? 1 : 0);
}

bool FAnomalySveCapturer::IsActive() const
{
	return ActiveFlag.GetValue() != 0;
}

void FAnomalySveCapturer::MarkWanted(uint64 GameFrameCounter)
{
	FScopeLock Lock(&StateCS);
	WantedFrames.Add(GameFrameCounter);
}

bool FAnomalySveCapturer::IsWanted(uint64 GameFrameCounter) const
{
	FScopeLock Lock(&StateCS);
	return WantedFrames.Contains(GameFrameCounter);
}

int32 FAnomalySveCapturer::NumPendingApprox() const
{
	FScopeLock Lock(&StateCS);
	return WantedFrames.Num();
}

void FAnomalySveCapturer::Reset()
{
	{
		FScopeLock Lock(&StateCS);
		WantedFrames.Reset();
	}
	{
		FScopeLock Lock(&CompletedCS);
		Completed.Reset();
	}
}

void FAnomalySveCapturer::SubmitInFlight_RenderThread(uint64 RequestId, const FIntRect& Rect, EPixelFormat Format,
	TUniquePtr<FRHIGPUTextureReadback>&& Readback)
{
	FInFlight Item;
	Item.RequestId = RequestId;
	Item.Readback = MoveTemp(Readback);
	Item.Rect = Rect;
	Item.Format = Format;
	InFlight.Add(MoveTemp(Item));

	{
		FScopeLock Lock(&StateCS);
		WantedFrames.Remove(RequestId);
	}

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(sve): keyed frame id=%llu submitted (rtframe=%u, fmt=%d, rect=%dx%d)."),
		RequestId, GFrameNumberRenderThread, (int32)Format, Rect.Width(), Rect.Height());
}

void FAnomalySveCapturer::EnqueueDrain()
{
	TWeakPtr<FAnomalySveCapturer, ESPMode::ThreadSafe> WeakSelf = AsShared();

	ENQUEUE_RENDER_COMMAND(AnomalySveCaptureDrain)(
		[WeakSelf](FRHICommandListImmediate& RHICmdList)
		{
			if (TSharedPtr<FAnomalySveCapturer, ESPMode::ThreadSafe> Self = WeakSelf.Pin())
			{
				Self->Drain_RenderThread();
			}
		});
}

void FAnomalySveCapturer::Drain_RenderThread()
{
	for (int32 i = InFlight.Num() - 1; i >= 0; --i)
	{
		FInFlight& Item = InFlight[i];
		if (!Item.Readback.IsValid() || !Item.Readback->IsReady())
		{
			continue;
		}

		int32 RowPitchInPixels = 0;
		int32 BufferHeight = 0;
		void* Src = Item.Readback->Lock(RowPitchInPixels, &BufferHeight);
		if (Src && RowPitchInPixels > 0)
		{
			const int32 W = Item.Rect.Width();
			const int32 H = Item.Rect.Height();
			const int32 BPP = GPixelFormats[Item.Format].BlockBytes;

			FAnomalyCapturedFrame Frame;
			Frame.RequestId = Item.RequestId;
			Frame.Width = W;
			Frame.Height = H;
			Frame.Format = Item.Format;
			Frame.BytesPerPixel = BPP;
			Frame.RawBytes.SetNumUninitialized((int64)W * H * BPP);

			const uint8* Base = static_cast<const uint8*>(Src);
			for (int32 y = 0; y < H; ++y)
			{
				const uint8* SrcRow = Base + ((int64)(Item.Rect.Min.Y + y) * RowPitchInPixels + Item.Rect.Min.X) * BPP;
				FMemory::Memcpy(Frame.RawBytes.GetData() + (int64)y * W * BPP, SrcRow, (int64)W * BPP);
			}

			Item.Readback->Unlock();

			{
				FScopeLock Lock(&CompletedCS);
				Completed.Add(MoveTemp(Frame));
			}
		}
		else if (Src)
		{
			Item.Readback->Unlock();
		}

		InFlight.RemoveAt(i);
	}
}

bool FAnomalySveCapturer::PopCompleted(FAnomalyCapturedFrame& Out)
{
	FScopeLock Lock(&CompletedCS);
	if (Completed.Num() == 0)
	{
		return false;
	}
	Out = MoveTemp(Completed[0]);
	Completed.RemoveAt(0);
	return true;
}

#endif
