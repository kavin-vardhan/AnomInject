import hashlib
import json
from pathlib import Path
import sys
import time

w = Path('E:/IA_BuildCache/_r921/09203c')
output = w / (sys.argv[1] + '.json')
assert not output.exists()
evidence = json.loads((w / 'source-evidence.json').read_text())
checked = []
for item in evidence:
    if 'path' not in item:
        continue
    source = Path(item['path'])
    actual = hashlib.sha256(source.read_bytes()).hexdigest()
    assert actual == item['sha256'], str(source)
    checked.append(dict(path=str(source), sha256=actual))
tool = json.loads((w / 'uat-compilation-attempt.json').read_text())['dll']
assert hashlib.sha256(Path(tool['path']).read_bytes()).hexdigest() == tool['sha256']
output.write_text(json.dumps(dict(checked=time.time(), source_claims=len(checked), unique_source_files=len({item['path'] for item in checked}), files=checked, existing_automation_tool=tool), indent=2))
print(json.dumps(dict(source_claims=len(checked), unique_source_files=len({item['path'] for item in checked}), automation_tool_unchanged=True)))
