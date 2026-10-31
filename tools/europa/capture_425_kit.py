import argparse
import datetime
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

from fixture_policy import HOST,TOOLS,ROOT,sha,verify_files,UPLOAD,ANALYTICS,managed_snapshot
from owner_attempts import Guard,create_json,stamp
from runtime_policy import protection_snapshot,verify_protection
from prepare_runtime import prepare
from runtime_runner import execute,crash_policy
from runtime_evidence import freeze,outcome
from fixture_uploads import check as uploads
from capture_425_reader import check as read_frames

JOB=ROOT/'Receipts/09432'
TEMP=ROOT/'Temp/09432/runtime'
DEADLINE='2026-11-01T06:30:00+00:00'
AUTHORITY='094-32 FN1-FN5 FM1 FL1-FL6 FJ1 FK1'
INSTALLED=ROOT/'Launch/Europa425-Runtime.json'
REFERENCE=ROOT/'Receipts/09427e/runtime-attempts/A7_-lod_corruption-positive/01/session/probe/lod_corruption-0-before.png'

def require_frame_format(argv):
    if any('defaultbackbufferpixelformat' in a.casefold() for a in argv):raise RuntimeError('FN2 refuses any backbuffer override')

def require_slot_history(config,leg,number,old,smoke_result):
    if leg!='Smoke' and smoke_result!='PASS':
        raise RuntimeError('Passing C1 binding smoke for this core revision required')
    if leg=='Smoke' and number==2:
        if config.get('guard_requalification')!='09432-UBT-command-race':raise RuntimeError('Unarmed core requalification')
    elif number==2 and not any(r['leg']==leg and r['result']=='FAIL' and not r['end_run'] for r in old):
        raise RuntimeError('Retry requires a diagnosed failed first session')

def native_source_equal(commit):
    code=subprocess.call(['git','-C',str(TOOLS.parent.parent),'diff','--quiet',commit,'HEAD','--','Source','Shaders','AnomalyInjector.uplugin'])
    if code not in (0,1):raise RuntimeError('Cannot compare build source')
    return code==0

def command(leg,root):
    if leg not in ('Smoke','C1PIE','C1Game'):raise RuntimeError('Unarmed C1 leg')
    argv=prepare('425',root/'session',True,False,leg!='C1Game')
    argv.insert(2,'/Engine/Maps/Entry' if leg!='C1Game' else '/Game/EuropaPhaseA/EA_Travel')
    argv += [UPLOAD,ANALYTICS,'-nowrite','-SkipCompile','-stdout','-unattended','-NoSplash',
        '-ini:EditorPerProjectUserSettings:[/Script/UnrealEd.EditorLoadingSavingSettings]:bAutoSaveEnable=False',
        '-ini:EditorPerProjectUserSettings:[/Script/UnrealEd.LevelEditorPlaySettings]:bAutoCompileBlueprintsOnLaunch=False,RunUnderOneProcess=True,PlayNumberOfClients=1,PlayNetMode=PIE_Standalone,NewWindowWidth=1280,NewWindowHeight=720,ShowMouseControlLabel=False']
    commands=['r.PostProcessAAQuality 0','r.ScreenPercentage 100','r.DynamicRes.OperationMode 0','r.MotionBlurQuality 0','t.MaxFPS 30']
    if leg!='C1Game':commands += ['py '+(root/'launch.py').as_posix()]
    else:
        argv += ['-game','-IAICaptureExit']
        commands += ['IAI.Capture.C1 '+(root/'plugin-frames').as_posix()+' 60']
    if leg!='Smoke':argv += ['-IAICaptureDiagnostics='+(root/'diagnostics').as_posix()]
    argv += ['-ExecCmds='+','.join(commands)]
    require_frame_format(argv)
    return argv

def run_session(leg,*args,**kwargs):
    mode={'Smoke':'Smoke','C1PIE':'Load','C1Game':'C1Game'}[leg]
    return execute(mode,*args,**kwargs)

