import json
import shutil
import subprocess
import time
from pathlib import Path

work = Path('E:/IA_BuildCache/_r921/09203c')
start = time.time()
deadline = start + 3600
while True:
    disk = {drive: shutil.disk_usage(drive + ':\\').free for drive in ('D', 'E')}
    pagefiles = subprocess.check_output(['powershell', '-NoProfile', '-Command', '@(Get-CimInstance Win32_PageFileUsage | Select-Object Name,AllocatedBaseSize,CurrentUsage,PeakUsage) | ConvertTo-Json -Compress'], text=True, timeout=45)
    sample = dict(time=time.time(), start=start, deadline=deadline, waited_s=time.time()-start, disk_free_bytes=disk, pagefiles=json.loads(pagefiles), clear=min(disk.values()) >= 50*1024**3)
    with (work / 'disk-history.jsonl').open('a', encoding='utf-8') as stream:
        stream.write(json.dumps(sample)+'\n')
    (work / 'disk-latest.json').write_text(json.dumps(sample, indent=2), encoding='utf-8')
    print(json.dumps(sample), flush=True)
    if sample['clear'] or time.time() >= deadline:
        raise SystemExit(0 if sample['clear'] else 1)
    remaining = min(300, deadline-time.time())
    while remaining > 0:
        interval = min(30, remaining)
        time.sleep(interval)
        remaining -= interval
