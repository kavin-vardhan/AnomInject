import argparse,datetime,json,os,re,subprocess,sys
from pathlib import Path
R=Path(__file__).resolve().parent
P=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector')
T=P/'tools/europa';sys.path.insert(0,str(T))
J=Path('D:/IA_Europa/Receipts/09445')
BUILDJ=J
B=Path('E:/IA_BuildCache/09445-evidence')
F=Path('E:/IA_Fixtures/Outline425')
INSTALLED=Path('D:/IA_Europa/Launch/Europa425-09445-H4.json')
SPECS={'Calibration':('D3D11',1280,720),'D3D11-1280':('D3D11',1280,720),'D3D12-1280':('D3D12',1280,720),'D3D12-1279':('D3D12',1279,719)}
import capture_c5_kit as old
import capture_425_kit as c1
import host_reader09445 as reader
from window09445 import require_launch,require_runtime,admission_deadline
from fixture_policy import HOST,sha,verify_files
from runtime_policy import protection_snapshot,verify_protection
from owner_attempts import Guard,create_json
from runtime_runner import execute,crash_policy
from runtime_evidence import freeze,outcome
from fixture_uploads import check as uploads

def files(paths):return [dict(path=str(p),size=p.stat().st_size,sha256=sha(p)) for p in paths]
def unscored_setup_abort(path):
    assert path==J/'H4/D3D11-1280/01/result.json','Earlier gate failed; no further launch'
    value=json.loads(path.read_text());stop=json.loads((path.parent/'owned-setup-stop.json').read_text())
    diagnostic=json.loads((J/'setup-abort-diagnostic.json').read_text())
    capture=json.loads((B/'D3D11-1280/01/attempt-01/geometry-000/capture.json').read_text())
    native=json.loads((B/'D3D11-1280/01/attempt-01/geometry-000/labels-receipt.json').read_text())
    assert diagnostic['result']=='PASS' and value['exit_code']==0 and not value['crashed'] and not value['end_run']
    assert capture['written']==0 and not native['events'] and not (B/'D3D11-1280/01/plugin-frames').exists()
    assert stop['pid']==value['pid'] and stop['stop_intents'][0]['utc']<value['root_ended_utc']
    from runtime_evidence import ensure_frozen
    ensure_frozen(path.parent/'raw-evidence.json')

def arm(leg,number):
    require_launch()
    assert leg in SPECS and number in ('01','02')
    root=J/'H4'/leg/number
    assert not root.exists()
    for prior_path in (J/'H4').glob('*/*/result.json'):
        prior=json.loads(prior_path.read_text())
        if prior['result']=='PASS':continue
        unscored_setup_abort(prior_path)

    audit=json.loads((BUILDJ/'build-audit-02.json').read_text())
    assert sha(P.parents[1]/'Source/EuropaPhaseAProbe/EuropaTextureBench09444e.cpp')==audit['helper_sha256'],'Unbuilt host helper'
    assert not subprocess.check_output(['git','-C',str(P),'status','--porcelain'],text=True).strip()
    if leg!='Calibration':
        assert json.loads((J/'freeze.json').read_text())['result']=='PASS'
    if number=='02':
        prior=json.loads((root.parent/'01/result.json').read_text())
        fix=json.loads((J/('H4-retry-'+leg+'.json')).read_text())
        assert prior['result']=='FAIL' and not prior['end_run'] and fix['result']=='PASS' and fix['tests']
    builds=[json.loads(p.read_text()) for p in (BUILDJ/'builds').glob('*/result.json')]
    good=[b for b in builds if b.get('gate_result')=='PASS' and b.get('exit_code')==0 and c1.native_source_equal(b['commit'])]
    assert {b['target'] for b in good}=={'StylizedParisStreetEditor','StylizedParisStreet'}
    entry=json.loads((J/'entry.json').read_text())['protected'];protected=protection_snapshot()
    assert all(entry[k]==protected[k] for k in ('content','config','descriptors'))
    prior=json.loads(c1.INSTALLED.read_text())
    manifest=json.loads(Path(prior['manifest']['path']).read_text())
    for row in manifest['files']:
        p=Path(row['path']);row.update(size=p.stat().st_size,mtime_ns=p.stat().st_mtime_ns,sha256=sha(p))
    head=subprocess.check_output(['git','-C',str(P),'rev-parse','HEAD'],text=True).strip()
    manifest.update(source_commit=head,tooling_commit=head,build_sources={b['target']:b['commit'] for b in good},purpose='094-45 GL2-GL5 texture default-pool regression',
        production_source_equal='Built native source matches current; new bench-only private geometry probe and locked home-host helper',prior_manifest=prior['manifest'])
    path=J/('H4-manifest-'+leg+'-'+number+'.json');create_json(path,manifest)
    cfg=dict(job='09445-Textures-H4',leg=leg,number=number,root=str(root),spec=SPECS[leg],source=head,protected=protected,
        manifest=dict(path=str(path),sha256=sha(path)),engine=prior['engine_files'],
        tools=files([Path(__file__),R/'host_reader09445.py',R/'calibration_reader09445.py',R/'console_probe09443r.py',R/'geometry_reader09445.py',R/'window09445.py',R/'c_space09445.py',*T.glob('*.py')]),
        source_files=files([p for part in ('Source','Shaders') for p in (P/part).rglob('*') if p.is_file()]+list((P.parents[1]/'Source').rglob('*.cpp'))),limit=900)
    cfg.update(arming_job=cfg['job'],source_commit=head,engine_files=cfg['engine'],tooling_files=cfg['tools'],stage='H4')
    create_json(J/('H4-arming-'+leg+'-'+number+'.json'),cfg);INSTALLED.write_text(json.dumps(cfg,indent=2))
    create_json(J/('H4-prior-default-'+leg+'-'+number+'.json'),prior)
    launcher=c1.INSTALLED.with_suffix('.bat')
    (J/('H4-prior-default-'+leg+'-'+number+'.bat')).write_bytes(launcher.read_bytes())
    c1.INSTALLED.write_text(json.dumps(cfg,indent=2))
    launcher.write_text('@echo off\n"C:\\Python313\\python.exe" -B "'+str(Path(__file__).resolve())+'" run\n',encoding='ascii')
    print(json.dumps(dict(result='ARMED',leg=leg,source=head,manifest=cfg['manifest'])),flush=True)
