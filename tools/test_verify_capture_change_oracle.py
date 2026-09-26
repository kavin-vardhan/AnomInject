"""m55 Stage 3 change-oracle contracts: printed scope sentence, exit codes and independence."""

import contextlib
import io
import json
import pathlib
import sys
import tempfile
import unittest
from unittest.mock import patch

import verify_capture as vc

SPEC_SENTENCE = ("agreement validates arithmetic and transport only — that the numbers in the sidecar "
                 "are the numbers the delivered images contain. It does not establish renderer pairing, "
                 "visible effect, or cause.")


def run_main(argv):
    out = io.StringIO()
    code = None
    with patch.object(sys, "argv", ["verify_capture.py"] + argv), contextlib.redirect_stdout(out), \
            contextlib.redirect_stderr(io.StringIO()):
        try:
            vc.main()
            code = 0
        except SystemExit as exc:
            code = exc.code if isinstance(exc.code, int) else (0 if exc.code is None else 1)
    return code, out.getvalue()


class ChangeOracleContracts(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(prefix="m55_oracle_contracts_")
        self.addCleanup(self.tmp.cleanup)
        self.root = pathlib.Path(self.tmp.name)

    def test_selftest_is_green(self):
        with contextlib.redirect_stdout(io.StringIO()) as buf:
            self.assertEqual(vc._change_oracle_selftest(), 0, buf.getvalue())
        self.assertIn("SELFTEST: OK", buf.getvalue())

    def test_selftest_cli_entry(self):
        code, text = run_main(["--change-oracle", "--selftest"])
        self.assertEqual(code, 0)
        self.assertIn("change-oracle cases", text)

    def test_sentence_is_the_spec_sentence_verbatim(self):
        self.assertEqual(vc.CHANGE_ORACLE_SENTENCE, SPEC_SENTENCE)
        session = vc._oracle_baseline(str(self.root))
        code, text = run_main(["--change-oracle", session])
        self.assertEqual(code, 0)
        self.assertIn(SPEC_SENTENCE, text.splitlines())

    def test_sentence_printed_when_it_cannot_run(self):
        empty = self.root / "empty"
        empty.mkdir()
        code, text = run_main(["--change-oracle", str(empty)])
        self.assertEqual(code, 3)
        self.assertIn("CANNOT RUN", text)
        self.assertIn(SPEC_SENTENCE, text.splitlines())

    def test_oracle_exit_code_is_1_on_a_mismatch_and_0_when_all_match(self):
        good = vc._oracle_baseline(str(self.root), "good")

        def corrupt(_f, _m, rows):
            for r in rows:
                r["chg_gt8"] += 1
        bad = vc._oracle_baseline(str(self.root), "bad", mutate=corrupt)
        code_good, text_good = run_main(["--change-oracle", good])
        code_bad, text_bad = run_main(["--change-oracle", bad])
        self.assertEqual((code_good, code_bad), (0, 1))
        self.assertIn("mismatched             0", text_good)
        self.assertIn("mismatched             4", text_bad)
        for word in vc.RUN_OUTCOMES:
            self.assertNotIn(" %s " % word, text_bad)

    def test_dir_flag_and_bank_folder_resolution(self):
        session = pathlib.Path(vc._oracle_baseline(str(self.root / "LEG_A1"), "session_x"))
        code, text = run_main(["--dir", str(session), "--change-oracle"])
        self.assertEqual(code, 0)
        code, text = run_main(["--change-oracle", str(session.parent)])
        self.assertEqual(code, 0)
        self.assertIn("resolved to its only session folder session_x", text)

    def test_json_detail_written(self):
        session = vc._oracle_baseline(str(self.root))
        out = self.root / "detail.json"
        code, _text = run_main(["--change-oracle", session, "--oracle-json", str(out)])
        self.assertEqual(code, 0)
        detail = json.loads(out.read_text(encoding="utf-8"))
        self.assertEqual(detail["summary"]["matched"], 4)
        self.assertEqual(len(detail["rows"]), 4)

    def test_published_values_never_feed_the_recomputation(self):
        clean = vc._oracle_baseline(str(self.root), "clean")

        def scramble(_f, _m, rows):
            for r in rows:
                r["chg_n"], r["ctl_n"] = 1, 1
                r["chg_hist"] = r["ctl_hist"] = [0] * 8
                r["chg_mean"] = r["ctl_mean"] = r["ref_mean"] = 0.5
                r["ref_gt8"] = 0
        scrambled = vc._oracle_baseline(str(self.root), "scrambled", mutate=scramble)
        _c, _l, a = vc.change_oracle(clean, quiet=True)
        _c, _l, b = vc.change_oracle(scrambled, quiet=True)
        self.assertEqual([r["recomputed"] for r in a["rows"]], [r["recomputed"] for r in b["rows"]])
        self.assertEqual(b["summary"]["mismatched"], 4)

    def _sidecar(self, name, edit):
        session = pathlib.Path(vc._oracle_baseline(str(self.root), name))
        path = session / vc.CHANGE_ORACLE_SIDECAR
        lines = [x for x in path.read_text(encoding="utf-8").splitlines() if x.strip()]
        path.write_text("".join(x + "\n" for x in edit(lines)), encoding="utf-8")
        return str(session)

    def _run_no_traceback(self, session):
        err = io.StringIO()
        out = io.StringIO()
        with patch.object(sys, "argv", ["verify_capture.py", "--change-oracle", session, "--quiet"]), \
                contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            with self.assertRaises(SystemExit) as ctx:
                vc.main()
        self.assertNotIn("Traceback", out.getvalue() + err.getvalue())
        self.assertIn(SPEC_SENTENCE, out.getvalue().splitlines())
        return ctx.exception.code, out.getvalue()

    def test_f3_malformed_only_exits_3(self):
        code, text = self._run_no_traceback(self._sidecar("malformed_only", lambda _l: ["{bad json"]))
        self.assertEqual(code, 3)
        self.assertIn("UNINTERPRETABLE line 1", text)
        self.assertIn("mismatched 0", text)
        self.assertIn("uninterpretable 1", text)

    def test_f3_valid_plus_malformed_exits_3(self):
        code, text = self._run_no_traceback(self._sidecar("valid_malformed", lambda l: l + ["{bad json"]))
        self.assertEqual(code, 3)
        self.assertIn("matched 4", text)
        self.assertIn("UNINTERPRETABLE line 5", text)

    def test_f3_mismatching_plus_malformed_exits_1(self):
        def edit(lines):
            row = json.loads(lines[0])
            row["chg_gt8"] += 1
            return [json.dumps(row)] + lines[1:] + ["{bad json"]
        code, text = self._run_no_traceback(self._sidecar("mismatch_malformed", edit))
        self.assertEqual(code, 1)
        self.assertIn("mismatched 1", text)
        self.assertIn("uninterpretable 1", text)

    def test_f3_receipt_list_with_item_exits_3(self):
        def edit(lines):
            row = json.loads(lines[1])
            row["receipt"] = [1]
            return [lines[0], json.dumps(row)] + lines[2:]
        code, text = self._run_no_traceback(self._sidecar("receipt_item", edit))
        self.assertEqual(code, 3)
        self.assertIn("UNINTERPRETABLE line 2 si=4: receipt is a JSON array, an object is required", text)

    def test_f3_receipt_empty_list_exits_3(self):
        def edit(lines):
            row = json.loads(lines[1])
            row["receipt"] = []
            return [lines[0], json.dumps(row)] + lines[2:]
        code, _text = self._run_no_traceback(self._sidecar("receipt_empty", edit))
        self.assertEqual(code, 3)

    def test_f3_required_scalar_of_wrong_type_exits_3_and_tau_out_of_range_exits_1(self):
        def edit(key, value):
            def f(lines):
                row = json.loads(lines[1])
                row[key] = value
                return [lines[0], json.dumps(row)] + lines[2:]
            return f
        code, text = self._run_no_traceback(self._sidecar("tau_string", edit("tau_px", "8")))
        self.assertEqual(code, 3)
        self.assertIn("tau_px is a JSON string, an integer is required", text)
        code, _text = self._run_no_traceback(self._sidecar("mask_null", edit("mask_value", None)))
        self.assertEqual(code, 3)
        code, text = self._run_no_traceback(self._sidecar("tau_300", edit("tau_px", 300)))
        self.assertEqual(code, 1)
        self.assertIn("tau_px outside 0..255", text)

    def test_f3_well_formed_session_with_no_comparisons_still_exits_0(self):
        session = vc._oracle_baseline(str(self.root), "nothing", run_summary={"capture_path": "backbuffer"})
        code, text = self._run_no_traceback(session)
        self.assertEqual(code, 0)
        self.assertIn("compared 0", text)

    def test_f1_contradicted_empty_region_exits_1(self):
        def edit(lines):
            row = json.loads(lines[1])
            row.update(chg_measured=False, chg_eligible=False, pair_valid=False, reason="empty_region",
                       chg_n=-1, chg_gt8=-1, chg_sum=-1, chg_hist=None, chg_mean=None, ref_gt8=None, ref_mean=None)
            return [lines[0], json.dumps(row)] + lines[2:]
        code, text = self._run_no_traceback(self._sidecar("empty_region_contradicted", edit))
        self.assertEqual(code, 1)
        self.assertIn("empty_region_disagrees 1", text)
        self.assertIn("DISAGREES si=4", text)

    def test_contradictory_measured_row_exits_1(self):
        def edit(lines):
            row = json.loads(lines[1])
            row.update(chg_eligible=False, pair_valid=False, reason="budget_exceeded", expected_prev_session_index=71)
            return [lines[0], json.dumps(row)] + lines[2:]
        code, text = self._run_no_traceback(self._sidecar("contradicted_row", edit))
        self.assertEqual(code, 1)
        self.assertIn("measured_row:reason", text)
        self.assertIn("measured_row:expected_prev_session_index", text)

    def test_internal_error_is_exit_3_without_traceback(self):
        session = vc._oracle_baseline(str(self.root), "internal")
        with patch.object(vc, "_oracle_read_sidecar", side_effect=RuntimeError("boom")):
            code, text = self._run_no_traceback(session)
        self.assertEqual(code, 3)
        self.assertIn("CANNOT RUN - internal error RuntimeError", text)

    def test_six_existing_cli_cases_ignore_the_oracle(self):
        from PIL import Image
        base = pathlib.Path(vc._synth_session(str(self.root / "bank"), "leg/session_same", with_mask=True,
                                              blank_region=True))
        frame0 = base / "Actual_Frames" / "frame_00000.png"
        with Image.open(frame0) as im:
            size = im.size
        Image.new("RGB", size, (0, 0, 0)).save(frame0)

        def variants():
            yield "absent", None
            yield "mismatching", json.dumps({"kind": "pair", "stage_version": 2, "session_index": 3,
                                             "prev_session_index": 2, "mask_value": 200, "chg_measured": True,
                                             "tau_px": 8, "chg_n": 1, "chg_gt8": 999}) + "\n"
            yield "garbage", "{not json\n"

        def cases(out_dir):
            return [
                ("overlay", ["--dir", str(base), "--out", str(out_dir / "ov"), "--quiet"]),
                ("black_frame_gate", ["--dir", str(base), "--black-frame-gate", "--quiet"]),
                ("label_pixel_gate", ["--dir", str(base), "--label-pixel-gate", "--quiet"]),
                ("all_label_pixel_gate", ["--all", str(self.root / "bank"), "--label-pixel-gate",
                                          "--out", str(out_dir / "batch"), "--quiet"]),
                ("label_pixel_selftest", ["--label-pixel-gate", "--selftest"]),
                ("selftest", ["--selftest"]),
            ]

        def forbidden(*_a, **_k):
            raise AssertionError("an existing CLI case reached the change oracle")

        seen = {}
        with patch.object(vc, "_label_pixel_selftest", return_value=0), \
                patch.object(vc, "change_oracle", side_effect=forbidden), \
                patch.object(vc, "_change_oracle_selftest", side_effect=forbidden):
            for name, text in variants():
                sidecar = base / vc.CHANGE_ORACLE_SIDECAR
                if text is None:
                    if sidecar.exists():
                        sidecar.unlink()
                else:
                    sidecar.write_text(text, encoding="utf-8")
                out_dir = self.root / ("out_" + name)
                seen[name] = {case: run_main(argv)[0] for case, argv in cases(out_dir)}
        self.assertEqual(seen["absent"], seen["mismatching"])
        self.assertEqual(seen["absent"], seen["garbage"])
        self.assertEqual(seen["absent"]["black_frame_gate"], 1, seen)
        self.assertEqual(seen["absent"]["label_pixel_gate"], 2, seen)
        self.assertEqual(seen["absent"]["all_label_pixel_gate"], 2, seen)
        self.assertEqual(set(seen["absent"]), {"overlay", "black_frame_gate", "label_pixel_gate",
                                               "all_label_pixel_gate", "label_pixel_selftest", "selftest"})


if __name__ == "__main__":
    unittest.main()
