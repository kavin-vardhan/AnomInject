import datetime
import json
from pathlib import Path
import numpy as np
from PIL import Image
from capture_c4_reader import raw,rgb,silhouette,table,classify,ROUTES,CLASSIFIER
from capture_c3_reader import need,metrics,distribution,THRESHOLDS

REFERENCE_RULE='FT3: six fixed poses, pitch -12, yaw step 60; turn 18, settle 30, census-off 240 then census-on 240; score hold 480. First off-hold reference per renderer grain phase (4.25 GetFrameIndex(8) modulo 7). Per-phase range >1, union dilated 2px. First 120 build self-map, second 120 must be C2-clean. Keep >=50% stable and clean census-on. Freeze before first scored-pass image write. First clean held census sample per actor defines its pose silhouette. Event requires both edges in scored holds and >=50% stable target pixels. No after-run reference selection.'

ROOT_KEYS={'label_schema','label_schema_minor','session_id','transition_frame_count','video','anomalies'}
EVENT_KEYS={'event_id','transition_frames','transition_reasons','anomaly_type','anomaly_subtype','affected_frames','injected_frames','bbox_source','observable_frame_count','unmeasured_frame_count','observability_measured','manifested','coverage_ratio','coverage_pct','affected_objects','camera','engine','mask','depth'}
SET_KEYS={'start_frame','end_frame','frame_count','span_frame_count','frame_indices'}

def dilate(mask,radius=2):
    h,w=mask.shape;out=np.zeros_like(mask);pad=np.pad(mask,radius)
    for y in range(2*radius+1):
        for x in range(2*radius+1):out|=pad[y:y+h,x:x+w]
    return out

def stable_map(images):
    low=images[0].copy();high=low.copy()
    for a in images[1:]:np.minimum(low,a,out=low);np.maximum(high,a,out=high)
    unstable=dilate(np.max(high.astype(np.int16)-low.astype(np.int16),axis=2)>1)
    return unstable,float(unstable.mean())<=0.5

def changed_stats(image,reference,unstable):
    stable=~unstable;changed=(np.max(np.abs(image.astype(np.int16)-reference.astype(np.int16)),axis=2)>1)&stable
    count=int(changed.sum());n=int(stable.sum())
    return dict(changed_stable_pixels=count,stable_pixels=n,fraction=count/max(1,n))

def phase_prepass(samples,load):
    need(len(samples)>=240 and len(samples)%2==0,'Off hold shorter than 240 or uneven')
    refs={};low={};high={};half=len(samples)//2;self_frames=[]
    def map_now():
        need(set(refs)==set(range(7)),'Missing renderer grain phase')
        bad=np.zeros(next(iter(refs.values())).shape[:2],bool)
        for phase in refs:bad|=np.max(high[phase].astype(np.int16)-low[phase].astype(np.int16),axis=2)>1
        return dilate(bad)
    for n,sample in enumerate(samples):
        phase=sample['phase'];image=load(sample['index'])
        if phase not in refs:refs[phase]=image.copy();low[phase]=image.copy();high[phase]=image.copy()
        if n==half:halfmap=map_now()
        if n>=half:self_frames.append(dict(index=sample['index'],**changed_stats(image,refs[phase],halfmap)))
        np.minimum(low[phase],image,out=low[phase]);np.maximum(high[phase],image,out=high[phase])
    return refs,map_now(),self_frames

def freeze_before_frame(frozen,files,first_frame):
    epoch=Path(first_frame).stat().st_mtime
    return datetime.datetime.fromisoformat(frozen['frozen_utc'].replace('Z','+00:00')).timestamp()<=epoch and all(Path(p).stat().st_mtime<=epoch for p in files)

def census_protocol(rows,off,on):
    bad_off=[s['index'] for s in off if rows[s['index']]['census']['enabled']]
    bad_on=[s['index'] for s in on if not rows[s['index']]['census']['enabled']]
    return dict(result='FAIL' if bad_off or bad_on else 'PASS',enabled_in_off=bad_off,disabled_in_on=bad_on)

def frame_set(indices):
    return dict(start_frame=indices[0] if indices else 0,end_frame=indices[-1] if indices else 0,frame_count=len(indices),span_frame_count=indices[-1]-indices[0]+1 if indices else 0,frame_indices=indices)

def drawn_names(bank):
    return sorted(name for name,packed in bank.items() if np.any(packed))

