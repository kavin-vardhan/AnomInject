import copy
import json
from pathlib import Path
import sys
import capture_c5_reader as r

def run():
    event={k:None for k in r.EVENT_KEYS}
    event.update(event_id='blinking@10|actor',affected_frames=r.frame_set([1]),injected_frames=r.frame_set([1,2]),coverage_ratio=0.2,coverage_pct=17,bbox_source='projected',camera={k:None for k in ('path','global_position','near','far','rotation','fov_deg','aspect')},engine={k:None for k in ('ticks_msec','name','version','project')},affected_objects=dict(count=0,primary_index=0,nodes=[]))
    event['camera'].update(near=10,far=0)
    annotation={k:None for k in r.ROOT_KEYS}
    annotation.update(label_schema=2,label_schema_minor=1,anomalies=[event],video={k:None for k in ('path','frames_dir','resolution','fps','target_fps','total_frames')})
    labels=[dict(frame_index=i,camera=dict(near=10,far=0),sampled_states=[dict(event_id='blinking@10',injected=i in (1,2),affected=i==1,projected_screen_share=0.2,drawn_pixels=0)]) for i in range(4)]
    events=[dict(event_id='blinking@10',target_name='actor',selection_provenance=dict(coverage_pct=17,valid=True))]
    assert r.annotation_check(annotation,labels,events)['result']=='PASS'
    for key,value in [('coverage_ratio',0.5),('coverage_pct',20),('bbox_source','drawn')]:
        mutant=copy.deepcopy(annotation);mutant['anomalies'][0][key]=value
        assert r.annotation_check(mutant,labels,events)['result']=='FAIL'
    mutant=copy.deepcopy(annotation);mutant['anomalies'][0]['camera']['far']=100000
    assert r.annotation_check(mutant,labels,events)['result']=='FAIL'
    labels[1]['sampled_states'][0]['drawn_pixels']=100;event['bbox_source']='drawn'
    assert r.annotation_check(annotation,labels,events)['result']=='PASS'
    events[0].pop('selection_provenance')
    missing=r.annotation_check(annotation,labels,events)
    assert missing['result']=='FAIL' and missing['schema_and_sets']=='PASS'
    assert missing['values']['coverage_ratio']['result']=='PASS'
    event['coverage_pct']=-1;events[0]['selection_provenance']=dict(coverage_pct=-1,valid=False)
    assert r.annotation_check(annotation,labels,events)['result']=='PASS'
    root=Path(__file__).resolve().parents[2]
    source=(root/'Source/AnomalyCapture/Private/Compat/AnomalyCaptureLabels425.cpp').read_text()
    assignment='E.CoveragePct=Selection.CoveragePct;'
    assert assignment in source and 'AnomalyViewport::EvaluateSelectionProvenance(World,Actor,Selection);' in source
    assert assignment not in source.replace(assignment,'E.CoveragePct=100*E.CoverageRatio;')
    return dict(result='PASS',fix='5.1 coverage_ratio is affected-frame mean screen share; coverage_pct is frozen selection provenance',negatives=['frame-ratio coverage','percentage derived from mean rather than provenance','unmeasured drawn bbox','constant far plane','unrecorded provenance'],source_citations=['AnomalyCaptureSubsystem.cpp:8643','AnomalyCaptureSubsystem.cpp:10270-10272','AnomalyViewport.cpp:912-950'],native='Source contract only; rebuild and real qualification pending')

if __name__=='__main__':
    result=run()
    if len(sys.argv)>1:Path(sys.argv[1]).write_text(json.dumps(result,indent=2))
    print(json.dumps(result))
