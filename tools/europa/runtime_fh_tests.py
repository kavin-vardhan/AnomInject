import copy
import json
from pathlib import Path
import subprocess
from unittest.mock import patch

from owner_attempts import create_json
from runtime_evidence import outcome,freeze,ensure_frozen
from runtime_monitor import Monitor,stop_reasons,GIB,validity
from runtime_owned import monitor_matches,environment_names
import runtime_policy as policy


def main(output):
    output=policy.canonical(output,policy.JOB/'scratch');output.mkdir(parents=True,exist_ok=False)
    cases=[]
    def check(name,value):
        assert value,name
        cases.append(dict(case=name,result='PASS'))
    process=dict(result='PROCESS_PASS',exit_code=0,launches=1)
    passed=dict(result='PASS');uploads=dict(result='PASS',logs=[])
    driver=dict(result='SMOKE_PASS',module_alive=True,bindings=[dict(present=True)])
    def classify(**kw):
        args=dict(leg='Load',execution=process,protection=passed,upload=uploads,reader=passed)
        args.update(kw);return outcome(**args)
    check('normal_pass',classify()['result']=='PASS')
    for name,change in (
        ('reader_failure',dict(reader=dict(result='FAIL',reason='known test mismatch'))),
        ('normal_ensure_folder',dict(crashes=['synthetic/Saved/Crashes/ensure'])),
        ('deadline_owned_stop',dict(execution=dict(process,result='SESSION_STOP',exit_code=125,stopped_owned_processes=[dict(pid=1)],reason='Session limit'))),
        ('memory_owned_stop',dict(execution=dict(process,result='SESSION_STOP',exit_code=125,stopped_owned_processes=[dict(pid=1)],reason='FH7 free commit below 4 GiB'))),
        ('drive_owned_stop',dict(execution=dict(process,result='SESSION_STOP',exit_code=125,stopped_owned_processes=[dict(pid=1)],reason='disk D below floor')))):
        result=classify(**change);check(name,result['result']=='FAIL' and not result['end_run'])
    for name,change in (
        ('abnormal_exit',dict(execution=dict(process,result='SESSION_STOP',exit_code=7))),
        ('protected_change',dict(protection=dict(result='FAIL'))),
        ('upload',dict(upload=dict(result='FAIL',logs=[dict(hits=[dict(line=1,text='POST synthetic')])]))),
        ('canary_hit',dict(raw=dict(canary_hit_paths=['synthetic']))),
        ('smoke_missing_binding',dict(leg='Smoke',driver=dict(driver,bindings=[dict(present=False)]))),
        ('smoke_nonzero',dict(leg='Smoke',driver=driver,execution=dict(process,exit_code=7))),
        ('smoke_no_start',dict(leg='Smoke',driver=driver,execution=dict(result='SESSION_STOP',launches=0)))):
        check(name,classify(**change)['end_run'])
    check('smoke_reader_failure_continues',not classify(leg='Smoke',driver=driver,reader=dict(result='FAIL',reason='reader test'))['end_run'])
    for reason in ('FB2 protected file changed during session','FD2 write to build outputs; only the owned tree is stopped.','FH3 upload sign during session'):
        stopped=dict(process,result='SESSION_STOP',exit_code=125,stopped_owned_processes=[dict(pid=1)],reason=reason)
        check('observed_stop_retained_'+reason,classify(execution=stopped)['end_run'])
    check('exact_monitor_pid',monitor_matches('arbitrary.exe -MONITOR=123',123))
    for command in ('CrashReportClientEditor.exe','x -MONITOR=1234','x -MONITOR=0123','x -MONITOR=123 -MONITOR=123','x --MONITOR=123','x -foo=-MONITOR=123'):
        check('not_adopted_'+command,not monitor_matches(command,123))
    check('commit_4_inclusive',not stop_reasons(dict(free_commit_bytes=4*GIB)))
    check('commit_below_4_stop',bool(stop_reasons(dict(free_commit_bytes=4*GIB-1))))
    check('commit_5_continues',not stop_reasons(dict(free_commit_bytes=5*GIB)))
    check('policy_8_6_4',policy.MEMORY==dict(start_physical_GiB=6,start_commit_GiB=8,stop_physical_GiB=None,stop_commit_GiB=4))
    for ended in (False,True):
        root=output/('close-race-'+str(ended));root.mkdir()
        monitor=Monitor(root,root/'host','Load',dict(content=[],config=[],descriptors=[]),game=False,synthetic=True)
        def failing_counter(command):
            if ended:monitor.process_exited(0)
            else:monitor.done.set()
            raise subprocess.CalledProcessError(1,['synthetic-counter'])
        # A collector close race is excluded only after an explicitly recorded root exit.
        with patch('runtime_monitor.idle_gate.powershell_json',side_effect=failing_counter):monitor.observe()
        sample=monitor.queue.get_nowait()
        if ended:check('close_race_separate',sample.get('kind')=='collector_close_race')
        else:check('live_collector_error_retained',sample.get('kind')!='collector_close_race' and sample.get('error')=='CalledProcessError')
        monitor.close()
    check('in_event_unreadable_invalid',not validity([dict(started_qpc=0,ended_qpc=10,last_input_qpc=-1,error='CalledProcessError')],1,9)['valid'])
    root=output/'raw';(root/'session').mkdir(parents=True);file=root/'session/probe.jsonl';file.write_text('real-file-copy-test')
    seal=freeze(root);check('raw_sealed',ensure_frozen(root/'raw-evidence.json')==1)
    file.write_text('mutated owned scratch')
    try:ensure_frozen(root/'raw-evidence.json')
    except RuntimeError:check('raw_mutation_refused',True)
    else:raise AssertionError('Raw mutation admitted')
    import os
    from build_memory import OwnedTree
    tree=OwnedTree(os.getpid())
    try:
        names=environment_names(os.getpid(),tree.known[os.getpid()]['created_filetime'])
        check('native_environment_names_readable',names['result']=='readable' and names['canary_name_present'] is False and not names['values_recorded'])
        check('native_environment_systemroot_control',any(n.upper()=='SYSTEMROOT' for n in names['names']))
    finally:tree.close()
    create_json(output/'summary.json',dict(result='PASS',cases=cases,unreal_launches=0))
    print(json.dumps(dict(result='PASS',cases=len(cases))))


if __name__=='__main__':
    import sys
    main(Path(sys.argv[1]))
