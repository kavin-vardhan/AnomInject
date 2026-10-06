import hashlib
import json
import re
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
archive=json.loads((w/'probe-archive.json').read_text())
def read(label,name):
    return json.loads((w/'runs'/label/name).read_text())
def final_launch(label):
    launch=read(label,'launch.json')
    assert launch['exit']==0 and launch['archive']['files']==archive['files'],label
    return launch
identity=read('stacks-identity-final-v2','native-result.json')
assert identity['status']=='PASS' and identity['frame_count']==480
final_launch('stacks-identity-final-v2')
positive=[]
for label,count in [('stacks-pie-targeted-final-v2',10),('stacks-pie-auto-final-v2',3),('stacks-staged-targeted-final-v2',5),('stacks-staged-auto-final-v2',3),('stacks-readiness-delay-control-v2',1)]:
    final_launch(label)
    p=read(label,'positive-result.json')
    layers=read(label,'layer-result.json')
    cycles=read(label,'cycles-result.json')
    assert p['status']==layers['status']==cycles['status']=='PASS'
    assert len(p['events'])>=count and p['e1_summary']['t50']['unexcused_frames']==0
    positive.append(dict(label=label,events=len(p['events']),frames=p['frame_count'],labelled=sum(len(e['frames']) for e in p['events']),e1=p['e1_summary'],layers=layers,cycles=cycles,controls=p['control_failures'],revert=p['restore_failures'],missing_masks=p['missing_masks']))
lifetimes=[]
for kind in ('destroy','displace','world_end'):
    label='stacks-'+kind.replace('_','-')+'-final-v2'
    final_launch(label)
    result=read(label,'world-end-result.json' if kind=='world_end' else 'lifetime-result.json')
    assert result['status']=='PASS'
    log=(w/'runs'/label/'game.log').read_text(errors='replace')
    refs=re.findall(r'VT_COMBINED_DRAIN refs=(\d+)',log)
    assert refs and set(refs)=={'0'}
    lifetimes.append(dict(label=label,result=result,combined_reference_drains=refs))
mutants=[]
for label in ('stacks-partial-final-v2','stacks-no-bind-final-v2','stacks-early-delay-final-v2'):
    final_launch(label)
    result=read(label,'mutant-result.json')
    assert result['status']=='PASS' and result['caught']
    mutants.append(dict(label=label,result=result))
refusals=[]
refusal_attempts=json.loads((w/'refusal-attempt-gates.json').read_text())
assert refusal_attempts['status']=='PASS' and len(refusal_attempts['cases'])==3
assert {row['label'] for row in refusal_attempts['cases']}=={'stacks-release-refusal-final-v2','stacks-accepted-refusal-final-v2','stacks-unnamed-refusal-final-v2'}
for label in ('stacks-release-refusal-final-v2','stacks-accepted-refusal-final-v2','stacks-unnamed-refusal-final-v2'):
    launch=final_launch(label)
    result=read(label,'refusal-result.json')
    assert result['status']=='PASS' and result['events']==0
    if 'release-' in label: assert launch['exe_sha256'].startswith('218ba4c7')
    if 'accepted-' in label: assert launch['exe_sha256'].startswith('1ea292e8')
    refusals.append(dict(label=label,result=result,exe_sha256=launch.get('exe_sha256')))
regressions=[]
for kind in ('hold','proxy','uv','normal','single-vt'):
    label='regression-'+kind+'-final-v2'
    final_launch(label)
    result=read(label,'positive-result.json' if kind=='single-vt' else 'regression-result.json')
    assert result['status']=='PASS'
    events=len(result['events']) if kind=='single-vt' else result['fires']
    unflagged=result['e1_summary']['t50']['unexcused_frames'] if kind=='single-vt' else result['unflagged']
    assert events>=3 and unflagged==0
    regressions.append(dict(label=label,events=events,unflagged=unflagged,result=result))
census=[]
for label in ('census-stacks-all-part-b-v2','census-single-all-part-b-v2','census-stacks-view-part-b-v2','census-single-view-part-b-v2'):
    final_launch(label)
    result=read(label,'census-result.json')
    assert result['status']=='PASS' and result['null_precedes_census'] and result['state_unchanged']
    census.append(dict(label=label,result=result))
miscounts=[]
for fixture in ('stacks','single'):
    label='census-'+fixture+'-miscount-part-b-v2'
    final_launch(label)
    result=read(label,'census-result.json')
    assert result['status']=='PASS' and result['mutant_caught']
    miscounts.append(dict(label=label,result=result))
build=json.loads((w/'part-b-build-gate.json').read_text())
assert build['status']=='PASS' and build['archive']['files']==archive['files']
record=dict(status='PASS',source_commit=archive['source_commit'],archive=archive,identity=identity,positive=positive,lifetimes=lifetimes,mutants=mutants,refusals=refusals,refusal_attempts=refusal_attempts,regressions=regressions,census=census,miscounts=miscounts)
(w/'part-b-gates.json').write_text(json.dumps(record,indent=2))
print(json.dumps(dict(status='PASS',required_positive_events=sum(row['events'] for row in positive[:4]),required_positive_frames=sum(row['frames'] for row in positive[:4]),labelled=sum(row['labelled'] for row in positive[:4]),regression_events=sum(row['events'] for row in regressions))))
