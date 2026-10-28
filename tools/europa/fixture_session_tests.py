import copy
import json
import os
from pathlib import Path
import subprocess
import sys
import threading
import time
from unittest.mock import patch

from fixture_session_gate import SessionGate,stop_reasons,GIB
import fixture_policy as policy
from fixture_attempts import Guard,create_json
from fixture_self_tests import config
from environment import execution_env
from idle_gate import collect,launcher_tree,reasons,process_record,FLOORS
from fixture_quarantine import quarantine


def tests(output):
    import write_ledger as ledger
    import disk_pause
    output=Path(output);output.mkdir(parents=True,exist_ok=False)
    temp=policy.ROOT/'Temp'/output.relative_to(policy.ROOT/'Receipts')
    temp.mkdir(parents=True,exist_ok=False)
    rows=[]
    def check(name,ok):
        rows.append(dict(case=name,passed=bool(ok)))
        (output/'progress.json').write_text(json.dumps(rows,indent=2)+'\n')
        assert ok,name
    def refuses(name,fn):
        try:fn()
        except (RuntimeError,ValueError,OSError) as error:
            check(name,bool(str(error)));rows[-1]['reason']=str(error)
        else:check(name,False)
    template=json.loads(Path(os.environ['EUROPA_JOB_CONFIG']).read_text())
    real=config(template)
    healthy=dict(free_commit_bytes=6*GIB,volumes={d:dict(free_bytes=n,resolved_drive=d) for d,n in FLOORS.items()})
    good=dict(idle_seconds=300,free_memory_bytes=6*GIB,free_commit_bytes=12*GIB,gpu_percent=15,
        processes=[dict(pid=10,parent_pid=1,image='EpicGamesLauncher.exe',created_utc='2026-01-01T00:00:00Z')],
        volumes=copy.deepcopy(healthy['volumes']))
    group=copy.deepcopy(good)
    group['processes'] += [process_record(dict(pid=11,parent_pid=10,image='EOSOverlayRenderer-Win64-Shipping.exe',created_utc='2026-01-01T00:01:00Z')),
        process_record(dict(pid=12,parent_pid=11,image='EOSOverlayRenderer-Win64-Shipping.exe')),
        process_record(dict(pid=13,parent_pid=12,image='UE4Editor.exe')),
        process_record(dict(pid=14,parent_pid=10,image='UnrealCEFSubProcess.exe'))]
    for process in group['processes']:
        process['image_path']=('C:/Program Files (x86)/Epic Games/Launcher/Portal/Binaries/Win64/EpicGamesLauncher.exe' if process['pid']==10 else
            'C:/Program Files (x86)/Epic Games/Epic Online Services/managedArtifacts/synthetic/'+process['image'])
    check('EY2_editor_inside_launcher_tree_blocks',reasons(group,require_launcher=True)==['foreign_process:13'])
    check('EX1_every_descendant_recorded',{p['pid'] for p in group['launcher_tree_excluded']}=={10,11,12,14})
    check('EX1_ancestry_recorded',next(p for p in group['launcher_tree_excluded'] if p['pid']==12)['ancestry']==[12,11,10])
    for image in ('UnrealEditor.exe','OtherGame-Win64-Shipping.exe','UnrealBuildTool.exe','UnrealHeaderTool.exe'):
        foreign=copy.deepcopy(group);foreign['processes'].append(process_record(dict(pid=90,parent_pid=1,image=image)))
        check('EX1_outside_tree_'+image,'foreign_process:90' in reasons(foreign,require_launcher=True))
    foreign=copy.deepcopy(group);foreign['processes'].append(process_record(dict(pid=91,parent_pid=1,image='dotnet.exe',command_line='dotnet E:/Engine/UnrealBuildTool.dll')))
    check('EX1_outside_dotnet_UBT_blocks','foreign_process:91' in reasons(foreign,require_launcher=True))
    no_launcher=copy.deepcopy(group);no_launcher['processes']=no_launcher['processes'][1:]
    check('EX1_absent_launcher_waits','launcher' in reasons(no_launcher,require_launcher=True))
    check('EX1_prior_ancestry_not_cached','foreign_process:11' in reasons(no_launcher,require_launcher=True) and not no_launcher['launcher_tree_excluded'])
    changed=copy.deepcopy(group);changed['processes'][1]['parent_pid']=1
    check('EX1_changed_parent_rechecks_descendants',all('foreign_process:'+str(p) in reasons(changed,require_launcher=True) for p in (11,12,13)))
    reused=copy.deepcopy(group);reused['processes'][0]['created_utc']='2026-01-02T00:00:00Z'
    check('EX1_parent_created_after_child_not_ancestor','foreign_process:11' in reasons(reused,require_launcher=True))
    outside=copy.deepcopy(good);outside['processes'].append(dict(pid=90,parent_pid=1,image='UnrealCEFSubProcess.exe',blocked_runtime=True))
    check('EY3_CEF_is_recorded_only','foreign_process:90' not in reasons(outside,require_launcher=True))
    check('EX2_thresholds_inclusive',not stop_reasons(healthy))
    observe=copy.deepcopy(group)
    observe.update(idle_seconds=0,free_memory_bytes=1,gpu_percent=100)
    observe['processes'].append(dict(pid=90,parent_pid=1,image='UnrealEditor.exe'))
    with SessionGate(output/'observe-only.jsonl',limits_reader=lambda:healthy) as monitor:
        monitor.results.put(dict(sample=observe,collected=time.monotonic()))
        monitor.check(())
    recorded=[json.loads(s) for s in (output/'observe-only.jsonl').read_text().splitlines()]
    notes=next(r for r in recorded if r['kind']=='observe_only')
    check('EX2_own_GPU_physical_memory_input_do_not_stop',notes['action']=='record_only' and all(x in notes['reasons'] for x in ('input_idle','physical_memory','gpu')))
    check('EX2_foreign_process_recorded_not_acting','foreign_process:90' in notes['reasons'])
    for name,mutate in [('commit',lambda s:s.update(free_commit_bytes=6*GIB-1))]+[(d,lambda s,d=d:s['volumes'][d].update(free_bytes=FLOORS[d]-1)) for d in FLOORS]:
        bad=copy.deepcopy(healthy);mutate(bad)
        with SessionGate(output/('limit-'+name+'.jsonl'),limits_reader=lambda:bad) as monitor:
            refuses('EX2_'+name+'_acts',lambda:monitor.check(()))
    for name,observation in [('unavailable',{}),('launcher_closed',dict(good,processes=[]))]:
        with SessionGate(output/(name+'.jsonl'),clock=lambda:1000,limits_reader=lambda:healthy) as monitor:
            monitor.last=0
            monitor.results.put(dict(sample=observation,collected=0))
            monitor.check(())
        check('EX2_'+name+'_and_stale_observation_not_acting',True)
    marker=output/'synthetic-started.json'
    check('EX2_pre_session_does_not_delegate',not disk_pause.authoring_started(marker,123))
    marker.write_text(json.dumps(dict(authority='094-22-ruling EX2',wrapper_pid=123,phase='authoring_started')))
    check('EX2_started_session_delegates_no_pause',disk_pause.authoring_started(marker,123))
    check('EX2_old_wrapper_marker_not_reused',not disk_pause.authoring_started(marker,124))
    source=(policy.TOOLS/'disk_pause.py').read_text()
    check('EX2_outer_pause_requires_not_started',"and not session_started:\n                detected_at" in source)
    import shutil
    with patch.object(ledger,'AUTHORING_STARTED',True),patch.dict(os.environ,dict(EUROPA_DISK_SUPERVISOR_PID='123')),patch.object(ledger.shutil,'disk_usage',return_value=shutil._ntuple_diskusage(100,99,1)),patch.object(disk_pause,'checkpoint',side_effect=AssertionError('No session pause')):
        refuses('EX2_runtime_floor_never_enters_checkpoint',ledger.floors)
    old_operation=os.environ['EUROPA_OPERATION_DIR']
    env,_=execution_env(os.environ)
    sibling=subprocess.Popen([sys.executable,'-B','-c','import time;time.sleep(90)'],env=env)
    code="import subprocess,sys,time; p=subprocess.Popen([sys.executable,'-B','-c','import time;time.sleep(60)']); print(p.pid,flush=True); time.sleep(60)"
    try:
        for condition in ('commit','disk','deadline'):
            folder=output/('owned-'+condition);folder.mkdir()
            os.environ['EUROPA_OPERATION_DIR']=str(folder)
            entered=[None]
            def limits():
                data=copy.deepcopy(healthy)
                if entered[0] is not None and time.monotonic()-entered[0]>1:
                    if condition=='commit':data['free_commit_bytes']=6*GIB-1
                    if condition=='disk':data['volumes']['D']['free_bytes']=FLOORS['D']-1
                return data
            release=threading.Event()
            def blocked():release.wait(60);return good
            slot_config=policy.throwaway(real,folder/'slots',temp/condition)
            with Guard(slot_config,'EA_Core') as guard:
                slot,_=guard.claim()
                with SessionGate(folder/'session.jsonl',collector=blocked,limits_reader=limits) as monitor:
                    def launched(pid):
                        guard.mark_launched(slot,pid);entered[0]=time.monotonic();monitor.start(pid)
                    began=time.monotonic()
                    try:
                        ledger.run_child([sys.executable,'-B','-c',code],runtime_guard=True,timeout_seconds=2 if condition=='deadline' else 10,
                            echo=False,on_started=launched,running_check=monitor.check,
                            before_create=lambda:guard.reserve_launch(slot),on_creation_failed=lambda:guard.cancel_unstarted(slot))
                    except RuntimeError as error:
                        check('EX2_'+condition+'_owned_tree_stop',('Session limit' if condition=='deadline' else 'Authoring session stopped') in str(error))
                    else:check('EX2_'+condition+'_owned_tree_stop',False)
                    finally:release.set()
                    check('EX2_'+condition+'_not_delayed_by_collector',time.monotonic()-began<8)
                refuses('EX2_'+condition+'_retry_refused',guard.check_unused_launch)
            receipt=json.loads((folder/'child-001.json').read_text())
            check('EX2_'+condition+'_descendant_in_stop',len(receipt['stopped_owned_processes'])>=2)
            check('EX2_'+condition+'_sibling_survives',sibling.poll() is None and sibling.pid not in [r['pid'] for r in receipt['stopped_owned_processes']])
    finally:
        os.environ['EUROPA_OPERATION_DIR']=old_operation
        if sibling.poll() is None:sibling.terminate()
        sibling.wait()
    synthetic_host=temp/'quarantine-host'
    fixture=synthetic_host/'Content/EuropaPhaseA';fixture.mkdir(parents=True)
    outputs=dict(EA_Core=[fixture/'SM_EA_TwoLod.uasset',fixture/'EA_Core.umap'],EA_Travel=[fixture/'EA_Travel.umap'])
    preserved=synthetic_host/'Content/existing.uasset';preserved.write_bytes(b'preexisting synthetic content')
    untouched=fixture/'undeclared.bin';untouched.write_bytes(b'undeclared synthetic content stays put')
    travel=outputs['EA_Travel'][0];travel.write_bytes(b'other recipe stays put')
    before=[dict(path=str(p),size=p.stat().st_size,sha256=policy.sha(p)) for p in (preserved,untouched,travel)]
    attempt=output/'quarantine-attempt';attempt.mkdir()
    for p in outputs['EA_Core']:p.write_bytes(b'partial synthetic package')
    create_json(attempt/'prepared.json',dict(recipe='EA_Core',declared_outputs=list(map(str,outputs['EA_Core']))))
    with patch.object(policy,'HOST',synthetic_host),patch.object(policy,'OUTPUTS',outputs):
        q=quarantine('EA_Core',attempt,before,'synthetic failed session')
        check('EX3_two_declared_outputs_quarantined',q['result']=='QUARANTINED' and len(q['moved'])==2)
        check('EX3_each_move_hashed_before_and_after',all(policy.sha(Path(r['destination']))==r['sha256'] and not Path(r['source']).exists() for r in q['moved']))
        check('EX3_every_other_file_unchanged',policy.verify_files(before)==3)
        check('EX3_retry_needs_rearm',q['retry_authorized'] is False and q['launch_lock_unchanged'])
        refuses('EX3_quarantine_never_overwritten',lambda:quarantine('EA_Core',attempt,before,'second call'))
        invalid=output/'quarantine-invalid';invalid.mkdir()
        create_json(invalid/'prepared.json',dict(recipe='EA_Core',declared_outputs=list(map(str,outputs['EA_Core']))))
        refuses('EX3_preexisting_declared_output_refused',lambda:quarantine('EA_Core',invalid,[dict(path=str(outputs['EA_Core'][0]),size=1,sha256='synthetic')],'synthetic'))
        invalid2=output/'quarantine-wrong-declaration';invalid2.mkdir()
        create_json(invalid2/'prepared.json',dict(recipe='EA_Core',declared_outputs=[str(preserved)]))
        refuses('EX3_declaration_escape_refused',lambda:quarantine('EA_Core',invalid2,before,'synthetic'))
        aliased=fixture/'alias';target=temp/'outside-quarantine';target.mkdir()
        made=subprocess.run(['cmd.exe','/d','/c','mklink','/J',str(aliased),str(target)],env=env,capture_output=True)
        check('EX3_synthetic_junction_created',made.returncode==0)
        try:
            bad_outputs=dict(outputs,EA_Core=[aliased/'EA_Core.umap'])
            (target/'EA_Core.umap').write_bytes(b'foreign synthetic target')
            bad=output/'quarantine-alias';bad.mkdir()
            create_json(bad/'prepared.json',dict(recipe='EA_Core',declared_outputs=list(map(str,bad_outputs['EA_Core']))))
            with patch.object(policy,'OUTPUTS',bad_outputs):
                refuses('EX3_source_junction_refused',lambda:quarantine('EA_Core',bad,before,'synthetic'))
            check('EX3_alias_target_untouched',(target/'EA_Core.umap').read_bytes()==b'foreign synthetic target')
        finally:
            assert aliased.parent.resolve()==fixture.resolve() and aliased.resolve()==target.resolve()
            create_json(output/'quarantine-alias-removal.json',dict(path=str(aliased),target=str(target),remove_link_only=True,created_by_this_test=True))
            os.rmdir(aliased)
    bad=copy.deepcopy(real);bad['arming_job']='09422'
    refuses('EX4_old09422_arming_refused',lambda:policy.validate(bad,False))
    check('EX5_job_template_armed_wording','armed for EW2' in real['job_template']['authority'])
    for name,mutate in [('missing_amendments',lambda d:d.pop('amendment_citations')),
        ('old_session_policy',lambda d:d['session_policy'].update(during=['all_measured_conditions'])),
        ('old_template_wording',lambda d:d['job_template'].update(authority='preparation and dry-run only'))]:
        bad=copy.deepcopy(real);mutate(bad)
        refuses('EX5_'+name,lambda:policy.validate(bad,False))
    result=dict(result='PASS',cases=rows,unreal_launches=0,real_content_untouched=True,
        synthetic_quarantine=q,authority='094-22-ruling EX1-EX5')
    (output/'result.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(result='PASS',cases=len(rows),unreal_launches=0)))
    return rows


if __name__=='__main__':tests(sys.argv[1])
