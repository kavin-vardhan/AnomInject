import argparse
import datetime as dt
import hashlib
import json
import math
import os
from pathlib import Path
import shutil
import subprocess
import sys
import threading
import time

GIB = 1024 ** 3
LIMITS = {'C': (13, 11, 10), 'D': (40, 30, 25), 'E': (25, 20, 15)}


def reading():
    return {d: dict(free=shutil.disk_usage(d+':/').free, start=a*GIB, warning=b*GIB,
                    floor=c*GIB, resolved=str(Path(d+':/').resolve(strict=True)))
            for d, (a,b,c) in LIMITS.items()}


def decision(before, after, growth):
    affected = sorted(d for d in LIMITS if after[d]['free'] < after[d]['floor'])
    own = sorted(d for d in affected if before[d]['free'] >= before[d]['floor']
                 and before[d]['free'] - growth[d] < after[d]['floor'])
    return dict(result='STOP' if own else 'PAUSE' if affected else 'PASS', affected=affected,
                port_caused=own, before=before, current=after, port_growth_upper_bound=growth)


def write(path, row):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(row, indent=2)+'\n', encoding='utf-8')


def recover(affected, path, base, sampler=reading, sleeper=time.sleep, clock=time.monotonic,
            limit=4*3600, wall_remaining=9*3600, started_at=None):
    started = clock() if started_at is None else started_at
    consecutive, rows, watched = 0, [], set(affected)
    limit = min(limit, wall_remaining+(clock()-started))
    while True:
        current = sampler()
        watched.update(d for d in LIMITS if current[d]['free'] < current[d]['floor'])
        clear = all(current[d]['free'] > current[d]['warning'] for d in watched)
        consecutive = consecutive + 1 if clear else 0
        elapsed = clock()-started
        rows.append(dict(utc=dt.datetime.now(dt.timezone.utc).isoformat(), elapsed_seconds=elapsed,
                         disks=current, affected=sorted(watched), consecutive_above_warning=consecutive))
        state = 'RESUMED' if consecutive >= 3 else 'TIMEOUT' if elapsed >= limit else 'PAUSED'
        result = dict(base, state=state, polls=rows, elapsed_seconds=elapsed,
                      only_pause_receipt_written_while_paused=True, recovery_polls=3, poll_seconds=60)
        if state == 'TIMEOUT':
            result['reason'] = 'no disk window' if limit >= 4*3600 else '9-hour work limit'
        write(path, result)
        print(json.dumps({'EU2': state, 'wait_seconds': round(elapsed), 'affected': sorted(watched),
                          'free_GiB': {d: round(current[d]['free']/GIB, 3) for d in watched}}), flush=True)
        if state != 'PAUSED':
            return result
        sleeper(min(60, limit-elapsed))


class PreparationEnded(RuntimeError):
    def __init__(self, code):
        self.code = code
        super().__init__('EU2 preparation ended with status '+str(code))


class PreparationGuard:
    def __init__(self, start, output, remaining, sampler=reading, clock=time.monotonic,
                 sleeper=time.sleep):
        self.start, self.output, self.remaining = start, output, remaining
        self.sampler, self.clock, self.sleeper = sampler, clock, sleeper
        self.last = float('-inf')
        self.receipts = []
        self.polls = 0

    def poll(self, force=False):
        if not force and self.clock()-self.last < 1:
            return
        self.last = self.clock()
        current = self.sampler()
        self.polls += 1
        if self.remaining() <= 0:
            raise PreparationEnded(77)
        if not any(v['free'] < v['floor'] for v in current.values()):
            return
        owned = {d: 0 for d in LIMITS}
        for path in [self.output]+self.receipts:
            if path.exists():
                owned[path.drive[0].upper()] += math.ceil(path.stat().st_size/4096)*4096
        choice = decision(self.start, current, owned)
        path = self.output.with_name(self.output.stem+'-preparation-pause-'+str(len(self.receipts)+1)+'.json')
        self.receipts.append(path)
        base = dict(choice, phase='read-only baseline, before operation child creation',
                    stopped_owned_processes=[], operation_child_started=False,
                    own_writes_scope='Supervisor and pause receipts only; no operation child exists')
        if choice['result'] == 'STOP':
            write(path, dict(base, state='STOP', reason='Port-caused floor breach'))
            raise PreparationEnded(2)
        result = recover(choice['affected'], path, base, sampler=self.sampler,
                         sleeper=self.sleeper, clock=self.clock, wall_remaining=self.remaining(),
                         started_at=self.last)
        if result['state'] != 'RESUMED':
            raise PreparationEnded(76)
        self.last = float('-inf')
        self.poll(force=True)


