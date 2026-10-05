import ctypes
import ctypes.wintypes
import json
import shutil
import subprocess
import sys
import time
from pathlib import Path

work = Path('E:/IA_BuildCache/_r921/09203c')
ps = r'C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe'


class LastInput(ctypes.Structure):
    _fields_ = [('cbSize', ctypes.wintypes.UINT), ('dwTime', ctypes.wintypes.DWORD)]


def read():
    sample = dict(time=time.time())
    fast_command = "@(Get-Process | Where-Object { $_.ProcessName -match '^(UnrealEditor|UE4Editor|StackOBot|LyraGame|cl|link)(-|$)' } | ForEach-Object { @{ProcessId=$_.Id; Name=($_.ProcessName+'.exe'); ExecutablePath=$_.Path} }) | ConvertTo-Json -Depth 4 -Compress"
    try:
        fast = subprocess.run([ps, '-NoProfile', '-Command', fast_command], capture_output=True, text=True, timeout=15)
        if fast.returncode or fast.stderr:
            return dict(sample, clear=False, error=fast.stderr, output=fast.stdout, observation='fast process query')
        blockers = json.loads(fast.stdout) if fast.stdout.strip() else []
        if isinstance(blockers, dict):
            blockers = [blockers]
        if blockers:
            return dict(sample, clear=False, processes=blockers, observation='early rejection on a blocking process')
    except (subprocess.TimeoutExpired, OSError, ValueError) as error:
        return dict(sample, clear=False, error=str(error), observation='fast process query')
    command = "$gateProcesses=Get-CimInstance Win32_Process; @{processes=@($gateProcesses | Where-Object { $_.Name -match '^UnrealEditor|^UE4Editor|^StackOBot.exe$|^LyraGame.exe$|^cl.exe$|^link.exe$' -or ($_.Name -eq 'python.exe' -and $_.CommandLine -match '_r921.*09203c.*(strict_build|strict_driver|build)\\.py') } | Select-Object ProcessId,Name,ExecutablePath); background_gpu=@($gateProcesses | Where-Object { $_.Name -eq 'blender.exe' -and $_.CommandLine -match '\\s(-b|--background)(\\s|$)' } | Select-Object ProcessId,Name); memory_gib=((Get-CimInstance Win32_OperatingSystem).FreePhysicalMemory/1048576)} | ConvertTo-Json -Depth 4 -Compress"
    try:
        result = subprocess.run([ps, '-NoProfile', '-Command', command], capture_output=True, text=True, timeout=40)
    except (subprocess.TimeoutExpired, OSError) as error:
        return dict(sample, clear=False, error=str(error), observation='full process/memory query')
    sample['command_exit'] = result.returncode
    if result.returncode or result.stderr:
        sample.update(clear=False, error=result.stderr, output=result.stdout)
        return sample
    try:
        sample.update(json.loads(result.stdout))
    except ValueError as error:
        return dict(sample, clear=False, error=str(error), output=result.stdout)
    idle = LastInput()
    idle.cbSize = ctypes.sizeof(idle)
    ok = ctypes.windll.user32.GetLastInputInfo(ctypes.byref(idle))
    sample['person_idle_s'] = ((ctypes.windll.kernel32.GetTickCount() - idle.dwTime) & 0xFFFFFFFF) / 1000 if ok else None
    sample['disk_free_bytes'] = {drive: shutil.disk_usage(drive + ':\\').free for drive in ('D', 'E')}
    sample['gpu'] = []
    for index in range(3):
        try:
            gpu = subprocess.run(['nvidia-smi', '--query-gpu=utilization.gpu,memory.used,memory.free', '--format=csv,noheader,nounits'], capture_output=True, text=True, timeout=15)
        except (subprocess.TimeoutExpired, OSError) as error:
            return dict(sample, clear=False, gpu_error=str(error))
        if gpu.returncode:
            sample.update(clear=False, gpu_error=gpu.stderr)
            return sample
        try:
            rows = [list(map(float, row.split(','))) for row in gpu.stdout.strip().splitlines()]
            sample['gpu'].extend(dict(percent=row[0], used_mib=row[1], free_mib=row[2], sample=index) for row in rows)
        except ValueError:
            sample.update(clear=False, gpu_error=gpu.stdout)
            return sample
        if index < 2:
            time.sleep(1)
    sample['gpu_instrument'] = 'NVIDIA total GPU utilization, three samples; no process exemption'
    sample['gpu_blocking'] = [v for v in sample['gpu'] if v['percent'] >= 10]
    sample['person_idle_policy'] = 'logged note; no foreign editor is permitted regardless of keyboard idleness'
    process_query = "$samples=@(Get-CimInstance -ClassName Win32_PerfFormattedData_GPUPerformanceCounters_GPUEngine -ErrorAction Stop); @{gpu_samples=$samples.Count; gpu=@($samples | Where-Object { $_.UtilizationPercentage -ge 10 } | ForEach-Object { $match=[regex]::Match($_.Name,'pid_(\\d+)'); $app=Get-Process -Id ([int]$match.Groups[1].Value) -ErrorAction SilentlyContinue; @{pid=[int]$match.Groups[1].Value; name=$app.ProcessName; percent=$_.UtilizationPercentage} })} | ConvertTo-Json -Depth 4 -Compress"
    try:
        process_result = subprocess.run([ps, '-NoProfile', '-Command', process_query], capture_output=True, text=True, timeout=40)
        if process_result.returncode or process_result.stderr:
            return dict(sample, clear=False, error=process_result.stderr, observation='per-process GPU query')
        process_gpu = json.loads(process_result.stdout)
        sample['process_gpu_samples'] = process_gpu.get('gpu_samples', 0)
        sample['process_gpu_blocking'] = [row for row in process_gpu.get('gpu', []) if str(row.get('name', '')).lower() not in ('dwm', 'csrss', 'system', 'idle', 'explorer', 'searchhost', 'textinputhost', 'startmenuexperiencehost', 'shellexperiencehost')]
    except (subprocess.TimeoutExpired, OSError, ValueError) as error:
        return dict(sample, clear=False, error=str(error), observation='per-process GPU query')
    sample['clear'] = not sample['processes'] and not sample['background_gpu'] and not sample['gpu_blocking'] and not sample['process_gpu_blocking'] and sample['process_gpu_samples'] > 0 and sample['memory_gib'] >= 6 and min(sample['disk_free_bytes'].values()) >= 50*1024**3
    return sample


def main():
    watch = '--watch' in sys.argv
    start = time.time()
    deadline = start + 4*60*60
    while True:
        sample = read()
        sample.update(start=start, deadline=deadline, waited_s=time.time()-start)
        with (work / 'idle-history.jsonl').open('a', encoding='utf-8') as stream:
            stream.write(json.dumps(sample) + '\n')
        (work / 'idle-latest.json').write_text(json.dumps(sample, indent=2), encoding='utf-8')
        print(json.dumps(sample), flush=True)
        if sample['clear'] or not watch or time.time() >= deadline:
            return 0 if sample['clear'] else 1
        remaining = min(300, deadline-time.time())
        while remaining > 0:
            interval = min(30, remaining)
            time.sleep(interval)
            remaining -= interval


if __name__ == '__main__':
    raise SystemExit(main())
