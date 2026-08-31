#include "AnomalyMaskMeasure.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"
#include "AnomalyMaskSceneViewExtension.h"
#include "AnomalyStencilTag.h"

#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"

namespace
{
	int32 ReadCustomDepthCVar()
	{
		if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth")))
		{
			return CVar->GetInt();
		}
		return -1;
	}
}

void FAnomalyMaskMeasure::BeginRun(FAnomalyStencilTagLedger* InLedger)
{
	Records.Reset();
	ArmedRequestToRecord.Reset();
	PollutedRequests.Reset();
	ProbeRequests.Reset();
	EndFrameSample.Reset();
	ArmedThisFrame.Reset();
	ExtraAssignedTags.Reset();
	Ledger = InLedger;
	NextTagOffset = 0;

	const int32 Before = ReadCustomDepthCVar();
	AnomalyStencilTag::EnableCustomStencil();
	const int32 After = ReadCustomDepthCVar();

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(mask): M23 CVAR beginRun rCustomDepth before=%d after=%d (3 = EnabledWithStencil; ")
		TEXT("1 = Enabled WITHOUT stencil writes, which is the engine default)"),
		Before, After);

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(mask): M36 TAG POOL assignable %d..%d (255 is NEVER mintable by any allocator - it stays ")
		TEXT("the residual StencilDummy detector), hostReserved=%d, assignable=%d."),
		AnomalyStencilTag::ReservedStencilBase, AnomalyStencilTag::AssignableStencilMax,
		Ledger ? Ledger->HostReserved.Num() : 0,
		Ledger ? Ledger->NumAssignable() : (AnomalyStencilTag::AssignableStencilMax - AnomalyStencilTag::ReservedStencilBase + 1));
}

void FAnomalyMaskMeasure::EndRun()
{
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(mask): M23 CVAR finishRun rCustomDepth before restore=%d"), ReadCustomDepthCVar());

	UntagAll();
	AnomalyStencilTag::DisableCustomStencil();
	ArmedRequestToRecord.Reset();
	PollutedRequests.Reset();
	ProbeRequests.Reset();
	EndFrameSample.Reset();
	ArmedThisFrame.Reset();
	ExtraAssignedTags.Reset();
	if (Ledger)
	{
		Ledger->EventClaimed.Reset();
	}
	Ledger = nullptr;
	NextTagOffset = 0;
}

void FAnomalyMaskMeasure::UntagAll()
{
	AnomalyStencilTag::RestoreAll();
}

int32 FAnomalyMaskMeasure::AllocateTag()
{
	const int32 Span = AnomalyStencilTag::AssignableStencilMax - AnomalyStencilTag::ReservedStencilBase + 1;
	int32 Skipped = 0;
	for (int32 i = 0; i < Span; ++i)
	{
		const int32 Tag = AnomalyStencilTag::ReservedStencilBase + ((NextTagOffset + i) % Span);
		if (!Ledger || Ledger->IsFree((uint8)Tag))
		{
			if (Ledger)
			{
				Ledger->EventClaimed.Add((uint8)Tag);
			}
			NextTagOffset = (NextTagOffset + i + 1) % Span;
			if (Skipped > 0)
			{
				UE_LOG(LogAnomalyCapture, Log,
					TEXT("Capture(mask): M36 event tag allocator skipped %d reserved/claimed value(s) and assigned %d."),
					Skipped, Tag);
			}
			check(Tag >= AnomalyStencilTag::ReservedStencilBase && Tag <= AnomalyStencilTag::AssignableStencilMax);
			return Tag;
		}
		++Skipped;
	}

	const int32 Fallback = AnomalyStencilTag::ReservedStencilBase + (NextTagOffset % Span);
	++NextTagOffset;
	UE_LOG(LogAnomalyCapture, Error,
		TEXT("Capture(mask): M36 TAG-POOL EXHAUSTED - every assignable stencil value %d..%d is reserved or ")
		TEXT("claimed. Re-assigning %d; the collision detectors (verify read-back + unassigned-tag) are the ")
		TEXT("backstop and affected frames discard toward NOT_MEASURED, which ADMITS."),
		AnomalyStencilTag::ReservedStencilBase, AnomalyStencilTag::AssignableStencilMax, Fallback);
	check(Fallback >= AnomalyStencilTag::ReservedStencilBase && Fallback <= AnomalyStencilTag::AssignableStencilMax);
	return Fallback;
}

