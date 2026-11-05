#include "AnomalyCaptureSession425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyCaptureSubsystem.h"
#include "AnomalyCaptureLog.h"
#include "AnomalyPixel425.h"
#include "AnomalyWriteQueue425.h"
#include "AnomalyInjectorSubsystem.h"
#include "Engine/GameViewportClient.h"
#include "Misc/App.h"
#include "Camera/PlayerCameraManager.h"
#include "AnomalyPreviewCapture.h"
#include "Async/Async.h"
#include "DynamicRHI.h"
#include "CoreGlobals.h"
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
    if (Running || !World || !World->IsGameWorld() || InCount < 1 || InCount > 8192 || FPaths::IsRelative(Directory) || IFileManager::Get().DirectoryExists(*Directory)) { return false; }
    if (!IFileManager::Get().MakeDirectory(*Directory, true)) { return false; }
    FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
    Backend = AnomalyCompat::CreateCaptureBackend425();
    SessionWorld = World;
    if (AnomalyCaptureC2::Enabled())
    {
        Labels = MakeUnique<FAnomalyCaptureLabels425>();
        if (!Labels->Start(World, Directory, AnomalyCaptureC2::Boundary())) { Labels->Abort(); Labels.Reset(); Backend->Stop(); Backend.Reset(); return false; }
        static_cast<FAnomalyCaptureBackend425*>(Backend.Get())->Labelled(Labels->Epoch());
    }
    Output = Directory; Count = InCount; Requested = Completed = Failures = 0; NextId = 1;
    Writes.Empty(); Rows.Empty(); Running = true; Started = FPlatformTime::Seconds();
    PeakPendingWrites = WriterDrains = 0;
    RHI = GDynamicRHI ? GDynamicRHI->GetName() : TEXT("unknown");
    Diagnostic=MakeUnique<FAnomalyCaptureDiagnostic425>(); Diagnostic->Start(World,Output);
    UE_LOG(LogAnomalyCapture, Log, TEXT("Capture425: start RHI=%s frames=%d native=viewport labels=paired_when_enabled masks=live_targets_when_enabled"), *RHI, Count);
    return true;
}

