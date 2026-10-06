import json
import re
import subprocess
import sys
import time
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
label,kind=sys.argv[1:3]
receipt=[]
launcher='launch_staged_stack.py' if kind=='early_delay' else 'launch_stack.py'
for script,args in [(launcher,[label,kind,'1']),('check_stack_positive.py',[label,'1']),('check_stack_layers.py',[label])]:
    log_path=w/('driver-'+label+'-'+script+'.log')
    assert not log_path.exists()
    with log_path.open('w') as log:
        result=subprocess.run([sys.executable,'-B',str(w/script),*args],stdout=log,stderr=subprocess.STDOUT)
    row=dict(script=script,args=args,exit=result.returncode,log=str(log_path))
    receipt.append(row)
    print(json.dumps(row),flush=True)
    if script==launcher and result.returncode: raise SystemExit(result.returncode)
root=w/'runs'/label
positive=json.loads((root/'positive-result.json').read_text())
layers=json.loads((root/'layer-result.json').read_text())
assert len(positive['events'])==1,'The bench mutant must exercise a real event'
if kind=='partial':
    caught=layers['status']=='FAIL' and any('not measurably blurred' in issue for issue in layers['issues'])
elif kind in ('early_bind','early_delay'):
    caught=layers['status']=='FAIL' and any('incorrect pixel edges or bound appearance' in issue for issue in layers['issues'])
elif kind=='no_bind':
    caught=positive['status']=='FAIL' and 'event without measured target change' in positive['issues']
else: raise ValueError(kind)
result=dict(status='PASS' if caught else 'INCONCLUSIVE',caught=caught,kind=kind,steps=receipt,positive_issues=positive['issues'],layer_issues=layers['issues'])
if kind=='early_delay':
    delay=json.loads((root/'upload-delay.json').read_text())
    assert delay['restored'] and not delay['timeout'] and 120<delay['restore_written_frames']<142
    log=(root/'game.log').read_text(errors='replace')
    for name,value in delay['original'].items():
        observed=[int(v) for v in re.findall(re.escape(name)+r'\s*=\s*"?(\d+)',log)]
        assert observed[0]==observed[-1]==value and 0 in observed,observed
    result['delay']=delay
(root/'mutant-result.json').write_text(json.dumps(result,indent=2))
print(json.dumps(result),flush=True)
raise SystemExit(not caught)