def c2_result(rows,events,mismatches,ambiguity):
    frames=sum(bool(r['sweep']['score']) for r in rows)
    counts={route:sum(e['scored'] for e in events if e['anomaly']==route) for route in ROUTES}
    covered=frames>0 and all(n>=3 for n in counts.values())
    return dict(result='PASS' if covered and not mismatches and not ambiguity else 'FAIL',scored_frames=frames,scored_events=counts,coverage='PASS' if covered else 'FAIL',mismatches=mismatches,ambiguous=ambiguity)

def annotation_check(annotation,rows,events):
    issues=[]
    if set(annotation)!=ROOT_KEYS or annotation.get('label_schema')!=2 or annotation.get('label_schema_minor')!=1:issues.append('root schema')
    if set(annotation.get('video',{}))!={'path','frames_dir','resolution','fps','target_fps','total_frames'}:issues.append('video schema')
    wanted={e['event_id']+'|'+e['target_name']:e for e in events}
    if set(wanted)!={e['event_id'] for e in annotation['anomalies']}:issues.append('event identities')
    for e in annotation['anomalies']:
        key=e['event_id'].split('|')[0]
        if set(e)!=EVENT_KEYS:issues.append(key+' event schema')
        for name,field in (('affected_frames','affected'),('injected_frames','injected')):
            indices=[r['frame_index'] for r in rows if any(s['event_id']==key and s[field] for s in r['sampled_states'])]
            if e[name]!=frame_set(indices):issues.append(key+' '+name)
        matching=[(r,s) for r in rows for s in r['sampled_states'] if s['event_id']==key]
        visible=[s for r,s in matching if s['affected']]
        coverage=sum(s['projected_screen_share'] for s in visible)/max(1,len(visible))
        if abs(e['coverage_ratio']-coverage)>1e-8 or abs(e['coverage_pct']-100*coverage)>1e-6:issues.append(key+' coverage screen share')
        if e['bbox_source']!=('drawn' if any(s['drawn_pixels'] for _,s in matching) else 'projected'):issues.append(key+' bbox source')
        if matching and (abs(e['camera']['far']-matching[0][0]['camera']['far'])>0.01 or abs(e['camera']['near']-matching[0][0]['camera']['near'])>0.01):issues.append(key+' view clip planes')
        if set(e['camera'])!={'path','global_position','near','far','rotation','fov_deg','aspect'}:issues.append(key+' camera schema')
        if set(e['engine'])!={'ticks_msec','name','version','project'}:issues.append(key+' engine schema')
        if set(e['affected_objects'])!={'count','primary_index','nodes'}:issues.append(key+' objects schema')
        for node in e['affected_objects']['nodes']:
            if set(node)!={'name','path','global_position','asset_name','component_class','bounds'} or set(node['bounds'])!={'origin','extent'}:issues.append(key+' node schema')
    return dict(result='FAIL' if issues else 'PASS',issues=issues,events=len(wanted),schema='5.1 AnomalyLabelWriter.cpp WriteSessionAnnotation, 2.1')

def view_matches(camera):
    rotation=(np.asarray(camera['player_rotation'])-np.asarray(camera['view_rotation'])+180)%360-180
    return max(abs(rotation))<=0.01 and max(abs(np.asarray(camera['player_location'])-np.asarray(camera['view_location'])))<=0.01 and abs(camera['player_fov']-camera['view_fov'])<=0.01

def initial_view_check(first,declared):
    camera=dict(player_rotation=first['player_rotation'],view_rotation=declared['player_rotation'],player_location=first['player_location'],view_location=declared['player_location'],player_fov=first['player_fov'],view_fov=declared['player_fov'])
    return dict(result='PASS' if view_matches(camera) else 'FAIL',actual=first,declared=declared)

def enforce_initial_view(census,check):
    census['initial_view']=check
    if check['result']!='PASS':
        census['late_candidates_against_declared_pose']=census['late_scored_targets']
        census['late_scored_targets']=[]
        census['result']='FAIL'
    return census

def depth_check(before,after,rows):
    def states(v):return {c['component']:(c['on'],c['value'],c['write_mask']) for c in v['components']}
    b=states(before);a=states(after);diff=sorted(k for k in set(a)|set(b) if a.get(k)!=b.get(k))
    mids=[r['frame_index'] for r in rows if r['custom_depth']['unexpected'] or any(c['on']!=(c['prior_on'] or c['tagged']) for c in r['custom_depth']['components'])]
    untouched=[dict(index=r['frame_index'],component=c['component']) for r in rows for c in r['custom_depth']['components'] if c['prior_on'] and (c['tagged'] or (c['on'],c['value'],c['write_mask'])!=(c['prior_on'],c['prior_value'],c['prior_write_mask']))]
    reserved={c['value'] for c in before['components'] if c['on'] and 200<=c['value']<=254}
    assigned={o['tag'] for r in rows for o in r['census']['owners']}
    return dict(result='PASS' if not diff and not mids and not untouched and reserved and not(reserved&assigned) else 'FAIL',candidates=len(b),after_candidates=len(a),differences=diff,mid_failures=mids,host_custom_depth_changes=untouched,reserved=sorted(reserved),assigned_reserved=sorted(reserved&assigned),setup=[c for c in before['components'] if c['bench_setup']],mid_samples=len(rows))

