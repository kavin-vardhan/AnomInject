import json
from pathlib import Path
import sys

import fixture_policy as policy
from fixture_children import reason
from fixture_self_tests import config
from fixture_uploads import check as uploads
import write_ledger as ledger


def main(output):
    output=policy.canonical(output,ledger.JOB)
    output.mkdir(parents=True,exist_ok=False)
    rows=[]
    def check(name,value):
        assert value,name
        rows.append(dict(case=name,result='PASS'))
    def refuses(name,action):
        try:action()
        except RuntimeError as error:rows.append(dict(case=name,result='EXPECTED_FAIL',reason=str(error)))
        else:raise AssertionError(name+' accepted')
    data=config(ledger.SCOPE)
    policy.validate(data,False)
    for old in ('09422','09423','09424','09425','09428','09426b'):
        bad=dict(data,arming_job=old)
        refuses('retired_'+old,lambda:policy.validate(bad,False))
    for recipe in policy.SESSIONS:
        root=output/recipe
        argv=policy.command(recipe,root)
        check(recipe+'_argv',policy.validate_argv(argv,recipe,root))
        for flag in (policy.UPLOAD,policy.ANALYTICS):
            refuses(recipe+'_missing_'+flag,lambda:policy.validate_argv([a for a in argv if a!=flag],recipe,root))
        refuses(recipe+'_contradictory_analytics',lambda:policy.validate_argv(argv+[policy.ANALYTICS.replace('False','True')],recipe,root))
    for relative in ('Saved/Config/CrashReportClient/UE4CC-test/CrashReportClient.ini',
                     'Saved/Autosaves/PackageRestoreData.json','Intermediate/CachedAssetRegistry.bin',
                     'Intermediate/AssetRegistryCache/cache.bin','Saved/Logs/editor.log'):
        check('managed_'+relative,bool(policy.managed_kind(policy.HOST/relative)))
    for relative in ('Config/DefaultEngine.ini','StylizedParisStreet.uproject','Plugins/AnomalyInjector/AnomalyInjector.uplugin',
                     'Saved/Config/Windows/EditorSettings.ini','Intermediate/Build/Makefile.bin','Binaries/Win64/game.dll'):
        check('bound_'+relative,not policy.managed_kind(policy.HOST/relative))
    check('UBT_query',reason('UnrealBuildTool.exe','UnrealBuildTool.exe -Mode=QueryTargets -Project="owned"') is None)
    for image,command in [('UnrealBuildTool.exe','UnrealBuildTool.exe Target Win64 Development'),
                          ('UnrealBuildTool.exe','UnrealBuildTool.exe -Mode=Build'),('cl.exe','cl.exe'),('link.exe','link.exe'),
                          ('UnrealHeaderTool.exe','UnrealHeaderTool.exe'),('UE4Editor.exe','UE4Editor.exe')]:
        check('build_refused_'+str(command),bool(reason(image,command)))
    check('UBT_unreadable_not_build',reason('UnrealBuildTool.exe',None) is None)
    check('UBT_validate',reason('UnrealBuildTool.exe','UnrealBuildTool.exe -Mode=ValidatePlatforms -allplatforms') is None)
    check('standin_build',bool(reason('python.exe','python.exe --fc6-ubt-standin -Mode=Build',rehearsal=True)))
    check('standin_only_explicit_rehearsal',reason('python.exe','python.exe --fc6-ubt-standin -Mode=Build') is None)
    old=uploads(policy.ROOT/'Receipts/09425/fixture-attempts/EA_Core/01/session')
    check('old_editor_POST_can_fail',any(r.get('hits') for r in old['logs'] if Path(r['path']).name=='Runtime.log'))
    check('old_crash_client_POST_can_fail',any(r.get('hits') for r in old['logs'] if Path(r['path']).name=='CrashClient.log'))
    clean=output/'clean';clean.mkdir()
    for name in ('Runtime.log','CrashClient.log'):(clean/name).write_text('No request was made.\n')
    check('clean_upload_logs',uploads(clean)['result']=='PASS')
    (clean/'CrashClient.log').write_text('LogEditorSessionSummary: EditorSessionSummary sent report.\n')
    check('session_summary_can_fail',uploads(clean)['result']=='FAIL')
    from build_memory import OwnedTree
    import os
    tree=OwnedTree(os.getpid())
    try:check('retained_handle_native_argv','fixture_fc_tests.py' in tree.command_line(tree.known[os.getpid()]))
    finally:tree.close()
    from unittest.mock import patch
    import datetime
    import fixture_precheck
    from fixture_rehearsal import samples
    for name,mutate in [('input_599',lambda s:s.update(idle_seconds=599)),
                        ('commit_below_8',lambda s:s.update(free_commit_bytes=8*1024**3-1)),
                        ('launcher_absent',lambda s:s.update(processes=[])),
                        ('disk_between_floor_start',lambda s:s['volumes']['D'].update(free_bytes=39*1024**3))]:
        sample=samples()[0];sample['idle_seconds']=600;mutate(sample)
        good=samples()[0];good['idle_seconds']=600
        readings=iter([sample,good])
        clock=[datetime.datetime(2026,10,29,18,tzinfo=datetime.timezone.utc)]
        slept=[]
        def sleep(seconds):
            slept.append(seconds);clock[0]+=datetime.timedelta(seconds=seconds)
        with patch.object(fixture_precheck,'reading',lambda:{'D':dict(free=60,floor=25)}):
            fixture_precheck.wait(output/(name+'.jsonl'),collector=lambda:next(readings),sleeper=sleep,now=lambda:clock[0])
        check(name+'_waits_then_recovers',slept==[60])
    ledger.dump(output/'summary.json',dict(result='PASS',cases=rows,unreal_launches=0))
    print(json.dumps(dict(result='PASS',cases=len(rows),unreal_launches=0)))


if __name__=='__main__':main(Path(sys.argv[1]))
