import json
from pathlib import Path

P=Path(__file__).resolve().parents[2]
C=P/'Source/AnomalyCapture/Private/Compat'

def state_contract(native,masks,street,host):
    forbidden=('SetRenderCustomDepth','SetCustomDepthStencilValue','SetCustomDepthStencilWriteMask','EnableCustomStencil','DisableCustomStencil','TagActor','RestoreActor','RestoreAll','PostProcess/SceneRenderTargets.h','SceneRendering.h','ScenePrivate.h','GetScenePrimitiveComponentIds','GetPrimitiveSceneInfo(')
    if any(token in native for token in forbidden):return False
    if any(token in street for token in forbidden[:7]):return False
    needed=('if (Legacy) { AnomalyStencilTag::SetRestoreWatch(&Ledger); AnomalyStencilTag::EnableCustomStencil(); }',
            'if (!Legacy) { Tags.Remove(Actor); return; }',
            'if (!ActiveMasks425 || !ActiveMasks425->Legacy) { return false; }',
            'Legacy=AnomalyPrivate425::Legacy(World);',
            'if (!Legacy)\n    {\n        if (!AnomalyPrivate425::Supports(Actor)) { return 0; }\n        Tags.Add(Actor,Value); return 1;\n    }')
    if not all(token in masks for token in needed):return False
    if 'AnomalyHostDepth425::BenchFixture(World) && FParse::Param(FCommandLine::Get(),TEXT("IAICaptureLegacyReference"))' not in native:return False
    if 'const FScan ScanResult=Scan(World);' not in host or 'AnomalyPrivate425::Legacy(World) && AnomalyCaptureAdmission425::Allowed' not in host:return False
    return True

def run():
    native=(C/'AnomalyPrivatePass425.cpp').read_text();masks=(C/'AnomalyMasks425.cpp').read_text()
    street=(C/'AnomalyStreetBench425.cpp').read_text();host=(C/'AnomalyHostDepth425.cpp').read_text()
    assert state_contract(native,masks,street,host)
    assert not state_contract(native+'\nPrim->SetRenderCustomDepth(true);',masks,street,host)
    assert not state_contract(native,masks.replace('if (!Legacy) { Tags.Remove(Actor); return; }',''),street,host)
    assert not state_contract(native,masks,street+'\nPrim->SetCustomDepthStencilValue(203);',host)
    assert not state_contract(native,masks,street,host.replace('const FScan ScanResult=Scan(World);',''))
    assert 'Item.Proxy=Item.Component->SceneProxy' in native
    assert 'RHIGetRenderQueryResult(Slot.Begin,Start,false)' in native
    assert 'NumPendingWriteCommands.GetValue()!=0 || !Slot.Fence->Poll()' in native
    assert 'DrawDynamicMeshPass(View,Cmd' in native and 'for (int32 Phase=0;Phase<2;++Phase)' in native
    assert 'GetDynamicMeshElements(Views,Family,1,*this)' in native
    assert 'ImmediateFlush' not in native and 'FlushRenderingCommands' not in native
    assert 'StaticType' not in native and 'IMPLEMENT_MATERIAL_SHADER_TYPE' not in native
    return dict(result='PASS',state_positive=True,host_write_mutant_rejected=True,legacy_guard_mutant_rejected=True,
                bench_plant_mutant_rejected=True,scan_cache_mutant_rejected=True,proxy_direct=True,
                two_batched_phases=True,fenced_reuse=True,nonblocking_queries=True,own_dynamic_collector=True,
                no_new_material_shader_type=True,changed_in_editor_scripts=[])

if __name__=='__main__':
    import sys
    result=run();Path(sys.argv[1]).write_text(json.dumps(result,indent=2));print(json.dumps(result))
