from __future__ import print_function
import copy
import json
import os

ROUTES=('blinking','lighting_mismatch','lod_corruption','missing_object')
ROIS={'blinking':(805,310,1165,670),'missing_object':(115,310,475,670),
      'lod_corruption':(450,170,830,590),'lighting_mismatch':(80,250,1200,700)}
SEPARATION_RATIO=5.0
MARGIN_FLOOR=0.5

def need(value,message):
    if not value:raise ValueError(message)

def claimed(row,route):
    return any(a['id']==route for a in row['anomalies'])

def expected_frames(profile):
    need(profile in ('C2','C3'),'Unknown label profile')
    return 480 if profile=='C2' else 240

def evaluate(rows,observed,commands,annotation,strict=True,profile='C2'):
    count=expected_frames(profile)
    need(len(rows)==len(observed)==count,'Expected '+str(count)+' primary frames')
    need([r['frame_index'] for r in rows]==list(range(count)),'Missing, duplicate or out-of-order index')
    keys=[(r['epoch'],r['world_owner'],r['request'],r['session_index']) for r in rows]
    need(len(set(keys))==count and len(set(k[:2] for k in keys))==1,'Stale or duplicate frame key')
    need([r['request'] for r in rows]==list(range(1,count+1)),'Request sequence differs')
    need(all(r['session_index']==r['frame_index'] and r['aa']==0 and r['tickpin_compiled']==0 for r in rows),'Frame metadata differs')
    need(all(b['sample_frame']>a['sample_frame'] and b['family_frame']>a['family_frame'] for a,b in zip(rows,rows[1:])),'Stale or reordered render family')
    events=[]
    mismatches=[]
    for i,row in enumerate(rows):
        expected_route=ROUTES[(i//60)%4]
        need(row['event_id']==i//20 and row['event_anomaly']==expected_route,'Event identity differs')
        need(row['tick_order']==i//240 and row['pace']==(1 if i<240 else 0),'Effective tick order or Pace 0 block missing')
        need(row['fixed_step']==(i<240),'Effective pacing state differs')
        need(len(row['sampled_states'])==4,'Incomplete sampled state')
        need(row['anomaly_present']==bool(row['anomalies']),'Summary label differs')
        for route in ROUTES:
            state=[s for s in row['sampled_states'] if s['id']==route]
            need(len(state)==1 and state[0]['labelled']==claimed(row,route),'Exported label disagrees with sampled policy')
            pixel=observed[i] if route==expected_route else False
            if claimed(row,route)!=pixel:mismatches.append(dict(index=i,anomaly=route,label=claimed(row,route),pixel=pixel))
    for event in range(count//20):
        start=event*20;route=ROUTES[(event//3)%4];bits=observed[start:start+20]
        need(not any(bits[:3]) and not any(bits[17:]),'No clean apply/revert flanks for event '+str(event))
        edges=[start+i for i in range(1,20) if bits[i]!=bits[i-1]]
        need(len(edges)>=4 if route=='blinking' else len(edges)==2,'Missing visible edges for event '+str(event))
        need(bits[16] and edges[-1]==start+17,'Revert edge was not observed for event '+str(event))
        apply=[c for c in commands if c['command']=='apply' and c['event_id']==event and c['success']]
        revert=[c for c in commands if c['command']=='revert' and c['event_id']==event and c['success']]
        need(len(apply)==len(revert)==1 and apply[0]['next_index']==start+3 and revert[0]['next_index']==start+17,'Apply/revert command evidence differs')
        actual=set(start+i for i,v in enumerate(bits) if v)
        entry=[e for e in annotation['anomalies'] if e['bench_event']==event]
        need(len(entry)==1 and set(entry[0]['affected_frames']['frame_indices'])==set(i for i in range(start,start+20) if claimed(rows[i],route)), 'Annotation differs from exported labels')
        events.append(dict(event=event,anomaly=route,order=event//12,apply_edges=1,revert_edges=1,
            blink_toggle_edges=len(edges)-2 if route=='blinking' else 0,edge_indices=edges,affected_frames=sorted(actual)))
    if strict:need(not mismatches,'Pixel/label mismatch: '+json.dumps(mismatches[:12]))
    totals=[]
    for order in range(count//240):
        for route in ROUTES:
            subset=[e for e in events if e['order']==order and e['anomaly']==route]
            totals.append(dict(anomaly=route,order=order,events=len(subset),apply_edges=sum(e['apply_edges'] for e in subset),
                revert_edges=sum(e['revert_edges'] for e in subset),blink_toggle_edges=sum(e['blink_toggle_edges'] for e in subset),
                mismatches=sum(m['anomaly']==route and m['index']//240==order for m in mismatches)))
    return dict(result='FAIL' if mismatches else 'PASS',frames=count,profile=profile,mismatches=len(mismatches),mismatch_rows=mismatches,ambiguous=0,totals=totals,events=events,
        offset='ALIGNED',separation_ratio=SEPARATION_RATIO,transition_exclusions=0)

def mutate(rows,observed,kind):
    changed=copy.deepcopy(rows);indices=[]
    for event in range(len(rows)//20):
        start=event*20;route=ROUTES[(event//3)%4]
        edges=[start+i for i in range(1,20) if observed[start+i]!=observed[start+i-1]]
        selected=edges[:1] if kind=='apply' else edges[-1:] if kind=='revert' else edges[1:-1] if route=='blinking' else []
        for index in selected:
            prev=claimed(rows[index-1],route)
            row=changed[index]
            row['anomalies']=[a for a in row['anomalies'] if a['id']!=route]
            if prev:row['anomalies'].append(dict(id=route))
            for state in row['sampled_states']:
                if state['id']==route:state['labelled']=prev
            row['anomaly_present']=bool(row['anomalies']);indices.append(index)
    need(indices,'Negative has no relevant edge')
    return changed,indices

def annotation_for(rows):
    return dict(anomalies=[dict(bench_event=e,affected_frames=dict(frame_indices=[i for i in range(e*20,e*20+20) if claimed(rows[i],ROUTES[(e//3)%4])])) for e in range(len(rows)//20)])

def pixel_states(root,rows,references):
    from PIL import Image,ImageChops,ImageStat
    def distance(a,b):return sum(ImageStat.Stat(ImageChops.difference(a,b)).mean)/3.0
    refs={}
    for route in ROUTES:
        clean=Image.open(references[route]['clean']).convert('RGB').crop(ROIS[route])
        active=Image.open(references[route]['active']).convert('RGB').crop(ROIS[route])
        separation=distance(clean,active)
        need(separation>2.5,'References insufficiently separated: '+route)
        refs[route]=(clean,active,separation)
    observed=[];measurements=[]
    for row in rows:
        route=row['event_anomaly'];path=os.path.join(root,row['image'])
        image=Image.open(path).convert('RGB')
        need(image.size==(1280,720),'Non-native image extent')
        clean,active,separation=refs[route];crop=image.crop(ROIS[route])
        dc,da=distance(crop,clean),distance(crop,active)
        need(abs(dc-da)>MARGIN_FLOOR and min(dc,da)*SEPARATION_RATIO<separation,'Ambiguous pixel edge/frame '+str(row['frame_index']))
        observed.append(da<dc)
        measurements.append(dict(index=row['frame_index'],anomaly=route,clean_error=dc,active_error=da,separation=separation,observed=da<dc))
    return observed,measurements

def read(root,references,negative_output=None,profile='C2'):
    def load(name):
        with open(os.path.join(root,name)) as stream:return json.load(stream)
    with open(os.path.join(root,'labels.jsonl')) as stream:rows=[json.loads(line) for line in stream if line.strip()]
    observed,measurements=pixel_states(root,rows,references)
    native=load('labels-receipt.json');annotation=load('annotation.json');capture=load('capture.json')
    count=expected_frames(profile)
    need(annotation['label_schema']==2 and annotation['label_schema_minor']==1 and annotation['video']['total_frames']==count,'Annotation schema differs')
    need(native['result']=='PASS' and native['paired']==native['written']==count and native['unmatched_dropped']==native['dropped']==0,'Native pairing/stop failure')
    need(native['pacing_restored'],'Pacing was not restored')
    need(capture['written']==capture['requested']==count and capture['dropped']==0 and capture['backbuffer_format'] in (2,18,37),'Capture accounting differs')
    need([r['index'] for r in sorted(capture['frames'],key=lambda r:r['index'])]==list(range(count)),'PNG index differs')
    result=evaluate(rows,observed,native['commands'],annotation,strict=False,profile=profile)
    result.update(rhi=capture['rhi'],backbuffer_format=capture['backbuffer_format'],dropped=capture['dropped'],pixel_measurements=measurements)
    negatives=[]
    for kind in ('apply','revert','blink_toggle'):
        mutant,indices=mutate(rows,observed,kind)
        try:evaluate(mutant,observed,native['commands'],annotation_for(mutant),profile=profile)
        except ValueError as error:
            need('Pixel/label mismatch' in str(error),'Negative failed for wrong reason: '+str(error))
            negatives.append(dict(kind=kind,result='PASS',shifted_indices=indices,refusal=str(error)))
        else:raise ValueError('One-frame shift escaped: '+kind)
        if negative_output:
            with open(os.path.join(negative_output,kind+'-labels.jsonl'),'w') as stream:
                stream.write(''.join(json.dumps(row)+'\n' for row in mutant))
    result['negatives']=negatives
    return result
