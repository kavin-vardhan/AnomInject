import json
from pathlib import Path
import zlib

import numpy as np
from PIL import Image

ROUTES=('blinking','lighting_mismatch','lod_corruption','missing_object')
TAGS={'blinking':128,'lighting_mismatch':255,'lod_corruption':129,'missing_object':130}
DEPTH_BIAS=1e-6
CHANGE_LEVEL=1
THRESHOLDS=dict(iou=0.90,spill=0.01,completeness=0.95,precision=dict(blinking=0.90,missing_object=0.90,lod_corruption=0.80,lighting_mismatch=0.80))

def need(ok,reason):
    if not ok:raise ValueError(reason)

def dilate(a):
    p=np.pad(a,1)
    return np.logical_or.reduce([p[y:y+a.shape[0],x:x+a.shape[1]] for y in range(3) for x in range(3)])

def silhouette(data,tag):
    stencil=data[:,:,0]
    custom=data[:,:,1].copy().view('<f4')
    scene=data[:,:,2].copy().view('<f4')
    selected=(stencil>=128)&(stencil<=131) if tag==255 else stencil==tag
    return selected & (custom>0) & (custom>=scene-DEPTH_BIAS)

def metrics(mask,reference,changed,anomaly='blinking'):
    union=np.count_nonzero(mask|reference)
    inside=np.count_nonzero(mask&reference)
    n=int(np.count_nonzero(mask));r=int(np.count_nonzero(reference))
    result=dict(mask_pixels=n,reference_pixels=r,iou=inside/union if union else 0.,
                spill=float(np.count_nonzero(mask&~dilate(reference)))/n if n else 1.)
    ok=n>0 and r>0 and result['spill']<=THRESHOLDS['spill']
    if anomaly!='lighting_mismatch':ok &= result['iou']>=THRESHOLDS['iou']
    result['completeness']=float(inside)/r if r else 0.
    if anomaly=='lighting_mismatch':ok &= result['completeness']>=THRESHOLDS['completeness']
    count=int(np.count_nonzero(changed&mask))
    result.update(changed_mask_pixels=count,precision=float(count)/n if n else 0.)
    ok &= result['precision']>=THRESHOLDS['precision'][anomaly]
    result['result']='PASS' if ok else 'FAIL'
    return result

def load_data(root,index):
    raw=zlib.decompress((root/('evidence-%06d.u32x4.zlib'%index)).read_bytes())
    need(len(raw)==1280*720*16,'Mask evidence extent differs')
    return np.frombuffer(raw,dtype='<u4').reshape(720,1280,4)

def reduction(data):
    packed=data[:,:,3];m=packed&255;d=(packed>>8)&255
    result=np.zeros((256,6),dtype='<u4')
    for tag in range(128,132):
        yy,xx=np.nonzero(m==tag)
        if len(xx):result[tag,:5]=[len(xx),(~int(xx.min()))&0xffffffff,(~int(yy.min()))&0xffffffff,int(xx.max()),int(yy.max())]
        result[tag,5]=np.count_nonzero(d==tag)
    return result

def distribution(values):
    return dict(min=float(min(values)),median=float(np.median(values)),p95=float(np.percentile(values,95)),max=float(max(values))) if values else None

