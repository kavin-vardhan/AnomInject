#pragma once

#include "CoreMinimal.h"
#include "AnomalyLodPolicy.h"

class UWorld;
class UMeshComponent;

namespace AnomalyLod
{
	static constexpr int32 WorstLodSentinel = -1;

	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<UMeshComponent>> ResolveLodComponents(UWorld* World, const FString& Substring);

	ANOMALYINJECTOR_API int32 GetWorstLod(const UMeshComponent* Component);

	ANOMALYINJECTOR_API bool HasMultipleLods(const UMeshComponent* Component);

	struct FLodFacts
	{
		int32 NumLods = 0;
		int32 MinLod = 0;
		int32 FirstUsable = 0;
		int32 UsableCount = 0;
		int32 FirstResident = 0;
		bool bResidentKnown = false;
		const TCHAR* MinLodSource = TEXT("none");
	};

	ANOMALYINJECTOR_API FLodFacts GetLodFacts(const UMeshComponent* Component);

	ANOMALYINJECTOR_API int32 GetUsableLodCount(const UMeshComponent* Component);

	ANOMALYINJECTOR_API FString DescribeLodFacts(const FLodFacts& Facts);

	ANOMALYINJECTOR_API AnomalyLodPolicy::EForcedLodRefusal GetForcedLodRefusal(const UMeshComponent* Component);

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

	struct FForcedLodContrast
	{
		AnomalyLodPolicy::EForcedLodRefusal Refusal = AnomalyLodPolicy::EForcedLodRefusal::None;
		bool bCurrentKnown = false;
		int32 ShownLod = -1;
		int32 ForcedLod = -1;
		float ScreenSize = -1.0f;
		const TCHAR* Source = TEXT("undetermined");
	};

	ANOMALYINJECTOR_API FForcedLodContrast GetForcedLodContrast(UWorld* World, const UMeshComponent* Component, int32 ForcedLodModel);
}
