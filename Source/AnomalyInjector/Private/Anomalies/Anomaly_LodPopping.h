#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class UWorld;
class UMeshComponent;

class FAnomaly_LodPopping final : public IAnomaly
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
	struct FPoppingTarget
	{
		TWeakObjectPtr<UMeshComponent> Mesh;
		int32 BaselineLod = 0;
		int32 PoppedLod = 0;
	};

	TArray<FPoppingTarget> Targets;
	float HalfPeriodSeconds = 0.25f;
	float Accumulator = 0.0f;
	bool  bPoppedPhase = false;
	bool  bActive = false;

	static constexpr float DefaultHz = 2.0f;
	static constexpr float MinHz = 0.1f;
	static constexpr float MaxHz = 30.0f;
};
