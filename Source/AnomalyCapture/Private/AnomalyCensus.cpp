#include "AnomalyCensus.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"
#include "AnomalyStencilTag.h"
#include "AnomalyMaskSceneViewExtension.h"
#include "AnomalyViewport.h"

#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "SceneInterface.h"
#include "RenderUtils.h"
#include "RHI.h"
#include "Materials/MaterialInterface.h"
#include "MaterialShared.h"
#include "HAL/PlatformTime.h"

const TCHAR* LexToStringAnomalyCensusVerdict(EAnomalyCensusVerdict V)
{
	switch (V)
	{
	case EAnomalyCensusVerdict::MeasuredZero:           return TEXT("MEASURED_ZERO");
	case EAnomalyCensusVerdict::MeasuredNonZero:        return TEXT("MEASURED_NONZERO");
	case EAnomalyCensusVerdict::NotMeasurableNanite:    return TEXT("NOT_MEASURABLE(nanite)");
	case EAnomalyCensusVerdict::NotMeasurableTagFailed: return TEXT("NOT_MEASURABLE(tag_failed)");
	case EAnomalyCensusVerdict::NotMeasurableHidden:    return TEXT("NOT_MEASURABLE(hidden)");
	case EAnomalyCensusVerdict::ExcludedTranslucent:    return TEXT("EXCLUDED(translucent)");
	default:                                            return TEXT("NOT_MEASURABLE(not_yet_measured)");
	}
}

namespace
{
	constexpr uint64 GCensusRequestBit = 1ull << 62;

	bool ComponentSlotsAllTranslucent(const UPrimitiveComponent* Prim, bool bAllowCustomDepthOptIn)
	{
		const int32 NumSlots = Prim->GetNumMaterials();
		if (NumSlots <= 0)
		{
			return false;
		}
		for (int32 Slot = 0; Slot < NumSlots; ++Slot)
		{
			UMaterialInterface* M = const_cast<UPrimitiveComponent*>(Prim)->GetMaterial(Slot);
			if (!M)
			{
				return false;
			}
			if (!IsTranslucentBlendMode(M->GetBlendMode()))
			{
				return false;
			}
			if (bAllowCustomDepthOptIn && M->IsTranslucencyWritingCustomDepth())
			{
				return false;
			}
		}
		return true;
	}

	enum class ECensusClass : uint8 { Measurable, Nanite, Translucent, Hidden, HeldElsewhere };

	bool ComponentRendersAsNanite(const UStaticMeshComponent* SMC, EShaderPlatform ShaderPlatform)
	{
		if (SMC->bDisallowNanite)
		{
			return false;
		}
#if WITH_EDITORONLY_DATA
		if (SMC->bDisplayNaniteFallbackMesh)
		{
			return false;
		}
#endif
		const UStaticMesh* Mesh = SMC->GetStaticMesh();
		return Mesh && Mesh->HasValidNaniteData() && UseNanite(ShaderPlatform);
	}

	ECensusClass ClassifyCandidate(const AActor* Actor, bool bExcludeTranslucent, bool bAllowCustomDepthOptIn,
		EShaderPlatform ShaderPlatform)
	{
		if (Actor->IsHidden())
		{
			return ECensusClass::Hidden;
		}
		if (AnomalyStencilTag::IsAnyComponentTagged(Actor))
		{
			return ECensusClass::HeldElsewhere;
		}

		TInlineComponentArray<UPrimitiveComponent*> Prims;
		const_cast<AActor*>(Actor)->GetComponents(Prims);

		int32 Renderable = 0;
		int32 NaniteOnly = 0;
		int32 TranslucentOnly = 0;
		for (const UPrimitiveComponent* Prim : Prims)
		{
			if (!AnomalyViewport::IsRenderableComponent(Prim))
			{
				continue;
			}
			++Renderable;

			if (bExcludeTranslucent && ComponentSlotsAllTranslucent(Prim, bAllowCustomDepthOptIn))
			{
				++TranslucentOnly;
				continue;
			}

			if (const UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Prim))
			{
				if (ComponentRendersAsNanite(SMC, ShaderPlatform))
				{
					++NaniteOnly;
					continue;
				}
			}
		}

		if (Renderable == 0)
		{
			return ECensusClass::HeldElsewhere;
		}
		if (bExcludeTranslucent && TranslucentOnly == Renderable)
		{
			return ECensusClass::Translucent;
		}
		if (NaniteOnly + TranslucentOnly == Renderable)
		{
			return ECensusClass::Nanite;
		}
		return ECensusClass::Measurable;
	}
}

