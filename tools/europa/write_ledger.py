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

from job_scope import load

SCOPE = load()
ROOT = Path('D:/IA_Europa')
JOB = Path(SCOPE['job_root'])
LEDGER = JOB / 'ledger'
USER = Path('C:/Users/kavin')
LOCAL = USER / 'AppData/Local'
GIB = 1024 ** 3
LIMITS = {'C': 250000000, 'E': 50000000, 'engine51': 100000000}
AUTHORING_STARTED = False

def now():
    return dt.datetime.now(dt.timezone.utc).isoformat()

def dump(p, value):
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(json.dumps(value, indent=2) + '\n', encoding='utf-8')

def floors(start=False):
    if (start and not AUTHORING_STARTED and SCOPE.get('runtime_authorized') is True
            and SCOPE.get('runtime_scope', {}).get('kind') == 'fixture-authoring'):
        start = False
    limits = {'C': (13, 11, 10), 'D': (40, 30, 25), 'E': (15, 12, 10)}
    if JOB in (ROOT/'Receipts/09431', ROOT/'Receipts/09431b', ROOT/'Receipts/09432',ROOT/'Receipts/09433',ROOT/'Receipts/09433b',ROOT/'Receipts/09433c',ROOT/'Receipts/09434',ROOT/'Receipts/09435',ROOT/'Receipts/09436',ROOT/'Receipts/09437',ROOT/'Receipts/09438',ROOT/'Receipts/09439',ROOT/'Receipts/09440',ROOT/'Receipts/09442',ROOT/'Receipts/09442c'):
        limits['D'] = (25, 25, 15)
    data = {d: {'resolved': str(Path(d + ':/').resolve(strict=True)), 'free': shutil.disk_usage(d + ':/').free,
        'start': v[0] * GIB, 'warning': v[1] * GIB, 'floor': v[2] * GIB} for d, v in limits.items()}
    if any(v['free'] < v['floor'] for v in data.values()) and os.environ.get('EUROPA_DISK_SUPERVISOR_PID') and not AUTHORING_STARTED:
        from disk_pause import checkpoint
        checkpoint(data)
        return floors(start)
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
        ('uht', str(LOCAL / 'UnrealHeaderTool'), 'tree', True),
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
    errors, links, totals, uht_hashes = [], [], {}, []
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
                                if mode == 'top' or name == 'uht':
                                    batch.append((name, str(p), 0, s.st_mtime_ns, 'directory'))
                                    folders += 1
                                if mode != 'top':
                                    stack.append(p)
                            elif entry.is_file():
                                batch.append((name, str(p), s.st_size, s.st_mtime_ns, 'file'))
                                count += 1
                                size += s.st_size
                                if name == 'uht':
                                    with p.open('rb') as source:
                                        value = hashlib.file_digest(source, 'sha256').hexdigest()
                                    uht_hashes.append({'path': str(p), 'size': s.st_size, 'mtime_ns': s.st_mtime_ns, 'sha256': value})
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
    return {'time_utc': now(), 'snapshot_sha256': hashvalue, 'totals': totals, 'access_errors': errors, 'links': links, 'uht_file_sha256': uht_hashes, 'disks': floors()}

