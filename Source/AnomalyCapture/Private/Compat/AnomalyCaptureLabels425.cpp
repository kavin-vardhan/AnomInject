#include "AnomalyCaptureLabels425.h"
#include "AnomalyBenchGate.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyTransitionCounts425.h"
#include "AnomalyCaptureSubsystem.h"
#include "AnomalyCaptureLog.h"
#include "AnomalyCensusBatch425.h"
#include "AnomalySessionRun425.h"
#include "AnomalyHostDepth425.h"
#include "AnomalyPrivatePass425.h"
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
#include "HAL/FileManager.h"
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
    bool CaptureGeometry425 = false;
    bool CaptureExit425 = true;
    bool CaptureFreePlay425 = false;
    bool CaptureStarting425 = false;
    TWeakObjectPtr<UWorld> CaptureOwner425;
    FDelegateHandle CaptureWarmup425;
    uint64 CaptureGeneration425 = 0;
    int32 CaptureBenchCamera425 = 0;
    FAutoConsoleVariableRef CaptureBenchCameraCommand425(TEXT("IAI.Capture.BenchCamera"),CaptureBenchCamera425,TEXT("Bench-only first-camera override; default off"));
    int32 CaptureSeed425 = 4242;
    int32 CaptureCount425 = 1440;
    TArray<FString> CaptureRoutes425 = { TEXT("blinking"),TEXT("lod_corruption"),TEXT("missing_texture"),TEXT("corrupted_texture") };
    FAutoConsoleVariableRef CapturePaceCommand425(TEXT("IAI.Capture.Pace"),CapturePace425,TEXT("4.25 capture pacing"));
    FString Json425(const TSharedPtr<FJsonObject>& Object)
    {
        FString Text;
        auto Writer = TJsonWriterFactory<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>::Create(&Text);
        FJsonSerializer::Serialize(Object.ToSharedRef(),Writer); return Text;
    }
    FString FireKey425(const FAutoLiveFireInfo& Fire) { return FString::Printf(TEXT("%s@%llu"),*Fire.Id.ToString(),Fire.StartFrame); }
    bool Routes425(UWorld* World,const TArray<FString>& Args,int32 Index)
    {
        auto* Auto=World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr;
        if (!Auto) { return false; }
        const auto Pool=Auto->GetPoolIds(); std::vector<std::string> Routes;
        if (!AnomalyCaptureAdmission425::Routes(Args.Num()>Index ? TCHAR_TO_UTF8(*Args[Index]) : "",Args.Num()>Index,
            [&Pool](const std::string& Id) { return Pool.Contains(UTF8_TO_TCHAR(Id.c_str())); },Routes))
        {
            UE_LOG(LogAnomalyCapture,Error,TEXT("Capture425 refused: invalid route list"));
            if (FParse::Param(FCommandLine::Get(),TEXT("IAICaptureExit"))) { FPlatformMisc::RequestExit(false); }
            return false;
        }
        CaptureRoutes425.Empty(); for (const auto& Id:Routes) { CaptureRoutes425.Add(UTF8_TO_TCHAR(Id.c_str())); }
        UE_LOG(LogAnomalyCapture,Log,TEXT("Capture425 routes=%s"),*FString::Join(CaptureRoutes425,TEXT(",")));
        return true;
    }
    TArray<TSharedPtr<FJsonValue>> RouteRows425()
    {
        TArray<TSharedPtr<FJsonValue>> Rows;
        for (const auto& Id:CaptureRoutes425) { Rows.Add(MakeShared<FJsonValueString>(Id)); }
        return Rows;
    }
}

