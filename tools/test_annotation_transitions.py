import copy
import json
from pathlib import Path
import tempfile
import unittest

from annotation_transition_check import check_session


class TransitionTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        self.addCleanup(self.temp.cleanup)
        reasons = ['temporal_aa', 'partial', 'hide_return', 'pie_end_settle', 'effect_interrupted',
                   'nanite_unmaskable', 'capture_unpaired', 'camera_clipping_unconfirmed', 'unresolved']
        self.rows = []
        counts = {}
        for i, reason in enumerate(reasons):
            counts[reason] = 1
            self.rows.append(dict(session_index=i, anomalies=[dict(id='stuck_low_mip', target_name='Target',
                start_frame=10, event_id='stuck_low_mip@10|Target', transition=1, transition_reason=[reason], labelled=i < 2)]))
        self.rows.append(dict(session_index=10, anomalies=[dict(id='stuck_low_mip', target_name='Target', start_frame=20,
            event_id='stuck_low_mip@20|Target', transition=1, transition_reason=['temporal_aa'], labelled=False)]))
        self.annotation = dict(label_schema=2, label_schema_minor=1, transition_frame_count=10, anomalies=[
            dict(event_id='stuck_low_mip@10|Target', transition_frames=list(range(9)), transition_reasons=counts),
            dict(event_id='stuck_low_mip@20|Target', transition_frames=[10], transition_reasons={'temporal_aa': 1})])

    def check(self, annotation=None, rows=None):
        (self.root / 'annotation.json').write_text(json.dumps(self.annotation if annotation is None else annotation))
        (self.root / 'labels.jsonl').write_text('\n'.join(json.dumps(r) for r in (self.rows if rows is None else rows)))
        return check_session(self.root)[0]

    def test_exact_including_detached_and_every_reason(self):
        self.assertEqual([], self.check())

    def test_doctored_files_fail(self):
        for kind in ('missing', 'extra', 'duplicate', 'unsorted', 'wrong_event', 'reason', 'count', 'event', 'schema', 'old'):
            a = copy.deepcopy(self.annotation)
            e = a['anomalies'][0]
            if kind == 'missing': e['transition_frames'].pop()
            if kind == 'extra': e['transition_frames'].append(123)
            if kind == 'duplicate': e['transition_frames'].append(8)
            if kind == 'unsorted': e['transition_frames'].reverse()
            if kind == 'wrong_event': e['event_id'] = 'stuck_low_mip@30|Target'
            if kind == 'reason': e['transition_reasons']['temporal_aa'] += 1
            if kind == 'count': a['transition_frame_count'] += 1
            if kind == 'event': a['anomalies'].pop()
            if kind == 'schema': a.pop('label_schema_minor')
            if kind == 'old':
                for item in a['anomalies']:
                    item.pop('transition_frames')
                    item.pop('transition_reasons')
            with self.subTest(kind=kind): self.assertTrue(self.check(a))

    def test_session_count_is_union(self):
        self.rows[-1]['session_index'] = 8
        self.annotation['anomalies'][-1]['transition_frames'] = [8]
        self.annotation['transition_frame_count'] = 9
        self.assertEqual([], self.check())

    def test_empty_aa_off(self):
        self.assertEqual([], self.check(dict(label_schema=2, label_schema_minor=1, transition_frame_count=0, anomalies=[]), []))

    def test_bad_labels_fail(self):
        self.rows[-1]['transition_reason'] = []
        self.rows[-1]['anomalies'][0]['event_id'] = 'doctored'
        self.assertTrue(self.check())


if __name__ == '__main__':
    unittest.main()
