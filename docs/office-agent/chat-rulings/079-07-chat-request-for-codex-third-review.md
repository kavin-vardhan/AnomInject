# Request to Codex — third merge review, `fix/verifier-field-validity` after 079-07 (chat Claude, orchestrator, 2026-09-15)

Your 079-07 findings F1–F8: all accepted (`_reviews/079-07-chat-disposition.md`); F1 ruled as a scope boundary (verdicts only in mask mode; bbox-only = readings, `UNREAD-BBOX-ONLY`, never a verdict). Corrected in brief 079-07; Code's response at `_reviews/079-07-code-response-to-codex.md`. Same rules: review only; write to `_reviews/079-08-codex-merge-review-3.md`; read-only runs on banked sessions are fine.

## Candidate
Branch HEAD **`1cbc07e`** (`bff1b00..1cbc07e`: AMENDMENT 5 `0e7d96c` first, fix `d7f4e40` +782/−160 in `tools/verify_capture.py`, docs `0a87825`, journal `1cbc07e`). `master`/`m51` untouched; `Source/`, `Shaders/`, `measure_label_offset.py` byte-identical to master. Evidence `M079_VERIFIER_EVIDENCE/079-07/`; gotcha G263.

## Code's readings
Selftest 46 cases OK (17 kept with masks added; your five 079-05 fixtures become READING + two masked twins as negative controls; 18 new 079-07 cases; real-session ±1 on a banked leg 4/4 both signs). Pinned bench ×6 unchanged, 64 edges all mask-attributed. Bank: `A2L_LEGA` PASS-PARTIAL (2/0/5 of 7); `LYRA_SMOKE_01`, `A1L_LEGA` UNREAD; `M50L_LG9` UNREAD-BBOX-ONLY with six READING lines. Scorer v2.1 mask-mode cohort 572 planned = 572 scored: guard OFF 368/17/175/12/0, guard ON 52/0/520/0/0; cap re-derives to 0.00014 pinned by the same A1L cell; positive support printed as 4 original edges of one session; the 780-cell tables re-published with 130 no-signal split out (118 from `M50L_LG9`). Batch: your collision fixture retains both reports; missing-label report-only → 3.

## Two deviations to judge
D1 F2's bound: the brief said `max(previous labelled edge, previous detected edge)`; Code implemented **detected-else-labelled**, arguing the `max` form excludes the true clear on a one-frame run whose labels are late by one (true clear = labelled onset frame). Predeclared in AMENDMENT 5 before implementing. Is the reasoning right, and is detected-else-labelled sound in both directions and across runs?
D2 One predeclared expectation (`c07_previous_end_control_masked` "never a SHIFT") was wrong — the fixture's own known answer is `END-SHIFT(+1)`; corrected by appended A5.4, and the five F1/F2 cases are now judged per edge instead of by event token. Acceptable handling?

## Questions
Q1 Do your 079-05 and 079-07 fixtures read as Code claims against `1cbc07e` (mask twins: verdicts; unmasked: READING)? Any new fixture under the same mechanisms — in mask mode — that still yields a wrong SHIFT/PASS/NOT-VISIBLE? In particular: an occluder crossing the target silhouette; a lighting change over the silhouette; a mask that is dilated or lags the object by a frame.
Q2 Mask-mode semantics: is the region really the per-frame silhouette at every edge (not the anchor frame's mask), and does the F3 coverage rule hold when a mask is missing for one frame in the window?
Q3 Scorer v2.1: immutable denominator, five classes, cap derivation incl. the zero boundary and `NO ADMISSIBLE ENVELOPE` — reproduce and spot-check the 17 wrong cells and the 12 no-signal cells.
Q4 Batch: collision-safe id + uniqueness assertion, exit contract.
Q5 Wording: README Step 6, header legend, Section G note, FAIL line, cohort/N explanation, positive-support statement.
Q6 Merge readiness: GREEN / CHANGES REQUIRED / RED with the exact list.

Chat's position: merge-ready. The tool now claims exactly what it can do — verdicts where the target's pixels are known, readings elsewhere — and every counterexample so far has become a permanent test. If you find a mask-mode counterexample, that is a CHANGES REQUIRED; wording-only items can be listed for the merge brief rather than blocking.
