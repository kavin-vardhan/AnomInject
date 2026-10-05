import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import time
import idle_gate
import runtime_guard

w = Path('E:/IA_BuildCache/_r921/09203c')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
assets = host / 'Content/CaptureBenchTexCorrupt/ProxyVT0921C'
gate = idle_gate.read()
if not gate['clear']:
    print(json.dumps(gate, indent=2))
    raise SystemExit('Named-stack author idle gate blocked')
assert min(shutil.disk_usage(d+':\\').free for d in ('D','E')) - 1024**3 >= 50*1024**3
assert not (w / 'author-named-stack-launch.json').exists()
assert not (w / 'author-named-stack.json').exists()
assert not (assets / 'M_VTNamedStack.uasset').exists()
sha = lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
before = {str(p):sha(p) for p in assets.iterdir() if p.is_file()}
args = ['D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe', str(host / 'StackOBot.uproject'), '-run=pythonscript', '-script=' + str(w / 'author_named_stack.py'), '-unattended', '-nosplash', '-NoLiveCoding', '-AllowCommandletRendering', '-abslog=' + str(w / 'author-named-stack-engine.log')]
record = dict(started=time.time(), gate=gate, args=args, before=before)
with (w / 'author-named-stack-stdout.log').open('w') as log:
    process = subprocess.Popen(args, stdout=log, stderr=subprocess.STDOUT, creationflags=subprocess.CREATE_NO_WINDOW)
    record['pid'] = process.pid
    (w / 'author-named-stack-launch.json').write_text(json.dumps(record, indent=2))
    record['runtime_guard'] = runtime_guard.wait_owned(process, 600)
    record.update(exit=record['runtime_guard']['exit'], finished=time.time())
    record['after'] = {str(p):sha(p) for p in assets.iterdir() if p.is_file()}
    (w / 'author-named-stack-launch.json').write_text(json.dumps(record, indent=2))
assert all(record['after'][path] == value for path,value in before.items())
if record['exit'] or not (w / 'author-named-stack.json').exists():
    raise SystemExit('Named-stack author failed: ' + str(record['exit']))
report = json.loads((w / 'author-named-stack.json').read_text())
print(json.dumps(report, indent=2))
raise SystemExit(bool(report['errors']))
