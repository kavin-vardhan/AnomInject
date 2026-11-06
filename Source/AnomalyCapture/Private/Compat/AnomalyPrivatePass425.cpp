#include "AnomalyPrivatePass425.h"
#include "AnomalyBenchGate.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyCaptureLabels425.h"
#include "AnomalyPrivatePolicy425.h"
#include "AnomalyPrivate425Shader.h"
#include "AnomalyHostDepth425.h"
#include "AnomalyFrameCapturer.h"
#include "AnomalyCaptureLog.h"
#include "AnomalyHiddenClass.h"
#include "AnomalyViewport.h"
#include "AnomalyStencilTag.h"
#include "AnomalyMaskReduceShader.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "MaterialShared.h"
#include "MeshBatch.h"
#include "PrimitiveSceneProxy.h"
#include "StaticMeshResources.h"
#include "SceneManagement.h"
#include "SceneView.h"
#include "SceneInterface.h"
#include "MeshMaterialShader.h"
#include "MeshPassProcessor.h"
#include "MeshPassProcessor.inl"
#include "SceneRenderTargetParameters.h"
#include "DynamicBufferAllocator.h"
#include "RHIStaticStates.h"
#include "RenderUtils.h"
#include "CommonRenderResources.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "PipelineStateCache.h"
#include "RenderingThread.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeLock.h"
#include "HAL/FileManager.h"
#include "Containers/Queue.h"

namespace AnomalyPrivate425
{
namespace
{
    using EKind = AnomalyPrivatePolicy425::Kind;
    struct FStatus { FString Name,Kind,Reason; const FPrimitiveSceneProxy* Proxy=nullptr; bool Verified=false; };
    struct FFeedback { uint32 Token=0; FString Reason; };
    struct FItem
    {
        TWeakObjectPtr<UPrimitiveComponent> Component;
        const FPrimitiveSceneProxy* Proxy=nullptr;
        uint32 Token=0;
        uint8 Value=0;
        bool Hidden=false,Static=false;
        TArray<int32> Sections;
    };
    FCriticalSection Guard;
    TMap<uint64,TArray<FItem>> Pending,Bound;
    TMap<TWeakObjectPtr<UPrimitiveComponent>,uint32> Tokens;
    TMap<uint32,FStatus> Statuses;
    TQueue<FFeedback,EQueueMode::Mpsc> Feedback;
    FString Directory;
    bool Running=false;
    uint32 NextToken=0;

    bool Relevant(const UPrimitiveComponent* P,const AActor* Actor)
    {
        return P->IsA<UMeshComponent>() && AnomalyViewport::IsInEventComponentScope(P) &&
            (P->IsVisible() || AnomalyHiddenClass::IsLogicallyHidden(Actor));
    }

