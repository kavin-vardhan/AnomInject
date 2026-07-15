#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class AActor;
class UWorld;
class UMeshComponent;
class UMaterialInterface;

class FAnomaly_MissingTexture final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("missing_texture")); }
	virtual FString GetDescription() const override { return TEXT("Missing-texture look (UV checker) on an actor's meshes."); }
	virtual FString GetUsage() const override { return TEXT(""); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }

private:
	struct FCapturedSlot
	{
		TWeakObjectPtr<UMeshComponent> Mesh;
		TWeakObjectPtr<AActor> Owner;
		FName ComponentName = NAME_None;
		int32 SlotIndex = 0;
		TWeakObjectPtr<UMaterialInterface> OriginalMaterial;
		bool bWasExplicitOverride = false;
	};
	TArray<FCapturedSlot> Captured;
	TWeakObjectPtr<UMaterialInterface> AppliedChecker;
	bool bActive = false;
};
