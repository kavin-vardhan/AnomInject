#include "AnomalyMasks425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyCaptureLog.h"
#include "AnomalyInjectorSubsystem.h"
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

bool FAnomalyMasks425::Start(UWorld* World)
{
    if (!World || Saved.Num()) { return false; }
    const TCHAR* Names[] = { TEXT("EA_BlinkCube"), TEXT("EA_LOD"), TEXT("EA_HideCube"), TEXT("EA_Floor") };
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        TInlineComponentArray<UPrimitiveComponent*> Components(*It);
        for (UPrimitiveComponent* C : Components)
        {
            if (C->bRenderCustomDepth && C->CustomDepthStencilValue >= 128 && C->CustomDepthStencilValue <= 131) { return false; }
        }
    }
    for (int32 I = 0; I < 4; ++I)
    {
        AActor* Found = nullptr;
        for (TActorIterator<AActor> It(World); It; ++It) { if (It->ActorHasTag(Names[I])) { if (Found) { return false; } Found = *It; } }
        if (!Found) { return false; }
        TInlineComponentArray<UPrimitiveComponent*> Components(Found);
        if (Components.Num() != 1) { return false; }
        FSaved Row; Row.Component = Components[0]; Row.Depth = Components[0]->bRenderCustomDepth;
        Row.Tag = Components[0]->CustomDepthStencilValue; Row.WriteMask = Components[0]->CustomDepthStencilWriteMask;
        Saved.Add(Row);
    }
    IConsoleVariable* CustomDepth = IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth"));
    if (!CustomDepth) { Saved.Empty(); return false; }
    PriorCustomDepth = CustomDepth->GetInt(); CustomDepth->Set(3, ECVF_SetByConsole);
    for (int32 I = 0; I < Saved.Num(); ++I)
    {
        UPrimitiveComponent* C = Saved[I].Component.Get();
        C->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_Default);
        C->SetCustomDepthStencilValue(128 + I); C->SetRenderCustomDepth(true);
    }
    UE_LOG(LogAnomalyCapture, Log, TEXT("Capture425 C3: stencil owners=4 reserved=128..131 custom_depth=%d prior=%d"), CustomDepth->GetInt(), PriorCustomDepth);
    return CustomDepth->GetInt() == 3;
}
bool FAnomalyMasks425::Restore()
{
    bool Good = true;
    for (const FSaved& Row : Saved)
    {
        UPrimitiveComponent* C = Row.Component.Get();
        if (!C) { Good = false; continue; }
        C->SetRenderCustomDepth(Row.Depth); C->SetCustomDepthStencilValue(Row.Tag); C->SetCustomDepthStencilWriteMask(Row.WriteMask);
        Good &= C->bRenderCustomDepth == Row.Depth && C->CustomDepthStencilValue == Row.Tag && C->CustomDepthStencilWriteMask == Row.WriteMask;
    }
    if (PriorCustomDepth >= 0)
    {
        IConsoleVariable* V = IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth"));
        V->Set(PriorCustomDepth, ECVF_SetByConsole); Good &= V->GetInt() == PriorCustomDepth;
    }
    FlushRenderingCommands();
    UE_LOG(LogAnomalyCapture, Log, TEXT("Capture425 C3: stencil_restore=%d owners=%d"), Good ? 1 : 0, Saved.Num());
    Saved.Empty(); PriorCustomDepth = -1;
    return Good;
}
uint32 FAnomalyMasks425::Selection(UWorld* World)
{
    UAnomalyInjectorSubsystem* Injector = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
    if (!Injector) { return 0; }
    uint32 Result = 0;
    for (const FActiveAnomalyInfo& Active : Injector->GetActiveAnomalies())
    {
        if (Active.Id == TEXT("blinking")) { Result = 128; }
        else if (Active.Id == TEXT("lod_corruption")) { Result = 129; }
        else if (Active.Id == TEXT("missing_object")) { Result = 130; }
        else if (Active.Id == TEXT("lighting_mismatch")) { Result = 255; }
    }
    return Result;
}
bool FAnomalyMasks425::Render(FRHICommandListImmediate& Cmd, FSceneViewFamily& Family, uint64 Request, uint32 Target, FAnomalyFrameCapturer& Capturer)
{
    if (Family.Views.Num() != 1 || !Family.Scene) { return false; }
    const FViewInfo& View = static_cast<const FViewInfo&>(*Family.Views[0]);
    if (View.UnscaledViewRect.Size() != FIntPoint(1280,720) || View.ViewRect.Size() != FIntPoint(1280,720)) { return false; }
    FSceneRenderTargets& Context = FSceneRenderTargets::Get(Cmd);
    if (!Context.bCustomDepthIsValid || !Context.CustomDepth || !Context.CustomStencilSRV.IsValid()) { return false; }
    FRDGBuilder Graph(Cmd);
    auto MakeTexture = [&](EPixelFormat Format, const TCHAR* Name)
    {
        return Graph.CreateTexture(FRDGTextureDesc::Create2DDesc(FIntPoint(1280,720), Format, FClearValueBinding::None,
            TexCreate_ShaderResource, TexCreate_UAV, false), Name);
    };
    FRDGTextureRef Evidence = MakeTexture(PF_R32G32B32A32_UINT, TEXT("AnomalyEvidence425"));
    FRDGTextureRef Mask = MakeTexture(PF_R32_UINT, TEXT("AnomalyMask425"));
    FRDGTextureRef Drawn = MakeTexture(PF_R32_UINT, TEXT("AnomalyDrawn425"));
    TShaderMapRef<FAnomalyMask425CS> Shader(GetGlobalShaderMap(Family.GetFeatureLevel()));
    auto* Params = Graph.AllocParameters<FAnomalyMask425CS::FParameters>();
    Params->SceneTextures = CreateSceneTextureUniformBuffer(Context, Family.GetFeatureLevel(), ESceneTextureSetupMode::SceneDepth | ESceneTextureSetupMode::CustomDepth, UniformBuffer_SingleDraw);
    Params->Evidence = Graph.CreateUAV(Evidence); Params->Mask = Graph.CreateUAV(Mask); Params->Drawn = Graph.CreateUAV(Drawn);
    Params->OutputSize = FIntPoint(1280,720); Params->InternalMin = View.ViewRect.Min; Params->InternalSize = View.ViewRect.Size();
    Params->TargetTag = Target; Params->DepthBias = 0.000001f;
    FComputeShaderUtils::AddPass(Graph, RDG_EVENT_NAME("AnomalyMask425"), Shader, Params, FIntVector(160,90,1));
    FRDGBufferRef Table = Graph.CreateBuffer(FRDGBufferDesc::CreateBufferDesc(4,1536), TEXT("AnomalyMaskTable425"));
    auto TableUAV = Graph.CreateUAV(Table, PF_R32_UINT);
    AddClearUAVPass(Graph, TableUAV, 0u);
    TShaderMapRef<FAnomalyMaskReduceCS> Reduce(GetGlobalShaderMap(Family.GetFeatureLevel()));
    auto* RP = Graph.AllocParameters<FAnomalyMaskReduceCS::FParameters>();
    RP->MaskTexture = Mask; RP->DrawnTexture = Drawn; RP->OutTable = TableUAV; RP->MaskSize = FIntPoint(1280,720);
    FComputeShaderUtils::AddPass(Graph, RDG_EVENT_NAME("AnomalyMaskReduce425"), Reduce, RP, FIntVector(160,90,1));
    TRefCountPtr<IPooledRenderTarget> Extracted;
    TRefCountPtr<FPooledRDGBuffer> Reduced;
    Graph.QueueTextureExtraction(Evidence, &Extracted);
    Graph.QueueBufferExtraction(Table, &Reduced, FRDGResourceState::EAccess::Read, FRDGResourceState::EPipeline::Graphics);
    Graph.Execute();
    if (!Extracted || !Reduced || !Reduced->VertexBuffer.IsValid()) { return false; }
    Capturer.CaptureMask_RenderThread(Request, Extracted->GetRenderTargetItem().ShaderResourceTexture, Reduced->VertexBuffer);
    if (Request == 1) { UE_LOG(LogAnomalyCapture, Log, TEXT("Capture425 C3: shaders_loaded=visible425,reduce local_rdg=1 output=1280x720 depth_bias=0.000001")); }
    return true;
}
bool FAnomalyMasks425::Save(const FAnomalyCapturedFrame& Frame, const FString& Output)
{
    if (Frame.MaskData425.Num() != 1280 * 720 * 16 || Frame.MaskTable425.Num() != 1536) { return false; }
    const uint32* Data = reinterpret_cast<const uint32*>(Frame.MaskData425.GetData());
    TArray<FColor> Pixels; Pixels.SetNumUninitialized(1280*720);
    uint32 Cpu[1536] = {};
    for (int32 I = 0; I < Pixels.Num(); ++I)
    {
        const uint32 M = Data[I*4+3] & 255, D = (Data[I*4+3] >> 8) & 255;
        Pixels[I] = FColor(uint8(M),uint8(M),uint8(M),255);
        if (M)
        {
            uint32* T = Cpu + M*6; ++T[0];
            T[1] = FMath::Max(T[1],~uint32(I%1280)); T[2] = FMath::Max(T[2],~uint32(I/1280));
            T[3] = FMath::Max(T[3],uint32(I%1280)); T[4] = FMath::Max(T[4],uint32(I/1280));
        }
        if (D) { ++Cpu[D*6+5]; }
    }
    if (FMemory::Memcmp(Cpu, Frame.MaskTable425.GetData(), sizeof(Cpu)) != 0) { return false; }
    const FString Stem = FString::Printf(TEXT("%06llu"),Frame.RequestId-1);
    TArray<uint8> Png;
    if (!AnomalyPreview::EncodePixels(AnomalyPreview::EImageFormat::PNG,Pixels,1280,720,Png,0) ||
        !FFileHelper::SaveArrayToFile(Png,*(Output/(TEXT("mask-")+Stem+TEXT(".png"))))) { return false; }
    int32 Size = FCompression::CompressMemoryBound(NAME_Zlib,Frame.MaskData425.Num());
    TArray<uint8> Packed; Packed.SetNumUninitialized(Size);
    if (!FCompression::CompressMemory(NAME_Zlib,Packed.GetData(),Size,Frame.MaskData425.GetData(),Frame.MaskData425.Num())) { return false; }
    Packed.SetNum(Size,false);
    TArray<uint8> TableBytes; TableBytes.Append(reinterpret_cast<const uint8*>(Cpu),sizeof(Cpu));
    return FFileHelper::SaveArrayToFile(Packed,*(Output/(TEXT("evidence-")+Stem+TEXT(".u32x4.zlib")))) &&
        FFileHelper::SaveArrayToFile(TableBytes,*(Output/(TEXT("reduce-")+Stem+TEXT(".u32"))));
}
#endif
