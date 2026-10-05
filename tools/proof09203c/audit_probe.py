import ast
import collections
import hashlib
import json
import mmap
from pathlib import Path
import re
import shutil
import struct
import subprocess
import sys

work = Path('E:/IA_BuildCache/_r921/09203c')
repo = work.parent / 'AnomalyInjector'
record = json.loads((work / 'probe-archive.json').read_text())
archive = Path(record['archive'])
audit = work / 'audits' / archive.name
assert not audit.exists()
audit.mkdir(parents=True)
(work / 'latest-audit.json').write_text(json.dumps(dict(path=str(audit), archive=str(archive))), encoding='utf-8')
baseline = Path('E:/IA_BuildCache/_binary_baselines/release-2026-10-05-23098fa-218BA4C7')
source = Path('E:/IA_BuildCache/_r912b/binary_checks.py')
tree = ast.parse(source.read_text(encoding='utf-8'))
functions = [node for node in tree.body if isinstance(node, ast.FunctionDef) and node.name in ('sections', 'compare')]
exec(compile(ast.Module(body=functions, type_ignores=[]), str(source), 'exec'))


def scan(path):
    out = {}
    with path.open('rb') as stream, mmap.mmap(stream.fileno(), 0, access=mmap.ACCESS_READ) as data:
        ranges = sections(data)
        for encoding, expression in [('ascii', rb'[\x20-\x7e]{6,}'), ('utf16', rb'(?:[\x20-\x7e]\x00){6,}')]:
            values = collections.defaultdict(list)
            for match in re.finditer(expression, data):
                text = match[0].decode('ascii' if encoding == 'ascii' else 'utf-16-le')
                section = next((name for first, last, name in ranges if first <= match.start() < last), 'headers/overlay')
                values[text].append(dict(offset=match.start(), section=section))
            out[encoding] = values
    return out


results = []
review = []
for relative in record['files']:
    old = scan(baseline / relative)
    new = scan(archive / relative)
    diff = compare(old, new)
    results.append(dict(file=relative, diff=diff))
    for encoding, group in diff.items():
        for item in group['added']:
            if item['classification'] == 'REVIEW':
                review.append(dict(file=relative, encoding=encoding, **item))
    print(relative, {encoding: group['classes'] for encoding, group in diff.items()}, flush=True)
    if relative == 'StackOBot.exe':
        assert all(group['added_count'] == group['removed_count'] == 0 for group in compare(new, new).values())
        mutant = audit / 'string-probe-mutant.bin'
        assert not mutant.exists()
        with (archive / relative).open('rb') as original, mutant.open('wb') as output:
            shutil.copyfileobj(original, output, 1024*1024)
            output.write(b'\x00IAI_09203_UNAPPROVED_STRING_CAN_FAIL\x00')
        mutant_diff = compare(new, scan(mutant))
        caught = any(item['string'] == 'IAI_09203_UNAPPROVED_STRING_CAN_FAIL' and item['classification'] == 'REVIEW' for item in mutant_diff['ascii']['added'])
        assert caught
        (audit / 'string-can-fail.json').write_text(json.dumps(dict(mutant=str(mutant), caught=caught, self_pair='zero differences', diff=mutant_diff), indent=2), encoding='utf-8')
(audit / 'strings-full.json').write_text(json.dumps(results, indent=2), encoding='utf-8')
(audit / 'strings-review-required.json').write_text(json.dumps(review, indent=2), encoding='utf-8')
result = subprocess.run([sys.executable, '-B', str(work / 'lever_current.py')], capture_output=True, text=True)
(audit / 'lever-driver.log').write_text(result.stdout + result.stderr, encoding='utf-8')
print('LEVER EXIT', result.returncode, flush=True)
print('REVIEW STRINGS', len(review), flush=True)
raise SystemExit(result.returncode)
