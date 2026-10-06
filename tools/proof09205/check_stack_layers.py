import json
import sys
from pathlib import Path
import numpy as np
from PIL import Image

w=Path('E:/IA_BuildCache/_r921/09205')
r=w/'runs'/sys.argv[1]
gate=json.loads((w/'part-b-pixel-gates-before.json').read_text())
session=next((r/'out').glob('session_*'))
rows=[json.loads(s) for s in (session/'labels.jsonl').read_text().splitlines()]
rows.sort(key=lambda r:r['session_index'])
assert [r['session_index'] for r in rows]==list(range(len(rows)))
annotation=json.loads((session/'annotation.json').read_text())
boxes=gate['bands_xyxy']
reference=np.asarray(Image.open(session/rows[100]['image']).convert('RGB'),dtype=np.int16)
windows=[]
for event in annotation['anomalies']:
    frames=event.get('injected_frames',{}).get('frame_indices',[])
    if not frames: continue
    middle=frames[len(frames)//2]
    template=np.asarray(Image.open(session/rows[middle]['image']).convert('RGB'),dtype=np.int16)
    windows.append(dict(event=event,frames=frames,middle=middle,templates={name:template[y0:y1,x0:x1].copy() for name,(x0,y0,x1,y1) in boxes.items()},deltas={name:{} for name in boxes}))
metrics=[]
for row in rows:
    array=np.asarray(Image.open(session/row['image']).convert('RGB'),dtype=np.int16)
    assert array.shape==(1080,1920,3)
    band_arrays={name:array[y0:y1,x0:x1].copy() for name,(x0,y0,x1,y1) in boxes.items()}
    for window in windows:
        index=row['session_index']
        if window['frames'][0]-8<=index<=window['frames'][-1]+8:
            for name,band in band_arrays.items():
                window['deltas'][name][index]=float(np.abs(band-window['templates'][name]).mean())
    metrics.append(dict(index=row['session_index'],bands={name:dict(mad=float(np.abs(band-reference[boxes[name][1]:boxes[name][3],boxes[name][0]:boxes[name][2]]).mean()),gradient_y=float(np.abs(np.diff(band,axis=0)).mean())) for name,band in band_arrays.items()}))
null={name:max(row['bands'][name]['mad'] for row in metrics[:120]) for name in boxes}
issues=[]
events=[]
for event in annotation['anomalies']:
    frames=event.get('injected_frames',{}).get('frame_indices',[])
    if not frames:
        issues.append('empty event')
        continue
    middle=frames[len(frames)//2]
    bands={}
    for name in boxes:
        window=next(window for window in windows if window['event']['event_id']==event['event_id'])
        delta=window['deltas'][name]
        stable=max(delta[index] for index in range(middle-2,middle+3))
        template_limit=max(null[name],stable)+.05
        positive_limit=max(4.,4*max(null[name],.5))
        accepted=[i for i in delta if delta[i]<=template_limit and metrics[i]['bands'][name]['mad']>=positive_limit]
        expected=set(frames)
        missing=sorted(expected-set(accepted))
        extra=sorted(set(accepted)-expected)
        mad=float(np.median([metrics[i]['bands'][name]['mad'] for i in frames]))
        gradient=float(np.median([metrics[i]['bands'][name]['gradient_y'] for i in frames]))
        original=metrics[100]['bands'][name]['gradient_y']
        visible=mad>=positive_limit
        blurred=gradient<=.75*original
        if not visible or not blurred: issues.append(name+' not measurably blurred')
        if missing or extra: issues.append(name+' incorrect pixel edges or bound appearance')
        bands[name]=dict(median_mad=mad,positive_limit=positive_limit,baseline_gradient=original,blurred_gradient=gradient,gradient_ratio=gradient/original if original else None,visible=visible,blurred=blurred,template_reference=middle,template_limit=template_limit,missing_blur_frames=missing,extra_blur_frames=extra,pixel_first=min(accepted) if accepted else None,pixel_last=max(accepted) if accepted else None,label_first=frames[0],label_last=frames[-1])
    events.append(dict(id=event['event_id'],bands=bands))
if not events: issues.append('no events')
result=dict(status='PASS' if not issues else 'FAIL',issues=sorted(set(issues)),null=null,events=events,gate=str(w/'part-b-pixel-gates-before.json'))
(r/'layer-pixels.json').write_text(json.dumps(metrics,indent=1))
(r/'layer-result.json').write_text(json.dumps(result,indent=2))
print(json.dumps(result,indent=2))
raise SystemExit(result['status']!='PASS')