def validate(cfg):
    assert cfg['job']=='09445-Textures-H4' and tuple(cfg['spec'])==SPECS[cfg['leg']]
    assert cfg['limit']==900
    for key in ('engine','tools','source_files'):verify_files(cfg[key])
    assert sha(cfg['manifest']['path'])==cfg['manifest']['sha256']
    verify_files(json.loads(Path(cfg['manifest']['path']).read_text())['files']);verify_protection(cfg['protected'])
def command(cfg,root):
    argv=old.command('C5',root);old.select_map(argv,'fallback')
    macro=root/'Binaries/capture.txt'
    output=B/cfg['leg']/cfg['number']
    if cfg['leg']=='Calibration':
        mode='calibrate';exposure=0
    else:
        frozen=json.loads((J/'freeze.json').read_text());assert frozen['result']=='PASS'
        mode='run';exposure=frozen['exposure']
    command='Europa.TextureBench '+output.as_posix()+' '+mode+' '+str(exposure)
    macro.write_text(command+'\n',encoding='ascii')
    (root/'capture-command.json').write_text(json.dumps(dict(command=command,macro=str(macro),sha256=sha(macro),routes='compiled defaults'),indent=2))
    rhi,w,h=cfg['spec']
    argv=[a for a in argv if a.lower() not in ('-d3d11','-d3d12')]
    argv+=['-'+rhi.lower()]
    argv=['-ResX='+str(w) if a.lower().startswith('-resx=') else '-ResY='+str(h) if a.lower().startswith('-resy=') else a for a in argv]
    argv=[a.replace('-ExecCmds=','-ExecCmds=r.CustomDepth,') for a in argv]
    argv=[a for a in argv if a!='-IAICaptureExit']
    assert '-nowrite' in argv and '-IAIBench' in argv and '-'+rhi.lower() in argv and '-game' in argv and '-BENCHMARK' not in argv
    assert not any('IAIBenchBypassHostDepth' in a or 'IAICaptureLegacyReference' in a for a in argv)
    c1.require_frame_format(argv);return argv
