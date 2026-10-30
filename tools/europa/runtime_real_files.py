import copy
import json
from pathlib import Path
import shutil

from fixture_policy import ROOT,sha
from runtime_policy import JOB,canonical
from owner_attempts import create_json

REAL_ROOTS=[ROOT/'Receipts'/p for p in (
    '09426c/fixture-attempts/EA_Core/01','09426d/fixture-attempts/EA_Core/01',
    '09426d/fixture-attempts/EA_Travel/01','09427/runtime-attempts/Smoke/01',
    '09426c/fixture-attempts/Smoke/01')]
SMOKE=REAL_ROOTS[3]


def clone(source,target,include_execution=True):
    source=Path(source);target=canonical(target,JOB/'scratch')
    (target/'session/probe').mkdir(parents=True,exist_ok=False)
    paths=['session/probe/probe.jsonl','session/Runtime.log','session/CrashClient.log']
    if (source/'driver.json').exists():paths.append('driver.json')
    provenance=[]
    for name in paths:
        original=source/name;destination=target/name
        shutil.copyfile(original,destination)
        provenance.append(dict(source=str(original),destination=str(destination),size=original.stat().st_size,sha256=sha(original)))
    if include_execution:
        if (source/'execution.json').exists():
            shutil.copyfile(source/'execution.json',target/'execution.json')
        else:
            original=source/'ledger/fixture-runtime/child-001.json'
            receipt=json.loads(original.read_text());assert receipt['exit_code']==0
            create_json(target/'execution.json',dict(result='PROCESS_PASS',exit_code=receipt['exit_code'],
                real_child_receipt=str(original),real_child_sha256=sha(original),adapter='Fixture receipt shape only; actual exit code retained'))
    create_json(target/'real-file-provenance.json',dict(files=provenance,authority='FH2 real-file copy; original never rewritten'))
    return [json.loads(line) for line in (target/'session/probe/probe.jsonl').read_text().splitlines() if line]


def main(output):
    from runtime_probe_contract import stream,EMITTERS
    from runtime_readers import clean_exit,startup,read_leg
    import gate_readers as gates
    output=canonical(output,JOB/'scratch');output.mkdir(parents=True,exist_ok=False)
    table=[];mutations=[];bindings=[]
    for index,source in enumerate(REAL_ROOTS):
        target=output/('real-%02d'%index)
        rows=clone(source,target)
        binding=dict(path=str(source/'session/probe/probe.jsonl'),size=(source/'session/probe/probe.jsonl').stat().st_size,sha256=sha(source/'session/probe/probe.jsonl'))
        bindings.append(binding)
        common=clean_exit('Smoke',target,rows)
        start=startup(rows,(target/'session/Runtime.log').read_text(encoding='utf-8-sig',errors='replace'))
        observed=dict(common_cleanup='PASS',startup='PASS',world_snapshot_fields='PASS',component_state_fields='PASS',
                      A5_lifecycle='NOT_OBSERVED: no PIE/pause/travel/lifetime leg yet',GC='NOT_OBSERVED',
                      A7_screenshots='NOT_OBSERVED',A8_seeded_auto='NOT_OBSERVED')
        if any(r['kind']=='gc_retained' for r in rows):gates.gc(rows);observed['GC']='PASS'
        if any(r['kind']=='screenshot_request' for r in rows):gates.screenshot_pairs(rows);observed['A7_screenshots']='PASS'
        table.append(dict(source=str(source),result='PASS',rows=len(rows),readers=observed,common=common,startup=start))
        cases=[]
        missing=copy.deepcopy(rows);del missing[len(missing)//2];cases.append(('missing-row',missing))
        duplicate=copy.deepcopy(rows);duplicate.insert(2,copy.deepcopy(duplicate[1]));cases.append(('duplicated-row',duplicate))
        reordered=copy.deepcopy(rows);reordered[1],reordered[2]=reordered[2],reordered[1];cases.append(('out-of-order',reordered))
        unsafe=copy.deepcopy(rows);unsafe[-1]['root_release']='completed';cases.append(('unsafe-stop',unsafe))
        outstanding=copy.deepcopy(rows);outstanding[-1]['outstanding_screenshot']=True;cases.append(('outstanding-screenshot-at-stop',outstanding))
        schemas={r['kind']:r['fields'].split() for r in EMITTERS if '.' not in r['kind'] and r['kind']!='*'}
        for kind,fields in schemas.items():
            found=next((n for n,r in enumerate(rows) if r['kind']==kind),None)
            if found is None:continue
            for field in fields:
                changed=copy.deepcopy(rows);del changed[found][field]
                cases.append(('missing-'+kind+'-'+field,changed))
        for name,changed in cases:
            path=target/'mutations'/(name+'.jsonl');path.parent.mkdir(exist_ok=True)
            path.write_text(''.join(json.dumps(row)+'\n' for row in changed))
            try:stream(changed)
            except (gates.GateFailure,KeyError,TypeError,ValueError) as error:
                mutations.append(dict(source=binding['path'],case=name,result='EXPECTED_FAIL',reason=str(error),path=str(path),sha256=sha(path)))
            else:raise AssertionError('Real-file mutant survived: '+name)
        assert sha(binding['path'])==binding['sha256']
    smoke_copy=output/'exact-prior-smoke';clone(SMOKE,smoke_copy)
    smoke=read_leg('Smoke',smoke_copy);assert smoke['result']=='PASS',smoke
    result=dict(result='PASS',real_file_table=table,mutations=mutations,original_bindings=bindings,
                exact_previous_failed_smoke=smoke,emitters=EMITTERS,
                future_rows='FH2d: absent real rows require their source emitters; full gate verdicts remain unproved until real legs.')
    create_json(output/'summary.json',result)
    print(json.dumps(dict(result='PASS',real_files=len(table),mutations=len(mutations),prior_smoke_reader=smoke['result'])))


if __name__=='__main__':
    import sys
    main(Path(sys.argv[1]))
