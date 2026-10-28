import argparse
import ctypes
from ctypes import wintypes
import datetime
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time

GIB = 1024**3
FLOORS = {'C': 10 * GIB, 'D': 25 * GIB, 'E': 15 * GIB}
FOREIGN = re.compile(r'unrealeditor|ue4editor|unrealgame|ue4game|shadercompileworker|automationtool|runuat|(?:^|[ /\\=])cook(?:[ .]|$)|stackobot|lyragame|stylizedparisstreet|[a-z0-9_]+-win64-(?:shipping|test|development)', re.I)

def descendants(processes, roots):
    owned = set(roots)
    if not isinstance(processes, list):
        return owned
    changed = True
    while changed:
        changed = False
        for process in processes:
            pid = process.get('pid')
            if process.get('parent_pid') in owned and pid not in owned:
                owned.add(pid)
                changed = True
    return owned


def launcher_tree(processes):
    if not isinstance(processes,list):return []
    by_pid={p['pid']:p for p in processes if isinstance(p.get('pid'),int)}
    roots={pid for pid,p in by_pid.items() if Path(p.get('image','')).name.casefold()=='epicgameslauncher.exe'}
    excluded=[]
    for pid,p in by_pid.items():
        ancestry=[];seen=set();current=pid
        while current in by_pid and current not in seen:
            seen.add(current);ancestry.append(current)
            if current in roots:
                excluded.append(dict(p,launcher_root_pid=current,ancestry=ancestry,
                    exclusion='094-22-ruling EX1: current launcher process ancestry'))
                break
            parent=by_pid[current].get('parent_pid')
            if parent in by_pid:
                child_created=by_pid[current].get('created_utc')
                parent_created=by_pid[parent].get('created_utc')
                if child_created and parent_created and child_created<parent_created:break
            current=parent
    return sorted(excluded,key=lambda p:p['pid'])


def validate_profile(profile, human_idle=None):
    if profile not in ('measured', 'owner-preview'):
        raise RuntimeError('Unknown idle-gate profile')
    expected = profile == 'measured'
    if human_idle is not None and human_idle is not expected:
        raise RuntimeError('Measured legs require human idle; only the owner preview omits it')
    return expected


def reasons(sample, owned_pids=(), profile='measured', require_launcher=False):
    human_idle = validate_profile(profile)
    failures = []
    for name, predicate in [
        ('input_idle', lambda: sample['idle_seconds'] >= 300),
        ('physical_memory', lambda: sample['free_memory_bytes'] >= 6 * GIB),
        ('gpu', lambda: 0 <= sample['gpu_percent'] <= 15),
    ]:
        if name == 'input_idle' and not human_idle:
            continue
        try:
            if not predicate():
                failures.append(name)
        except (KeyError, TypeError, ValueError):
            failures.append(name + ':unknown')
    owned = descendants(sample.get('processes'), owned_pids)
    processes = sample.get('processes')
    if not isinstance(processes, list):
        failures.append('processes:unknown')
    else:
        if require_launcher and not any(Path(p.get('image','')).name.casefold() == 'epicgameslauncher.exe' for p in processes):
            failures.append('launcher')
        excluded={p['pid'] for p in launcher_tree(processes)}
        sample['launcher_tree_excluded']=launcher_tree(processes)
        for process in processes:
            if process.get('pid') in owned or process.get('pid') in excluded:
                continue
            text = process.get('image', '')
            if Path(text).name.casefold() in ('unrealbuildtool.exe','unrealheadertool.exe'):
                failures.append('foreign_process:' + str(process.get('pid')))
                continue
            if profile == 'owner-preview' and Path(text).name.casefold() in ('unrealtraceserver.exe', 'unrealversionselector.exe'):
                continue
            if process.get('is_game') or process.get('blocked_runtime') or FOREIGN.search(text) or (profile == 'owner-preview' and re.search(r'unreal|ue4', text, re.I)):
                failures.append('foreign_process:' + str(process.get('pid')))
    for drive, minimum in FLOORS.items():
        try:
            value = sample['volumes'][drive]
            if value['resolved_drive'].upper() != drive or value['free_bytes'] < minimum:
                failures.append('disk:' + drive)
        except (KeyError, TypeError, ValueError):
            failures.append('disk:' + drive + ':unknown')
    return failures

class Window:
    def __init__(self, started, profile='measured', human_idle=None, require_launcher=False):
        validate_profile(profile, human_idle)
        self.profile = profile
        self.require_launcher = require_launcher
        self.started = started
        self.consecutive = 0
        self.previous_poll = None
        self.cleared = False

    def poll(self, now, sample, owned_pids=()):
        if now - self.started >= (2700 if self.profile == 'owner-preview' else 4 * 3600):
            return {'status': 'NEEDS-DECISION', 'reason': 'no idle window', 'd2_attempt': False}
        failures = reasons(sample, owned_pids, self.profile, self.require_launcher)
        spaced = self.previous_poll is None or now - self.previous_poll >= 24
        if failures:
            self.consecutive = 0
        elif spaced:
            self.consecutive += 1
        self.previous_poll = now
        self.cleared = self.consecutive >= 3
        return {'status': 'CLEAR' if self.cleared else 'WAIT', 'reasons': failures, 'consecutive': self.consecutive}

    def mid_leg(self, sample, owned_pids=()):
        failures = reasons(sample, owned_pids, self.profile, self.require_launcher)
        valid = self.cleared and not failures
        if failures:
            self.cleared = False
            self.consecutive = 0
            self.previous_poll = None
        return {'valid': valid, 'reasons': failures, 'rerun_at_next_idle_window': bool(failures), 'd2_attempt': False}

