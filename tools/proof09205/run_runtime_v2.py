import concurrent.futures
import json
import os
from pathlib import Path
import subprocess
import sys
import threading
import time

w = Path('E:/IA_BuildCache/_r921/09205')
assert json.loads((w / 'part-b-build-gate.json').read_text())['archive']['source_commit'].startswith('502de71')
assert json.loads((w / 'current-cook.json').read_text())['source_commit'].startswith('502de71')
env = os.environ.copy()
env.update(TEMP=str(w / 'temp'), TMP=str(w / 'temp'))
records = []
lock = threading.Lock()
receipt = w / 'runtime-v2-queue.json'
assert not receipt.exists()


def run(script, args, label=None):
    label = label or args[0]
    path = w / ('v2-' + label + '-' + script + '.log')
    assert not path.exists(), path
    started = time.time()
    with path.open('w') as output:
        result = subprocess.run([sys.executable, '-B', str(w / script), *args], stdout=output, stderr=subprocess.STDOUT, env=env)
    row = dict(script=script, args=args, exit=result.returncode, seconds=time.time()-started, log=str(path))
    with lock:
        records.append(row)
        receipt.write_text(json.dumps(records, indent=2))
    print(json.dumps(row), flush=True)
    assert result.returncode == 0, row


def gate(label):
    run('idle_gate.py', ['--watch'], label)


for fixture in ('stacks', 'single'):
    for scope in ('all', 'view'):
        label = 'census-' + fixture + '-' + scope + '-part-b-v2'
        gate(label)
        run('launch_final_census.py', [label, fixture, scope])
        run('check_census.py', [label])
    label = 'census-' + fixture + '-miscount-part-b-v2'
    gate(label)
    run('launch_final_census.py', [label, fixture, 'all', 'mutant'])
    run('check_census.py', [label])


def positive_checks(label, count):
    for script, args in (
        ('check_stack_positive.py', [label, count]),
        ('check_stack_layers.py', [label]),
        ('collect_stack_lifetime.py', [label, count]),
    ):
        run(script, args)


with concurrent.futures.ThreadPoolExecutor(max_workers=1) as pool:
    pending = []
    label = 'stacks-identity-final-v2'
    gate(label)
    run('launch_stack.py', [label, 'identity', '1'])
    pending.append(pool.submit(run, 'check_stack_identity.py', [label]))
    for label, leg, kind, count in (
        ('stacks-pie-targeted-final-v2', 'pie', 'targeted', '10'),
        ('stacks-pie-auto-final-v2', 'pie', 'auto', '3'),
        ('stacks-staged-targeted-final-v2', 'staged', 'targeted', '5'),
        ('stacks-staged-auto-final-v2', 'staged', 'auto', '3'),
        ('stacks-readiness-delay-control-v2', 'staged', 'ready_delay', '1'),
    ):
        for future in pending:
            if future.done(): future.result()
        gate(label)
        run('launch_staged_stack.py' if leg == 'staged' else 'launch_stack.py', [label, kind, count])
        pending.append(pool.submit(positive_checks, label, count))
    for future in pending: future.result()

for kind in ('partial', 'no_bind', 'early_delay'):
    label = 'stacks-' + kind.replace('_', '-') + '-final-v2'
    gate(label)
    run('run_mutant_case.py', [label, kind])
for kind in ('destroy', 'displace', 'world_end'):
    label = 'stacks-' + kind.replace('_', '-') + '-final-v2'
    gate(label)
    run('run_lifetime_case.py', [label, kind])
for label, kind in (
    ('stacks-release-refusal-final-v2', 'baseline'),
    ('stacks-accepted-refusal-final-v2', 'accepted_baseline'),
    ('stacks-unnamed-refusal-final-v2', 'unnamed'),
):
    gate(label)
    run('run_refusal_case.py', [label, kind])
for kind in ('hold', 'proxy', 'uv', 'normal', 'single_vt'):
    label = 'regression-' + kind.replace('_', '-') + '-final-v2'
    gate(label)
    run('run_regression_case.py', [label, kind])
print('FINAL RUNTIME V2 QUEUE PASS', flush=True)