bool FAnomalyCaptureLabels425::Start(UWorld* World,const FString& Directory,bool Boundary)
{
    if (!World || !World->IsGameWorld() || !World->GetGameViewport() || !World->GetGameViewport()->Viewport) { return false; }
    UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
    if (!Auto || Auto->IsRunning() || Auto->GetLiveFireCount()) { return false; }
    auto Routes=MakeShared<FJsonObject>(); Routes->SetArrayField(TEXT("routes"),RouteRows425());
    if (!FFileHelper::SaveStringToFile(Json425(Routes),*(Directory/TEXT("routes.json")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)) { return false; }
    if (!AnomalyHostDepth425::Admit(World,Directory)) { return false; }
    AnomalyPrivate425::Start(World,Directory);
    Owner = World; Output = Directory; RunEpoch = ++CaptureEpoch425; Size = World->GetGameViewport()->Viewport->GetSizeXY();
    Started = FPlatformTime::Seconds(); SavedFixed = FApp::UseFixedTimeStep(); SavedDelta = FApp::GetFixedDeltaTime(); SavedPace = CapturePace425;
    SavedEnabled = Auto->IsEnabled(); SavedRunning = Auto->IsRunning(); SavedSeed = Auto->GetSeed(); SavedMax = Auto->GetMaxConcurrent(); SavedPersist = Auto->GetPersist(); SavedIds = Auto->GetEnabledIds();
    Auto->GetIntervalRange(SavedIntervalMin,SavedIntervalMax); Auto->GetHoldRange(SavedHoldMin,SavedHoldMax);
    Initialized = true;
    if (CaptureStreet425)
    {
        Street=MakeUnique<FAnomalyStreetBench425>();
        if (!Street->Start(World,Directory,CaptureCount425)) { Street->Stop(); Street.Reset(); return false; }
    }
    MaskOwner = MakeUnique<FAnomalyMasks425>();
    if (!MaskOwner->Start(World)) { MaskOwner->Restore(); MaskOwner.Reset(); return false; }
    Auto->SetAllAnomaliesEnabled(false);
    for (const FString& Id : CaptureRoutes425) { if (!Auto->SetAnomalyEnabled(FName(*Id),true)) { Failure = TEXT("Auto pool admission failed"); } }
    Auto->SetSeed(CaptureSeed425); Auto->SetIntervalRange(0.8f,0.8f); Auto->SetHoldRange(0.4f,0.4f); Auto->SetMaxConcurrent(1); Auto->SetPersist(false); Auto->SetEnabled(true);
    if (!SessionGlobals.Start(World,CaptureRoutes425,Directory,!CaptureFreePlay425)) { Failure=TEXT("Session global apply failed"); }
    FApp::SetFixedDeltaTime(1.0/30.0); FApp::SetUseFixedTimeStep(true); CapturePace425 = 1;
    PrepareHandle = FWorldDelegates::OnWorldPreActorTick.AddRaw(this,&FAnomalyCaptureLabels425::Prepare);
    return !Failed();
}

void FAnomalyCaptureLabels425::Prepare(UWorld* World,ELevelTick TickType,float Delta)
{
    if (World != Owner.Get() || TickType != LEVELTICK_All || Prepared == Next || Failed() || FPlatformTime::Seconds()-Started < 3) { return; }
    if (Street && !Street->Ready(Next)) { return; }
    Prepared = Next;
    if (CaptureGeometry425) { return; }
    if (Street) { Street->Prepare(Next); }
    SessionGlobals.Prepare(Next);
    UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
    if (!Auto) { Failure = TEXT("Auto injector missing"); return; }
    if (Street)
    {
        const bool Wanted=AnomalySessionRun425::Wanted(AutoStarted,Street->AutoOn(Next),Next>=FAnomalyStreetBench425::TotalFrames-24);
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
        if ((!CaptureFreePlay425 || CaptureCount425>0) && Next>=CaptureCount425-24 && Auto->IsRunning()) { Auto->SetRunning(false); }
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
    if (CaptureFreePlay425)
    {
        const APlayerController* PC=World->GetFirstPlayerController();
        Row->SetBoolField(TEXT("move_input_ignored"),!PC || PC->IsMoveInputIgnored());
        Row->SetBoolField(TEXT("look_input_ignored"),!PC || PC->IsLookInputIgnored());
    }
    Row->SetNumberField(TEXT("pace"),CapturePace425); Row->SetNumberField(TEXT("tick_order"),0);
    Row->SetBoolField(TEXT("fixed_step"),FApp::UseFixedTimeStep());
    Row->SetObjectField(TEXT("census"),MaskOwner->State());
    if (Street) { Row->SetObjectField(TEXT("sweep"),Street->Describe(int32(Request-1))); Row->SetObjectField(TEXT("custom_depth"),Street->DepthSample()); }
    TSet<FString> Live;
    TArray<TSharedPtr<FJsonValue>> States,Anomalies;
    auto Fires=Auto->GetLiveFires(); SessionGlobals.Append(Row,Fires);
    for (const auto& Fire : Fires)
    {
        const FString Key = FireKey425(Fire); Live.Add(Key);
        const TArray<uint8> Tags = MaskOwner->TagsFor(Fire);

        const bool Labelled = FAnomalyMasks425::Labelled(World,Fire);
        auto State = MakeShared<FJsonObject>();
        State->SetStringField(TEXT("id"),Fire.Id.ToString()); State->SetStringField(TEXT("event_id"),Key);
        State->SetStringField(TEXT("target_name"),Fire.Target); State->SetNumberField(TEXT("start_frame"),double(Fire.StartFrame));
        State->SetNumberField(TEXT("seconds_remaining"),Fire.SecondsRemaining);
        State->SetBoolField(TEXT("whole_frame"),Fire.bWholeFrameExtent);
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
        bool Unconfirmed=false;
        if (Fire.Id==FName(TEXT("camera_clipping")) && Row->TryGetBoolField(TEXT("camera_clipping.unconfirmed"),Unconfirmed) && Unconfirmed)
        { auto& T=Transitions.FindOrAdd(Key+TEXT("|")+Fire.Target); T.Frames.Add(int32(Request-1)); ++T.Reasons.FindOrAdd(TEXT("camera_clipping_unconfirmed")); }
        States.Add(MakeShared<FJsonValueObject>(State));
        if (Labelled) { Anomalies.Add(MakeShared<FJsonValueObject>(State)); }
        if (!Events.Contains(Key))
        {
            auto Event = MakeShared<FJsonObject>(*State); Event->SetNumberField(TEXT("first_index"),double(Request-1));
            Events.Add(Key,Event);
            auto& E=AnnotationEvents.FindOrAdd(Key); E.EventId=Key+TEXT("|")+Fire.Target;
            FSelectionProvenance Selection;
            AnomalyViewport::EvaluateSelectionProvenance(World,Actor,Selection);
            E.CoveragePct=Selection.CoveragePct;
            auto Provenance=MakeShared<FJsonObject>();
            Provenance->SetNumberField(TEXT("coverage_pct"),Selection.CoveragePct);
            Provenance->SetBoolField(TEXT("valid"),Selection.bValid);
            Provenance->SetNumberField(TEXT("occlusion_samples_passed"),Selection.OcclusionSamplesPassed);
            Provenance->SetNumberField(TEXT("occlusion_samples_total"),Selection.OcclusionSamplesTotal);
            Provenance->SetNumberField(TEXT("poll_distance"),Selection.PollDistance);
            Event->SetObjectField(TEXT("selection_provenance"),Provenance);
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
        const bool Whole=State->GetBoolField(TEXT("whole_frame"));
        const bool Injected=State->GetBoolField(TEXT("injected")),Visible=Injected && (Whole || Pixels>0);
        State->SetBoolField(TEXT("labelled"),Visible); State->SetBoolField(TEXT("affected"),Visible); State->SetNumberField(TEXT("mask_pixels"),Pixels); State->SetNumberField(TEXT("drawn_pixels"),Drawn);
        if (Drawn) { AnnotationEvents[State->GetStringField(TEXT("event_id"))].BboxSource=TEXT("drawn"); }
        auto& E=AnnotationEvents[State->GetStringField(TEXT("event_id"))];
        if (Injected) { E.InjectedFrameIndices.Add(int32(Frame.RequestId-1)); }
        if (Visible) { CoverageSum.FindOrAdd(State->GetStringField(TEXT("event_id")))+=State->GetNumberField(TEXT("projected_screen_share")); ++CoverageCount.FindOrAdd(State->GetStringField(TEXT("event_id"))); E.FrameIndices.Add(int32(Frame.RequestId-1)); ++E.ObservableFrameCount; E.bMaskProvided=!Whole; Affected.Add(Value); }
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
        if (E.CamPath.IsEmpty()) { E.CamPath=Key.PlayerTarget; E.CamPosition=Key.PlayerLocation; E.CamRotation=Key.PlayerRotation; E.CamFovDeg=Key.PlayerFov; E.CamAspect=float(Size.X)/Size.Y; E.CamNear=Key.ViewNear; E.CamFar=AnomalyCensusBatch425::AnnotationFar(Key.ViewFar); }
    }
    Frames.Add(Key.SessionIndex, Row); Pending.Remove(Key.Request);
    return true;
}


void FAnomalyCaptureLabels425::Abort()
{
    Failure=TEXT("Label initialization failed"); Stop(0,0,false);
}
void FAnomalyCaptureLabels425::Stop(int32 Written,int32 Dropped,bool Complete,bool StoppedByUser)
{
    if (!Initialized) { return; }
    Initialized=false;
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
    if (!SessionGlobals.Stop()) { Failure=TEXT("Session global restoration failed"); }
    auto Summary=SessionGlobals.Summary();
    Summary->SetNumberField(TEXT("label_transition_camera_clipping_unconfirmed_entries"),AnomalyTransitionCounts425::CameraUnconfirmedEntries(Transitions));
    FFileHelper::SaveStringToFile(Json425(Summary),*(Output/TEXT("run_summary.json")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    TArray<TSharedPtr<FJsonValue>> Owners;
    if (MaskOwner)
    {
        for (const auto& Pair : MaskOwner->Assignments())
        {
            auto Node = MakeShared<FJsonObject>(); Node->SetStringField(TEXT("actor"),GetNameSafe(Pair.Key.Get())); Node->SetNumberField(TEXT("tag"),Pair.Value);
            Owners.Add(MakeShared<FJsonValueObject>(Node));
        }
    }
    const FString PrivateState=Json425(AnomalyPrivate425::State(true));
    FFileHelper::SaveStringToFile(PrivateState,*(Output/TEXT("private-availability.json")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
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
    Annotation.Transitions=Transitions;
    Annotation.Video.TotalFrames=Written; Annotation.Video.Fps=30; Annotation.Video.TargetFps=30;
    for (const auto& Event:SortedEvents)
    {
        auto E=AnnotationEvents[Event->GetStringField(TEXT("event_id"))]; E.FrameIndices.Sort(); E.InjectedFrameIndices.Sort();
        E.bManifested=E.InjectedFrameIndices.Num()>0; E.CoverageRatio=CoverageSum.FindRef(Event->GetStringField(TEXT("event_id")))/FMath::Max(1,CoverageCount.FindRef(Event->GetStringField(TEXT("event_id"))));
        Annotation.Events.Add(E);
    }
    auto Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("result"),Failed() || Dropped || (Complete && (Pending.Num() || Written != Frames.Num())) ? TEXT("FAIL") : Complete ? TEXT("PASS") : TEXT("PARTIAL"));
    if (CaptureFreePlay425) { Result->SetBoolField(TEXT("stopped_by_user"),StoppedByUser); }
    Result->SetStringField(TEXT("failure"),Failure); Result->SetNumberField(TEXT("paired"),Frames.Num());
    Result->SetNumberField(TEXT("unmatched_dropped"),Pending.Num()); Result->SetNumberField(TEXT("dropped"),Dropped);
    Result->SetBoolField(TEXT("mask_state_restored"),Restored);
    Result->SetBoolField(TEXT("pacing_restored"),FApp::UseFixedTimeStep()==SavedFixed && FApp::GetFixedDeltaTime()==SavedDelta);
    Result->SetArrayField(TEXT("commands"),Commands); Result->SetNumberField(TEXT("seed"),CaptureSeed425);
    Result->SetArrayField(TEXT("routes"),RouteRows425());
    Result->SetArrayField(TEXT("events"),EventRows); Result->SetArrayField(TEXT("stencil_owners"),Owners);
    const bool Saved = FFileHelper::SaveStringToFile(Lines,*(Output/TEXT("labels.jsonl")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
        AnomalyAnnotation425::WriteSessionAnnotation(Output,Annotation) &&
        FFileHelper::SaveStringToFile(Json425(Result),*(Output/TEXT("labels-receipt.json")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    if (!Saved) { Failure = TEXT("Label write failed"); } Pending.Empty();
}

bool AnomalyCaptureC2::Street() { return CaptureStreet425; }
bool AnomalyCaptureC2::FreePlay() { return CaptureFreePlay425; }
bool AnomalyCaptureC2::Enabled() { return CaptureLive425; }
bool AnomalyCaptureC2::Boundary() { return false; }
bool AnomalyCaptureC2::Masks() { return CaptureLive425; }
bool AnomalyCaptureC2::Pending(UWorld* World) { return CaptureOwner425.Get()==World && CaptureWarmup425.IsValid(); }
bool AnomalyCaptureC2::Starting(UWorld* World) { return CaptureOwner425.Get()==World && CaptureStarting425; }
void AnomalyCaptureC2::Cancel(UWorld* World)
{
    if (CaptureOwner425.Get()!=World) { return; }
    ++CaptureGeneration425;
    if (CaptureWarmup425.IsValid())
    {
        FTicker::GetCoreTicker().RemoveTicker(CaptureWarmup425); CaptureWarmup425.Reset();
        UE_LOG(LogAnomalyCapture,Log,TEXT("C5 warmup cancelled; pending start removed"));
        Finished(World,false);
    }
}
void AnomalyCaptureC2::Finished(UWorld* World,bool Success)
{
    CaptureLive425 = false;
    if (CaptureGeometry425) { CaptureGeometry425=false; return; }
    if (CaptureWarmup425.IsValid()) { FTicker::GetCoreTicker().RemoveTicker(CaptureWarmup425); CaptureWarmup425.Reset(); }
    const bool Exit=CaptureExit425;
    CaptureOwner425.Reset(); CaptureStreet425=false; CaptureFreePlay425=false; CaptureStarting425=false; ++CaptureGeneration425;
    UE_LOG(LogAnomalyCapture,Log,TEXT("C5 transaction finished success=%d exit=%d"),Success?1:0,Exit?1:0);
    CaptureExit425=true;
    if (Exit) { FPlatformMisc::RequestExit(false); }
}
bool AnomalyCaptureC2::StartFreePlay(UWorld* World,const FString& Root,int32 Seed,int32 Count,const TArray<FString>& Routes,FString& Error)
{
    if (CaptureLive425 || CaptureWarmup425.IsValid()) { Error=TEXT("A capture owns the process"); return false; }
    if (!World || !World->IsGameWorld() || !World->GetFirstPlayerController() || Count<0) { Error=TEXT("Free-play requires a player game world and nonnegative frame count"); return false; }
    auto* Auto=World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
    if (!Auto || Auto->IsRunning() || Auto->GetLiveFireCount()) { Error=TEXT("Auto injection must be idle before capture"); return false; }
    const auto Pool=Auto->GetPoolIds(); std::vector<std::string> Validated;
    if (!AnomalyCaptureAdmission425::Routes(TCHAR_TO_UTF8(*FString::Join(Routes,TEXT(","))),true,
        [&Pool](const std::string& Id) { return Pool.Contains(UTF8_TO_TCHAR(Id.c_str())); },Validated)) { Error=TEXT("Invalid capture routes"); return false; }
    if (Root.IsEmpty() || FPaths::IsRelative(Root) || IFileManager::Get().DirectoryExists(*Root) ||
        !IFileManager::Get().MakeDirectory(*Root,true)) { Error=TEXT("Capture requires a fresh absolute run directory"); return false; }
    CaptureExit425=false;
    CaptureRoutes425=Routes;
    const TArray<FString> Order={TEXT("blinking"),TEXT("lod_corruption"),TEXT("missing_texture"),TEXT("corrupted_texture")};
    CaptureRoutes425.Sort([&Order](const FString& A,const FString& B)
    {
        const int32 AI=Order.Find(A),BI=Order.Find(B);
        const int32 AR=AI==INDEX_NONE ? Order.Num() : AI,BR=BI==INDEX_NONE ? Order.Num() : BI;
        return AR==BR ? A<B : AR<BR;
    });
    CaptureSeed425=Seed; CaptureCount425=Count; CaptureOwner425=World;
    CaptureLive425=true; CaptureStreet425=false; CaptureGeometry425=false; CaptureFreePlay425=true;
    return true;
}
bool AnomalyCaptureC2::StartC5(UWorld* World,const FString& Root,int32 Seed,const TArray<FString>& Routes,bool ExitOnFinish,FString& Error)
{
    if (CaptureLive425 || CaptureWarmup425.IsValid()) { Error=TEXT("A capture owns the process"); return false; }
    if (!World || !World->IsGameWorld() || !AnomalyBenchGate::IsCapture425Enabled()) { Error=TEXT("C5 requires a game world and IAIBench"); return false; }
    auto* Auto=World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
    if (!Auto || Auto->IsRunning() || Auto->GetLiveFireCount()) { Error=TEXT("Auto injection must be idle before C5"); return false; }
    const auto Pool=Auto->GetPoolIds(); std::vector<std::string> Validated;
    if (!AnomalyCaptureAdmission425::Routes(TCHAR_TO_UTF8(*FString::Join(Routes,TEXT(","))),true,
        [&Pool](const std::string& Id) { return Pool.Contains(UTF8_TO_TCHAR(Id.c_str())); },Validated)) { Error=TEXT("Invalid C5 routes"); return false; }
    if (Root.IsEmpty() || FPaths::IsRelative(Root) || IFileManager::Get().DirectoryExists(*(Root/TEXT("epoch01"))) ||
        !IFileManager::Get().MakeDirectory(*Root,true)) { Error=TEXT("C5 requires a fresh absolute run directory"); return false; }
    if (!AnomalyHostDepth425::Admit(World,Root)) { Error=TEXT("C5 host-depth/private-pass admission refused"); return false; }
    CaptureRoutes425=Routes;
    const TArray<FString> RouteOrder={TEXT("blinking"),TEXT("lod_corruption"),TEXT("missing_texture"),TEXT("corrupted_texture")};
    CaptureRoutes425.Sort([&RouteOrder](const FString& A,const FString& B)
    {
        const int32 AI=RouteOrder.Find(A),BI=RouteOrder.Find(B);
        const int32 AR=AI==INDEX_NONE ? RouteOrder.Num() : AI,BR=BI==INDEX_NONE ? RouteOrder.Num() : BI;
        return AR==BR ? A<B : AR<BR;
    });
    CaptureSeed425=Seed;
    CaptureLive425=true; CaptureStreet425=true; CaptureGeometry425=false; CaptureExit425=ExitOnFinish;
    CaptureOwner425=World; CaptureCount425=FAnomalyCameraSession425::FrameCount(World,Routes);
    const uint64 Generation=++CaptureGeneration425;
    const double Start=FPlatformTime::Seconds(); const TWeakObjectPtr<UWorld> Weak(World);
    TSharedRef<double> Quiet=MakeShared<double>(0);
    CaptureWarmup425=FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Weak,Root,Start,Quiet,Generation](float)
    {
        if (Generation!=CaptureGeneration425 || CaptureOwner425!=Weak) { return false; }
        if (!Weak.IsValid()) { CaptureWarmup425.Reset(); Finished(nullptr,false); return false; }
        const double Now=FPlatformTime::Seconds();
        const int32 Shaders=GShaderCompilingManager ? GShaderCompilingManager->GetNumRemainingJobs() : 0;
        const int32 Streaming=IStreamingManager::Get().GetNumWantingResources();
        if (Shaders || Streaming || Now-Start<10) { *Quiet=0; } else if (*Quiet==0) { *Quiet=Now; }
        if (Now-Start>120)
        {
            CaptureWarmup425.Reset();
            UE_LOG(LogAnomalyCapture,Error,TEXT("C5 warmup timeout shaders=%d streaming=%d"),Shaders,Streaming);
            Finished(Weak.Get(),false); return false;
        }
        if (*Quiet==0 || Now-*Quiet<2) { return true; }
        CaptureWarmup425.Reset(); CaptureStarting425=true;
        UE_LOG(LogAnomalyCapture,Log,TEXT("C5 warmup settled shaders=%d streaming=%d seconds=%.3f"),Shaders,Streaming,Now-Start);
        if (auto* Capture=Weak->GetSubsystem<UAnomalyCaptureSubsystem>()) { Capture->StartRun(Root/TEXT("epoch01"),true,CaptureSeed425,CaptureCount425); }
        else { Finished(Weak.Get(),false); }
        CaptureStarting425=false; return false;
    }));
    return true;
}

#if !UE_BUILD_SHIPPING
static FAutoConsoleCommandWithWorldAndArgs CaptureGeometryCommand425(TEXT("IAI.Capture.BenchGeometry"),TEXT("EA_Core bench-only no-fire private coverage probe: output"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* World)
    {
        if (CaptureLive425 || Args.Num()!=1 || !World || !World->IsGameWorld() || World->GetMapName()!=TEXT("EA_Core") ||
            !AnomalyBenchGate::IsCapture425Enabled()) { return; }
        auto* Auto=World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
        auto* Capture=World->GetSubsystem<UAnomalyCaptureSubsystem>();
        if (!Auto || !Capture || Auto->IsRunning() || Auto->GetLiveFireCount()) { return; }
        CaptureLive425=true; CaptureStreet425=false; CaptureGeometry425=true; CaptureCount425=32;
        UE_LOG(LogAnomalyCapture,Log,TEXT("Capture425 geometry probe frames=32 auto_fires=disabled"));
        Capture->StartRun(Args[0],true,0,CaptureCount425);
    }));
#endif
static FAutoConsoleCommandWithWorldAndArgs CaptureLiveCommand425(TEXT("IAI.Capture.C4"),TEXT("Capture live seeded auto-injection: output frame_count seed [route,route]; default blinking,lod_corruption,missing_texture,corrupted_texture"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* World)
    {
        if (CaptureLive425 || (Args.Num()!=3 && Args.Num()!=4) || !World || !World->IsGameWorld() || !AnomalyBenchGate::IsCapture425Enabled()) { return; }
        if (!Routes425(World,Args,3)) { return; }
        APlayerController* PC = World->GetFirstPlayerController(); if (!PC) { return; }
        if (CaptureBenchCamera425 && AnomalyBenchGate::IsCapture425Enabled()) { for (TActorIterator<ACameraActor> It(World); It; ++It) { PC->SetViewTarget(*It); break; } }
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
static FAutoConsoleCommandWithWorldAndArgs CaptureStreetCommand425(TEXT("IAI.Capture.C5"),TEXT("Bench-only moving player-view capture: output seed [route,route]; default blinking,lod_corruption,missing_texture,corrupted_texture"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* World)
    {
        if ((Args.Num()!=2 && Args.Num()!=3) || !World) { return; }
        TArray<FString> Routes;
        (Args.Num()==3 ? Args[2] : FString(TEXT("blinking,lod_corruption,missing_texture,corrupted_texture"))).ParseIntoArray(Routes,TEXT(","),false);
        FString Error; auto* Capture=World->GetSubsystem<UAnomalyCaptureSubsystem>();
        if (!Capture || !Capture->StartC5Run(Args[0],FCString::Atoi(*Args[1]),Routes,true,Error))
        {
            UE_LOG(LogAnomalyCapture,Error,TEXT("C5 refused: %s"),*Error);
            if (FParse::Param(FCommandLine::Get(),TEXT("IAICaptureExit"))) { FPlatformMisc::RequestExit(false); }
        }
    }));
#endif
