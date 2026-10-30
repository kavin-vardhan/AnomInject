import argparse
import json
import os
from pathlib import Path
import subprocess
import sys
import time
from environment import execution_env


def main():
    p=argparse.ArgumentParser();p.add_argument('--root',type=Path,required=True);p.add_argument('--case',required=True)
    p.add_argument('--hold',action='store_true');p.add_argument('--fc6-ubt-standin',action='store_true');p.add_argument('-Mode')
    a=p.parse_args()
    if not a.root.resolve().is_relative_to(Path('D:/IA_Europa/Receipts/09427/scratch').resolve()):raise RuntimeError('Owned rehearsal only')
    if a.hold:time.sleep(60);return 0
    (a.root/'standin-evidence.txt').write_text('Evidence must survive a failed leg.\n')
    if a.case in ('deadline','ubt-build'):
        env,_=execution_env(os.environ)
        command=[sys.executable,'-B',__file__,'--root',str(a.root),'--case',a.case,'--hold']
        if a.case=='ubt-build':command+=['--fc6-ubt-standin','-Mode=Build']
        child=subprocess.Popen(command,env=env,creationflags=subprocess.DETACHED_PROCESS|subprocess.CREATE_NEW_PROCESS_GROUP,
                               stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL,stdin=subprocess.DEVNULL)
        (a.root/'detached-pid.json').write_text(json.dumps(dict(pid=child.pid)))
        time.sleep(1.2)
        if a.case=='ubt-build':time.sleep(20)
        return 0
    if a.case=='content':
        path=a.root/'host/Content/undeclared.uasset';path.parent.mkdir(parents=True);path.write_bytes(b'synthetic content')
        time.sleep(20);return 0
    if a.case=='fail':return 7
    session=a.root/'session';(session/'probe').mkdir(parents=True)
    (session/'Runtime.log').write_text('LogExit: Exiting.\n')
    (session/'CrashClient.log').write_text('Synthetic stand-in: no crash client or network action.\n')
    rows=[dict(kind='world_snapshot_skipped',label='cleanup'),dict(kind='probe_stop',root_release='skipped',outstanding_screenshot=False)]
    (session/'probe/probe.jsonl').write_text(''.join(json.dumps(r)+'\n' for r in rows))
    (a.root/'driver.json').write_text(json.dumps(dict(result='SMOKE_PASS',module_alive=True,bindings=[dict(binding='standin',present=True)])))
    time.sleep(.4);return 0


if __name__=='__main__':raise SystemExit(main())
