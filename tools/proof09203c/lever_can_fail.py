import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys

work = Path('E:/IA_BuildCache/_r921/09203c')
repo = work.parent / 'AnomalyInjector'
mutant = work / 'lever-mutant'
assert not mutant.exists()
mutant.mkdir()
shutil.copytree(repo / 'Source', mutant / 'Source')
descriptor = json.loads((repo / 'AnomalyInjector.uplugin').read_text(encoding='utf-8-sig'))
for module in descriptor['Modules']:
    if module['Name'] == 'AnomalyInjector':
        module['TargetConfigurationDenyList'] = [entry for entry in module.get('TargetConfigurationDenyList', []) if entry != 'Shipping']
(mutant / 'AnomalyInjector.uplugin').write_text(json.dumps(descriptor, indent=2), encoding='utf-8')
path = mutant / 'Source/AnomalyInjector/Private/Anomalies/ProxyVTBench.cpp'
body = path.read_text(encoding='utf-8')
assert body.count('#if !UE_BUILD_SHIPPING') == 1
path.write_text(body.replace('#if !UE_BUILD_SHIPPING', '#if 1'), encoding='utf-8')
created = {}
for path in mutant.rglob('*'):
    if path.is_file():
        with path.open('rb') as stream:
            created[str(path)] = hashlib.file_digest(stream, 'sha256').hexdigest()
(work / 'lever-mutant-receipt.json').write_text(json.dumps(dict(created=created, deleted=[]), indent=2), encoding='utf-8')
result = subprocess.run([sys.executable, '-B', str(repo / 'tools/lever_audit.py'), '--root', str(mutant)], capture_output=True, text=True)
(work / 'lever-can-fail.log').write_text(result.stdout + result.stderr, encoding='utf-8')
failures = [line for line in result.stdout.splitlines() if ' FAIL ' in line]
caught = result.returncode == 1 and any('R1' in line and 'ProxyVTBench.cpp' in line for line in failures)
record = dict(exit=result.returncode, caught=caught, planted='new VT probe made live in Shipping in a disposable copy with its module admitted to Shipping', failures=failures)
(work / 'lever-can-fail.json').write_text(json.dumps(record, indent=2), encoding='utf-8')
print(json.dumps(record, indent=2))
assert caught
