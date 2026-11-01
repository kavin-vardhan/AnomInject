import json
from pathlib import Path
import re
import capture_c5_kit as kit
import capture_office_reader as r
import job_scope

def run():
    tests=[]
    events=[dict(id='lod_corruption',target_name='multi')]
    prefix='Auto.LodTarget target=multi eligible_components=1 min_lods=2\n'
    def log(drawn,forced,count=2):
        return prefix+'Auto.LodChoice component=mesh drawn=%d count=%d forced=%d screen=0.2 source=engine-screen-size-game-thread\nlod_corruption: \'mesh\' forced LOD %d of %d'%(drawn,count,forced,forced,count)
    for drawn,forced in ((0,2),(1,1)):
        assert r.lod_guard(log(drawn,forced),events)['result']=='PASS'
    for bad in (log(1,2),log(0,1),log(-1,1),log(2,1),log(0,1,1),log(0,2).replace('min_lods=2','min_lods=1'),log(0,2).replace('Auto.LodChoice','no_prediction')):
        assert r.lod_guard(bad,events)['result']=='FAIL'
    tests.append(dict(fix='FX3 choice reader',result='PASS',negative='unchanged forced model, unknown prediction and single LOD refused'))
    old=Path('D:/IA_Europa/Receipts/09439/c5-attempts/C5/02/plugin-frames/epoch01')
    rows=[json.loads(s) for s in (old/'labels.jsonl').read_text().splitlines()]
    prior=json.loads((old/'labels-receipt.json').read_text())
    changes=r.lod_changes(old,rows,prior['events'])
    assert len(changes)==12 and all(c['result']=='FAIL' for c in changes)
    tests.append(dict(fix='FX3 visible-change gate',result='PASS',negative='all twelve real unchanged 09439 LOD events fail',events=changes))
    calls=[]
    def wait(path,deadline,**kw):
        calls.append((deadline,kw));return dict(status='CLEAR')
    assert kit.admission_profile(wait,kit.J)[1]=='fallback'
    assert calls==[(kit.DEADLINE,dict(idle_seconds=300,memory=(8,4)))]
    try:kit.admission_profile(wait,kit.J,prior='street')
    except RuntimeError:pass
    else:raise AssertionError('Street admitted')
    argv=kit.command('C5',kit.J/'test-command');kit.select_map(argv,'fallback')
    assert '/Game/EuropaPhaseA/EA_Core' in argv and '-IAICaptureFallback' in argv
    assert any('IAI.SetMinScreenCoverage 0.5,IAI.SetMinScreenCoverage,' in a for a in argv)
    assert not any('DefaultBackBufferPixelFormat' in a for a in argv)
    for name in ('build-job-config.json','c5-job-config.json'):
        data=json.loads((kit.J/name).read_text());assert job_scope.memory_policy_allowed(data,kit.J)
        data['memory']['start_commit_GiB']=7;assert not job_scope.memory_policy_allowed(data,kit.J)
    source=(Path(__file__).resolve().parents[2]/'Source/AnomalyInjector/Private/AnomalyViewport.cpp').read_text()
    assert re.search(r'GMinScreenCoveragePct\s*=\s*6\.0f',source)
    try:kit.validate(dict(arming_job='09439',stage='OfficeSmoke',deadline=kit.DEADLINE))
    except RuntimeError:pass
    else:raise AssertionError('Spent kit accepted')
    tests.append(dict(fix='FX4/FX6 arming',result='PASS',negative='street/spent kit/lowered commit line refused',bench_floor_pct=0.5,shipped_floor_pct=6.0))
    result=dict(result='PASS',tests=tests,python27='N/A: no in-editor Python changed; native driver and external reader only',keyword_rotators='N/A: no in-editor script changed')
    (kit.J/'tests/fx-summary.json').write_text(json.dumps(result,indent=2))
    print(json.dumps(dict(result='PASS',tests=[dict(fix=t['fix'],result=t['result'],negative=t['negative']) for t in tests])))

if __name__=='__main__':run()
