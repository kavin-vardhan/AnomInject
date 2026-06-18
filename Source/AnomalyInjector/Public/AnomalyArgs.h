// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Shared argument parsing (M2 / A3). Consolidates the ad-hoc parse/clamp/warn "behavior
 * flicker" established in AMB-6 (flicker Hz) so every anomaly handles its args identically:
 *   - missing index      -> Default (silent; an omitted optional arg is normal)
 *   - non-numeric token  -> warn + Default
 *   - out-of-range value -> warn + clamp (never fails Apply)
 * All warnings go to LogAnomaly. Consumers: lighting_mismatch, lod_corruption, camera_clipping.
 */
namespace AnomalyArgs
{
	/** Parse Args[Index] as a float, clamped to [Min, Max]. See namespace doc for the contract. */
	ANOMALYINJECTOR_API float GetFloat(const TArray<FString>& Args, int32 Index, float Default, float Min, float Max);

	/** Parse Args[Index] as an int32, clamped to [Min, Max]. See namespace doc for the contract. */
	ANOMALYINJECTOR_API int32 GetInt(const TArray<FString>& Args, int32 Index, int32 Default, int32 Min, int32 Max);

	/** Return Args[Index] verbatim, or Default if the index is absent. No validation/clamp. */
	ANOMALYINJECTOR_API FString GetString(const TArray<FString>& Args, int32 Index, const FString& Default);
}
