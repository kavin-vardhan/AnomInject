import hashlib
import json
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
engine=Path('D:/UESource/UnrealEngine/Engine')
inherited=json.loads((w.parent/'09203c/source-evidence.json').read_text())
for row in inherited:
    if 'path' in row:
        assert hashlib.sha256(Path(row['path']).read_bytes()).hexdigest()==row['sha256']
needles=[
('Source/Runtime/Engine/Public/MaterialShared.h','TArray<IAllocatedVirtualTexture*> AllocatedVTs','Public uniform-cache actual material allocations'),
('Source/Runtime/Engine/Private/Materials/MaterialShared.cpp','OutUniformExpressionCache.bUpToDate = Context.Material.IsRenderingThreadShaderMapComplete();','Completeness flag does not mean page readiness'),
('Source/Runtime/Renderer/Private/VT/VirtualTextureSystem.cpp','FAllocatedVirtualTexture*& AllocatedVT = AllocatedVTs.FindOrAdd(Desc);','Equal public descriptor retrieves and retains identical allocation'),
('Source/Runtime/Renderer/Private/VT/AllocatedVirtualTexture.cpp','TextureLayers[LayerIndex].UniquePageTableLayerIndex = UniquePhysicalSpaceIndex;','Physical group selects page-table channel'),
('Source/Runtime/Renderer/Private/VT/AllocatedVirtualTexture.cpp','if (Description.bShareDuplicateLayers)','With sharing false, each one-layer producer receives its own appended page-table channel'),
('Source/Runtime/Renderer/Private/VT/VirtualTextureSpace.cpp','PageTableIndirection.TextureReferenceRHI = RHICreateTextureReference();','Non-null indirection reference exists without an adaptive stack'),
('Source/Runtime/RenderCore/Public/RendererInterface.h','virtual IAllocatedVirtualTexture* AllocateVirtualTexture','Public allocation retention API'),
('Source/Runtime/RHI/Public/RHIResources.h','GetReferencedTexture()','Public resolution of mutable RHI texture reference'),
('Source/Runtime/RHI/Public/RHIGPUReadback.h','class RHI_API FRHIGPUBufferReadback','Public GPU buffer readback')]
records=[]
for relative,needle,claim in needles:
    path=engine/relative
    data=path.read_bytes()
    lines=[index+1 for index,line in enumerate(data.decode('utf-8-sig').splitlines()) if needle in line]
    assert lines,(relative,needle)
    records.append(dict(path=str(path),sha256=hashlib.sha256(data).hexdigest(),lines=lines,needle=needle,claim=claim))
(w/'stack-source-evidence.json').write_text(json.dumps(dict(inherited_verified=len(inherited),records=records),indent=2))
print(json.dumps(dict(inherited_verified=len(inherited),new_source_checks=len(records))))
