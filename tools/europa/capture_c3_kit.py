import argparse
import copy
import json
import os
from pathlib import Path
import subprocess
import sys

import capture_425_kit as c1
from fixture_policy import HOST,TOOLS,sha,verify_files,managed_snapshot
from owner_attempts import Guard,create_json,stamp
from runtime_policy import protection_snapshot,verify_protection
from runtime_runner import execute,crash_policy
from runtime_evidence import freeze,outcome,ensure_frozen
from fixture_uploads import check as uploads
import capture_c3_reader as reader

J=c1.JOB
INSTALLED=c1.INSTALLED

def command(leg,root):
    if leg=='Smoke':return c1.command('Smoke',root)
    if leg!='C3':raise RuntimeError('Unarmed C3 leg')
    argv=c1.command('C1Game',root)
    argv=[a.replace('/Game/EuropaPhaseA/EA_Travel','/Game/EuropaPhaseA/EA_Core') for a in argv if not a.startswith('-IAICaptureDiagnostics=')]
    argv=[a.replace('IAI.Capture.C1 '+(root/'plugin-frames').as_posix()+' 60','IAI.Capture.C3 '+(root/'plugin-frames').as_posix()) for a in argv]
    c1.require_frame_format(argv)
    return argv

def validate(config):
    if config.get('arming_job')!='09433' or config.get('stage')!='C3' or config['deadline']!=c1.DEADLINE:raise RuntimeError('Wrong C3 arming')
    verify_files(config['engine_files']);verify_files(config['tooling_files']);verify_protection(config['protected'])
    if sha(config['manifest']['path'])!=config['manifest']['sha256']:raise RuntimeError('Manifest changed')
    verify_files(json.loads(Path(config['manifest']['path']).read_text())['files'])
    verify_files(config['reference_files'])

def arm():
    if subprocess.check_output(['git','-C',str(TOOLS.parent.parent),'status','--porcelain'],text=True).strip():raise RuntimeError('Commit before C3 arming')
    prior=json.loads(INSTALLED.read_text())
    if prior.get('stage')=='C3' and prior.get('arming_job')=='09433':raise RuntimeError('C3 already armed/spent')
    primary=J/'c2-attempts/C2/01'
    assert json.loads((primary/'result.json').read_text())['result']=='PASS'
    ensure_frozen(primary/'raw-evidence.json')
    for leg in ('C1PIE','C1Game'):
        slot=next(s for s in ('01','02') if (J/'capture-attempts'/leg/s/'result.json').exists() and json.loads((J/'capture-attempts'/leg/s/'result.json').read_text())['result']=='PASS')
        root=J/'capture-attempts'/leg/slot
        assert json.loads((root/'result.json').read_text())['result']=='PASS'
        ensure_frozen(root/'raw-evidence.json')
    built=[json.loads(p.read_text()) for p in (J/'builds').glob('*/result.json')]
    good=[r for r in built if r.get('gate_result')=='PASS' and r.get('exit_code')==0 and c1.native_source_equal(r['commit'])]
    assert {r['target'] for r in good}=={'StylizedParisStreetEditor','StylizedParisStreet'}
    head=subprocess.check_output(['git','-C',str(TOOLS.parent.parent),'rev-parse','HEAD'],text=True).strip()
    before=json.loads((J/'entry.json').read_text())['protected'];current=protection_snapshot()
    assert all(before[k]==current[k] for k in ('content','config','descriptors'))
    manifest=json.loads(Path(prior['manifest']['path']).read_text())
    for row in manifest['files']:
        p=Path(row['path']);row.update(size=p.stat().st_size,mtime_ns=p.stat().st_mtime_ns,sha256=sha(p))
    manifest.update(source_commit=head,tooling_commit=head,build_sources={r['target']:r['commit'] for r in good},purpose='09433 C3 label builds; C1 raw evidence retained')
    manifest_path=J/'c3-425-binaries.json';create_json(manifest_path,manifest)
    refs=json.loads((J/'c2-reference-plan.json').read_text())
    config=dict(arming_job='09433',stage='C3',authority=c1.AUTHORITY,source_commit=head,deadline=c1.DEADLINE,
        manifest=dict(path=str(manifest_path),sha256=sha(manifest_path)),protected=current,
        engine_files=prior['engine_files'],tooling_files=[dict(path=str(p),size=p.stat().st_size,sha256=sha(p)) for p in sorted(TOOLS.glob('*.py'))],
        reference_files=refs['files'],references=refs['references'],metrics=reader.THRESHOLDS,
        slots={leg:str(J/'c3-attempts'/leg/'01') for leg in ('Smoke','C3')},limits=dict(Smoke=90,C3=180))
    job=json.loads((J/'capture-job-config.json').read_text())
    job['runtime_scope'].update(legs=['Smoke','C3'],maximum_sessions_per_leg=1)
    job['temp_root']=str(c1.ROOT/'Temp/09433/c3')
    create_json(J/'c3-job-config.json',job)
    validate(config)
    create_json(J/'c3-prior-arming.json',prior);create_json(J/'c3-arming.json',config)
    INSTALLED.write_text(json.dumps(config,indent=2),encoding='utf-8')
    launcher=c1.ROOT/'Launch/Europa425-Runtime.bat'
    launcher.write_text('@echo off\n"C:\\Python313\\python.exe" -B "'+str(TOOLS/'capture_c3_kit.py')+'" run\n',encoding='ascii')
    return config


