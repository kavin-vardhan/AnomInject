#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class AActor;
class UWorld;
class UTexture2D;

class FAnomaly_StuckLowMip final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("stuck_low_mip")); }
	virtual FString GetDescription() const override { return TEXT("Texture held at a low resident mip on an actor's meshes (blurry object)."); }
	virtual FString GetUsage() const override { return TEXT("<substring> [mip_levels]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void TickAlways(float DeltaSeconds) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	virtual bool IsCurrentlyAnomalous() const override;
	virtual bool HasDeferredOnset() const override { return true; }
	virtual void NoteCapturedFrame(bool bAnomalousThisFrame) override;
	virtual bool GetTelemetry(FAnomalyTelemetry& Out) const override;

private:
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
		int32 ResidentAtOnset = -1;
		bool bUnlinked = false;
	};

	struct FRestoringTexture
	{
		TWeakObjectPtr<UTexture2D> Texture;
		FString TextureName;
		int32 BaselineResidentMips = 0;
		int32 FramesWaited = 0;
		int32 StreamInRequests = 0;
		int32 SkippedPending = 0;
		bool bTimeoutReported = false;
	};

	bool IsAwaitingRestore(const UTexture2D* Tex) const;

	TArray<FHeldTexture> Held;
	TArray<FRestoringTexture> Restoring;
	TWeakObjectPtr<AActor> PrimaryOwner;
	FString PrimaryOwnerName;
	int32 PrimaryIndex = 0;
	int32 CapturedFramesSeen = 0;
	int32 OnsetLatencyFrames = -1;
	bool bNoHoldLever = false;
	bool bStreamingDisabled = false;
	bool bUseAllMips = false;
	bool bActive = false;
};
