# 079-13 — regional-motion readings, one-sided peaks and stronger ties

## Authority and ownership

Owner supplied `D:/IntrusiveAnomalies/_reviews/079-13-codex-ruling-cap-decoupled-peaks-ties.md`.
079-12 is accepted as implemented. **Codex implements; Claude Code independently
reviews next; Chat orchestrates.** No mailbox or Code brief was sent.
Feature base `53e6e5154d4f36e3cfbcfa5ad524eac2fa092852` on
`fix/verifier-field-validity`. AMENDMENT 3 was appended and committed first at
`d08899750f8aa4cc38f6f9d2b9d52a8841626896`, before edits or new measurements.
Earlier predictions and reports remain historical; 079-11/12 reports are preserved.

## Implementation and limits

R7 removes numeric and refuse-all regional-motion guards, their API/CLI parameters
and scorer cap derivation. m_edge remains a printed reading. Any available edge
median above the historical0.42 marker adds a run caveat using the largest median.
CONSISTENT events count a caveated CONSISTENT run, with either lighting or motion
caveat; unread siblings do not manufacture this count. Both caveats can coexist.
Threshold unsatisfiability, whole-frame regions, baseline minimum, mask/RGB/identity
and session-coverage refusals remain. No marker tuning occurred.

R8 selects nearest distance, then larger d, then earlier frame. Both equidistant
strengths and the choice are printed. R9 uses d(k)>tau and
d(k)>=1.5*min(d(k-1),d(k+1)); outside the scanned window counts as zero. Adjacent
equal changes qualify; a positive plateau has qualifying edges but no interior
peaks. Per-target/mode labelled order, bounds and used-peak exclusion remain.
R10 reports decreasing assignments on the affected run, with no ordering floor.
Chat's prior missing6 prediction was wrong: A.end can choose free8, giving
4/8/10/none and OFFSET-NOTE/PARTIAL. That accepted known answer is unchanged.

The two-line header points to `docs/verifier-characterisation.md`. Scorer v3 now
runs one immutable572-key sweep, publishes all five classes by session, every
wrong recovery with expected/observed offsets, m_edge and competing peaks, and
all22 earlier wrong cells' current status. Recovered offsets do not prove label
correctness. Even a wrong association can coincide with a label and read CONSISTENT.

## Measured results and prediction disposition

P1:94/94 permanent fixtures and22/22 contracts pass after one measured correction.
Nine event strings change, eight as predeclared. **c08_one_frame_late is PARTIAL,
contrary to predeclared OFFSET-NOTE.** R9 admits60/61; labelled onset61 takes61;
end62 searches61..66 and has only consumed61. The first failing output was saved
before updating that known answer and adding exact selection/consumption assertions.
This is a prediction miss, not a reason to add look-ahead or tune the algorithm.
23 fixtures change shared edge/run fields; moving_fast additionally gains motion
caveat wording without changing its existing lighting-caveated CONSISTENT count.

Former cap fixtures obey the explicit marker condition: moving_over_cap m=.12
becomes CONSISTENT with its lighting caveat, without a motion caveat;
c07_no_envelope_masked m=0 becomes CONSISTENT without either caveat. moving_fast
m=.52 has both caveats. moving_unsat m=1 keeps its threshold refusal and adds
motion disclosure. Historical fixture names are retained; retired cap options
are intentionally rejected, not silently ignored.

P2-prime:64/64 pinned edges are TRANSITION with zero offset and tau0.0040. All six
legs have four CONSISTENT events each, including BL_SYN's one-frame runs. All24
events carry lighting caveats; no pinned edge needs a motion caveat.
P3: A2L_LEGA3CONSISTENT/4UNASSESSABLE; LYRA_SMOKE_01 3CONSISTENT/2PARTIAL/1UNASSESSABLE;
A1L_LEGA4CONSISTENT/1OFFSET-NOTE/1UNASSESSABLE. No NO-TRACE. A1L event4 has
m=.4275369507 and the motion caveat; A2L/LYRA have no >.42 medians. M50 remains
six READING events, with5NO-TRANSITION/11UNASSESSABLE edges and five motion-caveated
runs. Existing baseline, input, whole-frame and threshold refusals remain visible.

P4: one fixed sweep scores314 of572 keys, with258 unscored;
218 recovered, 55 wrong, 2 no-transition, 39 unassessable, 0 unsupported. All22 former wrong cells are re-listed:
16 recovered, 6 wrong. No cap is derived. The complete characterisation and ledgers are
linked from the report. Compared with079-12 guard-OFF, wrong recovery cells rise
from22 to55:16 former wrong cells recover,6 stay wrong and49 other keys become
wrong. One-sided prominence admits weak changes next to strong transitions;
nearest-distance selection can then prefer the weak change sitting on the shifted
label. The stronger tie rule applies only at equal distance. This limitation is
published without tuning; the pinned aligned bank passing does not erase it.
Of55 wrong cells,24 belong to CONSISTENT runs,23 to OFFSET-NOTE runs and8 to PARTIAL
runs. These are cells, not independent events. Thus the ruling's rationale that a
mis-picked peak always becomes OFFSET-NOTE is not supported by this measurement.
For example, A2L event3 onset+1/annotation chooses label40 itself (d=.03145988)
instead of intended39 (d=.99944259); expected-1 becomes0 in a CONSISTENT run.
The stronger tie rule also need not recover the intended edge: LYRA event0's
second-run onset+1 chooses23 over21 at label22 because .63662937>.63133631;
its end then selects22. Both annotation/coherent variants disclose inversion23>22.
The cohort has four inversion cells in total, including A1L's6>5 counterparts.
P5: batch collision/coverage behavior and CLI exits
NO-TRACE2, report-only0, execution-error3 pass. Scorer five-class, bbox-only zero-cell,
partial-run and unsupported fixed-denominator controls pass.

## Scope and disposition

**NEEDS-DECISION**, because P1's predeclared delayed one-frame reading misses,
despite the corrected regression suite and P2-prime passing. No independent
review approval or merge is claimed. **Chat review NOT REQUIRED** for this
authorized implementation and feature push. **Chat review REQUIRED** for the
prediction miss, completed evidence and independent Code review before merge;
this is the next mandatory checkpoint. Chat owns the next disposition.
Client/Section G, m51 campaign and release holds remain.

Only verifier/tests and guidance/prediction/journal/characterisation docs change
in AnomalyInjector. Source/, Shaders/ and measure_label_offset.py remain unchanged.
CaptureBench stays at472a409, tracked clean with no remote; only its permitted
already-untracked scorer changes. Feature alone is committed/pushed; original
m51 53bf725 is restored at the safe boundary. Master remains ac13700. No build,
cook, capture, merge, tag, mailbox, office-data transfer or owner-app change.
The external report records exact SHAs, full readings and measured wall time.
No monetary cost is inferred; billing data is unavailable. Same coherent
workstream; no session or context-setting change is recommended.


## Per-P assessment

| Prediction | Result | Evidence |
| --- | --- | --- |
| P1 | NEEDS-DECISION: one predeclared string misses; final94/94 +22/22 pass | initial/, first_prediction_mismatch.json, final/ and changed_readings.json |
| P2-prime | PASS:64/64 zero-offset transitions;6x4 CONSISTENT; tau0.004 | bank.json, audit.json and Appendix A |
| P3 | PASS: all readings, no NO-TRACE; M50 six READING; motion caveats exact | bank.json, audit.json and Appendix A |
| P4 | PASS:572 reconciled keys, one sweep, all wrong cells and22 prior statuses | characterisation_v3.json/.txt and frozen verifier-characterisation.md |
| P5 | PASS: collision/coverage contracts, five classes, CLI2/0/3 | final/contracts.txt, complete_checks.json and scorer_selftest.txt |

## Every changed fixture expectation

Comparison is against the frozen079-12 final94-case readings. The first079-13 run had exactly one mismatch; it remains in `initial/` with its original verifier hash. No detection rule changed after that result. The final source changes only that fixture's known answer/assertions and simplifies the lighting assertion to match the new chosen peak. Full shared-field changes: [changed_readings.json](079-13-evidence/changed_readings.json). New diagnostic schema fields and the global header change are documented once below; unchanged fixture outputs are retained in full.

| Fixture | 079-12 event string | 079-13 string | Reason / prediction |
| --- | --- | --- | --- |
| moving_over_cap | UNASSESSABLE | CONSISTENT | R7 removes old cap refusal; m=.12, lighting caveat only. |
| c07_no_envelope_masked | UNASSESSABLE | CONSISTENT | R7 removes refuse-all; m=0, no caveat. |
| c08_lighting_aligned | OFFSET-NOTE | CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_occluder_aligned | OFFSET-NOTE | CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_dilated_mask_ring_burst | OFFSET-NOTE | CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_precise_mask_target_move | OFFSET-NOTE | CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_one_frame_aligned | UNASSESSABLE | CONSISTENT | R9 admits adjacent on/off transitions; ordered exclusion determines the final reading. |
| c08_one_frame_late | OFFSET-NOTE | PARTIAL | MEASURED CORRECTION: R9 admits60/61; onset61 consumes61; end62 has no unused peak. |
| r6_blink_one_frame_second | PARTIAL | CONSISTENT | R9 admits adjacent on/off transitions; ordered exclusion determines the final reading. |

### Every affected run/edge reading

The following table exhausts all33 changed edge records across23 fixtures. `selected` is best_k; unchanged tau/baseline/window/input fields are omitted. JSON retains all changed fields, including region provenance and derived run reasons. All other fixture edge/run readings are unchanged, apart from the added diagnostics. moving_fast additionally prints its m=.52 motion caveat; its existing lighting caveat already made its CONSISTENT caveat count1.

