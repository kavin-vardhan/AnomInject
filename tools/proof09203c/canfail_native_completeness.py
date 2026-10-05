import ast
import hashlib
import json
from pathlib import Path

w = Path('E:/IA_BuildCache/_r921/09203c')
reader = w / 'check_native.py'
tree = ast.parse(reader.read_text())
gate = next(node for node in tree.body if isinstance(node, ast.Assert) and 'list(range(480))' in ast.unparse(node.test))
code = compile(ast.Module(body=[gate], type_ignores=[]), str(reader), 'exec')
results = []
for name, indices, expected in [('complete', list(range(480)), True), ('truncated', list(range(479)), False), ('duplicated', list(range(479))+[478], False)]:
    try:
        exec(code, dict(rows=[dict(session_index=i) for i in indices]))
        accepted = True
    except AssertionError:
        accepted = False
    assert accepted == expected
    results.append(dict(case=name, accepted=accepted, expected=expected))
record = dict(status='PASS', source_sha256=hashlib.sha256(reader.read_bytes()).hexdigest(), results=results)
(w / 'native-completeness-can-fails.json').write_text(json.dumps(record, indent=2))
print(json.dumps(record, indent=2))
