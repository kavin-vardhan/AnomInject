import copy
import json
import math
from pathlib import Path
import re
import numpy as np
from PIL import Image
from capture_c4_reader import raw
from capture_c5_reader import annotation_check,view_matches,census_protocol
from capture_c3_reader import need

REFERENCE_RULE='FV5/FU5: six poses, pitch -12, street yaw steps 60 or fallback stationary player view; turn 18, settle 30, clean off/on holds 28 each, auto hold 240; seed/start once at first scored hold, continue through all holds/turns, stop 24 frames before end. FX3: each LOD event must change at least one current-target mask pixel by more than 2 RGB levels against the preceding captured frame. No label-free grading. Street wall pose 3 rows 686..719: on-frame RGB within grain-phase off-hold channel range +/-1. Fallback waives only wall strip and census over 55.'
THRESHOLDS=dict(events_each=3,drawn_actors_exclusive_min=55,wall_changed_pixels=0,clean_mask_pixels=0,lost_batches=0,lod_changed_pixels_min=1,lod_rgb_tolerance=2)

def annotation_values(annotation,rows,events):
    normal=[]
    for row in rows:
        value=dict(row);value['camera']=dict(row['camera'])
        value['camera']['far']=row['camera']['far'] if row['camera']['far']>0 else 1000000
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

def self_checks(log,size=(1280,720)):
    lines=[line.split('Capture425 SELF_CHECK ',1)[1] for line in log.splitlines() if 'Capture425 SELF_CHECK ' in line]
    parsed=[dict(re.findall(r'(\w+)=([^ ]+)',line)) for line in lines]
    good=len(parsed)==2
    if good:
        for item,stage,frames in zip(parsed,('start','300'),('1','300')):
            good &= item.get('stage')==stage and item.get('frames')==frames and item.get('size')==('%dx%d'%size)
            good &= item.get('format') in ('2','18','37') and item.get('RHI') not in (None,'unknown')
            good &= item.get('backbuffer_format') in ('2','18','37') and item.get('viewport_size')==item.get('size')
            good &= int(item.get('PostRenderViewFamily','0'))>0 and item.get('pre_ui')=='1'
            good &= item.get('tick')=='OnWorldPostActorTick425' and item.get('tick_type')=='2' and int(item.get('tick_calls','0'))>0
            good &= item.get('tick_source')=='build_constant' and item.get('tick_type_source')=='build_constant' and int(item.get('ui_paired','0'))>0
            good &= item.get('census') in ('on','bench_off')
            good &= item.get('lost')=='0' and item.get('no_pass')=='0' and item.get('polluted')=='0'
            good &= item.get('viewport') not in (None,'None','none') and item.get('client') not in (None,'None','none')
            good &= all(item.get(k) in ('0','1') for k in ('slate','pie','fixed_step','fixed_rate','smoothing'))
            good &= all(math.isfinite(float(item.get(k,'nan'))) for k in ('fixed_dt','fixed_fps'))
            good &= int(item.get('ui_positive','0'))>0 and int(item.get('ui_controls','0'))>0 and int(item.get('ui_draws','0'))>0
            good &= item.get('ui_checked')==frames and item.get('ui_hits')=='0'
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
    affected=any(s['affected'] and not s.get('whole_frame',False) for s in row['sampled_states'])
    pixels=int(np.count_nonzero(mask))
    return affected==(pixels>0) and affected==bool(row['mask']),affected,pixels

MARKER=np.array([(255,0,255),(0,255,0),(0,0,255),(255,255,0),(0,255,255),(255,0,0),(255,255,255),(0,0,0)],dtype=np.int16)

