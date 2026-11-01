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

J=c1.ROOT/'Receipts/09433c'
AUTHORITY='094-33c FQ1-FQ4 FN2 FM1 FJ1 FK1'
INSTALLED=c1.INSTALLED
DEADLINE="2026-11-01T18:29:00+00:00"

def command(leg,root):
    if leg=='Smoke':return c1.command('Smoke',root)
    if leg!='C3':raise RuntimeError('Unarmed C3 leg')
    argv=c1.command('C1Game',root)
    argv=[a.replace('/Game/EuropaPhaseA/EA_Travel','/Game/EuropaPhaseA/EA_Core') for a in argv if not a.startswith('-IAICaptureDiagnostics=')]
    argv=[a.replace('IAI.Capture.C1 '+(root/'plugin-frames').as_posix()+' 60','IAI.Capture.C3 '+(root/'plugin-frames').as_posix()) for a in argv]
    argv=[a.replace('-ExecCmds=','-ExecCmds=r.EyeAdaptationQuality 0,r.EyeAdaptationQuality,') for a in argv]
    c1.require_frame_format(argv)
    return argv

def validate(config):
    if config.get('arming_job')!='09433c' or config.get('stage')!='C3' or config['deadline']!=DEADLINE:raise RuntimeError('Wrong C3 arming')
    if config.get('metrics')!=reader.THRESHOLDS or config.get('limits')!={'C3':180}:raise RuntimeError('C3 thresholds or limit changed')
    verify_files(config['engine_files']);verify_files(config['tooling_files']);verify_protection(config['protected'])
    if sha(config['manifest']['path'])!=config['manifest']['sha256']:raise RuntimeError('Manifest changed')
    verify_files(json.loads(Path(config['manifest']['path']).read_text())['files'])
    verify_files(config['reference_files'])

def retry_admitted(config):
    first=Path(config['slots']['01'])
    result=json.loads((first/'result.json').read_text())
    if result['result']!='FAIL' or result.get('end_run'):raise RuntimeError('Retry requires a nonterminal failed first session')
    ensure_frozen(first/'raw-evidence.json')
    proof=json.loads((J/'retry-qualification.json').read_text())
    if proof.get('result')!='PASS' or not proof.get('diagnosis') or not proof.get('quick_testable_fix') or not proof.get('tests'):raise RuntimeError('Retry lacks qualified fix')
    verify_files(proof['tests'])
    if Path(config['slots']['02']).exists():raise RuntimeError('Retry is already spent')
    return proof


def arm(retry=False):
    if subprocess.check_output(['git','-C',str(TOOLS.parent.parent),'status','--porcelain'],text=True).strip():raise RuntimeError('Commit before C3 arming')
    prior=json.loads(INSTALLED.read_text())
    if retry:
        if prior.get('arming_job')!='09433c':raise RuntimeError('No first 09433c arming')
        retry_admitted(prior)
    elif prior.get('arming_job')=='09433c':raise RuntimeError('C3 already armed; use the qualified retry route')
    revision='02' if retry else '01'
    built=[json.loads(p.read_text()) for p in (J/'builds').glob('*/result.json')]
    good=[r for r in built if r.get('gate_result')=='PASS' and r.get('exit_code')==0 and c1.native_source_equal(r['commit'])]
    assert {r['target'] for r in good}=={'StylizedParisStreetEditor','StylizedParisStreet'}
    head=subprocess.check_output(['git','-C',str(TOOLS.parent.parent),'rev-parse','HEAD'],text=True).strip()
    before=json.loads((J/'entry.json').read_text())['protected'];current=protection_snapshot()
    assert all(before[k]==current[k] for k in ('content','config','descriptors'))
    manifest=json.loads(Path(prior['manifest']['path']).read_text())
    for row in manifest['files']:
        p=Path(row['path']);row.update(size=p.stat().st_size,mtime_ns=p.stat().st_mtime_ns,sha256=sha(p))
    manifest.update(source_commit=head,tooling_commit=head,build_sources={r['target']:r['commit'] for r in good},purpose='09433c FQ2-FQ3 C3 build pair')
    manifest_path=J/('c3-425-binaries-'+revision+'.json');create_json(manifest_path,manifest)
    refs=json.loads((J/'c2-reference-plan.json').read_text())
    config=dict(arming_job='09433c',stage='C3',authority=AUTHORITY,source_commit=head,deadline=DEADLINE,
        manifest=dict(path=str(manifest_path),sha256=sha(manifest_path)),protected=current,
        engine_files=prior['engine_files'],tooling_files=[dict(path=str(p),size=p.stat().st_size,sha256=sha(p)) for p in sorted(TOOLS.glob('*.py'))],
        reference_files=refs['files'],references=refs['references'],metrics=reader.THRESHOLDS,
        slots={number:str(J/'c3-attempts/C3'/number) for number in ('01','02')},limits=dict(C3=180),retry_authorized=retry)
    validate(config)
    create_json(J/('c3-prior-arming-'+revision+'.json'),prior);create_json(J/('c3-arming-'+revision+'.json'),config)
    INSTALLED.write_text(json.dumps(config,indent=2),encoding='utf-8')
    launcher=c1.ROOT/'Launch/Europa425-Runtime.bat'
    launcher.write_text('@echo off\n"C:\\Python313\\python.exe" -B "'+str(TOOLS/'capture_c3_kit.py')+'" run --leg C3 --number '+revision+'\n',encoding='ascii')
    return config