def read(root):
    root=Path(root);rows=[json.loads(s) for s in (root/'labels.jsonl').read_text().splitlines() if s]
    receipt=json.loads((root/'labels-receipt.json').read_text());capture=json.loads((root/'capture.json').read_text())
    protocol=json.loads((root/'c5-protocol.json').read_text());frozen=json.loads((root/'prepass-frozen.json').read_text())
    need(len(rows)==protocol['total_frames']==capture['written']==capture['requested']==receipt['paired'],'C5 frame count differs')
    need([r['frame_index'] for r in rows]==list(range(len(rows))),'Missing or duplicate index')
    need(capture['dropped']==receipt['unmatched_dropped']==0,'Dropped/unpaired frame')
    need(receipt['result']=='PASS' and receipt['mask_state_restored'] and receipt['pacing_restored'],'Native pairing/restore failed')
    scored_first=next(r for r in rows if r['sweep']['phase']=='scored')
    frozen_files=[root/'prepass-frozen.json',root/'c5-protocol.json']
    pose_data={};prepass=[]
    for p in frozen['poses']:
        pose=p['pose'];samples=p['silhouette_samples'];off=p['off_samples']
        need(len(samples)==len(off)==protocol['clean_hold'] and len(off)>=240,'FT3 hold count differs')
        need(max(s['index'] for s in off)<min(s['index'] for s in samples),'Census-on precedes census-off')
        protocol_check=census_protocol(rows,off,samples)
        load=lambda i:np.asarray(Image.open(root/rows[i]['image']).convert('RGB'))
        refs,unstable,self_frames=phase_prepass(off,load)
        valid=True
        for ref in p['references']:
            phase=ref['phase'];frozen_files.append(root/ref['file'])
            valid &= ref['index']==next(s['index'] for s in off if s['phase']==phase)
            valid &= np.array_equal(refs[phase],np.asarray(Image.open(root/ref['file']).convert('RGB')))
        frozen_files.append(root/p['unstable'])
        valid &= np.array_equal(unstable,np.asarray(Image.open(root/p['unstable']).convert('L'))>0)
        interference=[dict(index=s['index'],**changed_stats(load(s['index']),refs[rows[s['index']]['grain_phase']],unstable)) for s in samples]
        self_max=max(s['fraction'] for s in self_frames);interference_max=max(s['fraction'] for s in interference)
        stable_share=1-float(unstable.mean());keep=stable_share>=0.5 and self_max<=CLASSIFIER['clean_fraction_max'] and interference_max<=CLASSIFIER['clean_fraction_max']
        valid &= abs(float(unstable.mean())-p['unstable_share'])<1e-9 and keep==p['kept'] and self_frames==p['self_frames'] and interference==p['interference_frames']
        bank={};sources={}
        for sample in samples:
            owners=sample['census']['owners'];pending=[o for o in owners if o['actor'] not in bank]
            if not pending:continue
            data=raw(root,rows[sample['index']])
            for owner in pending:
                bank[owner['actor']]=np.packbits(silhouette(data,[owner['tag']]))
                sources[owner['actor']]=dict(index=sample['index'],tag=owner['tag'])
        pose_data[pose]=dict(refs={k:v.astype(np.int16) for k,v in refs.items()},unstable=unstable,keep=keep,bank=bank,sources=sources)
        prepass.append(dict(pose=pose,kept=keep,stable_share=stable_share,unstable_share=1-stable_share,census_actors=len(bank),drawn_actors=len(drawn_names(bank)),self_check='PASS' if self_max<=CLASSIFIER['clean_fraction_max'] else 'FAIL',self_max=self_max,interference='PASS' if interference_max<=CLASSIFIER['clean_fraction_max'] else 'FAIL',interference_max_fraction=interference_max,interference_max_pixels=max(x['changed_stable_pixels'] for x in interference),interference_frames=interference,protocol=protocol_check,result='PASS' if valid and protocol_check['result']=='PASS' else 'FAIL'))
        print('C5 reader: pose %d stable=%.6f self=%.6f interference=%.6f kept=%s'%(pose,stable_share,self_max,interference_max,keep),flush=True)
    chronology=freeze_before_frame(frozen,frozen_files,root/scored_first['image'])
    shape=(rows[0]['height'],rows[0]['width'])
    def actor_ref(pose,name):
        packed=pose_data[pose]['bank'].get(name)
        return None if packed is None else np.unpackbits(packed,count=shape[0]*shape[1]).reshape(shape).astype(bool)
    events=receipt['events'];by_index={};event_out=[];initial=set(drawn_names(pose_data[0]['bank']))
    for e in events:
        first=e['first_index'];end=e.get('end_index',len(rows)-1)
        for index in range(max(0,first-1),min(len(rows),end+2)):
            need(index not in by_index,'Event windows overlap');by_index[index]=e
    scores=[];mismatches=[];ambiguity=[];issues=[];clean_dirty=[];view_bad=[];states={e['event_id']:[] for e in events};previous=None
    negatives={k:{a:[] for a in ROUTES} for k in ('shift','wrong_target')}
    references={}
    for i,row in enumerate(rows):
        if not view_matches(row['camera']):view_bad.append(i)
        data=raw(root,row);mask=(data[:,:,3]&255)>0
        if row['mask'] is None:need(not mask.any() and not (root/('mask-%06d.png'%i)).exists(),'Dirty absent mask')
        else:need(np.array_equal(np.asarray(Image.open(root/row['mask']).convert('L')),data[:,:,3]&255),'PNG mask differs')
        need(np.array_equal(table(data),np.fromfile(root/('reduce-%06d.u32'%i),dtype='<u4').reshape(256,6)),'GPU reduction differs')
        if not row['anomalies'] and mask.any():clean_dirty.append(dict(index=i,pixels=int(mask.sum())))
        if not row['sweep']['score']:
            previous=mask;continue
        pose=row['sweep']['pose'];p=pose_data[pose];stable=~p['unstable'];image=rgb(root,row)
        changed=(np.max(np.abs(image-p['refs'][row['grain_phase']]),axis=2)>1)&stable
        need(row['aa']==0 and row['eye_adaptation_quality']==0,'Scoring AA/exposure differs')
        need(image.max()>0,'Black frame')
        e=by_index.get(i);claimed=bool(row['anomalies'])
        if e is None:
            fraction=float(changed.sum())/max(1,int(stable.sum()))
            if fraction>CLASSIFIER['clean_fraction_max'] or claimed:mismatches.append(dict(index=i,reason='clean hold drift or positive outside event',fraction=fraction))
            previous=mask;continue
        route=e['id'];key=(e['event_id'],pose)
        if key not in references:
            names=e.get('receiver_names',[e['target_name']]);ref=np.zeros(shape,bool);missing=[]
            for name in names:
                silhouette0=actor_ref(pose,name)
                if silhouette0 is None:missing.append(name)
                else:ref |= silhouette0
            references[key]=dict(ref=ref&stable,missing=missing,names=names,stable_share=float((ref&stable).sum())/max(1,int(ref.sum())))
        reference=references[key];region=reference['ref']
        if reference['missing'] or not region.any():
            issues.append(dict(index=i,anomaly=route,reason='clean silhouette missing or empty',missing=reference['missing']))
        if reference['stable_share']<0.5:
            previous=mask;continue
        state,fraction=classify(changed,region) if region.any() else (None,0.)
        states[e['event_id']].append(dict(index=i,state=state,fraction=fraction,pose=pose))
        if state is None:ambiguity.append(dict(index=i,anomaly=route,fraction=fraction))
        elif state!=claimed:mismatches.append(dict(index=i,anomaly=route,pixel_state=state,label=claimed,fraction=fraction))
        if state:
            current=[s for s in row['sampled_states'] if s['event_id']==e['event_id']]
            tags=current[0]['target_tags'] if current else []
            ref=(silhouette(data,tags)&stable) if route=='lod_corruption' else region
            score=metrics(mask&stable,ref,changed,route);score.update(index=i,event=e['event_id'],anomaly=route,pose=pose);scores.append(score)
            shifted=previous&stable if previous is not None else np.zeros(shape,bool)
            if metrics(shifted,ref,changed,route)['result']=='FAIL':negatives['shift'][route].append(i)
            wrong=None
            for name in sorted(p['bank']):
                if name in reference['names']:continue
                candidate=actor_ref(pose,name)&stable
                if candidate.any():wrong=candidate;break
            if wrong is None:
                for name in sorted(reference['names']):
                    candidate=actor_ref(pose,name)
                    if candidate is not None and (candidate&stable).any():wrong=candidate&stable;break
            if wrong is not None and metrics(wrong,ref,changed,route)['result']=='FAIL':negatives['wrong_target'][route].append(i)
        elif previous is not None and previous.any():negatives['shift'][route].append(i)
        previous=mask
        if i%200==0:print('C5 reader: frame %d/%d'%(i,len(rows)),flush=True)
    for e in events:
        values=states[e['event_id']];bits=[v['state'] for v in values]
        complete=bool(bits) and values[0]['index']==e['first_index']-1 and values[-1]['index']>=e.get('end_index',len(rows)) and bits[0] is False and bits[-1] is False and True in bits and None not in bits
        event_out.append(dict(event=e['event_id'],anomaly=e['id'],target=e['target_name'],scored=complete,late_target=e['target_name'] not in initial,pixel_states=values,edge_in_turn=not rows[e['first_index']]['sweep']['score'] or not rows[e.get('end_index',len(rows)-1)]['sweep']['score']))
    per=[]
    for route in ROUTES:
        s=[x for x in scores if x['anomaly']==route];ev=[x for x in event_out if x['anomaly']==route]
        bad=sum(x['result']!='PASS' for x in s);mm=sum(x.get('anomaly')==route for x in mismatches);amb=sum(x['anomaly']==route for x in ambiguity)
        per.append(dict(anomaly=route,events_fired=len(ev),scored_events=sum(x['scored'] for x in ev),turn_edges=sum(x['edge_in_turn'] for x in ev),frames=len(s),failed_frames=bad,c2_mismatches=mm,ambiguous=amb,result='PASS' if sum(x['scored'] for x in ev)>=3 and s and not bad and not mm and not amb else 'FAIL',**{k:distribution([x[k] for x in s]) for k in ('iou','precision','spill','completeness')}))
    negative_results=[dict(kind=k,result='PASS' if all(v[a] for a in ROUTES) else 'FAIL',failed_frames_by_anomaly=v) for k,v in negatives.items()]
    annotation=annotation_check(json.loads((root/'annotation.json').read_text()),rows,events)
    depth=depth_check(json.loads((root/'depth-before.json').read_text()),json.loads((root/'depth-after.json').read_text()),rows)
    census=dict(peak_visible=max(p['drawn_actors'] for p in prepass),legacy_bounds_peak=max(r['census']['visible_count'] for r in rows),basis='First clean held census sample per actor; drawn pixels before coverage admission. First scored hold has the same fixed pose 0 player view.',initial_visible=sorted(initial),exhaustion=max(r['census']['ledger_exhausted'] for r in rows),late_scored_targets=sorted({e['target'] for e in event_out if e['scored'] and e['late_target']}))
    census['result']='PASS' if census['peak_visible']>55 and not census['exhaustion'] and census['late_scored_targets'] else 'FAIL'
    initial_pose=next(p for p in frozen['poses'] if p['pose']==0)
    census=enforce_initial_view(census,initial_view_check(scored_first['camera'],rows[initial_pose['off_samples'][0]['index']]['camera']))
    good=chronology and all(p['result']=='PASS' and p['interference']=='PASS' for p in prepass) and all(p['result']=='PASS' for p in per) and not mismatches and not ambiguity and not issues and not clean_dirty and not view_bad and all(n['result']=='PASS' for n in negative_results) and all(x['result']=='PASS' for x in (annotation,depth,census))
    return dict(result='PASS' if good else 'FAIL',reference_rule=REFERENCE_RULE,frames=len(rows),prepass=prepass,references_frozen_before_scoring=chronology,per_anomaly=per,per_frame=scores,events=event_out,mask_negatives=negative_results,c2=c2_result(rows,event_out,mismatches,ambiguity),reference_issues=issues,clean_mask_pixels=sum(x['pixels'] for x in clean_dirty),dirty_clean_frames=clean_dirty,view=dict(result='PASS' if not view_bad else 'FAIL',frames=len(rows),bad_frames=view_bad),annotation=annotation,custom_depth=depth,census=census,rhi=capture['rhi'],format=capture['backbuffer_format'],dropped=capture['dropped'])

if __name__=='__main__':
    import sys
    result=read(Path(sys.argv[1]));Path(sys.argv[2]).write_text(json.dumps(result,indent=2));print(json.dumps({k:result[k] for k in ('result','frames','per_anomaly','census','view','annotation')}))
