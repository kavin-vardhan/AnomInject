import ctypes
from ctypes import wintypes
import datetime
import json
from pathlib import Path
import subprocess
import sys
import time


def short_child():
    kernel = ctypes.WinDLL('kernel32', use_last_error=True)
    kernel.GetCurrentProcess.restype = wintypes.HANDLE
    kernel.GetProcessTimes.argtypes = [wintypes.HANDLE]+[ctypes.POINTER(wintypes.FILETIME)]*4
    values = [wintypes.FILETIME() for _ in range(4)]
    assert kernel.GetProcessTimes(kernel.GetCurrentProcess(), *[ctypes.byref(value) for value in values])
    created = values[0].dwLowDateTime+(values[0].dwHighDateTime << 32)
    deadline = (created-116444736000000000)/10000000+0.100
    while time.time() < deadline:
        pass


def parent():
    assert sys.stdin.readline().strip() == 'go'
    for index in range(12):
        mode = '-Mode=ValidatePlatforms' if index % 2 == 0 else '-Mode=Build'
        proc = subprocess.Popen([sys.executable, __file__, 'child', '--fc6-ubt-standin', mode])
        print(proc.pid, flush=True)
        assert proc.wait() == 0
    time.sleep(0.1)


def main():
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from runtime_owned import RuntimeTree
    from runtime_children import reason, short_child_retry
    out = Path(sys.argv[1])
    out.mkdir(parents=True, exist_ok=False)
    proc = subprocess.Popen([sys.executable, __file__, 'parent'], stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE, text=True)
    tree = RuntimeTree(proc.pid, creation_log=out/'creations.jsonl')
    try:
        proc.stdin.write('go\n')
        proc.stdin.flush()
        pids = [int(line) for line in proc.stdout]
        assert proc.wait() == 0 and len(pids) == 12
        tree.poll()
        rows = []
        for index, pid in enumerate(pids):
            row = tree.known[pid]
            created, exited = tree.times(row['handle'])
            command = row['creation_command_line']
            observed = datetime.datetime.fromisoformat(row['command_observed_utc']).timestamp()
            delay = observed-(created-116444736000000000)/10000000
            lifetime = (exited-created)/10000000
            assert delay < lifetime and lifetime < 0.150, (delay, lifetime)
            rejected = reason(row['image'], command, rehearsal=True)
            assert bool(rejected) == bool(index % 2)
            rows.append(dict(pid=pid, command_line=command, lifetime_seconds=lifetime,
                             command_delay_seconds=delay, rejected=rejected))
        assert reason('UnrealBuildTool.exe', None)
        assert reason('UnrealBuildTool.exe', 'UnrealBuildTool.exe -Mode=Clean')
        assert reason('UnrealBuildTool.exe', 'UnrealBuildTool.exe Host Win64 Development')
        assert reason('UnrealBuildTool.exe', 'UnrealBuildTool.exe -Mode=Unknown')
        assert reason('UnrealBuildTool.exe', 'UnrealBuildTool.exe -Mode=QueryTargets') is None
        stopped = dict(exit_classification='owned-stop', reason='FE3 unreadable UnrealBuildTool command line')
        observations = [dict(reason=stopped['reason'], live=False, command_line=None)]
        binaries = dict(owned_binary='unchanged-hash')
        assert short_child_retry(stopped, observations, binaries, binaries, 0)
        assert not short_child_retry(stopped, observations, binaries, binaries, 1)
        assert not short_child_retry(stopped, observations, binaries, dict(owned_binary='changed'), 0)
        assert not short_child_retry(dict(stopped, reason='product gate failed'), observations, binaries, binaries, 0)
        assert not short_child_retry(stopped, [dict(observations[0], live=True)], binaries, binaries, 0)
        assert not tree.creation_error
        result = dict(result='PASS', children=rows, real_short_children=12, build_children_refused=6,
                      query_children_accepted=6, unreadable_refused=True,
                      retained=tree.receipt(), elevation=False, machine_changes=False)
        (out/'result.json').write_text(json.dumps(result, indent=2))
        print(json.dumps({key: value for key, value in result.items() if key not in ('children', 'retained')}))
    finally:
        if proc.poll() is None:
            tree.stop()
            proc.wait()
        tree.close()


if __name__ == '__main__':
    if sys.argv[1] == 'child':
        short_child()
    elif sys.argv[1] == 'parent':
        parent()
    else:
        main()
