#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"
#include "AnomalyCaptureAdmission425.h"
#include "Dom/JsonObject.h"
class UWorld;
class AActor;
namespace AnomalyHostDepth425
{
    struct FScan
    {
        int32 Readers=0,Writers=0,Unknown=0,Materials=0,Entries=0,Volumes=0,CameraBlends=0;
        AnomalyCaptureAdmission425::Depth Result() const { return AnomalyCaptureAdmission425::Classify(Readers,Writers,Unknown); }
        const TCHAR* Name() const;
        TSharedPtr<FJsonObject> Receipt() const;
    };
    FScan Scan(UWorld* World);
    bool Admit(UWorld* World,const FString& Directory);
    bool BenchFixture(UWorld* World);
    bool FixtureActor(const AActor* Actor);
}
#endif
