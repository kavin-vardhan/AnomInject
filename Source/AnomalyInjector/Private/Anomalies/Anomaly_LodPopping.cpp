#include "Anomalies/Anomaly_LodPopping.h"

#include "AnomalyLod.h"
#include "AnomalyLodTrigger.h"
#include "AnomalyBenchGate.h"
#include "AnomalyViewport.h"
#include "AnomalyDefaults.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "Components/MeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "HAL/IConsoleManager.h"
#include "Engine/World.h"

namespace
{
	bool GLodReleaseLag = false;
	bool GLodForceTrigger = false;
	bool GLodSkipOnScreen = false;
	bool GLodPredictStaleView = false;
	bool GLodSkipRestore = false;
}

bool FAnomaly_LodPopping::ApplyPeriodic(UWorld* World, const TArray<FString>& Args)
{
	LastRefusal.Reset();
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("lod_popping: usage <substring> periodic [half_period_frames]"));
		return false;
	}

	static_assert(DefaultHalfPeriodFrames == AnomalyDefaults::LodPoppingHalfPeriodCompiled,
		"lod_popping's compiled half-period and AnomalyDefaults::LodPoppingHalfPeriodCompiled have drifted apart - "
		"the capture run-config echo reads the AnomalyDefaults copy, so a silent disagreement would print one "
		"number while the anomaly used another");
	static_assert(MinHalfPeriodFrames == AnomalyDefaults::HalfPeriodMin
		&& MaxHalfPeriodFrames == AnomalyDefaults::HalfPeriodMax,
		"lod_popping's half-period clamp and AnomalyDefaults' clamp have drifted apart");

	const int32 EffectiveDefault = AnomalyDefaults::GetHalfPeriodFrames(
		AnomalyDefaults::LodPoppingHalfPeriodKey(), DefaultHalfPeriodFrames,
		MinHalfPeriodFrames, MaxHalfPeriodFrames, TEXT("lod_popping"));

	int32 Frames = EffectiveDefault;
	if (Args.Num() >= 2)
	{
		if (Args[1].IsNumeric())
		{
			Frames = FCString::Atoi(*Args[1]);
		}
		else
		{
			UE_LOG(LogAnomaly, Warning, TEXT("lod_popping: '%s' is not a whole number of frames; using %d."),
				*Args[1], EffectiveDefault);
			Frames = EffectiveDefault;
		}
	}
	if (Frames < MinHalfPeriodFrames || Frames > MaxHalfPeriodFrames)
	{
		UE_LOG(LogAnomaly, Warning, TEXT("lod_popping: half-period %d out of range [%d..%d]; using %d."),
			Frames, MinHalfPeriodFrames, MaxHalfPeriodFrames, EffectiveDefault);
		Frames = EffectiveDefault;
	}
	HalfPeriodFrames = Frames;

	const FString& Substring = Args[0];

	TArray<TWeakObjectPtr<UMeshComponent>> Meshes = AnomalyLod::ResolveLodComponents(World, Substring);
	if (UAnomalyInjectorSubsystem::IsViewportScopingEnabled(World))
	{
		FAnomalyViewInfo View;
		if (AnomalyViewport::GetActiveViewInfo(World, View))
		{
			Meshes = AnomalyViewport::FilterVisibleComponents(View, World, Meshes);
		}
	}
	if (Meshes.Num() == 0)
	{
		UE_LOG(LogAnomaly, Log, TEXT("lod_popping: matched 0 mesh component(s) for '%s'."), *Substring);
		return false;
	}

	static_assert(MinCoveragePct == AnomalyDefaults::LodPoppingMinCoverageCompiled,
		"lod_popping's compiled screen-coverage minimum and AnomalyDefaults::LodPoppingMinCoverageCompiled have "
		"drifted apart - the m30 calibration is one number and it must have one home");

	const bool bAutoPool = UAnomalyInjectorSubsystem::IsAutoPoolSelection(World);
	const float MaxDistanceCm = bAutoPool ? AnomalyDefaults::GetLodPoppingMaxDistanceCm() : 0.0f;
	const float EffectiveMinCoveragePct = bAutoPool ? AnomalyDefaults::GetLodPoppingMinCoveragePct() : 0.0f;
	const bool bRequireHighestLod = bAutoPool && AnomalyDefaults::GetLodPoppingRequireHighestLod();

	if (!bAutoPool)
	{
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: TARGETED FIRE on '%s' — the %.4f%% screen-coverage gate and the %.2f cm proximity gate are ")
			TEXT("BYPASSED because they govern AUTO-POOL SELECTION only. An explicitly named object fires regardless of ")
			TEXT("distance or on-screen size: choosing the target is the operator's decision. LABELLING IS UNCHANGED and ")
			TEXT("still discriminates per frame, so if the pop is not visible the event carries zero positive frames and ")
			TEXT("the m23 F-LABEL guard reports it. The single-LOD guard still applies to both paths - it is not a ")
			TEXT("proximity gate, and forcing a LOD on a single-LOD mesh pops it to itself whoever picked it."),
			*Substring, MinCoveragePct, AnomalyDefaults::GetLodPoppingMaxDistanceCm());
	}

	Targets.Reset();
	int32 RefusedHism = 0;
	int32 RefusedSingleLod = 0;
	int32 RefusedSkeletal = 0;
	int32 RefusedTooSmall = 0;
	int32 RefusedTooFar = 0;
	int32 RefusedNotHighestLod = 0;
	TMap<const AActor*, float> CoverageByOwner;
	TMap<const AActor*, float> DistanceByOwner;
	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		UMeshComponent* Mesh = Weak.Get();
		if (!Mesh)
		{
			continue;
		}

		const AnomalyLodPolicy::EForcedLodRefusal Refusal = AnomalyLod::GetForcedLodRefusal(Mesh);
		if (Refusal == AnomalyLodPolicy::EForcedLodRefusal::HierarchicalInstanced)
		{
			++RefusedHism;
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: REFUSED '%s' (hism_ignores_forced_lod) — it is a hierarchical instanced static mesh, which picks its own LODs and ignores a forced LOD, so forcing one changes nothing: a positive label with no visible change, which the mask veto cannot catch (it still draws pixels). This applies to auto-pool and targeted fires alike."),
				*Mesh->GetName());
			continue;
		}
		if (Refusal == AnomalyLodPolicy::EForcedLodRefusal::SkeletalUnmeasured)
		{
			++RefusedSkeletal;
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: REFUSED '%s' (skeletal_unmeasured) — it is a skeletal mesh, which applies a forced LOD one frame late, so the labels at pop edges may be one frame off the picture; that is unmeasured, so it is refused unless IAI.Anomaly.LodAllowSkeletal is 1. This applies to auto-pool and targeted fires alike."),
				*Mesh->GetName());
			continue;
		}
		if (Refusal == AnomalyLodPolicy::EForcedLodRefusal::SingleLod)
		{
			++RefusedSingleLod;
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: REFUSED '%s' (single_lod) — it has at most one usable LOD [%s] (MinLOD and LODs stripped of vertices are not usable; the engine clamps a forced LOD to the usable range), so forcing a LOD would pop it to itself: no visible change, and a positive label with no visible change is an invisible anomaly the mask veto cannot catch (it still draws pixels)."),
				*Mesh->GetName(), *AnomalyLod::DescribeLodFacts(AnomalyLod::GetLodFacts(Mesh)));
			continue;
		}

		const AActor* Owner = Mesh->GetOwner();
		float Coverage = -1.0f;
		if (const float* Cached = CoverageByOwner.Find(Owner))
		{
			Coverage = *Cached;
		}
		else
		{
			Coverage = AnomalyViewport::GetActorScreenCoveragePct(World, Owner);
			CoverageByOwner.Add(Owner, Coverage);
			UE_LOG(LogAnomaly, Log,
				TEXT("lod_popping: COVERAGE '%s' bounds_coverage_pct=%.4f (threshold %.4f, %s) — bounds-projected screen coverage at PICK TIME, the same quantity the gate tests; NOT drawn extent, which is smaller."),
				*GetNameSafe(Owner), Coverage, MinCoveragePct,
				bAutoPool ? TEXT("ENFORCED, auto-pool selection") : TEXT("BYPASSED, targeted fire"));
		}

		if (MaxDistanceCm > 0.0f)
		{
			float Distance = 0.0f;
			if (const float* CachedD = DistanceByOwner.Find(Owner))
			{
				Distance = *CachedD;
			}
			else
			{
				Distance = AnomalyViewport::GetActorPollDistanceCm(World, Owner);
				DistanceByOwner.Add(Owner, Distance);
				UE_LOG(LogAnomaly, Log,
					TEXT("lod_popping: DISTANCE '%s' poll_distance_cm=%.2f (max %.2f) — the SAME metric as the poll radius (sphere-approx bounds distance from the poll origin); negative means the bounds sphere already contains the poll origin."),
					*GetNameSafe(Owner), Distance, MaxDistanceCm);
			}

			if (Distance > MaxDistanceCm)
			{
				++RefusedTooFar;
				UE_LOG(LogAnomaly, Warning,
					TEXT("lod_popping: REFUSED '%s' — poll_distance_cm %.2f exceeds the %.2f cm maximum. This gate is an OWNER PRODUCT PREFERENCE (pop only what the player is right next to), it ANDs with the %.4f%% coverage gate rather than replacing it, and it governs AUTO-POOL SELECTION only; a targeted fire on a named object is not subject to it."),
					*Mesh->GetName(), Distance, MaxDistanceCm, MinCoveragePct);
				continue;
			}
		}

		if (EffectiveMinCoveragePct > 0.0f && Coverage < EffectiveMinCoveragePct)
		{
			++RefusedTooSmall;
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: REFUSED '%s' — bounds_coverage_pct %.4f is below the %.4f minimum, so at this on-screen size its LODs do not differ visibly: forcing one would pop it to itself. Same failure as a single-LOD mesh, and the mask veto cannot catch it either (it still draws pixels). This gate governs AUTO-POOL SELECTION only; a targeted fire on a named object is not subject to it."),
				*Mesh->GetName(), Coverage, EffectiveMinCoveragePct);
			continue;
		}

		const AnomalyLod::FCurrentLod Current = AnomalyLod::GetCurrentLod(World, Mesh);
		const int32 PopNumLods = AnomalyLod::GetWorstLod(Mesh);
		const int32 PopForcedLodModel = PopNumLods;
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: CURRENT-LOD '%s' level=%s screen_size=%.6f source=%s forced_lod_model=%d (LOD index %d of %d) (%s) — the anomaly's ")
			TEXT("visible magnitude is the CONTRAST between this level and the forced one, so a candidate already at ")
			TEXT("a reduced LOD pops to something close to itself."),
			*Mesh->GetName(),
			Current.bKnown ? *FString::FromInt(Current.Level) : TEXT("UNDETERMINED"),
			Current.ScreenSize, Current.Source, PopForcedLodModel, PopForcedLodModel - 1, PopNumLods,
			bRequireHighestLod ? TEXT("ENFORCED, auto-pool selection") : TEXT("BYPASSED, targeted fire"));

		if (bRequireHighestLod && Current.bKnown && Current.Level != 0)
		{
			++RefusedNotHighestLod;
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: REFUSED '%s' — it is ALREADY AT LOD %d (source %s, screen_size %.6f), not its ")
				TEXT("highest-detail LOD 0, so forcing forced_lod_model=%d (LOD index %d of %d) onto it would change little or ")
				TEXT("nothing. This is the GRADED form of the single-LOD guard and it gates AUTO-POOL SELECTION only; a targeted ")
				TEXT("fire on a named object warns and fires anyway. IAI.Anomaly.LodRequireHighestLod 0 disables it."),
				*Mesh->GetName(), Current.Level, Current.Source, Current.ScreenSize, PopForcedLodModel, PopForcedLodModel - 1,
				PopNumLods);
			continue;
		}

		if (!bAutoPool && Current.bKnown && Current.Level != 0)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: TARGETED FIRE on '%s' which is ALREADY AT LOD %d (source %s, screen_size %.6f), ")
				TEXT("not its highest-detail LOD 0. FIRING ANYWAY — your pick wins — but the pop will be WEAKER than ")
				TEXT("on the same object at LOD 0, because the visible magnitude is the contrast between the current ")
				TEXT("level and the forced one. Move closer, or expect a small change."),
				*Mesh->GetName(), Current.Level, Current.Source, Current.ScreenSize);
		}

		if (Mesh->IsA<USkinnedMeshComponent>())
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: '%s' is a skeletal mesh, allowed by IAI.Anomaly.LodAllowSkeletal 1. Its forced LOD reaches the picture one frame late, so the labels at pop edges and at the revert may be one frame off. This is UNMEASURED."),
				*Mesh->GetName());
		}

		FPoppingTarget Target;
		Target.Mesh = Mesh;
		Target.BaselineLod = AnomalyLod::GetForcedLod(Mesh);
		Target.PoppedLod   = AnomalyLod::GetWorstLod(Mesh);
		Targets.Add(Target);
	}

	FramesSinceToggle = 0;
	bPoppedPhase = false;
	Mode = AnomalyLodTriggerRule::EMode::Periodic;
	bActive = Targets.Num() > 0;

	if (!bActive)
	{
		LastRefusal = UTF8_TO_TCHAR(AnomalyLodPolicy::DominantRefusal(RefusedHism, RefusedSingleLod, RefusedSkeletal,
			RefusedTooSmall + RefusedTooFar + RefusedNotHighestLod));
		UE_LOG(LogAnomaly, Warning,
			TEXT("lod_popping: matched %d component(s) for '%s' but REFUSED ALL [%s] — %d hierarchical-instanced, %d single-LOD, %d skeletal, %d below the %.4f bounds_coverage_pct minimum, %d beyond the %.2f cm proximity maximum, %d not at their highest LOD. Applying nothing, so no fire is recorded and no label is written."),
			Meshes.Num(), *Substring, bAutoPool ? TEXT("auto-pool, gates ENFORCED") : TEXT("targeted, proximity gates BYPASSED"),
			RefusedHism, RefusedSingleLod, RefusedSkeletal, RefusedTooSmall, EffectiveMinCoveragePct, RefusedTooFar, MaxDistanceCm, RefusedNotHighestLod);
		return false;
	}

	TArray<UMeshComponent*> Forced;
	for (const FPoppingTarget& Target : Targets)
	{
		if (UMeshComponent* Mesh = Target.Mesh.Get())
		{
			Forced.Add(Mesh);
		}
	}
	AnomalyLod::ScopeEventToForcedComponents(Forced, ScopedOwners, TEXT("lod_popping"));

	UE_LOG(LogAnomaly, Log,
		TEXT("lod_popping: matched %d component(s) for '%s' at half-period %d frame(s) [%s] — %d qualified, %d refused (hierarchical instanced), %d refused (single LOD), %d refused (skeletal), %d refused (below %.4f bounds_coverage_pct), %d refused (beyond %.2f cm), %d refused (not at highest LOD)."),
		Meshes.Num(), *Substring, HalfPeriodFrames,
		bAutoPool ? TEXT("auto-pool, gates ENFORCED") : TEXT("targeted, proximity gates BYPASSED"),
		Targets.Num(), RefusedHism, RefusedSingleLod, RefusedSkeletal, RefusedTooSmall, EffectiveMinCoveragePct, RefusedTooFar, MaxDistanceCm,
		RefusedNotHighestLod);
	return bActive;
}

