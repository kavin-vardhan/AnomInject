#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class AActor;
class UWorld;

class FAnomaly_Flicker final : public IAnomaly
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
	float HalfPeriodSeconds = 0.1f;
	float Accumulator = 0.0f;
	bool  bHiddenPhase = false;
	bool  bActive = false;

	static constexpr float DefaultHz = 5.0f;
	static constexpr float MaxHz = 60.0f;
};
