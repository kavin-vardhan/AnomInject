import json
from pathlib import Path
import re
import numpy as np
import capture_c5_kit as kit
from capture_c4_reader import raw,silhouette

def run():
    root=kit.J/'c5-attempts/C5/01/plugin-frames/epoch01'
    row=json.loads((root/'labels.jsonl').read_text().splitlines()[309])
    data=raw(root,row)
    measurements=[]
    for owner in row['census']['owners']:
        region=silhouette(data,[owner['tag']]);yy,xx=np.nonzero(region)
        if not len(xx):continue
        measurements.append(dict(actor=owner['actor'],pixels=len(xx),drawn_pct=100*len(xx)/region.size,
            pixel_bbox_pct=100*(int(xx.max())-int(xx.min())+1)*(int(yy.max())-int(yy.min())+1)/region.size,
            in_bounds_prefilter=owner['actor'] in row['census']['visible']))
    lost=[m for m in measurements if not m['in_bounds_prefilter'] and 0.5<=m['drawn_pct']<=25 and m['pixel_bbox_pct']<6]
    assert len(lost)>=2
    argv=kit.command('C5',kit.J/'test-fallback')
    original=list(argv)
    kit.select_map(argv,'fallback')
    commands=next(a for a in argv if a.startswith('-ExecCmds=')).split('=',1)[1].split(',')
    def admissible(sequence):
        values=[float(c.split()[1]) for c in sequence if c.startswith('IAI.SetMinScreenCoverage ')]
        return len(values)==1 and values[0]==row['census']['floor_pct'] and all(m['drawn_pct']>=values[0] for m in lost)
    assert admissible(commands)
    old_commands=next(a for a in original if a.startswith('-ExecCmds=')).split('=',1)[1].split(',')
    assert not admissible(old_commands)
    for value in ('0','6','26'):
        assert not admissible([c.replace('IAI.SetMinScreenCoverage 0.5','IAI.SetMinScreenCoverage '+value) for c in commands])
    street=kit.command('C5',kit.J/'test-street');kit.select_map(street,'street')
    assert not any('IAI.SetMinScreenCoverage' in a for a in street)
    source=(Path(__file__).resolve().parents[2]/'Source/AnomalyCapture/Private/Compat/AnomalyCensusPolicy425.h').read_text()
    assert 'Floor = 0.5f' in source and 'Ceiling = 25.0f' in source
    result=dict(result='PASS',source_frame=309,measurements=measurements,
        negative='Original fallback command has no bounds preset; missing, zero/disabled, 6 and 26 percent presets refused',
        positive='Fallback alone sets the bounds prefilter to the existing measured-census floor; street unchanged',
        census_floor=0.5,census_ceiling=25,native_change=False,python27='N/A: external Python launch tooling only')
    path=kit.J/'tests/fallback-summary.json';path.write_text(json.dumps(result,indent=2));print(json.dumps(result))

if __name__=='__main__':run()