def footprint(run, checkpoint=lambda:None, drives=None):
    local = Path('C:/Users/kavin/AppData/Local')
    roots = [Path('D:/IA_Europa'), run, Path('E:/EpicGames/UE_4.25/Engine/Intermediate/Build/BuildRules'),
             local/'UnrealBuildTool', local/'UnrealHeaderTool', local/'CrashReportClient',
             local/'UnrealEngine/4.25', Path('C:/Windows'), Path('C:/ProgramData/Microsoft/Windows/WER'),
             local/'Microsoft/Windows/WER', Path('D:/UESource/UnrealEngine/Engine/Intermediate/Build')]
    engine = Path('D:/UESource/UnrealEngine/Engine')
    for directory, directories, filenames in os.walk(engine/'Plugins', followlinks=False):
        checkpoint()
        directories[:] = [name for name in directories if name in ('Intermediate','Build')
                          or 'Intermediate' not in Path(directory).parts]
        if Path(directory).parts[-2:] == ('Intermediate','Build'):
            roots.append(Path(directory))
            directories[:] = []
    roots.append(engine/'Intermediate/UbtRuns')
    files, errors = {}, []
    for root in roots:
        checkpoint()
        if drives is not None and root.drive[0].upper() not in drives:
            continue
        if not root.exists():
            continue
        stack = [root]
        while stack:
            checkpoint()
            directory = stack.pop()
            try:
                with os.scandir(directory) as entries:
                    for item in entries:
                        checkpoint()
                        try:
                            stat = item.stat(follow_symlinks=False)
                            if stat.st_file_attributes & 1024:
                                continue
                            if item.is_dir(follow_symlinks=False):
                                stack.append(Path(item.path))
                            elif item.is_file(follow_symlinks=False):
                                key = str(Path(item.path).absolute()).casefold()
                                files[key] = int(math.ceil(stat.st_size/4096))*4096
                        except OSError as error:
                            errors.append(dict(path=item.path, type=type(error).__name__))
            except OSError as error:
                errors.append(dict(path=str(directory), type=type(error).__name__))
    if drives is None or 'C' in drives:
        for p in (local/'UnrealEngine').glob('XmlConfigCache-*.bin'):
            checkpoint()
            files[str(p).casefold()] = int(math.ceil(p.stat().st_size/4096))*4096
    return files, errors


def growth(before, after):
    values = {d: 0 for d in LIMITS}
    for path, size in after.items():
        drive = path[0].upper()
        if drive in values:
            values[drive] += max(0, size-before.get(path, 0))
    return values


def checkpoint(data):
    if not os.environ.get('EUROPA_DISK_SUPERVISOR_PID'):
        return False
    pid = int(os.environ['EUROPA_DISK_SUPERVISOR_PID'])
    import ctypes
    from ctypes import wintypes
    kernel = ctypes.WinDLL('kernel32', use_last_error=True)
    kernel.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
    kernel.OpenProcess.restype = wintypes.HANDLE
    kernel.WaitForSingleObject.argtypes = [wintypes.HANDLE, wintypes.DWORD]
    kernel.CloseHandle.argtypes = [wintypes.HANDLE]
    kernel.GetProcessTimes.argtypes = [wintypes.HANDLE]+[ctypes.POINTER(wintypes.FILETIME)]*4
    handle = kernel.OpenProcess(0x100000 | 0x1000, False, pid)
    if not handle:
        raise RuntimeError('EU2 disk supervisor unavailable')
    try:
        values = [wintypes.FILETIME() for _ in range(4)]
        if not kernel.GetProcessTimes(handle, *[ctypes.byref(v) for v in values]):
            raise RuntimeError('EU2 supervisor identity unavailable')
        created = values[0].dwLowDateTime+(values[0].dwHighDateTime<<32)
        if created != int(os.environ['EUROPA_DISK_SUPERVISOR_CREATED']):
            raise RuntimeError('EU2 supervisor PID reused')
        while any(v['free'] < v['floor'] for v in data.values()):
            if kernel.WaitForSingleObject(handle, 0) == 0:
                raise RuntimeError('EU2 disk supervisor exited during a dip')
            time.sleep(0.1)
            data = reading()
    finally:
        kernel.CloseHandle(handle)
    return True