bool FAnomaly_LodPopping::Apply(UWorld* World, const TArray<FString>& Args)
{
	LastRefusal.Reset();
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("lod_popping: usage %s"), *GetUsage());
		return false;
	}
	if (bActive)
	{
		Revert();
	}

	using namespace AnomalyLodTriggerRule;
	EMode Requested = EMode::Auto;
	TArray<FString> PeriodicArgs;
	PeriodicArgs.Add(Args[0]);
	if (Args.Num() >= 2)
	{
		const FString& M = Args[1];
		if (M.IsNumeric())
		{
			Requested = EMode::Periodic;
			PeriodicArgs.Add(M);
		}
		else if (M.Equals(TEXT("periodic"), ESearchCase::IgnoreCase))
		{
			Requested = EMode::Periodic;
			if (Args.Num() >= 3)
			{
				PeriodicArgs.Add(Args[2]);
			}
		}
		else if (M.Equals(TEXT("distance_threshold"), ESearchCase::IgnoreCase) || M.Equals(TEXT("distance"), ESearchCase::IgnoreCase))
		{
			Requested = EMode::DistanceThreshold;
		}
		else if (M.Equals(TEXT("view_entry"), ESearchCase::IgnoreCase))
		{
			Requested = EMode::ViewEntry;
		}
		else if (!M.Equals(TEXT("auto"), ESearchCase::IgnoreCase))
		{
			LastRefusal = TEXT("mode_invalid");
			UE_LOG(LogAnomaly, Warning, TEXT("lod_popping: REFUSED - unknown mode '%s' (mode_invalid). Usage: %s"), *M, *GetUsage());
			return false;
		}
	}

	if (Requested == EMode::Periodic)
	{
		if (!AnomalyBenchGate::IsEnabled())
		{
			LastRefusal = TEXT("periodic_bench_only");
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: REFUSED periodic mode (periodic_bench_only) - the timed pop has no spatial cause, so it runs only ")
				TEXT("with the bench gate open (-IAIBench). Captures use distance_threshold or view_entry (no mode = auto)."));
			return false;
		}
		return ApplyPeriodic(World, PeriodicArgs);
	}
	return ApplySpatial(World, Args[0], Requested);
}

