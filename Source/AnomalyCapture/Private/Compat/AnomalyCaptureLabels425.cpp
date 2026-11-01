#include "AnomalyCaptureLabels425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyCaptureSubsystem.h"
#include "AnomalyCaptureLog.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyAutoInjectorSubsystem.h"
#include "AnomalySelectorSubsystem.h"
#include "AnomalyViewport.h"
#include "AnomalyFrozenGeometry.h"
#include "Camera/CameraActor.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "ShaderCompiler.h"
#include "ContentStreaming.h"
#include "Misc/EngineVersion.h"
#include "Misc/DateTime.h"
#include "Misc/App.h"
#include "Containers/Ticker.h"
#include "CoreGlobals.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "Engine/GameViewportClient.h"
#include "UnrealClient.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Misc/App.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Policies/CondensedJsonPrintPolicy.h"

namespace
{
    uint64 CaptureEpoch425 = 0;
    int32 CapturePace425 = 1;
    bool CaptureLive425 = false;
    bool CaptureStreet425 = false;
    int32 CaptureBenchCamera425 = 0;
    FAutoConsoleVariableRef CaptureBenchCameraCommand425(TEXT("IAI.Capture.BenchCamera"),CaptureBenchCamera425,TEXT("Bench-only first-camera override; default off"));
    int32 CaptureSeed425 = 4242;
    int32 CaptureCount425 = 1440;
    const TCHAR* CaptureRoutes425[] = { TEXT("blinking"),TEXT("lighting_mismatch"),TEXT("lod_corruption"),TEXT("missing_object") };
    FAutoConsoleVariableRef CapturePaceCommand425(TEXT("IAI.Capture.Pace"),CapturePace425,TEXT("4.25 capture pacing"));
    FString Json425(const TSharedPtr<FJsonObject>& Object)
    {
        FString Text;
        auto Writer = TJsonWriterFactory<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>::Create(&Text);
        FJsonSerializer::Serialize(Object.ToSharedRef(),Writer); return Text;
    }
    FString FireKey425(const FAutoLiveFireInfo& Fire) { return FString::Printf(TEXT("%s@%llu"),*Fire.Id.ToString(),Fire.StartFrame); }
}

bool FAnomalyCaptureLabels425::Start(UWorld* World,const FString& Directory,bool Boundary)
{
    if (!World || !World->IsGameWorld() || !World->GetGameViewport() || !World->GetGameViewport()->Viewport) { return false; }
    UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
    if (!Auto || Auto->IsRunning() || Auto->GetLiveFireCount()) { return false; }
    Owner = World; Output = Directory; RunEpoch = ++CaptureEpoch425; Size = World->GetGameViewport()->Viewport->GetSizeXY();
    Started = FPlatformTime::Seconds(); SavedFixed = FApp::UseFixedTimeStep(); SavedDelta = FApp::GetFixedDeltaTime(); SavedPace = CapturePace425;
    SavedEnabled = Auto->IsEnabled(); SavedRunning = Auto->IsRunning(); SavedSeed = Auto->GetSeed(); SavedMax = Auto->GetMaxConcurrent(); SavedPersist = Auto->GetPersist(); SavedIds = Auto->GetEnabledIds();
    Auto->GetIntervalRange(SavedIntervalMin,SavedIntervalMax); Auto->GetHoldRange(SavedHoldMin,SavedHoldMax);
    if (CaptureStreet425)
    {
        Street=MakeUnique<FAnomalyStreetBench425>();
        if (!Street->Start(World,Directory)) { Street->Stop(); Street.Reset(); return false; }
    }
    MaskOwner = MakeUnique<FAnomalyMasks425>();
    if (!MaskOwner->Start(World)) { MaskOwner->Restore(); MaskOwner.Reset(); return false; }
    Auto->SetAllAnomaliesEnabled(false);
    for (const TCHAR* Id : CaptureRoutes425) { if (!Auto->SetAnomalyEnabled(FName(Id),true)) { Failure = TEXT("Auto pool admission failed"); } }
    Auto->SetSeed(CaptureSeed425); Auto->SetIntervalRange(0.8f,0.8f); Auto->SetHoldRange(0.4f,0.4f); Auto->SetMaxConcurrent(1); Auto->SetPersist(false); Auto->SetEnabled(true);
    FApp::SetFixedDeltaTime(1.0/30.0); FApp::SetUseFixedTimeStep(true); CapturePace425 = 1;
    PrepareHandle = FWorldDelegates::OnWorldPreActorTick.AddRaw(this,&FAnomalyCaptureLabels425::Prepare);
    return !Failed();
}

