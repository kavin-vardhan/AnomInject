import json
from pathlib import Path

w=Path('E:/IA_BuildCache/_r921/09205')
rows=json.loads((w/'census-expected-v2-before.json').read_text())
assert rows['single-all'][0]['only_vt']==4
rows['single-all'][0]['only_vt']=3
rows['single-all'][0]['only_vt_unassessed']=3
path=w/'census-expected-v3-before.json'
assert not path.exists()
path.write_text(json.dumps(rows,indent=2))