def run(leg):
    config=json.loads(INSTALLED.read_text());validate(config)
    if leg not in config['slots']:raise RuntimeError('Unarmed leg')
    if leg=='C3' and json.loads((Path(config['slots']['Smoke'])/'result.json').read_text())['result']!='PASS':raise RuntimeError('C3 binding smoke failed')
    root=Path(config['slots'][leg]);temp=c1.ROOT/'Temp/09433/c3'/leg
    os.environ['EUROPA_JOB_CONFIG']=str(J/'c3-job-config.json')
    import write_ledger as ledger
    from runtime_lean_gate import wait
    with Guard(dict(state_root=str(J/'c3-state'/leg))) as guard:
        guard.check_unused_launch();root.mkdir(parents=True,exist_ok=False);temp.mkdir(parents=True,exist_ok=False);(root/'session').mkdir()
        if leg=='Smoke':
            (root/'launch.py').write_text('import imp\n_capture=imp.load_source("capture_425_driver",%r)\n_capture.start(%r,%r,True)\n'%(str(TOOLS/'capture_425_driver.py'),str(root/'driver.json'),str(root/'plugin-frames')),encoding='utf-8')
        argv=command(leg,root);create_json(root/'prepared.json',dict(argv=argv,leg=leg))
        print(leg+': waiting for FJ1/FK1 gate.',flush=True)
        gate=wait(root/'gate.jsonl',c1.DEADLINE,idle_seconds=300)
        if gate['status']!='CLEAR':raise RuntimeError('Window ended')
        validate(config);create_json(root/'protection-before.json',dict(result='PASS',snapshot=protection_snapshot()))
        create_json(root/'managed-before.json',managed_snapshot())
        process=root/'process';process.mkdir();os.environ['EUROPA_OPERATION_DIR']=str(process)
        guard.reserve_launch(dict(number=1));print(leg+': launching bounded session.',flush=True)
        row=execute(leg,root,temp,HOST,config['protected'],argv,ledger,config['limits'][leg])
        try:validate(config);protected=dict(result='PASS')
        except Exception as error:protected=dict(result='FAIL',reason=str(error))
        create_json(root/'protection-after.json',protected);create_json(root/'managed-after.json',managed_snapshot())
        upload=uploads(root/'session');create_json(root/'upload-check.json',upload)
        driver=json.loads((root/'driver.json').read_text()) if (root/'driver.json').exists() else None
        log=''
        try:
            log=(root/'session/Runtime.log').read_text(encoding='utf-8-sig',errors='replace')
            reader.need(row.get('exit_code')==0 and 'LogExit: Exiting.' in log,'Normal exit missing')
            if leg=='Smoke':
                reader.need(driver and driver.get('result')=='SMOKE_PASS','Binding smoke failed');result=dict(result='PASS',bindings=driver['bindings'])
            else:
                reader.need('Capture425 C3: shaders_loaded=visible425,reduce' in log,'Mask shaders did not load/dispatch')
                reader.need('Capture425 C3: stencil_restore=1 owners=4' in log,'Stencil restoration missing')
                result=reader.read(root/'plugin-frames/epoch01',config['references'])
                from capture_fo_reader import check as check_ui
                result['ui_free']=check_ui(root/'plugin-frames','C2')

        except Exception as error:result=dict(result='FAIL',reason=str(error))
        create_json(root/'reader.json',result)
        shader_failure=any('error' in line.lower() and ('shader' in line.lower() or '.usf' in line.lower()) for line in log.splitlines())
        raw=freeze(root,list(process.glob('*'))+[p for p in (root/'plugin-frames').rglob('*') if p.is_file()])
        row.update(outcome(leg,row,protected,upload,result,driver,row.get('new_crash_report_folders',[]),raw))
        if shader_failure:
            row.update(crashed=True,result='FAIL',shader_compile_failure=True)
        prior_crashes=sum(bool(json.loads(p.read_text()).get('crashed')) for p in list((J/'capture-attempts').glob('*/*/result.json'))+list((J/'c2-attempts').glob('*/*/result.json'))+list((J/'c3-attempts').glob('*/*/result.json')))
        row=crash_policy(row,prior_crashes);create_json(root/'result.json',row)
        print(leg+': '+row['result']+' '+json.dumps({k:v for k,v in result.items() if k not in ('pixel_measurements','events','per_frame','ui_free','clean_noise')}),flush=True)

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('action',choices=['arm','run']);p.add_argument('--leg',choices=['Smoke','C3'])
    a=p.parse_args()
    if a.action=='arm':print(json.dumps(dict(result='PASS',slots=arm()['slots'])))
    else:run(a.leg)