bool FAnomaly_LodPopping::ApplySpatial(UWorld* World, const FString& Substring, AnomalyLodTriggerRule::EMode Requested)
{
	using namespace AnomalyLodTriggerRule;
	if (AnomalyLodTrigger::GetCaptureRenderRecord() == AnomalyLodTrigger::ECaptureRenderRecord::Unavailable)
	{
		LastRefusal = TEXT("render_record_unavailable");
		UE_LOG(LogAnomaly, Warning,
			TEXT("lod_popping: REFUSED '%s' (render_record_unavailable) - this capture grabs the backbuffer (IAI.Capture.SVE 0), which takes ")
			TEXT("no render-side LOD record, so a spatial label could not be confirmed on the render thread. A label that cannot be ")
			TEXT("confirmed is not written: spatial lod_popping runs on the default SVE path only."), *Substring);
		return false;
	}
	Settings = AnomalyLodTrigger::GetSettings();
	const TArray<TWeakObjectPtr<UMeshComponent>> Meshes = AnomalyLod::ResolveLodComponents(World, Substring);
	if (Meshes.Num() == 0)
	{
		UE_LOG(LogAnomaly, Log, TEXT("lod_popping: matched 0 mesh component(s) for '%s'."), *Substring);
		return false;
	}

	const AnomalyLod::FLodViewScope Scope(World);
	TMap<FString, int32> Refusals;
	UStaticMeshComponent* Best = nullptr;
	AnomalyLodTrigger::FArmEvaluation BestEval;
	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		UMeshComponent* Mesh = Weak.Get();
		if (!Mesh)
		{
			continue;
		}
		FString Why;
		const AnomalyLodPolicy::EForcedLodRefusal Policy = AnomalyLod::GetForcedLodRefusal(Mesh);
		AnomalyLodTrigger::FArmEvaluation E;
		if (Policy != AnomalyLodPolicy::EForcedLodRefusal::None)
		{
			Why = UTF8_TO_TCHAR(AnomalyLodPolicy::DescribeRefusal(Policy));
		}
		else if (UStaticMeshComponent* Static = Cast<UStaticMeshComponent>(Mesh))
		{
			E = AnomalyLodTrigger::EvaluateArm(World, Scope, Static, Requested, Settings);
			if (E.bArmable)
			{
				if (!Best || Static->Bounds.SphereRadius > Best->Bounds.SphereRadius)
				{
					Best = Static;
					BestEval = E;
				}
				continue;
			}
			Why = E.Refusal;
		}
		else
		{
			Why = AnomalyLodTrigger::GetStaticRefusal(Mesh);
		}
		Refusals.FindOrAdd(Why.IsEmpty() ? FString(TEXT("refused")) : Why) += 1;
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: NOT ARMABLE '%s' (%s) [requested %s] engine_lod=%d screen_size=%.6f closing_cm_s=%.1f angle_outside_deg=%.2f."),
			*Mesh->GetName(), *Why, UTF8_TO_TCHAR(DescribeMode(Requested)), E.Prediction.EngineLod, E.Prediction.BoundsScreenSize,
			E.ClosingSpeedCmPerS, E.AngleOutsideFrustumDeg);
	}

	if (!Best && GLodForceTrigger)
	{
		for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
		{
			UStaticMeshComponent* Static = Cast<UStaticMeshComponent>(Weak.Get());
			if (!Static || AnomalyLod::GetForcedLodRefusal(Static) != AnomalyLodPolicy::EForcedLodRefusal::None
				|| AnomalyLodTrigger::GetStaticRefusal(Static))
			{
				continue;
			}
			const AnomalyLod::FEngineLodPrediction Pred = AnomalyLod::PredictEngineLod(Scope.GetView(), Static);
			const int32 Coarsest = AnomalyLod::GetWorstLod(Static) - 1;
			if (!Pred.bValid || Coarsest <= Pred.EngineLod)
			{
				continue;
			}
			Best = Static;
			BestEval = AnomalyLodTrigger::FArmEvaluation();
			BestEval.bArmable = true;
			BestEval.Mode = EMode::ViewEntry;
			BestEval.HeldLod = Coarsest;
			BestEval.Prediction = Pred;
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: BENCH FORCE-TRIGGER - '%s' armed at its coarsest LOD %d with no camera motion, as view_entry, so ")
				TEXT("the still-camera leg can show that a 'no event' reading is not blindness. Never in a capture for a client."),
				*Static->GetName(), Coarsest);
			break;
		}
	}

	if (!Best)
	{
		FString Dominant(TEXT("none"));
		int32 DominantCount = 0;
		for (const TPair<FString, int32>& KV : Refusals)
		{
			if (KV.Value > DominantCount || (KV.Value == DominantCount && KV.Key < Dominant))
			{
				Dominant = KV.Key;
				DominantCount = KV.Value;
			}
		}
		LastRefusal = FString(TEXT("lod_not_armable:")) + Dominant;
		UE_LOG(LogAnomaly, Warning,
			TEXT("lod_popping: matched %d component(s) for '%s' but NONE CAN BE ARMED NOW [%s] - a spatial pop needs a plain static ")
			TEXT("multi-LOD mesh the camera is approaching (distance_threshold) or turning toward (view_entry). Applying nothing, ")
			TEXT("so no fire and no label."),
			Meshes.Num(), *Substring, *LastRefusal);
		return false;
	}

	Targets.Reset();
	Mode = BestEval.Mode;
	SpatialMesh = Best;
	SpatialMeshName = Best->GetName();
	WorldRef = World;
	SavedForcedLod = Best->ForcedLodModel;
	float Sizes[AnomalyLod::MaxLodEntries];
	NumLods = AnomalyLod::GetLodScreenSizes(Best, Sizes, Triangles);
	Tracker = FTracker();
	Tracker.Mode = Mode;
	Tracker.HeldLod = BestEval.HeldLod;
	ArmedFramesAhead = BestEval.FramesAhead;
	Tracker.ReleaseScreenSize = BestEval.ReleaseScreenSize;
	Tracker.bHeld = true;
	HistoryCount = 0;
	SwapsRecorded = 0;
	bHoldOpenAtLastTickEnd = false;
	Eval = FTickEndEval();
	StoredTickEnd = FTickEndEval();
	StoredTickEndFrame = 0;
	ReleasedAtSeconds = -1.0;
	bDithered = false;
	for (int32 MaterialIndex = 0; MaterialIndex < Best->GetNumMaterials(); ++MaterialIndex)
	{
		const UMaterialInterface* Material = Best->GetMaterial(MaterialIndex);
		bDithered = bDithered || (Material && Material->IsDitheredLODTransition());
	}
	AppliedFrame = GFrameCounter;
	LastCommittedFrame = 0;
	Best->SetForcedLodModel(BestEval.HeldLod + 1);
	bActive = true;
	TArray<UMeshComponent*> Forced;
	Forced.Add(Best);
	AnomalyLod::ScopeEventToForcedComponents(Forced, ScopedOwners, TEXT("lod_popping"));
	UE_LOG(LogAnomaly, Log,
		TEXT("lod_popping: ARMED '%s' mode=%s held_lod=%d (forced_lod_model=%d, saved %d) engine_lod=%d screen_size=%.6f ")
		TEXT("release_screen_size=%.6f look_ahead_screen_size=%.6f frames_ahead=%d closing_cm_s=%.1f angle_outside_deg=%.2f settings[%s] coverage_min=%.2f. ")
		TEXT("Arming changes nothing visible (the held LOD equals the engine's, or the target is off screen); labels start ")
		TEXT("only when the held LOD is visibly wrong."),
		*Best->GetName(), UTF8_TO_TCHAR(DescribeMode(Mode)), BestEval.HeldLod, BestEval.HeldLod + 1, SavedForcedLod,
		BestEval.Prediction.EngineLod, BestEval.Prediction.BoundsScreenSize, BestEval.ReleaseScreenSize, BestEval.LookAheadScreenSize, BestEval.FramesAhead,
		BestEval.ClosingSpeedCmPerS, BestEval.AngleOutsideFrustumDeg, *AnomalyDefaults::DescribeLodTriggerSettings(),
		Settings.CoverageMinPct);
	return true;
}

