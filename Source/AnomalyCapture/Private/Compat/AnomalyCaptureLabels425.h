#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"
#include "AnomalyLabelFrame425.h"
#include "Dom/JsonObject.h"
#include "Engine/World.h"
#include "AnomalyMasks425.h"
#include "AnomalyStreetBench425.h"
#include "AnomalyAnnotation425.h"

class FAnomalyCaptureLabels425
{
public:
    bool Start(UWorld* World, const FString& Directory, bool Boundary);
    void SetNext(int32 Index) { Next = Index; }
    bool Snapshot(UWorld* World, uint64 Request);
    bool Observe(const FAnomalyCapturedFrame& Frame);
    bool Ready() const { return !Street || Street->Ready(Next); }
    bool Pair(const FAnomalyLabelKey425& Key, const FString& File);
    void Stop(int32 Written, int32 Dropped);
    uint64 Epoch() const { return RunEpoch; }
    bool Failed() const { return !Failure.IsEmpty(); }
private:
    void Prepare(UWorld* World, ELevelTick TickType, float Delta);
    TUniquePtr<FAnomalyMasks425> MaskOwner;
    TUniquePtr<FAnomalyStreetBench425> Street;
    TMap<FString,AnomalyAnnotation425::FSessionEvent> AnnotationEvents;
    TWeakObjectPtr<UWorld> Owner;
    FDelegateHandle PrepareHandle;
    FString Output;
    FString Failure;
    uint64 RunEpoch = 0;
    int32 Next = 0;
    int32 Prepared = -1;
    bool BoundaryOnly = false;
    bool SavedFixed = false;
    int32 SavedPace = 1;
    double SavedDelta = 0;
    double Started = 0;
    TMap<uint64, TSharedPtr<FJsonObject>> Pending;
    TMap<int32, TSharedPtr<FJsonObject>> Frames;
    TArray<TSharedPtr<FJsonValue>> Commands;
    TMap<FString,TSharedPtr<FJsonObject>> Events;
    TSet<FString> PreviousFires;
    FIntPoint Size = FIntPoint::ZeroValue;
    bool AutoStarted = false;
    bool SavedEnabled = false;
    bool SavedRunning = false;
    bool SavedPersist = false;
    int32 SavedSeed = 0;
    int32 SavedMax = 1;
    float SavedIntervalMin = 0, SavedIntervalMax = 0, SavedHoldMin = 0, SavedHoldMax = 0;
    TArray<FString> SavedIds;
};

namespace AnomalyCaptureC2
{
    bool Enabled();
    bool Street();
    bool Boundary();
    bool Masks();
    void Finished(UWorld* World, bool Success);
}
#endif
