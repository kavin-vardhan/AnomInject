#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class UWorld;

class ANOMALYINJECTOR_API IAnomalyTextureAdapter
{
public:
	virtual ~IAnomalyTextureAdapter() = default;
	virtual bool Supports(FName Id) const = 0;
	virtual TUniquePtr<IAnomaly> Create(UWorld* World, FName Id) = 0;
};

namespace AnomalyCompat
{
	ANOMALYINJECTOR_API TUniquePtr<IAnomalyTextureAdapter> CreateTextureAdapter425();
}
