// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IGDPAnomaly.h"

class UWorld;
class UMeshComponent;

/**
 * lod_popping — ticking, component-scoped (static OR skeletal). Reuses flicker's mechanics:
 * resolve+cache targets in Apply (capturing each component's original forced-LOD as its baseline,
 * and its worst available LOD as the "popped" level), then in Tick accumulate DeltaSeconds and, on
 * each half-period, snap every target between its baseline LOD and the popped LOD. The visible snap
 * between detail levels is the anomaly. Revert restores each captured baseline regardless of the
 * current oscillation phase (flicker's lesson — never leave anything stuck at a forced value) and
 * resets the accumulator/phase.
 *
 * Default 2 Hz — slower than flicker so each LOD state dwells long enough to read as visibly
 * distinct; a fast pop just blurs into noise. Clamp ceiling 30 Hz (ratified AMB-3).
 *
 * Uses the shared GDPLod helper for the static/skeletal dispatch; the per-target record is keyed to
 * the common UMeshComponent base, so one apply pops a heterogeneous set (static prop + skeletal Bot).
 * Same Tick-path shape as flicker — no IGDPAnomaly change.
 */
class FGDPAnomaly_LodPopping final : public IGDPAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("lod_popping")); }
	virtual FString GetDescription() const override { return TEXT("Pop matching static or skeletal mesh components between LODs at a rate (Hz)."); }
	virtual FString GetUsage() const override { return TEXT("<substring> [hz]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }

private:
	/** Per-target capture: the baseline (original forced-LOD, 1-based, 0 = auto) and the popped/worst LOD. */
	struct FPoppingTarget
	{
		TWeakObjectPtr<UMeshComponent> Mesh;
		int32 BaselineLod = 0;
		int32 PoppedLod = 0;
	};

	TArray<FPoppingTarget> Targets;
	float HalfPeriodSeconds = 0.25f;   // default 2 Hz -> 0.25s; recomputed in Apply
	float Accumulator = 0.0f;
	bool  bPoppedPhase = false;
	bool  bActive = false;

	static constexpr float DefaultHz = 2.0f;
	static constexpr float MinHz = 0.1f;
	static constexpr float MaxHz = 30.0f;
};
