import datetime
import json
from pathlib import Path
import time

import idle_gate
from fixture_session_gate import GIB,numeric,volume_reading

START_GIB={'C':13,'D':25,'E':15}
FLOOR_GIB={'C':10,'D':15,'E':10}


def reasons(sample,idle_seconds=120,memory=(8,4)):
    if idle_seconds not in (120,300):raise ValueError('Unruled idle start line')
    if memory not in ((8,4),(12,6)):raise ValueError('Unruled memory start line')
    found=[]
    for key,minimum in (('idle_seconds',idle_seconds),('free_memory_bytes',memory[1]*GIB),('free_commit_bytes',memory[0]*GIB)):
        if not numeric(sample.get(key)) or sample[key]<minimum:found.append(key)
    for drive,minimum in START_GIB.items():
        volume=volume_reading(sample,drive)
        if volume.get('resolved_drive')!=drive or not numeric(volume.get('free_bytes')) or volume['free_bytes']<minimum*GIB:
            found.append('disk '+drive)
    return found


def wait(path,deadline,collector=idle_gate.collect,sleep=time.sleep,now=None,idle_seconds=120,memory=(8,4)):
    now=now or (lambda:datetime.datetime.now(datetime.timezone.utc))
    end=datetime.datetime.fromisoformat(deadline)
    began=time.monotonic();polls=0
    with Path(path).open('x',encoding='utf-8') as stream:
        while now()<end:
            sample=collector();polls+=1
            failures=reasons(sample,idle_seconds,memory)
            result=dict(status='WAIT' if failures else 'CLEAR',reasons=failures,polls=polls,
                elapsed_seconds=time.monotonic()-began,sample=sample,utc=now().isoformat(),
                authority='FV5; FK1; FJ1 one reading; GPU/process/input coverage recorded only',idle_start_seconds=idle_seconds,memory_start_gib=dict(commit=memory[0],physical=memory[1]))
            stream.write(json.dumps(result)+'\n');stream.flush()
            if not failures:return result
            sleep(min(30,max(0,(end-now()).total_seconds())))
        result=dict(status='WINDOW_ENDED',polls=polls,elapsed_seconds=time.monotonic()-began,utc=now().isoformat())
        stream.write(json.dumps(result)+'\n')
        return result
