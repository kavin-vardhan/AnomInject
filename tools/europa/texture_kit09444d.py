import argparse,datetime,hashlib,json,os,re,subprocess,sys
from pathlib import Path
RUN=Path(__file__).resolve().parent
P=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector')
T=P/'tools/europa';sys.path.insert(0,str(T))
J=Path('D:/IA_Europa/Receipts/09444d')
OLDJ=Path('D:/IA_Europa/Receipts/09444c')
F=Path('E:/IA_Fixtures/Outline425')
E=Path('E:/EpicGames/UE_4.25')
PROJECT=F/'Outline425.uproject'
BASE=F/'Receipts/09444d/textures'
INSTALLED=F/'Launch/Outline425-09444d-textures.json'
LEGS=('texture-on',)
PACKAGES=('EA_Outline.umap','SM_OutlineSphereCube.uasset','SM_OutlineCubeSphere.uasset','M_Outline.uasset','M_Surface.uasset','M_Masked.uasset','M_WPO.uasset')
from owner_attempts import Guard,create_json,stamp
def effective(path):return json.loads(Path(path).read_text())
from fixture_policy import sha,verify_files,UPLOAD,ANALYTICS
from runtime_policy import protection_snapshot,verify_protection
from runtime_evidence import freeze,outcome
from runtime_runner import execute,crash_policy
from fixture_uploads import check as uploads
from runtime_lean_gate import wait
from blackout09444d import require_launch,require_runtime,admission_deadline


def scoped(path):
    path=Path(path)
    if path.resolve()!=path or not path.is_relative_to(F):raise RuntimeError('Fixture path escaped physical owned root')
    for parent in (path,*path.parents):
        if parent.exists() and parent.stat().st_file_attributes&1024:raise RuntimeError('Reparse point refused')
    return path


def allowed_author(path,leg):
    return leg=='mutant-author' and Path(path)==F/'Content/OutlineFixture/M_MissingTexture_NoSkeletal.uasset'


def files(paths):
    return [dict(path=str(p),size=p.stat().st_size,sha256=sha(p)) for p in sorted(set(paths))]


def protected_fixture():
    paths=[PROJECT]
    for name in ('Content','Config','Source','Scripts','Plugins/AnomalyInjector/Content','Plugins/AnomalyInjector/Binaries','Binaries','Saved/Config'):
        paths.extend(p for p in (F/name).rglob('*') if p.is_file())
    paths.append(F/'Plugins/AnomalyInjector/AnomalyInjector.uplugin')
    return files(paths)


