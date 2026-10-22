import gzip
import importlib.util
import json
from pathlib import Path

def main():
    source = Path(__file__).with_name('write_ledger.py')
    spec = importlib.util.spec_from_file_location('subject', source)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    root = m.JOB / 'ledger-self-tests'
    root.mkdir(exist_ok=False)
    cases = [
        ('C_limit', 'unreal_engine', 'C:/Users/kavin/AppData/Local/UnrealEngine/4.25/SYNTHETIC', 250000000, 'file', 'PASS'),
        ('C_over', 'unreal_engine', 'C:/Users/kavin/AppData/Local/UnrealEngine/4.25/SYNTHETIC', 250000001, 'file', 'STOP'),
        ('E_limit', 'engine425', 'E:/EpicGames/UE_4.25/Engine/Intermediate/Build/BuildRules/SYNTHETIC', 50000000, 'file', 'PASS'),
        ('E_over', 'engine425', 'E:/EpicGames/UE_4.25/Engine/Intermediate/Build/BuildRules/SYNTHETIC', 50000001, 'file', 'STOP'),
        ('new_scanned_file', 'uht', 'C:/Users/kavin/AppData/Local/UnrealHeaderTool/Saved/Config/SYNTHETIC.ini', 1, 'file', 'STOP'),
        ('new_C_root_folder', 'c_root', 'C:/SYNTHETIC-NOT-CREATED', 0, 'directory', 'STOP'),
        ('new_D_root_folder', 'd_root', 'D:/SYNTHETIC-NOT-CREATED', 0, 'directory', 'STOP'),
        ('new_E_root_folder', 'e_root', 'E:/SYNTHETIC-NOT-CREATED', 0, 'directory', 'STOP'),
        ('engine51_binary', 'engine51', 'D:/UESource/UnrealEngine/Engine/Binaries/Win64/SYNTHETIC.dll', 1, 'file', 'STOP')]
    results = []
    for name, scope, path, size, kind, expected in cases:
        folder = root / name
        folder.mkdir()
        before, after = folder / 'before.sqlite', folder / 'after.sqlite'
        con = m.create_database(before)
        before_hash = m.digest(con)
        con.close()
        con = m.create_database(after)
        con.execute('INSERT INTO files VALUES(?,?,?,?,?)', (scope, path, size, 1, kind))
        con.commit()
        after_hash = m.digest(con)
        con.close()
        changes = m.delta(before, after)
        saved = folder / 'delta.jsonl.gz'
        with gzip.open(saved, 'wt', encoding='utf-8') as f:
            for row in changes:
                f.write(json.dumps(row) + '\n')
        with gzip.open(saved, 'rt', encoding='utf-8') as f:
            restored = [json.loads(line) for line in f]
        assert restored == changes and before_hash != after_hash
        result = m.assess(restored, 'build')
        assert result['result'] == expected, name
        results.append({'case': name, 'expected': expected, 'actual': result['result'], 'before_sha256': before_hash,
            'after_sha256': after_hash, 'delta_roundtrip_equal': True, 'synthetic_external_paths_created': 0})
    m.dump(root / 'results.json', {'result': 'PASS', 'cases': results, 'count': len(results)})
    print(json.dumps({'ledger_self_tests': 'PASS', 'cases': len(results)}))

if __name__ == '__main__':
    main()
