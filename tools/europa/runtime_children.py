from pathlib import Path

from fixture_children import arguments, reason as prior_reason


def reason(image, command_line, root=False, rehearsal=False):
    prior = prior_reason(image, command_line, root, rehearsal)
    if prior:
        return prior
    name = Path(image).name.lower()
    ubt = (name == 'unrealbuildtool.exe'
           or name == 'dotnet.exe' and 'unrealbuildtool.dll' in (command_line or '').lower()
           or rehearsal and '--fc6-ubt-standin' in (command_line or ''))
    if not ubt:
        return None
    if not command_line:
        return 'FE3 unreadable UnrealBuildTool command line'
    tokens = [t.lower() for t in arguments(command_line)[1:]]
    modes = [t.split('=', 1)[1] for t in tokens if t.startswith('-mode=')]
    if any(m in ('clean', 'execute', 'deploy') for m in modes):
        return 'FE3 forbidden UBT action mode'
    if not modes and any(t.startswith('-targetlist=') for t in tokens):
        return 'FE3 implicit TargetList build'
    return None


def poll(tree, path):
    import datetime
    import json
    rows = []
    error = None
    for row in tree.known.values():
        live = tree.kernel.WaitForSingleObject(row['handle'], 0) != 0
        command = row.get('command_line')
        if live:
            try:
                command = tree.command_line(row)
                row['command_line'] = command
                row.pop('command_line_error', None)
            except RuntimeError as exc:
                row['command_line_error'] = str(exc)
                command = None
        why = reason(row['image'], command, root=row['pid'] == tree.root_pid)
        if why:
            error = why
        rows.append(dict(pid=row['pid'], parent_pid=row['parent_pid'], image=row['image'],
                         live=live, command_line=command, read_error=row.get('command_line_error'), reason=why))
    with Path(path).open('a', encoding='utf-8') as stream:
        stream.write(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                                     authority='094-27 FB7 / FE3 / FD2', processes=rows))+'\n')
    if error:
        raise RuntimeError(error)