    void DrainFeedback()
    {
        FFeedback Row;
        while (Feedback.Dequeue(Row))
        {
            if (auto* S=Statuses.Find(Row.Token)) { S->Reason=Row.Reason; S->Verified=Row.Reason.IsEmpty(); }
        }
    }
    EKind Kind(const UPrimitiveComponent* P)
    {
        if (P->IsA<UHierarchicalInstancedStaticMeshComponent>()) { return EKind::Hierarchical; }
        if (P->IsA<UInstancedStaticMeshComponent>()) { return EKind::Instanced; }
        if (P->IsA<USkeletalMeshComponent>()) { return EKind::Skeletal; }
        if (P->IsA<UStaticMeshComponent>()) { return EKind::Static; }
        return EKind::Unsupported;
    }
    uint32 Token(UPrimitiveComponent* P)
    {
        if (const auto* Found=Tokens.Find(P)) { return *Found; }
        const uint32 Id=++NextToken; Tokens.Add(P,Id);
        FStatus S; S.Name=P->GetPathName(); S.Kind=P->GetClass()->GetName(); Statuses.Add(Id,S); return Id;
    }
    bool ComponentSupported(UPrimitiveComponent* P)
    {
        const uint32 Id=Token(P); FString Why;
        if (Statuses[Id].Proxy!=P->SceneProxy)
        {
            Statuses[Id].Proxy=P->SceneProxy; Statuses[Id].Verified=false; Statuses[Id].Reason.Empty();
        }
        const EKind Type=Kind(P);
        bool Surface=true,Translucent=false,Tessellated=false;
        UMeshComponent* Mesh=Cast<UMeshComponent>(P);
        if (Mesh)
        {
            for (int32 I=0;I<Mesh->GetNumMaterials();++I)
            {
                UMaterialInterface* MI=Mesh->GetMaterial(I);
                UMaterial* M=MI ? MI->GetMaterial() : UMaterial::GetDefaultMaterial(MD_Surface);
                Surface&=M && M->MaterialDomain==MD_Surface;
                Translucent|=M && M->GetBlendMode()!=BLEND_Opaque && M->GetBlendMode()!=BLEND_Masked;
                Tessellated|=M && M->D3D11TessellationMode!=MTM_NoTessellation;
            }
        }
        const bool Registered=P->IsRegistered() && P->SceneProxy;
        if (!AnomalyPrivatePolicy425::Supported(Type,Registered,Surface,Translucent,Tessellated))
        {
            Why=Type==EKind::Unsupported ? TEXT("component_type") : !Registered ? TEXT("no_scene_proxy") :
                !Surface ? TEXT("material_domain") : Translucent ? TEXT("translucent") : TEXT("tessellation");
            Statuses[Id].Reason=Why; Statuses[Id].Verified=false; return false;
        }
        if (Statuses[Id].Reason==TEXT("no_scene_proxy") || Statuses[Id].Reason==TEXT("material_domain") ||
            Statuses[Id].Reason==TEXT("translucent") || Statuses[Id].Reason==TEXT("tessellation")) { Statuses[Id].Reason.Empty(); }
        return Statuses[Id].Reason.IsEmpty();
    }
    TShaderRef<FMeshMaterialShader> ByName(const FMaterial& Material,FVertexFactoryType* Factory,const TCHAR* Name)
    {
        FShaderType* Type=FindShaderTypeByName(FHashedName(Name));
        FMaterialShaderMap* Map=Material.GetRenderingThreadShaderMap();
        FMeshMaterialShaderMap* MeshMap=Map ? Map->GetMeshShaderMap(Factory) : nullptr;
        FShader* Shader=Type && Type->GetMeshMaterialShaderType() && MeshMap ? MeshMap->GetShader(Type,0) : nullptr;
        return Shader ? TShaderRef<FMeshMaterialShader>(static_cast<FMeshMaterialShader*>(Shader),*Map) : TShaderRef<FMeshMaterialShader>();
    }
    using FShaders=TMeshProcessorShaders<FMeshMaterialShader,FMeshMaterialShader,FMeshMaterialShader,FMeshMaterialShader>;
    struct FResolved
    {
        const FMaterial* Material=nullptr;
        const FMaterialRenderProxy* Proxy=nullptr;
        FShaders Shaders;
        bool PositionOnly=false;
    };
    bool Resolve(const FMeshBatch& Mesh,ERHIFeatureLevel::Type Level,FResolved& R)
    {
        if (!Mesh.VertexFactory || !Mesh.MaterialRenderProxy || Mesh.Elements.Num()>64) { return false; }
        for (const auto& Element:Mesh.Elements) { if (Element.PrimitiveIdMode==PrimID_DynamicPrimitiveShaderData) { return false; } }
        const FMaterialRenderProxy* Fallback=nullptr;
        const FMaterial& Original=Mesh.MaterialRenderProxy->GetMaterialWithFallback(Level,Fallback);
        R.Proxy=Fallback ? Fallback : Mesh.MaterialRenderProxy; R.Material=&Original;
        const bool Masked=!Original.WritesEveryPixel();
        const bool Modified=Original.MaterialModifiesMeshPosition_RenderThread();
        const bool PDO=Original.HasPixelDepthOffsetConnected();
        R.PositionOnly=!Masked && !Modified && !PDO && Mesh.VertexFactory->SupportsPositionOnlyStream();
        if (!Masked && !Modified && !PDO)
        {
            R.Proxy=UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(); R.Material=R.Proxy->GetMaterial(Level);
        }
        R.Shaders.VertexShader=ByName(*R.Material,Mesh.VertexFactory->GetType(),R.PositionOnly ? TEXT("TDepthOnlyVS<true>") : TEXT("TDepthOnlyVS<false>"));
        if (Masked || PDO)
        {
            R.Shaders.PixelShader=ByName(*R.Material,Mesh.VertexFactory->GetType(),TEXT("FDepthOnlyPS<false>"));
            if (!R.Shaders.PixelShader.IsValid()) { return false; }
        }
        return R.Shaders.VertexShader.IsValid();
    }
    class FProcessor : public FMeshPassProcessor
    {
    public:
        FProcessor(const FScene* Scene,const FSceneView& View,FRHIUniformBuffer* Pass,FMeshPassDrawListContext* Context,bool Hidden)
            : FMeshPassProcessor(Scene,View.GetFeatureLevel(),&View,Context),State(View,Pass)
        {
            State.SetBlendState(TStaticBlendState<CW_NONE>::GetRHI());
            State.SetDepthStencilState(Hidden ?
                TStaticDepthStencilState<true,CF_DepthNearOrEqual,true,CF_Always,SO_Keep,SO_Keep,SO_Replace,false,CF_Always,SO_Keep,SO_Keep,SO_Keep,255,255>::GetRHI() :
                TStaticDepthStencilState<false,CF_Equal,true,CF_Always,SO_Keep,SO_Keep,SO_Replace,false,CF_Always,SO_Keep,SO_Keep,SO_Keep,255,255>::GetRHI());
            State.SetDepthStencilAccess(Hidden ? FExclusiveDepthStencil::DepthWrite_StencilWrite : FExclusiveDepthStencil::DepthRead_StencilWrite);
        }
        void SetId(uint8 Id) { State.SetStencilRef(Id); }
        virtual void AddMeshBatch(const FMeshBatch& Mesh,uint64 Elements,const FPrimitiveSceneProxy* Proxy,int32 StaticMeshId=-1) override
        {
            FResolved R; if (!Resolve(Mesh,FeatureLevel,R)) { return; }
            const FMaterialRenderProxy* Unused=nullptr;
            const FMaterial& Original=Mesh.MaterialRenderProxy->GetMaterialWithFallback(FeatureLevel,Unused);
            const auto Overrides=ComputeMeshOverrideSettings(Mesh);
            FMeshMaterialShaderElementData Element; Element.InitializeMeshMaterialData(ViewIfDynamicMeshCommand,Proxy,Mesh,StaticMeshId,false);
            BuildMeshDrawCommands(Mesh,Elements,Proxy,*R.Proxy,*R.Material,State,R.Shaders,
                ComputeMeshFillMode(Mesh,Original,Overrides),ComputeMeshCullMode(Mesh,Original,Overrides),
                FMeshDrawCommandSortKey::Default,R.PositionOnly ? EMeshPassFeatures::PositionOnly : EMeshPassFeatures::Default,Element);
        }
    private:
        FMeshPassProcessorRenderState State;
    };
    class FCollector : public FMeshElementCollector
    {
    public:
        TArray<FMeshBatchAndRelevance,SceneRenderingAllocator> Meshes;
        FSimpleElementCollector Simple;
        TArray<FPrimitiveUniformShaderParameters> PrimitiveData;
        FGlobalDynamicIndexBuffer Indices;
        FGlobalDynamicVertexBuffer Vertices;
        FGlobalDynamicReadBuffer Read;
        explicit FCollector(const FSceneView& View):FMeshElementCollector(View.GetFeatureLevel())
        {
            Read.InitResource();
            AddViewMeshArrays(const_cast<FSceneView*>(&View),&Meshes,&Simple,&PrimitiveData,View.GetFeatureLevel(),&Indices,&Vertices,&Read);
        }
        ~FCollector() { Read.ReleaseResource(); }
        void Gather(const FItem& Item,const FSceneView& View,const FSceneViewFamily& Family)
        {
            SetPrimitive(Item.Proxy,FHitProxyId());
            if (Item.Static)
            {
                const auto* Proxy=static_cast<const FStaticMeshSceneProxy*>(Item.Proxy);
                const int32 LOD=Proxy->GetLOD(&View);
                if (!Item.Sections.IsValidIndex(LOD)) { return; }
                for (int32 Batch=0;Batch<Proxy->GetNumMeshBatches();++Batch)
                {
                    for (int32 Section=0;Section<Item.Sections[LOD];++Section)
                    {
                        FMeshBatch& Mesh=AllocateMesh();
                        if (Proxy->GetMeshElement(LOD,Batch,Section,SDPG_World,false,false,Mesh)) { AddMesh(0,Mesh); }
                    }
                }
            }
            else
            {
                TArray<const FSceneView*> ViewList; ViewList.Add(&View);
                Item.Proxy->GetDynamicMeshElements(ViewList,Family,1,*this);
            }
            ProcessTasks();
        }
        void Commit() { Indices.Commit(); Vertices.Commit(); Read.Commit(); }
    };
    struct FDraw { const FMeshBatch* Mesh=nullptr; const FPrimitiveSceneProxy* Proxy=nullptr; uint8 Id=0; bool Hidden=false; uint32 Token=0; };
    struct FSlot
    {
        FIntPoint Size=FIntPoint::ZeroValue;
        FTexture2DRHIRef Depth;
        FShaderResourceViewRHIRef Ids;
        TRefCountPtr<IPooledRenderTarget> Evidence,Mask,Drawn;
        TRefCountPtr<FPooledRDGBuffer> Table;
        FGPUFenceRHIRef Fence;
        FRenderQueryRHIRef Begin,End;
        TUniquePtr<FCollector> Collector;
        uint64 Request=0;
        bool Busy=false,Timed=false;
        double SubmitMs=0;
        int32 Draws=0,Objects=0;
    };
    TArray<TUniquePtr<FSlot>> Slots;
    FString RenderDirectory;
    bool Measure=false;
    void NewTargets(FSlot& Slot,FIntPoint Size)
    {
        Slot.Size=Size;
        FRHIResourceCreateInfo Info(FClearValueBinding::DepthFar); Info.DebugName=TEXT("AnomalyPrivateIdDepth425");
        Slot.Depth=RHICreateTexture2D(Size.X,Size.Y,PF_DepthStencil,1,1,TexCreate_DepthStencilTargetable|TexCreate_ShaderResource,Info);
        Slot.Ids=RHICreateShaderResourceView(Slot.Depth,0,1,PF_X24_G8);
        Slot.Fence=RHICreateGPUFence(TEXT("AnomalyPrivateReuse425"));
        if (Measure) { Slot.Begin=RHICreateRenderQuery(RQT_AbsoluteTime); Slot.End=RHICreateRenderQuery(RQT_AbsoluteTime); }
    }
}

bool Legacy(UWorld* World)
{
    return AnomalyHostDepth425::BenchFixture(World) && FParse::Param(FCommandLine::Get(),TEXT("IAICaptureLegacyReference"));
}
bool Available(UWorld* World)
{
    return World && World->Scene && World->Scene->GetFeatureLevel()>=ERHIFeatureLevel::SM5 && !Legacy(World) &&
        FindShaderTypeByName(FHashedName(TEXT("TDepthOnlyVS<false>"))) && FindShaderTypeByName(FHashedName(TEXT("FDepthOnlyPS<false>")));
}
bool Active() { return Running; }
void Start(UWorld* World,const FString& Output)
{
    Running=Available(World); Directory=Output; Tokens.Reset(); Statuses.Reset(); NextToken=0;
    FFeedback Discard; while (Feedback.Dequeue(Discard)) {}
    if (Running)
    {
        for (TActorIterator<AActor> It(World);It;++It)
        {
            TInlineComponentArray<UPrimitiveComponent*> Parts(*It);
            for (UPrimitiveComponent* P:Parts) { if (Relevant(P,*It)) { ComponentSupported(P); } }
        }
    }
    { FScopeLock Lock(&Guard); Pending.Reset(); Bound.Reset(); }
    const bool Costs=AnomalyBenchGate::IsCapture425Enabled() && AnomalyCaptureC2::Evidence();
    ENQUEUE_RENDER_COMMAND(AnomalyPrivateBegin425)([Output,Costs](FRHICommandListImmediate& Cmd)
    { Poll_RenderThread(Cmd); Slots.Reset(); RenderDirectory=Output; Measure=Costs; });
}
void Stop()
{
    Running=false; { FScopeLock Lock(&Guard); Pending.Reset(); }
    ENQUEUE_RENDER_COMMAND(AnomalyPrivateEnd425)([](FRHICommandListImmediate& Cmd)
    { Poll_RenderThread(Cmd); Slots.Reset(); FScopeLock Lock(&Guard); Bound.Reset(); });
}
bool Supports(const AActor* Actor)
{
    if (!Actor) { return false; } DrainFeedback();
    TInlineComponentArray<UPrimitiveComponent*> Parts(Actor); bool Any=false,All=true;
    for (UPrimitiveComponent* P:Parts)
    {
        if (!Relevant(P,Actor)) { continue; }
        Any=true; All&=ComponentSupported(P);
    }
    return Any && All;
}
bool Verified(const AActor* Actor)
{
    if (!Supports(Actor)) { return false; }
    TInlineComponentArray<UPrimitiveComponent*> Parts(Actor);
    for (UPrimitiveComponent* P:Parts)
    {
        if (Relevant(P,Actor) && !Statuses[Token(P)].Verified) { return false; }
    }
    return true;
}
TSharedPtr<FJsonObject> State(bool Full)
{
    DrainFeedback(); auto R=MakeShared<FJsonObject>(); TArray<TSharedPtr<FJsonValue>> Rows; int32 Unavailable=0,Ready=0;
    for (const auto& Pair:Statuses)
    {
        const auto& S=Pair.Value; auto O=MakeShared<FJsonObject>(); O->SetStringField(TEXT("component"),S.Name);
        O->SetStringField(TEXT("type"),S.Kind); O->SetStringField(TEXT("reason"),S.Reason);
        O->SetBoolField(TEXT("verified"),S.Verified); O->SetBoolField(TEXT("unavailable"),!S.Reason.IsEmpty());
        Rows.Add(MakeShared<FJsonValueObject>(O)); Unavailable+=S.Reason.IsEmpty()?0:1; Ready+=S.Verified?1:0;
    }
    R->SetBoolField(TEXT("enabled"),Running); R->SetNumberField(TEXT("unavailable"),Unavailable); R->SetNumberField(TEXT("verified"),Ready);
    if (Full) { R->SetArrayField(TEXT("components"),Rows); }
    R->SetNumberField(TEXT("host_stencil_writes"),0);
    R->SetStringField(TEXT("id_storage"),TEXT("plugin_owned_depth_stencil_attachment")); return R;
}
void Prepare(uint64 Request,const TMap<TWeakObjectPtr<AActor>,uint8>& Actors)
{
    if (!Running) { return; }
    TArray<FItem> Items;
    for (const auto& Pair:Actors)
    {
        if (!Pair.Key.IsValid() || !Supports(Pair.Key.Get())) { continue; }
        TInlineComponentArray<UPrimitiveComponent*> Parts(Pair.Key.Get());
        for (UPrimitiveComponent* P:Parts)
        {
            if (!Relevant(P,Pair.Key.Get())) { continue; }
            FItem Item; Item.Component=P; Item.Token=Token(P); Item.Value=Pair.Value;
            Item.Hidden=AnomalyHiddenClass::IsLogicallyHidden(Pair.Key.Get()); Item.Static=P->GetClass()==UStaticMeshComponent::StaticClass();
            if (Item.Static)
            {
                const UStaticMeshComponent* Static=CastChecked<UStaticMeshComponent>(P);
                if (!Static->GetStaticMesh() || !Static->GetStaticMesh()->RenderData) { continue; }
                for (const auto& LOD:Static->GetStaticMesh()->RenderData->LODResources) { Item.Sections.Add(LOD.Sections.Num()); }
            }
            Items.Add(MoveTemp(Item));
        }
    }
    FScopeLock Lock(&Guard); Pending.Add(Request,MoveTemp(Items));
}
void BindView(uint64 Request)
{
    if (!Running) { return; } TArray<FItem> Items;
    { FScopeLock Lock(&Guard); if (auto* Found=Pending.Find(Request)) { Items=MoveTemp(*Found); Pending.Remove(Request); } }
    for (FItem& Item:Items) { if (Item.Component.IsValid()) { Item.Proxy=Item.Component->SceneProxy; } }
    ENQUEUE_RENDER_COMMAND(AnomalyPrivateBind425)([Request,Items=MoveTemp(Items)](FRHICommandListImmediate&) mutable
    { FScopeLock Lock(&Guard); Bound.Add(Request,MoveTemp(Items)); });
}
void Poll_RenderThread(FRHICommandListImmediate& Cmd)
{
    for (auto& Owned:Slots)
    {
        FSlot& Slot=*Owned; if (!Slot.Busy || Slot.Fence->NumPendingWriteCommands.GetValue()!=0 || !Slot.Fence->Poll()) { continue; }
        if (Slot.Timed)
        {
            uint64 Start=0,End=0;
            if (!RHIGetRenderQueryResult(Slot.Begin,Start,false) || !RHIGetRenderQueryResult(Slot.End,End,false)) { continue; }
            const double GPU=End>=Start ? double(End-Start)/1000.0 : -1;
            const FString Line=FString::Printf(TEXT("{\"request\":%llu,\"objects\":%d,\"draws\":%d,\"cpu_submit_ms\":%.6f,\"gpu_ms\":%.6f,\"blocking_wait\":false,\"host_depth_writes\":0,\"host_stencil_writes\":0}\n"),Slot.Request,Slot.Objects,Slot.Draws,Slot.SubmitMs,GPU);
            FFileHelper::SaveStringToFile(Line,*(RenderDirectory/TEXT("private-pass.jsonl")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM,&IFileManager::Get(),FILEWRITE_Append);
        }
        Slot.Busy=false; Slot.Collector.Reset();
    }
}
bool Render(FRHICommandListImmediate& Cmd,FSceneViewFamily& Family,uint64 Request,uint64 Selected,FAnomalyFrameCapturer& Capturer)
{
    if (Family.Views.Num()!=1 || !Family.Scene) { return false; }
    const FSceneView& View=*Family.Views[0]; const FIntPoint Size=View.UnscaledViewRect.Size();
    if (Size.X<=0 || Size.Y<=0) { return false; }
    Poll_RenderThread(Cmd);
    TArray<FItem> Items;
    { FScopeLock Lock(&Guard); auto* Found=Bound.Find(Request); if (!Found) { return false; } Items=MoveTemp(*Found); Bound.Remove(Request); }
    FSlot* Slot=nullptr;
    for (auto& Candidate:Slots) { if (!Candidate->Busy && Candidate->Size==Size) { Slot=Candidate.Get(); break; } }
    if (!Slot)
    {
        for (auto& Candidate:Slots)
        {
            if (!Candidate->Busy) { Candidate=MakeUnique<FSlot>(); NewTargets(*Candidate,Size); Slot=Candidate.Get(); break; }
        }
        if (!Slot)
        {
            if (Slots.Num()>=8) { UE_LOG(LogAnomalyCapture,Error,TEXT("Private425: no fenced slot available request=%llu"),Request); return false; }
            auto Added=MakeUnique<FSlot>(); NewTargets(*Added,Size); Slot=Added.Get(); Slots.Add(MoveTemp(Added));
        }
    }
    const double Began=FPlatformTime::Seconds(); Slot->Collector=MakeUnique<FCollector>(View); auto& Collector=*Slot->Collector;
    TArray<FDraw> Draws; TSet<uint8> InvalidIds; uint64 Hidden=0;
    for (const FItem& Item:Items)
    {
        if (!Item.Proxy) { InvalidIds.Add(Item.Value); Feedback.Enqueue({Item.Token,TEXT("no_bound_proxy")}); continue; }
        const int32 First=Collector.Meshes.Num(); Collector.Gather(Item,View,Family); bool Good=true;
        for (int32 I=First;I<Collector.Meshes.Num();++I)
        {
            const auto& Batch=Collector.Meshes[I];
            if (!Batch.GetHasOpaqueOrMaskedMaterial()) { Good=false; continue; }
            FResolved Check; if (!Resolve(*Batch.Mesh,View.GetFeatureLevel(),Check)) { Good=false; continue; }
            FDraw D; D.Mesh=Batch.Mesh; D.Proxy=Item.Proxy; D.Id=Item.Value; D.Hidden=Item.Hidden; D.Token=Item.Token; Draws.Add(D);
        }
        Feedback.Enqueue({Item.Token,Good ? FString() : FString(TEXT("shader_or_dynamic_binding"))});
        if (!Good) { InvalidIds.Add(Item.Value); }
        if (Item.Hidden) { Hidden|=uint64(1)<<(Item.Value-AnomalyStencilTag::ReservedStencilBase); }
    }
    for (uint8 Id:InvalidIds) { if (Selected & (uint64(1)<<(Id-AnomalyStencilTag::ReservedStencilBase))) { return false; } }
    Collector.Commit();
    if (Measure) { Cmd.EndRenderQuery(Slot->Begin); }
    const auto Uniform=CreateSceneTextureUniformBufferSingleDraw(Cmd,ESceneTextureSetupMode::SceneDepth,Family.GetFeatureLevel());
    Cmd.TransitionResource(EResourceTransitionAccess::EWritable,Slot->Depth);
    const auto Actions=MakeDepthStencilTargetActions(ERenderTargetActions::Clear_Store,ERenderTargetActions::Clear_Store);
    FRHIRenderPassInfo Pass(Slot->Depth,Actions,nullptr,FExclusiveDepthStencil::DepthWrite_StencilWrite);
    Cmd.BeginRenderPass(Pass,TEXT("AnomalyPrivateIds425")); Cmd.SetViewport(0,0,0,Size.X,Size.Y,1);
    TShaderMapRef<FAnomalyPrivateDepth425VS> VS(GetGlobalShaderMap(Family.GetFeatureLevel()));
    TShaderMapRef<FAnomalyPrivateDepth425PS> PS(GetGlobalShaderMap(Family.GetFeatureLevel()));
    FGraphicsPipelineStateInitializer Pipeline; Cmd.ApplyCachedRenderTargets(Pipeline);
    Pipeline.BlendState=TStaticBlendState<CW_NONE>::GetRHI(); Pipeline.RasterizerState=TStaticRasterizerState<FM_Solid,CM_None>::GetRHI();
    Pipeline.DepthStencilState=TStaticDepthStencilState<true,CF_Always>::GetRHI(); Pipeline.PrimitiveType=PT_TriangleList;
    Pipeline.BoundShaderState.VertexDeclarationRHI=GEmptyVertexDeclaration.VertexDeclarationRHI;
    Pipeline.BoundShaderState.VertexShaderRHI=VS.GetVertexShader(); Pipeline.BoundShaderState.PixelShaderRHI=PS.GetPixelShader();
    SetGraphicsPipelineState(Cmd,Pipeline);
    FAnomalyPrivateDepth425PS::FParameters Copy; Copy.View=View.ViewUniformBuffer; Copy.SceneTextures=Uniform; Copy.OutputSize=Size;
    SetShaderParameters(Cmd,PS,PS.GetPixelShader(),Copy); Cmd.DrawPrimitive(0,1,1);
    int32 Submitted=0;
    for (int32 Phase=0;Phase<2;++Phase)
    {
        DrawDynamicMeshPass(View,Cmd,[&](FMeshPassDrawListContext* Context)
        {
            FProcessor Processor(Family.Scene->GetRenderScene(),View,Uniform,Context,Phase!=0);
            for (const FDraw& D:Draws)
            {
                if (InvalidIds.Contains(D.Id) || D.Hidden!=(Phase!=0)) { continue; }
                if (D.Hidden && !(Selected & (uint64(1)<<(D.Id-AnomalyStencilTag::ReservedStencilBase)))) { continue; }
                Processor.SetId(D.Id); Processor.AddMeshBatch(*D.Mesh,~uint64(0),D.Proxy); ++Submitted;
            }
        });
    }
    Cmd.EndRenderPass(); Cmd.TransitionResource(EResourceTransitionAccess::EReadable,Slot->Depth);
    FRDGBuilder Graph(Cmd);
    auto Plane=[&](TRefCountPtr<IPooledRenderTarget>& Old,EPixelFormat Format,const TCHAR* Name)
    {
        return Old ? Graph.RegisterExternalTexture(Old,Name) : Graph.CreateTexture(FRDGTextureDesc::Create2DDesc(Size,Format,FClearValueBinding::None,TexCreate_None,TexCreate_ShaderResource|TexCreate_UAV,false),Name);
    };
    auto Evidence=Plane(Slot->Evidence,PF_R32G32B32A32_UINT,TEXT("AnomalyPrivateEvidence"));
    auto Mask=Plane(Slot->Mask,PF_R32_UINT,TEXT("AnomalyPrivateMask")); auto Drawn=Plane(Slot->Drawn,PF_R32_UINT,TEXT("AnomalyPrivateDrawn"));
    TShaderMapRef<FAnomalyPrivateIds425CS> Shader(GetGlobalShaderMap(Family.GetFeatureLevel()));
    auto* Params=Graph.AllocParameters<FAnomalyPrivateIds425CS::FParameters>();
    Params->View=View.ViewUniformBuffer; Params->SceneTextures=Uniform; Params->PrivateDepth=Slot->Depth; Params->PrivateIds=Slot->Ids;
    Params->Evidence=Graph.CreateUAV(Evidence); Params->Mask=Graph.CreateUAV(Mask); Params->Drawn=Graph.CreateUAV(Drawn); Params->OutputSize=Size;
    Params->SelectedLo=uint32(Selected); Params->SelectedHi=uint32(Selected>>32); Params->HiddenLo=uint32(Hidden); Params->HiddenHi=uint32(Hidden>>32);
    Params->BaseTag=AnomalyStencilTag::ReservedStencilBase; Params->DepthBias=AnomalyPrivatePolicy425::DepthTolerance;
    const FIntVector Groups(FMath::DivideAndRoundUp(Size.X,8),FMath::DivideAndRoundUp(Size.Y,8),1);
    FComputeShaderUtils::AddPass(Graph,RDG_EVENT_NAME("AnomalyPrivateAdmission425"),Shader,Params,Groups);
    FRDGBufferDesc Desc=FRDGBufferDesc::CreateBufferDesc(4,1536); Desc.Usage=EBufferUsageFlags(Desc.Usage|BUF_SourceCopy);
    auto Table=Slot->Table ? Graph.RegisterExternalBuffer(Slot->Table,TEXT("AnomalyPrivateTable")) : Graph.CreateBuffer(Desc,TEXT("AnomalyPrivateTable"));
    auto UAV=Graph.CreateUAV(Table,PF_R32_UINT); AddClearUAVPass(Graph,UAV,0u);
    TShaderMapRef<FAnomalyMaskReduceCS> Reduce(GetGlobalShaderMap(Family.GetFeatureLevel())); auto* RP=Graph.AllocParameters<FAnomalyMaskReduceCS::FParameters>();
    RP->MaskTexture=Mask; RP->DrawnTexture=Drawn; RP->OutTable=UAV; RP->MaskSize=Size;
    FComputeShaderUtils::AddPass(Graph,RDG_EVENT_NAME("AnomalyPrivateReduce425"),Reduce,RP,Groups);
    Graph.QueueTextureExtraction(Evidence,&Slot->Evidence); Graph.QueueTextureExtraction(Mask,&Slot->Mask); Graph.QueueTextureExtraction(Drawn,&Slot->Drawn);
    Graph.QueueBufferExtraction(Table,&Slot->Table,FRDGResourceState::EAccess::Read,FRDGResourceState::EPipeline::Graphics); Graph.Execute();
    if (Measure) { Cmd.EndRenderQuery(Slot->End); }
    Capturer.CaptureMask_RenderThread(Request,Slot->Evidence->GetRenderTargetItem().ShaderResourceTexture,Slot->Table->VertexBuffer);
    Slot->Fence->Clear(); Cmd.WriteGPUFence(Slot->Fence); Slot->Busy=true; Slot->Timed=Measure;
    Slot->Request=Request; Slot->SubmitMs=(FPlatformTime::Seconds()-Began)*1000; Slot->Draws=Submitted; Slot->Objects=Items.Num();
    return true;
}
}
#endif
