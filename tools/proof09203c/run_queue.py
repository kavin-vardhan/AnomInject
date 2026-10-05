import json
from pathlib import Path
import subprocess
import sys
import time
import idle_gate

w = Path('E:/IA_BuildCache/_r921/09203c')
plan_path = Path(sys.argv[1]).resolve()
assert plan_path.is_relative_to(w)
plan = json.loads(plan_path.read_text())
record_path = plan_path.with_suffix('.result.json')
assert not record_path.exists()
record = dict(plan=str(plan_path), started=time.time(), status='RUNNING', steps=[])
record_path.write_text(json.dumps(record, indent=2))
for index, step in enumerate(plan):
    current = dict(index=index, command=step['command'], engine=step.get('engine', False), started=time.time(), observations=[])
    record['steps'].append(current)
    while time.time() < step.get('not_before', 0):
        time.sleep(min(30, step['not_before']-time.time()))
    if current['engine']:
        blocked_since = None
        disk_since = None
        while True:
            gate = idle_gate.read()
            current['observations'].append(gate)
            record_path.write_text(json.dumps(record, indent=2))
            print(json.dumps(dict(step=index, gate=gate)), flush=True)
            if gate['clear']:
                break
            now = time.time()
            blocked_since = blocked_since or now
            low_disk = any(value < 50*1024**3 for value in gate.get('disk_free_bytes', {}).values())
            disk_since = (disk_since or now) if low_disk else None
            if now-blocked_since >= 4*3600 or (disk_since and now-disk_since >= 3600):
                record.update(status='NEEDS-DECISION', reason='bounded idle or disk wait exhausted', finished=now)
                record_path.write_text(json.dumps(record, indent=2))
                raise SystemExit(2)
            until = now+300
            while time.time() < until:
                time.sleep(min(30, until-time.time()))
    log_path = plan_path.with_name(plan_path.stem + '-step%02d.log' % index)
    command = [sys.executable, '-B', str(w / step['command'][0]), *step['command'][1:]]
    with log_path.open('w', encoding='utf-8') as stream:
        process = subprocess.Popen(command, stdout=stream, stderr=subprocess.STDOUT, creationflags=subprocess.CREATE_NO_WINDOW, cwd=w)
        current.update(pid=process.pid, log=str(log_path))
        record_path.write_text(json.dumps(record, indent=2))
        current['exit'] = process.wait()
    current['finished'] = time.time()
    expected = step.get('expected_exit', 0)
    if current['exit'] != expected:
        record.update(status='STOPPED', reason='unexpected step exit', finished=time.time())
        record_path.write_text(json.dumps(record, indent=2))
        print(json.dumps(current), flush=True)
        raise SystemExit(1)
    record_path.write_text(json.dumps(record, indent=2))
    print(json.dumps(dict(step=index, exit=current['exit'], seconds=current['finished']-current['started'])), flush=True)
record.update(status='PASS', finished=time.time())
record_path.write_text(json.dumps(record, indent=2))
