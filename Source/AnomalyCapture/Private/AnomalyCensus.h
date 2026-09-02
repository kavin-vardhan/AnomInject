#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "UObject/WeakObjectPtr.h"
#include "AnomalyCensusProvider.h"

class AActor;
class UWorld;
class FAnomalyMaskSceneViewExtension;
struct FAnomalyStencilTagLedger;

enum class EAnomalyCensusVerdict : uint8
{
	NotYetMeasured = 0,
	MeasuredZero,
	MeasuredNonZero,
	NotMeasurableNanite,
	NotMeasurableTagFailed,
	NotMeasurableHidden,
	ExcludedTranslucent
};

const TCHAR* LexToStringAnomalyCensusVerdict(EAnomalyCensusVerdict V);

struct FAnomalyCensusEntry
{
	TWeakObjectPtr<AActor> Actor;
	FString ActorName;
	EAnomalyCensusVerdict Verdict = EAnomalyCensusVerdict::NotYetMeasured;
	int32 DrawnPx = 0;
	int32 FramePx = 0;
	uint64 MeasuredAtTick = 0;
	int32 TimesMeasured = 0;
	int32 AttemptsThisCycle = 0;
};

struct FAnomalyCensusParams
{
	float FloorPct = 0.5f;
	float CeilingPct = 25.0f;
	int32 MaxVerdictAgeTicks = 12;
	bool bExcludeTranslucent = true;
	bool bLeakProbe = false;
	bool bCoArmOnly = false;
};

struct FAnomalyCensusCounters
{
	int32 CensusFrames = 0;
	int32 Cycles = 0;
	int32 Candidates = 0;
	int32 Zero = 0;
	int32 NonZero = 0;
	int32 BelowFloor = 0;
	int32 AboveCeiling = 0;
	int32 ExcludedTranslucent = 0;
	int32 UnmeasurableNanite = 0;
	int32 UnmeasurableTagFailed = 0;
	int32 UnmeasurableHidden = 0;
	int32 NotYetMeasured = 0;
	int32 FiresFallbackAll = 0;
	int32 FramesNoPass = 0;
	int32 FramesPolluted = 0;
	int32 BatchesLost = 0;
	int32 TagOvertaken = 0;
	int32 ProxyRecreatesQueued = 0;
	double TagBlockMsTotal = 0.0;
};

class FAnomalyCensus
{
public:
	static constexpr int32 LostAfterTicks = 8;
	static constexpr int32 MaxAttemptsPerCycle = 3;
	static constexpr int32 MaxInFlightBatches = 2;
	static constexpr int32 CycleListingCap = 512;

	void Begin(UWorld* World, FAnomalyStencilTagLedger* InLedger, const FAnomalyCensusParams& InParams);
	void End(UWorld* World);
	bool IsActive() const { return bActive; }

	void Tick(UWorld* World, FAnomalyMaskSceneViewExtension* Sve, bool bEventArmedThisTick, const TSet<uint8>& EventTags);

	const FAnomalyCensusCounters& GetCounters() const { return Counters; }
	const TArray<FAnomalyCensusEntry>& GetEntries() const { return Entries; }
	TSet<uint8> GetInFlightTags() const;
	TSet<uint8> GetLegitTags() const;
	bool ConsumeCycleJustCompleted();

	FAnomalyCensusOpinion QueryActor(const AActor* Actor) const;
	bool IsCeilingEnabled() const { return Params.CeilingPct > 0.0f; }
	bool HasCompletedACycle() const { return CycleNumber > 0 && Counters.Cycles > 0; }
	void NoteFireAllFallback(bool bAllFallback);

private:
	struct FBatch
	{
		uint64 RequestId = 0;
		TArray<int32> EntryIdx;
		TArray<uint8> Tags;
		TSet<uint8> CensusTagsAtArm;
		uint64 ArmedAtTick = 0;
		int32 PendingBefore = 0;
	};

	void StartCycle(UWorld* World);
	void CollectBatches(FAnomalyMaskSceneViewExtension* Sve, const TSet<uint8>& EventTags);
	void CreditEntryFromResult(int32 EntryIndex, uint8 Tag, const struct FAnomalyMaskResult& Result);
	void ReleaseBatch(FBatch& Batch, bool bRequeue);
	void ArmNextBatch(FAnomalyMaskSceneViewExtension* Sve, const TSet<uint8>& EventTags);
	void CloseCycle();

	bool bActive = false;
	bool bCycleOpen = false;
	bool bCycleJustCompleted = false;
	bool bLeakProbeFired = false;
	FAnomalyCensusParams Params;
	FAnomalyStencilTagLedger* Ledger = nullptr;
	FAnomalyCensusCounters Counters;
	TArray<FAnomalyCensusEntry> Entries;
	TArray<int32> CycleQueue;
	TArray<FBatch> InFlight;
	TArray<TPair<uint8, uint64>> RecentlyReleased;
	TWeakObjectPtr<UWorld> WorldPtr;
	uint64 CensusIdSerial = 0;
	uint64 CycleStartTick = 0;
	int32 HalfCap = 1;
	int32 CycleNumber = 0;
	double CycleStartTagBlockMs = 0.0;
	int32 CycleStartFlips = 0;
};

#endif
