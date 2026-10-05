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

work = Path('E:/IA_BuildCache/_r921/09203c')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
label = sys.argv[1] if len(sys.argv) > 1 else 'fixture-window-01'
no_request = len(sys.argv) > 2 and sys.argv[2] == 'no_request'
root = work / 'runs' / label
assert not root.exists()
gate = idle_gate.read()
if not gate['clear']:
    print(json.dumps(gate, indent=2))
    raise SystemExit('Identity refused: idle/disk gate not clear')
assert min(shutil.disk_usage(drive + ':\\').free for drive in ('D', 'E')) - 4*1024**3 >= 50*1024**3
fixture = json.loads((work / 'author_vt.json').read_text())
assert not fixture['errors'] and fixture['virtual_flag']
archive = proof_environment.checked_archive()
root.mkdir(parents=True)
out = root / 'out'
plan = dict(out_dir=str(out), settle_s=10, gap_ticks=2, done_timeout_s=240, post_s=5,
            identity_actions=str(root / 'actions.json'), cmds=[
    'r.DefaultFeature.AutoExposure 0', 'r.EyeAdaptationQuality 0', 'CaptureBench.Marker 0',
    'r.Editor.Viewport.OverridePIEScreenPercentage 0', 'r.ScreenPercentage 100',
    'r.SecondaryScreenPercentage.GameViewport 100', 'r.DynamicRes.OperationMode 0',
    'IAI.Capture.SVE 1', 'IAI.Capture.Delivery 0', 'IAI.Capture.Pace 1', 'IAI.Capture.Fps 30',
    'IAI.Capture.Mask 1', 'IAI.Capture.TargetMask 1', 'IAI.Capture.RunLog 1', 'IAI.Capture.FocusGate 0',
    'r.AntiAliasingMethod 0', 'r.Lumen.DiffuseIndirect.Allow 0', 'r.DynamicGlobalIlluminationMethod 0',
    'r.ReflectionMethod 0', 'IAI.Capture.Config 0 600 30 30 0', 'IAI.Auto.Pool all 0',
    'IAI.TexCorrupt.Census allreasons', 'WAIT 5',
    'IAI.Capture.Start %s png 777 %d oh=0' % (out, 720 if no_request else 480),
    'PYSTEP ' + str(work / 'identity_step.py')])
if no_request:
    plan.update(identity_probe_command='IAI.Bench.ProxyVT StaticMeshActor_77 Tex 0 no_request', identity_restore_at=660)
plan_path = root / 'plan.json'
plan_path.write_text(json.dumps(plan, indent=2), encoding='utf-8')
args = ['D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe', str(host / 'StackOBot.uproject'),
        fixture['map'], '-ExecCmds="py E:/IA_BuildCache/_r921/09203c/pie_window_driver.py"',
        '-IAIPiePlan=' + str(plan_path), '-IAIBench', '-ResX=1920', '-ResY=1080', '-windowed',
        '-nosplash', '-NoLiveCoding', '-unattended', '-abslog=' + str(root / 'game.log')]
command_line = ' '.join(arg if arg.startswith('-ExecCmds=') else subprocess.list2cmdline([arg]) for arg in args)
record = dict(gate=gate, args=args, command_line=command_line, started=time.time(), timeout_s=600, archive=archive)
with (root / 'stdout.log').open('w', encoding='utf-8') as output:
    process = subprocess.Popen(command_line, executable=args[0], stdout=output, stderr=subprocess.STDOUT)
    record['pid'] = process.pid
    (root / 'launch.json').write_text(json.dumps(record, indent=2), encoding='utf-8')
    guard = runtime_guard.wait_owned(process, 600)
    result = guard['exit']
    record['runtime_guard'] = guard
    record.update(exit=result, finished=time.time())
    (root / 'launch.json').write_text(json.dumps(record, indent=2), encoding='utf-8')
print(json.dumps(record, indent=2))
raise SystemExit(result)