void FAnomalyCaptureLabels425::Prepare(UWorld* World,ELevelTick TickType,float Delta)
{
    if (World != Owner.Get() || TickType != LEVELTICK_All || Prepared == Next || Failed() || FPlatformTime::Seconds()-Started < 3) { return; }
    if (Street && !Street->Ready(Next)) { return; }
    Prepared = Next;
    if (Street) { Street->Prepare(Next); }
    UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
    if (!Auto) { Failure = TEXT("Auto injector missing"); return; }
    if (Street)
    {
        const bool Wanted=Street->AutoOn(Next);
        if (Wanted!=Auto->IsRunning())
        {
            if (Wanted && !AutoStarted)
            {
                auto R=MakeShared<FJsonObject>(); R->SetStringField(TEXT("utc"),FDateTime::UtcNow().ToIso8601()); R->SetNumberField(TEXT("first_scored_index"),Next);
                FFileHelper::SaveStringToFile(Json425(R),*(Output/TEXT("scored-start.json")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM); AutoStarted=true;
            }
            Auto->SetRunning(Wanted);
        }
    }
    else
    {
        if (!AutoStarted && Next>=3) { AutoStarted=true; Auto->SetRunning(true); }
        if (Next>=CaptureCount425-24 && Auto->IsRunning()) { Auto->SetRunning(false); }
    }
}

bool FAnomalyCaptureLabels425::Snapshot(UWorld* World,uint64 Request)
{
    if (World != Owner.Get() || Pending.Contains(Request) || Pending.Num() >= 8 || Failed()) { return false; }
    UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
    if (!Auto || !MaskOwner) { return false; }
    auto Row = MakeShared<FJsonObject>();
    Row->SetNumberField(TEXT("frame_index"),double(Request-1)); Row->SetNumberField(TEXT("session_index"),double(Request-1));
    Row->SetNumberField(TEXT("epoch"),double(RunEpoch)); Row->SetNumberField(TEXT("request"),double(Request));
    Row->SetNumberField(TEXT("world_owner"),World->GetUniqueID()); Row->SetStringField(TEXT("world"),World->GetPathName());
    Row->SetNumberField(TEXT("sample_frame"),double(GFrameCounter)); Row->SetNumberField(TEXT("t"),World->GetTimeSeconds());
    Row->SetNumberField(TEXT("t_wall"),FPlatformTime::Seconds()-Started);
    Row->SetNumberField(TEXT("width"),Size.X); Row->SetNumberField(TEXT("height"),Size.Y);
    Row->SetNumberField(TEXT("pace"),CapturePace425); Row->SetNumberField(TEXT("tick_order"),0);
    Row->SetBoolField(TEXT("fixed_step"),FApp::UseFixedTimeStep());
    Row->SetObjectField(TEXT("census"),MaskOwner->State());
    if (Street) { Row->SetObjectField(TEXT("sweep"),Street->Describe(int32(Request-1))); Row->SetObjectField(TEXT("custom_depth"),Street->DepthSample()); }
    TSet<FString> Live;
    TArray<TSharedPtr<FJsonValue>> States,Anomalies;
    for (const auto& Fire : Auto->GetLiveFires())
    {
        const FString Key = FireKey425(Fire); Live.Add(Key);
        const TArray<uint8> Tags = MaskOwner->TagsFor(Fire);

        const bool Labelled = FAnomalyMasks425::Labelled(World,Fire);
        auto State = MakeShared<FJsonObject>();
        State->SetStringField(TEXT("id"),Fire.Id.ToString()); State->SetStringField(TEXT("event_id"),Key);
        State->SetStringField(TEXT("target_name"),Fire.Target); State->SetNumberField(TEXT("start_frame"),double(Fire.StartFrame));
        State->SetNumberField(TEXT("seconds_remaining"),Fire.SecondsRemaining);
        State->SetBoolField(TEXT("fire_active"),true); State->SetBoolField(TEXT("injected"),Labelled); State->SetBoolField(TEXT("labelled"),Labelled);
        TArray<TSharedPtr<FJsonValue>> ReceiverNames;
        for (const auto& A:MaskOwner->Receivers(Fire)) { ReceiverNames.Add(MakeShared<FJsonValueString>(GetNameSafe(A.Get()))); }
        State->SetArrayField(TEXT("receiver_names"),ReceiverNames);
        TArray<TSharedPtr<FJsonValue>> Values;
        for (uint8 Tag : Tags) { Values.Add(MakeShared<FJsonValueNumber>(Tag)); }
        State->SetArrayField(TEXT("target_tags"),Values);
        FAnomalyViewInfo View; AnomalyViewport::GetActiveViewInfo(World,View);
        FBox Box(ForceInit); const AActor* Actor=Fire.TargetActor.Get();
        const bool FoundBox=Actor && AnomalyViewport::GetActorLabelBounds(Actor,Box);
        const auto Geometry=AnomalyFrozenGeometry::Freeze(Fire.bWholeFrameExtent,Actor!=nullptr,!Fire.Target.IsEmpty(),FoundBox,Box);
        FVector2D Min,Max;
        AnomalyFrozenGeometry::Resolve(Geometry,View,[](const FAnomalyViewInfo& V,const FBox& B,FVector2D& A,FVector2D& Z)
        { return AnomalyViewport::ProjectBoxToScreenRect(V,B,A,Z); },Min,Max);
        const double Coverage=FMath::Max(0.0,double(FMath::Clamp(Max.X,0.0f,1.0f)-FMath::Clamp(Min.X,0.0f,1.0f)))*FMath::Max(0.0,double(FMath::Clamp(Max.Y,0.0f,1.0f)-FMath::Clamp(Min.Y,0.0f,1.0f)));
        State->SetNumberField(TEXT("projected_screen_share"),Coverage);
        States.Add(MakeShared<FJsonValueObject>(State));
        if (Labelled) { Anomalies.Add(MakeShared<FJsonValueObject>(State)); }
        if (!Events.Contains(Key))
        {
            auto Event = MakeShared<FJsonObject>(*State); Event->SetNumberField(TEXT("first_index"),double(Request-1));
            Events.Add(Key,Event);
            auto& E=AnnotationEvents.FindOrAdd(Key); E.EventId=Key+TEXT("|")+Fire.Target;
            E.AnomalyType=Fire.Id==FName(TEXT("blinking"))?TEXT("blink"):Fire.Id.ToString(); E.AnomalySubtype=Fire.Id==FName(TEXT("blinking"))?TEXT("disappear_reappear"):Fire.Id.ToString();
            E.EngineName=TEXT("UnrealEngine"); E.EngineVersion=FEngineVersion::Current().ToString(); E.EngineProject=FApp::GetProjectName(); E.TicksMsec=int64(World->GetTimeSeconds()*1000.0);
            E.BboxSource=TEXT("projected");
            for (const auto& Weak:MaskOwner->Receivers(Fire))
            {
                AActor* A=Weak.Get(); if (!A) { continue; }
                AnomalyAnnotation425::FSessionNode N; N.Name=A->GetName(); N.Path=A->GetPathName(); N.GlobalPosition=A->GetActorLocation();
                A->GetActorBounds(false,N.BoundsOrigin,N.BoundsExtent);
                if (auto* C=A->FindComponentByClass<UStaticMeshComponent>()) { N.ComponentClass=C->GetClass()->GetName(); N.AssetName=GetNameSafe(C->GetStaticMesh()); }
                E.Nodes.Add(N);
            }
            auto Command = MakeShared<FJsonObject>(*State); Command->SetStringField(TEXT("command"),TEXT("observed_auto_apply"));
            Command->SetNumberField(TEXT("process_frame"),double(GFrameCounter)); Commands.Add(MakeShared<FJsonValueObject>(Command));
        }
    }
    for (const FString& Key : PreviousFires)
    {
        if (!Live.Contains(Key))
        {
            Events[Key]->SetNumberField(TEXT("end_frame"),double(GFrameCounter));
            Events[Key]->SetNumberField(TEXT("end_index"),double(Request-1));
            auto Command = MakeShared<FJsonObject>(); Command->SetStringField(TEXT("event_id"),Key);
            Command->SetStringField(TEXT("command"),TEXT("observed_auto_revert")); Command->SetNumberField(TEXT("process_frame"),double(GFrameCounter));
            Commands.Add(MakeShared<FJsonValueObject>(Command));
        }
    }
    PreviousFires = Live;
    Row->SetArrayField(TEXT("sampled_states"),States); Row->SetArrayField(TEXT("anomalies"),Anomalies);
    Row->SetBoolField(TEXT("anomaly_present"),Anomalies.Num() != 0);
    Pending.Add(Request,Row); return true;
}

bool FAnomalyCaptureLabels425::Observe(const FAnomalyCapturedFrame& Frame)
{
    auto* Found=Pending.Find(Frame.RequestId); if (!Found) { return false; }
    auto Row=*Found; const uint32* Data=reinterpret_cast<const uint32*>(Frame.MaskData425.GetData());
    uint32 Counts[256]={};
    for (int32 I=0;I<Frame.Width*Frame.Height;++I) { ++Counts[Data[I*4+3]&255]; }
    TArray<TSharedPtr<FJsonValue>> Affected;
    for (const auto& Value:Row->GetArrayField(TEXT("sampled_states")))
    {
        auto State=Value->AsObject(); uint32 Pixels=0,Drawn=0;
        for (const auto& Tag:State->GetArrayField(TEXT("target_tags"))) { Pixels+=Counts[int32(Tag->AsNumber())]; Drawn+=Frame.MaskTable425[int32(Tag->AsNumber())*6+5]; }
        const bool Injected=State->GetBoolField(TEXT("injected")),Visible=Injected && Pixels>0;
        State->SetBoolField(TEXT("labelled"),Visible); State->SetBoolField(TEXT("affected"),Visible); State->SetNumberField(TEXT("mask_pixels"),Pixels); State->SetNumberField(TEXT("drawn_pixels"),Drawn);
        if (Drawn) { AnnotationEvents[State->GetStringField(TEXT("event_id"))].BboxSource=TEXT("drawn"); }
        auto& E=AnnotationEvents[State->GetStringField(TEXT("event_id"))];
        if (Injected) { E.InjectedFrameIndices.Add(int32(Frame.RequestId-1)); }
        if (Visible) { CoverageSum.FindOrAdd(State->GetStringField(TEXT("event_id")))+=State->GetNumberField(TEXT("projected_screen_share")); ++CoverageCount.FindOrAdd(State->GetStringField(TEXT("event_id"))); E.FrameIndices.Add(int32(Frame.RequestId-1)); ++E.ObservableFrameCount; E.bMaskProvided=true; Affected.Add(Value); }
    }
    Row->SetArrayField(TEXT("anomalies"),Affected); Row->SetBoolField(TEXT("anomaly_present"),Affected.Num()!=0);
    Row->SetBoolField(TEXT("mask_nonempty"),Counts[0]!=uint32(Frame.Width*Frame.Height));
    Row->SetNumberField(TEXT("grain_phase"),Frame.LabelKey425.GrainPhase);
    return !Street || Street->Observe(Frame,Row);
}

bool FAnomalyCaptureLabels425::Pair(const FAnomalyLabelKey425& Key, const FString& File)
{
    TSharedPtr<FJsonObject>* Found = Pending.Find(Key.Request);
    if (!Found || !Owner.IsValid() || !AnomalyLabelKey425::Same(RunEpoch, Owner->GetUniqueID(), Key.Request, Key.Epoch, Key.Owner, Key.Request) ||
        Frames.Contains(Key.SessionIndex) || Key.SessionIndex != int32(Key.Request - 1) || Key.AA != 0 ||
        (*Found)->GetNumberField(TEXT("sample_frame")) != double(Key.SampleFrame))
    {
        Failure = TEXT("Unpaired, stale or duplicate label key"); return false;
    }
    auto Row = *Found;
    if (MaskOwner)
    {
        if (!Key.Masks) { Failure = TEXT("Mask family not paired"); return false; }
        const IConsoleVariable* EyeAdaptation = IConsoleManager::Get().FindConsoleVariable(TEXT("r.EyeAdaptationQuality"));
        Row->SetNumberField(TEXT("eye_adaptation_quality"),EyeAdaptation ? EyeAdaptation->GetInt() : -1);
        if (Row->GetBoolField(TEXT("mask_nonempty")))
        {
            Row->SetStringField(TEXT("mask"),FString::Printf(TEXT("mask-%06d.png"),Key.SessionIndex));
            Row->SetStringField(TEXT("mask_state"),TEXT("present"));
        }
        else
        {
            Row->SetField(TEXT("mask"),MakeShared<FJsonValueNull>());
            Row->SetStringField(TEXT("mask_state"),TEXT("empty"));
        }
        Row->SetStringField(TEXT("mask_evidence"),FString::Printf(TEXT("evidence-%06d.u32x4.zlib"),Key.SessionIndex));
        Row->SetNumberField(TEXT("mask_target_tag"),Key.MaskTarget);
    }
    Row->SetStringField(TEXT("image"), File); Row->SetNumberField(TEXT("family_frame"), Key.FamilyFrame);
    Row->SetNumberField(TEXT("aa"), Key.AA); Row->SetStringField(TEXT("pairing"), TEXT("post_actor_tick_to_post_render_family"));
    auto Vector=[](const FVector& V) { return TArray<TSharedPtr<FJsonValue>>{MakeShared<FJsonValueNumber>(V.X),MakeShared<FJsonValueNumber>(V.Y),MakeShared<FJsonValueNumber>(V.Z)}; };
    auto Camera=MakeShared<FJsonObject>(); Camera->SetArrayField(TEXT("player_location"),Vector(Key.PlayerLocation)); Camera->SetArrayField(TEXT("view_location"),Vector(Key.ViewLocation));
    Camera->SetArrayField(TEXT("player_rotation"),Vector(FVector(Key.PlayerRotation.Pitch,Key.PlayerRotation.Yaw,Key.PlayerRotation.Roll)));
    Camera->SetArrayField(TEXT("view_rotation"),Vector(FVector(Key.ViewRotation.Pitch,Key.ViewRotation.Yaw,Key.ViewRotation.Roll)));
    Camera->SetNumberField(TEXT("player_fov"),Key.PlayerFov); Camera->SetNumberField(TEXT("view_fov"),Key.ViewFov); Camera->SetStringField(TEXT("target"),Key.PlayerTarget);
    Camera->SetNumberField(TEXT("near"),Key.ViewNear); Camera->SetNumberField(TEXT("far"),Key.ViewFar);
    Camera->SetBoolField(TEXT("far_infinite"),Key.ViewFar==0); Row->SetNumberField(TEXT("grain_phase"),Key.GrainPhase);
    Row->SetObjectField(TEXT("camera"),Camera);
    for (const auto& S:Row->GetArrayField(TEXT("sampled_states")))
    {
        auto& E=AnnotationEvents[S->AsObject()->GetStringField(TEXT("event_id"))];
        if (E.CamPath.IsEmpty()) { E.CamPath=Key.PlayerTarget; E.CamPosition=Key.PlayerLocation; E.CamRotation=Key.PlayerRotation; E.CamFovDeg=Key.PlayerFov; E.CamAspect=float(Size.X)/Size.Y; E.CamNear=Key.ViewNear; E.CamFar=Key.ViewFar; }
    }
    Frames.Add(Key.SessionIndex, Row); Pending.Remove(Key.Request);
    return true;
}


void FAnomalyCaptureLabels425::Stop(int32 Written,int32 Dropped)
{
    FWorldDelegates::OnWorldPreActorTick.Remove(PrepareHandle); PrepareHandle.Reset();
    if (Owner.IsValid())
    {
        if (UAnomalyAutoInjectorSubsystem* Auto = Owner->GetSubsystem<UAnomalyAutoInjectorSubsystem>())
        {
            Auto->SetRunning(false); Auto->SetAllAnomaliesEnabled(false);
            for (const auto& Id : SavedIds) { Auto->SetAnomalyEnabled(FName(*Id),true); }
            Auto->SetSeed(SavedSeed); Auto->SetIntervalRange(SavedIntervalMin,SavedIntervalMax); Auto->SetHoldRange(SavedHoldMin,SavedHoldMax);
            Auto->SetMaxConcurrent(SavedMax); Auto->SetPersist(SavedPersist); Auto->SetEnabled(SavedEnabled);
        }
    }
    TArray<TSharedPtr<FJsonValue>> Owners;
    if (MaskOwner)
    {
        for (const auto& Pair : MaskOwner->Assignments())
        {
            auto Node = MakeShared<FJsonObject>(); Node->SetStringField(TEXT("actor"),GetNameSafe(Pair.Key.Get())); Node->SetNumberField(TEXT("tag"),Pair.Value);
            Owners.Add(MakeShared<FJsonValueObject>(Node));
        }
    }
    const bool Restored = !MaskOwner || MaskOwner->Restore();
    if (!Restored || (Street && !Street->Stop())) { Failure = TEXT("Stencil restoration failed"); }
    FApp::SetFixedDeltaTime(SavedDelta); FApp::SetUseFixedTimeStep(SavedFixed); CapturePace425 = SavedPace;
    TArray<int32> Indices; Frames.GetKeys(Indices); Indices.Sort();
    FString Lines;
    for (int32 Index : Indices) { Lines += Json425(Frames[Index])+TEXT("\n"); }
    TArray<TSharedPtr<FJsonValue>> EventRows;
    TArray<TSharedPtr<FJsonObject>> SortedEvents;
    for (const auto& Pair : Events) { SortedEvents.Add(Pair.Value); }
    SortedEvents.Sort([](const TSharedPtr<FJsonObject>& A,const TSharedPtr<FJsonObject>& B) { return A->GetNumberField(TEXT("start_frame")) < B->GetNumberField(TEXT("start_frame")); });
    for (const auto& Event : SortedEvents) { EventRows.Add(MakeShared<FJsonValueObject>(Event)); }
    AnomalyAnnotation425::FSessionAnnotation Annotation; Annotation.SessionId=FPaths::GetCleanFilename(Output);
    Annotation.Video.FramesDir=TEXT("."); Annotation.Video.ResolutionW=Size.X; Annotation.Video.ResolutionH=Size.Y;
    Annotation.Video.TotalFrames=Written; Annotation.Video.Fps=30; Annotation.Video.TargetFps=30;
    for (const auto& Event:SortedEvents)
    {
        auto E=AnnotationEvents[Event->GetStringField(TEXT("event_id"))]; E.FrameIndices.Sort(); E.InjectedFrameIndices.Sort();
        E.bManifested=E.InjectedFrameIndices.Num()>0; E.CoverageRatio=CoverageSum.FindRef(Event->GetStringField(TEXT("event_id")))/FMath::Max(1,CoverageCount.FindRef(Event->GetStringField(TEXT("event_id"))));
        Annotation.Events.Add(E);
    }
    auto Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("result"),Failed() || Pending.Num() || Dropped || Written != Frames.Num() ? TEXT("FAIL") : TEXT("PASS"));
    Result->SetStringField(TEXT("failure"),Failure); Result->SetNumberField(TEXT("paired"),Frames.Num());
    Result->SetNumberField(TEXT("unmatched_dropped"),Pending.Num()); Result->SetNumberField(TEXT("dropped"),Dropped);
    Result->SetBoolField(TEXT("mask_state_restored"),Restored);
    Result->SetBoolField(TEXT("pacing_restored"),FApp::UseFixedTimeStep()==SavedFixed && FApp::GetFixedDeltaTime()==SavedDelta);
    Result->SetArrayField(TEXT("commands"),Commands); Result->SetNumberField(TEXT("seed"),CaptureSeed425);
    Result->SetArrayField(TEXT("events"),EventRows); Result->SetArrayField(TEXT("stencil_owners"),Owners);
    const bool Saved = FFileHelper::SaveStringToFile(Lines,*(Output/TEXT("labels.jsonl")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
        AnomalyAnnotation425::WriteSessionAnnotation(Output,Annotation) &&
        FFileHelper::SaveStringToFile(Json425(Result),*(Output/TEXT("labels-receipt.json")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    if (!Saved) { Failure = TEXT("Label write failed"); } Pending.Empty();
}

bool AnomalyCaptureC2::Street() { return CaptureStreet425; }
bool AnomalyCaptureC2::Enabled() { return CaptureLive425; }
bool AnomalyCaptureC2::Boundary() { return false; }
bool AnomalyCaptureC2::Masks() { return CaptureLive425; }
void AnomalyCaptureC2::Finished(UWorld* World,bool Success) { CaptureLive425 = false; FPlatformMisc::RequestExit(false); }
static FAutoConsoleCommandWithWorldAndArgs CaptureLiveCommand425(TEXT("IAI.Capture.C4"),TEXT("Capture live seeded auto-injection: output, frame count, seed"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* World)
    {
        if (CaptureLive425 || Args.Num()!=3 || !World || !World->IsGameWorld() || !FParse::Param(FCommandLine::Get(),TEXT("IAIBench"))) { return; }
        APlayerController* PC = World->GetFirstPlayerController(); if (!PC) { return; }
        if (CaptureBenchCamera425 && FParse::Param(FCommandLine::Get(),TEXT("IAIBench"))) { for (TActorIterator<ACameraActor> It(World); It; ++It) { PC->SetViewTarget(*It); break; } }
        PC->bShowMouseCursor = false;
        CaptureLive425 = true; CaptureCount425 = FCString::Atoi(*Args[1]); CaptureSeed425 = FCString::Atoi(*Args[2]);
        const FString Root = Args[0]; const double Ready = FPlatformTime::Seconds()+1;
        const TWeakObjectPtr<UWorld> Weak(World);
        FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Weak,Root,Ready](float)
        {
            if (!Weak.IsValid()) { CaptureLive425 = false; FPlatformMisc::RequestExit(false); return false; }
            if (FPlatformTime::Seconds()<Ready) { return true; }
            if (UAnomalyCaptureSubsystem* Capture = Weak->GetSubsystem<UAnomalyCaptureSubsystem>()) { Capture->StartRun(Root/TEXT("epoch01"),true,0,CaptureCount425); }
            return false;
        }));
    }));
