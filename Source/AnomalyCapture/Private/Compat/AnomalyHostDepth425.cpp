#include "AnomalyHostDepth425.h"
#include "AnomalyBenchGate.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyCaptureLabels425.h"
#include "AnomalyCaptureLog.h"
#include "AnomalyStencilTag.h"
#include "AnomalyPrivatePass425.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "MaterialShared.h"
#include "MaterialSceneTextureId.h"
#include "SceneInterface.h"
#include "UObject/Package.h"
#include "HAL/IConsoleManager.h"
#include "Misc/App.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

const TCHAR* AnomalyHostDepth425::FScan::Name() const
{
    using D=AnomalyCaptureAdmission425::Depth;
    switch (Result()) { case D::Reader:return TEXT("reader"); case D::Unknown:return TEXT("unknown"); case D::Writer:return TEXT("writer"); default:return TEXT("none"); }
}
TSharedPtr<FJsonObject> AnomalyHostDepth425::FScan::Receipt() const
{
    auto R=MakeShared<FJsonObject>(); R->SetStringField(TEXT("result"),Name());
    R->SetNumberField(TEXT("readers"),Readers); R->SetNumberField(TEXT("writers"),Writers); R->SetNumberField(TEXT("unknown"),Unknown);
    R->SetNumberField(TEXT("materials"),Materials); R->SetNumberField(TEXT("entries"),Entries); R->SetNumberField(TEXT("volumes"),Volumes); R->SetNumberField(TEXT("camera_blends"),CameraBlends);
    R->SetStringField(TEXT("scope"),TEXT("volume/cached-blend/camera-POV material snapshot; native/custom passes and later additions cannot be certified absent"));
    return R;
}
AnomalyHostDepth425::FScan AnomalyHostDepth425::Scan(UWorld* World)
{
    FScan R;
    if (!World || !World->Scene) { ++R.Unknown; return R; }
    const auto Level=World->Scene->GetFeatureLevel();
    auto Settings=[&](const FPostProcessSettings& S)
    {
        for (const FWeightedBlendable& B:S.WeightedBlendables.Array)
        {
            ++R.Entries;
            UMaterialInterface* MI=Cast<UMaterialInterface>(B.Object);
            if (!MI) { ++R.Unknown; continue; }
            ++R.Materials;
            const FMaterialResource* Resource=MI->GetMaterialResource(Level);
            FMaterialShaderMap* Map=Resource ? Resource->GetGameThreadShaderMap() : nullptr;
            if (!Map) { ++R.Unknown; continue; }
            if (Map->UsesSceneTexture(PPI_CustomDepth) || Map->UsesSceneTexture(PPI_CustomStencil)) { ++R.Readers; }
        }
    };
    for (TActorIterator<APostProcessVolume> It(World);It;++It) { ++R.Volumes; Settings(It->Settings); }
    APlayerController* PC=World->GetFirstPlayerController();
    APlayerCameraManager* Camera=PC ? PC->PlayerCameraManager : nullptr;
    if (!Camera) { ++R.Unknown; }
    else
    {
        const TArray<FPostProcessSettings>* Blends=nullptr; const TArray<float>* Weights=nullptr;
        Camera->GetCachedPostProcessBlends(Blends,Weights);
        if (Blends) { for (const auto& S:*Blends) { ++R.CameraBlends; Settings(S); } }
        ++R.CameraBlends; Settings(Camera->GetCameraCachePOV().PostProcessSettings);
    }
    for (TActorIterator<AActor> It(World);It;++It)
    {
        TInlineComponentArray<UPrimitiveComponent*> Parts(*It);
        for (const auto* Part:Parts) { R.Writers+=Part->bRenderCustomDepth ? 1 : 0; }
    }
    return R;
}
bool AnomalyHostDepth425::Admit(UWorld* World,const FString& Directory)
{
    TSet<TWeakObjectPtr<UPrimitiveComponent>> Before; AnomalyStencilTag::GetTaggedComponents(Before);
    const FScan ScanResult=Scan(World);
    const bool Bench=AnomalyBenchGate::IsCapture425Enabled();
    const bool Bypass=Bench && BenchFixture(World) && FParse::Param(FCommandLine::Get(),TEXT("IAIBenchBypassHostDepth"));
    const bool Private=AnomalyPrivate425::Available(World);
    const bool Allowed=Private || (AnomalyPrivate425::Legacy(World) && AnomalyCaptureAdmission425::Allowed(ScanResult.Result(),Bench,Bypass));
    TSet<TWeakObjectPtr<UPrimitiveComponent>> Tagged; AnomalyStencilTag::GetTaggedComponents(Tagged);
    auto R=ScanResult.Receipt(); R->SetBoolField(TEXT("admitted"),Allowed); R->SetBoolField(TEXT("bench_bypass"),Bypass); R->SetBoolField(TEXT("private_pass"),Private);
    R->SetNumberField(TEXT("tagged_before"),Before.Num()); R->SetNumberField(TEXT("tagged_after_scan"),Tagged.Num());
    const auto* Mode=IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth"));
    R->SetNumberField(TEXT("custom_depth_mode"),Mode ? Mode->GetInt() : -1);
    FString Text; auto Writer=TJsonWriterFactory<>::Create(&Text); FJsonSerializer::Serialize(R.ToSharedRef(),Writer);
    const bool Saved=!AnomalyCaptureC2::Evidence() || FFileHelper::SaveStringToFile(Text,*(Directory/TEXT("host-custom-depth.json")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    UE_LOG(LogAnomalyCapture,Log,TEXT("Capture425 host custom-depth %s readers=%d writers=%d unknown=%d tagged=%d bench_bypass=%d"),ScanResult.Name(),ScanResult.Readers,ScanResult.Writers,ScanResult.Unknown,Tagged.Num(),Bypass?1:0);
    if (!Allowed) { UE_LOG(LogAnomalyCapture,Warning,TEXT("C5 refused: host custom-depth %s"),ScanResult.Name()); }
    return Saved && Allowed && Tagged.Num()==0;
}
bool AnomalyHostDepth425::BenchFixture(UWorld* World)
{
    if (!World || !AnomalyBenchGate::IsCapture425Enabled()) { return false; }
    const FString Package=World->GetOutermost()->GetName();
    return Package==TEXT("/Game/EuropaPhaseA/EA_Core") || Package==TEXT("/Game/EuropaPhaseA/EA_Travel") ||
        (FString(FApp::GetProjectName())==TEXT("Outline425") && Package==TEXT("/Game/OutlineFixture/EA_Outline"));
}
bool AnomalyHostDepth425::FixtureActor(const AActor* Actor)
{
    if (!Actor) { return false; }
    for (const FName Tag:Actor->Tags) { if (Tag.ToString().StartsWith(TEXT("EA_"))) { return true; } }
    return false;
}
#endif
