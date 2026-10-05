import ctypes
import ctypes.wintypes
import json
import shutil
import subprocess
import sys
import time
from pathlib import Path

work = Path('E:/IA_BuildCache/_r921')
ps = r'C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe'


class LastInput(ctypes.Structure):
    _fields_ = [('cbSize', ctypes.wintypes.UINT), ('dwTime', ctypes.wintypes.DWORD)]


def read():
    command = "@{processes=@(Get-CimInstance Win32_Process | Where-Object { $_.Name -match '^UnrealEditor|^UE4Editor|^StackOBot.exe$|^LyraGame.exe$|^cl.exe$|^link.exe$' } | Select-Object ProcessId,Name,ExecutablePath); memory_gib=((Get-CimInstance Win32_OperatingSystem).FreeVirtualMemory/1048576); gpu=@((Get-Counter '\\GPU Engine(*)\\Utilization Percentage' -SampleInterval 1 -MaxSamples 2 -ErrorAction Stop).CounterSamples | Where-Object { $_.CookedValue -ge 10 } | ForEach-Object { $m=[regex]::Match($_.InstanceName,'pid_(\\d+)'); $gp=Get-Process -Id ([int]$m.Groups[1].Value) -ErrorAction SilentlyContinue; @{pid=[int]$m.Groups[1].Value; name=$gp.ProcessName; percent=$_.CookedValue} })} | ConvertTo-Json -Depth 4 -Compress"
    result = subprocess.run([ps, '-NoProfile', '-Command', command], capture_output=True, text=True, timeout=55)
    sample = dict(time=time.time(), command_exit=result.returncode)
    if result.returncode or result.stderr:
        sample.update(clear=False, error=result.stderr, output=result.stdout)
        return sample
    sample.update(json.loads(result.stdout))
    idle = LastInput()
    idle.cbSize = ctypes.sizeof(idle)
    ok = ctypes.windll.user32.GetLastInputInfo(ctypes.byref(idle))
    sample['person_idle_s'] = ((ctypes.windll.kernel32.GetTickCount() - idle.dwTime) & 0xFFFFFFFF) / 1000 if ok else None
    sample['disk_free_bytes'] = {drive: shutil.disk_usage(drive + ':\\').free for drive in ('D', 'E')}
    sample['gpu_blocking'] = [v for v in sample['gpu'] if str(v.get('name', '')).lower() not in ('dwm', 'csrss', 'system', 'idle', 'explorer', 'searchhost', 'textinputhost', 'startmenuexperiencehost', 'shellexperiencehost')]
    sample['clear'] = not sample['processes'] and not sample['gpu_blocking'] and sample['memory_gib'] >= 6 and sample['person_idle_s'] is not None and sample['person_idle_s'] >= 60 and min(sample['disk_free_bytes'].values()) >= 50*1024**3
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
