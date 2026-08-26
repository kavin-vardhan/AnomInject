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

void FAnomalySveCapturer::ArmWanted(uint64 RequestId)
{
	int32 DepthAfter = 0;
	int32 TraceIndex = 0;
	{
		FScopeLock Lock(&StateCS);
		PendingWanted.Add(RequestId);
		DepthAfter = PendingWanted.Num();
		++Handshake.ArmsIssued;
		Handshake.MaxPendingDepth = FMath::Max(Handshake.MaxPendingDepth, DepthAfter);
		if (Handshake.TracedArms < HandshakeTraceLimit)
		{
			TraceIndex = ++Handshake.TracedArms;
		}
	}
	if (TraceIndex > 0)
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(sve): SVE-WANT-TRACE arm %d/%d requestId=%llu gameFrame=%llu pendingAfter=%d"),
			TraceIndex, HandshakeTraceLimit, RequestId, (uint64)GFrameCounter, DepthAfter);
	}
}

bool FAnomalySveCapturer::ConsumeWantedForPublish(uint32 FamilyFrameNumber, uint64& OutRequestId)
{
	OutRequestId = 0;
	bool bWanted = false;
	int32 DepthBefore = 0;
	int32 TraceIndex = 0;
	{
		FScopeLock Lock(&StateCS);
		DepthBefore = PendingWanted.Num();
		if (DepthBefore > 0)
		{
			OutRequestId = PendingWanted[0];
			PendingWanted.RemoveAt(0);
			bWanted = true;
			++Handshake.Matches;
		}
		if (Handshake.TracedPublishes < HandshakeTraceLimit)
		{
			TraceIndex = ++Handshake.TracedPublishes;
		}
	}
	if (TraceIndex > 0)
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(sve): SVE-WANT-TRACE publish %d/%d familyFrame=%u wanted=%d requestId=%llu pendingBefore=%d"),
			TraceIndex, HandshakeTraceLimit, FamilyFrameNumber, bWanted ? 1 : 0, OutRequestId, DepthBefore);
	}
	return bWanted;
}

void FAnomalySveCapturer::NoteIneligibleFamily()
{
	FScopeLock Lock(&StateCS);
	++Handshake.FamiliesIneligible;
}

int32 FAnomalySveCapturer::NumPendingApprox() const
{
	FScopeLock Lock(&StateCS);
	return PendingWanted.Num();
}

void FAnomalySveCapturer::Reset()
{
	{
		FScopeLock Lock(&StateCS);
		PendingWanted.Reset();
		Handshake = FAnomalySveHandshakeStats();
	}
	Submits.Reset();
	{
		FScopeLock Lock(&CompletedCS);
		Completed.Reset();
	}
	{
		FScopeLock Lock(&LatencyCS);
		Latency = FAnomalyReadbackLatencyStats();
	}
	{
		FScopeLock Lock(&LayoutCS);
		Layout = FAnomalyReadbackLayout();
	}
}

FAnomalyReadbackLayout FAnomalySveCapturer::GetReadbackLayout() const
{
	FScopeLock Lock(&LayoutCS);
	return Layout;
}

FAnomalySveHandshakeStats FAnomalySveCapturer::GetHandshakeStats() const
{
	FAnomalySveHandshakeStats Out;
	{
		FScopeLock Lock(&StateCS);
		Out = Handshake;
		Out.PendingNow = PendingWanted.Num();
	}
	Out.SubmitsIssued = Submits.GetValue();
	return Out;
}

FAnomalyReadbackLatencyStats FAnomalySveCapturer::GetLatencyStats() const
{
	FScopeLock Lock(&LatencyCS);
	return Latency;
}

void FAnomalySveCapturer::SubmitInFlight_RenderThread(uint64 RequestId, const FIntRect& Rect,
	const FIntPoint& SourceExtent, EPixelFormat Format, TUniquePtr<FRHIGPUTextureReadback>&& Readback)
{
	FInFlight Item;
	Item.RequestId = RequestId;
	Item.Readback = MoveTemp(Readback);
	Item.Rect = Rect;
	Item.SourceExtent = SourceExtent;
	Item.Format = Format;
	Item.SubmitRtFrame = GFrameNumberRenderThread;
	InFlight.Add(MoveTemp(Item));

	Submits.Increment();

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
			if (Item.Readback.IsValid())
			{
				FScopeLock LatLock(&LatencyCS);
				++Latency.NotReadyPolls;
			}
			continue;
		}

		{
			const int32 LatencyFrames = (int32)((uint32)GFrameNumberRenderThread - Item.SubmitRtFrame);
			FScopeLock LatLock(&LatencyCS);
			++Latency.Samples;
			Latency.SumFrames += LatencyFrames;
			Latency.MinFrames = FMath::Min(Latency.MinFrames, LatencyFrames);
			Latency.MaxFrames = FMath::Max(Latency.MaxFrames, LatencyFrames);
			++Latency.Histogram.FindOrAdd(LatencyFrames);
		}

		int32 RowPitchInPixels = 0;
		int32 BufferHeight = 0;
		void* Src = Item.Readback->Lock(RowPitchInPixels, &BufferHeight);
		if (Src && RowPitchInPixels > 0)
		{
			const int32 W = Item.Rect.Width();
			const int32 H = Item.Rect.Height();
			const int32 BPP = GPixelFormats[Item.Format].BlockBytes;

			AnomalyReadback::NoteLayoutOnce(LayoutCS, Layout, TEXT("sve"), Item.SourceExtent, Item.Rect,
				RowPitchInPixels, BufferHeight, Item.Format);

			if (!AnomalyReadback::CheckDrainBounds(TEXT("sve"), Item.RequestId, Item.Rect, W, H,
				RowPitchInPixels, BufferHeight, GuardDrops))
			{
				Item.Readback->Unlock();
				InFlight.RemoveAt(i);
				continue;
			}

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
				const uint8* SrcRow = Base + (int64)y * RowPitchInPixels * BPP;
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
