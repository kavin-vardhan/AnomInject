// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IGDPAnomaly.h"

class AActor;
class UWorld;

/**
 * flicker — ticking, actor-scoped. Resolve+cache targets in Apply; in Tick, accumulate
 * DeltaSeconds and toggle the targets' hidden flag every half-period. Revert leaves the
 * actors VISIBLE regardless of the current toggle phase. Proves the Tick path.
 */
class FGDPAnomaly_Flicker final : public IGDPAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("flicker")); }
	virtual FString GetDescription() const override { return TEXT("Flicker matching actors by toggling visibility at a rate (Hz)."); }
	virtual FString GetUsage() const override { return TEXT("<name-substring> [hz]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }

private:
	TArray<TWeakObjectPtr<AActor>> Targets;
	float HalfPeriodSeconds = 0.1f;   // default 5 Hz -> 0.1s; recomputed in Apply
	float Accumulator = 0.0f;
	bool  bHiddenPhase = false;
	bool  bActive = false;

	static constexpr float DefaultHz = 5.0f;
	static constexpr float MaxHz = 60.0f;
};
