#include "AnomalyCaptureLabels425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyActiveSource.h"
#include "AnomalyCaptureSubsystem.h"
#include "AnomalyCaptureLog.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyHiddenClass.h"
#include "AnomalyInstallState.h"
#include "Camera/CameraActor.h"
#include "Containers/Ticker.h"
#include "CoreGlobals.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
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
    int32 CapturePhase425 = 0;
    FString CaptureRoot425;
    FAutoConsoleVariableRef CapturePaceCommand425(TEXT("IAI.Capture.Pace"), CapturePace425, TEXT("4.25 capture pacing, scoped to the running capture session"));
    const TCHAR* CaptureRoutes425[] = { TEXT("blinking"), TEXT("lighting_mismatch"), TEXT("lod_corruption"), TEXT("missing_object") };
    const TCHAR* CaptureTargets425[] = { TEXT("EA_BlinkCube"), TEXT("EA_Light"), TEXT("EA_LOD"), TEXT("EA_HideCube") };
    FString Json425(const TSharedPtr<FJsonObject>& Object)
    {
        FString Text;
        auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Text);
        FJsonSerializer::Serialize(Object.ToSharedRef(), Writer);
        return Text;
    }
    TSharedPtr<FJsonObject> IndexSet425(const TArray<int32>& Indices)
    {
        auto Object = MakeShared<FJsonObject>();
        TArray<TSharedPtr<FJsonValue>> Values;
        for (int32 Index : Indices) { Values.Add(MakeShared<FJsonValueNumber>(Index)); }
        Object->SetArrayField(TEXT("frame_indices"), Values);
        Object->SetNumberField(TEXT("frame_count"), Indices.Num());
        Object->SetNumberField(TEXT("start_frame"), Indices.Num() ? Indices[0] : 0);
        Object->SetNumberField(TEXT("end_frame"), Indices.Num() ? Indices.Last() : 0);
        Object->SetNumberField(TEXT("span_frame_count"), Indices.Num() ? Indices.Last() - Indices[0] + 1 : 0);
        return Object;
    }
    AActor* Target425(UWorld* World, const TCHAR* Tag)
    {
        for (TActorIterator<AActor> It(World); It; ++It) { if (It->ActorHasTag(FName(Tag))) { return *It; } }
        return nullptr;
    }
    bool Camera425(UWorld* World)
    {
        APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
        AActor* Camera = World ? Target425(World, TEXT("EA_Camera")) : nullptr;
        if (!PC || !Camera) { return false; }
        PC->SetViewTarget(Camera); PC->bShowMouseCursor = false;
        return true;
    }
}

bool FAnomalyCaptureLabels425::Start(UWorld* World, const FString& Directory, bool Boundary)
{
    if (!World || !World->IsGameWorld() || !World->GetSubsystem<UAnomalyInjectorSubsystem>()) { return false; }
    if (AnomalyCaptureC2::Masks())
    {
        MaskOwner = MakeUnique<FAnomalyMasks425>();
        if (!MaskOwner->Start(World)) { MaskOwner->Restore(); MaskOwner.Reset(); return false; }
    }
    Owner = World; Output = Directory; BoundaryOnly = Boundary; RunEpoch = ++CaptureEpoch425;
    Started = FPlatformTime::Seconds(); SavedFixed = FApp::UseFixedTimeStep(); SavedDelta = FApp::GetFixedDeltaTime();
    SavedPace = CapturePace425;
    PrepareHandle = FWorldDelegates::OnWorldPreActorTick.AddRaw(this, &FAnomalyCaptureLabels425::Prepare);
    return true;
}

