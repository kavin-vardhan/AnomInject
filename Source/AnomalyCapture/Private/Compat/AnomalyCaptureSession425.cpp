#include "AnomalyCaptureSession425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyCaptureSubsystem.h"
#include "AnomalyCaptureLog.h"
#include "AnomalyPixel425.h"
#include "Camera/PlayerCameraManager.h"
#include "AnomalyPreviewCapture.h"
#include "Async/Async.h"
#include "DynamicRHI.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapperModule.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#if WITH_EDITOR
#include "Editor.h"
#include "PlayInEditorDataTypes.h"
#include "Settings/LevelEditorPlaySettings.h"
#endif

bool FAnomalyCaptureSession425::Start(UWorld* World, const FString& Directory, int32 InCount)
{
    if (Running || !World || !World->IsGameWorld() || InCount < 1 || InCount > 512 || FPaths::IsRelative(Directory) || IFileManager::Get().DirectoryExists(*Directory)) { return false; }
    if (!IFileManager::Get().MakeDirectory(*Directory, true)) { return false; }
    FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
    Backend = AnomalyCompat::CreateCaptureBackend425();
    Output = Directory; Count = InCount; Requested = Completed = Failures = 0; NextId = 1;
    Writes.Empty(); Rows.Empty(); Running = true; Started = FPlatformTime::Seconds();
    RHI = GDynamicRHI ? GDynamicRHI->GetName() : TEXT("unknown");
    UE_LOG(LogAnomalyCapture, Log, TEXT("Capture425: start RHI=%s frames=%d native=1280x720 labels=unqualified masks=unavailable"), *RHI, Count);
    return true;
}

void FAnomalyCaptureSession425::Tick(UWorld* World)
{
    if (!Running) { return; }
    if (FPlatformTime::Seconds() - Started < 3.0) { return; }
    FAnomalyCapturedFrame Frame;
    while (Backend->PollFrame(Frame))
    {
        if (Frame.RequestId < 1 || Frame.RequestId > uint64(Count) || Frame.Width != 1280 || Frame.Height != 720 ||
            Frame.BytesPerPixel != 4 || !AnomalyPixel425::Accept(Frame.Format) || Frame.RawBytes.Num() != 1280 * 720 * 4)
        {
            ++Failures; continue;
        }
        const FString Name = FString::Printf(TEXT("frame-%06llu.png"), Frame.RequestId - 1);
        const FString Path = Output / Name;
        Rows.Add(FString::Printf(TEXT("{\"index\":%llu,\"request\":%llu,\"file\":\"%s\",\"width\":%d,\"height\":%d,\"format\":%d,\"aa\":%d}"),
            Frame.RequestId - 1, Frame.RequestId, *Name, Frame.Width, Frame.Height, int32(Frame.Format), Frame.TemporalAaMethod));
        Writes.Add(Async(EAsyncExecution::ThreadPool, [Frame = MoveTemp(Frame), Path]()
        {
            TArray<FColor> Pixels; Pixels.SetNumUninitialized(Frame.Width * Frame.Height);
            for (int32 I = 0; I < Pixels.Num(); ++I)
            {
                const uint8* P = Frame.RawBytes.GetData() + I * 4;
                const AnomalyPixel425::FPixel Pixel = AnomalyPixel425::Decode(Frame.Format, AnomalyPixel425::Word(P));
                if (!Pixel.Valid) { return false; }
                Pixels[I] = FColor(Pixel.R, Pixel.G, Pixel.B, 255);
            }
            TArray<uint8> Png;
            return AnomalyPreview::EncodePixels(AnomalyPreview::EImageFormat::PNG, Pixels, Frame.Width, Frame.Height, Png, 0) && FFileHelper::SaveArrayToFile(Png, *Path);
        }));
    }
    for (int32 I = Writes.Num() - 1; I >= 0; --I)
    {
        if (Writes[I].IsReady())
        {
            if (Writes[I].Get()) { ++Completed; } else { ++Failures; }
            Writes.RemoveAt(I);
        }
    }
    if (Completed + Failures >= Count || Failures || FPlatformTime::Seconds() - Started >= 80.0)
    {
        Stop(); return;
    }
    if (Requested < Count && Backend->RequestFrame(World, NextId)) { ++NextId; ++Requested; }
}

