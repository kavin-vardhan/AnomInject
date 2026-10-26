import argparse
import contextlib
import copy
import io
import json
import os
from pathlib import Path
import subprocess
import sys
import threading
from unittest.mock import patch

import idle_gate as idle
import owner_leg
import owner_wait
from owner_attempts import AUTHORITY, AMENDMENT, ARMING_JOB, RUNTIME_SCOPE, Guard, validate
from owner_wait import SyntheticClock, wait_idle
from prepare_runtime import owned


def fixture(root):
    return dict(host='425', kind='editor-preview-owner-leg', authorized_by_chat=True,
        authorization_citation=AUTHORITY, authorization_amendment=AMENDMENT, arming_job=ARMING_JOB,
        dry_run_only=True, gate_profile='owner-preview', human_idle=False, state_root=str(root/'state'),
        job_template=dict(runtime_authorized=True, runtime_scope=copy.deepcopy(RUNTIME_SCOPE), build_pairs=[], query_pairs=[]),
        attempt_slots=[dict(number=i, job_root=str(root/f'attempt-{i}'),
            temp_root=str(Path('D:/IA_Europa/Temp/09417/eq-tests')/root.name/f'attempt-{i}')) for i in (1,2,3)])


def tests(output):
    import write_ledger as ledger
    import compile_children
    from owner_kit_self_tests import tests as prior_tests
    output = owned(output)
    scratch = output.parent/(output.stem+'-scratch')
    scratch.mkdir(parents=True, exist_ok=False)
    cases, children = [], []
    def check(name, condition):
        cases.append(dict(case=name, result='PASS' if condition else 'FAIL'))
        if not condition:
            raise AssertionError(name)
    def refuses(name, callback):
        try:
            callback()
        except RuntimeError:
            check(name, True)
        else:
            check(name, False)
    good = dict(idle_seconds=300, free_memory_bytes=6*idle.GIB, gpu_percent=15, processes=[],
        volumes={d:dict(resolved_drive=d,free_bytes=n) for d,n in idle.FLOORS.items()})
    readers = dict(input_idle=lambda:good['idle_seconds'],physical_memory=lambda:good['free_memory_bytes'],
        gpu=lambda:good['gpu_percent'],processes=lambda:[],
        **{'disk:'+d:(lambda d=d: good['volumes'][d]) for d in idle.FLOORS})
    conditions = dict(physical_memory='free memory',gpu='GPU use',processes='running programs',
        **{'disk:'+d:'free space on '+d for d in idle.FLOORS})
    for name,label in conditions.items():
        for error_type in (subprocess.CalledProcessError,subprocess.TimeoutExpired):
            key = name.replace(':','-')+'-'+error_type.__name__
            def fail(kind=error_type):
                if kind is subprocess.CalledProcessError:
                    raise kind(7, ['synthetic-reader'])
                raise kind(['synthetic-reader'], 20)
            broken = dict(readers, **{name:fail})
            sample = idle.collect(broken)
            check(key+'_condition_unknown', name+':unknown' in idle.reasons(sample,profile='owner-preview'))
            check(key+'_other_readings_retained', set(sample['reading_errors'])=={name})
            sequence = iter([good,good,sample,good,good,good])
            clock, lines = SyntheticClock(), []
            receipt = scratch/(key+'.jsonl')
            result = wait_idle(receipt, collector=lambda:next(sequence),clock=clock.now,
                sleep=clock.sleep,emit=lines.append)
            rows = [json.loads(line) for line in receipt.read_text().splitlines()]
            check(key+'_resets_three_passes', rows[2]['result']['consecutive']==0 and result['elapsed_seconds']==125)
            check(key+'_cannot_read_notice', any('cannot read '+label in line for line in lines))
    clock, lines = SyntheticClock(), []
    def collector_failure():
        raise subprocess.TimeoutExpired(['synthetic-collector'],20)
    result = wait_idle(scratch/'collector-timeout.jsonl',collector=collector_failure,
        clock=clock.now,sleep=clock.sleep,emit=lines.append)
    check('whole_collector_timeout_waits_45_minutes', result['status']=='TIMEOUT' and result['elapsed_seconds']==2700)
    check('whole_collector_names_all_unknown', all('cannot read '+label in lines[0] for label in conditions.values()))
    check('whole_collector_one_line_per_minute', len([s for s in lines if s.startswith('Waiting:')])==45)

    error_root = scratch/'unexpected'
    error_root.mkdir()
    def unexpected_run(*args, context, **kwargs):
        context['attempt_root'] = error_root
        clock = SyntheticClock()
        def unexpected():
            raise ZeroDivisionError('synthetic unexpected collector bug')
        wait_idle(error_root/'wait.jsonl',collector=unexpected,clock=clock.now,sleep=clock.sleep,emit=lambda line:None)
    console = io.StringIO()
    with patch.object(owner_leg,'run',unexpected_run), contextlib.redirect_stdout(console):
        code = owner_leg.main(['--dry-run'])
    details = list(error_root.glob('unexpected-error-*.txt'))
    check('unexpected_exception_plain_top_level_stop', code==2 and len(console.getvalue().splitlines())==1
        and console.getvalue().startswith('Stopped: unexpected error; details saved to ')
        and console.getvalue().rstrip().endswith('No editor was started.') and 'Traceback' not in console.getvalue())
    check('unexpected_exception_full_details_in_attempt', len(details)==1 and 'Traceback' in details[0].read_text()
        and 'synthetic unexpected collector bug' in details[0].read_text())
    console = []
    try:
        raise RuntimeError('synthetic-value-for-redaction')
    except RuntimeError as error:
        detail = owner_leg.stop_unexpected(error,dict(attempt_root=error_root,process_created=True),
            sensitive=['synthetic-value-for-redaction'],emit=console.append)
    check('post_creation_message_never_claims_no_start', 'An editor may have started.' in console[0] and 'No editor was started.' not in console[0])
    check('details_redact_secret_values', 'synthetic-value-for-redaction' not in detail.read_text() and '<redacted>' in detail.read_text())

    for name in ('UnrealTraceServer.exe','UnrealVersionSelector.exe'):
        record = idle.process_record(dict(pid=99,parent_pid=1,image=name,command_line=''))
        sample = dict(good,processes=[record])
        check(name+'_recorded_owner_ignored', record['image']==name and not idle.reasons(sample,profile='owner-preview'))
        check(name+'_measured_unchanged', not idle.reasons(sample,profile='measured'))
    for image in ('UnrealEditor.exe','UE4Editor-Cmd.exe','UnrealBuildTool.exe','UnrealHeaderTool.exe','ShaderCompileWorker.exe','StackOBot.exe','UnrealTraceServerHelper.exe'):
        sample = dict(good,processes=[idle.process_record(dict(pid=99,parent_pid=1,image=image,command_line=''))])
        check(image+'_owner_still_blocks', bool(idle.reasons(sample,profile='owner-preview')))
    for command in ('dotnet.exe "E:/EpicGames/UE_5.7/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll" Example Win64 Development',
        'dotnet.exe D:\\Engine\\UnrealBuildTool.dll Example', 'dotnet.exe UnrealBuildTool Example'):
        record = idle.process_record(dict(pid=99,parent_pid=1,image='dotnet.exe',command_line=command))
        sample = dict(good,processes=[record])
        check('dotnet_UBT_both_profiles_'+str(len(cases)), all(idle.reasons(sample,profile=p) for p in ('measured','owner-preview')))
        check('command_line_discarded_'+str(len(cases)), 'command_line' not in record)
        check('owned_UBT_descendant_exempt_'+str(len(cases)), not idle.reasons(sample,[1],profile='owner-preview'))
    sample = dict(good,processes=[idle.process_record(dict(pid=99,parent_pid=1,image='dotnet.exe',command_line='dotnet.exe Other.dll'))])
    check('other_dotnet_does_not_block', not idle.reasons(sample,profile='owner-preview'))
    for job in ('09416',ARMING_JOB):
        for field in ('state_root','job_root','temp_root'):
            for relation in ('exact','parent','child'):
                config = fixture(scratch/('paths-'+job+'-'+field+'-'+relation))
                path = Path('D:/IA_Europa')/('Temp' if field=='temp_root' else 'Receipts')/job/('owner-state' if field=='state_root' else 'owner-attempts/01')
                if relation=='parent':
                    path = path.parent
                elif relation=='child':
                    path = path/'nested'
                if field=='state_root':
                    config[field] = str(path)
                else:
                    config['attempt_slots'][0][field] = str(path)
                refuses('real_'+job+'_'+field+'_'+relation+'_dry_refused',lambda config=config:validate(config,True))
    config = fixture(scratch/'superseded')
    refuses('old_arming_generation_refused',lambda:validate(dict(config,arming_job='09416'),True))
    refuses('amendment_missing_refused',lambda:validate(dict(config,authorization_amendment=None),True))
    check('throwaway_dry_config_valid', validate(config,True)['runtime_scope']==RUNTIME_SCOPE)
    for field in ('state_root','job_root','temp_root'):
        config = fixture(scratch/('resolved-'+field))
        path = Path('D:/IA_Europa')/('Temp' if field=='temp_root' else 'Receipts')/ARMING_JOB/'alias/..'/('owner-state' if field=='state_root' else 'owner-attempts/01')
        if field=='state_root':
            config[field]=str(path)
        else:
            config['attempt_slots'][0][field]=str(path)
        refuses('resolved_real_'+field+'_dry_refused',lambda config=config:validate(config,True))

    original_env = dict(os.environ)
    original_popen, original_dump = ledger.subprocess.Popen, ledger.dump
    original_thread, original_start = ledger.threading.Thread, ledger.threading.Thread.start
    original_open = Path.open
    failure_points = ('callback_after_mark','pid_receipt','observer_construct','observer_start',
        'console_construct','console_start','console_open','process_poll','observer_result','final_receipt')
    for point in (*failure_points,'process_creation','mark_storage'):
        config = fixture(scratch/('process-'+point))
        folder = scratch/('ledger-'+point)
        folder.mkdir()
        os.environ['EUROPA_OPERATION_DIR'] = str(folder)
        os.environ['EUROPA_BUILD_CHILDREN'] = '1'
        fired, observed = [], []
        created = []
        def trip():
            fired.append(point)
            raise RuntimeError('synthetic '+point)
        def one_trip():
            if not fired:
                trip()
        def marker_exists():
            return (Path(config['state_root'])/'launched.json').is_file()
        def fake_dump(path, value):
            if 'pid' in value:
                observed.append(('receipt',marker_exists()))
                if point=='pid_receipt' and 'ended_utc' not in value:
                    one_trip()
                if point=='final_receipt' and 'ended_utc' in value:
                    one_trip()
            return original_dump(path,value)
        def fake_thread(*args,**kwargs):
            target = kwargs.get('target')
            kind = 'observer' if target is compile_children.observe else 'console'
            observed.append((kind+'_construct',marker_exists()))
            if point==kind+'_construct':
                one_trip()
            thread = original_thread(*args,**kwargs)
            thread.eq_kind = kind
            return thread
        def fake_start(thread):
            observed.append((thread.eq_kind+'_start',marker_exists()))
            if point==thread.eq_kind+'_start':
                one_trip()
            return original_start(thread)
        def fake_open(path,*args,**kwargs):
            if path.name.startswith('child-') and path.suffix=='.log':
                observed.append(('console_open',marker_exists()))
                if point=='console_open':
                    one_trip()
            return original_open(path,*args,**kwargs)
        def fake_observe(pid,stop,result):
            if point=='observer_result':
                result.update(result='FAIL',error='synthetic observer result')
                fired.append(point)
            else:
                stop.wait()
                result['result']='PASS'
        def fake_popen(*args,**kwargs):
            if point=='process_creation':
                trip()
            proc = original_popen(*args,**kwargs)
            created.append(proc)
            real_poll = proc.poll
            def poll():
                observed.append(('process_poll',marker_exists()))
                if point=='process_poll':
                    one_trip()
                return real_poll()
            proc.poll = poll
            return proc
        with Guard(config) as guard:
            slot,_ = guard.claim()
            def started(pid):
                if point=='mark_storage':
                    trip()
                guard.mark_launched(slot,pid)
                if point=='callback_after_mark':
                    trip()
            with contextlib.ExitStack() as stack:
                stack.enter_context(patch.object(ledger.subprocess,'Popen',fake_popen))
                stack.enter_context(patch.object(ledger,'dump',fake_dump))
                stack.enter_context(patch.object(compile_children,'observe',fake_observe))
                stack.enter_context(patch.object(ledger.threading,'Thread',fake_thread))
                stack.enter_context(patch.object(original_thread,'start',fake_start))
                stack.enter_context(patch.object(Path,'open',fake_open))
                try:
                    ledger.run_child([sys.executable,'-B','-c','import time; print("harmless EQ2 child",flush=True); time.sleep(1)'],
                        before_create=lambda:guard.reserve_launch(slot), on_creation_failed=lambda:guard.cancel_unstarted(slot), on_started=started)
                except RuntimeError:
                    pass
                else:
                    raise AssertionError('Injected failure did not propagate: '+point)
            check(point+'_injection_reached', bool(fired))
            check(point+'_no_surviving_child', all(p.returncode is not None for p in created))
            children.extend(dict(pid=p.pid,exit_code=p.returncode,case=point) for p in created)
            if point=='process_creation':
                check('creation_failure_next_slot_usable',guard.claim()[0]['number']==2)
                check('creation_failure_reservation_removed_by_receipt',not (guard.state/'launch-reserved.json').exists()
                    and (Path(slot['job_root'])/'unstarted-reservation-removal.json').is_file())
            else:
                if point=='mark_storage':
                    check('mark_storage_failure_retains_reservation', (guard.state/'launch-reserved.json').is_file())
                else:
                    check(point+'_mark_survives',marker_exists())
                    check(point+'_mark_precedes_all_bookkeeping',all(flag for _,flag in observed))
                refuses(point+'_next_attempt_refused',guard.claim)
    os.environ.clear()
    os.environ.update(original_env)
    prior = prior_tests(scratch/'prior-tests.json')
    check('prior_62_owner_and_48_measured_regression',len(prior['cases'])==62 and len(prior['measured_regression']['cases'])==48)
    result = dict(result='PASS',cases=cases,harmless_children=children,UE_launches=0,
        prior_owner_cases=len(prior['cases']),prior_measured_cases=len(prior['measured_regression']['cases']),
        scope='Synthetic readings through real wait loop; actual harmless Python Popen failure paths; no editor executable used')
    output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    return result


if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    result=tests(args.output)
    print(json.dumps(dict(result=result['result'],cases=len(result['cases']),prior_owner=result['prior_owner_cases'],
        prior_measured=result['prior_measured_cases'],harmless_children=len(result['harmless_children'])+2,UE_launches=0)))
