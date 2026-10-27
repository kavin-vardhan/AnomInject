import hashlib
import gzip
import json
import os
from pathlib import Path
import sqlite3

import write_ledger as ledger
from disk_pause import acknowledge_restart
from process_wrapper import HOSTS
from recovery_cleanup import cleanup


def main():
    pending = ledger.JOB/'disk-restart-required.json'
    data = json.loads(pending.read_text())
    if data['kind'] != 'build':
        raise RuntimeError('EU2 interrupted setup needs explicit inspection; it is not replayed automatically')
    argv = data['argv']
    index = argv.index('--kind')
    if argv[index+1] != 'build' or argv[index+3] != '--':
        raise RuntimeError('Unrecognized interrupted ledger command')
    operation = argv[index+2]
    if Path(operation).name != operation:
        raise RuntimeError('Invalid interrupted operation')
    folder = ledger.LEDGER/operation
    inner = argv[index+4:]
    if len(inner)<7 or not Path(inner[2]).name.startswith('start_build'):
        raise RuntimeError('Unrecognized interrupted build command')
    host, target, configuration, label = inner[3:7]
    from job_scope import require_pair
    require_pair(host,target,configuration)
    project = HOSTS[host][0]
    plan_labels = {('425',True):'425-editor-rebound', ('425',False):'425-game-rebound',
                   ('51base',True):'51base-editor-fixed', ('51base',False):'51base-game',
                   ('51port',True):'51port-editor', ('51port',False):'51port-game'}
    plan = ledger.ROOT/'Receipts/09415/builds'/plan_labels[(host,target.endswith('Editor'))]/'actions.json'
    ledger.floors()
    os.environ['EUROPA_OPERATION_DIR'] = str(folder)
    next_db = ledger.LEDGER/'next.sqlite'
    if next_db.exists():
        ledger.remove_scratch(next_db)
    if not (folder/'before.json').exists():
        raise RuntimeError('EU2 interrupted initial inventory requires explicit entry recovery')
    if not (folder/'summary.json').exists():
        old_snapshot = ledger.snapshot
        try:
            after_path = folder/'after.json'
            if after_path.exists() and (ledger.LEDGER/'current.sqlite').exists():
                metadata = json.loads(after_path.read_text())
                with sqlite3.connect((ledger.LEDGER/'current.sqlite').as_uri()+'?mode=ro',uri=True) as con:
                    same = ledger.digest(con)==metadata['snapshot_sha256']
                if same:
                    with gzip.open(folder/'after-delta.jsonl.gz','rt',encoding='utf-8') as stream:
                        changes = [json.loads(line) for line in stream]
                    ledger.snapshot = lambda target,label: (metadata,changes)
            ledger.finish(folder,'build')
        finally:
            ledger.snapshot = old_snapshot
    if json.loads((folder/'summary.json').read_text())['result'] != 'PASS':
        raise RuntimeError('EU2 interrupted build ledger did not pass')
    suffix = 1
    while (ledger.LEDGER/(operation+'-disk-cleanup-'+str(suffix))).exists():
        suffix += 1
    recovery_folder = ledger.begin(operation+'-disk-cleanup-'+str(suffix), 'setup')
    receipt = recovery_folder/'partial-output-cleanup.json'
    children = [json.loads(p.read_text()) for p in folder.glob('child-*.json')]
    native_started = any(r.get('pid') and Path(r['argv'][0]).name.casefold()=='unrealbuildtool.exe' for r in children)
    if native_started:
        cleanup(plan,project,receipt)
    else:
        empty = dict(result='PASS', host=str(project.parent), files=[], removed_count=0, dry_run=False,
                     reason='Native UBT never started; no partial compile products. EO3 refresh is already receipted.')
        ledger.dump(receipt,empty)
        ledger.dump(receipt.with_name('partial-output-cleanup-result.json'),empty)
    ledger.finish(recovery_folder,'setup')
    acknowledge_restart(pending,receipt.with_name('partial-output-cleanup-result.json'),
                        recovery_folder/'summary.json',recovery_folder/'restart-permitted.json')
    print(json.dumps(dict(result='PASS', fresh_operation_required=True, original_label=label,
                          original_operation=operation, no_D2_attempt_charged=True)))


if __name__=='__main__':
    main()
