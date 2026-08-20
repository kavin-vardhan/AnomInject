#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class UWorld;
class UMeshComponent;

class FAnomaly_LodPopping final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("lod_popping")); }
	virtual FString GetDescription() const override { return TEXT("Pop matching static or skeletal mesh components between LODs every N frames."); }
	virtual FString GetUsage() const override { return TEXT("<substring> [half_period_frames]"); }

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
	int32 HalfPeriodFrames = DefaultHalfPeriodFrames;
	int32 FramesSinceToggle = 0;
	bool  bPoppedPhase = false;
	bool  bActive = false;

	static constexpr int32 DefaultHalfPeriodFrames = 8;
	static constexpr int32 MinHalfPeriodFrames = 1;
	static constexpr int32 MaxHalfPeriodFrames = 600;
};
