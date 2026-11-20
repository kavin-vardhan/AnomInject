# EFFORT: xhigh
# MODEL: claude-opus-5

# 071-03 — m49 step 2: the G-EDGE leg (8 legs, both tick orders) with the new verifier as the instrument, then cold-start prep for a session roll
Bench legs only. No source edits to Source/ or Shaders/. Headless; nobody answers; FINAL MESSAGE structure in §5; any failed gate = report and STOP, never re-run for green. Codename discipline.

## 0. Rulings carried
071-02 accepted as green (self-test 7/7 both directions, five known-good PASS, two known-bad fire). The verifier is now the instrument for every edge gate. The mask_value-0-vs-PNG defect you reported (071-02 §6.3) is ASSIGNED to Phase A's join — do not fix it here; carry it into the cold-start notes as a Phase A precondition.

## 1. Binary
Master tip should be ae54812 with NO change under Source/ or Shaders/ since m48 (20db6e5). Prove it: `git diff --stat 20db6e5..HEAD -- Source Shaders` must be empty; then the staged bench exe DE65F84A (m48) IS master's binary and no build is needed. If the diff is not empty, STOP and report (do not build). Checkout stays on master.

## 2. Predictions first (house style)
Write `docs/predictions/2026-09-04-m49-g-edge.md` BEFORE any leg: the eight legs, the pass condition, the instrument (verify_capture --label-pixel-gate, gate mode), the refusal rules, and the prediction: 0/0 on every event in all eight legs; the delivered-configuration legs REPORT-ONLY with the expected ~9% cross-run floor (G228). Commit it.

## 3. The legs (journal 071 §3.7, verbatim)
Packaged bench binary DE65F84A, CB_GateLevel, TARGETED capture, {missing_texture, corrupted_texture, missing_object, blinking} × {native, IAI.Bench.SynthTickOrder 1} = 8 legs. Delivery OFF; `IAI.Capture.Config 2 4 8 14 0` (ceiling ±7); marker OFF; AA-off configuration; 90 frames; pose gate on. Per event: ONSET := d(start) > tau AND d(start-1) <= tau; END := d(end+1) > tau AND d(end) <= tau. PASS = 0 shifts / 0 NOT-VISIBLE on every measurable event, all 8 legs. The truncated last event is NOT-MEASURABLE by construction — count it, never fold it into PASS. Run --selftest once before the first leg and quote its exit code.
Then the same 8 legs at the DELIVERED configuration, REPORT-ONLY (no pass/fail; table the readings).
Bank every leg under `_bench_sessions_bank\M49_GEDGE_*`. Quote every event line.

## 4. Cold-start prep (this session is at ~57% context; the next brief opens a fresh default session)
After the legs, regardless of result: (a) journal 071 §12 — leg results + the exact state; (b) CLAUDE.md status block: "you are here" = m49 in progress, step 1 verifier DONE (ae54812), step 2 G-EDGE result, Phase A NEXT with its file list pointer (journal 071 §8.1), the Q3–Q6 rulings from 071-02 §0, the mask_value-0 precondition, checkout on master; (c) a 10-line "COLD START FOR 072" section at the top of the journal naming exactly which docs a fresh session reads first (CLAUDE.md status → journal 071 §8, §11, §12 → predictions m49-g-edge → gotchas tail). Commit docs, push master.

## 5. Final message
1. Plain-language rundown (8 lines).
2. Binary proof (diff empty? exe hash).
3. Gate table: leg | events | PASS | SHIFT | NOT-VIS | NOT-MEAS | exit — 8 gate rows + 8 report-only rows.
4. Verdict: G-EDGE PASS/FAIL; if FAIL, the failing event lines verbatim and STOP.
5. Commits/SHAs, bank paths, checkout state, journal/CLAUDE.md pointers for the cold start.
6. NEEDS-DECISION if any.
