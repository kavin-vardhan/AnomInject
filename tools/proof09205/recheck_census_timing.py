import json
import shutil
import subprocess
import sys
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
legs=['census-single-view-final','census-single-all-final-v3','census-stacks-view-final','census-stacks-all-final','census-staged-stacks-all-02','census-staged-single-all']
for name in legs:
    root=w/'runs'/name
    old=root/'census-result-without-timing-gate.json'
    assert not old.exists()
    shutil.copyfile(root/'census-result.json',old)
    with (root/'timing-recheck.txt').open('w') as log:
        result=subprocess.run([sys.executable,'-B',str(w/'check_census.py'),name],stdout=log,stderr=subprocess.STDOUT)
    data=json.loads((root/'census-result.json').read_text())
    expected='FAIL' if name.startswith('census-staged-') else 'PASS'
    assert data['status']==expected,(name,data)
    print(json.dumps(dict(leg=name,status=data['status'],expected=expected,timing=data['timing'])),flush=True)
