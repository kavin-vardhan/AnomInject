import ctypes
from ctypes import wintypes
import datetime
import json
from pathlib import Path
import queue
import re
import threading
import time

import idle_gate
from fixture_session_gate import limits_sample, numeric, volume_reading, GIB
from fixture_children import BuildOutputWatch


def qpc():
    return time.perf_counter()+16777216.0


def stop_reasons(sample):
    found=[]
    if numeric(sample.get('free_commit_bytes')) and sample['free_commit_bytes'] < 6*GIB:
        found.append('FB3 free commit below 6 GiB')
    for drive, floor in idle_gate.FLOORS.items():
        volume=volume_reading(sample,drive)
        if volume.get('resolved_drive') == drive and numeric(volume.get('free_bytes')) and volume['free_bytes'] < floor:
            found.append('disk '+drive+' below floor')
    return found


def validity(observations, start, end):
    rows=[r for r in observations if r['ended_qpc'] >= start and r['started_qpc'] <= end]
    reasons=[]
    if not rows or rows[0]['started_qpc'] > start or rows[-1]['ended_qpc'] < end:
        reasons.append('observation coverage missing')
    if any(b['started_qpc']-a['ended_qpc']>0.25 for a,b in zip(rows,rows[1:])):
        reasons.append('observation gap above 250 ms')
    for row in rows:
        if row.get('error') or row.get('foreign_gpu_percent') is None:
            reasons.append('unreadable event observation')
        elif row['foreign_gpu_percent'] > 15:
            reasons.append('foreign GPU above 15 percent')
        last_input=row.get('last_input_qpc')
        if last_input is None or start <= last_input <= end:
            reasons.append('human input or unreadable input')
    return dict(valid=not reasons,reasons=sorted(set(reasons)),observations=len(rows))


def close_window(pid):
    user=ctypes.WinDLL('user32',use_last_error=True)
    callback_type=ctypes.WINFUNCTYPE(wintypes.BOOL,wintypes.HWND,wintypes.LPARAM)
    user.GetWindowThreadProcessId.argtypes=[wintypes.HWND,ctypes.POINTER(wintypes.DWORD)]
    user.PostMessageW.argtypes=[wintypes.HWND,wintypes.UINT,wintypes.WPARAM,wintypes.LPARAM]
    user.EnumWindows.argtypes=[callback_type,wintypes.LPARAM]
    user.IsWindowVisible.argtypes=[wintypes.HWND]
    windows=[]
    @callback_type
    def visit(hwnd, extra):
        owner=wintypes.DWORD()
        user.GetWindowThreadProcessId(hwnd,ctypes.byref(owner))
        if owner.value==pid and user.IsWindowVisible(hwnd):
            windows.append(int(hwnd))
            user.PostMessageW(hwnd,0x0010,0,0)
        return True
    user.EnumWindows(visit,0)
    if not windows:
        raise RuntimeError('No owned window for graceful completion')
    return windows


