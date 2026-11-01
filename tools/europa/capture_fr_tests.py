import json
import re
from pathlib import Path
import numpy as np
import capture_c4_kit as kit
import capture_c4_reader as reader
from capture_c3_reader import metrics,tests as metric_tests
import job_scope
from rotator_contract import check_all

P=Path(__file__).resolve().parents[2]

def violations(text):
    patterns=(r'\bEA_[A-Za-z_0-9]+',r'EuropaPhaseA',r'\b(?:1280|720)\b',r'(?:TargetTag|MaskTarget|Result|SetCustomDepthStencilValue)\s*(?:=|\()\s*(?:128|129|130|131)\b',r'Tag\s*(?:>=|<=|==)\s*(?:128|129|130|131)\b')
    return [p for p in patterns if re.search(p,text)]

def run():
    failures=[];files=0
    for p in (P/'Source').rglob('*'):
        if p.suffix not in ('.cpp','.h','.cs'):continue
        files+=1
        bad=violations(p.read_text(encoding='utf-8-sig'))
        if bad:failures.append(dict(path=str(p.relative_to(P)),patterns=bad))
    assert not failures,failures
    for text in ('ActorHasTag(TEXT("EA_BlinkCube"))','FIntPoint(1280,720)','MaskTarget = 128','Tag >= 128'):
        assert violations(text),text
    assert not violations('uint8 Green = 128; Ledger.IsFree(Value);')
    labels=(P/'Source/AnomalyCapture/Private/Compat/AnomalyCaptureLabels425.cpp').read_text()
    assert 'ApplyAnomaly(' not in labels and 'GetLiveFires()' in labels and 'SetRunning(true)' in labels
    masks=(P/'Source/AnomalyCapture/Private/Compat/AnomalyMasks425.cpp').read_text()
    for token in ('Ledger.IsFree','SnapshotHostReservedValues','Ledger.CensusClaimed','Ledger.EventClaimed','AnomalyStencilTag::TagActor','Fire.TargetActor','Light->AffectsPrimitive','Frame.Width * Frame.Height * 16'):
        assert token in masks,token
    assert 'AnomalyHiddenClass::IsLogicallyHidden(Fire.TargetActor.Get())' in masks
    assert 'Auto->SetCensusProvider' in masks and 'EAnomalyCensusDecision::ExcludedZero' in masks
    assert 'zero_stencil_depth_pixels' in masks and 'Frame.LabelKey425.Owner != Self->Owner->GetUniqueID()' in masks
    kept=Path('D:/IA_Europa/Receipts/09434/c4-attempts/C4_720/01/plugin-frames/epoch01')
    if kept.exists():
        first=json.loads((kept/'labels.jsonl').read_text().splitlines()[0])
        data=reader.raw(kept,first)
        owners=json.loads((kept/'labels-receipt.json').read_text())['stencil_owners']
        counts={o['actor']:int(reader.silhouette(data,[o['tag']]).sum()) for o in owners}
        assert counts['DefaultPawn_0']==0 and sum(v>0 for v in counts.values())==4
    rows=[dict(frame_index=i,sample_frame=100+i,anomalies=['deliberately wrong']) for i in range(10)]
    assert reader.select_reference(rows,dict(start_frame=105))['frame_index']==4
    try:reader.select_reference(rows,dict(start_frame=100))
    except ValueError:pass
    else:raise AssertionError('Missing reference admitted')
    a=np.zeros((64,96),bool);a[10:30,10:40]=True
    other=np.zeros_like(a);other[32:52,50:80]=True
    assert reader.classify(np.zeros_like(a),a)[0] is False
    assert reader.classify(a,a)[0] is True
    assert metrics(a,a|other,a|other,'lighting_mismatch')['result']=='FAIL'
    assert metrics(a|other,a|other,a|other,'lighting_mismatch')['result']=='PASS'
    data=np.zeros((64,96,4),dtype='<u4');data[:,:,1]=np.float32(1).view('u4');data[:,:,2]=np.float32(1).view('u4')
    data[:,:,0][a]=201;data[:,:,0][other]=202
    wrong_tag,wrong=reader.wrong_target(data,[200,201,202],[201])
    assert wrong_tag==202 and wrong.any()
    assert all(metrics(wrong,a,a,route)['result']=='FAIL' for route in reader.ROUTES)
    assert metrics(reader.wrong_target(data,[200,201,202],[201,202])[1],a|other,a|other,'lighting_mismatch')['result']=='FAIL'
    metric=metric_tests()
    commands=[]
    for leg,res in (('C4_720',(1280,720)),('C4_900',(1600,900))):
        argv=kit.command(leg,kit.J/'tests'/leg)
        assert '-ResX='+str(res[0]) in argv and '-ResY='+str(res[1]) in argv
        assert any('IAI.Capture.C4 ' in a and ' 1440 4242' in a and 'r.EyeAdaptationQuality 0' in a for a in argv)
        assert not any('DefaultBackBufferPixelFormat' in a for a in argv)
        commands.append(dict(leg=leg,argv=argv))
    try:kit.validate(dict(arming_job='09433c'))
    except RuntimeError:pass
    else:raise AssertionError('Old arming admitted')
    for config in ('build-job-config.json','c4-job-config.json'):job_scope.load(kit.J/config)
    result=dict(result='PASS',static=dict(files=files,violations=failures,mutants_refused=4),live_binding='PASS',reference_rule='PASS; labels ignored',completeness='PASS; proper subset refused',metrics=metric,commands=commands,keyword_rotators=check_all(),changed_in_editor_scripts=[],python27='N/A; no in-editor/game Python changed')
    (kit.J/'tests/fr-summary.json').write_text(json.dumps(result,indent=2))
    print(json.dumps({k:v for k,v in result.items() if k not in ('commands','metrics')}))

if __name__=='__main__':run()
