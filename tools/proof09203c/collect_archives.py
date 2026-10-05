import hashlib
import json
from pathlib import Path
import sys
import time

w = Path('E:/IA_BuildCache/_r921/09203c')
output = w / (sys.argv[1] + '.json')
assert not output.exists()
records = []
for manifest_path in sorted((w / 'archives').glob('*/manifest.json')):
    manifest = json.loads(manifest_path.read_text())
    archive = manifest_path.parent
    values = {}
    for name, expected in manifest['files'].items():
        with (archive/name).open('rb') as stream:
            actual = hashlib.file_digest(stream, 'sha256').hexdigest()
        assert actual == expected, (name, actual, expected)
        values[name] = actual
    records.append(dict(archive=str(archive), source_commit=manifest['source_commit'], files=values))
assert records
output.write_text(json.dumps(dict(verified=time.time(), archives=records), indent=2))
print(json.dumps(dict(archives=len(records), binaries=sum(len(record['files']) for record in records))))
