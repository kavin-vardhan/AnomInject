#include "AnomalyMasks425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyCaptureLog.h"
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
#include "PostProcess/SceneRenderTargets.h"
#include "SceneRendering.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"

namespace { FAnomalyMasks425* ActiveMasks425 = nullptr; }
bool FAnomalyMasks425::Start(UWorld* World)
{
    if (!World || ActiveMasks425 || Started || AnomalyStencilTag::IsAnyTagged()) { return false; }
    UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
    if (!Auto || Auto->HasCensusProvider()) { return false; }
    Owner = World; Ledger.Reset(); Ledger.HostReserved = AnomalyStencilTag::SnapshotHostReservedValues(World);
    AnomalyStencilTag::SetRestoreWatch(&Ledger);
    AnomalyStencilTag::EnableCustomStencil(); Started = true; ActiveMasks425 = this;
    TArray<TWeakObjectPtr<AActor>> Visible = AnomalyViewport::GetVisibleRenderableActors(World);
    Visible.Sort([](const TWeakObjectPtr<AActor>& A,const TWeakObjectPtr<AActor>& B) { return GetNameSafe(A.Get()) < GetNameSafe(B.Get()); });
    for (const auto& Weak : Visible)
    {
        AActor* Actor = Weak.Get(); if (!Actor) { continue; }
        uint8 Value = 0;
        for (int32 V = AnomalyStencilTag::ReservedStencilBase; V <= AnomalyStencilTag::AssignableStencilMax; ++V) { if (Ledger.IsFree(uint8(V))) { Value = uint8(V); break; } }
        if (!Value) { return false; }
        TInlineComponentArray<UPrimitiveComponent*> Prims(Actor);
        for (UPrimitiveComponent* Prim : Prims)
        {
            if (!AnomalyViewport::IsRenderableComponent(Prim) || !AnomalyViewport::IsInEventComponentScope(Prim)) { continue; }
            WriteMasks.Add(Prim,Prim->CustomDepthStencilWriteMask);
            Prim->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_Default);
        }
        if (AnomalyStencilTag::TagActor(Actor,Value) > 0) { Tags.Add(Actor,Value); Ledger.CensusClaimed.Add(Value); }
    }
    UE_LOG(LogAnomalyCapture,Log,TEXT("Capture425 C4: ledger owners=%d host_reserved=%d"),Tags.Num(),Ledger.HostReserved.Num());
    Auto->SetCensusProvider([this](const AActor* Actor)
    {
        FAnomalyCensusOpinion Opinion;
        const uint8* Tag = Tags.Find(const_cast<AActor*>(Actor));
        if (!Tag || !CensusFrame) { Opinion.Decision = EAnomalyCensusDecision::ExcludedZero; Opinion.Reason = TEXT("no_stencil_sample"); return Opinion; }
        Opinion.AgeTicks = int32(GFrameCounter-CensusFrame); Opinion.WindowTicks = MAX_int32;
        Opinion.DrawnPct = 100.0f * VisiblePixels[*Tag] / FMath::Max(1,CensusArea);
        Opinion.Decision = VisiblePixels[*Tag] ? EAnomalyCensusDecision::Eligible : EAnomalyCensusDecision::ExcludedZero;
        Opinion.Reason = VisiblePixels[*Tag] ? TEXT("stencil_depth_visible") : TEXT("zero_stencil_depth_pixels");
        return Opinion;
    },[this]() { return CensusFrame != 0; },FAnomalyCensusFireReportFn(),12);
    return Tags.Num() > 0;
}
void FAnomalyMasks425::Observe(const FAnomalyCapturedFrame& Frame)
{
    FAnomalyMasks425* Self = ActiveMasks425;
    if (!Self || !Self->Owner.IsValid() || Frame.LabelKey425.Owner != Self->Owner->GetUniqueID() || Frame.MaskData425.Num() != Frame.Width*Frame.Height*16) { return; }
    FMemory::Memzero(Self->VisiblePixels,sizeof(Self->VisiblePixels));
    const uint32* Data = reinterpret_cast<const uint32*>(Frame.MaskData425.GetData());
    for (int32 I = 0; I < Frame.Width*Frame.Height; ++I)
    {
        const uint32 Tag = Data[I*4]; float Custom,Scene;
        FMemory::Memcpy(&Custom,Data+I*4+1,4); FMemory::Memcpy(&Scene,Data+I*4+2,4);
        if (Tag < 256 && Custom > 0 && Custom >= Scene-0.000001f) { ++Self->VisiblePixels[Tag]; }
    }
    Self->CensusArea = Frame.Width*Frame.Height;
    if (!Self->CensusFrame)
    {
        for (const auto& Pair : Self->Tags) { UE_LOG(LogAnomalyCapture,Log,TEXT("Capture425 C4: census actor=%s tag=%d visible_pixels=%u"),*GetNameSafe(Pair.Key.Get()),Pair.Value,Self->VisiblePixels[Pair.Value]); }
    }
    Self->CensusFrame = Frame.LabelKey425.SampleFrame;
}
bool FAnomalyMasks425::Restore()
{
    if (!Started) { return true; }
    if (Started && Owner.IsValid()) { if (UAnomalyAutoInjectorSubsystem* Auto = Owner->GetSubsystem<UAnomalyAutoInjectorSubsystem>()) { Auto->ClearCensusProvider(); } }
    FlushRenderingCommands();
    AnomalyStencilTag::RestoreAll();
    for (const auto& Pair : WriteMasks) { if (Pair.Key.IsValid()) { Pair.Key->SetCustomDepthStencilWriteMask(Pair.Value); } }
    const bool Good = !AnomalyStencilTag::IsAnyTagged();
    AnomalyStencilTag::SetRestoreWatch(nullptr);
    if (Started) { AnomalyStencilTag::DisableCustomStencil(); }
    UE_LOG(LogAnomalyCapture,Log,TEXT("Capture425 C4: stencil_restore=%d owners=%d"),Good ? 1 : 0,Tags.Num());
    if (ActiveMasks425 == this) { ActiveMasks425 = nullptr; }
    Started = false; Tags.Empty(); WriteMasks.Empty(); Ledger.Reset();
    return Good;
}
TArray<uint8> FAnomalyMasks425::TagsFor(const FAutoLiveFireInfo& Fire) const
{
    TArray<uint8> Result;
    AActor* Actor = Fire.TargetActor.Get();
    if (!Actor) { return Result; }
    if (Fire.Id == FName(TEXT("lighting_mismatch")))
    {
        TInlineComponentArray<ULightComponent*> Lights(Actor);
        for (const auto& Pair : Tags)
        {
            if (!Pair.Key.IsValid()) { continue; }
            TInlineComponentArray<UPrimitiveComponent*> Prims(Pair.Key.Get());
            bool Affected = false;
            for (ULightComponent* Light : Lights) { for (UPrimitiveComponent* Prim : Prims) { Affected |= Light->AffectsPrimitive(Prim); } }
            if (Affected) { Result.Add(Pair.Value); }
        }
    }
    else if (const uint8* Value = Tags.Find(Actor)) { Result.Add(*Value); }
    Result.Sort(); return Result;
}
bool FAnomalyMasks425::Labelled(UWorld* World,const FAutoLiveFireInfo& Fire)
{
    UAnomalyInjectorSubsystem* Injector = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
    if (!Injector || !Fire.TargetActor.IsValid()) { return false; }
    const bool Installed = AnomalyInstall::IsInstalledByte(Injector->GetAnomalyVisualConditionState(Fire.Id));
    AnomalyActiveSource::ESource Source;
    if (!AnomalyActiveSource::Resolve(TCHAR_TO_UTF8(*Fire.Id.ToString()),Source)) { return false; }
    const bool Active = Source == AnomalyActiveSource::ESource::ActorHidden ? AnomalyHiddenClass::IsLogicallyHidden(Fire.TargetActor.Get()) : Injector->IsAnomalyCurrentlyAnomalous(Fire.Id);
    return Installed && AnomalyActiveSource::IsLabelledMember(Source,false,Active,true,Installed);
}
uint64 FAnomalyMasks425::Selection(UWorld* World)
{
    if (!ActiveMasks425 || ActiveMasks425->Owner.Get() != World) { return 0; }
    UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>();
    if (!Auto) { return 0; }
    uint64 Bits = 0;
    auto& Ledger = ActiveMasks425->Ledger;
    Ledger.EventClaimed.Reset(); Ledger.CensusClaimed.Reset();
    for (const auto& Pair : ActiveMasks425->Tags) { Ledger.CensusClaimed.Add(Pair.Value); }
    for (const auto& Fire : Auto->GetLiveFires())
    {
        const bool Emit = Labelled(World,Fire);
        for (uint8 Tag : ActiveMasks425->TagsFor(Fire))
        {
            Ledger.CensusClaimed.Remove(Tag); Ledger.EventClaimed.Add(Tag);
            if (Emit) { Bits |= uint64(1) << (Tag - AnomalyStencilTag::ReservedStencilBase); }
        }
    }
    return Bits;
}
bool FAnomalyMasks425::Render(FRHICommandListImmediate& Cmd, FSceneViewFamily& Family, uint64 Request, uint64 Selected, FAnomalyFrameCapturer& Capturer)
{
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
    if ((Frame.LabelKey425.MaskTarget != 0) != (MaskPixels != 0)) { return false; }
    if (MaskPixels)
    {
        TArray<uint8> Png;
        if (!AnomalyPreview::EncodePixels(AnomalyPreview::EImageFormat::PNG,Pixels,Frame.Width,Frame.Height,Png,0) ||
            !FFileHelper::SaveArrayToFile(Png,*(Output/(TEXT("mask-")+Stem+TEXT(".png"))))) { return false; }
    }
    int32 Size = FCompression::CompressMemoryBound(NAME_Zlib,Frame.MaskData425.Num());
    TArray<uint8> Packed; Packed.SetNumUninitialized(Size);
    if (!FCompression::CompressMemory(NAME_Zlib,Packed.GetData(),Size,Frame.MaskData425.GetData(),Frame.MaskData425.Num())) { return false; }
    Packed.SetNum(Size,false);
    TArray<uint8> TableBytes; TableBytes.Append(reinterpret_cast<const uint8*>(Cpu),sizeof(Cpu));
    return FFileHelper::SaveArrayToFile(Packed,*(Output/(TEXT("evidence-")+Stem+TEXT(".u32x4.zlib")))) &&
        FFileHelper::SaveArrayToFile(TableBytes,*(Output/(TEXT("reduce-")+Stem+TEXT(".u32"))));
}
#endif
