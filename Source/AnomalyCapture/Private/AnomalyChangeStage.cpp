#include "AnomalyChangeStage.h"
#if ANOMALY_CAPTURE
#include "AnomalyCaptureLog.h"
#include "Async/Async.h"
#include "Dom/JsonObject.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformProcess.h"
#include "HAL/ThreadSafeCounter64.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"

static TAutoConsoleVariable<int32> CVarChangeEnabled(TEXT("IAI.Capture.ChangeEvidence"), 1,
	TEXT("m55 identity sidecar. Sampled at run start; Stage 1 performs no pixel arithmetic."));
static TAutoConsoleVariable<int32> CVarChangeBytes(TEXT("IAI.Capture.ChangeMaxBytes"), 64 * 1024 * 1024,
	TEXT("Maximum bytes retained by m55 (not the writer pool). Sampled at run start."));
static TAutoConsoleVariable<int32> CVarChangeGate(TEXT("IAI.Bench.ChangeGate"), 0,
	TEXT("Bench fault at SI 8: 1=two-ready,2=skip,3=writer,4=readback,5=mask-write,6=coalesce,7=epoch,8=view,9=extent,10=budget,11=late,12=stale. 14=log frozen-mask CRC for transport audit."));
static FThreadSafeCounter64 GChangeEpoch, GChangeToken;

static FString ChangeSettingSource(const TCHAR* Name)
{
	const IConsoleVariable* Var = IConsoleManager::Get().FindConsoleVariable(Name);
	if (!Var) { return TEXT("unavailable"); }
	const uint32 By = Var->GetFlags() & ECVF_SetByMask;
	switch (By)
	{
	case ECVF_SetByConstructor: return TEXT("compiled");
	case ECVF_SetByConsole: return TEXT("console");
	case ECVF_SetByCommandline: return TEXT("command_line");
	case ECVF_SetByCode: return TEXT("code");
	case ECVF_SetBySystemSettingsIni: return TEXT("system_settings_ini");
	case ECVF_SetByConsoleVariablesIni: return TEXT("console_variables_ini");
	default: return FString::Printf(TEXT("engine_set_by_0x%08x"), By);
	}
}

const TCHAR* AnomalyChangeReasonName(EAnomalyChangeReason R)
{
	switch (R)
	{
	case EAnomalyChangeReason::FirstFrame: return TEXT("first_frame");
	case EAnomalyChangeReason::PredecessorMissing: return TEXT("predecessor_missing");
	case EAnomalyChangeReason::PredecessorUndelivered: return TEXT("predecessor_undelivered");
	case EAnomalyChangeReason::OutOfOrderTimeout: return TEXT("out_of_order_timeout");
	case EAnomalyChangeReason::EpochReset: return TEXT("epoch_reset");
	case EAnomalyChangeReason::ViewMismatch: return TEXT("view_mismatch");
	case EAnomalyChangeReason::ExtentMismatch: return TEXT("extent_mismatch");
	case EAnomalyChangeReason::MaskPayloadMissing: return TEXT("mask_payload_missing");
	case EAnomalyChangeReason::UnsupportedDelivery: return TEXT("unsupported_delivery");
	case EAnomalyChangeReason::BudgetExceeded: return TEXT("budget_exceeded");
	case EAnomalyChangeReason::EmptyRegion: return TEXT("empty_region");
	case EAnomalyChangeReason::NoLabelledFrames: return TEXT("no_labelled_frames");
	case EAnomalyChangeReason::CurrentUndelivered: return TEXT("current_undelivered");
	case EAnomalyChangeReason::ClosureTimeout: return TEXT("closure_timeout");
	default: return nullptr;
	}
}

