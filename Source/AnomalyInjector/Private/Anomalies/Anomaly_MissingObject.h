#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class AActor;
class UWorld;

class FAnomaly_MissingObject final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("missing_object")); }
	virtual FString GetDescription() const override { return TEXT("Hide actors whose name or class contains a substring."); }
	virtual FString GetUsage() const override { return TEXT("<name-substring>"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }

private:
	TArray<TWeakObjectPtr<AActor>> HiddenActors;
	bool bActive = false;
};
