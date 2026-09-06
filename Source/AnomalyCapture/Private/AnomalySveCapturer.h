#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "AnomalyFrameCapturer.h"

#include "HAL/ThreadSafeCounter.h"
#include "PixelFormat.h"
#include "RHIGPUReadback.h"

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

	void ArmWanted(uint64 RequestId);
	bool ConsumeWantedForPublish(uint32 FamilyFrameNumber, uint64& OutRequestId, uint64 GameFrame);
	void NoteIneligibleFamily();
	void RecordRenderView(uint64 RequestId, const FAnomalyViewInfo& View);

	void SubmitInFlight_RenderThread(uint64 RequestId, const FIntRect& Rect, const FIntPoint& SourceExtent,
		EPixelFormat Format, TUniquePtr<FRHIGPUTextureReadback>&& Readback,
		TUniquePtr<FRHIGPUTextureReadback>&& LegacyReadback = TUniquePtr<FRHIGPUTextureReadback>(), uint32 RenderFrame = 0);

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
		uint32 RenderFrame = 0;
	};

	void CompareDualPath_RenderThread(FInFlight& Item, const FAnomalyCapturedFrame& OwnedFrame);

	mutable FCriticalSection StateCS;
	TArray<uint64> PendingWanted;
	TMap<uint64, uint64> WantedGameFrames;
	TMap<uint64, FAnomalyViewInfo> RenderViews;
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
