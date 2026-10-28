import copy
import json
import os
from pathlib import Path
import threading
import time

from fixture_session_gate import SessionGate


def tests(output):
    import write_ledger as ledger
    import fixture_policy as policy
    from fixture_attempts import Guard
    from fixture_self_tests import config
    output=Path(output);output.mkdir(exist_ok=False)
    good=json.loads((policy.ROOT/'Receipts/09422/self-tests/attempt-2/samples.json').read_text())[0]
    rows=[]
    def check(name,ok):
        rows.append(dict(case=name,passed=bool(ok)))
        assert ok,name
    def sample_case(name,sample,expected,owned=()):
        with SessionGate(output/(name+'.jsonl')) as monitor:
            monitor.results.put(dict(sample=sample,collected=time.monotonic()))
            try:monitor.check(owned)
            except RuntimeError:actual=False
            else:actual=True
            check(name,actual==expected)
    sample_case('good',good,True)
    for name,mutate in (
        ('human_input',lambda d:d.update(idle_seconds=299)),
        ('physical_memory',lambda d:d.update(free_memory_bytes=1)),
        ('GPU',lambda d:d.update(gpu_percent=16)),
        ('unknown_GPU',lambda d:d.update(gpu_percent=None)),
        ('unknown_processes',lambda d:d.update(processes=None)),
        ('launcher_gone',lambda d:d.update(processes=[])),
        ('foreign_UBT',lambda d:d['processes'].append(dict(pid=888888,parent_pid=1,image='UnrealBuildTool.exe'))),
        ('D_floor',lambda d:d['volumes']['D'].update(free_bytes=24*1024**3))):
        bad=copy.deepcopy(good);mutate(bad);sample_case(name,bad,False)
    owned=copy.deepcopy(good);owned['processes'].append(dict(pid=888888,parent_pid=1,image='UE4Editor.exe'))
    sample_case('owned_editor_excluded',owned,True,(888888,))
    sample_case('foreign_editor_refused',owned,False)
    with SessionGate(output/'stale.jsonl',clock=lambda:76) as monitor:
        monitor.last=0
        try:monitor.check(())
        except RuntimeError:check('stale_reading_refused',True)
        else:check('stale_reading_refused',False)
        monitor.last=None
    def throws():raise OSError('synthetic collector failure')
    with SessionGate(output/'collector-error.jsonl',collector=throws) as monitor:
        monitor.start(999999)
        until=time.monotonic()+2
        while monitor.results.empty() and time.monotonic()<until:time.sleep(.01)
        try:monitor.check(())
        except RuntimeError:check('unexpected_collector_exception_fails_closed',True)
        else:check('unexpected_collector_exception_fails_closed',False)
    old=os.environ['EUROPA_OPERATION_DIR']
    try:
        slot_config=config(json.loads((policy.ROOT/'Receipts/09422/job-config.json').read_text()))
        slot_config=policy.throwaway(slot_config,output/'slots',policy.ROOT/'Temp'/output.relative_to(policy.ROOT/'Receipts')/'slots')
        child=output/'loss-child';child.mkdir()
        os.environ['EUROPA_OPERATION_DIR']=str(child)
        bad=copy.deepcopy(good);bad['free_memory_bytes']=1
        with Guard(slot_config,'EA_Core') as guard:
            slot,_=guard.claim()
            try:
                with SessionGate(output/'real-loop-loss.jsonl',collector=lambda:bad) as monitor:
                    def launched(pid):
                        guard.mark_launched(slot,pid);monitor.start(pid)
                    ledger.run_child([__import__('sys').executable,'-B','-c','import time;time.sleep(60)'],
                        runtime_guard=True,timeout_seconds=5,echo=False,running_check=monitor.check,
                        before_create=lambda:guard.reserve_launch(slot),on_started=launched,
                        on_creation_failed=lambda:guard.cancel_unstarted(slot))
            except RuntimeError as error:
                check('lost_gate_stops_harmless_child','Measured gate lost' in str(error))
            else:check('lost_gate_stops_harmless_child',False)
            try:guard.check_unused_launch()
            except RuntimeError:check('lost_gate_keeps_launch_spent',True)
            else:check('lost_gate_keeps_launch_spent',False)
        receipt=json.loads((child/'child-001.json').read_text())
        check('loss_stop_owned_receipt',len(receipt['stopped_owned_processes'])==1)
        deadline=output/'blocked-collector';deadline.mkdir()
        os.environ['EUROPA_OPERATION_DIR']=str(deadline)
        release=threading.Event()
        def blocked():release.wait(5);return good
        started=time.monotonic()
        with SessionGate(output/'blocked-reader.jsonl',collector=blocked) as monitor:
            try:
                ledger.run_child([__import__('sys').executable,'-B','-c','import time;time.sleep(60)'],
                    runtime_guard=True,timeout_seconds=2,echo=False,running_check=monitor.check,on_started=monitor.start)
            except RuntimeError as error:
                elapsed=time.monotonic()-started
                check('blocked_collector_cannot_delay_deadline','Session limit reached' in str(error) and elapsed<4)
            else:check('blocked_collector_cannot_delay_deadline',False)
            finally:release.set()
    finally:os.environ['EUROPA_OPERATION_DIR']=old
    (output/'result.json').write_text(json.dumps(dict(result='PASS',cases=rows,unreal_launches=0),indent=2)+'\n')
    return rows


if __name__=='__main__':
    import sys
    print(len(tests(sys.argv[1])))
