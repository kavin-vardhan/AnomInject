#pragma once

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#if __has_include("Templates/SharedPointerFwd.h")
#include "Templates/SharedPointerFwd.h"
#endif
#include "Templates/SharedPointerInternals.h"

#if ANOMALY_CAPTURE
#include "PixelFormat.h"
#include "AnomalyViewport.h"

class FAnomalyChangeStage;
class FJsonObject;
class FRenderTarget;
class FSceneInterface;

enum class EAnomalyChangeReason : uint8
{
	None, FirstFrame, PredecessorMissing, PredecessorUndelivered, OutOfOrderTimeout,
	EpochReset, ViewMismatch, ExtentMismatch, MaskPayloadMissing, UnsupportedDelivery,
	BudgetExceeded, EmptyRegion, NoLabelledFrames, CurrentUndelivered, ClosureTimeout
};
const TCHAR* AnomalyChangeReasonName(EAnomalyChangeReason Reason);

struct FAnomalyChangeIssue
{
	uint64 RunEpoch = 0;
	uint64 CutCounter = 0;
	uint64 CaptureToken = 0;
	uint64 RequestId = 0;
	int32 SessionIndex = -1;
	int64 SubmitMs = 0;
	FString FrameFile;
	FAnomalyViewInfo Camera;
	const FRenderTarget* OwnerTarget = nullptr;
	const FSceneInterface* OwnerScene = nullptr;
	const void* OwnerLevel = nullptr;
	bool bSupported = true;
	TWeakPtr<FAnomalyChangeStage, ESPMode::ThreadSafe> Stage;
};
using FAnomalyChangeIssuePtr = TSharedPtr<const FAnomalyChangeIssue, ESPMode::ThreadSafe>;

struct FAnomalyChangeReceipt
{
	FAnomalyChangeIssuePtr Issue;
	uint64 ServingToken = 0;
	uint64 ViewFamilyId = 0;
	uint32 FamilyFrame = 0;
	int32 ViewIndex = -1;
	FIntRect Rect = FIntRect(0, 0, 0, 0);
	FIntPoint Extent = FIntPoint::ZeroValue;
	EPixelFormat Format = PF_Unknown;
	int64 DrainMs = 0;
	uint64 MaskRequestId = 0;
	uint64 PayloadOwnerRequestId = 0;
	TArray<uint64> ServedRequestIds;
};
using FAnomalyChangeReceiptPtr = TSharedPtr<const FAnomalyChangeReceipt, ESPMode::ThreadSafe>;
using FAnomalyChangeColourPtr = TSharedPtr<const TArray<FColor>, ESPMode::ThreadSafe>;
using FAnomalyChangeMaskPtr = TSharedPtr<const TArray<uint8>, ESPMode::ThreadSafe>;

struct FAnomalyChangeCompletion
{
	FAnomalyChangeReceiptPtr Receipt;
	bool bDelivered = false;
	bool bMask = false;
	FString Stage;
};

struct FAnomalyChangeLabel
{
	FString Event, Type, Target;
	int32 Tag = 0, Phase = -1, Window = -1;
	bool bLabelled = false;
};

