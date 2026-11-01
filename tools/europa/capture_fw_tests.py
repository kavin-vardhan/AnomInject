import copy
import datetime
import json
from pathlib import Path
from unittest.mock import patch
import capture_fv_tests as fv
import capture_office_reader as r
import capture_c5_kit as kit
from PIL import Image
import numpy as np

def run():
    fv.run()
    tests=[]
    root=max((kit.J/'tests').glob('ui-pixels-*'),key=lambda p:p.stat().st_mtime_ns)
    control=np.zeros((32,64,3),np.uint8)
    for i,c in enumerate(r.MARKER):control[(i//4)*16:(i//4+1)*16,(i%4)*16:(i%4+1)*16]=c
    Image.fromarray(control).save(root/'ui-control-00.png')
    rows=[dict(frame_index=0,image='frame.png',sample_frame=101,family_frame=33)]
    original=json.loads((root/'ui-check.json').read_text())
    assert r.ui_pixels(root,rows)['result']=='PASS'
    for mutate in ('draw_frame','control_frame','request','no_draw','no_control','no_pair'):
        n=copy.deepcopy(original)
        if mutate=='draw_frame':n['drawn_frames'][0]['sample_frame']+=1
        if mutate=='control_frame':n['control_frames'][0]['sample_frame']+=1
        if mutate=='request':n['drawn_frames'][0]['request']+=1
        if mutate=='no_draw':n['drawn_frames']=[]
        if mutate=='no_control':n['control_frames']=[]
        if mutate=='no_pair':n['paired']=0
        (root/'ui-check.json').write_text(json.dumps(n))
        assert r.ui_pixels(root,rows)['result']=='FAIL',mutate
    (root/'ui-check.json').write_text(json.dumps(original))
    tests.append(dict(fix='FW4',result='PASS',negative='wrong draw/control frame, wrong request, missing draw/control/pair refused'))
    events=[dict(id='lod_corruption',target_name='multi')]
    log="Auto.LodTarget target=multi eligible_components=1 min_lods=2\nlod_corruption: 'mesh' forced LOD 2 of 2"
    assert r.lod_guard(log,events)['result']=='PASS'
    for bad in (log.replace('min_lods=2','min_lods=1'),log.replace('2 of 2','1 of 1'),log.replace('2 of 2','1 of 2'),log.replace('target=multi','target=wrong'),log.splitlines()[0]):
        assert r.lod_guard(bad,events)['result']=='FAIL'
    source=(fv.P/'Source/AnomalyInjector/Private/Anomalies/Anomaly_LodCorruption.cpp').read_text()
    assert 'IsAutoPoolSelection(World) && !AnomalyLod::HasMultipleLods(Mesh)' in source
    assert ': AnomalyLod::WorstLodSentinel' in source
    tests.append(dict(fix='FW3',result='PASS',negative='single-LOD or wrong target and finest-LOD substitution refused'))
    calls=[]
    def wait(path,deadline,**kwargs):
        calls.append((deadline,kwargs['memory']));return dict(status='WINDOW_ENDED' if len(calls)==1 else 'CLEAR')
    cutoff='2026-11-01T21:00:00+00:00'
    kit.admission_profile(wait,kit.J,now=datetime.datetime.fromisoformat('2026-11-01T20:59:00+00:00'),cutoff=cutoff)
    assert calls==[(cutoff,(12,6)),(kit.DEADLINE,(8,4))]
    calls.clear();kit.admission_profile(wait,kit.J,now=datetime.datetime.fromisoformat(cutoff),cutoff=cutoff)
    assert calls==[(kit.DEADLINE,(8,4))]
    tests.append(dict(fix='FW6',result='PASS',negative='build-finish cutoff overrides later 03:00 deadline; no street wait after cutoff'))
    result=dict(result='PASS',tests=tests,carried_suite='fv-summary.json',python27='N/A: native changes and external Python only')
    (kit.J/'tests/fw-summary.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))

if __name__=='__main__':run()