def delta(before, after):
    connections = [sqlite3.connect(path) for path in (before, after)]
    try:
        for con in connections:
            con.execute('PRAGMA cache_size=-1024')
            con.execute('PRAGMA mmap_size=0')
        cursors = [con.execute('SELECT scope,path,size,mtime,kind FROM files ORDER BY scope,path') for con in connections]
        old, new = [cursor.fetchone() for cursor in cursors]
        changes = []
        while old is not None or new is not None:
            if new is None or (old is not None and old[:2] < new[:2]):
                row = (*old[:2], old[2], None, old[3], None, old[4], None)
                old = cursors[0].fetchone()
            elif old is None or new[:2] < old[:2]:
                row = (*new[:2], None, new[2], None, new[3], None, new[4])
                new = cursors[1].fetchone()
            else:
                row = (*new[:2], old[2], new[2], old[3], new[3], old[4], new[4]) if old != new else None
                old, new = [cursor.fetchone() for cursor in cursors]
            if row is not None:
                changes.append(dict(zip(('scope', 'path', 'before_size', 'after_size', 'before_mtime_ns', 'after_mtime_ns', 'before_kind', 'after_kind'), row)))
        return changes
    finally:
        for con in connections:
            con.close()


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
        from fixture_policy import managed_kind
        managed=managed_kind(Path(path))
        if managed:
            return 'port_D', managed+'; recorded, not bound (094-26-ruling FC5; 094-26c-ruling FF2)'
        role = 'DDC' if '/ddc/' in p else 'shader_working' if '/shaderworking/' in p else 'crash_report' if '/crash' in p else 'host_output'
        return 'port_D', ('runtime ' + role) if kind == 'runtime' else 'owned scratch and hosts'
    if scope.startswith('tool_'):
        return 'tooling', 'EB1 orchestration overhead; physical volume retained'
    if scope == 'engine51':
        import re
        name = Path(p).name
        engine = 'd:/uesource/unrealengine/engine/'
        relative = p.removeprefix(engine)
        intermediate = relative.startswith('intermediate/build/') or (
            relative.startswith('plugins/') and '/intermediate/build/' in relative)
        allowed = name in ('sourcefilecache.bin', 'actionhistory.bin', 'dependencycache.bin',
            'makefile.ubt', 'makefile.bin', 'timestamp', 'livecodinginfo.json') or name.endswith(('.uhtmanifest', '.uhtpath', '.deps'))
        marker = re.fullmatch(r'intermediate/ubtruns/[0-9]+_[0-9a-f]+', relative) is not None
        if p.startswith(engine) and (intermediate and allowed or marker):
            return 'engine51_metadata', 'EG1 stock build bookkeeping; success, failure and interruption covered'
        return 'STOP', '5.1 engine write outside EG1 bookkeeping classes'
    if scope == 'engine425':
        import re
        if re.fullmatch(r'e:/epicgames/ue_4[.]25/engine/intermediate/editorruns/[0-9]+',p) and change and all(change.get(k) in (None,0) for k in ('before_size','after_size')):
            return 'port_E_exception', 'ET2 zero-byte stock runtime marker; process identities recorded by the runtime child tracker'
        if '/engine/intermediate/build/buildrules/' in p:
            return 'port_E_exception', 'EA4 installed BuildRules'
        return 'STOP', '4.25 engine mutation outside BuildRules'
    if scope == 'paris_original':
        return 'STOP', 'Original Paris changed'
    if scope in ('c_root', 'd_root', 'e_root'):
        if change and (change['before_kind'] is None or change['after_kind'] is None):
            from root_attribution import classify
            return classify(change, JOB)
        if change and (change['after_kind'] or change['before_kind']).startswith('directory'):
            return 'foreign', 'existing root directory timestamp; contents independently scoped'
        if Path(p).name in ('pagefile.sys', 'swapfile.sys', 'hiberfil.sys', 'dumpstack.log.tmp', 'dumpstack.log'):
            return 'foreign', 'Windows-managed paging and crash file'
        return 'STOP', 'unclassified root file mutation'
    if scope == 'unreal_engine':
        suffix = p.split('/unrealengine/', 1)[-1]
        if kind == 'runtime' and suffix.startswith(('common/', 'zen/', 'editor/', '5.1/')):
            return 'STOP', 'Runtime cache/settings must route to D; no port exception for this shared path'
        if suffix.startswith(('5.7/', 'common/', 'zen/', 'editor/')):
            return 'foreign', 'owner engine/shared editor activity; no editor or renderer port child'
        if suffix.startswith('4.25/') or (suffix.startswith('xmlconfigcache-') and suffix.endswith('.bin')):
            return 'port_C_exception', 'EA4 EngineUserDir or XML cache, conservative charge'
        if kind == 'read_only':
            return 'foreign', 'no UE child in the recorded operation'
        return 'STOP', 'unclassified UnrealEngine path'
    if scope == 'uht':
        relative = p.split('/unrealheadertool/', 1)[-1]
        windows = {'compat.ini', 'deviceprofiles.ini', 'engine.ini', 'game.ini', 'gameusersettings.ini',
            'hardware.ini', 'input.ini', 'lightmass.ini', 'runtimeoptions.ini', 'scalability.ini'}
        allowed = relative.startswith('saved/config/windows/') and relative.removeprefix('saved/config/windows/') in windows
        allowed = allowed or relative == 'saved/logs' or relative.startswith('saved/logs/')
        parts = relative.split('/')
        allowed = allowed or (len(parts) >= 4 and parts[:3] == ['saved', 'config', 'crashreportclient'] and parts[3].startswith('ue4cc-windows-'))
        parents = {'saved', 'saved/config', 'saved/config/windows', 'saved/config/crashreportclient'}
        if relative in parents and change and change['before_kind'] == change['after_kind'] == 'directory':
            allowed = True
        if allowed:
            return ('foreign', 'No UHT child in read-only window') if kind == 'read_only' else ('port_C_exception', 'EE1 exact UHT settings/log output; before/after hashes retained')
        return 'STOP', 'UHT path outside EE1 exact settings/log classes'
    if scope == 'ubt':
        if kind == 'read_only':
            return 'foreign', 'no UBT or UHT child in this window'
        if Path(p).suffix in ('.log', '.txt', '.json') and Path(p).name.startswith(('log', 'unreal')):
            return 'port_C_exception', 'EA4 UBT/UHT log; conservative child attribution'
        return 'STOP', 'UBT/UHT non-log path is not an EA4 exception'
    if scope == 'crashclient':
        return ('foreign', 'no port runtime child') if kind == 'read_only' else ('port_C_exception', 'EA4 crash monitor')
    if scope.startswith('gpu_'):
        if kind == 'runtime':
            return 'port_C_exception', 'EA4 GPU driver caches; conservatively charge runtime window'
        return 'foreign', 'no renderer/editor/game/shader worker launched; compiler children do not render'
    if scope.startswith('ue_temp_'):
        if kind == 'read_only':
            return 'foreign', 'no Unreal or compiler child in this window'
        if 'crashreportclient' in p:
            return 'port_C_exception', 'EA4 crash monitor temp'
        return 'STOP', 'UBT/UHT/cl/link/mspdbsrv/vctip TEMP must resolve to D'
    if scope in ('windows', 'wer_system', 'wer_user'):
        if kind in ('build', 'runtime'):
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
        'exception_growth_charged': current, 'cumulative_job_exception_growth': cumulative, 'stops': stops,
        'root_attributions': [item['root_attribution'] for item in changes if 'root_attribution' in item]}

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
    for change in changes:
        if classification(change['scope'], change['path'], kind, change, build_ok)[0] == 'engine51_metadata':
            p = Path(change['path'])
            if change['after_kind'] == 'file':
                before_hash = p.stat()
                with p.open('rb') as stream:
                    change['after_sha256'] = hashlib.file_digest(stream, 'sha256').hexdigest()
                after_hash = p.stat()
                if (after_hash.st_size, after_hash.st_mtime_ns) != (change['after_size'], change['after_mtime_ns']) or (before_hash.st_size, before_hash.st_mtime_ns) != (after_hash.st_size, after_hash.st_mtime_ns):
                    raise RuntimeError('EG1 metadata changed during receipt hash: ' + str(p))
            else:
                change['after_sha256'] = None
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

