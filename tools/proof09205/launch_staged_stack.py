import ctypes
import ctypes.wintypes
import hashlib
import json
import os
import shutil
import subprocess
import sys
import threading
import time
from pathlib import Path
import idle_gate
import runtime_guard
import proof_environment
import stage_upload_delay

w = Path('E:/IA_BuildCache/_r921/09205')
label, kind = sys.argv[1:3]
fires = int(sys.argv[3]) if len(sys.argv) > 3 else 5
root = w / 'runs' / label
assert not root.exists()
assert kind in ('targeted', 'auto', 'no_bind', 'baseline', 'accepted_baseline','early_delay','ready_delay')
gate = idle_gate.read()
if not gate['clear']:
    print(json.dumps(gate, indent=2))
    raise SystemExit('Staged idle gate blocked')
assert min(shutil.disk_usage(d + ':\\').free for d in ('D', 'E')) - 6 * 1024**3 >= 50 * 1024**3
archive = proof_environment.checked_archive()
cook = json.loads((w / 'current-cook.json').read_text())
assert archive['source_commit'] == cook['source_commit']
sha = lambda p: hashlib.sha256(Path(p).read_bytes()).hexdigest()
for path, expected in cook['hashes'].items():
    assert sha(path) == expected
exe = Path(cook['root']) / 'Binaries/Win64/StackOBot.exe'
root.mkdir(parents=True)
out = root / 'out'
commands = ['r.DefaultFeature.AutoExposure 0', 'r.EyeAdaptationQuality 0', 'CaptureBench.Marker 0',
            'r.ScreenPercentage 100', 'r.SecondaryScreenPercentage.GameViewport 100', 'r.DynamicRes.OperationMode 0',
            'IAI.Capture.SVE 1', 'IAI.Capture.Delivery 0', 'IAI.Capture.Pace 1', 'IAI.Capture.Fps 30',
            'IAI.Capture.Mask 1', 'IAI.Capture.TargetMask 1', 'IAI.Capture.RunLog 1', 'IAI.Capture.FocusGate 0',
            'r.AntiAliasingMethod 0', 'r.Lumen.DiffuseIndirect.Allow 0', 'r.DynamicGlobalIlluminationMethod 0',
            'r.ReflectionMethod 0', 'r.BloomQuality 0', 't.MaxFPS 60', 'IAI.Anomaly.StuckMipRoute auto', 'IAI.Auto.Pool all 0',
            'IAI.Capture.Config 0 121 48 96 %d' % fires,
            'DEFER ' * 300 + 'IAI.TexCorrupt.Census allreasons', 'DEFER ' * 850 + 'IAI.TexCorrupt.Census allreasons']
if kind == 'auto':
    commands += ['IAI.Auto.Pool stuck_low_mip 1']
elif kind == 'no_bind':
    commands += ['IAI.Bench.ProxyVTNoBind 1']
elif kind == 'early_delay':
    commands += ['IAI.Bench.ProxyVTEarlyBind 1']
command = 'IAI.Capture.Start %s png 777 %d' % (out, 120 + 264 * fires + 120)
if kind != 'auto':
    command += ' stuck_low_mip StaticMeshActor_77 -1'
commands += ['DEFER ' * 900 + command + ' oh=0']
if kind in ('baseline','accepted_baseline'):
    commands=[c for c in commands if 'Census allreasons' not in c]
if kind=='accepted_baseline':
    commands=['IAI.Capture.TexCorruptWarmDraw 0',*commands]
fixture = json.loads((w / 'author_stacks.json').read_text())
macro=root/'Binaries/stack.exec'
macro.parent.mkdir()
delay_macro=None
if kind in ('early_delay','ready_delay'):
    delay_macro=macro.parent/'upload-restore.exec'
    delay_macro.write_text('DEFER exec '+str(delay_macro)+'\n')
    commands=[*stage_upload_delay.names,*commands]
    commands += ['DEFER '*895+name+' 0' for name in stage_upload_delay.names]
    commands += ['DEFER '*900+'exec '+str(delay_macro)]
