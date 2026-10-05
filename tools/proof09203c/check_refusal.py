import hashlib
import json
from pathlib import Path
import re
import sys
import numpy as np
from PIL import Image

sys.dont_write_bytecode = True
w = Path('E:/IA_BuildCache/_r921/09203c')
sys.path.insert(0, str(w.parent / 'AnomalyInjector/tools'))
from verify_capture import annotation_transition_check, label_rule_check

root = w / 'runs' / sys.argv[1]
reason = sys.argv[2]
assert reason in ('virtual_texture', 'stack_layout', 'texture_not_parameter', 'texture_uniform', 'prefetch_incomplete')
sessions = list((root / 'out').glob('session_*'))
assert len(sessions) == 1
session = sessions[0]
rows = [json.loads(line) for line in (session / 'labels.jsonl').read_text().splitlines()]
rows.sort(key=lambda row: row['session_index'])
assert len(rows) > 120
annotation = json.loads((session / 'annotation.json').read_text())
fixture = json.loads((w / 'author_vt.json').read_text())
boxes = {}
for actor in fixture['actors']:
    if actor['role'] in ('target', 'co_user', 'ordinary'):
        x, y, width, height = map(int, actor['box'])
        boxes[actor['role']] = (x+3, y+3, x+width-3, y+height-3)
reference = np.asarray(Image.open(session / rows[100]['image']).convert('RGB'), dtype=np.int16)
assert reference.shape == (1080, 1920, 3)
pixels = []
for row in rows:
    array = np.asarray(Image.open(session / row['image']).convert('RGB'), dtype=np.int16)
    assert array.shape == reference.shape
    pixels.append(dict(frame=row['session_index'], mad={role: float(np.abs(array[y0:y1,x0:x1]-reference[y0:y1,x0:x1]).mean()) for role,(x0,y0,x1,y1) in boxes.items()}))
null = {role: max(p['mad'][role] for p in pixels if p['frame'] < 120) for role in boxes}
failures = {role: [p['frame'] for p in pixels if p['frame'] >= 120 and p['mad'][role] > null[role]+.05] for role in boxes}
labelled = [row['session_index'] for row in rows if row.get('anomaly_present') or any(a.get('labelled') for a in row.get('anomalies', []))]
log = (root / 'game.log').read_text(errors='replace')
named = [line for line in log.splitlines() if reason in line and ('refus' in line.lower() or 'allreasons' in line or 'reason=' in line or 'reasons=' in line)]
annotation_errors, detail = annotation_transition_check(session)
rule_code, rule_lines, rule_detail = label_rule_check(session)
launch = json.loads((root / 'launch.json').read_text())
issues = []
if launch.get('exit') != 0:
    issues.append('unclean process exit')
if labelled or annotation.get('anomalies'):
    issues.append('refusal emitted an event or label')
if any(failures.values()):
    issues.append('refusal changed pixels')
if not named:
    issues.append('missing named refusal')
if annotation_errors or rule_code:
    issues.append('annotation label disagreement')
if reason == 'prefetch_incomplete':
    if 'PROXYVT-PROBE installed' in log or not re.search(r'PROXYVT-PROBE restored resources=0 producers=0', log):
        issues.append('readiness refusal bound or did not drain')
result = dict(status='PASS' if not issues else 'FAIL', issues=issues, reason=reason, frames=len(rows),
              events=len(annotation.get('anomalies', [])), labelled=labelled, null=null, failures=failures,
              named_refusal=named, annotation_errors=annotation_errors, label_rule_code=rule_code,
              checker_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest())
(root / 'refusal-result.json').write_text(json.dumps(result, indent=2))
(root / 'refusal-pixels.json').write_text(json.dumps(pixels, indent=1))
(root / 'label-rule.txt').write_text('\n'.join(rule_lines))
print(json.dumps({k:result[k] for k in ('status','issues','reason','frames','events','null')}, indent=2))
raise SystemExit(bool(issues))
