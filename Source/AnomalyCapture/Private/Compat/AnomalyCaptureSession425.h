#pragma once
#include "AnomalyCaptureFeatures.h"
#include "CoreMinimal.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyCaptureBackend425.h"
#include "Async/Future.h"
#include "AnomalyCaptureLabels425.h"
#include "AnomalyCaptureDiagnostic425.h"
class FAnomalyCaptureSession425
{
public:
    bool Start(UWorld* World, const FString& Directory, int32 Count);
    void Tick(UWorld* World,ELevelTick TickType,FName Hook);
    void Stop();
    bool IsRunning() const { return Running; }
    int32 Written() const { return Completed; }
    int32 RequestedFrames() const { return Requested; }
    int32 PlannedFrames() const { return Count; }
private:
    TUniquePtr<IAnomalyCaptureBackend> Backend;
    TUniquePtr<FAnomalyCaptureLabels425> Labels;
    TUniquePtr<FAnomalyCaptureDiagnostic425> Diagnostic;
    TWeakObjectPtr<UWorld> SessionWorld;
    TArray<TFuture<bool>> Writes;
    TArray<FString> Rows;
    FString Output;
    FString RHI;
    uint64 NextId = 1;
    int32 Requested = 0;
    int32 Completed = 0;
    int32 Count = 0;
    int32 Failures = 0;
    int32 PeakPendingWrites = 0;
    int32 WriterDrains = 0;
    bool Running = false;
    double Started = 0;
};
#else
class FAnomalyCaptureSession425 {};
#endif