const FAnomaly_LodPopping::FTickEndEval& FAnomaly_LodPopping::EvaluateNow() const
{
	UWorld* World = WorldRef.Get();
	UStaticMeshComponent* Mesh = SpatialMesh.Get();
	FAnomalyViewInfo View;
	if (!World || !Mesh || !AnomalyViewport::GetActiveViewInfo(World, View) || !View.bValid)
	{
		Eval = FTickEndEval();
		return Eval;
	}
	if (Eval.bValid && Eval.CameraLocation.Equals(View.Origin, 0.0f) && Eval.CameraRotation.Equals(View.Rotation, 0.0f)
		&& Eval.BoundsOrigin.Equals(Mesh->Bounds.Origin, 0.0f) && Eval.bHeld == Tracker.bHeld && Eval.Frame == LastCommittedFrame)
	{
		return Eval;
	}
	using namespace AnomalyLodTriggerRule;
	FTickEndEval E;
	E.bValid = true;
	E.Frame = LastCommittedFrame;
	E.CameraLocation = View.Origin;
	E.CameraRotation = View.Rotation;
	E.BoundsOrigin = Mesh->Bounds.Origin;
	E.bHeld = Tracker.bHeld;
	const AnomalyLod::FLodViewScope Scope(World);
	const AnomalyLod::FEngineLodPrediction Pred = AnomalyLod::PredictEngineLod(Scope.GetView(), Mesh);
	bool bOnScreen = false;
	E.Input.CoveragePct = AnomalyLodTrigger::GetComponentCoveragePct(World, Mesh, &bOnScreen);
	E.Input.bOnScreen = bOnScreen;
	E.Input.bInFrustum = bOnScreen || AnomalyViewport::IsComponentInFrustum(View, Mesh);
	E.Input.EngineLod = Pred.bValid ? Pred.EngineLod : -1;
	E.Input.TemporalLod0 = Pred.bValid ? Pred.TemporalLod0 : -1;
	E.Input.TemporalLod1 = Pred.bValid ? Pred.TemporalLod1 : -1;
	E.Input.BoundsScreenSize = Pred.BoundsScreenSize;
	E.DistanceCm = AnomalyLodTrigger::GetDistanceToBoundsCm(View.Origin, Mesh);
	E.MinLod = Pred.bValid ? Pred.MinLod : -1;
	if (GLodPredictStaleView && HistoryCount > 0 && Pred.bValid)
	{
		float Sizes[AnomalyLod::MaxLodEntries];
		int32 Tris[AnomalyLod::MaxLodEntries];
		const int32 Entries = AnomalyLod::GetLodScreenSizes(Mesh, Sizes, Tris) > 0 ? AnomalyLod::MaxLodEntries : 0;
		const float Radius = (float)Mesh->Bounds.SphereRadius;
		const float StaleSize = Pred.BoundsScreenSize * (E.DistanceCm + Radius) / FMath::Max(1.0f, DistanceHistory[0] + Radius);
		E.Input.EngineLod = FMath::Clamp(EngineLodFromScreenSize(Sizes, Entries, StaleSize, Pred.Scale, Pred.MinLod), Pred.MinLod, Pred.NumLods - 1);
		E.Input.TemporalLod0 = E.Input.EngineLod;
		E.Input.TemporalLod1 = E.Input.EngineLod;
	}
	if (GLodSkipOnScreen)
	{
		E.Input.bOnScreen = true;
		E.Input.CoveragePct = FMath::Max(E.Input.CoveragePct, Settings.CoverageMinPct);
	}
	E.Input.bContrast = ContrastPasses(Triangles, NumLods, Tracker.HeldLod, E.Input.EngineLod, Settings.TriangleRatio);
	if (HistoryCount > 0)
	{
		const int32 Oldest = FMath::Min(HistoryCount, 8) - 1;
		const double Seconds = World->GetTimeSeconds() - TimeHistory[Oldest];
		if (Seconds > 0.0)
		{
			const float Speed = (float)((E.DistanceCm - DistanceHistory[Oldest]) / Seconds);
			E.Input.bRetreating = Speed >= Settings.RetreatSpeedCmPerS;
		}
	}
	FTracker Copy = Tracker;
	E.Observation = ObserveFrame(Copy, E.Input, Settings);
	E.bHoldOpen = AnomalyLodTriggerRule::IsHoldingEventOpen(Copy);
	E.bCapReached = Copy.bCapReached;
	Eval = E;
	return Eval;
}