def acknowledge_restart(pending, cleanup_receipt, ledger_summary, receipt):
    from job_scope import load
    job = Path(load()['job_root']).resolve(strict=True)
    if pending.resolve(strict=True) != job/'disk-restart-required.json' or any(
        not p.resolve().is_relative_to(job) for p in (cleanup_receipt,ledger_summary,receipt)):
        raise RuntimeError('EU2 recovery receipts outside scoped job')
    data = json.loads(pending.read_text())
    if data['result'] != 'RESTART_REQUIRED':
        raise RuntimeError('EU2 not a recovered interruption')
    if any(v['free'] < v['floor'] for v in reading().values()):
        raise RuntimeError('EU2 recovery must remain above the floor')
    summary = json.loads(ledger_summary.read_text())
    if summary['result'] != 'PASS':
        raise RuntimeError('Interrupted operation has not passed its ledger closeout')
    cleanup = json.loads(cleanup_receipt.read_text())
    if cleanup.get('result') != 'PASS' or cleanup.get('dry_run') or cleanup.get('removed_count') != len(cleanup.get('files', [])):
        raise RuntimeError('EU2 requires completed, receipted partial-output cleanup')
    host = Path(cleanup['host']).resolve(strict=True)
    allowed = {Path('D:/IA_Europa')/p for p in ('StylizedParisStreet','Baseline51/StackOBot','ReGate51/StackOBot')}
    if host not in allowed:
        raise RuntimeError('EU2 cleanup host outside allowed hosts')
    for row in cleanup['files']:
        path = Path(row['path'])
        if not path.resolve().is_relative_to(host) or path.exists():
            raise RuntimeError('EU2 partial output was not safely removed')
    write(receipt, dict(result='PASS', pending=data, pending_path=str(pending),
        pending_sha256=hashlib.sha256(pending.read_bytes()).hexdigest(), cleanup=str(cleanup_receipt),
        ledger_summary=str(ledger_summary), restart_from_beginning_required=True, counts_as_D2_attempt=False))
    pending.unlink()


