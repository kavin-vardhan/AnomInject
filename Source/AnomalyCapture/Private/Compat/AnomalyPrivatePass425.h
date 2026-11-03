#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
class UWorld;
class AActor;
class FSceneViewFamily;
class FRHICommandListImmediate;
class FAnomalyFrameCapturer;
namespace AnomalyPrivate425
{
    bool Legacy(UWorld* World);
    bool Available(UWorld* World);
    bool Active();
    void Start(UWorld* World, const FString& Directory);
    void Stop();
    bool Supports(const AActor* Actor);
    bool Verified(const AActor* Actor);
    TSharedPtr<FJsonObject> State(bool Full=false);
    void Prepare(uint64 Request, const TMap<TWeakObjectPtr<AActor>, uint8>& Actors);
    void BindView(uint64 Request);
    void Poll_RenderThread(FRHICommandListImmediate& Cmd);
    bool Render(FRHICommandListImmediate& Cmd, FSceneViewFamily& Family, uint64 Request, uint64 Selected,
        FAnomalyFrameCapturer& Capturer);
}
#endif
