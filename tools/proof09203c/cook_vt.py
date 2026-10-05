import hashlib
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path
import idle_gate
import cook_guard

w = Path('E:/IA_BuildCache/_r921/09203c')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
label = sys.argv[1]
root = w / 'cooks' / label
assert not root.exists()
gate = idle_gate.read()
if not gate['clear']:
    print(json.dumps(gate, indent=2))
    raise SystemExit('Cook idle gate blocked')
assert shutil.disk_usage('E:\\').free - 20 * 1024**3 >= 50 * 1024**3
assert shutil.disk_usage('D:\\').free - 2 * 1024**3 >= 50 * 1024**3
fixture = json.loads((w / 'author_vt.json').read_text())
assert not fixture['errors']
archive = json.loads((w / 'probe-archive.json').read_text())
head = subprocess.check_output(['git', '-C', str(host / 'Plugins/AnomalyInjector'), 'rev-parse', 'HEAD'], text=True).strip()
assert head == archive['source_commit']
assert not subprocess.check_output(['git', '-C', str(host / 'Plugins/AnomalyInjector'), 'status', '--porcelain'], text=True).strip()
root.mkdir(parents=True)
scratch = [root / n for n in ('archive', 'work', 'stage', 'logs', 'final-logs', 'engine-saved')]
for p in scratch:
    p.mkdir()
(root / 'created.json').write_text(json.dumps([str(p) for p in scratch], indent=2))
maps = [fixture['map'], '/Game/CaptureBenchTexCorrupt/CB_TexCorruptLevel', '/Game/StackOBot/UI/MainMenu/MainMenu']
args = ['D:/UESource/UnrealEngine/Engine/Binaries/ThirdParty/DotNet/6.0.302/windows/dotnet.exe',
        'D:/UESource/UnrealEngine/Engine/Binaries/DotNET/AutomationTool/AutomationTool.dll', '-nocompile', 'BuildCookRun',
        '-project=' + str(host / 'StackOBot.uproject'), '-platform=Win64', '-clientconfig=Development', '-cook', '-stage', '-pak', '-archive',
        '-archivedirectory=' + str(root / 'archive'), '-CookOutputDir=' + str(root / 'work/Windows'), '-stagingdirectory=' + str(root / 'stage'),
        '-utf8output', '-nocompileeditor', '-nodebuginfo', '-map=' + '+'.join(maps)]
env = os.environ.copy()
env.update(TEMP=str(w / 'temp'), TMP=str(w / 'temp'), uebp_LogFolder=str(root / 'logs'), uebp_FinalLogFolder=str(root / 'final-logs'), uebp_EngineSavedFolder=str(root / 'engine-saved'))
record = dict(args=args, started=time.time(), gate=gate, archive=archive)
with (root / 'driver.log').open('w', encoding='utf-8') as log:
    process = subprocess.Popen(args, stdout=log, stderr=subprocess.STDOUT, creationflags=subprocess.CREATE_NO_WINDOW, env=env)
    record['pid'] = process.pid
    (root / 'launch.json').write_text(json.dumps(record, indent=2))
    guard = cook_guard.wait_owned_tree(process, 7200, root / 'runtime-guard.json')
    record.update(exit=guard['exit'], guard_reason=guard['reason'], finished=time.time())
    (root / 'launch.json').write_text(json.dumps(record, indent=2))
if record['exit'] or record['guard_reason']:
    raise SystemExit('Cook failed: ' + json.dumps(record))
distribution = root / 'archive/Windows/StackOBot'
files = [distribution / 'Binaries/Win64/StackOBot.exe'] + list((distribution / 'Content/Paks').glob('*'))
assert len(files) >= 4 and all(p.is_file() for p in files)
hashes = {}
for p in files:
    with p.open('rb') as stream:
        hashes[str(p)] = hashlib.file_digest(stream, 'sha256').hexdigest()
assert hashes[str(files[0])] == archive['files']['StackOBot.exe']
result = dict(status='PASS', root=str(distribution), source_commit=head, hashes=hashes, seconds=record['finished'] - record['started'])
(root / 'result.json').write_text(json.dumps(result, indent=2))
(w / 'current-cook.json').write_text(json.dumps(result, indent=2))
print(json.dumps(result, indent=2))
