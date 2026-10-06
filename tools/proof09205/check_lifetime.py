import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import sys
import numpy as np
from PIL import Image

sys.dont_write_bytecode = True
w = Path('E:/IA_BuildCache/_r921/09205')
sys.path.insert(0, str(w.parent / 'AnomalyInjector/tools'))
from verify_capture import annotation_transition_check, label_rule_check

root = w / 'runs' / sys.argv[1]
launch = json.loads((root / 'launch.json').read_text())
kind = launch['kind']
assert kind in ('destroy', 'displace')
sessions = list((root / 'out').glob('session_*'))
assert len(sessions) == 1
session = sessions[0]
rows = [json.loads(line) for line in (session / 'labels.jsonl').read_text().splitlines()]
rows.sort(key=lambda row: row['session_index'])
fixture = json.loads((w / 'author_stacks.json').read_text())
boxes = {}
for actor in fixture['actors']:
    if actor['role'] in ('target', 'co_user', 'ordinary'):
        x, y, width, height = map(int, actor['box'])
        boxes[actor['role']] = (x+3, y+3, x+width-3, y+height-3)


def rois(index):
    array = np.asarray(Image.open(session / rows[index]['image']).convert('RGB'), dtype=np.int16)
    assert array.shape == (1080, 1920, 3)
    return {role: array[y0:y1, x0:x1] for role, (x0, y0, x1, y1) in boxes.items()}


def mad(a, b):
    return float(np.abs(a-b).mean())


reference = rois(100)
signatures = dict(baseline=reference['target'], anomaly=rois(130)['target'])
if kind == 'destroy':
    signatures['absent'] = rois(len(rows)-1)['target']
assert mad(signatures['baseline'], signatures['anomaly']) >= 4
if kind == 'destroy':
    assert all(mad(signatures['absent'], signatures[p]) >= 4 for p in ('baseline', 'anomaly'))
measurements = []
for i, row in enumerate(rows):
    assert row['session_index'] == i
    observed = rois(i)
    distances = {phase: mad(observed['target'], image) for phase, image in signatures.items()}
    ordered = sorted(distances, key=distances.get)
    phase = ordered[0] if distances[ordered[1]] - distances[ordered[0]] >= 4 else 'ambiguous'
    measurements.append(dict(frame=i, phase=phase, distances=distances,
                             mad={role: mad(observed[role], reference[role]) for role in boxes}))
null = {role: max(m['mad'][role] for m in measurements[:120]) for role in boxes}
issues = []
if launch.get('exit') != 0:
    issues.append('unclean process exit')
controls = {role: [m['frame'] for m in measurements[120:] if m['mad'][role] > null[role]+.05] for role in ('co_user', 'ordinary')}
if any(controls.values()):
    issues.append('control changed')
if any(m['phase'] == 'ambiguous' for m in measurements):
    issues.append('ambiguous target picture')


def disagreements(input_rows):
    bad = []
    for row, measurement in zip(input_rows, measurements):
        entries = [a for a in row.get('anomalies', []) if a.get('id') == 'stuck_low_mip']
        labelled = any(a.get('labelled') for a in entries)
        flagged = any(set(a.get('transition_reason', [])) & {'effect_interrupted', 'pie_end_settle'} for a in entries)
        anomalous = measurement['phase'] == 'anomaly'
        if (labelled and not anomalous) or (anomalous and not labelled and not flagged):
            bad.append(row['session_index'])
    return bad


bad = disagreements(rows)
if bad:
    issues.append('label picture disagreement')
last_anomaly = max((m['frame'] for m in measurements if m['phase'] == 'anomaly'), default=-1)
end_phase = 'absent' if kind == 'destroy' else 'baseline'
residue = [m['frame'] for m in measurements if m['frame'] > last_anomaly and m['distances'][end_phase] > null['target']+.05]
if residue:
    issues.append('end picture residue')
missing_masks = []
outside_masks = []
for row in rows:
    entries = [a for a in row.get('anomalies', []) if a.get('labelled')]
    if not entries:
        continue
    if not row.get('mask_file') or row.get('mask_state') != 'present':
        missing_masks.append(row['session_index'])
        continue
    mask = np.asarray(Image.open(session / row['mask_file']))
    if mask.ndim == 3:
        mask = mask[:, :, 0]
    for entry in entries:
        pixels = mask == entry['mask_value']
        count = int(pixels.sum())
        if not count or count != entry.get('target_pixels'):
            missing_masks.append(row['session_index'])
        x0, y0, x1, y1 = boxes['target']
        if count != int(pixels[y0-5:y1+5, x0-5:x1+5].sum()):
            outside_masks.append(row['session_index'])
if missing_masks or outside_masks:
    issues.append('mask mismatch')
annotation_errors, annotation_detail = annotation_transition_check(session)
rule_code, rule_lines, rule_detail = label_rule_check(session)
if annotation_errors or rule_code:
    issues.append('annotation label mismatch')
log = (root / 'game.log').read_text(errors='replace')
assert 'VT_COMBINED_DRAIN refs=0' in log
drains = re.findall(r'VT_DRAIN resources=(\d+) producers=(\d+)', log)
if not drains or any(r != ('0', '0') for r in drains):
    issues.append('private VT drain failed')
interrupted = [r['session_index'] for r in rows if any('effect_interrupted' in a.get('transition_reason', []) for a in r.get('anomalies', []))]
if not interrupted:
    issues.append('missing interruption evidence')
if kind == 'displace':
    os.environ['IAI_R59_CACHE'] = str(w / 'e1-cache')
    reader = Path('E:/IA_BuildCache/_r915/harness-090-10e-eval.py')
    spec = importlib.util.spec_from_file_location('lifetime_e1', reader)
    e1 = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(e1)
    reading = e1.m52_leg(str(session), log=log)
    summary = e1.summarize_rows(reading['rows'])
    (root / 'e1.json').write_text(json.dumps(dict(reading=reading, summary=summary), indent=2, default=str))
    if summary['t50']['unexcused_frames'] or summary['leg_verdict'] != 'PASS':
        issues.append('E1 failed')
mutant = json.loads(json.dumps(rows))
index = last_anomaly + 2
assert 0 <= index < len(mutant)
source_entry = next(a for r in rows for a in r.get('anomalies', []) if a.get('labelled'))
planted = dict(source_entry, labelled=True)
mutant[index]['anomalies'] = [planted]
caught = index in disagreements(mutant)
assert caught
result = dict(status='PASS' if not issues else 'FAIL', kind=kind, issues=issues, frames=len(rows), null=null,
              controls=controls, disagreements=bad, last_anomalous_frame=last_anomaly, residue=residue,
              missing_masks=missing_masks, outside_masks=outside_masks, drains=drains, interrupted_frames=interrupted,
              annotation_errors=annotation_errors, rule_code=rule_code, mutant_caught=caught, mutant_frame=index,
              reader_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest())
(root / 'lifetime-result.json').write_text(json.dumps(result, indent=2))
(root / 'lifetime-pixels.json').write_text(json.dumps(measurements, indent=1))
(root / 'label-rule.txt').write_text('\n'.join(rule_lines))
print(json.dumps(result, indent=2))
raise SystemExit(bool(issues))
