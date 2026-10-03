#include "AnomalySveCapturer.h"
#include "Engine/Texture.h"
#include "PixelFormat.h"
#include "RHIGPUReadback.h"
#if __has_include("Templates/SharedPointerFwd.h")
#include "Templates/SharedPointerFwd.h"
#endif
#include "Templates/SharedPointerInternals.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"

#include "CoreGlobals.h"
#include "Engine/Texture2D.h"
#include "Misc/ScopeLock.h"
#include "RHICommandList.h"
#include "RenderingThread.h"
#include "Rendering/StreamableTextureResource.h"
#include "TextureResource.h"

void FAnomalySveCapturer::SetActive(bool bInActive)
{
	ActiveFlag.Set(bInActive ? 1 : 0);
}

bool FAnomalySveCapturer::IsActive() const
{
	return ActiveFlag.GetValue() != 0;
}

bool FAnomalySveCapturer::ExtendRenderWatch(uint64 RequestId, const TArray<UTexture2D*>& Added)
{
	FScopeLock Lock(&StateCS);
	if (!PendingWanted.Contains(RequestId))
	{
		return false;
	}
	RenderWatchByRequest.FindOrAdd(RequestId).Append(Added);
	return true;
}

bool FAnomalySveCapturer::TakeRenderWatch_RenderThread(uint64 RequestId, TArray<UTexture2D*>& Out)
{
	FScopeLock Lock(&StateCS);
	TArray<UTexture2D*>* Found = RenderWatchByRequest.Find(RequestId);
	if (!Found)
	{
		return false;
	}
	Out = MoveTemp(*Found);
	RenderWatchByRequest.Remove(RequestId);
	return true;
}

void FAnomalySveCapturer::SampleRenderMips_RenderThread(const TArray<UTexture2D*>& Watch, TArray<FAnomalyRenderMipSample>& Out)
{
	Out.Reset();
	Out.Reserve(Watch.Num());
	for (UTexture2D* Tex : Watch)
	{
		FAnomalyRenderMipSample S;
		const FTextureResource* Res = Tex ? static_cast<const UTexture*>(Tex)->GetResource() : nullptr;
		if (!Tex)
		{
			S.Status = EAnomalyRenderMipStatus::NoTexture;
		}
		else if (!Res)
		{
			S.Status = EAnomalyRenderMipStatus::NoResource;
		}
		else if (Res->IsTextureRHIPartiallyResident())
		{
			S.Status = EAnomalyRenderMipStatus::PartiallyResident;
		}
		else if (const FStreamableTextureResource* SR = Res->GetStreamableTextureResource())
		{
			if (!SR->IsInitialized())
			{
				S.Status = EAnomalyRenderMipStatus::Uninitialized;
			}
			else
			{
				const FStreamableRenderResourceState State = SR->GetState();
				if (State.IsValid())
				{
					S.Status = EAnomalyRenderMipStatus::Ok;
					S.Resident = (int32)State.NumResidentLODs;
					S.FirstMip = SR->GetCurrentFirstMip();
				}
				else
				{
					S.Status = EAnomalyRenderMipStatus::Uninitialized;
				}
			}
		}
		else
		{
			S.Status = EAnomalyRenderMipStatus::NotStreamable;
		}
		S.ResourceId = (uint64)(UPTRINT)Res;
		Out.Add(S);
	}
}

