import json
import os
import subprocess
import sys
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
label,kind=sys.argv[1:3]
if kind=='single_vt':
    steps=[('launch_single_regression.py',[label,'targeted','3']),('check_single_regression.py',[label,'3'])]
else:
    steps=[('launch_regression.py',[label,kind]),('check_regression.py',[label])]
env=os.environ.copy()
env.update(TEMP=str(w/'temp'),TMP=str(w/'temp'))
for script,args in steps:
    path=w/('driver-'+label+'-'+script+'.log')
    assert not path.exists()
    with path.open('w') as output:
        result=subprocess.run([sys.executable,'-B',str(w/script),*args],stdout=output,stderr=subprocess.STDOUT,env=env)
    print(json.dumps(dict(script=script,exit=result.returncode,log=str(path))),flush=True)
    if result.returncode: raise SystemExit(result.returncode)
