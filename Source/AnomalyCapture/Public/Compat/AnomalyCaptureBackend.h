#pragma once

#include "CoreMinimal.h"

class UWorld;
struct FAnomalyCapturedFrame;

class ANOMALYCAPTURE_API IAnomalyCaptureBackend
{
public:
	virtual ~IAnomalyCaptureBackend() = default;
	virtual bool RequestFrame(UWorld* World, uint64 RequestId) = 0;
	virtual bool PollFrame(FAnomalyCapturedFrame& OutFrame) = 0;
	virtual void Stop() = 0;
};

namespace AnomalyCompat
{
	ANOMALYCAPTURE_API TUniquePtr<IAnomalyCaptureBackend> CreateCaptureBackend425();
}
