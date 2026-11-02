import argparse
import copy
import datetime
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
import capture_office_reader as reader

J=c1.ROOT/'Receipts/09442'
AUTHORITY='094-42 FZ3-FZ6 FN2 FM1 FJ1 FK1'
INSTALLED=c1.INSTALLED
DEADLINE=None
FALLBACK="2026-11-01T21:30:00+00:00"

def admission_profile(wait,root,prior=None,now=None,cutoff=FALLBACK):
    if prior not in (None,'fallback'):raise RuntimeError('09442 admits EA_Core only')
    return wait(root/'gate-fallback.jsonl',DEADLINE,idle_seconds=300,memory=(8,4)),'fallback'

def select_map(argv,profile):
    if profile not in ('street','fallback'):raise ValueError('Unknown map profile')
    if profile=='fallback':
        argv[:]=[a.replace('/Game/ParisStreet/Levels/SampleMap','/Game/EuropaPhaseA/EA_Core') for a in argv]+['-IAICaptureFallback']
        argv[:]=[a.replace('-ExecCmds=','-ExecCmds=IAI.SetMinScreenCoverage 0.5,IAI.SetMinScreenCoverage,') for a in argv]
    return argv

def command(leg,root):
    if leg=='Smoke':return c1.command('Smoke',root)
    if leg not in ('C5',):raise RuntimeError('Unarmed C4 leg')
    argv=c1.command('C1Game',root)
    argv=[a.replace('/Game/EuropaPhaseA/EA_Travel','/Game/ParisStreet/Levels/SampleMap') for a in argv if not a.startswith('-IAICaptureDiagnostics=')]
    argv=[a.replace('IAI.Capture.C1 '+(root/'plugin-frames').as_posix()+' 60','IAI.Capture.C5 '+(root/'plugin-frames').as_posix()+' 4242') for a in argv]
    argv=[a.replace('-ExecCmds=','-ExecCmds=r.EyeAdaptationQuality 0,r.EyeAdaptationQuality,') for a in argv]
    capture='IAI.Capture.C5 '+(root/'plugin-frames').as_posix()+' 4242'
    route_command=capture+' blinking,lighting_mismatch,lod_corruption,missing_object'
    path=root/'capture-command.json'
    create_json(path,dict(command=route_command,receipt=str(root/'capture-command-receipt.json')))
    launcher=root/'capture-command.py'
    launcher.write_text('import sys\nsys.path.insert(0,'+repr(str(TOOLS))+')\nfrom capture_route_driver425 import start\nstart('+repr(str(path))+')\n',encoding='ascii')
    argv=[a.replace(capture,'py '+launcher.as_posix()) for a in argv]
    width,height=1280,720
    argv=['-ResX='+str(width) if a.lower().startswith('-resx=') else '-ResY='+str(height) if a.lower().startswith('-resy=') else a for a in argv]
    c1.require_frame_format(argv)
    return argv

def validate(config):
    if config.get('arming_job')!='09442' or config.get('stage')!='OfficeSmoke' or config['deadline']!=DEADLINE:raise RuntimeError('Wrong office-smoke arming')
    finish=datetime.datetime.fromisoformat(config['build_finished_utc'])
    expected=None
    if config.get('fallback')!=expected:raise RuntimeError('Fallback time changed')
    if config.get('metrics')!=reader.THRESHOLDS or config.get('limits')!={'C5':900}:raise RuntimeError('FU5 gates or limit changed')
    verify_files(config['engine_files']);verify_files(config['tooling_files']);verify_protection(config['protected'])
    if sha(config['manifest']['path'])!=config['manifest']['sha256']:raise RuntimeError('Manifest changed')
    verify_files(json.loads(Path(config['manifest']['path']).read_text())['files'])
    verify_files(config['reference_files'])

def retry_admitted(config,leg):
    first=Path(config['slots'][leg]['01'])
    result=json.loads((first/'result.json').read_text())
    if result['result']!='FAIL' or result.get('end_run'):raise RuntimeError('Retry requires a nonterminal failed first session')
    ensure_frozen(first/'raw-evidence.json')
    proof=json.loads((J/(leg+'-retry-qualification.json')).read_text())
    if proof.get('result')!='PASS' or not proof.get('diagnosis') or not proof.get('quick_testable_fix') or not proof.get('tests'):raise RuntimeError('Retry lacks qualified fix')
    verify_files(proof['tests'])
    if Path(config['slots'][leg]['02']).exists():raise RuntimeError('Retry is already spent')
    return proof


