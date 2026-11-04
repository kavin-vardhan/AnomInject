#if !UE_BUILD_SHIPPING
#include "Engine/World.h"
#include "Engine/PostProcessVolume.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Components/MeshComponent.h"
#include "Materials/Material.h"
#include "AnomalyCaptureSubsystem.h"
#include "AnomalyAutoInjectorSubsystem.h"
#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Containers/Ticker.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
    void WriteTextureBench09444e(const TSharedPtr<FJsonObject>& Row,const FString& Path)
    {
        FString Text; auto Writer=TJsonWriterFactory<>::Create(&Text); FJsonSerializer::Serialize(Row.ToSharedRef(),Writer);
        FFileHelper::SaveStringToFile(Text,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    }
    bool TextureBenchAllowed09444e(UWorld* World)
    {
        return World && World->IsGameWorld() && World->GetMapName()==TEXT("EA_Core") &&
            FParse::Param(FCommandLine::Get(),TEXT("IAIBench"));
    }
}

static FAutoConsoleCommandWithWorldAndArgs TextureBench09444e(TEXT("Europa.TextureBench"),TEXT("EA_Core runtime-only exposure previews or frozen capture"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* World)
    {
        if (!TextureBenchAllowed09444e(World) || Args.Num()!=3 || (Args[1]!=TEXT("calibrate") && Args[1]!=TEXT("run"))) { return; }
        auto* Auto=World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
        auto* PC=World->GetFirstPlayerController();
        if (!Auto || !PC || Auto->IsRunning() || Auto->GetLiveFireCount()) { return; }
        const bool Calibrate=Args[1]==TEXT("calibrate");
        const float Frozen=FCString::Atof(*Args[2]);
        if (Frozen>0 || Frozen< -8) { return; }
        const FString Directory=Args[0];
        if (IFileManager::Get().DirectoryExists(*Directory)) { return; }
        IFileManager::Get().MakeDirectory(*Directory,true);
        auto Setup=MakeShared<FJsonObject>(); Setup->SetStringField(TEXT("map"),World->GetMapName());
        Setup->SetBoolField(TEXT("bench"),true); Setup->SetBoolField(TEXT("runtime_only"),true);
        Setup->SetBoolField(TEXT("calibration"),Calibrate); Setup->SetNumberField(TEXT("frozen_exposure"),Frozen);
        TArray<TSharedPtr<FJsonValue>> CVars;
        const TCHAR* Names[]={TEXT("r.BloomQuality"),TEXT("r.EyeAdaptationQuality"),TEXT("r.EyeAdaptation.MethodOverride"),TEXT("r.ExposureOffset")};
        const float Values[]={0,2,3,Frozen};
        for (int32 I=0;I<4;++I)
        {
            auto* Var=IConsoleManager::Get().FindConsoleVariable(Names[I]); if (!Var) { return; }
            auto R=MakeShared<FJsonObject>(); R->SetStringField(TEXT("name"),Names[I]); R->SetNumberField(TEXT("before"),Var->GetFloat());
            Var->Set(Values[I],ECVF_SetByConsole); R->SetNumberField(TEXT("after"),Var->GetFloat()); CVars.Add(MakeShared<FJsonValueObject>(R));
        }
        Setup->SetArrayField(TEXT("cvars"),CVars);
        int32 Volumes=0;
        for (TActorIterator<APostProcessVolume> It(World);It;++It)
        {
            It->Settings.bOverride_AutoExposureApplyPhysicalCameraExposure=true;
            It->Settings.AutoExposureApplyPhysicalCameraExposure=false; ++Volumes;
        }
        Setup->SetNumberField(TEXT("physical_camera_exposure_off_volumes"),Volumes);
        PC->SetControlRotation(FRotator(-12,0,0));
        Setup->SetNumberField(TEXT("custom_depth"),IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth"))->GetInt());
        WriteTextureBench09444e(Setup,Directory/TEXT("settings.json"));
        struct FPreview
        {
            int32 Step=-1;
            bool Waiting=false;
            bool Complete=false;
            bool Fired=false;
            double NextAt=0;
            FString Pending;
            TArray<TSharedPtr<FJsonValue>> Samples;
            TWeakObjectPtr<UWorld> Owner;
            TArray<TWeakObjectPtr<UMeshComponent>> Meshes;
            TArray<TArray<UMaterialInterface*>> Original;
            TArray<UMaterialInterface*> Materials;
        };
        auto State=MakeShared<FPreview>(); State->Owner=World; State->NextAt=FPlatformTime::Seconds()+15;
        State->Materials={nullptr,LoadObject<UMaterial>(nullptr,TEXT("/AnomalyInjector/Engine425/Materials/M_MissingTexture_Checker.M_MissingTexture_Checker")),
            LoadObject<UMaterial>(nullptr,TEXT("/AnomalyInjector/Engine425/Materials/M_CorruptedTexture_Pink.M_CorruptedTexture_Pink"))};
        if (!State->Materials[1] || !State->Materials[2]) { return; }
        for (TActorIterator<AActor> It(World);It;++It)
        {
            if (!It->ActorHasTag(TEXT("EA_HideCube")) && !It->ActorHasTag(TEXT("EA_BlinkCube")) && !It->ActorHasTag(TEXT("EA_LOD"))) { continue; }
            TInlineComponentArray<UMeshComponent*> Meshes(*It);
            for (auto* Mesh:Meshes)
            {
                State->Meshes.Add(Mesh); TArray<UMaterialInterface*> Original;
                for (int32 I=0;I<Mesh->GetNumMaterials();++I) { Original.Add(Mesh->GetMaterial(I)); }
                State->Original.Add(Original);
            }
        }
        if (State->Meshes.Num()!=3) { return; }
        FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Directory,State,Calibrate,Frozen](float)
        {
            UWorld* W=State->Owner.Get(); if (!W) { return false; }
            auto* Capture=W->GetSubsystem<UAnomalyCaptureSubsystem>(); auto* Controller=W->GetFirstPlayerController();
            if (!Capture || !Controller || FPlatformTime::Seconds()<State->NextAt) { return true; }
            if (State->Complete)
            {
                if (!State->Fired && IFileManager::Get().FileExists(*(Directory/TEXT("prefire-approved.json"))))
                {
                    State->Fired=true;
                    Controller->ConsoleCommand(FString::Printf(TEXT("IAI.Capture.C5 %s 4242"),*(Directory/TEXT("plugin-frames"))),true);
                }
                if (State->Fired && IFileManager::Get().FileExists(*(Directory/TEXT("plugin-frames/epoch01/labels-receipt.json"))))
                { FPlatformMisc::RequestExit(false); return false; }
                return true;
            }
            if (!State->Pending.IsEmpty())
            {
                if (!IFileManager::Get().FileExists(*(State->Pending/TEXT("capture.json")))) { return true; }
                Capture->StopRun(); State->Pending.Empty();
            }
            if (State->Waiting)
            {
                State->Waiting=false; State->Pending=Directory/FString::Printf(TEXT("preview-%03d"),State->Step);
                auto Row=MakeShared<FJsonObject>(); Row->SetNumberField(TEXT("step"),State->Step);
                Row->SetNumberField(TEXT("exposure"),IConsoleManager::Get().FindConsoleVariable(TEXT("r.ExposureOffset"))->GetFloat());
                Row->SetNumberField(TEXT("material"),State->Step%3); Row->SetStringField(TEXT("directory"),State->Pending);
                TArray<TSharedPtr<FJsonValue>> Centers;
                for (const auto& Weak:State->Meshes) { if (auto* Mesh=Weak.Get())
                {
                    FVector2D Pixel; const bool Visible=Controller->ProjectWorldLocationToScreen(Mesh->Bounds.Origin,Pixel);
                    auto C=MakeShared<FJsonObject>(); C->SetStringField(TEXT("actor"),Mesh->GetOwner()->Tags[0].ToString());
                    C->SetNumberField(TEXT("x"),Pixel.X); C->SetNumberField(TEXT("y"),Pixel.Y); C->SetBoolField(TEXT("projected"),Visible);
                    Centers.Add(MakeShared<FJsonValueObject>(C));
                } }
                Row->SetArrayField(TEXT("centers"),Centers); State->Samples.Add(MakeShared<FJsonValueObject>(Row));
                Capture->StartRun(State->Pending,true,0,8); return true;
            }
            ++State->Step;
            const int32 Kind=State->Step%3;
            for (int32 M=0;M<State->Meshes.Num();++M) { if (auto* Mesh=State->Meshes[M].Get())
            {
                for (int32 I=0;I<State->Original[M].Num();++I) { Mesh->SetMaterial(I,Kind==0?State->Original[M][I]:State->Materials[Kind]); }
            } }
            if (State->Step==(Calibrate?195:3))
            {
                auto Row=MakeShared<FJsonObject>(); Row->SetArrayField(TEXT("samples"),State->Samples);
                Row->SetBoolField(TEXT("auto_injection"),false); Row->SetNumberField(TEXT("preview_count"),State->Step);
                WriteTextureBench09444e(Row,Directory/(Calibrate?TEXT("calibration.json"):TEXT("prefire.json")));
                if (Calibrate) { FPlatformMisc::RequestExit(false); return false; }
                State->Complete=true; return true;
            }
            IConsoleManager::Get().FindConsoleVariable(TEXT("r.ExposureOffset"))->Set(Calibrate?-0.125f*(State->Step/3):Frozen,ECVF_SetByConsole);
            State->Waiting=true; State->NextAt=FPlatformTime::Seconds()+0.25; return true;
        }));
    }));
#endif
