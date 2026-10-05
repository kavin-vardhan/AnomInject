import hashlib
import inspect
import json
from pathlib import Path
import sys

work = Path('E:/IA_BuildCache/_r921/09203c')
tool = Path('E:/IA_BuildCache/_r53_host/StackOBot/Plugins/AnomalyInjector/tools/strict_include_build.py')
sys.dont_write_bytecode = True
sys.path.insert(0, str(tool.parent))
import strict_include_build as original

body = inspect.getsource(original.run_build)
old = '"-waitmutex"]'
assert body.count(old) == 1
modified = body.replace(old, '"-waitmutex", "-MaxParallelActions=2"]')
scope = dict(original.__dict__)
exec(compile(modified, str(work / 'strict_driver.py'), 'exec'), scope)
original.run_build = scope['run_build']
(work / 'strict-driver-receipt.json').write_text(json.dumps(dict(original_tool=str(tool), original_tool_sha256=hashlib.sha256(tool.read_bytes()).hexdigest(), change='Append -MaxParallelActions=2 to the UBT Build.bat command only; unchanged strict flags, header TU generation, diagnostics, restoration and normal rebuild', original_function=body, modified_function=modified), indent=2), encoding='utf-8')
raise SystemExit(original.main())