static FAutoConsoleCommandWithWorldAndArgs CaptureStreetCommand425(TEXT("IAI.Capture.C5"),TEXT("Bench-only moving player-view capture: output, seed"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* World)
    {
        if (CaptureLive425 || Args.Num()!=2 || !World || !World->IsGameWorld() || !FParse::Param(FCommandLine::Get(),TEXT("IAIBench"))) { return; }
        CaptureLive425=true; CaptureStreet425=true; CaptureCount425=FAnomalyStreetBench425::TotalFrames; CaptureSeed425=FCString::Atoi(*Args[1]);
        const FString Root=Args[0]; const double Start=FPlatformTime::Seconds(); const TWeakObjectPtr<UWorld> Weak(World);
        TSharedRef<double> Quiet=MakeShared<double>(0);
        FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Weak,Root,Start,Quiet](float)
        {
            if (!Weak.IsValid()) { CaptureLive425=false; FPlatformMisc::RequestExit(false); return false; }
            const double Now=FPlatformTime::Seconds();
            const int32 Shaders=GShaderCompilingManager ? GShaderCompilingManager->GetNumRemainingJobs() : 0;
            const int32 Streaming=IStreamingManager::Get().GetNumWantingResources();
            if (Shaders || Streaming || Now-Start<10) { *Quiet=0; } else if (*Quiet==0) { *Quiet=Now; }
            if (Now-Start>120) { UE_LOG(LogAnomalyCapture,Error,TEXT("C5 warmup timeout shaders=%d streaming=%d"),Shaders,Streaming); FPlatformMisc::RequestExit(false); return false; }
            if (*Quiet==0 || Now-*Quiet<2) { return true; }
            UE_LOG(LogAnomalyCapture,Log,TEXT("C5 warmup settled shaders=%d streaming=%d seconds=%.3f"),Shaders,Streaming,Now-Start);
            if (auto* Capture=Weak->GetSubsystem<UAnomalyCaptureSubsystem>()) { Capture->StartRun(Root/TEXT("epoch01"),true,0,CaptureCount425); }
            return false;
        }));
    }));
#endif
