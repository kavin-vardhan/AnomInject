import argparse
import datetime as dt
import fnmatch
import gzip
import hashlib
import json
import os
from pathlib import Path
import shutil
import sqlite3
import subprocess
import sys
import threading
import time

ROOT = Path('D:/IA_Europa')
JOB = ROOT / 'Receipts/09404'
LEDGER = JOB / 'ledger'
USER = Path('C:/Users/kavin')
LOCAL = USER / 'AppData/Local'
GIB = 1024 ** 3
LIMITS = {'C': 250000000, 'E': 50000000, 'engine51': 100000000}

def now():
    return dt.datetime.now(dt.timezone.utc).isoformat()

def dump(p, value):
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(json.dumps(value, indent=2) + '\n', encoding='utf-8')

def floors(start=False):
    limits = {'C': (13, 11, 10), 'D': (70, 55, 50), 'E': (25, 20, 15)}
    data = {d: {'resolved': str(Path(d + ':/').resolve(strict=True)), 'free': shutil.disk_usage(d + ':/').free,
        'start': v[0] * GIB, 'warning': v[1] * GIB, 'floor': v[2] * GIB} for d, v in limits.items()}
    if any(v['free'] < v['start' if start else 'floor'] for v in data.values()):
        raise RuntimeError('ED1 floor/headroom STOP: ' + json.dumps(data))
    return data

def specs():
    items = [
        ('engine51', 'D:/UESource/UnrealEngine', 'tree', True),
        ('engine425', 'E:/EpicGames/UE_4.25', 'tree', True),
        ('paris_original', 'E:/Unreal Projects/StylizedParisStreet', 'tree', True),
        ('unreal_engine', str(LOCAL / 'UnrealEngine'), 'tree', False),
        ('ubt', str(LOCAL / 'UnrealBuildTool'), 'tree', False),
        ('uht', str(LOCAL / 'UnrealHeaderTool'), 'tree', False),
        ('crashclient', str(LOCAL / 'CrashReportClient'), 'tree', False),
        ('epic', 'C:/ProgramData/Epic', 'tree', False),
        ('c_root', 'C:/', 'top', False), ('d_root', 'D:/', 'top', False), ('e_root', 'E:/', 'top', False),
        ('windows', 'C:/Windows', 'tree', False),
        ('wer_system', 'C:/ProgramData/Microsoft/Windows/WER', 'tree', False),
        ('wer_user', str(LOCAL / 'Microsoft/Windows/WER'), 'tree', False),
        ('gpu_nvidia', str(LOCAL / 'NVIDIA'), 'tree', False),
        ('gpu_nvidia_program', 'C:/ProgramData/NVIDIA Corporation/NV_Cache', 'tree', False),
        ('gpu_dx', str(LOCAL / 'D3DSCache'), 'tree', False),
        ('gpu_amd', str(LOCAL / 'AMD'), 'tree', False),
        ('tool_claude', str(USER / '.claude'), 'tree', False),
        ('tool_codex', str(USER / '.codex'), 'tree', False),
        ('tool_temp', 'C:/ClaudeTemp', 'tree', False),
        ('port', str(ROOT), 'tree', True)]
    for i, p in enumerate(dict.fromkeys(['C:/ClaudeTemp', str(LOCAL / 'Temp'), 'C:/Windows/Temp'])):
        items.append(('ue_temp_' + str(i), p, 'ue_temp', False))
    return items

def digest(con):
    h = hashlib.sha256()
    for row in con.execute('SELECT scope,path,size,mtime,kind FROM files ORDER BY scope,path'):
        h.update((json.dumps(row, separators=(',', ':'), ensure_ascii=True) + '\n').encode())
    return h.hexdigest()

def create_database(path):
    if path.exists():
        raise RuntimeError('Refusing preexisting snapshot: ' + str(path))
    con = sqlite3.connect(path)
    con.execute('PRAGMA journal_mode=MEMORY')
    con.execute('CREATE TABLE files(scope TEXT,path TEXT,size INTEGER,mtime INTEGER,kind TEXT,PRIMARY KEY(scope,path)) WITHOUT ROWID')
    return con