void FAnomalyCensus::Begin(UWorld* World, FAnomalyStencilTagLedger* InLedger, const FAnomalyCensusParams& InParams)
{
	bActive = true;
	bCycleOpen = false;
	bCycleJustCompleted = false;
	bLeakProbeFired = false;
	Params = InParams;
	Ledger = InLedger;
	Counters = FAnomalyCensusCounters();
	Entries.Reset();
	CycleQueue.Reset();
	InFlight.Reset();
	WorldPtr = World;
	CensusIdSerial = 0;
	CycleStartTick = GFrameCounter;
	LastCompletedCycleTicks = 0;
	CycleNumber = 0;

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Census: BEGIN floor=%.2f%% ceiling=%s maxVerdictAgeTicks=%d excludeTranslucent=%d ")
		TEXT("includeTranslucentWriters=%d leakProbe=%d ")
		TEXT("coArmOnly=%d hostReserved=%d assignable=%d. The band is INCLUSIVE: eligible iff ")
		TEXT("floor <= coverage <= ceiling. The census is UPSTREAM of selection only; the armed-frame ")
		TEXT("measurement and the zero-only veto are unchanged and remain the backstop."),
		Params.FloorPct,
		IsCeilingEnabled()
			? *FString::Printf(TEXT("%.2f%%"), Params.CeilingPct)
			: TEXT("DISABLED (<=0; NO upper bound, scenery-scale targets ARE eligible)"),
		Params.MaxVerdictAgeTicks, Params.bExcludeTranslucent ? 1 : 0,
		Params.bIncludeTranslucentCustomDepthWriters ? 1 : 0,
		Params.bLeakProbe ? 1 : 0, Params.bCoArmOnly ? 1 : 0,
		Ledger ? Ledger->HostReserved.Num() : 0, Ledger ? Ledger->NumAssignable() : 55);

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Census: BEGIN m41 EXPIRY - a verdict is fresh iff its age <= max(maxVerdictAgeTicks=%d, ")
		TEXT("lastCompletedCycleTicks + %d). The knob is the FLOOR of that window, never a cap that can ")
		TEXT("shrink it below one cycle - a host whose cycle exceeds the knob would otherwise expire its ")
		TEXT("earliest-measured candidates before the cycle closed, biasing selection toward whatever was ")
		TEXT("measured last, silently. A knob of 0 STILL expires everything (the P-C11 loud-inert control ")
		TEXT("is preserved by construction). Effective window right now: %d tick(s)%s."),
		Params.MaxVerdictAgeTicks, LostAfterTicks, GetFreshnessWindowTicks(),
		Params.bBenchFixedExpiry
			? TEXT(" [IAI.Bench.CensusFixedExpiry ON - BENCH LEVER forcing the pre-m41 FIXED window]")
			: TEXT(""));

	if (Params.BenchBatchCap > 0 || Params.BenchDropEveryNth > 0 || Params.bBenchFixedExpiry)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Census: BENCH LEVERS ACTIVE - batchCap=%d dropEveryNth=%d fixedExpiry=%d. These are ")
			TEXT("GATE ARTEFACTS (console-only, no ini key, default off, never in a client payload). A ")
			TEXT("capture taken with any of them on is a GATE LEG, not a dataset."),
			Params.BenchBatchCap, Params.BenchDropEveryNth, Params.bBenchFixedExpiry ? 1 : 0);
	}
}

void FAnomalyCensus::End(UWorld* World)
{
	if (!bActive)
	{
		return;
	}
	for (FBatch& Batch : InFlight)
	{
		ReleaseBatch(Batch, false);
	}
	InFlight.Reset();
	if (bCycleOpen)
	{
		CloseCycle();
	}
	bActive = false;

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Census: SUMMARY frames=%d cycles=%d candidates=%d zero=%d nonzero=%d belowFloor=%d aboveCeiling=%d ")
		TEXT("excludedTranslucent=%d nanite=%d tagFailed=%d hidden=%d notYetMeasured=%d firesFallbackAll=%d ")
		TEXT("firesPartialFallback=%d firesUnseenCandidates=%d hostPpCustomDepthReaders=%d ")
		TEXT("framesNoPass=%d framesPolluted=%d batchesLost=%d tagOvertaken=%d proxyRecreatesQueued=%d ")
		TEXT("tagBlockMsTotal=%.3f"),
		Counters.CensusFrames, Counters.Cycles, Counters.Candidates, Counters.Zero, Counters.NonZero,
		Counters.BelowFloor, Counters.AboveCeiling, Counters.ExcludedTranslucent, Counters.UnmeasurableNanite,
		Counters.UnmeasurableTagFailed, Counters.UnmeasurableHidden, Counters.NotYetMeasured,
		Counters.FiresFallbackAll, Counters.FiresPartialFallback, Counters.FiresUnseenCandidates,
		Counters.HostPpCustomDepthReaders, Counters.FramesNoPass, Counters.FramesPolluted, Counters.BatchesLost,
		Counters.TagOvertaken, Counters.ProxyRecreatesQueued, Counters.TagBlockMsTotal);
}

