import json
from pathlib import Path
import re
import sys

w = Path('E:/IA_BuildCache/_r921/09203c')
label = sys.argv[1]
legs = sys.argv[2:]
output = w / (label + '.json')
assert not output.exists()
result = []
for leg in legs:
    root = w / 'runs' / leg
    gate = json.loads((root / 'positive-result.json').read_text())
    pixels = {row['frame']:row['mad'] for row in json.loads((root / 'positive-pixels.json').read_text())}
    e1 = json.loads((root / 'e1.json').read_text())
    launch = json.loads((root / 'launch.json').read_text())
    log = (root / 'game.log').read_text(errors='replace')
    events = []
    for event, edge in zip(gate['events'], e1['rows']):
        start, end = min(event['frames']), max(event['frames'])
        changed = [i for i in range(start-4,end+5) if pixels[i]['target'] >= event['limit']]
        events.append(dict(event=event['event_id'], first_label=start, last_label=end,
                           first_changed=min(changed) if changed else None, last_changed=max(changed) if changed else None,
                           median_mad=event['median_mad'], change_limit=event['limit'], sharpness_drop=edge.get('depth'),
                           first_after_mad=pixels[end+1]['target'], onset_s=start/30, offset_s=(end+1)/30))
    controls = {role:dict(limit=gate['null'][role]+.05, peak=max(row[role] for i,row in pixels.items() if i>=120), failures=len(gate['control_failures'][role])) for role in ('co_user','ordinary')}
    positive = {index for event in gate['events'] for index in event['frames']}
    restored = [row['target'] for index,row in pixels.items() if index>=120 and index not in positive]
    result.append(dict(leg=leg, status=gate['status'], process_exit=launch.get('exit'), frames=gate['frame_count'], labelled=len(positive),
                       events=events, controls=controls, restore_limit=gate['null']['target']+.05,
                       all_unlabelled_restore_peak=max(restored) if restored else None,
                       missing_masks=len(gate['missing_masks']), mask_outside=len(gate['mask_outside']), bbox_bad=len(gate['bbox_bad']),
                       annotation_errors=gate['annotation_errors'], rule_code=gate['label_rule_code'], e1=gate['e1_summary'],
                       drains=re.findall(r'VT_DRAIN resources=(\d+) producers=(\d+)',log),
                       allocations=re.findall(r'TEXCORRUPT-LEDGER[^\r\n]*',log),
                       prefetch=re.findall(r'VT_READY[^\r\n]*',log), picks=re.findall(r'VT_PICK[^\r\n]*',log)))
output.write_text(json.dumps(result, indent=2))
print(json.dumps([dict(leg=row['leg'], status=row['status'], fires=len(row['events']), labelled=row['labelled'], controls=row['controls'], raw_edges=[[event['first_changed']-event['first_label'],event['last_changed']-event['last_label']] for event in row['events']] if all(event['first_changed'] is not None for event in row['events']) else None) for row in result], indent=2))
