import ctypes
from ctypes import wintypes
import datetime as dt
import json
from pathlib import Path
import time

GIB = 1024 ** 3

class NoMemoryWindow(RuntimeError):
    pass

class MemoryStopped(RuntimeError):
    pass

class Memory(ctypes.Structure):
    _fields_ = [('length', wintypes.DWORD), ('load', wintypes.DWORD)] + [(n, ctypes.c_ulonglong) for n in
        ('total_physical', 'free_physical', 'total_commit', 'free_commit', 'total_virtual', 'free_virtual', 'free_extended')]

def sample():
    value = Memory()
    value.length = ctypes.sizeof(value)
    if not ctypes.windll.kernel32.GlobalMemoryStatusEx(ctypes.byref(value)):
        raise ctypes.WinError()
    return {'free_physical': value.free_physical, 'free_commit': value.free_commit, 'load': value.load,
        'total_physical': value.total_physical, 'total_commit': value.total_commit}

def policy():
    from job_scope import load
    return load()['memory']

def admission(reading, limits=None):
    limits = limits or policy()
    physical = limits['start_physical_GiB']
    return (physical is None or reading['free_physical'] >= physical * GIB) and reading['free_commit'] >= limits['start_commit_GiB'] * GIB


def required_readings(start):
    from job_scope import load
    return 1 if load().get('incremental_memory_authority') == '094-31-ruling FM1' else (3 if start else 2)

def pressure(reading, limits=None):
    limits = limits or policy()
    physical = limits['stop_physical_GiB']
    return (physical is not None and reading['free_physical'] < physical * GIB) or reading['free_commit'] < limits['stop_commit_GiB'] * GIB

def event(path, value):
    value = dict(value, time_utc=dt.datetime.now(dt.timezone.utc).isoformat())
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('a', encoding='utf-8') as output:
        output.write(json.dumps(value) + '\n')
    if value['event'] != 'running':
        print(json.dumps({'EF1': value}), flush=True)

def wait_for_window(log, sampler=sample, sleeper=time.sleep, clock=time.monotonic, limit=4 * 3600):
    from job_scope import load
    if Path(load()['job_root']).name in ('09442c','09443a','09443r'):
        limit=min(limit,3*3600)
    start, consecutive = clock(), 0
    required = required_readings(True)
    while True:
        reading = sampler()
        consecutive = consecutive + 1 if admission(reading) else 0
        event(log, {'event': 'admitted' if consecutive >= required else 'waiting', 'reading': reading,
            'consecutive': consecutive, 'wait_seconds': clock() - start, 'policy': policy()})
        if consecutive >= required:
            return reading
        if clock() - start >= limit:
            event(log, {'event': 'timeout', 'wait_seconds': clock() - start})
            raise NoMemoryWindow('no memory window')
        sleeper(min(20, max(0, limit - (clock() - start))))

def monitor_sample(reading, consecutive, stop, log):
    consecutive = consecutive + 1 if pressure(reading) else 0
    event(log, {'event': 'running', 'reading': reading, 'consecutive_low': consecutive})
    if consecutive >= required_readings(False):
        stopped = stop()
        event(log, {'event': 'stopped_owned_tree', 'reading': reading, 'processes': stopped})
        raise MemoryStopped('Stopped owned UBT tree on ruled memory pressure; preserve evidence and retry only within the job attempt limit')
    return consecutive