| Fixture | Edge | Changed readings | Reason |
| --- | --- | --- | --- |
| moving_unsat | event0 run0 onset | run_caveat: False → True | R7 adds m=1 motion caveat; unsatisfiable tau still refuses. |
| moving_unsat | event0 run0 end | run_caveat: False → True | R7 adds m=1 motion caveat; unsatisfiable tau still refuses. |
| moving_over_cap | event0 run0 onset | observation: UNASSESSABLE → TRANSITION; reason: regional change m=0.120000 > cap=0.000140 → None; best_k: None → 10; best_d: None → 1.0; offset: None → 0; peaks: [] → [10]; available_peaks: [] → [10]; ring_d: None → 0.14125; ring_note: False → True; run_outcome: UNASSESSABLE → CONSISTENT; run_caveat: False → True | R7 removes old cap refusal; m=.12, lighting caveat only. |
| moving_over_cap | event0 run0 end | observation: UNASSESSABLE → TRANSITION; reason: regional change m=0.120000 > cap=0.000140 → None; best_k: None → 18; best_d: None → 1.0; offset: None → 0; peaks: [] → [18]; available_peaks: [] → [18]; ring_d: None → 0.14125; ring_note: False → True; run_outcome: UNASSESSABLE → CONSISTENT; run_caveat: False → True | R7 removes old cap refusal; m=.12, lighting caveat only. |
| codex_quiet_prefix | event0 run0 onset | best_k: 61 → 60; best_d: 0.528 → 0.264; offset: 1 → 0; peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [61]; ring_d: 0.5 → 0.16666666666666666 | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| codex_small_region | event0 run0 onset | best_k: 61 → 60; best_d: 0.528 → 0.264; offset: 1 → 0; peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [61] | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| codex_quiet_prefix_shift1 | event0 run0 onset | peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [60] | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| codex_small_region_shift1 | event0 run0 onset | peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [60] | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| codex_quiet_prefix_noburst_shift1 | event0 run0 onset | peaks: [60] → [60, 63]; available_peaks: [60] → [60, 63]; other_peaks: [] → [63] | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| codex_small_region_noburst_shift1 | event0 run0 onset | peaks: [60] → [60, 63]; available_peaks: [60] → [60, 63]; other_peaks: [] → [63] | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| codex_masked_negative_False | event0 run0 end | peaks: [68] → [68, 72, 73]; available_peaks: [68] → [68, 72, 73]; other_peaks: [] → [72, 73] | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| codex_masked_negative_True | event0 run0 end | peaks: [68] → [68, 72, 73]; available_peaks: [68] → [68, 72, 73]; other_peaks: [] → [72, 73] | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c07_burst_aligned | event0 run0 onset | best_k: 61 → 60; best_d: 0.496 → 0.16; offset: 1 → 0; peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [61] | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c07_burst_late | event0 run0 onset | peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [60] | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c07_no_envelope_masked | event0 run0 onset | observation: UNASSESSABLE → TRANSITION; reason: NO ADMISSIBLE ENVELOPE → None; best_k: None → 60; best_d: None → 1.0; offset: None → 0; peaks: [] → [60]; available_peaks: [] → [60]; ring_d: None → 0.0; run_outcome: UNASSESSABLE → CONSISTENT | R7 removes refuse-all; m=0, no caveat. |
| c07_no_envelope_masked | event0 run0 end | observation: UNASSESSABLE → TRANSITION; reason: NO ADMISSIBLE ENVELOPE → None; best_k: None → 68; best_d: None → 1.0; offset: None → 0; peaks: [] → [68]; available_peaks: [] → [68]; ring_d: None → 0.0; run_outcome: UNASSESSABLE → CONSISTENT | R7 removes refuse-all; m=0, no caveat. |
| c08_lighting_aligned | event0 run0 onset | best_k: 61 → 60; best_d: 1.0 → 0.0625; offset: 1 → 0; peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [61]; ring_d: 1.0 → 0.0; ring_note: True → False; run_outcome: OFFSET-NOTE → CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_lighting_aligned | event0 run0 end | run_outcome: OFFSET-NOTE → CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_lighting_late_onset | event0 run0 onset | peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [60] | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_occluder_aligned | event0 run0 onset | best_k: 61 → 60; best_d: 0.5 → 0.0625; offset: 1 → 0; peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [61]; run_outcome: OFFSET-NOTE → CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_occluder_aligned | event0 run0 end | run_outcome: OFFSET-NOTE → CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_dilated_mask_ring_burst | event0 run0 onset | best_k: 61 → 60; best_d: 0.5555555555555556 → 0.027777777777777776; offset: 1 → 0; peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [61]; run_outcome: OFFSET-NOTE → CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_dilated_mask_ring_burst | event0 run0 end | run_outcome: OFFSET-NOTE → CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_precise_mask_target_move | event0 run0 onset | best_k: 61 → 60; best_d: 0.8857142857142857 → 0.0625; offset: 1 → 0; peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [61]; run_outcome: OFFSET-NOTE → CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_precise_mask_target_move | event0 run0 end | run_outcome: OFFSET-NOTE → CONSISTENT | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_lagged_mask_target_move | event0 run0 onset | peaks: [61] → [60, 61]; available_peaks: [61] → [60, 61]; other_peaks: [] → [60] | R9 admits the neighbouring change as a peak; nearest selection and candidate disclosures follow. |
| c08_one_frame_aligned | event0 run0 onset | observation: NO-TRANSITION → TRANSITION; best_k: None → 60; best_d: None → 0.0625; offset: None → 0; peaks: [] → [60, 61]; available_peaks: [] → [60, 61]; other_peaks: [] → [61]; ring_d: None → 0.0; run_outcome: UNASSESSABLE → CONSISTENT | R9 admits adjacent on/off transitions; ordered exclusion determines the final reading. |
| c08_one_frame_aligned | event0 run0 end | observation: NO-TRANSITION → TRANSITION; best_k: None → 61; best_d: None → 0.0625; offset: None → 0; peaks: [] → [60, 61]; available_peaks: [] → [61]; consumed_peaks: [] → [60]; ring_d: None → 0.0; run_outcome: UNASSESSABLE → CONSISTENT | R9 admits adjacent on/off transitions; ordered exclusion determines the final reading. |
| c08_one_frame_late | event0 run0 onset | observation: NO-TRANSITION → TRANSITION; best_k: None → 61; best_d: None → 0.0625; offset: None → 0; peaks: [] → [60, 61]; available_peaks: [] → [60, 61]; other_peaks: [] → [60]; ring_d: None → 0.0; run_outcome: OFFSET-NOTE → PARTIAL | MEASURED CORRECTION: R9 admits60/61; onset61 consumes61; end62 has no unused peak. |
| c08_one_frame_late | event0 run0 end | observation: TRANSITION → UNASSESSABLE; reason: None → all peaks in window assigned to earlier edges: 61; best_k: 61 → None; best_d: 0.0625 → None; offset: -1 → None; available_peaks: [61] → []; consumed_peaks: [] → [61]; ring_d: 0.0 → None; run_outcome: OFFSET-NOTE → PARTIAL | MEASURED CORRECTION: R9 admits60/61; onset61 consumes61; end62 has no unused peak. |
| c08_one_frame_early | event0 run0 end | observation: NO-TRANSITION → TRANSITION; best_k: None → 61; best_d: None → 0.0625; offset: None → 1; peaks: [] → [60, 61]; available_peaks: [] → [61]; consumed_peaks: [] → [60]; ring_d: None → 0.0 | R9 admits adjacent on/off transitions; ordered exclusion determines the final reading. |
| r6_blink_one_frame_second | event0 run1 onset | observation: UNASSESSABLE → TRANSITION; reason: all peaks in window assigned to earlier edges: 7 → None; best_k: None → 10; best_d: None → 1.0; offset: None → 0; peaks: [7] → [7, 10, 11]; available_peaks: [] → [10, 11]; other_peaks: [] → [11]; ring_d: None → 0.0; run_outcome: PARTIAL → CONSISTENT | R9 admits adjacent on/off transitions; ordered exclusion determines the final reading. |
| r6_blink_one_frame_second | event0 run1 end | observation: NO-TRANSITION → TRANSITION; best_k: None → 11; best_d: None → 1.0; offset: None → 0; peaks: [] → [10, 11]; available_peaks: [] → [11]; consumed_peaks: [] → [10]; ring_d: None → 0.0; run_outcome: PARTIAL → CONSISTENT | R9 admits adjacent on/off transitions; ordered exclusion determines the final reading. |

### Retained missing6 answer and contracts

The accepted missing6 case remains selections4/8/10/none, run outcomes OFFSET-NOTE/PARTIAL. AMENDMENT3 records Chat's old prediction error and the free8 candidate. All nine original batch/union/mask contracts remain, plus the updated peak/assignment contracts and four additions for high/low motion, equal-strength ties and nearer-weaker precedence. The controlled separate-region case still selects15 then12 and checks the printed inversion note. Plateau boundaries, strict tau, nearest distance, stronger tie, scope, consumed/absent peaks, hard refusals and onset-first order are covered. Full22-test results: [contracts.txt](079-13-evidence/final/contracts.txt).

## Characterisation and known wrong recoveries

The complete published document follows. It was generated from the single frozen-source sweep; all22 previous wrong keys reconcile one-to-one. The original source sessions were read through temporary junctions; only scratch labels were perturbed. No cap, eligibility or peak tuning followed.

# Verifier characterisation — 079-13

This is a measured limitation record, not label approval. Recovery compares an observed offset with an injected label perturbation; it does not establish cause or correctness of the original label. Regional motion remains a reading. Values above the historical 0.42 marker add a caveat. Threshold, region, baseline and input-coverage refusals remain.

Each original run edge has 13 perturbation/variant keys. Every planned key remains in either the scored or unscored ledger. READING/UNASSESSABLE runs contribute zero scored cells. Rates use each session's planned denominator, not only the observed subset.

## Per-session readings

| Session | Planned | Scored | Unscored | recovered | wrong | no-transition | unassessable | unsupported | Positive original edges |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| A2L_LEGA | 208 | 70 | 138 | 46 | 16 | 0 | 8 | 0 | 6 |
| LYRA_SMOKE_01 | 208 | 118 | 90 | 73 | 24 | 0 | 21 | 0 | 9 |
| A1L_LEGA | 156 | 126 | 30 | 99 | 15 | 2 | 10 | 0 | 12 |
| TOTAL | 572 | 314 | 258 | 218 | 55 | 2 | 39 | 0 | 27 |

## Every wrong recovery — known limitations

Expected/observed values are offsets relative to the shifted label. All window peaks are listed; [used earlier] marks peaks excluded before this edge's choice. Nearest candidates win; equidistant candidates prefer larger d, then earlier frame. Thus a nearer unrelated peak can still beat a stronger intended transition. One-sided prominence admits plateau edges and adjacent changes. Different regions may yield out-of-order assignments, which are disclosed.

