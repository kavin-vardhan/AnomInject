import copy
import json
from pathlib import Path
import numpy as np
import capture_office_reader as r
import capture_c5_kit as kit
from capture_c5_reader import frame_set,ROOT_KEYS,EVENT_KEYS
from capture_fr_tests import violations
from rotator_contract import check_all
import job_scope

P=Path(__file__).resolve().parents[2]
J=Path('D:/IA_Europa/Receipts/09437')

def run():
    tests=[]
    line='Capture425 SELF_CHECK stage={stage} frames={frames} viewport=GameViewportClient/FSceneViewport size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily={frames} pre_ui=1 tick=OnWorldPostActorTick/LEVELTICK_All synth=0 fixed=1 census=on batches=1 lost=0 no_pass=0 polluted=0'
    log='\n'.join(line.format(stage=s,frames=n) for s,n in [('start',1),('300',300)])
    assert r.self_checks(log)['result']=='PASS'
    for bad in (log.replace('pre_ui=1','pre_ui=0'),log.replace('lost=0','lost=1'),log.splitlines()[0]):assert r.self_checks(bad)['result']=='FAIL'
    tests.append(dict(fix='self-check',result='PASS',negative='missing 300-frame line, post-UI or lost readback refused'))
    event={k:None for k in EVENT_KEYS}
    event.update(event_id='blinking@10|actor',affected_frames=frame_set([1]),injected_frames=frame_set([1,2]),coverage_ratio=0.2,coverage_pct=17,bbox_source='projected',camera={k:None for k in ('path','global_position','near','far','rotation','fov_deg','aspect')},engine={k:None for k in ('ticks_msec','name','version','project')},affected_objects=dict(count=0,primary_index=0,nodes=[]))
    event['camera'].update(near=10,far=1000000)
    annotation={k:None for k in ROOT_KEYS};annotation.update(label_schema=2,label_schema_minor=1,anomalies=[event],video={k:None for k in ('path','frames_dir','resolution','fps','target_fps','total_frames')})
    rows=[dict(frame_index=i,camera=dict(near=10,far=0),sampled_states=[dict(event_id='blinking@10',injected=i in (1,2),affected=i==1,projected_screen_share=0.2,drawn_pixels=0)]) for i in range(4)]
    events=[dict(event_id='blinking@10',id='blinking',target_name='actor',selection_provenance=dict(coverage_pct=17,valid=True))]
    assert r.annotation_values(annotation,rows,events)['result']=='PASS'
    mutant=copy.deepcopy(annotation);mutant['anomalies'][0]['camera']['far']=0
    assert r.annotation_values(mutant,rows,events)['result']=='FAIL'
    mutant=copy.deepcopy(events);mutant[0]['selection_provenance']['coverage_pct']=float('nan')
    assert r.annotation_values(annotation,rows,mutant)['result']=='FAIL'
    mutant=copy.deepcopy(annotation);mutant['anomalies'][0]['coverage_pct']=20
    assert r.annotation_values(mutant,rows,events)['result']=='FAIL'
    tests.append(dict(fix='far plane and provenance',result='PASS',negative='zero infinite far, nonfinite or substituted coverage refused'))
    off=[dict(index=i,phase=i%7) for i in range(28)];on=[dict(index=i,phase=i%7) for i in range(28,56)]
    images={i:np.full((34,12,3),90+i%7,np.uint8) for i in range(56)}
    assert r.wall_range(off,on,images.__getitem__)['result']=='PASS'
    images[30][10:12,1:3]+=5
    bad=r.wall_range(off,on,images.__getitem__);assert bad['result']=='FAIL' and bad['max_changed_pixels']==4
    row=dict(sampled_states=[dict(affected=False)],mask=None);mask=np.zeros((4,4),np.uint8)
    assert r.mask_agrees(row,mask)[0]
    mask[1,1]=200;assert not r.mask_agrees(row,mask)[0]
    row.update(sampled_states=[dict(affected=True)],mask='mask.png');assert r.mask_agrees(row,mask)[0]
    mask[:]=0;assert not r.mask_agrees(row,mask)[0]
    tests.append(dict(fix='FU5 reader',result='PASS',negative='four changed wall pixels, clean-frame mask and missing affected mask refused'))
    c=P/'Source/AnomalyCapture/Private/Compat'
    source=(c/'AnomalyMasks425.cpp').read_text()
    observe=source.split('void FAnomalyMasks425::Observe(')[1].split('TSharedPtr<FJsonObject> FAnomalyMasks425::State')[0]
    assert 'Self->ReleaseBatch(Frame.RequestId,true); return;' in observe
    assert observe.index('EOutcome::Credit')<observe.index('Self->Verdicts.Add')
    assert 'VerifyActorStillTagged' in observe
    session=(c/'AnomalyCaptureSession425.cpp').read_text()
    assert session.index('FAnomalyMasks425::Observe(Frame)')<session.index('FAnomalyMasks425::Tick(World)')<session.index('Backend->RequestFrame(World, NextId)')
    assert 'NoPassRequests.Enqueue(Request)' in source and 'Self->ReleaseBatch(Request,true)' in source
    assert 'Queue.Insert(Pair.Key,Cursor)' in source and 'ReleaseUnused();' in source
    advance=source.split('void FAnomalyMasks425::Advance(')[1].split('void FAnomalyMasks425::ReleaseBatch')[0]
    assert 'RestoreAll' not in advance and 'Batches.Num()>=2' in advance and 'HostDepth(A)' in advance
    assert 'const uint8 Value=Existing ? *Existing' in advance
    tests.append(dict(fix='census integration',result='PASS',contract='completed readbacks before expiry; discard returns before credit; release requeues and preserves event ownership'))
    for name in ('build-job-config.json','c5-job-config.json'):job_scope.load(J/name)
    assert kit.J==J and kit.AUTHORITY.startswith('094-37')
    for old in ('09436','09435'):
        try:kit.validate(dict(arming_job=old))
        except RuntimeError:pass
        else:raise AssertionError('Spent kit accepted')
    scan=[str(p) for p in (P/'Source').rglob('*') if p.suffix in ('.cpp','.h','.cs') and violations(p.read_text(encoding='utf-8-sig'))];assert not scan,scan
    result=dict(result='PASS',tests=tests,rotators=check_all(),fixture_scan=scan,python27='N/A: no in-editor Python changed; native driver only')
    (J/'tests/fu-summary.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))

if __name__=='__main__':run()