int64 FAnomalyChangeStage::NowMs() { return (int64)(FPlatformTime::Seconds() * 1000.0); }
bool FAnomalyChangeStage::IsEnabled() { return CVarChangeEnabled.GetValueOnGameThread() != 0; }
FAnomalyChangeStage::FAnomalyChangeStage(const FString& InRunDir) : RunDir(InRunDir)
{
	Epoch = (uint64)GChangeEpoch.Increment();
	MaxBytes = FMath::Max(0, CVarChangeBytes.GetValueOnGameThread());
	BenchGate = CVarChangeGate.GetValueOnGameThread();
	EnabledSource = ChangeSettingSource(TEXT("IAI.Capture.ChangeEvidence"));
	BytesSource = ChangeSettingSource(TEXT("IAI.Capture.ChangeMaxBytes"));
	GateSource = ChangeSettingSource(TEXT("IAI.Bench.ChangeGate"));
	for (const TCHAR* Name : { TEXT("issued"), TEXT("pairs_identity_valid"), TEXT("pairs_refused"), TEXT("rows_dropped"),
		TEXT("late_results"), TEXT("epoch_rejected"), TEXT("epoch_resets"), TEXT("view_rejected"), TEXT("duplicate_callback"),
		TEXT("duplicate_completion"), TEXT("mask_capture_served_ge2"), TEXT("mask_pass_deferred"),
		TEXT("colour_multi_ready_drain"), TEXT("capture_arm_dropped"), TEXT("throwaway_family_constructed"),
		TEXT("teardown_flush"), TEXT("persist_failed"), TEXT("late_record_mutations") }) { Counters.Add(Name, 0); }
	UE_LOG(LogAnomalyCapture, Log, TEXT("Capture(m55): EFFECTIVE enabled=1(from %s) maxBytes=%lld(from %s) gate=%d(from %s) epoch=%llu; snapshotted at run start"),
		*EnabledSource, MaxBytes, *BytesSource, BenchGate, *GateSource, Epoch);
}
bool FAnomalyChangeStage::Gate(int32 Id, int32 SessionIndex) const { return BenchGate == Id && SessionIndex == 8; }

