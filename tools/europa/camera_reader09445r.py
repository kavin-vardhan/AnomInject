import json,re,sys
from pathlib import Path
import numpy as np
from PIL import Image
sys.path.insert(0,'D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa')
from capture_c4_reader import raw
from capture_phaseb_h4_reader import self_checks,ui_pixels,depth_check
from capture_c5_reader import view_matches,census_protocol,ROOT_KEYS,EVENT_KEYS

def load(path):return json.loads(Path(path).read_text())
def spans(indices):
 out=[]
 for i in indices:
  if out and out[-1][1]==i:out[-1][1]=i+1
  else:out.append([i,i+1])
 return out

def agree(labelled,changed):
 return [i for i,(label,pixels) in enumerate(zip(labelled,changed)) if bool(label)!=(pixels>0)]

def selftest():
 labels=[0,1,1,0,1,1,0,1,1,0];pixels=[0,4,7,0,5,9,0,2,3,0]
 assert not agree(labels,pixels) and len(spans([i for i,v in enumerate(labels) if v]))==3
 assert agree(labels[1:]+[0],pixels)
 assert agree(labels,[1]+pixels[1:])
 assert agree(labels,[0]*len(labels))
 return dict(result='PASS',positive=1,early_label_canfail=True,unlabelled_difference_canfail=True,unchanged_positive_canfail=True)

def read(root,log,rhi,baseline):
 root=Path(root);rows=[json.loads(s) for s in (root/'labels.jsonl').read_text().splitlines()]
 receipt=load(root/'labels-receipt.json');capture=load(root/'capture.json');start=load(root/'camera-session-start.json');summary=load(root/'run_summary.json')
 assert len(rows)==capture['written']==capture['requested']==receipt['paired']==2352
 assert capture['dropped']==receipt['unmatched_dropped']==0 and receipt['result']=='PASS'
 assert receipt['mask_state_restored'] and receipt['pacing_restored'] and receipt['routes']==['camera_clipping']
 assert capture['rhi']==rhi and all((r['width'],r['height'])==(1280,720) for r in rows)
 assert [r['frame_index'] for r in rows]==list(range(len(rows)))
 assert start['held'] and start['bench'] and start['baseline_near']==10 and start['held_near']==(10 if baseline else 100)
 assert summary['session_global_near_restored'] and summary['session_global_restored_near']==start['baseline_near']
 labelled=[];bad=[];mask_pixels=0;unconfirmed=[]
 for i,row in enumerate(rows):
  value=bool(row['camera_clipping.slab']);labelled.append(value)
  if row.get('camera_clipping.unconfirmed'):unconfirmed.append(i)
  if row['anomaly_present']!=value or not view_matches(row['camera']):bad.append(i)
  if row['camera']['near']!=start['held_near']:bad.append(i)
  if value:
   if len(row['anomalies'])!=1:bad.append(i)
   for s in row['anomalies']:
    if not (s['id']=='camera_clipping' and s['whole_frame'] and s['affected'] and s['target_name']=='' and s['target_tags']==[] and s['projected_screen_share']==1 and s['mask_pixels']==0):bad.append(i)
  data=raw(root,row);count=int(np.count_nonzero(data[:,:,3]&255));mask_pixels+=count
  if count or row.get('mask') or (root/('mask-%06d.png'%i)).exists():bad.append(i)
  if i%400==0:print('Camera state reader %d/%d'%(i,len(rows)),flush=True)
 positive=[i for i,v in enumerate(labelled) if v]
 annotation=load(root/'annotation.json');events=annotation['anomalies']
 assert set(annotation)==ROOT_KEYS
 if baseline:
  assert not positive and not events and not receipt['events'],'Baseline near must never label'
 else:
  assert len(spans(positive))>=3 and len(events)==len(receipt['events'])==1
  event=events[0];assert set(event)==EVENT_KEYS
  assert event['anomaly_type']==event['anomaly_subtype']=='camera_clipping'
  assert event['event_id'].startswith('camera_clipping@') and event['event_id'].endswith('|')
  assert event['affected_frames']['frame_indices']==positive and event['injected_frames']['frame_indices']==positive
  assert event['coverage_ratio']==1 and not event['mask']['provided']
  assert event['affected_objects']['count']==0
 assert summary['camera_clipping_frames_evaluated']==len(rows) and summary['camera_clipping_slab_positive_frames']==len(positive)
 frozen=load(root/'prepass-frozen.json');protocol=[]
 for pose in frozen['poses']:protocol.append(census_protocol(rows,pose['off_samples'],pose['silhouette_samples']))
 census={k:max(r['census'][v] for r in rows) for k,v in [('exhaustion','ledger_exhausted'),('lost','batches_lost'),('no_pass','no_pass_batches'),('polluted','polluted_batches')]}
 assert not any(census.values()) and all(p['result']=='PASS' for p in protocol)
 depth=depth_check(load(root/'depth-before.json'),load(root/'depth-after.json'),rows)
 checks=self_checks(log,(1280,720));ui=ui_pixels(root,rows)
 unavailable=load(root/'private-availability.json')
 assert unavailable['enabled'] and unavailable['unavailable']==unavailable['host_stencil_writes']==0
 modes=re.findall(r'r.CustomDepth\s*=\s*"?(\d+)',log)
 assert len(modes)>=3 and set(modes)=={'1'},modes
 assert log.count('Capture425 routes=camera_clipping')==1
 assert 'IAI.Auto FIRE' not in log
 good=not bad and depth['result']==checks['result']==ui['result']=='PASS'
 result=dict(result='PASS' if good else 'FAIL',frames=len(rows),dropped=0,rhi=rhi,baseline=baseline,positive_frames=len(positive),positive_spans=spans(positive),unconfirmed_frames=unconfirmed,mask_pixels=mask_pixels,bad_frames=bad,summary=summary,start=start,depth=depth,depth_mode=modes,self_check=checks,ui=ui,unavailable=unavailable,census=census,protocol=protocol,skeletal='EA_Core has no skeletal target; accepted fixture proof remains unchanged')
 return result

