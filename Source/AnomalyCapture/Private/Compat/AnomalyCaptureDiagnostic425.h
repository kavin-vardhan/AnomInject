#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "AnomalyFrameCapturer.h"
#include "HAL/ThreadSafeCounter.h"
#include "Dom/JsonObject.h"
class UWorld;
class UCanvas;
class APlayerController;
class SWindow;
class FAnomalyCaptureDiagnostic425
{
public:
    void Start(UWorld* World,const FString& Directory);
    void Tick(ELevelTick Type,FName Hook);
    void Requested(uint64 Request,uint64 SampleFrame);
    void Observe(const FAnomalyCapturedFrame& Frame);
    FString Line() const;
    bool Stop();
private:
    void Draw(UCanvas* Canvas,APlayerController* PC);
    void Backbuffer(SWindow& Window,const FTexture2DRHIRef& Buffer);
    TWeakObjectPtr<UWorld> Owner;
    TWeakPtr<SWindow> Window;
    FString Output;
    FDelegateHandle DrawHandle,BackbufferHandle;
    FThreadSafeCounter Draws,Checked,Hits,Positive,Controls,Paired;
    FThreadSafeCounter BackbufferFormat;
    FCriticalSection Guard;
    TArray<TArray<FColor>> ControlPixels;
    TArray<TSharedPtr<FJsonValue>> Rows,DrawRows,ControlRows;
    TMap<uint64,uint64> RequestByFrame;
    TSet<uint64> DrawnRequests,ControlledRequests;
    uint64 PresentedRequest=0,PresentedFrame=0;
    FName ObservedHook;
    int32 TickType=-1,HookTicks=0;
    double Started=0;
    bool Enabled=false;
};
#endif