| Cell | Label | Selected | Expected | Observed | m_edge | Competing peaks |
| --- | --- | --- | --- | --- | --- | --- |
| A2L_LEGA event3 run0 onset onset+1/annotation | 40 | 40 | -1 | 0 | 0.0011036926011718218 | 36 d=0.19933600, 37 d=0.15221149, 39 d=0.99944259, 40 d=0.03145988, 43 d=0.16346337, 44 d=0.38385509 |
| A2L_LEGA event4 run0 onset onset+1/annotation | 52 | 52 | -1 | 0 | 5.820721769499418e-05 | 48 d=0.12063446, 51 d=0.99985448, 52 d=0.09403376, 53 d=0.00681024, 56 d=0.09534897 |
| A2L_LEGA event5 run0 onset onset+1/annotation | 76 | 76 | -1 | 0 | 5.820721769499418e-05 | 72 d=0.03009313, 75 d=1.00000000, 76 d=0.07001497 |
| A2L_LEGA event3 run0 onset onset+1/coherent | 40 | 40 | -1 | 0 | 0.0011036926011718218 | 36 d=0.19933600, 37 d=0.15221149, 39 d=0.99944259, 40 d=0.03145988, 43 d=0.16346337, 44 d=0.38385509 |
| A2L_LEGA event4 run0 onset onset+1/coherent | 52 | 52 | -1 | 0 | 5.820721769499418e-05 | 48 d=0.12063446, 51 d=0.99985448, 52 d=0.09403376, 53 d=0.00681024, 56 d=0.09534897 |
| A2L_LEGA event5 run0 onset onset+1/coherent | 76 | 76 | -1 | 0 | 5.820721769499418e-05 | 72 d=0.03009313, 75 d=1.00000000, 76 d=0.07001497 |
| A2L_LEGA event3 run0 end end+1/coherent | 48 | 48 | -1 | 0 | 0.0017577326611254939 | 44 d=0.38385509, 45 d=0.68355780, 47 d=0.99995912, 48 d=0.62592996, 52 d=0.73493664 |
| A2L_LEGA event4 run0 end end+1/coherent | 60 | 60 | -1 | 0 | 5.820721769499418e-05 | 56 d=0.09534897, 59 d=1.00000000, 60 d=0.12593132, 62 d=0.00497672 |
| A2L_LEGA event5 run0 end end+1/coherent | 84 | 84 | -1 | 0 | 0.0 | 81 d=0.04412661, 83 d=0.99994179, 84 d=0.02348661, 85 d=0.00713038, 88 d=0.02328289 |
| A2L_LEGA event5 run0 onset both+1/annotation | 76 | 76 | -1 | 0 | 5.820721769499418e-05 | 72 d=0.03009313, 75 d=1.00000000, 76 d=0.07001497 |
| A2L_LEGA event3 run0 onset both+1/coherent | 40 | 40 | -1 | 0 | 0.0011036926011718218 | 36 d=0.19713557, 37 d=0.15061596, 39 d=0.99984910, 40 d=0.02775263, 43 d=0.16053115, 44 d=0.36634362 |
| A2L_LEGA event3 run0 end both+1/coherent | 48 | 48 | -1 | 0 | 0.0011036926011718218 | 44 d=0.36634362, 45 d=0.64734447, 47 d=0.98028483, 48 d=0.62592996, 52 d=0.73493664 |
| A2L_LEGA event4 run0 onset both+1/coherent | 52 | 52 | -1 | 0 | 5.820721769499418e-05 | 48 d=0.12063446, 51 d=0.99985448, 52 d=0.09403376, 53 d=0.00681024, 56 d=0.05011641 |
| A2L_LEGA event4 run0 end both+1/coherent | 60 | 60 | -1 | 0 | 5.820721769499418e-05 | 56 d=0.05011641, 59 d=1.00000000, 60 d=0.12593132, 62 d=0.00497672 |
| A2L_LEGA event5 run0 onset both+1/coherent | 76 | 76 | -1 | 0 | 0.0 | 75 d=1.00000000, 76 d=0.05026193 |
| A2L_LEGA event5 run0 end both+1/coherent | 84 | 84 | -1 | 0 | 0.0 | 83 d=0.99994456, 84 d=0.02348661, 85 d=0.00713038, 88 d=0.02328289 |
| LYRA_SMOKE_01 event3 run0 end both+0/annotation | 59 | 60 | 0 | 1 | 0.21171586715867158 | 55 d=0.77722714, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event0 run1 onset onset+1/annotation | 22 | 23 | -1 | 1 | 0.002306219131135974 | 18 d=0.65651048 [used earlier], 19 d=0.02811813, 21 d=0.63133631, 23 d=0.63662937 |
| LYRA_SMOKE_01 event0 run1 end onset+1/annotation | 23 | 22 | 0 | -1 | 0.002306219131135974 | 22 d=0.05944479, 23 d=0.63662937 [used earlier], 24 d=0.05334140 |
| LYRA_SMOKE_01 event3 run0 end onset+1/annotation | 59 | 60 | 0 | 1 | 0.20285977859778598 | 55 d=0.77722714, 60 d=0.67776753 |
| LYRA_SMOKE_01 event0 run1 onset onset+1/coherent | 22 | 23 | -1 | 1 | 0.002306219131135974 | 18 d=0.65651048 [used earlier], 19 d=0.02811813, 21 d=0.63133631, 23 d=0.63662937 |
| LYRA_SMOKE_01 event0 run1 end onset+1/coherent | 23 | 22 | 0 | -1 | 0.002306219131135974 | 22 d=0.05944479, 23 d=0.63662937 [used earlier], 24 d=0.05334140 |
| LYRA_SMOKE_01 event3 run0 end onset+1/coherent | 59 | 60 | 0 | 1 | 0.20285977859778598 | 55 d=0.77722714, 60 d=0.67776753 |
| LYRA_SMOKE_01 event3 run0 end onset-1/annotation | 59 | 60 | 0 | 1 | 0.21220018450184502 | 55 d=0.77722714, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event3 run0 end onset-1/coherent | 59 | 60 | 0 | 1 | 0.21220018450184502 | 55 d=0.77722714, 60 d=0.67776753, 63 d=1.00000000 |
| LYRA_SMOKE_01 event0 run0 end end+1/coherent | 19 | 19 | -1 | 0 | 0.0035830001170123913 | 16 d=0.84143207 [used earlier], 18 d=0.65735123, 19 d=0.02812379, 21 d=0.63212091 |
| LYRA_SMOKE_01 event0 run1 end end+1/coherent | 24 | 24 | -1 | 0 | 0.0035717018132326115 | 21 d=0.63292408 [used earlier], 23 d=0.63662937, 24 d=0.05334140 |
| LYRA_SMOKE_01 event1 run0 end end+1/coherent | 36 | 36 | -1 | 0 | 0.06118311848843369 | 35 d=0.99991193, 36 d=0.15438601, 40 d=0.43068117 |
| LYRA_SMOKE_01 event3 run0 end end+1/coherent | 60 | 60 | -1 | 0 | 0.20728782287822878 | 56 d=0.90002575, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event4 run0 end end+1/coherent | 72 | 72 | -1 | 0 | 0.018656308983418347 | 71 d=1.00000000, 72 d=0.59385091, 76 d=0.08230770 |
| LYRA_SMOKE_01 event5 run0 onset both+1/annotation | 88 | 88 | -1 | 0 | 0.027754973515817412 | 87 d=0.99957443, 88 d=0.07046544 |
| LYRA_SMOKE_01 event0 run0 end both+1/coherent | 19 | 19 | -1 | 0 | 0.0029454843304268534 | 18 d=0.65695776, 19 d=0.02812379, 21 d=0.63212091, 22 d=0.05960756 |
| LYRA_SMOKE_01 event0 run1 onset both+1/coherent | 22 | 23 | -1 | 1 | 0.0029395065655691164 | 19 d=0.02806147 [used earlier], 21 d=0.63292408, 23 d=0.63420354, 24 d=0.05334140 |
| LYRA_SMOKE_01 event0 run1 end both+1/coherent | 24 | 24 | -1 | 0 | 0.0029395065655691164 | 22 d=0.05960756, 23 d=0.63420354 [used earlier], 24 d=0.05334140 |
| LYRA_SMOKE_01 event1 run0 end both+1/coherent | 36 | 36 | -1 | 0 | 0.05909941726999823 | 35 d=0.99913673, 36 d=0.15438601, 40 d=0.43068117 |
| LYRA_SMOKE_01 event3 run0 onset both+1/coherent | 52 | 52 | -1 | 0 | 0.20285977859778598 | 49 d=0.56099166, 52 d=0.89425186, 56 d=0.61472156 |
| LYRA_SMOKE_01 event3 run0 end both+1/coherent | 60 | 60 | -1 | 0 | 0.20285977859778598 | 56 d=0.61472156, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event4 run0 end both+1/coherent | 72 | 72 | -1 | 0 | 0.018617311682031602 | 71 d=1.00000000, 72 d=0.59385091, 76 d=0.08230770 |
| LYRA_SMOKE_01 event3 run0 onset both-1/coherent | 50 | 48 | 1 | -2 | 0.21268450184501844 | 48 d=0.48632441, 54 d=0.73434014 |
| LYRA_SMOKE_01 event3 run0 end both-1/coherent | 58 | 60 | 1 | 2 | 0.21268450184501844 | 54 d=0.73434014, 60 d=0.67776753 |
| A1L_LEGA event0 run0 end both+0/annotation | 6 | 8 | 0 | 2 | 0.0 | 4 d=0.60673932 [used earlier], 8 d=1.00000000 |
| A1L_LEGA event0 run0 onset onset+1/annotation | 5 | 6 | -1 | 1 | 0.001156737998843262 | 4 d=0.54235166, 6 d=0.99624060 |
| A1L_LEGA event0 run0 end onset+1/annotation | 6 | 5 | 0 | -1 | 0.001156737998843262 | 5 d=0.68165487, 8 d=1.00000000 |
| A1L_LEGA event1 run0 onset onset+1/annotation | 16 | 16 | -1 | 0 | 0.1035309750759843 | 15 d=0.99999473, 16 d=0.50537821 |
| A1L_LEGA event0 run0 onset onset+1/coherent | 5 | 6 | -1 | 1 | 0.001156737998843262 | 4 d=0.54235166, 6 d=0.99624060 |
| A1L_LEGA event0 run0 end onset+1/coherent | 6 | 5 | 0 | -1 | 0.001156737998843262 | 5 d=0.68165487, 8 d=1.00000000 |
| A1L_LEGA event1 run0 onset onset+1/coherent | 16 | 16 | -1 | 0 | 0.1035309750759843 | 15 d=0.99999473, 16 d=0.50537821 |
| A1L_LEGA event0 run0 end onset-1/coherent | 6 | 8 | 0 | 2 | 0.0 | 4 d=0.60673932 [used earlier], 8 d=1.00000000 |
| A1L_LEGA event0 run0 end end+1/coherent | 7 | 8 | -1 | 1 | 0.0 | 4 d=0.60673932 [used earlier], 8 d=1.00000000 |
| A1L_LEGA event2 run1 end end+1/coherent | 36 | 36 | -1 | 0 | 0.1199678923629692 | 33 d=0.99966365 [used earlier], 35 d=0.99931198, 36 d=0.52524425, 39 d=0.88644905 |
| A1L_LEGA event0 run0 end end-1/annotation | 5 | 8 | 1 | 3 | 0.12505850103630406 | 8 d=1.00000000 |
| A1L_LEGA event0 run0 end end-1/coherent | 5 | 8 | 1 | 3 | 0.12505850103630406 | 8 d=1.00000000 |
| A1L_LEGA event1 run0 onset both+1/coherent | 16 | 16 | -1 | 0 | 0.12561668944495766 | 15 d=0.99999473, 16 d=0.50537821 |
| A1L_LEGA event2 run1 end both+1/coherent | 36 | 36 | -1 | 0 | 0.1199678923629692 | 35 d=0.99866507, 36 d=0.52524425, 39 d=0.88644905 |
| A1L_LEGA event3 run0 onset both+1/coherent | 52 | 52 | -1 | 0 | 0.02057720812625338 | 51 d=1.00000000, 52 d=0.84183451, 55 d=0.84510466, 56 d=0.41943577 |

## Previously wrong cells after R8/R9

| Cell | Previous expected / observed | Current status | Expected | Observed | m_edge | Competing peaks |
| --- | --- | --- | --- | --- | --- | --- |
| A2L_LEGA event3 run0 end end-1/annotation | 1 / -1 | recovered | 1 | 1 | 0.0016533591418662794 | 43 d=0.16346337, 44 d=0.38385509, 45 d=0.68355780, 47 d=0.98028483, 48 d=0.60327446, 50 d=0.44127215 |
| A2L_LEGA event5 run0 end end-1/annotation | 1 / -1 | recovered | 1 | 1 | 0.0 | 81 d=0.04412661, 83 d=0.99994456 |
| A2L_LEGA event3 run0 end end-1/coherent | 1 / -1 | recovered | 1 | 1 | 0.0016533591418662794 | 43 d=0.16346337, 44 d=0.38385509, 45 d=0.68355780, 47 d=0.98028483, 48 d=0.60327446, 50 d=0.44127215 |
| A2L_LEGA event5 run0 end end-1/coherent | 1 / -1 | recovered | 1 | 1 | 0.0 | 81 d=0.04412661, 83 d=0.99994456 |
| A2L_LEGA event3 run0 end both-1/annotation | 1 / -1 | recovered | 1 | 1 | 0.0016533591418662794 | 43 d=0.16346337, 44 d=0.38385509, 45 d=0.68355780, 47 d=0.98028483, 48 d=0.60327446, 50 d=0.44127215 |
| A2L_LEGA event5 run0 end both-1/annotation | 1 / -1 | recovered | 1 | 1 | 0.0 | 81 d=0.04412661, 83 d=0.99994456 |
| A2L_LEGA event3 run0 end both-1/coherent | 1 / -1 | recovered | 1 | 1 | 0.0016214743153018121 | 43 d=0.16700627, 44 d=0.35910410, 45 d=0.66704713, 47 d=0.99995912, 48 d=0.62592996, 50 d=0.46389154 |
| A2L_LEGA event5 run0 end both-1/coherent | 1 / -1 | recovered | 1 | 1 | 1.4551804423748545e-05 | 81 d=0.04412661, 83 d=0.99994179, 84 d=0.02348661, 85 d=0.00713038 |
| LYRA_SMOKE_01 event3 run0 end both+0/annotation | 0 / 4 | wrong | 0 | 1 | 0.21171586715867158 | 55 d=0.77722714, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event3 run0 end onset-1/annotation | 0 / 4 | wrong | 0 | 1 | 0.21220018450184502 | 55 d=0.77722714, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event3 run0 end onset-1/coherent | 0 / 4 | wrong | 0 | 1 | 0.21220018450184502 | 55 d=0.77722714, 60 d=0.67776753, 63 d=1.00000000 |
| LYRA_SMOKE_01 event3 run0 end end+1/coherent | -1 / 3 | wrong | -1 | 0 | 0.20728782287822878 | 56 d=0.90002575, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event3 run0 end both+1/coherent | -1 / 3 | wrong | -1 | 0 | 0.20285977859778598 | 56 d=0.61472156, 60 d=0.67776753, 63 d=0.87669582 |
| A1L_LEGA event3 run0 onset both+0/annotation | 0 / 4 | recovered | 0 | 0 | 0.019164213027532413 | 51 d=1.00000000, 55 d=0.98550725 |
| A1L_LEGA event3 run0 onset onset+1/annotation | -1 / 3 | recovered | -1 | -1 | 0.03192006106446464 | 51 d=0.65483703, 55 d=0.98550725, 56 d=0.51220030 |
| A1L_LEGA event3 run0 onset onset+1/coherent | -1 / 3 | recovered | -1 | -1 | 0.03192006106446464 | 51 d=0.65483703, 55 d=0.98550725, 56 d=0.51220030 |
| A1L_LEGA event3 run0 onset end+1/annotation | 0 / 4 | recovered | 0 | 0 | 0.019661696747754816 | 51 d=1.00000000, 55 d=0.98550725 |
| A1L_LEGA event3 run0 onset end+1/coherent | 0 / 4 | recovered | 0 | 0 | 0.021075468774213544 | 51 d=1.00000000, 55 d=0.98550725 |
| A1L_LEGA event3 run0 onset end-1/annotation | 0 / 4 | recovered | 0 | 0 | 0.019891577887278685 | 51 d=1.00000000, 55 d=0.98550725 |
| A1L_LEGA event3 run0 onset end-1/coherent | 0 / 4 | recovered | 0 | 0 | 0.019891577887278685 | 51 d=1.00000000, 55 d=0.98550725 |
| A1L_LEGA event3 run0 onset both+1/annotation | -1 / 3 | recovered | -1 | -1 | 0.03591006869752272 | 51 d=0.65483703, 55 d=0.98550725, 56 d=0.51220030 |
| A1L_LEGA event3 run0 onset both+1/coherent | -1 / 3 | wrong | -1 | 0 | 0.02057720812625338 | 51 d=1.00000000, 52 d=0.84183451, 55 d=0.84510466, 56 d=0.41943577 |