def arm(leg,number='01'):
    require_launch()
    assert leg in LEGS and number in ('01','02')
    frozen_files=[]
    if leg!='preflight-cubes':
        base=F/'Receipts/09444c'
        frozen=effective(base/'freeze.json')
        calibration=effective(base/'calibration-cubes-result.json')
        assert calibration['result']=='PASS'
        assert sha(base/'calibration-cubes-result.json')==frozen['calibration_sha256']
        assert frozen['exposure']==next(row['exposure'] for row in calibration['table'] if row['qualified'])
        assert frozen['runtime_only'] and frozen['eye_adaptation_method']==3 and frozen['eye_adaptation_quality']==2
        frozen_files=[base/'freeze.json',base/'calibration-cubes-result.json',base/'cubes-calibration-prediction.json']
    for row in effective(OLDJ/'fixture-builds-uniform.json')['fixture_source']:
        assert sha(Path(row['path']))==row['sha256'],'Fixture native source differs from the rebuilt source'
    verify_files(effective(OLDJ/'fixture-builds-uniform.json')['files'])
    old_arm=effective(F/'Receipts/09444c/textures/texture-off/01-arming.json')
    verify_files(old_arm['engine'])
    root=scoped(BASE/leg/number)
    if root.exists():raise RuntimeError('Single-use fixture slot spent')
    if number=='02':
        first=effective(BASE/leg/'01/result.json')
        from runtime_children import short_child_retry
        prior_config=effective(BASE/leg/'01-arming.json')
        binary_rows=[row for row in prior_config['fixture'] if Path(row['path']).suffix.lower() in ('.exe','.dll','.lib','.pdb')]
        before={row['path']:row['sha256'] for row in binary_rows}
        after={path:sha(Path(path)) if Path(path).is_file() else None for path in before}
        observations=[]
        for path in (BASE/leg/'01/process').glob('*-process-polls.jsonl'):
            for line in path.read_text().splitlines():observations.extend(json.loads(line)['processes'])
        assert short_child_retry(first,observations,before,after,0),'GG2 retry is not authorized'
        create_json(BASE/leg/'02-retry-proof.json',dict(authority='094-44 GG2(a)',result='PASS',before=before,after=after,first_result=str(BASE/leg/'01/result.json')))
    tree=F/'Plugins/AnomalyInjector'
    head=subprocess.check_output(['git','-C',str(tree),'rev-parse','HEAD'],text=True).strip()
    status=subprocess.check_output(['git','-C',str(tree),'status','--porcelain','--untracked-files=all'],text=True).splitlines()
    assert all(row.startswith('?? Content/Engine425/Materials/') and Path(row[3:]).name in ('M_CorruptedTexture_Pink.uasset','M_MissingTexture_Checker.uasset') for row in status),status
    builds=[json.loads(p.read_text()) for folder in (F/'Receipts/09444r/builds',F/'Receipts/09444c/builds') for p in folder.glob('*/result.json')]
    def native_equal(commit):
        return all(subprocess.check_output(['git','-C',str(tree),'rev-parse',head+':'+part])==subprocess.check_output(['git','-C',str(tree),'rev-parse',commit+':'+part]) for part in ('Source','Shaders','AnomalyInjector.uplugin'))
    good=[v for v in builds if v['result']=='PASS' and native_equal(v['commit'])]
    native=good[-1]['commit'] if good else None
    assert {v['target'] for v in good}=={'Outline425Editor','Outline425'}
    root.parent.mkdir(parents=True,exist_ok=True);INSTALLED.parent.mkdir(exist_ok=True)
    source=files([p for name in ('Source','Shaders') for p in (tree/name).rglob('*') if p.is_file()])
    config=dict(job='09444d',authority='094-44d GI1-GI7',leg=leg,number=number,source=head,root=str(root),
        project=str(PROJECT),editor=str(E/'Engine/Binaries/Win64/UE4Editor.exe'),limit=2100 if leg in ('mutant-author',) else 900,
        native_build_commits=[native],paris=protection_snapshot(),fixture=[row for row in protected_fixture()+source if not allowed_author(row['path'],leg)],
        tools=files([Path(__file__),F/'Scripts/texture_pose09444d.py',F/'Scripts/capture_depth_read09444d.py',*T.glob('*.py'),*frozen_files]),engine=files([E/'Engine/Binaries/Win64/UE4Editor.exe',E/'Engine/Binaries/Win64/CrashReportClient.exe']))
    create_json(BASE/leg/(number+'-arming.json'),config);INSTALLED.write_text(json.dumps(config,indent=2))
    print(json.dumps(dict(result='ARMED',leg=leg,root=str(root),source=head)))
    return config


def validate(argv):
    if any(flag not in argv for flag in ('-nowrite','-d3d11','-IAIBench','-BENCHMARK')):
        raise RuntimeError('Fixture launch contract missing')
    if any('defaultbackbufferpixelformat' in a.lower() for a in argv):raise RuntimeError('Backbuffer override refused')
    if argv[0]!=str(E/'Engine/Binaries/Win64/UE4Editor.exe') or argv[1]!=str(PROJECT):raise RuntimeError('Fixture executable/project mismatch')


