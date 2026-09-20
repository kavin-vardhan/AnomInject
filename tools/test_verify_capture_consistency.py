"""079-09/10 batch, per-pair region and boundary-mask regressions.

The batch preflight is stubbed here because the complete image-fixture selftest is
run separately; batch tests exercise real sessions, report files and exit codes.
"""

import contextlib
import io
import json
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

    def signal(self, source, span=(40, 70)):
        mlo = vc._offset_module()
        rows, _state = mlo.read_labels(str(source / "labels.jsonl"))
        paths = vc._frame_paths(str(source), mlo)
        hot = vc._HotCache(mlo.FrameCache(paths, 0, limit=8), 8)
        entry = mlo.match_label_entry(rows[span[0]], "SynthTarget", None)
        return vc._PairSignal(str(source), rows, "SynthTarget", entry, "mask", hot,
                               paths, (180, 120), mlo, 1, span, 4)

    def span_masks(self, name):
        outside = tuple(k for k in range(100) if not 40 <= k <= 70)
        return pathlib.Path(vc._codex08_fixture(self.root, name, true_runs=((40, 70),), missing_masks=outside))

    def batch(self, source, name, report_only=False):
        output = self.root / name
        with patch.object(vc, "_label_pixel_selftest", return_value=0), contextlib.redirect_stdout(self.log):
            code = vc.label_pixel_batch(str(source), str(output), 8, 4, 1, False, report_only, region_cap=1.0)
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

    def test_extrapolation_distance_is_bounded_on_both_sides(self):
        signal = self.signal(self.span_masks("distance"))
        self.assertEqual(signal.mask(12)[3], [(40, -28)])
        self.assertIn("distance 29 > limit 28", signal.mask(11)[1])
        self.assertEqual(signal.mask(98)[3], [(70, 28)])
        self.assertIn("distance 29 > limit 28", signal.mask(99)[1])

    def test_actual_mask_overrides_the_boundary_template(self):
        source = self.span_masks("actual_priority")
        actual = Image.new("L", (180, 120))
        actual.paste(222, (120, 30, 130, 40))
        actual.save(source / "target_mask/frame_00039.png")
        signal = self.signal(source)
        region, error, _unresolved, extra = signal.mask(39)
        self.assertIsNone(error)
        self.assertEqual(region["box"], (120, 30, 130, 40))
        self.assertEqual(extra, [])
        self.assertEqual(signal.mask(38)[3], [(40, -2)])

    def test_unrelated_off_span_entry_does_not_supply_target_identity(self):
        source = self.span_masks("other_target")
        other = Image.new("L", (180, 120))
        other.paste(111, (120, 30, 130, 40))
        other.save(source / "target_mask/frame_00039.png")
        rows = [json.loads(s) for s in (source / "labels.jsonl").read_text(encoding="utf-8").splitlines()]
        rows[39]["anomalies"] = [{"target_name": "OtherTarget", "mask_value": 111}]
        (source / "labels.jsonl").write_text("".join(json.dumps(r) + "\n" for r in rows), encoding="utf-8")
        region, error, _unresolved, extra = self.signal(source).mask(39)
        self.assertIsNone(error)
        self.assertEqual(region["box"], (40, 30, 80, 70))
        self.assertEqual(extra, [(40, -1)])

    def test_unreadable_off_span_mask_is_not_extrapolated(self):
        source = self.span_masks("corrupt_outside")
        (source / "target_mask/frame_00039.png").write_bytes(b"not a PNG")
        region, error, _unresolved, extra = self.signal(source).mask(39)
        self.assertIsNone(region)
        self.assertIn("mask unreadable at frame 39", error)
        self.assertEqual(extra, [])

    def test_boundary_identity_comes_from_the_named_target(self):
        source = self.span_masks("named_anchor")
        rows = [json.loads(s) for s in (source / "labels.jsonl").read_text(encoding="utf-8").splitlines()]
        rows[40]["anomalies"] = [{"target_name": "OtherTarget", "mask_value": 111}]
        (source / "labels.jsonl").write_text("".join(json.dumps(r) + "\n" for r in rows), encoding="utf-8")
        detail = []
        code, _lines = vc.label_pixel_gate(str(source), 8, 4, 1, region_cap=1.0, out_detail=detail)
        self.assertEqual(code, 0)
        self.assertEqual([e["run_outcome"] for e in detail], [vc.R_CONSISTENT, vc.R_CONSISTENT])


if __name__ == "__main__":
    unittest.main(verbosity=2)
