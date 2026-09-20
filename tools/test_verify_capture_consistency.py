"""079-09 batch/region regressions. Run alongside verify_capture.py --selftest.

The batch preflight is stubbed here because the complete image-fixture selftest is
run separately; batch tests exercise real sessions, report files and exit codes.
"""

import contextlib
import io
import pathlib
import tempfile
import unittest
from unittest.mock import patch

from PIL import Image
import verify_capture as vc


class ConsistencyContracts(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(prefix="m079_contracts_")
        self.addCleanup(self.tmp.cleanup)
        self.root = pathlib.Path(self.tmp.name)
        self.log = io.StringIO()

    def batch(self, source, name, report_only=False):
        output = self.root / name
        with patch.object(vc, "_label_pixel_selftest", return_value=0), contextlib.redirect_stdout(self.log):
            code = vc.label_pixel_batch(str(source), str(output), 8, 4, 1, False, report_only)
        return code, output

    def test_flattened_path_collision_keeps_both_reports(self):
        source = self.root / "bank"
        vc._synth_session(str(source), "a__b/session_same", with_mask=True)
        vc._synth_session(str(source), "a/b__session_same", with_mask=True, blank_region=True)
        code, output = self.batch(source, "reports")
        self.assertEqual(code, 2)
        reports = [p for p in output.glob("*.txt") if p.name != "SUMMARY.txt"]
        self.assertEqual(len(reports), 2)
        text = [p.read_text(encoding="utf-8") for p in reports]
        self.assertEqual(sum("VERDICT                  NO FAILURE FOUND (1 consistent" in s for s in text), 1)
        self.assertEqual(sum("VERDICT                  FAIL: NO-TRACE" in s for s in text), 1)
        self.assertEqual(self.batch(source, "report_only", True)[0], 0)

    def test_forced_key_collision_fails_before_writing_reports(self):
        source = self.root / "bank"
        vc._synth_session(str(source), "a/session_same", with_mask=True)
        vc._synth_session(str(source), "b/session_same", with_mask=True)
        with patch.object(vc, "_batch_key", side_effect=lambda _r, s: ("collision", s)):
            code, output = self.batch(source, "reports", True)
        self.assertEqual(code, 3)
        self.assertEqual(list(output.glob("*.txt")), [])
        self.assertIn("REFUSING TO RUN", self.log.getvalue())

    def test_execution_failure_survives_report_only_and_keeps_other_readings(self):
        source = self.root / "bank"
        vc._synth_session(str(source), "ok/session_same", with_mask=True)
        vc._synth_session(str(source), "negative/session_same", with_mask=True, blank_region=True)
        missing = pathlib.Path(vc._synth_session(str(source), "missing/session_same", with_mask=True))
        (missing / "labels.jsonl").unlink()
        code, output = self.batch(source, "reports", True)
        self.assertEqual(code, 3)
        self.assertIn("CANNOT RUN", self.log.getvalue())
        self.assertEqual(len([p for p in output.glob("*.txt") if p.name != "SUMMARY.txt"]), 2)

    def test_union_retains_pixels_only_in_previous_mask(self):
        source = pathlib.Path(vc._codex08_fixture(self.root, "moving_mask"))
        # One pixel leaves the current silhouette. Its change must remain in the union.
        for k, x in ((59, 40), (60, 41)):
            mask = Image.new("L", (180, 120))
            mask.putpixel((x, 30), 222)
            mask.save(source / "target_mask" / ("frame_%05d.png" % k))
            rgb = Image.new("RGB", (180, 120), (90, 90, 90))
            if k == 59:
                rgb.putpixel((40, 30), (255, 255, 255))
            rgb.save(source / "Actual_Frames" / ("frame_%05d.png" % k))
        mlo = vc._offset_module()
        rows, _state = mlo.read_labels(str(source / "labels.jsonl"))
        paths = vc._frame_paths(str(source), mlo)
        hot = vc._HotCache(mlo.FrameCache(paths, 0, limit=8), 8)
        entry = mlo.match_label_entry(rows[60], "SynthTarget", None)
        signal = vc._PairSignal(str(source), rows, "SynthTarget", entry, "mask", hot,
                                paths, (180, 120), mlo, 1)
        pair = signal.pair(60)
        self.assertIsNone(pair["error"])
        self.assertEqual(pair["region"]["npix"], 2)
        self.assertEqual(pair["d"], 0.5)


if __name__ == "__main__":
    unittest.main(verbosity=2)
