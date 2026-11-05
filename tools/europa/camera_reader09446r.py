import datetime,hashlib,json
from pathlib import Path
import numpy as np
from PIL import Image
from capture_c5_reader import ROOT_KEYS,EVENT_KEYS,frame_set

def utc():return datetime.datetime.now(datetime.timezone.utc).isoformat()
def load(p):return json.loads(Path(p).read_text())
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rows(root):return [json.loads(s) for s in (Path(root)/'labels.jsonl').read_text().splitlines()]
def spans(indices):
    out=[]
    for i in indices:
        if out and out[-1][1]==i:out[-1][1]=i+1
        else:out.append([i,i+1])
    return out

def state(root):
    root=Path(root);data=rows(root);summary=load(root/'run_summary.json');annotation=load(root/'annotation.json');receipt=load(root/'labels-receipt.json')
    reason=sum(e['transition_reasons'].get('camera_clipping_unconfirmed',0) for e in annotation['anomalies'])
    assert summary['label_transition_camera_clipping_unconfirmed_entries']==reason,'GN4 entry counter differs'
    assert summary['session_global_near_restored'] and summary['session_global_baseline_near']==summary['session_global_restored_near']==10
    active='camera_clipping' in receipt['routes']
    if not active:
        assert len(data)==2352 and not any('camera_clipping.slab' in r for r in data)
        assert not any(s['id']=='camera_clipping' for r in data for s in r['sampled_states'])
        return dict(result='PASS',requested=False,gn4_entries=reason,summary=summary)
    start=load(root/'camera-session-start.json');baseline=start['held_near']==10
    assert start['held'] and start['bench'] and start['held_near'] in (10,100) and start['baseline_near']==10
    far_only=start.get('far_only',False)
    expected_count=2352 if far_only else 2640
    assert start['near_start']==2352 and start['pose_hold']==48 and len(data)==expected_count
    if far_only:
        assert start['held_near']==100
        assert all(r['camera']['view_location']==data[0]['camera']['view_location'] and r['camera']['view_rotation']==data[0]['camera']['view_rotation'] and r['camera']['view_fov']==data[0]['camera']['view_fov'] for r in data)
    positive=[r['frame_index'] for r in data if r['camera_clipping.slab']]
    expected=[] if baseline or far_only else [i for i in range(2352,2640) if ((i-2352)//48)%2==0]
    assert positive==expected,dict(actual=spans(positive),expected=spans(expected))
    for row in data:
        labelled=row['camera_clipping.slab'];states=[s for s in row['sampled_states'] if s['id']=='camera_clipping']
        assert row['camera']['near']==start['held_near']
        assert len(states)==int(labelled)
        assert row['anomaly_present']==any(s['affected'] for s in row['sampled_states'])
        for s in states:
            assert s['whole_frame'] and s['affected'] and s['injected'] and s['target_name']=='' and s['target_tags']==[] and s['projected_screen_share']==1 and s['mask_pixels']==0
        if row['frame_index']>=2352:
            assert not any(s['id']!='camera_clipping' for s in row['sampled_states'])
            assert not row['mask'] and not (root/('mask-%06d.png'%row['frame_index'])).exists()
    events=[e for e in annotation['anomalies'] if e['anomaly_type']=='camera_clipping']
    assert set(annotation)==ROOT_KEYS and len(events)==(0 if baseline or far_only else 1)
    assert len([e for e in receipt['events'] if e['id']=='camera_clipping'])==len(events)
    for e in events:
        assert set(e)==EVENT_KEYS and e['affected_frames']==e['injected_frames']==frame_set(positive)
        assert e['coverage_ratio']==1 and not e['mask']['provided'] and e['affected_objects']['count']==0
        assert e['anomaly_subtype']=='camera_clipping' and e['event_id'].startswith('camera_clipping@') and e['event_id'].endswith('|')
    assert summary['camera_clipping_frames_evaluated']==expected_count and summary['camera_clipping_slab_positive_frames']==len(positive)
    return dict(result='PASS',requested=True,baseline=baseline,far_only=far_only,frames=len(data),positive_frames=len(positive),spans=spans(positive),start=start,summary=summary,gn4_entries=reason)

def histogram(a,b):
    delta=np.max(np.abs(a.astype(np.int16)-b.astype(np.int16)),axis=2)
    return np.bincount(delta.ravel(),minlength=256).astype(np.int64)

def measured(pair):
    maximum=max(max(i for i,n in enumerate(h) if n) for h in pair)
    count=max(int(h[maximum]) for h in pair)
    return dict(max_delta=maximum,max_count_at_max_delta=count)

def within(h,floor):
    level=floor['max_delta']
    return not np.any(h[level+1:]) and int(h[level])<=floor['max_count_at_max_delta']

def compare_frames(a,b,count=None):
    a,b=Path(a),Path(b);aa,bb=rows(a),rows(b)
    if count is not None:
        assert len(aa)>=count and len(bb)>=count
        aa,bb=aa[:count],bb[:count]
    assert len(aa)==len(bb)
    for i,(x,y) in enumerate(zip(aa,bb)):
        assert x['frame_index']==y['frame_index']==i
        assert all(x['camera'][k]==y['camera'][k] for k in ('view_location','view_rotation','view_fov'))
        object_states=lambda r:sorted((v['id'],v['target_name'],v['affected']) for v in r['sampled_states'] if v['id']!='camera_clipping')
        assert object_states(x)==object_states(y),'Object events differ at frame '+str(i)
        lhs=np.asarray(Image.open(a/x['image']).convert('RGB'));rhs=np.asarray(Image.open(b/y['image']).convert('RGB'))
        assert lhs.shape==rhs.shape
        yield i,x,histogram(lhs,rhs)

def floor(a,b,out,provenance,near_plane=False):
    out=Path(out);assert not out.exists()
    sa,sb=state(a),state(b)
    if near_plane:
        assert sa['far_only'] and sa['positive_frames']==0 and sb['baseline'] and sb['positive_frames']==0
        count=2352
    else:
        assert sa['baseline'] and sb['baseline']
        count=None
    values=[];bins=[]
    for i,row,h in compare_frames(a,b,count):
        bins.append(h);values.append(dict(index=i,max_delta=int(np.flatnonzero(h)[-1]),histogram=h.tolist(),above_each_level=(np.cumsum(h[::-1])[::-1]-h).tolist()))
        if i%400==0:print('Control floor %d'%i,flush=True)
    limits=measured(bins)
    for v,h in zip(values,bins):v['count_at_floor_maximum']=int(h[limits['max_delta']])
    value=dict(result='PASS',sealed_utc=utc(),kind='near_plane' if near_plane else 'session',rule='GO2/GN2 full-frame per-pixel maximum absolute RGB delta; maximum over controls, then maximum pixel count at that level over all control frames',thresholds=limits,control_a=str(a),control_b=str(b),control_labels_sha256=[sha(Path(p)/'labels.jsonl') for p in (a,b)],reader_sha256=sha(__file__),frames=len(values),provenance=provenance,per_frame=values)
    out.write_text(json.dumps(value,indent=2));return value

def larger(floors):
    assert floors and all(f['result']=='PASS' for f in floors)
    return dict(max((f['thresholds'] for f in floors),key=lambda f:(f['max_delta'],f['max_count_at_max_delta'])))

def paired(positive,control,floor_path,allow_existing=False):
    sealed=load(floor_path);limits=sealed['thresholds'];positive=Path(positive)
    assert sealed['result']=='PASS' and limits==larger([load(f['path']) for f in sealed['floors']])
    now=utc();assert sealed['sealed_utc']<now
    if allow_existing:
        assert str(positive).replace('\\','/').endswith('09446-evidence/D3D11-1280-positive/01/plugin-frames/epoch01')
    else:assert datetime.datetime.fromisoformat(sealed['sealed_utc']).timestamp()<positive.stat().st_ctime,'Floors must precede new positive session'
    for item in sealed['floors']:assert sha(item['path'])==item['sha256']
    assert sha(Path(control)/'labels.jsonl')==sealed['control_labels_sha256']
    session=load(sealed['floors'][0]['path']);provenance=session['provenance']
    receipt=load(positive/'labels-receipt.json');capture=load(positive/'capture.json')
    assert receipt['seed']==provenance['seed'] and receipt['routes']==provenance['routes'] and capture['rhi']==provenance['spec'][0]
    assert load(positive.parents[1]/'settings.json')==provenance['settings']
    state(positive);state(control)
    values=[];bad=[];signals=[]
    individual=[load(v['path'])['thresholds'] for v in sealed['floors']]
    for i,row,h in compare_frames(positive,control):
        label=bool(row['camera_clipping.slab']);above=int(h[limits['max_delta']+1:].sum())
        above_each=[int(h[f['max_delta']+1:].sum()) for f in individual]
        signal=all(n>max(float(h.sum())*.01,f['max_count_at_max_delta'] if f['max_delta'] else 0) for n,f in zip(above_each,individual))
        passed=signal if label else within(h,limits)
        if not passed:bad.append(i)
        signals.append(signal)
        values.append(dict(index=i,labelled=label,max_delta=int(np.flatnonzero(h)[-1]),pixels_at_floor_max=int(h[limits['max_delta']]),pixels_above_floor=above,pixels_above_each_floor=above_each,within_floor=within(h,limits),passed=bool(passed),histogram=h.tolist()))
        if i%400==0:print('GO2 positive comparison %d'%i,flush=True)
    labels=[v['labelled'] for v in values];shifted=[i for i,(l,signal) in enumerate(zip(labels[1:]+[False],signals)) if l!=signal]
    return dict(result='PASS' if not bad and sum(labels)>0 and shifted else 'FAIL',floor=str(floor_path),floor_sha256=sha(floor_path),thresholds=limits,comparison_utc=now,reused_positive=allow_existing,frames=len(values),positive_frames=sum(labels),positive_above_floor_min=min(v['pixels_above_floor'] for v in values if v['labelled']),negative_max_delta=max(v['max_delta'] for v in values if not v['labelled']),negative_count_at_floor_max=max(v['pixels_at_floor_max'] for v in values if not v['labelled']),mismatches=bad,early_label_canfail=shifted,per_frame=values)

def selftest():
    a=np.zeros((8,8,3),dtype=np.uint8);b=a.copy();b[0,0,0]=3;b[1,1,1]=2
    h=histogram(a,b);f=measured([h]);assert f==dict(max_delta=3,max_count_at_max_delta=1) and within(h,f)
    bigger=b.copy();bigger[0,0,0]=4;assert not within(histogram(a,bigger),f)
    more=b.copy();more[2,2,2]=3;assert not within(histogram(a,more),f)
    zero=histogram(a,a);assert within(zero,measured([zero])) and not within(h,measured([zero]))
    return dict(result='PASS',positive=2,maximum_delta_canfail=True,maximum_count_canfail=True,zero_floor_canfail=True)
