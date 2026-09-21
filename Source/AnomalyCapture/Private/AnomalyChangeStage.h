#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE
#include "PixelFormat.h"
#include "AnomalyViewport.h"

class FAnomalyChangeStage;
class FJsonObject;
class FRenderTarget;
class FSceneInterface;

// None is internal only: it is serialized as JSON null, never as a refusal string.
enum class EAnomalyChangeReason : uint8
{
	None, FirstFrame, PredecessorMissing, PredecessorUndelivered, OutOfOrderTimeout,
	EpochReset, ViewMismatch, ExtentMismatch, MaskPayloadMissing, UnsupportedDelivery,
	BudgetExceeded, EmptyRegion, NoLabelledFrames, CurrentUndelivered, ClosureTimeout
};
const TCHAR* AnomalyChangeReasonName(EAnomalyChangeReason Reason);

// Constructed on the GT at arm time and only published through const shared pointers.
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
	TWeakPtr<FAnomalyChangeStage, ESPMode::ThreadSafe> Stage;
};
using FAnomalyChangeIssuePtr = TSharedPtr<const FAnomalyChangeIssue, ESPMode::ThreadSafe>;

// Geometry is recorded at submission; the complete immutable receipt is published at drain.
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

// Separate completion record. A writer never changes a published receipt or buffer.
struct FAnomalyChangeCompletion
{
	FAnomalyChangeReceiptPtr Receipt;
	bool bDelivered = false;
	bool bMask = false;
	FString Stage;
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
	void Mask(const FAnomalyChangeReceiptPtr& Receipt, const FAnomalyChangeMaskPtr& Pixels, bool bEmpty);
	void CompleteMask(const FAnomalyChangeCompletion& Completion);
	void Fail(const FAnomalyChangeIssuePtr& InIssue, EAnomalyChangeReason Reason, const TCHAR* FailureStage);
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
		bool bMaskExpected = false, bMaskDone = true, bMaskDelivered = false, bEmptyMask = false;
		EAnomalyChangeReason Reason = EAnomalyChangeReason::None;
		FString FailureStage;
	};
	bool AcceptLocked(const FAnomalyChangeIssuePtr& InIssue, const TCHAR* Path);
	void ScheduleLocked();
	void Work();
	void FinalizeLocked(FPending& Item, EAnomalyChangeReason Reason);
	void ReleaseLocked(FPending& Item);
	bool ReserveLocked(int64 Bytes, bool bColour);
	bool Persist();

	mutable FCriticalSection CS;
	TMap<int32, FPending> Pending;
	FPending Previous;
	TArray<FString> Rows;
	TMap<FString, int64> Counters;
	FString RunDir;
	FString EnabledSource, BytesSource, GateSource;
	uint64 Epoch = 0, Cut = 0;
	int32 Cursor = 0, LatestIndex = -1, FirstIndex = -1;
	int32 ClosureWatermark = -1, ClosureAtIndex = -1;
	double ClosureDeadline = 0, WorkerMs = 0;
	int64 BytesHeld = 0, BytesHighWater = 0, MaxBytes = 0;
	int32 ColoursHeld = 0, BenchGate = 0;
	bool bClosing = false, bClosed = false, bWorkerActive = false;
	bool bPersisted = false;
	// GT-only owner lifetime observation. World-subsystem teardown also closes this stage.
	const FRenderTarget* LastOwnerTarget = nullptr;
	const FSceneInterface* LastOwnerScene = nullptr;
	const void* LastOwnerLevel = nullptr;
};

#endif
