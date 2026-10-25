import argparse
import datetime as dt
import hashlib
import json
import os
from pathlib import Path
import time

import write_ledger as ledger
from a4_build import command, output_inventory
from a4_evidence import toolchain_responses, diagnostics, validate_build
from environment import decode, effective_receipt
from makefile_actions import read, executed
from process_wrapper import HOSTS
from toolchain_environment import selected
from working_build import classify_actions, current_binaries, hash_binaries, prepare_build_environment


def invalidate_makefile(path, receipt, host):
    if not path.exists():
        ledger.dump(receipt, dict(result='PASS', deleted=[], reason='No existing makefile'))
        return
    if path.resolve(strict=True) != path or not path.is_relative_to(host / 'Intermediate/Build'):
        raise RuntimeError('Makefile deletion outside owned host')
    data = path.read_bytes()
    values = decode(data)['environment']
    from environment import forbidden_child_name
    if any(forbidden_child_name(k) for k in values):
        raise RuntimeError('Blocked name in existing native environment')
    stat = path.stat()
    row = dict(path=str(path), size=stat.st_size, mtime_ns=stat.st_mtime_ns, sha256=hashlib.sha256(data).hexdigest())
    ledger.dump(receipt, dict(authority='EO3 target-owned cached environment refresh', files=[row],
        source='UE4 BuildMode.cs:530-534; UE5 BuildMode.cs:788-792', deleted=False))
    if path.stat().st_mtime_ns != stat.st_mtime_ns:
        raise RuntimeError('Makefile changed before receipt deletion')
    path.unlink()
    ledger.dump(receipt.with_name(receipt.stem+'-result.json'), dict(result='PASS', deleted=[row]))


def execute(host, target, configuration, output, query_only=False, allow_up_to_date=False):
    from job_scope import load
    scope = load()
    if configuration != 'Development':
        raise RuntimeError('A5 has Development rebuilds only')
    started = dt.datetime.fromisoformat(scope['started_utc'].replace('Z', '+00:00'))
    if (dt.datetime.now(dt.timezone.utc) - started).total_seconds() >= scope['wall_limit_hours']*3600:
        raise RuntimeError('A5 continues: clean wall-clock stop')
    project, engine, _ = HOSTS[host]
    argv, version = command(host, target, configuration, output, query=query_only)
    if output.exists():
        raise RuntimeError('Build receipt already exists')
    output.mkdir(parents=True)
    operation = Path(os.environ['EUROPA_OPERATION_DIR'])
    port = ledger.ROOT / 'StylizedParisStreet/Plugins/AnomalyInjector'
    tree = project.parent / 'Plugins/AnomalyInjector'
    tooling_sha = ledger.capture_child(['git','-C',str(port),'rev-parse','HEAD']).strip()
    sha = ledger.capture_child(['git','-C',str(tree),'rev-parse','HEAD']).strip()
    if sha != ('d7eee97731812384d4fd00af9032254923363ada' if host == '51base' else tooling_sha):
        raise RuntimeError('Build SHA does not match committed source')
    if ledger.capture_child(['git','-C',str(tree),'status','--porcelain=v1','--untracked-files=all']).strip():
        raise RuntimeError('Commit before build')
    if query_only:
        prior = [json.loads(p.read_text()) for p in (ledger.JOB/'builds').glob('*/result.json')]
        if not any(r['host']==host and r['target']==target and not r['query_only'] and r.get('gate_result')=='PASS' for r in prior):
            raise RuntimeError('EN2 requires successful rebuild before query')
    prepare_build_environment(os.environ, host, engine)
    before = output_inventory(project.parent)
    ledger.dump(output/'outputs-before.json', before)
    makefile = project.parent/'Intermediate/Build/Win64'/target/configuration/'Makefile.bin'
    invalidate_makefile(makefile, output/'makefile-refresh.json', project.parent)
    plan = output/'actions.json'
    if query_only:
        argv.append('-WriteOutdatedActions='+str(plan))
    result = dict(commit=sha, tooling_commit=tooling_sha, host=host, target=target, configuration=configuration,
        query_only=query_only, argv=argv, rules_version=[version['MajorVersion'],version['MinorVersion']],
        NoEngineChanges=True, operation=str(operation), started_utc=ledger.now(), exit_code=None,
        action_evidence='Post-build native makefile graph joined to executed log; no pre-build query',
        job_config_sha256=ledger.SCOPE['config_sha256'])
    ledger.dump(output/'invocation.json',result)
    start=time.monotonic()
    admission=dict(counts=dict(project=0,plugin=0,engine_source_in_host=0,engine=0,engine_plugin=0),binary_outputs=[])
    try:
        result['exit_code']=ledger.run_child(argv,cwd=project.parent,acceptable=tuple(range(256)),memory_guard=True)
        parents=[(p,json.loads(p.read_text())) for p in sorted(operation.glob('child-*.json'))]
        child, parent=[(p,r) for p,r in parents if r.get('argv')==argv][-1]
        text=child.with_suffix('.log').read_text(encoding='utf-8',errors='replace')
        result['logs']=[str(child.with_suffix('.log'))]
        graph=read(makefile,host)
        ledger.dump(output/'native-graph.json',graph)
        if query_only:
            data=json.loads(plan.read_text(encoding='utf-8-sig'))
            result['executed_action_lines']=[]
        else:
            data=executed(graph,text,complete=result['exit_code']==0)
            ledger.dump(plan,data)
            result['executed_action_lines']=data['executed_lines']
        expected=parent['environment_contract']['effective_sha256_by_name']
        result['effective_environment']=effective_receipt(plan,makefile,output/'effective-environment.json',expected,selected(host))
        admission=classify_actions(plan,project,engine)
        ledger.dump(output/'admission.json',admission)
        result['action_counts']=admission['counts']
        result['actual_compile_responses']=toolchain_responses(data,host,output/'responses')
        result['diagnostics']=diagnostics(text)
        result['toolchain_lines']=[line for line in text.splitlines() if 'Anomaly profile:' in line or ('Using ' in line and ('toolchain' in line or 'SDK' in line))]
        if not query_only:
            validate_build(result,require_compile=not allow_up_to_date)
        elif result['exit_code']!=0 or any(d['severity']=='error' for d in result['diagnostics']):
            raise RuntimeError('Clean-engine query failed')
        result['gate_result']='PASS'
    finally:
        result.update(ended_utc=ledger.now(),duration_seconds=time.monotonic()-start,binary_hashes=hash_binaries(admission),host_binary_inventory=current_binaries(project))
        after=output_inventory(project.parent)
        result['changed_outputs']=[dict(path=k,before=before.get(k),after=after.get(k)) for k in sorted(before.keys()|after.keys()) if before.get(k)!=after.get(k)]
        ledger.dump(output/'result.json',result)
    print(json.dumps(dict(result=result['gate_result'],host=host,target=target,sha=sha,counts=result['action_counts'],seconds=result['duration_seconds'])),flush=True)
    return result


if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('--host',required=True,choices=HOSTS)
    parser.add_argument('--target',required=True)
    parser.add_argument('--configuration',required=True)
    parser.add_argument('--output',required=True,type=Path)
    parser.add_argument('--query-only',action='store_true')
    parser.add_argument('--allow-up-to-date',action='store_true')
    args=parser.parse_args()
    execute(args.host,args.target,args.configuration,args.output,args.query_only,args.allow_up_to_date)
