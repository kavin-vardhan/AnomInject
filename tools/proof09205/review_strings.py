import json
import mmap
from pathlib import Path

work = Path('E:/IA_BuildCache/_r921/09205')
base = Path('E:/IA_BuildCache/_r921/09203c/archives/probe-c80ef9b-1EA292E8')
archive = Path(json.loads((work / 'probe-archive.json').read_text())['archive'])
audit = Path(json.loads((work / 'latest-audit.json').read_text())['path'])
items = json.loads((audit / 'strings-review-required.json').read_text())
new_text = '\n'.join(path.read_text(encoding='utf-8') for path in (work.parent / 'AnomalyInjector/Source/AnomalyInjector/Private/Anomalies').glob('*VT*') if path.suffix in ('.h', '.cpp'))
review = []
for item in items:
    value = item['string']
    old_path = base / item['file']
    path = archive / item['file']
    encoded = value.encode('ascii' if item['encoding'] == 'ascii' else 'utf-16-le')
    with old_path.open('rb') as stream, mmap.mmap(stream.fileno(), 0, access=mmap.ACCESS_READ) as data:
        found = data.find(encoded)
    with path.open('rb') as stream:
        contexts = []
        codeview = True
        for place in item['places']:
            stream.seek(max(0, place['offset']-28))
            codeview &= b'RSDS' in stream.read(29)
            stream.seek(max(0, place['offset']-8))
            contexts.append(stream.read(len(encoded)+16).hex())
    if found >= 0:
        reason = 'Existing baseline bytes; printable-run boundary changed'
    elif '?' in value and '@@' in value:
        reason = 'C++ linkage/import/reflection symbol for the new UTexture subclass and VT resource'
    elif value in new_text or value.startswith('PROXYVT-PROBE') or value.startswith('BENCH <actor-name>') or 'ProxyVTBench.cpp' in value or 'AnomalyPrivateVT' in value:
        reason = 'Declared private VT class, bench command, diagnostic, or source path'
    elif codeview:
        reason = 'CodeView GUID bytes adjacent to RSDS'
    elif value.startswith('Rich') and all(place['offset'] < 1024 for place in item['places']):
        reason = 'PE Rich header metadata'
    elif value in ('CalcCumulativeLODSize', 'FTexture', 'StreamIn', 'StreamOut', 'UStreamableRenderAsset::%s has no default implementation', 'IsInGameThread() || IsInGarbageCollectorThread()', 'TStrongObjectPtr can only be created on the game thread otherwise it may introduce threading issues with Grbage Collector', 'UEStrongObjectPtr_Private::TInternalReferenceCollector') or value.endswith(('UObject\\StrongObjectPtr.h', 'Engine\\StreamableRenderAsset.h')):
        reason = 'Engine base-class/strong-reference implementation string; source verification required'
    else:
        reason = 'UNRESOLVED printable data fragment; no semantic approval inferred'
    review.append(dict(**item, review=reason, baseline_byte_offset=found, contexts_hex=contexts))
(audit / 'strings-reviewed.json').write_text(json.dumps(review, indent=2), encoding='utf-8')
unresolved = [item for item in review if item['review'].startswith('UNRESOLVED')]
print(json.dumps(dict(reviewed=len(review), unresolved=len(unresolved), fragments=[dict(string=item['string'], places=item['places'], contexts_hex=item['contexts_hex']) for item in unresolved]), indent=2))
