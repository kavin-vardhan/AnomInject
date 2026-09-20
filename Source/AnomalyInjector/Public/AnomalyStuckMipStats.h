#pragma once

#include "CoreMinimal.h"

namespace AnomalyStuckMip
{
	struct FRunStats
	{
		int32 FiresApplied = 0;
		int32 TexturesHeld = 0;
		int32 RefusedNoEligibleTextures = 0;
		int32 RefusedVirtual = 0;
		int32 RefusedNotStreamable = 0;
		int32 RefusedShared = 0;
		int32 RefusedImperceptible = 0;
		int32 CandidateTexturesSeen = 0;
	};

	ANOMALYINJECTOR_API void ResetRunStats();

	ANOMALYINJECTOR_API FRunStats GetRunStats();

	ANOMALYINJECTOR_API bool IsNoHoldLeverOn();

	ANOMALYINJECTOR_API bool IsUnlinkLockOn();
}
