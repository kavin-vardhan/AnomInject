import json
from pathlib import Path
import sys

w = Path('E:/IA_BuildCache/_r921/09203c')
record = []
for root in sorted((w / 'runs').iterdir()):
    path = root / 'launch.json'
    if not path.is_file():
        continue
    launch = json.loads(path.read_text())
    row = {key: launch.get(key) for key in ('kind', 'pid', 'exit', 'started', 'finished')}
    row['name'] = root.name
    row['source_commit'] = launch.get('archive', {}).get('source_commit')
    row['guard_reason'] = launch.get('runtime_guard', {}).get('reason')
    row['checks'] = {}
    for name in ('identity-result', 'native-result', 'positive-result', 'refusal-result', 'lifetime-result', 'world-end-result', 'regression-result'):
        result_path = root / (name + '.json')
        if result_path.is_file():
            value = json.loads(result_path.read_text())
            row['checks'][name] = {key: value[key] for key in ('status', 'issues', 'frames', 'frame_count', 'events', 'labelled_frames') if key in value}
    row['sessions'] = []
    for session in sorted((root / 'out').glob('session_*')):
        labels = session / 'labels.jsonl'
        entries = [json.loads(line) for line in labels.read_text().splitlines() if line.strip()] if labels.is_file() else []
        indices = [entry['session_index'] for entry in entries]
        annotation_path = session / 'annotation.json'
        annotation = json.loads(annotation_path.read_text()) if annotation_path.is_file() else {}
        row['sessions'].append(dict(path=str(session), frames=len(entries), first=min(indices, default=None), last=max(indices, default=None),
                                    labelled=sum(bool(entry.get('anomaly_present')) for entry in entries), events=len(annotation.get('anomalies', []))))
    record.append(row)
output = w / (sys.argv[1] + '.json')
assert not output.exists()
output.write_text(json.dumps(record, indent=2))
print(json.dumps(dict(output=str(output), runs=len(record))))
