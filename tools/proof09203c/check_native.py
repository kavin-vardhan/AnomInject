import collections
import json
import sys
from pathlib import Path
import numpy as np
from PIL import Image

w = Path('E:/IA_BuildCache/_r921/09203c')
root = w / 'runs' / sys.argv[1]
sessions = list((root / 'out').glob('session_*'))
assert len(sessions) == 1
session = sessions[0]
rows = [json.loads(line) for line in (session / 'labels.jsonl').read_text().splitlines()]
fixture = json.loads((w / 'author_vt.json').read_text())
sizes = collections.Counter((r['width'], r['height']) for r in rows)
assert set(sizes) == {(1920, 1080)}, sizes
reference = np.asarray(Image.open(session / rows[100]['image']).convert('RGB'), dtype=np.int16)
boxes = {}
for a in fixture['actors']:
    if a['role'] in ('target', 'co_user', 'ordinary'):
        x, y, width, height = map(int, a['box'])
        boxes[a['role']] = (x + 3, y + 3, x + width - 3, y + height - 3)
pixels = []
for row in rows:
    array = np.asarray(Image.open(session / row['image']).convert('RGB'), dtype=np.int16)
    assert array.shape == reference.shape
    mad = {role: float(np.abs(array[y0:y1, x0:x1] - reference[y0:y1, x0:x1]).mean()) for role, (x0, y0, x1, y1) in boxes.items()}
    pixels.append(dict(frame=row['session_index'], mad=mad))
results = {}
for role in boxes:
    null = max(p['mad'][role] for p in pixels if p['frame'] < 120)
    failures = [p['frame'] for p in pixels if p['frame'] >= 120 and p['mad'][role] > null + .05]
    results[role] = dict(null=null, limit=null + .05, maximum=max(p['mad'][role] for p in pixels if p['frame'] >= 120), failures=failures)
result = dict(status='PASS' if not any(r['failures'] for r in results.values()) else 'FAIL', session=str(session), frame_count=len(rows),
              dimensions=[1920, 1080], reference=100, null_frames=[0, 119], every_measured_frame=[120, rows[-1]['session_index']],
              results=results, launch=json.loads((root / 'launch.json').read_text()))
(root / 'native-result.json').write_text(json.dumps(result, indent=2))
(root / 'native-pixels.json').write_text(json.dumps(pixels, indent=1))
print(json.dumps({k:result[k] for k in ('status', 'frame_count', 'dimensions', 'results')}, indent=2))
raise SystemExit(result['status'] != 'PASS')
