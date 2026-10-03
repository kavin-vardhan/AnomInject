#include "AnomalyChangeStage.h"
#include "Dom/JsonValue.h"
#if ANOMALY_CAPTURE
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"

namespace
{
struct FChangeStats
{
	int64 N = 0, Gt8 = 0, Sum = 0, Hist[8] = {};
	void Add(int32 D)
	{
		++N; Sum += D; Gt8 += D > 8;
		const int32 Bin = D == 0 ? 0 : D <= 2 ? 1 : D <= 4 ? 2 : D <= 8 ? 3 : D <= 16 ? 4 : D <= 32 ? 5 : D <= 64 ? 6 : 7;
		++Hist[Bin];
	}
	double Mean() const { return N ? FMath::RoundToDouble((double)Sum / N / 255.0 * 10000.0) / 10000.0 : 0; }
};
int32 Difference(const FColor& A, const FColor& B)
{
	return FMath::Max3(FMath::Abs((int32)A.R - B.R), FMath::Abs((int32)A.G - B.G), FMath::Abs((int32)A.B - B.B));
}
void StatsJson(const TSharedRef<FJsonObject>& J, const TCHAR* Prefix, const FChangeStats& S, bool bMeasured)
{
	const FString P(Prefix);
	J->SetNumberField(P + TEXT("_n"), bMeasured ? S.N : -1);
	J->SetNumberField(P + TEXT("_gt8"), bMeasured ? S.Gt8 : -1);
	J->SetNumberField(P + TEXT("_sum"), bMeasured ? S.Sum : -1);
	if (bMeasured)
	{
		TArray<TSharedPtr<FJsonValue>> Bins;
		for (int64 V : S.Hist) { Bins.Add(MakeShared<FJsonValueNumber>((double)V)); }
		J->SetArrayField(P + TEXT("_hist"), Bins); J->SetNumberField(P + TEXT("_mean"), S.Mean());
	}
	else
	{
		J->SetField(P + TEXT("_hist"), MakeShared<FJsonValueNull>());
		J->SetField(P + TEXT("_mean"), MakeShared<FJsonValueNull>());
	}
}
}

