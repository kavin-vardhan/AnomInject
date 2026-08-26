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

struct FAnomalyReadbackLayout
{
	bool bValid = false;
	FIntPoint SourceExtent = FIntPoint::ZeroValue;
	FIntRect Rect = FIntRect(0, 0, 0, 0);
	int32 W = 0;
	int32 H = 0;
	int32 BufferHeight = 0;
	int32 RowPitchInPixels = 0;
	int32 Format = 0;
};

namespace AnomalyReadback
{
	void NoteLayoutOnce(FCriticalSection& Guard, FAnomalyReadbackLayout& Layout, const TCHAR* PathName,
		const FIntPoint& SourceExtent, const FIntRect& Rect, int32 RowPitchInPixels, int32 BufferHeight,
		EPixelFormat Format);
}

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

	FAnomalyReadbackLayout GetReadbackLayout() const;
	void ResetReadbackLayout();
	void SetLayoutPathName(const FString& InName);

private:
	void OnBackBufferReadyToPresent_RenderThread(SWindow& SlateWindow, const FTexture2DRHIRef& BackBuffer);
	void Drain_RenderThread();

	struct FArm
	{
		uint64 RequestId = 0;
		SWindow* Window = nullptr;
		FIntRect Rect;
		uint32 RegisteredRtFrame = 0;
	};

	struct FInFlight
	{
		uint64 RequestId = 0;
		TUniquePtr<FRHIGPUTextureReadback> Readback;
		FIntRect Rect;
		FIntPoint SourceExtent = FIntPoint::ZeroValue;
		EPixelFormat Format = PF_Unknown;
	};

	mutable FCriticalSection StateCS;
	TArray<FArm> PendingArms;

	TArray<FInFlight> InFlight;

	mutable FCriticalSection CompletedCS;
	TArray<FAnomalyCapturedFrame> Completed;

	mutable FCriticalSection LayoutCS;
	FAnomalyReadbackLayout Layout;
	FString LayoutPathName = TEXT("async");

	FDelegateHandle BackBufferHandle;
};

#endif
