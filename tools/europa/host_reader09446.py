import json
import re
from pathlib import Path
import sys
sys.path.insert(0,'D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa')
import h4_reader09446 as h4
import texture_reader09445r as texture
from window09446 import require_reader


def scored_section(log):
    marker='Capture425 routes=blinking,lod_corruption,missing_texture,corrupted_texture'
    assert log.count(marker)==1,'Exactly one scored C5 request required'
    at=log.index(marker)
    before=log[:at]
    count=before.count('Capture425 SELF_CHECK ')
    probes=before.count('Capture425 geometry probe frames=32 auto_fires=disabled')
    assert 3<=probes<=6 and probes+3<=count<=12,'Unexpected preflight sequence'
    assert 'IAI.Auto.Run -> ON' not in before,'Auto fired before qualification'
    return log[at:],count


def read(root,log,fallback,size,rhi):
    require_reader(600)
    scored_log,preview_count=scored_section(log)
    preview_prefix=log[:len(log)-len(scored_log)]
    h4_log='\n'.join(line for line in preview_prefix.splitlines() if 'Capture425 SELF_CHECK ' not in line)+'\n'+scored_log
    result=h4.read(root,h4_log,fallback,size,rhi)
    result['prefire_self_check_count']=preview_count
    attempts=sorted(Path(root).parents[1].glob('attempt-*/prefire-approved.json'))
    assert len(attempts)==1,'Exactly one pre-fire approval required'
    result['textures']=texture.read(root,log,attempts[0].parent)
    assert result['textures']['fallback_frames']==0 and result['textures']['strict_result']=='PASS','GN3 requires strict whole-frame G3'
    from camera_reader09446 import state
    result['camera_session']=state(root)
    assert result['camera_session']['result']=='PASS'
    values=re.findall(r'r.CustomDepth\s*=\s*"?(\d+)',log)
    result['depth_mode']=dict(result='PASS' if len(values)>=3 and set(values)=={'1'} else 'FAIL',readings=values)
    result['skeletal_target']='EA_Core has no skeletal candidate; skeletal coverage is the accepted fixture GF4/texture proof.'
    result['result']='PASS' if result['result']==result['textures']['result']==result['depth_mode']['result']=='PASS' else 'FAIL'
    return result
