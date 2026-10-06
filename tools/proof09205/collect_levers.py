import collections
import json
import time
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
values=collections.defaultdict(set)
files=[]
def note(command):
    while command.startswith('DEFER '): command=command[6:]
    name,_,args=command.partition(' ')
    if name and name not in ('WAIT','PYSTEP','py','exec'):
        values[name].add(args)
for root in sorted((w/'runs').iterdir()):
    if not root.is_dir(): continue
    for name in ('plan.json','actions.json','launch.json'):
        path=root/name
        if not path.exists(): continue
        data=json.loads(path.read_text())
        files.append(str(path))
        for command in data.get('cmds',[])+data.get('post_commands',[]): note(command)
        for action in data.get('actions',[]):
            if action.get('command'): note(action['command'])
        for arg in data.get('args',[]):
            if arg.startswith('-ExecCmds='):
                for command in arg.split('=',1)[1].strip('"').split(', '): note(command)
    for path in (root/'Binaries').glob('*.exec'):
        files.append(str(path))
        for command in path.read_text().splitlines(): note(command)
record=dict(collected=time.time(),files=files,commands={key:sorted(value) for key,value in sorted(values.items())},restoration='Console overrides expire at owned process exit; explicit mutant resets are logged. Original host configuration and six binaries are restored by the separate preflight receipt. Temporary ordinary-fixture device profiles are removed by receipt. New fixture texture LOD biases 0/1/2 and fixed light levels are retained as authored fixture content; no global/pool VT bias or source residency forcing is used.')
(w/'levers-used-final.json').write_text(json.dumps(record,indent=2))
print(json.dumps(dict(files=len(files),commands=len(values))))
