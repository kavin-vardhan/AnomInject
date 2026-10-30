import argparse
import datetime
import json
import os
from pathlib import Path
import sys

from environment import filtered
import runtime_policy as policy
from owner_attempts import Guard,create_json,stamp
from fixture_policy import sha,verify_files,managed_snapshot
from runtime_evidence import freeze,outcome


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
    monitor=Monitor(root,host,leg,protected,game=leg not in ('Smoke','Load','WorldsPIE'),synthetic=synthetic,temp=temp)
    folder=Path(os.environ['EUROPA_OPERATION_DIR'])
    number=len(list(folder.glob('child-*.json')))+1
    receipt=folder/('child-%03d.json'%number)
    def launched(pid):
        result.update(launches=1,pid=pid)
        monitor.start(pid)
        if on_started:on_started(pid)
    previous_temp=ledger.SCOPE['temp_root']
    try:
        ledger.SCOPE['temp_root']=str(temp)
        result['exit_code']=ledger.run_child(argv,cwd=host,runtime_guard=True,timeout_seconds=deadline,
            echo=False,on_started=launched,running_check=monitor.check,on_root_exit=monitor.process_exited)
        result['result']='PROCESS_PASS'
    except BaseException as error:
        result.update(result='SESSION_STOP',reason=str(error),error_type=type(error).__name__)
    finally:
        ledger.SCOPE['temp_root']=previous_temp
        monitor.close()
        result['new_crash_report_folders']=monitor.crash_changes
        result['child_receipt']=str(receipt)
        if receipt.exists():
            child=json.loads(receipt.read_text())
            result.update(exit_code=child.get('exit_code'),stopped_owned_processes=child.get('stopped_owned_processes',[]),
                post_crash_owned_stop=child.get('post_crash_owned_stop',[]),
                root_ended_utc=child.get('root_ended_utc',child.get('ended_utc')),
                adopted_monitors=child.get('compile_children',{}).get('adopted_monitors',[]))
            reason=child.get('interruption',{}).get('message','')
            result['hard_stop']=any(s in reason for s in ('unauthorized compile','FD2 UBT build','FE3 forbidden UBT','FE3 implicit','additional editor','commandlet','packaging child'))
        result['ended_utc']=stamp()
        create_json(Path(root)/'execution.json',result)
    return result


def perform_session(leg,root,temp,host,protected,argv,ledger,deadline,check_protection,on_started=None,synthetic=False,prior_crashes=0):
    row=execute(leg,root,temp,host,protected,argv,ledger,deadline,on_started,synthetic)
    try:
        check_protection();protection=dict(result='PASS')
    except Exception as error:
        protection=dict(result='FAIL',reason=str(error))
    create_json(root/'protection-after.json',protection)
    child=Path(row['child_receipt'])
    extras=list(child.parent.glob(child.stem+'*'))
    raw=freeze(root,extras,ledger.SCOPE.get('canary_sha256_utf8'))
    from fixture_uploads import check
    upload=check(root/'session')
    from runtime_readers import read_leg
    reader=read_leg(leg,root)
    if not (root/'upload-check.json').exists():create_json(root/'upload-check.json',upload)
    driver=json.loads((root/'driver.json').read_text()) if (root/'driver.json').exists() else None
    row.update(outcome(leg,row,protection,upload,reader,driver,row['new_crash_report_folders'],raw))
    row=crash_policy(row,prior_crashes)
    row['raw_evidence']=str(root/'raw-evidence.json')
    create_json(root/'result.json',row)
    return row


def crash_policy(row,prior_crashes):
    row=dict(row,crash_count=prior_crashes+int(row.get('crashed',False)))
    if row.get('crashed') and row['crash_count']>=2 and 'FI4 second runtime crash' not in row['run_stop_reasons']:
        row.update(end_run=True,run_stop_reasons=list(row['run_stop_reasons'])+['FI4 second runtime crash'])
    return row


def record_leg(overall,row):
    row=crash_policy(row,overall.get('crash_count',0))
    overall['crash_count']=row['crash_count']
    overall['legs'].append(row)
    if row.get('end_run'):
        overall.update(result='STOP',reason='; '.join(row['run_stop_reasons']))
        return False
    return True


