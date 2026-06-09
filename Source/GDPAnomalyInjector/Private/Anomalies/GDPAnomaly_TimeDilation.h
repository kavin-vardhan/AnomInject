// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IGDPAnomaly.h"

class UWorld;

/**
 * time_dilation — world-global, no actors, no Tick. Apply captures the current global
 * time dilation as a baseline, then SetGlobalTimeDilation(scale). Revert restores the
 * captured baseline (falls back to 1.0 if nothing was captured). Proves the interface
 * does not assume actor-scoping.
 *
 * Note: AWorldSettings clamps Min/MaxGlobalTimeDilation, so extreme scales are clamped.
 */
class FGDPAnomaly_TimeDilation final : public IGDPAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("time_dilation")); }
	virtual FString GetDescription() const override { return TEXT("Scale global time dilation (slow-mo / fast-forward)."); }
	virtual FString GetUsage() const override { return TEXT("<scale>"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	// No Tick override — inherits the no-op.

private:
	TWeakObjectPtr<UWorld> WorldWeak;
	float PreviousDilation = 1.0f;   // baseline captured in Apply; fallback if never captured
	bool  bActive = false;
};
