import copy
import hashlib
import json
import os
from pathlib import Path
import re

ROOT = Path('D:/IA_Europa')
HOST = ROOT/'StylizedParisStreet'
TOOLS = HOST/'Plugins/AnomalyInjector/tools/europa'
CITATION = 'D:/IntrusiveAnomalies/_reviews/094-21-ruling.md#EW2'
RULE = 'D:/IntrusiveAnomalies/_reviews/094-21-ruling.md#EW3'
ARMING_JOB = '09423'
AMENDMENTS = ['D:/IntrusiveAnomalies/_reviews/094-22-ruling.md#EX'+str(i) for i in range(1,6)]
SESSION_POLICY = dict(start='full_measured',during=['disk_floor','free_commit_below_6_GiB','1800_second_limit'],
    observe_only=['physical_memory','GPU','human_input','foreign_processes'],automatic_retry=False,
    failed_declared_outputs='receipt_and_quarantine_before_chat_rearm')
UPLOAD = '-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False'
RECIPES = ('EA_Core', 'EA_Travel')
OUTPUTS = {
    'EA_Core': [HOST/'Content/EuropaPhaseA/SM_EA_TwoLod.uasset', HOST/'Content/EuropaPhaseA/EA_Core.umap'],
    'EA_Travel': [HOST/'Content/EuropaPhaseA/EA_Travel.umap']}
MANIFESTS = {
    '425': 'c501895c9856638dd76e1c2091be8726f6797014bcb0714e306fd9d6ebd98df6',
    'baseline51': 'b05e6f8c9ce12fa1c5b4dbd142a59383f737bb8a3b7885b7219d1bcdb6c1fc37',
    'regate51': '3865fe0c6ed039d5bad2bc54d1a6a89f11c403e98237b4c6caf74ca661ea12e2'}
SCOPE = dict(host='425', kind='fixture-authoring', recipes=list(RECIPES),
    maximum_editor_sessions=2, maximum_sessions_per_recipe=1, session_seconds=1800,
    new_fixture_packages_only=True, overwrite=False, pie=False, cook=False,
    commandlet=False, packaging=False, production_edit=False, config_edit=False)


