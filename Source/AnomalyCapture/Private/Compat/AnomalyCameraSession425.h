#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"
#include "AnomalyCaptureSubsystem.h"
#include "AnomalyAutoInjectorSubsystem.h"
#include "Dom/JsonObject.h"
class FAnomalyCameraSession425
{
public:
    ~FAnomalyCameraSession425();
    static int32 FrameCount(UWorld* World,const TArray<FString>& Routes);
    bool Start(UWorld* World,const TArray<FString>& Routes,const FString& Directory,bool AllowBench=true);
    void Append(const TSharedPtr<FJsonObject>& Row,TArray<FAutoLiveFireInfo>& Fires);
    void Prepare(int32 Index);
    bool Stop();
    TSharedPtr<FJsonObject> Summary() const;
private:
    TWeakObjectPtr<UWorld> Owner;
    TWeakObjectPtr<AActor> Camera,PriorTarget;
    FVector FarPosition=FVector::ZeroVector,NearPosition=FVector::ZeroVector,PriorPosition=FVector::ZeroVector;
    FRotator FarRotation=FRotator::ZeroRotator,PriorRotation=FRotator::ZeroRotator;
    float PriorFov=0,Baseline=0;
    bool Active=false,Bench=false,FarOnly=false,PriorAutoManage=true;
    uint64 StartFrame=0;
    int32 NearStart=624,PoseHold=288;
    FString Output;
    FCameraClipRunAccum Accum;
};
#endif
