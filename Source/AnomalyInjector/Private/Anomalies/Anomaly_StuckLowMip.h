#pragma once

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include "IAnomaly.h"
#include "UObject/ObjectKey.h"

class AActor;
class UActorComponent;
class ULevel;
class UWorld;
class UTexture2D;
namespace AnomalyTexCorrupt { class FAnomaly_TexCorrupt; }

class FAnomaly_StuckLowMip final : public IAnomaly
{
public:
	FAnomaly_StuckLowMip();
	virtual ~FAnomaly_StuckLowMip() override;

	bool HoldsOrRestores(const UTexture2D* Tex) const;

	bool IsRestoringAny() const { return Restoring.Num() > 0; }

	bool IsRestoringPastTimeout(int32 TimeoutFrames) const;

	virtual FName   GetId() const override { return FName(TEXT("stuck_low_mip")); }
	virtual FString GetDescription() const override { return TEXT("Texture held at a low resident mip on an actor's meshes (blurry object)."); }
	virtual FString GetUsage() const override { return TEXT("<substring> [mip_levels]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void TickAlways(float DeltaSeconds) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	virtual bool IsCurrentlyAnomalous() const override;
	virtual bool HasDeferredOnset() const override { return !bProxyRoute; }
	virtual bool UsesRenderResidencyTruth() const override { return !bProxyRoute; }
	virtual bool IsVisualConditionHeld() const override;
	virtual unsigned char GetVisualConditionState() const override;
	virtual bool GetRenderTruthTextures(TArray<FAnomalyRenderTruthTexture>& Out) const override;
	virtual bool GetRestoringRenderTruthTextures(TArray<FAnomalyRenderTruthTexture>& Out) const override;
	virtual bool ConsumeHoldContamination(FString& OutReason) override;
	virtual void NoteCapturedFrame(bool bAnomalousThisFrame) override;
	virtual bool GetTelemetry(FAnomalyTelemetry& Out) const override;
	virtual bool WantsTargetLostNotification() const override { return true; }
	virtual void OnTargetLost(AActor* Actor, bool bWorldEnding) override;
	virtual void OnWorldTeardown() override;
	virtual FString GetLastRefusalReason() const override { return LastRefusal; }

private:
	TUniquePtr<AnomalyTexCorrupt::FAnomaly_TexCorrupt> Proxy;
	bool bProxyRoute = false;
	struct FHeldTexture
	{
		TWeakObjectPtr<UTexture2D> Texture;
		FString TextureName;
		int32 SavedCinematicMips = 0;
		int32 AppliedCinematicMips = 0;
		int32 BaselineResidentMips = 0;
		int32 FullMips = 0;
		int32 FloorMips = 0;
		int32 TargetMips = 0;
		int32 PredictedMaxAllowedMips = 0;
		int32 TopResidentPxAtTarget = 0;
		int32 CoAffectedVisible = 0;
		int32 WorldUsers = -1;
		int32 ResidentAtOnset = -1;
		float RatioAtPick = -1.0f;
		uint64 BaselineResourceId = 0;
		bool bUnlinked = false;
	};

	struct FRestoringTexture
	{
		TWeakObjectPtr<UTexture2D> Texture;
		FString TextureName;
		int32 BaselineResidentMips = 0;
		int32 HeldResidentMips = 0;
		uint64 BaselineResourceId = 0;
		TWeakObjectPtr<AActor> Owner;
		FString OwnerName;
		int32 FramesWaited = 0;
		int32 StreamInRequests = 0;
		int32 SkippedPending = 0;
		int32 PlansRetireFrames = 0;
		bool bFenceRan = false;
		bool bTimeoutReported = false;
	};

	static int32 HeldLevelOf(const FHeldTexture& H);

	bool RunStreamerFence(int32& OutRetireFrames);
	bool bStreamerFencePending = false;

	bool IsAwaitingRestore(const UTexture2D* Tex) const;

	void ReleaseTargetWatch();

	void StartHoldMonitor(UWorld* World);
	void StopHoldMonitor();
	void ScanHoldForNewUsers(const TCHAR* Trigger);
	bool ConsiderHoldUser(UActorComponent* Component, const TSet<const ULevel*>& Loaded, const TCHAR* Trigger);
	void OnHoldLevelAdded(ULevel* Level, UWorld* World);
	void OnHoldRenderStateDirty(UActorComponent& Component);

	TSet<FObjectKey> HoldKnownComponents;
	TArray<TWeakObjectPtr<UActorComponent>> HoldUnregisteredWatch;
	TArray<TWeakObjectPtr<UActorComponent>> HoldDirtyRecheck;
	FCriticalSection HoldDirtyCS;
	FDelegateHandle HoldLevelAddedHandle;
	FDelegateHandle HoldRenderDirtyHandle;
	bool bHoldMonitorOn = false;
	bool bContaminationPending = false;
	FString ContaminationReason;

	TArray<FHeldTexture> Held;
	TArray<FRestoringTexture> Restoring;
	TArray<TWeakObjectPtr<AActor>> HeldOwners;
	TWeakObjectPtr<UWorld> HeldWorld;
	TWeakObjectPtr<AActor> PrimaryOwner;
	FString PrimaryOwnerName;
	FString LastRefusal;
	int32 PrimaryIndex = 0;
	int32 CapturedFramesSeen = 0;
	int32 OnsetLatencyFrames = -1;
	bool bNoHoldLever = false;
	bool bStreamingDisabled = false;
	bool bUseAllMips = false;
	bool bActive = false;
};
