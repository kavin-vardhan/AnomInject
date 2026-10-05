import json
from pathlib import Path
import subprocess
from types import SimpleNamespace
from unittest import mock
import idle_gate

w = Path('E:/IA_BuildCache/_r921/09203c')
results = []
for mode in ('clear', 'foreign_editor', 'gpu_busy', 'process_gpu_busy', 'gpu_no_observations', 'memory_low', 'disk_low', 'query_timeout'):
    def run(args, **kwargs):
        if mode == 'query_timeout':
            raise subprocess.TimeoutExpired(args, 15)
        command = args[-1]
        if args[0] == 'nvidia-smi':
            value = '11, 1000, 7000' if mode == 'gpu_busy' else '0, 1000, 7000'
        elif 'Win32_PerfFormattedData_GPUPerformanceCounters_GPUEngine' in command:
            value = json.dumps(dict(gpu_samples=0 if mode == 'gpu_no_observations' else 100, gpu=[dict(pid=999999, name='chrome', percent=11)] if mode == 'process_gpu_busy' else []))
        elif 'Get-CimInstance Win32_Process' in command:
            value = json.dumps(dict(processes=[], background_gpu=[], memory_gib=5 if mode == 'memory_low' else 10))
        else:
            value = json.dumps([dict(ProcessId=999999, Name='UnrealEditor.exe', ExecutablePath='foreign')] if mode == 'foreign_editor' else [])
        return SimpleNamespace(returncode=0, stderr='', stdout=value)
    with mock.patch.object(idle_gate.subprocess, 'run', run), mock.patch.object(idle_gate.time, 'sleep', lambda _:None), mock.patch.object(idle_gate.shutil, 'disk_usage', lambda _:SimpleNamespace(free=(49 if mode == 'disk_low' else 100)*1024**3)):
        result = idle_gate.read()
    assert result['clear'] == (mode == 'clear')
    results.append(dict(mode=mode, expected_clear=mode == 'clear', observed=result, pass_=True))
assert not (w / 'idle-monitor-can-fails-v2.json').exists()
(w / 'idle-monitor-can-fails-v2.json').write_text(json.dumps(results, indent=2))
print(json.dumps([dict(mode=r['mode'], passed=r['pass_']) for r in results]))
