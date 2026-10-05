import hashlib
import json
import shutil
import subprocess
import sys
import time
from pathlib import Path
import idle_gate
import runtime_guard
import proof_environment

w = Path('E:/IA_BuildCache/_r921/09203c')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
label, kind = sys.argv[1:3]
fires = int(sys.argv[3]) if len(sys.argv) > 3 else 5
root = w / 'runs' / label
assert not root.exists()
gate = idle_gate.read()
if not gate['clear']:
    print(json.dumps(gate, indent=2))
    raise SystemExit('Idle gate blocked')
assert min(shutil.disk_usage(d + ':\\').free for d in ('D', 'E')) - 6 * 1024**3 >= 50 * 1024**3
fixture = json.loads((w / 'author_vt.json').read_text())
archive = proof_environment.checked_archive()
root.mkdir(parents=True)
out = root / 'out'
commands = [
    'r.DefaultFeature.AutoExposure 0', 'r.EyeAdaptationQuality 0', 'CaptureBench.Marker 0',
    'r.Editor.Viewport.OverridePIEScreenPercentage 0', 'r.ScreenPercentage 100',
    'r.SecondaryScreenPercentage.GameViewport 100', 'r.DynamicRes.OperationMode 0',
    'IAI.Capture.SVE 1', 'IAI.Capture.Delivery 0', 'IAI.Capture.Pace 1', 'IAI.Capture.Fps 30',
    'IAI.Capture.Mask 1', 'IAI.Capture.TargetMask 1', 'IAI.Capture.RunLog 1', 'IAI.Capture.FocusGate 0',
    'r.AntiAliasingMethod 0', 'r.Lumen.DiffuseIndirect.Allow 0', 'r.DynamicGlobalIlluminationMethod 0',
    'r.ReflectionMethod 0', 'IAI.Anomaly.StuckMipRoute auto', 'IAI.Auto.Pool all 0']
frames = 120 + 264 * fires + 120
target = 'StaticMeshActor_77'
anomaly = 'stuck_low_mip'
extra_args = ' -1'
if kind == 'auto':
    commands += ['IAI.Auto.Pool stuck_low_mip 1']
elif kind == 'no_bind':
    commands += ['IAI.Bench.ProxyVTNoBind 1']
elif kind not in ('targeted', 'destroy', 'displace', 'world_end', 'negative', 'uniform', 'mixed'):
    raise ValueError(kind)
setup = {'negative': 'M_VTConstantLayer', 'uniform': 'M_UniformVT', 'mixed': 'M_MixedProof'}
if kind in setup:
    commands += ['PYSTEP ' + str(w / 'fixture_setup_step.py'), 'WAIT 5']
commands += ['IAI.TexCorrupt.Census allreasons', 'WAIT 5', 'IAI.TexCorrupt.Census allreasons',
             'IAI.Capture.Config 0 121 48 96 %d' % fires]
command = 'IAI.Capture.Start %s png 777 %d' % (out, frames)
if kind != 'auto':
    command += ' %s %s%s' % (anomaly, target, extra_args)
commands += [command + ' oh=0']
if kind in ('destroy', 'displace', 'world_end'):
    commands += ['PYSTEP ' + str(w / 'lifetime_step.py')]
plan = dict(out_dir=str(out), settle_s=10, gap_ticks=2, done_timeout_s=600, post_s=10, cmds=commands,
            post_commands=['IAI.Bench.ProxyVTInventory'])
if kind in setup:
    plan.update(fixture_material=setup[kind], fixture_setup_result=str(root / 'fixture-setup.json'))
if kind in ('destroy', 'displace', 'world_end'):
    plan.update(lifetime_action=kind, lifetime_result=str(root / 'lifetime-actions.json'))
plan_path = root / 'plan.json'
plan_path.write_text(json.dumps(plan, indent=2))
args = ['D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe', str(host / 'StackOBot.uproject'),
        fixture['map'], '-ExecCmds="py E:/IA_BuildCache/_r921/09203c/pie_window_driver.py"',
        '-IAIPiePlan=' + str(plan_path), '-IAIBench', '-ResX=1920', '-ResY=1080', '-windowed',
        '-nosplash', '-NoLiveCoding', '-unattended', '-abslog=' + str(root / 'game.log')]
command_line = ' '.join(arg if arg.startswith('-ExecCmds=') else subprocess.list2cmdline([arg]) for arg in args)
record = dict(kind=kind, fires_requested=fires, gate=gate, args=args, command_line=command_line,
              archive=archive, started=time.time())
with (root / 'stdout.log').open('w', encoding='utf-8') as log:
    process = subprocess.Popen(command_line, executable=args[0], stdout=log, stderr=subprocess.STDOUT)
    record['pid'] = process.pid
    (root / 'launch.json').write_text(json.dumps(record, indent=2))
    record['runtime_guard'] = runtime_guard.wait_owned(process, 900)
    record['exit'] = record['runtime_guard']['exit']
    record['finished'] = time.time()
    (root / 'launch.json').write_text(json.dumps(record, indent=2))
print(json.dumps(record, indent=2))
raise SystemExit(record['exit'])