def paired(positive,control):
 p,c=Path(positive),Path(control);a=[json.loads(s) for s in (p/'labels.jsonl').read_text().splitlines()];b=[json.loads(s) for s in (c/'labels.jsonl').read_text().splitlines()]
 assert len(a)==len(b)==2352
 out=[];labels=[];changed=[];raw_changed=[]
 for i,(x,y) in enumerate(zip(a,b)):
  assert x['camera']['view_location']==y['camera']['view_location'] and x['camera']['view_rotation']==y['camera']['view_rotation'] and x['camera']['view_fov']==y['camera']['view_fov']
  aa=np.asarray(Image.open(p/x['image']).convert('RGB')).astype(np.int16);bb=np.asarray(Image.open(c/y['image']).convert('RGB')).astype(np.int16)
  delta=np.max(np.abs(aa-bb),axis=2);pixels=int(np.count_nonzero(delta>2));exact=int(np.count_nonzero(delta));label=bool(x['camera_clipping.slab'])
  changed.append(pixels);raw_changed.append(exact);labels.append(label)
  out.append(dict(index=i,labelled=label,changed_pixels=pixels,any_level_pixels=exact,max_delta=int(delta.max())))
  if i%400==0:print('Camera pair reader %d/%d'%(i,len(a)),flush=True)
 mismatches=agree(labels,changed)
 return dict(result='PASS' if not mismatches and sum(labels)>0 else 'FAIL',frames=len(a),rule='Same-index/same-seed/full-frame comparison; established RGB difference >2 levels; raw any-level counts also reported; no stable-region or mask scoping',labelled_frames=sum(labels),spans=spans([i for i,v in enumerate(labels) if v]),label_picture_mismatches=mismatches,positive_changed_min=min(n for n,l in zip(changed,labels) if l),negative_changed_max=max(n for n,l in zip(changed,labels) if not l),negative_any_level_max=max(n for n,l in zip(raw_changed,labels) if not l),early_label_canfail_mismatches=agree(labels[1:]+[False],changed),per_frame=out)

if __name__=='__main__':print(json.dumps(selftest()))
