import json
import sys
from pathlib import Path
import numpy as np
from PIL import Image

w=Path('E:/IA_BuildCache/_r921/09205')
r=w/'runs'/sys.argv[1]
session=next((r/'out').glob('session_*'))
rows=[json.loads(s) for s in (session/'labels.jsonl').read_text().splitlines()]
annotation=json.loads((session/'annotation.json').read_text())
indices=[100]
for event in annotation['anomalies']:
    frames=event.get('injected_frames',{}).get('frame_indices',[])
    if frames: indices.append(frames[len(frames)//2])
regions=dict(roughness=[80,388,1328,479],normal=[80,498,1328,579],color=[80,600,1328,688],co_user=[1455,195,1705,445],ordinary=[1455,635,1705,885])
result=dict(events=len(annotation['anomalies']),samples=[])
reference=np.asarray(Image.open(session/rows[100]['image']).convert('RGB'),dtype=np.int16)
for index in indices:
    pixels=np.asarray(Image.open(session/rows[index]['image']).convert('RGB'),dtype=np.int16)
    measures={}
    for role,(x0,y0,x1,y1) in regions.items():
        data=pixels[y0:y1,x0:x1]
        measures[role]=dict(mean=float(data.mean()),mad=float(np.abs(data-reference[y0:y1,x0:x1]).mean()),gradient_y=float(np.abs(np.diff(data,axis=0)).mean()),gradient_x=float(np.abs(np.diff(data,axis=1)).mean()))
    result['samples'].append(dict(index=index,image=str(session/rows[index]['image']),measures=measures))
(r/'calibration.json').write_text(json.dumps(result,indent=2))
print(json.dumps(result,indent=2))
