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

	const float MaxDistanceCm = AnomalyDefaults::GetLodPoppingMaxDistanceCm();

	Targets.Reset();
	int32 RefusedSingleLod = 0;
	int32 RefusedTooSmall = 0;
	int32 RefusedTooFar = 0;
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
				TEXT("lod_popping: COVERAGE '%s' bounds_coverage_pct=%.4f (threshold %.4f) — bounds-projected screen coverage at PICK TIME, the same quantity the gate tests; NOT drawn extent, which is smaller."),
				*GetNameSafe(Owner), Coverage, MinCoveragePct);
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
					TEXT("lod_popping: REFUSED '%s' — poll_distance_cm %.2f exceeds the %.2f cm maximum. This gate is an OWNER PRODUCT PREFERENCE (pop only what the player is right next to), and it ANDs with the %.4f%% coverage gate rather than replacing it."),
					*Mesh->GetName(), Distance, MaxDistanceCm, MinCoveragePct);
				continue;
			}
		}

		if (MinCoveragePct > 0.0f && Coverage < MinCoveragePct)
		{
			++RefusedTooSmall;
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: REFUSED '%s' — bounds_coverage_pct %.4f is below the %.4f minimum, so at this on-screen size its LODs do not differ visibly: forcing one would pop it to itself. Same failure as a single-LOD mesh, and the mask veto cannot catch it either (it still draws pixels)."),
				*Mesh->GetName(), Coverage, MinCoveragePct);
			continue;
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
			TEXT("lod_popping: matched %d component(s) for '%s' but REFUSED ALL — %d single-LOD, %d below the %.4f bounds_coverage_pct minimum, %d beyond the %.2f cm proximity maximum. Applying nothing, so no fire is recorded and no label is written."),
			Meshes.Num(), *Substring, RefusedSingleLod, RefusedTooSmall, MinCoveragePct, RefusedTooFar, MaxDistanceCm);
		return false;
	}

	UE_LOG(LogAnomaly, Log,
		TEXT("lod_popping: matched %d component(s) for '%s' at half-period %d frame(s) — %d qualified, %d refused (single LOD), %d refused (below %.4f bounds_coverage_pct), %d refused (beyond %.2f cm)."),
		Meshes.Num(), *Substring, HalfPeriodFrames, Targets.Num(), RefusedSingleLod, RefusedTooSmall, MinCoveragePct,
		RefusedTooFar, MaxDistanceCm);
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
