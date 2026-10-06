import hashlib
import json
from pathlib import Path
import subprocess
import sys

w=Path('E:/IA_BuildCache/_r921/09205')
summary=json.loads((w/'builds/part-b-strict-01/summary.json').read_text())
assert summary['restore_ok']
for row in summary['strict']+summary['normal']:
    assert row['exit']==row['errors']==row['warnings']==0,row
assert all(row['header_tus_compiled']==72 for row in summary['strict'])
for script in (() if sys.argv[1:]==['--collect'] else ('archive_probe.py','audit_probe.py','review_strings.py','finish_strings.py')):
    with (w/('part-b-final-'+script+'.log')).open('w') as output:
        result=subprocess.run([sys.executable,'-B',str(w/script)],stdout=output,stderr=subprocess.STDOUT)
    assert result.returncode==0,(script,result.returncode)
archive=json.loads((w/'probe-archive.json').read_text())
audit=Path(json.loads((w/'latest-audit.json').read_text())['path'])
strings=json.loads((audit/'strings-result.json').read_text())
assert strings['unresolved']==0 and strings['semantic_mutant_caught']
assert all(row['exit']==0 for row in json.loads((audit/'lever-results.json').read_text()))
with Path(archive['archive'],'StackOBot.exe').open('rb') as stream:
    assert hashlib.file_digest(stream,'sha256').hexdigest()==archive['files']['StackOBot.exe']
(w/'part-b-build-gate.json').write_text(json.dumps(dict(status='PASS',builds=summary,archive=archive,audit=json.loads((w/'latest-audit.json').read_text())),indent=2))
print(json.dumps(dict(status='PASS',archive=archive['archive'])))
