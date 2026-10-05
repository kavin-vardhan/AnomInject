import json
import subprocess
import time
from pathlib import Path
import idle_gate
import runtime_guard
import proof_environment

w = Path('E:/IA_BuildCache/_r921/09203c')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')
root = w / 'runs/mainworld-inventory-01'
assert not root.exists()
gate = idle_gate.read()
if not gate['clear']:
    print(json.dumps(gate, indent=2))
    raise SystemExit('Inventory idle gate blocked')
archive = proof_environment.checked_archive()
root.mkdir(parents=True)
commands = ['IAI.Auto.Pool all 0', 'IAI.Bench.ProxyVTInventory', 'WAIT 10', 'IAI.Bench.ProxyVTInventory',
            'IAI.TexCorrupt.Census allreasons', 'WAIT 20', 'IAI.Bench.ProxyVTInventory']
plan = dict(settle_s=15, gap_ticks=2, hold_s=5, cmds=commands, viewport=[942, 869])
plan_path = root / 'plan.json'
plan_path.write_text(json.dumps(plan, indent=2))
args = ['D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe', str(host / 'StackOBot.uproject'), '/Game/StackOBot/Maps/MainWorld',
        '-ExecCmds="py E:/IA_BuildCache/_r921/09203c/pie_window_driver.py"', '-IAIPiePlan=' + str(plan_path), '-IAIBench',
        '-nosplash', '-NoLiveCoding', '-unattended', '-abslog=' + str(root / 'game.log')]
command_line = ' '.join(arg if arg.startswith('-ExecCmds=') else subprocess.list2cmdline([arg]) for arg in args)
record = dict(args=args, command_line=command_line, gate=gate, archive=archive, started=time.time())
with (root / 'stdout.log').open('w') as log:
    process = subprocess.Popen(command_line, executable=args[0], stdout=log, stderr=subprocess.STDOUT)
    record['pid'] = process.pid
    (root / 'launch.json').write_text(json.dumps(record, indent=2))
    record['runtime_guard'] = runtime_guard.wait_owned(process, 600)
    record.update(exit=record['runtime_guard']['exit'], finished=time.time())
    (root / 'launch.json').write_text(json.dumps(record, indent=2))
print(json.dumps({k:record[k] for k in ('pid', 'exit', 'started', 'finished')}, indent=2))
raise SystemExit(record['exit'])
