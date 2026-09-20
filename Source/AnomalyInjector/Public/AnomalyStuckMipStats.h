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
		int32 RefusedTooSmallForRatio = 0;
		int32 RefusedNotRestored = 0;
		int32 RefusedAlreadyHeld = 0;
		int32 CandidateTexturesSeen = 0;
		int32 RestoreTimeouts = 0;
		int32 RestoreFramesMax = 0;
		int32 RestoreStreamInReissues = 0;
		int32 RestoreSkippedPending = 0;
		int32 TexturesAwaitingRestore = 0;
		int32 RevertOnDestroy = 0;
		int32 UnverifiedAtTeardown = 0;
	};

	ANOMALYINJECTOR_API void ResetRunStats();

	ANOMALYINJECTOR_API FRunStats GetRunStats();

	ANOMALYINJECTOR_API bool IsNoHoldLeverOn();

	ANOMALYINJECTOR_API bool IsUnlinkLockOn();
}
