import copy
import json
from pathlib import Path
import sys

import fixture_policy as policy

ACTIVE=None
MODES={'pass','partial','extra','missing_probe_stop','deadline','crash_name','ubt_build','smoke_fail'}


def activate(config):
    global ACTIVE
    data=config.get('rehearsal')
    if not isinstance(data,dict) or data.get('authority')!='094-23-ruling EY1':
        raise RuntimeError('Rehearsal requires the explicit EY1 scratch contract.')
    receipts=policy.canonical(data['receipts_root'],policy.ROOT/'Receipts')
    temp=policy.canonical(data['temp_root'],policy.ROOT/'Temp')
    relative=receipts.relative_to(policy.ROOT/'Receipts')
    if len(relative.parts)!=3 or relative.parts[0] not in ('09428','CodeReview09428') or relative.parts[1]!='rehearsal':
        raise RuntimeError('Rehearsal receipts must be one owned 09428 rehearsal case.')
    if temp != policy.ROOT/'Temp'/relative or policy.canonical(data['host'],temp)!=temp/'host':
        raise RuntimeError('Rehearsal host and TEMP must be inside the matching scratch case.')
    if set(data.get('modes',{}))!=set(policy.SESSIONS) or any(v not in MODES for v in data['modes'].values()):
        raise RuntimeError('Unknown harmless stand-in variant.')
    if type(data.get('deadline_seconds')) is not int or not 2<=data['deadline_seconds']<=30:
        raise RuntimeError('Rehearsal deadline must be 2 to 30 seconds.')
    for row in data.get('manifests',[]):policy.canonical(row['path'],temp)
    if len(data.get('manifests',[]))!=3:
        raise RuntimeError('Rehearsal needs three explicitly synthetic manifests.')
    for group in ('host_files','engine_files','existing_content','existing_config'):
        for row in config.get(group,[]):policy.canonical(row['path'],temp)
    if ACTIVE is not None and ACTIVE!=data:
        raise RuntimeError('A process cannot change its rehearsal contract.')
    ACTIVE=copy.deepcopy(data)
    policy.REHEARSAL=ACTIVE
    policy.HOST=temp/'host'
    policy.OUTPUTS={'Smoke':[],
        'EA_Core':[policy.HOST/'Content/EuropaPhaseA/SM_EA_TwoLod.uasset',policy.HOST/'Content/EuropaPhaseA/EA_Core.umap'],
        'EA_Travel':[policy.HOST/'Content/EuropaPhaseA/EA_Travel.umap']}
    return ACTIVE


def validate(config):
    if ACTIVE is None or config.get('rehearsal')!=ACTIVE:
        raise RuntimeError('Rehearsal roots are refused outside explicit test mode.')
    receipts=Path(ACTIVE['receipts_root']);temp=Path(ACTIVE['temp_root'])
    expected={r:policy.make_recipe(r,receipts,temp) for r in policy.SESSIONS}
    if config.get('recipes')!=expected or Path(config['global_state'])!=receipts/'fixture-state/global':
        raise RuntimeError('Rehearsal state or attempt path escaped its owned scratch case.')
    if config.get('dry_run_only') is not False:
        raise RuntimeError('The rehearsal must exercise the non-dry branch.')
    return receipts,temp


def command(recipe,root):
    if ACTIVE is None:raise RuntimeError('No active rehearsal.')
    root=policy.canonical(root,Path(ACTIVE['receipts_root']))
    return [sys.executable,'-B',str(policy.TOOLS/'fixture_standin.py'),'--attempt',str(root),
        '--host',str(policy.HOST),'--recipe',recipe,'--mode',ACTIVE['modes'][recipe],policy.UPLOAD,policy.ANALYTICS]


def samples():
    from idle_gate import FLOORS,GIB
    sample=dict(idle_seconds=300,free_memory_bytes=6*GIB,free_commit_bytes=8*GIB,gpu_percent=0,
        processes=[dict(pid=1,parent_pid=0,image='EpicGamesLauncher.exe',
            image_path='C:/Program Files (x86)/Epic Games/Launcher/Portal/Binaries/Win64/EpicGamesLauncher.exe')],
        volumes={d:dict(resolved_drive=d,free_bytes=max(n,50*GIB)) for d,n in FLOORS.items()})
    return [copy.deepcopy(sample) for _ in range(3)]


def monitor_arguments():
    sample=samples()[0]
    return dict(collector=lambda:copy.deepcopy(sample),limits_reader=lambda:dict(
        free_commit_bytes=8*1024**3,volumes=copy.deepcopy(sample['volumes'])))


def preflight(root):
    from owner_attempts import create_json
    before=policy.content_snapshot();configs=policy.config_snapshot()
    policy.verify_files(before+configs)
    path=policy.canonical(root,Path(ACTIVE['receipts_root']))/'P9/entry-fixture/summary.json'
    path.parent.mkdir(parents=True,exist_ok=False)
    create_json(path,dict(result='PASS',rehearsal=True,scope='Synthetic scratch host only; not an EA8 proof.',
        host=str(policy.HOST),files=before+configs))


def check_ddc():
    return dict(result='REHEARSAL',routing='Synthetic D scratch; no Unreal DDC or shader writer exists.',host=str(policy.HOST))
