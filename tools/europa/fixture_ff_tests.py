import copy
import json
from pathlib import Path
import sys
from unittest.mock import patch

import fixture_policy as policy
from fixture_self_tests import config
import write_ledger as ledger


def main(output):
    output=policy.canonical(output,ledger.JOB)
    output.mkdir(parents=True,exist_ok=False)
    rows=[]
    def check(name,value,**evidence):
        assert value,name
        rows.append(dict(case=name,result='PASS',**evidence))
    def refuses(name,action):
        try:action()
        except RuntimeError as error:rows.append(dict(case=name,result='EXPECTED_FAIL',reason=str(error)))
        else:raise AssertionError(name+' accepted')
    data=config(ledger.SCOPE)
    check('two_recipe_scope',policy.validate(data,False)==data['job_template'] and policy.SESSIONS==('EA_Core','EA_Travel')
          and policy.SCOPE['maximum_editor_sessions']==2 and 'smoke' not in policy.SCOPE and set(policy.OUTPUTS)==set(policy.RECIPES))
    bad=copy.deepcopy(data)
    bad['recipes']['Smoke']=policy.make_recipe('Smoke',policy.ROOT/'Receipts'/policy.ARMING_JOB,policy.ROOT/'Temp'/policy.ARMING_JOB)
    refuses('smoke_session_refused',lambda:policy.validate(bad,False))
    for name in ('09426c','09426b','09428','09425','09424','09423','09422'):
        refuses('old_arming_'+name,lambda:policy.validate(dict(data,arming_job=name),False))
    check('FF4_window',policy.WINDOW_END=='2026-10-31T01:00:00+00:00')
    managed=('Saved/Config/WorldState/1202994220.json','Saved/Backup/EA_Core_stamp.umap',
             'Saved/Autosaves/Map.umap','Saved/Logs/Runtime.log',
             'Saved/Config/CrashReportClient/UE4CC-test/report.json','Saved/Elsewhere/settings.ini')
    protected=('Saved/Config/Windows/EditorSettings.ini','Saved/Config/CrashReportClient/UE4CC-test/CrashReportClient.ini',
               'Saved/Config/Nested/UPPER.INI','Config/DefaultEngine.ini','Config/WorldState.json')
    for relative in managed+protected:
        for action in ('new','changed','removed'):
            host=output/('host-%02d'%len(rows));host.mkdir()
            project=host/'StylizedParisStreet.uproject';project.write_text('{}')
            path=host/relative;path.parent.mkdir(parents=True,exist_ok=True)
            if action!='new':path.write_text('before')
            with patch.object(policy,'HOST',host):
                before=policy.config_snapshot()
                managed_before=policy.managed_snapshot()
                if action=='removed':
                    owned=dict(path=str(path),sha256=policy.sha(path),action='remove owned synthetic test file')
                    ledger.dump(host/'removal-receipt.json',owned)
                    assert path.resolve().is_relative_to(output.resolve())
                    path.unlink()
                else:path.write_text('after')
                name=action+'_'+relative
                if relative in managed:
                    policy.verify_config_after(before,'the session')
                    after=policy.managed_snapshot()
                    check(name,managed_before!=after and policy.managed_kind(path)=='FF2 editor-managed Saved file',
                          before=managed_before,after=after)
                    classified=ledger.classification('port',str(path),'runtime')
                    check(name+'_ledger_recorded',classified[0]=='port_D' and 'FF2' in classified[1])
                else:
                    check(name+'_bound_class',policy.managed_kind(path) is None)
                    refuses(name,lambda:policy.verify_config_after(before,'the session'))
    host=output/'bound-files';host.mkdir()
    with patch.object(policy,'HOST',host):
        check('outside_host_saved_not_managed',policy.managed_kind(output/'other/Saved/log.txt') is None)
        check('saved_sibling_not_managed',policy.managed_kind(host/'SavedElsewhere/log.txt') is None)
        check('lexical_escape_not_managed',policy.managed_kind(host/'Saved/../Config/file.json') is None)
        check('FC5_intermediate_cache_retained',policy.managed_kind(host/'Intermediate/AssetRegistryCache/cache.bin')=='FC5 asset-registry cache')
        for name in ('StylizedParisStreet.uproject','Plugins/AnomalyInjector/AnomalyInjector.uplugin'):
            path=host/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text('before')
            bound=[dict(path=str(path),size=path.stat().st_size,sha256=policy.sha(path))]
            path.write_text('after')
            refuses('descriptor_binding_'+name,lambda:policy.verify_files(bound))
    old=json.loads((policy.ROOT/'Receipts/09426c/arming/receipt.json').read_text())
    retained=[r for r in old['tooling'] if Path(r['path']).name in ('author_425_fixture.py','fixture_smoke_425.py','fixture_launcher.py',
        'fixture_children.py','idle_gate.py','fixture_session_gate.py','canary_eventlogs.py','09426c-a5-fixture-manifest.json','09411-fixture-recipes.md')]
    check('recipe_launcher_guard_memory_canary_and_A5_unchanged',len(retained)==9 and policy.verify_files(retained)==9)
    ledger.dump(output/'summary.json',dict(result='PASS',cases=rows,retained_bindings=retained,unreal_launches=0,
        dry_run_sessions=0,real_slots_used=0,authority='094-26c-ruling FF1-FF4'))
    print(json.dumps(dict(result='PASS',cases=len(rows),unreal_launches=0,dry_run_sessions=0)))


if __name__=='__main__':main(Path(sys.argv[1]))