def arm():
    if subprocess.check_output(['git','-C',str(TOOLS.parent.parent),'status','--porcelain'],text=True).strip():raise RuntimeError('Commit before arming C1')
    prior=json.loads(INSTALLED.read_text())
    assert prior.get('arming_job')!='09432'
    verify_files(prior['engine_files'])
    built=[json.loads(p.read_text()) for p in (JOB/'builds').glob('*/result.json')]
    built=[r for r in built if r.get('gate_result')=='PASS' and r.get('exit_code')==0 and native_source_equal(r['commit'])]
    for target in ('StylizedParisStreetEditor','StylizedParisStreet'):
        if not any(r.get('target')==target and r.get('gate_result')=='PASS' and r.get('exit_code')==0 for r in built):
            raise RuntimeError('Both qualified C1 builds are required before arming')
    entry=json.loads((JOB/'entry.json').read_text())['protected']
    current=protection_snapshot()
    if any(entry[k]!=current[k] for k in ('content','config','descriptors')):raise RuntimeError('Protected change since entry')
    manifest=json.loads((ROOT/'Receipts/09431b/final-425-binaries.json').read_text())
    for row in manifest['files']:
        path=Path(row['path']);row.update(size=path.stat().st_size,mtime_ns=path.stat().st_mtime_ns,sha256=sha(path))
    head=__import__('subprocess').check_output(['git','-C',str(TOOLS.parent.parent),'rev-parse','HEAD'],text=True).strip()
    manifest.update(source_commit=head,tooling_commit=head,authority=AUTHORITY,purpose='C1 authorized Development rebuild; Shipping retained',
        production_source_equal='C1 changed capture source; no Phase A production-equality claim',
        build_sources={r['target']:r['commit'] for r in built if r.get('gate_result')=='PASS'},
        prior_manifest=str(ROOT/'Receipts/09431b/final-425-binaries.json'),
        prior_manifest_sha256=sha(ROOT/'Receipts/09431b/final-425-binaries.json'))
    manifest_path=JOB/'final-425-binaries.json';create_json(manifest_path,manifest)
    verify_files(manifest['files']);verify_files(manifest['probe_source'])
    memory=dict(start_physical_GiB=4,start_commit_GiB=8,stop_physical_GiB=None,stop_commit_GiB=4)
    scope=dict(host='425',kind='phase-a-runtime',legs=['Smoke','C1PIE','C1Game'],maximum_sessions_per_leg=2,
        pie=True,standalone_game=True,save=False,cook=False,commandlet=False,packaging=False,build=False,content_changes=False)
    job=dict(job_root=str(JOB),temp_root=str(TEMP),authority=AUTHORITY,started_utc=stamp(),
        build_pairs=[],query_pairs=[],max_parallel_actions=1,memory=memory,runtime_authorized=True,runtime_scope=scope,
        window_deadline_utc=DEADLINE)
    create_json(JOB/'capture-job-config.json',job)
    config=dict(arming_job='09432',authority=AUTHORITY,source_commit=head,scope=scope,memory=memory,
        window_deadline_utc=DEADLINE,session_seconds=90,protected=protection_snapshot(),engine_files=prior['engine_files'],
        manifest=dict(path=str(manifest_path),sha256=sha(manifest_path)),reference=dict(path=str(REFERENCE),sha256=sha(REFERENCE)),
        tooling_files=[dict(path=str(p),size=p.stat().st_size,sha256=sha(p)) for p in sorted(TOOLS.glob('*.py'))],
        slots={leg:{str(n):str(JOB/'capture-attempts'/leg/('%02d'%n)) for n in ((1,) if leg=='Smoke' else (1,2))} for leg in scope['legs']})
    create_json(JOB/'superseded-runtime.json',prior)
    create_json(JOB/'capture-arming.json',config)
    shutil.copyfile(JOB/'capture-arming.json',INSTALLED)
    launcher=ROOT/'Launch/Europa425-Runtime.bat'
    shutil.copyfile(launcher,JOB/'superseded-runtime.bat')
    launcher.write_text('@echo off\n"C:\\Python313\\python.exe" -B "'+str(TOOLS/'capture_425_kit.py')+'" run\n',encoding='ascii')
    create_json(JOB/'arming-receipt.json',dict(result='PASS',slots=config['slots'],manifest=config['manifest'],source_commit=head,
        prior_job=prior['arming_job'],prior_refused=True,launches=0))
    return config