class Monitor:
    def __init__(self,root,host,leg,protected,game=True,synthetic=False):
        self.root,self.host,self.leg=Path(root),Path(host),leg
        self.game,self.synthetic=game,synthetic
        self.owned=()
        self.pid=None
        self.last_limits=0
        self.unreadable={}
        self.observations=[]
        self.queue=queue.Queue()
        self.done=threading.Event()
        self.thread=None
        self.closed=False
        self.probe_offset=0
        self.tail=''
        self.rows=[]
        self.watch=BuildOutputWatch(host,self.root/'build-watch.jsonl')
        self.protected_paths=[Path(r['path']) for key in ('content','config','descriptors') for r in protected[key]]
        self.before=self.metadata()
        self.crashes=self.crash_folders()

    def metadata(self):
        result={str(p):(p.stat().st_size,p.stat().st_mtime_ns) if p.is_file() else None for p in self.protected_paths}
        for base in (self.host/'Content',self.host/'Plugins/AnomalyInjector/Content',self.host/'Config',self.host/'Saved/Config'):
            if base.exists():
                for path in base.rglob('*'):
                    if path.is_file() and (base!=self.host/'Saved/Config' or path.suffix.lower()=='.ini'):
                        result.setdefault(str(path),(path.stat().st_size,path.stat().st_mtime_ns))
        return result

    def crash_folders(self):
        root=self.host/'Saved/Crashes'
        return sorted(str(p) for p in root.rglob('*') if p.is_dir()) if root.exists() else []

    def record(self,row):
        with (self.root/'monitor.jsonl').open('a',encoding='utf-8') as stream:
            stream.write(json.dumps(dict(row,utc=datetime.datetime.now(datetime.timezone.utc).isoformat()))+'\n')

    def start(self,pid):
        self.pid=pid
        self.owned=(pid,)
        if not self.synthetic:
            self.thread=threading.Thread(target=self.observe,daemon=True)
            self.thread.start()

    def observe(self):
        while not self.done.is_set():
            began=qpc()
            row=dict(started_qpc=began)
            try:
                gpu=idle_gate.powershell_json(r"$r=@((Get-Counter '\GPU Engine(*)\Utilization Percentage' -ErrorAction Stop).CounterSamples | ForEach-Object { @{instance=$_.InstanceName;value=$_.CookedValue} }); ConvertTo-Json -InputObject $r -Compress")
                if not isinstance(gpu,list) or not gpu:
                    raise RuntimeError('GPU samples unavailable')
                foreign=0.0
                pids=[]
                for counter in gpu:
                    match=re.search(r'pid_(\d+)_',counter['instance'])
                    if not match or not numeric(counter['value']):
                        raise RuntimeError('GPU counter attribution unavailable')
                    pid=int(match.group(1))
                    if pid not in self.owned:
                        foreign+=counter['value']
                        if counter['value']>0:pids.append(pid)
                ended=qpc()
                idle=idle_gate.read_idle()
                row.update(ended_qpc=ended,last_input_qpc=ended-idle,foreign_gpu_percent=min(100,foreign),
                           foreign_gpu_pids=sorted(set(pids)),owned_pids=list(self.owned),
                           free_physical_bytes=idle_gate.read_memory())
            except Exception as error:
                row.update(ended_qpc=qpc(),error=type(error).__name__)
            self.queue.put(row)

    def read_probe(self):
        path=self.root/'session/probe/probe.jsonl'
        if not path.exists():return
        with path.open('r',encoding='utf-8') as stream:
            stream.seek(self.probe_offset)
            text=self.tail+stream.read()
            self.probe_offset=stream.tell()
        lines=text.split('\n')
        self.tail=lines.pop()
        self.rows.extend(json.loads(line) for line in lines if line)

    def check(self,owned):
        self.owned=tuple(owned)
        while True:
            try:row=self.queue.get_nowait()
            except queue.Empty:break
            self.observations.append(row)
            self.record(dict(kind='event_observation',**row))
        now=time.monotonic()
        if now-self.last_limits >= 1:
            self.last_limits=now
            sample=limits_sample()
            failures=stop_reasons(sample)
            values={'commit':sample.get('free_commit_bytes')}
            for drive in idle_gate.FLOORS:
                volume=volume_reading(sample,drive)
                values[drive]=volume.get('free_bytes') if volume.get('resolved_drive')==drive else None
            for key,value in values.items():
                if numeric(value):self.unreadable.pop(key,None)
                else:
                    self.unreadable.setdefault(key,now)
                    if now-self.unreadable[key] >= 60:failures.append(key+' unreadable for 60 seconds')
            self.record(dict(kind='acting_limits',sample=sample,failures=failures))
            if failures:raise RuntimeError('; '.join(failures))
            self.watch.check()
            current=self.metadata()
            if current!=self.before:
                changed=[p for p in set(current)|set(self.before) if current.get(p)!=self.before.get(p)]
                self.record(dict(kind='protected_write',paths=changed))
                raise RuntimeError('FB2 protected file changed during session')
            if self.crash_folders()!=self.crashes:
                raise RuntimeError('New crash report folder; no further session')
        self.read_probe()
        if any(row.get('kind')=='driver_failure' for row in self.rows):
            raise RuntimeError('Native runtime driver failed')
        complete=(any(r.get('kind')=='world_initialized' and r.get('world_name','').endswith('EA_Travel') for r in self.rows)
                  if self.leg in ('WorldsGame','Admission') else any(r.get('kind')=='script_complete' for r in self.rows))
        if complete and not hasattr(self,'complete_at'):
            self.complete_at=now
        if self.game and complete and not self.closed and now-self.complete_at>=2:
            self.record(dict(kind='graceful_close',pid=self.pid,windows=close_window(self.pid),reason='native leg complete'))
            self.closed=True

    def close(self):
        self.done.set()
        if self.thread:self.thread.join(timeout=5)
        while True:
            try:row=self.queue.get_nowait()
            except queue.Empty:break
            self.observations.append(row);self.record(dict(kind='event_observation',**row))
        self.record(dict(kind='monitor_closed',collector_live=bool(self.thread and self.thread.is_alive())))
