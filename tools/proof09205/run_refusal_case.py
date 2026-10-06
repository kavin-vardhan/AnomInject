import json
import subprocess
import sys
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
label,kind=sys.argv[1:3]
reason={'baseline':'virtual_texture','accepted_baseline':'stack_layout','unnamed':'stack_unnamed'}[kind]
launcher='launch_stack.py' if kind=='unnamed' else 'launch_staged_stack.py'
for script,args in [(launcher,[label,kind,'1']),('check_refusal.py',[label,reason])]:
    path=w/('driver-'+label+'-'+script+'.log')
    assert not path.exists()
    with path.open('w') as output:
        result=subprocess.run([sys.executable,'-B',str(w/script),*args],stdout=output,stderr=subprocess.STDOUT)
    print(json.dumps(dict(script=script,exit=result.returncode,log=str(path))),flush=True)
    if result.returncode: raise SystemExit(result.returncode)
