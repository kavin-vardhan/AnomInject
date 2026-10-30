import json
import os
from pathlib import Path
import subprocess
import sys
from unittest.mock import patch

from environment import execution_env
import runtime_children
import runtime_policy as policy
from runtime_runner import execute
from runtime_readers import read_leg


def main(output):
    import write_ledger as ledger
    output=policy.canonical(output,policy.JOB/'scratch');output.mkdir(parents=True,exist_ok=False)
    temp=policy.TEMP/'scratch'/output.name;temp.mkdir(parents=True,exist_ok=False)
    env,_=execution_env(os.environ)
    sibling=subprocess.Popen([sys.executable,'-B','-c','import time;time.sleep(180)'],env=env)
    previous=os.environ['EUROPA_OPERATION_DIR'];rows=[]
    try:
        for case in ('pass','fail','deadline','content','ubt-build'):
            root=output/case;root.mkdir();scratch=temp/case;scratch.mkdir();host=root/'host';host.mkdir()
            os.environ['EUROPA_OPERATION_DIR']=str(root)
            original_poll=runtime_children.poll
            error=None
            def rehearsal_reason(image,command_line,root=False):
                return original_reason(image,command_line,root,True)
            original_reason=runtime_children.reason
            with patch.dict(ledger.SCOPE,runtime_scope=policy.SCOPE),patch.object(runtime_children,'reason',rehearsal_reason):
                try:
                    execute('Smoke',root,scratch,host,dict(content=[],config=[],descriptors=[]),
                        [sys.executable,'-B',str(policy.TOOLS/'runtime_standin.py'),'--root',str(root),'--case',case],
                        ledger,3 if case=='deadline' else 15,synthetic=True)
                    if case=='pass':read_leg('Smoke',root)
                except Exception as exc:error=str(exc)
            expected={'fail':'Child exit 7','deadline':'Session limit','content':'FB2 protected','ubt-build':'FD2 UBT build'}
            assert (error is None if case=='pass' else expected[case] in (error or '')),(case,error)
            child=json.loads((root/'child-001.json').read_text())
            stopped=child.get('stopped_owned_processes',[])
            assert sibling.poll() is None and sibling.pid not in [r['pid'] for r in stopped]
            assert (root/'standin-evidence.txt').read_text().startswith('Evidence must survive')
            if case=='deadline':
                detached=json.loads((root/'detached-pid.json').read_text())['pid']
                assert detached in [r['pid'] for r in stopped]
            rows.append(dict(case=case,result='PASS',expected_stop=error,receipt=str(root/'child-001.json'),
                             evidence_kept=True,sibling_survived=True))
    finally:
        os.environ['EUROPA_OPERATION_DIR']=previous
        if sibling.poll() is None:sibling.terminate()
        sibling.wait()
    result=dict(result='PASS',cases=rows,unreal_launches=0,roots=[str(output),str(temp)],
                real_host_and_slots_untouched=True,common_path='runtime_runner.execute / write_ledger.run_child / runtime_monitor.Monitor')
    (output/'summary.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(result='PASS',cases=len(rows),unreal_launches=0)))


if __name__=='__main__':main(Path(sys.argv[1]))
