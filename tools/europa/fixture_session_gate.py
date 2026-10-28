import datetime
import json
from pathlib import Path
import queue
import threading
import time

from build_memory import sample as memory_sample
from idle_gate import collect, reasons, read_volume, FLOORS, reading_error

GIB=1024**3


def limits_sample():
    result=dict(free_commit_bytes=None,volumes={},reading_errors={})
    try:
        memory=memory_sample()
        result.update(free_commit_bytes=memory['free_commit'],free_physical_bytes=memory['free_physical'])
    except Exception as error:
        result['reading_errors']['commit']=reading_error(error)
    for drive in FLOORS:
        try:result['volumes'][drive]=read_volume(drive)
        except Exception as error:result['reading_errors']['disk:'+drive]=reading_error(error)
    return result


def stop_reasons(sample):
    found=[]
    commit=sample.get('free_commit_bytes')
    if isinstance(commit,(int,float)) and commit<6*GIB:
        found.append('free commit below 6 GiB')
    for drive,minimum in FLOORS.items():
        volume=sample.get('volumes',{}).get(drive,{})
        if isinstance(volume.get('free_bytes'),(int,float)) and volume['free_bytes']<minimum:
            found.append('disk '+drive+': below its '+str(minimum//GIB)+' GiB floor')
    return found


class SessionGate:
    def __init__(self,path,collector=collect,clock=time.monotonic,limits_reader=limits_sample):
        self.path=Path(path)
        self.collector=collector
        self.clock=clock
        self.limits_reader=limits_reader
        self.results=queue.Queue()
        self.stop_event=threading.Event()
        self.thread=None
        self.last=None
        self.owned=()
        self.last_limits=float('-inf')

    def __enter__(self):
        self.stream=self.path.open('x',encoding='utf-8')
        return self

    def write(self,row):
        row=dict(row,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),authority='094-22-ruling EX2')
        self.stream.write(json.dumps(row)+'\n');self.stream.flush()

    def start(self,pid):
        self.owned=(pid,)
        self.last=self.clock()
        self.thread=threading.Thread(target=self.collect_loop,daemon=True)
        self.thread.start()

    def collect_loop(self):
        while not self.stop_event.is_set():
            began=self.clock()
            try:sample=self.collector()
            except BaseException as error:sample=dict(reading_errors=dict(collector=type(error).__name__))
            self.results.put(dict(collected=self.clock(),sample=sample))
            self.stop_event.wait(max(0,25-(self.clock()-began)))

    def check(self,owned):
        self.owned=tuple(owned)
        if self.clock()-self.last_limits>=1:
            self.last_limits=self.clock()
            limits=self.limits_reader()
            failures=stop_reasons(limits)
            self.write(dict(kind='acting_limits',sample=limits,reasons=failures,owned_pids=list(self.owned),
                action='stop_owned_tree' if failures else 'record',deadline='independent 1800-second runtime guard'))
            if failures:
                raise RuntimeError('Authoring session stopped: '+'; '.join(failures)+'. Only the owned tree is stopped; this recipe remains spent.')
        while True:
            try:row=self.results.get_nowait()
            except queue.Empty:break
            self.last=row['collected']
            row.update(kind='observe_only',owned_pids=list(self.owned),
                reasons=reasons(row['sample'],self.owned,profile='measured',require_launcher=True),
                action='record_only',human_input_gpu_physical_memory_foreign_processes_do_not_stop=True)
            self.write(row)

    def __exit__(self,kind,value,trace):
        self.stop_event.set()
        try:
            if self.thread:self.thread.join(timeout=.2)
            self.write(dict(kind='collector_close',collector_still_running=bool(self.thread and self.thread.is_alive()),
                            action='record_only',automatic_retry=False))
        finally:self.stream.close()
