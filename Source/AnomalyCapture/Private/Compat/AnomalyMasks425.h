#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "AnomalyFrameCapturer.h"
#include "Dom/JsonObject.h"
#include "AnomalyStencilTag.h"
#include "AnomalyAutoInjectorSubsystem.h"
#include "Containers/Queue.h"
class UWorld;
class FSceneViewFamily;
class FAnomalyMasks425
{
public:
    bool Start(UWorld* World);
    bool Restore();
    TArray<uint8> TagsFor(const FAutoLiveFireInfo& Fire) const;
    const TMap<TWeakObjectPtr<AActor>,uint8>& Assignments() const { return Tags; }
    static bool Labelled(UWorld* World,const FAutoLiveFireInfo& Fire);
    static uint64 Selection(UWorld* World,uint64 Request);
    TSharedPtr<FJsonObject> State() const;
    TArray<TWeakObjectPtr<AActor>> Receivers(const FAutoLiveFireInfo& Fire) const;
    static void SetBenchCensus(bool Enabled);
    static void Observe(const FAnomalyCapturedFrame& Frame);
    static void Tick(UWorld* World);
    static void NoPass(uint64 Request);
    static FString Diagnostic(UWorld* World);
    static bool Render(FRHICommandListImmediate& Cmd, FSceneViewFamily& Family, uint64 Request, uint64 Selected,
        FAnomalyFrameCapturer& Capturer);
    static bool Save(const FAnomalyCapturedFrame& Frame, const FString& Output);
private:
    FAnomalyStencilTagLedger Ledger;
    TWeakObjectPtr<UWorld> Owner;
    TMap<TWeakObjectPtr<AActor>,uint8> Tags;
    TMap<TWeakObjectPtr<UPrimitiveComponent>,ERendererStencilMask> WriteMasks;
    bool Started = false;
    bool Legacy = false;
    struct FCensusValue { int32 Pixels=0,Area=0; uint64 Tick=0; bool Translucent=false; };
    TMap<TWeakObjectPtr<AActor>,FCensusValue> Verdicts;
    struct FBatch { TMap<TWeakObjectPtr<AActor>,uint8> Actors; TSet<uint8> Allowed; uint64 Armed=0; };
    TMap<uint64,FBatch> Batches;
    TQueue<uint64,EQueueMode::Mpsc> NoPassRequests;
    int32 LostBatches=0,NoPassBatches=0,PollutedBatches=0;
    void ReleaseBatch(uint64 Request,bool Requeue);
    struct FEventBinding { uint64 Start=0; TArray<TWeakObjectPtr<AActor>> Actors; TArray<uint8> Values; };
    TMap<FName,FEventBinding> EventTags;
    TSet<TWeakObjectPtr<AActor>> HostActors;
    TArray<TWeakObjectPtr<AActor>> Queue,Visible;
    TSet<FString> InitialVisible;
    int32 Cursor=0,Cycles=0,LastCycleTicks=0,PeakVisible=0,Exhausted=0;
    uint64 CycleStarted=0;
    void Advance(uint64 Request);
    void Release();
    void ReleaseUnused();
    void ReleaseActor(AActor* Actor);
    bool HostDepth(const AActor* Actor) const;
    int32 Tag(AActor* Actor,uint8 Value);
    uint8 FreeTag() const;
};
#endif