## Coverage reasons

| Unscored reason | Keys |
| --- | --- |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (14 unavailable) | 20 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (16 unavailable) | 16 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (17 unavailable) | 16 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (13 unavailable) | 11 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (16 unavailable) | 9 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (15 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 6 | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 30 | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 35 | 8 |
| UNASSESSABLE: neither edge observed; baseline: only 2 valid clean pairs, need 3 (25 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (22 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; baseline: only 2 valid clean pairs, need 3 (24 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 53 | 7 |
| UNASSESSABLE: neither edge observed; mask missing at frame 23 | 6 |
| UNASSESSABLE: neither edge observed; mask missing at frame 47 | 6 |
| UNASSESSABLE: neither edge observed; baseline: only 2 valid clean pairs, need 3 (15 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 0; boundary mask unavailable: mask missing at frame 3 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 2; boundary mask unavailable: mask missing at frame 3 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 23; boundary mask unavailable: mask missing at frame 27 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 26; boundary mask unavailable: mask missing at frame 27 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 30; boundary mask unavailable: mask missing at frame 32 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 31; boundary mask unavailable: mask missing at frame 32 | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (13 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 11 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 59 | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 2 valid clean pairs, need 3 (16 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (23 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (21 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 18 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 42 | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (24 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (14 unavailable) | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 6; boundary mask unavailable: mask missing at frame 8 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 7; boundary mask unavailable: mask missing at frame 8 | 2 |
| UNASSESSABLE: neither edge observed; mask id 215 not present at frame 9; boundary mask unavailable: mask missing at frame 14 | 2 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (12 unavailable) | 2 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (15 unavailable) | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 10; boundary mask unavailable: mask missing at frame 15 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 14; boundary mask unavailable: mask missing at frame 15 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 18; boundary mask unavailable: mask missing at frame 20 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 19; boundary mask unavailable: mask missing at frame 20 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 23; boundary mask unavailable: mask missing at frame 26 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 35; boundary mask unavailable: mask missing at frame 39 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 38; boundary mask unavailable: mask missing at frame 39 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 42; boundary mask unavailable: mask missing at frame 44 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 43; boundary mask unavailable: mask missing at frame 44 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 81; boundary mask unavailable: mask missing at frame 86 | 2 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (23 unavailable) | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 71 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 69; boundary mask unavailable: mask missing at frame 74 | 2 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=2.3006 >= 1.0 | 2 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (22 unavailable) | 1 |
| UNASSESSABLE: neither edge observed; edge truncated by the session boundary | 1 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (25 unavailable) | 1 |
| UNASSESSABLE: neither edge observed; mask missing at frame 47; boundary mask unavailable: mask missing at frame 50 | 1 |
| UNASSESSABLE: neither edge observed; mask missing at frame 85; boundary mask unavailable: mask missing at frame 86 | 1 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=1.9478 >= 1.0 | 1 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=2.5772 >= 1.0 | 1 |

## Provenance and disposition

Verifier SHA256: `badd0e3e867814d1de702f4259d9a23f94d8ed3738b72f360c91740cc38ed8d0`. Scorer SHA256: `159e6944534cc348bb6a8711d8886766c4b541ce87cea10af8d458c3c17d0f2f`. Immutable planned keys: 572. The JSON ledger contains every scored and unscored key, candidate strengths and tie/ordering diagnostics.

**Chat review REQUIRED** for completed evidence and independent Code review before merge. Client/Section G, campaign and release holds remain; this document does not release them.


## Source identity, scope and commit evidence

| Item | Verified value |
| --- | --- |
| Feature base | 53e6e5154d4f36e3cfbcfa5ad524eac2fa092852 |
| AMENDMENT3 first | d08899750f8aa4cc38f6f9d2b9d52a8841626896 |
| Implementation / remote feature | c1945b64060ef5c10ed5a379443340d7574db8d6 |
| Restored m51 | 53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67 |
| Master / origin/master | ac13700d99fe116056f5aa11611f8931a563a3f2 |
| CaptureBench HEAD (no remote) | 472a409dc70ae78d01a38d08fc3bddb15fe41c58 |
| Source tree | 50d5507d8c07e9eb9f2fb18500aefdedf84950f3 |
| Shaders tree | cdacf50c690788f28fa3846e99f44f8c5ef0f3b9 |
| Unchanged measure_label_offset blob | 67b4c42941dee4244adf8d22c941aec502582850 |
| Preserved079-12 report SHA256 | 439e03d80bbd715381933e5023d7de7617eafb025d93ad841985b2939a247552 |
| Preserved079-11 report SHA256 | 687b9cc9fc9fd044b2d4ff207a3072150c6188312273af2ff92fe706bd8d511e |

Frozen tools used for the single sweep exactly match final working sources byte-for-byte. The final fixture and bank ledgers record that same verifier SHA. Final frozen copies include committed blob identities as well as on-disk SHA256; Git newline normalization can make those representations differ without a logic change. Both tracked trees are clean at restored m51. All10 plugin and seven other CaptureBench untracked files retain their preflight hashes; only the permitted existing CaptureBench scorer changed. No Source/Shaders/measure_label_offset diff, master move, merge, tag, build, capture, mailbox, office transfer or owner-app action.

[source_scope.json](079-13-evidence/source_scope.json) also verifies syntax-tree identity for the unchanged pair-region/input checks, threshold function, run/event aggregation and fixture generators, plus scorer perturbation, expected-offset, cohort-key, reconciliation and cleanup helpers.

```text
CLAUDE.md                                          |  60 ++---
 docs/client-readme.md                              |  66 ++---
 docs/gotchas.md                                    |  37 +++
 docs/office-rdp-card.md                            |  47 ++--
 .../2026-09-20-verifier-consistency-semantics.md   |  95 +++++++
 docs/sessions/2026-09-20-079-13-codex-delta.md     | 105 ++++++++
 docs/verifier-characterisation.md                  | 172 +++++++++++++
 tools/test_verify_capture_consistency.py           |  74 ++++--
 tools/verify_capture.py                            | 280 +++++++++------------
 9 files changed, 682 insertions(+), 254 deletions(-)
```

| Frozen file | SHA256 |
| --- | --- |
| verify_capture.py | badd0e3e867814d1de702f4259d9a23f94d8ed3738b72f360c91740cc38ed8d0 |
| measure_label_offset.py | 15538658dda9c14df791b6d4b1e746c4ffef94519ac9f7b59989db62ab9933d1 |
| test_verify_capture_consistency.py | 0f08ceb88260e879767ad33fbe01e01e434fbf8cbcffaf06c78f421ed6bdbcd1 |
| CLAUDE.md | 9942706381606ec486c8c6625188ae4e02617cbd5e8dcd7ccf0c648561f7acee |
| client-readme.md | 79ba5079d1af193498cf8e860746dd04906b45094a4def22cba509d7277c76e1 |
| office-rdp-card.md | 1356be8fc802bdd41a1a5ca73bb9799b10a397ccb6690bc4485160b5e4cce992 |
| gotchas.md | dd9fb1f3df16a54bb088b6ce6a452400ef74f6910cfb5fe5cc5f810eb5d98cf5 |
| 2026-09-20-verifier-consistency-semantics.md | 0869c5988f446403ead5bbebf92856f9578419aefbd5b9c77db33c09239e50cd |
| 2026-09-20-079-13-codex-delta.md | 86420bd53b7a1f5b4f0c1d90f0456df3ec6158918816ff5b6d58d9ad86ee8db2 |
| verifier-characterisation.md | 080ebc6479b22ce35b93ed74d834ac6fe43d48d880472081d6c358bd64054508 |
| m079_shift_recovery.py | 159e6944534cc348bb6a8711d8886766c4b541ce87cea10af8d458c3c17d0f2f |

Verifier removes cap/refuse-all state and parameters; adds peak strengths, ties, inversion and motion-caveat details. Scorer removes both guard phases and derivation, adds fixed-key comparison and markdown publication. Tests retain94 fixtures, update ruled answers/contracts, add four focused contracts. Current client/office guidance, CLAUDE status, dated G264 correction, predictions, journal and generated characterisation reflect the ruling. Historical records remain historical.

## Reproduction and evidence map

The live plugin checkout is restored to m51. Use the frozen files, not that older live verifier:

```powershell
$tools07913 = 'D:\IntrusiveAnomalies\_reviews\079-13-frozen'
python -B "$tools07913\verify_capture.py" --label-pixel-gate --selftest
python -B "$tools07913\test_verify_capture_consistency.py"
$env:IAI_VERIFIER_TOOLS = $tools07913
python -B "$tools07913\m079_shift_recovery.py" --selftest
python -B "$tools07913\verify_capture.py" --label-pixel-gate --report-only --dir '<banked session path from Appendix A>'
```

The full sweep command used the equivalent `079-13-sweep-tools` frozen directory,
`--json .../079-13-evidence/characterisation_v3.json`,
`--previous-cells .../079-12-evidence/cohort_v3.json` and
`--markdown .../AnomalyInjector/docs/verifier-characterisation.md`.
The first-run mismatch is `079-13-evidence/first_prediction_mismatch.json` and
`initial/selftest.txt`; final94 outputs and full structured details are in `final/`.
`audit.json` checks unchanged source hashes, all retained fixtures, all64 pinned
zero-offset/tau readings and every bank motion caveat. `complete_checks.json`
contains CLI stdout/stderr/exit evidence and scorer class controls. Full fixed-key
scorer output/ledger: `characterisation_v3.txt/.json`. `final/boundary.json` verifies
refs, preserved source/untracked hashes and restoration. `final/current-prepend.json`
records the handoff update; all earlier CURRENT bytes are retained.


## Wall time and next checkpoint

Task start: **2026-09-20 11:16:04 UTC**. Verified feature push/restoration boundary: **2026-09-20 11:46:19 UTC**. Measured wall interval: **30m 15s** (1815 seconds), including implementation, tests, one full sweep, commit and push; final report/handoff bookkeeping follows that timestamp. Monetary/token billing is unavailable and not inferred.

**Chat review NOT REQUIRED** for the authorized implementation and feature push, now complete. **Chat review REQUIRED** for the delayed one-frame prediction miss, complete evidence and independent Claude Code review before merge. This is the next mandatory checkpoint. Chat owns disposition; no Code brief was sent. Client/Section G, m51 campaign and release holds remain.

## Appendix A — complete final bank readings

### M49_GEDGE_BL_NAT

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_BL_NAT\session_20260904-135206
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  regional change          m_edge is a reading; >0.42 adds a caveat (historical marker, not a boundary)
  characterisation         docs/verifier-characterisation.md
                           Per-session recovery readings and all wrong cells with competing peaks; known limitations, not label approval.
  clean-frame pool         41 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds; peaks >tau and >=1.5 x min(neighbours), outside window=0
  assignment               per target/mode, labelled order: nearest unused peak; tie larger d, then earlier; inversions disclosed
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              StaticMeshActor_49     CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[4..5] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=18 (including baseline); assignment: onset 4 (nearest of {4, 6}), end 6 (nearest of {6, 9})
        onset TRANSITION 4 d=0.5454 tau=0.0040 label 4 delta=+0 ring d=0.3156 look at frames 3..5 note: whole-region change (lighting/camera?); other peaks in window: 6 m_edge=0.0000 base=11 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 5, delta=+18; max distance, 12 frames incl baseline)
        end   TRANSITION 6 d=0.5460 tau=0.0040 label 6 delta=+0 ring d=0.3145 look at frames 5..7 note: whole-region change (lighting/camera?); other peaks in window: 9 m_edge=0.0000 base=11 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 3 frames incl baseline) (mask extrapolated from frame 5, delta=+18; max distance, 14 frames incl baseline)
      run[9..10] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 9 (nearest of {9, 11}), end 11 (nearest of {11})
        onset TRANSITION 9 d=0.5453 tau=0.0040 label 9 delta=+0 ring d=0.3153 look at frames 8..10 note: whole-region change (lighting/camera?); other peaks in window: 11 m_edge=0.0000 base=14 pairs=6/6 (mask extrapolated from frame 9, delta=-9; max distance, 5 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 16 frames incl baseline)
        end   TRANSITION 11 d=0.5455 tau=0.0040 label 11 delta=+0 ring d=0.3158 look at frames 10..12 note: whole-region change (lighting/camera?) m_edge=0.0000 base=14 pairs=7/7 (mask extrapolated from frame 9, delta=-9; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 17 frames incl baseline)
idx=1   blink              StaticMeshActor_49     CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[26..27] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=26 (including baseline); assignment: onset 26 (nearest of {26, 28}), end 28 (nearest of {28, 31})
        onset TRANSITION 26 d=0.5457 tau=0.0040 label 26 delta=+0 ring d=0.3148 look at frames 25..27 note: whole-region change (lighting/camera?); other peaks in window: 28 m_edge=0.0000 base=21 pairs=7/7 (mask extrapolated from frame 26, delta=-26; max distance, 15 frames incl baseline) (mask extrapolated from frame 27, delta=+18; max distance, 12 frames incl baseline)
        end   TRANSITION 28 d=0.5458 tau=0.0040 label 28 delta=+0 ring d=0.3145 look at frames 27..29 note: whole-region change (lighting/camera?); other peaks in window: 31 m_edge=0.0000 base=21 pairs=6/6 (mask extrapolated from frame 26, delta=-26; max distance, 14 frames incl baseline) (mask extrapolated from frame 27, delta=+18; max distance, 14 frames incl baseline)
      run[31..32] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 31 (nearest of {31, 33}), end 33 (nearest of {33})
        onset TRANSITION 31 d=0.5457 tau=0.0040 label 31 delta=+0 ring d=0.3144 look at frames 30..32 note: whole-region change (lighting/camera?); other peaks in window: 33 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 31, delta=-18; max distance, 14 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 16 frames incl baseline)
        end   TRANSITION 33 d=0.5457 tau=0.0040 label 33 delta=+0 ring d=0.3155 look at frames 32..34 note: whole-region change (lighting/camera?) m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 31, delta=-18; max distance, 12 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 17 frames incl baseline)
idx=2   blink              StaticMeshActor_49     CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[48..49] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 48 (nearest of {48, 50}), end 50 (nearest of {50, 53})
        onset TRANSITION 48 d=0.5455 tau=0.0040 label 48 delta=+0 ring d=0.3158 look at frames 47..49 note: whole-region change (lighting/camera?); other peaks in window: 50 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 48, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 49, delta=+18; max distance, 12 frames incl baseline)
        end   TRANSITION 50 d=0.5453 tau=0.0040 label 50 delta=+0 ring d=0.3150 look at frames 49..51 note: whole-region change (lighting/camera?); other peaks in window: 53 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 48, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 49, delta=+18; max distance, 14 frames incl baseline)
      run[53..54] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 53 (nearest of {53, 55}), end 55 (nearest of {55})
        onset TRANSITION 53 d=0.5454 tau=0.0040 label 53 delta=+0 ring d=0.3161 look at frames 52..54 note: whole-region change (lighting/camera?); other peaks in window: 55 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 53, delta=-18; max distance, 14 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 16 frames incl baseline)
        end   TRANSITION 55 d=0.5458 tau=0.0040 label 55 delta=+0 ring d=0.3143 look at frames 54..56 note: whole-region change (lighting/camera?) m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 53, delta=-18; max distance, 12 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 17 frames incl baseline)
idx=3   blink              StaticMeshActor_49     CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[70..71] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 70 (nearest of {70, 72}), end 72 (nearest of {72, 75})
        onset TRANSITION 70 d=0.5457 tau=0.0040 label 70 delta=+0 ring d=0.3158 look at frames 69..71 note: whole-region change (lighting/camera?); other peaks in window: 72 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 70, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 71, delta=+18; max distance, 12 frames incl baseline)
        end   TRANSITION 72 d=0.5453 tau=0.0040 label 72 delta=+0 ring d=0.3147 look at frames 71..73 note: whole-region change (lighting/camera?); other peaks in window: 75 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 70, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 71, delta=+18; max distance, 14 frames incl baseline)
      run[75..76] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=18 (including baseline); assignment: onset 75 (nearest of {75, 77}), end 77 (nearest of {77})
        onset TRANSITION 75 d=0.5458 tau=0.0040 label 75 delta=+0 ring d=0.3148 look at frames 74..76 note: whole-region change (lighting/camera?); other peaks in window: 77 m_edge=0.0000 base=20 pairs=6/6 (mask extrapolated from frame 75, delta=-18; max distance, 14 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 12 frames incl baseline)
        end   TRANSITION 77 d=0.5452 tau=0.0040 label 77 delta=+0 ring d=0.3140 look at frames 76..78 note: whole-region change (lighting/camera?) m_edge=0.0000 base=20 pairs=7/7 (mask extrapolated from frame 75, delta=-18; max distance, 12 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 4 (4 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 events)
  run coverage             CONSISTENT 8  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (4 consistent (4 with caveat), 0 offset-notes for human review, 0 partial, 0 unassessable, 0 readings)
```

### M49_GEDGE_CT_NAT

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_CT_NAT\session_20260904-135137
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  regional change          m_edge is a reading; >0.42 adds a caveat (historical marker, not a boundary)
  characterisation         docs/verifier-characterisation.md
                           Per-session recovery readings and all wrong cells with competing peaks; known limitations, not label approval.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds; peaks >tau and >=1.5 x min(neighbours), outside window=0
  assignment               per target/mode, labelled order: nearest unused peak; tie larger d, then earlier; inversions disclosed
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   corrupted_texture  StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[3..10] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 3 d=0.7473 tau=0.0040 label 3 delta=+0 ring d=0.3302 look at frames 2..4 note: whole-region change (lighting/camera?) m_edge=0.0000 base=12 pairs=7/7 (mask id unresolved - sole value used) (mask extrapolated from frame 3, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 11 d=0.7472 tau=0.0040 label 11 delta=+0 ring d=0.3293 look at frames 10..12 note: whole-region change (lighting/camera?) m_edge=0.0000 base=12 pairs=9/9 (mask extrapolated from frame 10, delta=+28; max distance, 16 frames incl baseline)
idx=1   corrupted_texture  StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[25..32] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 25 d=0.7468 tau=0.0040 label 25 delta=+0 ring d=0.3302 look at frames 24..26 note: whole-region change (lighting/camera?) m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 12 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 33 d=0.7473 tau=0.0040 label 33 delta=+0 ring d=0.3298 look at frames 32..34 note: whole-region change (lighting/camera?) m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 10 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 16 frames incl baseline)
idx=2   corrupted_texture  StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[47..54] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 47 d=0.7470 tau=0.0040 label 47 delta=+0 ring d=0.3308 look at frames 46..48 note: whole-region change (lighting/camera?) m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 55 d=0.7471 tau=0.0040 label 55 delta=+0 ring d=0.3295 look at frames 54..56 note: whole-region change (lighting/camera?) m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 16 frames incl baseline)
idx=3   corrupted_texture  StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[69..76] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 69 d=0.7473 tau=0.0040 label 69 delta=+0 ring d=0.3302 look at frames 68..70 note: whole-region change (lighting/camera?) m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 11 frames incl baseline)
        end   TRANSITION 77 d=0.7471 tau=0.0040 label 77 delta=+0 ring d=0.3306 look at frames 76..78 note: whole-region change (lighting/camera?) m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 4 (4 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 events)
  run coverage             CONSISTENT 4  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (4 consistent (4 with caveat), 0 offset-notes for human review, 0 partial, 0 unassessable, 0 readings)
```

### M49_GEDGE_MO_NAT

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_MO_NAT\session_20260904-135147
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  regional change          m_edge is a reading; >0.42 adds a caveat (historical marker, not a boundary)
  characterisation         docs/verifier-characterisation.md
                           Per-session recovery readings and all wrong cells with competing peaks; known limitations, not label approval.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds; peaks >tau and >=1.5 x min(neighbours), outside window=0
  assignment               per target/mode, labelled order: nearest unused peak; tie larger d, then earlier; inversions disclosed
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   missing_object     StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[3..10] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 3 d=0.5453 tau=0.0040 label 3 delta=+0 ring d=0.3154 look at frames 2..4 note: whole-region change (lighting/camera?) m_edge=0.0000 base=12 pairs=7/7 (mask id unresolved - sole value used) (mask extrapolated from frame 3, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 11 d=0.5455 tau=0.0040 label 11 delta=+0 ring d=0.3158 look at frames 10..12 note: whole-region change (lighting/camera?) m_edge=0.0000 base=12 pairs=9/9 (mask extrapolated from frame 10, delta=+28; max distance, 16 frames incl baseline)
idx=1   missing_object     StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[25..32] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 25 d=0.5454 tau=0.0040 label 25 delta=+0 ring d=0.3145 look at frames 24..26 note: whole-region change (lighting/camera?) m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 12 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 33 d=0.5457 tau=0.0040 label 33 delta=+0 ring d=0.3155 look at frames 32..34 note: whole-region change (lighting/camera?) m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 10 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 16 frames incl baseline)
idx=2   missing_object     StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[47..54] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 47 d=0.5458 tau=0.0040 label 47 delta=+0 ring d=0.3156 look at frames 46..48 note: whole-region change (lighting/camera?) m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 55 d=0.5458 tau=0.0040 label 55 delta=+0 ring d=0.3143 look at frames 54..56 note: whole-region change (lighting/camera?) m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 16 frames incl baseline)
idx=3   missing_object     StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[69..76] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 69 d=0.5458 tau=0.0040 label 69 delta=+0 ring d=0.3155 look at frames 68..70 note: whole-region change (lighting/camera?) m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 11 frames incl baseline)
        end   TRANSITION 77 d=0.5452 tau=0.0040 label 77 delta=+0 ring d=0.3140 look at frames 76..78 note: whole-region change (lighting/camera?) m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 4 (4 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 events)
  run coverage             CONSISTENT 4  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (4 consistent (4 with caveat), 0 offset-notes for human review, 0 partial, 0 unassessable, 0 readings)
