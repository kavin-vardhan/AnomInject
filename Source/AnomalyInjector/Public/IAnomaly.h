#pragma once

#include "CoreMinimal.h"

class UWorld;

class IAnomaly
{
public:
	virtual ~IAnomaly() = default;

	virtual FName GetId() const = 0;

	virtual FString GetDescription() const = 0;

	virtual FString GetUsage() const = 0;

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) = 0;

	virtual void Tick(float DeltaSeconds) {}

	virtual void Revert() = 0;

	virtual bool IsActive() const = 0;

	virtual bool IsCurrentlyAnomalous() const { return IsActive(); }
};
