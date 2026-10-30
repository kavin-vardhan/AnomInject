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
    p.add_argument('-MONITOR',type=int);p.add_argument('--broker',action='store_true');p.add_argument('--leg',default='Load')
    a=p.parse_args()
    if not a.root.resolve().is_relative_to(Path('D:/IA_Europa/Receipts/09427d/scratch').resolve()):raise RuntimeError('Owned rehearsal only')
    if a.hold:time.sleep(60);return 0
    if a.broker:
        request=a.root/'monitor-request.json'
        for _ in range(300):
            if request.exists():break
            time.sleep(.1)
        else:raise RuntimeError('Rehearsal root did not request monitor')
        pid=json.loads(request.read_text())['pid']
        env,_=execution_env(os.environ)
        command=[sys.executable,'-B',__file__,'--root',str(a.root),'--case','monitor','--hold','-MONITOR='+str(pid)]
        child=subprocess.Popen(command,env=env,creationflags=subprocess.DETACHED_PROCESS|subprocess.CREATE_NEW_PROCESS_GROUP,
            stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL,stdin=subprocess.DEVNULL)
        (a.root/'detached-pid.json').write_text(json.dumps(dict(pid=child.pid,parent_pid=os.getpid(),monitor_of=pid)))
        return 0
    (a.root/'standin-evidence.txt').write_text('Evidence must survive a failed leg.\n')
    from runtime_real_files import clone,SMOKE,FAILED_LOAD
    clone(FAILED_LOAD if a.case=='failure-path' else SMOKE,a.root,include_execution=False)
    driver=json.loads((a.root/'driver.json').read_text())
    if a.leg!='Smoke':driver['result']='DRIVER_PASS'
    (a.root/'driver.json').write_text(json.dumps(driver))
    if a.case=='failure-path':
        import runtime_python27_tests as stub
        import runtime_editor_425 as driver_module
        sys.modules['unreal']=stub.stub()
        stub.ROTATION[:]=[0,0,-12];stub.REENTER[0]=True
        driver_module.start('Load',str(a.root/'single-shot-driver.json'),'p','n')
        for unused in range(30):driver_module.tick(.1)
        assert [c for c in stub.CALLS if c[0]=='console']==[['console','QUIT_EDITOR']]
        assert len([c for c in stub.CALLS if c[0]=='load_level'])==1
        assert [c for c in stub.CALLS if c[0]=='unregister']==[['unregister',1]]
        assert stub.CALLBACK[0] is None and stub.CALLS[-1]==['console','QUIT_EDITOR']
        (a.root/'single-shot-actions.json').write_text(json.dumps(stub.CALLS))
        changed=json.loads((a.root/'single-shot-driver.json').read_text())
        (a.root/'driver.json').write_text(json.dumps(changed))
        return 0
    if a.case=='monitor':
        (a.root/'monitor-request.json').write_text(json.dumps(dict(pid=os.getpid())))
        time.sleep(60);return 0
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
    if a.case=='fail':
        env,_=execution_env(os.environ)
        child=subprocess.Popen([sys.executable,'-B',__file__,'--root',str(a.root),'--case','post-crash','--hold'],
            env=env,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL,stdin=subprocess.DEVNULL)
        (a.root/'post-crash-child.json').write_text(json.dumps(dict(pid=child.pid)))
        time.sleep(1.2)
        return 7
    if a.case=='normal-fail':
        probe=a.root/'session/probe/probe.jsonl';lines=probe.read_text().splitlines()
        probe.write_text('\n'.join(lines[:-1])+'\n')
    if a.case=='ensure':
        folder=a.root/'host/Saved/Crashes/EnsureStandin';folder.mkdir(parents=True)
        (folder/'standin.json').write_text('{"standin":true}')
    time.sleep(.4);return 0


if __name__=='__main__':raise SystemExit(main())
