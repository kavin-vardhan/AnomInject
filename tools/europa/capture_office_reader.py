import copy
import json
import math
from pathlib import Path
import re
import numpy as np
from PIL import Image
from capture_c4_reader import raw,ROUTES
from capture_c5_reader import annotation_check,depth_check,view_matches,census_protocol
from capture_c3_reader import need

REFERENCE_RULE='FU5: six fixed poses, pitch -12 and yaw steps 60; turn 18, settle 30, clean off/on holds 28 each, auto hold 240. No label-free grading. Wall pose 3 rows 686..719: each on-frame RGB must stay inside its grain-phase off-hold channel range plus/minus one level; no unstable-pixel exclusion.'
THRESHOLDS=dict(events_each=3,drawn_actors_exclusive_min=55,wall_changed_pixels=0,clean_mask_pixels=0,lost_batches=0)

def annotation_values(annotation,rows,events):
    normal=[]
    for row in rows:
        value=dict(row);value['camera']=dict(row['camera'])
        if value['camera']['far']<=0:value['camera']['far']=1000000
        normal.append(value)
    result=annotation_check(annotation,normal,events)
    bad=[];measurements=[]
    for event in events:
        p=event.get('selection_provenance',{});v=p.get('coverage_pct')
        valid=isinstance(v,(int,float)) and math.isfinite(v) and ((p.get('valid') and 0<=v<=100) or (p.get('valid') is False and v==-1))
        if not valid:bad.append(event['event_id'])
        measurements.append(dict(event=event['event_id'],anomaly=event['id'],coverage_pct=v,measured=p.get('valid'),valid=valid))
    result['coverage_provenance']=measurements
    result['coverage_measured']=sum(bool(m['measured']) for m in measurements)
    result['coverage_unmeasurable']=sum(m['measured'] is False for m in measurements)
    result['coverage_value_errors']=bad
    if bad:result['result']='FAIL'
    return result

def self_checks(log):
    lines=[line.split('Capture425 SELF_CHECK ',1)[1] for line in log.splitlines() if 'Capture425 SELF_CHECK ' in line]
    parsed=[dict(re.findall(r'(\w+)=([^ ]+)',line)) for line in lines]
    good=len(parsed)==2
    if good:
        for item,stage,frames in zip(parsed,('start','300'),('1','300')):
            good &= item.get('stage')==stage and item.get('frames')==frames and item.get('size')=='1280x720'
            good &= item.get('format') in ('2','18','37') and item.get('RHI') not in (None,'unknown')
            good &= int(item.get('PostRenderViewFamily','0'))>0 and item.get('pre_ui')=='1'
            good &= item.get('tick')=='OnWorldPostActorTick/LEVELTICK_All' and item.get('census') in ('on','bench_off')
            good &= item.get('lost')=='0' and item.get('no_pass')=='0' and item.get('polluted')=='0'
            good &= item.get('viewport','').endswith('/FSceneViewport') and not item['viewport'].startswith('none')
    return dict(result='PASS' if good else 'FAIL',lines=['Capture425 SELF_CHECK '+line for line in lines],parsed=parsed)

def wall_range(off,on,load):
    low={};high={}
    for sample in off:
        phase=sample['phase'];a=load(sample['index']).astype(np.int16)
        if phase not in low:low[phase]=a.copy();high[phase]=a.copy()
        else:np.minimum(low[phase],a,out=low[phase]);np.maximum(high[phase],a,out=high[phase])
    frames=[]
    for sample in on:
        phase=sample['phase'];need(phase in low,'Wall grain phase absent from off hold')
        a=load(sample['index']).astype(np.int16)
        changed=np.any((a<low[phase]-1)|(a>high[phase]+1),axis=2)
        frames.append(dict(index=sample['index'],phase=phase,changed_pixels=int(changed.sum())))
    maximum=max((f['changed_pixels'] for f in frames),default=-1)
    return dict(result='PASS' if maximum==0 and set(low)==set(range(7)) else 'FAIL',max_changed_pixels=maximum,frames=frames,rule=REFERENCE_RULE)

def mask_agrees(row,mask):
    affected=any(s['affected'] for s in row['sampled_states'])
    pixels=int(np.count_nonzero(mask))
    return affected==(pixels>0) and affected==bool(row['mask']),affected,pixels

