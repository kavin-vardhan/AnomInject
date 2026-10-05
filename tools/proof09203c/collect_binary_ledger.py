import hashlib
import json
from pathlib import Path
import time

w = Path('E:/IA_BuildCache/_r921/09203c')
old = json.loads((w / 'binary-hash-ledger-final.json').read_text())
archives = {str(Path(row['archive']).resolve()): row for row in old['archives']}
for manifest_path in (w / 'archives').glob('*/manifest.json'):
    row = json.loads(manifest_path.read_text())
    archives[str(manifest_path.parent.resolve())] = dict(archive=str(manifest_path.parent.resolve()),
                                                       source_commit=row['source_commit'], files=row['files'])
groups = {}
for row in archives.values():
    for name, expected in row['files'].items():
        with (Path(row['archive']) / name).open('rb') as stream:
            assert hashlib.file_digest(stream, 'sha256').hexdigest() == expected
    key = tuple(sorted(row['files'].items()))
    groups.setdefault(key, []).append(row)
record = dict(verified=time.time(), archives=list(archives.values()), unique_binary_sets=len(groups),
              binary_count=sum(len(row['files']) for row in archives.values()))
(w / 'binary-hash-ledger-final.json').write_text(json.dumps(record, indent=2))
lines = ['All hashes are SHA-256. Each set lists the executable and all five Editor DLLs.', '']
for index, (files, rows) in enumerate(groups.items(), 1):
    lines.extend(['Set B%d:' % index, ''])
    lines.extend('- `%s` (source `%s`)' % (row['archive'], row['source_commit']) for row in rows)
    lines.extend(['', '| Binary | SHA-256 |', '|---|---|'])
    lines.extend('| %s | `%s` |' % (name, digest) for name, digest in files)
    lines.append('')
(w / 'binary-hash-ledger-final.md').write_text('\n'.join(lines))
print(json.dumps(dict(archives=len(archives), binaries=record['binary_count'], unique_sets=len(groups))))
