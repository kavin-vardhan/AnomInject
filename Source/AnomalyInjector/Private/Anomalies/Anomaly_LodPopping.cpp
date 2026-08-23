#include "Anomalies/Anomaly_LodPopping.h"

#include "AnomalyLod.h"
#include "AnomalyViewport.h"
#include "AnomalyDefaults.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "Components/MeshComponent.h"

bool FAnomaly_LodPopping::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("lod_popping: usage <substring> [half_period_frames]"));
		return false;
	}

	if (bActive)
	{
		Revert();
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
	int32 RefusedSingleLod = 0;
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

		if (!AnomalyLod::HasMultipleLods(Mesh))
		{
			++RefusedSingleLod;
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: REFUSED '%s' — it has a single LOD, so forcing a LOD would pop it to itself: no visible change, and a positive label with no visible change is an invisible anomaly the mask veto cannot catch (it still draws pixels)."),
				*Mesh->GetName());
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
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: CURRENT-LOD '%s' level=%s screen_size=%.6f source=%s worst=%d (%s) — the anomaly's ")
			TEXT("visible magnitude is the CONTRAST between this level and the forced one, so a candidate already at ")
			TEXT("a reduced LOD pops to something close to itself."),
			*Mesh->GetName(),
			Current.bKnown ? *FString::FromInt(Current.Level) : TEXT("UNDETERMINED"),
			Current.ScreenSize, Current.Source, AnomalyLod::GetWorstLod(Mesh),
			bRequireHighestLod ? TEXT("ENFORCED, auto-pool selection") : TEXT("BYPASSED, targeted fire"));

		if (bRequireHighestLod && Current.bKnown && Current.Level != 0)
		{
			++RefusedNotHighestLod;
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: REFUSED '%s' — it is ALREADY AT LOD %d (source %s, screen_size %.6f), not its ")
				TEXT("highest-detail LOD 0, so forcing LOD %d onto it would change little or nothing. This is the ")
				TEXT("GRADED form of the single-LOD guard and it gates AUTO-POOL SELECTION only; a targeted fire on ")
				TEXT("a named object warns and fires anyway. IAI.Anomaly.LodRequireHighestLod 0 disables it."),
				*Mesh->GetName(), Current.Level, Current.Source, Current.ScreenSize, AnomalyLod::GetWorstLod(Mesh));
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

		FPoppingTarget Target;
		Target.Mesh = Mesh;
		Target.BaselineLod = AnomalyLod::GetForcedLod(Mesh);
		Target.PoppedLod   = AnomalyLod::GetWorstLod(Mesh);
		Targets.Add(Target);
	}

	FramesSinceToggle = 0;
	bPoppedPhase = false;
	bActive = Targets.Num() > 0;

	if (!bActive)
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("lod_popping: matched %d component(s) for '%s' but REFUSED ALL [%s] — %d single-LOD, %d below the %.4f bounds_coverage_pct minimum, %d beyond the %.2f cm proximity maximum, %d not at their highest LOD. Applying nothing, so no fire is recorded and no label is written."),
			Meshes.Num(), *Substring, bAutoPool ? TEXT("auto-pool, gates ENFORCED") : TEXT("targeted, proximity gates BYPASSED"),
			RefusedSingleLod, RefusedTooSmall, EffectiveMinCoveragePct, RefusedTooFar, MaxDistanceCm, RefusedNotHighestLod);
		return false;
	}

	UE_LOG(LogAnomaly, Log,
		TEXT("lod_popping: matched %d component(s) for '%s' at half-period %d frame(s) [%s] — %d qualified, %d refused (single LOD), %d refused (below %.4f bounds_coverage_pct), %d refused (beyond %.2f cm), %d refused (not at highest LOD)."),
		Meshes.Num(), *Substring, HalfPeriodFrames,
		bAutoPool ? TEXT("auto-pool, gates ENFORCED") : TEXT("targeted, proximity gates BYPASSED"),
		Targets.Num(), RefusedSingleLod, RefusedTooSmall, EffectiveMinCoveragePct, RefusedTooFar, MaxDistanceCm,
		RefusedNotHighestLod);
	return bActive;
}

void FAnomaly_LodPopping::Tick(float DeltaSeconds)
{
	if (!bActive)
	{
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

void FAnomaly_LodPopping::Revert()
{
	for (const FPoppingTarget& Target : Targets)
	{
		if (UMeshComponent* Mesh = Target.Mesh.Get())
		{
			AnomalyLod::SetForcedLod(Mesh, Target.BaselineLod);
		}
	}

	Targets.Reset();
	FramesSinceToggle = 0;
	bPoppedPhase = false;
	bActive = false;
}
