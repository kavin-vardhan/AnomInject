import ctypes
import ctypes.wintypes as wt

kernel = ctypes.WinDLL('kernel32', use_last_error=True)


class Entry(ctypes.Structure):
    _fields_ = [('dwSize', wt.DWORD), ('cntUsage', wt.DWORD), ('th32ProcessID', wt.DWORD),
                ('th32DefaultHeapID', ctypes.c_size_t), ('th32ModuleID', wt.DWORD), ('cntThreads', wt.DWORD),
                ('th32ParentProcessID', wt.DWORD), ('pcPriClassBase', wt.LONG), ('dwFlags', wt.DWORD),
                ('szExeFile', wt.WCHAR * 260)]


kernel.CreateToolhelp32Snapshot.argtypes = [wt.DWORD, wt.DWORD]
kernel.CreateToolhelp32Snapshot.restype = wt.HANDLE
kernel.Process32FirstW.argtypes = [wt.HANDLE, ctypes.POINTER(Entry)]
kernel.Process32NextW.argtypes = [wt.HANDLE, ctypes.POINTER(Entry)]
kernel.OpenProcess.argtypes = [wt.DWORD, wt.BOOL, wt.DWORD]
kernel.OpenProcess.restype = wt.HANDLE
kernel.CloseHandle.argtypes = [wt.HANDLE]
kernel.GetProcessTimes.argtypes = [wt.HANDLE] + [ctypes.POINTER(wt.FILETIME)] * 4
kernel.TerminateProcess.argtypes = [wt.HANDLE, wt.UINT]


def creation(handle):
    times = [wt.FILETIME() for i in range(4)]
    if not kernel.GetProcessTimes(handle, *(ctypes.byref(t) for t in times)):
        return None
    return str((times[0].dwHighDateTime << 32) | times[0].dwLowDateTime)


def snapshot():
    handle = kernel.CreateToolhelp32Snapshot(2, 0)
    if handle == ctypes.c_void_p(-1).value:
        raise ctypes.WinError(ctypes.get_last_error())
    rows = []
    try:
        entry = Entry()
        entry.dwSize = ctypes.sizeof(entry)
        ok = kernel.Process32FirstW(handle, ctypes.byref(entry))
        while ok:
            process = kernel.OpenProcess(0x1000, False, entry.th32ProcessID)
            stamp = None
            if process:
                try:
                    stamp = creation(process)
                finally:
                    kernel.CloseHandle(process)
            rows.append(dict(ProcessId=entry.th32ProcessID, ParentProcessId=entry.th32ParentProcessID,
                             Name=entry.szExeFile, CreationStamp=stamp))
            ok = kernel.Process32NextW(handle, ctypes.byref(entry))
        if ctypes.get_last_error() not in (0, 18):
            raise ctypes.WinError(ctypes.get_last_error())
    finally:
        kernel.CloseHandle(handle)
    return rows


def stop_owned(record):
    handle = kernel.OpenProcess(0x1001, False, record['ProcessId'])
    if not handle:
        return dict(pid=record['ProcessId'], stopped=False, reason='process absent or inaccessible')
    try:
        observed = creation(handle)
        if not observed or observed != record['CreationStamp']:
            return dict(pid=record['ProcessId'], stopped=False, reason='creation identity differs')
        if not kernel.TerminateProcess(handle, 1):
            raise ctypes.WinError(ctypes.get_last_error())
        return dict(pid=record['ProcessId'], stopped=True, creation=observed)
    finally:
        kernel.CloseHandle(handle)
