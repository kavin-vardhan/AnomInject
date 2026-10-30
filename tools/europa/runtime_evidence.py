import datetime
import hashlib
import json
from pathlib import Path
import re

from fixture_policy import sha
from owner_attempts import create_json


def freeze(root, extra=(), canary_hash=None):
    root=Path(root)
    paths=set(p for p in (root/'session').rglob('*') if p.is_file())
    paths.update(root/name for name in ('execution.json','driver.json','monitor.jsonl','prepared.json','build-watch.jsonl') if (root/name).is_file())
    paths.update(Path(p) for p in extra if Path(p).is_file())
    patterns=[re.compile(rb'canary-[0-9a-f]{24}'),re.compile(rb'c\x00a\x00n\x00a\x00r\x00y\x00-\x00(?:[0-9a-f]\x00){24}')]
    files=[];hits=[]
    for path in sorted(paths):
        digest=hashlib.sha256();overlap=b'';size=0;found=False
        with path.open('rb') as stream:
            while True:
                data=stream.read(1024*1024)
                if not data:break
                digest.update(data);size+=len(data);window=overlap+data
                if canary_hash:
                    for index,pattern in enumerate(patterns):
                        for match in pattern.finditer(window):
                            value=match.group().decode('utf-16-le' if index else 'ascii')
                            found |= hashlib.sha256(value.encode('utf-8')).hexdigest()==canary_hash
                overlap=window[-128:]
        files.append(dict(path=str(path),size=size,sha256=digest.hexdigest()))
        if found:hits.append(str(path))
    result=dict(result='PASS' if not hits else 'CANARY_HIT',sealed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                files=files,canary_hash_checked=bool(canary_hash),canary_hit_paths=hits,
                authority='FH4 raw session evidence immutable; later reviewed readers may reread these bytes')
    create_json(root/'raw-evidence.json',result)
    return result


def outcome(leg,execution,protection,upload,reader,driver=None,crashes=(),raw=None):
    reasons=[];fatal=[]
    observed=execution.get('reason','')
    if protection.get('result')!='PASS' or any(cause in observed for cause in ('FB2 protected file changed','FD2 write to build outputs')):
        fatal.append('protected change or binding violation')
    if any(row.get('hits') for row in upload.get('logs',[])) or 'FH3 upload sign during session' in observed:
        fatal.append('upload sign')
    if (raw or {}).get('canary_hit_paths') or 'canary' in execution.get('reason','').lower():fatal.append('canary hit')
    deliberate=bool(execution.get('stopped_owned_processes'))
    crashed=bool(execution.get('launches') and execution.get('exit_code') not in (0,None) and not deliberate)
    if crashed:
        reasons.append('abnormal editor/game exit; FI4 runtime crash counted')
    if execution.get('hard_stop'):
        fatal.append('unauthorized build/launch hard STOP')
    if leg=='Smoke':
        present=bool(driver and driver.get('result')=='SMOKE_PASS' and driver.get('module_alive') and
                     driver.get('bindings') and all(row.get('present') for row in driver['bindings']))
        if not execution.get('launches') or execution.get('exit_code')!=0 or not present:
            fatal.append('smoke editor startup/exit/binding failure')
    if execution.get('result')!='PROCESS_PASS':reasons.append(execution.get('reason','session stopped'))
    if reader.get('result')!='PASS':reasons.append('reader FAIL: '+reader.get('reason','incomplete'))
    if upload.get('result')!='PASS':reasons.append('upload logs incomplete or failed')
    if crashes:reasons.append('new crash/ensure report folder after session')
    return dict(result='FAIL' if fatal or reasons else 'PASS',end_run=bool(fatal),run_stop_reasons=fatal,
                leg_failure_reasons=reasons,new_crash_report_folders=list(crashes),reader=reader,
                 upload=upload,deliberate_owned_stop=deliberate,crashed=crashed)


def ensure_frozen(path):
    data=json.loads(Path(path).read_text())
    for row in data['files']:
        file=Path(row['path'])
        if file.stat().st_size!=row['size'] or sha(file)!=row['sha256']:
            raise RuntimeError('FH4 raw evidence changed: '+str(file))
    return len(data['files'])
