import hashlib
import json
import shutil
from pathlib import Path
from pe_metadata import PE

w=Path('E:/IA_BuildCache/_r921/09205')
audit=Path(json.loads((w/'latest-audit.json').read_text())['path'])
archive=Path(json.loads((w/'probe-archive.json').read_text())['archive'])
source=audit/'strings-reviewed-final.json'
backup=audit/'strings-reviewed-before-stack-supplement.json'
assert not backup.exists()
shutil.copy2(source,backup)
items=json.loads(source.read_text())
engine=Path('D:/UESource/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandList.inl')
body=engine.read_text(encoding='utf-8-sig')
binary=PE(archive/'StackOBot.exe')
ranges=binary.eh4_ranges()
records=[]
for item in items:
    if not item['review'].startswith('UNRESOLVED'): continue
    value=item['string']
    if value=='FlushRHIThreadTotal':
        assert value in body
        item.update(review='Exact engine CSV timing identifier from the public immediate RHI flush',source=str(engine),source_sha256=hashlib.sha256(engine.read_bytes()).hexdigest())
        continue
    assert item['file']=='StackOBot.exe' and item['encoding']=='ascii',item
    matches=[[row for row in ranges if row['first']<=p['offset'] and p['offset']+len(value)<=row['last']] for p in item['places']]
    if all(matches):
        item.update(review='Decoded referenced Microsoft EH4 exception metadata; complete printable run covered',ranges=matches)
        records.append(dict(string=value,ranges=matches))
unresolved=[item for item in items if item['review'].startswith('UNRESOLVED')]
source.write_text(json.dumps(items,indent=2))
prior=json.loads((audit/'strings-result.json').read_text())
result=dict(prior,unresolved=len(unresolved),unresolved_strings=[dict(file=i['file'],string=i['string'],places=i['places']) for i in unresolved],eh4_supplement=records,engine_source=str(engine))
(audit/'strings-result.json').write_text(json.dumps(result,indent=2))
print(json.dumps(dict(reviewed=len(items),metadata_ranges=len(ranges),resolved_metadata=len(records),unresolved=result['unresolved_strings']),indent=2))
raise SystemExit(bool(unresolved))
