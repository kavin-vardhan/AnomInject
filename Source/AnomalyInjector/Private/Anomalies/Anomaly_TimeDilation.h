#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class UWorld;

class FAnomaly_TimeDilation final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("time_dilation")); }
	virtual FString GetDescription() const override { return TEXT("Scale global time dilation (slow-mo / fast-forward)."); }
	virtual FString GetUsage() const override { return TEXT("<scale>"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }

private:
	TWeakObjectPtr<UWorld> WorldWeak;
	float PreviousDilation = 1.0f;
	bool  bActive = false;
};