def read(root,references,mutant=None):
    from capture_c2_reader import pixel_states,claimed
    root=Path(root)
    rows=[json.loads(line) for line in (root/'labels.jsonl').read_text().splitlines() if line]
    need(len(rows)==240 and [r['frame_index'] for r in rows]==list(range(240)),'Expected 240 ordered RGB/label/mask frames')
    need(len({(r['epoch'],r['world_owner'],r['request']) for r in rows})==240,'Duplicate mask family key')
    need(all(b['sample_frame']>a['sample_frame'] and b['family_frame']>a['family_frame'] for a,b in zip(rows,rows[1:])),'Stale mask family')
    native=json.loads((root/'labels-receipt.json').read_text());capture=json.loads((root/'capture.json').read_text())
    need(native['result']=='PASS' and native['mask_state_restored'] and native['pacing_restored'],'Native mask/restore failed')
    need(capture['written']==capture['requested']==native['paired']==240 and native['unmatched_dropped']==capture['dropped']==0,'Dropped or unmatched mask')
    observed,pixel_measurements=pixel_states(str(root),rows,references)
    for row,bit in zip(rows,observed):need(claimed(row,row['event_anomaly'])==bit,'C3 RGB/label mismatch at '+str(row['frame_index']))
    results=[];noise=[];events=[];edge_failures=[];clean_frames=[]
    for event in range(12):
        start=event*20;route=ROUTES[event//3];tag=TAGS[route]
        clean=load_data(root,start+2)
        clean_rgb=np.asarray(Image.open(root/rows[start+2]['image']).convert('RGB'),dtype=np.int16)
        other=np.asarray(Image.open(root/rows[start+1]['image']).convert('RGB'),dtype=np.int16)
        clean_noise=np.max(np.abs(other-clean_rgb),axis=2)>CHANGE_LEVEL
        noise.append(dict(event=event,pixels_over_level=int(clean_noise.sum()),fraction=float(clean_noise.mean())))
        clean_ref=silhouette(clean,tag)
        need(clean_ref.any(),'No clean stencil/custom-depth reference for '+route)
        commands=native['commands']
        need(sum(c['command']=='apply' and c['event_id']==event and c['success'] for c in commands)==1,'Missing successful apply')
        need(sum(c['command']=='revert' and c['event_id']==event and c['success'] for c in commands)==1,'Missing successful revert')
        need(not any(observed[start:start+3]) and not any(observed[start+17:start+20]) and any(observed[start+3:start+17]),'Event lacks visible apply/revert flanks')
        for step in range(20):
            index=start+step;row=rows[index]
            need(row['request']==index+1 and row['aa']==0 and row['eye_adaptation_quality']==0,'Mask/AA/exposure association differs')
            data=load_data(root,index)
            path=root/('mask-%06d.png'%index)
            if row['mask'] is None:
                need(row['mask_state']=='empty' and not path.exists(),'Empty mask has a PNG')
                actual=np.zeros((720,1280),dtype=np.uint8)
            else:
                need(row['mask']==path.name and row['mask_state']=='present','Mask filename/state differs')
                actual=np.asarray(Image.open(path).convert('L'))
                need(actual.any(),'m44 refuses an all-zero PNG')
            need(np.array_equal(actual,data[:,:,3]&255),'Written mask differs from GPU mask')
            need(np.array_equal(np.fromfile(root/('reduce-%06d.u32'%index),dtype='<u4').reshape(256,6),reduction(data)),'GPU reduction differs from independently recomputed table')
            active=bool(observed[index])
            chosen=data
            if mutant=='shift' and index>0:chosen=load_data(root,index-1)
            mask=(chosen[:,:,3]&255)>0
            if mutant=='wrong_target':mask=silhouette(data,130 if tag!=130 else 128) if active else np.zeros((720,1280),bool)
            if not active:
                clean_frames.append(dict(index=index,anomaly=route,mask_pixels=int(mask.sum())))
                if mask.any():edge_failures.append(dict(index=index,anomaly=route,reason='mask remained on labelled-clean frame'))
                continue
            ref=silhouette(data,tag) if route=='lod_corruption' else clean_ref
            rgb=np.asarray(Image.open(root/row['image']).convert('RGB'),dtype=np.int16)
            changed=np.max(np.abs(rgb-clean_rgb),axis=2)>CHANGE_LEVEL
            result=metrics(mask,ref,changed,anomaly=route)
            custom=data[:,:,1].copy().view('<f4');scene=data[:,:,2].copy().view('<f4')
            drawn=silhouette(data,tag)&(np.abs(custom-scene)<=DEPTH_BIAS)
            hidden=route in ('blinking','missing_object') and observed[index]
            result.update(index=index,event=event,anomaly=route,hidden=hidden,drawn_pixels=int(drawn.sum()))
            if hidden and drawn.any():result['result']='FAIL';result['drawn_failure']='Hidden target still in scene depth'
            results.append(result)
        events.append(dict(event=event,anomaly=route,apply=1,revert=1))
    per=[]
    for route in ROUTES:
        subset=[r for r in results if r['anomaly']==route]
        dirty=[r for r in clean_frames if r['anomaly']==route and r['mask_pixels']]
        failed=sum(r['result']!='PASS' for r in subset)
        per.append(dict(anomaly=route,events=3,frames=len(subset),failed_frames=failed,clean_frames=sum(r['anomaly']==route for r in clean_frames),dirty_clean_frames=len(dirty),result='FAIL' if failed or dirty else 'PASS',
                        iou=distribution([r['iou'] for r in subset]),spill=distribution([r['spill'] for r in subset]),
                        precision=distribution([r['precision'] for r in subset])))
    good=not edge_failures and all(r['result']=='PASS' for r in results) and not any(r['pixels_over_level'] for r in noise)
    return dict(result='PASS' if good else 'FAIL',frames=240,thresholds=THRESHOLDS,change_level=CHANGE_LEVEL,depth_bias=DEPTH_BIAS,
                per_anomaly=per,per_frame=results,edge_failures=edge_failures,clean_noise=noise,mutant=mutant,clean_frames=clean_frames,
                clean_mask_pixels=sum(r['mask_pixels'] for r in clean_frames),
                rhi=capture['rhi'],format=capture['backbuffer_format'],dropped=capture['dropped'],events=events,
                pixel_measurements=pixel_measurements,stencil_restored=True,gpu_reduction='240 tables match')

def tests():
    a=np.zeros((32,48),bool);a[8:24,8:24]=True
    wrong=np.zeros_like(a);wrong[8:24,28:44]=True
    positive=metrics(a,a,a)
    need(positive['result']=='PASS','Positive metric test failed')
    shifted=[np.zeros_like(a),np.zeros_like(a),a,a]
    refs=[np.zeros_like(a),a,a,np.zeros_like(a)]
    failed=[i for i,(m,r) in enumerate(zip(shifted,refs)) if (r.any() and metrics(m,r,r)['result']=='FAIL') or (not r.any() and m.any())]
    need(failed==[1,3],'One-frame shift was not independently refused')
    bad=metrics(wrong,a,a)
    need(bad['result']=='FAIL' and bad['iou']==0 and bad['precision']==0,'Wrong target was not refused')
    precision_negatives=[]
    for route in ROUTES:
        reference=a|wrong if route=='lighting_mismatch' else wrong
        value=metrics(wrong,reference,a,route)
        need(value['spill']==0 and value['result']=='FAIL' and value['precision']==0,'Wrong target escaped independent precision: '+route)
        precision_negatives.append(dict(anomaly=route,metrics=value))
    clean=np.full((32,48,3),100,dtype=np.int16)
    grain=clean.copy();grain[a]+=1
    changed=np.max(np.abs(grain-clean),axis=2)>CHANGE_LEVEL
    need(not changed.any(),'One-level grain counts as visible change')
    grain[a]+=1
    need(np.array_equal(np.max(np.abs(grain-clean),axis=2)>CHANGE_LEVEL,a),'Two-level change was discarded')
    d=np.zeros((32,48,4),dtype='<u4');d[:,:,0]=a.astype('<u4')*128
    d[:,:,1]=np.full((32,48),0.5,dtype='<f4').view('<u4');d[:,:,2]=d[:,:,1]
    need(np.array_equal(silhouette(d,128),a),'Raw stencil/depth decoding failed')
    return dict(result='PASS',positive=positive,negatives=[dict(kind='one-frame mask shift',failed_indices=failed),dict(kind='wrong target',metrics=bad)],precision_negatives=precision_negatives,grain_boundary='PASS',raw_depth='PASS')

if __name__=='__main__':
    import argparse
    p=argparse.ArgumentParser();p.add_argument('action',choices=['test','read']);p.add_argument('--root');p.add_argument('--refs');p.add_argument('--output',required=True)
    a=p.parse_args()
    result=tests() if a.action=='test' else read(a.root,json.loads(Path(a.refs).read_text())['references'])
    Path(a.output).write_text(json.dumps(result,indent=2))
    print(json.dumps({k:v for k,v in result.items() if k not in ('per_frame','pixel_measurements')}))
