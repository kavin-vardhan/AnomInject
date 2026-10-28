import datetime
import json
from pathlib import Path
import time
from idle_gate import Window, collect

LABELS = dict(input_idle='mouse and keyboard idle for 300 seconds', physical_memory='6 GiB free physical memory',
              gpu='GPU use at or below 15 percent', processes='a readable running-program list',
              launcher='Epic Games Launcher already running')


def describe(reason):
    if reason.startswith('foreign_process:'):
        return 'foreign Unreal process PID '+reason.split(':')[1]+' to close'
    if reason.startswith('disk:'):
        return 'disk '+reason.split(':')[1]+': above its floor'+(' (reading unavailable)' if reason.endswith(':unknown') else '')
    name=reason.split(':')[0]
    return LABELS.get(name,reason)+(' (reading unavailable)' if reason.endswith(':unknown') else '')


def waiting_line(reasons):
    return 'Waiting: '+('; '.join(describe(r) for r in reasons) if reasons else 'three passing polls, 25 seconds apart')+'.'


def wait(path, owned=(), collector=collect, clock=time.monotonic, sleep=time.sleep, emit=print):
    started=clock()
    window=Window(started,profile='measured',require_launcher=True)
    next_message=started
    with Path(path).open('x',encoding='utf-8') as stream:
        while True:
            sample=collector()
            result=window.poll(clock(),sample,owned)
            row=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),elapsed=clock()-started,
                     sample=sample,result=result)
            stream.write(json.dumps(row)+'\n');stream.flush()
            if result['status']=='CLEAR':
                emit('Measured gate passed: three quiet polls; the launcher is already running.')
                return result
            if result['status']=='NEEDS-DECISION':
                emit('Stopped: no measured idle window within four hours. No editor was started.')
                return result
            if clock() >= next_message:
                emit(waiting_line(result['reasons']))
                next_message=clock()+60
            sleep(min(25,max(0,next_message-clock()),max(0,started+14400-clock())))
