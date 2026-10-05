import hashlib
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

work = Path('E:/IA_BuildCache/_r921/09203c')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
plugin = host / 'Plugins/AnomalyInjector'
engine = Path('D:/UESource/UnrealEngine')
temp = work / 'temp'
temp.mkdir(exist_ok=True)
os.environ['TEMP'] = str(temp)
os.environ['TMP'] = str(temp)
sys.dont_write_bytecode = True
sys.path.insert(0, str(plugin / 'tools'))
import strict_include_build as builds

label = sys.argv[1]
targets = sys.argv[2:] or ['StackOBotEditor', 'StackOBot']
logdir = work / 'builds' / label
logdir.mkdir(parents=True, exist_ok=False)
commit = subprocess.check_output(['git', '-C', str(plugin), 'rev-parse', 'HEAD'], text=True).strip()
results = []
for target in targets:
    assert min(shutil.disk_usage(drive + ':\\').free for drive in ('D', 'E')) > 50 * 1024**3
    start = time.time()
    result = builds.run_build(str(engine), target, str(host / 'StackOBot.uproject'), str(logdir / (target + '.log')))
    diagnostics = builds.parse_diagnostics(result['output'])
    errors, warnings = builds.summarize('NORMAL', result, diagnostics)
    results.append(dict(target=target, exit=result['exit'], errors=errors, warnings=warnings, seconds=time.time()-start, commit=commit))
    (logdir / 'summary.json').write_text(json.dumps(results, indent=2), encoding='utf-8')
    print(json.dumps(results[-1]), flush=True)
    if result['exit'] or errors or warnings:
        raise SystemExit(1)
