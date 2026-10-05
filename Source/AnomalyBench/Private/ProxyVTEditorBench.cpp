#include "CoreMinimal.h"

#if WITH_EDITOR && !UE_BUILD_SHIPPING
#include "AnomalyBenchGate.h"
#include "Editor/UnrealEdEngine.h"
#include "HAL/IConsoleManager.h"
#include "PlayInEditorDataTypes.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "UnrealEdGlobals.h"

namespace
{
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
        GUnrealEd->RequestPlaySession(Params);
        UE_LOG(LogTemp, Log, TEXT("IAI.Bench.PlayWindow: requested PIE client %dx%d"), Width, Height);
    }

    FAutoConsoleCommand PlayWindowCommand(TEXT("IAI.Bench.PlayWindow"),
        TEXT("Bench-only: request a new PIE window with the given client width and height."),
        FConsoleCommandWithArgsDelegate::CreateStatic(&PlayWindow));
}
#endif