TSet<uint8> FAnomalyCensus::GetInFlightTags() const
{
	TSet<uint8> Out;
	for (const FBatch& Batch : InFlight)
	{
		for (uint8 Tag : Batch.Tags)
		{
			Out.Add(Tag);
		}
	}
	return Out;
}

TSet<uint8> FAnomalyCensus::GetLegitTags() const
{
	TSet<uint8> Out = GetInFlightTags();
	for (const TPair<uint8, uint64>& Pair : RecentlyReleased)
	{
		Out.Add(Pair.Key);
	}
	return Out;
}

bool FAnomalyCensus::ConsumeCycleJustCompleted()
{
	const bool bWas = bCycleJustCompleted;
	bCycleJustCompleted = false;
	return bWas;
}

void FAnomalyCensus::NoteFire(int32 Consulted, int32 Fallback, int32 Unseen)
{
	if (Consulted > 0 && Fallback == Consulted)
	{
		++Counters.FiresFallbackAll;
	}
	if (Fallback > 0)
	{
		++Counters.FiresPartialFallback;
	}
	if (Unseen > 0)
	{
		++Counters.FiresUnseenCandidates;
	}
}

int32 FAnomalyCensus::GetFreshnessWindowTicks() const
{
	if (Params.MaxVerdictAgeTicks <= 0)
	{
		return 0;
	}
	if (Params.bBenchFixedExpiry)
	{
		return Params.MaxVerdictAgeTicks;
	}
	const uint64 CycleTerm = LastCompletedCycleTicks + (uint64)LostAfterTicks;
	const int32 CycleTermClamped = (int32)FMath::Min<uint64>(CycleTerm, 600ull);
	return FMath::Max(Params.MaxVerdictAgeTicks, CycleTermClamped);
}

FAnomalyCensusOpinion FAnomalyCensus::QueryActor(const AActor* Actor) const
{
	FAnomalyCensusOpinion Out;
	if (!bActive || !Actor)
	{
		return Out;
	}

	Out.WindowTicks = GetFreshnessWindowTicks();

	const FAnomalyCensusEntry* Found = nullptr;
	for (const FAnomalyCensusEntry& Entry : Entries)
	{
		if (Entry.Actor.Get() == Actor)
		{
			Found = &Entry;
			break;
		}
	}

	if (!Found)
	{
		Out.Decision = EAnomalyCensusDecision::FallbackBounds;
		Out.Reason = TEXT("unseen");
		Out.bUnseen = true;
		return Out;
	}

	if (Found->Verdict == EAnomalyCensusVerdict::ExcludedTranslucent)
	{
		Out.Decision = Params.bExcludeTranslucent
			? EAnomalyCensusDecision::ExcludedTranslucent
			: EAnomalyCensusDecision::FallbackBounds;
		Out.Reason = TEXT("translucent");
		return Out;
	}

	if (Found->Verdict == EAnomalyCensusVerdict::NotMeasurableNanite)
	{
		Out.Decision = EAnomalyCensusDecision::FallbackBounds;
		Out.Reason = TEXT("nanite");
		return Out;
	}
	if (Found->Verdict == EAnomalyCensusVerdict::NotMeasurableTagFailed)
	{
		Out.Decision = EAnomalyCensusDecision::FallbackBounds;
		Out.Reason = TEXT("tag_failed");
		return Out;
	}
	if (Found->Verdict == EAnomalyCensusVerdict::NotMeasurableHidden)
	{
		Out.Decision = EAnomalyCensusDecision::FallbackBounds;
		Out.Reason = TEXT("hidden");
		return Out;
	}
	if (Found->Verdict == EAnomalyCensusVerdict::NotYetMeasured || Found->TimesMeasured <= 0)
	{
		Out.Decision = EAnomalyCensusDecision::FallbackBounds;
		Out.Reason = TEXT("not_yet_measured");
		return Out;
	}

	const int32 Age = (int32)(GFrameCounter - Found->MeasuredAtTick);
	Out.AgeTicks = Age;
	if (Age > Out.WindowTicks)
	{
		Out.Decision = EAnomalyCensusDecision::FallbackBounds;
		Out.Reason = TEXT("expired");
		Out.bExpired = true;
		return Out;
	}

	if (Found->Verdict == EAnomalyCensusVerdict::MeasuredZero)
	{
		Out.Decision = EAnomalyCensusDecision::ExcludedZero;
		Out.Reason = TEXT("measured_zero");
		Out.DrawnPct = 0.0f;
		return Out;
	}

	const float Pct = (Found->FramePx > 0)
		? (100.0f * (float)Found->DrawnPx / (float)Found->FramePx)
		: 0.0f;
	Out.DrawnPct = Pct;

	if (Pct < Params.FloorPct)
	{
		Out.Decision = EAnomalyCensusDecision::ExcludedBelowFloor;
		Out.Reason = TEXT("below_floor");
		return Out;
	}
	if (IsCeilingEnabled() && Pct > Params.CeilingPct)
	{
		Out.Decision = EAnomalyCensusDecision::ExcludedAboveCeiling;
		Out.Reason = TEXT("above_ceiling");
		return Out;
	}
	Out.Decision = EAnomalyCensusDecision::Eligible;
	Out.Reason = TEXT("measured_nonzero");
	return Out;
}

