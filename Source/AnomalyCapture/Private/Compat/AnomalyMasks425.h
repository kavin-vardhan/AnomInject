#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "AnomalyFrameCapturer.h"
#include "AnomalyStencilTag.h"
#include "AnomalyAutoInjectorSubsystem.h"
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
    static uint64 Selection(UWorld* World);
    static bool Render(FRHICommandListImmediate& Cmd, FSceneViewFamily& Family, uint64 Request, uint64 Selected,
        FAnomalyFrameCapturer& Capturer);
    static bool Save(const FAnomalyCapturedFrame& Frame, const FString& Output);
private:
    FAnomalyStencilTagLedger Ledger;
    TWeakObjectPtr<UWorld> Owner;
    TMap<TWeakObjectPtr<AActor>,uint8> Tags;
    TMap<TWeakObjectPtr<UPrimitiveComponent>,ERendererStencilMask> WriteMasks;
    bool Started = false;
};
#endif