```

### M49_GEDGE_MT_NAT

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_MT_NAT\session_20260904-135030
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  regional change          m_edge is a reading; >0.42 adds a caveat (historical marker, not a boundary)
  characterisation         docs/verifier-characterisation.md
                           Per-session recovery readings and all wrong cells with competing peaks; known limitations, not label approval.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds; peaks >tau and >=1.5 x min(neighbours), outside window=0
  assignment               per target/mode, labelled order: nearest unused peak; tie larger d, then earlier; inversions disclosed
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   missing_texture    StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[3..10] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 3 d=0.0246 tau=0.0040 label 3 delta=+0 ring d=0.0926 look at frames 2..4 note: whole-region change (lighting/camera?) m_edge=0.0000 base=12 pairs=7/7 (mask id unresolved - sole value used) (mask extrapolated from frame 3, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 11 d=0.0245 tau=0.0040 label 11 delta=+0 ring d=0.0925 look at frames 10..12 note: whole-region change (lighting/camera?) m_edge=0.0000 base=12 pairs=9/9 (mask extrapolated from frame 10, delta=+28; max distance, 16 frames incl baseline)
idx=1   missing_texture    StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[25..32] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 25 d=0.0248 tau=0.0040 label 25 delta=+0 ring d=0.0938 look at frames 24..26 note: whole-region change (lighting/camera?) m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 12 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 33 d=0.0250 tau=0.0040 label 33 delta=+0 ring d=0.0934 look at frames 32..34 note: whole-region change (lighting/camera?) m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 10 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 16 frames incl baseline)
idx=2   missing_texture    StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[47..54] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 47 d=0.0246 tau=0.0040 label 47 delta=+0 ring d=0.0937 look at frames 46..48 note: whole-region change (lighting/camera?) m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 55 d=0.0247 tau=0.0040 label 55 delta=+0 ring d=0.0935 look at frames 54..56 note: whole-region change (lighting/camera?) m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 16 frames incl baseline)
idx=3   missing_texture    StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[69..76] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 69 d=0.0246 tau=0.0040 label 69 delta=+0 ring d=0.0929 look at frames 68..70 note: whole-region change (lighting/camera?) m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 11 frames incl baseline)
        end   TRANSITION 77 d=0.0249 tau=0.0040 label 77 delta=+0 ring d=0.0944 look at frames 76..78 note: whole-region change (lighting/camera?) m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 4 (4 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 events)
  run coverage             CONSISTENT 4  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (4 consistent (4 with caveat), 0 offset-notes for human review, 0 partial, 0 unassessable, 0 readings)
```