void FAnomalyCensus::Tick(UWorld* World, FAnomalyMaskSceneViewExtension* Sve, bool bEventArmedThisTick, const TSet<uint8>& EventTags)
{
	if (!bActive || !Sve || !World)
	{
		return;
	}

	const double T0 = FPlatformTime::Seconds();

	RecentlyReleased.RemoveAll([](const TPair<uint8, uint64>& Pair)
	{
		return GFrameCounter > Pair.Value + (uint64)LostAfterTicks;
	});

	CollectBatches(Sve, EventTags);

	if (CycleQueue.Num() == 0 && InFlight.Num() == 0)
	{
		if (bCycleOpen)
		{
			CloseCycle();
			bCycleJustCompleted = true;
		}
		StartCycle(World);
	}

	if (CycleQueue.Num() > 0 && (!Params.bCoArmOnly || bEventArmedThisTick))
	{
		ArmNextBatch(Sve, EventTags);
	}

	Counters.TagBlockMsTotal += (FPlatformTime::Seconds() - T0) * 1000.0;
}

void FAnomalyCensus::StartCycle(UWorld* World)
{
	TMap<const AActor*, FAnomalyCensusEntry> Carry;
	for (FAnomalyCensusEntry& E : Entries)
	{
		if (const AActor* A = E.Actor.Get())
		{
			Carry.Add(A, MoveTemp(E));
		}
	}

	Entries.Reset();
	CycleQueue.Reset();

	const TArray<TWeakObjectPtr<AActor>> Prefiltered = AnomalyViewport::GetCensusPrefilterActors(World);
	if (Prefiltered.Num() == 0)
	{
		bCycleOpen = false;
		return;
	}

	int32 PrefilterIndex = 0;
	int32 BenchDropped = 0;
	for (const TWeakObjectPtr<AActor>& Weak : Prefiltered)
	{
		AActor* Actor = Weak.Get();
		if (!Actor)
		{
			continue;
		}

		const int32 ThisIndex = PrefilterIndex++;
		if (Params.BenchDropEveryNth > 0 && (ThisIndex % Params.BenchDropEveryNth) == 0)
		{
			++BenchDropped;
			continue;
		}

		FAnomalyCensusEntry Entry;
		if (const FAnomalyCensusEntry* Prev = Carry.Find(Actor))
		{
			Entry = *Prev;
		}
		Entry.Actor = Actor;
		Entry.ActorName = Actor->GetName();
		Entry.AttemptsThisCycle = 0;

		const EShaderPlatform ShaderPlatform = World->Scene ? World->Scene->GetShaderPlatform() : GMaxRHIShaderPlatform;
		const ECensusClass Class = ClassifyCandidate(Actor, Params.bExcludeTranslucent,
			Params.bIncludeTranslucentCustomDepthWriters, ShaderPlatform);
		switch (Class)
		{
		case ECensusClass::Translucent:
			Entry.Verdict = EAnomalyCensusVerdict::ExcludedTranslucent;
			break;
		case ECensusClass::Nanite:
			Entry.Verdict = EAnomalyCensusVerdict::NotMeasurableNanite;
			break;
		case ECensusClass::Hidden:
			Entry.Verdict = EAnomalyCensusVerdict::NotMeasurableHidden;
			break;
		case ECensusClass::HeldElsewhere:
			break;
		default:
			CycleQueue.Add(Entries.Num());
			break;
		}
		Entries.Add(MoveTemp(Entry));
	}

	if (BenchDropped > 0)
	{
		UE_LOG(LogAnomalyCapture, Warning,
			TEXT("Census: BENCH DROP-ENTRY omitted %d of %d prefiltered actor(s) from cycle %d ")
			TEXT("(IAI.Bench.CensusDropEntry %d). Those actors are still SEEN BY THE FIRE PATH and now have ")
			TEXT("NO census entry, so they must read 'unseen' at fire time - this is E-G2's positive control ")
			TEXT("(G96), not a defect."),
			BenchDropped, PrefilterIndex, CycleNumber + 1, Params.BenchDropEveryNth);
	}

	CycleStartTick = GFrameCounter;
	CycleStartTagBlockMs = Counters.TagBlockMsTotal;
	CycleStartFlips = Counters.ProxyRecreatesQueued;
	++CycleNumber;
	bCycleOpen = true;
	HalfCap = FMath::Max(1, (Ledger ? Ledger->NumFree() : 55) / 2);
	if (Params.BenchBatchCap > 0)
	{
		HalfCap = FMath::Max(1, FMath::Min(HalfCap, Params.BenchBatchCap));
	}
}

