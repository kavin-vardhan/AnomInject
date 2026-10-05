import hashlib
import json
import re
from pathlib import Path
from pe_metadata import PE

w=Path('E:/IA_BuildCache/_r921/09205')
repo=w.parent/'AnomalyInjector'
audit=Path(json.loads((w/'latest-audit.json').read_text())['path'])
archive=Path(json.loads((w/'probe-archive.json').read_text())['archive'])
items=json.loads((audit/'strings-reviewed.json').read_text())
sources={p:re.sub(r'"\)\s*TEXT\("','',p.read_text(encoding='utf-8-sig')) for p in (repo/'Source').rglob('*') if p.suffix in ('.h','.cpp','.cs')}
binaries={}
evidence=[]
for item in items:
    if not item['review'].startswith('UNRESOLVED') and not item['review'].endswith('source verification required'): continue
    value=item['string']
    hits=[str(p) for p,text in sources.items() if value in text]
    if not hits and value.startswith('E:\\IA_BuildCache\\_r53_host\\StackOBot\\Plugins\\AnomalyInjector\\'):
        candidate=repo/value.split('AnomalyInjector\\',1)[1]
        if candidate.is_file(): hits=[str(candidate)]
    if hits:
        item.update(review='Exact declared source literal or existing source path',source=hits)
        continue
    binary=binaries.setdefault(item['file'],PE(archive/item['file']))
    if all(any(first<=p['offset'] and p['offset']+len(value)<=last for first,last,_ in binary.debug_ranges()) for p in item['places']):
        item.update(review='Verified CodeView GUID/age bytes',debug_ranges=binary.debug_ranges())
        continue
    rva,size=binary.directories[5]
    cursor=binary.offset(rva)
    end=cursor+size
    offsets={}
    while cursor<end:
        page,count=binary.u32(cursor),binary.u32(cursor+4)
        assert count>=8
        for position in range(cursor+8,cursor+count,2):
            entry=binary.u16(position)
            if entry>>12==10:
                relocation=page+(entry&4095)
                offsets[binary.offset(relocation)]=relocation
        cursor+=count
    records=[]
    for place in item['places']:
        start=place['offset']
        length=len(value)*(2 if item['encoding']=='utf16' else 1)
        found=[]
        for offset in range(max(0,start-7),start+length):
            if offset not in offsets: continue
            target=binary.u64(offset)-binary.image_base
            try: target_offset=binary.offset(target)
            except ValueError: continue
            found.append(dict(offset=offset,rva=offsets[offset],target_rva=target,target_offset=target_offset,
                bytes=binary.data[max(0,start-8):start+length+8].hex(),shifted_can_fail=offset+1 not in offsets))
        if not found: break
        records.append(found)
    if len(records)==len(item['places']):
        item.update(review='Printable run intersects a verified DIR64 relocated image pointer',relocations=records)
        evidence.append(dict(file=item['file'],string=value,relocations=records))
        continue
    if item['file'].endswith('.dll'):
        ranges=binary.eh4_ranges()
        matches=[[r for r in ranges if r['first']<=p['offset'] and p['offset']+len(value)<=r['last']] for p in item['places']]
        if all(matches): item.update(review='Decoded Microsoft EH4 exception metadata',ranges=matches)
unresolved=[item for item in items if item['review'].startswith('UNRESOLVED') or item['review'].endswith('source verification required')]
(audit/'strings-reviewed-final.json').write_text(json.dumps(items,indent=2))
result=dict(reviewed=len(items),unresolved=len(unresolved),unresolved_strings=[dict(file=i['file'],string=i['string'],places=i['places']) for i in unresolved],
    relocation_evidence=evidence,semantic_mutant_caught=json.loads((audit/'string-can-fail.json').read_text())['caught'])
(audit/'strings-result.json').write_text(json.dumps(result,indent=2))
print(json.dumps(dict(reviewed=len(items),unresolved=result['unresolved_strings']),indent=2))
raise SystemExit(bool(unresolved))
