import hashlib
import json
import subprocess
import time
from pathlib import Path
import idle_gate
import runtime_guard

work = Path('E:/IA_BuildCache/_r921')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
gate = idle_gate.read()
(work / 'author-gate.json').write_text(json.dumps(gate, indent=2), encoding='utf-8')
if not gate['clear']:
    raise SystemExit('Author refused: idle/disk gate is not clear')
assert not (work / 'author_vt.json').exists()
assert not (host / 'Content/CaptureBenchTexCorrupt/ProxyVT0921').exists()
args = ['D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe', str(host / 'StackOBot.uproject'), '-run=pythonscript', '-script=' + str(work / 'author_vt.py'), '-unattended', '-nosplash', '-NoLiveCoding', '-AllowCommandletRendering', '-abslog=' + str(work / 'author-engine.log')]
with (work / 'author-stdout.txt').open('w', encoding='utf-8') as output:
    process = subprocess.Popen(args, stdout=output, stderr=subprocess.STDOUT, creationflags=subprocess.CREATE_NO_WINDOW)
    receipt = dict(pid=process.pid, args=args, started=time.time(), gate=gate)
    (work / 'author-launch.json').write_text(json.dumps(receipt, indent=2), encoding='utf-8')
    guard = runtime_guard.wait_owned(process, 600)
    result = guard['exit']
    receipt['runtime_guard'] = guard
    receipt.update(exit=result, finished=time.time())
    (work / 'author-launch.json').write_text(json.dumps(receipt, indent=2), encoding='utf-8')
if result or not (work / 'author_vt.json').exists():
    raise SystemExit('Author process failed: ' + str(result))
report = json.loads((work / 'author_vt.json').read_text())
created = {}
for path in (host / 'Content/CaptureBenchTexCorrupt/ProxyVT0921').rglob('*'):
    if path.is_file():
        with path.open('rb') as stream:
            created[str(path.resolve())] = hashlib.file_digest(stream, 'sha256').hexdigest()
(work / 'fixture-created.json').write_text(json.dumps(created, indent=2), encoding='utf-8')
print(json.dumps(report, indent=2))
if report['errors']:
    raise SystemExit(1)
