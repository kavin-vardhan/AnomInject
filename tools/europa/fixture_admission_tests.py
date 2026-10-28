import argparse
import copy
import json
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

import fixture_wait as admission
import write_ledger as ledger
from idle_gate import GIB, FLOORS


def main(output):
    rows = []
    def check(name, condition):
        rows.append(dict(case=name, passed=bool(condition)))
        assert condition, name
    good = dict(idle_seconds=300, free_memory_bytes=6*GIB, free_commit_bytes=12*GIB,
        gpu_percent=15, processes=[], volumes={d: dict(resolved_drive=d, free_bytes=n) for d,n in admission.STARTS.items()})
    for drive, start in admission.STARTS.items():
        low = copy.deepcopy(good)
        low['volumes'][drive]['free_bytes'] = (FLOORS[drive]+start)//2
        window = admission.AdmissionWindow(0, profile='measured')
        result = window.poll(0, low)
        check(drive+'_between_floor_and_start_waits', result['status']=='WAIT' and result['reasons']==['disk:'+drive+':start'])
        check(drive+'_own_waiting_line', str(start//GIB)+' GiB start line' in admission.waiting_line(result['reasons']))
        check(drive+'_recovery_one', window.poll(25,good)['status']=='WAIT')
        check(drive+'_recovery_two', window.poll(50,good)['status']=='WAIT')
        check(drive+'_recovery_three', window.poll(75,good)['status']=='CLEAR')
        check(drive+'_four_hour_limit', window.poll(14400,low)['status']=='NEEDS-DECISION')
        low['volumes'][drive]['free_bytes']=FLOORS[drive]-1
        check(drive+'_below_floor_retained', 'disk:'+drive in admission.AdmissionWindow(0).poll(0,low)['reasons'])
    window=admission.AdmissionWindow(0,profile='measured')
    check('inclusive_start_lines', [window.poll(t,good)['status'] for t in (0,25,50)]==['WAIT','WAIT','CLEAR'])
    values={d:(FLOORS[d]+admission.STARTS[d])//2 for d in FLOORS}
    scope=dict(ledger.SCOPE,runtime_authorized=True,runtime_scope={'kind':'fixture-authoring'})
    with patch.object(ledger,'SCOPE',scope), patch.object(ledger.shutil,'disk_usage',side_effect=lambda p:SimpleNamespace(free=values[p[0]])):
        check('fixture_preflight_defers_start_to_measured_window', len(ledger.floors(start=True))==3)
        with patch.object(ledger,'SCOPE',dict(scope,runtime_authorized=False)):
            try: ledger.floors(start=True)
            except RuntimeError: check('preparation_entry_still_requires_start',True)
            else: check('preparation_entry_still_requires_start',False)
    output.mkdir(parents=True,exist_ok=False)
    (output/'result.json').write_text(json.dumps(dict(result='PASS',cases=rows),indent=2)+'\n')
    print(json.dumps(dict(result='PASS',cases=len(rows))))


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,required=True)
    main(parser.parse_args().output)