FAnomalyMaskRecord* FAnomalyMaskMeasure::FindRecord(FName Id, const FString& Target, uint64 StartFrame)
{
	return Records.FindByPredicate([&](const FAnomalyMaskRecord& R)
	{
		return R.Id == Id && R.StartFrame == StartFrame && R.Target == Target;
	});
}

FAnomalyMaskRecord* FAnomalyMaskMeasure::FindOrAddRecord(FName Id, const FString& Target, uint64 StartFrame, AActor* TargetActor)
{
	if (FAnomalyMaskRecord* Existing = FindRecord(Id, Target, StartFrame))
	{
		if (!Existing->TargetActor.IsValid() && TargetActor)
		{
			Existing->TargetActor = TargetActor;
		}
		return Existing;
	}

	FAnomalyMaskRecord New;
	New.Id = Id;
	New.Target = Target;
	New.StartFrame = StartFrame;
	New.TargetActor = TargetActor;
	New.Tag = (uint8)AllocateTag();
	const int32 Index = Records.Add(MoveTemp(New));
	return &Records[Index];
}

TSet<uint8> FAnomalyMaskMeasure::BuildBaseTagSet() const
{
	TSet<uint8> Out;
	for (const FAnomalyMaskRecord& R : Records)
	{
		Out.Add(R.Tag);
	}
	return Out;
}

TSet<uint8> FAnomalyMaskMeasure::BuildAssignedTagSet() const
{
	TSet<uint8> Out = BuildBaseTagSet();
	Out.Append(ExtraAssignedTags);
	return Out;
}

int32 FAnomalyMaskMeasure::NumUnmeasured() const
{
	int32 N = 0;
	for (const FAnomalyMaskRecord& R : Records)
	{
		if (R.State == EAnomalyMaskState::NotMeasured)
		{
			++N;
		}
	}
	return N;
}

int32 FAnomalyMaskMeasure::TotalProbeArms() const
{
	int32 N = 0;
	for (const FAnomalyMaskRecord& R : Records)
	{
		N += R.ProbeArms;
	}
	return N;
}

int32 FAnomalyMaskMeasure::TotalResidualDiscards() const
{
	int32 N = 0;
	for (const FAnomalyMaskRecord& R : Records)
	{
		N += R.FramesResidualDiscarded;
	}
	return N;
}

int32 FAnomalyMaskMeasure::TotalNoPassDiscards() const
{
	int32 N = 0;
	for (const FAnomalyMaskRecord& R : Records)
	{
		N += R.FramesNoPass;
	}
	return N;
}

bool FAnomalyMaskMeasure::ArmIfMeasurable(FAnomalyMaskSceneViewExtension* Sve, uint64 RequestId)
{
	if (!Sve)
	{
		return false;
	}

	for (int32 i = 0; i < Records.Num(); ++i)
	{
		FAnomalyMaskRecord& R = Records[i];
		if (R.ArmsIssued >= MaxArmsPerEvent)
		{
			continue;
		}

		AActor* Actor = R.TargetActor.Get();
		if (!Actor)
		{
			continue;
		}
		if (Actor->IsHidden())
		{
			++R.SkippedHidden;
			continue;
		}

		const int32 Tagged = AnomalyStencilTag::TagActor(Actor, (int32)R.Tag);
		if (Tagged <= 0)
		{
			R.bTagFailed = true;
			continue;
		}

		Sve->SetAssignedTags(BuildAssignedTagSet());
		Sve->ArmMask(RequestId);
		++R.ArmsIssued;
		ArmedRequestToRecord.Add(RequestId, i);
		ArmedThisFrame.Add(RequestId);

		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(mask): M23 ARM id=%llu target=%s tag=%d taggedComponents=%d ")
			TEXT("rCustomDepth_gameThread=%d armIndex=%d"),
			RequestId, *R.Target, (int32)R.Tag, Tagged, ReadCustomDepthCVar(), R.ArmsIssued);

		return true;
	}

	return false;
}

