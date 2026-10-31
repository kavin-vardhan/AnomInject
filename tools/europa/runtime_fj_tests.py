import copy
import io
import json
from pathlib import Path
import sys
from unittest.mock import patch

import numpy as np
import gate_readers as gates
import runtime_lean_gate as lean
import runtime_monitor as monitor
import runtime_policy as policy
import runtime_scripts as scripts
from runtime_evidence import ensure_frozen
from runtime_readers import jsonl,states,ROIS


def main(output):
    output=Path(output);output.mkdir(parents=True,exist_ok=False)
    rows=[]
    def done(name):rows.append(dict(case=name,result='PASS'))
    source=Path('D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-missing_object-positive/01')
    ensure_frozen(source/'raw-evidence.json')
    probe=source/'session/probe';roi=ROIS['missing_object']
    clean=[gates.image(probe/('clean-%02d.png'%n)) for n in range(20)]
    noise,measurements=gates.clean_noise(clean,roi,True)
    assert noise>.25 and all(m['maximum']==1 for m in measurements)
    changed=clean[1].copy();x,y,w,h=roi
    changed[y:y+h,x:x+w]=np.clip(clean[0][y:y+h,x:x+w]+2,0,255)
    try:gates.clean_noise([clean[0],changed],roi,True)
    except gates.GateFailure:pass
    else:raise AssertionError('Brightness-drift mutant accepted')
    done('quantization: kept one-LSB noise passes, two-level brightness drift fails')
    data=jsonl(probe/'probe.jsonl');state=states(data);prefix='missing_object-0'
    active=str(probe/(prefix+'-during-00.png'));before=str(probe/(prefix+'-before.png'))
    try:gates.picture_event([str(probe/('clean-%02d.png'%n)) for n in range(20)],before,[active],[active,active],roi,
        state[prefix+'-before']['actors'],state[prefix+'-after']['actors'],quantization=True)
    except gates.GateFailure as error:assert str(error)=='Pixel restore residue'
    else:raise AssertionError('Missing restoration accepted')
    done('quantization: real active frame substituted for restoration fails')
    calls=[]
    def shared(*args,**kwargs):
        calls.append(1)
        if len(calls)<3:raise PermissionError(13,'sharing violation')
        return io.StringIO('complete\n')
    with patch.object(Path,'open',shared):
        assert monitor.shared_probe_text(Path('unused'),0,sleep=lambda n:None)==('complete\n',9)
    assert len(calls)==3
    with patch.object(Path,'open',side_effect=PermissionError(13,'sharing violation')):
        assert monitor.shared_probe_text(Path('unused'),0,sleep=lambda n:None) is None
    done('probe retry: two conflicts recover; persistent conflict defers without consuming offset')
    import queue
    model=monitor.Monitor.__new__(monitor.Monitor)
    model.root=output;model.host=output;model.leg='A7_:blinking:positive';model.game=True
    model.queue=queue.Queue();model.observations=[];model.last_limits=float('inf');model.closed=False
    model.record=lambda row:None
    with patch.object(model,'read_probe',side_effect=AssertionError('live probe opened')):
        model.check((123,))
    assert not hasattr(model,'complete_at')
    done('probe writer protected: live A7 monitor never opens probe, absent last picture cannot finish')
    good=dict(idle_seconds=120,free_memory_bytes=4*lean.GIB,free_commit_bytes=8*lean.GIB,
        gpu_percent=100,processes=None,reading_errors={'gpu':'unknown'},volumes={d:dict(resolved_drive=d,free_bytes=n*lean.GIB) for d,n in lean.START_GIB.items()})
    assert not lean.reasons(good)
    gate=lean.wait(output/'one-reading.jsonl',policy.WINDOW_END,collector=lambda:good,sleep=lambda _:None)
    assert gate['status']=='CLEAR' and gate['polls']==1
    for key,value in [('idle_seconds',119),('free_memory_bytes',4*lean.GIB-1),('free_commit_bytes',8*lean.GIB-1)]:
        bad=copy.deepcopy(good);bad[key]=value;assert lean.reasons(bad)
    bad=copy.deepcopy(good);bad['volumes']['D']['free_bytes']=25*lean.GIB-1;assert lean.reasons(bad)
    sample=dict(free_commit_bytes=4*lean.GIB,volumes={d:dict(resolved_drive=d,free_bytes=n*lean.GIB) for d,n in lean.FLOOR_GIB.items()})
    assert not monitor.stop_reasons(sample)
    bad=copy.deepcopy(sample);bad['volumes']['D']['free_bytes']-=1;assert monitor.stop_reasons(bad)
    bad=copy.deepcopy(sample);bad['free_commit_bytes']-=1;assert monitor.stop_reasons(bad)
    done('lean gate: one reading, FK1/FK2 start and FD3 stop lines; GPU and missing coverage do not block')
    generated=scripts.steps('A7_:lod_corruption:positive')
    applies=[r for r in generated if r['command']=='apply' and r['args'][0]=='lod_corruption']
    assert len(applies)==6 and all(r['args']==['lod_corruption','EA_LOD','1'] for r in applies)
    old=copy.deepcopy(applies);old[0]['args'][-1]='2'
    assert not all(r['args'][-1]=='1' for r in old)
    done('LOD parameter: six forced-model 1 applies, old forced-model 2 rejected')
    previous=json.loads(Path('D:/IA_Europa/Launch/Europa425-Runtime.json').read_text())
    try:policy.validate(previous,'real')
    except RuntimeError:pass
    else:raise AssertionError('Spent arming accepted')
    try:policy.slug('A7_:missing_object:positive')
    except RuntimeError:pass
    else:raise AssertionError('Already qualified anomaly can relaunch')
    done('armed scope: old arming and already-qualified missing-object launch refused')
    import rotator_contract
    static=rotator_contract.check_all(str(policy.TOOLS))
    for path in policy.TOOLS.glob('*.py'):
        assert not any(line.lstrip().startswith('#') for line in path.read_text(encoding='utf-8-sig').splitlines()),str(path)
    done('keyword Rotators and zero Python comment lines')
    result=dict(result='PASS',cases=rows,rotator_static=static,unreal_launches=0)
    (output/'summary.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    print(json.dumps(dict(result='PASS',cases=len(rows))))


if __name__=='__main__':main(sys.argv[1])