void FAnomalyCaptureSession425::Stop()
{
    if (!Running) { return; }
    Running = false;
    Backend->Stop();
    for (TFuture<bool>& Write : Writes) { if (Write.Get()) { ++Completed; } else { ++Failures; } }
    Writes.Empty();
    FAnomalyCaptureBackend425* Concrete = static_cast<FAnomalyCaptureBackend425*>(Backend.Get());
    const FAnomalyReadbackLayout Layout = Concrete->Layout();
    const int32 Dropped = Concrete->Dropped() + Failures;
    FString Json = FString::Printf(TEXT("{\"rhi\":\"%s\",\"backbuffer_format\":%d,\"requested\":%d,\"written\":%d,\"dropped\":%d,\"pitch\":%d,\"mapped_height\":%d,\"elapsed_seconds\":%.6f,\"labels\":\"unqualified\",\"masks\":\"unavailable\",\"frames\":["),
        *RHI, Layout.Format, Count, Completed, Dropped, Layout.RowPitchInPixels, Layout.BufferHeight, FPlatformTime::Seconds() - Started);
    Json += FString::Join(Rows, TEXT(",")) + TEXT("]}");
    const bool Saved = FFileHelper::SaveStringToFile(Json, *(Output / TEXT("capture.json")), FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    UE_LOG(LogAnomalyCapture, Log, TEXT("Capture425: finish RHI=%s written=%d requested=%d dropped=%d receipt=%d"), *RHI, Completed, Count, Dropped, Saved ? 1 : 0);
    Backend.Reset();
    if (FParse::Param(FCommandLine::Get(), TEXT("IAICaptureExit"))) { FPlatformMisc::RequestExit(false); }
}

static FAutoConsoleCommandWithWorldAndArgs CaptureFrames425(TEXT("IAI.Capture.C1"), TEXT("C1 native PNG frames: absolute output directory and count; labels/masks unavailable"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
    {
        if (!World || Args.Num() != 2) { return; }
        APlayerController* PC = World->GetFirstPlayerController();
        if (!PC) { return; }
        for (TActorIterator<ACameraActor> It(World); It; ++It)
        {
            if (It->ActorHasTag(TEXT("EA_Camera"))) { PC->SetViewTarget(*It); break; }
        }
        PC->bShowMouseCursor = false;
        UE_LOG(LogAnomalyCapture, Log, TEXT("Capture425: view target=%s location=%s rotation=%s"), *GetNameSafe(PC->GetViewTarget()),
            PC->PlayerCameraManager ? *PC->PlayerCameraManager->GetCameraLocation().ToString() : TEXT("unknown"),
            PC->PlayerCameraManager ? *PC->PlayerCameraManager->GetCameraRotation().ToString() : TEXT("unknown"));
        if (UAnomalyCaptureSubsystem* Capture = World->GetSubsystem<UAnomalyCaptureSubsystem>())
        {
            Capture->StartRun(Args[0], true, 0, FCString::Atoi(*Args[1]));
        }
    }));

#if WITH_EDITOR
static FAutoConsoleCommand CapturePIEStart425(TEXT("IAI.Capture.PIEStart"), TEXT("Begin the C1 in-process PIE viewport"),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        if (!GEditor || GEditor->PlayWorld) { return; }
        FRequestPlaySessionParams Params;
        GEditor->RequestPlaySession(Params);
    }));
static FAutoConsoleCommand CapturePIEEnd425(TEXT("IAI.Capture.PIEEnd"), TEXT("End the C1 PIE viewport"),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        if (GEditor) { GEditor->RequestEndPlayMap(); }
    }));
#endif
#endif