void FAnomaly_LodPopping::CommitPreviousFrame()
{
	if (GFrameCounter <= AppliedFrame || LastCommittedFrame == GFrameCounter)
	{
		return;
	}
	const bool bStored = StoredTickEnd.bValid && StoredTickEndFrame + 1 == GFrameCounter;
	const FTickEndEval E = bStored ? StoredTickEnd : EvaluateNow();
	LastCommittedFrame = GFrameCounter;
	if (!E.bValid)
	{
		return;
	}
	const AnomalyLodTriggerRule::FObservation O = AnomalyLodTriggerRule::ObserveFrame(Tracker, E.Input, Settings);
	for (int32 i = FMath::Min(HistoryCount, 7); i > 0; --i)
	{
		DistanceHistory[i] = DistanceHistory[i - 1];
		TimeHistory[i] = TimeHistory[i - 1];
	}
	DistanceHistory[0] = E.DistanceCm;
	TimeHistory[0] = WorldRef.IsValid() ? WorldRef->GetTimeSeconds() : 0.0;
	HistoryCount = FMath::Min(HistoryCount + 1, 8);
	if (O.bSwap)
	{
		++SwapsRecorded;
		UE_LOG(LogAnomaly, Log, TEXT("lod_popping: SWAP %d cause=%s direction=%s lod %d -> %d screen_size=%.6f distance_cm=%.1f coverage_pct=%.3f."),
			SwapsRecorded, UTF8_TO_TCHAR(AnomalyLodTriggerRule::DescribeCause(O.Cause)), O.bDown ? TEXT("down") : TEXT("up"),
			O.FromLod, O.ToLod, E.Input.BoundsScreenSize, E.DistanceCm, E.Input.CoveragePct);
	}
}

