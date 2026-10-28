import copy
import json
from pathlib import Path
import sys

import idle_gate as gate
from fixture_session_gate import SessionGate
from fixture_wait import waiting_line
import fixture_policy as policy
from environment_reads import check as environment_check,self_tests


def tests(output):
    import disk_pause,write_ledger
    output=Path(output);output.mkdir(parents=True,exist_ok=False)
    rows=[]
    def check(name,condition):
        rows.append(dict(case=name,passed=bool(condition)))
        (output/'progress.json').write_text(json.dumps(rows,indent=2)+'\n')
        assert condition,name
    good=dict(idle_seconds=300,free_memory_bytes=6*gate.GIB,free_commit_bytes=12*gate.GIB,gpu_percent=15,
        processes=[dict(pid=1,parent_pid=0,image='EpicGamesLauncher.exe',image_path='C:/Program Files (x86)/Epic Games/Launcher/Portal/Binaries/Win64/EpicGamesLauncher.exe')],
        volumes={d:dict(free_bytes=n,resolved_drive=d) for d,n in gate.FLOORS.items()})
    group=copy.deepcopy(good)
    group['processes'] += [gate.process_record(dict(pid=2,parent_pid=1,image='EpicWebHelper.exe',image_path='C:/Program Files (x86)/Epic Games/Launcher/Engine/Binaries/Win64/EpicWebHelper.exe')),
        gate.process_record(dict(pid=3,parent_pid=1,image='EOSOverlayRenderer-Win64-Shipping.exe',image_path='C:/Program Files (x86)/Epic Games/Epic Online Services/managedArtifacts/synthetic/EOSOverlayRenderer-Win64-Shipping.exe')),
        gate.process_record(dict(pid=4,parent_pid=3,image='EOSOverlayRenderer-Win64-Shipping.exe',image_path='C:/Program Files (x86)/Epic Games/Epic Online Services/managedArtifacts/synthetic/EOSOverlayRenderer-Win64-Shipping.exe'))]
    check('EY2_folder_and_ancestry_exempt',not gate.reasons(group,require_launcher=True) and {p['pid'] for p in group['launcher_tree_excluded']}=={1,2,3,4})
    check('EY2_exemption_receipts_include_folders',all(p['install_folder'] and p['resolved_image_path'] for p in group['launcher_tree_excluded']))
    for image,path in [('UE4Editor.exe','D:/Foreign/UE4Editor.exe'),('UnrealGame.exe','D:/Foreign/UnrealGame.exe'),
        ('UE4Editor.exe','C:/Program Files (x86)/Epic Games/Launcher/UE4Editor.exe'),
        ('OtherGame-Win64-Shipping.exe','C:/Program Files (x86)/Epic Games/Epic Online Services/OtherGame-Win64-Shipping.exe'),
        ('EOSOverlayRenderer-Win64-Shipping.exe',None),('EOSOverlayRenderer-Win64-Shipping.exe','D:/Foreign/EOSOverlayRenderer-Win64-Shipping.exe')]:
        sample=copy.deepcopy(group)
        sample['processes'].append(gate.process_record(dict(pid=5,parent_pid=3,image=image,image_path=path)))
        check('EY2_not_exempt_'+image+'_'+str(path),'foreign_process:5' in gate.reasons(sample,require_launcher=True) and 5 not in [p['pid'] for p in sample['launcher_tree_excluded']])
    sample=copy.deepcopy(group);sample['processes'][2]['parent_pid']=88
    check('EY2_changed_ancestry_rechecked',all('foreign_process:'+str(p) in gate.reasons(sample,require_launcher=True) for p in (3,4)))
    sample=copy.deepcopy(group);sample['processes']=sample['processes'][1:]
    check('EY2_launcher_absent_waits','launcher' in gate.reasons(sample,require_launcher=True))
    for image in ('UnrealPak.exe','UnrealLightmass.exe','UnrealBuildTool.exe','UnrealHeaderTool.exe'):
        sample=copy.deepcopy(good);sample['processes'].append(gate.process_record(dict(pid=5,parent_pid=1,image=image)))
        check('EY3_block_'+image,'foreign_process:5' in gate.reasons(sample,require_launcher=True))
    for image in gate.OBSERVATION_IMAGES:
        sample=copy.deepcopy(good);sample['processes'].append(dict(pid=5,parent_pid=1,image=image,blocked_runtime=True))
        check('EY3_observe_'+image,'foreign_process:5' not in gate.reasons(sample,require_launcher=True))
    sample=copy.deepcopy(good);sample['free_commit_bytes']-=1
    check('EY3_12_GiB_start_commit','commit_memory' in gate.reasons(sample))
    check('EY3_commit_Waiting_line',waiting_line(['commit_memory'])=='Waiting: 12 GiB free commit.')
    check('EY3_inclusive_12_GiB',not gate.reasons(good,require_launcher=True))
    for drive,expected in [('C',(13,11,10)),('D',(40,30,25)),('E',(15,12,10))]:
        check('EY4_'+drive+'_pause_lines',disk_pause.LIMITS[drive]==expected)
        check('EY4_'+drive+'_floor',gate.FLOORS[drive]==expected[-1]*gate.GIB)
    healthy=dict(free_commit_bytes=6*gate.GIB,volumes=copy.deepcopy(good['volumes']))
    for key in ('commit','C','D','E'):
        now=[0];sample=copy.deepcopy(healthy)
        if key=='commit':sample['free_commit_bytes']=None
        else:sample['volumes'][key]={}
        with SessionGate(output/(key+'-unreadable.jsonl'),clock=lambda:now[0],limits_reader=lambda:sample) as monitor:
            monitor.check(())
            now[0]=59;monitor.check(())
            check('EY3_'+key+'_59_seconds_note',True)
            now[0]=60
            try:monitor.check(())
            except RuntimeError as error:check('EY3_'+key+'_60_seconds_stop','reading unavailable for 60 seconds' in str(error))
            else:check('EY3_'+key+'_60_seconds_stop',False)
    now=[0];sample=copy.deepcopy(healthy);sample['free_commit_bytes']=None
    with SessionGate(output/'recovery.jsonl',clock=lambda:now[0],limits_reader=lambda:sample) as monitor:
        monitor.check(());now[0]=59;sample['free_commit_bytes']=6*gate.GIB;monitor.check(())
        check('EY3_recovery_clears_timer',not monitor.unreadable_since)
        now[0]=60;sample['free_commit_bytes']=None;monitor.check(());now[0]=119;monitor.check(())
        check('EY3_new_gap_has_fresh_timer',monitor.unreadable_since=={'free_commit':60})
    for name,reader in [('exception',lambda:(_ for _ in ()).throw(OSError('synthetic'))),('invalid',lambda:None),('volumes_null',lambda:dict(free_commit_bytes=6*gate.GIB,volumes=None))]:
        now=[0]
        with SessionGate(output/(name+'.jsonl'),clock=lambda:now[0],limits_reader=reader) as monitor:
            monitor.check(());now[0]=60
            try:monitor.check(())
            except RuntimeError as error:check('EY3_'+name+'_bounded', 'unavailable for 60 seconds' in str(error))
            else:check('EY3_'+name+'_bounded',False)
    result=environment_check(policy.TOOLS);result['self_tests']=self_tests()
    (output/'environment-reads.json').write_text(json.dumps(result,indent=2)+'\n')
    check('EY1_all_named_environment_reads',result['result']=='PASS')
    check('EY1_environment_mutants',all(r['passed'] for r in result['self_tests']))
    check('EY1_marker_not_read_or_forwarded',all('EUROPA_AUTHORING_MARKER' not in (policy.TOOLS/p).read_text() for p in ('fixture_leg.py','disk_pause.py')))
    result=dict(result='PASS',cases=rows,unreal_launches=0)
    (output/'result.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(result='PASS',cases=len(rows),unreal_launches=0)))
    return result


if __name__=='__main__':tests(sys.argv[1])
