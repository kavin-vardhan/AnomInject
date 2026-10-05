import json
import re
import sys
from pathlib import Path
import numpy as np
from PIL import Image

w = Path('E:/IA_BuildCache/_r921/09205')
root = w/'runs'/sys.argv[1]
reader_mutant=sys.argv[2] if len(sys.argv)>2 else None
assert reader_mutant in (None,'state','pixel')
launch = json.loads((root/'launch.json').read_text())
expected_path=Path(launch.get('expected',str(w/'census-expected-before.json')))
expected = json.loads(expected_path.read_text())[launch['fixture']+'-'+launch['scope']]
text = (root/'game.log').read_text(encoding='utf-8',errors='replace')
printed = [dict() for _ in range(3)]
lines = re.findall(r'IAI-VT-CENSUS v1 (.*)',text)
assert len(lines) == 18, len(lines)
for line in lines:
    assert re.fullmatch(r'[a-z0-9_=,:\- ]+', line), line
    pairs = dict(field.split('=',1) for field in line.split())
    family = int(pairs.pop('family'))
    for key,value in pairs.items():
        printed[family][key] = ({k:int(v) for k,v in (piece.split(':') for piece in value.split(','))} if ':' in value else int(value))
failures = []
for family,row in enumerate(expected):
    for key,value in row.items():
        if key == 'family': continue
        actual = printed[family].get(key)
        if isinstance(value,dict) and not value and actual == {'0':0}: actual = {}
        if actual != value: failures.append(dict(family=family,key=key,expected=value,actual=actual))
result = dict(expected_file=str(expected_path),counts_pass=not failures,failures=failures,expected=expected,printed=printed)
if launch['mutant']:
    result['mutant_caught'] = len(failures)==3 and all(f['key']=='bindings' and f['actual']==f['expected']+1 for f in failures)
    result['status'] = 'PASS' if result['mutant_caught'] else 'FAIL'
else:
    states = [dict(re.findall(r'(\w+)=(\d+)',line)) for line in re.findall(r'VT-CENSUS-STATE (.*)',text)]
    assert len(states)==2, states
    if reader_mutant=='state': states[1]['textures']=str(int(states[1]['textures'])+1)
    result['state_unchanged'] = states[0]==states[1] and states[0]['resources']==states[0]['producers']=='0'
    result['states'] = states
    session = list((root/'out').glob('session_*'))
    assert len(session)==1
    frames = sorted((session[0]/'Actual_Frames').glob('frame_*.png'))
    assert len(frames)==480, len(frames)
    rows=sorted((json.loads(line) for line in (session[0]/'labels.jsonl').read_text().splitlines()),key=lambda row:row['session_index'])
    assert len(rows)==len(frames)
    begin=re.findall(r'\[[^\]\r\n]+\]\[\s*(\d+)\]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 begin ',text)
    end=re.findall(r'\[[^\]\r\n]+\]\[\s*(\d+)\]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 end ',text)
    assert len(begin)==len(end)==1,(begin,end)
    first,last=rows[0]['frame_index'],rows[-1]['frame_index']
    starts=[i for i in range(first,last+1) if i%1000==int(begin[0])]
    ends=[i for i in range(first,last+1) if i%1000==int(end[0])]
    result['timing']=dict(first=first,last=last,null_end=rows[119]['frame_index'],census_begin=starts,census_end=ends)
    result['null_precedes_census']=len(starts)==len(ends)==1 and rows[119]['frame_index']+1<starts[0]<=ends[0]<last-1
    result['labelled_events_absent']=all(not row['anomaly_present'] and not row['anomalies'] for row in rows)
    reference = np.asarray(Image.open(frames[100]).convert('RGB'),dtype=np.int16)
    assert reference.shape==(1080,1920,3)
    boxes = dict(target=[67,383,1341,697],co_user=[1455,195,1705,445],ordinary=[1455,635,1705,885])
    mads = {role:[] for role in boxes}
    for index,frame in enumerate(frames):
        pixels = np.asarray(Image.open(frame).convert('RGB'),dtype=np.int16)
        assert pixels.shape==reference.shape
        if reader_mutant=='pixel' and index==320: pixels[383:697,67:1341]=0
        for role,(x0,y0,x1,y1) in boxes.items():
            mads[role].append(float(np.abs(pixels[y0:y1,x0:x1]-reference[y0:y1,x0:x1]).mean()))
    result['pixels'] = {role:dict(null_max=max(values[:120]),limit=max(values[:120])+.05,peak=max(values[120:]),
        failed_frames=[i for i in range(120,480) if values[i]>max(values[:120])+.05]) for role,values in mads.items()}
    result['status'] = 'PASS' if result['counts_pass'] and result['state_unchanged'] and result['null_precedes_census'] and result['labelled_events_absent'] and all(not row['failed_frames'] for row in result['pixels'].values()) else 'FAIL'
if reader_mutant:
    result['reader_mutant']=reader_mutant
    result['caught']=result['status']=='FAIL' and (not result['state_unchanged'] if reader_mutant=='state' else 320 in result['pixels']['target']['failed_frames'])
(root/('census-result.json' if not reader_mutant else 'reader-can-fail-'+reader_mutant+'.json')).write_text(json.dumps(result,indent=2))
print(json.dumps(result,indent=2))
raise SystemExit(result['status']!='PASS')
