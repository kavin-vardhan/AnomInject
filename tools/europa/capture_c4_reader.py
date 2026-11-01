import json
from pathlib import Path
import zlib
import numpy as np
from PIL import Image
from capture_c3_reader import metrics,distribution,need,THRESHOLDS,DEPTH_BIAS

ROUTES=('blinking','lighting_mismatch','lod_corruption','missing_object')
REFERENCE_RULE='Last captured frame with sample_frame strictly less than live-fire StartFrame; the auto-fire intervals must not overlap. No post-selection by pixels or labels.'
CLASSIFIER=dict(change_level=1,clean_fraction_max=0.001,active_fraction_min=0.05)

def raw(root,row):
    size=(row['height'],row['width'],4)
    data=zlib.decompress((root/row['mask_evidence']).read_bytes())
    need(len(data)==int(np.prod(size))*4,'Raw extent differs')
    return np.frombuffer(data,dtype='<u4').reshape(size)

def rgb(root,row):
    value=np.asarray(Image.open(root/row['image']).convert('RGB'),dtype=np.int16)
    need(value.shape==(row['height'],row['width'],3),'RGB extent differs')
    return value

def silhouette(data,tags):
    custom=data[:,:,1].copy().view('<f4');scene=data[:,:,2].copy().view('<f4')
    return np.isin(data[:,:,0],tags)&(custom>0)&(custom>=scene-DEPTH_BIAS)

def classify(changed,region):
    fraction=float(np.count_nonzero(changed&region))/max(1,int(region.sum()))
    state=False if fraction<=CLASSIFIER['clean_fraction_max'] else True if fraction>=CLASSIFIER['active_fraction_min'] else None
    return state,fraction

def select_reference(rows,event):
    candidates=[r for r in rows if r['sample_frame']<event['start_frame']]
    need(bool(candidates),'No pre-apply reference')
    return candidates[-1]

def table(data):
    result=np.zeros((256,6),dtype='<u4');m=data[:,:,3]&255;d=(data[:,:,3]>>8)&255
    for tag in np.unique(m):
        if not tag:continue
        yy,xx=np.nonzero(m==tag)
        result[tag,:5]=[len(xx),(~int(xx.min()))&0xffffffff,(~int(yy.min()))&0xffffffff,int(xx.max()),int(yy.max())]
    for tag,count in zip(*np.unique(d,return_counts=True)):
        if tag:result[tag,5]=count
    return result

