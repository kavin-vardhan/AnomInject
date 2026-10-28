import argparse
import contextlib
import datetime
import json
import os
from pathlib import Path
import sys
import traceback
import uuid

import fixture_policy as policy
from environment import filtered, masked
from fixture_policy import (ROOT, HOST, TOOLS, SCOPE, RECIPES, OUTPUTS, canonical, sha, validate,
    verify_files, verify_manifests, command, validate_argv, content_snapshot, config_snapshot,
    output_preflight, verify_content_after)
from fixture_attempts import Guard, GlobalGuard, create_json
from fixture_wait import wait
from fixture_session_gate import SessionGate
from fixture_quarantine import quarantine
from owner_wait import SyntheticClock
from owner_leg import check_ddc


def probe_exit(path):
    previous=cleanup=stop=0
    with path.open(encoding='utf-8') as stream:
        for line in stream:
            row=json.loads(line)
            if row['sequence'] != previous+1 or stop:
                raise RuntimeError('Probe sequence is torn, out of order or continues after probe_stop.')
            previous=row['sequence']
            cleanup += (row.get('kind')=='world_cleanup' and row.get('actor_snapshot')=='skipped'
                        and row.get('skip_reason')=='engine_exit')
            if row.get('kind')=='probe_stop':
                if not cleanup or row.get('root_release')!='skipped':
                    raise RuntimeError('Probe stop lacks the earlier exit cleanup or skipped root release.')
                stop += 1
            if row.get('world_type') == 3:
                raise RuntimeError('PIE was observed during authoring.')
    if not cleanup or stop != 1:
        raise RuntimeError('Clean exit needs an engine-exit skipped world_cleanup and exactly one final probe_stop.')
    return dict(rows=previous,skipped_cleanup=cleanup,probe_stop=stop,root_release='skipped')


