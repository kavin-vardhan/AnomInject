import copy
import json
import re
from pathlib import Path
import numpy as np
import capture_c5_reader as reader
import capture_c5_kit as kit
from capture_fr_tests import violations
from capture_c3_reader import tests as mask_tests
from rotator_contract import check_all
import job_scope

def run():
    p=Path(__file__).resolve().parents[2];t=p/'Source/AnomalyCapture/Private/Compat'
    base=np.full((20,20,3),100,dtype=np.uint8);grain=base.copy();grain[:,:,0]+=1
    assert not reader.stable_map([base,grain])[0].any()
    edge=base.copy();edge.reshape(-1,3)[:20]+=2
    unstable,keep=reader.stable_map([base,edge]);assert keep and int(unstable.sum())==20
    edge.reshape(-1,3)[20]+=2
    assert not reader.stable_map([base,edge])[1]
    camera=dict(player_location=[1,2,3],view_location=[1,2,3],player_rotation=[0,360,0],view_rotation=[0,0,0],player_fov=90,view_fov=90)
    assert reader.view_matches(camera);camera['view_location']=[2,2,3];assert not reader.view_matches(camera)
    component=dict(component='candidate',on=True,value=203,write_mask=0,prior_on=True,prior_value=203,prior_write_mask=0,tagged=False,bench_setup=True)
    before=dict(components=[component]);after=copy.deepcopy(before)
    rows=[dict(frame_index=0,custom_depth=dict(unexpected=0,components=[component]),census=dict(owners=[dict(tag=200)]))]
    assert reader.depth_check(before,after,rows)['result']=='PASS'
    after['components'][0]['value']=204;assert reader.depth_check(before,after,rows)['result']=='FAIL'
    after=copy.deepcopy(before);rows[0]['census']['owners'][0]['tag']=203;assert reader.depth_check(before,after,rows)['result']=='FAIL'
    assert reader.frame_set([1,3])==dict(start_frame=1,end_frame=3,frame_count=2,span_frame_count=3,frame_indices=[1,3])
    raw=(p/'Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp').read_text();a=raw.index('\tbool WriteSessionAnnotation(');b=raw.index('\n}\n',a)
    copied=(t/'AnomalyAnnotation425.cpp').read_text();assert raw[a:b] in copied
    event={k:None for k in reader.EVENT_KEYS};event.update(event_id='blinking@10|actor',affected_frames=reader.frame_set([1]),injected_frames=reader.frame_set([1,2]),camera={k:None for k in ('path','global_position','near','far','rotation','fov_deg','aspect')},engine={k:None for k in ('ticks_msec','name','version','project')},affected_objects=dict(count=0,primary_index=0,nodes=[]))
    annotation={k:None for k in reader.ROOT_KEYS};annotation.update(label_schema=2,label_schema_minor=1,anomalies=[event],video={k:None for k in ('path','frames_dir','resolution','fps','target_fps','total_frames')})
    labels=[dict(frame_index=i,sampled_states=[dict(event_id='blinking@10',injected=i in (1,2),affected=i==1)]) for i in range(4)]
    events=[dict(event_id='blinking@10',target_name='actor')]
    assert reader.annotation_check(annotation,labels,events)['result']=='PASS'
    annotation['anomalies'][0]['affected_frames']=reader.frame_set([1,2]);assert reader.annotation_check(annotation,labels,events)['result']=='FAIL'
    source=[]
    for path in (p/'Source').rglob('*'):
        if path.suffix in ('.h','.cpp','.cs'):
            assert not violations(path.read_text(encoding='utf-8-sig')),str(path);source.append(str(path))
    for mutant in ('ActorHasTag(TEXT("EA_BlinkCube"))','FIntPoint(1280,720)','MaskTarget = 128'):assert violations(mutant)
    labels_cpp=(t/'AnomalyCaptureLabels425.cpp').read_text();assert 'CaptureBenchCamera425 = 0' in labels_cpp and 'TEXT("IAIBench")' in labels_cpp
    bench=(t/'AnomalyStreetBench425.cpp').read_text();assert 'SaveMap' not in bench and 'SavePackage' not in bench
    argv=kit.command('C5',kit.J/'tests/argv');assert '/Game/ParisStreet/Levels/SampleMap' in argv and '-game' in argv
    assert any('IAI.Capture.C5 ' in a and ' 4242' in a and 'r.EyeAdaptationQuality 0' in a for a in argv)
    assert not any('DefaultBackBufferPixelFormat' in a for a in argv)
    try:kit.validate(dict(arming_job='09434'))
    except RuntimeError:pass
    else:raise AssertionError('Spent arming admitted')
    for name in ('build-job-config.json','c5-job-config.json'):job_scope.load(kit.J/name)
    result=dict(result='PASS',reference_rule=reader.REFERENCE_RULE,unstable='1-level grain accepted; exactly 5 percent kept; above 5 percent dropped',view='angle wrap accepted; displaced view refused',depth='exact prior accepted; changed prior and reserved allocation refused',annotation='writer body identical to 5.1; gapped affected/injected sets accepted; wrong affected set refused',source_static=dict(files=len(source),violations=0,mutants=3),mask_negatives=mask_tests(),rotators=check_all(),python27='N/A: changed bench driver is native, no in-editor/game Python changed',argv=argv)
    (kit.J/'tests/fs-summary.json').write_text(json.dumps(result,indent=2));print(json.dumps({k:v for k,v in result.items() if k not in ('argv','mask_negatives')}))

if __name__=='__main__':run()