def child_env(folder, execution=False):
    from environment import filtered, masked
    env = filtered(os.environ)
    if any(key in env for key in ('GIT_AUTHOR_DATE', 'GIT_COMMITTER_DATE')):
        raise RuntimeError('W4 dates present')
    temp = Path(SCOPE['temp_root']) / folder.name
    temp.mkdir(parents=True, exist_ok=True)
    routing = {'TEMP': str(temp), 'TMP': str(temp), 'uebp_EngineSavedFolder': str(temp / 'UAT/Saved'),
        'uebp_LogFolder': str(temp / 'UAT/Working'), 'uebp_FinalLogFolder': str(temp / 'UAT/Final'),
        'DOTNET_CLI_HOME': str(temp / 'DotNet'), 'DOTNET_CLI_TELEMETRY_OPTOUT': '1', 'DOTNET_SKIP_FIRST_TIME_EXPERIENCE': '1',
        'DOTNET_NOLOGO': '1', 'NUGET_PACKAGES': str(temp / 'NuGet'), 'GIT_OPTIONAL_LOCKS': '0',
        'PYTHONDONTWRITEBYTECODE': '1', 'EUROPA_OPERATION_DIR': str(folder),
        'EUROPA_JOB_CONFIG': SCOPE['config_path']}
    if execution:
        from environment import execution_env, masked, wrapper_metadata_name
        routing = {k:v for k,v in routing.items() if not wrapper_metadata_name(k)}
        env, contract = execution_env(os.environ,routing)
        return env, masked(env)
    env.update({key.upper(): value for key, value in routing.items()})
    selected = {key.casefold() for key in routing}
    selected.update(key.casefold() for key in ('DOTNET_ROOT', 'DOTNET_MULTILEVEL_LOOKUP', 'EUROPA_DESCRIPTOR_PROBE', 'UE-CmdLineArgs', 'EUROPA_BUILD_CHILDREN'))
    from environment import boundary
    boundary(os.environ, env, routing)
    receipt = masked(env)
    return env, receipt

