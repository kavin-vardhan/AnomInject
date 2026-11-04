import datetime
import hashlib
import json
import os
from pathlib import Path
import shutil
import sys
import time
import traceback

JOB=Path('D:/IA_Europa/Receipts/09445r')
TOOLS=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa')
sys.path.insert(0,str(TOOLS))
os.environ['EUROPA_JOB_CONFIG']=str(JOB/'paris-build-job.json')
import a5_build
from window09445r import require_launch,require_runtime
require_launch(600)
from build_memory import MemoryStopped,NoMemoryWindow

target=sys.argv[1]
assert target in ('StylizedParisStreetEditor','StylizedParisStreet')
prefix='editor' if target.endswith('Editor') else 'game'
deadline=datetime.datetime.now(datetime.timezone.utc)+datetime.timedelta(hours=3)

def launched():
    rows=[]
    for path in (JOB/'build-processes').glob(prefix+'-*/child-*.json'):
        row=json.loads(path.read_text())
        if row.get('pid') and Path(row.get('argv',[''])[0]).name.casefold()=='unrealbuildtool.exe':rows.append(row)
    return rows

def quarantine_partial(label):
    host=Path('D:/IA_Europa/StylizedParisStreet').resolve(strict=True)
    roots=[host/'Binaries',host/'Intermediate/Build',host/'Plugins/AnomalyInjector/Binaries',host/'Plugins/AnomalyInjector/Intermediate']
    report=json.loads((JOB/'builds'/label/'result.json').read_text())
    plans=[]
    for row in report['changed_outputs']:
        path=Path(row['path'])
        if row['after'] is None or path.suffix.lower() not in ('.obj','.pch','.res','.dll','.exe','.lib','.exp','.pdb','.modules','.target'):continue
        if path.resolve(strict=True)!=path or not any(path.is_relative_to(root) for root in roots):raise RuntimeError('Partial output outside owned build roots')
        stat=path.stat()
        if [stat.st_size,stat.st_mtime_ns]!=row['after']:raise RuntimeError('Partial output changed after build stopped')
        destination=JOB/'partial-build-quarantine'/label/path.relative_to(host)
        if not destination.resolve().is_relative_to(JOB.resolve()):raise RuntimeError('Quarantine escaped receipt root')
        plans.append(dict(source=str(path),destination=str(destination),size=stat.st_size,sha256=hashlib.sha256(path.read_bytes()).hexdigest()))
    receipt=JOB/'builds'/label/'partial-quarantine.json'
    receipt.write_text(json.dumps(dict(authority='094-33 FM1 retry; retain incomplete outputs',files=plans),indent=2))
    for row in plans:
        source,destination=Path(row['source']),Path(row['destination'])
        destination.parent.mkdir(parents=True,exist_ok=True)
        if destination.exists():raise RuntimeError('Quarantine destination exists')
        source.rename(destination)
        assert hashlib.sha256(destination.read_bytes()).hexdigest()==row['sha256']
    receipt.with_name('partial-quarantine-result.json').write_text(json.dumps(dict(result='PASS',files=plans),indent=2))

while len(launched())<3 and datetime.datetime.now(datetime.timezone.utc)<deadline:
    while True:
        free={d:shutil.disk_usage(d+':/').free for d in ('C','D','E')}
        if all(free[d]>=minimum*1024**3 for d,minimum in (('C',10),('D',50),('E',50))):break
        if datetime.datetime.now(datetime.timezone.utc)>=deadline:raise SystemExit('WINDOW_ENDED before build')
        with (JOB/'build-disk-admission.jsonl').open('a') as stream:stream.write(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),free_bytes=free,status='WAIT'))+'\n')
        print('Waiting on build disk admission.',flush=True)
        time.sleep(20)
    ordinal=1+len(list((JOB/'builds').glob(prefix+'-*')))
    label=prefix+'-%02d'%ordinal
    operation=JOB/'build-processes'/label
    operation.mkdir(parents=True,exist_ok=False)
    os.environ['EUROPA_OPERATION_DIR']=str(operation)
    print('FM1 '+target+': next actual build attempt '+str(len(launched())+1)+' of 3; '+label,flush=True)
    try:
        require_launch(600)
        a5_build.execute('425',target,'Development',JOB/'builds'/label,allow_up_to_date=True)
        break
    except (MemoryStopped,NoMemoryWindow) as error:
        if isinstance(error,MemoryStopped):quarantine_partial(label)
        (operation/'retry.json').write_text(json.dumps(dict(reason=str(error),actual_attempts=len(launched()),retry='memory admission only',utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2))
        print('FM1 owned build stopped/wait expired; evidence kept; waiting before retry.',flush=True)
    except RuntimeError as error:
        (operation/'wrapper-error.json').write_text(json.dumps(dict(error=str(error),traceback=traceback.format_exc(),utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2))
        created=[json.loads(p.read_text()) for p in operation.glob('child-*.json')]
        if str(error).startswith('ED1 floor/headroom STOP:') and not any(r.get('pid') and Path(r['argv'][0]).name.casefold()=='unrealbuildtool.exe' for r in created):
            print('Final disk admission changed during memory wait; no UBT created; returning to admission.',flush=True)
            continue
        print(traceback.format_exc(),flush=True)
        raise
else:
    raise SystemExit('FM1 attempt limit or real-session window reached')
