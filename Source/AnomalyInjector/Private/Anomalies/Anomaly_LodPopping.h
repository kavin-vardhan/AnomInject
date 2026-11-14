#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"
#include "AnomalyLodTriggerRule.h"

class AActor;
class UWorld;
class UMeshComponent;
class UStaticMeshComponent;

class FAnomaly_LodPopping final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("lod_popping")); }
	virtual FString GetDescription() const override { return TEXT("A static mesh shows a coarse LOD, then pops to full detail when the camera approaches it (distance_threshold) or turns toward it (view_entry). periodic = the old timer, bench only."); }
	virtual FString GetUsage() const override { return TEXT("<substring> [auto|distance_threshold|view_entry|periodic] [half_period_frames]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	virtual bool IsCurrentlyAnomalous() const override;
	virtual bool HasDeferredOnset() const override { return Mode != AnomalyLodTriggerRule::EMode::Periodic; }
	virtual int32 GetDeferredOnsetTimeoutFrames() const override;
	virtual bool IsHoldingEventOpen() const override;
	virtual void NoteCapturedFrame(bool bAnomalousThisFrame) override;
	virtual bool GetTelemetry(FAnomalyTelemetry& Out) const override;
	virtual bool WantsTargetLostNotification() const override { return bActive && Mode != AnomalyLodTriggerRule::EMode::Periodic; }
	virtual void OnTargetLost(AActor* Actor, bool bWorldEnding) override;
	virtual void OnWorldTeardown() override;
	virtual FString GetLastRefusalReason() const override { return LastRefusal; }

private:
	struct FPoppingTarget
	{
		TWeakObjectPtr<UMeshComponent> Mesh;
		int32 BaselineLod = 0;
		int32 PoppedLod = 0;
	};

	struct FTickEndEval
	{
		bool bValid = false;
		uint64 Frame = 0;
		FVector CameraLocation = FVector::ZeroVector;
		FRotator CameraRotation = FRotator::ZeroRotator;
		FVector BoundsOrigin = FVector::ZeroVector;
		bool bHeld = false;
		AnomalyLodTriggerRule::FFrameInput Input;
		AnomalyLodTriggerRule::FObservation Observation;
		bool bHoldOpen = false;
		bool bCapReached = false;
		float DistanceCm = -1.0f;
	};

	bool ApplyPeriodic(UWorld* World, const TArray<FString>& Args);
	bool ApplySpatial(UWorld* World, const FString& Substring, AnomalyLodTriggerRule::EMode Requested);
	const FTickEndEval& EvaluateNow() const;
	void ReleaseSpatial();
	void CommitPreviousFrame();

	TArray<FPoppingTarget> Targets;
	TArray<TWeakObjectPtr<const AActor>> ScopedOwners;
	FString LastRefusal;
	int32 HalfPeriodFrames = DefaultHalfPeriodFrames;
	int32 FramesSinceToggle = 0;
	bool  bPoppedPhase = false;
	bool  bActive = false;

	AnomalyLodTriggerRule::EMode Mode = AnomalyLodTriggerRule::EMode::DistanceThreshold;
	TWeakObjectPtr<UWorld> WorldRef;
	TWeakObjectPtr<UStaticMeshComponent> SpatialMesh;
	int32 SavedForcedLod = 0;
	uint64 AppliedFrame = 0;
	uint64 LastCommittedFrame = 0;
	AnomalyLodTriggerRule::FSettings Settings;
	AnomalyLodTriggerRule::FTracker Tracker;
	int32 NumLods = 0;
	int32 Triangles[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	float DistanceHistory[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	double TimeHistory[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int32 HistoryCount = 0;
	int32 SwapsRecorded = 0;
	bool bHoldOpenAtLastTickEnd = false;
	bool bLaggedRelease = false;
	mutable FTickEndEval Eval;

	static constexpr int32 DefaultHalfPeriodFrames = 8;
	static constexpr int32 MinHalfPeriodFrames = 1;
	static constexpr int32 MaxHalfPeriodFrames = 600;

	static constexpr float MinCoveragePct = 7.0f;
};
