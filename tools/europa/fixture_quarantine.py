import json
import os
from pathlib import Path

import fixture_policy as policy
from owner_attempts import create_json,stamp


def quarantine(recipe,attempt,before,reason):
    if recipe not in policy.RECIPES:
        raise RuntimeError('Unknown quarantine recipe.')
    attempt=policy.canonical(attempt,policy.ROOT/'Receipts')
    prepared=json.loads((attempt/'prepared.json').read_text())
    declared=list(map(str,policy.OUTPUTS[recipe]))
    if prepared.get('recipe')!=recipe or prepared.get('declared_outputs')!=declared:
        raise RuntimeError('Quarantine requires this attempt\'s exact declared-output preflight.')
    prior={row['path'] for row in before}
    if any(path in prior for path in declared):
        raise RuntimeError('Quarantine refuses a file that existed before the session.')
    policy.verify_files(before)
    folder=policy.canonical(attempt/'quarantine',attempt)
    if folder.exists():
        raise RuntimeError('Quarantine folder already exists; review the retained move receipt before any re-arm.')
    plans=[]
    for raw in policy.OUTPUTS[recipe]:
        source=policy.canonical(raw,policy.HOST/'Content/EuropaPhaseA')
        if not source.exists():continue
        info=source.stat(follow_symlinks=False)
        if not source.is_file() or info.st_file_attributes&1024 or info.st_nlink!=1:
            raise RuntimeError('Quarantine refuses a non-regular, linked or aliased output: '+str(source))
        destination=policy.canonical(folder/source.name,attempt)
        if destination.exists():raise RuntimeError('Quarantine destination already exists: '+str(destination))
        plans.append(dict(source=str(source),destination=str(destination),size=info.st_size,
                          mtime_ns=info.st_mtime_ns,sha256=policy.sha(source),file_id=[info.st_dev,info.st_ino]))
    receipt=attempt/'quarantine-receipt.json'
    row=dict(result='PLANNED',authority='D:/IntrusiveAnomalies/_reviews/094-22-ruling.md#EX3',
        recipe=recipe,reason=reason,utc=stamp(),declared_outputs=declared,files=plans,moved=[],
        original_paths_absent=False,retry_authorized=False,launch_lock_unchanged=True)
    create_json(receipt,row)
    try:
        if plans:folder.mkdir(exist_ok=False)
        for plan in plans:
            source=policy.canonical(plan['source'],policy.HOST/'Content/EuropaPhaseA')
            destination=policy.canonical(plan['destination'],attempt)
            info=source.stat(follow_symlinks=False)
            if info.st_file_attributes&1024 or [info.st_dev,info.st_ino]!=plan['file_id'] or policy.sha(source)!=plan['sha256']:
                raise RuntimeError('Quarantine source changed after its move receipt: '+str(source))
            os.rename(source,destination)
            if source.exists() or policy.sha(destination)!=plan['sha256']:
                raise RuntimeError('Quarantine move verification failed: '+str(source))
            row['moved'].append(plan)
            receipt.write_text(json.dumps(row,indent=2)+'\n')
        policy.verify_files(before)
        row.update(result='QUARANTINED',original_paths_absent=all(not Path(p).exists() for p in declared),
                   completed_utc=stamp())
        receipt.write_text(json.dumps(row,indent=2)+'\n')
        print(recipe+': '+str(len(plans))+' declared partial outputs quarantined by receipt. A retry requires chat re-arm.',flush=True)
        return row
    except BaseException as error:
        row.update(result='QUARANTINE_INCOMPLETE',error_type=type(error).__name__,reason=str(error))
        receipt.write_text(json.dumps(row,indent=2)+'\n')
        raise
