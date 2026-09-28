#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "AnomalyTelemetry.h"

class AActor;
class UWorld;
class UTexture2D;

struct FAnomalyRenderTruthTexture
{
	TWeakObjectPtr<UTexture2D> Texture;
	FString Name;
	int32 BaselineResidentMips = 0;
	uint64 BaselineResourceId = 0;
	TWeakObjectPtr<AActor> Owner;
	FString OwnerName;
};

class IAnomaly
{
public:
	virtual ~IAnomaly() = default;

	virtual FName GetId() const = 0;

	virtual FString GetDescription() const = 0;

	virtual FString GetUsage() const = 0;

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) = 0;

	virtual void Tick(float DeltaSeconds) {}

	virtual void TickAlways(float DeltaSeconds) {}

	virtual void Revert() = 0;

	virtual bool IsActive() const = 0;

	virtual bool IsCurrentlyAnomalous() const { return IsActive(); }

	virtual bool IsVisualConditionHeld() const { return IsCurrentlyAnomalous(); }

	virtual bool HasDeferredOnset() const { return false; }

	virtual bool UsesRenderResidencyTruth() const { return false; }

	virtual bool GetRenderTruthTextures(TArray<FAnomalyRenderTruthTexture>& Out) const { return false; }

	virtual bool GetRestoringRenderTruthTextures(TArray<FAnomalyRenderTruthTexture>& Out) const { return false; }

	virtual bool ConsumeHoldContamination(FString& OutReason) { return false; }

	virtual void NoteCapturedFrame(bool bAnomalousThisFrame) {}

	virtual bool GetTelemetry(FAnomalyTelemetry& Out) const { return false; }

	virtual bool WantsTargetLostNotification() const { return false; }

	virtual void OnTargetLost(AActor* Actor, bool bWorldEnding) {}

	virtual void OnWorldTeardown() {}
};