def capture_command(config):
    root=Path(config['root']);leg=config['leg'];session=root/'session';session.mkdir()
    workers=str(max(1,(os.cpu_count() or 2)-2))
    argv=[config['editor'],str(PROJECT),'/Game/OutlineFixture/EA_Outline','-d3d11','-nowrite',
        '-UserDir='+F.as_posix()+'/', '-ShaderWorkingDir='+(F/'ShaderWorking').as_posix()+'/', '-DDC=Outline425DDC',
        '-abslog='+(session/'Runtime.log').as_posix(),'-AbsCrashReportClientLog='+(session/'CrashClient.log').as_posix(),
        '-NoEpicPortal','-BENCHMARK','-NoLiveCoding','-NoHotReload','-windowed','-ResX=1280','-ResY=720','-IAIBench','-unattended','-NoSplash','-SkipCompile','-stdout',UPLOAD,ANALYTICS,
        '-ini:EditorPerProjectUserSettings:[/Script/UnrealEd.EditorLoadingSavingSettings]:bAutoSaveEnable=False',
        '-ini:Engine:[DevOptions.Shaders]:NumUnusedShaderCompilingThreads='+workers+',[DevOptions.Shaders]:NumUnusedShaderCompilingThreadsDuringGame='+workers+',[DevOptions.Shaders]:bAllowAsynchronousShaderCompiling=False',
        '-LogCmds=LogMaterial Verbose,LogShaderCompilers Display']
    commands=['r.PostProcessAAQuality 0','r.ScreenPercentage 100','r.DynamicRes.OperationMode 0','r.MotionBlurQuality 0','r.BloomQuality 0','r.EyeAdaptationQuality 2','r.EyeAdaptation.MethodOverride 3','t.MaxFPS 30']
    if leg in ('warmup','warm'):
        commands+=['OutlineFixture.ShaderProbe '+(root/'shader-probe.json').as_posix()]
    else:
        exposure=-2
        commands+=['r.CustomDepth']


        script=root/'pose-launch.py'
        chosen=None if leg=='preflight-cubes' else effective(F/'Receipts/09444c/freeze.json')['exposure']
        code="import sys,json\nimport unreal\nsys.path.insert(0,"+repr(str(F/'Scripts'))+")\nimport texture_pose09444d as driver\ntry:\n    state=driver.main("+repr(str(root))+","+repr(leg)+","+repr(chosen)+")\nexcept Exception as error:\n    with open("+repr(str(root/'pose-error.json'))+",'w') as stream:json.dump(dict(result='FAIL',error=str(error)),stream)\n    unreal.SystemLibrary.execute_console_command(unreal.EditorLevelLibrary.get_editor_world(),'QUIT_EDITOR')\n"
        script.write_text(code,encoding='ascii')
        commands+=['py '+script.as_posix()]
        argv+=['-EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities']
        if leg!='preflight-cubes':argv+=['-IAICaptureExit','-IAICaptureFallback']

    argv+=['-ExecCmds='+','.join(commands)];validate(argv);return argv


def command(config):
    if config['leg'] not in ('mutant-author',):
        return capture_command(config)
    root=Path(config['root']);leg=config['leg'];session=root/'session';session.mkdir()
    script=root/'author-launch.py'
    action="main("+repr(str(root/'author.json'))+")"
    module='author_texture_mutant09444'
    code="import sys,json\nimport unreal\nsys.path.insert(0,"+repr(str(T))+ ")\nsys.path.insert(0,"+repr(str(F/'Scripts'))+")\nimport "+module+" as author\ntry:\n    author."+action+"\nexcept Exception as error:\n    with open("+repr(str(root/'author-error.json'))+",'w') as stream:json.dump(dict(result='FAIL',error=str(error)),stream)\n    unreal.SystemLibrary.execute_console_command(None,'QUIT_EDITOR')\nelse:\n    unreal.SystemLibrary.execute_console_command(unreal.EditorLevelLibrary.get_editor_world(),"+repr('OutlineFixture.ShaderProbe '+(root/'author-shader-probe.json').as_posix())+")\n"
    script.write_text(code,encoding='ascii')
    workers=str(max(1,(os.cpu_count() or 2)-2))
    argv=[config['editor'],str(PROJECT),'/Engine/Maps/Entry','-d3d11','-nowrite','-IAIBench',
        '-UserDir='+F.as_posix()+'/', '-ShaderWorkingDir='+(F/'ShaderWorking').as_posix()+'/', '-DDC=Outline425DDC',
        '-abslog='+(session/'Runtime.log').as_posix(),'-AbsCrashReportClientLog='+(session/'CrashClient.log').as_posix(),
        '-NoEpicPortal','-BENCHMARK','-NoLiveCoding','-NoHotReload','-windowed','-ResX=1280','-ResY=720','-unattended','-NoSplash','-SkipCompile','-stdout',UPLOAD,ANALYTICS,
        '-BENCHMARK','-EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities',
        '-ini:EditorPerProjectUserSettings:[/Script/UnrealEd.EditorLoadingSavingSettings]:bAutoSaveEnable=False',
        '-ini:Engine:[DevOptions.Shaders]:NumUnusedShaderCompilingThreads='+workers+',[DevOptions.Shaders]:NumUnusedShaderCompilingThreadsDuringGame='+workers+',[DevOptions.Shaders]:bAllowAsynchronousShaderCompiling=False',
        '-LogCmds=LogMaterial Verbose,LogShaderCompilers Display','-ExecCmds=py '+script.as_posix()]
    validate(argv);return argv


