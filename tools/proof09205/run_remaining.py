import json
import subprocess
import sys
import time
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
assert json.loads((w/'part-b-build-gate.json').read_text())['status']=='PASS'
jobs=[
('run_lifetime_case.py',['stacks-world-end-final','world_end']),
('run_mutant_case.py',['stacks-no-bind-final','no_bind']),
('run_refusal_case.py',['stacks-release-refusal-final','baseline']),
('run_refusal_case.py',['stacks-accepted-refusal-final','accepted_baseline']),
('run_refusal_case.py',['stacks-unnamed-refusal-final','unnamed']),
('run_regression_case.py',['regression-hold-final','hold']),
('run_regression_case.py',['regression-proxy-final','proxy']),
('run_regression_case.py',['regression-uv-final','uv']),
('run_regression_case.py',['regression-normal-final','normal']),
('run_regression_case.py',['regression-single-vt-final','single_vt']),
('launch_final_census.py',['census-stacks-all-part-b','stacks','all']),
('check_census.py',['census-stacks-all-part-b']),
('launch_final_census.py',['census-single-all-part-b','single','all']),
('check_census.py',['census-single-all-part-b'])]
receipt=[]
if sys.argv[1:]==['resume-01']:
    jobs=jobs[3:]
    jobs[0][1][0]='stacks-accepted-refusal-final-02'
path=w/('remaining-queue-resume-01.json' if sys.argv[1:] else 'remaining-queue.json')
assert not path.exists()
path.write_text(json.dumps(dict(jobs=jobs,results=[]),indent=2))
deadline=time.time()+300
pre=w/'runs/stacks-displace-final/lifetime-result.json'
while not pre.exists() and time.time()<deadline: time.sleep(1)
assert pre.exists() and json.loads(pre.read_text())['status']=='PASS'
for script,args in jobs:
    log_path=w/('queue-'+args[0]+'-'+script+'.log')
    assert not log_path.exists()
    start=time.time()
    print(json.dumps(dict(start=script,args=args,time=start)),flush=True)
    with log_path.open('w') as log:
        result=subprocess.run([sys.executable,'-B',str(w/script),*args],stdout=log,stderr=subprocess.STDOUT)
    row=dict(script=script,args=args,exit=result.returncode,seconds=time.time()-start,log=str(log_path))
    receipt.append(row)
    path.write_text(json.dumps(dict(jobs=jobs,results=receipt),indent=2))
    print(json.dumps(row),flush=True)
    if result.returncode: raise SystemExit(result.returncode)
