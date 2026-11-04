import json
from pathlib import Path
from unittest.mock import patch
import h4_kit09445 as kit

path=kit.J/'H4/D3D11-1280/01/result.json'
kit.unscored_setup_abort(path)
original=Path.read_text
def nonempty(self,*a,**kw):
    text=original(self,*a,**kw)
    if self.name=='capture.json':
        value=json.loads(text);value['written']=1;return json.dumps(value)
    return text
negative=0
with patch.object(Path,'read_text',nonempty):
    try:kit.unscored_setup_abort(path)
    except AssertionError:negative+=1
    else:raise AssertionError('Captured gate data incorrectly admitted as setup abort')
try:kit.unscored_setup_abort(kit.J/'H4/D3D12-1280/01/result.json')
except AssertionError:negative+=1
else:raise AssertionError('Unknown failure admitted')
def crash(self,*a,**kw):
    text=original(self,*a,**kw)
    if self==path:
        value=json.loads(text);value['crashed']=True;return json.dumps(value)
    return text
with patch.object(Path,'read_text',crash):
    try:kit.unscored_setup_abort(path)
    except AssertionError:negative+=1
    else:raise AssertionError('Runtime crash admitted')
value=dict(result='PASS',positive=1,negative=negative,raw_mutated=False)
(kit.J/'retry-scope-tests.json').write_text(json.dumps(value,indent=2));print(json.dumps(value))
