"""EA6: fail-closed port idle gate. Source/build work does not call this gate."""
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
FLOORS = {'C': 10 * GIB, 'D': 50 * GIB, 'E': 50 * GIB}
FOREIGN = re.compile(r'unrealeditor|ue4editor|unrealgame|ue4game|shadercompileworker|automationtool|runuat|(?:^|[ /\\=])cook(?:[ .]|$)|stackobot|lyragame|stylizedparisstreet|[a-z0-9_]+-win64-(?:shipping|test|development)', re.I)

def reasons(sample, owned_pids=()):
    failures = []
    for name, predicate in [
        ('input_idle', lambda: sample['idle_seconds'] >= 300),
        ('physical_memory', lambda: sample['free_memory_bytes'] >= 6 * GIB),
        ('gpu', lambda: 0 <= sample['gpu_percent'] <= 15),
    ]:
        try:
            if not predicate():
                failures.append(name)
        except (KeyError, TypeError, ValueError):
            failures.append(name + ':unknown')
    owned = set(owned_pids)
    processes = sample.get('processes')
    if not isinstance(processes, list):
        failures.append('processes:unknown')
    else:
        for process in processes:
            if process.get('pid') in owned:
                continue
            text = process.get('image', '') + ' ' + process.get('command_line', '')
            if process.get('is_game') or FOREIGN.search(text):
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
    def __init__(self, started):
        self.started = started
        self.consecutive = 0
        self.previous_poll = None
        self.cleared = False

    def poll(self, now, sample, owned_pids=()):
        if now - self.started >= 4 * 3600:
            return {'status': 'NEEDS-DECISION', 'reason': 'no idle window', 'd2_attempt': False}
        failures = reasons(sample, owned_pids)
        spaced = self.previous_poll is None or now - self.previous_poll >= 24
        if failures:
            self.consecutive = 0
        elif spaced:
            self.consecutive += 1
        self.previous_poll = now
        self.cleared = self.consecutive >= 3
        return {'status': 'CLEAR' if self.cleared else 'WAIT', 'reasons': failures, 'consecutive': self.consecutive}

    def mid_leg(self, sample, owned_pids=()):
        failures = reasons(sample, owned_pids)
        valid = self.cleared and not failures
        if failures:
            self.cleared = False
            self.consecutive = 0
            self.previous_poll = None
        return {'valid': valid, 'reasons': failures, 'rerun_at_next_idle_window': bool(failures), 'd2_attempt': False}

def collect():
    class LastInput(ctypes.Structure):
        _fields_ = [('cbSize', wintypes.UINT), ('dwTime', wintypes.DWORD)]
    last = LastInput(ctypes.sizeof(LastInput), 0)
    if not ctypes.windll.user32.GetLastInputInfo(ctypes.byref(last)):
        raise ctypes.WinError()
    idle = ((ctypes.windll.kernel32.GetTickCount() - last.dwTime) & 0xffffffff) / 1000
    command = r'''$ErrorActionPreference='Stop';
$os=Get-CimInstance Win32_OperatingSystem;
$gpu=(Get-Counter '\GPU Engine(*)\Utilization Percentage' -ErrorAction Stop).CounterSamples;
if (-not $gpu) { throw 'No GPU counter samples' };
$procs=@(Get-CimInstance Win32_Process | ForEach-Object { @{pid=$_.ProcessId; image=$_.Name; command_line=[string]$_.CommandLine} });
@{free_memory_bytes=([long]$os.FreePhysicalMemory*1024);gpu_percent=[math]::Min(100,($gpu|Measure-Object CookedValue -Sum).Sum);processes=$procs}|ConvertTo-Json -Depth 5 -Compress'''
    result = subprocess.run(['powershell', '-NoProfile', '-NonInteractive', '-Command', command], capture_output=True, text=True, check=True, timeout=20)
    sample = json.loads(result.stdout)
    sample['idle_seconds'] = idle
    sample['volumes'] = {d: {'resolved_drive': Path(d + ':/').resolve(strict=True).drive[0].upper(), 'free_bytes': shutil.disk_usage(d + ':/').free} for d in FLOORS}
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
    return {'result': 'PASS', 'cases': results, 'runtime_probe_executed': False}

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--self-test', action='store_true')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--owned-pid', type=int, action='append', default=[])
    args = parser.parse_args()
    if args.self_test:
        args.output.write_text(json.dumps(self_test(), indent=2)+'\n', encoding='utf-8')
        print(args.output.read_text())
        return 0
    window = Window(time.monotonic())
    with args.output.open('x', encoding='utf-8') as log:
        while True:
            sample = collect()
            result = window.poll(time.monotonic(), sample, args.owned_pid)
            log.write(json.dumps({'utc': datetime.datetime.now(datetime.timezone.utc).isoformat(), 'sample': sample, 'result': result})+'\n'); log.flush()
            print(json.dumps(result), flush=True)
            if result['status']=='CLEAR': return 0
            if result['status']=='NEEDS-DECISION': return 20
            time.sleep(25)

if __name__ == '__main__':
    raise SystemExit(main())
