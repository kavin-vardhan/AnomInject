import collections
import json
from pathlib import Path
import time

w = Path('E:/IA_BuildCache/_r921/09203c')
values = collections.defaultdict(set)
files = []
def note(command):
    while command.startswith('DEFER '):
        command = command[6:]
    name, _, args = command.partition(' ')
    if name and name != 'WAIT':
        values[name].add(args)

for root in sorted((w / 'runs').iterdir()):
    if not root.is_dir():
        continue
    for name in ('plan.json', 'actions.json', 'launch.json'):
        path = root / name
        if not path.exists():
            continue
        data = json.loads(path.read_text())
        files.append(str(path))
        for command in data.get('cmds', []):
            note(command)
        for action in data.get('actions', []):
            if action.get('command'):
                note(action['command'])
        for arg in data.get('args', []):
            if arg.startswith('-ExecCmds='):
                for command in arg.split('=', 1)[1].strip('"').split(', '):
                    if not command.startswith('py '):
                        note(command)
record = dict(collected=time.time(), files=files, commands={k:sorted(v) for k,v in sorted(values.items())},
              restoration='Process-local console changes expire at owned process exit. Host Config/SavedConfig are restored from preflight hashes; regression-created device profile is removed by its receipt.')
(w / 'levers-used-final.json').write_text(json.dumps(record, indent=2))
print(json.dumps(dict(files=len(files), commands=len(values))))
