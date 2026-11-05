import datetime,json,sys
from pathlib import Path
P=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector');sys.path.insert(0,str(P/'tools/europa'))
from camera_reader09446s import load,sha,state,larger
from runtime_evidence import ensure_frozen
J=Path('D:/IA_Europa/Receipts/09446s');OLD=J.parent/'09446r';B=Path('E:/IA_BuildCache/09446s-evidence')
root=B/'D3D12-1279-control-a/01/plugin-frames/epoch01'
receipt=J/'H4/D3D12-1279-control-a/01'
assert load(receipt/'result.json')['result']=='PASS'
ensure_frozen(receipt/'raw-evidence.json')
assert state(root)['positive_frames']==0 and state(root)['baseline']
cfg=load(J/'H4-arming-D3D12-1279-control-a-01.json')
oldcfg=load(OLD/'H4-arming-D3D12-1280-positive-01.json')
manifest=load(cfg['manifest']['path']);old_manifest=load(oldcfg['manifest']['path'])
canonical=lambda m:{v['path']:v['sha256'] for v in m['files']}
assert canonical(manifest)==canonical(old_manifest)
settings=load(root.parents[1]/'settings.json')
assert settings==load('E:/IA_BuildCache/09446r-evidence/D3D12-1280-positive/01/settings.json')
paths=[OLD/'floors'/('D3D12-1280-'+kind+'.json') for kind in ('session','near-plane')]
floors=[load(p) for p in paths];limits=larger(floors)
assert limits['max_delta']==2
labels=load(root/'labels-receipt.json')
provenance=dict(spec=cfg['spec'],seed=labels['seed'],routes=labels['routes'],settings=settings,pose=load(root/'camera-session-start.json'),control_raw_evidence=str(receipt/'raw-evidence.json'),binary_files=canonical(manifest))
value=dict(result='PASS',sealed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),authority='GP3 explicitly permits sharing D3D12 floors; same-size control required for pixel pairing',rule='GP2 max level only; count information only',thresholds=limits,floors=[dict(path=str(p),sha256=sha(p),thresholds=v['thresholds']) for p,v in zip(paths,floors)],control=str(root),control_labels_sha256=sha(root/'labels.jsonl'),comparison_provenance=provenance,source_floor_size=[1280,720],comparison_size=[1279,719],same_binaries=True,same_settings=True,no_resampling=True)
(J/'floors').mkdir(exist_ok=True)
out=J/'floors/D3D12-1279-combined.json';assert not out.exists();out.write_text(json.dumps(value,indent=2))
print(json.dumps(dict(result='PASS',path=str(out),sha256=sha(out),thresholds=limits,sealed_utc=value['sealed_utc'])))