void FAnomalyCaptureLabels425::Prepare(UWorld* World, ELevelTick TickType, float Delta)
{
    if (World != Owner.Get() || TickType != LEVELTICK_All || Prepared == Next || Failed() || FPlatformTime::Seconds() - Started < 3) { return; }
    Prepared = Next;
    UAnomalyInjectorSubsystem* Injector = World->GetSubsystem<UAnomalyInjectorSubsystem>();
    if (!Injector) { Failure = TEXT("Injector missing"); return; }
    const int32 Event = Next / 20;
    const int32 Step = Next % 20;
    const int32 Order = Next >= 240 ? 1 : 0;
    const int32 Route = (Event / 3) % 4;
    auto Record = [&](const TCHAR* Command, bool Success)
    {
        auto Row = MakeShared<FJsonObject>();
        Row->SetStringField(TEXT("command"), Command); Row->SetBoolField(TEXT("success"), Success);
        Row->SetNumberField(TEXT("next_index"), Next); Row->SetNumberField(TEXT("event_id"), Event);
        Row->SetStringField(TEXT("anomaly"), CaptureRoutes425[Route]); Row->SetNumberField(TEXT("tick_order"), Order);
        Row->SetNumberField(TEXT("process_frame"), double(GFrameCounter));
        Commands.Add(MakeShared<FJsonValueObject>(Row));
        if (!Success) { Failure = FString(TEXT("Driver command failed: ")) + Command; }
    };
    if (BoundaryOnly || Next >= 480) { return; }
    if (Next == 0 || Next == 240)
    {
        Injector->SetSynthTickOrder(Order != 0);
        CapturePace425 = Order == 0 ? 1 : 0;
        if (GEngine) { GEngine->Exec(World, Order == 0 ? TEXT("IAI.Capture.Pace 1") : TEXT("IAI.Capture.Pace 0")); }
        FApp::SetFixedDeltaTime(1.0 / 30.0);
        FApp::SetUseFixedTimeStep(CapturePace425 != 0);
        Record(TEXT("tick_order_and_pace"), Injector->IsSynthTickOrderEnabled() == (Order != 0));
        UE_LOG(LogAnomalyCapture, Log, TEXT("Capture425 C2: tick_order=%d pace=%d fixed_step=%d tickpin_compiled=0"), Order, CapturePace425, FApp::UseFixedTimeStep() ? 1 : 0);
    }
    if (Step == 3)
    {
        TArray<FString> Args = { CaptureTargets425[Route] };
        if (Route == 0) { Args.Add(TEXT("4")); }
        if (Route == 1) { Args.Add(TEXT("dim")); Args.Add(TEXT("0.05")); }
        if (Route == 2) { Args.Add(TEXT("1")); }
        Record(TEXT("apply"), Injector->ApplyAnomaly(FName(CaptureRoutes425[Route]), Args));
    }
    if (Step == 17) { Record(TEXT("revert"), Injector->RevertAnomaly(FName(CaptureRoutes425[Route]))); }
}