void FAnomalyCensus::CreditEntryFromResult(int32 EntryIndex, uint8 Tag, const FAnomalyMaskResult& Result)
{
	if (!Entries.IsValidIndex(EntryIndex))
	{
		return;
	}
	FAnomalyCensusEntry& Entry = Entries[EntryIndex];
	int32 Count = 0;
	if (const FAnomalyMaskTagResult* Found = Result.TagResults.Find(Tag))
	{
		Count = Found->Count;
	}
	Entry.DrawnPx = Count;
	Entry.FramePx = Result.ViewRectSize.X * Result.ViewRectSize.Y;
	Entry.Verdict = (Count > 0) ? EAnomalyCensusVerdict::MeasuredNonZero : EAnomalyCensusVerdict::MeasuredZero;
	Entry.MeasuredAtTick = GFrameCounter;
	++Entry.TimesMeasured;
}

void FAnomalyCensus::ReleaseBatch(FBatch& Batch, bool bRequeue)
{
	for (int32 k = 0; k < Batch.EntryIdx.Num(); ++k)
	{
		const int32 EntryIndex = Batch.EntryIdx[k];
		const uint8 Tag = Batch.Tags[k];
		AActor* Actor = Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].Actor.Get() : nullptr;
		if (Actor)
		{
			FString Detail;
			if (AnomalyStencilTag::VerifyActorStillTagged(Actor, (int32)Tag, Detail))
			{
				if (Params.bLeakProbe && !bLeakProbeFired)
				{
					const FString Forgotten = AnomalyStencilTag::ForgetOneTaggedComponentOfActor(Actor);
					if (!Forgotten.IsEmpty())
					{
						bLeakProbeFired = true;
						UE_LOG(LogAnomalyCapture, Warning,
							TEXT("Census: LEAK PROBE fired (gate use only) - component '%s' of '%s' was ")
							TEXT("DELIBERATELY dropped from the restore map with tag %d still applied. The ")
							TEXT("final CENSUS-HYGIENE check MUST report a DIFF naming it; a clean final ")
							TEXT("hygiene read on this leg means the instrument is blind (G96)."),
							*Forgotten, *Entries[EntryIndex].ActorName, (int32)Tag);
					}
				}
				AnomalyStencilTag::RestoreActor(Actor);
			}
			else
			{
				++Counters.TagOvertaken;
				UE_LOG(LogAnomalyCapture, Log,
					TEXT("Census: TAG-OVERTAKEN '%s' tag=%d (%s) - another writer re-tagged this actor while the ")
					TEXT("census batch was in flight (the event mask tagging a fresh fire is the expected case). ")
					TEXT("No credit taken, no restore performed here; the run-end RestoreAll covers the prior."),
					*Entries[EntryIndex].ActorName, (int32)Tag, *Detail);
			}
		}
		if (Ledger)
		{
			Ledger->CensusClaimed.Remove(Tag);
		}
		RecentlyReleased.Add(TPair<uint8, uint64>(Tag, GFrameCounter));
		if (bRequeue && Entries.IsValidIndex(EntryIndex)
			&& Entries[EntryIndex].AttemptsThisCycle < MaxAttemptsPerCycle)
		{
			CycleQueue.Add(EntryIndex);
		}
	}
	Batch.EntryIdx.Reset();
	Batch.Tags.Reset();
}

