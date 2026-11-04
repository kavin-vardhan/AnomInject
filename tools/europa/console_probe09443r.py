import ctypes
from ctypes import wintypes
import datetime
import json
from pathlib import Path


def events():
    result=[(0x100,0xc0,0x00290001),(0x101,0xc0,0xc0290001),
            (0x100,0x25,0x014b0001),(0x101,0x25,0xc14b0001)]
    result.extend((0x102,ord(char),1) for char in 'r.CustomDepth')
    result.extend([(0x100,0x0d,0x001c0001),(0x101,0x0d,0xc01c0001),
                   (0x100,0x1b,0x00010001),(0x101,0x1b,0xc0010001)])
    return result


def query(pid):
    user=ctypes.windll.user32
    user.PostMessageW.argtypes=[wintypes.HWND,wintypes.UINT,wintypes.WPARAM,wintypes.LPARAM]
    user.PostMessageW.restype=wintypes.BOOL
    user.GetWindowThreadProcessId.argtypes=[wintypes.HWND,ctypes.POINTER(wintypes.DWORD)]
    user.IsWindowVisible.argtypes=[wintypes.HWND]
    user.GetClassNameW.argtypes=[wintypes.HWND,wintypes.LPWSTR,ctypes.c_int]
    windows=[]
    callback=ctypes.WINFUNCTYPE(wintypes.BOOL,wintypes.HWND,wintypes.LPARAM)
    def visit(hwnd,param):
        process=wintypes.DWORD()
        user.GetWindowThreadProcessId(hwnd,ctypes.byref(process))
        if process.value==pid and user.IsWindowVisible(hwnd):
            name=ctypes.create_unicode_buffer(256)
            user.GetClassNameW(hwnd,name,256)
            if name.value=='UnrealWindow':windows.append(hwnd)
        return True
    user.EnumWindows(callback(visit),0)
    if len(windows)!=1:raise RuntimeError('Owned visible game viewport is not unique')
    hwnd=windows[0]
    sequence=events()
    for message,key,bits in sequence:
        if not user.PostMessageW(hwnd,message,key,bits):raise ctypes.WinError()
    return dict(pid=pid,hwnd=int(hwnd),command='r.CustomDepth',messages=len(sequence),utc=datetime.datetime.now(datetime.timezone.utc).isoformat())


class Probe:
    def __init__(self,root):
        self.root=Path(root)
        self.done=set()

    def check(self,pid):
        for index in (80,2300):
            if index in self.done or not (self.root/'plugin-frames/epoch01'/('frame-%06d.png'%index)).exists():continue
            self.done.add(index)
            row=dict(index_available=index)
            try:row.update(query(pid),result='SENT')
            except Exception as error:row.update(result='FAIL',reason=str(error))
            with (self.root/'console-probe.jsonl').open('a') as stream:stream.write(json.dumps(row)+'\n')
