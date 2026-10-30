import argparse
import contextlib
import datetime
import json
import os
from pathlib import Path
import sys

from environment import filtered
import runtime_policy as policy
from owner_attempts import Guard,create_json,stamp
from fixture_policy import sha,verify_files,managed_snapshot


def precheck(path,now=None,collector=None,sleeper=None):
    import fixture_precheck
    fixture_precheck.WINDOW_END=policy.WINDOW_END
    args={}
    if now is not None:args['now']=now
    if collector is not None:args['collector']=collector
    if sleeper is not None:args['sleeper']=sleeper
    return fixture_precheck.wait(path,**args)


def quick_check(config):
    policy.verify_protection(config['protected'])
    policy.manifest_check(config['manifest'])
    for key in ('host_files','engine_files','tooling_files'):verify_files(config[key])
    from owner_leg import check_ddc
    return check_ddc(policy.HOST/'StylizedParisStreet.uproject')


def execute(leg,root,temp,host,protected,argv,ledger,deadline,on_started=None,synthetic=False):
    from runtime_monitor import Monitor
    result=dict(leg=leg,started_utc=stamp(),argv=argv,launches=0)
    monitor=Monitor(root,host,leg,protected,game=leg not in ('Smoke','Load','WorldsPIE'),synthetic=synthetic)
    def launched(pid):
        result.update(launches=1,pid=pid)
        monitor.start(pid)
        if on_started:on_started(pid)
    try:
        previous_temp=ledger.SCOPE['temp_root']
        ledger.SCOPE['temp_root']=str(temp)
        result['exit_code']=ledger.run_child(argv,cwd=host,runtime_guard=True,timeout_seconds=deadline,
                                             echo=False,on_started=launched,running_check=monitor.check)
        result['result']='PROCESS_PASS'
    except BaseException as error:
        result.update(result='STOP',reason=str(error),error_type=type(error).__name__)
        raise
    finally:
        ledger.SCOPE['temp_root']=previous_temp
        monitor.close()
        result['ended_utc']=stamp()
        create_json(Path(root)/'execution.json',result)
    return result


def run(config_path,dry=False):
    config_path=policy.canonical(config_path)
    config=json.loads(config_path.read_text())
    policy.validate(config,'dry' if dry else 'real')
    quick_check(config)
    base=Path(config['receipts_root'])
    base.mkdir(parents=True,exist_ok=True)
    overall=dict(result='RUNNING',config=str(config_path),sha256=sha(config_path),legs=[],launches=0,
                 started_utc=stamp(),automatic_retry=False)
    with Guard(dict(state_root=str(base/'runtime-state/global'))) as global_guard:
        if (base/'run-result.json').exists():raise RuntimeError('This runtime run is already spent')
        import write_ledger as ledger
        operation=None
        try:
            if not dry:
                proof=json.loads(Path(config['entry_proof']).read_text())
                if proof['result']!='PASS':raise RuntimeError('FB4 full entry proof is absent')
                operation=ledger.begin('runtime-run','runtime')
            for leg in policy.LEGS:
                slot=config['slots'][leg]
                root,temp=Path(slot['root']),Path(slot['temp'])
                if not dry:
                    precheck(base/(policy.slug(leg)+'-precheck.jsonl'))
                with Guard(dict(state_root=slot['state'])) as guard:
                    guard.check_unused_launch()
                    root.mkdir(parents=True,exist_ok=False);temp.mkdir(parents=True,exist_ok=False)
                    prepared=policy.prepare_files(leg,root)
                    policy.validate_argv(prepared['argv'],leg,root)
                    create_json(root/'prepared.json',prepared)
                    print(leg+': checking Content, config, binaries, tooling and routing.',flush=True)
                    quick_check(config)
                    from fixture_wait import wait
                    args={}
                    if dry:
                        from fixture_rehearsal import samples
                        from owner_wait import SyntheticClock
                        clock=SyntheticClock();readings=samples()
                        for sample in readings:sample['idle_seconds']=600
                        iterator=iter(readings)
                        args=dict(collector=lambda:next(iterator,readings[-1]),clock=clock.now,sleep=clock.sleep)
                    gate=wait(root/'gate.jsonl',owned=(os.getpid(),),**args)
                    if gate['status']!='CLEAR':raise RuntimeError('No measured idle window; this run ends without retry')
                    quick_check(config)
                    create_json(root/'managed-before.json',managed_snapshot())
                    if dry:
                        row=dict(result='ARMED_DRY_RUN_PASS',leg=leg,launches=0,gate=gate,prepared=prepared)
                        create_json(root/'result.json',row);overall['legs'].append(row)
                        print(leg+': dry run passed; no editor or game started.',flush=True)
                        continue
                    if datetime.datetime.now(datetime.timezone.utc)>=datetime.datetime.fromisoformat(policy.WINDOW_END):
                        raise RuntimeError('FB5 launch window ended')
                    if leg!='Smoke' and overall['legs'][0]['result']!='PASS':
                        raise RuntimeError('Smoke must pass before every runtime leg')
                    guard.reserve_launch(dict(number=1))
                    print(leg+': starting its one session; limit '+str(policy.LIMITS[leg])+' seconds.',flush=True)
                    try:
                        row=execute(leg,root,temp,policy.HOST,config['protected'],prepared['argv'],ledger,policy.LIMITS[leg],
                            on_started=lambda pid:create_json(Path(slot['state'])/'launched.json',dict(pid=pid,leg=leg,utc=stamp(),authority=policy.AUTHORITY)))
                    finally:
                        create_json(root/'managed-after.json',managed_snapshot())
                        try:
                            quick_check(config)
                            protection=dict(result='PASS')
                        except Exception as error:
                            protection=dict(result='FAIL',reason=str(error))
                        create_json(root/'protection-after.json',protection)
                    if protection['result']!='PASS':raise RuntimeError(protection['reason'])
                    from runtime_readers import read_leg
                    row['reader']=read_leg(leg,root)
                    row['result']='PASS'
                    create_json(root/'result.json',row);overall['legs'].append(row)
                    print(leg+': reader and clean exit passed; the slot is spent.',flush=True)
            overall['result']='DRY_RUN_PASS' if dry else 'PASS'
        except BaseException as error:
            overall.update(result='STOP',reason=str(error),error_type=type(error).__name__)
            print('Stopped: '+str(error)+'. No further session will be started.',flush=True)
            raise
        finally:
            try:
                if operation is not None:overall['ledger']=ledger.finish(operation,'runtime')
            except Exception as error:
                overall.update(result='STOP',ledger_error=str(error))
                raise
            finally:
                executions=[json.loads(p.read_text()) for p in base.glob('runtime-attempts/*/01/execution.json')]
                overall['launches']=sum(r['launches'] for r in executions)
                overall['executions']=executions
                overall['evidence_retained']=True
                overall['ended_utc']=stamp()
                create_json(base/'run-result.json',overall)
    return overall


def main():
    p=argparse.ArgumentParser();p.add_argument('--config',type=Path,required=True);p.add_argument('--dry-run',action='store_true')
    p.add_argument('--job-config',type=Path,required=True)
    a=p.parse_args()
    clean=filtered(os.environ);os.environ.clear();os.environ.update(clean)
    os.environ['EUROPA_JOB_CONFIG']=str(a.job_config)
    try:return 0 if run(a.config,a.dry_run)['result'] in ('PASS','DRY_RUN_PASS') else 2
    except BaseException:return 2


if __name__=='__main__':raise SystemExit(main())
