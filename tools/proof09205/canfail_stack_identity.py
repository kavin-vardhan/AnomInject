import ast
import contextlib
import hashlib
import json
import runpy
import sys
from pathlib import Path
import numpy as np
from PIL import Image

w=Path('E:/IA_BuildCache/_r921/09205')
reader=w/'check_stack_identity.py'
root=w/'runs'/sys.argv[1]
assert json.loads((root/'native-result.json').read_text())['status']=='PASS'
session=next((root/'out').glob('session_*'))
rows=sorted([json.loads(row) for row in (session/'labels.jsonl').read_text().splitlines()],key=lambda r:r['session_index'])
tree=ast.parse(reader.read_text())
gate=next(node for node in tree.body if isinstance(node,ast.Assert) and 'list(range(480))' in ast.unparse(node.test))
compiled=compile(ast.Module(body=[gate],type_ignores=[]),str(reader),'exec')
cases=[]
for name,indices,wanted in [('complete',list(range(480)),True),('truncated',list(range(479)),False),('duplicate',list(range(479))+[478],False)]:
    try:
        exec(compiled,dict(rows=[dict(session_index=i) for i in indices]))
        accepted=True
    except AssertionError:
        accepted=False
    assert accepted==wanted
    cases.append(dict(case=name,accepted=accepted,expected=wanted))
planted=120
selected=session/rows[planted]['image']
digest=hashlib.sha256(selected.read_bytes()).hexdigest()
original_open=Image.open
original_write=Path.write_text
products={}
def injected_open(path,*args,**kwargs):
    img=original_open(path,*args,**kwargs)
    if Path(path)==selected:
        array=np.asarray(img.convert('RGB')).copy()
        img.close()
        array[388:690,80:1328]=0
        return Image.fromarray(array)
    return img
def intercepted_write(path,text,*args,**kwargs):
    if path.name in ('native-result.json','native-pixels.json'):
        products[path.name]=json.loads(text)
        return len(text)
    return original_write(path,text,*args,**kwargs)
saved=sys.argv
try:
    Image.open=injected_open
    Path.write_text=intercepted_write
    sys.argv=[str(reader),root.name]
    code=0
    with (root/'identity-mutant-reader.log').open('w') as log,contextlib.redirect_stdout(log):
        try: runpy.run_path(str(reader),run_name='__main__')
        except SystemExit as error: code=error.code
finally:
    Image.open=original_open
    Path.write_text=original_write
    sys.argv=saved
assert code==1 and products['native-result.json']['status']=='FAIL'
assert planted in products['native-result.json']['results']['target']['failures']
assert hashlib.sha256(selected.read_bytes()).hexdigest()==digest
assert json.loads((root/'native-result.json').read_text())['status']=='PASS'
result=dict(status='PASS',completeness=cases,pixel_mutant_caught=True,frame=planted,result=products['native-result.json']['results'],reader_sha256=hashlib.sha256(reader.read_bytes()).hexdigest(),original_image_sha256=digest)
(root/'identity-can-fail.json').write_text(json.dumps(result,indent=2))
print(json.dumps(result,indent=2))
