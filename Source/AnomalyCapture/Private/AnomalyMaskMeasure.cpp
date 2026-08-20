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

void FAnomalyMaskMeasure::BeginRun()
{
	Records.Reset();
	ArmedRequestToRecord.Reset();
	PollutedRequests.Reset();
	ArmedThisFrame.Reset();
	NextTagOffset = 0;

	const int32 Before = ReadCustomDepthCVar();
	AnomalyStencilTag::EnableCustomStencil();
	const int32 After = ReadCustomDepthCVar();

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(mask): M23 CVAR beginRun rCustomDepth before=%d after=%d (3 = EnabledWithStencil; ")
		TEXT("1 = Enabled WITHOUT stencil writes, which is the engine default)"),
		Before, After);
}

void FAnomalyMaskMeasure::EndRun()
{
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(mask): M23 CVAR finishRun rCustomDepth before restore=%d"), ReadCustomDepthCVar());

	UntagAll();
	AnomalyStencilTag::DisableCustomStencil();
	ArmedRequestToRecord.Reset();
	PollutedRequests.Reset();
	ArmedThisFrame.Reset();
	NextTagOffset = 0;
}

void FAnomalyMaskMeasure::UntagAll()
{
	AnomalyStencilTag::RestoreAll();
}

int32 FAnomalyMaskMeasure::AllocateTag()
{
	const int32 Span = AnomalyStencilTag::ReservedStencilMax - AnomalyStencilTag::ReservedStencilBase + 1;
	const int32 Tag = AnomalyStencilTag::ReservedStencilBase + (NextTagOffset % Span);
	++NextTagOffset;
	return Tag;
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

TSet<uint8> FAnomalyMaskMeasure::BuildAssignedTagSet() const
{
	TSet<uint8> Out;
	for (const FAnomalyMaskRecord& R : Records)
	{
		Out.Add(R.Tag);
	}
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
			continue;
		}

		FAnomalyMaskRecord& R = Records[*IndexPtr];
		++R.ArmsResolved;
		R.ViewportPixels = Mask.ViewRectSize.X * Mask.ViewRectSize.Y;

		bool bFramePolluted = PollutedRequests.Remove(RequestId) > 0;

		if (Mask.bSawUnassignedReservedTag)
		{
			bFramePolluted = true;
			++R.CollisionHits;
			if (R.FirstCollisionDetail.IsEmpty())
			{
				R.FirstCollisionDetail = FString::Printf(
					TEXT("unassigned reserved tag %d observed, cause not established"),
					(int32)Mask.FirstUnassignedTag);
			}
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(mask): OBSERVED - the mask carried reserved-range tag %d, which this run never ")
				TEXT("assigned. CAUSE NOT ESTABLISHED. THIS FRAME's read is discarded; frames of this event ")
				TEXT("whose reads were clean still contribute, and an event with no clean frame stays ")
				TEXT("NOT_MEASURED (admit), never MEASURED_ZERO. DISCRIMINATORS: 255 uniformly across the ")
				TEXT("frame is the engine's StencilDummy fallback (FColor::White), bound when custom depth ")
				TEXT("was NOT produced for the frame - i.e. our stencil was never read; a value in a ")
				TEXT("GEOMETRY-SHAPED region, or any value other than 255, is something genuinely writing ")
				TEXT("into the reserved range."),
				(int32)Mask.FirstUnassignedTag);
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
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(mask): M24 ENDFRAME id=%llu target=%s tag=%d hiddenAtArm=0 hiddenAtEndOfFrame=%d ")
			TEXT("actorValid=%d (sampled after all subsystem ticks; this is the state the frame rendered)"),
			RequestId, *R.Target, (int32)R.Tag,
			(Actor && Actor->IsHidden()) ? 1 : 0,
			Actor ? 1 : 0);
	}
	ArmedThisFrame.Reset();
}

#endif
