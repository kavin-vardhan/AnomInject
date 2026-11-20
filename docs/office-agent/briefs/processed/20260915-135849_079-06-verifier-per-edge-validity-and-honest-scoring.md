# SESSION: new-default
# NAME: 079 verifier field validity (post-Codex)
# MODEL: claude-opus-5
# EFFORT: xhigh

# 079-06 — Codex merge review 079-05: all five findings ACCEPTED. Per-edge validity, honest scorer, wording, batch fixes. No merge.

## 0. Read first
`_reviews/079-05-codex-merge-review.md` in full, its probe script `079-05-review-validation.py` and the JSON readings beside it, then `docs/predictions/2026-09-15-verifier-field-validity.md` (+AMENDMENTS 1–3), the 079-01..04 journals, and the tool. Fresh session: bootstrap from CLAUDE.md + latest journal as usual. Branch `fix/verifier-field-validity` at `57e91cd`; `master`/`m51` untouched; `Source/`, `Shaders/`, `measure_label_offset.py` byte-unchanged throughout.

## 1. Chat's rulings on 079-05
- **F1 (motion guard is not a measurability test) — ACCEPTED.** The whole-frame, opening-only `M_med` is demoted to a printed reading. Measurability becomes **per event, per edge, local in time and in region** (§2).
- **F2 (scorer hides per-run/per-edge failures) — ACCEPTED.** "A2L full recovery" is WITHDRAWN as a claim; the correct record is 8/8 · 7/8 · 6/8 per edge. The scorer is rebuilt (§3) and the envelope re-derived from it.
- **F3 (wording) — ACCEPTED** in full, including the aggregate-VERDICT distinction (§4).
- **F4, F5 (batch collisions, exit contract) — ACCEPTED** (§5).
- Codex's Q1 precision points accepted: `tau` is bounded by its inputs, not mathematically unbounded — fix the wording in G259 by dated append, not rewrite; the home reproduction explains the two banked unsatisfiable events and the mechanism class, not every office reading (office readings still owed).
- Not adopted, as Codex also says: ring subtraction, provenance veto, another physical campaign.

## 2. Per-edge validity (design fixed by chat; implement exactly, then measure)
For every event, every contiguous run, each edge (onset, end) separately:
V1 **Local regional baseline.** `tau_edge` and `base_edge` are learned from the nearest `N_BASE` clean pairs to THAT edge, using THAT run's region (the box/mask actually used for the edge, not the event's anchor region — closes the calibration mismatch Codex noted). If fewer than `MIN_BASELINE_FRAMES` are available ⇒ `NOT-MEASURABLE(baseline n<min at <edge>)`.
V2 **Local regional motion.** `m_edge` := median of the region's clean-pair `d` in that same local window. If `m_edge > REGION_CAP` ⇒ `NOT-MEASURABLE(regional motion m=… cap=… at <edge>)`. `REGION_CAP` is set by §3's cohort procedure; until then use the value the procedure yields on the real cohort, and print it as `provisional heuristic` in header and lines.
V3 **Edge dominance.** In the ±`edge_window` search, the chosen edge frame's `d` must exceed the second-largest `d` in the window by `DOMINANCE` (ratio; state it, e.g. ≥ 1.5×) AND exceed `tau_edge`. If the best frame is not dominant ⇒ `NOT-MEASURABLE(ambiguous edge: best=… second=… at <edge>)` — never a SHIFT. This is the direct cure for "a background burst wins the onset search".
V4 **NOT-VISIBLE only from a judgeable region.** NOT-VISIBLE is emitted only when V1 and V2 hold for BOTH edges of the run AND no frame in the searched windows exceeds `tau_edge`. Otherwise the run reads `NOT-MEASURABLE(no edge found; region not judgeable: …)`. NOT-VISIBLE lines keep `(bbox-only)` when no mask was used.
V5 **Event verdict = worst run/edge, with every run/edge line printed** (no more one-token event summary hiding an unread run). A run with one measurable edge and one refused edge reports both: e.g. `onset PASS · end NOT-MEASURABLE(ambiguous …)`; the event's verdict token is then `PARTIAL` (counts separately in the summary), not PASS.
V6 **Unsatisfiable-threshold and whole-frame-region refusals stay**, now per edge/run.
V7 **Selftest gains Codex's three fixtures as aligned POSITIVE controls** (quiet opening then motion; small moving region in a static frame; alternating scroll) — required reading: PASS or NOT-MEASURABLE, never SHIFT or NOT-VISIBLE — plus shifted versions of the first two as NEGATIVE controls that must still read the shift where the edge is dominant. Port them from `079-05-review-validation.py` with attribution in the file header. Existing 17 cases keep passing (some expected strings will change from NOT-VISIBLE→NOT-MEASURABLE where V4 applies — list each change and why).