def run(leg,number):
    config=json.loads(INSTALLED.read_text());validate(config)
    if leg!='C3' or number not in config['slots']:raise RuntimeError('Unarmed leg')
    if number=='02':
        if not config.get('retry_authorized'):raise RuntimeError('Retry not qualified/re-armed')
        retry_admitted(config)
    root=Path(config['slots'][number]);temp=c1.ROOT/'Temp/09433c/c3'/number
    os.environ['EUROPA_JOB_CONFIG']=str(J/'c3-job-config.json')
    import write_ledger as ledger
    from runtime_lean_gate import wait
    with Guard(dict(state_root=str(J/'c3-state'/number))) as guard:
        guard.check_unused_launch();root.mkdir(parents=True,exist_ok=False);temp.mkdir(parents=True,exist_ok=False);(root/'session').mkdir()
        argv=command(leg,root);create_json(root/'prepared.json',dict(argv=argv,leg=leg))
        print(leg+': waiting for FJ1/FK1 gate.',flush=True)
        gate=wait(root/'gate.jsonl',DEADLINE,idle_seconds=300)
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
            reader.need('Capture425 C3: shaders_loaded=visible425,reduce' in log,'Mask shaders did not load/dispatch')
            reader.need('Capture425 C3: stencil_restore=1 owners=4' in log,'Stencil restoration missing')
            from capture_c2_reader import read as read_labels
            labels=read_labels(str(root/'plugin-frames/epoch01'),config['references'],profile='C3')
            create_json(root/'c2-reader.json',labels)
            result=reader.read(root/'plugin-frames/epoch01',config['references'])
            negatives=[]
            for mutant in ('shift','wrong_target'):
                negative=reader.read(root/'plugin-frames/epoch01',config['references'],mutant=mutant)
                create_json(root/('mask-negative-'+mutant+'.json'),negative)
                negatives.append(dict(kind=mutant,result='PASS' if all(r['result']=='FAIL' for r in negative['per_anomaly']) else 'FAIL',reader_result=negative['result'],per_anomaly=[dict(anomaly=r['anomaly'],result=r['result'],failed_frames=r['failed_frames'],dirty_clean_frames=r['dirty_clean_frames']) for r in negative['per_anomaly']]))
            result['mask_negatives']=negatives
            result['c2']=dict(result=labels['result'],mismatches=labels['mismatches'],ambiguous=labels['ambiguous'])
            if labels['result']!='PASS' or any(n['result']!='PASS' for n in negatives):result['result']='FAIL'
            from capture_fo_reader import check as check_ui
            result['ui_free']=check_ui(root/'plugin-frames','C2')

        except Exception as error:result=dict(result='FAIL',reason=str(error))
        create_json(root/'reader.json',result)
        shader_failure=any('error' in line.lower() and ('shader' in line.lower() or '.usf' in line.lower()) for line in log.splitlines())
        raw=freeze(root,list(process.glob('*'))+[p for p in (root/'plugin-frames').rglob('*') if p.is_file()])
        row.update(outcome(leg,row,protected,upload,result,driver,row.get('new_crash_report_folders',[]),raw))
        if shader_failure:
            row.update(crashed=True,result='FAIL',shader_compile_failure=True)
        prior_crashes=sum(bool(json.loads(p.read_text()).get('crashed')) for p in (J/'c3-attempts').glob('*/*/result.json'))
        row=crash_policy(row,prior_crashes);create_json(root/'result.json',row)
        print(leg+': '+row['result']+' '+json.dumps({k:v for k,v in result.items() if k not in ('pixel_measurements','events','per_frame','ui_free','clean_noise','clean_frames')}),flush=True)

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('action',choices=['arm','run']);p.add_argument('--leg',choices=['C3']);p.add_argument('--number',choices=['01','02'],default='01');p.add_argument('--retry',action='store_true')
    a=p.parse_args()
    if a.action=='arm':print(json.dumps(dict(result='PASS',slots=arm(a.retry)['slots'])))
    else:run(a.leg,a.number)