def arm(retry=False,leg=None):
    if subprocess.check_output(['git','-C',str(TOOLS.parent.parent),'status','--porcelain'],text=True).strip():raise RuntimeError('Commit before C3 arming')
    prior=json.loads(INSTALLED.read_text())
    if retry:
        if prior.get('arming_job')!='09442':raise RuntimeError('No first 09442 arming')
        retry_admitted(prior,leg)
    elif prior.get('arming_job')=='09442':raise RuntimeError('Office smoke already armed; use the qualified retry route')
    revision=('02-'+leg) if retry else '01'
    built=[json.loads(p.read_text()) for p in (J/'builds').glob('*/result.json')]
    good=[r for r in built if r.get('gate_result')=='PASS' and r.get('exit_code')==0 and c1.native_source_equal(r['commit'])]
    assert {r['target'] for r in good}=={'StylizedParisStreetEditor','StylizedParisStreet'}
    finished=max(datetime.datetime.fromisoformat(r['ended_utc']) for r in good)
    cutoff=None
    head=subprocess.check_output(['git','-C',str(TOOLS.parent.parent),'rev-parse','HEAD'],text=True).strip()
    before=json.loads((J/'entry.json').read_text())['protected'];current=protection_snapshot()
    assert all(before[k]==current[k] for k in ('content','config','descriptors'))
    manifest=json.loads(Path(prior['manifest']['path']).read_text())
    for row in manifest['files']:
        p=Path(row['path']);row.update(size=p.stat().st_size,mtime_ns=p.stat().st_mtime_ns,sha256=sha(p))
    manifest.update(source_commit=head,tooling_commit=head,build_sources={r['target']:r['commit'] for r in good},purpose='09442 FV5 office smoke build pair')
    manifest_path=J/('c5-425-binaries-'+revision+'.json');create_json(manifest_path,manifest)
    reference=TOOLS.parent.parent/'docs/sessions/2026-11-03-094-42-guard-and-fixture.md'
    refs=dict(files=[dict(path=str(reference),size=reference.stat().st_size,sha256=sha(reference))])
    config=dict(arming_job='09442',stage='OfficeSmoke',authority=AUTHORITY,source_commit=head,deadline=DEADLINE,fallback=cutoff,build_finished_utc=finished.isoformat(),
        manifest=dict(path=str(manifest_path),sha256=sha(manifest_path)),protected=current,
        engine_files=prior['engine_files'],tooling_files=[dict(path=str(p),size=p.stat().st_size,sha256=sha(p)) for p in sorted(TOOLS.glob('*.py'))],
        reference_files=refs['files'],reference_rule=reader.REFERENCE_RULE,metrics=reader.THRESHOLDS,
        slots={name:{number:str(J/'c5-attempts'/name/number) for number in ('01','02')} for name in ('C5',)},limits=dict(C5=900),retry_authorized=leg if retry else None)
    validate(config)
    create_json(J/('c5-prior-arming-'+revision+'.json'),prior);create_json(J/('c5-arming-'+revision+'.json'),config)
    INSTALLED.write_text(json.dumps(config,indent=2),encoding='utf-8')
    launcher=c1.ROOT/'Launch/Europa425-Runtime.bat'
    launcher.write_text('@echo off\n"C:\\Python313\\python.exe" -B "'+str(TOOLS/'capture_c5_kit.py')+'" run %*\n',encoding='ascii')
    return config


