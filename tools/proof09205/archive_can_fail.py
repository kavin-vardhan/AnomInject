import hashlib
import json
import shutil
from pathlib import Path

work = Path('E:/IA_BuildCache/_r921/09205')
record = json.loads((work / 'probe-archive.json').read_text())
archive = Path(record['archive'])
verified = []
for relative, expected in record['files'].items():
    path = archive / relative
    with path.open('rb') as stream:
        actual = hashlib.file_digest(stream, 'sha256').hexdigest()
    assert actual == expected
    verified.append(dict(path=str(path), sha256=actual))
relative = 'editor/UnrealEditor-AnomalyBench.dll'
source = archive / relative
mutant = work / 'archive-hash-mutant.dll'
assert not mutant.exists()
shutil.copyfile(source, mutant)
with mutant.open('ab') as stream:
    stream.write(b'09205_archive_hash_negative_control')
actual = hashlib.sha256(mutant.read_bytes()).hexdigest()
assert actual != record['files'][relative]
result = dict(verified=verified, mutant=str(mutant), mutant_sha256=actual, expected=record['files'][relative], caught=True, never_launched=True)
(work / 'archive-hash-can-fail.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
print(json.dumps(dict(verified=len(verified), mutant_caught=True)))