def run(config_path,recipe,dry_run=False,samples_path=None):
    config_path=canonical(config_path)
    config=json.loads(config_path.read_text())
    validate(config,dry_run)
    if recipe not in RECIPES:
        raise RuntimeError('Unknown recipe: '+recipe)
    if samples_path and not dry_run:
        raise RuntimeError('Synthetic readings are restricted to throwaway dry runs.')
    verify_files(config['tooling_files'])
    verify_files(config['host_files'])
    verify_files(config['engine_files'])
    verify_manifests(config)
    context={}
    with GlobalGuard(config),Guard(config,recipe) as guard:
        slot,scope_path=guard.claim()
        root=Path(slot['job_root'])
        result=dict(result='CHECKING',recipe=recipe,slot=slot['number'],launches=0,dry_run=dry_run,
                    authorization_citation=config['authorization_citation'],runtime_scope=SCOPE)
        print(recipe+': attempt '+str(slot['number'])+' of 3; checking protected files, Content, binaries and routing.',flush=True)
        os.environ['EUROPA_JOB_CONFIG']=str(scope_path)
        import write_ledger as ledger
        if ledger.SCOPE['config_path'] != str(scope_path):
            raise RuntimeError('Each recipe attempt requires a fresh process.')
        operation=None
        try:
            ledger.floors(start=True)
            before=content_snapshot()
            configs=config_snapshot()
            verify_files(config['existing_content'])
            core_path=Path(config['recipes']['EA_Core']['state_root'])/'success.json'
            core=json.loads(core_path.read_text()) if core_path.exists() else None
            output_preflight(recipe,core,dry_run)
            (root/'script').mkdir()
            author=TOOLS/'author_425_fixture.py'
            generated=("import runpy\nmodule = runpy.run_path(%r)\nmodule['author'](%r, %r)\n" %
                       (str(author),recipe,str(root/'authoring.json')))
            (root/'script/author.py').write_text(generated,encoding='utf-8')
            argv=command(recipe,root)
            validate_argv(argv,recipe,root)
            if policy.REHEARSAL is not None:
                import fixture_rehearsal
                ddc=fixture_rehearsal.check_ddc()
            else:ddc=check_ddc(HOST/'StylizedParisStreet.uproject')
            create_json(root/'content-before.json',before)
            create_json(root/'config-before.json',configs)
            with (root/'preflight-console.log').open('x',encoding='utf-8') as log,contextlib.redirect_stdout(log):
                preflight=ledger.begin('fixture-preflight','read_only')
                try:
                    if policy.REHEARSAL is not None:fixture_rehearsal.preflight(root)
                    else:ledger.run_child([sys.executable,'-B',str(TOOLS/'owner_p9.py'),'entry-fixture'])
                finally:
                    ledger.finish(preflight,'read_only')
            proof=root/'P9/entry-fixture/summary.json'
            if json.loads(proof.read_text())['result']!='PASS':
                raise RuntimeError('P9 fixture preflight failed.')
            result.update(argv=argv,p9=str(proof),ddc=ddc,manifests=config['manifests'],
                          declared_outputs=list(map(str,OUTPUTS[recipe])),
                          script_sha256=sha(root/'script/author.py'))
            create_json(root/'prepared.json',result)
            print(recipe+': protected files, Content, binaries and routing passed.',flush=True)
            arguments={}
            if samples_path or policy.REHEARSAL is not None:
                samples=fixture_rehearsal.samples() if policy.REHEARSAL is not None else json.loads(canonical(samples_path).read_text())
                if not isinstance(samples,list) or not samples:
                    raise RuntimeError('Dry run samples must be a nonempty list.')
                iterator=iter(samples)
                clock=SyntheticClock()
                arguments=dict(collector=lambda:next(iterator,samples[-1]),clock=clock.now,sleep=clock.sleep)
                result['gate_readings']='REHEARSAL scratch admission and synthetic gate' if policy.REHEARSAL is not None else 'SYNTHETIC; real P9, Content, hashes, routing and argv checks'
            else:
                result['gate_readings']='LIVE'
            result['gate']=wait(root/'idle.jsonl',owned=(os.getpid(),),**arguments)
            if result['gate']['status']!='CLEAR':
                result['result']='REFUSED_IDLE'
                return result
            verify_manifests(config)
            verify_files(config['host_files']);verify_files(config['engine_files']);verify_files(config['tooling_files'])
            verify_files(before)
            if config_snapshot()!=configs:
                raise RuntimeError('Config or descriptor changed during admission.')
            output_preflight(recipe,core,dry_run)
            ledger.floors()
            if dry_run:
                result['result']='ARMED_DRY_RUN_PASS'
                print(recipe+': armed dry run passed. No editor was started; real slots remain unused.',flush=True)
                return result
            print(recipe+': starting the one authorized authoring session; limit 1800 seconds. Output is recorded in '+str(root/'session')+'.',flush=True)
            monitor_args=fixture_rehearsal.monitor_arguments() if policy.REHEARSAL is not None else {}
            deadline=policy.REHEARSAL['deadline_seconds'] if policy.REHEARSAL is not None else 1800
            with (root/'runtime-ledger-console.log').open('x',encoding='utf-8') as log,contextlib.redirect_stdout(log), SessionGate(root/'session-gate.jsonl',**monitor_args) as monitor:
                operation=ledger.begin('fixture-runtime','runtime')
                marker=canonical(guard.state/'authoring-started.json')
                def reserve():
                    guard.reserve_launch(slot)
                    create_json(marker,dict(authority='094-22-ruling EX2',wrapper_pid=os.getpid(),
                        recipe=recipe,slot=slot['number'],phase='authoring_started'))
                    ledger.AUTHORING_STARTED=True
                def creation_failed():
                    guard.cancel_unstarted(slot)
                    create_json(root/'creation-failed-marker-removal.json',dict(path=str(marker),
                        sha256=sha(marker),reason='Process creation failed; no session began.'))
                    marker.unlink()
                    ledger.AUTHORING_STARTED=False
                def launched(pid):
                    context['process_created']=True
                    result.update(launches=1,editor_pid=pid)
                    guard.mark_launched(slot,pid)
                    monitor.start(pid)
                result['exit_code']=ledger.run_child(argv,cwd=HOST,acceptable=(0,),runtime_guard=True,
                    timeout_seconds=deadline,echo=False,on_started=launched,running_check=monitor.check,
                    before_create=reserve,on_creation_failed=creation_failed)
            result['probe_exit']=probe_exit(root/'session/probe/probe.jsonl')
            authored=json.loads((root/'authoring.json').read_text())
            if authored.get('result')!='AUTHORED_NOT_QUALIFIED' or authored.get('recipe')!=recipe:
                raise RuntimeError('Author script did not report the requested recipe complete.')
            result['outputs']=verify_content_after(before,recipe)
            if config_snapshot()!=configs:
                raise RuntimeError('Config or descriptor changed during the session.')
            verify_manifests(config)
            verify_files(config['host_files']);verify_files(config['engine_files']);verify_files(config['tooling_files'])
            result['result']='AUTHORING_PASS'
            return result
        except BaseException as error:
            result.update(result='STOP',error_type=type(error).__name__,reason=str(error))
            raise
        finally:
            try:
                if context.get('process_created') and result['result']!='AUTHORING_PASS':
                    result['quarantine']=quarantine(recipe,root,before,result.get('reason','Failed authoring session'))
                if operation is not None:
                    with (root/'runtime-ledger-console.log').open('a',encoding='utf-8') as log,contextlib.redirect_stdout(log):
                        ledger.finish(operation,'runtime')
                    if result['result']=='AUTHORING_PASS':
                        create_json(guard.state/'success.json',result)
                        print(recipe+': clean exit and output checks passed; this recipe is now spent.',flush=True)
            except BaseException as error:
                result.update(result='STOP',ledger_error=type(error).__name__,reason=str(error))
                if context.get('process_created') and not (root/'quarantine-receipt.json').exists():
                    try:result['quarantine']=quarantine(recipe,root,before,result['reason'])
                    except BaseException as cleanup_error:
                        result['quarantine_error']=dict(type=type(cleanup_error).__name__,reason=str(cleanup_error))
                raise
            finally:
                create_json(root/'attempt-result.json',result)


def main(argv=None):
    p=argparse.ArgumentParser()
    p.add_argument('--config',type=Path,required=True)
    p.add_argument('--recipe',choices=RECIPES,required=True)
    p.add_argument('--dry-run',action='store_true')
    p.add_argument('--rehearsal',action='store_true')
    p.add_argument('--dry-run-samples',type=Path)
    a=p.parse_args(argv)
    clean=filtered(os.environ)
    os.environ.clear();os.environ.update(clean)
    try:
        if a.rehearsal:
            if a.dry_run:raise RuntimeError('Rehearsal must use the non-dry branch.')
            from fixture_rehearsal import activate
            activate(json.loads(canonical(a.config).read_text()))
            global HOST,OUTPUTS
            HOST,OUTPUTS=policy.HOST,policy.OUTPUTS
        result=run(a.config,a.recipe,a.dry_run,a.dry_run_samples)
        return 0 if result['result'] in ('AUTHORING_PASS','ARMED_DRY_RUN_PASS') else 2
    except BaseException as error:
        print('Stopped: '+str(error)+' No further session will be started.',flush=True)
        return 2


if __name__=='__main__':
    raise SystemExit(main())
