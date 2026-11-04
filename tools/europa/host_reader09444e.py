import json
import re
from pathlib import Path
import sys
sys.path.insert(0,'D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa')
import capture_phaseb_h4_reader as h4
import capture_texture425_reader as texture
from blackout09444d import require_reader


def read(root,log,fallback,size,rhi):
    require_reader(600)
    result=h4.read(root,log,fallback,size,rhi)
    result['textures']=texture.read(root,log,False)
    values=re.findall(r'r.CustomDepth\s*=\s*"?(\d+)',log)
    result['depth_mode']=dict(result='PASS' if len(values)>=3 and set(values)=={'1'} else 'FAIL',readings=values)
    result['skeletal_target']='EA_Core has no skeletal candidate; skeletal coverage is the accepted fixture GF4/texture proof.'
    result['result']='PASS' if result['result']==result['textures']['result']==result['depth_mode']['result']=='PASS' else 'FAIL'
    return result