class FAnomalyChangeStage : public TSharedFromThis<FAnomalyChangeStage, ESPMode::ThreadSafe>
{
public:
	static bool IsEnabled();
	explicit FAnomalyChangeStage(const FString& InRunDir);
	FAnomalyChangeIssuePtr Issue(uint64 RequestId, int32 SessionIndex, const FAnomalyViewInfo& Camera,
		const FRenderTarget* OwnerTarget, const FSceneInterface* OwnerScene, const void* OwnerLevel,
		const FString& FrameFile, bool bSupported);
	FAnomalyChangeIssuePtr FindIssue(int32 SessionIndex) const;
	void ExpectMask(int32 SessionIndex);
	void SealArm(int32 SessionIndex);
	bool AcceptGeneration(const FAnomalyChangeIssuePtr& InIssue, const TCHAR* Path);
	void Diagnostic(const TCHAR* Name, int64 Amount = 1);
	void Colour(const FAnomalyChangeCompletion& Completion, const FAnomalyChangeColourPtr& Pixels, bool bSupported);
	void Mask(const FAnomalyChangeReceiptPtr& Receipt, const FAnomalyChangeMaskPtr& Pixels, bool bEmpty,
		const TMap<uint8, int32>& Counts = TMap<uint8, int32>());
	void Observe(int32 SessionIndex, const TArray<FAnomalyChangeLabel>& Labels);
	void EndEvents(const TCHAR* Cause);
	void CompleteMask(const FAnomalyChangeCompletion& Completion);
	void Fail(const FAnomalyChangeIssuePtr& InIssue, EAnomalyChangeReason Reason, const TCHAR* FailureStage);
	void Unsupported(const FAnomalyChangeIssuePtr& InIssue, bool bMask);
	void MarkSessionBegun();
	bool HasSession() const;
	void Discard();
	void Pulse();
	void BeginClosure();
	void CloseAndPersist(bool bTeardown = false);
	void ResetEpoch();
	TSharedPtr<FJsonObject> Summary() const;
	bool Gate(int32 Id, int32 SessionIndex = 8) const;
	int32 GetGate() const { return BenchGate; }
	static int64 NowMs();

private:
	struct FPending
	{
		FAnomalyChangeIssuePtr Issued;
		FAnomalyChangeReceiptPtr ColourReceipt, MaskReceipt;
		FAnomalyChangeColourPtr Pixels;
		FAnomalyChangeMaskPtr MaskPixels;
		bool bSealed = false, bColourDone = false, bColourDelivered = false;
		bool bColourCompletionReceived = false;
		uint64 IssueFrame = 0;
		int64 ColourLatencyMs = -1, ColourLatencyFrames = -1;
		bool bMaskExpected = false, bMaskDone = true, bMaskDelivered = false, bEmptyMask = false;
		EAnomalyChangeReason Reason = EAnomalyChangeReason::None;
		FString FailureStage;
		TArray<FAnomalyChangeLabel> Labels;
		TMap<uint8, int32> Counts;
		bool bObserved = false;
		TArray<TSharedPtr<FJsonObject>> BudgetCensus;
	};
	struct FPhase
	{
		int32 First = -1, Last = -1, Labelled = 0, Required = 0, Measured = 0;
		int32 MaxGt8 = -1, MaxRef = -1;
		double Deadline = 0, MaxMean = -1;
		bool bClosing = false, bFinal = false, bOnsetMeasured = false;
		EAnomalyChangeReason OnsetReason = EAnomalyChangeReason::None;
		FString Cause;
		TMap<FString, int64> Reasons;
		FAnomalyChangeColourPtr Reference;
		FAnomalyChangeReceiptPtr ReferenceReceipt;
	};
	struct FEvent
	{
		FString Type, Target, Cause;
		TArray<FPhase> Phases;
		int32 ActivePhase = -1, LastSeen = -1;
		bool bClosing = false, bFinal = false;
	};
	void ClosePhaseLocked(FPhase& Phase, const TCHAR* Cause);
	void EndEventsLocked(const TCHAR* Cause);
	void FinalizeEventsLocked();
	void MeasureLocked(FPending& Item, EAnomalyChangeReason Reason, const TSharedRef<FJsonObject>& Base);
	void AddRowLocked(const TSharedRef<FJsonObject>& Row);
	void ReleaseColourLocked(FAnomalyChangeColourPtr& Pixels);
	void RetainColourLocked(const FAnomalyChangeColourPtr& Pixels);
	bool AcceptLocked(const FAnomalyChangeIssuePtr& InIssue, const TCHAR* Path);
	void ScheduleLocked();
	void Work();
	void FinalizeLocked(FPending& Item, EAnomalyChangeReason Reason);
	void ReleaseLocked(FPending& Item);
	bool ReserveLocked(int64 Bytes);
	TSharedPtr<FJsonObject> CensusLocked(const TCHAR* Trigger, const TCHAR* Kind, int32 SessionIndex, int64 Bytes) const;
	void NoteBudgetRefusalLocked(FPending& Item, const TCHAR* Kind, int64 Bytes);
	void NoteHighWaterLocked(int32 SessionIndex, const TCHAR* Kind, int64 Bytes);
	void NoteColourCompletionLocked(FPending& Item);
	int32 ColourCompletionsAfterLocked(int32 Index) const;
	bool Persist();

	mutable FCriticalSection CS;
	TMap<int32, FPending> Pending;
	FPending Previous;
	TArray<FString> Rows;
	TMap<FString, int64> Counters;
	TMap<FString, FEvent> Events;
	TMap<const TArray<FColor>*, int32> ColourOwners;
	FString RunDir;
	FString DeferredEndCause;
	int32 DeferredEndAt = -1;
	FString EnabledSource, BytesSource, GateSource;
	uint64 Epoch = 0, Cut = 0;
	int32 Cursor = 0, LatestIndex = -1, FirstIndex = -1;
	int32 ClosureWatermark = -1;
	double ClosureDeadline = 0, WorkerMs = 0;
	int64 BytesHeld = 0, BytesHighWater = 0, MaxBytes = 0;
	const FPending* InHand = nullptr;
	TSharedPtr<FJsonObject> PeakCensus;
	int64 HighWaterLoggedBytes = 0, ColourUnitBytes = 0;
	int32 BenchGate = 0;
	TMap<int64, int64> ColourLatencyMsHistogram, ColourLatencyFramesHistogram;
	bool bClosing = false, bClosed = false, bWorkerActive = false;
	bool bPersisted = false;
	bool bSessionBegun = false, bDiscarded = false;
	const FRenderTarget* LastOwnerTarget = nullptr;
	const FSceneInterface* LastOwnerScene = nullptr;
	const void* LastOwnerLevel = nullptr;
};

#endif
