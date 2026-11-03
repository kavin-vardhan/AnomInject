#include "AnomalyMasks425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyCaptureLog.h"
#include "AnomalyPrivatePass425.h"
#include "AnomalyCaptureLabels425.h"
#include "AnomalyCensusPolicy425.h"
#include "AnomalyCensusBatch425.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyHiddenClass.h"
#include "AnomalyInstallState.h"
#include "AnomalyActiveSource.h"
#include "AnomalyViewport.h"
#include "Components/LightComponent.h"
#include "AnomalyMask425Shader.h"
#include "AnomalyMaskReduceShader.h"
#include "AnomalyPreviewCapture.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "Misc/Compression.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "PostProcess/SceneRenderTargets.h"
#include "SceneRendering.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"

namespace
{
    FAnomalyMasks425* ActiveMasks425 = nullptr;
    int32 CensusOff425=0;
    FAutoConsoleVariableRef CensusOffVariable425(TEXT("IAI.Capture.BenchCensusOff"),CensusOff425,TEXT("Bench-only census bypass; zero is normal census"));
}
void FAnomalyMasks425::SetBenchCensus(bool Enabled)
{
    if (FParse::Param(FCommandLine::Get(),TEXT("IAIBench"))) { CensusOff425=Enabled?0:1; }
}
bool FAnomalyMasks425::HostDepth(const AActor* Actor) const
{
    if (!Actor || HostActors.Contains(const_cast<AActor*>(Actor))) { return true; }
    if (Tags.Contains(const_cast<AActor*>(Actor))) { return false; }
    TInlineComponentArray<UPrimitiveComponent*> Prims(Actor);
    for (const UPrimitiveComponent* Prim:Prims) { if (Prim->bRenderCustomDepth) { return true; } }
    return false;
}
bool FAnomalyMasks425::Start(UWorld* World)
{
    if (!World || ActiveMasks425 || Started || AnomalyStencilTag::IsAnyTagged()) { return false; }
    UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
    if (!Auto || Auto->HasCensusProvider()) { return false; }
    Legacy=AnomalyPrivate425::Legacy(World);
    if (!Legacy && !AnomalyPrivate425::Available(World)) { return false; }
    Owner=World; Ledger.Reset(); Ledger.HostReserved=AnomalyStencilTag::SnapshotHostReservedValues(World);
    if (Legacy) { AnomalyStencilTag::SetRestoreWatch(&Ledger); AnomalyStencilTag::EnableCustomStencil(); }
    for (TActorIterator<AActor> It(World);It;++It) { if (HostDepth(*It)) { HostActors.Add(*It); } }
    Started=true; ActiveMasks425=this; CycleStarted=GFrameCounter;
    for (const auto& A:AnomalyViewport::GetVisibleRenderableActors(World)) { InitialVisible.Add(GetNameSafe(A.Get())); }
    Auto->SetCensusProvider([this](const AActor* Actor)
    {
        FAnomalyCensusOpinion O; O.WindowTicks=AnomalyCensusPolicy425::Window(LastCycleTicks);
        if (HostDepth(Actor)) { O.Decision=EAnomalyCensusDecision::ExcludedZero; O.Reason=TEXT("host_custom_depth_untouched"); return O; }
        if (!Legacy && !AnomalyPrivate425::Supports(Actor)) { O.Decision=EAnomalyCensusDecision::ExcludedZero; O.Reason=TEXT("private_unavailable"); return O; }
        if (!Legacy && !AnomalyPrivate425::Verified(Actor)) { O.Decision=EAnomalyCensusDecision::ExcludedZero; O.Reason=TEXT("private_unmeasured"); return O; }
        const FCensusValue* V=Verdicts.Find(const_cast<AActor*>(Actor));
        if (!V) { O.Decision=EAnomalyCensusDecision::FallbackBounds; O.Reason=TEXT("unseen"); O.bUnseen=true; return O; }
        if (V->Translucent) { O.Decision=EAnomalyCensusDecision::ExcludedTranslucent; O.Reason=TEXT("translucent"); return O; }
        O.AgeTicks=int32(GFrameCounter-V->Tick); O.DrawnPct=100.0f*V->Pixels/FMath::Max(1,V->Area);
        if (O.AgeTicks>O.WindowTicks) { O.Decision=EAnomalyCensusDecision::FallbackBounds; O.Reason=TEXT("expired"); O.bExpired=true; return O; }
        const int32 Band=AnomalyCensusPolicy425::Band(V->Pixels,V->Area);
        O.Decision=Band==0 ? EAnomalyCensusDecision::ExcludedZero : Band==-1 ? EAnomalyCensusDecision::ExcludedBelowFloor : Band==2 ? EAnomalyCensusDecision::ExcludedAboveCeiling : EAnomalyCensusDecision::Eligible;
        O.Reason=Band==0 ? TEXT("measured_zero") : Band==-1 ? TEXT("below_floor") : Band==2 ? TEXT("above_ceiling") : TEXT("measured_nonzero"); return O;
    },[this]() { return Cycles>0; },FAnomalyCensusFireReportFn(),12);
    return true;
}
void FAnomalyMasks425::Release()
{
    if (Legacy)
    {
        AnomalyStencilTag::RestoreAll();
        for (const auto& Pair:WriteMasks) { if (Pair.Key.IsValid()) { Pair.Key->SetCustomDepthStencilWriteMask(Pair.Value); } }
    }
    Tags.Reset(); WriteMasks.Reset(); Ledger.CensusClaimed.Reset(); Ledger.EventClaimed.Reset(); EventTags.Reset();
}
int32 FAnomalyMasks425::Tag(AActor* Actor,uint8 Value)
{
    if (!Actor || !Value || HostDepth(Actor)) { return 0; }
    if (Tags.Contains(Actor)) { return 1; }
    if (!Legacy)
    {
        if (!AnomalyPrivate425::Supports(Actor)) { return 0; }
        Tags.Add(Actor,Value); return 1;
    }
    TInlineComponentArray<UPrimitiveComponent*> Prims(Actor);
    for (UPrimitiveComponent* Prim:Prims)
    {
        if (AnomalyViewport::IsRenderableComponent(Prim) && AnomalyViewport::IsInEventComponentScope(Prim))
        {
            WriteMasks.FindOrAdd(Prim,Prim->CustomDepthStencilWriteMask);
            Prim->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_Default);
        }
    }
    const int32 N=AnomalyStencilTag::TagActor(Actor,Value); if (N) { Tags.Add(Actor,Value); } return N;
}
uint8 FAnomalyMasks425::FreeTag() const
{
    for (int32 V=AnomalyStencilTag::ReservedStencilBase;V<=AnomalyStencilTag::AssignableStencilMax;++V) { if (Ledger.IsFree(uint8(V))) { return uint8(V); } }
    return 0;
}
TArray<TWeakObjectPtr<AActor>> FAnomalyMasks425::Receivers(const FAutoLiveFireInfo& Fire) const
{
    TArray<TWeakObjectPtr<AActor>> Result;
    AActor* Actor=Fire.TargetActor.Get(); if (!Actor) { return Result; }
    if (Fire.Id!=FName(TEXT("lighting_mismatch"))) { Result.Add(Actor); return Result; }
    TInlineComponentArray<ULightComponent*> Lights(Actor);
    for (const auto& Weak:AnomalyViewport::GetCensusPrefilterActors(Owner.Get()))
    {
        if (!Weak.IsValid() || (!Legacy && !AnomalyPrivate425::Supports(Weak.Get()))) { continue; }
        TInlineComponentArray<UPrimitiveComponent*> Prims(Weak.Get()); bool Affected=false;
        for (ULightComponent* Light:Lights) { for (UPrimitiveComponent* Prim:Prims) { Affected|=Light->AffectsPrimitive(Prim); } }
        if (Affected) { Result.Add(Weak); }
    }
    return Result;
}
TArray<uint8> FAnomalyMasks425::TagsFor(const FAutoLiveFireInfo& Fire) const
{
    TArray<uint8> Result;
    if (const FEventBinding* Binding=EventTags.Find(Fire.Id)) { Result=Binding->Values; } return Result;
}
void FAnomalyMasks425::ReleaseActor(AActor* Actor)
{
    if (!Actor) { return; }
    if (!Legacy) { Tags.Remove(Actor); return; }
    AnomalyStencilTag::RestoreActor(Actor);
    TInlineComponentArray<UPrimitiveComponent*> Prims(Actor);
    for (UPrimitiveComponent* Prim:Prims)
    {
        if (auto* Mask=WriteMasks.Find(Prim)) { Prim->SetCustomDepthStencilWriteMask(*Mask); WriteMasks.Remove(Prim); }
    }
    Tags.Remove(Actor);
}
void FAnomalyMasks425::ReleaseUnused()
{
    Ledger.EventClaimed.Reset(); Ledger.CensusClaimed.Reset();
    TSet<TWeakObjectPtr<AActor>> Held;
    for (const auto& Event:EventTags)
    {
        for (const auto& A:Event.Value.Actors) { Held.Add(A); }
        for (uint8 Value:Event.Value.Values) { Ledger.EventClaimed.Add(Value); }
    }
    for (const auto& Batch:Batches)
    {
        for (const auto& Pair:Batch.Value.Actors) { Held.Add(Pair.Key); Ledger.CensusClaimed.Add(Pair.Value); }
    }
    TArray<TWeakObjectPtr<AActor>> Retired;
    for (const auto& Pair:Tags) { if (!Held.Contains(Pair.Key)) { Retired.Add(Pair.Key); } }
    for (const auto& A:Retired) { ReleaseActor(A.Get()); }
}
void FAnomalyMasks425::Advance(uint64 Request)
{
    UWorld* World=Owner.Get(); if (!World) { return; }
    Visible=AnomalyViewport::GetVisibleRenderableActors(World);
    PeakVisible=FMath::Max(PeakVisible,Visible.Num());
    auto* Auto=World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
    const auto Fires=Auto ? Auto->GetLiveFires() : TArray<FAutoLiveFireInfo>();
    TArray<FName> Retired;
    for (const auto& Pair:EventTags)
    {
        bool Live=false;
        for (const auto& Fire:Fires) { Live|=Fire.Id==Pair.Key && Fire.StartFrame==Pair.Value.Start; }
        if (!Live) { Retired.Add(Pair.Key); }
    }
    for (FName Id:Retired) { EventTags.Remove(Id); }
    ReleaseUnused();
    for (const auto& Fire:Fires)
    {
        if (EventTags.Contains(Fire.Id)) { continue; }
        FEventBinding Binding; Binding.Start=Fire.StartFrame;
        uint8 Group=0;
        for (const auto& A:Receivers(Fire))
        {
            if (!A.IsValid() || HostDepth(A.Get())) { continue; }
            const uint8* Existing=Tags.Find(A);
            const uint8 Value=Existing ? *Existing : (Group ? Group : FreeTag());
            if (!Value) { ++Exhausted; continue; }
            if (Tag(A.Get(),Value))
            {
                Binding.Actors.Add(A); Binding.Values.AddUnique(Value); Ledger.EventClaimed.Add(Value);
                if (!Existing) { Group=Value; }
            }
        }
        EventTags.Add(Fire.Id,Binding);
    }
    if (CensusOff425 && FParse::Param(FCommandLine::Get(),TEXT("IAIBench"))) { return; }
    if (Batches.Num()>=2) { return; }
    if (Cursor>=Queue.Num())
    {
        if (Queue.Num()) { LastCycleTicks=int32(GFrameCounter-CycleStarted); ++Cycles; }
        Queue=AnomalyViewport::GetCensusPrefilterActors(World);
        Queue.Sort([](const TWeakObjectPtr<AActor>& A,const TWeakObjectPtr<AActor>& B) { return GetNameSafe(A.Get())<GetNameSafe(B.Get()); });
        Cursor=0; CycleStarted=GFrameCounter;
    }
    TMap<TWeakObjectPtr<AActor>,uint8> Batch;
    const int32 Cap=AnomalyCensusPolicy425::Batch(Ledger.NumFree());
    while (Cursor<Queue.Num() && Batch.Num()<Cap)
    {
        AActor* A=Queue[Cursor++].Get(); if (!A || HostDepth(A) || Tags.Contains(A) || A->IsHidden() || AnomalyHiddenClass::IsLogicallyHidden(A)) { continue; }
        TInlineComponentArray<UPrimitiveComponent*> Prims(A); int32 Renderable=0,Translucent=0;
        for (UPrimitiveComponent* Prim:Prims) { if (AnomalyViewport::IsRenderableComponent(Prim)) { ++Renderable; Translucent+=AnomalyViewport::IsTranslucentOnlyComponent(Prim,false) ? 1 : 0; } }
        if (Renderable && Renderable==Translucent) { FCensusValue V; V.Translucent=true; Verdicts.Add(A,V); continue; }
        const uint8 Value=FreeTag(); if (!Value) { break; }
        if (Tag(A,Value)) { Ledger.CensusClaimed.Add(Value); Batch.Add(A,Value); }
    }
    if (Batch.Num())
    {
        FBatch Pending; Pending.Actors=MoveTemp(Batch); Pending.Armed=GFrameCounter;
        Pending.Allowed=Ledger.HostReserved;
        for (const auto& Pair:Tags) { Pending.Allowed.Add(Pair.Value); }
        Batches.Add(Request,MoveTemp(Pending));
    }
}
void FAnomalyMasks425::ReleaseBatch(uint64 Request,bool Requeue)
{
    if (auto* Batch=Batches.Find(Request))
    {
        if (Requeue) { for (const auto& Pair:Batch->Actors) { if (Pair.Key.IsValid()) { Queue.Insert(Pair.Key,Cursor); } } }
        Batches.Remove(Request); ReleaseUnused();
    }
}
void FAnomalyMasks425::NoPass(uint64 Request)
{
    if (ActiveMasks425) { ActiveMasks425->NoPassRequests.Enqueue(Request); }
}
void FAnomalyMasks425::Tick(UWorld* World)
{
    auto* Self=ActiveMasks425; if (!Self || Self->Owner.Get()!=World) { return; }
    uint64 Request=0;
    while (Self->NoPassRequests.Dequeue(Request))
    {
        if (Self->Batches.Contains(Request)) { ++Self->NoPassBatches; Self->ReleaseBatch(Request,true); }
    }
    TArray<uint64> Lost;
    for (const auto& Pair:Self->Batches)
    {
        if (AnomalyCensusBatch425::Outcome(GFrameCounter,Pair.Value.Armed,false,false,false)==AnomalyCensusBatch425::EOutcome::Lost) { Lost.Add(Pair.Key); }
    }
    for (uint64 Id:Lost)
    {
        ++Self->LostBatches; Self->ReleaseBatch(Id,true);
        UE_LOG(LogAnomalyCapture,Warning,TEXT("Census425: BATCH LOST request=%llu timeout=8 requeued=1 verdict_unchanged=1"),Id);
    }
}
FString FAnomalyMasks425::Diagnostic(UWorld* World)
{
    auto* Self=ActiveMasks425;
    if (!Self || Self->Owner.Get()!=World) { return TEXT("census=off batches=0 lost=0 no_pass=0 polluted=0"); }
    return FString::Printf(TEXT("census=%s batches=%d lost=%d no_pass=%d polluted=%d"),CensusOff425?TEXT("bench_off"):TEXT("on"),Self->Batches.Num(),Self->LostBatches,Self->NoPassBatches,Self->PollutedBatches);
}
void FAnomalyMasks425::Observe(const FAnomalyCapturedFrame& Frame)
{
    FAnomalyMasks425* Self=ActiveMasks425;
    if (!Self || !Self->Owner.IsValid() || Frame.LabelKey425.Owner!=Self->Owner->GetUniqueID()) { return; }
    const bool Pass=Frame.Width>1 && Frame.Height>1 && Frame.MaskData425.Num()==Frame.Width*Frame.Height*16;
    if (!Pass)
    {
        if (Self->Batches.Contains(Frame.RequestId)) { ++Self->NoPassBatches; Self->ReleaseBatch(Frame.RequestId,true); }
        return;
    }
    uint32 Counts[256]={}; const uint32* Data=reinterpret_cast<const uint32*>(Frame.MaskData425.GetData());
    for (int32 I=0;I<Frame.Width*Frame.Height;++I)
    {
        const uint32 Tag=Data[I*4]; float Custom,Scene;
        FMemory::Memcpy(&Custom,Data+I*4+1,4); FMemory::Memcpy(&Scene,Data+I*4+2,4);
        if (Tag<256 && Custom>0 && Custom>=Scene-0.000001f) { ++Counts[Tag]; }
    }
    if (auto* Batch=Self->Batches.Find(Frame.RequestId))
    {
        bool Polluted=false;
        for (int32 Tag=AnomalyStencilTag::ReservedStencilBase;Tag<=AnomalyStencilTag::AssignableStencilMax;++Tag)
        {
            Polluted|=Counts[Tag]>0 && !Batch->Allowed.Contains(uint8(Tag)) && !Self->Ledger.HostReserved.Contains(uint8(Tag)) && !Self->Ledger.CensusClaimed.Contains(uint8(Tag)) && !Self->Ledger.EventClaimed.Contains(uint8(Tag));
        }
        if (AnomalyCensusBatch425::Outcome(GFrameCounter,Batch->Armed,true,Pass,Polluted)!=AnomalyCensusBatch425::EOutcome::Credit)
        {
            ++Self->PollutedBatches; Self->ReleaseBatch(Frame.RequestId,true); return;
        }
        for (const auto& Pair:Batch->Actors)
        {
            FString Detail;
            if (!Pair.Key.IsValid() || (Self->Legacy ? !AnomalyStencilTag::VerifyActorStillTagged(Pair.Key.Get(),Pair.Value,Detail) : !AnomalyPrivate425::Verified(Pair.Key.Get()))) { continue; }
            FCensusValue V; V.Pixels=Counts[Pair.Value]; V.Area=Frame.Width*Frame.Height; V.Tick=Frame.LabelKey425.SampleFrame;
            Self->Verdicts.Add(Pair.Key,V);
        }
        Self->ReleaseBatch(Frame.RequestId,false);
    }
}
TSharedPtr<FJsonObject> FAnomalyMasks425::State() const
{
    auto R=MakeShared<FJsonObject>(); TArray<TSharedPtr<FJsonValue>> Owners,VisibleNames,Reserved,Initial;
    for (const auto& Pair:Tags)
    {
        auto O=MakeShared<FJsonObject>(); O->SetStringField(TEXT("actor"),GetNameSafe(Pair.Key.Get())); O->SetNumberField(TEXT("tag"),Pair.Value);
        O->SetBoolField(TEXT("event"),Ledger.EventClaimed.Contains(Pair.Value)); Owners.Add(MakeShared<FJsonValueObject>(O));
    }
    for (const auto& A:Visible) { VisibleNames.Add(MakeShared<FJsonValueString>(GetNameSafe(A.Get()))); }
    for (uint8 V:Ledger.HostReserved) { Reserved.Add(MakeShared<FJsonValueNumber>(V)); }
    for (const FString& N:InitialVisible) { Initial.Add(MakeShared<FJsonValueString>(N)); }
    R->SetArrayField(TEXT("owners"),Owners); R->SetArrayField(TEXT("visible"),VisibleNames); R->SetArrayField(TEXT("host_reserved"),Reserved); R->SetArrayField(TEXT("initial_visible"),Initial);
    R->SetNumberField(TEXT("batches_lost"),LostBatches); R->SetNumberField(TEXT("no_pass_batches"),NoPassBatches); R->SetNumberField(TEXT("polluted_batches"),PollutedBatches);
    R->SetNumberField(TEXT("in_flight_batches"),Batches.Num()); R->SetBoolField(TEXT("enabled"),!CensusOff425);
    R->SetNumberField(TEXT("host_actors_excluded"),HostActors.Num());
    R->SetObjectField(TEXT("private_pass"),AnomalyPrivate425::State()); R->SetBoolField(TEXT("legacy_reference"),Legacy);
    R->SetNumberField(TEXT("visible_count"),Visible.Num()); R->SetNumberField(TEXT("peak_visible"),PeakVisible); R->SetNumberField(TEXT("cycles"),Cycles);
    R->SetNumberField(TEXT("ledger_exhausted"),Exhausted); R->SetNumberField(TEXT("floor_pct"),AnomalyCensusPolicy425::Floor); R->SetNumberField(TEXT("ceiling_pct"),AnomalyCensusPolicy425::Ceiling);
    return R;
}
bool FAnomalyMasks425::Restore()
{
    if (!Started) { return true; }
    if (Owner.IsValid()) { if (auto* Auto=Owner->GetSubsystem<UAnomalyAutoInjectorSubsystem>()) { Auto->ClearCensusProvider(); } }
    if (Legacy) { FlushRenderingCommands(); }
    Release(); const bool Good=!AnomalyStencilTag::IsAnyTagged();
    if (Legacy) { AnomalyStencilTag::SetRestoreWatch(nullptr); AnomalyStencilTag::DisableCustomStencil(); }
    else { AnomalyPrivate425::Stop(); }
    UE_LOG(LogAnomalyCapture,Log,TEXT("Capture425 C5: stencil_restore=%d peak_visible=%d cycles=%d exhausted=%d lost=%d no_pass=%d polluted=%d"),Good?1:0,PeakVisible,Cycles,Exhausted,LostBatches,NoPassBatches,PollutedBatches);
    CensusOff425=0;
    if (ActiveMasks425==this) { ActiveMasks425=nullptr; } Started=false; Ledger.Reset(); return Good;
}
bool FAnomalyMasks425::Labelled(UWorld* World,const FAutoLiveFireInfo& Fire)
{
    UAnomalyInjectorSubsystem* Injector=World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
    if (!Injector || !Fire.TargetActor.IsValid()) { return false; }
    const bool Installed=AnomalyInstall::IsInstalledByte(Injector->GetAnomalyVisualConditionState(Fire.Id));
    AnomalyActiveSource::ESource Source;
    if (!AnomalyActiveSource::Resolve(TCHAR_TO_UTF8(*Fire.Id.ToString()),Source)) { return false; }
    const bool Active=Source==AnomalyActiveSource::ESource::ActorHidden ? AnomalyHiddenClass::IsLogicallyHidden(Fire.TargetActor.Get()) : Injector->IsAnomalyCurrentlyAnomalous(Fire.Id);
    return Installed && AnomalyActiveSource::IsLabelledMember(Source,false,Active,true,Installed);
}
uint64 FAnomalyMasks425::Selection(UWorld* World,uint64 Request)
{
    if (!ActiveMasks425 || ActiveMasks425->Owner.Get()!=World) { return 0; }
    ActiveMasks425->Advance(Request);
    if (!ActiveMasks425->Legacy) { AnomalyPrivate425::Prepare(Request,ActiveMasks425->Tags); }
    UAnomalyAutoInjectorSubsystem* Auto=World->GetSubsystem<UAnomalyAutoInjectorSubsystem>(); if (!Auto) { return 0; }
    uint64 Bits=0;
    for (const auto& Fire:Auto->GetLiveFires()) { if (Labelled(World,Fire)) { for (uint8 Tag:ActiveMasks425->TagsFor(Fire)) { Bits|=uint64(1)<<(Tag-AnomalyStencilTag::ReservedStencilBase); } } }
    return Bits;
}
bool FAnomalyMasks425::Render(FRHICommandListImmediate& Cmd, FSceneViewFamily& Family, uint64 Request, uint64 Selected, FAnomalyFrameCapturer& Capturer)
{
    if (AnomalyPrivate425::Active()) { return AnomalyPrivate425::Render(Cmd,Family,Request,Selected,Capturer); }
    if (!ActiveMasks425 || !ActiveMasks425->Legacy) { return false; }
    if (Family.Views.Num() != 1 || !Family.Scene) { return false; }
    const FViewInfo& View = static_cast<const FViewInfo&>(*Family.Views[0]);
    const FIntPoint Size = View.UnscaledViewRect.Size();
    if (Size.X <= 0 || Size.Y <= 0 || View.ViewRect.Size() != Size) { return false; }
    FSceneRenderTargets& Context = FSceneRenderTargets::Get(Cmd);
    if (!Context.bCustomDepthIsValid || !Context.CustomDepth || !Context.CustomStencilSRV.IsValid()) { return false; }
    FRDGBuilder Graph(Cmd);
    auto MakeTexture = [&](EPixelFormat Format, const TCHAR* Name)
    {
        return Graph.CreateTexture(FRDGTextureDesc::Create2DDesc(Size, Format, FClearValueBinding::None,
            TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false), Name);
    };
    FRDGTextureRef Evidence = MakeTexture(PF_R32G32B32A32_UINT, TEXT("AnomalyEvidence425"));
    FRDGTextureRef Mask = MakeTexture(PF_R32_UINT, TEXT("AnomalyMask425"));
    FRDGTextureRef Drawn = MakeTexture(PF_R32_UINT, TEXT("AnomalyDrawn425"));
    TShaderMapRef<FAnomalyMask425CS> Shader(GetGlobalShaderMap(Family.GetFeatureLevel()));
    auto* Params = Graph.AllocParameters<FAnomalyMask425CS::FParameters>();
    Params->SceneTextures = CreateSceneTextureUniformBuffer(Context, Family.GetFeatureLevel(), ESceneTextureSetupMode::SceneDepth | ESceneTextureSetupMode::CustomDepth, UniformBuffer_SingleDraw);
    Params->Evidence = Graph.CreateUAV(Evidence); Params->Mask = Graph.CreateUAV(Mask); Params->Drawn = Graph.CreateUAV(Drawn);
    Params->OutputSize = Size; Params->InternalMin = View.ViewRect.Min; Params->InternalSize = View.ViewRect.Size();
    Params->SelectedLo = uint32(Selected); Params->SelectedHi = uint32(Selected >> 32); Params->BaseTag = AnomalyStencilTag::ReservedStencilBase; Params->DepthBias = 0.000001f;
    FComputeShaderUtils::AddPass(Graph, RDG_EVENT_NAME("AnomalyMask425"), Shader, Params, FIntVector(FMath::DivideAndRoundUp(Size.X,8),FMath::DivideAndRoundUp(Size.Y,8),1));
    FRDGBufferDesc TableDesc = FRDGBufferDesc::CreateBufferDesc(4,1536);
    TableDesc.Usage = EBufferUsageFlags(TableDesc.Usage | BUF_SourceCopy);
    FRDGBufferRef Table = Graph.CreateBuffer(TableDesc, TEXT("AnomalyMaskTable425"));
    auto TableUAV = Graph.CreateUAV(Table, PF_R32_UINT);
    AddClearUAVPass(Graph, TableUAV, 0u);
    TShaderMapRef<FAnomalyMaskReduceCS> Reduce(GetGlobalShaderMap(Family.GetFeatureLevel()));
    auto* RP = Graph.AllocParameters<FAnomalyMaskReduceCS::FParameters>();
    RP->MaskTexture = Mask; RP->DrawnTexture = Drawn; RP->OutTable = TableUAV; RP->MaskSize = Size;
    FComputeShaderUtils::AddPass(Graph, RDG_EVENT_NAME("AnomalyMaskReduce425"), Reduce, RP, FIntVector(FMath::DivideAndRoundUp(Size.X,8),FMath::DivideAndRoundUp(Size.Y,8),1));
    TRefCountPtr<IPooledRenderTarget> Extracted;
    TRefCountPtr<FPooledRDGBuffer> Reduced;
    Graph.QueueTextureExtraction(Evidence, &Extracted);
    Graph.QueueBufferExtraction(Table, &Reduced, FRDGResourceState::EAccess::Read, FRDGResourceState::EPipeline::Graphics);
    Graph.Execute();
    if (!Extracted || !Reduced || !Reduced->VertexBuffer.IsValid()) { return false; }
    Capturer.CaptureMask_RenderThread(Request, Extracted->GetRenderTargetItem().ShaderResourceTexture, Reduced->VertexBuffer);
    if (Request == 1) { UE_LOG(LogAnomalyCapture, Log, TEXT("Capture425 C3: shaders_loaded=visible425,reduce local_rdg=1 output=native_view depth_bias=0.000001")); }
    return true;
}
bool FAnomalyMasks425::Save(const FAnomalyCapturedFrame& Frame, const FString& Output)
{
    if (Frame.MaskData425.Num() != Frame.Width * Frame.Height * 16 || Frame.MaskTable425.Num() != 1536) { return false; }
    const uint32* Data = reinterpret_cast<const uint32*>(Frame.MaskData425.GetData());
    TArray<FColor> Pixels; Pixels.SetNumUninitialized(Frame.Width*Frame.Height);
    uint32 Cpu[1536] = {};
    for (int32 I = 0; I < Pixels.Num(); ++I)
    {
        const uint32 M = Data[I*4+3] & 255, D = (Data[I*4+3] >> 8) & 255;
        Pixels[I] = FColor(uint8(M),uint8(M),uint8(M),255);
        if (M)
        {
            uint32* T = Cpu + M*6; ++T[0];
            T[1] = FMath::Max(T[1],~uint32(I%Frame.Width)); T[2] = FMath::Max(T[2],~uint32(I/Frame.Width));
            T[3] = FMath::Max(T[3],uint32(I%Frame.Width)); T[4] = FMath::Max(T[4],uint32(I/Frame.Width));
        }
        if (D) { ++Cpu[D*6+5]; }
    }
    if (FMemory::Memcmp(Cpu, Frame.MaskTable425.GetData(), sizeof(Cpu)) != 0) { return false; }
    const FString Stem = FString::Printf(TEXT("%06llu"),Frame.RequestId-1);
    uint32 MaskPixels = 0;
    for (int32 Tag = 1; Tag <= 255; ++Tag) { MaskPixels += Cpu[Tag*6]; }
    if (!Frame.LabelKey425.MaskTarget && MaskPixels) { return false; }
    if (MaskPixels)
    {
        TArray<uint8> Png;
        if (!AnomalyPreview::EncodePixels(AnomalyPreview::EImageFormat::PNG,Pixels,Frame.Width,Frame.Height,Png,0) ||
            !FFileHelper::SaveArrayToFile(Png,*(Output/(TEXT("mask-")+Stem+TEXT(".png"))))) { return false; }
    }
    TArray<uint8> Planes;
    const uint8* Bytes=Frame.MaskData425.GetData();
    const bool Planar=AnomalyCaptureC2::Street();
    if (Planar)
    {
        Planes.SetNumUninitialized(Frame.MaskData425.Num());
        const int32 N=Frame.Width*Frame.Height;
        for (int32 B=0;B<16;++B) { for (int32 I=0;I<N;++I) { Planes[B*N+I]=Bytes[I*16+B]; } }
        Bytes=Planes.GetData();
    }
    int32 Size = FCompression::CompressMemoryBound(NAME_Zlib,Frame.MaskData425.Num());
    TArray<uint8> Packed; Packed.SetNumUninitialized(Size);
    if (!FCompression::CompressMemory(NAME_Zlib,Packed.GetData(),Size,Bytes,Frame.MaskData425.Num())) { return false; }
    Packed.SetNum(Size,false);
    if (Planar) { const uint8 Magic[4]={65,83,80,49}; Packed.Insert(Magic,4,0); }
    TArray<uint8> TableBytes; TableBytes.Append(reinterpret_cast<const uint8*>(Cpu),sizeof(Cpu));
    return FFileHelper::SaveArrayToFile(Packed,*(Output/(TEXT("evidence-")+Stem+TEXT(".u32x4.zlib")))) &&
        FFileHelper::SaveArrayToFile(TableBytes,*(Output/(TEXT("reduce-")+Stem+TEXT(".u32"))));
}
#endif
