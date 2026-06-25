#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class UWorld;

class FAnomaly_CameraClipping final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("camera_clipping")); }
	virtual FString GetDescription() const override { return TEXT("Push the near clip plane out so near geometry clips away."); }
	virtual FString GetUsage() const override { return TEXT("[near-plane]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }

private:
	TWeakObjectPtr<UWorld> WorldWeak;
	float PreviousNearClip = 10.0f;
	bool  bActive = false;

	static constexpr float DefaultNearClip = 100.0f;
	static constexpr float MinNearClip = 1.0f;
	static constexpr float MaxNearClip = 100000.0f;

	void ExecuteSetNearClip(UWorld* World, float Value) const;
};