bool FAnomalyCaptureLabels425::Snapshot(UWorld* World, uint64 Request)
{
    if (World != Owner.Get() || Pending.Contains(Request) || Pending.Num() >= 8 || Failed()) { return false; }
    const int32 Index = int32(Request - 1);
    auto Row = MakeShared<FJsonObject>();
    Row->SetNumberField(TEXT("frame_index"), Index); Row->SetNumberField(TEXT("session_index"), Index);
    Row->SetNumberField(TEXT("epoch"), double(RunEpoch)); Row->SetNumberField(TEXT("request"), double(Request));
    Row->SetNumberField(TEXT("world_owner"), World->GetUniqueID()); Row->SetStringField(TEXT("world"), World->GetPathName());
    Row->SetNumberField(TEXT("sample_frame"), double(GFrameCounter)); Row->SetNumberField(TEXT("t"), World->GetTimeSeconds());
    Row->SetNumberField(TEXT("t_wall"), FPlatformTime::Seconds() - Started);
    Row->SetNumberField(TEXT("width"), 1280); Row->SetNumberField(TEXT("height"), 720);
    Row->SetNumberField(TEXT("event_id"), BoundaryOnly ? -1 : Index / 20);
    Row->SetStringField(TEXT("event_anomaly"), BoundaryOnly ? TEXT("none") : CaptureRoutes425[(Index / 60) % 4]);
    UAnomalyInjectorSubsystem* Injector = World->GetSubsystem<UAnomalyInjectorSubsystem>();
    Row->SetNumberField(TEXT("tick_order"), Injector->IsSynthTickOrderEnabled() ? 1 : 0);
    Row->SetNumberField(TEXT("pace"), CapturePace425);
    Row->SetBoolField(TEXT("fixed_step"), FApp::UseFixedTimeStep());
    Row->SetNumberField(TEXT("tickpin_compiled"), 0);
    TArray<TSharedPtr<FJsonValue>> Anomalies;
    TArray<TSharedPtr<FJsonValue>> States;
    const TArray<FActiveAnomalyInfo> Active = Injector->GetActiveAnomalies();
    APlayerController* PC = World->GetFirstPlayerController();
    for (int32 Route = 0; Route != 4; ++Route)
    {
        const FName Id(CaptureRoutes425[Route]);
        const bool Fire = Active.ContainsByPredicate([&](const FActiveAnomalyInfo& Info) { return Info.Id == Id; });
        const bool Installed = Fire && AnomalyInstall::IsInstalledByte(Injector->GetAnomalyVisualConditionState(Id));
        AActor* Target = Target425(World, CaptureTargets425[Route]);
        AActor* Geometry = Route == 1 ? Target425(World, TEXT("EA_Floor")) : Target;
        if (!Target || !Geometry || !PC) { Failure = TEXT("Target/view missing"); return false; }
        FVector Origin, Extent; Geometry->GetActorBounds(false, Origin, Extent);
        FVector2D Min(1280,720), Max(0,0);
        bool Any = false;
        for (int32 Corner = 0; Corner != 8; ++Corner)
        {
            FVector P = Origin + FVector(Corner & 1 ? Extent.X : -Extent.X, Corner & 2 ? Extent.Y : -Extent.Y, Corner & 4 ? Extent.Z : -Extent.Z);
            FVector2D Screen;
            if (PC->ProjectWorldLocationToScreen(P, Screen, true))
            {
                Any = true; Min.X = FMath::Min(Min.X,Screen.X); Min.Y = FMath::Min(Min.Y,Screen.Y);
                Max.X = FMath::Max(Max.X,Screen.X); Max.Y = FMath::Max(Max.Y,Screen.Y);
            }
        }
        const bool OnScreen = Any && Max.X > 0 && Max.Y > 0 && Min.X < 1280 && Min.Y < 720;
        AnomalyActiveSource::ESource Source;
        AnomalyActiveSource::Resolve(TCHAR_TO_UTF8(CaptureRoutes425[Route]), Source);
        const bool ActiveState = Source == AnomalyActiveSource::ESource::ActorHidden ? AnomalyHiddenClass::IsLogicallyHidden(Target) : Injector->IsAnomalyCurrentlyAnomalous(Id);
        const bool Labelled = Installed && AnomalyActiveSource::IsLabelledMember(Source, false, ActiveState, OnScreen, Installed);
        auto State = MakeShared<FJsonObject>();
        State->SetStringField(TEXT("id"), CaptureRoutes425[Route]); State->SetStringField(TEXT("target_name"), CaptureTargets425[Route]);
        State->SetBoolField(TEXT("installed"), Installed); State->SetBoolField(TEXT("active"), ActiveState);
        State->SetBoolField(TEXT("fire_active"), Fire);
        State->SetBoolField(TEXT("labelled"), Labelled); State->SetBoolField(TEXT("on_screen"), OnScreen);
        State->SetStringField(TEXT("policy"), UTF8_TO_TCHAR(AnomalyLabelSync::DescribeAnnotationPolicy(AnomalyActiveSource::PolicyFor(Source, false))));
        States.Add(MakeShared<FJsonValueObject>(State));
        if (Labelled)
        {
            auto Entry = MakeShared<FJsonObject>(*State);
            Entry->SetStringField(TEXT("event_id"), FString::Printf(TEXT("425-%llu-%d"), RunEpoch, Index / 20)); Entry->SetNumberField(TEXT("target_pixels"), -1);
            Entry->SetNumberField(TEXT("seconds_remaining"), -1); Entry->SetNumberField(TEXT("start_frame"), (Index / 20) * 20 + 3);
            Entry->SetNumberField(TEXT("target_drawn_pixels"), -1); Entry->SetField(TEXT("observable"), MakeShared<FJsonValueNull>());
            Entry->SetBoolField(TEXT("bbox_valid"), OnScreen);
            Min.X = FMath::Clamp(Min.X,0.0f,1280.0f); Min.Y = FMath::Clamp(Min.Y,0.0f,720.0f);
            Max.X = FMath::Clamp(Max.X,0.0f,1280.0f); Max.Y = FMath::Clamp(Max.Y,0.0f,720.0f);
            TArray<TSharedPtr<FJsonValue>> BBox = { MakeShared<FJsonValueNumber>(Min.X), MakeShared<FJsonValueNumber>(Min.Y), MakeShared<FJsonValueNumber>(Max.X-Min.X), MakeShared<FJsonValueNumber>(Max.Y-Min.Y) };
            Entry->SetArrayField(TEXT("bbox_px"), BBox);
            Entry->SetArrayField(TEXT("bbox_norm"), { MakeShared<FJsonValueNumber>(Min.X/1280), MakeShared<FJsonValueNumber>(Min.Y/720), MakeShared<FJsonValueNumber>(Max.X/1280), MakeShared<FJsonValueNumber>(Max.Y/720) });
            Entry->SetField(TEXT("bbox_drawn_px"), MakeShared<FJsonValueNull>());
            Entry->SetBoolField(TEXT("transition"), false);
            Anomalies.Add(MakeShared<FJsonValueObject>(Entry));
        }
    }
    Row->SetArrayField(TEXT("sampled_states"), States); Row->SetArrayField(TEXT("anomalies"), Anomalies);
    Row->SetBoolField(TEXT("anomaly_present"), Anomalies.Num() != 0);
    Pending.Add(Request, Row);
    return true;
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
        Row->SetStringField(TEXT("mask"),FString::Printf(TEXT("mask-%06d.png"),Key.SessionIndex));
        Row->SetStringField(TEXT("mask_evidence"),FString::Printf(TEXT("evidence-%06d.u32x4.zlib"),Key.SessionIndex));
        Row->SetNumberField(TEXT("mask_target_tag"),Key.MaskTarget);
    }
    Row->SetStringField(TEXT("image"), File); Row->SetNumberField(TEXT("family_frame"), Key.FamilyFrame);
    Row->SetNumberField(TEXT("aa"), Key.AA); Row->SetStringField(TEXT("pairing"), TEXT("post_actor_tick_to_post_render_family"));
    Frames.Add(Key.SessionIndex, Row); Pending.Remove(Key.Request);
    return true;
}

