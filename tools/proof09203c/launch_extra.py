import hashlib
import json
import shutil
import subprocess
import time
from pathlib import Path
from PIL import Image
import idle_gate
import runtime_guard

w = Path('E:/IA_BuildCache/_r921/09203c')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
assets = host / 'Content/CaptureBenchTexCorrupt/ProxyVT0921C'
gate = idle_gate.read()
if not gate['clear']:
    print(json.dumps(gate, indent=2))
    raise SystemExit('Extra fixture idle gate blocked')
assert min(shutil.disk_usage(d + ':\\').free for d in ('D', 'E')) - 1024**3 >= 50 * 1024**3
assert not (w / 'author-extra-launch.json').exists()
assert not (w / 'author-extra.json').exists()
for name in ('T_UniformVT', 'M_UniformVT', 'M_MixedProof'):
    assert not (assets / (name + '.uasset')).exists()
png = w / 'T_UniformVT.png'
assert not png.exists()
Image.new('RGB', (2048, 2048), (96, 96, 96)).save(png)
map_path = assets / 'CB_ProxyBlurVT.umap'
backup = w / 'fixture-extra-before' / map_path.name
backup.parent.mkdir()
shutil.copy2(map_path, backup)
sha = lambda p: hashlib.sha256(Path(p).read_bytes()).hexdigest()
assert sha(map_path) == sha(backup)
receipt = dict(map_path=str(map_path), backup=str(backup), before_sha256=sha(backup), png=str(png), png_sha256=sha(png))
(w / 'fixture-extra-before.json').write_text(json.dumps(receipt, indent=2))
args = ['D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe', str(host / 'StackOBot.uproject'),
        '-run=pythonscript', '-script=' + str(w / 'author_extra.py'), '-unattended', '-nosplash', '-NoLiveCoding',
        '-AllowCommandletRendering', '-abslog=' + str(w / 'author-extra-engine.log')]
record = dict(args=args, started=time.time(), gate=gate)
with (w / 'author-extra-stdout.log').open('w') as log:
    process = subprocess.Popen(args, stdout=log, stderr=subprocess.STDOUT, creationflags=subprocess.CREATE_NO_WINDOW)
    record['pid'] = process.pid
    (w / 'author-extra-launch.json').write_text(json.dumps(record, indent=2))
    record['runtime_guard'] = runtime_guard.wait_owned(process, 600)
    record.update(exit=record['runtime_guard']['exit'], finished=time.time())
    (w / 'author-extra-launch.json').write_text(json.dumps(record, indent=2))
after = {str(p): sha(p) for p in assets.rglob('*') if p.is_file()}
(w / 'fixture-extra-after.json').write_text(json.dumps(after, indent=2))
if record['exit'] or not (w / 'author-extra.json').exists():
    raise SystemExit('Extra author failed: ' + str(record['exit']))
report = json.loads((w / 'author-extra.json').read_text())
print(json.dumps(report, indent=2))
raise SystemExit(bool(report['errors']))
