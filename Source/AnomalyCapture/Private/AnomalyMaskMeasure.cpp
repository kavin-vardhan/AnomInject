#include "AnomalyMaskMeasure.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"
#include "AnomalyMaskSceneViewExtension.h"
#include "AnomalyStencilTag.h"

#include "GameFramework/Actor.h"

void FAnomalyMaskMeasure::BeginRun()
{
	Records.Reset();
	ArmedRequestToRecord.Reset();
	NextTagOffset = 0;
	AnomalyStencilTag::EnableCustomStencil();
}

void FAnomalyMaskMeasure::EndRun()
{
	UntagAll();
	AnomalyStencilTag::DisableCustomStencil();
	ArmedRequestToRecord.Reset();
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
		return true;
	}

	return false;
}

void FAnomalyMaskMeasure::VerifyPendingTags()
{
	for (FAnomalyMaskRecord& R : Records)
	{
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
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(mask): STENCIL TAG DID NOT SURVIVE for '%s' on '%s' (tag %d) - %s. ")
				TEXT("Treating this measurement as NOT_MEASURED (admit). A host title writing custom stencil ")
				TEXT("shares the 0-255 space with our reserved base %d; the base is a CONVENTION, not a reservation."),
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
			continue;
		}

		FAnomalyMaskRecord& R = Records[*IndexPtr];
		++R.ArmsResolved;
		R.ViewportPixels = Mask.ViewRectSize.X * Mask.ViewRectSize.Y;

		if (Mask.bSawUnassignedReservedTag)
		{
			++R.CollisionHits;
			if (R.FirstCollisionDetail.IsEmpty())
			{
				R.FirstCollisionDetail = FString::Printf(
					TEXT("mask carried unassigned reserved tag %d"), (int32)Mask.FirstUnassignedTag);
			}
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(mask): the mask carried reserved tag %d which this run never assigned. ")
				TEXT("A host title is writing into the reserved custom-stencil range; measurements this run ")
				TEXT("are NOT trustworthy and are treated as NOT_MEASURED (admit)."),
				(int32)Mask.FirstUnassignedTag);
		}

		if (R.CollisionHits > 0)
		{
			ArmedRequestToRecord.Remove(RequestId);
			continue;
		}

		int32 Count = 0;
		if (const FAnomalyMaskTagResult* Found = Mask.TagResults.Find(R.Tag))
		{
			Count = Found->Count;
		}

		R.MaxCount = FMath::Max(R.MaxCount, Count);
		R.State = (R.MaxCount > 0) ? EAnomalyMaskState::MeasuredNonZero : EAnomalyMaskState::MeasuredZero;

		ArmedRequestToRecord.Remove(RequestId);
	}
}

#endif
