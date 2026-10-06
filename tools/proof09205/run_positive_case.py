import json
import subprocess
import sys
import time
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
label,leg,kind,count=sys.argv[1:5]
launcher='launch_staged_stack.py' if leg=='staged' else 'launch_stack.py'
steps=[(launcher,[label,kind,count],0),('check_stack_positive.py',[label,count],0),('check_stack_layers.py',[label],0),('collect_stack_lifetime.py',[label,count],0)]
receipt=[]
for script,args,wanted in steps:
    path=w/('driver-'+label+'-'+script+'.log')
    assert not path.exists()
    start=time.time()
    with path.open('w') as log:
        result=subprocess.run([sys.executable,'-B',str(w/script),*args],stdout=log,stderr=subprocess.STDOUT)
    row=dict(script=script,args=args,exit=result.returncode,expected=wanted,seconds=time.time()-start,log=str(path))
    receipt.append(row)
    (w/('case-'+label+'.json')).write_text(json.dumps(receipt,indent=2))
    print(json.dumps(row),flush=True)
    if result.returncode!=wanted: raise SystemExit(result.returncode or 1)