def run():
    cfg=json.loads(INSTALLED.read_text());validate(cfg)
    leg=cfg['leg'];number=cfg['number'];root=Path(cfg['root'])
    assert root==J/'H4'/leg/number
    limits=json.loads((J/'c5-job-config.json').read_text());limits['temp_root']='D:/IA_Europa/Temp/09445/H4'
    limits['runtime_scope']['legs']=list(SPECS)
    path=J/'H4-job-config.json';path.write_text(json.dumps(limits,indent=2));os.environ['EUROPA_JOB_CONFIG']=str(path)
    import write_ledger as ledger
    import runtime_monitor
    from runtime_lean_gate import wait
    runtime_monitor.FLOOR_GIB=dict(runtime_monitor.FLOOR_GIB,E=50,D=50)
    original=runtime_monitor.Monitor
    class Monitor(original):
        def __init__(self,*args,**kwargs):
            super().__init__(*args,**kwargs)
            from console_probe09443r import Probe
            self.probe=Probe(B/leg/number)
            self.checked=set()
        def check(self,owned):
            require_runtime()
            super().check(owned)
            folder=B/leg/number
            for attempt in sorted(folder.glob('attempt-*')):
                def reject(value):
                    if attempt.name=='attempt-01':
                        decision=attempt/'reapply-request.json'
                        if not decision.exists():create_json(decision,dict(result='REAPPLY_ONCE',reason=value,utc=datetime.datetime.now(datetime.timezone.utc).isoformat()))
                    else:raise RuntimeError('GL2 second qualification failure: '+json.dumps(value))
                if (attempt/'reapply-request.json').exists():continue
                if (attempt/'view-failed.json').exists():
                    reject(json.loads((attempt/'view-failed.json').read_text()));continue
                for ready in sorted(attempt.glob('geometry-*/geometry-ready.json')):
                    if str(ready) in self.checked:continue
                    self.checked.add(str(ready))
                    from geometry_reader09445 import read as geometry_read
                    try:value,_=geometry_read(ready.parent)
                    except Exception as error:value=dict(result='FAIL',reason=type(error).__name__+': '+str(error))
                    if value['result']=='PASS':create_json(ready.parent/'geometry-approved.json',dict(result='PASS',reader_sha256=sha(R/'geometry_reader09445.py'),utc=datetime.datetime.now(datetime.timezone.utc).isoformat()))
                    else:reject(value)
                if (attempt/'reapply-request.json').exists():continue
                prefire=attempt/'prefire.json'
                if prefire.exists() and str(prefire) not in self.checked:
                    self.checked.add(str(prefire))
                    from calibration_reader09445 import read
                    value=read(attempt,True)
                    if value['result']!='PASS':reject(value)
                    else:
                        frozen=json.loads((J/'freeze.json').read_text())
                        assert value['table'][0]['exposure']==frozen['exposure']
                        create_json(attempt/'prefire-approved.json',dict(result='PASS',utc=value['utc'],reader_sha256=sha(R/'calibration_reader09445.py'),exposure=frozen['exposure']))
                self.probe.check(self.pid)
    with Guard(dict(state_root=str(J/'H4-state'/leg/number))) as guard:
        guard.check_unused_launch();root.mkdir(parents=True,exist_ok=False);(root/'session').mkdir()
        process=root/'process';process.mkdir();os.environ['EUROPA_OPERATION_DIR']=str(process)
        argv=command(cfg,root);create_json(root/'prepared.json',dict(argv=argv,spec=cfg['spec']))
        def admit():
            import shutil
            assert shutil.disk_usage('E:/').free>=50*2**30
            assert shutil.disk_usage('D:/').free>=53*2**30,'GJ4: D: needs50GiB floor plus3GiB launch reserve; STOP if blocked'
            deadline=admission_deadline()
            gate=wait(root/'gate.jsonl',deadline,idle_seconds=300,memory=(8,4))
            assert gate['status']=='CLEAR','GA5 admission held three hours'
            validate(cfg);require_launch(cfg['limit']);guard.reserve_launch(dict(number=1,leg=leg));return gate
        from c_space09445 import snapshot,finish
        space_before=snapshot();create_json(root/'c-space-before.json',space_before)
        runtime_monitor.Monitor=Monitor
        try:row=execute(leg,root,Path(limits['temp_root'])/leg/number,HOST,cfg['protected'],argv,ledger,cfg['limit'],admit=admit)
        finally:
            runtime_monitor.Monitor=original
            space=finish(space_before,snapshot());create_json(root/'c-space-after.json',space)
        assert space['floor_pass'],'GL4 C: below10GiB'
        try:validate(cfg);protected=dict(result='PASS')
        except Exception as error:protected=dict(result='FAIL',reason=str(error))
        upload=uploads(root/'session');create_json(root/'upload-check.json',upload)
        log=(root/'session/Runtime.log').read_text(encoding='utf-8-sig',errors='replace')
        try:
            assert row.get('exit_code')==0 and 'LogExit: Exiting.' in log
            rhi,w,h=cfg['spec']
            if leg=='Calibration':
                from calibration_reader09445 import read
                result=read(B/leg/number)
            else:result=reader.read(B/leg/number/'plugin-frames/epoch01',log,True,(w,h),rhi)
            if leg!='Calibration':
                scan=json.loads((B/leg/number/'plugin-frames/epoch01/host-custom-depth.json').read_text())
                assert scan['admitted'] and scan['private_pass'] and not scan['bench_bypass']
                result['host_depth']=scan
        except Exception as error:result=dict(result='FAIL',reason=str(error))
        create_json(root/'reader.json',result)
        raw=freeze(root,[p for p in root.rglob('*') if p.is_file() and p.name!='raw-evidence.json']+[p for p in (B/leg/number).rglob('*') if p.is_file()])
        row.update(outcome(leg,row,protected,upload,result,crashes=row.get('new_crash_report_folders',[]),raw=raw))
        if any('error' in x.lower() and ('shader' in x.lower() or '.usf' in x.lower()) for x in log.splitlines()):row.update(crashed=True,result='FAIL',shader_failure=True)
        prior=sum(bool(json.loads(p.read_text()).get('crashed')) for p in (F/'Receipts/09445/textures').glob('*/*/result.json'))
        prior+=sum(bool(json.loads(p.read_text()).get('crashed')) for p in (J/'H4').glob('*/*/result.json'))
        row=crash_policy(row,prior);create_json(root/'result.json',row)
        print(json.dumps(dict(leg=leg,result=row['result'],reason=result.get('reason'),frames=result.get('frames'),rhi=result.get('rhi'))),flush=True)
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('action',choices=['arm','run']);parser.add_argument('--leg',choices=SPECS);parser.add_argument('--number',choices=['01','02'],default='01');a=parser.parse_args()
    if a.action=='arm':arm(a.leg,a.number)
    else:run()
