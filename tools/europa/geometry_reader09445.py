import json,sys
from pathlib import Path
import numpy as np
sys.path.insert(0,'D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa')
from capture_c4_reader import raw,silhouette

TARGETS={'EA_HideCube','EA_BlinkCube','EA_LOD'}

def qualifies(center,pixels,width,height):
    return bool(center['projected'] and 0<=center['x']<width and 0<=center['y']<height and .005<=pixels/(width*height)<=.25)

def read(root):
    root=Path(root);meta=json.loads((root/'geometry-ready.json').read_text())
    rows=[json.loads(s) for s in (root/'labels.jsonl').read_text().splitlines()]
    native=json.loads((root/'labels-receipt.json').read_text());cap=json.loads((root/'capture.json').read_text())
    assert meta['valid'] and len(rows)==native['paired']==cap['written']==32
    assert cap['dropped']==native['dropped']==native['unmatched_dropped']==0
    assert native['result']=='PASS' and not native['events'] and native['mask_state_restored'] and native['pacing_restored']
    measurements=[];regions={};valid=True
    for row in rows[-8:]:
        assert not row['sampled_states'] and row['mask'] is None
        assert row['width']==meta['width'] and row['height']==meta['height']
        camera=row['camera'];assert camera['target'].endswith('.'+meta['target']) and abs(camera['view_fov']-meta['fov'])<.01
        assert np.allclose(camera['view_location'],camera['player_location'],atol=.01) and np.allclose(camera['view_rotation'],camera['player_rotation'],atol=.01)
        data=raw(root,row);owners={o['actor']:o['tag'] for o in row['census']['owners']}
        for center in meta['centers']:
            name=center['actor'];assert name in TARGETS and name in owners
            region=silhouette(data,[owners[name]]);pixels=int(region.sum());good=qualifies(center,pixels,row['width'],row['height']);valid &=good
            measurements.append(dict(frame=row['frame_index'],actor=name,pixels=pixels,drawn_pct=100*pixels/(row['width']*row['height']),center=center,result='PASS' if good else 'FAIL'))
            if name in regions:assert np.array_equal(regions[name],region),'Unstable private silhouette before preview'
            regions[name]=region
    assert set(regions)==TARGETS
    result=dict(result='PASS' if valid else 'FAIL',measurements=measurements,view=meta,frames=32,measured_frames=8,no_events=True,source='same-session private ID/depth; no projection-area estimate')
    (root/'geometry-result.json').write_text(json.dumps(result,indent=2))
    return result,regions

def test():
    c=dict(projected=True,x=50,y=50)
    assert qualifies(c,100,100,100)
    assert not qualifies(dict(c,x=100),100,100,100)
    assert not qualifies(dict(c,projected=False),100,100,100)
    assert not qualifies(c,49,100,100)
    assert not qualifies(c,2501,100,100)
    return dict(result='PASS',positive=1,negative=4)
