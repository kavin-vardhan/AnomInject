import hashlib
import json
from pathlib import Path
import sys
import numpy as np
from PIL import Image

work = Path('E:/IA_BuildCache/_r921')
root = work / 'runs' / (sys.argv[1] if len(sys.argv) > 1 else 'identity-pie-01')
sessions = list((root / 'out').glob('session_*'))
assert len(sessions) == 1
session = sessions[0]
files = sorted((session / 'Actual_Frames').glob('frame_*.png'))
assert len(files) == 480
reference = np.asarray(Image.open(files[100]).convert('RGB'), dtype=np.int16)
height, width = reference.shape[:2]
fixture = json.loads((work / 'author_vt.json').read_text())
scale = width / fixture['reference_size'][0]
regions = {}
for actor in fixture['actors']:
    if actor['role'] == 'constant_layer':
        continue
    x, y, w, h = actor['box']
    x0, x1 = round(x*scale)+3, round((x+w)*scale)-3
    y0 = round(height/2+(y-540)*scale)+3
    y1 = round(height/2+(y+h-540)*scale)-3
    assert 0 <= x0 < x1 <= width and 0 <= y0 < y1 <= height
    regions[actor['role']] = [x0, y0, x1, y1]


def differences(array):
    return {role: float(np.abs(array[y0:y1, x0:x1]-reference[y0:y1, x0:x1]).mean())
            for role, (x0, y0, x1, y1) in regions.items()}


rows = []
for index, path in enumerate(files):
    array = np.asarray(Image.open(path).convert('RGB'), dtype=np.int16)
    assert array.shape == reference.shape
    rows.append(dict(frame=index, file=path.name, sha256=hashlib.sha256(path.read_bytes()).hexdigest(), mad=differences(array)))
null = {role: max(row['mad'][role] for row in rows[:120]) for role in regions}
threshold = {role: value + .05 for role, value in null.items()}
results = {}
for role in regions:
    failed = [row['frame'] for row in rows[120:] if row['mad'][role] > threshold[role]]
    results[role] = dict(null_max_mad=null[role], limit=threshold[role], failed_frames=failed,
                         peak_mad=max(row['mad'][role] for row in rows[120:]),
                         late_bound_max_mad=max(row['mad'][role] for row in rows[240:295]),
                         late_restored_max_mad=max(row['mad'][role] for row in rows[360:]),
                         pass_all_frames=not failed)
mutant = reference.copy()
x0, y0, x1, y1 = regions['target']
mutant[y0:y1, x0:x1] = np.clip(mutant[y0:y1, x0:x1]+32, 0, 255)
mutant_change = differences(mutant)['target']
assert mutant_change > threshold['target']
log = (root / 'game.log').read_text(encoding='utf-8', errors='replace')
actions = json.loads((root / 'actions.json').read_text())
assert actions['phase'] == 2 and not actions.get('timeout')
assert 'drop=0 first_mip=0 size=2048x2048 tile=128 virtual=1 resources=1 producers=1' in log
assert log.count('PROXYVT-PROBE restored resources=0 producers=0') == 2
summary = dict(status='PASS' if all(row['pass_all_frames'] for row in results.values()) else 'FAIL',
    session=str(session), frame_count=len(files), frame_size=[width, height], reference_frame=100,
    region_derivation='Authored world planes projected with the fixed horizontal FOV: both axes scale by width/1920; vertical origin is actual height/2. Three-pixel interior border.',
    regions_xyxy=regions, results=results, actions=actions,
    private_resource_bind=dict(drop=0, first_mip=0, width=2048, height=2048, tile=128, resources=1, producers=1),
    after_restore=dict(resources=0, producers=0),
    analysis_can_fail=dict(kind='in-memory +32 RGB target mutation, raw captures unchanged', target_mad=mutant_change, limit=threshold['target'], caught=True))
(root / 'pixel-rows.json').write_text(json.dumps(rows, indent=2), encoding='utf-8')
(root / 'identity-result.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
print(json.dumps(summary, indent=2))
raise SystemExit(0 if summary['status'] == 'PASS' else 1)