def marker_cells(pixels,origin=(16,16)):
    x,y=origin
    if pixels.shape[0]<y+32 or pixels.shape[1]<x+64:return -1
    return sum(bool(np.all(np.abs(pixels[y+(i//4)*16+8,x+(i%4)*16+8,:3].astype(np.int16)-MARKER[i])<=4)) for i in range(8))

def ui_pixels(root,rows):
    native=json.loads((root/'ui-check.json').read_text())
    controls=[];bad=[];observations=[];pairs=[]
    for control in native.get('control_frames',[]):
        path=root/control['image']
        cells=marker_cells(np.asarray(Image.open(path).convert('RGB')),(0,0)) if path.is_file() else -1
        controls.append(dict(control,observed_cells=cells))
    for row in rows:
        cells=marker_cells(np.asarray(Image.open(root/row['image']).convert('RGB')))
        observations.append(dict(request=row['frame_index']+1,sample_frame=row['sample_frame'],family_frame=row['family_frame'],matching_cells=cells))
        if cells<0 or cells>=7:bad.append(row['frame_index'])
    fields=('request','sample_frame','family_frame','matching_cells')
    same=[tuple(r.get(k) for k in fields) for r in native['frames']]==[tuple(r[k] for k in fields) for r in observations]
    by_request={r['request']:r for r in observations}
    native_by_request={r['request']:r for r in native['frames']}
    drawn=native.get('drawn_frames',[])
    for draw in drawn:
        n=native_by_request.get(draw['request'],{});frame=by_request.get(draw['request'],{})
        exact=[c for c in controls if c['request']==draw['request'] and c['sample_frame']==draw['sample_frame']]
        good=(len(exact)==1 and exact[0]['observed_cells']>=7 and exact[0]['observed_cells']==exact[0]['matching_cells']
              and frame.get('sample_frame')==draw['sample_frame'] and 0<=frame.get('matching_cells',-1)<7
              and n.get('drawn') is True and n.get('control_positive') is True)
        pairs.append(dict(draw,result='PASS' if good else 'FAIL',capture=frame,controls=exact))
    keys=[d['request'] for d in drawn]
    positive=sum(c['observed_cells']>=7 for c in controls)
    exact_keys=set(keys)=={r['request'] for r in native['frames'] if r.get('drawn')}=={c['request'] for c in controls}
    good=(bool(pairs) and len(keys)==len(set(keys)) and exact_keys and all(p['result']=='PASS' for p in pairs)
          and native.get('paired')==len(pairs)==native.get('draws')==native.get('controls')
          and not bad and same and native['checked']==len(rows) and native['hits']==0 and native['positive_controls']==positive)
    return dict(result='PASS' if good else 'FAIL',positive_controls=controls,checked=len(rows),marker_frames=bad,native_matches=same,per_frame_pairs=pairs,frames=observations)

def lod_guard(log,events):
    selections=[dict(target=name,eligible_components=int(count),min_lods=int(lods)) for name,count,lods in re.findall(r'Auto.LodTarget target=(\S+) eligible_components=(\d+) min_lods=(\d+)',log)]
    applied=[dict(component=name,chosen=int(chosen),lods=int(lods)) for name,chosen,lods in re.findall(r"lod_corruption: '([^']+)' forced LOD (\d+) of (\d+)",log)]
    choices=[dict(component=name,drawn=int(drawn),count=int(count),forced=int(forced)) for name,drawn,count,forced in re.findall(r'Auto.LodChoice component=(\S+) drawn=(-?\d+) count=(\d+) forced=(\d+)',log)]
    expected=[e for e in events if e['id']=='lod_corruption']
    by_name={e['target_name']:sum(s['target']==e['target_name'] for s in selections) for e in expected}
    good=(bool(expected) and len(applied)>=len(expected) and all(s['eligible_components']>0 and s['min_lods']>=2 for s in selections)
          and len(choices)==len(applied) and all(c['count']>=2 and 0<=c['drawn']<c['count'] and c['forced']==(1 if c['drawn']==c['count']-1 else c['count']) and c['forced']-1!=c['drawn'] for c in choices)
          and all(a['component']==c['component'] and a['lods']==c['count'] and a['chosen']==c['forced'] for a,c in zip(applied,choices))
          and all(by_name[e['target_name']]>=sum(x['target_name']==e['target_name'] for x in expected) for e in expected))
    return dict(result='PASS' if good else 'FAIL',events=len(expected),selections=selections,applied_components=applied,predictions=choices)


def lod_changes(root,rows,events):
    output=[]
    for event in events:
        if event['id']!='lod_corruption':continue
        first=event['first_index'];samples=[]
        before=max(0,first-1)
        reference=np.asarray(Image.open(root/rows[before]['image']).convert('RGB')).astype(np.int16)
        for row in rows[first:event.get('end_index',first+1)+1]:
            states=[s for s in row['sampled_states'] if s['event_id']==event['event_id'] and s['affected']]
            if not states:continue
            mask=raw(root,row)[:,:,3]&255
            target=np.isin(mask,states[0]['target_tags']);pixels=int(target.sum())
            frame=np.asarray(Image.open(root/row['image']).convert('RGB')).astype(np.int16)
            changed=int((np.any(np.abs(frame-reference)>THRESHOLDS['lod_rgb_tolerance'],axis=2)&target).sum())
            samples.append(dict(index=row['frame_index'],target_pixels=pixels,changed_pixels=changed,changed_fraction=changed/max(1,pixels)))
        output.append(dict(event=event['event_id'],target=event['target_name'],reference=before,frames=samples,max_changed_fraction=max((s['changed_fraction'] for s in samples),default=None),result='PASS' if any(s['changed_pixels']>=THRESHOLDS['lod_changed_pixels_min'] for s in samples) else 'FAIL',rule='FX3: RGB difference >2 on current target mask versus preceding captured frame; each event needs changed pixels'))
    return output

def depth_check(before,after,rows):
    states=lambda value: {c['component']:(c['on'],c['value'],c['write_mask']) for c in value['components']}
    old,new=states(before),states(after)
    differences=[k for k in set(old)|set(new) if old.get(k)!=new.get(k)]
    bad=[]
    for row in rows:
        d=row['custom_depth']
        if d['on_count'] or d['tagged_components'] or d['unexpected']:bad.append(row['frame_index'])
        for c in d['components']:
            if (c['on'],c['value'],c['write_mask'])!=(c['prior_on'],c['prior_value'],c['prior_write_mask']):bad.append(row['frame_index'])
    planted=[c for c in before['components'] if c.get('bench_setup')]
    return dict(result='PASS' if not differences and not bad and not planted else 'FAIL',candidates=len(old),differences=differences,bad_frames=sorted(set(bad)),bench_planted=planted,host_writes=0 if not bad else None,rule='GD5 removes the bench plant; before/mid/after states must be identical, enabled and tagged counts zero')

def read(root,log,fallback=False,size=(1280,720),expected_rhi=None,routes=("blinking","lod_corruption","missing_texture","corrupted_texture"),selection_floor=0.5):
    root=Path(root);rows=[json.loads(s) for s in (root/'labels.jsonl').read_text().splitlines() if s]
    receipt=json.loads((root/'labels-receipt.json').read_text());capture=json.loads((root/'capture.json').read_text())
    protocol=json.loads((root/'c5-protocol.json').read_text());frozen=json.loads((root/'prepass-frozen.json').read_text())
    need(protocol.get('fallback')==fallback and protocol['yaw_step']==(0 if fallback else 60),'Map profile and bench protocol differ')
    need(len(rows)==protocol['total_frames']==capture['written']==capture['requested']==receipt['paired'],'Frame count differs')
    need([r['frame_index'] for r in rows]==list(range(len(rows))),'Missing or duplicate frame index')
    need(capture['dropped']==receipt['unmatched_dropped']==0,'Dropped/unpaired frame')
    need(receipt['result']=='PASS' and receipt['mask_state_restored'] and receipt['pacing_restored'],'Native pairing/restoration failed')
    need(all((r['width'],r['height'])==size for r in rows),'Frame extent differs')
    need(all((root/r['image']).is_file() for r in rows),'Frame PNG absent')
    need(expected_rhi is None or capture['rhi']==expected_rhi,'RHI differs')
    events=[e for e in receipt['events'] if e['id']!='camera_clipping'];counts={a:dict(anomaly=a,events_fired=sum(e['id']==a for e in events),affected_frames=0,mask_frames=0,mask_pixels=0) for a in routes}
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
            if state['affected'] and not state.get('whole_frame',False):
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
    wall=dict(result='PASS',waived='FV5 fallback only') if fallback else wall_range(off[3],on[3],load)
    wall['pose']=3;wall['region']=[0,686,1280,720]
    census=dict(peak_drawn=max(len(v) for v in drawn.values()),per_pose=[dict(pose=p,drawn=len(v)) for p,v in drawn.items()],exhaustion=max(r['census']['ledger_exhausted'] for r in rows),lost=max(r['census']['batches_lost'] for r in rows),no_pass=max(r['census']['no_pass_batches'] for r in rows),polluted=max(r['census']['polluted_batches'] for r in rows))
    census['over_55_waived']=fallback
    census['result']='PASS' if (fallback or census['peak_drawn']>55) and all(census[k]==0 for k in ('exhaustion','lost','no_pass','polluted')) and all(p['result']=='PASS' for p in protocols) else 'FAIL'
    depth=depth_check(json.loads((root/'depth-before.json').read_text()),json.loads((root/'depth-after.json').read_text()),rows)
    annotation_document=json.loads((root/'annotation.json').read_text())
    annotation_document['anomalies']=[e for e in annotation_document['anomalies'] if e['anomaly_type']!='camera_clipping']
    annotation=annotation_values(annotation_document,rows,events)
    checks=self_checks(log,size)
    ui=ui_pixels(root,rows)
    lod=lod_changes(root,rows,events)
    guard=lod_guard(log,events)
    starts=re.findall(r'IAI.Auto.Run -> ON \(seed (\d+),',log)
    floors=[float(v) for v in re.findall(r'IAI.SetMinScreenCoverage -> ([0-9.]+)%',log)]
    single_start=dict(result='PASS' if starts==['4242'] and (floors==[selection_floor] or (selection_floor==6.0 and not floors)) else 'FAIL',seeds=starts,selection_floor_pct=floors,shipped_floor_pct=6.0)
    view=dict(result='PASS' if not view_bad else 'FAIL',frames=len(rows),bad_frames=view_bad)
    unavailable=json.loads((root/'private-availability.json').read_text())
    need(unavailable['enabled'] and unavailable['host_stencil_writes']==0,'Private availability/state receipt absent')
    good=all(c['result']=='PASS' for c in counts.values()) and not bad_masks and all(c['result']=='PASS' for c in (wall,census,depth,annotation,checks,view,ui,guard,single_start)) and bool(lod) and all(e['result']=='PASS' for e in lod)
    return dict(result='PASS' if good else 'FAIL',unavailable=unavailable,profile='GD5/FZ7 FV5/FU5; no label-free grading',fallback=fallback,frames=len(rows),per_anomaly=list(counts.values()),lod_visible_change=lod,lod_guard=guard,session_start=single_start,mask_check=dict(result='PASS' if not bad_masks else 'FAIL',affected_frames=affected_total,clean_frames=clean_total,clean_mask_pixels=clean_pixels,failures=bad_masks),wall_strip=wall,census=census,custom_depth=depth,annotation=annotation,self_check=checks,ui_pixels=ui,view=view,rhi=capture['rhi'],format=capture['backbuffer_format'],dropped=capture['dropped'])
