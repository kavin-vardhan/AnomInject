import copy
import json
from pathlib import Path
import sys

P=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector')
sys.path.insert(0,str(P/'tools/europa'))
import job_scope
J=Path('D:/IA_Europa/Receipts/09444c')
build=json.loads((J/'build-job-config.json').read_text())
runtime=json.loads((J/'c5-job-config.json').read_text())
assert job_scope.fm1(build,J)
assert job_scope.memory_policy_allowed(runtime,J)
fixture=copy.deepcopy(build)
fixture.update(fixture_project='E:/IA_Fixtures/Outline425/Outline425.uproject',fixture_engine='E:/EpicGames/UE_4.25',
    fixture_first_build_authority='094-42 FZ5',incremental_builds_only=False,
    build_pairs=[['outline425','Outline425Editor','Development'],['outline425','Outline425','Development']])
assert job_scope.outline_fm1(fixture,J)
wrong=copy.deepcopy(fixture);wrong['fixture_project']='E:/IA_Fixtures/Other/Other.uproject'
assert not job_scope.outline_fm1(wrong,J)
wrong=copy.deepcopy(fixture);wrong['max_parallel_actions']=3
assert not job_scope.outline_fm1(wrong,J)
wrong=copy.deepcopy(runtime);wrong['authority']='unruled'
assert not job_scope.memory_policy_allowed(wrong,J)
result=dict(result='PASS',positive=3,negative=3,limits_unchanged=True)
(J/'scope-tests.json').write_text(json.dumps(result,indent=2))
print(json.dumps(result))
