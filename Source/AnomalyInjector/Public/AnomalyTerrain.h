#pragma once

#include "CoreMinimal.h"

class AActor;

namespace AnomalyTerrain
{
	struct FTerrainVerdict
	{
		bool bTerrain = false;
		bool bEngineLandscape = false;
		bool bRefuseBlink = false;
		FString Rule;
	};

	ANOMALYINJECTOR_API FTerrainVerdict Classify(const AActor* Actor);

	ANOMALYINJECTOR_API bool IsClassRuleActive();
	ANOMALYINJECTOR_API FString DescribeClassRule();
	ANOMALYINJECTOR_API FString DescribeRules();

	ANOMALYINJECTOR_API bool IsBlinkRefusalEnabled();

	ANOMALYINJECTOR_API void ResetRunStats();
	ANOMALYINJECTOR_API void NoteUnlabelableRefusal(const AActor* Actor, const FString& Rule);
	ANOMALYINJECTOR_API int32 GetUnlabelableRefusalCount();

	ANOMALYINJECTOR_API FString TerrainBlinkSubtype();
	ANOMALYINJECTOR_API FString DefaultBlinkSubtype();
	ANOMALYINJECTOR_API FString UnlabelableReason();
}
