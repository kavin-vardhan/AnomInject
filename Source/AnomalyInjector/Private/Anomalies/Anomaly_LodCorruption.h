#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class AActor;
class UWorld;
class UMeshComponent;

class FAnomaly_LodCorruption final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("lod_corruption")); }
	virtual FString GetDescription() const override { return TEXT("Force matching static or skeletal mesh components to a corrupted LOD (default worst)."); }
	virtual FString GetUsage() const override { return TEXT("<substring> [lod-index]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	virtual FString GetLastRefusalReason() const override { return LastRefusal; }

private:
	struct FCapturedLod
	{
		TWeakObjectPtr<UMeshComponent> Mesh;
		int32 PrevForcedLodModel = 0;
	};

	TArray<FCapturedLod> Captured;
	TArray<TWeakObjectPtr<const AActor>> ScopedOwners;
	FString LastRefusal;
	bool bActive = false;
};
