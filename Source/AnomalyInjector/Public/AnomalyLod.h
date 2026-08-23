#pragma once

#include "CoreMinimal.h"

class UWorld;
class UMeshComponent;

namespace AnomalyLod
{
	static constexpr int32 WorstLodSentinel = -1;

	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<UMeshComponent>> ResolveLodComponents(UWorld* World, const FString& Substring);

	ANOMALYINJECTOR_API int32 GetWorstLod(const UMeshComponent* Component);

	ANOMALYINJECTOR_API bool HasMultipleLods(const UMeshComponent* Component);

	ANOMALYINJECTOR_API int32 GetForcedLod(const UMeshComponent* Component);

	ANOMALYINJECTOR_API void SetForcedLod(UMeshComponent* Component, int32 LodIndex);

	ANOMALYINJECTOR_API int32 ResolveTargetLod(const UMeshComponent* Component, int32 RequestedOrSentinel);

	struct FCurrentLod
	{
		int32 Level = -1;
		float ScreenSize = -1.0f;
		const TCHAR* Source = TEXT("undetermined");
		bool bKnown = false;
	};

	ANOMALYINJECTOR_API FCurrentLod GetCurrentLod(UWorld* World, const UMeshComponent* Component);
}