def read(root):
    root=Path(root)
    rows=[json.loads(s) for s in (root/'labels.jsonl').read_text().splitlines() if s]
    native=json.loads((root/'labels-receipt.json').read_text());capture=json.loads((root/'capture.json').read_text())
    need(native['result']=='PASS' and native['mask_state_restored'] and native['pacing_restored'],'Native pairing/restore failed')
    need(len(rows)==capture['written']==capture['requested']==native['paired'],'Frame count differs')
    need([r['frame_index'] for r in rows]==list(range(len(rows))),'Missing/duplicate index')
    need(capture['dropped']==native['unmatched_dropped']==0,'Dropped frames')
    need(len({(r['epoch'],r['world_owner'],r['request']) for r in rows})==len(rows),'Duplicate pairing key')
    need(all(b['sample_frame']>a['sample_frame'] and b['family_frame']>a['family_frame'] for a,b in zip(rows,rows[1:])),'Stale family')
    events=native['events'];need(bool(events),'No real auto fires')
    need(all('end_index' in e and e['end_index']<len(rows)-1 for e in events),'Incomplete auto event')
    need(all(a['end_frame']<b['start_frame'] for a,b in zip(events,events[1:])),'Overlapping reference intervals')
    references={};frames={};scores=[];clean=[];mismatches=[];ambiguous=[];edge_counts={r:dict(apply=0,revert=0,blink_toggle=0) for r in ROUTES}
    negatives={kind:{r:[] for r in ROUTES} for kind in ('shift','wrong_target')}
    tagmap={v['tag']:v['actor'] for v in native['stencil_owners']}
    for e in events:
        base=select_reference(rows,e);data=raw(root,base)
        reference=silhouette(data,e['target_tags']);need(reference.any(),'Empty pre-apply silhouette')
        references[e['event_id']]=dict(row=base,rgb=rgb(root,base),silhouette=reference,data=data)
        for i in range(base['frame_index'],min(e['end_index']+2,len(rows))):
            need(i not in frames,'Event scoring windows overlap')
            frames[i]=e
    previous_mask=None
    states_by_event={e['event_id']:[] for e in events}
    for i,row in enumerate(rows):
        need(row['aa']==0 and row['eye_adaptation_quality']==0,'AA/exposure differs')
        need(capture['frames'][i]['format'] in (2,18,37),'Unsupported format')
        data=raw(root,row);image=rgb(root,row)
        need(image.max()>0,'Black frame')
        gpu=data[:,:,3]&255;mask=gpu>0
        if row['mask'] is None:
            need(row['mask_state']=='empty' and not mask.any() and not (root/('mask-%06d.png'%i)).exists(),'Dirty empty mask')
        else:
            actual=np.asarray(Image.open(root/row['mask']).convert('L'))
            need(mask.any() and np.array_equal(actual,gpu),'Mask PNG/GPU mismatch')
        need(np.array_equal(table(data),np.fromfile(root/('reduce-%06d.u32'%i),dtype='<u4').reshape(256,6)),'Reduction mismatch')
        claimed=bool(row['anomalies']);e=frames.get(i)
        if e is None:
            if claimed or mask.any():mismatches.append(dict(index=i,reason='activity outside complete auto event'))
            clean.append(dict(index=i,pixels=int(mask.sum())))
            previous_mask=mask;continue
        route=e['id'];base=references[e['event_id']]
        changed=np.max(np.abs(image-base['rgb']),axis=2)>CLASSIFIER['change_level']
        region=base['silhouette']
        state,fraction=classify(changed,region)
        states_by_event[e['event_id']].append(dict(index=i,state=state,fraction=fraction))
        if state is None:ambiguous.append(dict(index=i,anomaly=route,fraction=fraction))
        elif state!=claimed:mismatches.append(dict(index=i,anomaly=route,pixels=state,label=claimed,fraction=fraction))
        if state:
            ref=silhouette(data,e['target_tags']) if route=='lod_corruption' else base['silhouette']
            result=metrics(mask,ref,changed,route)
            result.update(index=i,event=e['event_id'],anomaly=route,reference_index=base['row']['frame_index'])
            scores.append(result)
            shifted=previous_mask if previous_mask is not None else np.zeros_like(mask)
            shifted_result=metrics(shifted,ref,changed,route)
            if shifted_result['result']=='FAIL':negatives['shift'][route].append(i)
            others=sorted(set(tagmap)-set(e['target_tags']))
            if others:wrong=silhouette(base['data'],[others[0]])
            else:
                candidates=[silhouette(base['data'],[t]) for t in e['target_tags']]
                wrong=max(candidates,key=lambda x:int(x.sum()))
            if metrics(wrong,ref,changed,route)['result']=='FAIL':negatives['wrong_target'][route].append(i)
        else:
            clean.append(dict(index=i,pixels=int(mask.sum()),anomaly=route))
            if previous_mask is not None and previous_mask.any():negatives['shift'][route].append(i)
        previous_mask=mask
    event_results=[]
    for e in events:
        values=states_by_event[e['event_id']];bits=[x['state'] for x in values]
        transitions=[(a,b) for a,b in zip(bits,bits[1:]) if a is not None and b is not None and a!=b]
        complete=bool(bits) and bits[0] is False and bits[-1] is False and True in bits and None not in bits
        edge_counts[e['id']]['apply']+=int(complete);edge_counts[e['id']]['revert']+=int(complete)
        if e['id']=='blinking':edge_counts[e['id']]['blink_toggle']+=max(0,len(transitions)-2)
        event_results.append(dict(event=e['event_id'],anomaly=e['id'],target=e['target_name'],reference_index=references[e['event_id']]['row']['frame_index'],complete=complete,pixel_states=values))
    per=[]
    for route in ROUTES:
        subset=[r for r in scores if r['anomaly']==route];fired=[e for e in event_results if e['anomaly']==route]
        failed=sum(r['result']!='PASS' for r in subset)
        mismatch=sum(r.get('anomaly')==route for r in mismatches)
        per.append(dict(anomaly=route,events_fired=len(fired),completed=sum(e['complete'] for e in fired),frames=len(subset),failed_frames=failed,c2_mismatches=mismatch,
            result='PASS' if len(fired)>=3 and all(e['complete'] for e in fired) and subset and not failed and not mismatch else 'FAIL',
            **{k:distribution([r[k] for r in subset]) for k in ('iou','precision','spill','completeness')}))
    negative_results=[dict(kind=k,result='PASS' if all(v[r] for r in ROUTES) else 'FAIL',failed_frames_by_anomaly=v) for k,v in negatives.items()]
    good=not mismatches and not ambiguous and not any(c['pixels'] for c in clean) and all(p['result']=='PASS' for p in per) and all(n['result']=='PASS' for n in negative_results)
    return dict(result='PASS' if good else 'FAIL',reference_rule=REFERENCE_RULE,classifier=CLASSIFIER,thresholds=THRESHOLDS,frames=len(rows),size=[rows[0]['width'],rows[0]['height']],
        per_anomaly=per,c2=dict(result='PASS' if not mismatches and not ambiguous and all(e['complete'] for e in event_results) else 'FAIL',mismatches=mismatches,ambiguous=ambiguous,edge_counts=edge_counts),
        per_frame=scores,events=event_results,mask_negatives=negative_results,clean_mask_pixels=sum(c['pixels'] for c in clean),
        sequence=[[e['id'],e['target_name']] for e in events],seed=native['seed'],rhi=capture['rhi'],format=capture['backbuffer_format'],dropped=capture['dropped'])