void FAnomalySveCapturer::ArmWanted(uint64 RequestId, FAnomalyChangeIssuePtr ChangeIssue, const TArray<UTexture2D*>* RenderWatch)
{
	int32 DepthAfter = 0;
	int32 TraceIndex = 0;
	{
		FScopeLock Lock(&StateCS);
		PendingWanted.Add(RequestId);
		if (RenderWatch)
		{
			RenderWatchByRequest.Add(RequestId, *RenderWatch);
		}
		if (ChangeIssue.IsValid()) { PendingIssues.Add(RequestId, ChangeIssue); ChangeStage = ChangeIssue->Stage; LastIssuedIdentity = ChangeIssue; }
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

void FAnomalySveCapturer::CancelPendingOtherGeneration(const FAnomalyChangeIssuePtr& Current, TArray<uint64>& Cancelled)
{
	check(IsInGameThread());
	if (!Current.IsValid()) { return; }
	FScopeLock Lock(&StateCS);
	for (int32 I = PendingWanted.Num() - 1; I >= 0; --I)
	{
		const auto Old = PendingIssues.FindRef(PendingWanted[I]);
		if (Old.IsValid() && (Old->RunEpoch != Current->RunEpoch || Old->CutCounter != Current->CutCounter))
		{
			Cancelled.Add(PendingWanted[I]);
			PendingIssues.Remove(PendingWanted[I]);
			RenderWatchByRequest.Remove(PendingWanted[I]);
			PendingWanted.RemoveAt(I);
		}
	}
}

FAnomalyChangeIssuePtr FAnomalySveCapturer::PeekChangeIssue() const
{
	FScopeLock Lock(&StateCS);
	return PendingWanted.Num() ? PendingIssues.FindRef(PendingWanted[0]) : nullptr;
}

TSharedPtr<FAnomalyChangeStage, ESPMode::ThreadSafe> FAnomalySveCapturer::GetChangeStage() const
{
	FScopeLock Lock(&StateCS); return ChangeStage.Pin();
}

FAnomalyChangeIssuePtr FAnomalySveCapturer::GetOwnerIssue() const
{
	FScopeLock Lock(&StateCS); return LastIssuedIdentity;
}

bool FAnomalySveCapturer::ConsumeWantedForPublish(uint32 FamilyFrameNumber, uint64& OutRequestId, FAnomalyChangeIssuePtr& OutIssue)
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
			OutIssue = PendingIssues.FindRef(OutRequestId);
			PendingIssues.Remove(OutRequestId);
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
		PendingIssues.Reset();
		RenderWatchByRequest.Reset();
		ChangeStage.Reset();
		LastIssuedIdentity.Reset();
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
	const FIntPoint& SourceExtent, EPixelFormat Format, TUniquePtr<FRHIGPUTextureReadback>&& Readback,
	TUniquePtr<FRHIGPUTextureReadback>&& LegacyReadback, const FAnomalyChangeReceipt& ChangeSubmission,
	TArray<FAnomalyRenderMipSample>&& RenderMips, bool bRenderRecord,
	int32 TemporalAaMethod, bool bTemporalViewKnown, bool bTemporalUpscaler)
{
	const bool bDual = LegacyReadback.IsValid();

	FInFlight Item;
	Item.RequestId = RequestId;
	Item.Readback = MoveTemp(Readback);
	Item.LegacyReadback = MoveTemp(LegacyReadback);
	Item.Rect = Rect;
	Item.SourceExtent = SourceExtent;
	Item.Format = Format;
	Item.SubmitRtFrame = GFrameNumberRenderThread;
	Item.ChangeSubmission = ChangeSubmission;
	Item.RenderMips = MoveTemp(RenderMips);
	Item.bRenderRecord = bRenderRecord;
	Item.TemporalAaMethod = TemporalAaMethod;
	Item.bTemporalViewKnown = bTemporalViewKnown;
	Item.bTemporalUpscaler = bTemporalUpscaler;
	InFlight.Add(MoveTemp(Item));

	Submits.Increment();

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(sve): keyed frame id=%llu submitted (rtframe=%u, fmt=%d, rect=%dx%d, dualPath=%d)."),
		RequestId, GFrameNumberRenderThread, (int32)Format, Rect.Width(), Rect.Height(), bDual ? 1 : 0);
}

int32 FAnomalySveCapturer::GetDualPathComparisons() const
{
	return DualPathComparisons.GetValue();
}

int32 FAnomalySveCapturer::GetDualPathMismatches() const
{
	return DualPathMismatches.GetValue();
}