def inventory(database):
    con = create_database(database)
    errors, links, totals = [], [], {}
    for name, raw, mode, strict in specs():
        root = Path(raw)
        if not root.exists():
            totals[name] = {'path': raw, 'present': False, 'files': 0, 'folders': 0, 'bytes': 0}
            continue
        physical = root.resolve(strict=True)
        count, size, folders, batch = 0, 0, 0, []
        stack, visited = [physical], set()
        while stack:
            directory = stack.pop()
            key = str(directory).casefold()
            if key in visited:
                continue
            visited.add(key)
            if name == 'port' and directory == LEDGER:
                continue
            try:
                with os.scandir(directory) as entries:
                    for entry in entries:
                        p = Path(entry.path)
                        if mode == 'ue_temp' and directory == physical and not any(fnmatch.fnmatch(entry.name.lower(), pat) for pat in
                            ('unrealshaderworkingdir*', 'unrealbuildtool*', 'ue4*', 'unreal*', 'crashreportclient*', 'mspdbsrv*', 'vctip*')):
                            continue
                        try:
                            s = entry.stat(follow_symlinks=False)
                            isdir = entry.is_dir(follow_symlinks=False)
                            if s.st_file_attributes & 1024:
                                target = p.resolve(strict=True)
                                links.append({'scope': name, 'path': str(p), 'target': str(target)})
                                if entry.is_dir():
                                    if strict:
                                        raise RuntimeError('Unexpected reparse directory: ' + str(p))
                                    if mode == 'top':
                                        batch.append((name, str(p), 0, s.st_mtime_ns, 'directory_link'))
                                        folders += 1
                                    continue
                            if isdir:
                                if mode == 'top':
                                    batch.append((name, str(p), 0, s.st_mtime_ns, 'directory'))
                                    folders += 1
                                else:
                                    stack.append(p)
                            elif entry.is_file():
                                batch.append((name, str(p), s.st_size, s.st_mtime_ns, 'file'))
                                count += 1
                                size += s.st_size
                            if len(batch) >= 4000:
                                con.executemany('INSERT INTO files VALUES(?,?,?,?,?)', batch)
                                batch = []
                        except OSError as error:
                            if strict:
                                raise
                            errors.append({'scope': name, 'path': str(p), 'error': str(error)})
            except OSError as error:
                if strict:
                    raise
                errors.append({'scope': name, 'path': str(directory), 'error': str(error)})
        con.executemany('INSERT INTO files VALUES(?,?,?,?,?)', batch)
        con.commit()
        totals[name] = {'path': raw, 'resolved': str(physical), 'present': True, 'files': count, 'folders': folders, 'bytes': size}
        print(json.dumps({'scope': name, 'files': count, 'folders': folders, 'errors': sum(e['scope'] == name for e in errors)}), flush=True)
        floors()
    hashvalue = digest(con)
    con.close()
    return {'time_utc': now(), 'snapshot_sha256': hashvalue, 'totals': totals, 'access_errors': errors, 'links': links, 'disks': floors()}

def delta(before, after):
    con = sqlite3.connect(before)
    con.execute('ATTACH DATABASE ? AS aft', (str(after),))
    rows = con.execute('''SELECT a.scope,a.path,b.size,a.size,b.mtime,a.mtime,b.kind,a.kind FROM aft.files a LEFT JOIN main.files b
        ON a.scope=b.scope AND a.path=b.path WHERE b.path IS NULL OR a.size!=b.size OR a.mtime!=b.mtime OR a.kind!=b.kind
        UNION ALL SELECT b.scope,b.path,b.size,NULL,b.mtime,NULL,b.kind,NULL FROM main.files b LEFT JOIN aft.files a
        ON a.scope=b.scope AND a.path=b.path WHERE a.path IS NULL ORDER BY 1,2''').fetchall()
    con.close()
    return [dict(zip(('scope', 'path', 'before_size', 'after_size', 'before_mtime_ns', 'after_mtime_ns', 'before_kind', 'after_kind'), row)) for row in rows]

def remove_scratch(path):
    resolved = path.resolve(strict=True)
    if not resolved.is_relative_to(LEDGER.resolve(strict=True)) or resolved.name not in ('current.sqlite', 'next.sqlite'):
        raise RuntimeError('Unreceipted scratch removal: ' + str(path))
    with (LEDGER / 'scratch-removals.jsonl').open('a', encoding='utf-8') as f:
        f.write(json.dumps({'path': str(resolved), 'size': resolved.stat().st_size, 'time_utc': now()}) + '\n')
    resolved.unlink()