void FAnomalyCensus::CollectBatches(FAnomalyMaskSceneViewExtension* Sve, const TSet<uint8>& EventTags)
{
	for (int32 i = InFlight.Num() - 1; i >= 0; --i)
	{
		FBatch& Batch = InFlight[i];

		FAnomalyMaskResult Result;
		if (Sve->TakeMaskResult(Batch.RequestId, Result))
		{
			const bool bPassRan = Result.CustomStencilExtent.X > 1 && Result.CustomStencilExtent.Y > 1;

			bool bPolluted = false;
			uint8 PollutingTag = 0;
			if (bPassRan)
			{
				TSet<uint8> Allowed = EventTags;
				Allowed.Append(Batch.CensusTagsAtArm);
				Allowed.Append(GetLegitTags());
				if (Ledger)
				{
					Allowed.Append(Ledger->CensusClaimed);
					Allowed.Append(Ledger->HostReserved);
				}
				for (const TPair<uint8, FAnomalyMaskTagResult>& Pair : Result.TagResults)
				{
					if (!Allowed.Contains(Pair.Key))
					{
						bPolluted = true;
						PollutingTag = Pair.Key;
						break;
					}
				}
			}

			if (!bPassRan)
			{
				++Counters.FramesNoPass;
				UE_LOG(LogAnomalyCapture, Log,
					TEXT("Census: NO-PASS batch id=%llu customStencilExtent=%dx%d - the frame carries no evidence ")
					TEXT("about any candidate (view-level; the frame is discarded and the batch re-queued; this ")
					TEXT("NEVER classifies a candidate - per-candidate NOT_MEASURABLE comes only from per-candidate ")
					TEXT("evidence, the H6 rule)."),
					Batch.RequestId, Result.CustomStencilExtent.X, Result.CustomStencilExtent.Y);
				ReleaseBatch(Batch, true);
			}
			else if (bPolluted)
			{
				++Counters.FramesPolluted;
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Census: OBSERVED - batch id=%llu carried reserved-range tag %d assigned to no batch, no ")
					TEXT("event record and no host reservation. CAUSE NOT ESTABLISHED. The frame is discarded and ")
					TEXT("the batch re-queued; candidates keep their prior verdicts."),
					Batch.RequestId, (int32)PollutingTag);
				ReleaseBatch(Batch, true);
			}
			else
			{
				for (int32 k = 0; k < Batch.EntryIdx.Num(); ++k)
				{
					const int32 EntryIndex = Batch.EntryIdx[k];
					const uint8 Tag = Batch.Tags[k];
					AActor* Actor = Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].Actor.Get() : nullptr;
					FString Detail;
					if (Actor && AnomalyStencilTag::VerifyActorStillTagged(Actor, (int32)Tag, Detail))
					{
						CreditEntryFromResult(EntryIndex, Tag, Result);
					}
				}
				ReleaseBatch(Batch, false);
			}
			InFlight.RemoveAt(i);
		}
		else if (GFrameCounter > Batch.ArmedAtTick + (uint64)LostAfterTicks)
		{
			++Counters.BatchesLost;
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Census: BATCH LOST id=%llu armedAtTick=%llu (+%d ticks, no readback) - tags released, ")
				TEXT("candidates re-queued, no verdict changed. Expected count on this bench: ZERO."),
				Batch.RequestId, Batch.ArmedAtTick, LostAfterTicks);
			ReleaseBatch(Batch, true);
			InFlight.RemoveAt(i);
		}
	}
}

void FAnomalyCensus::ArmNextBatch(FAnomalyMaskSceneViewExtension* Sve, const TSet<uint8>& EventTags)
{
	if (InFlight.Num() >= MaxInFlightBatches || !Ledger)
	{
		return;
	}

	FBatch Batch;
	Batch.RequestId = GCensusRequestBit | (++CensusIdSerial);
	Batch.ArmedAtTick = GFrameCounter;

	int32 TotalFlips = 0;
	int32 Cursor = AnomalyStencilTag::ReservedStencilBase;

	while (Batch.EntryIdx.Num() < HalfCap && CycleQueue.Num() > 0)
	{
		uint8 Tag = 0;
		bool bFound = false;
		for (; Cursor <= AnomalyStencilTag::AssignableStencilMax; ++Cursor)
		{
			if (Ledger->IsFree((uint8)Cursor))
			{
				Tag = (uint8)Cursor;
				++Cursor;
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			break;
		}

		const int32 EntryIndex = CycleQueue[0];
		CycleQueue.RemoveAt(0);
		if (!Entries.IsValidIndex(EntryIndex))
		{
			continue;
		}
		FAnomalyCensusEntry& Entry = Entries[EntryIndex];
		++Entry.AttemptsThisCycle;

		AActor* Actor = Entry.Actor.Get();
		if (!Actor)
		{
			continue;
		}
		if (Actor->IsHidden())
		{
			Entry.Verdict = EAnomalyCensusVerdict::NotMeasurableHidden;
			continue;
		}
		if (AnomalyStencilTag::IsAnyComponentTagged(Actor))
		{
			continue;
		}

		int32 Flips = 0;
		const int32 Tagged = AnomalyStencilTag::TagActor(Actor, (int32)Tag, &Flips);
		if (Tagged <= 0)
		{
			Entry.Verdict = EAnomalyCensusVerdict::NotMeasurableTagFailed;
			continue;
		}

		TotalFlips += Flips;
		Ledger->CensusClaimed.Add(Tag);
		Batch.EntryIdx.Add(EntryIndex);
		Batch.Tags.Add(Tag);
	}

	if (Batch.EntryIdx.Num() == 0)
	{
		return;
	}

	Batch.CensusTagsAtArm = Ledger->CensusClaimed;

	Counters.ProxyRecreatesQueued += TotalFlips;
	Batch.PendingBefore = Sve->NumPendingArms();

	TSet<uint8> Assigned = EventTags;
	Assigned.Append(GetLegitTags());
	for (uint8 Tag : Batch.Tags)
	{
		Assigned.Add(Tag);
	}
	Sve->SetAssignedTags(Assigned);
	Sve->ArmMask(Batch.RequestId);

	++Counters.CensusFrames;
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Census: ARM cycle=%d batch id=%llu size=%d tags=%d..%d pendingBefore=%d flagFlips=%d queueLeft=%d"),
		CycleNumber, Batch.RequestId, Batch.EntryIdx.Num(),
		(int32)Batch.Tags[0], (int32)Batch.Tags.Last(), Batch.PendingBefore, TotalFlips, CycleQueue.Num());

	InFlight.Add(MoveTemp(Batch));
}

