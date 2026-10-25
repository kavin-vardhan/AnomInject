import concurrent.futures as futures
import gzip
import hashlib
import json
import os
from pathlib import Path
import sqlite3
import sys
import time

sys.path.insert(0, 'D:\\IntrusiveAnomalies\\_relay\\runs\\2026-10-16-094-01-europa-phase-a-plan')
import preflight09402 as pre
import write_ledger as ledger
from p9_exit09405 import binary_required

BASE = ledger.ROOT / 'Receipts/P9/baseline'
V2 = ledger.ROOT / 'Receipts/P9/baseline-v2'
PORT = ledger.ROOT / 'StylizedParisStreet/Plugins/AnomalyInjector'
receipt = json.loads((V2 / 'v2-receipt.json').read_text())
def norm(p):
    return str(p).replace('/', '\\').rstrip('\\').casefold()
removed = [norm(r['root']) for r in receipt['removed_roots']]
aliases = {
    norm('D:/IntrusiveAnomalies/_binary_baselines'): norm('E:/IA_BuildCache/_binary_baselines'),
    norm('D:/IntrusiveAnomalies/_bench_sessions_bank'): norm('E:/IA_BuildCache/_bench_sessions_bank'),
    **{norm('D:/IntrusiveAnomalies/StackOBot/' + n): norm('E:/IA_BuildCache/StackOBot/' + n) for n in ('Builds', 'Intermediate', 'Saved', 'Binaries')},
}
def physical_name(p):
    p = norm(p)
    for alias, target in aliases.items():
        if p == alias or p.startswith(alias + '\\'):
            return target + p[len(alias):]
    return p
def excluded(p):
    p = physical_name(p)
    return any(p == root or p.startswith(root + '\\') for root in removed)

def derive(out):
    streams = {k: Path(v['path']).open('rb') for k, v in receipt['files'].items()}
    hashes = {k: hashlib.sha256() for k in streams}
    counts = {k: 0 for k in streams}
    sizes = {k: 0 for k in streams}
    bytecounts = {k: 0 for k in streams}
    try:
        for line in streams['v1']:
            row = json.loads(line)
            key = 'removed' if excluded(row['path']) else 'v2'
            if streams[key].readline() != line:
                raise RuntimeError('STOP: v2 byte-for-byte re-derivation mismatch')
            for name in ('v1', key):
                hashes[name].update(line)
                counts[name] += 1
                sizes[name] += row['size']
                bytecounts[name] += len(line)
        assert all(not streams[k].read(1) for k in ('v2', 'removed'))
    finally:
        for stream in streams.values():
            stream.close()
    for key in hashes:
        expected = receipt['files'][key]
        assert hashes[key].hexdigest() == expected['sha256']
        assert counts[key] == expected['lines'] and sizes[key] == expected['sum_size_field_bytes'] and bytecounts[key] == expected['file_bytes']
    assert hashes['v1'].hexdigest() == 'db884f1e9c3debd7b9ed678db08036509959b1fc5a0d27799b10ecccb0625cce'
    assert hashes['v2'].hexdigest() == '8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74'
    result = {'result': 'PASS', 'method': 'Stream v1 in order; derive partition from removal roots; compare every output line byte for byte with v2 and removed; no protected-file full rehash.',
        'files': {k: {'sha256': hashes[k].hexdigest(), 'lines': counts[k], 'bytes': bytecounts[k], 'size_field_sum': sizes[k]} for k in hashes}, 'roots': receipt['removed_roots']}
    ledger.dump(out / 'v2-rederivation.json', result)