def supervise(argv, output, config, run, kind, recovery=False):
    from build_memory import OwnedTree
    from environment import filtered, forbidden_child_name, masked
    assert not any(forbidden_child_name(k) for k in os.environ)
    scope = json.loads(config.read_text())
    job = Path(scope['job_root'])
    pending = job/'disk-restart-required.json'
    recovery_tool = Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/disk_recovery.py')
    if recovery and not (len(argv)==3 and Path(argv[0]).resolve()==Path(sys.executable).resolve()
                         and argv[1]=='-B' and Path(argv[2]).resolve()==recovery_tool.resolve()):
        raise RuntimeError('EU2 recovery admits only the scoped recovery tool')
    if pending.exists() and not recovery:
        raise RuntimeError('EU2 previous interruption requires receipted cleanup and a fresh attempt')
    if output.exists():
        raise RuntimeError('Refusing reused disk supervisor receipt')
    started = dt.datetime.fromisoformat(scope['started_utc'])
    def remaining():
        return max(0, scope['wall_limit_hours']*3600-(dt.datetime.now(dt.timezone.utc)-started).total_seconds())
    pause_path = output.with_name(output.stem+'-pause.json')
    start_free = reading()
    low = sorted(d for d,v in start_free.items() if v['free'] < v['floor'])
    if low:
        recovered = recover(low, pause_path, dict(result='PAUSE', reason='Pre-existing dip; operation has not started',
            before=start_free, port_growth_upper_bound={d:0 for d in LIMITS}, stopped_owned_processes=[]),
            wall_remaining=remaining())
        if recovered['state']!='RESUMED':
            return 76
        start_free = reading()
    if remaining()<=0:
        write(output, dict(result='NEEDS-DECISION', reason='9-hour work limit', launches=0))
        return 77
    preparation = PreparationGuard(start_free, output, remaining)
    initial, errors = footprint(run, preparation.poll)
    footprint_hash = hashlib.sha256()
    for path in sorted(initial):
        preparation.poll()
        footprint_hash.update((json.dumps([path,initial[path]])+'\n').encode())
    initial_summary = dict(files=len(initial), bytes=sum(initial.values()), errors=errors,
        sha256=footprint_hash.hexdigest(),
        scope='Port tree including ledger; run folder; both engines build bookkeeping; C Windows/WER and admitted UE paths. Positive allocated-size deltas never netted against deletions; foreign writes in this scope are conservatively charged.')
    preparation.poll(force=True)
    env = filtered(os.environ)
    identity = OwnedTree(os.getpid())
    try:
        stamp = identity.known[os.getpid()]['created_filetime']
    finally:
        identity.close()
    env.update(EUROPA_JOB_CONFIG=str(config), EUROPA_DISK_SUPERVISOR_PID=str(os.getpid()),
               EUROPA_DISK_SUPERVISOR_CREATED=str(stamp))
    row = dict(result='RUNNING', argv=argv, kind=kind, start_disks=start_free, footprint=initial_summary,
               started_utc=dt.datetime.now(dt.timezone.utc).isoformat(), environment=masked(env),
               wrapper_pid=os.getpid(), authority='094-19-ruling EU2', launches_of_unreal_runtime=0,
               preparation_polls=preparation.polls, preparation_pause_receipts=[str(p) for p in preparation.receipts])
    preparation.poll(force=True)
    write(output, row)
    preparation.poll(force=True)
    proc = subprocess.Popen(argv, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, encoding='utf-8', errors='replace')
    tree = OwnedTree(proc.pid)
    row['pid'] = proc.pid
    write(output, row)
    def consume():
        for line in proc.stdout:
            print(line.rstrip(), flush=True)
    thread = threading.Thread(target=consume)
    thread.start()
    try:
        while True:
            tree.poll()
            current = reading()
            if any(v['free'] < v['floor'] for v in current.values()):
                detected_at = time.monotonic()
                detected_utc = dt.datetime.now(dt.timezone.utc).isoformat()
                stopped = tree.stop()
                proc.wait()
                thread.join()
                write(pause_path,dict(state='ATTRIBUTING', detected_utc=detected_utc, current=current,
                                     stopped_owned_processes=stopped, only_pause_receipt_written_while_paused=True))
                affected = {d for d,v in current.items() if v['free'] < v['floor']}
                after, after_errors = footprint(run, drives=affected)
                choice = decision(start_free, current, growth(initial, after))
                base = dict(choice, detected_utc=detected_utc, stopped_owned_processes=stopped, owned_identity=tree.receipt(),
                            scope=initial_summary, scan_errors_after=after_errors, interrupted_argv=argv,
                            interrupted_kind=kind, partial_outputs_usable=False, counts_as_D2_attempt=False)
                if choice['result'] == 'STOP':
                    write(pause_path, dict(base, state='STOP', reason='Port-caused floor breach'))
                    return 2
                recovered = recover(choice['affected'], pause_path, base, wall_remaining=remaining(), started_at=detected_at)
                if recovered['state'] != 'RESUMED':
                    return 76
                if not recovery:
                    write(pending, dict(result='RESTART_REQUIRED', supervisor=str(output), pause=str(pause_path),
                        argv=argv, kind=kind, cleanup_required=True, restart_from_beginning=True))
                row.update(result='RESTART_REQUIRED', exit_code=proc.returncode, pause=str(pause_path))
                write(output, row)
                return 75
            if proc.poll() is not None:
                break
            time.sleep(1)
        thread.join()
        row.update(result='PASS' if proc.returncode == 0 else 'CHILD_FAILED', exit_code=proc.returncode,
                   ended_utc=dt.datetime.now(dt.timezone.utc).isoformat(), final_disks=reading(), owned_identity=tree.receipt())
        write(output, row)
        return proc.returncode
    finally:
        if proc.poll() is None:
            tree.stop()
            proc.wait()
        thread.join()
        proc.stdout.close()
        tree.close()


