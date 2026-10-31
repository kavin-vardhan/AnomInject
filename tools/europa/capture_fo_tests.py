import json
from pathlib import Path
import sys
from PIL import Image
import capture_425_kit as c1
import capture_c2_kit as c2
import capture_fo_reader as ui
from fixture_policy import UPLOAD,ANALYTICS,sha
from job_scope import memory_policy_allowed

def main(destination):
    J=Path(destination).parent
    scratch=J/'fo-reader-negative';scratch.mkdir(exist_ok=True)
    controls=[]
    for leg,path in ui.CONTROLS.items():
        with Image.open(path) as image:
            count=ui.green(image)
            assert count>=100,(leg,count)
            clean=image.convert('RGB')
            clean.paste((20,20,20),ui.REGION)
            clean.save(scratch/'frame-000000.png')
        assert ui.check(scratch,leg)['result']=='PASS'
        with Image.open(path) as image:image.save(scratch/'frame-000000.png')
        try:ui.check(scratch,leg)
        except ValueError as error:assert 'Heartbeat present' in str(error)
        else:raise AssertionError('Heartbeat mutation was accepted')
        controls.append(dict(leg=leg,path=path,sha256=sha(path),heartbeat_pixels=count,negative='PASS'))
    for leg in ('Smoke','C1PIE','C1Game'):
        argv=c1.command(leg,J/leg)
        assert UPLOAD in argv and ANALYTICS in argv and '-nowrite' in argv
        assert not any('defaultbackbufferpixelformat' in a.casefold() for a in argv)
    argv=c2.command('C2',J/'C2');assert UPLOAD in argv and ANALYTICS in argv
    build=json.loads((c1.JOB/'build-job-config.json').read_text())
    assert memory_policy_allowed(build,c1.JOB)
    runtime=dict(authority=c1.AUTHORITY,runtime_authorized=True,runtime_scope=dict(kind='phase-a-runtime'),build_pairs=[],query_pairs=[],
        memory=dict(start_physical_GiB=4,start_commit_GiB=8,stop_physical_GiB=None,stop_commit_GiB=4))
    assert memory_policy_allowed(runtime,c1.JOB)
    runtime['memory']['start_commit_GiB']=7
    assert not memory_policy_allowed(runtime,c1.JOB)
    old=json.loads(c1.INSTALLED.read_text())
    try:c1.validate(old)
    except RuntimeError as error:assert 'Spent/older arming' in str(error)
    else:raise AssertionError('Old arming accepted')
    result=dict(result='PASS',controls=controls,heartbeat_negative='PASS',argv='PASS',old_arming='REFUSED',launches=0)
    Path(destination).write_text(json.dumps(result,indent=2))
    print(json.dumps(result))

if __name__=='__main__':main(sys.argv[1])
