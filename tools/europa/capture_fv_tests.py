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
J=Path('D:/IA_Europa/Receipts/09438')

def run():
    tests=[]
    line='Capture425 SELF_CHECK stage={stage} frames={frames} viewport=SceneViewport client=GameViewportClient slate=1 pie=0 size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily={frames} pre_ui=1 tick=OnWorldPostActorTick425 tick_type=2 tick_calls={frames} fixed_step=1 fixed_dt=0.033333333 fixed_rate=0 fixed_fps=30 smoothing=0 ui_draws=8 ui_controls=3 ui_positive=3 ui_checked={frames} ui_hits=0 synth=0 census=on batches=1 lost=0 no_pass=0 polluted=0'
    log='\n'.join(line.format(stage=s,frames=n) for s,n in [('start',1),('300',300)])
    assert r.self_checks(log)['result']=='PASS'
    for bad in (log.replace('pre_ui=1','pre_ui=0'),log.replace('lost=0','lost=1'),log.splitlines()[0]):assert r.self_checks(bad)['result']=='FAIL'
    tests.append(dict(fix='self-check',result='PASS',negative='missing 300-frame line, post-UI or lost readback refused'))
    import datetime
    from runtime_lean_gate import reasons
    GIB=2**30
    sample=dict(idle_seconds=301,free_memory_bytes=6*GIB,free_commit_bytes=12*GIB,volumes={d:dict(resolved_drive=d,free_bytes=100*GIB) for d in 'CDE'})
    assert not reasons(sample,300,(12,6))
    sample['free_commit_bytes']=11.99*GIB;assert 'free_commit_bytes' in reasons(sample,300,(12,6))
    sample['free_commit_bytes']=12*GIB;sample['free_memory_bytes']=5.99*GIB;assert 'free_memory_bytes' in reasons(sample,300,(12,6))
    sample.update(free_commit_bytes=8*GIB,free_memory_bytes=4*GIB);assert not reasons(sample,300,(8,4))
    calls=[]
    def wait(path,deadline,**kw):
        calls.append(dict(path=str(path),deadline=deadline,**kw))
        return dict(status='WINDOW_ENDED' if kw['memory']==(12,6) else 'CLEAR')
    gate,profile=kit.admission_profile(wait,J,now=datetime.datetime.fromisoformat('2026-11-01T21:29:00+00:00'))
    assert profile=='fallback' and [c['memory'] for c in calls]==[(12,6),(8,4)]
    calls.clear();gate,profile=kit.admission_profile(wait,J,'street');assert profile=='street' and len(calls)==1 and gate['status']=='WINDOW_ENDED'
    calls.clear();gate,profile=kit.admission_profile(wait,J,now=datetime.datetime.fromisoformat(kit.FALLBACK));assert profile=='fallback' and len(calls)==1
    argv=['editor','/Game/ParisStreet/Levels/SampleMap'];kit.select_map(argv,'fallback');assert argv[1]=='/Game/EuropaPhaseA/EA_Core' and argv[-1]=='-IAICaptureFallback'
    tests.append(dict(fix='FV5 admission',result='PASS',negative='12/6 street boundaries; fallback only after cutoff; failed street retry cannot silently change maps'))
    a=np.zeros((64,64,3),np.uint8)
    for i,c in enumerate(r.MARKER):a[(i//4)*16:(i//4+1)*16,(i%4)*16:(i%4+1)*16]=c
    assert r.marker_cells(a,(0,0))==8
    a[:]=80;assert r.marker_cells(a,(0,0))<7
    import time
    ui=J/'tests'/('ui-pixels-'+str(time.time_ns()));ui.mkdir(exist_ok=False)
    from PIL import Image
    control=np.zeros((32,64,3),np.uint8)
    for i,c in enumerate(r.MARKER):control[(i//4)*16:(i//4+1)*16,(i%4)*16:(i%4+1)*16]=c
    Image.fromarray(control).save(ui/'ui-control-00.png')
    frame=np.full((80,100,3),80,np.uint8);Image.fromarray(frame).save(ui/'frame.png')
    native=dict(checked=1,hits=0,positive_controls=1,frames=[dict(request=1,matching_cells=0)])
    (ui/'ui-check.json').write_text(json.dumps(native))
    rr=[dict(frame_index=0,image='frame.png')];assert r.ui_pixels(ui,rr)['result']=='PASS'
    frame[16:48,16:80]=control;Image.fromarray(frame).save(ui/'frame.png')
    assert r.ui_pixels(ui,rr)['result']=='FAIL'
    frame[:]=80;Image.fromarray(frame).save(ui/'frame.png');control[:]=80;Image.fromarray(control).save(ui/'ui-control-00.png')
    assert r.ui_pixels(ui,rr)['result']=='FAIL'
    tests.append(dict(fix='measured pre-UI',result='PASS',negative='marker leakage and absent post-UI positive control both fail independent PNG test'))
    event={k:None for k in EVENT_KEYS}
    event.update(event_id='blinking@10|actor',affected_frames=frame_set([1]),injected_frames=frame_set([1,2]),coverage_ratio=0.2,coverage_pct=17,bbox_source='projected',camera={k:None for k in ('path','global_position','near','far','rotation','fov_deg','aspect')},engine={k:None for k in ('ticks_msec','name','version','project')},affected_objects=dict(count=0,primary_index=0,nodes=[]))
    event['camera'].update(near=10,far=1000000)
    annotation={k:None for k in ROOT_KEYS};annotation.update(label_schema=2,label_schema_minor=1,anomalies=[event],video={k:None for k in ('path','frames_dir','resolution','fps','target_fps','total_frames')})
    rows=[dict(frame_index=i,camera=dict(near=10,far=0),sampled_states=[dict(event_id='blinking@10',injected=i in (1,2),affected=i==1,projected_screen_share=0.2,drawn_pixels=0)]) for i in range(4)]
    events=[dict(event_id='blinking@10',id='blinking',target_name='actor',selection_provenance=dict(coverage_pct=17,valid=True))]
    assert r.annotation_values(annotation,rows,events)['result']=='PASS'
    for row in rows:row['camera']['far']=98765
    assert r.annotation_values(annotation,rows,events)['result']=='PASS'
    mutant=copy.deepcopy(annotation);mutant['anomalies'][0]['camera']['far']=98765
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
    auto=(P/'Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp').read_text(encoding='utf-8')
    args=auto.split('TArray<FString> ApplyArgs;',1)[1].split('const uint64 FireFrame',1)[0]
    assert 'GetWorstLod' not in auto and 'ApplyArgs.Add(TEXT("1"))' not in auto
    assert 'ApplyArgs.Add(Token)' in args
    caps=(P/'Source/AnomalyInjector/Private/Compat/AnomalyCapabilities.cpp').read_text()
    assert 'AnomalyBenchGate' not in caps
    defaults=auto.split('GAutoPoolDefaultEnabled[]')[1].split('};')[0]
    assert 'lighting_mismatch' not in defaults and 'lod_corruption' not in defaults
    tests.append(dict(fix='auto-injector parity',result='PASS',negative='finest-LOD override rejected; no port-only LOD filter or missing-object bench admission; original default pool retained'))
    before=np.full((4,4,3),80,np.uint8);after=before.copy();after[1:3,1:3]=130
    Image.fromarray(before).save(ui/'before.png');Image.fromarray(after).save(ui/'active.png')
    pair=[dict(frame_index=0,image='before.png',sampled_states=[]),dict(frame_index=1,image='active.png',sampled_states=[dict(event_id='lod@1',affected=True,target_tags=[200])])]
    data=np.zeros((4,4,4),np.uint32);data[1:3,1:3,3]=200
    from unittest.mock import patch
    with patch.object(r,'raw',return_value=data):
        values=r.lod_changes(ui,pair,[dict(id='lod_corruption',event_id='lod@1',target_name='unit',first_index=1,end_index=1)])
    assert values[0]['max_changed_fraction']==1 and values[0]['frames'][0]['changed_pixels']==4
    tests.append(dict(fix='LOD visible-change information',result='PASS',measurement='four known changed target pixels counted; informational only'))
    for name in ('build-job-config.json','c5-job-config.json'):job_scope.load(J/name)
    assert kit.J==J and kit.AUTHORITY.startswith('094-38')
    for old in ('09437','09436','09435'):
        try:kit.validate(dict(arming_job=old))
        except RuntimeError:pass
        else:raise AssertionError('Spent kit accepted')
    scan=[str(p) for p in (P/'Source').rglob('*') if p.suffix in ('.cpp','.h','.cs') and violations(p.read_text(encoding='utf-8-sig'))];assert not scan,scan
    result=dict(result='PASS',tests=tests,rotators=check_all(),fixture_scan=scan,python27='N/A: no in-editor Python changed; native driver only')
    (J/'tests/fv-summary.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))

if __name__=='__main__':run()
