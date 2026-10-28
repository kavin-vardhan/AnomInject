import argparse
import copy
import json
import os
from pathlib import Path
import subprocess
import sys

from environment import filtered
from idle_gate import FLOORS, GIB, Window, reasons, self_test
from owner_attempts import AUTHORITY, AMENDMENT, ARMING_JOB, RUNTIME_SCOPE, Guard, create_json, validate
from owner_wait import CLEAR_LINE, TIMEOUT_LINE, SyntheticClock, wait_idle, waiting_line
from prepare_runtime import owned


def callback_child():
    import write_ledger as ledger
    seen = []
    ledger.run_child([sys.executable, '-B', '-c', 'print("synthetic callback child")'], on_started=seen.append)
    assert len(seen) == 1 and seen[0] > 0
    def fail(pid):
        seen.append(pid)
        raise RuntimeError('synthetic callback rejection')
    try:
        ledger.run_child([sys.executable, '-B', '-c', 'import time; time.sleep(2)'], on_started=fail)
    except RuntimeError as error:
        assert str(error) == 'synthetic callback rejection'
    else:
        raise AssertionError('Callback failure was ignored')
    rows = [json.loads(p.read_text()) for p in sorted(Path(os.environ['EUROPA_OPERATION_DIR']).glob('child-*.json'))]
    assert len(rows) == 2 and rows[1]['exit_code'] is not None and rows[1]['interruption']['type'] == 'RuntimeError'
    print(json.dumps(dict(result='PASS', callback_pids=seen, harmless_python_children=2, UE_launches=0)))