void FAnomalyChangeStage::RetainColourLocked(const FAnomalyChangeColourPtr& Pixels)
{
	check(Pixels.IsValid() && ColourOwners.Contains(Pixels.Get()));
	++ColourOwners.FindChecked(Pixels.Get());
}
void FAnomalyChangeStage::ReleaseColourLocked(FAnomalyChangeColourPtr& Pixels)
{
	if (!Pixels.IsValid()) { return; }
	int32& Owners = ColourOwners.FindChecked(Pixels.Get());
	if (--Owners == 0)
	{
		BytesHeld -= (int64)Pixels->GetAllocatedSize();
		ColourOwners.Remove(Pixels.Get());
	}
	Pixels.Reset();
}
void FAnomalyChangeStage::AddRowLocked(const TSharedRef<FJsonObject>& J)
{
	if (Rows.Num() >= 100000) { ++Counters.FindOrAdd(TEXT("rows_dropped")); return; }
	FString Line; auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Line);
	FJsonSerializer::Serialize(J, Writer); Rows.Add(MoveTemp(Line));
}
void FAnomalyChangeStage::ClosePhaseLocked(FPhase& Phase, const TCHAR* Cause)
{
	if (Phase.bClosing || Phase.bFinal) { return; }
	Phase.bClosing = true; Phase.Deadline = FPlatformTime::Seconds() + 5.0;
	Phase.Cause = Cause;
}
void FAnomalyChangeStage::Observe(int32 SI, const TArray<FAnomalyChangeLabel>& Labels)
{
	FScopeLock Lock(&CS);
	FPending* Item = Pending.Find(SI);
	if (!Item || bClosed || Item->bObserved) { return; }
	Item->bObserved = true;
	TSet<FString> Seen;
	for (FAnomalyChangeLabel Label : Labels)
	{
		Seen.Add(Label.Event);
		FEvent& Event = Events.FindOrAdd(Label.Event);
		if (Event.bClosing || Event.bFinal) { ++Counters.FindOrAdd(TEXT("late_event_observations")); continue; }
		Event.Type = Label.Type; Event.Target = Label.Target; Event.LastSeen = SI;
		if (Event.ActivePhase >= 0 && (!Label.bLabelled || Event.Phases[Event.ActivePhase].Last != SI - 1))
		{
			ClosePhaseLocked(Event.Phases[Event.ActivePhase], TEXT("phase_end")); Event.ActivePhase = -1;
		}
		if (Label.bLabelled)
		{
			if (Event.ActivePhase < 0)
			{
				Event.ActivePhase = Event.Phases.AddDefaulted();
				Event.Phases.Last().First = SI;
			}
			FPhase& Phase = Event.Phases[Event.ActivePhase];
			Label.Phase = Event.ActivePhase; Label.Window = Phase.Labelled++;
			Phase.Last = SI; Phase.Required = FMath::Min(4, Phase.Labelled);
		}
		Item->Labels.Add(MoveTemp(Label));
	}
	for (auto& KV : Events)
	{
		FEvent& Event = KV.Value;
		if (!Seen.Contains(KV.Key) && !Event.bClosing && !Event.bFinal)
		{
			Event.bClosing = true; Event.Cause = TEXT("fire_removed_or_target_lost");
			for (FPhase& Phase : Event.Phases) { ClosePhaseLocked(Phase, *Event.Cause); }
		}
	}
	if (!DeferredEndCause.IsEmpty() && SI >= DeferredEndAt)
	{
		EndEventsLocked(*DeferredEndCause); DeferredEndCause.Reset(); DeferredEndAt = -1;
	}
	ScheduleLocked();
}
void FAnomalyChangeStage::EndEventsLocked(const TCHAR* Cause)
{
	for (auto& KV : Events)
	{
		FEvent& Event = KV.Value;
		if (Event.bClosing || Event.bFinal) { continue; }
		Event.bClosing = true; Event.Cause = Cause;
		for (FPhase& Phase : Event.Phases) { ClosePhaseLocked(Phase, Cause); }
	}
}
void FAnomalyChangeStage::EndEvents(const TCHAR* Cause)
{
	FScopeLock Lock(&CS);
	const FPending* Last = Pending.Find(LatestIndex);
	if (Last && !Last->bObserved) { DeferredEndCause = Cause; DeferredEndAt = LatestIndex; }
	else { EndEventsLocked(Cause); }
	ScheduleLocked();
}
void FAnomalyChangeStage::FinalizeEventsLocked()
{
	for (auto& KV : Events)
	{
		FEvent& Event = KV.Value;
		if (Event.bFinal) { continue; }
		bool bAllFinal = true;
		for (FPhase& Phase : Event.Phases)
		{
			if (!Phase.bFinal && Phase.bClosing && Cursor > Phase.Last)
			{
				if (!Phase.bOnsetMeasured && Phase.OnsetReason == EAnomalyChangeReason::None)
				{
					Phase.OnsetReason = EAnomalyChangeReason::ClosureTimeout;
				}
				Phase.bFinal = true; ReleaseColourLocked(Phase.Reference); Phase.ReferenceReceipt.Reset();
				++Counters.FindOrAdd(Phase.bOnsetMeasured ? TEXT("phases_measured") : TEXT("phases_indeterminate"));
			}
			bAllFinal &= Phase.bFinal;
		}
		if (!Event.bClosing || !bAllFinal || Cursor <= Event.LastSeen) { continue; }
		auto J = MakeShared<FJsonObject>();
		J->SetStringField(TEXT("kind"), TEXT("event")); J->SetNumberField(TEXT("stage_version"), 2);
		J->SetStringField(TEXT("event"), KV.Key); J->SetStringField(TEXT("anomaly_type"), Event.Type);
		J->SetStringField(TEXT("target"), Event.Target); J->SetStringField(TEXT("finalized_by"), Event.Cause);
		J->SetNumberField(TEXT("phase_count"), Event.Phases.Num());
		if (Event.Phases.IsEmpty()) { J->SetStringField(TEXT("reason"), TEXT("no_labelled_frames")); ++Counters.FindOrAdd(TEXT("reason_no_labelled_frames")); }
		else { J->SetField(TEXT("reason"), MakeShared<FJsonValueNull>()); ++Counters.FindOrAdd(TEXT("events_with_phases")); }
		TArray<TSharedPtr<FJsonValue>> Phases;
		for (int32 I = 0; I < FMath::Min(8, Event.Phases.Num()); ++I)
		{
			const FPhase& P = Event.Phases[I]; auto V = MakeShared<FJsonObject>();
			V->SetNumberField(TEXT("ordinal"), I); V->SetStringField(TEXT("state"), P.bOnsetMeasured ? TEXT("measured") : TEXT("indeterminate"));
			V->SetNumberField(TEXT("first_labelled_index"), P.First); V->SetNumberField(TEXT("onset_prev_index"), P.First - 1);
			V->SetNumberField(TEXT("last_labelled_index"), P.Last); V->SetNumberField(TEXT("closure_watermark"), P.Last);
			V->SetStringField(TEXT("finalized_by"), P.Cause);
			V->SetNumberField(TEXT("pairs_required"), P.Required); V->SetNumberField(TEXT("pairs_measured"), P.Measured);
			const TCHAR* Reason = AnomalyChangeReasonName(P.OnsetReason);
			if (Reason) { V->SetStringField(TEXT("reason"), Reason); } else { V->SetField(TEXT("reason"), MakeShared<FJsonValueNull>()); }
			auto Reasons = MakeShared<FJsonObject>(); for (const auto& R : P.Reasons) { Reasons->SetNumberField(R.Key, R.Value); }
			V->SetObjectField(TEXT("reasons"), Reasons); V->SetNumberField(TEXT("chg_gt8_max"), P.MaxGt8);
			V->SetNumberField(TEXT("chg_mean_max"), P.MaxMean); V->SetNumberField(TEXT("ref_gt8_max"), P.MaxRef);
			Phases.Add(MakeShared<FJsonValueObject>(V));
		}
		J->SetArrayField(TEXT("phases"), Phases); AddRowLocked(J); Event.bFinal = true;
	}
}

