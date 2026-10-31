import copy
import json
from pathlib import Path
import re
import shutil
from unittest.mock import patch

import capture_c2_reader as labels
import capture_c3_kit as kit
import capture_c3_reader as masks
import capture_c3_tests
import job_scope
from fixture_policy import sha
from rotator_contract import check_source

P=Path(__file__).resolve().parents[2]
ENGINE=Path('E:/EpicGames/UE_4.25/Engine/Source/Runtime/RenderCore')

def descriptor(source):
    match=re.search(r'Create2DDesc\(FIntPoint\(1280,720\), Format, FClearValueBinding::None,\s*([^,]+),\s*([^,]+), false\)',source)
    if not match:raise ValueError('C3 factory shape differs')
    result=dict(Flags=set(re.findall(r'TexCreate_\w+',match.group(1))),TargetableFlags=set(re.findall(r'TexCreate_\w+',match.group(2))))
    if 'TexCreate_ShaderResource' not in result['TargetableFlags']:raise ValueError('4.25 RDG shader-read missing from TargetableFlags')
    if 'TexCreate_UAV' not in result['TargetableFlags']:raise ValueError('C3 UAV capability missing')
    return {k:sorted(v) for k,v in result.items()}

def run():
    source=(P/'Source/AnomalyCapture/Private/Compat/AnomalyMasks425.cpp').read_text()
    header=(ENGINE/'Public/RendererInterface.h').read_text()
    validation=(ENGINE/'Private/ShaderParameterStruct.cpp').read_text()
    assert 'NewDesc.Flags = InFlags;' in header and 'NewDesc.TargetableFlags = InTargetableFlags;' in header
    assert 'GraphTexture->Desc.TargetableFlags & TexCreate_ShaderResource' in validation
    actual=descriptor(source)
    old=source.replace('TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV','TexCreate_ShaderResource, TexCreate_UAV')
    assert old!=source
    try:descriptor(old)
    except ValueError as error:refusal=str(error)
    else:raise AssertionError('Old Flags-only descriptor escaped')
    assert source.count('Graph.CreateTexture(')==1
    assert all(('TEXT("'+name+'")') in source for name in ('AnomalyEvidence425','AnomalyMask425','AnomalyDrawn425'))
    assert 'FRDGBufferDesc::CreateBufferDesc(4,1536)' in source
    buffer=(ENGINE/'Public/RenderGraphResources.h').read_text().split('static FRDGBufferDesc CreateBufferDesc',1)[1].split('return Desc;',1)[0]
    assert 'BUF_UnorderedAccess' in buffer and 'BUF_ShaderResource' in buffer
    factory=dict(result='PASS',accepted=actual,old_refused=refusal,textures=['Evidence','Mask','Drawn'],
                 other_descriptors=dict(table='CreateBufferDesc has BUF_ShaderResource|BUF_UnorderedAccess',staging='CPUReadback only; no shader binding'),
                 source_citations=['RendererInterface.h:109-110','ShaderParameterStruct.cpp:496-503','RenderGraphResources.h:556-562'])
    (kit.J/'tests/descriptor.json').write_text(json.dumps(factory,indent=2))
    root=Path('D:/IA_Europa/Receipts/09433/c2-attempts/C2/01/plugin-frames/epoch01')
    refs=json.loads(Path('D:/IA_Europa/Receipts/09433/c2-reference-plan.json').read_text())['references']
    full=labels.read(str(root),refs)
    assert full['result']=='PASS' and full['frames']==480
    rows=[json.loads(line) for line in (root/'labels.jsonl').read_text().splitlines()][:240]
    observed,_=labels.pixel_states(str(root),rows,refs)
    native=json.loads((root/'labels-receipt.json').read_text())
    annotation=json.loads((root/'annotation.json').read_text())
    single=labels.evaluate(rows,observed,native['commands'],annotation,profile='C3')
    assert single['result']=='PASS' and single['frames']==240 and len(single['totals'])==4
    try:labels.evaluate(rows,observed,native['commands'],annotation)
    except ValueError as error:assert 'Expected 480' in str(error)
    else:raise AssertionError('Truncated C2 accepted')
    negatives=[]
    for kind in ('apply','revert','blink_toggle'):
        mutant,indices=labels.mutate(rows,observed,kind)
        try:labels.evaluate(mutant,observed,native['commands'],labels.annotation_for(mutant),profile='C3')
        except ValueError as error:
            assert 'Pixel/label mismatch' in str(error)
            negatives.append(dict(kind=kind,shifted=len(indices),result='PASS'))
        else:raise AssertionError('Label shift escaped C3 profile')
    build=json.loads((kit.J/'build-job-config.json').read_text())
    runtime=json.loads((kit.J/'c3-job-config.json').read_text())
    assert job_scope.fm1(build,kit.J) and job_scope.memory_policy_allowed(runtime,kit.J)
    bad=copy.deepcopy(build);bad['max_parallel_actions']=3
    assert not job_scope.fm1(bad,kit.J)
    bad=copy.deepcopy(runtime);bad['memory']['start_commit_GiB']=7
    assert not job_scope.memory_policy_allowed(bad,kit.J)
    job_scope.load(kit.J/'build-job-config.json');job_scope.load(kit.J/'c3-job-config.json')
    scratch=kit.J/'tests/retry-contract';scratch.mkdir(exist_ok=False)
    first=scratch/'01';first.mkdir()
    original=Path('D:/IA_Europa/Receipts/09433/c3-attempts/C3/01')
    for name in ('result.json','raw-evidence.json'):shutil.copyfile(original/name,first/name)
    config=dict(slots={'01':str(first),'02':str(scratch/'02')})
    test_path=kit.J/'tests/descriptor.json'
    proof=dict(result='PASS',diagnosis='Copied real failed C3; descriptor can-fail',quick_testable_fix=True,
               tests=[dict(path=str(test_path),size=test_path.stat().st_size,sha256=sha(test_path))])
    (scratch/'retry-qualification.json').write_text(json.dumps(proof))
    with patch.object(kit,'J',scratch):
        kit.retry_admitted(config)
        proof['quick_testable_fix']=False
        (scratch/'retry-qualification.json').write_text(json.dumps(proof))
        try:kit.retry_admitted(config)
        except RuntimeError:pass
        else:raise AssertionError('Unqualified retry escaped')
    smoke_tests=capture_c3_tests.run()
    for name in ('capture_c2_reader.py','capture_c3_kit.py','capture_fp_tests.py','capture_425_driver.py'):
        assert not check_source((P/'tools/europa'/name).read_text(),name)
    result=dict(result='PASS',descriptor=factory,c2_kept_full=dict(frames=480,mismatches=full['mismatches']),
                c2_c3_profile=dict(frames=240,mismatches=single['mismatches'],default_truncation='refused',negatives=negatives),
                mask_negatives=masks.tests()['negatives'],scoped_FM1_FJ1='PASS',qualified_retry='PASS',
                armed_argv=smoke_tests['armed_command'],keyword_rotators='PASS',changed_in_editor_scripts=[])
    (kit.J/'tests/fp-summary.json').write_text(json.dumps(result,indent=2))
    print(json.dumps(result))

if __name__=='__main__':run()