void FAnomalyCensus::CloseCycle()
{
	bCycleOpen = false;
	++Counters.Cycles;

	int32 Zero = 0, NonZero = 0, BelowFloor = 0, AboveCeiling = 0, Translucent = 0, Nanite = 0, TagFailed = 0, Hidden = 0, NotYet = 0;
	TArray<const FAnomalyCensusEntry*> Measured;
	for (const FAnomalyCensusEntry& Entry : Entries)
	{
		switch (Entry.Verdict)
		{
		case EAnomalyCensusVerdict::MeasuredZero:
			++Zero;
			Measured.Add(&Entry);
			break;
		case EAnomalyCensusVerdict::MeasuredNonZero:
		{
			++NonZero;
			Measured.Add(&Entry);
			const double Pct = (Entry.FramePx > 0) ? (100.0 * (double)Entry.DrawnPx / (double)Entry.FramePx) : 0.0;
			if (Pct < (double)Params.FloorPct)
			{
				++BelowFloor;
			}
			else if (IsCeilingEnabled() && Pct > (double)Params.CeilingPct)
			{
				++AboveCeiling;
				UE_LOG(LogAnomalyCapture, Log,
					TEXT("Census: ABOVE-CEILING '%s' drawn=%dpx (%.3f%%) > ceiling %.2f%% - EXCLUDED (label ")
					TEXT("unusable at scenery scale, not a failed anomaly)."),
					*Entry.ActorName, Entry.DrawnPx, Pct, Params.CeilingPct);
			}
			break;
		}
		case EAnomalyCensusVerdict::ExcludedTranslucent:    ++Translucent; break;
		case EAnomalyCensusVerdict::NotMeasurableNanite:    ++Nanite; break;
		case EAnomalyCensusVerdict::NotMeasurableTagFailed: ++TagFailed; break;
		case EAnomalyCensusVerdict::NotMeasurableHidden:    ++Hidden; break;
		default:                                            ++NotYet; break;
		}
	}

	Counters.Candidates = Entries.Num();
	Counters.Zero = Zero;
	Counters.NonZero = NonZero;
	Counters.BelowFloor = BelowFloor;
	Counters.AboveCeiling = AboveCeiling;
	Counters.ExcludedTranslucent = Translucent;
	Counters.UnmeasurableNanite = Nanite;
	Counters.UnmeasurableTagFailed = TagFailed;
	Counters.UnmeasurableHidden = Hidden;
	Counters.NotYetMeasured = NotYet;

	const uint64 CycleTicks = GFrameCounter - CycleStartTick;
	const double CycleMs = Counters.TagBlockMsTotal - CycleStartTagBlockMs;
	const int32 CycleFlips = Counters.ProxyRecreatesQueued - CycleStartFlips;

	const int32 WindowBefore = GetFreshnessWindowTicks();
	LastCompletedCycleTicks = CycleTicks;
	const int32 WindowAfter = GetFreshnessWindowTicks();

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Census: CYCLE %d DONE ticks=%llu window=%d(was %d) candidates=%d zero=%d nonzero=%d ")
		TEXT("belowFloor=%d(floor %.2f%%) ")
		TEXT("aboveCeiling=%d(ceiling %s) excludedTranslucent=%d nanite=%d tagFailed=%d hidden=%d ")
		TEXT("notYetMeasured=%d"),
		CycleNumber, CycleTicks, WindowAfter, WindowBefore, Entries.Num(), Zero, NonZero,
		BelowFloor, Params.FloorPct,
		AboveCeiling,
		IsCeilingEnabled() ? *FString::Printf(TEXT("%.2f%%"), Params.CeilingPct) : TEXT("DISABLED"),
		Translucent, Nanite, TagFailed, Hidden, NotYet);

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Census: CYCLE %d COST tagBlockMs=%.4f overTicks=%llu perTickMs=%.4f flagFlips=%d ")
		TEXT("(GAME-THREAD QUEUING COST ONLY - SetRenderCustomDepth only QUEUES the proxy recreate; ")
		TEXT("the destroy/create runs later inside SendAllEndOfFrameUpdates, OUTSIDE this timed block. ")
		TEXT("flagFlips is the SIZE of the excluded work, never its price.)"),
		CycleNumber, CycleMs, CycleTicks,
		(CycleTicks > 0) ? (CycleMs / (double)CycleTicks) : 0.0, CycleFlips);

	Measured.Sort([](const FAnomalyCensusEntry& A, const FAnomalyCensusEntry& B)
	{
		return A.DrawnPx > B.DrawnPx;
	});

	int32 Buckets[7] = {};
	FString Rows;
	int32 Listed = 0;
	for (const FAnomalyCensusEntry* Entry : Measured)
	{
		const double Pct = (Entry->FramePx > 0) ? (100.0 * (double)Entry->DrawnPx / (double)Entry->FramePx) : 0.0;
		int32 Bucket = 0;
		if (Pct <= 0.0) { Bucket = 0; }
		else if (Pct <= 1.0) { Bucket = 1; }
		else if (Pct <= 3.0) { Bucket = 2; }
		else if (Pct <= 6.0) { Bucket = 3; }
		else if (Pct <= 12.0) { Bucket = 4; }
		else if (Pct <= 25.0) { Bucket = 5; }
		else { Bucket = 6; }
		++Buckets[Bucket];
		if (Listed < CycleListingCap)
		{
			Rows += FString::Printf(TEXT(" %s=%d(%.3f%%)"), *Entry->ActorName, Entry->DrawnPx, Pct);
			++Listed;
		}
	}
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Census: CYCLE %d DRAWN-COVERAGE histogram zero=%d (0,1]=%d (1,3]=%d (3,6]=%d (6,12]=%d ")
		TEXT("(12,25]=%d >25=%d |%s%s"),
		CycleNumber, Buckets[0], Buckets[1], Buckets[2], Buckets[3], Buckets[4], Buckets[5], Buckets[6],
		*Rows, (Measured.Num() > Listed) ? *FString::Printf(TEXT(" (+%d more)"), Measured.Num() - Listed) : TEXT(""));

	if (IsCeilingEnabled())
	{
		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Census: CYCLE %d HISTOGRAM NOTE - the '>25' bucket edge and the DEFAULT ceiling coincide at ")
			TEXT("25, so at ceiling %.2f%% that bucket is the ceiling's population only when the ceiling IS 25. ")
			TEXT("aboveCeiling=%d is what the ceiling ACTUALLY excluded and is the number to read; the bucket is ")
			TEXT("the distribution."),
			CycleNumber, Params.CeilingPct, AboveCeiling);
	}

	FString NotMeasuredRows;
	int32 NotMeasuredListed = 0;
	int32 NotMeasuredTotal = 0;
	for (const FAnomalyCensusEntry& Entry : Entries)
	{
		if (Entry.Verdict == EAnomalyCensusVerdict::MeasuredZero
			|| Entry.Verdict == EAnomalyCensusVerdict::MeasuredNonZero)
		{
			continue;
		}
		++NotMeasuredTotal;
		if (NotMeasuredListed < CycleListingCap)
		{
			NotMeasuredRows += FString::Printf(TEXT(" %s=%s(measured %dx)"),
				*Entry.ActorName, LexToStringAnomalyCensusVerdict(Entry.Verdict), Entry.TimesMeasured);
			++NotMeasuredListed;
		}
	}
	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Census: CYCLE %d NOT-MEASURED n=%d |%s%s"),
		CycleNumber, NotMeasuredTotal, NotMeasuredRows.IsEmpty() ? TEXT(" (none)") : *NotMeasuredRows,
		(NotMeasuredTotal > NotMeasuredListed)
			? *FString::Printf(TEXT(" (+%d more)"), NotMeasuredTotal - NotMeasuredListed) : TEXT(""));
}

#endif
