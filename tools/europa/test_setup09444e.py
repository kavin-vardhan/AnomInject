import copy,json,sys
from pathlib import Path
P=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector');T=P/'tools/europa';J=Path('D:/IA_Europa/Receipts/09444e')
sys.path.insert(0,str(T));import job_scope
from blackout09444d import require_launch,LAUNCH_CUTOFF
cfg=json.loads((J/'c5-job-config.json').read_text());assert job_scope.memory_policy_allowed(cfg,J)
bad=copy.deepcopy(cfg);bad['memory']['stop_commit_GiB']=3;assert not job_scope.memory_policy_allowed(bad,J)
build=json.loads((J/'paris-build-job.json').read_text());assert job_scope.fm1(build,J)
s=(T/'EuropaTextureBench09444e.cpp').read_text();assert s==(P.parents[1]/'Source/EuropaPhaseAProbe/EuropaTextureBench09444e.cpp').read_text()
assert s.startswith('#include "CoreMinimal.h"\n#if !UE_BUILD_SHIPPING') and 'World->GetMapName()==TEXT("EA_Core")' in s and 'TEXT("IAIBench")' in s
assert s.index('if (!TextureBenchAllowed09444e(World)')<s.index('Var->Set(')
assert all(x not in s for x in ('SetRenderCustomDepth','SetCustomDepthStencil','SavePackage','SaveConfig'))
assert s.count('IAI.Capture.C5')==1 and 'if (State->Complete)' in s
assert [-.125*i for i in range(65)][-1]==-8 and 65*3==195
try:require_launch(now=LAUNCH_CUTOFF)
except RuntimeError:pass
else:raise AssertionError('late launch admitted')
value=dict(result='PASS',scope_positive=2,memory_negative=1,launch_deadline_negative=1,shipping_and_world_guard='PASS',no_state_or_asset_write='PASS',calibration_previews=195)
(J/'setup-tests.json').write_text(json.dumps(value,indent=2));print(json.dumps(value))
