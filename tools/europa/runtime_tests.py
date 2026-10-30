import argparse
import copy
import json
from pathlib import Path
from unittest.mock import patch

from fixture_rehearsal import samples
from fixture_children import BuildOutputWatch
from idle_gate import FLOORS,reasons
from job_scope import memory_policy_allowed
import runtime_policy as policy
from runtime_children import reason
from runtime_monitor import Monitor,stop_reasons,validity,GIB
from runtime_scripts import steps,validate


def main(output):
    output=policy.canonical(output,policy.JOB/'scratch');output.mkdir(parents=True,exist_ok=False)
    rows=[]
    def check(name,value):
        assert value,name
        rows.append(dict(case=name,result='PASS'))
    def refuses(name,call):
        try:call()
        except (RuntimeError,ValueError):check(name,True)
        else:check(name,False)
    base=output/'slots';temp=policy.TEMP/'scratch/tests'
    config=dict(arming_job='09427',authority=policy.AUTHORITY,scope=policy.SCOPE,limits=policy.LIMITS,
        memory=policy.MEMORY,window_deadline_utc=policy.WINDOW_END,manifest=policy.MANIFEST,mode='rehearsal',
        receipts_root=str(base),temp_root=str(temp),slots={leg:policy.paths(base,temp,leg) for leg in policy.LEGS})
    check('exact_runtime_scope',policy.validate(config,'rehearsal'))
    for job in ('09417','09422','09423','09425','09428','09426b','09426c','09426d'):
        refuses('old_arming_'+job,lambda job=job:policy.validate(dict(config,arming_job=job),'rehearsal'))
    refuses('unknown_leg',lambda:policy.command('unarmed',output/'unarmed'))
    altered=copy.deepcopy(config);altered['slots']['Smoke']['root']='C:/wrong'
    refuses('slot_escape',lambda:policy.validate(altered,'rehearsal'))
    altered=copy.deepcopy(config);altered['manifest']['host']='51base'
    refuses('frozen_51_not_bound',lambda:policy.validate(altered,'rehearsal'))
    for leg in policy.LEGS:
        argv=policy.command(leg,output/policy.slug(leg));check('argv_'+leg,policy.validate_argv(argv,leg,output/policy.slug(leg)))
        for switch in (policy.fixture.UPLOAD,policy.fixture.ANALYTICS):
            refuses('missing_optout_'+leg+'_'+switch,lambda a=argv,s=switch,l=leg:policy.validate_argv([v for v in a if v!=s],l,output/policy.slug(l)))
    for command in ('-Mode=QueryTargets','-Mode=ValidatePlatforms -allplatforms'):
        check('UBT_nonbuild_'+command,reason('UnrealBuildTool.exe','UnrealBuildTool.exe '+command) is None)
    for command in (None,'','UnrealBuildTool.exe -TargetList=x','UnrealBuildTool.exe -Mode=Clean',
        'UnrealBuildTool.exe -Mode=Execute','UnrealBuildTool.exe -Mode=Deploy','UnrealBuildTool.exe -Mode=Build',
        'UnrealBuildTool.exe Target Win64 Development','UnrealBuildTool.exe -Target="Target Win64 Development"'):
        check('UBT_STOP_'+str(command),bool(reason('UnrealBuildTool.exe',command)))
    for image in ('cl.exe','link.exe','UnrealHeaderTool.exe','clang-cl.exe','AutomationTool.exe','UE4Editor-Cmd.exe'):
        check('forbidden_child_'+image,bool(reason(image,image)))
    check('additional_editor',bool(reason('UE4Editor.exe','UE4Editor.exe')))
    check('authorized_root',reason('UE4Editor.exe','UE4Editor.exe',root=True) is None)
    scope=dict(runtime_authorized=True,runtime_scope=policy.SCOPE,authority='094-27 FB1-FB7',build_pairs=[],query_pairs=[],memory=policy.MEMORY)
    check('FB3_scope',memory_policy_allowed(scope,policy.JOB))
    sample=samples()[0];sample['free_commit_bytes']=8*GIB;sample['free_memory_bytes']=6*GIB
    check('start_lines_inclusive',not reasons(sample,require_launcher=True))
    sample['free_commit_bytes']-=1;check('start_commit_wait','commit_memory' in reasons(sample,require_launcher=True))
    limit=dict(free_commit_bytes=6*GIB,volumes={d:dict(free_bytes=v,resolved_drive=d) for d,v in FLOORS.items()})
    check('session_6_inclusive',not stop_reasons(limit))
    check('session_below_6',bool(stop_reasons(dict(limit,free_commit_bytes=6*GIB-1))))
    observations=[dict(started_qpc=10,ended_qpc=20,last_input_qpc=0,foreign_gpu_percent=0,free_physical_bytes=1)]
    check('valid_window',validity(observations,11,19)['valid'])
    for key,value in (('last_input_qpc',15),('foreign_gpu_percent',15.01),('foreign_gpu_percent',None)):
        check('invalid_'+key+str(value),not validity([dict(observations[0],**{key:value})],11,19)['valid'])
    check('missing_coverage',not validity(observations,9,19)['valid'])
    check('internal_gap',not validity([dict(observations[0],ended_qpc=13),dict(observations[0],started_qpc=14)],11,19)['valid'])
    for relative in ('Content/test.uasset','Config/test.ini','Saved/Config/Windows/test.ini'):
        case=output/('protected-'+str(len(rows)));case.mkdir()
        monitor=Monitor(case,case/'host','Smoke',dict(content=[],config=[],descriptors=[]),game=False,synthetic=True)
        path=case/'host'/relative;path.parent.mkdir(parents=True);path.write_bytes(b'synthetic')
        with patch('runtime_monitor.limits_sample',return_value=limit):refuses('protected_'+relative,lambda:monitor.check(()))
        monitor.close()
    for relative in ('Saved/Config/WorldState/x.json','Saved/Backup/test.umap','Saved/Autosaves/x','Saved/Logs/x.log'):
        case=output/('managed-'+str(len(rows)));case.mkdir()
        monitor=Monitor(case,case/'host','Smoke',dict(content=[],config=[],descriptors=[]),game=False,synthetic=True)
        path=case/'host'/relative;path.parent.mkdir(parents=True);path.write_bytes(b'synthetic')
        with patch('runtime_monitor.limits_sample',return_value=limit):monitor.check(())
        monitor.close();check('managed_'+relative,True)
    case=output/'build';case.mkdir();watch=BuildOutputWatch(case,case/'watch.jsonl')
    path=case/'Binaries/test.dll';path.parent.mkdir();path.write_bytes(b'synthetic')
    refuses('build_output_write',watch.check)
    case=output/'unreadable';case.mkdir();monitor=Monitor(case,case/'host','Smoke',dict(content=[],config=[],descriptors=[]),game=False,synthetic=True)
    with patch('runtime_monitor.limits_sample',return_value=dict(limit,free_commit_bytes=None)),patch('runtime_monitor.time.monotonic',return_value=100):monitor.check(())
    with patch('runtime_monitor.limits_sample',return_value=dict(limit,free_commit_bytes=None)),patch('runtime_monitor.time.monotonic',return_value=160):
        refuses('acting_unreadable_60s',lambda:monitor.check(()))
    monitor.close()
    for leg in policy.LEGS:
        if leg not in ('Smoke','Load','WorldsPIE'):check('native_'+leg,validate(steps(leg)))
    (output/'summary.json').write_text(json.dumps(dict(result='PASS',cases=rows,unreal_launches=0),indent=2)+'\n')
    print(json.dumps(dict(result='PASS',cases=len(rows))))


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();main(a.output)
