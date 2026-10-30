import argparse
import datetime
import json
import os
from pathlib import Path
import subprocess
import sys
import uuid

from environment import execution_env, masked
from fixture_policy import ROOT, TOOLS, RECIPES, SESSIONS, ARMING_JOB, canonical, validate, verify_files, throwaway
from owner_attempts import create_json


def main(argv=None):
    parser=argparse.ArgumentParser()
    parser.add_argument('--config',type=Path,default=ROOT/'Launch/Europa425-FixtureAuthoring.json')
    parser.add_argument('--dry-run',action='store_true')
    parser.add_argument('--rehearsal',action='store_true')
    parser.add_argument('--dry-run-root',type=Path)
    parser.add_argument('--dry-run-temp',type=Path)
    parser.add_argument('--dry-run-samples',type=Path)
    args=parser.parse_args(argv)
    base=dict(os.environ)
    env,contract=execution_env(base,dict(PYTHONDONTWRITEBYTECODE='1',GIT_OPTIONAL_LOCKS='0'))
    os.environ.clear();os.environ.update(env)
    try:
        config_path=canonical(args.config)
        config=json.loads(config_path.read_text())
        if args.rehearsal:
            if args.dry_run:raise RuntimeError('Rehearsal must use the non-dry branch.')
            from fixture_rehearsal import activate
            activate(config)
            print('Rehearsal: owned scratch and harmless stand-ins; no Unreal process will start.',flush=True)
        if args.dry_run and not config.get('dry_run_only'):
            if not args.dry_run_root or not args.dry_run_temp:
                raise RuntimeError('Dry run needs explicit throwaway receipt and TEMP roots.')
            config=throwaway(config,args.dry_run_root,args.dry_run_temp)
        validate(config,args.dry_run)
        verify_files(config['tooling_files'])
        if not args.dry_run and any((args.dry_run_root,args.dry_run_temp,args.dry_run_samples)):
            raise RuntimeError('Throwaway options are dry-run only.')
        if args.dry_run and not args.dry_run_samples:
            raise RuntimeError('Dry run needs explicitly labelled synthetic gate readings.')
        receipts=Path(config['rehearsal']['receipts_root']) if args.rehearsal else ROOT/'Receipts'/ARMING_JOB
        scratch=Path(config['rehearsal']['temp_root']) if args.rehearsal else ROOT/'Temp'/ARMING_JOB
        parent=(canonical(args.dry_run_root) if args.dry_run_root else receipts)/'fixture-runs'
        run=parent/uuid.uuid4().hex
        temp=(canonical(args.dry_run_temp) if args.dry_run_temp else scratch)/'fixture-runs'/run.name
        canonical(run);canonical(temp)
        run.mkdir(parents=True,exist_ok=False)
        temp.mkdir(parents=True,exist_ok=False)
        create_json(run/'kit.json',config)
        create_json(run/'wrapper.json',dict(contract=contract,environment=masked(env),dry_run=args.dry_run,launches=0))
        scope=dict(config['job_template'],job_root=str(run),temp_root=str(temp),
                   started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
        create_json(run/'job-config.json',scope)
        codes=[]
        print('Europa fixture authoring: checking the EW2 arming.',flush=True)
        for recipe in SESSIONS:
            success=Path(config['recipes'][recipe]['state_root'])/'success.json'
            if not args.dry_run and success.exists():
                completed=json.loads(success.read_text())
                if completed.get('result') != 'AUTHORING_PASS':
                    raise RuntimeError(recipe+': invalid prior completion record.')
                verify_files(completed['outputs'])
                print(recipe+': previously completed; its launch authorization remains spent.',flush=True)
                continue
            if not args.dry_run and not args.rehearsal:
                from fixture_precheck import wait as idle_precheck
                idle_precheck(run/(recipe+'-idle-precheck.jsonl'))
            command=[sys.executable,'-B',str(TOOLS/'disk_pause.py'),'--output',str(run/(recipe+'-disk.json')),
                '--job-config',str(run/'job-config.json'),'--run-folder',str(run),'--kind','runtime',
                '--authoring-marker',str(Path(config['recipes'][recipe]['state_root'])/'authoring-started.json'),'--',
                sys.executable,'-B',str(TOOLS/'fixture_leg.py'),'--config',str(run/'kit.json'),'--recipe',recipe]
            if args.rehearsal:command += ['--rehearsal']
            if args.dry_run:
                command += ['--dry-run','--dry-run-samples',str(canonical(args.dry_run_samples))]
            routed,child_contract=execution_env(env,dict(TEMP=str(temp),TMP=str(temp)))
            code=subprocess.call(command,env=routed)
            codes.append(dict(recipe=recipe,exit_code=code,boundary=child_contract))
            create_json(run/(recipe+'-entry-result.json'),codes[-1])
            if code:
                print('Stopped: '+recipe+' did not pass. No later recipe or automatic retry will start.',flush=True)
                return code
        print('Both recipe '+('dry runs' if args.dry_run else 'sessions')+' finished. '+
              ('No editor was started.' if args.dry_run else 'The two launch authorizations are spent.'),flush=True)
        return 0
    except BaseException as error:
        print('Stopped: '+str(error)+' No further session will be started.',flush=True)
        return 2


if __name__=='__main__':
    raise SystemExit(main())
