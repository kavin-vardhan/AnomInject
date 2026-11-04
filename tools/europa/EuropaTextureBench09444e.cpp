#include "CoreMinimal.h"
#if !UE_BUILD_SHIPPING
#include "Engine/World.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/GameViewportClient.h"
#include "UnrealClient.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
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
    struct FTextureView09445
    {
        TWeakObjectPtr<UWorld> World;
        TWeakObjectPtr<ACameraActor> Camera;
        FVector Location;
        FRotator Rotation;
        float Fov=0;
        FDelegateHandle Handle;
        void Hold()
        {
            auto* W=World.Get(); auto* C=Camera.Get(); auto* PC=W ? W->GetFirstPlayerController() : nullptr;
            if (!C || !PC) { return; }
            C->SetActorLocationAndRotation(Location,Rotation); C->GetCameraComponent()->SetFieldOfView(Fov);
            PC->bAutoManageActiveCameraTarget=false;
            PC->SetControlRotation(Rotation); PC->SetViewTarget(C); PC->bShowMouseCursor=false;
        }
        TSharedPtr<FJsonObject> Read(const TArray<TWeakObjectPtr<UMeshComponent>>& Meshes)
        {
            auto Row=MakeShared<FJsonObject>(); auto* W=World.Get(); auto* PC=W ? W->GetFirstPlayerController() : nullptr;
            bool Valid=PC && Camera.IsValid() && W->GetGameViewport() && W->GetGameViewport()->Viewport;
            if (!Valid) { Row->SetBoolField(TEXT("valid"),false); return Row; }
            const FIntPoint Size=W->GetGameViewport()->Viewport->GetSizeXY();
            const FVector Actual=PC->PlayerCameraManager->GetCameraLocation();
            const FRotator Angle=PC->PlayerCameraManager->GetCameraRotation();
            const float ActualFov=PC->PlayerCameraManager->GetFOVAngle();
            Valid=PC->GetViewTarget()==Camera.Get() && Actual.Equals(Location,0.01f) && Angle.Equals(Rotation,0.01f) && FMath::IsNearlyEqual(ActualFov,Fov,0.01f);
            Row->SetStringField(TEXT("target"),GetNameSafe(PC->GetViewTarget()));
            Row->SetStringField(TEXT("location"),Actual.ToString()); Row->SetStringField(TEXT("rotation"),Angle.ToString());
            Row->SetNumberField(TEXT("fov"),ActualFov); Row->SetNumberField(TEXT("width"),Size.X); Row->SetNumberField(TEXT("height"),Size.Y);
            TArray<TSharedPtr<FJsonValue>> Centers;
            for (const auto& Weak:Meshes) { if (auto* Mesh=Weak.Get())
            {
                FVector2D Pixel; const bool Projected=PC->ProjectWorldLocationToScreen(Mesh->Bounds.Origin,Pixel);
                const bool Inside=Projected && Pixel.X>=0 && Pixel.Y>=0 && Pixel.X<Size.X && Pixel.Y<Size.Y;
                auto C=MakeShared<FJsonObject>(); C->SetStringField(TEXT("actor"),Mesh->GetOwner()->Tags[0].ToString());
                C->SetNumberField(TEXT("x"),Pixel.X); C->SetNumberField(TEXT("y"),Pixel.Y); C->SetBoolField(TEXT("projected"),Projected);
                C->SetBoolField(TEXT("inside"),Inside); Centers.Add(MakeShared<FJsonValueObject>(C)); Valid &= Inside;
            } else { Valid=false; } }
            Row->SetArrayField(TEXT("centers"),Centers); Row->SetBoolField(TEXT("valid"),Valid); return Row;
        }
    };
}

