import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import time

w = Path('E:/IA_BuildCache/_r921/09205')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot').resolve()
plugin = host / 'Plugins/AnomalyInjector'
before = json.loads((w / 'preflight.json').read_text())
ps = r'C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe'
query = "@(Get-CimInstance Win32_Process | Where-Object { $_.Name -match '^UnrealEditor|^UE4Editor|^StackOBot.exe$' -and ($_.CommandLine -match '_r53_host|_r921[\\/]09205') } | Select-Object ProcessId,Name,CommandLine) | ConvertTo-Json -Depth 3 -Compress"
owned_live = subprocess.check_output([ps, '-NoProfile', '-Command', query], text=True, timeout=40).strip()
assert not owned_live or json.loads(owned_live) == []
assert not subprocess.check_output(['git', '-C', str(plugin), 'status', '--porcelain'], text=True).strip()
assert min(shutil.disk_usage(d+':\\').free for d in ('D', 'E')) > 50*1024**3


def sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


record = dict(started=time.time(), feature_head=subprocess.check_output(['git', '-C', str(plugin), 'rev-parse', 'HEAD'], text=True).strip(), binaries=[], settings=[], created_settings=[])
settings = {Path(r['path']).resolve():r for r in before['settings']}
for directory in (host / 'Config', host / 'Saved/Config'):
    for p in directory.rglob('*'):
        if p.is_file() and p.resolve() not in settings:
            assert p.resolve().is_relative_to(host)
            relative = str(p.resolve().relative_to(host)).replace('\\', '/')
            assert relative.startswith('Saved/Config/CrashReportClient/') and p.name == 'CrashReportClient.ini', relative
            assert p.stat().st_ctime >= before['time'] - 2
            record['created_settings'].append(dict(path=str(p.resolve()), sha256=sha(p), created=p.stat().st_ctime,
                                                    origin='Unreal-generated crash-client INI under the isolated host during this job'))
(w / 'restore-created-settings-receipt.json').write_text(json.dumps(record['created_settings'], indent=2))
subprocess.run(['git', '-C', str(plugin), 'checkout', '--detach', before['host_head']], check=True)
for item in before['binaries']:
    source, target = Path(item['archive']), Path(item['live']).resolve()
    assert target.is_relative_to(host) and sha(source) == item['sha256']
    shutil.copy2(source, target)
    assert sha(target) == item['sha256']
    record['binaries'].append(dict(path=str(target), sha256=sha(target)))
for target, item in settings.items():
    source = Path(item['backup'])
    assert target.is_relative_to(host) and sha(source) == item['sha256']
    target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, target)
    assert sha(target) == item['sha256']
    record['settings'].append(dict(path=str(target), sha256=sha(target)))
for item in record['created_settings']:
    target = Path(item['path']).resolve()
    assert target.is_relative_to(host) and sha(target) == item['sha256']
    assert target not in settings
    target.unlink()
    item['removed'] = not target.exists()
record.update(host_head=subprocess.check_output(['git', '-C', str(plugin), 'rev-parse', 'HEAD'], text=True).strip(), host_branch=subprocess.check_output(['git', '-C', str(plugin), 'rev-parse', '--abbrev-ref', 'HEAD'], text=True).strip(), host_status=subprocess.check_output(['git', '-C', str(plugin), 'status', '--porcelain'], text=True), finished=time.time())
assert record['host_head'] == before['host_head'] and record['host_branch'] == before['host_branch'] and not record['host_status'].strip()
(w / 'restoration.json').write_text(json.dumps(record, indent=2))
print(json.dumps(dict(host_head=record['host_head'], host_branch=record['host_branch'], binaries=len(record['binaries']), settings=len(record['settings']), removed_created_settings=len(record['created_settings'])), indent=2))