def validate(config):
    if config.get('arming_job')!='09432' or config.get('authority')!=AUTHORITY:raise RuntimeError('Spent/older arming refused')
    if config['session_seconds']!=90 or config['window_deadline_utc']!=DEADLINE:raise RuntimeError('C1 scope changed')
    verify_files(config['tooling_files'])
    verify_files(config['engine_files'])
    manifest=Path(config['manifest']['path'])
    if sha(manifest)!=config['manifest']['sha256']:raise RuntimeError('Manifest changed')
    verify_files(json.loads(manifest.read_text())['files'])
    if sha(config['reference']['path'])!=config['reference']['sha256']:raise RuntimeError('Reference changed')
    verify_protection(config['protected'])

def run(only=None,number=1):
    config=json.loads(INSTALLED.read_text());validate(config)
    if json.loads((JOB/'capture-dry/summary.json').read_text()).get('result')!='PASS':raise RuntimeError('C1 launcher qualification missing')
    os.environ['EUROPA_JOB_CONFIG']=str(JOB/'capture-job-config.json')
    import write_ledger as ledger
    from runtime_lean_gate import wait
    result=dict(result='RUNNING',legs=[],crash_count=0,started_utc=stamp())
    old=[json.loads(p.read_text()) for p in (JOB/'capture-attempts').glob('*/*/result.json')]
    if any(r.get('end_run') for r in old):raise RuntimeError('Prior hard STOP ends this armed run')
    result['crash_count']=sum(bool(r.get('crashed')) for r in old)
    for leg in ([only] if only else ['Smoke','C1PIE','C1Game']):
        if leg not in config['slots'] or str(number) not in config['slots'][leg]:raise RuntimeError('Unarmed slot')
        smoke_path=JOB/'capture-attempts/Smoke'/('%02d'%config.get('required_smoke_slot',1))/'result.json'
        require_slot_history(config,leg,number,old,json.loads(smoke_path.read_text()).get('result') if smoke_path.exists() else None)
        root=Path(config['slots'][leg][str(number)]);temp=TEMP/leg/('%02d'%number)
        with Guard(dict(state_root=str(JOB/'capture-state'/leg/('%02d'%number)))) as guard:
            guard.check_unused_launch();root.mkdir(parents=True,exist_ok=False);temp.mkdir(parents=True,exist_ok=False)
            (root/'session').mkdir()
            if leg!='C1Game':
                (root/'launch.py').write_text('import imp\n_capture=imp.load_source("capture_425_driver",%r)\n_capture.start(%r,%r,%r)\n'%
                    (str(TOOLS/'capture_425_driver.py'),str(root/'driver.json'),str(root/'plugin-frames'),leg=='Smoke'),encoding='utf-8')
            argv=command(leg,root)
            create_json(root/'prepared.json',dict(leg=leg,argv=argv))
            print(leg+': waiting for one FJ1/FK1 reading (5 idle minutes, 4/8 GiB).',flush=True)
            gate=wait(root/'gate.jsonl',DEADLINE,idle_seconds=300)
            if gate['status']!='CLEAR':result['result']='WINDOW_ENDED';break
            validate(config)
            create_json(root/'protection-before.json',dict(result='PASS',snapshot=protection_snapshot()))
            create_json(root/'managed-before.json',managed_snapshot())
            process=root/'process';process.mkdir();os.environ['EUROPA_OPERATION_DIR']=str(process)
            guard.reserve_launch(dict(number=number))
            print(leg+': launching bounded C1 session.',flush=True)
            row=run_session(leg,root,temp,HOST,config['protected'],argv,ledger,90,
                on_started=lambda pid:create_json(JOB/'capture-state'/leg/('%02d'%number)/'launched.json',dict(pid=pid,utc=stamp())))
            row['leg']=leg
            try:validate(config);protected=dict(result='PASS')
            except Exception as error:protected=dict(result='FAIL',reason=str(error))
            create_json(root/'protection-after.json',protected)
            create_json(root/'managed-after.json',managed_snapshot())
            upload=uploads(root/'session');create_json(root/'upload-check.json',upload)
            driver=None
            try:
                driver=json.loads((root/'driver.json').read_text()) if (root/'driver.json').exists() else None
                if leg=='Smoke':
                    if not driver or driver.get('result')!='SMOKE_PASS':raise ValueError('C1 binding smoke failed')
                    reader=dict(result='PASS',bindings=driver['bindings'],loads=0,saves=0)
                else:reader=read_frames(str(root),str(REFERENCE))
                log=(root/'session/Runtime.log').read_text(encoding='utf-8-sig',errors='replace')
                if row.get('exit_code')!=0 or 'LogExit: Exiting.' not in log:raise ValueError('Normal exit missing')
                if 'AnomalyControlServer module started (compiled out: ANOMALY_CONTROL_SERVER=0).' not in log or 'AnomalyShaders compiled out (ANOMALY_SHADERS=0).' not in log:raise ValueError('C1 disabled modules differ')
            except Exception as error:reader=dict(result='FAIL',reason=str(error))
            create_json(root/'reader.json',reader)
            raw=freeze(root,list(process.glob('*'))+list((root/'plugin-frames').glob('*'))+list((root/'diagnostics').glob('*')))
            row.update(outcome(leg,row,protected,upload,reader,driver=driver,crashes=row.get('new_crash_report_folders',[]),raw=raw))
            row=crash_policy(row,result['crash_count']);result['crash_count']=row['crash_count']
            create_json(root/'result.json',row);result['legs'].append(row)
            old.append(row)
            print(leg+': '+row['result']+' '+str({k:v for k,v in reader.items() if k!='frames'}),flush=True)
            if row['end_run']:result['result']='STOP';break
    if result['result']=='RUNNING':result['result']='PASS' if all(r['result']=='PASS' for r in result['legs']) else 'FAIL'
    result['ended_utc']=stamp()
    create_json(JOB/('capture-run-'+(only or 'both')+'-'+str(number)+'.json'),result)
    return result

