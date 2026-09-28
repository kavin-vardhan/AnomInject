#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "AnomalyMaskTypes.h"
#include "UObject/WeakObjectPtr.h"

class AActor;
class FAnomalyMaskSceneViewExtension;
struct FAnomalyStencilTagLedger;

struct FAnomalyMaskRecord
{
	FName Id = NAME_None;
	FString Target;
	uint64 StartFrame = 0;

	TWeakObjectPtr<AActor> TargetActor;
	uint8 Tag = 0;

	EAnomalyMaskState State = EAnomalyMaskState::NotMeasured;
	int32 MaxCount = 0;
	int32 ViewportPixels = 0;

	int32 ArmsIssued = 0;
	int32 ArmsResolved = 0;
	int32 SkippedHidden = 0;
	int32 CollisionHits = 0;
	int32 FramesDiscarded = 0;
	int32 FramesResidualDiscarded = 0;
	int32 FramesUnconfirmed = 0;
	int32 FramesNoPass = 0;
	int32 FramesContributed = 0;
	int32 ProbeArms = 0;
	FString FirstCollisionDetail;
	bool bTagFailed = false;

	bool bKnownUnmeasurable = false;
	FString UnmeasurableReason;

	bool bAwaitLabelled = false;
	bool bLabelledThisTick = false;
	int32 ArmsDeferred = 0;
	TArray<int32> PerArmCounts;

	bool bReleasable = false;
	int64 ReleasableSinceTick = -1;
	bool bTagRecycled = false;
};

struct FAnomalyTagReleaseExternal
{
	bool bFireLive = false;
	bool bTrailBlocks = false;
	bool bInPendingSnapshot = false;
	bool bInPendingTargetMask = false;
};

class FAnomalyMaskMeasure
{
public:
	static constexpr int32 MaxArmsPerEvent = 4;

	void BeginRun(FAnomalyStencilTagLedger* InLedger = nullptr);
	void EndRun();

	FAnomalyMaskRecord* FindOrAddRecord(FName Id, const FString& Target, uint64 StartFrame, AActor* TargetActor);
	FAnomalyMaskRecord* FindRecord(FName Id, const FString& Target, uint64 StartFrame);

	void SetArmWindowGate(bool bInGate);
	bool IsArmWindowGateOn() const { return bArmWindowGate; }
	void ClearLabelledThisTick();
	void SetRecordLabelledThisTick(FName Id, const FString& Target, uint64 StartFrame, bool bLabelled);

	bool ArmIfMeasurable(FAnomalyMaskSceneViewExtension* Sve, uint64 RequestId, bool bWantPixels = false);
	bool ArmProbeOnHidden(FAnomalyMaskSceneViewExtension* Sve, uint64 RequestId);
	void VerifyPendingTags();
	void CollectResults(FAnomalyMaskSceneViewExtension* Sve);
	void SampleEndOfFrame();
	void UntagAll();

	const TArray<FAnomalyMaskRecord>& GetRecords() const { return Records; }
	TSet<uint8> BuildAssignedTagSet() const;
	TSet<uint8> BuildBaseTagSet() const;
	void SetExtraAssignedTags(const TSet<uint8>& InExtra) { ExtraAssignedTags = InExtra; }
	int32 NumUnmeasured() const;
	int32 NumKnownUnmeasurable() const;
	int32 TotalProbeArms() const;
	int32 TotalResidualDiscards() const;
	int32 TotalNoPassDiscards() const;

	int32 RefreshTagReleasability(TFunctionRef<FAnomalyTagReleaseExternal(const FAnomalyMaskRecord&)> External, uint64 Tick);
	int32 GetTagRecycles() const { return TagRecycles; }
	int32 GetTagPeakLive() const { return TagPeakLive; }
	int32 GetTagExhausted() const { return TagExhausted; }
	int32 GetTagRetireQuarantined() const { return TagRetireQuarantined; }
	int32 GetTagRetireHostFlagKept() const { return TagRetireHostFlagKept; }

private:
	int32 AllocateTag(FName ForId, const FString& ForTarget, uint64 ForStartFrame);
	int32 ReclaimReleasableTag(FName ForId, const FString& ForTarget, uint64 ForStartFrame);
	bool IsRecordArmInFlight(int32 Index) const;

	TArray<FAnomalyMaskRecord> Records;
	TMap<uint64, int32> ArmedRequestToRecord;
	TSet<uint64> PollutedRequests;
	TSet<uint64> ProbeRequests;
	TMap<uint64, uint8> EndFrameSample;
	TArray<uint64> ArmedThisFrame;
	TSet<uint8> ExtraAssignedTags;
	FAnomalyStencilTagLedger* Ledger = nullptr;
	int32 NextTagOffset = 0;
	bool bArmWindowGate = true;
	int32 TagRecycles = 0;
	int32 TagPeakLive = 0;
	int32 TagExhausted = 0;
	int32 TagRetireQuarantined = 0;
	int32 TagRetireHostFlagKept = 0;
};

#endif
