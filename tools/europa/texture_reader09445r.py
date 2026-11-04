import json
from pathlib import Path
import numpy as np
from PIL import Image
import capture_texture425_reader as strict
from capture_c4_reader import raw


def explained(reference,current,prediction,mask):
    delta=strict.changed(current,reference)
    unchanged=mask & ~delta
    black=unchanged & np.all(reference==0,axis=2) & np.all(current==0,axis=2)
    matched=unchanged & ~strict.changed(reference,prediction) & ~strict.changed(current,prediction)
    accounted=black|matched
    count=int(mask.sum())
    return dict(changed=int((delta&mask).sum()),unchanged=int(unchanged.sum()),black=int(black.sum()),matching_prediction=int((matched&~black).sum()),unexplained=int((unchanged&~accounted).sum()),outside=int((delta&~mask).sum()),accounted_share=float(((delta&mask)|accounted).sum()/max(count,1)))


def timing_edges(event,rows,load,reference):
    values=[]
    for index in (event['first']-2,event['first']-1,event['end'],event['end']+1):
        if not 0<=index<len(rows):
            values.append(dict(index=index,pass_clean=False,reason='Edge not captured'));continue
        count=int(strict.changed(load(index),reference).sum())
        values.append(dict(index=index,changed_pixels=count,pass_clean=count==0))
    return values


def read(root,log,attempt):
    root=Path(root);attempt=Path(attempt)
    qualification=json.loads((attempt/'prefire-result.json').read_text())
    assert qualification['result']=='PASS' and len(qualification['table'])==1
    objects={o['actor']:o for o in qualification['table'][0]['objects']}
    assert set(objects)=={'EA_HideCube','EA_BlinkCube','EA_LOD','EA_Floor'}
    preflight=json.loads((attempt/'prefire.json').read_text())
    predictions={}
    for sample in preflight['samples']:
        if sample['material']==0:continue
        folder=Path(sample['directory']);cap=json.loads((folder/'capture.json').read_text())
        images=[np.asarray(Image.open(folder/r['file']).convert('RGB'),dtype=np.float32) for r in cap['frames']]
        predictions[('missing_texture','corrupted_texture')[sample['material']-1]]=np.rint(np.mean(images,axis=0)).astype(np.uint8)
    result=strict.read(root,log,False)
    rows=[json.loads(s) for s in (root/'labels.jsonl').read_text().splitlines()]
    by_event={e['event']:e for e in result['events']}
    def load(index):return np.asarray(Image.open(root/rows[index]['image']).convert('RGB'))
    references={e['event']:load(e['reference_index']) for e in result['events']}
    for value in result['per_frame']:
        name=value['target'];anomaly=value['anomaly'];assert name in objects,'Selected target was not prequalified: '+name
        mode=objects[name]['swaps'][anomaly]['g3_mode'];value['g3_mode']=mode
        row=rows[value['index']];state=next(s for s in row['sampled_states'] if s['event_id']==value['event'])
        mask=np.isin(raw(root,row)[:,:,3]&255,state['target_tags'])
        evidence=explained(references[value['event']],load(value['index']),predictions[anomaly],mask)
        value['explained_pixels']=evidence
        value['g3_pass']=value['iou']>=.98 if mode=='strict' else evidence['outside']==0 and evidence['accounted_share']>=.98
        value['g2_pass']=value['changed_target_pixels']>0
    for event in result['events']:
        samples=[v for v in result['per_frame'] if v['event']==event['event']]
        event['edges']=timing_edges(event,rows,load,references[event['event']])
        event['strict_result']=event['result']
        event['g3_mode']=objects[event['target']]['swaps'][event['anomaly']]['g3_mode']
        event['explained_share']=strict.distribution([s['explained_pixels']['accounted_share'] for s in samples])
        event['result']='PASS' if samples and all(s['g3_pass'] and s['g2_pass'] for s in samples) and event['edges'] and all(e['pass_clean'] for e in event['edges']) else 'FAIL'
    result['strict_result']=result['result']
    result['fallback_rule']='GM2 explained-pixel G3: selected before fire only from failed98% preview visibility. Independent mean-of-eight preview RGB must be within2 of both the frame before fire and the unchanged current pixel; alternatively exact black in both. No outside change; all edges clean; changed-or-explained>=.98.'
    result['qualification']=str(attempt/'prefire-result.json')
    result['fallback_frames']=sum(v['g3_mode']!='strict' for v in result['per_frame'])
    result['result']='PASS' if all(v['events']>=3 for v in result['per_anomaly'].values()) and result['events'] and all(e['result']=='PASS' for e in result['events']) and result['residual_result']=='PASS' else 'FAIL'
    return result


def test():
    base=np.full((10,10,3),80,dtype=np.uint8);active=base.copy();mask=np.zeros((10,10),bool);mask[2:8,2:8]=True
    active[mask]=[30,80,170];prediction=active.copy()
    assert explained(base,active,prediction,mask)['accounted_share']==1
    active[2:5,2:8]=base[2:5,2:8]
    assert explained(base,active,prediction,mask)['accounted_share']==.5
    prediction[2:5,2:8]=base[2:5,2:8]
    assert explained(base,active,prediction,mask)['accounted_share']==1
    active[0,0]=255;assert explained(base,active,prediction,mask)['outside']==1
    pictures=[base.copy() for _ in range(8)];event=dict(first=3,end=6)
    assert all(v['pass_clean'] for v in timing_edges(event,pictures,lambda i:pictures[i],base))
    pictures[1][mask]=255
    assert not all(v['pass_clean'] for v in timing_edges(event,pictures,lambda i:pictures[i],base))
    pictures[1]=base.copy();pictures[7][mask]=255
    assert not all(v['pass_clean'] for v in timing_edges(event,pictures,lambda i:pictures[i],base))
    return dict(result='PASS',positive=3,unpredicted_unchanged_negative=1,spill_negative=1,early_edge_negative=1,late_edge_negative=1)