class OwnedTree:
    def __init__(self, root_pid):
        self.kernel = ctypes.WinDLL('kernel32', use_last_error=True)
        k = self.kernel
        k.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
        k.OpenProcess.restype = wintypes.HANDLE
        k.GetProcessTimes.argtypes = [wintypes.HANDLE] + [ctypes.POINTER(wintypes.FILETIME)] * 4
        k.GetProcessTimes.restype = wintypes.BOOL
        k.QueryFullProcessImageNameW.argtypes = [wintypes.HANDLE, wintypes.DWORD, wintypes.LPWSTR, ctypes.POINTER(wintypes.DWORD)]
        k.QueryFullProcessImageNameW.restype = wintypes.BOOL
        k.TerminateProcess.argtypes = [wintypes.HANDLE, wintypes.UINT]
        k.WaitForSingleObject.argtypes = [wintypes.HANDLE, wintypes.DWORD]
        k.CloseHandle.argtypes = [wintypes.HANDLE]
        self.root_pid = root_pid
        self.known = {}
        self.excluded = []
        self.add(root_pid, None, 0)
        if root_pid not in self.known:
            raise RuntimeError('Cannot retain own UBT identity for EF1')

    def times(self, handle):
        values = [wintypes.FILETIME() for _ in range(4)]
        if not self.kernel.GetProcessTimes(handle, *[ctypes.byref(v) for v in values]):
            raise ctypes.WinError(ctypes.get_last_error())
        return tuple(v.dwLowDateTime + (v.dwHighDateTime << 32) for v in values[:2])

    def add(self, pid, parent, depth):
        handle = self.kernel.OpenProcess(0x1000 | 0x100000 | 0x1, False, pid)
        if not handle:
            return False
        keep = False
        try:
            created, exited = self.times(handle)
            length = wintypes.DWORD(32768)
            buffer = ctypes.create_unicode_buffer(length.value)
            if not self.kernel.QueryFullProcessImageNameW(handle, 0, buffer, ctypes.byref(length)):
                return False
            if Path(buffer.value).name.casefold() == 'mspdbsrv.exe':
                self.excluded.append({'pid': pid, 'image': buffer.value, 'reason': 'EF1 never stops mspdbsrv'})
                return False
            if parent is not None:
                pcreated, pexited = self.times(parent['handle'])
                if created < pcreated or pexited and created > pexited:
                    return False
            old = self.known.get(pid)
            if old:
                return False
            self.known[pid] = {'pid': pid, 'parent_pid': parent['pid'] if parent else None, 'depth': depth,
                'created_filetime': created, 'image': buffer.value, 'handle': handle}
            keep = True
            return True
        finally:
            if not keep:
                self.kernel.CloseHandle(handle)

    def poll(self):
        from compile_children import snapshot
        rows = snapshot()
        progress = True
        while progress:
            progress = False
            for row in rows:
                parent = self.known.get(row['parent_pid'])
                if parent and row['pid'] not in self.known and row['exe'].casefold() != 'mspdbsrv.exe':
                    progress = self.add(row['pid'], parent, parent['depth'] + 1) or progress

    def command_line(self, row):
        class UnicodeString(ctypes.Structure):
            _fields_ = [('length',wintypes.USHORT),('maximum',wintypes.USHORT),('buffer',ctypes.c_void_p)]
        query=ctypes.WinDLL('ntdll').NtQueryInformationProcess
        query.argtypes=[wintypes.HANDLE,wintypes.ULONG,ctypes.c_void_p,wintypes.ULONG,ctypes.POINTER(wintypes.ULONG)]
        query.restype=ctypes.c_long
        length=wintypes.ULONG()
        query(row['handle'],60,None,0,ctypes.byref(length))
        if not length.value or length.value>1024*1024:
            raise RuntimeError('Owned process command line length unavailable')
        data=ctypes.create_string_buffer(length.value)
        status=query(row['handle'],60,data,len(data),ctypes.byref(length))
        if status<0:
            raise RuntimeError('Owned process command line query status '+hex(status & 0xffffffff))
        value=UnicodeString.from_buffer(data)
        if value.length%2 or not value.buffer:
            raise RuntimeError('Invalid owned process command line')
        return ctypes.wstring_at(value.buffer,value.length//2)

    def stop(self):
        self.poll()
        stopped = []
        for row in sorted(self.known.values(), key=lambda v: v['depth']):
            if self.kernel.WaitForSingleObject(row['handle'], 0) == 0:
                continue
            if not self.kernel.TerminateProcess(row['handle'], 125):
                if self.kernel.WaitForSingleObject(row['handle'], 0) != 0:
                    raise ctypes.WinError(ctypes.get_last_error())
            stopped.append({k: v for k, v in row.items() if k != 'handle'})
        for row in self.known.values():
            self.kernel.WaitForSingleObject(row['handle'], 10000)
        return stopped

    def receipt(self):
        return {'root_pid': self.root_pid, 'owned': [{k: v for k, v in row.items() if k != 'handle'} for row in self.known.values()],
            'excluded': self.excluded, 'identity': 'retained process handle plus creation time and parent lifetime; mspdbsrv excluded'}

    def close(self):
        for row in self.known.values():
            self.kernel.CloseHandle(row['handle'])
