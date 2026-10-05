#include "CoreMinimal.h"

#if WITH_EDITOR && !UE_BUILD_SHIPPING
#include "AnomalyBenchGate.h"
#include "Containers/Ticker.h"
#include "Editor.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "LevelEditorViewport.h"
#include "PlayInEditorDataTypes.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "Slate/SceneViewport.h"
#include "UnrealEdGlobals.h"

namespace
{
    struct FPausedEditorView
    {
        FLevelEditorViewportClient* Client = nullptr;
        FViewportStateGetter Visibility;
        bool bRealtime = false;
    };

    TArray<FPausedEditorView> PausedEditorViews;
    FDelegateHandle EndPlayHandle;

    void RestoreEditorViews(bool)
    {
        if (GUnrealEd)
        {
            for (const FPausedEditorView& Saved : PausedEditorViews)
            {
                if (GUnrealEd->GetLevelViewportClients().Contains(Saved.Client))
                {
                    Saved.Client->VisibilityDelegate = Saved.Visibility;
                    Saved.Client->SetRealtime(Saved.bRealtime);
                }
            }
        }
        PausedEditorViews.Reset();
        FEditorDelegates::EndPIE.Remove(EndPlayHandle);
        EndPlayHandle.Reset();
    }

    void PlayWindow(const TArray<FString>& Args)
    {
        if (!AnomalyBenchGate::IsEnabled() || !GUnrealEd || GUnrealEd->PlayWorld || Args.Num() != 2)
        {
            return;
        }
        const int32 Width = FCString::Atoi(*Args[0]);
        const int32 Height = FCString::Atoi(*Args[1]);
        if (Width < 320 || Width > 3840 || Height < 240 || Height > 2160)
        {
            return;
        }
        ULevelEditorPlaySettings* Settings = GetMutableDefault<ULevelEditorPlaySettings>();
        Settings->NewWindowWidth = Width;
        Settings->NewWindowHeight = Height;
        Settings->CenterNewWindow = true;
        FRequestPlaySessionParams Params;
        Params.EditorPlaySettings = Settings;
        Params.bAllowOnlineSubsystem = false;
        for (FLevelEditorViewportClient* Viewport : GUnrealEd->GetLevelViewportClients())
        {
            if (Viewport)
            {
                PausedEditorViews.Add({ Viewport, Viewport->VisibilityDelegate, Viewport->IsRealtime() });
                Viewport->SetRealtime(false);
                Viewport->VisibilityDelegate.BindLambda([]() { return false; });
            }
        }
        EndPlayHandle = FEditorDelegates::EndPIE.AddStatic(&RestoreEditorViews);
        GUnrealEd->RequestPlaySession(Params);
        FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Width, Height, Attempts = 0](float) mutable
        {
            UGameViewportClient* Client = GUnrealEd && GUnrealEd->PlayWorld ? GUnrealEd->PlayWorld->GetGameViewport() : nullptr;
            if (Client && Client->GetGameViewport())
            {
                Client->GetGameViewport()->SetFixedViewportSize(Width, Height);
                UE_LOG(LogTemp, Log, TEXT("IAI.Bench.PlayWindow: fixed native viewport %dx%d"), Width, Height);
                return false;
            }
            if (++Attempts >= 600)
            {
                RestoreEditorViews(false);
                return false;
            }
            return true;
        }));
        UE_LOG(LogTemp, Log, TEXT("IAI.Bench.PlayWindow: requested PIE client %dx%d"), Width, Height);
    }

    FAutoConsoleCommand PlayWindowCommand(TEXT("IAI.Bench.PlayWindow"),
        TEXT("Bench-only: request a new PIE window with the given client width and height."),
        FConsoleCommandWithArgsDelegate::CreateStatic(&PlayWindow));
}
#endif
