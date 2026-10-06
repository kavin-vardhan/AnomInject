import json
import re
import sys
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
root=w/'runs'/sys.argv[1]
minimum=int(sys.argv[2])
text=(root/'game.log').read_text(errors='replace')
launch=json.loads((root/'launch.json').read_text())
positive=json.loads((root/'positive-result.json').read_text())
assert launch['exit']==0 and positive['status']=='PASS'
events=len(positive['events'])
assert events>=minimum
drains=re.findall(r'VT_DRAIN resources=(\d+) producers=(\d+)',text)
refs=re.findall(r'VT_COMBINED_DRAIN refs=(\d+)',text)
combined=[dict(stack=int(a),layers=int(b),ready=int(c),pages=int(d),attempts=int(e),wait_ms=float(f),mapping=g) for a,b,c,d,e,f,g in re.findall(r'VT_COMBINED stack=(\d+) layers=(\d+) ready=(\d+) pages=(\d+) attempts=(\d+) wait_ms=([\d.]+) layer_bias_table_channel=([^\s]+)',text)]
applies=[dict(required=int(a),reserved=int(b),live=int(c),pending=int(d),peak=int(e),cap=int(f)) for a,b,c,d,e,f in re.findall(r'APPLIED .*?required=(\d+) reserved=(\d+) live=(\d+) pending=(\d+) peak=(\d+) cap=(\d+)',text)]
assert len(drains)==len(refs)==len(combined)==len(applies)==events,(events,len(drains),len(refs),len(combined),len(applies))
assert all(row==('0','0') for row in drains) and all(row=='0' for row in refs)
assert all(row['layers']==3 and row['ready']==1 and row['pages']==3 and row['mapping']=='0:0:0:0,1:0:0:1,2:0:0:2' and 0<row['attempts']<=120 and row['wait_ms']<=10000 for row in combined)
assert len({row['peak'] for row in applies})==1
assert all(row['required']==row['reserved']==row['live'] and row['pending']==0 and row['peak']<=row['cap'] for row in applies)
mutant=[dict(row) for row in applies]
mutant[-1]['peak']+=1
assert len({row['peak'] for row in mutant})>1 or len(mutant)==1
result=dict(status='PASS',events=events,drains=drains,combined_reference_drains=refs,combined=combined,applies=applies,growth_mutant_caught=len(mutant)>1)
(root/'cycles-result.json').write_text(json.dumps(result,indent=2))
print(json.dumps(dict(status='PASS',events=events,peak=applies[0]['peak'],combined_attempts=[r['attempts'] for r in combined],combined_ms=[r['wait_ms'] for r in combined])))
