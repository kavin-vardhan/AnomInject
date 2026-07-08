#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "RHIGPUReadback.h"
#include "PixelFormat.h"

class SWindow;

struct FAnomalyCapturedFrame
{
	uint64 RequestId = 0;
	int32 Width = 0;
	int32 Height = 0;
	EPixelFormat Format = PF_Unknown;
	int32 BytesPerPixel = 0;
	TArray<uint8> RawBytes;
};

class FAnomalyFrameCapturer : public TSharedFromThis<FAnomalyFrameCapturer, ESPMode::ThreadSafe>
{
public:
	FAnomalyFrameCapturer() = default;
	~FAnomalyFrameCapturer();

	void RegisterBackbufferHook();
	void UnregisterBackbufferHook();
	void ArmForCapture(uint64 RequestId, SWindow* TargetWindow, const FIntRect& CaptureRect);
	void EnqueueDrain();
	bool PopCompleted(FAnomalyCapturedFrame& Out);
	int32 NumPendingApprox() const;

private:
	void OnBackBufferReadyToPresent_RenderThread(SWindow& SlateWindow, const FTexture2DRHIRef& BackBuffer);
	void Drain_RenderThread();

	struct FArm
	{
		uint64 RequestId = 0;
		SWindow* Window = nullptr;
		FIntRect Rect;
	};

	struct FInFlight
	{
		uint64 RequestId = 0;
		TUniquePtr<FRHIGPUTextureReadback> Readback;
		FIntRect Rect;
		EPixelFormat Format = PF_Unknown;
	};

	mutable FCriticalSection StateCS;
	TArray<FArm> PendingArms;

	TArray<FInFlight> InFlight;

	mutable FCriticalSection CompletedCS;
	TArray<FAnomalyCapturedFrame> Completed;

	FDelegateHandle BackBufferHandle;
};

#endif