void FAnomalyCaptureSession425::Tick(UWorld* World,ELevelTick TickType,FName Hook)
{
    if (!Running) { return; }
    Diagnostic->Tick(TickType,Hook);
    if (FPlatformTime::Seconds() - Started < 3.0) { return; }
    FAnomalyCapturedFrame Frame;
    while (Backend->PollFrame(Frame))
    {
        if (Frame.RequestId < 1 || Frame.RequestId > uint64(Count) || Frame.Width <= 0 || Frame.Height <= 0 ||
            Frame.BytesPerPixel != 4 || !AnomalyPixel425::Accept(Frame.Format) || Frame.RawBytes.Num() != Frame.Width * Frame.Height * 4)
        {
            ++Failures; continue;
        }
        const FString Name = FString::Printf(TEXT("frame-%06llu.png"), Frame.RequestId - 1);
        const FString Path = Output / Name;
        const FString MaskOutput = AnomalyCaptureC2::Masks() ? Output : FString();
        if (Labels) { FAnomalyMasks425::Observe(Frame); if (!Labels->Observe(Frame)) { ++Failures; continue; } }
        if (Labels && !Labels->Pair(Frame.LabelKey425, Name)) { ++Failures; continue; }
        Diagnostic->Observe(Frame);
        if (Frame.RequestId==1 || Frame.RequestId==300)
        {
            const int32 Hooks=static_cast<FAnomalyCaptureBackend425*>(Backend.Get())->PostRenderCount();
            UE_LOG(LogAnomalyCapture,Log,TEXT("Capture425 SELF_CHECK stage=%s frames=%llu size=%dx%d format=%d RHI=%s PostRenderViewFamily=%d synth=%d %s %s"),
                Frame.RequestId==1?TEXT("start"):TEXT("300"),Frame.RequestId,
                Frame.Width,Frame.Height,int32(Frame.Format),*RHI,
                Hooks,UAnomalyInjectorSubsystem::IsSynthTickOrderEnabled(World)?1:0,*Diagnostic->Line(),*FAnomalyMasks425::Diagnostic(World));
        }
        Rows.Add(FString::Printf(TEXT("{\"index\":%llu,\"request\":%llu,\"file\":\"%s\",\"width\":%d,\"height\":%d,\"format\":%d,\"aa\":%d}"),
            Frame.RequestId - 1, Frame.RequestId, *Name, Frame.Width, Frame.Height, int32(Frame.Format), Frame.TemporalAaMethod));
        WriterDrains += AnomalyWriteQueue425::MakeRoom(Writes,[this](bool Good) { if (Good) { ++Completed; } else { ++Failures; } });
        Writes.Add(Async(EAsyncExecution::ThreadPool, [Frame = MoveTemp(Frame), Path, MaskOutput]()
        {
            if (!MaskOutput.IsEmpty() && !FAnomalyMasks425::Save(Frame,MaskOutput)) { return false; }
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
        PeakPendingWrites = FMath::Max(PeakPendingWrites,Writes.Num());
    }
    FAnomalyMasks425::Tick(World);
    for (int32 I = Writes.Num() - 1; I >= 0; --I)
    {
        if (Writes[I].IsReady())
        {
            if (Writes[I].Get()) { ++Completed; } else { ++Failures; }
            Writes.RemoveAt(I);
        }
    }
    if (Completed + Failures >= Count || Failures || (Labels && Labels->Failed()) || FPlatformTime::Seconds() - Started >= (AnomalyCaptureC2::Street() ? 2000.0 : Labels ? 480.0 : 80.0))
    {
        Stop(); return;
    }
    if (Requested < Count && (!Labels || Labels->Ready()) && Backend->RequestFrame(World, NextId))
    {
        Diagnostic->Requested(NextId,GFrameCounter);
        if (Labels && !Labels->Snapshot(World, NextId)) { ++Failures; }
        ++NextId; ++Requested;
        if (Labels) { Labels->SetNext(Requested); }
    }
}

void FAnomalyCaptureSession425::Stop()
{
    if (!Running) { return; }
    Running = false;
    Backend->Stop();
    if (!Diagnostic->Stop()) { ++Failures; }
    Diagnostic.Reset();
    for (TFuture<bool>& Write : Writes) { if (Write.Get()) { ++Completed; } else { ++Failures; } }
    Writes.Empty();
    FAnomalyCaptureBackend425* Concrete = static_cast<FAnomalyCaptureBackend425*>(Backend.Get());
    const FAnomalyReadbackLayout Layout = Concrete->Layout();
    const int32 Dropped = Concrete->Dropped() + Failures;
    bool LabelsPassed = true;
    bool LabelsFailed = false;
    const bool HadLabels = bool(Labels);
    if (Labels)
    {
        Labels->Stop(Completed, Dropped, Completed==Count);
        LabelsFailed = Labels->Failed();
        LabelsPassed = !Labels->Failed() && Dropped == 0 && Completed == Count;
        Labels.Reset();
    }
    FString Json = FString::Printf(TEXT("{\"rhi\":\"%s\",\"backbuffer_format\":%d,\"requested\":%d,\"written\":%d,\"dropped\":%d,\"pitch\":%d,\"mapped_height\":%d,\"elapsed_seconds\":%.6f,\"labels\":\"unqualified\",\"masks\":\"unavailable\",\"frames\":["),
        *RHI, Layout.Format, Requested, Completed, Dropped, Layout.RowPitchInPixels, Layout.BufferHeight, FPlatformTime::Seconds() - Started);
    Json += FString::Join(Rows, TEXT(",")) + FString::Printf(TEXT("],\"peak_pending_writes\":%d,\"writer_drains\":%d,\"writer_limit\":%d}"),PeakPendingWrites,WriterDrains,AnomalyWriteQueue425::MaxPending);
    Json.RemoveAt(Json.Len()-1);
    Json+=FString::Printf(TEXT(",\"planned\":%d,\"complete\":%s,\"result\":\"%s\"}"),Count,
        Completed==Count && !Dropped && !LabelsFailed ? TEXT("true") : TEXT("false"),Dropped || LabelsFailed ? TEXT("FAIL") : Completed==Count ? TEXT("PASS") : TEXT("PARTIAL"));
    if (AnomalyCaptureC2::Masks()) { Json.ReplaceInline(TEXT("\"masks\":\"unavailable\""),TEXT("\"masks\":\"stencil-depth-rdg425\"")); }

    const bool Saved = FFileHelper::SaveStringToFile(Json, *(Output / TEXT("capture.json")), FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    UE_LOG(LogAnomalyCapture, Log, TEXT("Capture425: finish RHI=%s written=%d requested=%d dropped=%d receipt=%d"), *RHI, Completed, Requested, Dropped, Saved ? 1 : 0);
    UE_LOG(LogAnomalyCapture, Log, TEXT("Capture425: writer peak=%d limit=%d drains=%d"),PeakPendingWrites,AnomalyWriteQueue425::MaxPending,WriterDrains);
    Backend.Reset();
    if (HadLabels) { AnomalyCaptureC2::Finished(SessionWorld.Get(), LabelsPassed && Saved); }
    else if (FParse::Param(FCommandLine::Get(), TEXT("IAICaptureExit"))) { FPlatformMisc::RequestExit(false); }
}

static FAutoConsoleCommandWithWorldAndArgs CaptureFrames425(TEXT("IAI.Capture.C1"), TEXT("C1 native PNG frames: absolute output directory and count; labels/masks unavailable"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
    {
        if (!World || Args.Num() != 2) { return; }
        APlayerController* PC = World->GetFirstPlayerController();
        if (!PC) { return; }
        const IConsoleVariable* BenchCamera=IConsoleManager::Get().FindConsoleVariable(TEXT("IAI.Capture.BenchCamera"));
        if (FParse::Param(FCommandLine::Get(),TEXT("IAIBench")) && BenchCamera && BenchCamera->GetInt())
        {
            for (TActorIterator<ACameraActor> It(World);It;++It) { PC->SetViewTarget(*It); break; }
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
