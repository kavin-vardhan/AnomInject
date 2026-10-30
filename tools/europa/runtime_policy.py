import copy
import datetime
import json
from pathlib import Path

import fixture_policy as fixture
from fixture_policy import ROOT, HOST, TOOLS, canonical, sha, verify_files
from runtime_scripts import ROUTES

JOB = ROOT/'Receipts/09427'
TEMP = ROOT/'Temp/09427'
AUTHORITY = 'D:/IntrusiveAnomalies/_reviews/094-27-request.md#FB1-FB7'
WINDOW_END = '2026-10-31T00:30:00+00:00'
MANIFEST = dict(host='425', path=str(ROOT/'Receipts/09421/qualification/final-425-binaries.json'),
                sha256=fixture.MANIFESTS['425'])
LEGS = ['Smoke','Load','WorldsPIE','WorldsGame','GC']
LEGS += ['A7_:%s:positive' % r for r in ROUTES]
LEGS += ['A7_:%s:%s' % (r,c) for c in ('NoApply','SkipRestore') for r in ROUTES]
LEGS += ['Admission']
LIMITS = {name: (900 if name.endswith(':positive') else 600 if name in ('WorldsPIE','Admission') else 300)
          for name in LEGS}
SCOPE = dict(host='425', kind='phase-a-runtime', legs=LEGS, maximum_sessions_per_leg=1,
             pie=True, standalone_game=True, save=False, cook=False, commandlet=False,
             packaging=False, build=False, content_changes=False)
MEMORY = dict(start_physical_GiB=6,start_commit_GiB=8,stop_physical_GiB=None,stop_commit_GiB=6)


def slug(leg):
    if leg not in LEGS:
        raise RuntimeError('Launch outside the armed runtime legs')
    return leg.replace(':','-')


def paths(base, temp, leg):
    name = slug(leg)
    return dict(root=str(Path(base)/'runtime-attempts'/name/'01'),
                temp=str(Path(temp)/'runtime-attempts'/name/'01'),
                state=str(Path(base)/'runtime-state'/name))


def manifest_check(row):
    if row != MANIFEST or sha(row['path']) != row['sha256']:
        raise RuntimeError('Runtime must bind only the unchanged 4.25 manifest')
    data = json.loads(Path(row['path']).read_text())
    if len(data['files']) != 27:
        raise RuntimeError('4.25 bound artifact count changed')
    verify_files(data['files']); verify_files(data['probe_source'])
    return 27


def validate(config, mode):
    if config.get('arming_job') != '09427' or config.get('authority') != AUTHORITY:
        raise RuntimeError('Older or unknown runtime arming refused')
    if config.get('scope') != SCOPE or config.get('limits') != LIMITS or config.get('memory') != MEMORY:
        raise RuntimeError('Runtime scope/limits changed')
    if config.get('window_deadline_utc') != WINDOW_END or config.get('manifest') != MANIFEST:
        raise RuntimeError('Runtime deadline/4.25 binding changed')
    if config.get('mode') != mode or mode not in ('real','dry','rehearsal'):
        raise RuntimeError('Runtime mode mismatch')
    base, temp = canonical(config['receipts_root']), canonical(config['temp_root'])
    if mode == 'real':
        if base != JOB or temp != TEMP:
            raise RuntimeError('Real runtime slots must be 09427')
    elif not base.is_relative_to(JOB/'scratch') or not temp.is_relative_to(TEMP/'scratch'):
        raise RuntimeError('Throwaway runtime roots must be owned 09427 scratch')
    expected = {leg: paths(base,temp,leg) for leg in LEGS}
    if config.get('slots') != expected:
        raise RuntimeError('Slot paths differ from armed scope')
    for slot in expected.values():
        for path in slot.values():
            canonical(path)
    return True


