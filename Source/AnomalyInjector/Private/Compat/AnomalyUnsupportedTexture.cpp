#include "Compat/AnomalyEngineVersion.h"
#if ANOMALY_ENGINE_425
#include "AnomalyTexCorrupt.h"
#include "AnomalyStuckMipStats.h"
#include "UObject/WeakObjectPtr.h"

namespace AnomalyTexCorrupt
{
	void ResetRunStats() {}
	FRunStats GetRunStats() { return FRunStats(); }
	const TArray<FString>& AllFinalReasons() { static const TArray<FString> Values = { TEXT("unsupported_engine_phase_a") }; return Values; }
	const TArray<FString>& AllRollbackSteps() { static const TArray<FString> Values; return Values; }
	bool IsTexCorruptId(FName Id) { return Id == FName(TEXT("uv_corruption")) || Id == FName(TEXT("normal_corruption")); }
	bool IsEligibleTarget(UWorld*, FName, AActor*, FString& OutReason) { OutReason = TEXT("unsupported_engine_phase_a"); return false; }
	TArray<FString> GetAutoDrawModes(FName) { return TArray<FString>(); }
	FString TargetedNoModeBenchLever(FName) { return FString(); }
	bool IsRevertSettling(UWorld*) { return false; }
	void GatherCorruptorMaterials(UWorld*, TArray<UMaterialInterface*>& Out) { Out.Reset(); }
	int32 BeginWarmDraw(UWorld*) { return -1; }
	void EndWarmDraw() {}
	int32 KickUniformMeasurements(UWorld*, const FString&) { return 0; }
	int32 NumUniformMeasurementsPending() { return 0; }
	FString DescribeUniformMeasurements() { return TEXT("unsupported_engine_phase_a"); }
	void RestoreBenchAssetSlotMid(const TCHAR*) {}
}

namespace AnomalyStuckMip
{
	void ResetRunStats() {}
	FRunStats GetRunStats() { return FRunStats(); }
	void GatherPurityActors(UWorld*, TArray<TWeakObjectPtr<AActor>>& Out) { Out.Reset(); }
	void CountActorTextureUsers(AActor*, TMap<UTexture2D*, int32>& Out) { Out.Reset(); }
	void InspectHold(AActor*, const TMap<UTexture2D*, int32>&, bool& bOutShared, bool& bOutEligible, TArray<FString>& OutReasons)
	{
		bOutShared = false;
		bOutEligible = false;
		OutReasons.Reset();
		OutReasons.Add(TEXT("unsupported_engine_phase_a"));
	}
	bool IsNoHoldLeverOn() { return false; }
	bool IsUnlinkLockOn() { return false; }
	bool IsTextureHeldOrRestoring(const UTexture2D*) { return false; }
#if !UE_BUILD_SHIPPING
	void SetLegacyPurityLever(bool) {}
	bool IsLegacyPurityLeverOn() { return false; }
#endif
}
#endif