def self_tests(output):
    rows = []
    def test(name, passed):
        rows.append(dict(name=name, passed=bool(passed)))
        assert passed, name
    def disks(d):
        return {key: dict(free=(d if key=='D' else 30)*GIB, floor=v[2]*GIB, warning=v[1]*GIB)
                for key,v in LIMITS.items()}
    before = disks(74)
    small = {'C':0, 'D':GIB, 'E':0}
    test('foreign_drop_pauses', decision(before, disks(LIMITS['D'][2]-6), small)['result']=='PAUSE')
    test('port_growth_alone_stops', decision(before, disks(LIMITS['D'][2]-6), dict(small, D=(74-LIMITS['D'][2]+1)*GIB))['result']=='STOP')
    test('healthy_does_not_pause', decision(before, disks(60), small)['result']=='PASS')
    test('preexisting_drop_before_any_port_write_pauses', decision(disks(LIMITS['D'][2]-6), disks(LIMITS['D'][2]-6), {d:0 for d in LIMITS})['result']=='PAUSE')
    test('deletions_do_not_hide_growth', growth({'d:/a':10000}, {'d:/b':20000})['D']==20000)
    fake_time = [0]
    def sleep(seconds):
        fake_time[0] += seconds
    sequence = iter([disks(LIMITS['D'][2]-6), disks(LIMITS['D'][1]+1), disks(LIMITS['D'][1]+1), disks(LIMITS['D'][1]-1), disks(LIMITS['D'][1]+1), disks(LIMITS['D'][1]+1), disks(LIMITS['D'][1]+1)])
    result = recover(['D'], output.with_name('foreign-recovery.json'), {}, sampler=lambda:next(sequence),
                     sleeper=sleep, clock=lambda:fake_time[0])
    test('three_spaced_clear_polls_and_reset', result['state']=='RESUMED' and fake_time[0]==360)
    fake_time[0] = 0
    result = recover(['D'], output.with_name('no-disk-window.json'), {}, sampler=lambda:disks(LIMITS['D'][2]-6),
                     sleeper=sleep, clock=lambda:fake_time[0])
    test('four_hour_timeout', result['state']=='TIMEOUT' and result['reason']=='no disk window' and fake_time[0]==14400)
    fake_time[0] = 14340
    result = recover(['D'], output.with_name('attribution-time.json'), {}, sampler=lambda:disks(LIMITS['D'][2]-6),
                     sleeper=sleep, clock=lambda:fake_time[0], started_at=0)
    test('attribution_time_in_four_hour_limit', result['state']=='TIMEOUT' and fake_time[0]==14400)
    fake_time[0] = 0
    result = recover(['D'], output.with_name('wall-limit.json'), {}, sampler=lambda:disks(LIMITS['D'][2]-6),
                     sleeper=sleep, clock=lambda:fake_time[0], wall_remaining=120)
    test('pauses_count_towards_wall_limit', result['state']=='TIMEOUT' and fake_time[0]==120)
    from build_memory import OwnedTree
    from environment import filtered
    env = filtered(os.environ)
    control = subprocess.Popen([sys.executable, '-B', '-c', 'import time; time.sleep(60)'], env=env)
    own = subprocess.Popen([sys.executable, '-B', '-c',
        'import subprocess,sys,time; p=subprocess.Popen([sys.executable,"-B","-c","import time; time.sleep(60)"]); print(p.pid,flush=True); time.sleep(60)'],
        env=env, stdout=subprocess.PIPE, text=True)
    descendant = int(own.stdout.readline())
    tree = OwnedTree(own.pid)
    try:
        stopped = tree.stop()
        own.wait()
        test('drop_stops_only_our_retained_PID_tree', {r['pid'] for r in stopped}=={own.pid,descendant} and control.poll() is None)
    finally:
        if own.poll() is None:
            tree.stop()
            own.wait()
        tree.close()
        own.stdout.close()
        if control.poll() is None:
            control.terminate()
        control.wait()
    fake_time[0] = 0
    sequence = iter([disks(LIMITS['D'][2]-6), disks(LIMITS['D'][2]-6), disks(LIMITS['D'][1]+1), disks(LIMITS['D'][1]+1), disks(LIMITS['D'][1]+1), disks(LIMITS['D'][1]+1)])
    preparation = PreparationGuard(before, output.with_name('preparation-supervisor.json'),
        lambda:9*3600, sampler=lambda:next(sequence), clock=lambda:fake_time[0], sleeper=sleep)
    preparation.poll(force=True)
    test('baseline_checkpoint_pauses_before_child_until_three_clear_polls',
         fake_time[0]==180 and len(preparation.receipts)==1 and
         json.loads(preparation.receipts[0].read_text())['state']=='RESUMED')
    source = Path(__file__).read_text()
    test('baseline_scan_and_hash_loop_invoke_checkpoint',
         'initial, errors = footprint(run, preparation.poll)' in source and
         'for path in sorted(initial):\n        preparation.poll()' in source and
         'for item in entries:\n                        checkpoint()' in source)
    result = dict(result='PASS', tests=rows, policy='EU2', D2_attempt=2, actual_disk_changes=False,
                  stopped_test_pids=[own.pid,descendant], unselected_control_pid=control.pid,
                  control_cleanup='Separately closed our own harmless control child by its retained Popen handle after proving it survived the tree stop',
                  no_unreal_launch=True)
    write(output, result)
    return result


if __name__ == '__main__':
    sys.path.insert(0, 'D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa')
    p = argparse.ArgumentParser()
    p.add_argument('--self-test', action='store_true')
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--job-config', type=Path)
    p.add_argument('--run-folder', type=Path)
    p.add_argument('--kind', default='setup')
    p.add_argument('--recovery', action='store_true')
    p.add_argument('command', nargs=argparse.REMAINDER)
    a = p.parse_args()
    if a.self_test:
        print(json.dumps(self_tests(a.output)))
    else:
        args = a.command[1:] if a.command[:1]==['--'] else a.command
        try:
            raise SystemExit(supervise(args, a.output, a.job_config, a.run_folder, a.kind, a.recovery))
        except PreparationEnded as stopped:
            raise SystemExit(stopped.code)
