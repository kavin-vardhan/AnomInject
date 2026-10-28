import datetime
import json
from pathlib import Path
import queue
import threading
import time

from idle_gate import collect, reasons
from fixture_wait import waiting_line


class SessionGate:
    def __init__(self,path,collector=collect,clock=time.monotonic):
        self.path=Path(path)
        self.collector=collector
        self.clock=clock
        self.results=queue.Queue()
        self.stop_event=threading.Event()
        self.thread=None
        self.last=None
        self.owned=()

    def __enter__(self):
        self.stream=self.path.open('x',encoding='utf-8')
        return self

    def start(self,pid):
        self.owned=(pid,)
        self.last=self.clock()
        self.thread=threading.Thread(target=self.collect_loop,daemon=True)
        self.thread.start()

    def collect_loop(self):
        while not self.stop_event.is_set():
            began=self.clock()
            try:
                sample=self.collector()
            except BaseException as error:
                sample=dict(reading_errors=dict(collector=type(error).__name__))
            self.results.put(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                                  collected=self.clock(),sample=sample))
            self.stop_event.wait(max(0,25-(self.clock()-began)))

    def check(self,owned):
        self.owned=tuple(owned)
        failures=[]
        while True:
            try:row=self.results.get_nowait()
            except queue.Empty:break
            self.last=row['collected']
            why=reasons(row['sample'],self.owned,profile='measured',require_launcher=True)
            row.update(owned_pids=list(self.owned),valid=not why,reasons=why)
            self.stream.write(json.dumps(row)+'\n');self.stream.flush()
            failures.extend(why)
        if self.last is not None and self.clock()-self.last>75:
            failures.append('session_gate_reading:stale')
        if failures:
            raise RuntimeError('Measured gate lost during session: '+waiting_line(sorted(set(failures)))+
                               ' The owned tree is stopped and this recipe remains spent.')

    def __exit__(self,kind,value,trace):
        self.stop_event.set()
        try:
            if self.thread:
                self.thread.join(timeout=45)
                if self.thread.is_alive() and kind is None:
                    raise RuntimeError('Session gate collector did not finish; the session cannot qualify.')
            if kind is None:
                self.check(self.owned)
        finally:
            self.stream.close()
