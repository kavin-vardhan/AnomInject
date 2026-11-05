import json,sys
from pathlib import Path
P=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector')
sys.path.insert(0,str(P/'tools/europa'))
from camera_reader09446s import paired,state,load,sha,selftest
from runtime_evidence import ensure_frozen
J=Path('D:/IA_Europa/Receipts/09446s');OLD=J.parent/'09446r';B=Path('E:/IA_BuildCache/09446r-evidence')
assert load(J/'owner-p9-result.json')['result']=='PASS'
assert load(J/'predictions.json')['authority']=='094-46r GP2 GP3'
assert not (J/'D3D12-rejudged.json').exists()
test=selftest();(J/'reader-selftest.json').write_text(json.dumps(test,indent=2));print(json.dumps(test),flush=True)
for leg in ('D3D12-1280-control-a','D3D12-1280-positive'):
    ensure_frozen(OLD/'H4'/leg/'01/raw-evidence.json')
positive=B/'D3D12-1280-positive/01/plugin-frames/epoch01';control=B/'D3D12-1280-control-a/01/plugin-frames/epoch01'
value=paired(positive,control,OLD/'floors/D3D12-1280-combined.json')
full=J/'D3D12-rejudged-full.json';full.write_text(json.dumps(value,indent=2))
summary={k:v for k,v in value.items() if k!='per_frame'}
summary.update(full=str(full),positive_state=state(positive),control_state=state(control),old_result=str(OLD/'H4/D3D12-1280-positive/01/paired-pixels.json'),old_result_sha256=sha(OLD/'H4/D3D12-1280-positive/01/paired-pixels.json'))
(J/'D3D12-rejudged.json').write_text(json.dumps(summary,indent=2))
print(json.dumps({k:v for k,v in summary.items() if k not in ('positive_state','control_state')}),flush=True)
assert value['result']=='PASS'