def read_idle():
    class LastInput(ctypes.Structure):
        _fields_ = [('cbSize', wintypes.UINT), ('dwTime', wintypes.DWORD)]
    last = LastInput(ctypes.sizeof(LastInput), 0)
    if not ctypes.windll.user32.GetLastInputInfo(ctypes.byref(last)):
        raise ctypes.WinError()
    return ((ctypes.windll.kernel32.GetTickCount() - last.dwTime) & 0xffffffff) / 1000


def powershell_json(command):
    from environment import execution_env
    env, contract = execution_env(os.environ)
    result = subprocess.run(['powershell', '-NoProfile', '-NonInteractive', '-Command',
        "$ErrorActionPreference='Stop'; " + command], capture_output=True, text=True,
        check=True, timeout=20, env=env)
    return json.loads(result.stdout)


def read_memory():
    from build_memory import sample
    return sample()['free_physical']


def read_gpu():
    return powershell_json(r"$gpu=(Get-Counter '\GPU Engine(*)\Utilization Percentage' -ErrorAction Stop).CounterSamples; if (-not $gpu) { throw 'No GPU counter samples' }; [math]::Min(100,($gpu|Measure-Object CookedValue -Sum).Sum)|ConvertTo-Json -Compress")


def process_record(row):
    image = row.get('image', '')
    command = row.get('command_line') or ''
    blocked = bool(re.search(r'UnrealEditor|UE4Editor|ShaderCompileWorker|AutomationTool|RunUAT|UnrealBuildTool|UnrealHeaderTool|UnrealGame|UE4Game|StackOBot|StylizedParisStreet|LyraGame|.+-Win64-(Shipping|Development|Test)', image, re.I)
        or re.search(r'-run=cook|AutomationTool\.dll|RunUAT\.bat', command, re.I)
        or (Path(image).name.casefold() == 'dotnet.exe' and re.search(r'(?:^|[\\/\s"\'])UnrealBuildTool(?:\.dll|\.exe)?(?:[\s"\']|$)', command, re.I)))
    return dict(pid=row['pid'], parent_pid=row['parent_pid'], image=image,
        created=row.get('created'), created_utc=row.get('created_utc'), blocked_runtime=blocked)


def read_processes():
    rows = powershell_json(r"$rows=@(Get-CimInstance Win32_Process | ForEach-Object { @{pid=$_.ProcessId;parent_pid=$_.ParentProcessId;image=$_.Name;created=[string]$_.CreationDate;created_utc=$(if ($_.CreationDate) { $_.CreationDate.ToUniversalTime().ToString('o') } else { $null });command_line=$_.CommandLine} }); ConvertTo-Json -InputObject $rows -Depth 4 -Compress")
    if not isinstance(rows, list) or not rows:
        raise ValueError('Running-program reading is not a nonempty list')
    return [process_record(row) for row in rows]


def read_volume(drive):
    return dict(resolved_drive=Path(drive+':/').resolve(strict=True).drive[0].upper(),
        free_bytes=shutil.disk_usage(drive+':/').free)


def reading_error(error):
    row = dict(type=type(error).__name__)
    if isinstance(error, subprocess.CalledProcessError):
        row['returncode'] = error.returncode
    if isinstance(error, subprocess.TimeoutExpired):
        row['timeout_seconds'] = error.timeout
    return row


def collect(readers=None):
    readers = readers or dict(input_idle=read_idle, physical_memory=read_memory,
        gpu=read_gpu, processes=read_processes,
        **{'disk:'+d: (lambda d=d: read_volume(d)) for d in FLOORS})
    sample = dict(idle_seconds=None, free_memory_bytes=None, gpu_percent=None,
        processes=None, volumes={}, reading_errors={})
    fields = dict(input_idle='idle_seconds', physical_memory='free_memory_bytes',
        gpu='gpu_percent', processes='processes')
    for name in (*fields, *('disk:'+d for d in FLOORS)):
        try:
            value = readers[name]()
            if name.startswith('disk:'):
                sample['volumes'][name[-1]] = value
            else:
                sample[fields[name]] = value
        except Exception as error:
            sample['reading_errors'][name] = reading_error(error)
    sample['launcher_tree_excluded']=launcher_tree(sample['processes'])
    return sample


