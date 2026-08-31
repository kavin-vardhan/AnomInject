#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtr.h"

class AActor;
class UWorld;
class UPrimitiveComponent;

struct FAnomalyStencilTagLedger
{
	TSet<uint8> HostReserved;
	TSet<uint8> EventClaimed;
	TSet<uint8> CensusClaimed;

	bool IsAssignable(uint8 Value) const;
	bool IsFree(uint8 Value) const;
	int32 NumFree() const;
	int32 NumAssignable() const;
	void Reset();
};

namespace AnomalyStencilTag
{
	static constexpr int32 ReservedStencilBase = 200;
	static constexpr int32 ReservedStencilMax = 255;
	static constexpr int32 AssignableStencilMax = 254;

	int32 TagActor(AActor* Actor, int32 StencilValue);
	int32 TagActor(AActor* Actor, int32 StencilValue, int32* OutFlagFlips);
	void RestoreActor(AActor* Actor);
	void RestoreAll();
	bool IsAnyTagged();
	bool IsAnyComponentTagged(const AActor* Actor);

	bool VerifyActorStillTagged(const AActor* Actor, int32 StencilValue, FString& OutDetail);

	FString ForgetOneTaggedComponentOfActor(AActor* Actor);

	void GetTaggedComponents(TSet<TWeakObjectPtr<UPrimitiveComponent>>& Out);

	TSet<uint8> SnapshotHostReservedValues(UWorld* World);

	void SnapshotCustomDepthEnabled(UWorld* World, TMap<TWeakObjectPtr<UPrimitiveComponent>, int32>& Out);
	int32 DiffCustomDepthSnapshots(
		const TMap<TWeakObjectPtr<UPrimitiveComponent>, int32>& Before,
		const TMap<TWeakObjectPtr<UPrimitiveComponent>, int32>& After,
		const TSet<TWeakObjectPtr<UPrimitiveComponent>>* Exclude,
		FString& OutFirstDiff);

	void EnableCustomStencil();
	void DisableCustomStencil();
}
