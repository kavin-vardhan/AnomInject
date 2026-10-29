import copy
import ctypes
import hashlib
import io
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import time
from contextlib import redirect_stdout
from unittest.mock import patch

from environment import execution_env, execution_boundary, fingerprints, decode, execution_command
import fixture_policy as policy
from fixture_attempts import Guard
from fixture_wait import waiting_line, wait
from idle_gate import Window, reasons, process_record, self_test as measured_tests
from owner_wait import SyntheticClock
from probe_teardown_check import self_tests as static_tests
from toolchain_environment import compare, selected


def config(template):
    return dict(authorized_by_chat=True,authorization_citation=policy.CITATION,policy_citation=policy.RULE,
        arming_job=policy.ARMING_JOB,amendment_citations=policy.AMENDMENTS,session_policy=policy.SESSION_POLICY,gate_profile='measured',human_idle=True,session_seconds=1800,dry_run_only=False,
        manifests=policy.manifest_rows(),declared_outputs={r:list(map(str,p)) for r,p in policy.OUTPUTS.items()},
        recipes={r:policy.make_recipe(r,policy.ROOT/'Receipts'/policy.ARMING_JOB,policy.ROOT/'Temp'/policy.ARMING_JOB) for r in policy.SESSIONS},
        global_state=str(policy.ROOT/'Receipts'/policy.ARMING_JOB/'fixture-state/global'),
        job_template=dict(template,runtime_authorized=True,runtime_scope=policy.SCOPE,build_pairs=[],query_pairs=[],
            authority=policy.ARMING_JOB+' armed for EW2 and EW3, as amended by EX1-EX5, EY1-EY4 and EZ1-EZ2, FC1-FC7, FD1-FD5',
            memory=dict(start_physical_GiB=6,start_commit_GiB=8,stop_physical_GiB=None,stop_commit_GiB=4)),
        tooling_files=[],host_files=[],engine_files=[],existing_content=[])


