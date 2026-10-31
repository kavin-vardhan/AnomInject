import hashlib
import json
import os
from pathlib import Path

ROOT = Path('D:/IA_Europa')


def fm1(data, job):
    return (job in (ROOT/'Receipts/09431b',ROOT/'Receipts/09432',ROOT/'Receipts/09433',ROOT/'Receipts/09433b') and
            data.get('incremental_memory_authority') == '094-31-ruling FM1' and
            data.get('incremental_builds_only') is True and not data.get('full_rebuild', False) and
            data.get('runtime_authorized') is False and data.get('query_pairs') == [] and
            data.get('max_parallel_actions') == 2 and data.get('max_build_attempts_per_target') == 3 and
            {tuple(p) for p in data.get('build_pairs', [])} == {
                ('425','StylizedParisStreetEditor','Development'), ('425','StylizedParisStreet','Development')} and
            data.get('memory') == {'start_physical_GiB':None,'start_commit_GiB':8,
                                   'stop_physical_GiB':None,'stop_commit_GiB':3})


def memory_policy_allowed(data, job):
    if fm1(data, job):
        return True
    if ((job,data.get('authority')) in ((ROOT/'Receipts/09432','094-32 FN1-FN5 FM1 FL1-FL6 FJ1 FK1'),(ROOT/'Receipts/09433','094-33 FO1-FO5 FN2 FM1 FJ1 FK1'),(ROOT/'Receipts/09433b','094-33b FP1-FP3 FO3 FN2 FM1 FJ1 FK1')) and data.get('runtime_authorized') is True
        and data.get('runtime_scope',{}).get('kind')=='phase-a-runtime'
        and data.get('build_pairs')==[] and data.get('query_pairs')==[]
        and data['memory']=={'start_physical_GiB':4,'start_commit_GiB':8,'stop_physical_GiB':None,'stop_commit_GiB':4}):
        return True
    if (job == ROOT/'Receipts/09431b' and data.get('runtime_authorized') is True
        and data.get('runtime_scope',{}).get('kind')=='phase-a-runtime'
        and data.get('authority')=='094-31b FM1-FM2 FL1-FL6 FJ1 FK1'
        and data.get('build_pairs')==[] and data.get('query_pairs')==[]
        and data['memory']=={'start_physical_GiB':4,'start_commit_GiB':8,'stop_physical_GiB':None,'stop_commit_GiB':4}):
        return True
    if (job == ROOT/'Receipts/09431' and data.get('runtime_authorized') is True
        and data.get('runtime_scope',{}).get('kind')=='phase-a-runtime'
        and data.get('authority')=='094-31 FL1-FL6 FJ1 FK1'
        and data.get('build_pairs')==[] and data.get('query_pairs')==[]
        and data['memory']=={'start_physical_GiB':4,'start_commit_GiB':8,'stop_physical_GiB':None,'stop_commit_GiB':4}):
        return True
    if (job == ROOT/'Receipts/09427e' and data.get('runtime_authorized') is True
        and data.get('runtime_scope',{}).get('kind')=='phase-a-runtime'
        and '094-27d-ruling FK1-FK4' in data.get('authority','')
        and data.get('build_pairs')==[] and data.get('query_pairs')==[]
        and data['memory']=={'start_physical_GiB':4,'start_commit_GiB':8,'stop_physical_GiB':None,'stop_commit_GiB':4}):
        return True
    if (data.get('runtime_authorized') is True and data.get('runtime_scope',{}).get('kind')=='phase-a-runtime'
        and 'FH1-FH9' in data.get('authority','') and data.get('build_pairs')==[] and data.get('query_pairs')==[]
        and data['memory']=={'start_physical_GiB':6,'start_commit_GiB':8,'stop_physical_GiB':None,'stop_commit_GiB':4}):
        return True
    if (data.get('runtime_authorized') is True and data.get('runtime_scope',{}).get('kind')=='fixture-authoring'
        and 'FD1-FD5' in data.get('authority','') and data.get('build_pairs')==[] and data.get('query_pairs')==[]
        and data['memory']=={'start_physical_GiB':6,'start_commit_GiB':8,'stop_physical_GiB':None,'stop_commit_GiB':4}):
        return True
    if (data.get('runtime_authorized') is True and data.get('runtime_scope',{}).get('kind')=='fixture-authoring'
        and 'EY1-EY4' in data.get('authority','') and data.get('build_pairs')==[] and data.get('query_pairs')==[]
        and data['memory']=={'start_physical_GiB':6,'start_commit_GiB':12,'stop_physical_GiB':None,'stop_commit_GiB':6}):
        return True
    if data['memory'] in (
        {'start_physical_GiB': 6, 'start_commit_GiB': 12, 'stop_physical_GiB': 1, 'stop_commit_GiB': 4},
        {'start_physical_GiB': 4, 'start_commit_GiB': 16, 'stop_physical_GiB': None, 'stop_commit_GiB': 6}):
        return True
    pairs = {
        ('425','StylizedParisStreetEditor','Development'), ('425','StylizedParisStreet','Development'),
        ('51base','StackOBotEditor','Development'), ('51base','StackOBot','Development'),
        ('51port','StackOBotEditor','Development'), ('51port','StackOBot','Development')}
    return (job == ROOT/'Receipts/09421' and
            data.get('incremental_memory_authority') == '094-20-ruling EV4' and
            data.get('incremental_builds_only') is True and not data.get('full_rebuild', False) and
            not data.get('runtime_authorized', False) and
            {tuple(p) for p in data['build_pairs']} == pairs and
            data['memory'] == {'start_physical_GiB':4,'start_commit_GiB':10,
                               'stop_physical_GiB':None,'stop_commit_GiB':6})


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
    if data['max_parallel_actions'] not in (1, 4) and not fm1(data, job):
        raise RuntimeError('Unapproved action parallelism')
    if not memory_policy_allowed(data, job):
        raise RuntimeError('Memory policy is not a ruled EF1/EI2/EV4/FD3/FK1 policy')
    return dict(data, config_path=str(path), config_sha256=hashlib.sha256(path.read_bytes()).hexdigest())


def require_pair(host, target, configuration, query=False):
    data = load()
    if [host, target, configuration] not in data['query_pairs' if query else 'build_pairs']:
        raise RuntimeError('Target is outside this job allow-list')
    return data
