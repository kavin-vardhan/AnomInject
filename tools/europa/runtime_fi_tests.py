import inspect
import json
from pathlib import Path
import sys

from canary_eventlogs import exclusion_class,FOLDER
from owner_attempts import create_json
import runtime_policy as policy
from runtime_evidence import outcome
from runtime_runner import record_leg,crash_policy
from rotator_contract import check_all,check_source,SCRIPTS


def main(output):
    output=policy.canonical(output,policy.JOB/'scratch');output.mkdir(parents=True,exist_ok=False)
    cases=[]
    def check(name,value):
        assert value,name
        cases.append(dict(case=name,result='PASS'))
    static=check_all()
    for name in SCRIPTS:
        for expression in ('unreal.Rotator(-12,0,0)','unreal.Rotator(pitch=-12)','unreal.Rotator(*angles)','unreal.Rotator(**angles)'):
            check(name+':'+expression,bool(check_source(expression,name)))
    check('zero-argument-allowed',not check_source('unreal.Rotator()','empty'))
    check('all-keywords-allowed',not check_source('unreal.Rotator(roll=0,yaw=0,pitch=-12)','keywords'))
    class Denied(OSError):winerror=5
    class Locked(OSError):winerror=32
    class RegionLocked(OSError):winerror=33
    for name,path,error,wanted in (
        ('FE2-event',FOLDER/'x.evtx',Denied(),True),
        ('FI5-datastore','C:/Windows/SoftwareDistribution/DataStore/DataStore.edb',Locked(),True),
        ('FI5-region','c:/WINDOWS/System32/locked.bin',RegionLocked(),True),
        ('readable','C:/Windows/readable.bin',None,False),
        ('generic-access-denial','C:/Windows/unknown.bin',Denied(),False),
        ('wrong-drive','D:/Windows/locked.bin',Locked(),False),
        ('prefix-sibling','C:/Windows.old/locked.bin',Locked(),False),
        ('parent-escape','C:/Windows/../outside.bin',Locked(),False),
        ('owned-locked','D:/IA_Europa/Receipts/locked.bin',Locked(),False)):
        check(name,(exclusion_class(path,error) is not None)==wanted)
    passed=dict(result='PASS');upload=dict(result='PASS',logs=[])
    def result(code):
        return outcome('Load',dict(result='PROCESS_PASS' if code==0 else 'SESSION_STOP',exit_code=code,launches=1),passed,upload,passed)
    run=dict(legs=[],result='RUNNING')
    check('first-crash-continues',record_leg(run,result(7)) and run['crash_count']==1)
    check('pass-after-first-crash',record_leg(run,result(0)) and run['crash_count']==1)
    check('second-crash-stops',not record_leg(run,result(3)) and run['crash_count']==2)
    check('second-crash-recorded',run['result']=='STOP' and len(run['legs'])==3)
    second=crash_policy(result(7),1)
    check('classifier-idempotent',crash_policy(second,1)==second)
    broken=outcome('Smoke',dict(result='SESSION_STOP',exit_code=7,launches=1),passed,upload,passed)
    check('broken-smoke-always-stops',broken['end_run'])
    create_json(output/'summary.json',dict(result='PASS',cases=cases,static=static,unreal_launches=0))
    print(json.dumps(dict(result='PASS',cases=len(cases))))


if __name__=='__main__':main(Path(sys.argv[1]))
