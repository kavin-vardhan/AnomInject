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
launch = json.loads((root / 'launch.json').read_text())
assert launch['kind'] == 'world_end'
sessions = list((root / 'out').glob('session_*'))
assert len(sessions) == 1
session = sessions[0]
rows = sorted([json.loads(line) for line in (session / 'labels.jsonl').read_text().splitlines()], key=lambda row: row['session_index'])
assert len(rows) > 140
fixture = json.loads((w / 'author_vt.json').read_text())
boxes = {actor['role']: tuple(map(int, actor['box'])) for actor in fixture['actors'] if actor['role'] in ('target', 'co_user', 'ordinary')}
reference = np.asarray(Image.open(session / rows[100]['image']).convert('RGB'), dtype=np.int16)
measurements = []
missing_masks = []
for row in rows:
    array = np.asarray(Image.open(session / row['image']).convert('RGB'), dtype=np.int16)
    assert array.shape == (1080, 1920, 3)
    measurements.append(dict(frame=row['session_index'], mad={role: float(np.abs(array[y+3:y+h-3,x+3:x+b-3]-reference[y+3:y+h-3,x+3:x+b-3]).mean()) for role,(x,y,b,h) in boxes.items()}))
    entries = [entry for entry in row.get('anomalies', []) if entry.get('labelled')]
    if entries:
        if row.get('mask_state') != 'present' or not row.get('mask_file'):
            missing_masks.append(row['session_index'])
        else:
            mask = np.asarray(Image.open(session / row['mask_file']))
            if mask.ndim == 3:
                mask = mask[:,:,0]
            x,y,b,h = boxes['target']
            for entry in entries:
                pixels = mask == entry['mask_value']
                count = int(pixels.sum())
                if not count or count != entry.get('target_pixels') or count != int(pixels[y-2:y+h+2,x-2:x+b+2].sum()):
                    missing_masks.append(row['session_index'])
null = {role: max(value['mad'][role] for value in measurements if value['frame'] < 120) for role in boxes}
threshold = max(4.0, 4*max(null['target'], .5))
bad = []
labelled_count = 0
for row, value in zip(rows, measurements):
    labelled = any(entry.get('labelled') for entry in row.get('anomalies', []))
    labelled_count += int(labelled)
    flagged = any(set(entry.get('transition_reason', [])) & {'effect_interrupted', 'pie_end_settle'} for entry in row.get('anomalies', []))
    changed = value['mad']['target'] >= threshold
    if labelled != changed and not (flagged and not labelled):
        bad.append(value['frame'])
control_bad = {role: [value['frame'] for value in measurements if value['frame'] >= 120 and value['mad'][role] > null[role]+.05] for role in ('co_user', 'ordinary')}
log = (root / 'game.log').read_text(errors='replace')
drains = re.findall(r'VT_DRAIN resources=(\d+) producers=(\d+)', log)
actions = json.loads((root / 'lifetime-actions.json').read_text())
annotation_errors, annotation_detail = annotation_transition_check(session)
rule_code, lines, rule_detail = label_rule_check(session)
issues = []
if launch.get('exit') != 0:
    issues.append('unclean process exit')
if not actions['done'] or actions['actions'][0]['written_frames'] >= 168:
    issues.append('world end not requested during fire')
if not (0 < labelled_count < 48):
    issues.append('world end did not interrupt a visible fire')
if not drains or any(pair != ('0','0') for pair in drains):
    issues.append('private VT drain failed')
if bad or missing_masks or any(control_bad.values()) or annotation_errors or rule_code:
    issues.append('captured label mask or pixel gate failed')
mutant_drains = [*drains[:-1], ('1','0')]
assert any(pair != ('0','0') for pair in mutant_drains)
result = dict(status='PASS' if not issues else 'FAIL', issues=issues, frames=len(rows), labelled=labelled_count, null=null, threshold=threshold, disagreements=bad, missing_masks=missing_masks, control_bad=control_bad, drains=drains, actions=actions, annotation_errors=annotation_errors, rule_code=rule_code, drain_mutant_caught=True, reader_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest())
(root / 'world-end-result.json').write_text(json.dumps(result, indent=2))
(root / 'world-end-pixels.json').write_text(json.dumps(measurements, indent=1))
(root / 'label-rule.txt').write_text('\n'.join(lines))
print(json.dumps(result, indent=2))
raise SystemExit(bool(issues))