bool FAnomalyMaskMeasure::ArmProbeOnHidden(FAnomalyMaskSceneViewExtension* Sve, uint64 RequestId)
{
	if (!Sve || ArmedRequestToRecord.Contains(RequestId))
	{
		return false;
	}

	for (int32 i = 0; i < Records.Num(); ++i)
	{
		FAnomalyMaskRecord& R = Records[i];
		if (R.ArmsIssued >= MaxArmsPerEvent)
		{
			continue;
		}

		AActor* Actor = R.TargetActor.Get();
		if (!Actor || !Actor->IsHidden())
		{
			continue;
		}

		const int32 Tagged = AnomalyStencilTag::TagActor(Actor, (int32)R.Tag);
		if (Tagged <= 0)
		{
			continue;
		}

		Sve->SetAssignedTags(BuildAssignedTagSet());
		Sve->ArmMask(RequestId);
		++R.ArmsIssued;
		++R.ProbeArms;
		ArmedRequestToRecord.Add(RequestId, i);
		ArmedThisFrame.Add(RequestId);
		ProbeRequests.Add(RequestId);

		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Capture(mask): M26S1 PROBE ARM id=%llu target=%s tag=%d taggedComponents=%d - ")
			TEXT("DELIBERATELY armed on a KNOWN-HIDDEN tick (F-6 item 5 liveness demonstration, gate use ")
			TEXT("only; LOCK-1 bypassed for THIS ONE ARM). Expected: the 255/StencilDummy detector FIRES, ")
			TEXT("the end-of-frame confirmation reads HIDDEN, and the frame is bucketed PROBE - it can ")
			TEXT("never contribute to a measurement."),
			RequestId, *R.Target, (int32)R.Tag, Tagged);

		return true;
	}

	return false;
}

void FAnomalyMaskMeasure::VerifyPendingTags()
{
	for (int32 RecordIndex = 0; RecordIndex < Records.Num(); ++RecordIndex)
	{
		FAnomalyMaskRecord& R = Records[RecordIndex];
		if (R.ArmsIssued <= 0 || R.ArmsIssued == R.ArmsResolved)
		{
			continue;
		}
		AActor* Actor = R.TargetActor.Get();
		if (!Actor || Actor->IsHidden())
		{
			continue;
		}
		FString Detail;
		if (!AnomalyStencilTag::VerifyActorStillTagged(Actor, (int32)R.Tag, Detail))
		{
			++R.CollisionHits;
			if (R.FirstCollisionDetail.IsEmpty())
			{
				R.FirstCollisionDetail = Detail;
			}
			for (const TPair<uint64, int32>& Pair : ArmedRequestToRecord)
			{
				if (Pair.Value == RecordIndex)
				{
					PollutedRequests.Add(Pair.Key);
				}
			}
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(mask): OBSERVED - the stencil tag did not read back for '%s' on '%s' (tag %d) - %s. ")
				TEXT("CAUSE NOT ESTABLISHED. The frames currently in flight for this event are discarded; ")
				TEXT("frames whose reads were clean still contribute, and an event with no clean frame stays ")
				TEXT("NOT_MEASURED (admit), never MEASURED_ZERO. DISCRIMINATORS: the component property being ")
				TEXT("overwritten means something else re-asserted it; the property reading back correctly while ")
				TEXT("the MASK still misses the tag means the fault is on the READ side, not the write. Reserved ")
				TEXT("base %d is a CONVENTION, not a reservation."),
				*R.Id.ToString(), *R.Target, (int32)R.Tag, *Detail, AnomalyStencilTag::ReservedStencilBase);
		}
	}
}

