import copy
import json
import os
from pathlib import Path
import subprocess
import sys
import time
from unittest.mock import patch

from environment import execution_env
import fixture_policy as policy
from fixture_children import BuildOutputWatch
from fixture_session_gate import SessionGate,GIB
from idle_gate import FLOORS


def main(output):
    import write_ledger as ledger
    output=policy.canonical(output,ledger.JOB)
    output.mkdir(parents=True,exist_ok=False)
    temp=policy.ROOT/'Temp/09426b/fd-rehearsal'/output.name
    temp.mkdir(parents=True,exist_ok=False)
    old_operation=os.environ['EUROPA_OPERATION_DIR']
    env,_=execution_env(os.environ)
    sibling=subprocess.Popen([sys.executable,'-B','-c','import time;time.sleep(120)'],env=env)
    rows=[]
    try:
        for action in ('query','validate','build','triple','write','commit'):
            folder=output/action;folder.mkdir()
            scratch=temp/action;scratch.mkdir()
            os.environ['EUROPA_OPERATION_DIR']=str(folder)
            watcher=BuildOutputWatch(scratch/'host',folder/'build-watch.jsonl')
            began=[None]
            def limits():
                low=action=='commit' and began[0] is not None and time.monotonic()-began[0]>.8
                return dict(free_commit_bytes=4*GIB-(1 if low else 0),
                            volumes={d:dict(free_bytes=n,resolved_drive=d) for d,n in FLOORS.items()})
            error=None
            with patch.object(policy,'REHEARSAL',dict(changed_checks_only=True)), SessionGate(folder/'session.jsonl',
                    collector=lambda:{},limits_reader=limits,build_check=watcher.check) as monitor:
                def started(pid):began[0]=time.monotonic();monitor.start(pid)
                try:
                    ledger.run_child([sys.executable,'-B',str(policy.TOOLS/'fixture_fd_standin.py'),
                        '--scratch',str(scratch),'--action',action],runtime_guard=True,timeout_seconds=15,
                        echo=False,on_started=started,running_check=monitor.check)
                    watcher.check()
                except RuntimeError as exc:error=str(exc)
            expected=action not in ('query','validate')
            receipt=json.loads((folder/'child-001.json').read_text())
            stopped=receipt.get('stopped_owned_processes',[])
            expected_reason={'build':'FD2 UBT build invocation','triple':'FD2 UBT build invocation',
                             'write':'FD2 write to build outputs','commit':'free commit below 4 GiB'}.get(action)
            assert bool(error)==expected,(action,error)
            if expected:
                assert expected_reason in error,(action,error)
                assert stopped,(action,'owned tree not stopped')
            assert sibling.poll() is None and sibling.pid not in [r['pid'] for r in stopped]
            polls=[json.loads(s) for s in (folder/'child-001-process-polls.jsonl').read_text().splitlines()]
            argv=[p['command_line'] for row in polls for p in row['processes'] if '--fc6-ubt-standin' in (p.get('command_line') or '')]
            assert argv,(action,'missing child argv')
            rows.append(dict(case=action,result='EXPECTED_STOP' if expected else 'PASS',reason=error,
                             sibling_survived=True,receipt=str(folder/'child-001.json'),argv=sorted(set(argv))))
            ledger.dump(output/'progress.json',rows)
    finally:
        os.environ['EUROPA_OPERATION_DIR']=old_operation
        if sibling.poll() is None:sibling.terminate()
        sibling.wait()
    ledger.dump(output/'summary.json',dict(result='PASS',cases=rows,unreal_launches=0,dry_run_sessions=0,
        scratch_roots=[str(output),str(temp)],changed_checks_only=True))
    print(json.dumps(dict(result='PASS',cases=len(rows),unreal_launches=0)))


if __name__=='__main__':main(Path(sys.argv[1]))
