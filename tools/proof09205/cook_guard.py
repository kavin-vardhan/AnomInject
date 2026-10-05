import json
import subprocess
import time
import process_snapshot

ps = r'C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe'
query = "$samples=@(Get-CimInstance Win32_PerfFormattedData_GPUPerformanceCounters_GPUEngine -ErrorAction Stop); @{gpu_samples=$samples.Count; gpu=@($samples | Where-Object { $_.UtilizationPercentage -ge 10 } | ForEach-Object { $match=[regex]::Match($_.Name,'pid_(\\d+)'); $app=Get-Process -Id ([int]$match.Groups[1].Value) -ErrorAction SilentlyContinue; @{pid=[int]$match.Groups[1].Value; name=$app.ProcessName; percent=$_.UtilizationPercentage} })} | ConvertTo-Json -Depth 4 -Compress"


def wait_owned_tree(process, seconds, receipt_path):
    started = time.time()
    owned = {}
    checks = []
    reason = None
    while process.poll() is None:
        try:
            process_rows = process_snapshot.snapshot()
            live = {p['ProcessId']: p for p in process_rows}
            if process.pid in live and process.pid not in owned:
                owned[process.pid] = live[process.pid]
            valid = {pid for pid, p in owned.items() if p.get('CreationStamp') and pid in live and p['CreationStamp'] == live[pid]['CreationStamp']}
            changed = True
            while changed:
                changed = False
                for pid, p in live.items():
                    if pid not in valid and p.get('CreationStamp') and p['ParentProcessId'] in valid:
                        owned[pid] = p
                        valid.add(pid)
                        changed = True
            read = subprocess.run([ps, '-NoProfile', '-Command', query], capture_output=True, text=True, timeout=45)
        except (subprocess.TimeoutExpired, OSError) as error:
            reason = 'process/GPU observation failed'
            checks.append(dict(time=time.time(), error=str(error)))
            break
        if read.returncode or read.stderr:
            reason = 'process/GPU observation failed'
            checks.append(dict(time=time.time(), error=read.stderr, output=read.stdout))
            break
        sample = json.loads(read.stdout)
        foreign = [p for pid, p in live.items() if p['Name'].lower().startswith(('unrealeditor', 'ue4editor')) and pid not in valid]
        gpu = [p for p in sample['gpu'] if p['pid'] not in valid and str(p.get('name', '')).lower() not in ('dwm', 'csrss', 'system', 'idle', 'explorer', 'searchhost', 'textinputhost', 'startmenuexperiencehost', 'shellexperiencehost')]
        checks.append(dict(time=time.time(), owned=sorted(valid), foreign_editors=foreign, foreign_gpu=gpu, gpu_samples=sample['gpu_samples']))
        receipt_path.write_text(json.dumps(dict(owned=list(owned.values()), checks=checks), indent=2))
        if foreign or gpu or not sample['gpu_samples']:
            reason = 'foreign editor/GPU work appeared or GPU observation unavailable'
            break
        if time.time() - started > seconds:
            reason = 'cook timeout'
            break
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            pass
    stopped = []
    if reason:
        for pid, record in reversed(list(owned.items())):
            try:
                stopped.append(process_snapshot.stop_owned(record))
            except OSError as error:
                stopped.append(dict(pid=pid, error=str(error)))
        if process.poll() is None:
            process.terminate()
        process.wait()
    result = dict(exit=process.returncode, reason=reason, owned=list(owned.values()), stopped=stopped, checks=checks)
    receipt_path.write_text(json.dumps(result, indent=2))
    return result