void FAnomalyMaskMeasure::CollectResults(FAnomalyMaskSceneViewExtension* Sve)
{
	if (!Sve)
	{
		return;
	}

	TArray<uint64> Armed;
	ArmedRequestToRecord.GetKeys(Armed);
	for (uint64 RequestId : Armed)
	{
		FAnomalyMaskResult Mask;
		if (!Sve->TakeMaskResult(RequestId, Mask))
		{
			continue;
		}

		const int32* IndexPtr = ArmedRequestToRecord.Find(RequestId);
		if (!IndexPtr || !Records.IsValidIndex(*IndexPtr))
		{
			ArmedRequestToRecord.Remove(RequestId);
			PollutedRequests.Remove(RequestId);
			ProbeRequests.Remove(RequestId);
			EndFrameSample.Remove(RequestId);
			continue;
		}

		FAnomalyMaskRecord& R = Records[*IndexPtr];
		++R.ArmsResolved;
		R.ViewportPixels = Mask.ViewRectSize.X * Mask.ViewRectSize.Y;

		const bool bProbe = ProbeRequests.Remove(RequestId) > 0;
		bool bFramePolluted = PollutedRequests.Remove(RequestId) > 0;

		uint8 Sample = 255;
		const bool bSampled = EndFrameSample.RemoveAndCopyValue(RequestId, Sample);
		const bool bConfirmedVisible = bSampled && Sample == 1;

		if (Mask.bSawUnassignedReservedTag)
		{
			bFramePolluted = true;
			++R.CollisionHits;
			if (R.FirstCollisionDetail.IsEmpty())
			{
				R.FirstCollisionDetail = FString::Printf(
					TEXT("unassigned reserved tag %d observed%s, cause not established"),
					(int32)Mask.FirstUnassignedTag,
					bProbe ? TEXT(" (PROBE frame - expected)") : TEXT(""));
			}
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(mask): OBSERVED - the mask carried reserved-range tag %d, which this run never ")
				TEXT("assigned.%s CAUSE NOT ESTABLISHED. THIS FRAME's read is discarded; frames of this event ")
				TEXT("whose reads were clean still contribute, and an event with no clean frame stays ")
				TEXT("NOT_MEASURED (admit), never MEASURED_ZERO. DISCRIMINATORS: 255 uniformly across the ")
				TEXT("frame is the engine's StencilDummy fallback (FColor::White), bound when custom depth ")
				TEXT("was NOT produced for the frame - i.e. our stencil was never read; a value in a ")
				TEXT("GEOMETRY-SHAPED region, or any value other than 255, is something genuinely writing ")
				TEXT("into the reserved range."),
				(int32)Mask.FirstUnassignedTag,
				bProbe ? TEXT(" THIS IS THE PROBE FRAME - the fire is EXPECTED and demonstrates the detector is live.") : TEXT(""));
		}

		if (bProbe)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(mask): M26S1 PROBE RESULT id=%llu target=%s tag=%d detector255Fired=%d ")
				TEXT("confirmationReadHidden=%d - frame bucketed PROBE, discarded, contributes nothing. ")
				TEXT("F-6 item 5: the detectors demonstrated LIVE on this binary."),
				RequestId, *R.Target, (int32)R.Tag,
				Mask.bSawUnassignedReservedTag ? 1 : 0,
				(bSampled && !bConfirmedVisible) ? 1 : 0);
			ArmedRequestToRecord.Remove(RequestId);
			continue;
		}

		const bool bPassRan = Mask.CustomStencilExtent.X > 1 && Mask.CustomStencilExtent.Y > 1;
		if (!bPassRan)
		{
			++R.FramesNoPass;
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(mask): M26S1 NO-PASS id=%llu target=%s tag=%d customStencilExtent=%dx%d - the ")
				TEXT("custom-depth pass did not produce for this target on this frame, so the bound stencil is ")
				TEXT("the engine's 1x1 StencilDummy and the frame carries NO EVIDENCE about the target. It is ")
				TEXT("discarded (frame-scoped). A frame contributes only on POSITIVE evidence that the pass ran; ")
				TEXT("the 255 detector is a SECONDARY signal and cannot supply that evidence, because it can ")
				TEXT("fire on at most one pixel and only when the depth gate passes there (G133). ")
				TEXT("THIS IS NOT A NANITE-SPECIFIC COUNTER: the causes include NANITE geometry, which cannot ")
				TEXT("write custom depth at all on UE 5.1 (G134), FRUSTUM CULLING, and any other route by which ")
				TEXT("the target is absent from the view's relevant set. In every case the frame is discarded ")
				TEXT("and the event tends toward NOT_MEASURED, which ADMITS."),
				RequestId, *R.Target, (int32)R.Tag,
				Mask.CustomStencilExtent.X, Mask.CustomStencilExtent.Y);
			ArmedRequestToRecord.Remove(RequestId);
			continue;
		}

		if (bSampled && !bConfirmedVisible)
		{
			++R.FramesResidualDiscarded;
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(mask): M26S1 RESIDUAL id=%llu target=%s tag=%d - the ARM gate accepted this ")
				TEXT("frame and the end-of-frame confirmation read HIDDEN: the state changed AFTER ")
				TEXT("OnWorldTickEnd and before the frame was drawn. OBSERVED, cause not established. The ")
				TEXT("frame is discarded (frame-scoped). On this bench the expected count is ZERO; a ")
				TEXT("non-zero on a host title is post-tick hidden toggling occurring in the wild."),
				RequestId, *R.Target, (int32)R.Tag);
			ArmedRequestToRecord.Remove(RequestId);
			continue;
		}

		if (bFramePolluted)
		{
			++R.FramesDiscarded;
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(mask): M24 FRAME DISCARDED id=%llu target=%s tag=%d framesDiscarded=%d ")
				TEXT("framesContributed=%d (frame-scoped: only this read is dropped)"),
				RequestId, *R.Target, (int32)R.Tag, R.FramesDiscarded, R.FramesContributed);
			ArmedRequestToRecord.Remove(RequestId);
			continue;
		}

		if (!bSampled)
		{
			++R.FramesUnconfirmed;
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(mask): M26S1 UNCONFIRMED id=%llu target=%s tag=%d - no end-of-frame sample ")
				TEXT("ran for this armed frame, so it is discarded under the whitelist polarity: a missing ")
				TEXT("check must never read as a passed check. Expected count is ZERO everywhere; non-zero ")
				TEXT("means the confirmation instrument itself was not live for this frame."),
				RequestId, *R.Target, (int32)R.Tag);
			ArmedRequestToRecord.Remove(RequestId);
			continue;
		}

		int32 Count = 0;
		if (const FAnomalyMaskTagResult* Found = Mask.TagResults.Find(R.Tag))
		{
			Count = Found->Count;
		}

		++R.FramesContributed;
		R.MaxCount = FMath::Max(R.MaxCount, Count);
		R.State = (R.MaxCount > 0) ? EAnomalyMaskState::MeasuredNonZero : EAnomalyMaskState::MeasuredZero;

		ArmedRequestToRecord.Remove(RequestId);
	}
}

