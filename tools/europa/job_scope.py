import hashlib
import json
import os
from pathlib import Path

ROOT = Path('D:/IA_Europa')


def load(path=None):
    raw = path or os.environ.get('EUROPA_JOB_CONFIG')
    if not raw:
        raise RuntimeError('Explicit EUROPA_JOB_CONFIG is required; use run_job.py --job-config')
    path = Path(raw)
    if not path.is_absolute() or path.resolve(strict=True) != path or not path.is_relative_to(ROOT / 'Receipts'):
        raise RuntimeError('Job configuration must be in physical-D port receipts')
    data = json.loads(path.read_text(encoding='utf-8'))
    job = Path(data['job_root'])
    temp = Path(data['temp_root'])
    if job.resolve(strict=True) != job or path.parent != job or not job.is_relative_to(ROOT / 'Receipts'):
        raise RuntimeError('Job receipt root/configuration mismatch')
    if temp.resolve() != temp or not temp.is_relative_to(ROOT / 'Temp'):
        raise RuntimeError('Job TEMP root is not physical-D owned scratch')
    known = {'425': ('StylizedParisStreet', 'StylizedParisStreetEditor'),
             '51base': ('StackOBot', 'StackOBotEditor'), '51port': ('StackOBot', 'StackOBotEditor')}
    for key in ('build_pairs', 'query_pairs'):
        if len(data[key]) != len({tuple(p) for p in data[key]}):
            raise RuntimeError('Duplicate permitted target')
        for host, target, configuration in data[key]:
            if host not in known or target not in known[host] or configuration not in ('Development', 'Shipping'):
                raise RuntimeError('Unknown target in job allow-list')
            if configuration == 'Shipping' and (target.endswith('Editor') or host == '51base'):
                raise RuntimeError('Unapproved Shipping pair')
    if data['max_parallel_actions'] not in (1, 4):
        raise RuntimeError('Unapproved action parallelism')
    if data['memory'] not in (
        {'start_physical_GiB': 6, 'start_commit_GiB': 12, 'stop_physical_GiB': 1, 'stop_commit_GiB': 4},
        {'start_physical_GiB': 4, 'start_commit_GiB': 16, 'stop_physical_GiB': None, 'stop_commit_GiB': 6}):
        raise RuntimeError('Memory policy is not a ruled EF1/EI2 policy')
    return dict(data, config_path=str(path), config_sha256=hashlib.sha256(path.read_bytes()).hexdigest())


def require_pair(host, target, configuration, query=False):
    data = load()
    if [host, target, configuration] not in data['query_pairs' if query else 'build_pairs']:
        raise RuntimeError('Target is outside this job allow-list')
    return data