def git_proof(out):
    expected = json.loads((BASE / 'git-state.json').read_text())
    refs = pre.git('--git-dir=' + str(pre.COMMON), 'for-each-ref', '--format=%(refname) %(objectname)').splitlines()
    assert [r for r in refs if not r.startswith('refs/heads/feat/europa-port ')] == expected['refs']
    states, exempt = [], []
    for old in expected['worktrees']:
        p = old['path']
        if excluded(p):
            exempt.append(p)
            continue
        gitdir = Path(pre.git('-C', p, 'rev-parse', '--absolute-git-dir').strip())
        current = {'path': p, 'head': pre.git('-C', p, 'rev-parse', 'HEAD').strip(),
            'branch': pre.git('-C', p, 'rev-parse', '--abbrev-ref', 'HEAD').strip(),
            'tracked_status': pre.git('-C', p, 'status', '--porcelain=v1', '--untracked-files=no').splitlines(),
            'index_sha256': pre.sha(gitdir / 'index'),
            'full_status': pre.git('-C', p, 'status', '--porcelain=v1', '--untracked-files=all').splitlines()}
        if current != old:
            ledger.dump(out / 'git-STOP.json', {'before': old, 'after': current})
            raise RuntimeError('Protected git state changed')
        states.append(current)
    owned = []
    for p in (PORT, ledger.ROOT / 'ReGate51/StackOBot/Plugins/AnomalyInjector', ledger.ROOT / 'Baseline51/StackOBot/Plugins/AnomalyInjector'):
        status = pre.git('-C', str(p), 'status', '--porcelain=v1', '--untracked-files=all')
        if status.strip():
            raise RuntimeError('Owned checkout dirty at proof boundary: ' + str(p))
        owned.append({'path': str(p), 'head': pre.git('-C', str(p), 'rev-parse', 'HEAD').strip(), 'branch': pre.git('-C', str(p), 'rev-parse', '--abbrev-ref', 'HEAD').strip(), 'status': status})
    assert owned[0]['branch'] == 'feat/europa-port' and owned[1]['branch'] == owned[2]['branch'] == 'HEAD'
    assert owned[2]['head'] == 'd7eee97731812384d4fd00af9032254923363ada'
    ledger.dump(out / 'git-state.json', {'result': 'PASS', 'refs': refs, 'protected': states, 'owned': owned,
        'EG5_deleted_root_worktrees_excluded': exempt, 'worktree_registrations': pre.git('--git-dir=' + str(pre.COMMON), 'worktree', 'list', '--porcelain')})

def allowed_metadata():
    rows = json.loads((ledger.ROOT / 'Receipts/09406/stop/engine-differences.json').read_text())['changes']
    result = {norm(r['path']): dict(r, after_sha256=r['sha256'], authority='EG1 inherited 1023 files') for r in rows}
    for file in [*sorted((ledger.ROOT / 'Receipts/09407/ledger').glob('*/classified-changes.jsonl.gz')), *sorted((ledger.ROOT / 'Receipts/09408/ledger').glob('*/classified-changes.jsonl.gz')), *sorted((ledger.ROOT / 'Receipts/09409/ledger').glob('*/classified-changes.jsonl.gz')), *sorted((ledger.ROOT / 'Receipts/09415/ledger').glob('*/classified-changes.jsonl.gz')), *sorted(ledger.LEDGER.glob('*/classified-changes.jsonl.gz'))]:
        with gzip.open(file, 'rt', encoding='utf-8') as stream:
            for line in stream:
                row = json.loads(line)
                if row['classification'] == 'engine51_metadata':
                    result[norm(row['path'])] = dict(row, authority=str(file))
    return result

