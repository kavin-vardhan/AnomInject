import hashlib
import json
import shutil
import sys
import zipfile
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
source=Path('E:/IA_BuildCache/_r53_host/StackOBot/Content/CaptureBenchTexCorrupt/ProxyVT09205')
label=sys.argv[1]
archive=w/'archives'/('fixture-'+label+'.zip')
assert not archive.exists()
paths=sorted(source.rglob('*'))
paths=[p for p in paths if p.is_file()]
assert len(paths)==10,len(paths)
assert min(shutil.disk_usage(d+':\\').free for d in ('D','E'))-sum(p.stat().st_size for p in paths)>50*1024**3
hashes={str(p.relative_to(source)).replace('\\','/'):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths}
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
    for p in paths: z.write(p,str(p.relative_to(source)).replace('\\','/'))
with zipfile.ZipFile(archive) as z:
    assert set(z.namelist())==set(hashes)
    for name,digest in hashes.items(): assert hashlib.sha256(z.read(name)).hexdigest()==digest
result=dict(source=str(source),archive=str(archive),sha256=hashlib.sha256(archive.read_bytes()).hexdigest(),files=hashes,rehashed=True)
archive.with_suffix('.json').write_text(json.dumps(result,indent=2))
print(json.dumps(result,indent=2))
