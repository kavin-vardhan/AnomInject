# NAME: 079 verifier field validity
# MODEL: claude-opus-5
# EFFORT: high

# 079-04 — chat's ruling on MOTION_CAP, re-prove, freeze for Codex review. Resume session f7ac5221 (62%). Small brief.

## 0. Rulings (chat, 2026-09-15)
1. **MOTION_CAP = 0.040.** P4's two constraints were "largest fully-recovering M_med" (0.0049) and "≥ 2× margin below the first incomplete session" (0.0801/2 = 0.040). Code took the min; chat rules the cap sits at the margin bound, because a cap resting on the single derived value is fragile (0.0001 drift refuses A2L). Provenance line in the header must say exactly that: `cap=0.040 (2x margin below first incomplete recovery, LYRA_SMOKE_01 M_med=0.0801; only full recovery observed at 0.0049, A2L_LEGA; ruling 079-04)`.
2. **No two-tier policy.** A PASS that cannot read back a one-frame shift is not evidence of alignment — it is the client's F1 complaint restated. Above the cap the answer stays `NOT-MEASURABLE(camera motion: M_med=… cap=0.040)`. Record Code's measurement (zero false shifts up to 0.085, recovery incomplete) in the journal as a fact, and in the tool header as the reason there is no middle tier.
3. **The synthetic ladder stays non-binding** (uniform scroll, no parallax) — keep it in selftest as the "gate can still read a shift under motion" proof, state in the header that the real cap is set by real sessions only.
4. Ground-truth correction accepted: §1 of 079-03 was circular; the shift-injection recovery checker `m079_shift_recovery.py` is the known-answer method for this gate from now on. Say so in the predictions file (AMENDMENT 3) and the journal.
5. The CaptureBench stripper incident: add a gotcha ("the comment stripper is plugin-repo-only; never run it in CaptureBench or Lyra — A53-protected files live there") and confirm `git status` of CaptureBench is clean at `472a409` in the report.

## 1. Do
1. AMENDMENT 3 (docs commit): the cap ruling, the no-tier ruling, the ground-truth correction, and the predeclared re-readings below.
2. Set `MOTION_CAP = 0.040` in `verify_capture.py`; update header/provenance; nothing else in the tool changes.
3. Re-run and bank (new subfolder under `M079_VERIFIER_EVIDENCE\`):
   - P1 selftest 17/17 OK.
   - P2 pinned bench ×6: byte-identical verdict lines to 079-03 (PASS 4, tau=0.0040).
   - P3 shift recovery on `A2L_LEGA` (0.0049): full 6/6 ×3 as in 079-03.
   - P5 `M50L_LG9`: unchanged — six refusals, zero NOT-VISIBLE, zero SHIFT.
   - P8 `LYRA_SMOKE_01` (0.0801) and `A1L_LEGA` (0.085): every event `NOT-MEASURABLE(camera motion …)` — zero PASS, zero SHIFT, zero NOT-VISIBLE.
   - P6 batch counts identical.
4. Docs: header, `client-readme.md` Step 6 (plain words: the tool gives verdicts only when the camera is nearly still; under gameplay motion it reports can't-measure with the motion number; M3 datasets carry per-frame visibility in the labels themselves), journal `2026-09-15-079-04-…`, gotcha per §0.5.
5. Commit(s) on `fix/verifier-field-validity`, pushed. **No merge.** Restore checkout to `m51`.
6. Report: per-P lines, the final header block of the tool verbatim, branch SHA, CaptureBench status, `VERDICT: GREEN` if P1/P2/P3/P5/P6/P8 read as predeclared.