def main():
    phase = sys.argv[1]
    out = ledger.JOB / 'P9' / phase
    out.mkdir(parents=True, exist_ok=False)
    ledger.dump(out / 'disks.json', ledger.floors(start=phase.startswith('entry')))
    derive(out)
    git_proof(out)
    expected = {norm(r['path']): r for r in map(json.loads, (V2 / 'full-sha256.jsonl').open(encoding='utf-8'))}
    allowed = allowed_metadata()
    roots = json.loads((BASE / 'allowlist.json').read_text())['roots']
    roots.append('D:/IntrusiveAnomalies/StackOBot/Plugins/RoomGenerator/Content')
    seen, directories, differences, admitted, selected = set(), set(), [], [], []
    with (out / 'protected-inventory.jsonl').open('x', encoding='utf-8') as inventory:
        for raw in roots:
            if excluded(raw):
                continue
            stack = [Path(physical_name(raw)).resolve(strict=True)]
            while stack:
                p = stack.pop()
                if excluded(p) or norm(p) == norm(pre.COMMON):
                    continue
                s = p.stat()
                key = norm(p)
                if p.is_dir():
                    if key in directories:
                        continue
                    directories.add(key)
                    with os.scandir(p) as entries:
                        for entry in entries:
                            child = Path(entry.path)
                            if excluded(child):
                                continue
                            if entry.stat(follow_symlinks=False).st_file_attributes & 1024:
                                child = child.resolve(strict=True)
                            stack.append(child)
                else:
                    if key in seen:
                        continue
                    seen.add(key)
                    actual = {'path': str(p), 'size': s.st_size, 'mtime_ns': s.st_mtime_ns, 'file_id': [s.st_dev, s.st_ino]}
                    inventory.write(json.dumps(actual, separators=(',', ':')) + '\n')
                    old = expected.get(key)
                    if not old or any(actual[k] != old[k] for k in ('size', 'mtime_ns', 'file_id')):
                        exemption = allowed.get(key)
                        if exemption and (s.st_size, s.st_mtime_ns) == (exemption['after_size'], exemption['after_mtime_ns']) and pre.sha(p) == exemption['after_sha256']:
                            admitted.append(dict(actual, authority=exemption['authority'], sha256=exemption['after_sha256']))
                        else:
                            differences.append({'path': str(p), 'before': old, 'after': actual})
                    if old and binary_required(p):
                        selected.append(old)
            ledger.floors()
        for key in expected.keys() - seen:
            exemption = allowed.get(key)
            if exemption and exemption['after_size'] is None:
                admitted.append({'path': expected[key]['path'], 'removed': True, 'authority': exemption['authority']})
            else:
                differences.append({'path': expected[key]['path'], 'after': 'MISSING'})
    ledger.dump(out / 'inventory-comparison.json', {'result': 'STOP' if differences else 'PASS', 'files': len(seen), 'baseline_files': len(expected), 'differences': differences, 'EG1_admitted': admitted})
    if differences:
        raise RuntimeError('P9 inventory STOP: ' + str(len(differences)))
    print(json.dumps({'inventory': 'PASS', 'files': len(seen), 'EG1': len(admitted), 'hash_selection': len(selected)}), flush=True)
    def compute(row):
        h = pre.sha(Path(row['path']))
        if h != row['sha256']:
            raise RuntimeError('P9 hash STOP: ' + row['path'])
        return {'path': row['path'], 'size': row['size'], 'sha256': h, 'match': True}
    count, total, last = 0, 0, time.monotonic()
    with (out / 'binary-config-sha256.jsonl').open('x', encoding='utf-8') as output, futures.ThreadPoolExecutor(max_workers=3) as pool:
        iterator = iter(selected)
        pending = {pool.submit(compute, r) for r in [next(iterator, None) for _ in range(12)] if r}
        while pending:
            done, pending = futures.wait(pending, timeout=10, return_when=futures.FIRST_COMPLETED)
            for task in done:
                row = task.result()
                output.write(json.dumps(row) + '\n')
                count += 1
                total += row['size']
                nextrow = next(iterator, None)
                if nextrow:
                    pending.add(pool.submit(compute, nextrow))
            if time.monotonic() - last > 20:
                ledger.floors()
                output.flush()
                print(json.dumps({'hashed': count, 'total': len(selected), 'GiB': round(total / 1024**3, 2)}), flush=True)
                last = time.monotonic()
    from setup_europa09403 import HOSTS, manifest
    source_checks = []
    for host in ('425', '51base'):
        source, _, project = HOSTS[host]
        current = manifest(source, None if host == '425' else [project + '.uproject', 'Config', 'Content', 'Source'])
        before = json.loads((ledger.ROOT / 'Receipts/09403/copies' / host / 'source-before.json').read_text())
        assert current == before, 'Original source manifest changed'
        ledger.dump(out / ('source-' + host + '.json'), current)
        source_checks.append({'source': str(source), 'files': len(current['files']), 'bytes': current['bytes'], 'match': True})
    ledger.dump(out / 'sources.json', source_checks)
    from compiler_identity09405 import check
    ledger.dump(ledger.JOB / 'toolchains' / (phase + '-identity.json'), {h: check(h) for h in ('425', '51port')})
    if phase.startswith('entry'):
        fsck = pre.git('--git-dir=' + str(pre.COMMON), 'fsck', '--connectivity-only')
        ledger.dump(out / 'git-fsck.json', {'exit_code': 0, 'output': fsck})
        olddb = ledger.ROOT / 'Receipts/09405/ledger/current.sqlite'
        with sqlite3.connect(olddb.as_uri() + '?mode=ro', uri=True) as con:
            integrity = con.execute('PRAGMA integrity_check').fetchall()
        assert integrity == [('ok',)]
        ledger.dump(out / 'interrupted-ledger-integrity.json', {'path': str(olddb), 'result': integrity, 'read_only': True})
    summary = {'result': 'PASS', 'baseline': 'v2', 'inventory_files': len(seen), 'EG1_admitted': len(admitted), 'full_binary_config_hashes': count, 'hash_bytes': total, 'protected_git_equal': True, 'source_manifests_equal': True, 'disks': ledger.floors()}
    ledger.dump(out / 'summary.json', summary)
    print(json.dumps(summary), flush=True)

if __name__ == '__main__':
    main()
