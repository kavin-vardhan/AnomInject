#pragma once
#include "IAnomaly.h"
#include "Anomalies/Anomaly_CorruptedTexture.h"

class FAnomaly_ChangeCase final : public IAnomaly
{
public:
	FAnomaly_ChangeCase(FName InId, bool bInSwap) : Id(InId), bSwap(bInSwap) {}
	FName GetId() const override { return Id; }
	FString GetDescription() const override { return TEXT("Bench-only matched change-evidence case."); }
	FString GetUsage() const override { return TEXT("<target> [delay=<captured labelled frames>]"); }
	bool Apply(UWorld* World, const TArray<FString>& Args) override;
	void TickAlways(float DeltaSeconds) override;
	void NoteCapturedFrame(bool bLabelled) override;
	void Revert() override;
	bool IsActive() const override { return bActive; }
private:
	FName Id;
	bool bSwap = false, bActive = false, bApplied = false;
	int32 Delay = 0, Captured = 0;
	uint64 LastNotedFrame = 0;
	TWeakObjectPtr<UWorld> ActiveWorld;
	TArray<FString> SavedArgs;
	FAnomaly_CorruptedTexture MaterialSwap;
};