def command(leg, root):
    slug(leg)
    root = canonical(root)
    editor = leg in ('Smoke','Load','WorldsPIE')
    from prepare_runtime import prepare
    argv = prepare('425',root/'session',True,leg in ('WorldsPIE','WorldsGame','GC'),editor)
    argv.insert(2, '/Engine/Maps/Entry' if editor else '/Game/EuropaPhaseA/EA_Core')
    argv += [fixture.UPLOAD,fixture.ANALYTICS,'-nowrite','-SkipCompile','-stdout','-unattended',
             '-LogCmds=LogLinker Verbose,LogStreaming Verbose',
             '-ini:EditorPerProjectUserSettings:[/Script/UnrealEd.EditorLoadingSavingSettings]:bAutoSaveEnable=False',
             '-ini:EditorPerProjectUserSettings:[/Script/UnrealEd.LevelEditorPlaySettings]:bAutoCompileBlueprintsOnLaunch=False']
    commands = ['r.PostProcessAAQuality 0','r.ScreenPercentage 100','r.DynamicRes.OperationMode 0',
                'r.MotionBlurQuality 0','t.MaxFPS 30']
    if editor:
        argv = [a.replace('PythonScriptPlugin,EditorScriptingUtilities','PythonScriptPlugin,EditorScriptingUtilities,EditorTests') for a in argv]
        argv += [
                  '-ini:EditorPerProjectUserSettings:[/Script/UnrealEd.LevelEditorPlaySettings]:RunUnderOneProcess=True,PlayNumberOfClients=2,PlayNetMode=PIE_Standalone,bLaunchSeparateServer=False',
                  '-ini:Engine:[/Script/UnrealEd.EditorEngine]:bAllowMultiplePIEWorlds=True',
                 '-ini:Engine:[/Script/Engine.AutomationTestSettings]:AutomationTestmap=/Game/EuropaPhaseA/EA_Core.EA_Core']
        commands += ['py '+(root/'script/launch.py').as_posix()]
    else:
        argv += ['-game']
        commands += ['Europa.Probe script '+(root/'script/native.json').as_posix()]
    argv += ['-ExecCmds='+','.join(commands)]
    if leg.endswith(':NoApply'):
        argv += ['-EuropaNoApply']
    if leg.endswith(':SkipRestore'):
        argv += ['-EuropaSkipRestore']
    return argv


def validate_argv(argv,leg,root):
    if argv != command(leg,root):
        raise RuntimeError('Runtime argv differs from bound leg/routing/opt-outs')
    if argv.count(fixture.UPLOAD) != 1 or argv.count(fixture.ANALYTICS) != 1:
        raise RuntimeError('EU4/FC4 opt-outs required for every session')
    return True


def protection_snapshot():
    return dict(content=fixture.content_snapshot(), config=fixture.config_snapshot(),
                descriptors=[dict(path=str(p),size=p.stat().st_size,sha256=sha(p))
                             for p in sorted(HOST.rglob('*.uplugin'))], builds=fixture.build_snapshot())


def verify_protection(before):
    after = protection_snapshot()
    for key in before:
        if before[key] != after[key]:
            raise RuntimeError('FB2 protected '+key+' changed; no later leg may start')
    return after


def prepare_files(leg, root):
    from runtime_scripts import steps, pie_steps, launcher, validate as validate_script
    root = canonical(root)
    (root/'script').mkdir(parents=True,exist_ok=False)
    (root/'session').mkdir()
    if leg in ('Smoke','Load','WorldsPIE'):
        generated = launcher(str(TOOLS/'runtime_editor_425.py'),leg,str(root/'driver.json'),
                             str(root/'session/probe/probe.jsonl'),str(root/'script/native.json'))
        (root/'script/launch.py').write_text(generated,encoding='utf-8')
        native = pie_steps()
    else:
        native = steps(leg)
    validate_script(native)
    (root/'script/native.json').write_text(json.dumps(native,indent=2)+'\n')
    return dict(leg=leg,argv=command(leg,root),script_files=[dict(path=str(p),sha256=sha(p))
        for p in sorted((root/'script').iterdir())])