### M49_GEDGE_BL_SYN

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_BL_SYN\session_20260904-135302
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  regional change          m_edge is a reading; >0.42 adds a caveat (historical marker, not a boundary)
  characterisation         docs/verifier-characterisation.md
                           Per-session recovery readings and all wrong cells with competing peaks; known limitations, not label approval.
  clean-frame pool         41 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds; peaks >tau and >=1.5 x min(neighbours), outside window=0
  assignment               per target/mode, labelled order: nearest unused peak; tie larger d, then earlier; inversions disclosed
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              StaticMeshActor_49     CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[4..6] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=17 (including baseline); assignment: onset 4 (nearest of {4, 7}), end 7 (nearest of {7, 10})
        onset TRANSITION 4 d=0.5454 tau=0.0040 label 4 delta=+0 ring d=0.3156 look at frames 3..5 note: whole-region change (lighting/camera?); other peaks in window: 7 m_edge=0.0000 base=11 pairs=7/7 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 6, delta=+17; max distance, 12 frames incl baseline)
        end   TRANSITION 7 d=0.5457 tau=0.0040 label 7 delta=+0 ring d=0.3144 look at frames 6..8 note: whole-region change (lighting/camera?); other peaks in window: 10 m_edge=0.0000 base=11 pairs=7/7 (mask extrapolated from frame 4, delta=-4; max distance, 3 frames incl baseline) (mask extrapolated from frame 6, delta=+17; max distance, 14 frames incl baseline)
      run[10..10] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 10 (nearest of {10, 11}), end 11 (nearest of {11})
        onset TRANSITION 10 d=0.5454 tau=0.0040 label 10 delta=+0 ring d=0.3148 look at frames 9..11 note: whole-region change (lighting/camera?); other peaks in window: 11 m_edge=0.0000 base=14 pairs=5/5 (mask extrapolated from frame 10, delta=+28; max distance, 21 frames incl baseline)
        end   TRANSITION 11 d=0.5455 tau=0.0040 label 11 delta=+0 ring d=0.3158 look at frames 10..12 note: whole-region change (lighting/camera?) m_edge=0.0000 base=14 pairs=6/6 (mask extrapolated from frame 10, delta=+28; max distance, 20 frames incl baseline)
idx=1   blink              StaticMeshActor_49     CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[26..28] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=26 (including baseline); assignment: onset 26 (nearest of {26, 29}), end 29 (nearest of {29, 32})
        onset TRANSITION 26 d=0.5457 tau=0.0040 label 26 delta=+0 ring d=0.3148 look at frames 25..27 note: whole-region change (lighting/camera?); other peaks in window: 29 m_edge=0.0000 base=21 pairs=8/8 (mask extrapolated from frame 26, delta=-26; max distance, 15 frames incl baseline) (mask extrapolated from frame 28, delta=+17; max distance, 12 frames incl baseline)
        end   TRANSITION 29 d=0.5453 tau=0.0040 label 29 delta=+0 ring d=0.3143 look at frames 28..30 note: whole-region change (lighting/camera?); other peaks in window: 32 m_edge=0.0000 base=21 pairs=7/7 (mask extrapolated from frame 26, delta=-26; max distance, 14 frames incl baseline) (mask extrapolated from frame 28, delta=+17; max distance, 14 frames incl baseline)
      run[32..32] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 32 (nearest of {32, 33}), end 33 (nearest of {33})
        onset TRANSITION 32 d=0.5455 tau=0.0040 label 32 delta=+0 ring d=0.3148 look at frames 31..33 note: whole-region change (lighting/camera?); other peaks in window: 33 m_edge=0.0000 base=23 pairs=5/5 (mask extrapolated from frame 32, delta=+28; max distance, 30 frames incl baseline)
        end   TRANSITION 33 d=0.5457 tau=0.0040 label 33 delta=+0 ring d=0.3155 look at frames 32..34 note: whole-region change (lighting/camera?) m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 32, delta=+28; max distance, 29 frames incl baseline)
idx=2   blink              StaticMeshActor_49     CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[48..50] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 48 (nearest of {48, 51}), end 51 (nearest of {51, 54})
        onset TRANSITION 48 d=0.5455 tau=0.0040 label 48 delta=+0 ring d=0.3158 look at frames 47..49 note: whole-region change (lighting/camera?); other peaks in window: 51 m_edge=0.0000 base=23 pairs=8/8 (mask extrapolated from frame 48, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 50, delta=+17; max distance, 12 frames incl baseline)
        end   TRANSITION 51 d=0.5455 tau=0.0040 label 51 delta=+0 ring d=0.3164 look at frames 50..52 note: whole-region change (lighting/camera?); other peaks in window: 54 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 48, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 50, delta=+17; max distance, 14 frames incl baseline)
      run[54..54] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 54 (nearest of {54, 55}), end 55 (nearest of {55})
        onset TRANSITION 54 d=0.5457 tau=0.0040 label 54 delta=+0 ring d=0.3144 look at frames 53..55 note: whole-region change (lighting/camera?); other peaks in window: 55 m_edge=0.0000 base=23 pairs=5/5 (mask extrapolated from frame 54, delta=+28; max distance, 30 frames incl baseline)
        end   TRANSITION 55 d=0.5458 tau=0.0040 label 55 delta=+0 ring d=0.3143 look at frames 54..56 note: whole-region change (lighting/camera?) m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 54, delta=+28; max distance, 29 frames incl baseline)
idx=3   blink              StaticMeshActor_49     CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[70..72] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 70 (nearest of {70, 73}), end 73 (nearest of {73, 76})
        onset TRANSITION 70 d=0.5457 tau=0.0040 label 70 delta=+0 ring d=0.3158 look at frames 69..71 note: whole-region change (lighting/camera?); other peaks in window: 73 m_edge=0.0000 base=23 pairs=8/8 (mask extrapolated from frame 70, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 72, delta=+17; max distance, 12 frames incl baseline)
        end   TRANSITION 73 d=0.5456 tau=0.0040 label 73 delta=+0 ring d=0.3146 look at frames 72..74 note: whole-region change (lighting/camera?); other peaks in window: 76 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 70, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 72, delta=+17; max distance, 14 frames incl baseline)
      run[76..76] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=19 (including baseline); assignment: onset 76 (nearest of {76, 77}), end 77 (nearest of {77})
        onset TRANSITION 76 d=0.5456 tau=0.0040 label 76 delta=+0 ring d=0.3137 look at frames 75..77 note: whole-region change (lighting/camera?); other peaks in window: 77 m_edge=0.0000 base=20 pairs=5/5 (mask extrapolated from frame 76, delta=-19; max distance, 26 frames incl baseline)
        end   TRANSITION 77 d=0.5452 tau=0.0040 label 77 delta=+0 ring d=0.3140 look at frames 76..78 note: whole-region change (lighting/camera?) m_edge=0.0000 base=20 pairs=6/6 (mask extrapolated from frame 76, delta=-19; max distance, 25 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 4 (4 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 events)
  run coverage             CONSISTENT 8  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (4 consistent (4 with caveat), 0 offset-notes for human review, 0 partial, 0 unassessable, 0 readings)
```

### M49_GEDGE_MT_SYN

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_MT_SYN\session_20260904-135223
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  regional change          m_edge is a reading; >0.42 adds a caveat (historical marker, not a boundary)
  characterisation         docs/verifier-characterisation.md
                           Per-session recovery readings and all wrong cells with competing peaks; known limitations, not label approval.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds; peaks >tau and >=1.5 x min(neighbours), outside window=0
  assignment               per target/mode, labelled order: nearest unused peak; tie larger d, then earlier; inversions disclosed
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   missing_texture    StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[3..10] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 3 d=0.0246 tau=0.0040 label 3 delta=+0 ring d=0.0926 look at frames 2..4 note: whole-region change (lighting/camera?) m_edge=0.0000 base=12 pairs=7/7 (mask id unresolved - sole value used) (mask extrapolated from frame 3, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 11 d=0.0245 tau=0.0040 label 11 delta=+0 ring d=0.0925 look at frames 10..12 note: whole-region change (lighting/camera?) m_edge=0.0000 base=12 pairs=9/9 (mask extrapolated from frame 10, delta=+28; max distance, 16 frames incl baseline)
idx=1   missing_texture    StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[25..32] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 25 d=0.0248 tau=0.0040 label 25 delta=+0 ring d=0.0938 look at frames 24..26 note: whole-region change (lighting/camera?) m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 12 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 33 d=0.0250 tau=0.0040 label 33 delta=+0 ring d=0.0934 look at frames 32..34 note: whole-region change (lighting/camera?) m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 10 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 16 frames incl baseline)
idx=2   missing_texture    StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[47..54] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 47 d=0.0246 tau=0.0040 label 47 delta=+0 ring d=0.0937 look at frames 46..48 note: whole-region change (lighting/camera?) m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 55 d=0.0247 tau=0.0040 label 55 delta=+0 ring d=0.0935 look at frames 54..56 note: whole-region change (lighting/camera?) m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 16 frames incl baseline)
idx=3   missing_texture    StaticMeshActor_49     CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[69..76] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 69 d=0.0246 tau=0.0040 label 69 delta=+0 ring d=0.0929 look at frames 68..70 note: whole-region change (lighting/camera?) m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 11 frames incl baseline)
        end   TRANSITION 77 d=0.0249 tau=0.0040 label 77 delta=+0 ring d=0.0944 look at frames 76..78 note: whole-region change (lighting/camera?) m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 4 (4 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 events)
  run coverage             CONSISTENT 4  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (4 consistent (4 with caveat), 0 offset-notes for human review, 0 partial, 0 unassessable, 0 readings)
```

### A2L_LEGA

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\A2L_LEGA\session_20260904-180418
  frames / labels / events 90 / 90 / 7
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  regional change          m_edge is a reading; >0.42 adds a caveat (historical marker, not a boundary)
  characterisation         docs/verifier-characterisation.md
                           Per-session recovery readings and all wrong cells with competing peaks; known limitations, not label approval.
  clean-frame pool         15 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds; peaks >tau and >=1.5 x min(neighbours), outside window=0
  assignment               per target/mode, labelled order: nearest unused peak; tie larger d, then earlier; inversions disclosed
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              Cube5                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=drawn obs=measured
      run[4..5] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=4 (including baseline)
        onset UNASSESSABLE(baseline: only 0 valid clean pairs, need 3 (15 unavailable)) label 4 m_edge=n/a base=0 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 5, delta=+1; max distance, 1 frames incl baseline)
        end   UNASSESSABLE(baseline: only 0 valid clean pairs, need 3 (15 unavailable)) label 6 m_edge=n/a base=0 pairs=6/6 (mask extrapolated from frame 4, delta=-1; max distance, 1 frames incl baseline) (mask extrapolated from frame 5, delta=+3; max distance, 3 frames incl baseline)
      run[9..10] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=5 (including baseline)
        onset UNASSESSABLE(baseline: only 0 valid clean pairs, need 3 (15 unavailable)) label 9 m_edge=n/a base=0 pairs=6/6 (mask extrapolated from frame 9, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+1; max distance, 1 frames incl baseline)
        end   UNASSESSABLE(baseline: only 0 valid clean pairs, need 3 (15 unavailable)) label 11 m_edge=n/a base=0 pairs=7/7 (mask extrapolated from frame 9, delta=-1; max distance, 1 frames incl baseline) (mask extrapolated from frame 10, delta=+5; max distance, 5 frames incl baseline)
