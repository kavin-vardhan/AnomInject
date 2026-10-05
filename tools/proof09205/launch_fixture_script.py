import json
import os
import subprocess
import sys
import time
from pathlib import Path
import idle_gate
import runtime_guard

w = Path('E:/IA_BuildCache/_r921/09205')
label = sys.argv[1]
script = w / sys.argv[2]
gate = idle_gate.read()
assert gate['clear'],gate
assert not (w/(label+'-launch.json')).exists()
args = ['D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe','E:/IA_BuildCache/_r53_host/StackOBot/StackOBot.uproject','-run=pythonscript','-script='+str(script),'-unattended','-nosplash','-NoLiveCoding','-AllowCommandletRendering','-abslog='+str(w/(label+'.log'))]
env = os.environ.copy()
env.update(TEMP=str(w/'temp'),TMP=str(w/'temp'))
record = dict(gate=gate,args=args,started=time.time())
with (w/(label+'-stdout.log')).open('w') as log:
    p = subprocess.Popen(args,stdout=log,stderr=subprocess.STDOUT,env=env,creationflags=subprocess.CREATE_NO_WINDOW)
    record['pid'] = p.pid
    (w/(label+'-launch.json')).write_text(json.dumps(record,indent=2))
    record['guard'] = runtime_guard.wait_owned(p,600)
record['finished'] = time.time()
(w/(label+'-launch.json')).write_text(json.dumps(record,indent=2))
print(json.dumps(record,indent=2))
raise SystemExit(record['guard']['exit'])
