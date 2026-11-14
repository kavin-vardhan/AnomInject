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

    def settle_fixture(self):
        rows = copy.deepcopy(self.rows)
        rows.append(dict(session_index=11, transition_present=True, transition_reason=['settle'], anomalies=[
            dict(id='stuck_low_mip', target_name='Target', start_frame=20, event_id='stuck_low_mip@20|Target',
                 transition=1, transition_reason=['settle'], labelled=True)]))
        rows.append(dict(session_index=12, transition_present=True, transition_reason=['settle'], anomalies=[]))
        a = copy.deepcopy(self.annotation)
        a['label_schema_minor'] = 2
        a['anomalies'][1]['transition_frames'] = [10, 11]
        a['anomalies'][1]['transition_reasons'] = {'temporal_aa': 1, 'settle': 1}
        a['transition_frame_count'] = 12
        a['transition_frames'] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12]
        return a, rows

    def test_schema_2_2_settle_and_settle_only_row(self):
        a, rows = self.settle_fixture()
        self.assertEqual([], self.check(a, rows))

    def test_schema_2_2_doctored_fail(self):
        for kind in ('settle_only_row_not_counted', 'settle_frame_not_listed', 'settle_reason_count', 'minor_1', 'minor_4',
                     'root_list_missing', 'root_list_without_settle_only_row', 'root_list_unsorted'):
            a, rows = self.settle_fixture()
            if kind == 'root_list_missing': a.pop('transition_frames')
            if kind == 'root_list_without_settle_only_row': a['transition_frames'].remove(12)
            if kind == 'root_list_unsorted': a['transition_frames'].reverse()
            if kind == 'settle_only_row_not_counted': a['transition_frame_count'] = 11
            if kind == 'settle_frame_not_listed': a['anomalies'][1]['transition_frames'] = [10]
            if kind == 'settle_reason_count': a['anomalies'][1]['transition_reasons'].pop('settle')
            if kind == 'minor_1': a['label_schema_minor'] = 1
            if kind == 'minor_4': a['label_schema_minor'] = 4
            with self.subTest(kind=kind): self.assertTrue(self.check(a, rows))

    def summary(self, **fields):
        (self.root / 'run_summary.json').write_text(json.dumps(fields))

    def vetoed_fixture(self):
        a, rows = self.settle_fixture()
        rows[9]['anomalies'][0]['transition_reason'] = ['settle']
        a['anomalies'].pop()
        return a, rows

    def test_vetoed_event_excused_only_by_its_listed_id(self):
        a, rows = self.vetoed_fixture()
        vid = 'stuck_low_mip@20|Target'
        self.assertTrue(self.check(a, rows))
        self.summary(vetoed_events=1)
        self.assertTrue(self.check(a, rows))
        self.summary(vetoed_events=1, vetoed_event_ids=['stuck_low_mip@10|Target'])
        self.assertTrue(self.check(a, rows))
        self.summary(vetoed_events=1, vetoed_event_ids=[vid])
        self.assertEqual([], self.check(a, rows))
        self.summary(vetoed_events=0, vetoed_event_ids=[vid])
        self.assertEqual([], self.check(a, rows))

    def test_schema_2_3_reads_like_2_2(self):
        a, rows = self.settle_fixture()
        a['label_schema_minor'] = 3
        self.assertEqual([], self.check(a, rows))

    def lod_untriggered_fixture(self, reasons):
        a, rows = self.settle_fixture()
        a['label_schema_minor'] = 3
        lid = 'lod_popping@30|Crate'
        rows.append(dict(session_index=13, transition_present=True, transition_reason=['settle'], anomalies=[
            dict(id='lod_popping', target_name='Crate', start_frame=30, event_id=lid, transition=1,
                 transition_reason=list(reasons), labelled=False)]))
        a['transition_frame_count'] = 13
        a['transition_frames'] = a['transition_frames'] + [13]
        return a, rows, lid

    def test_lod_untriggered_orphan_excused_only_by_its_listed_id_on_2_3(self):
        for reasons in (['settle'], ['lod_subvisible'], ['settle', 'lod_subvisible'], ['lod_unconfirmed']):
            with self.subTest(reasons=reasons):
                a, rows, lid = self.lod_untriggered_fixture(reasons)
                self.summary()
                self.assertTrue(self.check(a, rows))
                self.summary(lod_untriggered=1, lod_untriggered_event_ids=['lod_popping@31|Crate'])
                self.assertTrue(self.check(a, rows))
                self.summary(lod_untriggered=1, lod_untriggered_event_ids=[lid])
                self.assertEqual([], self.check(a, rows))

    def test_lod_untriggered_orphan_with_another_reason_fails(self):
        a, rows, lid = self.lod_untriggered_fixture(['temporal_aa'])
        self.summary(lod_untriggered=1, lod_untriggered_event_ids=[lid])
        self.assertTrue(self.check(a, rows))

    def test_lod_untriggered_orphan_not_excused_on_2_2(self):
        a, rows, lid = self.lod_untriggered_fixture(['settle'])
        a['label_schema_minor'] = 2
        self.summary(lod_untriggered=1, lod_untriggered_event_ids=[lid])
        self.assertTrue(self.check(a, rows))

    def test_vetoed_id_with_a_non_settle_reason_fails(self):
        a, rows = self.vetoed_fixture()
        rows[9]['anomalies'][0]['transition_reason'] = ['temporal_aa']
        self.summary(vetoed_events=1, vetoed_event_ids=['stuck_low_mip@20|Target'])
        self.assertTrue(self.check(a, rows))

    def test_schema_2_1_is_judged_as_before_m56(self):
        rows = copy.deepcopy(self.rows)
        a = copy.deepcopy(self.annotation)
        a['anomalies'].pop()
        self.summary(vetoed_events=1, vetoed_event_ids=['stuck_low_mip@20|Target'])
        self.assertTrue(self.check(a, rows))
        self.summary(vetoed_events=1)
        self.assertTrue(self.check(a, rows))

    def test_lost_event_beside_a_vetoed_one_fails(self):
        a, rows = self.vetoed_fixture()
        a['anomalies'].pop()
        self.summary(vetoed_events=1, vetoed_event_ids=['stuck_low_mip@20|Target'])
        errors = self.check(a, rows)
        self.assertTrue(any('stuck_low_mip@10|Target' in e for e in errors))


if __name__ == '__main__':
    unittest.main()
