import hashlib
import json
from pathlib import Path
import re
from pe_metadata import PE

w = Path('E:/IA_BuildCache/_r921/09203c')
repo = w.parent / 'AnomalyInjector'
audit = Path(json.loads((w / 'latest-audit.json').read_text())['path'])
archive = Path(json.loads((w / 'probe-archive.json').read_text())['archive'])
items = json.loads((audit / 'strings-reviewed.json').read_text())
addresses = json.loads((audit / 'fragment-address-evidence.json').read_text())
engine = Path('D:/UESource/UnrealEngine/Engine/Source/Runtime')
paths = [p for p in (repo / 'Source').rglob('*') if p.suffix in ('.h', '.cpp', '.cs')]
paths += list((repo / 'Shaders').rglob('*.usf'))
paths += [engine / p for p in ('Core/Public/Containers/ArrayView.h', 'RHI/Public/RHICommandList.h', 'Engine/Classes/Engine/StreamableRenderAsset.h', 'CoreUObject/Public/UObject/StrongObjectPtr.h', 'Engine/Public/TextureResource.h', 'Core/Public/Delegates/DelegateInstancesImpl.h', 'RenderCore/Public/ShaderParameterMetadata.h')]
texts = {}
for path in paths:
    body = path.read_text(encoding='utf-8-sig')
    joined = re.sub(r'"\)\s*TEXT\("', '', body)
    joined = re.sub(r'"\s*"', '', joined)
    texts[path] = (body, joined)
pe = {}
used = set()
fragments = {'9Y>)F P!B', '@08~A`', '@8@HNA`', '@8@NA`', 'P08~A`', '`8@HVA`', 'r_@u:K'}
fragments.update({'8M%iB0', '9Y>)F@R!B', '@08~ab', '@8@HNab', '@8@Nab', 'P08~ab', '`8@HVab', 'ltuo .'})
fragments.update({"'7@|}K", '@p@P*$K', 'JIKQR8'})
for item in items:
    if not item['review'].startswith('UNRESOLVED') and not item['review'].endswith('source verification required'):
        continue
    value = item['string']
    hits = [p for p, (body, joined) in texts.items() if value in body or value in joined or value == str(p)]
    if not hits and value.startswith('E:\\IA_BuildCache\\_r53_host\\StackOBot\\Plugins\\AnomalyInjector\\'):
        path = repo / value.split('AnomalyInjector\\', 1)[1]
        if path.is_file():
            hits = [path]
    if hits:
        item.update(review='Declared production/bench diagnostic, shader identifier, inline engine assertion, or verified source path', source=[str(p) for p in hits])
        used.update(hits)
        continue
    binary = pe.setdefault(item['file'], PE(archive / item['file']))
    if value in ('UnrealEditor-UnrealEd.dll', 'api-ms-win-crt-convert-l1-1-0.dll'):
        rva, length = binary.directories[1]
        off = binary.offset(rva)
        modules = []
        while binary.u32(off+12):
            modules.append(binary.cstring(binary.offset(binary.u32(off+12))))
            off += 20
        assert value in modules
        item.update(review='Verified PE import-directory module name', import_modules=modules)
        continue
    debug = binary.debug_ranges()
    if all(any(first <= place['offset'] and place['offset']+len(value) <= last for first,last,pdb in debug) for place in item['places']):
        item.update(review='Verified PE CodeView RSDS/GUID/age bytes, before the PDB path', codeview_ranges=debug)
        continue
    if item['file'] == 'StackOBot.exe' and value in fragments:
        row = next(r for r in addresses if r['file'] == item['file'] and r['string'] == value)
        assert all(p['embedded_address_candidates'] for p in row['evidence'])
        item.update(review='Exact manually reviewed printable run intersecting pointer/RVA data; byte and address context retained', address_evidence=row['evidence'])
        if value in {"'7@|}K", '@p@P*$K', 'JIKQR8'}:
            confirmed = json.loads((audit / 'pe-pointer-relocations-final.json').read_text())
            assert confirmed['sha256'] == hashlib.sha256((archive / item['file']).read_bytes()).hexdigest()
            item['relocation_evidence'] = next(entry for entry in confirmed['rows'] if entry['string'] == value)
        continue
    if item['encoding'] == 'ascii' and item['file'] == 'editor/UnrealEditor-AnomalyInjector.dll':
        ranges = binary.eh4_ranges()
        matches = [[r for r in ranges if r['first'] <= p['offset'] and p['offset']+len(value) <= r['last']] for p in item['places']]
        if all(matches):
            item.update(review='Decoded Microsoft EH4 compressed exception metadata; every byte belongs to a referenced IP-to-state map', eh4_ranges=matches)
            continue
    if value == ']IAI.Bench.ProxyVTNoBind=%d':
        path = repo / 'Source/AnomalyInjector/Private/Anomalies/ProxyVTBench.cpp'
        assert value[1:] in texts[path][0] and item['encoding'] == 'utf16'
        assert all(bytes.fromhex(h)[8:10] == b']\x00' for h in item['contexts_hex'])
        item.update(review='One pooled-data prefix code unit before the exact declared bench diagnostic; hex context retained', source=[str(path)])
        used.add(path)
        continue
unresolved = [i for i in items if i['review'].startswith('UNRESOLVED') or i['review'].endswith('source verification required')]
(audit / 'strings-reviewed-final.json').write_text(json.dumps(items, indent=2))
ms = Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.38.33130')
used.update([ms / 'include/ehdata4_export.h', ms / 'crt/src/x64/gshandlereh4.cpp'])
result = dict(reviewed=len(items), unresolved=len(unresolved), sources=[dict(path=str(p), sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in sorted(used)], appended_semantic_mutant_caught=json.loads((audit / 'string-can-fail.json').read_text())['caught'])
(audit / 'strings-result.json').write_text(json.dumps(result, indent=2))
print(json.dumps(dict(reviewed=len(items), unresolved=[i['string'] for i in unresolved]), indent=2))
raise SystemExit(bool(unresolved))
