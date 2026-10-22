import argparse
import contextlib
import importlib.util
import io
import json
from pathlib import Path

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    spec = importlib.util.spec_from_file_location('budget_subject', Path(__file__).with_name('write_ledger.py'))
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    root = args.output.resolve()
    if not root.is_relative_to(m.ROOT.resolve(strict=True)):
        raise RuntimeError('Synthetic evidence must remain in owned D scratch')
    root.mkdir(exist_ok=False)
    results = []
    cases = [('C_limit', 'C', 250000000, 0, 'PASS'), ('C_over', 'C', 250000001, 0, 'STOP'),
        ('E_limit', 'E', 50000000, 0, 'PASS'), ('E_over', 'E', 50000001, 0, 'STOP'),
        ('cumulative_C', 'C', 249999901, 100, 'STOP')]
    for name, drive, size, prior, expected in cases:
        m.LEDGER = root / name
        folder = m.LEDGER / 'operation'
        folder.mkdir(parents=True)
        before = folder / 'before.sqlite'
        after = folder / 'after.sqlite'
        con = m.create_database(before)
        before_hash = m.digest(con)
        con.close()
        con = m.create_database(after)
        scope = 'unreal_engine' if drive == 'C' else 'engine425'
        path = 'C:/Users/kavin/AppData/Local/UnrealEngine/4.25/SYNTHETIC-NOT-A-FILE' if drive == 'C' else 'E:/EpicGames/UE_4.25/Engine/Intermediate/Build/BuildRules/SYNTHETIC-NOT-A-FILE'
        con.execute('INSERT INTO files VALUES(?,?,?,?,?)', (scope, path, size, 1, 'file'))
        con.commit()
        after_hash = m.digest(con)
        con.close()
        changes = m.delta(before, after)
        metadata = {'totals': {'port': {'bytes': 0}}, 'disks': {}, 'access_errors': [], 'snapshot_sha256': before_hash}
        m.dump(folder / 'before.json', metadata)
        after_metadata = dict(metadata, snapshot_sha256=after_hash)
        if prior:
            m.dump(m.LEDGER / 'prior/summary.json', {'exception_growth_charged': {'C': prior, 'E': 0, 'engine51': 0}})
        m.snapshot = lambda folder, label: (after_metadata, changes)
        caught = False
        with contextlib.redirect_stdout(io.StringIO()):
            try:
                m.finish(folder, 'ubt')
            except RuntimeError as error:
                if str(error) != 'EB1/ED4/ED5 ledger STOP':
                    raise
                caught = True
        result = json.loads((folder / 'summary.json').read_text())
        assert result['result'] == expected and caught == (expected == 'STOP'), name
        assert result['cumulative_job_exception_growth'][drive] == size + prior
        results.append({'case': name, 'expected': expected, 'actual': result['result'],
            'cumulative': result['cumulative_job_exception_growth'], 'pass': True, 'actual_C_E_files_written': 0})
    m.dump(root / 'results.json', {'result': 'PASS', 'cases': results,
        'scope': 'Actual finish/prior_charge/assess on synthetic SQLite deltas; no synthetic C/E paths created'})
    print(json.dumps({'ledger_budget_can_fails': 'PASS', 'cases': len(results)}))

if __name__ == '__main__':
    main()
