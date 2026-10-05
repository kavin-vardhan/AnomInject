import json
import subprocess
import time


def wait_owned(process, seconds):
    start = time.time()
    checks = []
    ps = r'C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe'
    query = "@(Get-CimInstance Win32_Process | Where-Object { $_.Name -match '^UnrealEditor|^UE4Editor' } | Select-Object ProcessId,Name,ExecutablePath) | ConvertTo-Json -Compress"
    while process.poll() is None and time.time() - start < seconds:
        result = subprocess.run([ps, '-NoProfile', '-Command', query], capture_output=True, text=True, timeout=40)
        if result.returncode or result.stderr:
            condition = dict(time=time.time(), error=result.stderr, output=result.stdout)
            checks.append(condition)
            process.terminate()
            process.wait()
            return dict(exit=-2, stopped_own_pid=process.pid, reason='foreign-editor observation unavailable', checks=checks)
        rows = json.loads(result.stdout or '[]')
        rows = rows if isinstance(rows, list) else [rows]
        foreign = [row for row in rows if row['ProcessId'] != process.pid]
        checks.append(dict(time=time.time(), foreign=foreign))
        if foreign:
            process.terminate()
            process.wait()
            return dict(exit=-3, stopped_own_pid=process.pid, reason='foreign editor appeared during leg', checks=checks)
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            pass
    if process.poll() is None:
        process.terminate()
        process.wait()
        return dict(exit=-1, stopped_own_pid=process.pid, reason='timeout', checks=checks)
    return dict(exit=process.returncode, checks=checks)