def read(config,root,log):
    leg=config['leg']
    if 'LogExit: Exiting.' not in log:raise RuntimeError('Normal engine exit absent')
    if leg in ('mutant-author',):
        if (root/'author-error.json').exists():raise RuntimeError((root/'author-error.json').read_text())
        author=effective(root/'author.json');assert author['result']=='PASS'
        probe=effective(root/'author-shader-probe.json');assert probe['ready'] and probe['remaining_jobs']==0
        return dict(result='PASS',author=author,probe=probe)
    if leg in ('warmup','warm'):
        probe=json.loads((root/'shader-probe.json').read_text())
        assert probe['ready'] and not probe['remaining_jobs'] and not probe['spike_type_present']
        missed=re.findall(r'Missing cached shader map for material (.*?), compiling\.',log,re.I)
        if leg=='warm':assert len(missed)==0,'Warm material shader cache missed'
        return dict(result='PASS',probe=probe,missing_cached_shader_maps=len(missed),materials=missed)
    if leg=='preflight-cubes':
        assert not (root/'pose-error.json').exists()
        value=effective(root/'calibration/calibration.json')
        assert not value['auto_injection'] and value['preview_count']==51
        assert not re.search(r'AutoInjector:.*fired|: apply.*event',log,re.I)
        for sample in value['samples']:
            cap=effective(Path(sample['directory'])/'capture.json')
            assert cap['written']==8 and cap['dropped']==0
        return dict(result='PASS',previews=51,frames=408,anomaly_fires=0)
    epoch=root/'plugin-frames/epoch01'
    receipt=json.loads((epoch/'labels-receipt.json').read_text());scan=json.loads((epoch/'host-custom-depth.json').read_text())
    assert receipt['result']=='PASS' and receipt['paired']==2352 and receipt['dropped']==0
    assert scan['admitted'] and scan['result']=='reader'
    if leg!='mutant-author':
        rows=[json.loads(line) for line in (epoch/'labels.jsonl').read_text().splitlines()]
        assert scan['private_pass'] and not scan['bench_bypass']
        assert all(v['custom_depth']['on_count']==0 and v['custom_depth']['tagged_components']==0 and v['custom_depth']['unexpected']==0 for v in rows)
    missed=re.findall(r'Missing cached shader map for material (.*?), compiling\.',log,re.I)
    if leg=='texture-on':assert not missed,'Stage B warm material shader cache missed'
    assert receipt['routes']==(['missing_texture'] if leg=='texture-mutant' else ['blinking','lod_corruption','missing_texture','corrupted_texture'])
    setup=effective(root/'texture-setup.json');assert setup['result']=='PASS' and setup['runtime_only']
    assert setup['mode']==('mutant' if leg=='texture-mutant' else 'good')
    return dict(result='PASS',scan=scan,frames=receipt['paired'],routes=receipt['routes'],events=len(receipt['events']),missing_cached_shader_maps=len(missed))


