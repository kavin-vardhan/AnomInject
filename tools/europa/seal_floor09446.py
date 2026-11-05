import json,sys
from pathlib import Path
R=Path(__file__).resolve().parent
sys.path.insert(0,'D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa')
from camera_reader09446 import load,sha,floor
from runtime_evidence import ensure_frozen
J=Path('D:/IA_Europa/Receipts/09446');B=Path('E:/IA_BuildCache/09446-evidence')
base=sys.argv[1];assert base in ('D3D11-1280','D3D12-1280','D3D12-1279')
legs=[base+'-control-'+letter for letter in ('a','b')]
roots=[B/leg/'01/plugin-frames/epoch01' for leg in legs]
configs=[];settings=[];starts=[];manifests=[];receipts=[]
for leg,root in zip(legs,roots):
    receipt=J/'H4'/leg/'01';assert load(receipt/'result.json')['result']=='PASS'
    ensure_frozen(receipt/'raw-evidence.json')
    configs.append(load(J/('H4-arming-'+leg+'-01.json')))
    settings.append(load(B/leg/'01/settings.json'))
    start=load(root/'camera-session-start.json');start.pop('start_frame');starts.append(start)
    manifests.append(load(configs[-1]['manifest']['path']))
    labels=load(root/'labels-receipt.json');receipts.append(dict(seed=labels['seed'],routes=labels['routes']))
assert configs[0]['spec']==configs[1]['spec'] and configs[0]['source']==configs[1]['source']
assert manifests[0]['files']==manifests[1]['files'] and settings[0]==settings[1] and starts[0]==starts[1] and receipts[0]==receipts[1]
assert receipts[0]['seed']==4242
assert not (B/(base+'-positive')).exists(),'Positive cannot precede noise measurement'
provenance=dict(result='PASS',spec=configs[0]['spec'],source=configs[0]['source'],seed=receipts[0]['seed'],routes=receipts[0]['routes'],settings=settings[0],pose=starts[0],binary_manifest_files=manifests[0]['files'],control_arming=[str(J/('H4-arming-'+leg+'-01.json')) for leg in legs],control_raw_evidence=[str(J/'H4'/leg/'01/raw-evidence.json') for leg in legs])
(J/'floors').mkdir(exist_ok=True)
value=floor(*roots,J/'floors'/(base+'.json'),provenance)
print(json.dumps(dict(result=value['result'],thresholds=value['thresholds'],sealed_utc=value['sealed_utc'],frames=value['frames'])))
