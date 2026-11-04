import copy
import datetime
import json
from pathlib import Path
import sys
import unittest

from runtime_exit import classify_exit, stop_with_intent
from runtime_evidence import outcome


class ExitTests(unittest.TestCase):
    def execute_stop(self, running=True, stop_error=False, reason='session limit'):
        receipt = dict(started_utc='2026-01-01T00:00:00+00:00', launches=1,
                       pid=123, result='SESSION_STOP', stop_intents=[])
        persisted = []
        def stop():
            self.assertEqual(len(persisted), 1)
            self.assertEqual(persisted[0]['stop_intents'][0]['pid'], 123)
            self.assertEqual(persisted[0]['stop_intents'][0]['reason'], reason)
            if stop_error:
                raise OSError('synthetic denied stop')
            return [dict(pid=123)]
        try:
            receipt['stopped_owned_processes'] = stop_with_intent(
                receipt, 123, reason, running,
                lambda value: persisted.append(copy.deepcopy(value)), stop)
        except OSError:
            pass
        receipt.update(exit_code=125, root_ended_utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
        return receipt

    def test_intent_precedes_stop_and_classification(self):
        row = self.execute_stop()
        self.assertEqual(classify_exit(row), 'owned-stop')
        passed = dict(result='PASS')
        result = outcome('Load', row, passed, passed, passed)
        self.assertTrue(result['deliberate_owned_stop'])
        self.assertFalse(result['crashed'])

    def test_failed_stop_still_retains_intent(self):
        row = self.execute_stop(stop_error=True)
        self.assertNotIn('stopped_owned_processes', row)
        self.assertEqual(classify_exit(row), 'owned-stop')

    def test_empty_reason_is_owned_stop(self):
        row = self.execute_stop(reason='')
        self.assertEqual(row['stop_intents'][0]['reason'], '')
        self.assertEqual(classify_exit(row), 'owned-stop')
        passed = dict(result='PASS')
        result = outcome('Load', row, passed, passed, passed)
        self.assertTrue(result['deliberate_owned_stop'])
        self.assertFalse(result['crashed'])

    def test_empty_reason_cannot_replace_missing_or_stale_intent(self):
        row = self.execute_stop(reason='')
        row['stop_intents'][0]['pid'] = 456
        self.assertEqual(classify_exit(row), 'crash')
        row['stop_intents'] = []
        self.assertEqual(classify_exit(row), 'crash')

    def test_genuine_crash_and_cleanup(self):
        row = self.execute_stop(running=False)
        self.assertEqual(classify_exit(row), 'crash')
        row['stop_intents'] = []
        self.assertEqual(classify_exit(row), 'crash')

    def test_negative_missing_intent(self):
        row = self.execute_stop()
        row['stop_intents'] = []
        self.assertEqual(classify_exit(row), 'crash')

    def test_wrong_pid_and_late_intent(self):
        for mutation in ('pid', 'utc'):
            row = self.execute_stop()
            row['stop_intents'][0][mutation] = 456 if mutation == 'pid' else '2999-01-01T00:00:00+00:00'
            self.assertEqual(classify_exit(row), 'crash')

    def test_no_stop_if_intent_cannot_be_saved(self):
        called = []
        def fail(value):
            raise OSError('synthetic receipt write failure')
        with self.assertRaises(OSError):
            stop_with_intent({}, 123, 'limit', True, fail, lambda: called.append(True))
        self.assertEqual(called, [])

    def test_pending_stop_and_normal_exit(self):
        row = self.execute_stop()
        row['exit_code'] = None
        self.assertEqual(classify_exit(row), 'stop-pending')
        row.update(exit_code=0, stop_intents=[])
        self.assertEqual(classify_exit(row), 'normal')


if __name__ == '__main__':
    result = unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(ExitTests))
    if len(sys.argv) > 1:
        Path(sys.argv[1]).write_text(json.dumps(dict(result='PASS' if result.wasSuccessful() else 'FAIL',
                                                  tests=result.testsRun, failures=len(result.failures), errors=len(result.errors)), indent=2))
    raise SystemExit(not result.wasSuccessful())
