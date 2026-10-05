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

w=Path('E:/IA_BuildCache/_r921/09205')
label,fixture,scope=sys.argv[1:4]
root=w/'runs'/label
assert not root.exists()
gate=idle_gate.read()
assert gate['clear'],gate
assert min(shutil.disk_usage(d+':\\').free for d in ('D','E'))-3*1024**3>50*1024**3
archive=proof_environment.checked_archive()
cook=json.loads((w/'current-cook.json').read_text())
assert archive['source_commit']==cook['source_commit']
for path,digest in cook['hashes'].items():
    with Path(path).open('rb') as stream: assert hashlib.file_digest(stream,'sha256').hexdigest()==digest
exe=Path(cook['root'])/'Binaries/Win64/StackOBot.exe'
root.mkdir(parents=True)
out=root/'out'
commands=['r.DefaultFeature.AutoExposure 0','r.EyeAdaptationQuality 0','CaptureBench.Marker 0','r.ScreenPercentage 100',
    'r.SecondaryScreenPercentage.GameViewport 100','r.DynamicRes.OperationMode 0','r.AntiAliasingMethod 0',
    'r.Lumen.DiffuseIndirect.Allow 0','r.DynamicGlobalIlluminationMethod 0','r.ReflectionMethod 0','t.MaxFPS 60','Log LogAnomalyCapture Warning',
    'IAI.Auto.Pool all 0','IAI.Anomaly.StuckMipRoute auto','IAI.Capture.SVE 1','IAI.Capture.Delivery 0',
    'IAI.Capture.Pace 1','IAI.Capture.Fps 30','IAI.Capture.Mask 1','IAI.Capture.TargetMask 1','IAI.Capture.RunLog 1','IAI.Capture.FocusGate 0',
    'DEFER '*780+'IAI.Capture.Start %s png 777 480 oh=0'%out,
    'DEFER '*1120+'IAI.Bench.VTCensusState',
    'DEFER '*1124+'IAI.TexCorrupt.Census '+('all ' if scope=='all' else '')+'vt',
    'DEFER '*1210+'IAI.Bench.VTCensusState']
map_path='/Game/CaptureBenchTexCorrupt/ProxyVT0921C/CB_ProxyBlurVT' if fixture=='single' else '/Game/CaptureBenchTexCorrupt/ProxyVT09205/CB_ProxyBlurStacks'
macro=root/'Binaries/census.exec'
macro.parent.mkdir()
macro.write_text('\n'.join(commands)+'\n')
args=[str(exe),map_path,'-windowed','-ResX=1920','-ResY=1080','-ForceRes','-IAIBench','-ExecCmds="exec '+str(macro)+'"','-unattended','-nosplash','-abslog='+str(root/'game.log')]
command=' '.join(arg if arg.startswith('-ExecCmds=') else subprocess.list2cmdline([arg]) for arg in args)
assert len(command)<16300,len(command)
record=dict(args=args,command=command,macro=str(macro),macro_sha256=hashlib.sha256(macro.read_bytes()).hexdigest(),gate=gate,archive=archive,cook=cook,fixture=fixture,scope=scope,mutant=False,staged=True,expected=str(w/'census-expected-v3-before.json'),started=time.time())

def close_after_capture(process):
    while process.poll() is None:
        if list(out.glob('session_*/run_summary.json')):
            time.sleep(1)
            sent=[]
            callback_type=ctypes.WINFUNCTYPE(ctypes.wintypes.BOOL,ctypes.wintypes.HWND,ctypes.wintypes.LPARAM)
            def callback(hwnd,ignored):
                owner=ctypes.wintypes.DWORD()
                ctypes.windll.user32.GetWindowThreadProcessId(hwnd,ctypes.byref(owner))
                if owner.value==process.pid and ctypes.windll.user32.IsWindowVisible(hwnd):
                    ctypes.windll.user32.PostMessageW(hwnd,0x0010,0,0)
                    sent.append(int(hwnd))
                return True
            ctypes.windll.user32.EnumWindows(callback_type(callback),0)
            (root/'close-request.json').write_text(json.dumps(dict(pid=process.pid,handles=sent,time=time.time()),indent=2))
            return
        time.sleep(2)

env=os.environ.copy()
env.update(TEMP=str(w/'temp'),TMP=str(w/'temp'))
with (root/'stdout.log').open('w') as log:
    process=subprocess.Popen(command,executable=str(exe),stdout=log,stderr=subprocess.STDOUT,cwd=exe.parent,env=env,creationflags=subprocess.CREATE_NO_WINDOW)
    record['pid']=process.pid
    (root/'launch.json').write_text(json.dumps(record,indent=2))
    threading.Thread(target=close_after_capture,args=(process,),daemon=True).start()
    record['guard']=runtime_guard.wait_owned(process,900)
record.update(exit=record['guard']['exit'],finished=time.time())
(root/'launch.json').write_text(json.dumps(record,indent=2))
print(json.dumps(dict(label=label,exit=record['exit'],seconds=record['finished']-record['started'])))
raise SystemExit(record['exit'])
