import argparse
import hashlib
import json
import os
from pathlib import Path
import sys
import time

from environment import boundary, filtered, masked
from prepare_runtime import prepare, owned
from runtime_attribution import validate_redirects

ROOT=Path('D:/IA_Europa')
TOOLS=Path(__file__).resolve().parent


def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream,'sha256').hexdigest()


def admit(config, scope, dry_run):
    if config.get('host')!='425' or config.get('kind')!='editor-preview-owner-leg':
        raise RuntimeError('Owner kit is scoped to the 4.25 preview leg')
    if not dry_run and (not config.get('authorized_by_chat') or not scope.get('runtime_authorized')):
        raise RuntimeError('A6 authorization/configuration is not armed; no launch')
    for name in ('job_config','manifest'):
        owned(Path(config[name]))
    if digest(Path(config['manifest']))!=config['manifest_sha256']:
        raise RuntimeError('Owner binary manifest changed')
    return True


def verify_files(rows):
    checked=[]
    for row in rows:
        path=owned(Path(row['path']))
        if not path.is_file() or path.stat().st_size!=row['size'] or digest(path)!=row['sha256']:
            raise RuntimeError('Bound owner-leg file changed: '+str(path))
        checked.append(dict(path=str(path),sha256=row['sha256']))
    return checked


def check_ddc(project):
    path=project.parent/'Config/DefaultEngine.ini'
    text=path.read_text(encoding='utf-8-sig')
    for graph in ('Europa425DDC','InstalledDerivedDataBackendGraph','DerivedDataBackendGraph'):
        section=text.split('['+graph+']',1)[1].split('\n[',1)[0]
        if 'D:/IA_Europa/Caches/425/Boot.ddc' not in section or 'D:/IA_Europa/Caches/425/Local' not in section or 'Shared' in section or 'EnvPathOverride' in section:
            raise RuntimeError('Owner DDC graph failed routing check')
    return dict(path=str(path),sha256=digest(path),graphs=3)


def wait_idle(output):
    from idle_gate import collect,Window
    window=Window(time.monotonic())
    with output.open('x',encoding='utf-8') as stream:
        while True:
            sample=collect()
            result=window.poll(time.monotonic(),sample,[os.getpid()])
            stream.write(json.dumps(dict(sample=sample,result=result))+'\n')
            stream.flush()
            print(json.dumps(result),flush=True)
            if result['status']=='CLEAR':return
            if result['status']=='NEEDS-DECISION':raise RuntimeError('no idle window')
            time.sleep(25)


def run(config_path,dry_run=False):
    owned(config_path)
    config=json.loads(config_path.read_text())
    base=dict(os.environ)
    clean=filtered(base)
    clean['EUROPA_JOB_CONFIG']=str(Path(config['job_config']))
    contract=boundary(base,clean,{'EUROPA_JOB_CONFIG':clean['EUROPA_JOB_CONFIG']})
    os.environ.clear()
    os.environ.update(clean)
    from job_scope import load
    scope=load()
    admit(config,scope,dry_run)
    import write_ledger as ledger
    ledger.floors(start=True)
    output=owned(Path(scope['job_root'])/('owner-leg-dry-run' if dry_run else 'owner-leg'))
    output.mkdir(parents=True,exist_ok=False)
    manifest=json.loads(Path(config['manifest']).read_text())
    binaries=verify_files(manifest['files'])
    sources=verify_files(config['host_source_and_routing_files'])
    from prepare_runtime import HOSTS
    project,engine,graph,_=HOSTS['425']
    ddc=check_ddc(project)
    argv=prepare('425',output,probe=True,audit=True,authoring=False)
    redirects=validate_redirects(argv,graph)
    preflight=ledger.begin('owner-leg-preflight','read_only')
    try:
        ledger.run_child([sys.executable,'-B',str(TOOLS/'owner_p9.py'),'entry-owner'])
    finally:
        ledger.finish(preflight,'read_only')
    proof=Path(scope['job_root'])/'P9/entry-owner/summary.json'
    if json.loads(proof.read_text())['result']!='PASS':raise RuntimeError('P9 owner preflight failed')
    result=dict(result='PREPARED_ONLY' if dry_run else 'ADMITTED',launches=0,dry_run=dry_run,
        argv=argv,environment=masked(clean),boundary=contract,redirects=redirects,ddc=ddc,
        p9_proof=str(proof),manifest=str(config['manifest']),manifest_sha256=config['manifest_sha256'],
        binaries=binaries,sources=sources,engine_exe_sha256=digest(Path(argv[0])),
        owner_preview='Owner input is part of the sentinel leg; it is not a measured idle PIE leg.')
    ledger.dump(output/'prepared.json',result)
    if dry_run:
        print(json.dumps(dict(result='PREPARED_ONLY',launches=0,receipt=str(output/'prepared.json'))))
        return result
    print('Waiting for a quiet start. Leave this window open.',flush=True)
    wait_idle(output/'idle.jsonl')
    verify_files(manifest['files'])
    verify_files(config['host_source_and_routing_files'])
    folder=ledger.begin('owner-leg-editor','runtime')
    try:
        os.environ['EUROPA_BUILD_CHILDREN']='1'
        result['exit_code']=ledger.run_child(argv,cwd=project.parent,acceptable=tuple(range(256)))
        result['launches']=1
    finally:
        ledger.dump(output/'runtime-result.json',result)
        ledger.finish(folder,'runtime')
    return result


def self_tests(config):
    import copy
    scope=json.loads(Path(config['job_config']).read_text())
    assert admit(config,scope,True)
    cases=[dict(case='dry_run_allowed_without_runtime_authority',result='PASS')]
    for name,mutant,other in [('runtime_not_armed',copy.deepcopy(config),scope),
        ('wrong_host',dict(config,host='51port'),scope),('wrong_manifest_hash',dict(config,manifest_sha256='0'*64),scope)]:
        try:admit(mutant,other,name!='runtime_not_armed')
        except RuntimeError:cases.append(dict(case=name,result='REJECTED'))
        else:raise AssertionError(name)
    from prepare_runtime import selftest
    return dict(result='PASS',cases=cases,routing=selftest(),launches=0)


if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('--config',type=Path,default=ROOT/'Launch/Europa425-OwnerLeg.json')
    parser.add_argument('--dry-run',action='store_true')
    parser.add_argument('--self-test-output',type=Path)
    args=parser.parse_args()
    if args.self_test_output:
        owned(args.self_test_output)
        args.self_test_output.parent.mkdir(parents=True,exist_ok=True)
        args.self_test_output.write_text(json.dumps(self_tests(json.loads(args.config.read_text())),indent=2)+'\n')
    else:
        run(args.config,args.dry_run)
