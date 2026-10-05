import hashlib
import json
from pathlib import Path

work = Path('E:/IA_BuildCache/_r921/09203c')
engine = Path('D:/UESource/UnrealEngine/Engine')
records = json.loads((work.parent / 'source-evidence.json').read_text())
for record in records:
    if 'path' in record:
        assert hashlib.sha256(Path(record['path']).read_bytes()).hexdigest() == record['sha256']
evidence = [
    ('Source/Runtime/Renderer/Private/VT/VirtualTextureSpace.cpp', 'const uint32 vAddress = Allocator.Alloc(VirtualTexture);', 'VT allocation reserves a virtual region; this operation does not clear its GPU page-table bytes'),
    ('Source/Runtime/Renderer/Private/VT/VirtualTextureSpace.cpp', 'Allocator.Free(VirtualTexture);', 'Released virtual regions can be reused without a GPU clear in FreeVirtualTexture'),
    ('Source/Runtime/RenderCore/Public/VirtualTexturing.h', 'inline const FVirtualTextureProducerHandle& GetProducerHandle(uint32 InLayerIndex)', 'Public allocated-VT producer ownership can be checked before initializing its region'),
    ('Source/Runtime/RenderCore/Public/VirtualTexturing.h', 'inline uint32 GetVirtualPageX() const', 'Public allocated-VT coordinates scope page-table initialization and readback'),
    ('Source/Runtime/RHI/Public/RHIResources.h', 'struct FRHIRenderPassInfo', 'Public RHI render pass permits zero initialization of a temporary integer render target'),
    ('Source/Runtime/RHI/Public/RHI.h', 'struct FRHICopyTextureInfo', 'Public RHI copy specifies destination mip, position and extent for the private region'),
    ('Source/Runtime/RenderCore/Public/RendererInterface.h', 'virtual void RequestVirtualTextureTilesForRegion', 'Public request is scoped to an allocated VT'),
    ('Source/Runtime/RenderCore/Public/RendererInterface.h', 'virtual void LoadPendingVirtualTextureTiles', 'Public render-thread loader'),
    ('Source/Runtime/Renderer/Private/SceneRendering.cpp', 'FVirtualTextureSystem::Get().LoadPendingTiles(GraphBuilder, FeatureLevel);', 'Loader executes an RDG graph'),
    ('Source/Runtime/Renderer/Private/VT/VirtualTextureSystem.cpp', 'SubmitRequests(GraphBuilder, FeatureLevel, Allocator, Settings, RequestList, false);', 'Explicit requested tiles use synchronous production'),
    ('Source/Runtime/Renderer/Private/VT/VirtualTextureSystem.cpp', 'bForceProduceTile = !bAsync', 'Synchronous request waits for pending producer tasks but can still saturate'),
    ('Source/Runtime/Renderer/Private/VT/VirtualTextureSystem.cpp', 'Spaces[ID]->ApplyUpdates(this, GraphBuilder);', 'Submit applies page-table updates'),
    ('Source/Runtime/Engine/Private/Texture2D.cpp', 'VTDesc.ProducerHandle[LayerIndex] = ProducerHandle;', 'Acquire allocation references the private resource producer'),
    ('Source/Runtime/RHI/Public/RHIGPUReadback.h', 'void* Lock(int32& OutRowPitchInPixels', 'Readback lock exposes actual row pitch'),
    ('Source/Runtime/RHI/Private/RHIGPUReadback.cpp', 'RHICmdList.WriteGPUFence(Fence);', 'Readback readiness is a GPU fence after copying'),
    ('Shaders/Private/VirtualTextureCommon.ush', 'return (PackedPageTableValue >> 4) != 0;', 'VT shader independently defines a valid physical page'),
    ('Shaders/Private/VirtualTextureCommon.ush', 'const uint vLevel = PackedPageTableValue & 0xf;', 'VT shader decodes actual mapped level from low four bits'),
    ('Source/Runtime/Renderer/Private/VT/VirtualTextureSpace.cpp', 'case 1u: return bUse16Bits ? PF_R16_UINT : PF_R32_UINT;', 'Page table integer formats distinguish 16-bit and 32-bit entries'),
    ('Source/Runtime/Engine/Public/MaterialShared.h', 'GetVTStacks() const', 'Compiled virtual stack descriptions are public; the protected GetVTStackAndLayerIndex helper is not used'),
    ('Source/Runtime/Engine/Public/MaterialShared.h', 'LayerUniformExpressionIndices', 'Compiled stack layers identify each named texture expression'),
    ('Shaders/Private/VirtualTextureCommon.ush', 'const uint pPageX', 'Physical page X is decoded using the selected page-table format'),
    ('Shaders/Private/VirtualTextureCommon.ush', 'const uint pPageY', 'Physical page Y is decoded using the selected page-table format'),
    ('Source/Editor/UnrealEd/Public/EditorViewportClient.h', 'FViewportStateGetter VisibilityDelegate;', 'Public visibility delegate can temporarily suppress an editor viewport in a bench process'),
    ('Source/Editor/UnrealEd/Private/EditorViewportClient.cpp', 'bIsVisible = VisibilityDelegate.Execute();', 'Visibility is read through the delegate'),
    ('Source/Editor/UnrealEd/Private/EditorEngine.cpp', 'ViewportClient->IsVisible()', 'Editor viewport draws are conditioned on visibility'),
    ('Source/Editor/UnrealEd/Public/Editor.h', 'static FOnPIEEvent EndPIE;', 'Editor end-play delegate restores temporary bench viewport state'),
    ('Source/Runtime/Engine/Private/Slate/SceneViewport.cpp', 'void FSceneViewport::SetFixedViewportSize', 'Public native viewport sizing is an editor fixture control'),
    ('Source/Runtime/Engine/Private/GameEngine.cpp', 'ForceRes', 'Packaged game resolution can be kept above the desktop work-area limit with ForceRes'),
    ('Source/Runtime/Engine/Private/UnrealEngine.cpp', 'FParse::Command(&Cmd,TEXT("DEFER"))', 'DEFER is an engine console command used only by the staged harness'),
    ('Source/Runtime/Engine/Private/UnrealEngine.cpp', 'const int32 DeferredCommandsCount = DeferredCommands.Num();', 'Deferred-command tick snapshots queue length before executing; nested DEFER advances through later ticks'),
]
for relative, needle, claim in evidence:
    path = engine / relative
    data = path.read_bytes()
    lines = data.decode('utf-8-sig').splitlines()
    matches = [i + 1 for i, line in enumerate(lines) if needle in line]
    assert matches, needle
    records.append(dict(path=str(path), sha256=hashlib.sha256(data).hexdigest(), lines=matches, needle=needle, claim=claim, evidence_kind='source; runtime must prove identity'))
(work / 'source-evidence.json').write_text(json.dumps(records, indent=2), encoding='utf-8')
print(json.dumps(dict(inherited_hashes_verified=True, new_source_checks=len(evidence))))
