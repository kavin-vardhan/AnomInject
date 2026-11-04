import collections
import json
from pathlib import Path
import re
import numpy as np
from PIL import Image
from capture_c4_reader import raw

TEXTURES=('missing_texture','corrupted_texture')
DEFAULT_ROUTES=('blinking','lod_corruption',*TEXTURES)
REGRESSION_ROUTES=('blinking','lighting_mismatch','lod_corruption','missing_object',*TEXTURES)
RULE='094-44: reference is the capture immediately before the event first_index; never selected by pixels. Colour change means max absolute RGB difference greater than 2 quantized levels. Active mask IoU against the whole colour-change silhouette must be at least0.98. Inspect the frame before apply and the first reverted frame too. Clean edge frames must have zero changed pixels under the same quantization-aware threshold; no anomalous frame is exempt. Missing-usage negative uses the engine default material as the skeletal baseline, so the same grey fallback must change no target pixels. Paired new magenta uses the kept R>220,G<40,B>220 and RGB change>32 rule.'


def distribution(values):
    a=np.asarray(values,dtype=float)
    return dict(count=len(a),min=float(a.min()),mean=float(a.mean()),p95=float(np.percentile(a,95)),max=float(a.max())) if len(a) else dict(count=0)


def iou(a,b):
    union=int((a|b).sum())
    return int((a&b).sum())/union if union else 1.0


def changed(a,b):
    return np.max(np.abs(a.astype(np.int16)-b.astype(np.int16)),axis=2)>2


def read(root,log,require_skeletal=False):
    root=Path(root)
    rows=[json.loads(line) for line in (root/'labels.jsonl').read_text().splitlines()]
    events=json.loads((root/'labels-receipt.json').read_text())['events']
    measurements=[];event_results=[];early=[];examples={}
    load=lambda r:np.asarray(Image.open(root/r['image']).convert('RGB'))
    for event in events:
        if event['id'] not in TEXTURES:continue
        first=event['first_index'];end=event.get('end_index')
        if first<1 or end is None or end>=len(rows):
            event_results.append(dict(event=event['event_id'],anomaly=event['id'],result='FAIL',reason='both edges not captured'));continue
        reference=load(rows[first-1]);before_raw=raw(root,rows[first-1])
        owners=[v['tag'] for v in rows[first-1]['census']['owners'] if v['actor']==event['target_name']]
        region=np.isin(before_raw[:,:,0],owners)
        visible=before_raw[:,:,1].copy().view('<f4')>0
        region &= visible
        samples=[];edges=[]
        for index in range(first-1,end+1):
            row=rows[index];states=[s for s in row['sampled_states'] if s['event_id']==event['event_id']]
            state=states[0] if states else None
            label=bool(state and state['affected'])
            delta=changed(load(row),reference)
            data=raw(root,row)
            mask=np.isin(data[:,:,3]&255,state['target_tags']) if state else np.zeros(delta.shape,dtype=bool)
            if state and not state['affected']:mask[:]=False
            score=iou(mask,delta)
            n=int((delta&mask).sum())
            value=dict(event=event['event_id'],anomaly=event['id'],target=event['target_name'],index=index,labelled=label,mask_pixels=int(mask.sum()),changed_pixels=int(delta.sum()),changed_target_pixels=n,iou=score)
            if label:
                samples.append(value);measurements.append(value)
                examples.setdefault(event['id'],dict(image=str(root/row['image']),mask=str(root/row['mask']),reference=str(root/rows[first-1]['image'])))
            else:
                count=int(delta.sum());total=int(delta.size)
                edge=dict(index=index,changed_target_pixels=count,target_pixels=total,pass_clean=total>0 and count==0)
                edges.append(edge)
            if index==first-1 and state and state['affected']:early.append(index)
        positive=bool(samples) and all(s['changed_target_pixels']>0 and s['iou']>=.98 for s in samples)
        positive &= bool(edges) and all(e['pass_clean'] for e in edges)
        event_results.append(dict(event=event['event_id'],anomaly=event['id'],target=event['target_name'],reference_index=first-1,first=first,end=end,frames=len(samples),changed_target_pixels=distribution([s['changed_target_pixels'] for s in samples]),iou=distribution([s['iou'] for s in samples]),edges=edges,result='PASS' if positive else 'FAIL'))
    counts={name:dict(events=sum(e['id']==name for e in events),skeletal_events=sum(e['id']==name and e['target_name']=='EA_Skeletal' for e in events),static_events=sum(e['id']==name and e['target_name']!='EA_Skeletal' for e in events),iou=distribution([v['iou'] for v in measurements if v['anomaly']==name])) for name in TEXTURES}
    residues=[dict(anomaly=name,residual=int(n)) for name,n in re.findall(r'(missing_texture|corrupted_texture): revert[^\n]* residual=(\d+)',log)]
    required=sum(e['id'] in TEXTURES for e in events)
    state_ok=len(residues)==required and all(r['residual']==0 for r in residues)
    good=all(v['events']>=3 and (not require_skeletal or (v['skeletal_events']>=3 and v['static_events']>=3)) for v in counts.values())
    good &= len(event_results)==required and bool(event_results) and all(v['result']=='PASS' for v in event_results) and state_ok and not early
    return dict(result='PASS' if good else 'FAIL',rule=RULE,per_anomaly=counts,events=event_results,per_frame=measurements,residuals=residues,residual_result='PASS' if state_ok else 'FAIL',examples=examples,require_skeletal=require_skeletal)


def test():
    clean=np.zeros((32,32,3),dtype=np.uint8);active=clean.copy();active[8:24,8:24]=[255,0,255]
    mask=np.zeros((32,32),dtype=bool);mask[8:24,8:24]=True
    assert iou(mask,changed(active,clean))==1
    assert iou(mask,changed(clean,clean))==0
    assert not changed(clean,clean).any()
    return dict(result='PASS',correct_iou=1,one_frame_early_iou=0,grey_fallback_changed_pixels=0)