void FAnomalyCaptureLabels425::Stop(int32 Written, int32 Dropped)
{
    FWorldDelegates::OnWorldPreActorTick.Remove(PrepareHandle); PrepareHandle.Reset();
    if (Owner.IsValid())
    {
        if (UAnomalyInjectorSubsystem* Injector = Owner->GetSubsystem<UAnomalyInjectorSubsystem>())
        {
            Injector->RevertAllActive(); Injector->SetSynthTickOrder(false);
        }
    }
    const bool MaskRestored = !MaskOwner || MaskOwner->Restore();
    if (!MaskRestored) { Failure = TEXT("Stencil ownership restoration failed"); }
    FApp::SetFixedDeltaTime(SavedDelta); FApp::SetUseFixedTimeStep(SavedFixed);
    CapturePace425 = SavedPace;
    TArray<int32> Indices; Frames.GetKeys(Indices); Indices.Sort();
    FString Lines;
    TArray<TSharedPtr<FJsonValue>> Events;
    for (int32 Index : Indices) { Lines += Json425(Frames[Index]) + TEXT("\n"); }
    for (int32 Event = 0; !BoundaryOnly && Event != (AnomalyCaptureC2::Masks() ? 12 : 24); ++Event)
    {
        TArray<int32> Affected, Injected;
        const TCHAR* Route = CaptureRoutes425[(Event / 3) % 4];
        for (int32 Index : Indices)
        {
            if (Index / 20 != Event) { continue; }
            const auto Row = Frames[Index];
            for (const auto& StateValue : Row->GetArrayField(TEXT("sampled_states")))
            {
                auto State = StateValue->AsObject();
                if (State->GetStringField(TEXT("id")) != Route) { continue; }
                if (State->GetBoolField(TEXT("fire_active"))) { Injected.Add(Index); }
                if (State->GetBoolField(TEXT("labelled"))) { Affected.Add(Index); }
            }
        }
        auto Item = MakeShared<FJsonObject>(); Item->SetStringField(TEXT("event_id"),FString::Printf(TEXT("425-%llu-%d"),RunEpoch,Event));
        Item->SetNumberField(TEXT("bench_event"),Event);
        Item->SetStringField(TEXT("anomaly_type"),Route); Item->SetNumberField(TEXT("tick_order"), Event / 12);
        Item->SetStringField(TEXT("anomaly_subtype"),TEXT(""));
        Item->SetObjectField(TEXT("affected_frames"),IndexSet425(Affected)); Item->SetObjectField(TEXT("injected_frames"),IndexSet425(Injected));
        Item->SetArrayField(TEXT("transition_frames"),TArray<TSharedPtr<FJsonValue>>());
        Item->SetObjectField(TEXT("transition_reasons"),MakeShared<FJsonObject>());
        Item->SetStringField(TEXT("bbox_source"),TEXT("projected_actor_bounds"));
        Item->SetBoolField(TEXT("observability_measured"),false); Item->SetNumberField(TEXT("observable_frame_count"),0);
        Item->SetNumberField(TEXT("unmeasured_frame_count"),Affected.Num());
        Item->SetBoolField(TEXT("manifested"),Affected.Num() != 0);
        auto Objects = MakeShared<FJsonObject>(); Objects->SetNumberField(TEXT("count"),1); Objects->SetNumberField(TEXT("primary_index"),0);
        auto Node = MakeShared<FJsonObject>(); Node->SetStringField(TEXT("name"),CaptureTargets425[(Event/3)%4]);
        Objects->SetArrayField(TEXT("nodes"), { MakeShared<FJsonValueObject>(Node) });
        Item->SetObjectField(TEXT("affected_objects"),Objects);
        Events.Add(MakeShared<FJsonValueObject>(Item));
    }
    auto Annotation = MakeShared<FJsonObject>();
    Annotation->SetNumberField(TEXT("label_schema"),2); Annotation->SetNumberField(TEXT("label_schema_minor"),1);
    Annotation->SetNumberField(TEXT("transition_frame_count"),0);
    Annotation->SetStringField(TEXT("session_id"),FString::Printf(TEXT("425-%llu"),RunEpoch));
    Annotation->SetStringField(TEXT("profile"),AnomalyCaptureC2::Masks() ? TEXT("4.25 paired colour, labels, stencil/depth and masks") : TEXT("4.25 colour and frame-exact labels; masks unavailable"));
    auto Video = MakeShared<FJsonObject>(); Video->SetStringField(TEXT("frames_dir"),Output);
    Video->SetStringField(TEXT("path"),TEXT("")); Video->SetNumberField(TEXT("fps"),30);
    Video->SetNumberField(TEXT("total_frames"),Written); Video->SetNumberField(TEXT("target_fps"),30);
    Video->SetArrayField(TEXT("resolution"), { MakeShared<FJsonValueNumber>(1280), MakeShared<FJsonValueNumber>(720) });
    Annotation->SetObjectField(TEXT("video"),Video); Annotation->SetArrayField(TEXT("anomalies"),Events);
    auto Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("result"), Failed() || Pending.Num() || Dropped || Written != Frames.Num() ? TEXT("FAIL") : TEXT("PASS"));
    Result->SetStringField(TEXT("failure"),Failure); Result->SetNumberField(TEXT("epoch"),double(RunEpoch));
    Result->SetNumberField(TEXT("written"),Written); Result->SetNumberField(TEXT("paired"),Frames.Num());
    Result->SetNumberField(TEXT("unmatched_dropped"),Pending.Num()); Result->SetNumberField(TEXT("dropped"),Dropped);
    Result->SetBoolField(TEXT("mask_state_restored"),MaskRestored);
    Result->SetBoolField(TEXT("boundary_only"),BoundaryOnly); Result->SetBoolField(TEXT("pacing_restored"),FApp::UseFixedTimeStep() == SavedFixed && FApp::GetFixedDeltaTime() == SavedDelta);
    Result->SetArrayField(TEXT("commands"),Commands);
    const bool Saved = FFileHelper::SaveStringToFile(Lines,*(Output/TEXT("labels.jsonl")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
        FFileHelper::SaveStringToFile(Json425(Annotation),*(Output/TEXT("annotation.json")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
        FFileHelper::SaveStringToFile(Json425(Result),*(Output/TEXT("labels-receipt.json")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    if (!Saved) { Failure = TEXT("Label write failed"); }
    Pending.Empty();
}

bool AnomalyCaptureC2::Enabled() { return CapturePhase425 != 0; }
bool AnomalyCaptureC2::Boundary() { return CapturePhase425 == 2; }
bool AnomalyCaptureC2::Masks() { return CapturePhase425 == 3; }
void AnomalyCaptureC2::Finished(UWorld* World, bool Success)
{
    if (!Success || CapturePhase425 != 1)
    {
        CapturePhase425 = 0; FPlatformMisc::RequestExit(false); return;
    }
    CapturePhase425 = 2;
    if (GEngine) { GEngine->Exec(World,TEXT("open /Game/EuropaPhaseA/EA_Travel")); }
    const double Deadline = FPlatformTime::Seconds() + 12;
    FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Deadline](float)
    {
        if (FPlatformTime::Seconds() > Deadline) { CapturePhase425 = 0; FPlatformMisc::RequestExit(false); return false; }
        if (!GEngine) { return true; }
        for (const FWorldContext& Context : GEngine->GetWorldContexts())
        {
            UWorld* Next = Context.World();
            if (!Next || !Next->IsGameWorld() || Next->GetName() != TEXT("EA_Travel") || !Camera425(Next)) { continue; }
            UAnomalyCaptureSubsystem* Capture = Next->GetSubsystem<UAnomalyCaptureSubsystem>();
            if (!Capture) { continue; }
            Capture->StartRun(CaptureRoot425/TEXT("epoch02"),true,0,12);
            return false;
        }
        return true;
    }));
}
static FAutoConsoleCommandWithWorldAndArgs CaptureLabelsCommand425(TEXT("IAI.Capture.C2"),TEXT("C2 bounded two-order label qualification"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* World)
    {
        if (CapturePhase425 || Args.Num() != 1 || !FParse::Param(FCommandLine::Get(),TEXT("IAIBench")) || !Camera425(World)) { return; }
        CaptureRoot425 = Args[0]; CapturePhase425 = 1;
        if (UAnomalyCaptureSubsystem* Capture = World->GetSubsystem<UAnomalyCaptureSubsystem>())
        {
            Capture->StartRun(CaptureRoot425/TEXT("epoch01"),true,0,480);
        }
    }));
static FAutoConsoleCommandWithWorldAndArgs CaptureMasksCommand425(TEXT("IAI.Capture.C3"),TEXT("C3 bounded paired RGB, stencil and masks"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args,UWorld* World)
    {
        if (CapturePhase425 || Args.Num() != 1 || !FParse::Param(FCommandLine::Get(),TEXT("IAIBench")) || !Camera425(World)) { return; }
        CaptureRoot425 = Args[0]; CapturePhase425 = 3;
        if (UAnomalyCaptureSubsystem* Capture = World->GetSubsystem<UAnomalyCaptureSubsystem>())
        {
            Capture->StartRun(CaptureRoot425/TEXT("epoch01"),true,0,240);
        }
    }));
#endif