macro.write_text('\n'.join(commands)+'\n')
args = [str(exe), fixture['map'], '-windowed', '-ResX=1920', '-ResY=1080', '-ForceRes', '-IAIBench',
        '-ExecCmds="exec ' + str(macro) + '"', '-unattended', '-nosplash', '-abslog=' + str(root / 'game.log')]
command_line = ' '.join(arg if arg.startswith('-ExecCmds=') else subprocess.list2cmdline([arg]) for arg in args)
record = dict(kind=kind, fires_requested=fires, gate=gate, archive=archive, cook=cook, args=args, command_line=command_line, started=time.time())
backup = root / 'current-exe-before-baseline.exe'
if kind in ('baseline','accepted_baseline'):
    baseline = Path('E:/IA_BuildCache/_binary_baselines/release-2026-10-05-23098fa-218BA4C7/StackOBot.exe')
    assert sha(baseline) == '218ba4c79bb2bf6d9e14706772c04b3c53027cc05b84d4ab100a3349b650b7d6'
    if kind=='accepted_baseline':
        baseline=Path('E:/IA_BuildCache/_r921/09203c/archives/probe-c80ef9b-1EA292E8/StackOBot.exe')
        assert sha(baseline).startswith('1ea292e8')
    shutil.copy2(exe, backup)
    assert sha(backup) == archive['files']['StackOBot.exe']
    shutil.copy2(baseline, exe)
record['exe_sha256'] = sha(exe)
record['macro']=str(macro)
record['macro_sha256']=sha(macro)
assert len(command_line)<16300
env=os.environ.copy()
env.update(TEMP=str(w/'temp'),TMP=str(w/'temp'))


def close_after_capture(process):
    while process.poll() is None:
        if list(out.glob('session_*/run_summary.json')):
            time.sleep(1)
            if process.poll() is not None:
                return
            sent = []
            callback_type = ctypes.WINFUNCTYPE(ctypes.wintypes.BOOL, ctypes.wintypes.HWND, ctypes.wintypes.LPARAM)
            def callback(hwnd, ignored):
                owner = ctypes.wintypes.DWORD()
                ctypes.windll.user32.GetWindowThreadProcessId(hwnd, ctypes.byref(owner))
                if owner.value == process.pid and ctypes.windll.user32.IsWindowVisible(hwnd):
                    ctypes.windll.user32.PostMessageW(hwnd, 0x0010, 0, 0)
                    sent.append(int(hwnd))
                return True
            ctypes.windll.user32.EnumWindows(callback_type(callback), 0)
            (root / 'close-request.json').write_text(json.dumps(dict(pid=process.pid, window_handles=sent, time=time.time()), indent=2))
            return
        time.sleep(2)


try:
    with (root / 'stdout.log').open('w') as log:
        process = subprocess.Popen(command_line, executable=str(exe), stdout=log, stderr=subprocess.STDOUT, cwd=exe.parent,env=env,creationflags=subprocess.CREATE_NO_WINDOW)
        record['pid'] = process.pid
        (root / 'launch.json').write_text(json.dumps(record, indent=2))
        threading.Thread(target=close_after_capture, args=(process,), daemon=True).start()
        if delay_macro:
            threading.Thread(target=stage_upload_delay.monitor,args=(process,root,out,delay_macro),daemon=True).start()
        record['runtime_guard'] = runtime_guard.wait_owned(process, 1200)
        record.update(exit=record['runtime_guard']['exit'], finished=time.time())
        (root / 'launch.json').write_text(json.dumps(record, indent=2))
finally:
    if kind in ('baseline','accepted_baseline'):
        assert sha(backup) == archive['files']['StackOBot.exe']
        shutil.copy2(backup, exe)
        assert sha(exe) == archive['files']['StackOBot.exe']
        (root / 'baseline-exe-restored.json').write_text(json.dumps(dict(path=str(exe), sha256=sha(exe)), indent=2))
print(json.dumps({k:record[k] for k in ('kind', 'pid', 'exit', 'exe_sha256', 'started', 'finished')}, indent=2))
raise SystemExit(record['exit'])
