import datetime
import json
from pathlib import Path
import time

import idle_gate
from fixture_policy import WINDOW_END
from fixture_wait import waiting_line, STARTS
from disk_pause import reading, recover


def wait(path, collector=idle_gate.collect, sleeper=time.sleep, now=None):
    now=now or (lambda:datetime.datetime.now(datetime.timezone.utc))
    deadline=datetime.datetime.fromisoformat(WINDOW_END)
    previous=None
    while now()<deadline:
        disks=reading()
        low=[d for d,v in disks.items() if v['free']<v['floor']]
        if low:
            recover(low,Path(path).with_name(Path(path).stem+'-pause-'+str(time.time_ns())+'.json'),
                dict(result='PAUSE',reason='Disk below floor before session admission',before=disks,
                     stopped_owned_processes=[],authority='EU2 / FA6'),
                wall_remaining=max(0,(deadline-now()).total_seconds()))
            continue
        sample=collector()
        blocked=idle_gate.reasons(sample,profile='measured',require_launcher=True)
        if sample.get('idle_seconds',0)<600:
            blocked.append('FA5_input_idle_600')
        for drive,start in STARTS.items():
            free=(sample.get('volumes',{}).get(drive) or {}).get('free_bytes')
            if isinstance(free,(int,float)) and free<start:
                blocked.append('disk:'+drive+':start')
        row=dict(time_utc=now().isoformat(),sample=sample,reasons=blocked,result='WAIT' if blocked else 'PASS')
        with Path(path).open('a',encoding='utf-8') as stream:
            stream.write(json.dumps(row)+'\n')
        if not blocked:
            print('Idle pre-check passed: ten minutes idle and all measured conditions hold.',flush=True)
            return row
        message='Waiting: ten minutes of human idle; '+waiting_line([r for r in blocked if r!='FA5_input_idle_600']).removeprefix('Waiting: ')
        if message!=previous:
            print(message,flush=True)
            previous=message
        sleeper(min(60,max(0,(deadline-now()).total_seconds())))
    raise RuntimeError('FC7 session-start window ended. No editor was started.')
