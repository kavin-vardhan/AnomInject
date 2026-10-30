import argparse
import json
import os
from pathlib import Path
import subprocess
import sys

from environment import execution_env,masked
from fixture_policy import verify_files,canonical,sha
from owner_attempts import create_json
import runtime_policy as policy


def main():
    p=argparse.ArgumentParser()
    p.add_argument('--dry-run',action='store_true')
    a=p.parse_args()
    env,contract=execution_env(os.environ,dict(PYTHONDONTWRITEBYTECODE='1',GIT_OPTIONAL_LOCKS='0'))
    os.environ.clear();os.environ.update(env)
    mode='dry' if a.dry_run else 'real'
    config_path=policy.JOB/'scratch/dry-arming.json' if a.dry_run else policy.ROOT/'Launch/Europa425-Runtime.json'
    config=json.loads(config_path.read_text())
    policy.validate(config,mode);verify_files(config['tooling_files'])
    verify_files([config['job_config']])
    if sha(policy.ROOT/'Launch/Europa425-Runtime.bat')!=sha(policy.TOOLS/'Europa425-Runtime.bat'):
        raise RuntimeError('Runtime entry-point bytes differ')
    if not a.dry_run:
        qualification=json.loads((policy.JOB/'qualification.json').read_text())
        if qualification.get('result')!='PASS':raise RuntimeError('FB4 qualification absent')
        verify_files(qualification['files'])
    base=canonical(config['receipts_root']);temp=canonical(config['temp_root'])
    base.mkdir(parents=True,exist_ok=True);temp.mkdir(parents=True,exist_ok=True)
    if (base/'entry-console.txt').exists():raise RuntimeError('Runtime entry point already started')
    routed,child_contract=execution_env(env,dict(TEMP=str(temp),TMP=str(temp)))
    create_json(base/'entry-environment.json',dict(contract=contract,child_contract=child_contract,environment=masked(routed)))
    command=[sys.executable,'-B',str(policy.TOOLS/'runtime_runner.py'),'--config',str(config_path),
             '--job-config',str(policy.JOB/'runtime-job-config.json')]+(['--dry-run'] if a.dry_run else [])
    with (base/'entry-console.txt').open('x',encoding='utf-8') as output:
        proc=subprocess.Popen(command,env=routed,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,encoding='utf-8')
        for line in proc.stdout:
            output.write(line);output.flush();print(line,end='',flush=True)
        code=proc.wait()
    create_json(base/'entry-result.json',dict(exit_code=code,argv=command,automatic_retry=False))
    return code


if __name__=='__main__':raise SystemExit(main())
