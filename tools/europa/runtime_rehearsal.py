import json
import os
from pathlib import Path
import subprocess
import sys
from unittest.mock import patch

from environment import execution_env
import runtime_children
import runtime_policy as policy
from runtime_runner import perform_session,record_leg
from runtime_evidence import ensure_frozen


def main(output):
    import write_ledger as ledger
    output=policy.canonical(output,policy.JOB/'scratch');output.mkdir(parents=True,exist_ok=False)
    temp=policy.TEMP/'scratch'/output.name;temp.mkdir(parents=True,exist_ok=False)
    env,_=execution_env(os.environ)
    sibling=subprocess.Popen([sys.executable,'-B','-c','import time;time.sleep(300)'],env=env)
    previous=os.environ['EUROPA_OPERATION_DIR'];rows=[]
    def case(name,standin=None):
        root=output/name;root.mkdir();scratch=temp/name;scratch.mkdir();host=root/'host';host.mkdir()
        os.environ['EUROPA_OPERATION_DIR']=str(root)
        original=runtime_children.reason
        def rehearsal_reason(image,command_line,root=False):return original(image,command_line,root,True)
        def protected():
            if any(p.is_file() for folder in ('Content','Config','Binaries') for p in (host/folder).rglob('*')):
                raise RuntimeError('Rehearsal protected change')
        command=[sys.executable,'-B',str(policy.TOOLS/'runtime_standin.py'),'--root',str(root),'--case',standin or name]
        broker=None
        try:
            if name=='monitor':
                broker=subprocess.Popen(command+['--broker'],env=env,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
            with patch.dict(ledger.SCOPE,runtime_scope=policy.SCOPE),patch.object(runtime_children,'reason',rehearsal_reason):
                result=perform_session('Load',root,scratch,host,dict(content=[],config=[],descriptors=[]),command,
                    ledger,3 if name in ('deadline','monitor') else 15,check_protection=protected,synthetic=True)
            assert sibling.poll() is None
            child=json.loads((root/'child-001.json').read_text())
            stopped=child.get('stopped_owned_processes',[])
            assert sibling.pid not in [r['pid'] for r in stopped]
            assert (root/'standin-evidence.txt').is_file() and ensure_frozen(root/'raw-evidence.json')>0
            if name=='deadline':
                detached=json.loads((root/'detached-pid.json').read_text())['pid']
                assert detached in [r['pid'] for r in stopped]
            if name=='monitor':
                detached=json.loads((root/'detached-pid.json').read_text())
                adopted=child['compile_children']['adopted_monitors']
                match=next(r for r in adopted if r['pid']==detached['pid'])
                assert match['monitor_of']==child['pid'] and match['parent_pid']==broker.pid
                assert broker.pid not in [r['pid'] for r in child['compile_children']['owned']]
                assert match['environment']['result']=='readable' and match['environment']['canary_name_present'] is False
                assert [r['pid'] for r in stopped].index(detached['pid'])<[r['pid'] for r in stopped].index(child['pid'])
            return dict(result,receipt=str(root/'child-001.json'),evidence_kept=True,sibling_survived=True)
        finally:
            if broker is not None:
                if broker.poll() is None:broker.terminate()
                broker.wait()
    try:
        for name in ('pass','fail','deadline','content','ubt-build','monitor'):
            result=case(name)
            assert (result['result']=='PASS')==(name=='pass'),(name,result)
            assert result['end_run']==(name in ('fail','content','ubt-build')),(name,result)
            rows.append(dict(case=name,result='PASS',leg=result))
        following=dict(legs=[],result='RUNNING')
        bad=case('continuation-fail','normal-fail')
        assert not bad['end_run'] and bad['result']=='FAIL'
        assert record_leg(following,bad)
        good=case('continuation-pass','pass');assert good['result']=='PASS' and record_leg(following,good)
        assert len(following['legs'])==2
        rows.append(dict(case='failing-leg-then-passing-leg',result='PASS',legs=following['legs']))
        ensure=case('normal-ensure','ensure');abnormal=case('abnormal-exit','fail')
        assert ensure['result']=='FAIL' and not ensure['end_run'] and ensure['new_crash_report_folders']
        assert abnormal['result']=='FAIL' and abnormal['end_run']
        rows.append(dict(case='ensure-folder-versus-abnormal-exit',result='PASS',normal_ensure=ensure,abnormal=abnormal))
    finally:
        os.environ['EUROPA_OPERATION_DIR']=previous
        if sibling.poll() is None:sibling.terminate()
        sibling.wait()
    result=dict(result='PASS',cases=rows,unreal_launches=0,roots=[str(output),str(temp)],
        real_host_and_slots_untouched=True,common_path='runtime_runner.perform_session / execute / write_ledger.run_child / RuntimeTree / Monitor / outcome')
    (output/'summary.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(result='PASS',cases=len(rows),unreal_launches=0)))


if __name__=='__main__':main(Path(sys.argv[1]))