void FAnomaly_LodPopping::Tick(float DeltaSeconds)
{
	if (!bActive)
	{
		return;
	}

	if (Mode != AnomalyLodTriggerRule::EMode::Periodic)
	{
		CommitPreviousFrame();
		UStaticMeshComponent* Mesh = SpatialMesh.Get();
		if (!Mesh)
		{
			return;
		}
		if (bLaggedRelease)
		{
			bLaggedRelease = false;
			Mesh->SetForcedLodModel(SavedForcedLod);
			ReleasedAtSeconds = WorldRef.IsValid() ? WorldRef->GetTimeSeconds() : -1.0;
			UE_LOG(LogAnomaly, Warning, TEXT("lod_popping: BENCH RELEASE-LAG - the release reaches the mesh one tick after the recorded swap."));
		}
		const AnomalyLodTriggerRule::ETickAction Action = AnomalyLodTriggerRule::DecideTick(Tracker, Settings);
		if (Action == AnomalyLodTriggerRule::ETickAction::ReleaseVisible && GLodReleaseLag)
		{
			bLaggedRelease = true;
		}
		else if (Action == AnomalyLodTriggerRule::ETickAction::ReleaseVisible || Action == AnomalyLodTriggerRule::ETickAction::ReleaseInvisible)
		{
			Mesh->SetForcedLodModel(SavedForcedLod);
			ReleasedAtSeconds = WorldRef.IsValid() ? WorldRef->GetTimeSeconds() : -1.0;
			UE_LOG(LogAnomaly, Log, TEXT("lod_popping: RELEASE (%s) '%s' -> forced_lod_model=%d; the engine draws its own LOD from this frame."),
				Action == AnomalyLodTriggerRule::ETickAction::ReleaseVisible ? TEXT("trigger, visible") : TEXT("the engine LOD is at or past the held one, invisible"),
				*Mesh->GetName(), SavedForcedLod);
		}
		else if (Action == AnomalyLodTriggerRule::ETickAction::ReHold)
		{
			Mesh->SetForcedLodModel(Tracker.HeldLod + 1);
			ReleasedAtSeconds = -1.0;
			UE_LOG(LogAnomaly, Log, TEXT("lod_popping: RE-HOLD (retreat past the hysteresis band) '%s' -> forced_lod_model=%d."),
				*Mesh->GetName(), Tracker.HeldLod + 1);
		}
		return;
	}

	++FramesSinceToggle;
	while (FramesSinceToggle >= HalfPeriodFrames)
	{
		FramesSinceToggle -= HalfPeriodFrames;
		bPoppedPhase = !bPoppedPhase;

		int32 Affected = 0;
		for (const FPoppingTarget& Target : Targets)
		{
			if (UMeshComponent* Mesh = Target.Mesh.Get())
			{
				AnomalyLod::SetForcedLod(Mesh, bPoppedPhase ? Target.PoppedLod : Target.BaselineLod);
				++Affected;
			}
		}
		UE_LOG(LogAnomaly, Verbose, TEXT("lod_popping snap -> %s (%d components)."),
			bPoppedPhase ? TEXT("POPPED") : TEXT("BASELINE"), Affected);
	}
}

void FAnomaly_LodPopping::ReleaseSpatial()
{
	UStaticMeshComponent* Mesh = SpatialMesh.Get();
	if (Mesh)
	{
		const bool bWasHeld = Tracker.bHeld;
		const bool bVisible = bWasHeld && Tracker.bHasLast && Tracker.LastClass == AnomalyLodTriggerRule::EFrameClass::Positive;
		if (Mesh->ForcedLodModel != SavedForcedLod && !GLodSkipRestore)
		{
			Mesh->SetForcedLodModel(SavedForcedLod);
		}
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: REVERT '%s' mode=%s forced_lod_model read back %d (saved %d) %s; swaps=%d, last state %s%s."),
			*Mesh->GetName(), UTF8_TO_TCHAR(AnomalyLodTriggerRule::DescribeMode(Mode)), Mesh->ForcedLodModel, SavedForcedLod,
			Mesh->ForcedLodModel == SavedForcedLod ? TEXT("RESTORED") : TEXT("NOT RESTORED"), SwapsRecorded,
			UTF8_TO_TCHAR(AnomalyLodTriggerRule::DescribeState(Tracker.State)),
			bVisible ? TEXT(", released while visibly wrong (hold_cap)") : (bWasHeld ? TEXT(", released while unseen or matching (invisible)") : TEXT("")));
	}
	else
	{
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: REVERT '%s' mode=%s NOTHING TO RESTORE (mesh destroyed): the held component no longer exists, so its forced LOD went with it; swaps=%d, last state %s."),
			*SpatialMeshName, UTF8_TO_TCHAR(AnomalyLodTriggerRule::DescribeMode(Mode)), SwapsRecorded,
			UTF8_TO_TCHAR(AnomalyLodTriggerRule::DescribeState(Tracker.State)));
	}
	SpatialMesh.Reset();
	SpatialMeshName.Reset();
	WorldRef.Reset();
	bLaggedRelease = false;
	bDithered = false;
	ReleasedAtSeconds = -1.0;
	Tracker = AnomalyLodTriggerRule::FTracker();
	Eval = FTickEndEval();
	StoredTickEnd = FTickEndEval();
	StoredTickEndFrame = 0;
	HistoryCount = 0;
	SwapsRecorded = 0;
	bHoldOpenAtLastTickEnd = false;
	AppliedFrame = 0;
	LastCommittedFrame = 0;
}

