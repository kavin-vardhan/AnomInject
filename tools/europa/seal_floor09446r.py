import datetime,json,sys
from pathlib import Path
R=Path(__file__).parent;P=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector');sys.path.insert(0,str(P/'tools/europa'))
from camera_reader09446r import load,sha,state,floor,larger,paired
from runtime_evidence import ensure_frozen
J=Path('D:/IA_Europa/Receipts/09446r');B=Path('E:/IA_BuildCache/09446r-evidence');OLDJ=Path('D:/IA_Europa/Receipts/09446');OLDB=Path('E:/IA_BuildCache/09446-evidence')
def session(job,evidence,leg):
 receipt=job/'H4'/leg/'01';result=load(receipt/'result.json');assert result['result']=='PASS',str(receipt)
 ensure_frozen(receipt/'raw-evidence.json')
 root=evidence/leg/'01/plugin-frames/epoch01';cfg=load(job/('H4-arming-'+leg+'-01.json'))
 labels=load(root/'labels-receipt.json');start=load(root/'camera-session-start.json');start.pop('start_frame')
 return dict(root=root,receipt=receipt,cfg=cfg,labels=labels,start=start,settings=load(evidence/leg/'01/settings.json'),manifest=load(cfg['manifest']['path']))
def same(a,b):
 assert a['cfg']['spec']==b['cfg']['spec'] and a['settings']==b['settings']
 assert a['labels']['seed']==b['labels']['seed']==4242 and a['labels']['routes']==b['labels']['routes']
def provenance(a,b,kind):
 return dict(spec=a['cfg']['spec'],seed=a['labels']['seed'],routes=a['labels']['routes'],settings=a['settings'],pose=a['start'],kind=kind,matched_indices=[0,2352] if kind=='near_plane' else [0,2640],control_raw_evidence=[str(v['receipt']/'raw-evidence.json') for v in (a,b)],sources=[v['cfg']['source'] for v in (a,b)],binary_manifests=[v['manifest']['files'] for v in (a,b)],same_binaries=a['manifest']['files']==b['manifest']['files'],difference='D3D11 reuses accepted control; new guarded far-only switch and additive start receipt are the only native changes' if a['cfg']['source']!=b['cfg']['source'] else 'none')
def seal(base,kind):
 if kind=='session':
  assert base=='D3D12-1280';a=session(J,B,base+'-control-a');b=session(J,B,base+'-control-b');same(a,b)
  assert a['manifest']['files']==b['manifest']['files'] and a['start']==b['start']
 else:
  a=session(J,B,base+'-near-floor');b=session(OLDJ,OLDB,base+'-control-a') if base=='D3D11-1280' else session(J,B,base+'-control-a');same(a,b)
  assert state(a['root'])['far_only'] and state(a['root'])['positive_frames']==0
  assert state(b['root'])['baseline'] and state(b['root'])['positive_frames']==0
  assert all(a['start'][k]==b['start'][k] for k in ('near_start','pose_hold','near_position','far_position'))
  if base=='D3D12-1280':assert a['manifest']['files']==b['manifest']['files']
 (J/'floors').mkdir(exist_ok=True)
 out=J/'floors'/(base+'-'+kind.replace('_','-')+'.json')
 value=floor(a['root'],b['root'],out,provenance(a,b,kind),near_plane=kind=='near_plane')
 print(json.dumps(dict(result=value['result'],path=str(out),thresholds=value['thresholds'],frames=value['frames'],sealed_utc=value['sealed_utc'])),flush=True)
def merge(base):
 session_path=OLDJ/'floors'/('D3D11-1280.json') if base=='D3D11-1280' else J/'floors'/(base+'-session.json')
 near_path=J/'floors'/(base+'-near-plane.json');paths=[session_path,near_path];values=[load(p) for p in paths]
 assert all(v['result']=='PASS' for v in values)
 control=Path(values[0]['control_a']);thresholds=larger(values)
 result=dict(result='PASS',sealed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),thresholds=thresholds,rule='GO2 larger maximum delta; count from that level, max count only if levels tie',floors=[dict(path=str(p),sha256=sha(p),thresholds=v['thresholds']) for p,v in zip(paths,values)],control=str(control),control_labels_sha256=sha(control/'labels.jsonl'),rejudge_existing_authority='GO4' if base=='D3D11-1280' else None)
 out=J/'floors'/(base+'-combined.json');assert not out.exists();out.write_text(json.dumps(result,indent=2));print(json.dumps(result),flush=True)
def rejudge():
 base='D3D11-1280';root=OLDB/(base+'-positive')/'01/plugin-frames/epoch01'
 ensure_frozen(OLDJ/'H4'/(base+'-positive')/'01/raw-evidence.json')
 positive_state=state(root);assert positive_state['positive_frames']==144
 value=paired(root,OLDB/(base+'-control-a')/'01/plugin-frames/epoch01',J/'floors'/(base+'-combined.json'),allow_existing=True)
 full=J/'D3D11-rejudged-full.json';assert not full.exists();full.write_text(json.dumps(value,indent=2))
 summary={k:v for k,v in value.items() if k!='per_frame'};summary.update(full=str(full),spans=positive_state['spans']);(J/'D3D11-rejudged.json').write_text(json.dumps(summary,indent=2));print(json.dumps(summary),flush=True)
if __name__=='__main__':
 action=sys.argv[1]
 if action in ('session','near_plane'):seal(sys.argv[2],action)
 elif action=='merge':merge(sys.argv[2])
 elif action=='rejudge':rejudge()
 else:raise SystemExit('Unknown operation')
