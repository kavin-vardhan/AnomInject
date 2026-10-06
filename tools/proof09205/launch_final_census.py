import json
import shutil
import subprocess
import sys
import time
from pathlib import Path
import idle_gate
import runtime_guard
import proof_environment

w = Path('E:/IA_BuildCache/_r921/09205')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
label, fixture, scope = sys.argv[1:4]
mutant = 'mutant' in sys.argv[4:]
root = w / 'runs' / label
assert not root.exists()
gate = idle_gate.read()
if not gate['clear']:
    print(json.dumps(gate))
    raise SystemExit('engine gate blocked')
assert min(shutil.disk_usage(d + ':\\').free for d in ('D','E')) - 3*1024**3 > 50*1024**3
archive = proof_environment.checked_archive()
root.mkdir(parents=True)
out = root / 'out'
map_path = '/Game/CaptureBenchTexCorrupt/ProxyVT0921C/CB_ProxyBlurVT' if fixture == 'single' else '/Game/CaptureBenchTexCorrupt/ProxyVT09205/CB_ProxyBlurStacks'
commands = ['r.DefaultFeature.AutoExposure 0', 'r.EyeAdaptationQuality 0', 'CaptureBench.Marker 0',
    'r.Editor.Viewport.OverridePIEScreenPercentage 0', 'r.ScreenPercentage 100', 'r.SecondaryScreenPercentage.GameViewport 100',
    'r.DynamicRes.OperationMode 0', 'r.AntiAliasingMethod 0', 'r.Lumen.DiffuseIndirect.Allow 0', 'r.DynamicGlobalIlluminationMethod 0',
    'r.ReflectionMethod 0', 'r.BloomQuality 0', 'IAI.Auto.Pool all 0', 'IAI.Anomaly.StuckMipRoute auto', 'WAIT 10']
census = 'IAI.TexCorrupt.Census ' + ('allreasons ' if 'reasons' in sys.argv[4:] else 'all ' if scope == 'all' else '') + 'vt'
plan = dict(settle_s=15, gap_ticks=2, viewport=[1920,1080], census_command=census, census_actions=str(root/'actions.json'))
if mutant:
    commands += ['IAI.Bench.VTCensusMiscount 1', census, 'WAIT 5', 'IAI.Bench.VTCensusMiscount 0']
    plan.update(hold_s=3)
else:
    commands += ['IAI.Capture.SVE 1', 'IAI.Capture.Delivery 0', 'IAI.Capture.Pace 1', 'IAI.Capture.Fps 30', 'IAI.Capture.Mask 1',
        'IAI.Capture.TargetMask 1', 'IAI.Capture.RunLog 1', 'IAI.Capture.FocusGate 0',
        'IAI.Capture.Start %s png 777 480 oh=0' % out, 'PYSTEP ' + str(w/'census_step.py')]
    plan.update(out_dir=str(out), done_timeout_s=600, post_s=1)
plan['cmds'] = commands
(root/'plan.json').write_text(json.dumps(plan, indent=2))
args = ['D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe', str(host/'StackOBot.uproject'), map_path,
    '-ExecCmds="py E:/IA_BuildCache/_r921/09205/pie_window_driver.py"', '-IAIPiePlan='+str(root/'plan.json'), '-IAIBench',
    '-ResX=1920', '-ResY=1080', '-windowed', '-nosplash', '-NoLiveCoding', '-unattended', '-abslog='+str(root/'game.log')]
command = ' '.join(arg if arg.startswith('-ExecCmds=') else subprocess.list2cmdline([arg]) for arg in args)
if 'debug' in sys.argv[4:]:
    args.append('-d3ddebug')
    command += ' -d3ddebug'
receipt = dict(args=args, command=command, gate=gate, archive=archive, fixture=fixture, scope=scope, mutant=mutant, expected=str(w/'census-expected-part-b-before.json'), started=time.time())
with (root/'stdout.log').open('w') as log:
    process = subprocess.Popen(command, executable=args[0], stdout=log, stderr=subprocess.STDOUT, creationflags=subprocess.CREATE_NO_WINDOW)
    receipt['pid'] = process.pid
    (root/'launch.json').write_text(json.dumps(receipt, indent=2))
    receipt['guard'] = runtime_guard.wait_owned(process,900)
receipt.update(exit=receipt['guard']['exit'], finished=time.time())
(root/'launch.json').write_text(json.dumps(receipt, indent=2))
print(json.dumps(dict(label=label,exit=receipt['exit'],seconds=receipt['finished']-receipt['started'])))
raise SystemExit(receipt['exit'])
