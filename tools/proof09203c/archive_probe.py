import hashlib
import json
from pathlib import Path
import shutil
import subprocess

work = Path('E:/IA_BuildCache/_r921/09203c')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
plugin = host / 'Plugins/AnomalyInjector'
commit = subprocess.check_output(['git', '-C', str(plugin), 'rev-parse', 'HEAD'], text=True).strip()
paths = [host / 'Binaries/Win64/StackOBot.exe'] + sorted((plugin / 'Binaries/Win64').glob('UnrealEditor-Anomaly*.dll'))
assert len(paths) == 6
assert shutil.disk_usage('E:\\').free - sum(path.stat().st_size for path in paths) > 50*1024**3


def sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


stamp = sha(paths[0])[:8].upper()
archive = work / 'archives' / ('probe-' + commit[:7] + '-' + stamp)
(archive / 'editor').mkdir(parents=True, exist_ok=False)
files = {}
for source in paths:
    target = archive / ('editor' if source.suffix == '.dll' else '') / source.name
    shutil.copyfile(source, target)
    digest = sha(source)
    assert sha(target) == digest
    files[str(target.relative_to(archive)).replace('\\', '/')] = digest
record = dict(source_commit=commit, archive=str(archive), files=files, status='built and re-hashed; no runtime proof status implied', built_data='fixture content and cooked data are receipted separately for each leg')
(archive / 'manifest.json').write_text(json.dumps(record, indent=2), encoding='utf-8')
(work / 'probe-archive.json').write_text(json.dumps(record, indent=2), encoding='utf-8')
print(json.dumps(record, indent=2))
