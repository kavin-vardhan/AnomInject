// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class AActor;
class UWorld;

/**
 * missing_object — static, actor-scoped. Re-homes the M0 hide: resolve actors whose
 * Name/Class contains a substring, SetActorHiddenInGame(true), cache weak-ptrs, restore
 * on Revert. No Tick.
 */
class FAnomaly_MissingObject final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("missing_object")); }
	virtual FString GetDescription() const override { return TEXT("Hide actors whose name or class contains a substring."); }
	virtual FString GetUsage() const override { return TEXT("<name-substring>"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	// No Tick override — inherits the no-op.

private:
	/** Actors we hid, tracked weakly so destroyed actors never dangle. */
	TArray<TWeakObjectPtr<AActor>> HiddenActors;
	bool bActive = false;
};
