// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Anomalies/Anomaly_LodPopping.h"

#include "AnomalyLod.h"
#include "AnomalyArgs.h"
#include "AnomalyInjectorLog.h"
#include "Components/MeshComponent.h"   // UMeshComponent

bool FAnomaly_LodPopping::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("lod_popping: usage <substring> [hz]"));
		return false;
	}

	// Re-entrancy: revert-then-reapply so re-firing mid-oscillation never leaks — the prior targets
	// are restored to baseline before the new capture, leaving exactly one capture set.
	if (bActive)
	{
		Revert();
	}

	const FString& Substring = Args[0];

	// Optional Hz (A3): default 2; non-numeric -> warn + default; clamped to [MinHz, MaxHz] so the
	// half-period is always finite and positive.
	const float Hz = AnomalyArgs::GetFloat(Args, 1, DefaultHz, MinHz, MaxHz);
	HalfPeriodSeconds = 0.5f / Hz;

	// Static AND skeletal, keyed to the common base (one apply pops a heterogeneous target set).
	const TArray<TWeakObjectPtr<UMeshComponent>> Meshes = AnomalyLod::ResolveLodComponents(World, Substring);
	if (Meshes.Num() == 0)
	{
		UE_LOG(LogAnomaly, Log, TEXT("lod_popping: matched 0 mesh component(s) for '%s'."), *Substring);
		return false;   // AMB-2: zero match -> not applied / inactive
	}

	Targets.Reset();
	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		UMeshComponent* Mesh = Weak.Get();
		if (!Mesh)
		{
			continue;
		}

		FPoppingTarget Target;
		Target.Mesh = Mesh;
		Target.BaselineLod = AnomalyLod::GetForcedLod(Mesh);   // capture original (usually 0 = auto)
		Target.PoppedLod   = AnomalyLod::GetWorstLod(Mesh);    // worst available per component
		Targets.Add(Target);
	}

	// Start at the baseline phase; the first half-period flips to the popped LOD (mirrors flicker
	// starting visible and first toggling hidden).
	Accumulator = 0.0f;
	bPoppedPhase = false;
	bActive = Targets.Num() > 0;

	UE_LOG(LogAnomaly, Log, TEXT("lod_popping: matched %d component(s) for '%s' at %.2f Hz (half-period %.3fs)."),
		Targets.Num(), *Substring, Hz, HalfPeriodSeconds);
	return bActive;
}

void FAnomaly_LodPopping::Tick(float DeltaSeconds)
{
	if (!bActive)
	{
		return;
	}

	Accumulator += DeltaSeconds;
	// 'while' (not 'if') so a single long frame replays all elapsed half-periods and the phase
	// never desyncs (flicker's while-drain).
	while (Accumulator >= HalfPeriodSeconds)
	{
		Accumulator -= HalfPeriodSeconds;
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
	// Restore each captured baseline regardless of the current oscillation phase; skip stale ptrs.
	for (const FPoppingTarget& Target : Targets)
	{
		if (UMeshComponent* Mesh = Target.Mesh.Get())
		{
			AnomalyLod::SetForcedLod(Mesh, Target.BaselineLod);
		}
	}

	Targets.Reset();
	Accumulator = 0.0f;
	bPoppedPhase = false;
	bActive = false;
}