void FAnomalySveCapturer::CompareDualPath_RenderThread(FInFlight& Item, const FAnomalyCapturedFrame& OwnedFrame)
{
	int32 LegacyPitch = 0;
	int32 LegacyBufferHeight = 0;
	void* LegacySrc = Item.LegacyReadback->Lock(LegacyPitch, &LegacyBufferHeight);
	if (!LegacySrc || LegacyPitch <= 0)
	{
		if (LegacySrc)
		{
			Item.LegacyReadback->Unlock();
		}
		UE_LOG(LogAnomalyCapture, Error,
			TEXT("Capture(sve): DUAL-PATH COMPARE UNAVAILABLE id=%llu — the legacy readback did not map ")
			TEXT("(ptr=%d pitch=%d). NO VERDICT is offered for this frame; it is not a pass and not a ")
			TEXT("mismatch."),
			Item.RequestId, LegacySrc ? 1 : 0, LegacyPitch);
		return;
	}

	const int32 W = OwnedFrame.Width;
	const int32 H = OwnedFrame.Height;
	const int32 BPP = OwnedFrame.BytesPerPixel;
	const int64 RowBytes = (int64)W * BPP;

	const int32 NeededRows = Item.Rect.Min.Y + H;
	const bool bLegacyBoundsOk = (LegacyBufferHeight >= NeededRows) && (LegacyPitch >= Item.Rect.Min.X + W);
	if (!bLegacyBoundsOk)
	{
		Item.LegacyReadback->Unlock();
		UE_LOG(LogAnomalyCapture, Error,
			TEXT("Capture(sve): DUAL-PATH COMPARE UNAVAILABLE id=%llu — the LEGACY form would read outside ")
			TEXT("its own mapped buffer (rect.min=(%d,%d) W=%d H=%d neededRows=%d bufferHeight=%d ")
			TEXT("pitch=%d). That is the pre-m35 defect this milestone exists for, observed here rather ")
			TEXT("than crashed on. NO VERDICT: the owned-copy picture is still written normally."),
			Item.RequestId, Item.Rect.Min.X, Item.Rect.Min.Y, W, H, NeededRows, LegacyBufferHeight,
			LegacyPitch);
		return;
	}

	TArray<uint8> LegacyBytes;
	LegacyBytes.SetNumUninitialized((int64)W * H * BPP);
	const uint8* LegacyBase = static_cast<const uint8*>(LegacySrc);
	for (int32 y = 0; y < H; ++y)
	{
		const uint8* SrcRow = LegacyBase + ((int64)(Item.Rect.Min.Y + y) * LegacyPitch + Item.Rect.Min.X) * BPP;
		FMemory::Memcpy(LegacyBytes.GetData() + (int64)y * RowBytes, SrcRow, RowBytes);
	}
	Item.LegacyReadback->Unlock();

	if (AnomalyReadback::IsDualPathForcedMismatch() && LegacyBytes.Num() > 0)
	{
		LegacyBytes[LegacyBytes.Num() / 2] ^= 0xFF;
	}

	DualPathComparisons.Increment();

	int64 FirstDiff = -1;
	int64 DiffCount = 0;
	const int64 Total = FMath::Min((int64)LegacyBytes.Num(), (int64)OwnedFrame.RawBytes.Num());
	for (int64 b = 0; b < Total; ++b)
	{
		if (LegacyBytes[b] != OwnedFrame.RawBytes[b])
		{
			if (FirstDiff < 0)
			{
				FirstDiff = b;
			}
			++DiffCount;
		}
	}

	if (DiffCount == 0 && LegacyBytes.Num() == OwnedFrame.RawBytes.Num())
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(sve): DUAL-PATH COMPARE IDENTICAL id=%llu — %lld bytes, 0 differing (%dx%d, ")
			TEXT("bpp=%d, forcedMismatch=%d). The m35 owned-copy readback and the PRE-m35 whole-source ")
			TEXT("readback drained the SAME frame to the same bytes."),
			Item.RequestId, (long long)Total, W, H, BPP,
			AnomalyReadback::IsDualPathForcedMismatch() ? 1 : 0);
		return;
	}

	DualPathMismatches.Increment();

	const int64 DiffPixel = (FirstDiff >= 0 && BPP > 0) ? (FirstDiff / BPP) : -1;
	const int64 DiffRow = (DiffPixel >= 0 && W > 0) ? (DiffPixel / W) : -1;
	const int64 DiffCol = (DiffPixel >= 0 && W > 0) ? (DiffPixel % W) : -1;

	UE_LOG(LogAnomalyCapture, Error,
		TEXT("Capture(sve): DUAL-PATH COMPARE MISMATCH id=%llu — %lld of %lld bytes differ, firstDiff at ")
		TEXT("byte %lld (row %lld, col %lld), sizes owned=%d legacy=%d, forcedMismatch=%d. ")
		TEXT("⛔ THERE ARE TWO CAUSES AND THEY NEED OPPOSITE RESPONSES — CHECK (ii) FIRST. ")
		TEXT("(ii) ADJACENCY BROKEN: if any pass was added between the two AddEnqueueCopyPass calls in ")
		TEXT("AfterPass_RenderThread, or anything now WRITES scene colour between them, the two readbacks ")
		TEXT("no longer observe the same source contents and THIS COMPARISON IS MEANINGLESS RATHER THAN ")
		TEXT("FAILING - reading it as a defect would send someone hunting a fault in working code. ")
		TEXT("(i) Only once adjacency is confirmed intact does this mean the OWNED COPY IS NOT ")
		TEXT("REPRODUCING THE SUB-RECT, which is a real m35 defect: report the row/col above. ")
		TEXT("A NON-ZERO forcedMismatch means IAI.Bench.DualPathReadback 2 provoked this deliberately - ")
		TEXT("that is the comparator's proof-by-breaking and NOT a defect."),
		Item.RequestId, (long long)DiffCount, (long long)Total, (long long)FirstDiff,
		(long long)DiffRow, (long long)DiffCol, OwnedFrame.RawBytes.Num(), LegacyBytes.Num(),
		AnomalyReadback::IsDualPathForcedMismatch() ? 1 : 0);
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
	for (const auto& Held : InFlight)
	{
		const auto Issued = Held.ChangeSubmission.Issue;
		auto Stage = Issued.IsValid() ? Issued->Stage.Pin() : nullptr;
		if (Stage.IsValid() && Stage->Gate(1, Issued->SessionIndex))
		{
			bool bSuccessorReady = false;
			for (const auto& Other : InFlight)
			{
				if (Other.ChangeSubmission.Issue.IsValid() && Other.ChangeSubmission.Issue->SessionIndex == 9
					&& Other.Readback.IsValid() && Other.Readback->IsReady()) { bSuccessorReady = true; }
			}
			if ((!Held.Readback.IsValid() || !Held.Readback->IsReady() || !bSuccessorReady)
				&& FAnomalyChangeStage::NowMs() - Issued->SubmitMs < 1000) { return; }
		}
	}
	int32 ReadyThisDrain = 0;
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

		if (Item.LegacyReadback.IsValid() && !Item.LegacyReadback->IsReady())
		{
			continue;
		}
		const auto Issue = Item.ChangeSubmission.Issue;
		auto Change = Issue.IsValid() ? Issue->Stage.Pin() : nullptr;
		if (Change.IsValid())
		{
			const int32 SI = Issue->SessionIndex;
			const int64 AgeMs = FAnomalyChangeStage::NowMs() - Issue->SubmitMs;
			if ((Change->Gate(7, SI) && AgeMs < 2500)
				|| (Change->Gate(11, SI) && AgeMs < 6500)) { continue; }
			if (!Change->AcceptGeneration(Issue, TEXT("colour_drain"))) { Change.Reset(); }
		}
		if (Change.IsValid())
		{
			const int32 SI = Issue->SessionIndex;
			if (Change->Gate(4, SI))
			{
				Change->Fail(Issue, EAnomalyChangeReason::CurrentUndelivered, TEXT("colour"));
				InFlight.RemoveAt(i); continue;
			}
			if (++ReadyThisDrain >= 2) { Change->Diagnostic(TEXT("colour_multi_ready_drain")); }
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
				if (Change.IsValid()) { Change->Fail(Issue, EAnomalyChangeReason::CurrentUndelivered, TEXT("colour")); }
				InFlight.RemoveAt(i);
				continue;
			}

			FAnomalyCapturedFrame Frame;
			Frame.RequestId = Item.RequestId;
			Frame.Width = W;
			Frame.Height = H;
			Frame.Format = Item.Format;
			Frame.BytesPerPixel = BPP;
			Frame.RenderMips = MoveTemp(Item.RenderMips);
			Frame.bRenderRecord = Item.bRenderRecord;
			Frame.TemporalAaMethod = Item.TemporalAaMethod;
			Frame.bTemporalViewKnown = Item.bTemporalViewKnown;
			Frame.bTemporalUpscaler = Item.bTemporalUpscaler;
			Frame.RawBytes.SetNumUninitialized((int64)W * H * BPP);

			const uint8* Base = static_cast<const uint8*>(Src);
			for (int32 y = 0; y < H; ++y)
			{
				const uint8* SrcRow = Base + (int64)y * RowPitchInPixels * BPP;
				FMemory::Memcpy(Frame.RawBytes.GetData() + (int64)y * W * BPP, SrcRow, (int64)W * BPP);
			}

			Item.Readback->Unlock();
			if (Change.IsValid())
			{
				Item.ChangeSubmission.DrainMs = FAnomalyChangeStage::NowMs();
				if (Change->Gate(9, Issue->SessionIndex)) { ++Item.ChangeSubmission.Rect.Min.X; }
				Frame.ChangeReceipt = MakeShared<const FAnomalyChangeReceipt, ESPMode::ThreadSafe>(Item.ChangeSubmission);
			}

			if (Item.LegacyReadback.IsValid())
			{
				CompareDualPath_RenderThread(Item, Frame);
			}

			{
				FScopeLock Lock(&CompletedCS);
				Completed.Add(MoveTemp(Frame));
			}
		}
		else
		{
			if (Src) { Item.Readback->Unlock(); }
			if (Change.IsValid()) { Change->Fail(Issue, EAnomalyChangeReason::CurrentUndelivered, TEXT("colour")); }
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
