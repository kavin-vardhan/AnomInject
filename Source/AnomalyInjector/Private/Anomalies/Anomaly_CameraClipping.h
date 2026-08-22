#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class UWorld;
class AActor;

class FAnomaly_CameraClipping final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("camera_clipping")); }
	virtual FString GetDescription() const override { return TEXT("Push the near clip plane out so near geometry clips away; whole-session, or triggered by proximity to a named object."); }
	virtual FString GetUsage() const override { return TEXT("[near-plane] | =<actor-name> [near-plane]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	virtual bool IsCurrentlyAnomalous() const override;

private:
	TWeakObjectPtr<UWorld> WorldWeak;
	TArray<TWeakObjectPtr<AActor>> Targets;
	FString TargetToken;
	float PreviousNearClip = 10.0f;
	float AnomalousNearClip = DefaultNearClip;
	float TriggerRadiusCm = 0.0f;
	int32 TriggerTransitions = 0;
	bool  bTargetedMode = false;
	bool  bPushed = false;
	bool  bActive = false;

	static constexpr float DefaultNearClip = 100.0f;
	static constexpr float MinNearClip = 1.0f;
	static constexpr float MaxNearClip = 100000.0f;

	void ExecuteSetNearClip(UWorld* World, float Value) const;
	bool IsPlayerWithinTriggerRadius() const;
};
