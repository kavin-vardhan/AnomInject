#pragma once

#include "CoreMinimal.h"

class AActor;
class UWorld;
class UMaterialInterface;

namespace AnomalyTexCorrupt
{
	struct FRunStats
	{
		int32 FiresApplied = 0;
		TMap<FString, int32> RefusedByReason;
		TMap<FString, int32> SlotDispositions;
		TMap<FString, int32> BindingDispositions;
		TMap<FString, int32> RollbackByStep;
		int64 RtBytesPeak = 0;
		int32 RestoredExact = 0;
		int32 RestoredDefault = 0;
		int32 LeftToGame = 0;
		int32 Swept = 0;
		int32 RtMipMismatch = 0;
		int32 CollateralDrops = 0;
		int32 CollateralIncompleteFrames = 0;
		int32 SlotsPartialSet = 0;
		int32 CopySourceChanged = 0;
		int32 ResidentChainOutputs = 0;
		int32 FiresWithSkippedParts = 0;
		int32 ComponentsSkipped = 0;
	};

	ANOMALYINJECTOR_API void ResetRunStats();

	ANOMALYINJECTOR_API FRunStats GetRunStats();

	ANOMALYINJECTOR_API const TArray<FString>& AllFinalReasons();

	ANOMALYINJECTOR_API const TArray<FString>& AllRollbackSteps();

	ANOMALYINJECTOR_API bool IsTexCorruptId(FName Id);

	ANOMALYINJECTOR_API bool IsEligibleTarget(UWorld* World, FName Id, AActor* Actor, FString& OutReason);

	ANOMALYINJECTOR_API TArray<FString> GetAutoDrawModes(FName Id);

	ANOMALYINJECTOR_API FString TargetedNoModeBenchLever(FName Id);

	ANOMALYINJECTOR_API bool IsRevertSettling(UWorld* World);

	ANOMALYINJECTOR_API void GatherCorruptorMaterials(UWorld* World, TArray<UMaterialInterface*>& Out);

	ANOMALYINJECTOR_API int32 BeginWarmDraw(UWorld* World);

	ANOMALYINJECTOR_API void EndWarmDraw();

	ANOMALYINJECTOR_API int32 KickUniformMeasurements(UWorld* World, const FString& TargetName);

	ANOMALYINJECTOR_API int32 NumUniformMeasurementsPending();

	ANOMALYINJECTOR_API FString DescribeUniformMeasurements();

	ANOMALYINJECTOR_API void RestoreBenchAssetSlotMid(const TCHAR* Context);
}
