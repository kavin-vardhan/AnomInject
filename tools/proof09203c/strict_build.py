import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

work = Path('E:/IA_BuildCache/_r921/09203c')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
plugin = host / 'Plugins/AnomalyInjector'
logdir = work / 'builds' / sys.argv[1]
assert not logdir.exists()
assert min(shutil.disk_usage(drive + ':\\').free for drive in ('D', 'E')) >= 50*1024**3
assert subprocess.check_output(['git', '-C', str(plugin), 'status', '--porcelain'], text=True).strip() == ''
os.environ['TEMP'] = str(work / 'temp')
os.environ['TMP'] = str(work / 'temp')
logdir.mkdir(parents=True)
args = [sys.executable, '-B', str(work / 'strict_driver.py'), '--uproject', str(host / 'StackOBot.uproject'), '--engine-root', 'D:/UESource/UnrealEngine', '--log-dir', str(logdir)]
with (logdir / 'driver.log').open('w', encoding='utf-8') as output:
    process = subprocess.Popen(args, stdout=output, stderr=subprocess.STDOUT)
    (logdir / 'launch.json').write_text(json.dumps(dict(pid=process.pid, args=args)), encoding='utf-8')
    result = process.wait()
print('STRICT EXIT', result, flush=True)
if (logdir / 'summary.json').exists():
    print((logdir / 'summary.json').read_text(), flush=True)
raise SystemExit(result)
