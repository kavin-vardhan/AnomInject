#pragma once

#include "CoreMinimal.h"
#include "AnomalyLodPolicy.h"

class UWorld;
class UMeshComponent;
class UStaticMeshComponent;
class FSceneView;
class FSceneViewFamilyContext;
class FStaticMeshRenderData;

namespace AnomalyLod
{
	static constexpr int32 MaxLodEntries = 8;

	struct FEngineLodPrediction
	{
		bool bValid = false;
		int32 EngineLod = -1;
		int32 TemporalLod0 = -1;
		int32 TemporalLod1 = -1;
		float BoundsScreenSize = -1.0f;
		float Scale = 1.0f;
		int32 MinLod = 0;
		int32 NumLods = 0;
		const TCHAR* Refusal = TEXT("");
	};

	class ANOMALYINJECTOR_API FLodViewScope
	{
	public:
		explicit FLodViewScope(UWorld* World);
		~FLodViewScope();
		FLodViewScope(const FLodViewScope&) = delete;
		FLodViewScope& operator=(const FLodViewScope&) = delete;
		const FSceneView* GetView() const { return View; }
		bool IsValid() const { return View != nullptr; }

	private:
		TUniquePtr<FSceneViewFamilyContext> Family;
		const FSceneView* View = nullptr;
	};

	ANOMALYINJECTOR_API bool IsForceLodCvarActive();

	ANOMALYINJECTOR_API float GetStaticMeshLodDistanceScale();

	ANOMALYINJECTOR_API FEngineLodPrediction PredictEngineLod(const FSceneView* View, const UStaticMeshComponent* Component);

	ANOMALYINJECTOR_API FEngineLodPrediction PredictEngineLod(UWorld* World, const UStaticMeshComponent* Component);

	ANOMALYINJECTOR_API int32 GetLodTriangleCount(const UStaticMeshComponent* Component, int32 LodIndex);

	ANOMALYINJECTOR_API bool GetRenderProbeInputs(const UStaticMeshComponent* Component, const FStaticMeshRenderData*& OutData,
		int32& OutMinLod, int32& OutNumLods, float& OutFactorScale);

	ANOMALYINJECTOR_API int32 GetLodScreenSizes(const UStaticMeshComponent* Component, float (&OutSizes)[MaxLodEntries],
		int32 (&OutTriangles)[MaxLodEntries]);

	static constexpr int32 WorstLodSentinel = -1;

	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<UMeshComponent>> ResolveLodComponents(UWorld* World, const FString& Substring);

	ANOMALYINJECTOR_API int32 GetWorstLod(const UMeshComponent* Component);

	ANOMALYINJECTOR_API bool HasMultipleLods(const UMeshComponent* Component);

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
}