def snapshot(folder, label):
    print('LEDGER ' + label + ' ' + now(), flush=True)
    LEDGER.mkdir(parents=True, exist_ok=True)
    current, incoming = LEDGER / 'current.sqlite', LEDGER / 'next.sqlite'
    metadata = inventory(incoming)
    initial = not current.exists()
    if initial:
        if (LEDGER / 'initial.sqlite.gz').exists():
            raise RuntimeError('Missing live ledger state after initial snapshot')
        with incoming.open('rb') as source, gzip.open(LEDGER / 'initial.sqlite.gz', 'wb', compresslevel=6) as output:
            shutil.copyfileobj(source, output)
        dump(LEDGER / 'initial.json', metadata)
        changes = []
    else:
        changes = delta(current, incoming)
    with gzip.open(folder / (label + '-delta.jsonl.gz'), 'wt', encoding='utf-8') as output:
        for change in changes:
            output.write(json.dumps(change, separators=(',', ':')) + '\n')
    metadata['delta_rows'] = len(changes)
    metadata['initial_full_snapshot'] = initial
    dump(folder / (label + '.json'), metadata)
    if current.exists():
        remove_scratch(current)
    incoming.replace(current)
    return metadata, changes

def classification(scope, path, kind, change=None, build_ok=False):
    p = path.lower().replace('\\', '/')
    if scope == 'port':
        return 'port_D', 'owned scratch and hosts'
    if scope.startswith('tool_'):
        return 'tooling', 'EB1 orchestration overhead; physical volume retained'
    if scope == 'engine51':
        name = Path(p).name
        allowed = name in ('sourcefilecache.bin', 'actionhistory.bin', 'dependencycache.bin', 'makefile.ubt', 'makefile.bin', 'timestamp') or name.endswith(('.uhtmanifest', '.uhtpath', '.deps'))
        if kind == 'build' and build_ok and '/engine/intermediate/build/' in p and allowed:
            return 'engine51_metadata', 'ED4 successful NoEngineChanges build with zero engine actions'
        return 'STOP', '5.1 engine write outside admitted ED4 metadata'
    if scope == 'engine425':
        if '/engine/intermediate/build/buildrules/' in p:
            return 'port_E_exception', 'EA4 installed BuildRules'
        return 'STOP', '4.25 engine mutation outside BuildRules'
    if scope == 'paris_original':
        return 'STOP', 'Original Paris changed'
    if scope in ('c_root', 'd_root', 'e_root'):
        if change and (change['after_kind'] or change['before_kind']).startswith('directory'):
            if change['before_kind'] is None or change['after_kind'] is None:
                return 'STOP', 'ED5 new or removed root folder'
            return 'foreign', 'existing root directory timestamp; contents independently scoped'
        if Path(p).name in ('pagefile.sys', 'swapfile.sys', 'hiberfil.sys', 'dumpstack.log.tmp', 'dumpstack.log'):
            return 'foreign', 'Windows-managed paging and crash file'
        return 'STOP', 'unclassified root file mutation'
    if scope == 'unreal_engine':
        suffix = p.split('/unrealengine/', 1)[-1]
        if suffix.startswith(('5.7/', 'common/', 'zen/', 'editor/')):
            return 'foreign', 'owner engine/shared editor activity; no editor or renderer port child'
        if suffix.startswith('4.25/') or (suffix.startswith('xmlconfigcache-') and suffix.endswith('.bin')):
            return 'port_C_exception', 'EA4 EngineUserDir or XML cache, conservative charge'
        if kind == 'read_only':
            return 'foreign', 'no UE child in the recorded operation'
        return 'STOP', 'unclassified UnrealEngine path'
    if scope in ('ubt', 'uht'):
        if kind == 'read_only':
            return 'foreign', 'no UBT or UHT child in this window'
        if Path(p).suffix in ('.log', '.txt', '.json') and Path(p).name.startswith(('log', 'unreal')):
            return 'port_C_exception', 'EA4 UBT/UHT log; conservative child attribution'
        return 'STOP', 'UBT/UHT non-log path is not an EA4 exception'
    if scope == 'crashclient':
        return ('foreign', 'no port runtime child') if kind == 'read_only' else ('port_C_exception', 'EA4 crash monitor')
    if scope.startswith('gpu_'):
        return 'foreign', 'no renderer/editor/game/shader worker launched; compiler children do not render'
    if scope.startswith('ue_temp_'):
        if kind == 'read_only':
            return 'foreign', 'no Unreal or compiler child in this window'
        if 'crashreportclient' in p:
            return 'port_C_exception', 'EA4 crash monitor temp'
        return 'STOP', 'UBT/UHT/cl/link/mspdbsrv/vctip TEMP must resolve to D'
    if scope in ('windows', 'wer_system', 'wer_user'):
        if kind == 'build':
            return 'port_C_exception', 'EA4 Windows/WER: conservatively charge build window including compiler children'
        return 'foreign', 'Windows-managed files; compiler TEMP/TMP explicitly D'
    if scope == 'epic':
        return 'foreign', 'foreign launcher and EOS services'
    return 'STOP', 'unclassified scanned path'