void FAnomalyMaskMeasure::SampleEndOfFrame()
{
	for (uint64 RequestId : ArmedThisFrame)
	{
		const int32* IndexPtr = ArmedRequestToRecord.Find(RequestId);
		if (!IndexPtr || !Records.IsValidIndex(*IndexPtr))
		{
			continue;
		}
		const FAnomalyMaskRecord& R = Records[*IndexPtr];
		const AActor* Actor = R.TargetActor.Get();
		const bool bHiddenNow = !Actor || Actor->IsHidden();
		const bool bProbe = ProbeRequests.Contains(RequestId);
		EndFrameSample.Add(RequestId, bHiddenNow ? (uint8)0 : (uint8)1);
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(mask): M24 ENDFRAME id=%llu target=%s tag=%d hiddenAtArm=%d hiddenAtEndOfFrame=%d ")
			TEXT("actorValid=%d probe=%d (sampled after all subsystem ticks; this is the state the frame ")
			TEXT("rendered; a frame contributes ONLY if this sample ran and read visible)"),
			RequestId, *R.Target, (int32)R.Tag,
			bProbe ? 1 : 0,
			bHiddenNow ? 1 : 0,
			Actor ? 1 : 0,
			bProbe ? 1 : 0);
	}
	ArmedThisFrame.Reset();
}

#endif