def qualify():
    config=json.loads(INSTALLED.read_text());validate(config)
    base=JOB/'capture-dry';base.mkdir(exist_ok=False)
    from runtime_lean_gate import reasons
    from fixture_rehearsal import samples
    sample=samples()[-1];sample.update(idle_seconds=300,free_memory_bytes=5*1024**3,free_commit_bytes=9*1024**3)
    for drive in sample['volumes']:sample['volumes'][drive]['free_bytes']=100*1024**3
    assert not reasons(sample,300)
    rows=[]
    for leg in config['slots']:
        root=base/leg;root.mkdir();argv=command(leg,root)
        assert UPLOAD in argv and ANALYTICS in argv and '-nowrite' in argv and '-unattended' in argv
        assert ('-game' in argv)==(leg=='C1Game')
        row=dict(result='PASS',leg=leg,argv=argv,launches=0,real_slot_unused=True,synthetic_gate=sample)
        create_json(root/'result.json',row);rows.append(row)
    from idle_gate import collect
    sample=collect()
    create_json(JOB/'capture-live-reading.json',dict(sample=sample,reasons=reasons(sample,300),observe_only=True,launches=0))
    create_json(base/'summary.json',dict(result='PASS',legs=rows,launches=0))
    return rows

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('action',choices=['arm','qualify','run']);p.add_argument('--leg',choices=['Smoke','C1PIE','C1Game']);p.add_argument('--number',type=int,default=1)
    a=p.parse_args()
    if a.action=='arm':arm()
    elif a.action=='qualify':qualify()
    else:run(a.leg,a.number)
