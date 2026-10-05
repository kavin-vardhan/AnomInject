import hashlib
import importlib.util
import json
import os
import sys
from pathlib import Path
import numpy as np
from PIL import Image

sys.dont_write_bytecode = True
w = Path('E:/IA_BuildCache/_r921/09203c')
os.environ['IAI_R59_CACHE'] = str(w / 'e1-cache')
sys.path.insert(0, str(w.parent / 'AnomalyInjector/tools'))
from verify_capture import annotation_transition_check, label_rule_check
root = w / 'runs' / sys.argv[1]
kind = json.loads((root / 'launch.json').read_text())['kind']
sessions = list((root / 'out').glob('session_*'))
assert len(sessions) == 1
session = sessions[0]
rows = [json.loads(line) for line in (session / 'labels.jsonl').read_text().splitlines()]
annotation = json.loads((session / 'annotation.json').read_text())
events = annotation['anomalies']
issues = []
if len(events) < 3:
    issues.append('insufficient events')
if kind in ('hold', 'proxy') and any(e.get('stuck_mip_route') != kind for e in events):
    issues.append('route mismatch')
missing = []
for row in rows:
    entries = [a for a in row.get('anomalies', []) if a.get('labelled')]
    if not entries:
        continue
    if row.get('mask_state') != 'present' or not row.get('mask_file'):
        missing.append(row['session_index'])
        continue
    mask = np.asarray(Image.open(session / row['mask_file']))
    if mask.ndim == 3:
        mask = mask[:, :, 0]
    if any(not np.any(mask == a['mask_value']) for a in entries):
        missing.append(row['session_index'])
if missing:
    issues.append('label without measured mask')
annotation_errors, annotation_detail = annotation_transition_check(session)
if annotation_errors:
    issues.append('annotation mismatch')
code, lines, detail = label_rule_check(session)
(root / 'label-rule.txt').write_text('\n'.join(lines))
if code:
    issues.append('label-rule failed')
reader = Path('E:/IA_BuildCache/_r915/harness-090-10e-eval.py')
spec = importlib.util.spec_from_file_location('e1_09203c_regression', reader)
e1 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(e1)
log = (root / 'game.log').read_text(errors='replace')
if kind in ('hold', 'proxy'):
    reading = e1.m52_leg(str(session), log=log)
    summary = e1.summarize_rows(reading['rows'])
    unflagged = summary['t50']['unexcused_frames']
    judgeable = summary['judgeable']
else:
    reading = e1.diff_leg(str(session), log=log)
    summary = e1.summarize_diff(reading['events'])
    judgeable = summary['t50']['verdicts'].get('PASS', 0)
    unflagged = sum(len(r['per']['t50']['transition_gate']['unlabelled_visible_without_transition']) + len(r['per']['t50']['transition_gate']['labelled_not_visible_outside_transition']) for r in reading['events'] if r.get('status') == 'OK')
if summary['leg_verdict'] != 'PASS' or unflagged or judgeable != len(events) or judgeable < 3:
    issues.append('E1 failed')
reading['reader'] = dict(path=str(reader), sha256=hashlib.sha256(reader.read_bytes()).hexdigest())
(root / 'e1.json').write_text(json.dumps(reading, indent=2, default=str))
result = dict(status='PASS' if not issues else 'FAIL', issues=issues, kind=kind, session=str(session), fires=len(events),
              e1_summary=summary, unflagged=unflagged, judgeable=judgeable, missing_masks=missing,
              annotation_errors=annotation_errors, annotation_detail=annotation_detail, label_rule_code=code)
(root / 'regression-result.json').write_text(json.dumps(result, indent=2, default=str))
print(json.dumps(result, indent=2, default=str))
raise SystemExit(bool(issues))
