import hashlib
import json
import shutil
import subprocess
import time
import zipfile
from pathlib import Path

w = Path('E:/IA_BuildCache/_r921/09205')
r = w.parent / 'AnomalyInjector'
h = Path('E:/IA_BuildCache/_r53_host/StackOBot')
before = json.loads((w / 'preflight.json').read_text())
restored = json.loads((w / 'restoration.json').read_text())
gates = json.loads((w / 'part-b-gates.json').read_text())
assert gates['status'] == 'PASS'


def sha(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def git(*args, repo=r):
    return subprocess.check_output(['git', '-C', str(repo), *args], text=True).strip()


assert git('branch', '--show-current') == 'feat/vt-blur-stacks'
runtime = gates['source_commit']
trees = {}
for name in ('Source', 'Shaders', 'AnomalyInjector.uplugin'):
    assert not git('diff', runtime, '--', name)
    trees[name] = git('rev-parse', runtime + ':' + name)
protected = []
for line in before['protected_refs']:
    ref, expected = line.split()
    actual = git('rev-parse', ref)
    assert actual == expected, ref
    protected.append(line)
assert git('rev-parse', 'HEAD', repo=h / 'Plugins/AnomalyInjector') == before['host_head']
assert git('rev-parse', '--abbrev-ref', 'HEAD', repo=h / 'Plugins/AnomalyInjector') == 'HEAD'
assert not git('status', '--porcelain', repo=h / 'Plugins/AnomalyInjector')
for row in before['binaries']:
    assert sha(row['live']) == sha(row['archive']) == row['sha256']
for row in before['settings']:
    assert sha(row['path']) == row['sha256']
for row in restored['created_settings']:
    assert row['removed'] and not Path(row['path']).exists()
engine = json.loads((w / 'engine-source-recheck.json').read_text())
for row in engine:
    assert sha(row['path']) == row['prior'] == row['current']
extra = json.loads((w / 'stack-source-evidence.json').read_text())['records']
for row in extra:
    assert sha(row['path']) == row['sha256']
single = json.loads((w / 'single-fixture-unchanged.json').read_text())
for row in single:
    assert sha(row['path']) == row['prior'] == row['current']
fixtures = []
for name in ('fixture-part-a', 'fixture-part-b-calibrated'):
    manifest = json.loads((w / 'archives' / (name + '.json')).read_text())
    assert sha(manifest['archive']) == manifest['sha256']
    with zipfile.ZipFile(manifest['archive']) as archive:
        for path, expected in manifest['files'].items():
            assert hashlib.sha256(archive.read(path)).hexdigest() == expected
            if name == 'fixture-part-b-calibrated':
                assert sha(Path(manifest['source']) / path) == expected
    fixtures.append(dict(archive=manifest['archive'], sha256=manifest['sha256'], files=len(manifest['files'])))
cooks = []
for part in ('part-a-final', 'part-b-final', 'part-b-final-v2'):
    result = json.loads((w / 'cooks' / part / 'result.json').read_text())
    for path, expected in result['hashes'].items():
        assert sha(path) == expected
    cooks.append(result)
accepted_trees = {}
for name in ('Source', 'Shaders', 'AnomalyInjector.uplugin'):
    accepted_trees[name] = git('rev-parse', 'af79520:' + name)
    assert accepted_trees[name] == git('rev-parse', 'c80ef9b:' + name)
disk = {letter: shutil.disk_usage(letter + ':\\').free for letter in ('D', 'E')}
assert min(disk.values()) > 50 * 1024 ** 3
record = dict(status='PASS', time=time.time(), branch='feat/vt-blur-stacks', runtime=runtime, trees=trees,
              protected_refs=protected, host_head=before['host_head'], restored_binaries=len(before['binaries']),
              restored_settings=len(before['settings']), removed_created_settings=len(restored['created_settings']),
              engine_entries=len(engine), extra_engine_entries=len(extra), single_fixture_files=len(single),
              fixtures=fixtures, cooks=cooks, accepted_head_runtime_trees=accepted_trees, disk_free_bytes=disk)
(w / 'final-verification.json').write_text(json.dumps(record, indent=2))
print(json.dumps({key: record[key] for key in ('status','runtime','restored_binaries','restored_settings','removed_created_settings','engine_entries','extra_engine_entries','single_fixture_files','disk_free_bytes')}))
