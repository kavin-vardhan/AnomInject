import hashlib
import importlib
import json
import os
from pathlib import Path
import sys

ROOT=Path('D:/IA_Europa/Receipts/09447t')
RUN=Path('D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan')
CACHE={}

def floors(scope,start=False):
    if Path(scope['job_root'])!=ROOT or scope.get('resource_policy')!='09447t-OD16-r2':
        raise RuntimeError('Wrong OD16 job/resource binding')
    module_name=scope.get('resource_guard_module','stage_budget09447t_r2')
    if module_name not in ('stage_budget09447t_r2','stage_budget_fast09447t_r2'):
        raise RuntimeError('Unapproved current-job resource helper')
    expected={'resource09447t_r2.py','allocation09447t_r2.py',module_name+'.py'}
    if set(scope.get('resource_helpers',{}))!=expected:
        raise RuntimeError('Missing pinned OD16 helpers')
    for name,pin in scope['resource_helpers'].items():
        path=RUN/name
        if path.resolve(strict=True)!=path or any(p.lstat().st_file_attributes&1024 for p in (path,*path.parents)) or hashlib.sha256(path.read_bytes()).hexdigest()!=pin:
            raise RuntimeError('Changed/nonphysical OD16 helper')
    path=Path(os.environ.get('EUROPA_BUDGET_PATH',''))
    pin=os.environ.get('EUROPA_BUDGET_SHA256')
    if path.parent!=ROOT or not pin or hashlib.sha256(path.read_bytes()).hexdigest()!=pin:
        raise RuntimeError('Missing/unpinned operation budget')
    budget=json.loads(path.read_text(encoding='utf-8'))
    if budget['operation']!=scope.get('resource_operation') or budget['kind']!=scope.get('resource_kind'):
        raise RuntimeError('Budget is for a different operation or write class')
    key=(pin,module_name)
    if key not in CACHE:
        sys.path.insert(0,str(RUN))
        module=importlib.import_module(module_name)
        CACHE[key]=module.Guard(path,pin,budget['operation'],budget['kind'])
    return CACHE[key].sample(start)