def tests(output):
    owned(output)
    scratch = output.parent / (output.stem+'-scratch')
    scratch.mkdir(parents=True, exist_ok=False)
    cases = []
    def check(name, passed):
        cases.append(dict(case=name, result='PASS' if passed else 'FAIL'))
        if not passed:
            raise AssertionError(name)
    def rejects(name, fn):
        try:
            fn()
        except (RuntimeError, OSError):
            check(name, True)
        else:
            check(name, False)
    def fixture(name):
        root = scratch / name
        return dict(host='425', kind='editor-preview-owner-leg', authorized_by_chat=True,
            authorization_citation=AUTHORITY, authorization_amendment=AMENDMENT, arming_job=ARMING_JOB, dry_run_only=True, gate_profile='owner-preview', human_idle=False,
            state_root=str(root/'state'), job_template=dict(build_pairs=[], query_pairs=[], runtime_authorized=True,
                runtime_scope=copy.deepcopy(RUNTIME_SCOPE), authority=AUTHORITY, max_parallel_actions=4,
                memory=dict(start_physical_GiB=4, start_commit_GiB=16, stop_physical_GiB=None, stop_commit_GiB=6)),
            attempt_slots=[dict(number=i, job_root=str(root/f'attempt-{i}'),
                temp_root=str(Path('D:/IA_Europa/Temp')/scratch.name/name/f'attempt-{i}')) for i in (1,2,3)])
    config = fixture('authorization')
    check('armed_EP1_dry_run_admitted', validate(config, True)['runtime_scope'] == RUNTIME_SCOPE)
    for key, value in [('authorized_by_chat', False), ('authorization_citation', 'unapproved'), ('host', '51port'),
        ('gate_profile', 'measured'), ('human_idle', True)]:
        rejects('reject_'+key, lambda k=key,v=value: validate(dict(config, **{k:v}), True))
    bad = copy.deepcopy(config)
    bad['job_template']['runtime_scope']['pie'] = True
    rejects('scope_widening_refused', lambda: validate(bad, True))
    rejects('dry_run_cannot_use_live_slots', lambda: validate(dict(config,dry_run_only=False), True))
    rejects('throwaway_slots_cannot_launch', lambda: validate(config, False))
    bad = copy.deepcopy(config)
    bad['attempt_slots'][1]['job_root'] = bad['attempt_slots'][0]['job_root']
    rejects('duplicate_slot_refused', lambda: validate(bad, True))
    config = fixture('ordered')
    for i in (1,2,3):
        with Guard(config) as guard:
            slot, path = guard.claim()
            check('first_unused_slot_'+str(i), slot['number']==i and path.is_file())
            check('fresh_scope_'+str(i), json.loads(path.read_text())['runtime_scope']==RUNTIME_SCOPE)
    with Guard(config) as guard:
        rejects('all_three_used_refuses_fourth', guard.claim)
    config = fixture('preexisting')
    Path(config['attempt_slots'][0]['job_root']).mkdir(parents=True)
    with Guard(config) as guard:
        check('existing_slot_skipped', guard.claim()[0]['number']==2)
    config = fixture('refused')
    with Guard(config) as guard:
        slot, path = guard.claim()
        guard.reserve_launch(slot)
        guard.cancel_unstarted(slot)
        check('unstarted_reservation_receipted', (path.parent/'unstarted-reservation-removal.json').is_file())
    with Guard(config) as guard:
        check('refused_attempt_leaves_next_usable', guard.claim()[0]['number']==2)
    config = fixture('launched')
    with Guard(config) as guard:
        slot, _ = guard.claim()
        guard.reserve_launch(slot)
        guard.mark_launched(slot, 12345)
    with Guard(config) as guard:
        rejects('launched_marker_blocks_second', guard.claim)
        for slot in config['attempt_slots'][1:]:
            Path(slot['job_root']).mkdir()
        rejects('launched_marker_blocks_fourth', guard.claim)
    config = fixture('reservation')
    with Guard(config) as guard:
        slot, _ = guard.claim()
        guard.reserve_launch(slot)
    with Guard(config) as guard:
        rejects('ambiguous_interrupted_start_refuses', guard.claim)
    config = fixture('concurrent')
    with Guard(config):
        rejects('concurrent_double_click_refused', lambda: Guard(config).__enter__())
    with Guard(config) as guard:
        check('mutex_released_without_consuming_slot', guard.claim()[0]['number']==1)
    rejects('measured_cannot_drop_human_idle', lambda: Window(0, profile='measured', human_idle=False))
    rejects('unknown_profile_refused', lambda: Window(0, profile='other'))
    good = dict(idle_seconds=0, free_memory_bytes=6*GIB, free_commit_bytes=12*GIB, gpu_percent=15, processes=[],
        volumes={d:dict(resolved_drive=d,free_bytes=n) for d,n in FLOORS.items()})
    check('owner_recent_input_allowed', not reasons(good, profile='owner-preview'))
    check('measured_recent_input_refused', 'input_idle' in reasons(good))
    bad = dict(good)
    bad.pop('idle_seconds')
    check('owner_input_collector_not_required', not reasons(bad, profile='owner-preview'))
    check('measured_unknown_input_refused', 'input_idle:unknown' in reasons(bad))
    for key, value in [('free_memory_bytes', 6*GIB-1), ('gpu_percent', 15.1), ('processes',None)]:
        check('owner_retains_'+key, bool(reasons(dict(good, **{key:value}), profile='owner-preview')))
    for name in ('UnrealBuildTool.exe','UnrealHeaderTool.exe','UnrealEditor.exe','UE4Editor.exe','ShaderCompileWorker.exe'):
        bad = dict(good, processes=[dict(pid=42,parent_pid=1,image=name,image_path='D:/Synthetic/'+name)])
        check('foreign_'+name+'_refused', bool(reasons(bad, profile='owner-preview')))
        check('own_'+name+'_exempt', not reasons(bad,[42],profile='owner-preview'))
    labels = {'physical_memory':'free memory is below 6 GiB', 'gpu':'GPU use is above 15%',
        'foreign_process:42':'another Unreal or game program is running (PID 42)', 'disk:D':'D: free space is below its safety floor',
        'physical_memory:unknown':'cannot read free memory', 'gpu:unknown':'cannot read GPU use',
        'processes:unknown':'cannot read running programs', 'disk:E:unknown':'cannot read free space on E'}
    for reason, label in labels.items():
        check('waiting_message_'+reason, label in waiting_line(dict(reasons=[reason]),good))
    combined = waiting_line(dict(reasons=list(labels)),good)
    check('waiting_names_every_unmet_condition', all(label in combined for label in labels.values()))
    clock = SyntheticClock()
    notices = []
    busy = dict(good, free_memory_bytes=GIB, gpu_percent=80,
        processes=[dict(pid=42,parent_pid=1,image='UnrealBuildTool.exe',image_path='D:/Synthetic/UnrealBuildTool.exe')])
    result = wait_idle(scratch/'timeout.jsonl', collector=lambda:busy, clock=clock.now, sleep=clock.sleep,
        emit=lambda line:notices.append((clock.now(),line)))
    waiting = [(t,line) for t,line in notices if line.startswith('Waiting:')]
    check('one_plain_wait_line_each_minute', [t for t,_ in waiting] == list(range(0,2700,60)))
    check('each_minute_names_all_three_unmet', all(all(x in line for x in list(labels.values())[:3]) for _,line in waiting))
    check('give_up_exactly_45_minutes', result['status']=='TIMEOUT' and result['elapsed_seconds']==2700 and notices[-1]==(2700,TIMEOUT_LINE))
    clock = SyntheticClock()
    lines = []
    result = wait_idle(scratch/'clear.jsonl', collector=lambda:good, clock=clock.now, sleep=clock.sleep, emit=lines.append)
    check('three_spaced_owner_polls', result['status']=='CLEAR' and result['polls']==3 and result['elapsed_seconds']==50)
    check('clear_console_line', lines[-1]==CLEAR_LINE)
    clock = SyntheticClock()
    lowdisk = copy.deepcopy(good)
    lowdisk['volumes']['D']['free_bytes'] -= 1
    rejects('floor_breach_stops_without_wait', lambda: wait_idle(scratch/'floor.jsonl',collector=lambda:lowdisk,
        clock=clock.now,sleep=clock.sleep,emit=lambda line:None))
    check('floor_stop_immediate', clock.now()==0)
    measured = self_test()
    check('all_existing_measured_tests', all(c['pass'] for c in measured['cases']))
    config = fixture('callback')
    with Guard(config) as guard:
        _, scope = guard.claim()
    folder = scope.parent/'callbacks'
    folder.mkdir()
    env = filtered(os.environ)
    env.update(EUROPA_JOB_CONFIG=str(scope), EUROPA_OPERATION_DIR=str(folder), PYTHONDONTWRITEBYTECODE='1',
        TEMP=str(folder), TMP=str(folder), GIT_OPTIONAL_LOCKS='0')
    code = 'import sys; sys.path.insert(0, '+repr(str(Path(__file__).parent))+'); import owner_kit_self_tests as t; t.callback_child()'
    completed = subprocess.run([sys.executable,'-B','-c',code],env=env,capture_output=True,text=True,check=True)
    (scratch/'callback-console.log').write_text(completed.stdout)
    check('actual_safe_Popen_callback_and_failure_cleanup', completed.returncode==0)
    result = dict(result='PASS',cases=cases,measured_regression=measured,UE_launches=0,
        synthetic_marker_pid=12345,waiting_console_lines=notices,clear_console_lines=lines)
    output.write_text(json.dumps(result,indent=2)+'\n')
    return result


if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    args.output.parent.mkdir(parents=True,exist_ok=True)
    result=tests(args.output)
    print(json.dumps(dict(result=result['result'],cases=len(result['cases']),measured_cases=len(result['measured_regression']['cases']),UE_launches=0)))
