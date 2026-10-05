import hashlib
import json
import shutil
import subprocess
import time
from pathlib import Path

work = Path('E:/IA_BuildCache/_r921/09203c')
source = Path('E:/IA_BuildCache/_r921/AnomalyInjector')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
plugin = host / 'Plugins/AnomalyInjector'
archive = Path('E:/IA_BuildCache/_binary_baselines/release-2026-10-05-23098fa-218BA4C7')


def git(root, *args):
    return subprocess.check_output(['git', '--no-optional-locks', '-C', str(root), *args], text=True).strip()


def sha(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


disk = {d: shutil.disk_usage(d + ':\\').free for d in ('D', 'E')}
assert min(disk.values()) > 50 * 1024**3
assert git(source, 'rev-parse', 'HEAD') == '465f2e7510b0a4e077c8f36ec4f5595e5db65421'
assert git(source, 'status', '--porcelain') == ''
assert git(plugin, 'status', '--porcelain') == ''
assert git(plugin, 'rev-parse', 'HEAD') == '23098fa7415f148e935acbd4a97e18da04e6c46c'
manifest = json.loads((archive / 'manifest.json').read_text(encoding='utf-8-sig'))
binaries = []
for name, expected in manifest['files'].items():
    saved = archive / name
    live = plugin / 'Binaries/Win64' / saved.name if name.startswith('editor') else host / 'Binaries/Win64' / saved.name
    assert sha(saved) == sha(live) == expected
    binaries.append(dict(archive=str(saved), live=str(live), sha256=expected))
settings = []
for folder in (host / 'Config', host / 'Saved/Config'):
    for path in sorted(folder.rglob('*')):
        if path.is_file():
            backup = work / 'settings-before' / path.relative_to(host)
            backup.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(path, backup)
            assert sha(path) == sha(backup)
            settings.append(dict(path=str(path), backup=str(backup), sha256=sha(path)))
record = dict(time=time.time(), disk=disk, source_head=git(source, 'rev-parse', 'HEAD'),
    host_head=git(plugin, 'rev-parse', 'HEAD'), host_branch=git(plugin, 'rev-parse', '--abbrev-ref', 'HEAD'),
    protected_refs=git(source, 'for-each-ref', '--format=%(refname) %(objectname)', 'refs/heads/release/2026-10-05', 'refs/heads/master', 'refs/heads/m51', 'refs/tags').splitlines(),
    binaries=binaries, settings=settings)
with (work / 'preflight.json').open('x', encoding='utf-8') as stream:
    json.dump(record, stream, indent=2)
print(json.dumps(dict(binaries=len(binaries), settings=len(settings), disk=disk, host_head=record['host_head'])))