static FAutoConsoleCommandWithWorldAndArgs TextureBench09444e(TEXT("Europa.TextureBench"),TEXT("EA_Core runtime-only locked-view texture qualification"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* World)
    {
        if (!World || !World->IsGameWorld() || World->GetMapName()!=TEXT("EA_Core") || !FParse::Param(FCommandLine::Get(),TEXT("IAIBench")) ||
            Args.Num()!=3 || (Args[1]!=TEXT("calibrate") && Args[1]!=TEXT("run"))) { return; }
        auto* Auto=World->GetSubsystem<UAnomalyAutoInjectorSubsystem>(); auto* PC=World->GetFirstPlayerController();
        if (!Auto || !PC || Auto->IsRunning() || Auto->GetLiveFireCount()) { return; }
        const bool Calibrate=Args[1]==TEXT("calibrate"); const float Frozen=FCString::Atof(*Args[2]);
        if (Frozen>0 || Frozen< -8) { return; }
        const FString Directory=Args[0]; if (IFileManager::Get().DirectoryExists(*Directory)) { return; }
        IFileManager::Get().MakeDirectory(*Directory,true);
        auto View=MakeShared<FTextureView09445>(); View->World=World;
        for (TActorIterator<ACameraActor> It(World);It;++It)
        {
            if (It->ActorHasTag(TEXT("EA_Camera")) || It->GetName()==TEXT("EA_Camera")) { View->Camera=*It; break; }
        }
        if (!View->Camera.IsValid())
        {
            auto* C=World->SpawnActor<ACameraActor>(); if (!C) { return; }
            C->SetActorLocationAndRotation(FVector(-550,0,500),FRotator(-12,0,0)); C->GetCameraComponent()->SetFieldOfView(90);
            View->Camera=C;
        }
        View->Location=View->Camera->GetActorLocation(); View->Rotation=View->Camera->GetActorRotation();
        View->Fov=View->Camera->GetCameraComponent()->FieldOfView; View->Hold();
        View->Handle=FWorldDelegates::OnWorldPreActorTick.AddLambda([View](UWorld* W,ELevelTick,float) { if (W==View->World.Get()) { View->Hold(); } });
        auto Setup=MakeShared<FJsonObject>(); Setup->SetStringField(TEXT("map"),World->GetMapName());
        Setup->SetBoolField(TEXT("bench"),true); Setup->SetBoolField(TEXT("runtime_only"),true); Setup->SetBoolField(TEXT("calibration"),Calibrate);
        Setup->SetNumberField(TEXT("frozen_exposure"),Frozen); Setup->SetStringField(TEXT("view_target"),View->Camera->GetName());
        Setup->SetStringField(TEXT("locked_location"),View->Location.ToString()); Setup->SetStringField(TEXT("locked_rotation"),View->Rotation.ToString());
        Setup->SetNumberField(TEXT("locked_fov"),View->Fov);
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
        int32 Volumes=0; for (TActorIterator<APostProcessVolume> It(World);It;++It)
        {
            It->Settings.bOverride_AutoExposureApplyPhysicalCameraExposure=true; It->Settings.AutoExposureApplyPhysicalCameraExposure=false; ++Volumes;
        }
        Setup->SetNumberField(TEXT("physical_camera_exposure_off_volumes"),Volumes);
        Setup->SetNumberField(TEXT("custom_depth"),IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth"))->GetInt());
        WriteTextureBench09444e(Setup,Directory/TEXT("settings.json"));
        struct FPreview
        {
            int32 Step=-1;
            int32 Attempt=1;
            int32 Phase=0;
            bool Complete=false;
            bool Fired=false;
            double NextAt=0;
            FString Pending;
            TSharedPtr<FJsonObject> Geometry;
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
            TInlineComponentArray<UMeshComponent*> Meshes(*It); for (auto* Mesh:Meshes)
            {
                State->Meshes.Add(Mesh); TArray<UMaterialInterface*> Original;
                for (int32 I=0;I<Mesh->GetNumMaterials();++I) { Original.Add(Mesh->GetMaterial(I)); } State->Original.Add(Original);
            }
        }
        if (State->Meshes.Num()!=3) { return; }
        FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Directory,State,View,Calibrate,Frozen](float)
        {
            UWorld* W=State->Owner.Get(); if (!W) { FWorldDelegates::OnWorldPreActorTick.Remove(View->Handle); return false; }
            View->Hold(); auto* Capture=W->GetSubsystem<UAnomalyCaptureSubsystem>(); auto* Controller=W->GetFirstPlayerController();
            if (!Capture || !Controller || FPlatformTime::Seconds()<State->NextAt) { return true; }
            const FString Attempt=Directory/FString::Printf(TEXT("attempt-%02d"),State->Attempt);
            IFileManager::Get().MakeDirectory(*Attempt,true);
            auto Restore=[State]() { for (int32 M=0;M<State->Meshes.Num();++M) { if (auto* Mesh=State->Meshes[M].Get())
                { for (int32 I=0;I<State->Original[M].Num();++I) { Mesh->SetMaterial(I,State->Original[M][I]); } } } };
            if (!State->Fired && IFileManager::Get().FileExists(*(Attempt/TEXT("reapply-request.json"))))
            {
                if (State->Attempt>=2) { return true; }
                Restore(); ++State->Attempt; State->Step=-1; State->Phase=0; State->Samples.Empty(); State->Complete=false;
                View->Hold(); State->NextAt=FPlatformTime::Seconds()+3; return true;
            }
            if (State->Complete)
            {
                if (!State->Fired && IFileManager::Get().FileExists(*(Attempt/TEXT("prefire-approved.json"))))
                {
                    State->Fired=true; Controller->ConsoleCommand(FString::Printf(TEXT("IAI.Capture.C5 %s 4242"),*(Directory/TEXT("plugin-frames"))),true);
                }
                return true;
            }
            if (State->Phase==0)
            {
                Restore(); ++State->Step;
                if (State->Step==(Calibrate?195:3))
                {
                    auto Row=MakeShared<FJsonObject>(); Row->SetArrayField(TEXT("samples"),State->Samples);
                    Row->SetBoolField(TEXT("auto_injection"),false); Row->SetNumberField(TEXT("preview_count"),State->Step);
                    WriteTextureBench09444e(Row,Attempt/(Calibrate?TEXT("calibration.json"):TEXT("prefire.json")));
                    if (Calibrate) { FWorldDelegates::OnWorldPreActorTick.Remove(View->Handle); FPlatformMisc::RequestExit(false); return false; }
                    State->Complete=true; return true;
                }
                IConsoleManager::Get().FindConsoleVariable(TEXT("r.ExposureOffset"))->Set(Calibrate?-0.125f*(State->Step/3):Frozen,ECVF_SetByConsole);
                State->NextAt=FPlatformTime::Seconds()+0.25; State->Phase=1; return true;
            }
            const FString Geometry=Attempt/FString::Printf(TEXT("geometry-%03d"),State->Step);
            if (State->Phase==1)
            {
                State->Geometry=View->Read(State->Meshes); State->Geometry->SetStringField(TEXT("directory"),Geometry);
                if (!State->Geometry->GetBoolField(TEXT("valid")))
                {
                    WriteTextureBench09444e(State->Geometry,Attempt/TEXT("view-failed.json")); return true;
                }
                Controller->ConsoleCommand(FString::Printf(TEXT("IAI.Capture.BenchGeometry %s"),*Geometry),true); State->Phase=2; return true;
            }
            if (State->Phase==2)
            {
                if (!IFileManager::Get().FileExists(*(Geometry/TEXT("capture.json")))) { return true; }
                WriteTextureBench09444e(State->Geometry,Geometry/TEXT("geometry-ready.json")); State->Phase=3; return true;
            }
            if (State->Phase==3)
            {
                if (!IFileManager::Get().FileExists(*(Geometry/TEXT("geometry-approved.json")))) { return true; }
                const int32 Kind=State->Step%3;
                for (int32 M=0;M<State->Meshes.Num();++M) { if (auto* Mesh=State->Meshes[M].Get())
                { for (int32 I=0;I<State->Original[M].Num();++I) { Mesh->SetMaterial(I,Kind==0?State->Original[M][I]:State->Materials[Kind]); } } }
                State->NextAt=FPlatformTime::Seconds()+0.25; State->Phase=4; return true;
            }
            if (State->Phase==4)
            {
                auto Row=View->Read(State->Meshes); if (!Row->GetBoolField(TEXT("valid"))) { WriteTextureBench09444e(Row,Attempt/TEXT("view-failed.json")); return true; }
                State->Pending=Attempt/FString::Printf(TEXT("preview-%03d"),State->Step);
                Row->SetNumberField(TEXT("step"),State->Step); Row->SetNumberField(TEXT("material"),State->Step%3);
                Row->SetNumberField(TEXT("exposure"),IConsoleManager::Get().FindConsoleVariable(TEXT("r.ExposureOffset"))->GetFloat());
                Row->SetStringField(TEXT("directory"),State->Pending); Row->SetStringField(TEXT("geometry"),Geometry);
                State->Samples.Add(MakeShared<FJsonValueObject>(Row)); Capture->StartRun(State->Pending,true,0,8); State->Phase=5; return true;
            }
            if (!IFileManager::Get().FileExists(*(State->Pending/TEXT("capture.json")))) { return true; }
            Capture->StopRun(); State->Phase=0; return true;
        }));
    }));
#endif
