import json
from pathlib import Path
import sys
from runtime_children import poll

GOOD='UnrealBuildTool.exe -Mode=ValidatePlatforms -allplatforms'

class Tree:
    def __init__(self,commands,cached=None,exit_on_wait=False):
        self.kernel=self
        self.root_pid=1
        self.row=dict(handle=10,pid=2,parent_pid=1,image='UnrealBuildTool.exe')
        if cached is not None:self.row['command_line']=cached
        self.known={2:self.row}
        self.commands=list(commands);self.exited=False;self.exit_on_wait=exit_on_wait;self.waits=[]
    def WaitForSingleObject(self,handle,ms):
        self.waits.append(ms)
        if ms and self.exit_on_wait:self.exited=True
        return 0 if self.exited else 258
    def command_line(self,row):
        value=self.commands.pop(0) if self.commands else None
        if value is None:raise RuntimeError('Owned process command line length unavailable')
        return value

def main(output):
    output=Path(output);output.mkdir(parents=True,exist_ok=False)
    cases=[]
    for name,tree,passes in [
        ('startup_transient',Tree([None,None,GOOD]),True),
        ('known_exit',Tree([None],GOOD,True),True),
        ('live_unreadable_even_with_cache',Tree([None],GOOD),False),
        ('never_read_exit',Tree([None],None,True),False),
        ('build_mode',Tree(['UnrealBuildTool.exe -Mode=Build']),False),
        ('build_triple',Tree(['UnrealBuildTool.exe Example Win64 Development']),False)]:
        try:poll(tree,output/(name+'.jsonl'))
        except RuntimeError as exc:
            if passes:raise
            status='EXPECTED_FAIL';reason=str(exc)
        else:
            assert passes,name
            status='PASS';reason=None
        assert sum(tree.waits)<=500
        cases.append(dict(case=name,result=status,reason=reason,maximum_wait_ms=sum(tree.waits)))
    data=dict(result='PASS',cases=cases,editor_launches=0)
    from capture_425_kit import require_slot_history
    config=dict(guard_requalification='09431b-UBT-command-race')
    old=[dict(leg='C1Game',result='FAIL',end_run=False)]
    require_slot_history(config,'Smoke',2,old,None)
    require_slot_history(config,'C1Game',2,old,'PASS')
    for args in (({},'Smoke',2,old,None),(config,'C1Game',2,old,None),(config,'C1Game',2,[],'PASS')):
        try:require_slot_history(*args)
        except RuntimeError:pass
        else:raise AssertionError('Unqualified retry admitted')
    data['revision_gate']='PASS: fresh smoke required; unarmed smoke and retry without first failure refused'
    (output/'summary.json').write_text(json.dumps(data,indent=2),encoding='utf-8')
    print(json.dumps(data))

if __name__=='__main__':main(sys.argv[1])
