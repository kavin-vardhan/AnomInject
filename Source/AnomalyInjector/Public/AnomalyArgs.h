#pragma once

#include "CoreMinimal.h"

namespace AnomalyArgs
{
	ANOMALYINJECTOR_API float GetFloat(const TArray<FString>& Args, int32 Index, float Default, float Min, float Max);

	ANOMALYINJECTOR_API int32 GetInt(const TArray<FString>& Args, int32 Index, int32 Default, int32 Min, int32 Max);

	ANOMALYINJECTOR_API FString GetString(const TArray<FString>& Args, int32 Index, const FString& Default);
}