void FAnomaly_LodPopping::Revert()
{
	for (const FPoppingTarget& Target : Targets)
	{
		if (UMeshComponent* Mesh = Target.Mesh.Get())
		{
			AnomalyLod::SetForcedLod(Mesh, Target.BaselineLod);
		}
	}
	if (SpatialMesh.IsValid() || WorldRef.IsValid())
	{
		ReleaseSpatial();
	}

	AnomalyLod::ClearForcedComponentScope(ScopedOwners);
	Targets.Reset();
	FramesSinceToggle = 0;
	bPoppedPhase = false;
	bActive = false;
}

int32 FAnomaly_LodPopping::GetDeferredOnsetTimeoutFrames() const
{
	return Mode == AnomalyLodTriggerRule::EMode::Periodic ? -1
		: AnomalyLodTriggerRule::ArmTimeoutFramesFor(Mode,
			AnomalyDefaults::GetLodTriggerSetting(AnomalyDefaults::ELodTriggerSetting::ArmTimeoutFrames), ArmedFramesAhead, Settings.EntryFrames);
}

bool FAnomaly_LodPopping::IsHoldingEventOpen() const
{
	return bActive && Mode != AnomalyLodTriggerRule::EMode::Periodic && bHoldOpenAtLastTickEnd;
}

bool FAnomaly_LodPopping::HasDeferredOnsetBegun() const
{
	if (!bActive || Mode == AnomalyLodTriggerRule::EMode::Periodic)
	{
		return false;
	}
	if (Tracker.bWrongSeen)
	{
		return true;
	}
	const bool bStored = StoredTickEnd.bValid && StoredTickEndFrame + 1 >= GFrameCounter;
	const AnomalyLodTriggerRule::EFrameClass Class = bStored ? StoredTickEnd.Observation.Class : EvaluateNow().Observation.Class;
	return Class == AnomalyLodTriggerRule::EFrameClass::Positive || Class == AnomalyLodTriggerRule::EFrameClass::Subvisible;
}

void FAnomaly_LodPopping::NoteCapturedFrame(bool bAnomalousThisFrame)
{
	if (!bActive || Mode == AnomalyLodTriggerRule::EMode::Periodic)
	{
		bHoldOpenAtLastTickEnd = false;
		return;
	}
	StoredTickEnd = EvaluateNow();
	StoredTickEndFrame = GFrameCounter;
	bHoldOpenAtLastTickEnd = StoredTickEnd.bHoldOpen || (bDithered && !Tracker.bHeld && InPostReleaseWindow());
}

bool FAnomaly_LodPopping::InPostReleaseWindow() const
{
	if (ReleasedAtSeconds < 0.0 || !WorldRef.IsValid())
	{
		return false;
	}
	static IConsoleVariable* TemporalLag = IConsoleManager::Get().FindConsoleVariable(TEXT("lod.TemporalLag"));
	const double LagSeconds = TemporalLag ? FMath::Max(0.0f, TemporalLag->GetFloat()) : 0.5;
	return WorldRef->GetTimeSeconds() - ReleasedAtSeconds <= LagSeconds + 0.05;
}

bool FAnomaly_LodPopping::IsCurrentlyAnomalous() const
{
	if (!bActive)
	{
		return false;
	}
	if (Mode == AnomalyLodTriggerRule::EMode::Periodic)
	{
		return bPoppedPhase;
	}
	return EvaluateNow().Observation.Class == AnomalyLodTriggerRule::EFrameClass::Positive;
}

bool FAnomaly_LodPopping::GetTelemetry(FAnomalyTelemetry& Out) const
{
	if (!bActive)
	{
		return false;
	}
	using namespace AnomalyLodTriggerRule;
	if (Mode == EMode::Periodic)
	{
		Out.AddString(TEXT("lod.trigger"), TEXT("periodic"));
		Out.AddString(TEXT("lod.state"), bPoppedPhase ? TEXT("popped") : TEXT("baseline"));
		return true;
	}
	const FTickEndEval& E = EvaluateNow();
	const EFrameClass Class = E.Observation.Class;
	const TCHAR* State = Class == EFrameClass::Positive ? TEXT("labelled")
		: Class == EFrameClass::Subvisible ? TEXT("subvisible")
		: Class == EFrameClass::Occluded ? TEXT("occluded")
		: Class == EFrameClass::PastHeld ? TEXT("past_held")
		: Tracker.bHeld ? TEXT("armed") : TEXT("swapped");
	Out.AddInt(TEXT("lod.held_lod"), Tracker.HeldLod);
	Out.AddInt(TEXT("lod.engine_lod"), E.Input.EngineLod);
	Out.AddFloat(TEXT("lod.screen_size"), E.Input.BoundsScreenSize);
	Out.AddFloat(TEXT("lod.distance_cm"), E.DistanceCm);
	Out.AddFloat(TEXT("lod.coverage_pct"), E.Input.CoveragePct);
	Out.AddBool(TEXT("lod.on_screen"), E.Input.bOnScreen);
	Out.AddBool(TEXT("lod.held"), Tracker.bHeld);
	Out.AddBool(TEXT("lod.probe"), Tracker.bHeld || InPostReleaseWindow());
	Out.AddBool(TEXT("lod.dithered"), bDithered);
	Out.AddInt(TEXT("lod.min_lod"), E.MinLod);
	Out.AddInt(TEXT("lod.temporal0"), E.Input.TemporalLod0);
	Out.AddInt(TEXT("lod.temporal1"), E.Input.TemporalLod1);
	Out.AddBool(TEXT("lod.in_frustum"), E.Input.bInFrustum);
	Out.AddBool(TEXT("lod.subvisible"), IsFlaggedClass(Class));
	Out.AddBool(TEXT("lod.swap"), E.Observation.bSwap);
	Out.AddBool(TEXT("lod.cap_reached"), E.bCapReached);
	Out.AddString(TEXT("lod.trigger"), UTF8_TO_TCHAR(DescribeMode(Mode)));
	Out.AddString(TEXT("lod.state"), State);
	Out.AddString(TEXT("lod.component"), SpatialMesh.IsValid() ? SpatialMesh->GetName() : FString());
	if (E.Observation.bSwap)
	{
		Out.AddInt(TEXT("lod.swap_from_lod"), E.Observation.FromLod);
		Out.AddInt(TEXT("lod.swap_to_lod"), E.Observation.ToLod);
		Out.AddString(TEXT("lod.swap_cause"), UTF8_TO_TCHAR(DescribeCause(E.Observation.Cause)));
		Out.AddString(TEXT("lod.swap_direction"), E.Observation.bDown ? TEXT("down") : TEXT("up"));
	}
	return true;
}