## 3. Honest recovery scorer (`m079_shift_recovery.py` v2)
S1 Fixed cohort: every contiguous run × both edges of every event in the unshifted session = the denominator; it never shrinks under perturbation.
S2 Perturbations: onset-only ±1, end-only ±1, both ±1, and δ=0 — annotation-only (as now) AND a coherent variant that also shifts the per-frame `labels.jsonl` rows, reported separately (Codex Q2).
S3 Per cell: `recovered` (offset equals −δ on that edge), `wrong` (an offset returned but not −δ), `refused` (NOT-MEASURABLE with reason). Publish recovery rate, wrong rate, refusal rate with the reasons histogram. **Loss of coverage can never raise recovery.**
S4 Run on `A2L_LEGA`, `LYRA_SMOKE_01`, `A1L_LEGA`, `M50L_LG9` with the guard DISABLED (to see the raw gate) and ENABLED (to see the shipped behaviour). Record `m_edge` per cell.
S5 `REGION_CAP` := the largest `m_edge` at which the guard-disabled cohort shows ZERO `wrong` cells across all real sessions, rounded down to 2 s.f., printed as `provisional heuristic — from N real edges`. If zero-wrong is never reached at any `m_edge`, say so and set the cap to refuse everything above the floor — honesty over usefulness.
S6 Re-derive with the fixed scorer; keep 079-03/04's numbers in the journal as history with a dated correction.

## 4. Wording and verdict semantics
- README Step 6 and header: NOT-VISIBLE = "no detectable above-threshold change at either edge within the searched windows of a judgeable region; not a guarantee of absence"; `(bbox-only)` = "the supplied box was used; no per-frame mask"; `camera motion` reason renamed `regional image change` with scope stated; `region covers the picture` states the 90 % rectangle rule; cap = provisional heuristic with its cohort provenance; remove "measured exactly", "never calls a correct label NOT-VISIBLE", and the stale ring-normalised docstring (lines ~838–841).
- Summary line adds `PARTIAL n`; **VERDICT line distinguishes** `VERDICT PASS (n of N events fully checked)` / `VERDICT PASS-PARTIAL (n fully, p partially, u unread)` / `VERDICT UNREAD (0 of N events checked)` / `VERDICT FAIL (…)`. Exit codes unchanged (0/2/3; `--report-only` still 0 except 3) — Section G's card is corrected prospectively (header names, "always exits 0" prose) in `docs/office-rdp-card.md` with a dated note.
- `bbox=… obs=…` annotation labelled `producer metadata` in the header legend.

## 5. Batch
- Identifier and output filename = relative path from `--all` root with separators → `__`; collision test with two same-named sessions of different outcome, both reports retained, summary distinguishes them.
- `--report-only` in batch preserves CANNOT-RUN/error status exactly as single mode (return 3 if any session was 3 or errored; 0 otherwise); test: missing labels, unreadable input, mixed batch.

## 6. Predeclare, then run (AMENDMENT 4 in the predictions file, committed first)
P1 selftest: all old + V7 controls OK, with the listed expected-string changes.
P2 pinned bench ×6: PASS 4 / tau 0.0040 unchanged (every edge dominant; `m_edge` ≈ 0.0005).
P3 scorer v2 tables for the four real sessions, guard off and on, with the cohort denominator visible; the derived `REGION_CAP` and its N.
P4 Codex's fixtures through the shipped gate: PASS or NOT-MEASURABLE only.
P5 `M50L_LG9`, `A1L_LEGA`, `LYRA_SMOKE_01`: zero NOT-VISIBLE, zero SHIFT; VERDICT lines read `UNREAD` or `PASS-PARTIAL` as the data dictate.
P6 batch collision + exit-contract tests pass.
If P2 or P4 fails, STOP — NEEDS-DECISION with numbers.

## 7. Rules
Predictions before results. No constant tuned outside §3's procedure. Commits on `fix/verifier-field-validity`, Conventional Commits, pushed. **No merge.** Journal `2026-09-15-079-06-…`; gotcha: "an event-level score can hide a per-edge failure — score the fixed cohort" (G-number); write `_reviews/079-06-code-response-to-codex-079-05.md` (finding-by-finding: what changed, where, the reading that proves it) for the re-review. Restore checkout to `m51`. Report per-P with exact lines; `VERDICT: GREEN` only if P1–P6 read as predeclared.