idx=1   corrupted_texture  Cube4                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=drawn obs=measured
      run[15..22] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=5 (including baseline)
        onset UNASSESSABLE(baseline: only 0 valid clean pairs, need 3 (15 unavailable)) label 15 m_edge=n/a base=0 pairs=9/9 (mask extrapolated from frame 15, delta=-5; max distance, 5 frames incl baseline)
        end   UNASSESSABLE(baseline: only 0 valid clean pairs, need 3 (15 unavailable)) label 23 m_edge=n/a base=0 pairs=9/9 (mask extrapolated from frame 22, delta=+5; max distance, 5 frames incl baseline)
idx=2   blink              Cube4                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=drawn obs=measured
      run[28..29] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=5 (including baseline)
        onset UNASSESSABLE(baseline: only 0 valid clean pairs, need 3 (15 unavailable)) label 28 m_edge=n/a base=0 pairs=7/7 (mask extrapolated from frame 28, delta=-5; max distance, 5 frames incl baseline) (mask extrapolated from frame 29, delta=+1; max distance, 1 frames incl baseline)
        end   UNASSESSABLE(baseline: only 0 valid clean pairs, need 3 (15 unavailable)) label 30 m_edge=n/a base=0 pairs=6/6 (mask extrapolated from frame 28, delta=-1; max distance, 1 frames incl baseline) (mask extrapolated from frame 29, delta=+3; max distance, 3 frames incl baseline)
      run[33..34] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (14 unavailable)) label 33 m_edge=n/a base=1 pairs=6/6 (mask extrapolated from frame 33, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 34, delta=+28; max distance, 3 frames incl baseline)
        end   UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (14 unavailable)) label 35 m_edge=n/a base=1 pairs=7/7 (mask extrapolated from frame 33, delta=-1; max distance, 1 frames incl baseline) (mask extrapolated from frame 34, delta=+28; max distance, 7 frames incl baseline)
idx=3   missing_texture    B_Manny_C_7            CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=drawn obs=measured
      run[39..46] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=26 (including baseline); assignment: onset 39 (nearest of {36, 37, 39, 40, 43}), end 47 (nearest of {43, 44, 45, 47, 48, 51})
        onset TRANSITION 39 d=0.9998 tau=0.0150 label 39 delta=+0 ring d=0.0516 look at frames 38..40 note: whole-region change (lighting/camera?); other peaks in window: 36, 37, 40, 43 m_edge=0.0024 base=11 pairs=9/9 (mask extrapolated from frame 39, delta=-5; max distance, 5 frames incl baseline) (mask extrapolated from frame 46, delta=+26; max distance, 12 frames incl baseline)
        end   TRANSITION 47 d=1.0000 tau=0.0150 label 47 delta=+0 ring d=0.1734 look at frames 46..48 note: whole-region change (lighting/camera?); other peaks in window: 43, 44, 45, 48, 51 m_edge=0.0024 base=11 pairs=9/9 (mask extrapolated from frame 46, delta=+26; max distance, 17 frames incl baseline)
idx=4   corrupted_texture  Cube2_8                CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=drawn obs=measured
      run[51..58] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 51 (nearest of {47, 51, 52, 53}), end 59 (nearest of {56, 59, 60, 62})
        onset TRANSITION 51 d=0.9999 tau=0.0040 label 51 delta=+0 ring d=0.1421 look at frames 50..52 note: whole-region change (lighting/camera?); other peaks in window: 47, 52, 53 m_edge=0.0001 base=12 pairs=9/9 (mask extrapolated from frame 51, delta=-5; max distance, 5 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 59 d=1.0000 tau=0.0040 label 59 delta=+0 ring d=0.1516 look at frames 58..60 note: whole-region change (lighting/camera?); other peaks in window: 56, 60, 62 m_edge=0.0001 base=12 pairs=9/9 (mask extrapolated from frame 58, delta=+28; max distance, 16 frames incl baseline)
idx=5   missing_texture    Cube2_8                CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=drawn obs=measured
      run[75..82] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=14 (including baseline); assignment: onset 75 (nearest of {75, 76}), end 83 (nearest of {81, 83, 84, 85})
        onset TRANSITION 75 d=1.0000 tau=0.0040 label 75 delta=+0 ring d=0.1202 look at frames 74..76 note: whole-region change (lighting/camera?); other peaks in window: 76 m_edge=0.0000 base=15 pairs=9/9 (mask extrapolated from frame 75, delta=-14; max distance, 14 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 5 frames incl baseline)
        end   TRANSITION 83 d=0.9999 tau=0.0040 label 83 delta=+0 ring d=0.1189 look at frames 82..84 note: whole-region change (lighting/camera?); other peaks in window: 81, 84, 85 m_edge=0.0000 base=15 pairs=9/9 (mask extrapolated from frame 75, delta=-14; max distance, 12 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 7 frames incl baseline)
idx=6   lod_popping        B_Manny_C_7            UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=projected obs=unmeasured
      UNASSESSABLE(non-manifested/empty event or no explicit frame_indices)
------------------------------------------------------------------------------
  CONSISTENT 3 (3 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 7 events)
  run coverage             CONSISTENT 3  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 5  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (3 consistent (3 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

### LYRA_SMOKE_01

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\LYRA_SMOKE_01\session_20260904-151503
  frames / labels / events 90 / 90 / 6
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  regional change          m_edge is a reading; >0.42 adds a caveat (historical marker, not a boundary)
  characterisation         docs/verifier-characterisation.md
                           Per-session recovery readings and all wrong cells with competing peaks; known limitations, not label approval.
  clean-frame pool         24 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds; peaks >tau and >=1.5 x min(neighbours), outside window=0
  assignment               per target/mode, labelled order: nearest unused peak; tie larger d, then earlier; inversions disclosed
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              Cube7                  CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[16..17] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=16 (including baseline); assignment: onset 16 (nearest of {16, 18}), end 18 (nearest of {18, 19, 21})
        onset TRANSITION 16 d=0.8414 tau=0.0212 label 16 delta=+0 ring d=0.0263 look at frames 15..17 note: whole-region change (lighting/camera?); other peaks in window: 18 m_edge=0.0036 base=13 pairs=7/7 (mask extrapolated from frame 16, delta=-16; max distance, 16 frames incl baseline) (mask extrapolated from frame 17, delta=+1; max distance, 1 frames incl baseline)
        end   TRANSITION 18 d=0.6574 tau=0.0212 label 18 delta=+0 ring d=0.0040 look at frames 17..19; other peaks in window: 19, 21 m_edge=0.0036 base=13 pairs=6/6 (mask extrapolated from frame 16, delta=-16; max distance, 15 frames incl baseline) (mask extrapolated from frame 17, delta=+3; max distance, 3 frames incl baseline)
      run[21..22] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=21 (including baseline); assignment: onset 21 (nearest of {19, 21, 23}), end 23 (nearest of {23, 24})
        onset TRANSITION 21 d=0.6329 tau=0.0211 label 21 delta=+0 ring d=0.0077 look at frames 20..22; other peaks in window: 19, 23 m_edge=0.0036 base=13 pairs=6/6 (mask extrapolated from frame 21, delta=-21; max distance, 17 frames incl baseline) (mask extrapolated from frame 22, delta=+1; max distance, 1 frames incl baseline)
        end   TRANSITION 23 d=0.6366 tau=0.0211 label 23 delta=+0 ring d=0.0122 look at frames 22..24; other peaks in window: 24 m_edge=0.0036 base=13 pairs=7/7 (mask extrapolated from frame 21, delta=-21; max distance, 15 frames incl baseline) (mask extrapolated from frame 22, delta=+5; max distance, 5 frames incl baseline)
idx=1   corrupted_texture  Cube4                  CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[27..34] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=27 (including baseline); assignment: onset 27 (nearest of {23, 27, 31}), end 35 (nearest of {31, 35, 36})
        onset TRANSITION 27 d=0.9999 tau=0.1334 label 27 delta=+0 ring d=0.4296 look at frames 26..28 note: whole-region change (lighting/camera?); other peaks in window: 23, 31 m_edge=0.0612 base=13 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 27, delta=-27; max distance, 18 frames incl baseline)
        end   TRANSITION 35 d=0.9999 tau=0.1334 label 35 delta=+0 ring d=0.3668 look at frames 34..36 note: whole-region change (lighting/camera?); other peaks in window: 31, 36 m_edge=0.0612 base=13 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 27, delta=-27; max distance, 14 frames incl baseline) (mask extrapolated from frame 34, delta=+5; max distance, 5 frames incl baseline)
idx=2   blink              Cube4                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[40..41] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 40 m_edge=n/a base=1 pairs=7/7 (mask extrapolated from frame 40, delta=-28; max distance, 7 frames incl baseline) (mask extrapolated from frame 41, delta=+1; max distance, 1 frames incl baseline)
        end   UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 42 m_edge=n/a base=1 pairs=6/6 (mask extrapolated from frame 40, delta=-28; max distance, 3 frames incl baseline) (mask extrapolated from frame 41, delta=+3; max distance, 3 frames incl baseline)
      run[45..46] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 45 m_edge=n/a base=1 pairs=6/6 (mask extrapolated from frame 45, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 46, delta=+28; max distance, 3 frames incl baseline)
        end   UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 47 m_edge=n/a base=1 pairs=7/7 (mask extrapolated from frame 45, delta=-1; max distance, 1 frames incl baseline) (mask extrapolated from frame 46, delta=+28; max distance, 7 frames incl baseline)
idx=3   missing_texture    B_Manny_C_14           PARTIAL runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=1 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[51..58] PARTIAL - one edge observed; the other is unassessable; masks: in-span actual, edges extrapolated <=26 (including baseline); assignment: onset none (nearest of {}), end 60 (nearest of {55, 60, 63})
        onset UNASSESSABLE(mask missing at frame 53) label 51 m_edge=n/a base=0 pairs=7/9 (mask id unresolved - sole value used) (mask extrapolated from frame 51, delta=-4; max distance, 4 frames incl baseline)
        end   TRANSITION 60 d=0.6778 tau=0.3376 label 59 delta=+1 ring d=0.3041 look at frames 59..61; other peaks in window: 55, 63 m_edge=0.2117 base=11 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 58, delta=+26; max distance, 16 frames incl baseline)
idx=4   corrupted_texture  Cube3                  CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[63..70] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=14 (including baseline); assignment: onset 63 (nearest of {59, 63, 64}), end 71 (nearest of {71, 72, 75})
        onset TRANSITION 63 d=1.0000 tau=0.0580 label 63 delta=+0 ring d=0.0837 look at frames 62..64 note: whole-region change (lighting/camera?); other peaks in window: 59, 64 m_edge=0.0187 base=11 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 63, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 70, delta=+14; max distance, 12 frames incl baseline)
        end   TRANSITION 71 d=1.0000 tau=0.0580 label 71 delta=+0 ring d=0.5494 look at frames 70..72 note: whole-region change (lighting/camera?); other peaks in window: 72, 75 m_edge=0.0187 base=11 pairs=9/9 (mask extrapolated from frame 70, delta=+14; max distance, 14 frames incl baseline)