def read(root,log):
    root=Path(root);rows=[json.loads(s) for s in (root/'labels.jsonl').read_text().splitlines() if s]
    receipt=json.loads((root/'labels-receipt.json').read_text());capture=json.loads((root/'capture.json').read_text())
    protocol=json.loads((root/'c5-protocol.json').read_text());frozen=json.loads((root/'prepass-frozen.json').read_text())
    need(len(rows)==protocol['total_frames']==capture['written']==capture['requested']==receipt['paired'],'Frame count differs')
    need([r['frame_index'] for r in rows]==list(range(len(rows))),'Missing or duplicate frame index')
    need(capture['dropped']==receipt['unmatched_dropped']==0,'Dropped/unpaired frame')
    need(receipt['result']=='PASS' and receipt['mask_state_restored'] and receipt['pacing_restored'],'Native pairing/restoration failed')
    need(all((r['width'],r['height'])==(1280,720) for r in rows),'Frame extent differs')
    need(all((root/r['image']).is_file() for r in rows),'Frame PNG absent')
    events=receipt['events'];counts={a:dict(anomaly=a,events_fired=sum(e['id']==a for e in events),affected_frames=0,mask_frames=0,mask_pixels=0) for a in ROUTES}
    bad_masks=[];view_bad=[];clean_pixels=0;affected_total=0;clean_total=0;drawn={p:set() for p in range(6)}
    on={};off={};protocols=[]
    for pose in frozen['poses']:
        p=pose['pose'];off[p]=pose['off_samples'];on[p]=[dict(index=s['index'],phase=rows[s['index']]['grain_phase']) for s in pose['silhouette_samples']]
        need(len(off[p])==len(on[p])==28,'Smoke clean hold length differs')
        protocols.append(census_protocol(rows,off[p],on[p]))
    sample_pose={s['index']:p for p in on for s in on[p]}
    for i,row in enumerate(rows):
        if not view_matches(row['camera']):view_bad.append(i)
        data=raw(root,row);mask=data[:,:,3]&255
        good,affected,pixels=mask_agrees(row,mask)
        path=root/('mask-%06d.png'%i)
        if affected:
            affected_total+=1
            good &= path.is_file()
            if path.is_file():good &= np.array_equal(np.asarray(Image.open(path).convert('L')),mask)
        else:
            clean_total+=1;clean_pixels+=pixels;good &= not path.exists()
        if not good:bad_masks.append(dict(index=i,affected=affected,pixels=pixels,png=path.exists()))
        for state in row['sampled_states']:
            if state['affected']:
                c=counts[state['id']];c['affected_frames']+=1
                n=int(np.isin(mask,state['target_tags']).sum());c['mask_frames']+=int(n>0);c['mask_pixels']+=n
        if i in sample_pose:
            custom=data[:,:,1].copy().view('<f4');scene=data[:,:,2].copy().view('<f4')
            visible=(custom>0)&(custom>=scene-0.000001)
            tags=set(int(v) for v in np.unique(data[:,:,0][visible]))
            drawn[sample_pose[i]].update(o['actor'] for o in row['census']['owners'] if o['tag'] in tags)
        if i%400==0:print('Office reader: frame %d/%d'%(i,len(rows)),flush=True)
    for value in counts.values():value['result']='PASS' if value['events_fired']>=3 and value['affected_frames']==value['mask_frames'] and value['mask_frames']>0 else 'FAIL'
    load=lambda i:np.asarray(Image.open(root/rows[i]['image']).convert('RGB'))[686:720,:,:]
    wall=wall_range(off[3],on[3],load);wall['pose']=3;wall['region']=[0,686,1280,720]
    census=dict(peak_drawn=max(len(v) for v in drawn.values()),per_pose=[dict(pose=p,drawn=len(v)) for p,v in drawn.items()],exhaustion=max(r['census']['ledger_exhausted'] for r in rows),lost=max(r['census']['batches_lost'] for r in rows),no_pass=max(r['census']['no_pass_batches'] for r in rows),polluted=max(r['census']['polluted_batches'] for r in rows))
    census['result']='PASS' if census['peak_drawn']>55 and all(census[k]==0 for k in ('exhaustion','lost','no_pass','polluted')) and all(p['result']=='PASS' for p in protocols) else 'FAIL'
    depth=depth_check(json.loads((root/'depth-before.json').read_text()),json.loads((root/'depth-after.json').read_text()),rows)
    annotation=annotation_values(json.loads((root/'annotation.json').read_text()),rows,events)
    checks=self_checks(log)
    view=dict(result='PASS' if not view_bad else 'FAIL',frames=len(rows),bad_frames=view_bad)
    good=all(c['result']=='PASS' for c in counts.values()) and not bad_masks and all(c['result']=='PASS' for c in (wall,census,depth,annotation,checks,view))
    return dict(result='PASS' if good else 'FAIL',profile='FU5; no label-free grading',frames=len(rows),per_anomaly=list(counts.values()),mask_check=dict(result='PASS' if not bad_masks else 'FAIL',affected_frames=affected_total,clean_frames=clean_total,clean_mask_pixels=clean_pixels,failures=bad_masks),wall_strip=wall,census=census,custom_depth=depth,annotation=annotation,self_check=checks,view=view,rhi=capture['rhi'],format=capture['backbuffer_format'],dropped=capture['dropped'])
