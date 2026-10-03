#pragma once

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#if __has_include("Templates/SharedPointerFwd.h")
#include "Templates/SharedPointerFwd.h"
#endif
#include "Templates/SharedPointerInternals.h"

#if ANOMALY_CAPTURE

#include "AnomalyFrameCapturer.h"
#include "AnomalyChangeStage.h"

#include "HAL/ThreadSafeCounter.h"
#include "PixelFormat.h"
#include "RHIGPUReadback.h"

class UTexture2D;

struct FAnomalyReadbackLatencyStats
{
	int32 Samples = 0;
	int32 MinFrames = MAX_int32;
	int32 MaxFrames = MIN_int32;
	int64 SumFrames = 0;
	int32 NotReadyPolls = 0;
	TMap<int32, int32> Histogram;
};

struct FAnomalySveHandshakeStats
{
	int32 ArmsIssued = 0;
	int32 Matches = 0;
	int32 SubmitsIssued = 0;
	int32 MaxPendingDepth = 0;
	int32 PendingNow = 0;
	int32 FamiliesIneligible = 0;
	int32 TracedArms = 0;
	int32 TracedPublishes = 0;
};

class FAnomalySveCapturer : public TSharedFromThis<FAnomalySveCapturer, ESPMode::ThreadSafe>
{
public:
	static constexpr int32 HandshakeTraceLimit = 64;

	void SetActive(bool bInActive);
	bool IsActive() const;

	void ArmWanted(uint64 RequestId, FAnomalyChangeIssuePtr ChangeIssue = nullptr,
		const TArray<class UTexture2D*>* RenderWatch = nullptr);
	bool ExtendRenderWatch(uint64 RequestId, const TArray<class UTexture2D*>& Added);
	bool TakeRenderWatch_RenderThread(uint64 RequestId, TArray<class UTexture2D*>& Out);
	static void SampleRenderMips_RenderThread(const TArray<class UTexture2D*>& Watch, TArray<FAnomalyRenderMipSample>& Out);
	void CancelPendingOtherGeneration(const FAnomalyChangeIssuePtr& Current, TArray<uint64>& Cancelled);
	FAnomalyChangeIssuePtr PeekChangeIssue() const;
	FAnomalyChangeIssuePtr GetOwnerIssue() const;
	TSharedPtr<FAnomalyChangeStage, ESPMode::ThreadSafe> GetChangeStage() const;
	bool ConsumeWantedForPublish(uint32 FamilyFrameNumber, uint64& OutRequestId, FAnomalyChangeIssuePtr& OutIssue);
	void NoteIneligibleFamily();

	void SubmitInFlight_RenderThread(uint64 RequestId, const FIntRect& Rect, const FIntPoint& SourceExtent,
		EPixelFormat Format, TUniquePtr<FRHIGPUTextureReadback>&& Readback,
		TUniquePtr<FRHIGPUTextureReadback>&& LegacyReadback = TUniquePtr<FRHIGPUTextureReadback>(),
		const FAnomalyChangeReceipt& ChangeSubmission = FAnomalyChangeReceipt(),
		TArray<FAnomalyRenderMipSample>&& RenderMips = TArray<FAnomalyRenderMipSample>(), bool bRenderRecord = false,
		int32 TemporalAaMethod = -1, bool bTemporalViewKnown = false, bool bTemporalUpscaler = false);

	int32 GetDualPathComparisons() const;
	int32 GetDualPathMismatches() const;

	void EnqueueDrain();
	bool PopCompleted(FAnomalyCapturedFrame& Out);
	int32 NumPendingApprox() const;

	FAnomalySveHandshakeStats GetHandshakeStats() const;
	FAnomalyReadbackLatencyStats GetLatencyStats() const;
	FAnomalyReadbackLayout GetReadbackLayout() const;

	void Reset();

private:
	void Drain_RenderThread();

	struct FInFlight
	{
		uint64 RequestId = 0;
		TUniquePtr<FRHIGPUTextureReadback> Readback;
		TUniquePtr<FRHIGPUTextureReadback> LegacyReadback;
		FIntRect Rect;
		FIntPoint SourceExtent = FIntPoint::ZeroValue;
		EPixelFormat Format = PF_Unknown;
		uint32 SubmitRtFrame = 0;
		FAnomalyChangeReceipt ChangeSubmission;
		TArray<FAnomalyRenderMipSample> RenderMips;
		bool bRenderRecord = false;
		int32 TemporalAaMethod = -1;
		bool bTemporalViewKnown = false;
		bool bTemporalUpscaler = false;
	};

	void CompareDualPath_RenderThread(FInFlight& Item, const FAnomalyCapturedFrame& OwnedFrame);

	mutable FCriticalSection StateCS;
	TArray<uint64> PendingWanted;
	TMap<uint64, FAnomalyChangeIssuePtr> PendingIssues;
	TMap<uint64, TArray<class UTexture2D*>> RenderWatchByRequest;
	TWeakPtr<FAnomalyChangeStage, ESPMode::ThreadSafe> ChangeStage;
	FAnomalyChangeIssuePtr LastIssuedIdentity;
	FAnomalySveHandshakeStats Handshake;
	FThreadSafeCounter ActiveFlag;
	FThreadSafeCounter Submits;

	TArray<FInFlight> InFlight;

	mutable FCriticalSection CompletedCS;
	TArray<FAnomalyCapturedFrame> Completed;

	mutable FCriticalSection LatencyCS;
	FAnomalyReadbackLatencyStats Latency;

	mutable FCriticalSection LayoutCS;
	FAnomalyReadbackLayout Layout;

	FThreadSafeCounter GuardDrops;
	FThreadSafeCounter DualPathComparisons;
	FThreadSafeCounter DualPathMismatches;
};

#endif
