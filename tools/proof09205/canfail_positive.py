import contextlib
import hashlib
import json
import os
import runpy
import shutil
import sys
from pathlib import Path
import numpy as np
from PIL import Image

sys.dont_write_bytecode = True
w = Path('E:/IA_BuildCache/_r921/09205')
source_label = sys.argv[1]
minimum = int(sys.argv[2]) if len(sys.argv) > 2 else 5
source = w / 'runs' / source_label
good = json.loads((source / 'positive-result.json').read_text())
assert good['status'] == 'PASS'
session = Path(good['session'])
rows = [json.loads(line) for line in (session / 'labels.jsonl').read_text().splitlines()]
first = next(row for row in rows if any(a.get('labelled') for a in row.get('anomalies', [])))
labelled = set(i for e in good['events'] for i in e['frames'])
restore = next(row for row in rows if row['session_index'] > good['events'][0]['frames'][-1] + 2 and row['session_index'] not in labelled and row['session_index'] not in good['end_settle'])
results = []
for mode, wanted in [('annotation', 'label rule mismatch'), ('transition_annotation', 'annotation mismatch'), ('zero_mask', 'mask or bbox mismatch'), ('co_user', 'control pixels changed'), ('revert', 'target revert residue')]:
    label = source_label + '-reader-mutant-v2-' + mode
    root = w / 'runs' / label
    assert not root.exists()
    destination = root / 'out' / session.name
    destination.mkdir(parents=True)
    created = []
    for original in session.rglob('*'):
        if not original.is_file():
            continue
        target = destination / original.relative_to(session)
        target.parent.mkdir(parents=True, exist_ok=True)
        if original.suffix.lower() in ('.png', '.jpg', '.jpeg'):
            os.link(original, target)
        else:
            shutil.copy2(original, target)
        created.append(str(target))
    shutil.copy2(source / 'game.log', root / 'game.log')
    (root / 'created.json').write_text(json.dumps(created, indent=2))
    mutated_rows = json.loads(json.dumps(rows))
    if mode == 'annotation':
        row = next(row for row in mutated_rows if row['session_index'] == first['session_index'])
        next(a for a in row['anomalies'] if a.get('labelled'))['labelled'] = False
        (destination / 'labels.jsonl').write_text(''.join(json.dumps(r, separators=(',', ':')) + '\n' for r in mutated_rows))
    if mode == 'transition_annotation':
        metadata = destination / 'annotation.json'
        annotation = json.loads(metadata.read_text())
        assert annotation['anomalies'][0]['transition_frames']
        annotation['anomalies'][0]['transition_frames'][0] += 1
        metadata.write_text(json.dumps(annotation, indent=2))
    selected = restore if mode == 'revert' else first
    image_path = destination / selected['image']
    mask_path = destination / first['mask_file']
    tracked = session / selected['image']
    before = hashlib.sha256(tracked.read_bytes()).hexdigest()
    original_open = Image.open
    def injected_open(path, *args, **kwargs):
        result = original_open(path, *args, **kwargs)
        if not isinstance(path, (str, os.PathLike)):
            return result
        if mode == 'zero_mask' and Path(path) == mask_path:
            size = result.size
            result.close()
            return Image.new('L', size, 0)
        if mode in ('co_user', 'revert') and Path(path) == image_path:
            array = np.array(result.convert('RGB'), dtype=np.int16)
            result.close()
            x0, y0, x1, y1 = good['boxes']['co_user' if mode == 'co_user' else 'target']
            array[y0:y1, x0:x1] = np.minimum(255, array[y0:y1, x0:x1] + 32)
            return Image.fromarray(array.astype(np.uint8))
        return result
    Image.open = injected_open
    saved_argv = sys.argv
    code = 0
    try:
        sys.argv = [str(w / 'check_stack_positive.py'), label, str(minimum)]
        with (root / 'checker.log').open('w') as output, contextlib.redirect_stdout(output):
            try:
                runpy.run_path(str(w / 'check_stack_positive.py'), run_name='__main__')
            except SystemExit as error:
                code = error.code
    finally:
        Image.open = original_open
        sys.argv = saved_argv
    assert hashlib.sha256(tracked.read_bytes()).hexdigest() == before
    result = json.loads((root / 'positive-result.json').read_text())
    caught = code == 1 and result['status'] == 'FAIL' and wanted in result['issues']
    record = dict(mode=mode, mutation_frame=selected['session_index'], expected_issue=wanted, caught=caught,
                  exit=code, issues=result['issues'], original_image_unchanged_sha256=before,
                  method='metadata copy' if mode in ('annotation', 'transition_annotation') else 'in-memory Image.open substitution; image and mask files untouched',
                  checker_sha256=hashlib.sha256((w / 'check_stack_positive.py').read_bytes()).hexdigest())
    (root / 'can-fail.json').write_text(json.dumps(record, indent=2))
    results.append(record)
    (w / 'positive-reader-can-fails-v2.json').write_text(json.dumps(results, indent=2))
    print(json.dumps(record), flush=True)
    assert caught, record