idx=5   missing_texture    Cube3                  PARTIAL runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=1 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[87..89] PARTIAL - one edge observed; the other is unassessable; masks: in-span actual, edges extrapolated <=14 (including baseline)
        onset TRANSITION 87 d=1.0000 tau=0.0892 label 87 delta=+0 ring d=0.0440 look at frames 86..88 m_edge=0.0291 base=11 pairs=7/7 (mask id unresolved - sole value used) (mask extrapolated from frame 87, delta=-14; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(edge truncated by the session boundary) label 90 m_edge=0.0291 base=11 pairs=3/3 (mask id unresolved - sole value used) (mask extrapolated from frame 87, delta=-14; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 3 (3 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 2  UNASSESSABLE 1  READING 0  (of 6 events)
  run coverage             CONSISTENT 4  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 2  UNASSESSABLE 2  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (3 consistent (3 with caveat), 0 offset-notes for human review, 2 partial, 1 unassessable, 0 readings)
```

### A1L_LEGA

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\A1L_LEGA\session_20260904-165004
  frames / labels / events 90 / 90 / 6
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  regional change          m_edge is a reading; >0.42 adds a caveat (historical marker, not a boundary)
  characterisation         docs/verifier-characterisation.md
                           Per-session recovery readings and all wrong cells with competing peaks; known limitations, not label approval.
  clean-frame pool         30 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds; peaks >tau and >=1.5 x min(neighbours), outside window=0
  assignment               per target/mode, labelled order: nearest unused peak; tie larger d, then earlier; inversions disclosed
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              Cube7                  OFFSET-NOTE runs{CONSISTENT=0 OFFSET-NOTE=1 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=measured
      run[4..5] OFFSET-NOTE(+0,+2) - a target transition sits off the label - possible label offset OR an unrelated change (lighting, occlusion, neighbouring event); a person must look at the listed frames k-1..k+1; does not establish cause; masks: in-span actual, edges extrapolated <=7 (including baseline); assignment: onset 4 (nearest of {4, 6}), end 8 (nearest of {8})
        onset TRANSITION 4 d=0.6067 tau=0.0040 label 4 delta=+0 ring d=0.5727 look at frames 3..5 note: whole-region change (lighting/camera?); other peaks in window: 6 m_edge=0.0000 base=4 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 5, delta=+7; max distance, 6 frames incl baseline)
        end   TRANSITION 8 d=1.0000 tau=0.0040 label 6 delta=+2 ring d=1.0000 look at frames 7..9 note: whole-region change (lighting/camera?) m_edge=0.0000 base=4 pairs=7/7 (mask extrapolated from frame 4, delta=-1; max distance, 1 frames incl baseline) (mask extrapolated from frame 5, delta=+7; max distance, 7 frames incl baseline)
idx=1   corrupted_texture  Cube6_0                CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=measured
      run[15..22] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=26 (including baseline); assignment: onset 15 (nearest of {12, 15, 16}), end 23 (nearest of {23})
        onset TRANSITION 15 d=1.0000 tau=0.3372 label 15 delta=+0 ring d=0.1651 look at frames 14..16; other peaks in window: 12, 16 m_edge=0.1036 base=15 pairs=9/9 (mask extrapolated from frame 15, delta=-7; max distance, 7 frames incl baseline) (mask extrapolated from frame 22, delta=+26; max distance, 12 frames incl baseline)
        end   TRANSITION 23 d=1.0000 tau=0.3372 label 23 delta=+0 ring d=0.0654 look at frames 22..24 m_edge=0.1036 base=15 pairs=9/9 (mask extrapolated from frame 15, delta=-7; max distance, 5 frames incl baseline) (mask extrapolated from frame 22, delta=+26; max distance, 17 frames incl baseline)
idx=2   blink              Cube6_0                CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=measured
      run[28..29] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=20 (including baseline); assignment: onset 28 (nearest of {28, 30}), end 30 (nearest of {30, 33})
        onset TRANSITION 28 d=1.0000 tau=0.3780 label 28 delta=+0 ring d=0.2324 look at frames 27..29; other peaks in window: 30 m_edge=0.1083 base=15 pairs=7/7 (mask extrapolated from frame 28, delta=-20; max distance, 10 frames incl baseline) (mask extrapolated from frame 29, delta=+19; max distance, 13 frames incl baseline)
        end   TRANSITION 30 d=0.9997 tau=0.3780 label 30 delta=+0 ring d=0.2322 look at frames 29..31; other peaks in window: 33 m_edge=0.1083 base=15 pairs=6/6 (mask extrapolated from frame 28, delta=-20; max distance, 6 frames incl baseline) (mask extrapolated from frame 29, delta=+19; max distance, 15 frames incl baseline)
      run[33..34] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 33 (nearest of {33, 35}), end 35 (nearest of {35, 36, 39})
        onset TRANSITION 33 d=0.9997 tau=0.3440 label 33 delta=+0 ring d=0.2281 look at frames 32..34; other peaks in window: 35 m_edge=0.1081 base=16 pairs=6/6 (mask extrapolated from frame 33, delta=-25; max distance, 8 frames incl baseline) (mask extrapolated from frame 34, delta=+28; max distance, 15 frames incl baseline)
        end   TRANSITION 35 d=0.9993 tau=0.3440 label 35 delta=+0 ring d=0.2268 look at frames 34..36; other peaks in window: 36, 39 m_edge=0.1081 base=16 pairs=7/7 (mask extrapolated from frame 33, delta=-25; max distance, 6 frames incl baseline) (mask extrapolated from frame 34, delta=+28; max distance, 16 frames incl baseline)
idx=3   corrupted_texture  Cube2_8                CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=measured
      run[51..58] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 51 (nearest of {51, 55}), end 59 (nearest of {55, 56, 57, 59})
        onset TRANSITION 51 d=1.0000 tau=0.0763 label 51 delta=+0 ring d=0.2554 look at frames 50..52 note: whole-region change (lighting/camera?); other peaks in window: 55 m_edge=0.0192 base=23 pairs=9/9 (mask extrapolated from frame 51, delta=-14; max distance, 14 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 59 d=0.9903 tau=0.0763 label 59 delta=+0 ring d=0.0184 look at frames 58..60; other peaks in window: 55, 56, 57 m_edge=0.0192 base=23 pairs=9/9 (mask extrapolated from frame 51, delta=-14; max distance, 12 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 16 frames incl baseline)
idx=4   missing_texture    B_Manny_C_12           CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=measured
      run[75..82] CONSISTENT (caveat: high regional change m=0.4275 - readings under motion are less reliable) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 75 d=0.9999 tau=0.8523 label 75 delta=+0 ring d=0.1620 look at frames 74..76 m_edge=0.4275 base=16 pairs=9/9 (mask extrapolated from frame 75, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 5 frames incl baseline)
        end   TRANSITION 83 d=0.9987 tau=0.8523 label 83 delta=+0 ring d=0.0229 look at frames 82..84 m_edge=0.4275 base=16 pairs=9/9 (mask extrapolated from frame 75, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 7 frames incl baseline)
idx=5   lod_popping        B_Manny_C_12           UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=unmeasured
      UNASSESSABLE(non-manifested/empty event or no explicit frame_indices)
------------------------------------------------------------------------------
  CONSISTENT 4 (2 with caveat)  OFFSET-NOTE 1  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 1  READING 0  (of 6 events)
  run coverage             CONSISTENT 5  OFFSET-NOTE 1  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 6 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (4 consistent (2 with caveat), 1 offset-notes for human review, 0 partial, 1 unassessable, 0 readings)
```

### M50L_LG9

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M50L_LG9\session_20260904-195949
  frames / labels / events 90 / 90 / 6
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  regional change          m_edge is a reading; >0.42 adds a caveat (historical marker, not a boundary)
  characterisation         docs/verifier-characterisation.md
                           Per-session recovery readings and all wrong cells with competing peaks; known limitations, not label approval.
  clean-frame pool         24 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds; peaks >tau and >=1.5 x min(neighbours), outside window=0
  assignment               per target/mode, labelled order: nearest unused peak; tie larger d, then earlier; inversions disclosed
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              BakedStaticMeshActor_C_UAID_E04F43E623D48FEB00_1131370154 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=2} bbox=projected obs=unmeasured
      run[16..17] READING (caveat: high regional change m=0.4385 - readings under motion are less reliable) (bbox-only)
        onset UNASSESSABLE(threshold unsatisfiable: tau=1.1837 >= 1.0) label 16 m_edge=0.4385 base=24 pairs=7/7 (bbox-only)
        end   UNASSESSABLE(threshold unsatisfiable: tau=1.1837 >= 1.0) label 18 m_edge=0.4385 base=24 pairs=6/6 (bbox-only)
      run[21..22] READING (caveat: high regional change m=0.4376 - readings under motion are less reliable) (bbox-only)
        onset UNASSESSABLE(threshold unsatisfiable: tau=1.1535 >= 1.0) label 21 m_edge=0.4376 base=24 pairs=6/6 (bbox-only)
        end   UNASSESSABLE(threshold unsatisfiable: tau=1.1535 >= 1.0) label 23 m_edge=0.4376 base=24 pairs=7/7 (bbox-only)
idx=1   corrupted_texture  BakedStaticMeshActor_C_UAID_E04F43E623D418F900_1959252093 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=1} bbox=projected obs=unmeasured
      run[27..34] READING (bbox-only)
        onset NO-TRANSITION [23..31] tau=0.5789 label 27 m_edge=0.2710 base=24 pairs=9/9 (bbox-only)
        end   NO-TRANSITION [31..39] tau=0.5789 label 35 m_edge=0.2710 base=24 pairs=9/9 (bbox-only)
idx=2   blink              BakedStaticMeshActor_C_UAID_E04F43E623D47EF200_1779408643 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=2} bbox=projected obs=unmeasured
      run[40..41] READING (bbox-only)
        onset UNASSESSABLE(threshold unsatisfiable: tau=1.0507 >= 1.0) label 40 m_edge=0.4180 base=24 pairs=7/7 (bbox-only)
        end   UNASSESSABLE(threshold unsatisfiable: tau=1.0507 >= 1.0) label 42 m_edge=0.4180 base=24 pairs=6/6 (bbox-only)
      run[45..46] READING (caveat: high regional change m=0.4223 - readings under motion are less reliable) (bbox-only)
        onset NO-TRANSITION [42..47] tau=0.9725 label 45 m_edge=0.4223 base=24 pairs=6/6 (bbox-only)
        end   UNASSESSABLE(region covers the picture at frame 51) label 47 m_edge=n/a base=0 pairs=6/7 (bbox-only)
idx=3   missing_texture    BakedStaticMeshActor_C_UAID_E04F43E623D408E000_1155742006 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=1} bbox=projected obs=unmeasured
      run[51..58] READING (bbox-only)
        onset UNASSESSABLE(region covers the picture at frame 47) label 51 m_edge=n/a base=0 pairs=0/9 (bbox-only)
        end   UNASSESSABLE(region covers the picture at frame 55) label 59 m_edge=n/a base=0 pairs=1/9 (bbox-only)
idx=4   corrupted_texture  BakedStaticMeshActor_C_UAID_E04F43E623D418F900_1266499077 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=1} bbox=projected obs=unmeasured
      run[63..70] READING (caveat: high regional change m=0.4473 - readings under motion are less reliable) (bbox-only)
        onset NO-TRANSITION [59..67] tau=0.9558 label 63 m_edge=0.4473 base=24 pairs=9/9 (bbox-only)
        end   NO-TRANSITION [67..75] tau=0.9558 label 71 m_edge=0.4473 base=24 pairs=9/9 (bbox-only)
idx=5   missing_texture    BakedStaticMeshActor_C_UAID_E04F43E623D417F900_1074739879 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=1} bbox=projected obs=unmeasured
      run[87..89] READING (caveat: high regional change m=0.4426 - readings under motion are less reliable) (bbox-only)
        onset UNASSESSABLE(threshold unsatisfiable: tau=1.2314 >= 1.0) label 87 m_edge=0.4426 base=24 pairs=7/7 (bbox-only)
        end   UNASSESSABLE(edge truncated by the session boundary) label 90 m_edge=0.4426 base=24 pairs=3/3 (bbox-only)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 6  (of 6 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 8  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 0 unassessable, 6 readings)
  coverage                 UNREAD-BBOX-ONLY
```

## Final disposition

P2-prime passes. P1 has one preserved predeclared-reading mismatch; passing the corrected regression suite does not turn that prediction into a pass. The literal ruled implementation is ready for independent review, with all measured limitations retained.

VERDICT: NEEDS-DECISION
