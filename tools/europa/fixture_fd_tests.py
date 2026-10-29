import copy
import json
from pathlib import Path
import sys

import fixture_policy as policy
from fixture_children import reason,BuildOutputWatch
from fixture_self_tests import config
from fixture_session_gate import stop_reasons,GIB
from fixture_wait import waiting_line
from fixture_rehearsal import samples
from idle_gate import reasons,FLOORS
from job_scope import memory_policy_allowed


def main(output):
    import write_ledger as ledger
    output=policy.canonical(output,ledger.JOB)
    output.mkdir(parents=True,exist_ok=False)
    rows=[]
    def check(name,value):
        assert value,name
        rows.append(dict(case=name,result='PASS'))
    data=config(ledger.SCOPE)
    check('FD3_config',policy.validate(data,False)==data['job_template'])
    check('FD3_scope',memory_policy_allowed(data['job_template'],ledger.JOB))
    for old in ('09422','09423','09424','09425','09428'):
        try:policy.validate(dict(data,arming_job=old),False)
        except RuntimeError:check('retired_'+old,True)
        else:check('retired_'+old,False)
    for command in ('-Mode=QueryTargets -Project="D:/Target Win64 Development.uproject"',
                    '-Mode=ValidatePlatforms -allplatforms','-Mode=FutureNonBuildQuery',
                    'Win64 Development','-Project="D:/My Game.uproject" -Mode=QueryTargets'):
        check('allowed_'+command,reason('UnrealBuildTool.exe','UnrealBuildTool.exe '+command) is None)
    check('unreadable_argv_not_build_action',reason('UnrealBuildTool.exe',None) is None)
    for command in ('-Mode=Build','-mode=build','"-Mode=Build"','-Mode="Build"',
                    'Target Win64 Development','Win64 Development Target','Target Win64+Linux DebugGame+Shipping',
                    '-Target="Target Win64 Development"','-Target=Target Win64 Development','-TargetType=Editor Win64 Development',
                    '-Mode=QueryTargets Target Win64 Development'):
        check('build_'+command,bool(reason('UnrealBuildTool.exe','UnrealBuildTool.exe '+command)))
    check('dotnet_build',bool(reason('dotnet.exe','dotnet.exe UnrealBuildTool.dll Target Win64 Development')))
    for image in ('cl.exe','link.exe','UnrealHeaderTool.exe','clang.exe','clang-cl.exe','lld-link.exe'):
        check('child_'+image,bool(reason(image,image)))
    sample=samples()[0]
    sample['free_commit_bytes']=8*GIB
    check('start_8_inclusive',not reasons(sample,require_launcher=True))
    sample['free_commit_bytes']-=1
    check('start_below_8_waits','commit_memory' in reasons(sample,require_launcher=True))
    check('start_waiting_line',waiting_line(['commit_memory'])=='Waiting: 8 GiB free commit.')
    sample['free_commit_bytes']=8*GIB;sample['free_memory_bytes']=6*GIB-1
    check('physical_6_unchanged','physical_memory' in reasons(sample,require_launcher=True))
    limits=dict(free_commit_bytes=4*GIB,volumes={d:dict(free_bytes=n,resolved_drive=d) for d,n in FLOORS.items()})
    check('session_4_inclusive',not stop_reasons(limits))
    limits['free_commit_bytes']-=1
    check('session_below_4_stops',stop_reasons(limits)==['free commit below 4 GiB'])
    for relative in ('Binaries/Win64/test.dll','Intermediate/Build/test.obj','Intermediate/Makefile.bin',
                     'Plugins/AnomalyInjector/Binaries/Win64/test.dll'):
        case=output/('write-'+str(len(rows)));case.mkdir()
        watcher=BuildOutputWatch(case,case/'watch.jsonl')
        path=case/relative;path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(b'synthetic build product')
        try:watcher.check()
        except RuntimeError as error:check(relative+'_stops','FD2 write to build outputs' in str(error))
        else:check(relative+'_stops',False)
    case=output/'managed';case.mkdir()
    watcher=BuildOutputWatch(case,case/'watch.jsonl')
    path=case/'Intermediate/CachedAssetRegistry.bin';path.parent.mkdir();path.write_bytes(b'synthetic cache')
    watcher.check();check('FC5_cache_recorded_not_build',True)
    path=case/'Binaries/Win64/test.dll';path.parent.mkdir(parents=True);path.write_bytes(b'unchanged bytes')
    watcher=BuildOutputWatch(case,case/'same-byte-watch.jsonl')
    import os
    before=path.stat();os.utime(path,ns=(before.st_atime_ns,before.st_mtime_ns+1000000000))
    try:watcher.check()
    except RuntimeError:check('same_byte_build_product_write_stops',True)
    else:check('same_byte_build_product_write_stops',False)
    ledger.dump(output/'summary.json',dict(result='PASS',cases=rows,unreal_launches=0,dry_run_sessions=0))
    print(json.dumps(dict(result='PASS',cases=len(rows))))


if __name__=='__main__':main(Path(sys.argv[1]))