void FAnomalyChangeStage::MeasureLocked(FPending& Item, EAnomalyChangeReason Reason, const TSharedRef<FJsonObject>& Base)
{
	struct FWindow { FAnomalyChangeLabel Label; FAnomalyChangeColourPtr Ref; FAnomalyChangeReceiptPtr Receipt; FChangeStats RefStats; };
	TArray<FWindow> Windows;
	for (const FAnomalyChangeLabel& Label : Item.Labels)
	{
		if (!Label.bLabelled || Label.Window < 0 || Label.Window >= 4) { continue; }
		FEvent* Event = Events.Find(Label.Event);
		if (!Event || Event->bFinal || !Event->Phases.IsValidIndex(Label.Phase) || Event->Phases[Label.Phase].bFinal)
		{
			++Counters.FindOrAdd(TEXT("late_results")); continue;
		}
		FPhase& Phase = Event->Phases[Label.Phase];
		if (Label.Window == 0 && Previous.Pixels.IsValid() && Previous.bColourDelivered &&
			Previous.Issued.IsValid() && Previous.Issued->SessionIndex == Phase.First - 1 &&
			Previous.ColourReceipt.IsValid() && Item.ColourReceipt.IsValid() &&
			Previous.ColourReceipt->ServingToken == Previous.Issued->CaptureToken && Previous.ColourReceipt->ViewIndex == 0 &&
			Previous.Issued->RunEpoch == Item.Issued->RunEpoch && Previous.Issued->CutCounter == Item.Issued->CutCounter &&
			Previous.ColourReceipt->Rect == Item.ColourReceipt->Rect && Previous.ColourReceipt->Extent == Item.ColourReceipt->Extent &&
			Previous.ColourReceipt->Format == Item.ColourReceipt->Format)
		{
			Phase.Reference = Previous.Pixels; Phase.ReferenceReceipt = Previous.ColourReceipt; RetainColourLocked(Phase.Reference);
		}
		const bool RefGeometry = Phase.ReferenceReceipt.IsValid() && Item.ColourReceipt.IsValid() &&
			Phase.ReferenceReceipt->Rect == Item.ColourReceipt->Rect && Phase.ReferenceReceipt->Extent == Item.ColourReceipt->Extent &&
			Phase.ReferenceReceipt->Format == Item.ColourReceipt->Format &&
			Phase.ReferenceReceipt->Issue->RunEpoch == Item.Issued->RunEpoch && Phase.ReferenceReceipt->Issue->CutCounter == Item.Issued->CutCounter;
		Windows.Add({Label, RefGeometry ? Phase.Reference : nullptr, RefGeometry ? Phase.ReferenceReceipt : nullptr, {}});
	}
	if (Windows.IsEmpty()) { return; }
	FChangeStats Stats[256];
	const bool bIdentity = Reason == EAnomalyChangeReason::None || Reason == EAnomalyChangeReason::EmptyRegion;
	const bool bArrays = Item.Pixels.IsValid() && Previous.Pixels.IsValid() && Item.MaskPixels.IsValid() &&
		Item.Pixels->Num() == Previous.Pixels->Num() && Item.Pixels->Num() == Item.MaskPixels->Num();
	if (bIdentity && bArrays)
	{
		CS.Unlock();
		for (int32 I = 0; I < Item.Pixels->Num(); ++I)
		{
			const uint8 Tag = (*Item.MaskPixels)[I];
			Stats[Tag].Add(Difference((*Item.Pixels)[I], (*Previous.Pixels)[I]));
			for (FWindow& W : Windows)
			{
				if (Tag == W.Label.Tag && Tag != 0 && W.Ref.IsValid() && W.Ref->Num() == Item.Pixels->Num())
				{
					W.RefStats.Add(Difference((*Item.Pixels)[I], (*W.Ref)[I]));
				}
			}
		}
		CS.Lock();
	}
	else if (bIdentity) { Reason = EAnomalyChangeReason::ExtentMismatch; }
	for (FWindow& W : Windows)
	{
		FPhase& Phase = Events.FindChecked(W.Label.Event).Phases[W.Label.Phase];
		const FChangeStats& Target = Stats[FMath::Clamp(W.Label.Tag, 0, 255)];
		EAnomalyChangeReason RowReason = Reason;
		if (bIdentity && bArrays && (W.Label.Tag <= 0 || Target.N == 0 || Stats[0].N == 0)) { RowReason = EAnomalyChangeReason::EmptyRegion; }
		const bool bMeasured = RowReason == EAnomalyChangeReason::None;
		const TCHAR* R = AnomalyChangeReasonName(RowReason);
		auto J = MakeShared<FJsonObject>(*Base);
		J->SetStringField(TEXT("event"), W.Label.Event); J->SetNumberField(TEXT("phase_ordinal"), W.Label.Phase);
		J->SetNumberField(TEXT("window_index"), W.Label.Window); J->SetNumberField(TEXT("mask_value"), W.Label.Tag);
		J->SetBoolField(TEXT("chg_measured"), bMeasured); J->SetBoolField(TEXT("chg_eligible"), bMeasured);
		J->SetBoolField(TEXT("pair_valid"), bMeasured); J->SetNumberField(TEXT("tau_px"), 8);
		if (R) { J->SetStringField(TEXT("reason"), R); ++Phase.Reasons.FindOrAdd(R); }
		else { J->SetField(TEXT("reason"), MakeShared<FJsonValueNull>()); }
		StatsJson(J, TEXT("chg"), Target, bMeasured);
		StatsJson(J, TEXT("ctl"), Stats[0], bIdentity && bArrays && Stats[0].N > 0);
		const bool bRef = bMeasured && W.Ref.IsValid() && W.RefStats.N == Target.N;
		J->SetNumberField(TEXT("ref_session_index"), W.Receipt.IsValid() ? W.Receipt->Issue->SessionIndex : -1);
		if (bRef) { J->SetNumberField(TEXT("ref_gt8"), W.RefStats.Gt8); J->SetNumberField(TEXT("ref_mean"), W.RefStats.Mean()); }
		else { J->SetField(TEXT("ref_gt8"), MakeShared<FJsonValueNull>()); J->SetField(TEXT("ref_mean"), MakeShared<FJsonValueNull>()); }
		int32 PrevCount = -1;
		if (W.Label.Window > 0 && Previous.Issued.IsValid() && Previous.Issued->SessionIndex == Item.Issued->SessionIndex - 1)
		{
			for (const auto& L : Previous.Labels)
			{
				if (L.Event == W.Label.Event && L.bLabelled && L.Phase == W.Label.Phase)
				{
					if (const int32* N = Previous.Counts.Find((uint8)L.Tag)) { PrevCount = *N; }
				}
			}
		}
		J->SetNumberField(TEXT("prev_target_pixels"), PrevCount);
		if (W.Label.Window == 0) { Phase.bOnsetMeasured = bMeasured; Phase.OnsetReason = RowReason; }
		++Counters.FindOrAdd(bMeasured ? TEXT("pairs_measured") : TEXT("measurement_pairs_refused"));
		if (bMeasured)
		{
			++Phase.Measured; Phase.MaxGt8 = FMath::Max(Phase.MaxGt8, (int32)Target.Gt8); Phase.MaxMean = FMath::Max(Phase.MaxMean, Target.Mean());
			const int32* Count = Item.Counts.Find((uint8)W.Label.Tag);
			if (!Count || *Count != Target.N) { ++Counters.FindOrAdd(TEXT("denominator_mismatch")); }
			int64 Total = 0; for (int64 V : Target.Hist) { Total += V; }
			if (Total != Target.N || Target.Gt8 != Target.Hist[4] + Target.Hist[5] + Target.Hist[6] + Target.Hist[7]) { ++Counters.FindOrAdd(TEXT("histogram_mismatch")); }
			if (bRef) { Phase.MaxRef = FMath::Max(Phase.MaxRef, (int32)W.RefStats.Gt8); }
			if (W.Label.Window == 0 && (!bRef || W.RefStats.Gt8 != Target.Gt8 || W.RefStats.Mean() != Target.Mean())) { ++Counters.FindOrAdd(TEXT("ref_onset_mismatch")); }
		}
		J->SetNumberField(TEXT("chg_gt8_max_sofar"), Phase.MaxGt8); J->SetNumberField(TEXT("chg_mean_max_sofar"), Phase.MaxMean);
		AddRowLocked(J);
		if (W.Label.Window == 3) { ReleaseColourLocked(Phase.Reference); Phase.ReferenceReceipt.Reset(); }
	}
}
#endif
