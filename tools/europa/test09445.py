import ast,copy,datetime,hashlib,json,sys
from pathlib import Path
R=Path(__file__).resolve().parent;P=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector');T=P/'tools/europa';J=Path('D:/IA_Europa/Receipts/09445')
sys.path.insert(0,str(T))
import job_scope
import geometry_reader09445 as geometry
import calibration_reader09445 as calibration
import host_reader09445 as host
import window09445 as window
end=datetime.datetime.fromisoformat(window.admission_deadline())
assert 10790<(end-datetime.datetime.now(datetime.timezone.utc)).total_seconds()<=10800

for name in ('window09445.py','geometry_reader09445.py','c_space09445.py','calibration_reader09445.py','host_reader09445.py','h4_kit09445.py','host_build09445.py'):
    ast.parse((R/name).read_text())
positive=negative=0
for name in ('paris-build-job.json','c5-job-config.json'):
    cfg=json.loads((J/name).read_text());assert job_scope.load(J/name);positive+=1
    bad=copy.deepcopy(cfg);bad['memory']['stop_commit_GiB']=1
    assert not job_scope.memory_policy_allowed(bad,J);negative+=1
    assert not job_scope.memory_policy_allowed(cfg,J.parent/'Unruled');negative+=1
marker='Capture425 routes=blinking,lod_corruption,missing_texture,corrupted_texture'
valid=('Capture425 geometry probe frames=32 auto_fires=disabled\nCapture425 SELF_CHECK stage=start\nCapture425 SELF_CHECK stage=start\n')*3+marker+'\nCapture425 SELF_CHECK stage=start\nCapture425 SELF_CHECK stage=300'
assert host.scored_section(valid)[1]==6
for bad in (valid+marker,valid.replace('Capture425 geometry probe frames=32 auto_fires=disabled',''), 'IAI.Auto.Run -> ON\n'+valid):
    try:host.scored_section(bad)
    except AssertionError:negative+=1
    else:raise AssertionError('Sequence can-fail admitted')
helper=(T/'EuropaTextureBench09444e.cpp').read_bytes()
assert helper==(P.parents[1]/'Source/EuropaPhaseAProbe/EuropaTextureBench09444e.cpp').read_bytes()
assert b'Var->Set' in helper and b'FindConsoleVariable(TEXT("r.CustomDepth"))->GetInt()' in helper
assert b'FindConsoleVariable(TEXT("r.CustomDepth"))->Set' not in helper
labels=(P/'Source/AnomalyCapture/Private/Compat/AnomalyCaptureLabels425.cpp').read_text()
assert labels.index('if (CaptureGeometry425) { return; }')<labels.index('if (!AutoStarted && Next>=3)')
assert labels.index('Prepared = Next;')<labels.index('if (CaptureGeometry425) { return; }')
mutant=labels.replace('    Prepared = Next;\n    if (CaptureGeometry425) { return; }','    if (CaptureGeometry425) { return; }\n    Prepared = Next;')
assert mutant.index('Prepared = Next;')>mutant.index('if (CaptureGeometry425) { return; }')
assert '#if !UE_BUILD_SHIPPING\nstatic FAutoConsoleCommandWithWorldAndArgs CaptureGeometryCommand425' in labels
result=dict(result='PASS',geometry=geometry.test(),gk2=calibration.test(),scope_positive=positive,scope_and_sequence_negative=negative,helper_sha256=hashlib.sha256(helper).hexdigest(),native_contract='Bench geometry returns before auto start; Shipping guard, IAIBench and exact map; same-session readers fail closed before fire')
(J/'focused-tests.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))