def run_child(argv, cwd=None, acceptable=(0,), memory_guard=False, on_started=None, before_create=None, on_creation_failed=None,
              runtime_guard=False, timeout_seconds=None, echo=True, running_check=None, on_root_exit=None):
    folder = Path(os.environ['EUROPA_OPERATION_DIR'])
    lognum = len(list(folder.glob('child-*.json'))) + 1
    from environment import execution_command
    execution = memory_guard or runtime_guard or execution_command(argv)
    env, routing = child_env(folder,execution=execution)
    receipt = {'argv': list(map(str, argv)), 'windows_command_line': subprocess.list2cmdline(list(map(str, argv))),
        'cwd': str(cwd or Path.cwd()), 'started_utc': now(), 'environment': routing,
        'job_config_sha256': SCOPE['config_sha256'],
        'W4': {x: None for x in ('GIT_AUTHOR_DATE', 'GIT_COMMITTER_DATE')}}
    from environment import fingerprints
    receipt['environment_contract'] = {'result': 'PASS', 'policy': 'EL1/EM2/EW3' if execution else 'EL1/EM2 orchestration', 'nonblocked_base_preserved': True, 'effective_sha256_by_name': fingerprints(env)}
    path = folder / f'child-{lognum:03}.json'
    from root_attribution import context
    with (folder / 'root-contexts.jsonl').open('a', encoding='utf-8') as contexts:
        contexts.write(json.dumps(dict(utc=now(), child_receipt=str(path), references=context(argv,cwd or Path.cwd(),env)))+'\n')
    dump(path, receipt)
    memory_log = folder / f'child-{lognum:03}-memory.jsonl'
    tree = None
    if runtime_guard and (not timeout_seconds or timeout_seconds <= 0):
        raise RuntimeError('Runtime requires a positive session deadline')
    runtime_phase=runtime_guard and SCOPE.get('runtime_scope',{}).get('kind')=='phase-a-runtime'
    monitor_adoption=runtime_guard and SCOPE.get('runtime_scope',{}).get('kind') in ('phase-a-runtime','fixture-authoring')
    if monitor_adoption:
        from runtime_owned import RuntimeTree as OwnedTree
    elif runtime_guard:
        from build_memory import OwnedTree
    if memory_guard:
        if Path(argv[0]).name.casefold() != 'unrealbuildtool.exe':
            raise RuntimeError('EF1 guard requires a directly owned UBT executable')
        from build_memory import OwnedTree, wait_for_window, monitor_sample, sample
        receipt['memory_admission'] = wait_for_window(memory_log)
        receipt['memory_log'] = str(memory_log)
        dump(path, receipt)
    observer, thread, code = None, None, None
    child_result = {}
    stop_observer = threading.Event()
    start = time.monotonic()
    if before_create is not None:
        before_create()
    if memory_guard and JOB in (ROOT/'Receipts/09431', ROOT/'Receipts/09431b', ROOT/'Receipts/09432',ROOT/'Receipts/09433',ROOT/'Receipts/09433b',ROOT/'Receipts/09433c',ROOT/'Receipts/09434',ROOT/'Receipts/09435',ROOT/'Receipts/09436',ROOT/'Receipts/09437',ROOT/'Receipts/09438',ROOT/'Receipts/09439',ROOT/'Receipts/09440',ROOT/'Receipts/09442',ROOT/'Receipts/09442c'):
        floors(start=True)
    try:
        proc = subprocess.Popen(argv, cwd=cwd, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            text=True, encoding='utf-8', errors='replace', bufsize=1,
            creationflags=subprocess.BELOW_NORMAL_PRIORITY_CLASS if memory_guard else 0)
    except BaseException:
        if on_creation_failed is not None:
            on_creation_failed()
        raise
    try:
        if runtime_guard:
            tree = OwnedTree(proc.pid)
        if on_started is not None:
            on_started(proc.pid)
        receipt['pid'] = proc.pid
        dump(path, receipt)
        if memory_guard:
            tree = OwnedTree(proc.pid)
        if os.environ.get('EUROPA_BUILD_CHILDREN') == '1' and not (memory_guard or runtime_guard):
            from compile_children import observe
            observer = threading.Thread(target=observe, args=(proc.pid, stop_observer, child_result))
            observer.start()
        def consume():
            try:
                with (folder / f'child-{lognum:03}.log').open('w', encoding='utf-8') as output:
                    for line in proc.stdout:
                        output.write(line)
                        output.flush()
                        if echo and 'TICKPIN probe' not in line:
                            print(line.rstrip(), flush=True)
            except BaseException as error:
                child_result['console_error'] = dict(type=type(error).__name__, message=str(error))
        thread = threading.Thread(target=consume)
        thread.start()
        last_memory, last_floor, low_count = time.monotonic(), time.monotonic(), 0
        root_exit_at=None
        while proc.poll() is None or runtime_guard and any(tree.kernel.WaitForSingleObject(r['handle'],0) != 0 for r in tree.known.values()):
            if tree:
                tree.poll()
                if runtime_guard:
                    if SCOPE.get('runtime_scope', {}).get('kind') == 'phase-a-runtime':
                        from runtime_children import poll as poll_runtime_children
                    else:
                        from fixture_children import poll as poll_runtime_children
                    poll_runtime_children(tree,folder/f'child-{lognum:03}-process-polls.jsonl')
                root_code=proc.poll()
                if monitor_adoption and root_code is not None and root_exit_at is None:
                    root_exit_at=time.monotonic();receipt['root_ended_utc']=now()
                    if on_root_exit:on_root_exit(root_code)
                    if root_code!=0:
                        receipt['post_crash_owned_stop']=tree.stop()
                if monitor_adoption and root_exit_at is not None and time.monotonic()-root_exit_at>=60:
                    tree.stop_monitors('FH5 still alive 60 seconds after root exit')
                if running_check is not None and (not monitor_adoption or root_code is None):
                    running_check(tuple(tree.known))
                if runtime_guard and time.monotonic()-start >= timeout_seconds and (not monitor_adoption or root_code is None or any(not r.get("monitor_of") and tree.kernel.WaitForSingleObject(r["handle"],0)!=0 for r in tree.known.values())):
                    receipt['deadline_seconds'] = timeout_seconds
                    raise RuntimeError('Session limit reached; stopped only the owned editor tree, including crash reporter children.')
                if runtime_guard and any(Path(r['image']).name.casefold() in ('cl.exe','link.exe','unrealheadertool.exe','automationtool.exe','unrealpak.exe','ue4editor-cmd.exe') for r in tree.known.values()):
                    raise RuntimeError('An unauthorized compile, commandlet, cook or packaging child appeared.')
                if memory_guard and time.monotonic() - last_memory >= 5:
                    low_count = monitor_sample(sample(), low_count, tree.stop, memory_log)
                    last_memory = time.monotonic()
            if time.monotonic() - last_floor >= 20:
                if not runtime_guard or running_check is None:
                    floors()
                last_floor = time.monotonic()
            if child_result.get('console_error'):
                raise RuntimeError('Child console reader failed')
            time.sleep(0.2 if tree else 1)
        code = proc.returncode
        if monitor_adoption and root_exit_at is None:
            receipt['root_ended_utc']=now()
            if on_root_exit:on_root_exit(code)
    except BaseException as error:
        receipt['interruption'] = {'type': type(error).__name__, 'message': str(error)}
        if tree:
            receipt['stopped_owned_processes'] = tree.stop()
        elif proc.poll() is None:
            proc.terminate()
        code = proc.wait()
        raise
    finally:
        stop_observer.set()
        if thread is not None and thread.ident is not None:
            thread.join(timeout=10)
            if thread.is_alive():
                child_result['console_error'] = dict(type='TimeoutError',message='Child output did not close after owned-tree stop')
        if tree:
            receipt['compile_children'] = tree.receipt()
            tree.close()
        if observer is not None and observer.ident is not None:
            observer.join()
            receipt['compile_children'] = child_result
        if proc.stdout:
            proc.stdout.close()
        receipt.update({'ended_utc': now(), 'exit_code': code, 'duration_seconds': time.monotonic() - start})
        dump(path, receipt)
    if child_result.get('console_error'):
        raise RuntimeError('Child console reader failed')

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
    parser.add_argument('--kind', choices=['read_only', 'setup', 'ubt', 'build', 'runtime'], default='setup')
    parser.add_argument('operation')
    parser.add_argument('command', nargs=argparse.REMAINDER)
    args = parser.parse_args()
    if args.kind == 'runtime' and not SCOPE.get('runtime_authorized', False):
        raise RuntimeError('Runtime execution is not authorized in this job scope')
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