def run(leg,number):
    config=json.loads(INSTALLED.read_text());validate(config)
    verified=stamp()
    if leg not in config['slots'] or number not in config['slots'][leg]:raise RuntimeError('Unarmed leg')
    if number=='02':
        if config.get('retry_authorized')!=leg:raise RuntimeError('Retry not qualified/re-armed')
        retry_admitted(config,leg)
    root=Path(config['slots'][leg][number]);temp=c1.ROOT/'Temp/09442/c5'/leg/number
    os.environ['EUROPA_JOB_CONFIG']=str(J/'c5-job-config.json')
    import write_ledger as ledger
    from runtime_lean_gate import wait
    with Guard(dict(state_root=str(J/'c5-state'/leg/number))) as guard:
        guard.check_unused_launch();root.mkdir(parents=True,exist_ok=False);temp.mkdir(parents=True,exist_ok=False);(root/'session').mkdir()
        argv=command(leg,root);create_json(root/'prepared.json',dict(argv=argv,leg=leg))
        create_json(root/'protection-before.json',dict(result='PASS',snapshot=config['protected'],hash_verified_utc=verified,verification='validate rehashed the protected snapshot'))
        create_json(root/'managed-before.json',managed_snapshot())
        process=root/'process';process.mkdir();os.environ['EUROPA_OPERATION_DIR']=str(process)
        def admit():
            print(leg+': setup complete; waiting for FZ4 explicit four-route EA_Core admission.',flush=True)
            prior=json.loads((Path(config['slots'][leg]['01'])/'admitted.json').read_text())['profile'] if number=='02' else None
            gate,profile=admission_profile(wait,root,prior,cutoff=config['fallback'])
            if gate['status']!='CLEAR':raise RuntimeError('Window ended before process creation')
            select_map(argv,profile)
            create_json(root/'admitted.json',dict(profile=profile,argv=argv,gate=gate))
            guard.reserve_launch(dict(number=1));print(leg+': launching bounded session.',flush=True)
            return dict(utc=gate['utc'],status=gate['status'],idle_seconds=gate['sample']['idle_seconds'],free_commit_bytes=gate['sample']['free_commit_bytes'],free_memory_bytes=gate['sample']['free_memory_bytes'],volumes=gate['sample']['volumes'])
        row=execute(leg,root,temp,HOST,config['protected'],argv,ledger,config['limits'][leg],admit=admit)
        try:validate(config);protected=dict(result='PASS')
        except Exception as error:protected=dict(result='FAIL',reason=str(error))
        create_json(root/'protection-after.json',protected);create_json(root/'managed-after.json',managed_snapshot())
        upload=uploads(root/'session');create_json(root/'upload-check.json',upload)
        driver=json.loads((root/'driver.json').read_text()) if (root/'driver.json').exists() else None
        log=''
        try:
            log=(root/'session/Runtime.log').read_text(encoding='utf-8-sig',errors='replace')
            reader.need(row.get('exit_code')==0 and 'LogExit: Exiting.' in log,'Normal exit missing')
            reader.need('Capture425 C3: shaders_loaded=visible425,reduce' in log,'Mask shaders did not load/dispatch')
            reader.need('Capture425 C5: stencil_restore=1' in log,'Stencil restoration missing')
            result=reader.read(root/'plugin-frames/epoch01',log,fallback='-IAICaptureFallback' in argv)
            routes=json.loads((root/'plugin-frames/epoch01/routes.json').read_text())['routes']
            reader.need(routes==['blinking','lighting_mismatch','lod_corruption','missing_object'],'FZ4 explicit routes differ')
            result['routes']=routes
            scan=json.loads((root/'plugin-frames/epoch01/host-custom-depth.json').read_text())
            reader.need(scan['admitted'] and not scan['bench_bypass'] and scan['result'] in ('none','writer'),'FZ3 guard admission missing')
            result['host_custom_depth']=scan

        except Exception as error:result=dict(result='FAIL',reason=str(error))
        create_json(root/'reader.json',result)
        shader_failure=any('error' in line.lower() and ('shader' in line.lower() or '.usf' in line.lower()) for line in log.splitlines())
        raw=freeze(root,list(process.glob('*'))+[p for p in (root/'plugin-frames').rglob('*') if p.is_file()])
        row.update(outcome(leg,row,protected,upload,result,driver,row.get('new_crash_report_folders',[]),raw))
        if shader_failure:
            row.update(crashed=True,result='FAIL',shader_compile_failure=True)
        prior_crashes=sum(bool(json.loads(p.read_text()).get('crashed')) for p in (J/'c5-attempts').glob('*/*/result.json'))
        row=crash_policy(row,prior_crashes);create_json(root/'result.json',row)
        print(leg+': '+row['result']+' '+json.dumps({k:result.get(k) for k in ('result','reason','frames','per_anomaly','census')}),flush=True)

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('action',choices=['arm','run']);p.add_argument('--leg',choices=['C5']);p.add_argument('--number',choices=['01','02'],default='01');p.add_argument('--retry',action='store_true')
    a=p.parse_args()
    if a.action=='arm':print(json.dumps(dict(result='PASS',slots=arm(a.retry,a.leg)['slots'])))
    else:run(a.leg,a.number)
