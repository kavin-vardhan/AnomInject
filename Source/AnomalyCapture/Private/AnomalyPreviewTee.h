#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "HAL/ThreadSafeCounter.h"

class FAnomalyFrameCapturer;
class SWindow;

class FAnomalyPreviewTee
{
public:
	FAnomalyPreviewTee();
	~FAnomalyPreviewTee();

	void Arm(SWindow* TargetWindow, const FIntRect& CaptureRect, uint32 Epoch);
	void Pump(bool bSuppressed);
	bool PollJpeg(TArray<uint8>& OutJpeg, int32& OutW, int32& OutH, uint32& OutEpoch);
	bool IsBusy() const;
	void DiscardReady();

private:
	struct FSlot
	{
		FCriticalSection CS;
		TArray<uint8> Jpeg;
		int32 W = 0;
		int32 H = 0;
		uint32 Epoch = 0;
		bool bHasFrame = false;
		FThreadSafeCounter EncodesInFlight;
	};

	TSharedPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> Capturer;
	TSharedPtr<FSlot, ESPMode::ThreadSafe> Slot;
	TMap<uint64, uint32> ArmEpochs;
	uint64 NextRequestId = 1;
	int32 Outstanding = 0;
	bool bLoggedFormat = false;
};

#endif
