from collections import Counter
import json
from pathlib import Path
import sys

import fixture_policy as policy
import fixture_smoke_425 as smoke
from canary_eventlogs import excluded_denial, FOLDER
import write_ledger as ledger


def main(output):
    output=policy.canonical(output,ledger.JOB)
    assert not output.exists()
    rows=[]
    def check(name,value):
        assert value,name
        rows.append(dict(case=name,result='PASS'))
    port=policy.TOOLS.parents[1]
    original=ledger.capture_child(['git','-C',str(port),'show','c1ef76b:tools/europa/author_425_fixture.py'])
    expected=original.replace("('override_motion_blur_amount',True),('motion_blur_amount',0.0),\n                            ('override_anti_aliasing_method',True),('anti_aliasing_method',unreal.AntiAliasingMethod.AAM_NONE)]", "('override_motion_blur_amount',True),('motion_blur_amount',0.0)]")
    current=(policy.TOOLS/'author_425_fixture.py').read_text()
    check('FE1_removed_properties_stay_absent',expected!=original and 'override_anti_aliasing_method' not in current and 'anti_aliasing_method' not in current)
    prior=policy.ROOT/'Receipts/09426b/fixture-attempts/Smoke/01/smoke.json'
    native=json.loads(prior.read_text())
    missing={r['name'] for r in native['checks'] if not r['exists']}
    check('prior_missing_exactly_FE1',missing=={'PostProcessSettings.override_anti_aliasing_method','PostProcessSettings.anti_aliasing_method'})
    actual=Counter([(n,a) for n,a in smoke.CALLS]+[(n,None) for n in smoke.NAMES]+[(k+'.'+p,None) for k,ps in smoke.PROPERTIES.items() for p in ps])
    present=Counter((r['name'],r.get('arguments')) for r in native['checks'] if r['exists'] and 'error' not in r)
    removed=Counter([('EditorLevelLibrary.new_level',1),('EditorLevelLibrary.get_editor_world',0),('EditorLevelLibrary.save_current_level',0)])
    added=Counter([('EditorLoadingAndSavingUtils.new_blank_map',1),('EditorLoadingAndSavingUtils.save_map',2),
                   ('SystemLibrary.execute_console_command',2),('Actor.get_actor_rotation',1)])
    check('remaining_native_bindings_plus_FI2_source_bound_calls',actual==present-removed+added)
    manifest=port/'docs/europa/09427c-a5-fixture-manifest.json'
    data=json.loads(manifest.read_text())
    check('recipe_and_manifest_bindings',policy.verify_files([data[k] for k in ('recipe','recipe_document','smoke_checker')])==3)
    check('EW1_binary_bindings_not_revised',data['binary_manifests_unchanged']==policy.manifest_rows())
    check('six_postprocess_items',len(data['remaining_post_process_items'])==6)
    class Denied(OSError):winerror=5
    class Sharing(OSError):winerror=32
    cases=[('denied_windows_eventlog',FOLDER/'sample.evtx',Denied(),True),
           ('case_insensitive_folder','c:/WINDOWS/system32/WINEVT/logs/sample.EVTX',Denied(),True),
           ('other_file_type',FOLDER/'sample.ini',Denied(),False),
           ('other_directory','D:/IA_Europa/Receipts/sample.evtx',Denied(),False),
           ('FI5_sharing_locked_eventlog',FOLDER/'sample.evtx',Sharing(),True),
           ('readable_eventlog',FOLDER/'sample.evtx',None,False),
           ('path_escape',str(FOLDER/'../sample.evtx'),Denied(),False)]
    for name,path,error,wanted in cases:check('FE2_'+name,excluded_denial(path,error)==wanted)
    old=json.loads((policy.ROOT/'Launch/Europa425-FixtureAuthoring.json').read_text())
    guards=[r for r in old['tooling_files'] if Path(r['path']).name in ('fixture_children.py','fixture_session_gate.py','idle_gate.py','fixture_launcher.py')]
    check('FD2_FD3_FC2_implementation_unchanged',len(guards)==4 and policy.verify_files(guards)==4)
    output.parent.mkdir(parents=True,exist_ok=True)
    output.write_text(json.dumps(dict(result='PASS',cases=rows,native_receipt=dict(path=str(prior),sha256=policy.sha(prior)),
        manifest=dict(path=str(manifest),sha256=policy.sha(manifest)),unreal_launches=0,dry_run_sessions=0),indent=2)+'\n')
    print(json.dumps(dict(result='PASS',cases=len(rows),remaining_native_checks=51)))


if __name__=='__main__':main(Path(sys.argv[1]))
