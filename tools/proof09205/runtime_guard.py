import json
import subprocess
import time


def wait_owned(process, seconds):
    start = time.time()
    checks = []
    ps = r'C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe'
    query = "$samples=@(Get-CimInstance -ClassName Win32_PerfFormattedData_GPUPerformanceCounters_GPUEngine -ErrorAction Stop); @{editors=@(Get-CimInstance Win32_Process | Where-Object { $_.Name -match '^UnrealEditor|^UE4Editor' } | Select-Object ProcessId,Name,ExecutablePath); gpu_samples=$samples.Count; gpu=@($samples | Where-Object { $_.UtilizationPercentage -ge 10 } | ForEach-Object { $match=[regex]::Match($_.Name,'pid_(\\d+)'); $app=Get-Process -Id ([int]$match.Groups[1].Value) -ErrorAction SilentlyContinue; @{pid=[int]$match.Groups[1].Value; name=$app.ProcessName; percent=$_.UtilizationPercentage} })} | ConvertTo-Json -Depth 4 -Compress"
    while process.poll() is None and time.time() - start < seconds:
        try:
            result = subprocess.run([ps, '-NoProfile', '-Command', query], capture_output=True, text=True, timeout=40)
        except (subprocess.TimeoutExpired, OSError) as error:
            checks.append(dict(time=time.time(), error=str(error)))
            if process.poll() is None:
                process.terminate()
            process.wait()
            return dict(exit=-2, stopped_own_pid=process.pid, reason='foreign-editor observation unavailable', checks=checks)
        if result.returncode or result.stderr:
            condition = dict(time=time.time(), error=result.stderr, output=result.stdout)
            checks.append(condition)
            process.terminate()
            process.wait()
            return dict(exit=-2, stopped_own_pid=process.pid, reason='foreign-editor observation unavailable', checks=checks)
        observation = json.loads(result.stdout or '{}')
        rows = observation.get('editors', [])
        foreign = [row for row in rows if row['ProcessId'] != process.pid]
        gpu = [row for row in observation.get('gpu', []) if row['pid'] != process.pid and str(row.get('name', '')).lower() not in ('dwm', 'csrss', 'system', 'idle', 'explorer', 'searchhost', 'textinputhost', 'startmenuexperiencehost', 'shellexperiencehost')]
        checks.append(dict(time=time.time(), foreign=foreign, foreign_gpu=gpu, gpu_samples=observation.get('gpu_samples', 0)))
        if foreign or gpu or not observation.get('gpu_samples', 0):
            process.terminate()
            process.wait()
            return dict(exit=-3, stopped_own_pid=process.pid, reason='foreign editor/GPU work appeared or GPU observation unavailable', checks=checks)
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            pass
    if process.poll() is None:
        process.terminate()
        process.wait()
        return dict(exit=-1, stopped_own_pid=process.pid, reason='timeout', checks=checks)
    return dict(exit=process.returncode, checks=checks)
