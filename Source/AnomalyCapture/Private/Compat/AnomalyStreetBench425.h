#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Components/PrimitiveComponent.h"
#include "AnomalyFrameCapturer.h"
class UWorld;
class AActor;
class FAnomalyStreetBench425
{
public:
    static constexpr int32 Poses=6,Turn=18,Settle=30,CleanHold=28,ScoreHold=240;
    static constexpr int32 PreFrames=Poses*(Turn+Settle+2*CleanHold);
    static constexpr int32 TotalFrames=PreFrames+Poses*(Turn+Settle+ScoreHold);
    bool Start(UWorld* World,const FString& Output);
    void Prepare(int32 Index);
    bool Observe(const FAnomalyCapturedFrame& Frame,const TSharedPtr<FJsonObject>& Row);
    TSharedPtr<FJsonObject> Describe(int32 Index) const;
    TSharedPtr<FJsonObject> DepthSample(bool Full=false) const;
    bool Ready(int32 Index) const;
    bool AutoOn(int32 Index) const;
    bool Stop();
private:
    struct FPrior { bool On=false; int32 Value=0; ERendererStencilMask Mask=ERendererStencilMask::ERSM_Default; };
    struct FPhase { TArray<FColor> Reference,Minimum,Maximum; int32 Index=-1,Count=0; };
    struct FPose
    {
        FPhase Phase[7]; TArray<uint8> HalfMap,Bad; TArray<TSharedPtr<FJsonValue>> Samples,OffSamples,SelfFrames,InterferenceFrames;
        int32 Count=0,OnCount=0; double Unstable=1,SelfMax=0,InterferenceMax=0;
        bool Keep=false,OffReady=false,Complete=false;
    };
    TWeakObjectPtr<UWorld> Owner;
    TWeakObjectPtr<AActor> Target;
    TMap<TWeakObjectPtr<UPrimitiveComponent>,FPrior> Before,Original;
    FRotator Base,TargetRotation,OriginalViewRotation;
    FVector Position;
    bool IgnoreMove=false,IgnoreLook=false,Frozen=false;
    FString Directory;
    FPose Pose[Poses];
    bool Freeze(int32 Width,int32 Height);
};
#endif
