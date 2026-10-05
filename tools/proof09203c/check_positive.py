import collections
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import sys
import numpy as np
from PIL import Image

sys.dont_write_bytecode = True
w = Path('E:/IA_BuildCache/_r921/09203c')
repo = w.parent / 'AnomalyInjector'
os.environ['IAI_R59_CACHE'] = str(w / 'e1-cache')
sys.path.insert(0, str(repo / 'tools'))
from verify_capture import annotation_transition_check, label_rule_check

root = w / 'runs' / sys.argv[1]
minimum = int(sys.argv[2]) if len(sys.argv) > 2 else 5
sessions = list((root / 'out').glob('session_*'))
assert len(sessions) == 1
session = sessions[0]
rows = {r['session_index']: r for r in map(json.loads, (session / 'labels.jsonl').read_text().splitlines())}
annotation = json.loads((session / 'annotation.json').read_text())
fixture = json.loads((w / 'author_vt.json').read_text())
issues = []
sizes = collections.Counter((r['width'], r['height']) for r in rows.values())
if set(sizes) != {(1920, 1080)}:
    raise SystemExit('Not native 1920x1080: ' + str(sizes))
boxes = {}
for actor in fixture['actors']:
    if actor['role'] not in ('target', 'co_user', 'ordinary'):
        continue
    x, y, width, height = actor['box']
    boxes[actor['role']] = [int(round(x)) + 3, int(round(y)) + 3, int(round(x + width)) - 3, int(round(y + height)) - 3]
reference = np.asarray(Image.open(session / rows[100]['image']).convert('RGB'), dtype=np.int16)
pixels = []
for index, row in rows.items():
    image_path = session / row['image']
    array = np.asarray(Image.open(image_path).convert('RGB'), dtype=np.int16)
    if array.shape != reference.shape:
        raise SystemExit('Written image dimensions differ at ' + str(index))
    mads = {role: float(np.abs(array[y0:y1, x0:x1] - reference[y0:y1, x0:x1]).mean())
            for role, (x0, y0, x1, y1) in boxes.items()}
    pixels.append(dict(frame=index, mad=mads))
null = {role: max(p['mad'][role] for p in pixels if p['frame'] < 120) for role in boxes}
lookup = {p['frame']: p['mad'] for p in pixels}
control_failures = {role: [p['frame'] for p in pixels if p['frame'] >= 120 and p['mad'][role] > null[role] + .05]
                    for role in ('co_user', 'ordinary')}
if any(control_failures.values()):
    issues.append('control pixels changed')
events = []
all_labels = set()
end_settle = set()
missing_masks = []
mask_outside = []
bbox_bad = []
mask_counts = []
target = next(a for a in fixture['actors'] if a['role'] == 'target')
x, y, width, height = map(int, target['box'])
for event in annotation['anomalies']:
    frames = event.get('injected_frames', {}).get('frame_indices', [])
    all_labels.update(frames)
    change = [lookup[i]['target'] for i in frames]
    limit = max(4.0, 4 * max(null['target'], .5))
    visible = bool(change) and float(np.median(change)) >= limit
    events.append(dict(event_id=event['event_id'], frames=frames, median_mad=float(np.median(change)) if change else None,
                       limit=limit, visible=visible, route=event.get('stuck_mip_route')))
    if not visible:
        issues.append('event without measured target change')
    if event.get('stuck_mip_route') != 'proxy_vt':
        issues.append('wrong route')
for index, row in rows.items():
    for entry in row.get('anomalies', []):
        if 'pie_end_settle' in entry.get('transition_reason', []):
            end_settle.add(index)
        if not entry.get('labelled'):
            continue
        if entry.get('stuck_mip.route') != 'proxy_vt' or not entry.get('stuck_mip.held'):
            issues.append('frame route or binding-held mismatch')
        if row.get('mask_state') != 'present' or not row.get('mask_file'):
            missing_masks.append(index)
            continue
        mask = np.asarray(Image.open(session / row['mask_file']))
        if mask.ndim == 3:
            mask = mask[:, :, 0]
        on = mask == entry['mask_value']
        count = int(on.sum())
        if not count:
            missing_masks.append(index)
        outside = count - int(on[max(0, y - 2):y + height + 2, max(0, x - 2):x + width + 2].sum())
        if outside:
            mask_outside.append([index, outside])
        if entry.get('target_pixels') != count:
            mask_counts.append(index)
        for field in ('bbox_px', 'bbox_drawn_px'):
            box = entry.get(field)
            if not box or box[0] < x - 2 or box[1] < y - 2 or box[0] + box[2] > x + width + 2 or box[1] + box[3] > y + height + 2:
                bbox_bad.append([index, field, box])
if missing_masks or mask_outside or mask_counts or bbox_bad:
    issues.append('mask or bbox mismatch')
restored = [i for i in rows if i >= 120 and i not in all_labels and i not in end_settle]
restore_failures = [i for i in restored if lookup[i]['target'] > null['target'] + .05]
if restore_failures:
    issues.append('target revert residue')
if len(events) < minimum:
    issues.append('insufficient events')
annotation_errors, annotation_detail = annotation_transition_check(session)
if annotation_errors:
    issues.append('annotation mismatch')
rule_code, rule_lines, rule_detail = label_rule_check(session)
(root / 'label-rule.txt').write_text('\n'.join(rule_lines))
if rule_code:
    issues.append('label rule mismatch')
reader = Path('E:/IA_BuildCache/_r915/harness-090-10e-eval.py')
spec = importlib.util.spec_from_file_location('e1_09203c', reader)
e1 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(e1)
reading = e1.m52_leg(str(session), log=(root / 'game.log').read_text(errors='replace'))
reading['summary'] = e1.summarize_rows(reading['rows'])
reading['reader'] = dict(path=str(reader), sha256=hashlib.sha256(reader.read_bytes()).hexdigest())
(root / 'e1.json').write_text(json.dumps(reading, indent=2, default=str))
e1_summary = reading['summary']
if e1_summary['judgeable'] != len(events) or e1_summary['judgeable'] < minimum or e1_summary['t50']['unexcused_frames'] or e1_summary['leg_verdict'] != 'PASS':
    issues.append('E1 t50 failed')
result = dict(status='PASS' if not issues else 'FAIL', issues=sorted(set(issues)), session=str(session),
              frame_count=len(rows), size_counts={str(k):v for k,v in sizes.items()}, boxes=boxes, reference=100, null=null,
              events=events, control_failures=control_failures, restore_failures=restore_failures,
              missing_masks=missing_masks, mask_outside=mask_outside, mask_counts=mask_counts, bbox_bad=bbox_bad,
              annotation_errors=annotation_errors, annotation_detail=annotation_detail, label_rule_code=rule_code,
              end_settle=sorted(end_settle), e1_summary=e1_summary)
(root / 'positive-result.json').write_text(json.dumps(result, indent=2, default=str))
(root / 'positive-pixels.json').write_text(json.dumps(pixels, indent=1))
print(json.dumps({k:result[k] for k in ('status','issues','frame_count','null','e1_summary')}, indent=2))
raise SystemExit(0 if not issues else 1)
