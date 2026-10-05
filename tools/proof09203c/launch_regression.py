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
root = w / 'runs' / label
assert not root.exists()
gate = idle_gate.read()
if not gate['clear']:
    print(json.dumps(gate, indent=2))
    raise SystemExit('Regression idle gate blocked')
assert min(shutil.disk_usage(d + ':\\').free for d in ('D', 'E')) - 2 * 1024**3 >= 50 * 1024**3
archive = proof_environment.checked_archive()
fixtures = {
    'hold': ('/Game/CaptureBenchTexCorrupt/Hold0913/CB_HoldDetail', 'stuck_low_mip', 'StaticMeshActor_77', '-1'),
    'proxy': ('/Game/CaptureBenchTexCorrupt/Proxy0912/Detail/CB_ProxyDetail', 'stuck_low_mip', 'StaticMeshActor_77', '-1'),
    'uv': ('/Game/CaptureBenchTexCorrupt/CB_TexCorruptLevel', 'uv_corruption', 'StaticMeshActor_0', 'tile'),
    'normal': ('/Game/CaptureBenchTexCorrupt/CB_TexCorruptLevel', 'normal_corruption', 'StaticMeshActor_12', 'invert')}
map_path, anomaly, target, extra = fixtures[kind]
profile = host / 'Config/DefaultDeviceProfiles.ini'
assert not profile.exists()
root.mkdir(parents=True)
body = '\n'.join('[%s DeviceProfile]\nDeviceType=Windows\nBaseProfileName=%s\n+TextureLODGroups=(Group=TEXTUREGROUP_Project02,MinLODSize=1,MaxLODSize=16384,LODBias=1,MinMagFilter=aniso,MipFilter=point,MipGenSettings=TMGS_SimpleAverage)\n' % (p, '' if p == 'Windows' else 'Windows') for p in ('Windows', 'WindowsEditor'))
profile.write_text(body)
fingerprint = hashlib.sha256(profile.read_bytes()).hexdigest()
(root / 'profile-created.json').write_text(json.dumps(dict(path=str(profile), sha256=fingerprint, body=body, prior_exists=False), indent=2))
out = root / 'out'
commands = ['r.DefaultFeature.AutoExposure 0', 'r.EyeAdaptationQuality 0', 'CaptureBench.Marker 0',
            'r.Editor.Viewport.OverridePIEScreenPercentage 0', 'r.ScreenPercentage 100',
            'r.SecondaryScreenPercentage.GameViewport 100', 'r.DynamicRes.OperationMode 0',
            'IAI.Capture.SVE 1', 'IAI.Capture.Delivery 0', 'IAI.Capture.Pace 1', 'IAI.Capture.Fps 30',
            'IAI.Capture.Mask 1', 'IAI.Capture.TargetMask 1', 'IAI.Capture.RunLog 1', 'IAI.Capture.FocusGate 0',
            'r.AntiAliasingMethod 0', 'r.Lumen.DiffuseIndirect.Allow 0', 'r.DynamicGlobalIlluminationMethod 0',
            'r.ReflectionMethod 0', 'IAI.Anomaly.StuckMipRoute ' + ('hold' if kind == 'hold' else 'auto'),
            'IAI.Auto.Pool all 0', 'IAI.TexCorrupt.Census allreasons', 'WAIT 20', 'IAI.Capture.Config 0 192 48 192 3',
            'IAI.Capture.Start %s png 777 3000 %s %s %s oh=0' % (out, anomaly, target, extra)]
plan = dict(out_dir=str(out), settle_s=30, gap_ticks=2, done_timeout_s=600, post_s=5, cmds=commands, viewport=[942, 869])
plan_path = root / 'plan.json'
plan_path.write_text(json.dumps(plan, indent=2))
args = ['D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe', str(host / 'StackOBot.uproject'), map_path,
        '-ExecCmds="py E:/IA_BuildCache/_r921/09203c/pie_window_driver.py"', '-IAIPiePlan=' + str(plan_path), '-IAIBench',
        '-nosplash', '-NoLiveCoding', '-unattended', '-abslog=' + str(root / 'game.log')]
command_line = ' '.join(arg if arg.startswith('-ExecCmds=') else subprocess.list2cmdline([arg]) for arg in args)
record = dict(kind=kind, gate=gate, archive=archive, args=args, command_line=command_line, started=time.time())
try:
    with (root / 'stdout.log').open('w') as log:
        process = subprocess.Popen(command_line, executable=args[0], stdout=log, stderr=subprocess.STDOUT)
        record['pid'] = process.pid
        (root / 'launch.json').write_text(json.dumps(record, indent=2))
        record['runtime_guard'] = runtime_guard.wait_owned(process, 900)
        record.update(exit=record['runtime_guard']['exit'], finished=time.time())
        (root / 'launch.json').write_text(json.dumps(record, indent=2))
finally:
    assert profile.resolve().is_relative_to(host.resolve()) and hashlib.sha256(profile.read_bytes()).hexdigest() == fingerprint
    profile.unlink()
    (root / 'profile-restored.json').write_text(json.dumps(dict(path=str(profile), exists=profile.exists()), indent=2))
print(json.dumps({k:record[k] for k in ('kind', 'pid', 'exit', 'started', 'finished')}, indent=2))
raise SystemExit(record['exit'])