def self_test():
    import copy
    good = {'idle_seconds': 300, 'free_memory_bytes': 6 * GIB, 'gpu_percent': 15, 'processes': [], 'volumes': {d: {'resolved_drive': d, 'free_bytes': n} for d,n in FLOORS.items()}}
    results = []
    def check(name, condition):
        results.append({'case': name, 'pass': bool(condition)})
        if not condition:
            raise AssertionError(name)
    check('thresholds_inclusive', not reasons(good))
    for key, value in [('idle_seconds', 299.99), ('free_memory_bytes', 6*GIB-1), ('gpu_percent', 15.01)]:
        bad = copy.deepcopy(good); bad[key] = value
        check(key + '_can_fail', bool(reasons(bad)))
        del bad[key]
        check(key + '_unknown_fails', bool(reasons(bad)))
    for name in ['UnrealEditor.exe', 'UE4Editor.exe', 'StackOBot.exe', 'AutomationTool.exe', 'RunUAT.bat', 'ShaderCompileWorker.exe', 'UE4Editor-Cmd.exe -run=cook', 'UnrealGame.exe', 'UE4Game.exe', 'OtherGame-Win64-Shipping.exe']:
        bad = copy.deepcopy(good); bad['processes'] = [{'pid': 42, 'image': name}]
        check(name + '_can_fail', bool(reasons(bad)))
        check(name + '_owned_allowed', not reasons(bad, [42]))
    bad = copy.deepcopy(good); bad['processes'] = [{'pid': 44, 'is_game': True}]
    check('other_game_can_fail', bool(reasons(bad)))
    bad['processes'] = None
    check('process_probe_unknown_fails', bool(reasons(bad)))
    for drive in FLOORS:
        bad = copy.deepcopy(good); bad['volumes'][drive]['free_bytes'] -= 1
        check(drive + '_floor_can_fail', bool(reasons(bad)))
        bad = copy.deepcopy(good); bad['volumes'][drive]['resolved_drive'] = 'X'
        check(drive + '_resolved_volume_can_fail', bool(reasons(bad)))
    window = Window(0)
    check('one_poll_not_clear', window.poll(0,good)['status']=='WAIT')
    check('two_polls_not_clear', window.poll(25,good)['status']=='WAIT')
    check('three_spaced_polls_clear', window.poll(50,good)['status']=='CLEAR')
    bad = copy.deepcopy(good); bad['gpu_percent'] = 16
    check('mid_leg_loss_invalidates', not window.mid_leg(bad)['valid'])
    check('mid_leg_loss_not_d2', not window.mid_leg(bad)['d2_attempt'])
    check('mid_leg_cannot_reuse_old_clearance', not window.mid_leg(good)['valid'])
    check('failure_resets_streak', window.poll(75,bad)['consecutive']==0)
    check('after_reset_not_clear', window.poll(100,good)['status']=='WAIT')
    window = Window(0)
    window.poll(0,good); window.poll(1,good)
    check('rapid_poll_cannot_clear', window.poll(2,good)['status']=='WAIT')
    result = window.poll(14400,good)
    check('timeout_needs_decision', result['status']=='NEEDS-DECISION' and result['reason']=='no idle window' and not result['d2_attempt'])
    bad = copy.deepcopy(good)
    bad['processes'] = [{'pid': 10, 'parent_pid': 1, 'image': 'python.exe'},
                        {'pid': 11, 'parent_pid': 10, 'image': 'UE4Editor.exe'},
                        {'pid': 12, 'parent_pid': 11, 'image': 'ShaderCompileWorker.exe'}]
    check('own_descendants_exempt', not reasons(bad, [10]))
    bad['processes'].append({'pid': 20, 'parent_pid': 1, 'image': 'UE4Editor.exe'})
    check('foreign_sibling_not_exempt', bool(reasons(bad, [10])))
    bad['processes'] = [{'pid': 20, 'parent_pid': 1, 'image': 'dotnet.exe', 'blocked_runtime': True}]
    check('uat_command_classification_without_command_storage', bool(reasons(bad)))
    return {'result': 'PASS', 'cases': results, 'runtime_probe_executed': False}

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--self-test', action='store_true')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--owned-pid', type=int, action='append', default=[])
    parser.add_argument('--observe-seconds', type=int, default=0)
    args = parser.parse_args()
    if args.self_test:
        args.output.write_text(json.dumps(self_test(), indent=2)+'\n', encoding='utf-8')
        print(args.output.read_text())
        return 0
    if args.observe_seconds and args.observe_seconds < 900:
        parser.error('Observe-only qualification requires at least 900 seconds')
    roots = args.owned_pid + [os.getpid()]
    started = time.monotonic()
    window = Window(started)
    with args.output.open('x', encoding='utf-8') as log:
        while True:
            sample = collect()
            result = window.poll(time.monotonic(), sample, roots)
            log.write(json.dumps({'utc': datetime.datetime.now(datetime.timezone.utc).isoformat(), 'sample': sample, 'result': result})+'\n'); log.flush()
            print(json.dumps(result), flush=True)
            if args.observe_seconds:
                if time.monotonic() - started >= args.observe_seconds: return 0
            elif result['status']=='CLEAR': return 0
            if result['status']=='NEEDS-DECISION': return 20
            time.sleep(25)

if __name__ == '__main__':
    raise SystemExit(main())
