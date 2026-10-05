import json
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
expected=json.loads((w/'census-expected-before.json').read_text())
for fixture,layers in [('single',1),('stacks',3)]:
    for family,row in enumerate(expected[fixture+'-view']):
        row.update(components=1,vt_components=1,bindings=layers,streaming=1)
        row['stacks']={str(k):(1 if k==layers else 0) for k in range(1,9)}
        for field in ('required_components','required_bindings','only_vt','only_vt_unassessed','only_stack','only_stack_unassessed','layers_bc5','layers_dxt5','layers_bc7','layers_dxt1','layers_bgra8','layers_other'):
            assert row[field]%2==0,(fixture,family,field,row[field])
            row[field]//=2
        for field in ('binding_sets','component_sets','tiles','borders','address_x','address_y'):
            row[field]={key:value//2 for key,value in row[field].items()}
path=w/'census-expected-v2-before.json'
assert not path.exists()
path.write_text(json.dumps(expected,indent=2))
print(path)
