#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class AActor;
class UWorld;

class FAnomaly_Blinking final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("blinking")); }
	virtual FString GetDescription() const override { return TEXT("Blinking matching actors by toggling visibility at a rate (Hz)."); }
	virtual FString GetUsage() const override { return TEXT("<name-substring> [half_period_frames]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }

private:
	TArray<TWeakObjectPtr<AActor>> Targets;
	int32 HalfPeriodFrames = DefaultHalfPeriodFrames;
	int32 FramesSinceToggle = 0;
	bool  bHiddenPhase = false;
	bool  bActive = false;

	static constexpr int32 DefaultHalfPeriodFrames = 3;
	static constexpr int32 MinHalfPeriodFrames = 1;
	static constexpr int32 MaxHalfPeriodFrames = 600;
};
