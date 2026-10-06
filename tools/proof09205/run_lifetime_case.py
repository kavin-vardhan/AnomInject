import json
import subprocess
import sys
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
label,kind=sys.argv[1:3]
steps=[('launch_stack.py',[label,kind,'1']),('check_world_end.py' if kind=='world_end' else 'check_lifetime.py',[label])]
for script,args in steps:
    path=w/('driver-'+label+'-'+script+'.log')
    assert not path.exists()
    with path.open('w') as output:
        result=subprocess.run([sys.executable,'-B',str(w/script),*args],stdout=output,stderr=subprocess.STDOUT)
    print(json.dumps(dict(script=script,exit=result.returncode,log=str(path))),flush=True)
    if result.returncode: raise SystemExit(result.returncode)
