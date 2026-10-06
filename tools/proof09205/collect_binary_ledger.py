import hashlib
import json
import time
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
archives=[]
for path in sorted((w/'archives').glob('*/manifest.json')):
    row=json.loads(path.read_text())
    for name,digest in row['files'].items():
        with (path.parent/name).open('rb') as stream:
            assert hashlib.file_digest(stream,'sha256').hexdigest()==digest
    archives.append(row)
preflight=json.loads((w/'preflight.json').read_text())
baselines=[]
for path in [Path(preflight['binaries'][0]['archive']).parent/'manifest.json',w.parent/'09203c/archives/probe-c80ef9b-1EA292E8/manifest.json']:
    if path.is_file():
        row=json.loads(path.read_text())
        baselines.append(dict(path=str(path),manifest=row))
out=dict(verified=time.time(),archives=archives,binary_count=sum(len(row['files']) for row in archives),baseline_manifests=baselines)
(w/'binary-hash-ledger-final.json').write_text(json.dumps(out,indent=2))
lines=['All hashes are SHA-256. Every archived executable and DLL below was rehashed.','']
for row in archives:
    lines.extend(['Archive `'+row['archive']+'`, source `'+row['source_commit']+'`.','','| Binary | SHA-256 |','| --- | --- |'])
    lines.extend('| '+name+' | `'+digest+'` |' for name,digest in row['files'].items())
    lines.append('')
(w/'binary-hash-ledger-final.md').write_text('\n'.join(lines))
print(json.dumps(dict(archives=len(archives),binaries=out['binary_count'])))