void FAnomaly_LodPopping::OnTargetLost(AActor* Actor, bool bWorldEnding)
{
	const UStaticMeshComponent* Mesh = SpatialMesh.Get();
	if (!bActive || (Mesh && Mesh->GetOwner() != Actor))
	{
		return;
	}
	UE_LOG(LogAnomaly, Log, TEXT("lod_popping: TARGET LOST '%s'%s - releasing the hold and ending the event."),
		*GetNameSafe(Actor), bWorldEnding ? TEXT(" (world ending)") : TEXT(""));
	Revert();
}

void FAnomaly_LodPopping::OnWorldTeardown()
{
	if (bActive)
	{
		Revert();
	}
}

#if !UE_BUILD_SHIPPING

static FAutoConsoleCommand GLodReleaseLagCmd(
	TEXT("IAI.Bench.LodReleaseLag"),
	TEXT("BENCH DEVICE (m57), console only, default OFF - never in a client payload. ON delays every visible lod_popping release by one tick after the recorded swap frame, so the swap-frame gate (L2) must FAIL. Usage: IAI.Bench.LodReleaseLag <0|1>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() != 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.LodReleaseLag <0|1>  (current: %d)"), GLodReleaseLag ? 1 : 0);
			return;
		}
		GLodReleaseLag = FCString::Atoi(*Args[0]) != 0;
		UE_LOG(LogAnomaly, Warning, TEXT("IAI.Bench.LodReleaseLag -> %d. BENCH DEVICE."), GLodReleaseLag ? 1 : 0);
	}));

static FAutoConsoleCommand GLodForceTriggerCmd(
	TEXT("IAI.Bench.LodForceTrigger"),
	TEXT("BENCH DEVICE (m57), console only, default OFF - never in a client payload. ON arms a lod_popping target at its coarsest LOD with no camera motion (as view_entry), so the still-camera gate (L3) must produce events: its 'no event' reading is then shown not to be blindness. Usage: IAI.Bench.LodForceTrigger <0|1>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() != 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.LodForceTrigger <0|1>  (current: %d)"), GLodForceTrigger ? 1 : 0);
			return;
		}
		GLodForceTrigger = FCString::Atoi(*Args[0]) != 0;
		UE_LOG(LogAnomaly, Warning, TEXT("IAI.Bench.LodForceTrigger -> %d. BENCH DEVICE."), GLodForceTrigger ? 1 : 0);
	}));

static FAutoConsoleCommand GLodSkipOnScreenCmd(
	TEXT("IAI.Bench.LodSkipOnScreen"),
	TEXT("BENCH DEVICE (m57), console only, default OFF - never in a client payload. ON drops the on-screen term (and lifts coverage to the minimum) from lod_popping's label rule, so the unseen gate (L4) must produce labelled off-screen frames. Usage: IAI.Bench.LodSkipOnScreen <0|1>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() != 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.LodSkipOnScreen <0|1>  (current: %d)"), GLodSkipOnScreen ? 1 : 0);
			return;
		}
		GLodSkipOnScreen = FCString::Atoi(*Args[0]) != 0;
		UE_LOG(LogAnomaly, Warning, TEXT("IAI.Bench.LodSkipOnScreen -> %d. BENCH DEVICE."), GLodSkipOnScreen ? 1 : 0);
	}));

static FAutoConsoleCommand GLodPredictStaleViewCmd(
	TEXT("IAI.Bench.LodPredictStaleView"),
	TEXT("BENCH DEVICE (m57), console only, default OFF - never in a client payload. ON makes lod_popping's tick-end prediction use the previous frame's camera distance, so the render-record gate (L5) must produce lod_unconfirmed mismatches on moving legs. Usage: IAI.Bench.LodPredictStaleView <0|1>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() != 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.LodPredictStaleView <0|1>  (current: %d)"), GLodPredictStaleView ? 1 : 0);
			return;
		}
		GLodPredictStaleView = FCString::Atoi(*Args[0]) != 0;
		UE_LOG(LogAnomaly, Warning, TEXT("IAI.Bench.LodPredictStaleView -> %d. BENCH DEVICE."), GLodPredictStaleView ? 1 : 0);
	}));

static FAutoConsoleCommand GLodSkipRestoreCmd(
	TEXT("IAI.Bench.LodSkipRestore"),
	TEXT("BENCH DEVICE (m57), console only, default OFF - never in a client payload. ON skips restoring the saved forced LOD at a spatial lod_popping revert, so the revert read-back gate (L6) must FAIL. Usage: IAI.Bench.LodSkipRestore <0|1>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() != 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.LodSkipRestore <0|1>  (current: %d)"), GLodSkipRestore ? 1 : 0);
			return;
		}
		GLodSkipRestore = FCString::Atoi(*Args[0]) != 0;
		UE_LOG(LogAnomaly, Warning, TEXT("IAI.Bench.LodSkipRestore -> %d. BENCH DEVICE."), GLodSkipRestore ? 1 : 0);
	}));
#endif
