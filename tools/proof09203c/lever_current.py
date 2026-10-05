import json
import subprocess
import sys
from pathlib import Path

w = Path('E:/IA_BuildCache/_r921/09203c')
repo = w.parent / 'AnomalyInjector'
manifest = json.loads((w / 'probe-archive.json').read_text())
archive = Path(manifest['archive'])
audit = Path(json.loads((w / 'latest-audit.json').read_text())['path'])
assert audit.name == archive.name
results = []
for config in ('Development', 'Editor'):
    paths = [p for p in manifest['files'] if (p.endswith('.exe') if config == 'Development' else p.endswith('.dll'))]
    args = [sys.executable, '-B', str(repo / 'tools/lever_audit.py'), '--root', str(repo), '--binary-config', config]
    for path in paths:
        args += ['--binary', str(archive / path)]
    result = subprocess.run(args, capture_output=True, text=True)
    log = audit / ('lever-' + config + '.log')
    assert not log.exists()
    log.write_text(result.stdout + result.stderr)
    results.append(dict(config=config, exit=result.returncode, files=paths, log=str(log)))
    print('\n'.join(line for line in result.stdout.splitlines() if any(word in line for word in ('BINARY', 'SUMMARY', 'VERDICT', 'FAIL'))), flush=True)
(audit / 'lever-results.json').write_text(json.dumps(results, indent=2))
raise SystemExit(any(r['exit'] for r in results))
