import hashlib
import json
from pathlib import Path
import shutil

w = Path('E:/IA_BuildCache/_r921/09203c')
source = Path('E:/IA_BuildCache/_r53_host/StackOBot/Content/CaptureBenchTexCorrupt/ProxyVT0921C')
target = w / 'fixture-final-archive'
assert not target.exists()
paths = sorted(p for p in source.iterdir() if p.is_file())
assert len(paths) == 10
assert min(shutil.disk_usage(d+':\\').free for d in ('D','E')) - sum(p.stat().st_size for p in paths) >= 50*1024**3
target.mkdir()
record = []
for path in paths:
    destination = target / path.name
    shutil.copyfile(path, destination)
    digest = hashlib.sha256(path.read_bytes()).hexdigest()
    assert hashlib.sha256(destination.read_bytes()).hexdigest() == digest
    record.append(dict(source=str(path), archive=str(destination), bytes=path.stat().st_size, sha256=digest))
(w / 'fixture-final-archive.json').write_text(json.dumps(record, indent=2))
print(json.dumps(dict(files=len(record), bytes=sum(row['bytes'] for row in record), archive=str(target))))