def sha(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def canonical(path, root=ROOT):
    path = Path(path)
    if not path.is_absolute() or '..' in path.parts or re.search(r'~[0-9]', str(path)):
        raise RuntimeError('Path is not an absolute canonical long path: '+str(path))
    resolved = path.resolve()
    if resolved != path or not resolved.is_relative_to(root.resolve()):
        raise RuntimeError('Path alias or escape is refused: '+str(path))
    for parent in (path, *path.parents):
        if parent.exists() and parent.stat(follow_symlinks=False).st_file_attributes & 1024:
            raise RuntimeError('Junction or reparse alias is refused: '+str(path))
        if parent == root:
            break
    return path


def manifest_rows():
    return [dict(host=h, path=str(ROOT/'Receipts/09421/qualification'/('final-'+h+'-binaries.json')), sha256=s)
            for h,s in MANIFESTS.items()]


def validate(config, dry_run):
    if config.get('authorized_by_chat') is not True or config.get('authorization_citation') != CITATION:
        raise RuntimeError('Fixture authoring is not armed by chat EW2.')
    if config.get('arming_job') != ARMING_JOB or config.get('policy_citation') != RULE:
        raise RuntimeError('Fixture arming is not the 09423 EW2/EW3 arming amended by EX1-EX5.')
    if config.get('amendment_citations') != AMENDMENTS or config.get('session_policy') != SESSION_POLICY:
        raise RuntimeError('The EX1-EX5 amendments and EX2 session policy are required.')
    template = config.get('job_template', {})
    if 'armed for EW2' not in template.get('authority','') or 'preparation and dry-run only' in template.get('authority',''):
        raise RuntimeError('Armed slot authority must say armed for EW2, as amended by EX1-EX5.')
    if template.get('runtime_authorized') is not True or template.get('runtime_scope') != SCOPE:
        raise RuntimeError('Runtime scope must be exactly the two EW2 authoring recipes.')
    if template.get('build_pairs') != [] or template.get('query_pairs') != []:
        raise RuntimeError('This authoring kit cannot build or query targets.')
    if config.get('gate_profile') != 'measured' or config.get('human_idle') is not True:
        raise RuntimeError('Fixture authoring requires the full measured gate, including human idle.')
    if config.get('session_seconds') != 1800:
        raise RuntimeError('The bound session limit is 1800 seconds.')
    if config.get('manifests') != manifest_rows():
        raise RuntimeError('The kit must bind exactly EW1\'s three 09421 manifests.')
    if config.get('declared_outputs') != {k:list(map(str,v)) for k,v in OUTPUTS.items()}:
        raise RuntimeError('Declared outputs differ from the three new fixture packages.')
    if dry_run != (config.get('dry_run_only') is True):
        raise RuntimeError('Dry runs require throwaway slots; live runs require real slots.')
    if set(config.get('recipes', {})) != set(RECIPES):
        raise RuntimeError('Exactly EA_Core and EA_Travel recipes are required.')
    all_paths = []
    for recipe in RECIPES:
        row = config['recipes'][recipe]
        slots = row.get('attempt_slots', [])
        if len(slots) != 3 or [s.get('number') for s in slots] != [1,2,3]:
            raise RuntimeError('Each recipe needs three ordered prelaunch attempt slots.')
        expected = make_recipe(recipe, ROOT/'Receipts'/ARMING_JOB, ROOT/'Temp'/ARMING_JOB)
        if not dry_run and row != expected:
            raise RuntimeError('Real slots must use the bound 09423 recipe paths.')
        paths = [canonical(row['state_root'], ROOT/'Receipts')]
        for slot in slots:
            paths += [canonical(slot['job_root'], ROOT/'Receipts'),canonical(slot['temp_root'], ROOT/'Temp')]
        for p in paths:
            if dry_run and any(p.is_relative_to(ROOT/base/job) for base in ('Receipts','Temp') for job in ('09416','09417','09422')):
                raise RuntimeError('Dry run cannot consume old owner arming paths.')
            if dry_run and any(p.is_relative_to(ROOT/base/ARMING_JOB/part) for base in ('Receipts','Temp') for part in ('fixture-attempts','fixture-state','fixture-runs')):
                raise RuntimeError('Dry run cannot consume real fixture slots.')
        all_paths.extend(paths)
    for index,p in enumerate(all_paths):
        if any(p == q or p.is_relative_to(q) or q.is_relative_to(p) for q in all_paths[index+1:]):
            raise RuntimeError('Fixture state and attempt paths overlap.')
    global_state = canonical(config['global_state'], ROOT/'Receipts')
    if any(global_state == p or global_state.is_relative_to(p) or p.is_relative_to(global_state) for p in all_paths):
        raise RuntimeError('Global lock overlaps a recipe state or attempt path.')
    if not dry_run and global_state != ROOT/'Receipts'/ARMING_JOB/'fixture-state/global':
        raise RuntimeError('Real global lock path changed.')
    if dry_run and any(global_state.is_relative_to(ROOT/'Receipts'/job/part) for job,part in
                      [('09416','owner-state'),('09417','owner-state'),('09422','fixture-state'),(ARMING_JOB,'fixture-state')]):
        raise RuntimeError('Dry run cannot consume a real global lock.')
    return template


def make_recipe(recipe, receipts, temp):
    return dict(state_root=str(receipts/'fixture-state'/recipe), attempt_slots=[
        dict(number=i,job_root=str(receipts/'fixture-attempts'/recipe/('%02d'%i)),
             temp_root=str(temp/'fixture-attempts'/recipe/('%02d'%i))) for i in (1,2,3)])


def throwaway(config, receipts, temp):
    data = copy.deepcopy(config)
    data['dry_run_only'] = True
    data['recipes'] = {r:make_recipe(r,canonical(receipts),canonical(temp)) for r in RECIPES}
    data['global_state'] = str(Path(receipts)/'fixture-state/global')
    return data


def verify_files(rows):
    for row in rows:
        p = Path(row['path'])
        if not p.is_file() or p.stat().st_size != row['size'] or sha(p) != row['sha256']:
            raise RuntimeError('Bound file changed: '+str(p))
    return len(rows)


def verify_manifests(config):
    count = 0
    for row in config['manifests']:
        p = canonical(row['path'])
        if sha(p) != row['sha256']:
            raise RuntimeError('EW1 manifest changed: '+str(p))
        data = json.loads(p.read_text())
        count += verify_files(data['files'])
        verify_files(data['probe_source'])
    if count != 110:
        raise RuntimeError('EW1 must bind 110 files.')
    return count


def command(recipe, root):
    from prepare_runtime import prepare
    root = canonical(root)
    argv = prepare('425',root/'session',probe=True,audit=False,authoring=True)
    argv.insert(2,'/Engine/Maps/Entry')
    argv += ['-ExecutePythonScript='+(root/'script/author.py').as_posix(),UPLOAD,
             '-nowrite','-SkipCompile','-stdout']
    return argv


def validate_argv(argv, recipe, root):
    if recipe not in RECIPES:
        raise RuntimeError('Unknown authoring recipe.')
    if argv.count(UPLOAD) != 1 or any('bsendunattendedbugreports' in a.lower() and a != UPLOAD for a in argv):
        raise RuntimeError('The exact EU4 no-upload override is required once, without contradiction.')
    if argv != command(recipe,root):
        raise RuntimeError('Authoring argv differs from the bound executable, recipe script or routing.')
    return True


def content_snapshot():
    rows=[]
    for root in (HOST/'Content', HOST/'Plugins/AnomalyInjector/Content'):
        for p in sorted(root.rglob('*')):
            if p.is_file():
                canonical(p)
                rows.append(dict(path=str(p),size=p.stat().st_size,sha256=sha(p)))
    return rows


def config_snapshot():
    paths=[HOST/'StylizedParisStreet.uproject']
    for root in (HOST/'Config',HOST/'Saved/Config'):
        if root.exists():
            paths += [p for p in root.rglob('*') if p.is_file()]
    return [dict(path=str(p),size=p.stat().st_size,sha256=sha(p)) for p in sorted(paths)]


def output_preflight(recipe, core_success=None, dry_run=False):
    for p in OUTPUTS[recipe]:
        canonical(p)
        if p.exists() or any(p.with_suffix(s).exists() for s in ('.uexp','.ubulk','.uptnl')):
            raise RuntimeError('No overwrite: destination already exists: '+str(p))
    if recipe == 'EA_Core':
        folder=HOST/'Content/EuropaPhaseA'
        if folder.exists() and any(folder.rglob('*')):
            raise RuntimeError('The fixture folder must be empty before EA_Core.')
    elif not dry_run:
        if not core_success or core_success.get('result') != 'AUTHORING_PASS':
            raise RuntimeError('EA_Travel requires EA_Core\'s successful postflight.')
        verify_files(core_success['outputs'])


def verify_content_after(before, recipe):
    verify_files(before)
    old={r['path'] for r in before}
    current=content_snapshot()
    added=[r for r in current if r['path'] not in old]
    if {r['path'] for r in added} != set(map(str,OUTPUTS[recipe])):
        raise RuntimeError('Authoring wrote missing or undeclared Content files.')
    if any(r['size'] <= 0 for r in added):
        raise RuntimeError('An authored package is empty.')
    return added