def run():
    config=json.loads(INSTALLED.read_text());leg=config['leg'];root=scoped(Path(config['root']))
    assert config['job']=='09444d' and leg in LEGS and config['project']==str(PROJECT)
    verify_files(config['tools']);verify_files(config['fixture']);verify_files(config['engine']);verify_protection(config['paris'])
    state=J/'outline-texture-state'/leg/config['number']
    with Guard(dict(state_root=str(state))) as guard:
        guard.check_unused_launch();root.mkdir(exist_ok=False)
        argv=command(config);create_json(root/'prepared.json',dict(argv=argv,leg=leg))
        cfg=json.loads((J/'c5-job-config.json').read_text())
        cfg.update(fixture_project=PROJECT.as_posix(),fixture_engine=E.as_posix(),temp_root=(F/'Temp/runtime').as_posix())
        cfg['runtime_scope']=dict(cfg['runtime_scope'],host='outline425',legs=[leg],save=leg in ('mutant-author',),content_changes=leg in ('mutant-author',),standalone_game=False)
        path=J/'outline-texture-runtime-job.json';path.write_text(json.dumps(cfg,indent=2));os.environ['EUROPA_JOB_CONFIG']=str(path)
        process=root/'process';process.mkdir();os.environ['EUROPA_OPERATION_DIR']=str(process)
        import write_ledger as ledger
        import runtime_monitor
        runtime_monitor.FLOOR_GIB=dict(runtime_monitor.FLOOR_GIB,D=50,E=50)
        original=runtime_monitor.Monitor
        class FixtureMonitor(original):
            def __init__(self,*args,**kwargs):
                kwargs['game']=False
                super().__init__(*args,**kwargs)

            def check(self,owned):
                super().check(owned)
                require_runtime()
            def metadata(self):
                result=super().metadata()
                if leg in ('mutant-author',):result={p:v for p,v in result.items() if not allowed_author(p,leg)}
                return result
        runtime_monitor.Monitor=FixtureMonitor
        def admit():
            import shutil
            if any(shutil.disk_usage(d).free<50*2**30 for d in ('D:/','E:/')):raise RuntimeError('E: below 50 GiB floor')
            deadline=admission_deadline()
            result=wait(root/'gate.jsonl',deadline,idle_seconds=300,memory=(8,4))
            if result['status']!='CLEAR':raise RuntimeError('GA5: admission held three hours')
            if any(shutil.disk_usage(d).free<50*2**30 for d in ('D:/','E:/')):raise RuntimeError('E: below 50 GiB floor')
            verify_files(config['fixture']);verify_protection(config['paris'])
            require_launch(config['limit'])
            guard.reserve_launch(dict(number=1,leg=leg));return result
        row=execute('Outline-'+leg,root,F/'Temp/runtime'/leg, F,config['paris'],argv,ledger,config['limit'],admit=admit)
        runtime_monitor.Monitor=original
        try:
            verify_protection(config['paris']);verify_files(config['fixture'])
            protected=dict(result='PASS')
        except Exception as error:protected=dict(result='FAIL',reason=str(error))
        upload=uploads(root/'session');create_json(root/'upload-check.json',upload)
        log=(root/'session/Runtime.log').read_text(encoding='utf-8-sig',errors='replace') if (root/'session/Runtime.log').exists() else ''
        try:
            if row.get('exit_code')!=0:raise RuntimeError('Nonzero exit')
            result=read(config,root,log)
        except Exception as error:result=dict(result='FAIL',reason=str(error))
        create_json(root/'reader.json',result)
        raw=freeze(root,[p for p in root.rglob('*') if p.is_file() and p.name!='raw-evidence.json'])
        row.update(outcome(leg,row,protected,upload,result,crashes=row.get('new_crash_report_folders',[]),raw=raw))
        if any(('error' in line.lower() and ('shader' in line.lower() or '.usf' in line.lower())) for line in log.splitlines()):row.update(crashed=True,result='FAIL',shader_failure=True)
        prior=sum(bool(effective(p).get('crashed')) for p in BASE.glob('*/*/result.json'))
        prior+=sum(bool(json.loads(p.read_text()).get('crashed')) for p in (J/'c5-attempts').glob('*/*/result.json'))
        row=crash_policy(row,prior)
        if leg=='mutant-author' and row.get('crashed'):row.update(end_run=True,run_stop_reasons=['Authoring crash'])
        create_json(root/'result.json',row)
        print(json.dumps(dict(leg=leg,result=row['result'],reason=result.get('reason'),exit=row.get('exit_code'),root=str(root))),flush=True)
        return row


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('action',choices=['arm','run']);parser.add_argument('--leg',choices=LEGS);parser.add_argument('--number',default='01',choices=['01','02'])
    args=parser.parse_args()
    if args.action=='arm':arm(args.leg,args.number)
    else:run()
