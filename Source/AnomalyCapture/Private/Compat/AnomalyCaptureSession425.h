#pragma once
#include "AnomalyCaptureFeatures.h"
#include "CoreMinimal.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyCaptureBackend425.h"
#include "Async/Future.h"
class FAnomalyCaptureSession425
{
public:
    bool Start(UWorld* World, const FString& Directory, int32 Count);
    void Tick(UWorld* World);
    void Stop();
    bool IsRunning() const { return Running; }
    int32 Written() const { return Completed; }
private:
    TUniquePtr<IAnomalyCaptureBackend> Backend;
    TArray<TFuture<bool>> Writes;
    TArray<FString> Rows;
    FString Output;
    FString RHI;
    uint64 NextId = 1;
    int32 Requested = 0;
    int32 Completed = 0;
    int32 Count = 0;
    int32 Failures = 0;
    bool Running = false;
    double Started = 0;
};
#else
class FAnomalyCaptureSession425 {};
#endif
