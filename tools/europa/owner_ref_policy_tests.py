import json
from pathlib import Path
import subprocess
import sys
import unittest

import owner_ref_policy as policy


class RefTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        root = Path(__file__).resolve().parents[2]
        cls.git_dir = subprocess.check_output(['git', '-C', str(root), 'rev-parse', '--git-common-dir'], text=True).strip()
        cls.baseline = json.loads(Path('D:/IA_Europa/Receipts/P9/baseline/git-state.json').read_text())['refs']
        cls.current = subprocess.check_output(['git', '--git-dir='+cls.git_dir, 'for-each-ref', '--format=%(refname) %(objectname)'], text=True).splitlines()

    def validate(self, current):
        return policy.validate_refs(self.baseline, current, lambda a, b: policy.git_ancestor(self.git_dir, a, b))

    def moved(self, name, sha):
        return [line if not line.startswith(name+' ') else name+' '+sha for line in self.current]

    def test_current_state(self):
        result = self.validate(self.current)
        self.assertEqual(result['baseline_refs'], 71)
        self.assertEqual(len(result['admitted']), 4)

    def test_unknown_extra(self):
        with self.assertRaisesRegex(RuntimeError, 'Unknown extra ref'):
            self.validate(self.current+['refs/heads/unrecorded-extra '+policy.SPIKE42_SHA])

    def test_moved_backup(self):
        with self.assertRaisesRegex(RuntimeError, 'violates GE1'):
            self.validate(self.moved(policy.BACKUP, policy.SPIKE42_SHA))

    def test_moved_42c(self):
        with self.assertRaisesRegex(RuntimeError, 'violates GE1'):
            self.validate(self.moved(policy.SPIKE42, policy.SPIKE43_BASE))

    def test_moved_baseline(self):
        name = self.baseline[0].split(' ', 1)[0]
        with self.assertRaisesRegex(RuntimeError, 'Missing or moved baseline'):
            self.validate(self.moved(name, policy.SPIKE43_BASE))

    def test_missing_baseline(self):
        with self.assertRaisesRegex(RuntimeError, 'Missing or moved baseline'):
            self.validate([line for line in self.current if line != self.baseline[0]])

    def test_origin_not_ancestor(self):
        with self.assertRaisesRegex(RuntimeError, 'violates GE1'):
            self.validate(self.moved(policy.ORIGIN, policy.SPIKE43_BASE))

    def test_43a_not_descendant(self):
        with self.assertRaisesRegex(RuntimeError, 'violates GE1'):
            self.validate(self.moved(policy.SPIKE43, policy.SPIKE42_SHA))


if __name__ == '__main__':
    result = unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(RefTests))
    if len(sys.argv) > 1:
        Path(sys.argv[1]).write_text(json.dumps(dict(result='PASS' if result.wasSuccessful() else 'FAIL',
            tests=result.testsRun, failures=len(result.failures), errors=len(result.errors)), indent=2))
    raise SystemExit(not result.wasSuccessful())
