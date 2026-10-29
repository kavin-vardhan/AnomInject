import datetime
import json
from pathlib import Path
import re

import fixture_policy

FORBIDDEN={'cl.exe','link.exe','clang.exe','clang-cl.exe','lld-link.exe','unrealheadertool.exe',
    'automationtool.exe','unrealpak.exe','unreallightmass.exe','ue4editor-cmd.exe','unrealeditor-cmd.exe'}


def reason(image, command_line, root=False, rehearsal=False):
    name=Path(image).name.lower()
    if name in FORBIDDEN or not root and name in ('ue4editor.exe','unrealeditor.exe'):
        return 'FC6 unauthorized build, commandlet or additional editor child'
    standin=rehearsal and '--fc6-ubt-standin' in (command_line or '')
    ubt=name=='unrealbuildtool.exe' or name=='dotnet.exe' and 'unrealbuildtool.dll' in (command_line or '').lower() or standin
    if ubt:
        if not command_line:
            return 'FC6 UBT command line unreadable'
        modes=re.findall(r'(?i)(?:^|\s)-mode=([^\s"]+)',command_line)
        if modes!=['QueryTargets']:
            return 'FC6 UBT build action or unknown mode; only QueryTargets is permitted'
    return None


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
