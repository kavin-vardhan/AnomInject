import datetime
import ctypes
import json
from pathlib import Path

import fixture_policy

FORBIDDEN={'cl.exe','link.exe','clang.exe','clang-cl.exe','lld-link.exe','unrealheadertool.exe',
    'automationtool.exe','unrealpak.exe','unreallightmass.exe','ue4editor-cmd.exe','unrealeditor-cmd.exe'}
PLATFORMS=set('Win32 Win64 HoloLens Mac XboxOne PS4 IOS Android HTML5 Linux LinuxAArch64 AllDesktop TVOS Switch Lumin'.lower().split())
CONFIGURATIONS=set('Debug DebugGame Development Shipping Test'.lower().split())


def arguments(command_line):
    shell=ctypes.WinDLL('shell32',use_last_error=True)
    kernel=ctypes.WinDLL('kernel32',use_last_error=True)
    shell.CommandLineToArgvW.argtypes=[ctypes.c_wchar_p,ctypes.POINTER(ctypes.c_int)]
    shell.CommandLineToArgvW.restype=ctypes.POINTER(ctypes.c_wchar_p)
    kernel.LocalFree.argtypes=[ctypes.c_void_p]
    count=ctypes.c_int()
    pointer=shell.CommandLineToArgvW(command_line,ctypes.byref(count))
    if not pointer:raise ctypes.WinError(ctypes.get_last_error())
    try:return [pointer[i] for i in range(count.value)]
    finally:kernel.LocalFree(pointer)


def build_invocation(command_line):
    if not command_line:return None
    tokens=arguments(command_line)[1:]
    for token in tuple(tokens):
        if token.lower().startswith('-target='):
            tokens.extend(arguments('UnrealBuildTool.exe '+token.split('=',1)[1])[1:])
    platform=configuration=target=False
    for token in tokens:
        lower=token.lower()
        if lower=='-mode=build':return 'explicit Mode=Build'
        if lower.startswith('-targettype='):target=True
        if token.startswith('-') or lower.endswith(('.exe','.dll','.py','.uproject')):continue
        first=lower.split('+',1)[0]
        if first in PLATFORMS:platform=True
        elif first in CONFIGURATIONS:configuration=True
        else:target=True
    return 'target/platform/configuration triple' if target and platform and configuration else None


def reason(image, command_line, root=False, rehearsal=False):
    name=Path(image).name.lower()
    if name in FORBIDDEN or not root and name in ('ue4editor.exe','unrealeditor.exe'):
        return 'FD2 unauthorized build, commandlet or additional editor child'
    standin=rehearsal and '--fc6-ubt-standin' in (command_line or '')
    ubt=name=='unrealbuildtool.exe' or name=='dotnet.exe' and 'unrealbuildtool.dll' in (command_line or '').lower() or standin
    if ubt:
        action=build_invocation(command_line)
        if action:return 'FD2 UBT build invocation: '+action
    return None


class BuildOutputWatch:
    def __init__(self,host,receipt):
        self.host=Path(host)
        self.receipt=Path(receipt)
        self.before=self.snapshot()
        self.record(dict(kind='baseline',files=self.before))

    def snapshot(self):
        rows={}
        plugin=self.host/'Plugins/AnomalyInjector'
        for base in (self.host,plugin):
            for folder in ('Binaries','Intermediate'):
                root=base/folder
                if not root.exists():continue
                for path in sorted(root.rglob('*')):
                    if not path.is_file():continue
                    relative=path.relative_to(root)
                    product=(folder=='Binaries' or 'build' in [p.lower() for p in relative.parts]
                             or 'makefile' in path.name.lower()
                             or path.suffix.lower() in ('.obj','.o','.pch','.gch','.lib','.a','.pdb','.exp','.res'))
                    if product:
                        stat=path.stat()
                        rows[str(path)]=dict(size=stat.st_size,mtime_ns=stat.st_mtime_ns)
        return rows

    def record(self,row):
        with self.receipt.open('a',encoding='utf-8') as stream:
            stream.write(json.dumps(dict(row,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),authority='094-28-ruling FD2'))+'\n')

    def check(self):
        after=self.snapshot()
        changed=[dict(path=p,before=self.before.get(p),after=after.get(p))
                 for p in sorted(self.before.keys()|after.keys()) if self.before.get(p)!=after.get(p)]
        if changed:
            self.record(dict(kind='build_output_write',changes=changed,action='stop_owned_tree'))
            raise RuntimeError('FD2 write to build outputs; only the owned tree is stopped.')


def poll(tree, path):
    rows=[]
    error=None
    rehearsal=fixture_policy.REHEARSAL is not None
    for row in tree.known.values():
        live=tree.kernel.WaitForSingleObject(row['handle'],0)!=0
        command=row.get('command_line')
        if live:
            try:
                command=tree.command_line(row)
                row['command_line']=command
                row.pop('command_line_error',None)
            except RuntimeError as exc:
                row['command_line_error']=str(exc)
                command=None
        why=reason(row['image'],command,root=row['pid']==tree.root_pid,rehearsal=rehearsal)
        if why:error=why
        rows.append(dict(pid=row['pid'],parent_pid=row['parent_pid'],image=row['image'],live=live,
            command_line=command,read_error=row.get('command_line_error'),reason=why))
    with Path(path).open('a',encoding='utf-8') as stream:
        stream.write(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),processes=rows))+'\n')
    if error:raise RuntimeError(error)