def assess(changes, kind, previous=None, build_ok=False):
    previous = previous or {d: 0 for d in LIMITS}
    charged = {d: {} for d in LIMITS}
    stops, classes = [], {}
    for item in changes:
        c, why = classification(item['scope'], item['path'], kind, item, build_ok)
        item.update({'classification': c, 'basis': why})
        classes[c] = classes.get(c, 0) + 1
        if c == 'STOP':
            stops.append(item)
        charge = {'port_C_exception': 'C', 'port_E_exception': 'E', 'engine51_metadata': 'engine51'}.get(c)
        if charge:
            amount = max(0, (item['after_size'] or 0) - (item['before_size'] or 0))
            if charge == 'engine51':
                amount = item['after_size'] or 0
            charged[charge][item['path'].casefold()] = amount
    current = {d: sum(values.values()) for d, values in charged.items()}
    cumulative = {d: previous.get(d, 0) + current[d] for d in LIMITS}
    for d, limit in LIMITS.items():
        if cumulative[d] > limit:
            stops.append({'budget': d, 'bytes': cumulative[d], 'limit': limit})
    return {'result': 'STOP' if stops else 'PASS', 'classification_counts': classes,
        'exception_growth_charged': current, 'cumulative_job_exception_growth': cumulative, 'stops': stops}

def prior_charge():
    result = {d: 0 for d in LIMITS}
    for p in LEDGER.glob('*/summary.json'):
        values = json.loads(p.read_text())['exception_growth_charged']
        for d in result:
            result[d] += values.get(d, 0)
    return result

def finish(folder, kind):
    after, changes = snapshot(folder, 'after')
    proof = folder / 'build-admission-result.json'
    build_ok = False
    if proof.exists():
        b = json.loads(proof.read_text())
        build_ok = b.get('exit_code') == 0 and b.get('engine_actions') == 0 and b.get('NoEngineChanges') is True
    result = assess(changes, kind, prior_charge(), build_ok)
    before = json.loads((folder / 'before.json').read_text())
    with gzip.open(folder / 'classified-changes.jsonl.gz', 'wt', encoding='utf-8') as output:
        for change in changes:
            output.write(json.dumps(change, separators=(',', ':')) + '\n')
    result.update({'operation': folder.name, 'kind': kind, 'time_utc': now(), 'disks_before': before['disks'], 'disks_after': after['disks'],
        'snapshot_before_sha256': before['snapshot_sha256'], 'snapshot_after_sha256': after['snapshot_sha256'],
        'port_D_bytes_before': before['totals']['port']['bytes'], 'port_D_bytes_after': after['totals']['port']['bytes'],
        'tooling_before': {k: v for k, v in before['totals'].items() if k.startswith('tool_')},
        'tooling_after': {k: v for k, v in after['totals'].items() if k.startswith('tool_')},
        'scan_access_errors_before': len(before['access_errors']), 'scan_access_errors_after': len(after['access_errors']),
        'scope_note': 'One initial full inventory, canonical SHA-256 per snapshot, before/after deltas per operation. Error-fatal protected enumeration; listed denied OS/tooling paths are not treated as empty.'})
    size = sum(p.stat().st_size for p in LEDGER.rglob('*') if p.is_file())
    result['ledger_bytes'] = size
    if size > 2 * GIB:
        result['stops'].append({'ledger_size': size, 'limit': 2 * GIB})
        result['result'] = 'STOP'
    dump(folder / 'summary.json', result)
    print(json.dumps(result), flush=True)
    if result['result'] == 'STOP':
        raise RuntimeError('EB1/ED4/ED5 ledger STOP')
    return result