def main(output):
    import write_ledger as ledger
    output=policy.canonical(output)
    output.mkdir(parents=True,exist_ok=False)
    temp=policy.ROOT/'Temp'/output.relative_to(policy.ROOT/'Receipts')
    temp.mkdir(parents=True,exist_ok=False)
    rows=[]
    def check(name,condition):
        rows.append(dict(case=name,pass_=bool(condition)))
        (output/'progress.json').write_text(json.dumps(rows,indent=2)+'\n')
        if not condition: raise AssertionError(name)
    def refuses(name,fn):
        try:fn()
        except (RuntimeError,ValueError,FileNotFoundError) as error:
            check(name,bool(str(error)))
            rows[-1]['reason']=str(error)
        else:check(name,False)
    template=json.loads(Path(os.environ['EUROPA_JOB_CONFIG']).read_text())
    real=config(template)
    check('exact_EW2_real_config',policy.validate(real,False)==real['job_template'])
    dry=policy.throwaway(real,output/'dry',temp/'dry')
    check('throwaway_config',policy.validate(dry,True)==dry['job_template'])
    for name,mutate in (
        ('unarmed',lambda d:d.update(authorized_by_chat=False)),
        ('wrong_citation',lambda d:d.update(authorization_citation='synthetic')),
        ('owner_profile',lambda d:d.update(gate_profile='owner-preview')),
        ('human_idle_removed',lambda d:d.update(human_idle=False)),
        ('wrong_limit',lambda d:d.update(session_seconds=999)),
        ('old_manifest',lambda d:d['manifests'][0].update(sha256='synthetic')),
        ('scope_extra_session',lambda d:d['job_template']['runtime_scope'].update(maximum_editor_sessions=4)),
        ('build_scope',lambda d:d['job_template'].update(build_pairs=[['425','StylizedParisStreetEditor','Development']])),
        ('wrong_outputs',lambda d:d['declared_outputs']['EA_Core'].append('D:/bad.uasset')),
        ('real_paths_in_dry_run',lambda d:d.update(recipes=copy.deepcopy(real['recipes']))),
        ('global_lock_overlap',lambda d:d.update(global_state=d['recipes']['EA_Core']['attempt_slots'][0]['job_root'])),
        ('slot_83_alias',lambda d:d['recipes']['EA_Core']['attempt_slots'][0].update(job_root=str(output/'LONGNA~1/slot'))),
        ('duplicate_slots',lambda d:d['recipes']['EA_Core']['attempt_slots'][1].update(job_root=d['recipes']['EA_Core']['attempt_slots'][0]['job_root']))):
        bad=copy.deepcopy(dry);mutate(bad)
        refuses(name,lambda:policy.validate(bad,True))
    stdout=io.StringIO()
    import fixture_leg
    for index,field in enumerate(('authorized_by_chat','human_idle','manifests')):
        bad=copy.deepcopy(dry)
        bad[field]=False if field!='manifests' else []
        path=output/('refusal-%d.json'%index);path.write_text(json.dumps(bad))
        with redirect_stdout(stdout):code=fixture_leg.main(['--config',str(path),'--recipe','EA_Core','--dry-run'])
        check('real_main_specific_refusal_'+field,code==2)
    lines=stdout.getvalue().splitlines()
    check('refusals_have_distinct_visible_reasons',len(lines)==3 and len(set(lines))==3 and all(x.startswith('Stopped: ') for x in lines))
    (output/'specific-refusals.txt').write_text(stdout.getvalue())
    prepared=output/'argv'
    argv=policy.command('EA_Core',prepared)
    check('bound_argv',policy.validate_argv(argv,'EA_Core',prepared))
    for name,mutant in [('missing_EU4',[x for x in argv if x!=policy.UPLOAD]),
        ('conflicting_EU4',argv+[policy.UPLOAD.replace('=False','=True')]),('unattended_unproved',argv+['-unattended']),
        ('wrong_executable',['C:/wrong.exe']+argv[1:]),('cook_added',argv+['-run=cook'])]:
        refuses(name,lambda:policy.validate_argv(mutant,'EA_Core',prepared))
    refuses('short_alias_literal',lambda:policy.canonical(temp/'LONGNA~1/slots'))
    alias_target=temp/'Long directory for canonical path check'
    alias_target.mkdir()
    short=ctypes.create_unicode_buffer(32768)
    count=ctypes.windll.kernel32.GetShortPathNameW(str(alias_target),short,len(short))
    if count and short.value.casefold()!=str(alias_target).casefold():
        refuses('actual_83_alias',lambda:policy.canonical(Path(short.value)))
    else:
        check('83_generation_disabled_literal_still_refused',True)
    alias=temp/'junction-alias';alias.mkdir()
    from types import SimpleNamespace
    original_stat=Path.stat
    def synthetic_reparse(path,*args,**kwargs):
        info=original_stat(path,*args,**kwargs)
        if path==alias and kwargs.get('follow_symlinks') is False:
            values={key:getattr(info,key) for key in dir(info) if key.startswith('st_')}
            values['st_file_attributes'] |= 1024
            return SimpleNamespace(**values)
        return info
    with patch.object(Path,'stat',synthetic_reparse):
        refuses('FA1_simulated_reparse_attribute',lambda:policy.canonical(alias/'slots'))
        aliased=copy.deepcopy(dry)
        aliased['recipes']['EA_Core']['attempt_slots'][0]['temp_root']=str(alias/'slots')
        refuses('FA1_simulated_reparse_in_config',lambda:policy.validate(aliased,True))
    check('FA1_no_reparse_created',not alias.stat(follow_symlinks=False).st_file_attributes&1024)
    (output/'aliases.json').write_text(json.dumps(dict(path=str(alias),method='Mock stat reparse attribute; actual folder remains plain',
        authority='094-25-ruling FA1; no junction or symlink creation',created_reparse_points=0),indent=2))

    package=temp/'synthetic.uasset'
    package.write_bytes(b'synthetic package')
    binding=[dict(path=str(package),size=package.stat().st_size,sha256=policy.sha(package))]
    check('bound_file_accepts_exact_bytes',policy.verify_files(binding)==1)
    bad_binding=copy.deepcopy(binding);bad_binding[0]['sha256']='0'*64
    refuses('bound_file_changed',lambda:policy.verify_files(bad_binding))
    with patch.object(policy,'OUTPUTS',dict(EA_Core=[package],EA_Travel=[temp/'travel.umap'])):
        refuses('existing_destination_no_overwrite',lambda:policy.output_preflight('EA_Core'))
        refuses('travel_without_core_success',lambda:policy.output_preflight('EA_Travel'))
        sidecar=temp/'travel.uexp';sidecar.write_bytes(b'synthetic sidecar')
        refuses('existing_destination_sidecar',lambda:policy.output_preflight('EA_Travel',dry_run=True))
    with patch.object(policy,'content_snapshot',lambda:binding):
        refuses('undeclared_content_file',lambda:policy.verify_content_after([],'EA_Core'))
    exit_rows=[dict(sequence=1,kind='world_cleanup',actor_snapshot='skipped',skip_reason='engine_exit'),
               dict(sequence=2,kind='probe_stop',root_release='skipped')]
    exit_file=output/'synthetic-exit.jsonl'
    def write_exit(rows):
        exit_file.write_text(''.join(json.dumps(r)+'\n' for r in rows))
        return fixture_leg.probe_exit(exit_file)
    check('probe_clean_exit_rows',write_exit(exit_rows)['probe_stop']==1)
    for name,mutate in (
        ('cleanup_wrong_reason',lambda d:d[0].update(skip_reason='garbage_collection')),
        ('root_release_not_skipped',lambda d:d[1].update(root_release='completed')),
        ('torn_sequence',lambda d:d[1].update(sequence=3)),
        ('PIE_row',lambda d:d[0].update(world_type=3)),
        ('missing_probe_stop',lambda d:d.pop()),
        ('row_after_stop',lambda d:d.append(dict(sequence=3,kind='world_cleanup')))):
        bad=copy.deepcopy(exit_rows);mutate(bad)
        refuses('exit_'+name,lambda:write_exit(bad))
    sequence=policy.throwaway(real,output/'sequence',temp/'sequence')
    with Guard(sequence,'EA_Core') as g:
        first,_=g.claim()
    with Guard(sequence,'EA_Core') as g:
        second,_=g.claim()
        check('refused_attempt_leaves_next_usable',(first['number'],second['number'])==(1,2))
        g.reserve_launch(second)
        g.cancel_unstarted(second)
        check('failed_creation_releases_only_own_reservation',not (g.state/'launch-reserved.json').exists())
    with Guard(sequence,'EA_Core') as g:
        third,_=g.claim();check('third_slot',third['number']==3)
        g.reserve_launch(third);g.mark_launched(third,123456)
    with Guard(sequence,'EA_Core') as g:
        refuses('fourth_attempt_after_launched',g.claim)
    existing=policy.throwaway(real,output/'existing',temp/'existing')
    Path(existing['recipes']['EA_Core']['attempt_slots'][0]['job_root']).mkdir(parents=True)
    with Guard(existing,'EA_Core') as g:
        slot,_=g.claim();check('existing_slot_folder_is_not_reused',slot['number']==2)
    good=dict(idle_seconds=300,free_memory_bytes=6*1024**3,free_commit_bytes=8*1024**3,gpu_percent=15,
        processes=[dict(pid=700001,parent_pid=1,image='EpicGamesLauncher.exe',image_path='C:/Program Files (x86)/Epic Games/Launcher/Portal/Binaries/Win64/EpicGamesLauncher.exe')],
        volumes={d:dict(resolved_drive=d,free_bytes=80*1024**3) for d in 'CDE'})
    window=Window(0,profile='measured',require_launcher=True)
    check('three_measured_polls', [window.poll(t,good)['status'] for t in (0,25,50)]==['WAIT','WAIT','CLEAR'])
    missing=copy.deepcopy(good);missing['processes']=[]
    check('launcher_absent_waits','launcher' in reasons(missing,require_launcher=True))
    launchers=copy.deepcopy(good)
    launchers['processes'] += [dict(pid=700002,parent_pid=700001,image='UnrealCEFSubProcess.exe',image_path='C:/Program Files (x86)/Epic Games/Launcher/Engine/Binaries/Win64/UnrealCEFSubProcess.exe'),dict(pid=700003,parent_pid=700001,image='EpicWebHelper.exe',image_path='C:/Program Files (x86)/Epic Games/Launcher/Engine/Binaries/Win64/EpicWebHelper.exe')]
    check('launcher_family_recorded_but_not_blocking',not reasons(launchers,require_launcher=True) and len(launchers['processes'])==3)
    for image,cmd in [('UnrealBuildTool.exe',''),('UnrealHeaderTool.exe',''),('dotnet.exe','dotnet E:/Engine/UnrealBuildTool.dll'),('UnrealEditor.exe','')]:
        bad=copy.deepcopy(good);bad['processes'].append(process_record(dict(pid=710000,parent_pid=1,image=image,image_path='D:/Synthetic/'+image,command_line=cmd)))
        check('foreign_'+image+'_'+str(len(cmd)),any(x.startswith('foreign_process:') for x in reasons(bad,require_launcher=True)))
    for field,value,reason in [('idle_seconds',299,'input_idle'),('free_memory_bytes',1,'physical_memory'),('gpu_percent',16,'gpu'),('gpu_percent',None,'gpu:unknown')]:
        bad=copy.deepcopy(good);bad[field]=value
        check('measured_'+reason,reason in reasons(bad,require_launcher=True))
    unmet=['input_idle','physical_memory','gpu:unknown','processes:unknown','launcher','foreign_process:42','disk:D']
    line=waiting_line(unmet)
    check('waiting_names_each_unmet_condition',all(x in line for x in ('300','6 GiB','GPU','running-program','Launcher','PID 42','disk D:')))
    clock=SyntheticClock();messages=[]
    result=wait(output/'wait-timeout.jsonl',collector=lambda:missing,clock=clock.now,sleep=clock.sleep,emit=messages.append)
    check('four_hour_give_up',result['status']=='NEEDS-DECISION' and clock.now()==14400)
    check('one_wait_message_per_minute',len([x for x in messages if x.startswith('Waiting:')])<=241)
    base=dict(os.environ,EUROPA_BOOTSTRAP_PROOF='synthetic',EUROPA_JOB_CONFIG='synthetic',EUROPA_OPERATION_DIR='synthetic',EUROPA_BUILD_CHILDREN='1',EUROPA_FUTURE_METADATA='synthetic',IA_CANARY_TOKEN='synthetic-canary')
    clean,boundary=execution_env(base)
    check('all_wrapper_metadata_and_canary_removed',not any(k.startswith('EUROPA_') or k=='IA_CANARY_TOKEN' for k in clean))
    check('all_other_base_values_unchanged',all(clean[k.upper()]==v for k,v in os.environ.items() if k.upper() in clean))
    for key in ('EUROPA_BOOTSTRAP_PROOF','EUROPA_FUTURE_METADATA','IA_CANARY_TOKEN'):
        bad=dict(clean,**{key:'synthetic'})
        refuses('execution_boundary_'+key,lambda:execution_boundary(base,bad))
    for host in ('425','51port'):
        native=dict(clean,PATH=';'.join(selected(host)['prefix'])+';'+clean['PATH'])
        def pack(values):
            chunks=[struct.pack('<i',len(values))]
            for pair in values.items():
                for value in pair:
                    raw=value.encode('utf-8');chunks += [struct.pack('<i',len(raw)),raw]
            return b''.join(chunks)
        check(host+'_EN1_synthetic_makefile',compare(fingerprints(clean),decode(pack(native))['environment'],selected(host))['result']=='PASS')
        for key in ('EUROPA_BOOTSTRAP_PROOF','EUROPA_FUTURE_METADATA','IA_CANARY_TOKEN'):
            bad=dict(native,**{key:'synthetic'})
            check(host+'_native_refuses_'+key,compare(fingerprints(clean),decode(pack(bad))['environment'],selected(host))['result']=='STOP')
        bad=dict(native,PATH='C:/foreign;'+native['PATH'])
        check(host+'_foreign_path_refused',compare(fingerprints(clean),bad,selected(host))['result']=='STOP')
    check('native_commands_identified',all(execution_command(x) for x in ([r'E:\Engine\UnrealBuildTool.exe'],['dotnet.exe','UnrealBuildTool.dll'],['UE4Editor.exe'],['UnrealHeaderTool.exe'])))
    sources={p.name:p.read_text() for p in (policy.HOST/'Source/EuropaPhaseAProbe').glob('*') if p.suffix in ('.cpp','.h')}
    static=static_tests(sources)
    check('static_real_and_32_mutants',len(static)==33 and all(r['pass_'] for r in static))
    (output/'static-tests.json').write_text(json.dumps(static,indent=2)+'\n')
    measured=measured_tests();check('measured_regression',measured['result']=='PASS')
    (output/'measured-tests.json').write_text(json.dumps(measured,indent=2)+'\n')
    sibling=subprocess.Popen([sys.executable,'-B','-c','import time; time.sleep(60)'],env=clean)
    old_operation=os.environ['EUROPA_OPERATION_DIR']
    try:
        child_output=output/'deadline';child_output.mkdir()
        os.environ['EUROPA_OPERATION_DIR']=str(child_output)
        code="import subprocess,sys,time; p=subprocess.Popen([sys.executable,'-B','-c','import time;time.sleep(60)']);print(p.pid,flush=True);time.sleep(60)"
        refuses('owned_tree_deadline',lambda:ledger.run_child([sys.executable,'-B','-c',code],runtime_guard=True,timeout_seconds=2,echo=False))
        receipt=json.loads((child_output/'child-001.json').read_text())
        stopped=receipt['stopped_owned_processes']
        check('deadline_stops_child_and_descendant',len(stopped)>=2)
        check('deadline_leaves_sibling_running',sibling.poll() is None and sibling.pid not in [r['pid'] for r in stopped])
        check('runtime_child_environment_has_no_metadata',not any(k.startswith('EUROPA_') or k=='IA_CANARY_TOKEN' for k in receipt['environment']))
        post=policy.throwaway(real,output/'post-create',temp/'post-create')
        with Guard(post,'EA_Core') as g:
            slot,_=g.claim()
            def failed(pid):
                g.mark_launched(slot,pid)
                raise OSError('synthetic post-create receipt failure')
            try:
                ledger.run_child([sys.executable,'-B','-c','import time;time.sleep(60)'],runtime_guard=True,timeout_seconds=2,echo=False,
                    before_create=lambda:g.reserve_launch(slot),on_started=failed,on_creation_failed=lambda:g.cancel_unstarted(slot))
            except OSError: pass
            else: raise AssertionError('Post-create injection did not fail')
            refuses('post_create_failure_keeps_launch_lock',g.check_unused_launch)
        creation=policy.throwaway(real,output/'creation-failed',temp/'creation-failed')
        with Guard(creation,'EA_Core') as g:
            slot,_=g.claim()
            refuses('creation_failure',lambda:ledger.run_child([str(temp/'missing.exe')],runtime_guard=True,timeout_seconds=2,
                before_create=lambda:g.reserve_launch(slot),on_creation_failed=lambda:g.cancel_unstarted(slot)))
            check('creation_failure_keeps_next_slot_usable',g.claim()[0]['number']==2)
    finally:
        os.environ['EUROPA_OPERATION_DIR']=old_operation
        if sibling.poll() is None:sibling.terminate()
        sibling.wait()
    parser_code='import sys;compile(open(sys.argv[1],"rb").read(),sys.argv[1],"exec");print("PY27_PARSE_PASS")'
    parser=subprocess.run([r'E:\EpicGames\UE_4.25\Engine\Binaries\ThirdParty\Python\Win64\python.exe','-B','-c',parser_code,str(policy.TOOLS/'author_425_fixture.py')],env=clean,capture_output=True,text=True)
    check('Python27_recipe_syntax',parser.returncode==0 and 'PY27_PARSE_PASS' in parser.stdout)
    (output/'python27-parser.json').write_text(json.dumps(dict(exit_code=parser.returncode,stdout=parser.stdout,stderr=parser.stderr,environment_boundary=boundary,authoring_called=False),indent=2))
    (output/'samples.json').write_text(json.dumps([good,good,good],indent=2)+'\n')
    result=dict(result='PASS',cases=rows,static_cases=len(static),measured_cases=len(measured['cases']),unreal_launches=0,
                harmless_children_only=True,real_slots_untouched=True)
    (output/'result.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(result='PASS',cases=len(rows),static_cases=len(static),unreal_launches=0)))


if __name__=='__main__':
    main(Path(sys.argv[1]))