def run(config_path,dry=False):
    config_path=policy.canonical(config_path)
    config=json.loads(config_path.read_text())
    policy.validate(config,'dry' if dry else 'real')
    quick_check(config)
    base=Path(config['receipts_root']);base.mkdir(parents=True,exist_ok=True)
    overall=dict(result='RUNNING',config=str(config_path),sha256=sha(config_path),legs=[],launches=0,
                  started_utc=stamp(),automatic_retry=False,crash_count=0)
    with Guard(dict(state_root=str(base/'runtime-state/global'))):
        if (base/'run-result.json').exists():raise RuntimeError('This runtime run is already spent')
        import write_ledger as ledger
        operation=None
        try:
            if not dry:
                proof=json.loads(Path(config['entry_proof']).read_text())
                if proof['result']!='PASS':raise RuntimeError('FH9 full entry proof is absent')
                operation=ledger.begin('runtime-run','runtime')
            for leg in policy.LEGS:
                slot=config['slots'][leg];root,temp=Path(slot['root']),Path(slot['temp'])
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
                    if gate['status']!='CLEAR':
                        row=dict(leg=leg,result='FAIL',launches=0,reason='No measured idle window',end_run=False,gate=gate)
                        create_json(root/'result.json',row);record_leg(overall,row)
                        print(leg+': no session started; evidence kept; proceeding to the next leg.',flush=True)
                        continue
                    quick_check(config)
                    create_json(root/'managed-before.json',managed_snapshot())
                    if dry:
                        row=dict(result='ARMED_DRY_RUN_PASS',leg=leg,launches=0,gate=gate,prepared=prepared)
                        create_json(root/'result.json',row);overall['legs'].append(row)
                        print(leg+': dry run passed; no editor or game started.',flush=True)
                        continue
                    if datetime.datetime.now(datetime.timezone.utc)>=datetime.datetime.fromisoformat(policy.WINDOW_END):
                        raise RuntimeError('FH9 launch window ended')
                    if leg!='Smoke' and not any(r['leg']=='Smoke' and r.get('launches')==1 and not r.get('end_run') for r in overall['legs']):
                        raise RuntimeError('Smoke did not start; no runtime leg can start')
                    guard.reserve_launch(dict(number=1))
                    print(leg+': starting its one session; limit '+str(policy.LIMITS[leg])+' seconds.',flush=True)
                    row=perform_session(leg,root,temp,policy.HOST,config['protected'],prepared['argv'],ledger,policy.LIMITS[leg],
                        check_protection=lambda:quick_check(config),
                        prior_crashes=overall['crash_count'],
                        on_started=lambda pid:create_json(Path(slot['state'])/'launched.json',dict(pid=pid,leg=leg,utc=stamp(),authority=policy.AUTHORITY)))
                    create_json(root/'managed-after.json',managed_snapshot())
                    if not record_leg(overall,row):
                        print('Stopped: '+overall['reason']+'. No further session will be started.',flush=True)
                        break
                    print(leg+': '+row['result']+'; raw evidence hashed; proceeding to the next leg.',flush=True)
            else:
                overall['result']='DRY_RUN_PASS' if dry else 'COMPLETE_WITH_FAILURES' if any(r['result']!='PASS' for r in overall['legs']) else 'PASS'
        except BaseException as error:
            overall.update(result='STOP',reason=str(error),error_type=type(error).__name__)
            print('Stopped: '+str(error)+'. No further session will be started.',flush=True)
        finally:
            try:
                if operation is not None:overall['ledger']=ledger.finish(operation,'runtime')
            except Exception as error:
                overall.update(result='STOP',ledger_error=str(error))
            finally:
                executions=[json.loads(p.read_text()) for p in base.glob('runtime-attempts/*/01/execution.json')]
                overall.update(launches=sum(r['launches'] for r in executions),executions=executions,evidence_retained=True,ended_utc=stamp())
                create_json(base/'run-result.json',overall)
    return overall


def main():
    p=argparse.ArgumentParser();p.add_argument('--config',type=Path,required=True);p.add_argument('--dry-run',action='store_true')
    p.add_argument('--job-config',type=Path,required=True)
    a=p.parse_args();clean=filtered(os.environ);os.environ.clear();os.environ.update(clean)
    os.environ['EUROPA_JOB_CONFIG']=str(a.job_config)
    try:return 0 if run(a.config,a.dry_run)['result'] in ('PASS','DRY_RUN_PASS') else 2
    except BaseException:return 2


if __name__=='__main__':raise SystemExit(main())
