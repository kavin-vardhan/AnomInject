import json
import os
from pathlib import Path
import subprocess
import sys
import time

w = Path('E:/IA_BuildCache/_r921/09205')
path = w / 'builds/part-b-strict-02/summary.json'
deadline = time.time() + 7200
while not path.exists():
    assert time.time() < deadline
    time.sleep(5)
summary = json.loads(path.read_text())
assert summary['restore_ok']
assert all(row['exit'] == row['errors'] == row['warnings'] == 0 for row in summary['strict'] + summary['normal'])
assert all(row['header_tus_compiled'] == 72 for row in summary['strict'])
env = os.environ.copy()
env.update(TEMP=str(w / 'temp'), TMP=str(w / 'temp'))
steps = []
for name in ('archive_probe.py', 'audit_probe.py', 'review_strings.py', 'finish_strings.py', 'finish_stack_strings.py'):
    log = w / ('part-b-v2-' + name + '.log')
    assert not log.exists()
    with log.open('w') as output:
        result = subprocess.run([sys.executable, '-B', str(w / name)], stdout=output, stderr=subprocess.STDOUT, env=env)
    steps.append(dict(script=name, exit=result.returncode, log=str(log)))
    print(json.dumps(steps[-1]), flush=True)
    if result.returncode and name != 'finish_strings.py':
        raise SystemExit(result.returncode)
archive = json.loads((w / 'probe-archive.json').read_text())
audit = json.loads((w / 'latest-audit.json').read_text())
strings = json.loads((Path(audit['path']) / 'strings-result.json').read_text())
assert strings['unresolved'] == 0 and strings['semantic_mutant_caught']
assert all(row['exit'] == 0 for row in json.loads((Path(audit['path']) / 'lever-results.json').read_text()))
record = dict(status='PASS', builds=summary, archive=archive, audit=audit, steps=steps)
(w / 'part-b-build-gate.json').write_text(json.dumps(record, indent=2))
print(json.dumps(dict(status='PASS', archive=archive['archive'], strings=strings['reviewed'])), flush=True)
