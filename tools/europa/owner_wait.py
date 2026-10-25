import json
import os
import time

from idle_gate import Window, collect

TIMEOUT_LINE = 'No quiet start within 45 minutes. No editor was started. Close heavy jobs, then double-click again to use the next attempt.'
CLEAR_LINE = 'Quiet-start checks passed three times.'


def waiting_line(result, sample):
    labels = []
    for reason in result.get('reasons', []):
        if reason == 'physical_memory':
            labels.append('free memory is below 6 GiB')
        elif reason == 'gpu':
            labels.append('GPU use is above 15%')
        elif reason.startswith('foreign_process:'):
            pid = reason.split(':')[1]
            labels.append('another Unreal or game program is running (PID '+pid+')')
        elif reason.startswith('disk:') and not reason.endswith(':unknown'):
            labels.append(reason.split(':')[1]+': free space is below its safety floor')
        elif reason == 'input_idle':
            labels.append('mouse or keyboard idle time is below five minutes')
        else:
            field = reason.removesuffix(':unknown')
            names = {'physical_memory': 'free memory', 'gpu': 'GPU use', 'processes': 'running programs', 'input_idle': 'mouse or keyboard idle time'}
            labels.append('cannot read '+names.get(field, field.replace('disk:', 'free space on ')))
    if not labels:
        labels.append('quiet-start checks '+str(result.get('consecutive', 0))+' of 3')
    return 'Waiting: '+'; '.join(labels)+'. Leave this window open.'


def wait_idle(output, collector=collect, clock=time.monotonic, sleep=time.sleep, emit=print):
    started = clock()
    window = Window(started, profile='owner-preview')
    next_poll = next_notice = started
    result, sample = None, None
    with output.open('x', encoding='utf-8') as stream:
        while True:
            now = clock()
            if now - started >= 2700:
                emit(TIMEOUT_LINE)
                row = dict(status='TIMEOUT', elapsed_seconds=now-started, launches=0)
                stream.write(json.dumps(row)+'\n')
                return row
            if now >= next_poll:
                sample = collector()
                now = clock()
                result = window.poll(now, sample, [os.getpid()])
                stream.write(json.dumps(dict(elapsed_seconds=now-started, sample=sample, result=result))+'\n')
                stream.flush()
                if result['status'] == 'NEEDS-DECISION':
                    continue
                if any(r.startswith('disk:') and not r.endswith(':unknown') for r in result['reasons']):
                    raise RuntimeError('A disk is below its safety floor. No editor was started.')
                if result['status'] == 'CLEAR':
                    emit(CLEAR_LINE)
                    return dict(status='CLEAR', elapsed_seconds=now-started, polls=window.consecutive)
                next_poll = now + 25
            if now >= next_notice:
                emit(waiting_line(result, sample))
                next_notice = started + (int((now-started)//60)+1)*60
            sleep(max(0, min(next_poll, next_notice, started+2700)-clock()))


class SyntheticClock:
    def __init__(self):
        self.value = 0.0

    def now(self):
        return self.value

    def sleep(self, seconds):
        self.value += seconds
