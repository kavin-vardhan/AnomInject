#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Misc/EngineVersionComparison.h"

namespace AnomalyEngineCompat
{
	inline ERHIFeatureLevel::Type WorldFeatureLevel(const UWorld* World)
	{
#if UE_VERSION_OLDER_THAN(5, 3, 0)
		return World->FeatureLevel;
#else
		return World->GetFeatureLevel();
#endif
	}
}
