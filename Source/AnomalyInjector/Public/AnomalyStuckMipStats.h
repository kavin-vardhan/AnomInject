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
		int32 RefusedSharedWorld = 0;
		int32 RefusedBaselinePending = 0;
		int32 PurityEnumerations = 0;
		double PurityEnumerationMsMax = 0.0;
		int32 RestoreTrackedWhileBusy = 0;
		int32 PurityInactiveLevelUsers = 0;
		int32 PurityUnregisteredUsers = 0;
		int32 PurityLevelsScannedMax = 0;
		int32 HoldMonitorScans = 0;
		int32 HoldContaminations = 0;
	};

	ANOMALYINJECTOR_API void ResetRunStats();

	ANOMALYINJECTOR_API FRunStats GetRunStats();

	ANOMALYINJECTOR_API bool IsNoHoldLeverOn();

	ANOMALYINJECTOR_API bool IsUnlinkLockOn();

#if !UE_BUILD_SHIPPING
	ANOMALYINJECTOR_API void SetLegacyPurityLever(bool bOn);

	ANOMALYINJECTOR_API bool IsLegacyPurityLeverOn();
#endif
}
