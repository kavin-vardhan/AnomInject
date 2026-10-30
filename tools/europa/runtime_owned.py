import ctypes
from ctypes import wintypes
import datetime
import time

from build_memory import OwnedTree
from fixture_children import arguments


def monitor_matches(command, owned_pid):
    if not command:
        return False
    values=[a.split('=',1)[1] for a in arguments(command)[1:] if a.lower().startswith('-monitor=')]
    return values==[str(owned_pid)]


def environment_names(pid, created_filetime):
    k=ctypes.WinDLL('kernel32',use_last_error=True)
    k.OpenProcess.argtypes=[wintypes.DWORD,wintypes.BOOL,wintypes.DWORD]
    k.OpenProcess.restype=wintypes.HANDLE
    k.CloseHandle.argtypes=[wintypes.HANDLE]
    k.GetProcessTimes.argtypes=[wintypes.HANDLE]+[ctypes.POINTER(wintypes.FILETIME)]*4
    k.IsWow64Process.argtypes=[wintypes.HANDLE,ctypes.POINTER(wintypes.BOOL)]
    k.ReadProcessMemory.argtypes=[wintypes.HANDLE,ctypes.c_void_p,ctypes.c_void_p,ctypes.c_size_t,ctypes.POINTER(ctypes.c_size_t)]
    handle=k.OpenProcess(0x0400|0x0010,False,pid)
    if not handle:
        return dict(result='not readable',error='OpenProcess',winerror=ctypes.get_last_error())
    try:
        times=[wintypes.FILETIME() for _ in range(4)]
        if not k.GetProcessTimes(handle,*[ctypes.byref(v) for v in times]):raise OSError('GetProcessTimes')
        if times[0].dwLowDateTime+(times[0].dwHighDateTime<<32)!=created_filetime:raise OSError('PID identity changed')
        wow=wintypes.BOOL()
        if ctypes.sizeof(ctypes.c_void_p)!=8 or not k.IsWow64Process(handle,ctypes.byref(wow)) or wow.value:
            raise OSError('Native x64 environment reader only')
        query=ctypes.WinDLL('ntdll').NtQueryInformationProcess
        query.argtypes=[wintypes.HANDLE,wintypes.ULONG,ctypes.c_void_p,wintypes.ULONG,ctypes.POINTER(wintypes.ULONG)]
        query.restype=ctypes.c_long
        basic=(ctypes.c_void_p*6)();length=wintypes.ULONG()
        if query(handle,0,basic,ctypes.sizeof(basic),ctypes.byref(length))<0:raise OSError('ProcessBasicInformation')
        def read(address,size):
            buffer=ctypes.create_string_buffer(size);got=ctypes.c_size_t()
            if not k.ReadProcessMemory(handle,address,buffer,size,ctypes.byref(got)) or got.value!=size:
                raise OSError('ReadProcessMemory')
            return buffer.raw
        parameters=int.from_bytes(read(basic[1]+0x20,8),'little')
        address=int.from_bytes(read(parameters+0x80,8),'little')
        raw=bytearray()
        while len(raw)<1024*1024:
            size=min(4096-(address%4096),1024*1024-len(raw))
            raw.extend(read(address,size));address+=size
            end=next((i for i in range(0,len(raw)-3,2) if raw[i:i+4]==b'\0\0\0\0'),None)
            if end is not None:
                names=[]
                for value in bytes(raw[:end]).decode('utf-16-le').split('\0'):
                    if not value:continue
                    offset=value.find('=',1 if value.startswith('=') else 0)
                    if offset<1:raise OSError('Malformed environment entry')
                    names.append(value[:offset])
                names=sorted(set(names),key=str.casefold)
                return dict(result='readable',names=names,canary_name_present=any(n.upper()=='IA_CANARY_TOKEN' for n in names),values_recorded=False)
        raise OSError('Environment exceeds bounded read')
    except (OSError,ValueError,UnicodeError) as error:
        return dict(result='not readable',error=str(error),values_recorded=False)
    finally:
        k.CloseHandle(handle)


class RuntimeTree(OwnedTree):
    def __init__(self,pid):
        super().__init__(pid)
        self.adoption_events=[]
        self.monitor_stops=[]

    def adopt(self,item):
        pid=item['pid']
        if pid==self.root_pid:return
        existing=self.known.get(pid)
        if existing and existing.get('monitor_of'):return
        handle=existing['handle'] if existing else self.kernel.OpenProcess(0x1000|0x100000,False,pid)
        if not handle:return
        try:
            created,exited=self.times(handle)
            if exited or created<self.known[self.root_pid]['created_filetime']:return
            command=self.command_line(dict(handle=handle))
            if not monitor_matches(command,self.root_pid):return
            if not existing:
                if not self.add(pid,None,-1):return
                existing=self.known[pid]
                if existing['created_filetime']!=created:
                    self.kernel.CloseHandle(existing['handle']);del self.known[pid]
                    return
            existing.update(parent_pid=item['parent_pid'],depth=-1,monitor_of=self.root_pid,command_line=command)
            existing['started_utc']=datetime.datetime.fromtimestamp((created-116444736000000000)/10000000,datetime.timezone.utc).isoformat()
            existing['environment']=environment_names(pid,created)
            event={k:v for k,v in existing.items() if k!='handle'}
            event.update(adopted_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),authority='FH5 exact -MONITOR PID match')
            self.adoption_events.append(event)
        except (OSError,RuntimeError):
            return
        finally:
            if not any(r['handle']==handle for r in self.known.values()):self.kernel.CloseHandle(handle)

    def poll(self):
        super().poll()
        from compile_children import snapshot
        for item in snapshot():self.adopt(item)
        if any(r.get('environment',{}).get('canary_name_present') for r in self.known.values()):
            raise RuntimeError('FH3 canary variable found in adopted monitor environment')

    def stop_monitors(self,reason):
        stopped=[]
        for row in self.known.values():
            if not row.get('monitor_of') or self.kernel.WaitForSingleObject(row['handle'],0)==0:continue
            if not self.kernel.TerminateProcess(row['handle'],125) and self.kernel.WaitForSingleObject(row['handle'],0)!=0:
                raise ctypes.WinError(ctypes.get_last_error())
            self.kernel.WaitForSingleObject(row['handle'],10000)
            record={k:v for k,v in row.items() if k!='handle'}
            record.update(stop_reason=reason,stopped_utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
            stopped.append(record)
        self.monitor_stops.extend(stopped)
        return stopped

    def stop(self):
        # Retained handles survive reparenting and PID reuse; monitor stops precede the editor.
        try:self.poll()
        except RuntimeError:pass
        monitors=self.stop_monitors('owned-tree stop: monitor before editor')
        stopped=[]
        for row in sorted(self.known.values(),key=lambda r:r['depth']):
            if self.kernel.WaitForSingleObject(row['handle'],0)==0:continue
            if not self.kernel.TerminateProcess(row['handle'],125) and self.kernel.WaitForSingleObject(row['handle'],0)!=0:
                raise ctypes.WinError(ctypes.get_last_error())
            stopped.append({k:v for k,v in row.items() if k!='handle'})
        for row in self.known.values():self.kernel.WaitForSingleObject(row['handle'],10000)
        return monitors+stopped

    def receipt(self):
        data=super().receipt()
        data.update(adopted_monitors=self.adoption_events,monitor_stops=self.monitor_stops,
                    monitor_authority='Exact -MONITOR=<root PID>, retained handle, creation time; never image name')
        return data
