import hashlib
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path
import idle_gate
import runtime_guard

w = Path('E:/IA_BuildCache/_r921/09203c')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
assets = host / 'Content/CaptureBenchTexCorrupt/ProxyVT0921C'
label = sys.argv[1]
assert label.isalnum()
prefix = 'author-camera' + label
fixture_prefix = 'fixture-camera' + label
gate = idle_gate.read()
if not gate['clear']:
    print(json.dumps(gate, indent=2))
    raise SystemExit('Camera fixture idle gate blocked')
assert min(shutil.disk_usage(d + ':\\').free for d in ('D', 'E')) - 1024**3 >= 50 * 1024**3
assert not (w / (prefix + '-launch.json')).exists()
assert not (w / (prefix + '.json')).exists()
map_path = assets / 'CB_ProxyBlurVT.umap'
backup = w / (fixture_prefix + '-before') / map_path.name
backup.parent.mkdir()
shutil.copy2(map_path, backup)
sha = lambda p: hashlib.sha256(Path(p).read_bytes()).hexdigest()
assert sha(map_path) == sha(backup)
(w / (fixture_prefix + '-before.json')).write_text(json.dumps(dict(map_path=str(map_path), backup=str(backup), before_sha256=sha(backup)), indent=2))
args = ['D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe', str(host / 'StackOBot.uproject'), '-run=pythonscript', '-script=' + str(w / 'author_camera.py'), '-unattended', '-nosplash', '-NoLiveCoding', '-AllowCommandletRendering', '-abslog=' + str(w / (prefix + '-engine.log'))]
record = dict(args=args, started=time.time(), gate=gate)
with (w / (prefix + '-stdout.log')).open('w') as log:
    process = subprocess.Popen(args, stdout=log, stderr=subprocess.STDOUT, creationflags=subprocess.CREATE_NO_WINDOW, env=dict(os.environ, IAI_CAMERA_RESULT=str(w / (prefix + '.json'))))
    record['pid'] = process.pid
    (w / (prefix + '-launch.json')).write_text(json.dumps(record, indent=2))
    record['runtime_guard'] = runtime_guard.wait_owned(process, 600)
    record.update(exit=record['runtime_guard']['exit'], finished=time.time())
    (w / (prefix + '-launch.json')).write_text(json.dumps(record, indent=2))
(w / (fixture_prefix + '-after.json')).write_text(json.dumps({str(p): sha(p) for p in assets.rglob('*') if p.is_file()}, indent=2))
if record['exit'] or not (w / (prefix + '.json')).exists():
    raise SystemExit('Camera author failed: ' + str(record['exit']))
report = json.loads((w / (prefix + '.json')).read_text())
print(json.dumps(report, indent=2))
raise SystemExit(bool(report['errors']))
