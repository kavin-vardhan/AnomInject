import ctypes
from ctypes import wintypes
import datetime

class ProcessEntry(ctypes.Structure):
    _fields_ = [('dwSize', wintypes.DWORD), ('cntUsage', wintypes.DWORD), ('pid', wintypes.DWORD),
        ('heap', ctypes.c_size_t), ('module', wintypes.DWORD), ('threads', wintypes.DWORD),
        ('parent', wintypes.DWORD), ('priority', wintypes.LONG), ('flags', wintypes.DWORD), ('exe', wintypes.WCHAR * 260)]

def snapshot():
    kernel = ctypes.WinDLL('kernel32', use_last_error=True)
    kernel.CreateToolhelp32Snapshot.argtypes = [wintypes.DWORD, wintypes.DWORD]
    kernel.CreateToolhelp32Snapshot.restype = wintypes.HANDLE
    kernel.Process32FirstW.argtypes = [wintypes.HANDLE, ctypes.POINTER(ProcessEntry)]
    kernel.Process32NextW.argtypes = [wintypes.HANDLE, ctypes.POINTER(ProcessEntry)]
    kernel.CloseHandle.argtypes = [wintypes.HANDLE]
    handle = kernel.CreateToolhelp32Snapshot(2, 0)
    if handle == ctypes.c_void_p(-1).value:
        raise ctypes.WinError(ctypes.get_last_error())
    result = []
    try:
        entry = ProcessEntry()
        entry.dwSize = ctypes.sizeof(ProcessEntry)
        ok = kernel.Process32FirstW(handle, ctypes.byref(entry))
        if not ok:
            raise ctypes.WinError(ctypes.get_last_error())
        while ok:
            result.append({'pid': entry.pid, 'parent_pid': entry.parent, 'exe': entry.exe})
            ok = kernel.Process32NextW(handle, ctypes.byref(entry))
    finally:
        kernel.CloseHandle(handle)
    return result

def observe(root_pid, stop, result):
    known = {root_pid}
    seen = {}
    try:
        while True:
            rows = snapshot()
            changed = True
            while changed:
                changed = False
                for row in rows:
                    if row['parent_pid'] in known and row['pid'] not in known:
                        known.add(row['pid'])
                        changed = True
            when = datetime.datetime.now(datetime.timezone.utc).isoformat()
            for row in rows:
                if row['pid'] in known:
                    key = (row['pid'], row['parent_pid'], row['exe'])
                    if key not in seen:
                        seen[key] = dict(row, first_observed_utc=when)
                    seen[key]['last_observed_utc'] = when
            if stop.wait(0.2):
                break
        result.update({'result': 'PASS', 'root_pid': root_pid, 'observed': list(seen.values()),
            'scope': 'Read-only Toolhelp process snapshots, descendants only; short-lived children may fall between samples. UHT command and compiler/link command lines also come from UBT logs and action exports.'})
    except Exception as error:
        result.update({'result': 'FAIL', 'error': str(error)})