def child_env(folder):
    env = os.environ.copy()
    if any(key in env for key in ('GIT_AUTHOR_DATE', 'GIT_COMMITTER_DATE')):
        raise RuntimeError('W4 dates present')
    temp = ROOT / 'Temp/09404' / folder.name
    temp.mkdir(parents=True, exist_ok=True)
    routing = {'TEMP': str(temp), 'TMP': str(temp), 'uebp_EngineSavedFolder': str(temp / 'UAT/Saved'),
        'uebp_LogFolder': str(temp / 'UAT/Working'), 'uebp_FinalLogFolder': str(temp / 'UAT/Final'),
        'DOTNET_CLI_HOME': str(temp / 'DotNet'), 'DOTNET_CLI_TELEMETRY_OPTOUT': '1', 'DOTNET_SKIP_FIRST_TIME_EXPERIENCE': '1',
        'DOTNET_NOLOGO': '1', 'NUGET_PACKAGES': str(temp / 'NuGet'), 'GIT_OPTIONAL_LOCKS': '0',
        'PYTHONDONTWRITEBYTECODE': '1', 'EUROPA_OPERATION_DIR': str(folder)}
    env.update(routing)
    for key in ('DOTNET_ROOT', 'DOTNET_MULTILEVEL_LOOKUP', 'EUROPA_DESCRIPTOR_PROBE', 'UE-CmdLineArgs'):
        if key in env:
            routing[key] = env[key]
    return env, routing

def run_child(argv, cwd=None, acceptable=(0,)):
    folder = Path(os.environ['EUROPA_OPERATION_DIR'])
    lognum = len(list(folder.glob('child-*.json'))) + 1
    env, routing = child_env(folder)
    receipt = {'argv': list(map(str, argv)), 'windows_command_line': subprocess.list2cmdline(list(map(str, argv))),
        'cwd': str(cwd or Path.cwd()), 'started_utc': now(), 'environment': routing,
        'W4': {x: None for x in ('GIT_AUTHOR_DATE', 'GIT_COMMITTER_DATE')}}
    path = folder / f'child-{lognum:03}.json'
    dump(path, receipt)
    start = time.monotonic()
    proc = subprocess.Popen(argv, cwd=cwd, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, encoding='utf-8', errors='replace', bufsize=1)
    receipt['pid'] = proc.pid
    dump(path, receipt)
    observer = None
    child_result = {}
    stop_observer = threading.Event()
    if env.get('EUROPA_BUILD_CHILDREN') == '1':
        from compile_children import observe
        observer = threading.Thread(target=observe, args=(proc.pid, stop_observer, child_result))
        observer.start()
    def consume():
        with (folder / f'child-{lognum:03}.log').open('w', encoding='utf-8') as output:
            for line in proc.stdout:
                output.write(line)
                output.flush()
                print(line.rstrip(), flush=True)
    thread = threading.Thread(target=consume)
    thread.start()
    code = None
    try:
        while True:
            try:
                code = proc.wait(timeout=20)
                break
            except subprocess.TimeoutExpired:
                floors()
    except BaseException:
        proc.terminate()
        proc.wait()
        raise
    finally:
        thread.join()
        if observer:
            stop_observer.set()
            observer.join()
            receipt['compile_children'] = child_result
        receipt.update({'ended_utc': now(), 'exit_code': code, 'duration_seconds': time.monotonic() - start})
        dump(path, receipt)
    if observer and child_result.get('result') != 'PASS':
        raise RuntimeError('Compile child observer failed: ' + json.dumps(child_result))
    if code not in acceptable:
        raise RuntimeError('Child exit ' + str(code) + ': ' + receipt['windows_command_line'])
    return code

def capture_child(argv, cwd=None):
    folder = Path(os.environ['EUROPA_OPERATION_DIR'])
    lognum = len(list(folder.glob('child-*.json'))) + 1
    run_child(argv, cwd)
    return (folder / f'child-{lognum:03}.log').read_text(encoding='utf-8')

def begin(operation, kind):
    floors()
    folder = LEDGER / operation
    folder.mkdir(parents=True, exist_ok=False)
    dump(folder / 'operation.json', {'operation': operation, 'kind': kind, 'started_utc': now()})
    metadata, changes = snapshot(folder, 'before')
    gap = assess(changes, 'read_only')
    dump(folder / 'between-operations.json', gap)
    if gap['stops']:
        raise RuntimeError('Protected/out-of-scope mutation between operations')
    os.environ['EUROPA_OPERATION_DIR'] = str(folder)
    return folder

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--kind', choices=['read_only', 'setup', 'ubt', 'build'], default='setup')
    parser.add_argument('operation')
    parser.add_argument('command', nargs=argparse.REMAINDER)
    args = parser.parse_args()
    argv = args.command
    if argv and argv[0] == '--':
        argv = argv[1:]
    folder = begin(args.operation, args.kind)
    failure = None
    try:
        run_child(argv)
    except BaseException as error:
        failure = str(error)
        dump(folder / 'child-failure.json', {'error': failure})
    finish(folder, args.kind)
    if failure:
        raise RuntimeError(failure)

if __name__ == '__main__':
    main()
