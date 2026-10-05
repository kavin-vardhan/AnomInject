import hashlib
import json
from pathlib import Path
from pe_metadata import PE

w = Path('E:/IA_BuildCache/_r921/09203c')
audit = Path(json.loads((w / 'latest-audit.json').read_text())['path'])
archive = Path(json.loads((w / 'probe-archive.json').read_text())['archive'])
path = archive / 'StackOBot.exe'
binary = PE(path)
rva, size = binary.directories[5]
cursor = binary.offset(rva)
end = cursor + size
relocations = set()
while cursor < end:
    page, count = binary.u32(cursor), binary.u32(cursor+4)
    assert count >= 8
    for position in range(cursor+8, cursor+count, 2):
        entry = binary.u16(position)
        if entry >> 12 == 10:
            relocations.add(page + (entry & 4095))
    cursor += count
rows = json.loads((audit / 'fragment-address-evidence.json').read_text())
selected_sets = {
    '1ea292e815e5acf5c6ab214043b7d255c0f35d7e456c23c0d4e0a67fe489e891': {"         '(((((((((0", '8M%iB@', '9Y>)FPS!B', 'ltuo@/', 'r_@v:K'},
    'fe338251aab6bbfef0f0dc39baf7b2d91ce9e2356ba06d9763da314b28b20651': {"'7@|}K", '@p@P*$K', 'JIKQR8'},
    '0f79605d3d9e66e6fc0685afaededf3430f5368db35fa452f1adf307c7315bcb': {"         '(((((((((P", ' A -NC', '0@ srF', '8M%iB`', '@p@p<$K', 'JIKQRx+iL', 'ltuo`<'},
}
selected = selected_sets[hashlib.sha256(binary.data).hexdigest()]
result = []
for row in rows:
    if row['string'] not in selected:
        continue
    assert row['file'] == 'StackOBot.exe'
    evidence = []
    for occurrence in row['evidence']:
        pointer = next(p for p in occurrence['embedded_address_candidates'] if p['size'] == 8 and p['offset'] % 8 == 0)
        source_rva = next(start + pointer['offset'] - raw for name, start, length, raw, raw_length in binary.sections if raw <= pointer['offset'] < raw + raw_length)
        assert source_rva in relocations
        assert source_rva + 1 not in relocations
        target_rva = binary.u64(pointer['offset']) - binary.image_base
        target_offset = binary.offset(target_rva)
        evidence.append(dict(pointer=pointer, source_rva=hex(source_rva), relocation='IMAGE_REL_BASED_DIR64', target_offset=target_offset,
                             target_bytes=binary.data[target_offset:target_offset+48].hex(), shifted_pointer_can_fail=True))
    result.append(dict(string=row['string'], evidence=evidence))
assert {row['string'] for row in result} == selected
record = dict(binary=str(path), sha256=hashlib.sha256(binary.data).hexdigest(), rows=result)
(audit / 'pe-pointer-relocations-final.json').write_text(json.dumps(record, indent=2))
print(json.dumps(record, indent=2))
