import copy
import json
from pathlib import Path
import subprocess

import capture_c3_kit as kit
import capture_c3_reader as reader
import capture_c3_tests
import job_scope
from rotator_contract import check_all,check_source

P=Path(__file__).resolve().parents[2]

def blink_contract(source):
    body=source.split('uint32 FAnomalyMasks425::Selection(UWorld* World)',1)[1].split('bool FAnomalyMasks425::Render',1)[0]
    return all(value in body for value in ('AnomalyHiddenClass::IsLogicallyHidden(*It)','AnomalyInstall::IsInstalledByte','Result = 128')) and 'if (Active.Id == TEXT("blinking")) { Result = 128; }' not in body

def run():
    source_path='Source/AnomalyCapture/Private/Compat/AnomalyMasks425.cpp'
    source=(P/source_path).read_text()
    old=subprocess.check_output(['git','-C',str(P),'show','d43635d:'+source_path],text=True)
    assert blink_contract(source) and not blink_contract(old)
    assert 'if (MaskPixels)' in source and '(Frame.LabelKey425.MaskTarget != 0) != (MaskPixels != 0)' in source
    labels=(P/'Source/AnomalyCapture/Private/Compat/AnomalyCaptureLabels425.cpp').read_text()
    assert 'TEXT("mask_state"),TEXT("empty")' in labels and 'TEXT("mask"),MakeShared<FJsonValueNull>()' in labels
    real=Path('D:/IA_Europa/Receipts/09433b/c3-attempts/C3/02/plugin-frames/epoch01/labels.jsonl')
    rows=[json.loads(line) for line in real.read_text().splitlines()][:60]
    wrong=[]
    for row in rows:
        state=next(s for s in row['sampled_states'] if s['id']=='blinking')
        selected=state['fire_active'] and state['installed'] and state['active']
        assert selected==state['labelled']
        if state['fire_active'] and not selected:wrong.append(row['frame_index'])
    assert len(wrong)==21
    metric=reader.tests()
    for route in reader.ROUTES:
        assert metric['positive']['result']=='PASS'
        assert next(r for r in metric['precision_negatives'] if r['anomaly']==route)['metrics']['result']=='FAIL'
    commands=capture_c3_tests.run()
    argv=kit.command('C3',kit.J/'tests/command')
    assert sum('r.EyeAdaptationQuality 0,r.EyeAdaptationQuality,' in a for a in argv)==1
    assert kit.DEADLINE=='2026-11-01T18:29:00+00:00'
    try:kit.validate(dict(arming_job='09433b',stage='C3'))
    except RuntimeError:pass
    else:raise AssertionError('Spent arming accepted')
    build=json.loads((kit.J/'build-job-config.json').read_text())
    runtime=json.loads((kit.J/'c3-job-config.json').read_text())
    assert job_scope.fm1(build,kit.J) and job_scope.memory_policy_allowed(runtime,kit.J)
    bad=copy.deepcopy(build);bad['max_parallel_actions']=3
    assert not job_scope.fm1(bad,kit.J)
    job_scope.load(kit.J/'build-job-config.json');job_scope.load(kit.J/'c3-job-config.json')
    for name in ('capture_c3_kit.py','capture_c3_reader.py','capture_fq_tests.py','job_scope.py','write_ledger.py'):
        assert not check_source((P/'tools/europa'/name).read_text(),name)
    result=dict(result='PASS',blinking=dict(old_source='REFUSED',new_source='PASS',kept_clean_drawn_mask_failures=wrong),metrics=metric,
        commands=commands,keyword_rotators=check_all(),changed_in_editor_scripts=[],scoped_FM1_FJ1='PASS',exposure_argv=argv[-1])
    (kit.J/'tests/fq-summary.json').write_text(json.dumps(result,indent=2))
    print(json.dumps(result))

if __name__=='__main__':run()
