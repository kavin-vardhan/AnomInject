import copy
import json
import os
from pathlib import Path
import sys
import build_memory as memory
import job_scope


def main(output):
    scope=job_scope.load()
    assert job_scope.fm1(scope,Path(scope['job_root']))
    low_physical=dict(free_physical=1,free_commit=8*memory.GIB)
    assert memory.admission(low_physical)
    assert not memory.admission(dict(low_physical,free_commit=8*memory.GIB-1))
    assert not memory.pressure(dict(low_physical,free_commit=3*memory.GIB))
    assert memory.pressure(dict(low_physical,free_commit=3*memory.GIB-1))
    output.mkdir(parents=True,exist_ok=False)
    reads=[]
    def sampler():
        reads.append(1)
        return low_physical
    def no_sleep(seconds):
        raise AssertionError('FM1 waited for a second reading')
    memory.wait_for_window(output/'admission.jsonl',sampler=sampler,sleeper=no_sleep)
    assert len(reads)==1
    stops=[]
    try:
        memory.monitor_sample(dict(low_physical,free_commit=3*memory.GIB-1),0,
            lambda:stops.append(123) or [dict(pid=123,owned=True)],output/'stop.jsonl')
    except memory.MemoryStopped:pass
    else:raise AssertionError('First low reading did not stop')
    assert stops==[123]
    mutations=[]
    for field,value in [('max_parallel_actions',4),('max_build_attempts_per_target',4),('full_rebuild',True),('runtime_authorized',True)]:
        mutant=copy.deepcopy(scope);mutant[field]=value
        assert not job_scope.fm1(mutant,Path(scope['job_root']))
        mutations.append(field)
    old=dict(start_physical_GiB=4,start_commit_GiB=16,stop_physical_GiB=None,stop_commit_GiB=6)
    assert not memory.admission(dict(free_physical=5*memory.GIB,free_commit=15*memory.GIB),old)
    result=dict(result='PASS',start='one reading, commit >=8 GiB, no physical blocker',stop='first reading below 3 GiB',
        negative='8 GiB minus one byte refuses; 3 GiB minus one byte stops once',scope_mutants=mutations,old_policy_preserved=True,processes_launched=0)
    (output/'summary.json').write_text(json.dumps(result,indent=2))
    print(json.dumps(result))


if __name__=='__main__':main(Path(sys.argv[1]))
