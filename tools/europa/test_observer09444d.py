import ast
import copy
import json
from pathlib import Path
import subprocess
import sys

P=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector')
T=P/'tools/europa';sys.path.insert(0,str(T))
J=Path('D:/IA_Europa/Receipts/09444d')
F=Path('E:/IA_Fixtures/Outline425')
import blackout09444d as deadline

def rejects(fn):
    try:fn()
    except (RuntimeError,AssertionError):return True
    return False

deadline.require_launch(now=deadline.LAUNCH_CUTOFF-1)
assert rejects(lambda:deadline.require_launch(now=deadline.LAUNCH_CUTOFF))
assert rejects(lambda:deadline.require_launch(seconds=10000,now=deadline.LAUNCH_CUTOFF-1))
deadline.require_runtime(now=deadline.CLOSE_DEADLINE-deadline.CLOSE_RESERVE-1)
assert rejects(lambda:deadline.require_runtime(now=deadline.CLOSE_DEADLINE-deadline.CLOSE_RESERVE))
assert rejects(lambda:deadline.require_reader(now=deadline.CLOSE_DEADLINE-300))
old=ast.parse((F/'Scripts/texture_pose09444c.py').read_text())
new=ast.parse((F/'Scripts/texture_pose09444d.py').read_text())
for name in ('setup','vector','camera_settled'):
    a=next(x for x in old.body if isinstance(x,ast.FunctionDef) and x.name==name)
    b=next(x for x in new.body if isinstance(x,ast.FunctionDef) and x.name==name)
    assert ast.dump(a)==ast.dump(b)
py='E:/EpicGames/UE_4.25/Engine/Binaries/ThirdParty/Python/Win64/python.exe'
values=[]
for script in ('pose_py27_test09444d.py','observer_py27_test09444d.py'):
    values.append(json.loads(subprocess.check_output([py,script],text=True)))
subprocess.check_call([sys.executable,str(T/'scope09444d_selftest.py')])
value=dict(result='PASS',deadline_negatives=4,unchanged_setup=True,python27=values)
(J/'observer-tests.json').write_text(json.dumps(value,indent=2))
print('PASS: frozen setup, Python 2.7/Rotator, direct-read callback, deadline boundaries and scoped policy')
