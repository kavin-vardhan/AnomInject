import hashlib
import json
import shutil
import subprocess
import sys
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
repo=w.parent/'AnomalyInjector'
def read_text(path):
    raw=path.read_bytes()
    return raw.decode('utf-16' if raw.startswith((b'\xff\xfe',b'\xfe\xff')) else 'utf-8-sig')
record=json.loads((w/'probe-archive.json').read_text())
archive=Path(record['archive'])
summary=json.loads((w/'builds/part-a-strict-01/summary.json').read_text())
assert summary['restore_ok']
for row in summary['strict']+summary['normal']: assert row['exit']==row['errors']==row['warnings']==0
assert all(row['header_tus_compiled']==71 for row in summary['strict'])
audit=Path(json.loads((w/'latest-audit.json').read_text())['path'])
assert all(row['exit']==0 for row in json.loads((audit/'lever-results.json').read_text()))
strings=json.loads((audit/'strings-result.json').read_text())
assert strings['unresolved']==0 and strings['semantic_mutant_caught']
assert json.loads((w/'lever-can-fail.json').read_text())['caught']
assert json.loads((w/'archive-hash-can-fail.json').read_text())['caught']
for path,digest in record['files'].items():
    with (archive/path).open('rb') as stream: assert hashlib.file_digest(stream,'sha256').hexdigest()==digest
legs=['census-single-view-final','census-single-all-final-v3','census-stacks-view-final','census-stacks-all-final','census-staged-stacks-all-final','census-staged-single-all-final']
results=[]
for name in legs:
    root=w/'runs'/name
    launch=json.loads((root/'launch.json').read_text())
    result=json.loads((root/'census-result.json').read_text())
    assert launch['exit']==0 and result['status']=='PASS',name
    assert result['null_precedes_census'] and result['labelled_events_absent'],name
    assert launch['archive']['files']==record['files'],name
    check=subprocess.run([sys.executable,'-B',str(repo/'tools/census_readonly_check.py'),'--log',str(root/'game.log')],capture_output=True,text=True)
    (root/'numeric-log-check.txt').write_text(check.stdout+check.stderr)
    assert check.returncode==0,(name,check.stdout)
    results.append(dict(leg=name,scope=launch['scope'],fixture=launch['fixture'],staged=launch.get('staged',False),expected=result['expected_file'],printed=result['printed'],state_unchanged=result['state_unchanged'],pixels=result['pixels'],numeric_log='PASS'))
mutants=[]
for name in ('census-single-miscount','census-stacks-miscount'):
    result=json.loads((w/'runs'/name/'census-result.json').read_text())
    assert result['mutant_caught'] and result['status']=='PASS'
    mutants.append(dict(leg=name,failures=result['failures']))
for mode in ('state','pixel'):
    assert json.loads((w/'runs/census-stacks-all-02'/('reader-can-fail-'+mode+'.json')).read_text())['caught']
assert '47 case(s): OK' in read_text(w/'census-readonly-selftest-03.txt')
assert "'changed': 0" in read_text(w/'comments-part-a.txt')
assert not subprocess.check_output(['git','-C',str(repo),'diff','16b5f58','--','Source','Shaders','AnomalyInjector.uplugin'],text=True).strip()
prior=json.loads((w/'preflight.json').read_text())
refs=subprocess.check_output(['git','-C',str(repo),'show-ref'],text=True).splitlines()
for row in prior['protected_refs']:
    name,digest=row.split()
    assert digest+' '+name in refs,row
disk={d:shutil.disk_usage(d+':\\').free for d in ('D','E')}
assert min(disk.values())>=50*1024**3
out=dict(status='PASS',source_commit=record['source_commit'],archive=record,builds=summary,strings=strings['reviewed'],legs=results,miscount_can_fails=mutants,reader_can_fails=['state','pixel'],readonly_selftests=47,protected_refs_unchanged=True,disk_free_bytes=disk)
(w/'part-a-gates.json').write_text(json.dumps(out,indent=2))
print(json.dumps(dict(status='PASS',legs=len(legs),mutants=len(mutants),source_commit=record['source_commit'],archive=str(archive))))
