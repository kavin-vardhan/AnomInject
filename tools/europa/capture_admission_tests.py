import ast
import json
import os
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch
import runtime_runner as runner
from runtime_lean_gate import reasons

GIB=2**30

def run(output):
    output=Path(output);output.mkdir(parents=True,exist_ok=False)
    cases=[]
    def case(name,drop=False,old=False,mutate=None,legacy=False):
        root=output/name;root.mkdir();calls=[]
        state=dict(commit=9*GIB,protected=False,binary=False)
        class Watch:
            def check(self):
                calls.append('watch')
                if state['binary']:raise RuntimeError('bound binary changed')
        class Monitor:
            def __init__(self,*args,**kwargs):
                calls.append('monitor_setup');self.watch=Watch();self.before={'protected':False};self.crash_changes=[]
                if drop:state['commit']=7*GIB
            def metadata(self):return {'protected':state['protected']}
            def start(self,pid):calls.append('started')
            def check(self,*args):pass
            def process_exited(self,*args):pass
            def close(self):calls.append('closed')
        def admit():
            calls.append('admit')
            sample=dict(free_commit_bytes=state['commit'],free_memory_bytes=5*GIB,idle_seconds=301,volumes={d:dict(resolved_drive=d,free_bytes=100*GIB) for d in 'CDE'})
            if reasons(sample,300):raise RuntimeError('admission WAIT')
            if mutate:state[mutate]=True
            return dict(status='CLEAR',free_commit_bytes=state['commit'])
        def child(*args,**kwargs):
            calls.append('create')
            kwargs['on_started'](1234)
            return 0
        ledger=SimpleNamespace(SCOPE=dict(temp_root='original'),run_child=child)
        with patch('runtime_monitor.Monitor',Monitor),patch.dict(os.environ,EUROPA_OPERATION_DIR=str(root)):
            if old:admit()
            row=runner.execute('C5',root,root/'temp',root/'host',{},['standin'],ledger,3,synthetic=True,admit=None if old or legacy else admit)
        assert ledger.SCOPE['temp_root']=='original'
        if drop or mutate:
            assert 'create' not in calls,(name,calls)
            assert row['launches']==0 and row['result']=='SESSION_STOP'
        else:
            assert row['exit_code']==0 and row['launches']==1
            if not legacy:assert calls.index('monitor_setup')<calls.index('admit')<calls.index('create')
        assert calls[-1]=='closed'
        return dict(case=name,result='PASS',calls=calls,launches=row['launches'])
    cases.append(case('setup-before-admission'))
    cases.append(case('setup-memory-drop-refuses-launch',drop=True))
    try:case('old-admission-before-setup',drop=True,old=True)
    except AssertionError:cases.append(dict(case='old-order-mutant',result='EXPECTED_FAIL'))
    else:raise AssertionError('Old admission order passed')
    cases.append(case('protected-change-during-wait',mutate='protected'))
    cases.append(case('binary-change-during-wait',mutate='binary'))
    cases.append(case('legacy-callers-unchanged',legacy=True))
    tree=ast.parse(Path(__file__).with_name('capture_c5_kit.py').read_text())
    fn=next(n for n in tree.body if isinstance(n,ast.FunctionDef) and n.name=='run')
    nested=next(n for n in ast.walk(fn) if isinstance(n,ast.FunctionDef) and n.name=='admit')
    waits=[n for n in ast.walk(fn) if isinstance(n,ast.Call) and isinstance(n.func,ast.Name) and n.func.id=='wait']
    assert len(waits)==1 and waits[0] in list(ast.walk(nested))
    execution=next(n for n in ast.walk(fn) if isinstance(n,ast.Call) and isinstance(n.func,ast.Name) and n.func.id=='execute')
    assert any(k.arg=='admit' and isinstance(k.value,ast.Name) and k.value.id=='admit' for k in execution.keywords)
    cases.append(dict(case='C5-uses-creation-admission',result='PASS'))
    result=dict(result='PASS',cases=cases,unreal_launches=0)
    (output/'summary.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    print(json.dumps(result))

if __name__=='__main__':
    import sys
    run(sys.argv[1])
