import argparse
from collections import Counter, defaultdict
import json
from pathlib import Path


def check_session(directory, require_schema=True):
    root = Path(directory)
    errors = []
    try:
        annotation = json.loads((root / 'annotation.json').read_text(encoding='utf-8-sig'))
        if not require_schema and 'label_schema_minor' not in annotation and not any(
                'transition_frames' in e for e in annotation.get('anomalies', [])):
            return [], {'legacy': True}
        if annotation.get('label_schema') != 2 or annotation.get('label_schema_minor') != 1:
            errors.append('expected annotation schema 2.1')
        expected_frames = defaultdict(set)
        expected_reasons = defaultdict(Counter)
        union = set()
        with (root / 'labels.jsonl').open(encoding='utf-8-sig') as stream:
            for line in stream:
                if not line.strip():
                    continue
                row = json.loads(line)
                si = row['session_index']
                if type(si) is not int:
                    raise ValueError('non-integer session index')
                for entry in row.get('anomalies', []):
                    key = '%s@%d|%s' % (entry['id'], entry['start_frame'], entry['target_name'])
                    if entry.get('event_id') != key:
                        errors.append('labels event_id mismatch at %d' % si)
                    if entry.get('transition') != 1:
                        continue
                    reasons = entry.get('transition_reason')
                    if not isinstance(reasons, list) or not reasons or not all(isinstance(r, str) for r in reasons):
                        raise ValueError('invalid transition reasons at %d' % si)
                    expected_frames[key].add(si)
                    expected_reasons[key].update(reasons)
                    union.add(si)
        seen = set()
        for event in annotation['anomalies']:
            key = event.get('event_id')
            if not isinstance(key, str) or not key or key in seen:
                errors.append('missing or duplicate annotation event_id: %r' % key)
                continue
            seen.add(key)
            actual = event.get('transition_frames')
            if not isinstance(actual, list) or not all(type(i) is int for i in actual) or actual != sorted(expected_frames[key]):
                errors.append('transition_frames mismatch: %s' % key)
            reasons = event.get('transition_reasons')
            if not isinstance(reasons, dict) or not all(type(n) is int and n > 0 for n in reasons.values()) or reasons != dict(expected_reasons[key]):
                errors.append('transition_reasons mismatch: %s' % key)
        for key in expected_frames.keys() - seen:
            errors.append('transition event missing from annotation: %s' % key)
        count = annotation.get('transition_frame_count')
        if type(count) is not int or count != len(union):
            errors.append('transition_frame_count mismatch')
        return errors, dict(legacy=False, transition_frame_count=len(union), transition_events=sum(bool(f) for f in expected_frames.values()))
    except (OSError, ValueError, KeyError, TypeError) as exc:
        return errors + ['cannot compare transitions: %s' % exc], {}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--dir', required=True)
    args = parser.parse_args()
    errors, detail = check_session(args.dir)
    print(json.dumps(dict(errors=errors, **detail), indent=2))
    return 1 if errors else 0


if __name__ == '__main__':
    raise SystemExit(main())
