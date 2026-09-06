#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"
#include "UObject/StrongObjectPtr.h"

class AActor;
class UWorld;
class UMeshComponent;
class UMaterialInterface;

class FAnomaly_CorruptedTexture final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("corrupted_texture")); }
	virtual FString GetDescription() const override { return TEXT("Corrupted-texture look (solid pink) on an actor's meshes."); }
	virtual FString GetUsage() const override { return TEXT(""); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	virtual bool IsVisualConditionHeld() const override;

private:
	struct FCapturedSlot
	{
		TWeakObjectPtr<UMeshComponent> Mesh;
		TWeakObjectPtr<AActor> Owner;
		FName ComponentName = NAME_None;
		int32 SlotIndex = 0;
		TStrongObjectPtr<UMaterialInterface> OriginalMaterial;
		bool bWasExplicitOverride = false;
	};
	TArray<FCapturedSlot> Captured;
	TWeakObjectPtr<UMaterialInterface> AppliedPink;
	bool bActive = false;
};