FAnomalyChangeIssuePtr FAnomalyChangeStage::Issue(uint64 RequestId, int32 SI, const FAnomalyViewInfo& Camera,
	const FRenderTarget* OwnerTarget, const FSceneInterface* OwnerScene, const void* OwnerLevel, bool bSupported)
{
	if (LastOwnerScene && (LastOwnerScene != OwnerScene || LastOwnerTarget != OwnerTarget || LastOwnerLevel != OwnerLevel)) { ResetEpoch(); }
	LastOwnerScene = OwnerScene; LastOwnerTarget = OwnerTarget; LastOwnerLevel = OwnerLevel;
	FScopeLock Lock(&CS);
	if (bClosed || bClosing) { return nullptr; }
	auto New = MakeShared<FAnomalyChangeIssue, ESPMode::ThreadSafe>();
	New->RunEpoch = Epoch; New->CutCounter = Cut; New->CaptureToken = (uint64)GChangeToken.Increment();
	New->RequestId = RequestId; New->SessionIndex = SI; New->SubmitMs = NowMs(); New->Camera = Camera;
	New->OwnerTarget = OwnerTarget; New->OwnerScene = OwnerScene; New->OwnerLevel = OwnerLevel; New->Stage = AsShared();
	if (FirstIndex < 0) { FirstIndex = SI; Cursor = SI; }
	LatestIndex = SI;
	FPending& Item = Pending.Add(SI);
	Item.Issued = New;
	if (!bSupported) { Item.Reason = EAnomalyChangeReason::UnsupportedDelivery; Item.bSealed = true; }
	++Counters.FindOrAdd(TEXT("issued"));
	ScheduleLocked();
	return New;
}
FAnomalyChangeIssuePtr FAnomalyChangeStage::FindIssue(int32 SI) const
{
	FScopeLock Lock(&CS);
	const FPending* Item = Pending.Find(SI);
	return Item ? Item->Issued : nullptr;
}
void FAnomalyChangeStage::ExpectMask(int32 SI)
{
	FScopeLock Lock(&CS);
	if (FPending* Item = Pending.Find(SI)) { Item->bMaskExpected = true; Item->bMaskDone = false; }
}
void FAnomalyChangeStage::SealArm(int32 SI)
{
	FScopeLock Lock(&CS);
	if (FPending* Item = Pending.Find(SI)) { Item->bSealed = true; }
	ScheduleLocked();
}
bool FAnomalyChangeStage::AcceptLocked(const FAnomalyChangeIssuePtr& InIssue, const TCHAR* Path)
{
	if (!InIssue.IsValid()) { return false; }
	if (InIssue->RunEpoch != Epoch || InIssue->CutCounter != Cut)
	{
		++Counters.FindOrAdd(TEXT("epoch_rejected"));
		++Counters.FindOrAdd(FString(TEXT("epoch_rejected_")) + Path);
		UE_LOG(LogAnomalyCapture, Log, TEXT("Capture(m55): EPOCH-REJECT path=%s token=%llu issuedEpoch=%llu currentEpoch=%llu"), Path, InIssue->CaptureToken, InIssue->RunEpoch, Epoch);
		return false;
	}
	if (bClosed || InIssue->SessionIndex < Cursor || !Pending.Contains(InIssue->SessionIndex))
	{
		++Counters.FindOrAdd(TEXT("late_results"));
		UE_LOG(LogAnomalyCapture, Log, TEXT("Capture(m55): LATE-REJECT path=%s token=%llu si=%d cursor=%d closed=%d rows=%d"), Path, InIssue->CaptureToken, InIssue->SessionIndex, Cursor, bClosed ? 1 : 0, Rows.Num());
		return false;
	}
	return true;
}
bool FAnomalyChangeStage::AcceptGeneration(const FAnomalyChangeIssuePtr& InIssue, const TCHAR* Path)
{
	FScopeLock Lock(&CS); return AcceptLocked(InIssue, Path);
}
void FAnomalyChangeStage::Diagnostic(const TCHAR* Name, int64 Amount)
{
	FScopeLock Lock(&CS); Counters.FindOrAdd(Name) += Amount;
}
bool FAnomalyChangeStage::ReserveLocked(int64 Bytes, bool bColour)
{
	if (Bytes < 0 || Bytes > MaxBytes - BytesHeld || (bColour && ColoursHeld >= 3)) { return false; }
	BytesHeld += Bytes; BytesHighWater = FMath::Max(BytesHighWater, BytesHeld);
	if (bColour) { ++ColoursHeld; }
	return true;
}
void FAnomalyChangeStage::Colour(const FAnomalyChangeCompletion& Completion, const FAnomalyChangeColourPtr& Pixels, bool bSupported)
{
	if (!Completion.Receipt.IsValid()) { return; }
	FScopeLock Lock(&CS);
	if (!AcceptLocked(Completion.Receipt->Issue, TEXT("colour_writer"))) { return; }
	FPending& Item = Pending.FindChecked(Completion.Receipt->Issue->SessionIndex);
	if (Item.bColourDone) { ++Counters.FindOrAdd(TEXT("duplicate_completion")); return; }
	Item.bColourDone = true; Item.bColourDelivered = Completion.bDelivered;
	Item.ColourReceipt = Completion.Receipt;
	if (!Completion.bDelivered)
	{
		Item.Reason = EAnomalyChangeReason::CurrentUndelivered; Item.FailureStage = Completion.Stage;
	}
	else if (!bSupported) { Item.Reason = EAnomalyChangeReason::UnsupportedDelivery; }
	else if (Pixels.IsValid())
	{
		// Reservation precedes taking the stage's reference; writer threads never wait for capacity.
		if (!Gate(10, Completion.Receipt->Issue->SessionIndex) && ReserveLocked((int64)Pixels->GetAllocatedSize(), true)) { Item.Pixels = Pixels; }
		else { Item.Reason = EAnomalyChangeReason::BudgetExceeded; }
	}
	else { Item.Reason = EAnomalyChangeReason::CurrentUndelivered; Item.FailureStage = TEXT("colour"); }
	ScheduleLocked();
}
void FAnomalyChangeStage::Mask(const FAnomalyChangeReceiptPtr& Receipt, const FAnomalyChangeMaskPtr& Pixels, bool bEmpty)
{
	if (!Receipt.IsValid()) { return; }
	FScopeLock Lock(&CS);
	if (!AcceptLocked(Receipt->Issue, TEXT("mask_admission"))) { return; }
	FPending& Item = Pending.FindChecked(Receipt->Issue->SessionIndex);
	if (Item.MaskReceipt.IsValid()) { ++Counters.FindOrAdd(TEXT("duplicate_completion")); return; }
	Item.MaskReceipt = Receipt; Item.bEmptyMask = bEmpty;
	if (!Pixels.IsValid()) { Item.Reason = EAnomalyChangeReason::MaskPayloadMissing; Item.bMaskDone = true; }
	else if (!ReserveLocked((int64)Pixels->GetAllocatedSize(), false)) { Item.Reason = EAnomalyChangeReason::BudgetExceeded; Item.bMaskDone = true; }
	else
	{
		Item.MaskPixels = Pixels;
		// Empty masks are deliberately not written by m44. Their zero denominator is known.
		if (bEmpty) { Item.bMaskDone = true; Item.bMaskDelivered = true; }
	}
	ScheduleLocked();
}
void FAnomalyChangeStage::CompleteMask(const FAnomalyChangeCompletion& Completion)
{
	if (!Completion.Receipt.IsValid()) { return; }
	FScopeLock Lock(&CS);
	if (!AcceptLocked(Completion.Receipt->Issue, TEXT("mask_writer"))) { return; }
	FPending& Item = Pending.FindChecked(Completion.Receipt->Issue->SessionIndex);
	Item.bMaskDone = true; Item.bMaskDelivered = Completion.bDelivered;
	if (!Completion.bDelivered) { Item.Reason = EAnomalyChangeReason::CurrentUndelivered; Item.FailureStage = TEXT("mask"); }
	ScheduleLocked();
}
void FAnomalyChangeStage::Fail(const FAnomalyChangeIssuePtr& InIssue, EAnomalyChangeReason Reason, const TCHAR* FailureStage)
{
	FScopeLock Lock(&CS);
	if (!AcceptLocked(InIssue, FailureStage)) { return; }
	FPending& Item = Pending.FindChecked(InIssue->SessionIndex);
	Item.Reason = Reason; Item.FailureStage = FailureStage;
	if (FCString::Strcmp(FailureStage, TEXT("mask")) == 0) { Item.bMaskDone = true; }
	else { Item.bColourDone = true; }
	ScheduleLocked();
}
void FAnomalyChangeStage::Pulse() { FScopeLock Lock(&CS); ScheduleLocked(); }
void FAnomalyChangeStage::ScheduleLocked()
{
	if (bWorkerActive || bClosed) { return; }
	bWorkerActive = true;
	auto Self = AsShared();
	Async(EAsyncExecution::ThreadPool, [Self]() { Self->Work(); });
}
void FAnomalyChangeStage::Work()
{
	for (;;)
	{
		const double Start = FPlatformTime::Seconds();
		bool bWaitOnOwnQueue = false;
		{
			FScopeLock Lock(&CS);
			while (FPending* Item = Pending.Find(Cursor))
			{
				EAnomalyChangeReason Reason = Item->Reason;
				const bool bReady = Item->bSealed && Item->bColourDone && Item->bMaskDone;
				const bool bTerminal = bClosing && (FPlatformTime::Seconds() >= ClosureDeadline || LatestIndex - ClosureAtIndex >= 8);
				if (!bReady && !(Item->bSealed && Item->bColourDone && Reason != EAnomalyChangeReason::None))
				{
					if (bTerminal) { Reason = EAnomalyChangeReason::ClosureTimeout; }
					else if (!bClosing && LatestIndex - Cursor >= 4) { Reason = EAnomalyChangeReason::OutOfOrderTimeout; }
					else { break; }
				}
				FinalizeLocked(*Item, Reason);
				Pending.Remove(Cursor++);
			}
			if (bClosing && Cursor > ClosureWatermark) { bClosed = true; ReleaseLocked(Previous); }
			bWaitOnOwnQueue = bClosing && !bClosed;
			WorkerMs += (FPlatformTime::Seconds() - Start) * 1000.0;
			if (!bWaitOnOwnQueue) { bWorkerActive = false; }
		}
		if (!bWaitOnOwnQueue) { return; }
		// No writer waits for a predecessor or for capacity. Only this serial stage waits.
		FPlatformProcess::Sleep(0.001f);
	}
}

static TSharedRef<FJsonObject> ChangeReceiptJson(const FAnomalyChangeReceiptPtr& R, const FAnomalyChangeIssuePtr& Issued)
{
	auto J = MakeShared<FJsonObject>();
	if (!Issued.IsValid()) { return J; }
	J->SetNumberField(TEXT("run_epoch"), (double)Issued->RunEpoch);
	J->SetNumberField(TEXT("cut_counter"), (double)Issued->CutCounter);
	J->SetNumberField(TEXT("capture_token"), (double)Issued->CaptureToken);
	J->SetNumberField(TEXT("t_submit_ms"), (double)Issued->SubmitMs);
	J->SetNumberField(TEXT("t_drain_ms"), R.IsValid() ? (double)R->DrainMs : -1.0);
	J->SetNumberField(TEXT("view_family_id"), R.IsValid() ? (double)R->ViewFamilyId : 0.0);
	J->SetNumberField(TEXT("serving_token"), R.IsValid() ? (double)R->ServingToken : 0.0);
	J->SetNumberField(TEXT("format"), R.IsValid() ? (int32)R->Format : (int32)PF_Unknown);
	TArray<TSharedPtr<FJsonValue>> Rect, Extent;
	if (R.IsValid())
	{
		for (int32 V : { R->Rect.Min.X, R->Rect.Min.Y, R->Rect.Width(), R->Rect.Height() }) { Rect.Add(MakeShared<FJsonValueNumber>(V)); }
		for (int32 V : { R->Extent.X, R->Extent.Y }) { Extent.Add(MakeShared<FJsonValueNumber>(V)); }
		J->SetNumberField(TEXT("family_frame"), R->FamilyFrame);
		J->SetNumberField(TEXT("view_index"), R->ViewIndex);
		if (R->MaskRequestId)
		{
			// Mask request serials use high bits, so preserve their exact uint64 values as strings.
			J->SetStringField(TEXT("mask_request_id"), FString::Printf(TEXT("%llu"), R->MaskRequestId));
			J->SetStringField(TEXT("payload_owner_request_id"), FString::Printf(TEXT("%llu"), R->PayloadOwnerRequestId));
			TArray<TSharedPtr<FJsonValue>> Ids;
			for (uint64 Id : R->ServedRequestIds) { Ids.Add(MakeShared<FJsonValueString>(FString::Printf(TEXT("%llu"), Id))); }
			J->SetArrayField(TEXT("served_request_ids"), Ids);
		}
	}
	J->SetArrayField(TEXT("rect"), Rect); J->SetArrayField(TEXT("extent"), Extent);
	return J;
}
void FAnomalyChangeStage::FinalizeLocked(FPending& Item, EAnomalyChangeReason Reason)
{
	const auto& C = Item.ColourReceipt;
	const auto& M = Item.MaskReceipt;
	const auto& P = Previous.ColourReceipt;
	if (Reason == EAnomalyChangeReason::None)
	{
		if (Cursor == FirstIndex) { Reason = Cut ? EAnomalyChangeReason::EpochReset : EAnomalyChangeReason::FirstFrame; }
		else if (!Previous.Issued.IsValid() || Previous.Issued->SessionIndex != Cursor - 1) { Reason = EAnomalyChangeReason::PredecessorMissing; }
		else if (Previous.Reason == EAnomalyChangeReason::OutOfOrderTimeout) { Reason = EAnomalyChangeReason::PredecessorMissing; }
		else if (!Previous.bColourDelivered) { Reason = EAnomalyChangeReason::PredecessorUndelivered; }
		else if (!P.IsValid() || !Previous.Pixels.IsValid()) { Reason = EAnomalyChangeReason::PredecessorMissing; }
		else if (!C.IsValid() || !Item.bColourDelivered) { Reason = EAnomalyChangeReason::CurrentUndelivered; Item.FailureStage = TEXT("colour"); }
		else if (C->ServingToken != Item.Issued->CaptureToken || C->ViewIndex != 0) { Reason = EAnomalyChangeReason::ViewMismatch; }
		else if (C->Rect != P->Rect || C->Extent != P->Extent || C->Format != P->Format) { Reason = EAnomalyChangeReason::ExtentMismatch; }
		else if (!M.IsValid() || !Item.MaskPixels.IsValid() || !Item.bMaskDelivered) { Reason = EAnomalyChangeReason::MaskPayloadMissing; }
		else if (M->ServingToken != Item.Issued->CaptureToken || M->ViewFamilyId != C->ViewFamilyId || M->ViewIndex != C->ViewIndex) { Reason = EAnomalyChangeReason::ViewMismatch; }
		else if (M->Rect != C->Rect || M->Extent != C->Extent) { Reason = EAnomalyChangeReason::ExtentMismatch; }
		else if (Item.bEmptyMask) { Reason = EAnomalyChangeReason::EmptyRegion; }
	}
	++Counters.FindOrAdd(Reason == EAnomalyChangeReason::None ? TEXT("pairs_identity_valid") : TEXT("pairs_refused"));
	const TCHAR* ReasonName = AnomalyChangeReasonName(Reason);
	if (ReasonName) { ++Counters.FindOrAdd(FString(TEXT("reason_")) + ReasonName); }
	auto J = MakeShared<FJsonObject>();
	J->SetStringField(TEXT("kind"), TEXT("pair")); J->SetNumberField(TEXT("stage_version"), 1);
	J->SetStringField(TEXT("session_id"), FPaths::GetCleanFilename(RunDir));
	J->SetNumberField(TEXT("session_index"), Cursor);
	J->SetNumberField(TEXT("prev_session_index"), Previous.Issued.IsValid() ? Previous.Issued->SessionIndex : -1);
	J->SetNumberField(TEXT("expected_prev_session_index"), Cursor - 1);
	J->SetStringField(TEXT("frame_file"), FString::Printf(TEXT("Actual_Frames/frame_%05d.png"), Cursor));
	J->SetBoolField(TEXT("pair_valid"), Reason == EAnomalyChangeReason::None);
	if (ReasonName) { J->SetStringField(TEXT("reason"), ReasonName); }
	else { J->SetField(TEXT("reason"), MakeShared<FJsonValueNull>()); }
	if (!Item.FailureStage.IsEmpty()) { J->SetStringField(TEXT("stage"), Item.FailureStage); }
	J->SetObjectField(TEXT("receipt"), ChangeReceiptJson(C, Item.Issued));
	if (Previous.Issued.IsValid()) { J->SetObjectField(TEXT("prev_receipt"), ChangeReceiptJson(P, Previous.Issued)); }
	if (M.IsValid()) { J->SetObjectField(TEXT("mask_receipt"), ChangeReceiptJson(M, M->Issue)); }
	int32 DPos = 0, DRot = 0, DFov = 0;
	if (Previous.Issued.IsValid())
	{
		const auto& A = Item.Issued->Camera; const auto& B = Previous.Issued->Camera;
		DPos = FMath::RoundToInt(FVector::Distance(A.Origin, B.Origin));
		DRot = FMath::RoundToInt(FMath::RadiansToDegrees(A.Rotation.Quaternion().AngularDistance(B.Rotation.Quaternion())) * 10.0);
		DFov = FMath::RoundToInt(FMath::Abs(A.HorizontalFOVDeg - B.HorizontalFOVDeg) * 10.0f);
	}
	J->SetNumberField(TEXT("cam_dpos_cm"), DPos); J->SetNumberField(TEXT("cam_drot_deg"), DRot);
	J->SetNumberField(TEXT("cam_dfov_deg"), DFov); J->SetBoolField(TEXT("cam_moved"), DPos || DRot || DFov);
	if (Rows.Num() < 100000)
	{
		FString Line; auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Line);
		FJsonSerializer::Serialize(J, Writer); Rows.Add(MoveTemp(Line));
	}
	else { ++Counters.FindOrAdd(TEXT("rows_dropped")); }
	UE_LOG(LogAnomalyCapture, Verbose, TEXT("Capture(m55): PAIR si=%d valid=%d reason=%s"), Cursor, Reason == EAnomalyChangeReason::None, ReasonName ? ReasonName : TEXT("null"));
	if (BenchGate == 12 && Cursor == 7)
	{
		// Keep the real SI 6 buffer/receipt so SI 8 sees an actual wrong predecessor.
		ReleaseLocked(Item); return;
	}
	ReleaseLocked(Previous);
	if (Item.MaskPixels.IsValid()) { BytesHeld -= (int64)Item.MaskPixels->GetAllocatedSize(); Item.MaskPixels.Reset(); }
	Item.Reason = Reason;
	Previous = MoveTemp(Item);
	Previous.MaskReceipt.Reset();
}
void FAnomalyChangeStage::ReleaseLocked(FPending& Item)
{
	if (Item.Pixels.IsValid()) { BytesHeld -= (int64)Item.Pixels->GetAllocatedSize(); --ColoursHeld; }
	if (Item.MaskPixels.IsValid()) { BytesHeld -= (int64)Item.MaskPixels->GetAllocatedSize(); }
	Item = FPending();
}
void FAnomalyChangeStage::BeginClosure()
{
	FScopeLock Lock(&CS);
	if (!bClosing && !bClosed)
	{
		bClosing = true; ClosureWatermark = LatestIndex; ClosureAtIndex = LatestIndex;
		ClosureDeadline = FPlatformTime::Seconds() + 2.0;
		ScheduleLocked();
	}
}
bool FAnomalyChangeStage::Persist()
{
	// Called by the GT only after closure. Rows are immutable; disk I/O never holds the
	// admission mutex that colour/mask writer workers use for their completion messages.
	const bool Ok = FFileHelper::SaveStringToFile(FString::Join(Rows, TEXT("\n")) + (Rows.Num() ? TEXT("\n") : TEXT("")),
		*FPaths::Combine(RunDir, TEXT("change_evidence.jsonl")), FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	FScopeLock Lock(&CS);
	bPersisted = Ok;
	if (!Ok) { ++Counters.FindOrAdd(TEXT("persist_failed")); UE_LOG(LogAnomalyCapture, Error, TEXT("Capture(m55): sidecar persistence failed: %s"), *RunDir); }
	return Ok;
}
void FAnomalyChangeStage::CloseAndPersist(bool bTeardown)
{
	BeginClosure();
	bool bNeedsPersist = false;
	for (;;)
	{
		{
			FScopeLock Lock(&CS);
			if (bClosed && !bWorkerActive)
			{
				if (bTeardown) { ++Counters.FindOrAdd(TEXT("teardown_flush")); }
				bNeedsPersist = !bPersisted;
				break;
			}
			ScheduleLocked();
		}
		FPlatformProcess::Sleep(0.001f);
	}
	if (bNeedsPersist) { Persist(); }
}
void FAnomalyChangeStage::ResetEpoch()
{
	CloseAndPersist();
	FScopeLock Lock(&CS);
	if (!bPersisted) { return; } // Never erase unpublished final records.
	Epoch = (uint64)GChangeEpoch.Increment(); ++Cut;
	Pending.Reset(); ReleaseLocked(Previous);
	FirstIndex = -1; bClosing = false; bClosed = false; bPersisted = false;
	++Counters.FindOrAdd(TEXT("epoch_resets"));
}
TSharedPtr<FJsonObject> FAnomalyChangeStage::Summary() const
{
	FScopeLock Lock(&CS);
	auto J = MakeShared<FJsonObject>();
	J->SetStringField(TEXT("change_evidence_file"), TEXT("change_evidence.jsonl"));
	J->SetNumberField(TEXT("change_stage_version"), 1);
	J->SetNumberField(TEXT("change_max_bytes"), (double)MaxBytes);
	J->SetNumberField(TEXT("change_bytes_high_water"), (double)BytesHighWater);
	J->SetNumberField(TEXT("change_bytes_retained"), (double)BytesHeld);
	J->SetNumberField(TEXT("change_worker_ms_total"), WorkerMs);
	J->SetNumberField(TEXT("change_closure_watermark"), ClosureWatermark);
	J->SetNumberField(TEXT("change_bench_gate"), BenchGate);
	J->SetStringField(TEXT("change_enabled_source"), EnabledSource);
	J->SetStringField(TEXT("change_max_bytes_source"), BytesSource);
	J->SetStringField(TEXT("change_bench_gate_source"), GateSource);
	for (int32 I = 1; I <= (int32)EAnomalyChangeReason::ClosureTimeout; ++I)
	{
		J->SetNumberField(FString(TEXT("change_reason_")) + AnomalyChangeReasonName((EAnomalyChangeReason)I), 0);
	}
	for (const auto& C : Counters) { J->SetNumberField(FString(TEXT("change_")) + C.Key, (double)C.Value); }
	return J;
}
#endif
