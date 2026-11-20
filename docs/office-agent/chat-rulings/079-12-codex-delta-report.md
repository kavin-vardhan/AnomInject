# 079-12 — local peaks and ordered nearest assignment

**Disposition: NEEDS-DECISION. Implementation and authorized feature push complete; unmerged.**

R6′ is implemented as specified, but calibration derives **NO ADMISSIBLE ENVELOPE**:
wrong recovery cells exist even at baseline median0. The required final default
therefore refuses all otherwise-assessable edges. **Final P2′:0/64 edges observed;
all six pinned legs have four UNASSESSABLE events**, with tau=0.0040.

The separate pre-calibration reading at the prior numeric cap0.42 isolates R6′:
**60/64 observed:56 TRANSITION, four NO-TRANSITION, four UNASSESSABLE**. Five legs
have four CONSISTENT events; BL_SYN has four PARTIAL events from one-frame runs.
The four non-blink legs match 079-10's shared observation/numeric fields at that
same numeric cap. These diagnostic readings are not the final default readings.

**94/94 permanent fixtures and 18/18 contract tests pass the implemented rule.**
That does not make the supplied predictions pass: the missing-peak6 prediction
conflicts with the ordered walk, as AMENDMENT 2 recorded before implementation.
Exclusion also does not guarantee monotonic assigned frames when edge regions
admit different peaks. Both limitations remain explicit; no extra reservation,
ordering floor or peak-ratio tuning was added.

**Codex implemented; Claude Code independently reviews next; Chat orchestrates.**
**Chat review NOT REQUIRED** for the authorized implementation and push, now complete.
**Chat review REQUIRED** for this evidence, the prediction/monotonicity conflicts
and independent Code review before merge; this is the next mandatory checkpoint.
Client/Section G, m51 campaign and release holds remain.

Wall time: **37m 07s**, 2026-09-20 10:18:55 UTC to 2026-09-20 10:56:02 UTC.
This includes implementation, tests, calibration, commit/push and checkout restoration.
Monetary/token billing is unavailable; no charge is inferred.

## 1. Authority, revisions and scope

Authority: [079-12 ruling](D:/IntrusiveAnomalies/_reviews/079-12-codex-ruling-monotone-nearest-assignment.md),
superseding 079-11's withdrawn R6 while retaining 079-09/10's scope.
The [079-11 clarification report](D:/IntrusiveAnomalies/_reviews/079-11-codex-delta-report.md) is byte-unchanged
(SHA256 `687b9cc9fc9fd044b2d4ff207a3072150c6188312273af2ff92fe706bd8d511e`). Earlier reports and predictions remain historical.

| Revision | SHA |
| --- | --- |
| Feature base | e6351ce396c53ede5a7acd8186497811776e9e34 |
| AMENDMENT 2, committed before implementation/results | 1ea76240f388b6824dd74164f94bb3668b605663 |
| Implementation: local and remote fix/verifier-field-validity | 53e6e5154d4f36e3cfbcfa5ad524eac2fa092852 |
| Restored m51 | 53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67 |
| Unchanged master / origin-master | ac13700d99fe116056f5aa11611f8931a563a3f2 |
| Unchanged CaptureBench HEAD | 472a409dc70ae78d01a38d08fc3bddb15fe41c58 |

Both tracked trees are clean at the completed boundary. Source/, Shaders/ and
tools/measure_label_offset.py retain their base trees/blob. All ten pre-existing
plugin handoffs and seven other CaptureBench untracked tools retain their hashes.
Only the permitted local/untracked CaptureBench scorer changed: its NO-TRANSITION
reason now says “no local peak in labelled window”; its scoring, key set and cap
derivation are unchanged. CaptureBench has no remote. No mailbox/Code brief,
merge, tag, build, cook, capture, office-data transfer or owner-app action occurred.
No owned process or scratch junction remains in flight.

Boundary evidence: [preflight](D:/IntrusiveAnomalies/_reviews/079-12-evidence/preflight.json) and
[final audit](D:/IntrusiveAnomalies/_reviews/079-12-evidence/final/boundary.json). The report's frozen files remain
usable after restoration; m51's live verifier is an older version.

## 2. Implementation and interpretations

- Keep the existing per-edge mask union, labelled bounds, baselines, cap and hard
  refusals. A peak satisfies d(k)>tau and d(k)>=1.5*d(k−1), d(k)>=1.5*d(k+1).
  “Observed range” means the edge's scanned [lo..hi]; outside neighbours count as zero.
- Group all edges of all runs by (target, mode); sort by nominal frame, then onset
  before end at ties, then event index/run ordinal. Subtract previously selected
  frame numbers, choose nearest nominal frame, tie earlier, consume it. Hard-refused
  or session-truncated edges consume nothing. Detected frames do not clip windows.
- No peak gives NO-TRANSITION. A nonempty peak set entirely consumed gives
  `UNASSESSABLE(all peaks in window assigned to earlier edges: …)`.
  Ring values come from the selected peak. Same-line alternatives exclude already
  consumed peaks. Multi-peak run lines disclose both selections and the candidates
  available at each turn; `none` means no selection.
- Structured detail adds `peaks`, `available_peaks`, `consumed_peaks`, `other_peaks`.
  `eligible` now counts local peaks; `best_k`/`best_d` describe the actual selection.
  Obsolete global-winner `second_d` and `ratio` fields are removed.
- NO-TRANSITION is not absence of above-threshold change. The unchanged whole-span
  plus window check is still required for NO-TRACE. Producer-class, identity,
  mask extrapolation, run/event aggregation, caveats and exit contracts remain.

Implementation touches the verifier, contract tests, client guidance, prospective
office-card correction, dated G264 correction, CLAUDE.md current block and journal.
The predictions-only commit appends AMENDMENT 2 without rewriting earlier text.

[Full committed diff](D:/IntrusiveAnomalies/_reviews/079-12-evidence/final/implementation.diff)

## 3. Per-P readings

| Prediction | Reading / disposition |
| --- | --- |
| P1 — regression suite and ruled examples | 94/94 + 18/18 pass the literal rule. Supplied missing6 reading misses; four additional older expected event strings were corrected after first measurement, with original output preserved. |
| P2′ — all64 observed, four C per pinned leg | MISS: final default0/64 observed, all24 events UNASSESSABLE. Prior-cap diagnostic60/64 observed,56 transitions; BL_SYN4 PARTIAL. All tau.0040; non-blink fields match only at the prior cap. |
| P3 — three masked gameplay sessions + M50 | Completed at final derived default; full lines in Appendix A. No NO-TRACE. M50 retains6 READING events / UNREAD-BBOX-ONLY. |
| P4 — immutable572-key calibration | Completed OFF and ON; cap=NO ADMISSIBLE ENVELOPE, N=268 baseline-bearing perturbation cells. No tuning; tables and all ledgers below. |
| P5 — batch and exit contracts | Pass: flattened-path collisions retain both reports; forced key collision fails before writes; error3 survives report-only and valid siblings remain; direct CLI2/0/3 verified. |
| P6 — limitations and independent review | Cause/producer/noise-floor/extrapolation limits retained. Missing-peak and monotonicity conflicts disclosed. Independent Code review and Chat disposition pending. |

### P1: ruled examples and changed expectations

| Fixture | Selected peaks A.onset / A.end / B.onset / B.end | Run outcomes | Prediction status |
| --- | --- | --- | --- |
| r6_blink_aligned | 4 / 6 / 8 / 10; delta0 each | CONSISTENT / CONSISTENT | Supplied prediction met |
| r6_blink_late_second_onset | 4 / 6 / 8 / 10; B.onset delta−1 | CONSISTENT / OFFSET-NOTE | Supplied prediction met |
| r6_blink_missing_peak6 | 4 / 8 / 10 / none; delta0,+2,+2,n/a | OFFSET-NOTE / PARTIAL | Supplied PARTIAL / CONSISTENT misses; literal result predeclared |
| adjacent_late_second_onset (079-07 / 079-08 F4) | 10 / 12 / 15 / 17; B.onset delta−1 | CONSISTENT / OFFSET-NOTE | New supplied prediction met; no A.end15 / +3 reading |
| r6_blink_one_frame_second | 4 / 7 / none / none | CONSISTENT / PARTIAL | Added after the measured synthetic-bank miss |

For missing6, A.end's unchanged window[4..8] contains peaks4 and8. Frame4 is
consumed; frame8 is still free, so A.end must take8. B.onset then takes10;
B.end's peaks8/10 are exhausted. Nothing in the specified walk reserves8 for B.

Six old event-token changes were explicitly predicted in AMENDMENT 2. Four more
were measured in the first run: previous_end_refusal_masked, previous_end_control_masked,
partial_multirun_masked and mixed_runs. The first run and explanations were saved
before updating those known answers. The missing6 fixture's initial callback also
incorrectly required a printed alternative although none remained; only that
assertion was corrected. No peak or assignment rule changed after the first fixture
results; the default cap was subsequently rederived as predeclared.

**Every changed old event string (10 of90):**

| Fixture | 079-10 | 079-12 | Reason |
| --- | --- | --- | --- |
| c07_previous_end_refusal_masked | UNASSESSABLE | PARTIAL | Measured correction: three oversized-region refusals remain; B.end17 is now observed, giving B PARTIAL. |
| c07_previous_end_control_masked | PARTIAL | OFFSET-NOTE | Measured correction:10/13/15/17 yields A.end+1, then an aligned B. |
| c07_partial_multirun_masked | UNASSESSABLE | CONSISTENT | Measured correction: A10/12 is CONSISTENT; B stays UNASSESSABLE. R4 leaves B in coverage. |
| c07_mixed_runs | UNASSESSABLE | CONSISTENT | Measured correction: masked A10/12 is CONSISTENT; bbox B25/27 stays READING in its separate group. |
| c08_one_frame_late | PARTIAL | OFFSET-NOTE | Predeclared: onset has no peak, end61 survives at its window boundary with delta−1. |
| c08_one_frame_early | UNASSESSABLE | OFFSET-NOTE | Predeclared: onset60 survives at its window boundary with delta+1; end has no peak. |
| adjacent_control | UNASSESSABLE | CONSISTENT | Predeclared: isolated10/12/15/17 can each be assigned once at delta0. |
| adjacent_late_second_onset | PARTIAL | OFFSET-NOTE | Predeclared: A.end12 and B.onset15, delta−1, replace competing peak15 refusals. |
| adjacent_early_second_onset | UNASSESSABLE | OFFSET-NOTE | Predeclared:10/12/15/17; B.onset delta+1. |
| mixed_late_second_onset | PARTIAL | OFFSET-NOTE | Predeclared: separate bbox/mask groups choose10/12 and15/17; masked B has delta−1. |

**Every changed old edge/run reading (16 of90), including unchanged event tokens:** T=TRANSITION(frame, delta), N=NO-TRANSITION, U=UNASSESSABLE. Old refused candidate frames are not counted as observations.

| Fixture / reason | 079-10 edges; runs | 079-12 edges; runs |
| --- | --- | --- |
| c07_previous_end_refusal: Input refusals survive; B now takes its nearer15 and17 instead of old13 and an ambiguity refusal. | U(region covers the picture at frame 11); U(region covers the picture at frame 11); T13(-2); U(ambiguous: best=0.0900 second=0.0900 ratio=1.00 < 1.50); runs READING, READING | U(region covers the picture at frame 11); U(region covers the picture at frame 11); T15(+0); T17(+0); runs READING, READING |
| c07_partial_multirun: Separate10/12 peaks resolve A; oversized B remains unread. Bbox event remains READING. | U(transition 10 claimed by two edges (2 claimants)); U(transition 10 claimed by two edges (2 claimants)); U(region covers the picture at frame 21); U(region covers the picture at frame 25); runs READING, READING | T10(+0); T12(+0); U(region covers the picture at frame 21); U(region covers the picture at frame 25); runs READING, READING |
| c07_early_refusal_control: Separate10/12 peaks replace duplicate refusal; bbox event remains READING. | U(transition 10 claimed by two edges (2 claimants)); U(transition 10 claimed by two edges (2 claimants)); runs READING | T10(+0); T12(+0); runs READING |
| c07_previous_end_control: Ordered10/13/15/17 resolves duplicates; bbox event remains READING. | U(transition 10 claimed by two edges (2 claimants)); U(transition 10 claimed by two edges (2 claimants)); T13(-2); U(ambiguous: best=0.0900 second=0.0900 ratio=1.00 < 1.50); runs READING, READING | T10(+0); T13(+1); T15(+0); T17(+0); runs READING, READING |
| c07_previous_end_refusal_masked: Measured correction: three oversized-region refusals remain; B.end17 is now observed, giving B PARTIAL. | U(region covers the picture at frame 11); U(region covers the picture at frame 11); U(region covers the picture at frame 12); U(ambiguous: best=0.1406 second=0.1406 ratio=1.00 < 1.50); runs UNASSESSABLE, UNASSESSABLE | U(region covers the picture at frame 11); U(region covers the picture at frame 11); U(region covers the picture at frame 12); T17(+0); runs UNASSESSABLE, PARTIAL |
| c07_previous_end_control_masked: Measured correction:10/13/15/17 yields A.end+1, then an aligned B. | U(transition 10 claimed by two edges (2 claimants)); U(transition 10 claimed by two edges (2 claimants)); T13(-2); U(ambiguous: best=0.1406 second=0.1406 ratio=1.00 < 1.50); runs UNASSESSABLE, PARTIAL | T10(+0); T13(+1); T15(+0); T17(+0); runs OFFSET-NOTE, CONSISTENT |
| c07_partial_multirun_masked: Measured correction: A10/12 is CONSISTENT; B stays UNASSESSABLE. R4 leaves B in coverage. | U(transition 10 claimed by two edges (2 claimants)); U(transition 10 claimed by two edges (2 claimants)); U(region covers the picture at frame 25); U(region covers the picture at frame 25); runs UNASSESSABLE, UNASSESSABLE | T10(+0); T12(+0); U(region covers the picture at frame 25); U(region covers the picture at frame 25); runs CONSISTENT, UNASSESSABLE |
| c07_mixed_runs: Measured correction: masked A10/12 is CONSISTENT; bbox B25/27 stays READING in its separate group. | U(transition 10 claimed by two edges (2 claimants)); U(transition 10 claimed by two edges (2 claimants)); U(transition 25 claimed by two edges (2 claimants)); U(transition 25 claimed by two edges (2 claimants)); runs UNASSESSABLE, READING | T10(+0); T12(+0); T25(+0); T27(+0); runs CONSISTENT, READING |
| c08_occluder_aligned: Nearest end68 replaces stronger end66; onset61 stays+1, so event remains OFFSET-NOTE. | T61(+1); T66(-2); runs OFFSET-NOTE | T61(+1); T68(+0); runs OFFSET-NOTE |
| c08_one_frame_aligned: Adjacent equal changes60/61 suppress both peaks; whole-span above-tau change prevents NO-TRACE. | U(transition 60 claimed by two edges (2 claimants)); U(transition 60 claimed by two edges (2 claimants)); runs UNASSESSABLE | N; N; runs UNASSESSABLE |
| c08_one_frame_late: Predeclared: onset has no peak, end61 survives at its window boundary with delta−1. | U(ambiguous: best=0.0625 second=0.0625 ratio=1.00 < 1.50); T61(-1); runs PARTIAL | N; T61(-1); runs OFFSET-NOTE |
| c08_one_frame_early: Predeclared: onset60 survives at its window boundary with delta+1; end has no peak. | U(transition 60 claimed by two edges (2 claimants)); U(transition 60 claimed by two edges (2 claimants)); runs UNASSESSABLE | T60(+1); N; runs OFFSET-NOTE |
| adjacent_control: Predeclared: isolated10/12/15/17 can each be assigned once at delta0. | U(ambiguous: best=0.0400 second=0.0400 ratio=1.00 < 1.50); U(transition 15 claimed by two edges (3 claimants)); U(transition 15 claimed by two edges (3 claimants)); U(transition 15 claimed by two edges (3 claimants)); runs UNASSESSABLE, UNASSESSABLE | T10(+0); T12(+0); T15(+0); T17(+0); runs CONSISTENT, CONSISTENT |
| adjacent_late_second_onset: Predeclared: A.end12 and B.onset15, delta−1, replace competing peak15 refusals. | U(ambiguous: best=0.0400 second=0.0400 ratio=1.00 < 1.50); U(transition 15 claimed by two edges (2 claimants)); U(transition 15 claimed by two edges (2 claimants)); T17(+0); runs UNASSESSABLE, PARTIAL | T10(+0); T12(+0); T15(-1); T17(+0); runs CONSISTENT, OFFSET-NOTE |
| adjacent_early_second_onset: Predeclared:10/12/15/17; B.onset delta+1. | U(transition 10 claimed by two edges (2 claimants)); U(transition 10 claimed by two edges (2 claimants)); U(transition 15 claimed by two edges (2 claimants)); U(transition 15 claimed by two edges (2 claimants)); runs UNASSESSABLE, UNASSESSABLE | T10(+0); T12(+0); T15(+1); T17(+0); runs CONSISTENT, OFFSET-NOTE |
| mixed_late_second_onset: Predeclared: separate bbox/mask groups choose10/12 and15/17; masked B has delta−1. | U(ambiguous: best=0.0400 second=0.0400 ratio=1.00 < 1.50); T15(+3); U(ambiguous: best=0.3600 second=0.3600 ratio=1.00 < 1.50); T17(+0); runs READING, PARTIAL | T10(+0); T12(+0); T15(-1); T17(+0); runs READING, OFFSET-NOTE |

The other74 prior fixtures retain their observation/run/event readings. Both
lighting fixtures keep their outcomes: aligned OFFSET-NOTE and late-onset
CONSISTENT with a caveat. The occluder event stays OFFSET-NOTE with the changed
end observation shown above. Every prior fixture remains in the suite.

Evidence: [first run](D:/IntrusiveAnomalies/_reviews/079-12-evidence/initial/selftest.txt),
[measured corrections](D:/IntrusiveAnomalies/_reviews/079-12-evidence/initial/measured-expectation-corrections.md),
[all final fixture lines](D:/IntrusiveAnomalies/_reviews/079-12-evidence/final/selftest_full.txt),
[all structured fixture readings](D:/IntrusiveAnomalies/_reviews/079-12-evidence/final/selftest_details.json),
[18 contract tests](D:/IntrusiveAnomalies/_reviews/079-12-evidence/final/contracts.txt).

### P2′: six pinned legs

**Final default (NO ADMISSIBLE ENVELOPE):**

| Leg | Observed | T | N | U | Event outcomes | Tau |
| --- | --- | --- | --- | --- | --- | --- |
| M49_GEDGE_BL_NAT | 0 | 0 | 0 | 16 | UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE | 0.004 |
| M49_GEDGE_CT_NAT | 0 | 0 | 0 | 8 | UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE | 0.004 |
| M49_GEDGE_MO_NAT | 0 | 0 | 0 | 8 | UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE | 0.004 |
| M49_GEDGE_MT_NAT | 0 | 0 | 0 | 8 | UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE | 0.004 |
| M49_GEDGE_BL_SYN | 0 | 0 | 0 | 16 | UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE | 0.004 |
| M49_GEDGE_MT_SYN | 0 | 0 | 0 | 8 | UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE | 0.004 |

**Pre-calibration diagnostic, previous numeric cap0.42:**

| Leg | Observed | T | N | U | Event outcomes | Tau |
| --- | --- | --- | --- | --- | --- | --- |
| M49_GEDGE_BL_NAT | 16 | 16 | 0 | 0 | CONSISTENT, CONSISTENT, CONSISTENT, CONSISTENT | 0.004 |
| M49_GEDGE_CT_NAT | 8 | 8 | 0 | 0 | CONSISTENT, CONSISTENT, CONSISTENT, CONSISTENT | 0.004 |
| M49_GEDGE_MO_NAT | 8 | 8 | 0 | 0 | CONSISTENT, CONSISTENT, CONSISTENT, CONSISTENT | 0.004 |
| M49_GEDGE_MT_NAT | 8 | 8 | 0 | 0 | CONSISTENT, CONSISTENT, CONSISTENT, CONSISTENT | 0.004 |
| M49_GEDGE_BL_SYN | 12 | 8 | 4 | 4 | PARTIAL, PARTIAL, PARTIAL, PARTIAL | 0.004 |
| M49_GEDGE_MT_SYN | 8 | 8 | 0 | 0 | CONSISTENT, CONSISTENT, CONSISTENT, CONSISTENT | 0.004 |

At cap0.42, BL_NAT has eight CONSISTENT runs and four CONSISTENT events, all with caveats.
BL_SYN has four longer CONSISTENT runs and four one-frame PARTIAL runs, so each
event is PARTIAL; its caveated consistent runs remain printed even though the
consistent-event count is zero. Both use exactly the same ruled algorithm.

At BL_SYN's second run[10..10], onset window[7..11] has d10=0.5454314227149633
and d11=0.545536155123659, both far above tau0.004. Neither is1.5 times the other.
Only peak7 remains in that onset's original peak set, already assigned to A.end.
The end window[10..15] has no local peak. The same structure repeats at32/33,
54/55 and76/77. Cap tuning cannot create local peaks here: every baseline median
is0 and every edge tau is0.0040.

[Exact synthetic blink pair values and peak sets](D:/IntrusiveAnomalies/_reviews/079-12-evidence/synthetic_blink_peak_audit.json)

At the prior numeric cap, the four non-blink legs have **zero differences in all shared structured fields**
against 079-10 (32edges), including observation, selected frame/delta, d, tau,
baseline, window, ring, identity, extrapolation and run outcome. Removed global-
winner fields and new peak/disclosure fields are outside that schema intersection.
The final default refuses these edges, so that final comparison does NOT meet P2′.
Comparison (both stages): [bank comparison](D:/IntrusiveAnomalies/_reviews/079-12-evidence/bank_comparison.json).

### P3: gameplay and bbox-only control

| Session | 079-10 events | 079-12 events | Observed / total | Exit |
| --- | --- | --- | --- | --- |
| A2L_LEGA | UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, PARTIAL, CONSISTENT, CONSISTENT, UNASSESSABLE | UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE | 0/16 | 0 |
| LYRA_SMOKE_01 | UNASSESSABLE, CONSISTENT, UNASSESSABLE, UNASSESSABLE, CONSISTENT, UNASSESSABLE | UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE | 0/16 | 0 |
| A1L_LEGA | UNASSESSABLE, CONSISTENT, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE | UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, UNASSESSABLE | 0/12 | 0 |
| M50L_LG9 | READING, READING, READING, READING, READING, READING | READING, READING, READING, READING, READING, READING | 0/16 | 0 |

All final bank readings use the derived configured cap. Appendix A preserves every header, event, run, edge and summary line. No bank event produces NO-TRACE; M50 remains UNREAD-BBOX-ONLY.

**Pre-calibration diagnostic at0.42, retained separately:**

| Session | Event outcomes | Observed / total |
| --- | --- | --- |
| A2L_LEGA | UNASSESSABLE, UNASSESSABLE, UNASSESSABLE, CONSISTENT, CONSISTENT, CONSISTENT, UNASSESSABLE | 6/16 |
| LYRA_SMOKE_01 | CONSISTENT, CONSISTENT, UNASSESSABLE, PARTIAL, CONSISTENT, PARTIAL | 10/16 |
| A1L_LEGA | UNASSESSABLE, CONSISTENT, CONSISTENT, OFFSET-NOTE, UNASSESSABLE, UNASSESSABLE | 10/12 |
| M50L_LG9 | READING, READING, READING, READING, READING, READING | 2/16 |

### P4: calibration without a shrinking denominator

| Guard | Scored | Unscored | Recovered | Wrong | No-transition | Unassessable | Unsupported | Positive original edges |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| guard_off | 296 | 276 | 230 | 22 | 5 | 39 | 0 | 24 |
| guard_on | 0 | 572 | 0 | 0 | 0 | 0 | 0 | 0 |

**572 immutable keys per guard setting**: 208 A2L, 208 LYRA_SMOKE, 156 A1L,
44 original edges × 13 perturbation/variant combinations. Scored plus unscored
exactly reconciles to those keys; no READING/UNASSESSABLE run contributes scored
cells. Unsupported controls retain all 26 tiny keys, including 8 unsupported.
Rates, if compared, must use 572; positive support counts distinct original edges,
not perturbation cells or independently proven causal effects.

**Derived/default cap: NO ADMISSIBLE ENVELOPE; N=268 baseline-bearing perturbation cells.**
Derivation: the SMALLEST-m_edge cell is ALREADY WRONG (m=0.00000000) - no positive cap is defensible, so the answer is the explicit NO ADMISSIBLE ENVELOPE state and EVERY edge is refused. No threshold was changed to satisfy P2′.
The final verifier header includes this cohort's provenance.

| Wrong OFF cell | Expected | Observed | m_edge |
| --- | --- | --- | --- |
| A2L_LEGA event3 run[39, 46] end end-1/annotation | 1 | -1 | 0.0016533591418662794 |
| A2L_LEGA event5 run[75, 82] end end-1/annotation | 1 | -1 | 0.0 |
| A2L_LEGA event3 run[39, 46] end end-1/coherent | 1 | -1 | 0.0016533591418662794 |
| A2L_LEGA event5 run[75, 82] end end-1/coherent | 1 | -1 | 0.0 |
| A2L_LEGA event3 run[39, 46] end both-1/annotation | 1 | -1 | 0.0016533591418662794 |
| A2L_LEGA event5 run[75, 82] end both-1/annotation | 1 | -1 | 0.0 |
| A2L_LEGA event3 run[39, 46] end both-1/coherent | 1 | -1 | 0.0016214743153018121 |
| A2L_LEGA event5 run[75, 82] end both-1/coherent | 1 | -1 | 1.4551804423748545e-05 |
| LYRA_SMOKE_01 event3 run[51, 58] end both+0/annotation | 0 | 4 | 0.21171586715867158 |
| LYRA_SMOKE_01 event3 run[51, 58] end onset-1/annotation | 0 | 4 | 0.21220018450184502 |
| LYRA_SMOKE_01 event3 run[51, 58] end onset-1/coherent | 0 | 4 | 0.21220018450184502 |
| LYRA_SMOKE_01 event3 run[51, 58] end end+1/coherent | -1 | 3 | 0.20728782287822878 |
| LYRA_SMOKE_01 event3 run[51, 58] end both+1/coherent | -1 | 3 | 0.20285977859778598 |
| A1L_LEGA event3 run[51, 58] onset both+0/annotation | 0 | 4 | 0.019164213027532413 |
| A1L_LEGA event3 run[51, 58] onset onset+1/annotation | -1 | 3 | 0.03192006106446464 |
| A1L_LEGA event3 run[51, 58] onset onset+1/coherent | -1 | 3 | 0.03192006106446464 |
| A1L_LEGA event3 run[51, 58] onset end+1/annotation | 0 | 4 | 0.019661696747754816 |
| A1L_LEGA event3 run[51, 58] onset end+1/coherent | 0 | 4 | 0.021075468774213544 |
| A1L_LEGA event3 run[51, 58] onset end-1/annotation | 0 | 4 | 0.019891577887278685 |
| A1L_LEGA event3 run[51, 58] onset end-1/coherent | 0 | 4 | 0.019891577887278685 |
| A1L_LEGA event3 run[51, 58] onset both+1/annotation | -1 | 3 | 0.03591006869752272 |
| A1L_LEGA event3 run[51, 58] onset both+1/coherent | -1 | 3 | 0.02057720812625338 |

The zero-baseline boundary has a concrete nearest-tie explanation. In A2L event5
with end−1/annotation, the labelled clear is82; candidates81 and83 are equally
near. The specified tie rule takes81 (delta−1) although the injected perturbation
expects83 (delta+1). Their d values are0.04412661455734797 and0.9999445645545761;
both are local peaks above tau0.004, with baseline median0. The former global
winner would have preferred83, but R6′ deliberately selects by proximity and
earlier-frame tie. No positive regional cap can remove a wrong cell at median0.
The reproduced full candidate/pair evidence is
[zero_baseline_wrong_cell.json](D:/IntrusiveAnomalies/_reviews/079-12-evidence/zero_baseline_wrong_cell.json).
Its tie explanation was recorded before that diagnostic rerun.

Guard ON's zero wrong cells with zero scored cells supplies **no recovery
validation**: all572 planned keys remain explicitly unscored. The cap is installed
as the separate refuse-all state, not numeric0 and not the old0.42 fallback.

**guard_off: unscored reasons**

| Reason | Keys |
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
| UNASSESSABLE: whole-span change at frame 4 d=0.6067 > tau=0.0040 | 6 |
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
| UNASSESSABLE: whole-span change at frame 4 d=0.5424 > tau=0.0081 | 4 |
| UNASSESSABLE: whole-span change at frame 6 d=0.7735 > tau=0.7400 | 4 |
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
| UNASSESSABLE: NO-TRACE unavailable: mask id unresolved - sole value used | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 69; boundary mask unavailable: mask missing at frame 74 | 2 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=2.3006 >= 1.0 | 2 |
| UNASSESSABLE: whole-span change at frame 4 d=0.5424 > tau=0.0040 | 2 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (22 unavailable) | 1 |
| UNASSESSABLE: neither edge observed; edge truncated by the session boundary | 1 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (25 unavailable) | 1 |
| UNASSESSABLE: neither edge observed; mask missing at frame 47; boundary mask unavailable: mask missing at frame 50 | 1 |
| UNASSESSABLE: neither edge observed; mask missing at frame 85; boundary mask unavailable: mask missing at frame 86 | 1 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=1.9478 >= 1.0 | 1 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=2.5772 >= 1.0 | 1 |

**guard_off: scored non-recovery reasons**

| Reason | Cells |
| --- | --- |
| edge truncated by the session boundary | 8 |
| mask missing at frame 53 | 5 |
| no change above tau | 5 |
| mask missing at frame 83 | 4 |
| all peaks in window assigned to earlier edges: 87 | 3 |
| mask id 217 not present at frame 33; boundary mask unavailable: mask missing at frame 38 | 2 |
| mask id 218 not present at frame 45; boundary mask unavailable: mask missing at frame 50 | 2 |
| mask missing at frame 69; boundary mask unavailable: mask missing at frame 74 | 2 |
| mask missing at frame 59; boundary mask unavailable: mask missing at frame 62 | 2 |
| mask missing at frame 35 | 2 |
| mask missing at frame 9; boundary mask unavailable: mask missing at frame 14 | 2 |
| mask missing at frame 45; boundary mask unavailable: mask missing at frame 50 | 2 |
| mask missing at frame 23 | 2 |
| mask missing at frame 59 | 2 |
| mask missing at frame 47; boundary mask unavailable: mask missing at frame 50 | 1 |

**guard_on: unscored reasons**

| Reason | Keys |
| --- | --- |
| UNASSESSABLE: neither edge observed; NO ADMISSIBLE ENVELOPE | 278 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (14 unavailable) | 20 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (16 unavailable) | 16 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (17 unavailable) | 16 |
| UNASSESSABLE: neither edge observed; mask missing at frame 53 | 12 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (13 unavailable) | 11 |
| UNASSESSABLE: neither edge observed; mask missing at frame 35 | 10 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (16 unavailable) | 9 |
| UNASSESSABLE: neither edge observed; edge truncated by the session boundary | 9 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (15 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 6 | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 23 | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 30 | 8 |
| UNASSESSABLE: neither edge observed; baseline: only 2 valid clean pairs, need 3 (25 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (22 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; baseline: only 2 valid clean pairs, need 3 (24 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 47 | 6 |
| UNASSESSABLE: neither edge observed; mask missing at frame 59 | 6 |
| UNASSESSABLE: neither edge observed; baseline: only 2 valid clean pairs, need 3 (15 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 0; boundary mask unavailable: mask missing at frame 3 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 2; boundary mask unavailable: mask missing at frame 3 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 23; boundary mask unavailable: mask missing at frame 27 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 26; boundary mask unavailable: mask missing at frame 27 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 30; boundary mask unavailable: mask missing at frame 32 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 31; boundary mask unavailable: mask missing at frame 32 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 69; boundary mask unavailable: mask missing at frame 74 | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (13 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 11 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 83 | 4 |
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
| UNASSESSABLE: neither edge observed; mask id 217 not present at frame 33; boundary mask unavailable: mask missing at frame 38 | 2 |
| UNASSESSABLE: neither edge observed; mask id 218 not present at frame 45; boundary mask unavailable: mask missing at frame 50 | 2 |
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
| UNASSESSABLE: neither edge observed; mask missing at frame 47; boundary mask unavailable: mask missing at frame 50 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 59; boundary mask unavailable: mask missing at frame 62 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 81; boundary mask unavailable: mask missing at frame 86 | 2 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (23 unavailable) | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 71 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 9; boundary mask unavailable: mask missing at frame 14 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 45; boundary mask unavailable: mask missing at frame 50 | 2 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=2.3006 >= 1.0 | 2 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (22 unavailable) | 1 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (25 unavailable) | 1 |
| UNASSESSABLE: neither edge observed; mask missing at frame 85; boundary mask unavailable: mask missing at frame 86 | 1 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=1.9478 >= 1.0 | 1 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=2.5772 >= 1.0 | 1 |

**guard_on: scored non-recovery reasons**

| Reason | Cells |
| --- | --- |

Full tables/progress: [scorer output](D:/IntrusiveAnomalies/_reviews/079-12-evidence/scorer_v3.txt).
Every scored/unscored key and value: [cohort_v3.json](D:/IntrusiveAnomalies/_reviews/079-12-evidence/cohort_v3.json).
The frozen sweep scorer's legacy reason `no change above tau` is textually stale
under R6′; interpret those **NO-TRANSITION** cells as no qualifying local peak.
The final local scorer corrects that reason without changing scoring or derivation.
The raw calibration ledger is preserved, not rewritten to hide the wording change.

Calibration ran once against [the frozen sweep tools](D:/IntrusiveAnomalies/_reviews/079-12-sweep-tools/manifest.json).
Afterward, verifier changes are limited to cap/provenance, CLI help wording and the
added synthetic-blink regression; scorer changes are module documentation and the reason string.
An AST comparison proves all calibration logic otherwise identical, including
regions/identity/baselines, local peaks, assignment, eligibility, scoring and cap
derivation. Both sweep phases explicitly pass their cap. No second full sweep is
claimed. Final regressions, scorer controls/CLI and default-cap bank run afterward.
Audit: [source equivalence](D:/IntrusiveAnomalies/_reviews/079-12-evidence/final/source_equivalence.json) and its linked diffs.

### P5 and P6: contracts and remaining decisions

[Complete checks](D:/IntrusiveAnomalies/_reviews/079-12-evidence/final/complete_checks.json) verify scorer recovered,
wrong, no-transition and unassessable classes; [scorer self-test](D:/IntrusiveAnomalies/_reviews/079-12-evidence/final/scorer_selftest.txt)
verifies bbox-only exclusion, partially masked eligibility, unsupported denominators
and numeric-zero/refuse-all/undetermined cap distinctions. Direct CLI exits are
NO-TRACE2, report-only0, execution error with report-only3. The18contract tests
include the old9batch/region/mask contracts plus9peak/assignment contracts.
Semantic fixtures use explicit cap1, as AMENDMENT1 predeclared, with separate
numeric/zero/refuse-all guard cases. Passing those fixtures does not imply that
the final refuse-all default provides usable observations.

The non-monotonic contract uses two separate labelled runs10..11 and15..16.
A.end window[10..15] admits only15; B.onset window[12..17] admits only12 in its
different region. The literal walk assigns15 then12, without duplication.
This is a controlled per-pair signal contract, not a claim about the bank's geometry.
It disproves the general monotonicity assertion without adding the forbidden
ordering constraint. The supplied algorithm remains implemented exactly.

Chat's pending decisions are the zero-baseline wrong cells and resulting refuse-all
default, the synthetic one-frame P2′ miss even at the old numeric cap, the missing6
prediction conflict, and the monotonicity claim. No new brief is dispatched.
Independent Code review is pending; implementation self-review is not presented
as independent approval. No fresh Codex session or context-setting change is
recommended for this same workstream. CURRENT.md preserves the completed state
and links the current report, predictions and evidence.

## 4. Reproduction and source provenance

Use frozen final tools after checkout restoration (PowerShell):

```powershell
$env:IAI_VERIFIER_TOOLS='D:\IntrusiveAnomalies\_reviews\079-12-frozen'
python -B D:\IntrusiveAnomalies\_reviews\079-12-frozen\verify_capture.py --label-pixel-gate --selftest
python -B D:\IntrusiveAnomalies\_reviews\079-12-frozen\test_verify_capture_consistency.py
python -B D:\IntrusiveAnomalies\_reviews\079-12-frozen\m079_shift_recovery.py --selftest
python -B D:\IntrusiveAnomalies\_reviews\079-12-frozen\verify_capture.py --dir D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_BL_SYN\session_20260904-135302 --label-pixel-gate
```

To reproduce the entire calibration, run that frozen scorer without `--selftest`
and with a NEW `--json` output path. Its default three bank sessions and planned
keys are unchanged. Preserve this round's original artifacts.

| File | Final SHA256 |
| --- | --- |
| verify_capture.py | 367c4411f71c1e3ff0fe6f7be651863812d7e2a0503ce356cf05a86fdc58db8e |
| measure_label_offset.py | 15538658dda9c14df791b6d4b1e746c4ffef94519ac9f7b59989db62ab9933d1 |
| test_verify_capture_consistency.py | 649cd5c928c5ec438868d1160a5d65cab5a44e0780f3a39630b01218ddbc178b |
| CLAUDE.md | 084ae2cff945b8dfba39ced502f165374de010b1c3c568b803a715ef35a30db3 |
| client-readme.md | 2e4593b76f9bab46b3748817008dce74759a6b8973be85546fc44c4790f6c554 |
| office-rdp-card.md | 32b419b7f711301b25319a7c43a71eefbf29e480dfd2182b157c10badf6ae752 |
| gotchas.md | 6242616805c8ea68897e667ef4b517001ee2b99376d78858c52e2098f1ed1dc5 |
| 2026-09-20-verifier-consistency-semantics.md | cdf839186a689854c300537f33ba88a330521d90f9fe66acffa4b184413340a5 |
| 2026-09-20-079-12-codex-delta.md | 77798b2eb9ead7ba159019729456f1d0f05f9687398d19884be5919d71bcdd71 |
| m079_shift_recovery.py | 807391cbd804bf7a57aa94ceba87f3680be6896e188a2113440ba1346d81c854 |

Sweep source SHA256: verifier `78ee720690c001f8258d4b4d356e0e024f5bbd55dccb57ed2261802ffcc11efe`; scorer `ebef3d458689428c000896b5211487c7deb195ba28df99f2015961d0c931f215`. Final artifact manifest: [manifest.json](D:/IntrusiveAnomalies/_reviews/079-12-frozen/manifest.json).

| Evidence | SHA256 |
| --- | --- |
| cohort_v3.json | 8f5b4bc253afc9073fded0dcc56fc145761eff92c99566e5ec2ee0ee5bf676bb |
| bank.json | ab0ed1b8ab264bc7e04828d36d3214d425d8532ac68cf59cabaac3b0956747c1 |
| selftest_details.json | bb13aa858095c4c54ff4157efea3316a7023632c7d9b2f6f980435bd9c6c9501 |
| complete_checks.json | 0a1f44d913d6f82564a99e00f599660a8440c108f4f3a33f252eb7e534fed096 |
| source_equivalence.json | 5c36b9b29affb12d9a3800f60b0886797ab61ae571ad9bb57f7ddbfa088ae0ab |

## Appendix A — complete final bank readings

### M49_GEDGE_BL_NAT

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_BL_NAT\session_20260904-135206
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        NO ADMISSIBLE ENVELOPE
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         41 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[4..5] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=18 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 4 m_edge=0.0000 base=11 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 5, delta=+18; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 6 m_edge=0.0000 base=11 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 3 frames incl baseline) (mask extrapolated from frame 5, delta=+18; max distance, 14 frames incl baseline)
      run[9..10] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 9 m_edge=0.0000 base=14 pairs=6/6 (mask extrapolated from frame 9, delta=-9; max distance, 5 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 16 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 11 m_edge=0.0000 base=14 pairs=7/7 (mask extrapolated from frame 9, delta=-9; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 17 frames incl baseline)
idx=1   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[26..27] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=26 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 26 m_edge=0.0000 base=21 pairs=7/7 (mask extrapolated from frame 26, delta=-26; max distance, 15 frames incl baseline) (mask extrapolated from frame 27, delta=+18; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 28 m_edge=0.0000 base=21 pairs=6/6 (mask extrapolated from frame 26, delta=-26; max distance, 14 frames incl baseline) (mask extrapolated from frame 27, delta=+18; max distance, 14 frames incl baseline)
      run[31..32] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 31 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 31, delta=-18; max distance, 14 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 16 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 33 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 31, delta=-18; max distance, 12 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 17 frames incl baseline)
idx=2   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[48..49] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 48 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 48, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 49, delta=+18; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 50 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 48, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 49, delta=+18; max distance, 14 frames incl baseline)
      run[53..54] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 53 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 53, delta=-18; max distance, 14 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 16 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 55 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 53, delta=-18; max distance, 12 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 17 frames incl baseline)
idx=3   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[70..71] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 70 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 70, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 71, delta=+18; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 72 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 70, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 71, delta=+18; max distance, 14 frames incl baseline)
      run[75..76] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=18 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 75 m_edge=0.0000 base=20 pairs=6/6 (mask extrapolated from frame 75, delta=-18; max distance, 14 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 77 m_edge=0.0000 base=20 pairs=7/7 (mask extrapolated from frame 75, delta=-18; max distance, 12 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 8  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

### M49_GEDGE_CT_NAT

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_CT_NAT\session_20260904-135137
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        NO ADMISSIBLE ENVELOPE
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   corrupted_texture  StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[3..10] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 3 m_edge=0.0000 base=12 pairs=7/7 (mask id unresolved - sole value used) (mask extrapolated from frame 3, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 11 m_edge=0.0000 base=12 pairs=9/9 (mask extrapolated from frame 10, delta=+28; max distance, 16 frames incl baseline)
idx=1   corrupted_texture  StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[25..32] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 25 m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 12 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 33 m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 10 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 16 frames incl baseline)
idx=2   corrupted_texture  StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[47..54] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 47 m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 55 m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 16 frames incl baseline)
idx=3   corrupted_texture  StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[69..76] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 69 m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 11 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 77 m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

### M49_GEDGE_MO_NAT

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_MO_NAT\session_20260904-135147
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        NO ADMISSIBLE ENVELOPE
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   missing_object     StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[3..10] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 3 m_edge=0.0000 base=12 pairs=7/7 (mask id unresolved - sole value used) (mask extrapolated from frame 3, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 11 m_edge=0.0000 base=12 pairs=9/9 (mask extrapolated from frame 10, delta=+28; max distance, 16 frames incl baseline)
idx=1   missing_object     StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[25..32] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 25 m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 12 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 33 m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 10 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 16 frames incl baseline)
idx=2   missing_object     StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[47..54] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 47 m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 55 m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 16 frames incl baseline)
idx=3   missing_object     StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[69..76] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 69 m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 11 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 77 m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

### M49_GEDGE_MT_NAT

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_MT_NAT\session_20260904-135030
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        NO ADMISSIBLE ENVELOPE
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   missing_texture    StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[3..10] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 3 m_edge=0.0000 base=12 pairs=7/7 (mask id unresolved - sole value used) (mask extrapolated from frame 3, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 11 m_edge=0.0000 base=12 pairs=9/9 (mask extrapolated from frame 10, delta=+28; max distance, 16 frames incl baseline)
idx=1   missing_texture    StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[25..32] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 25 m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 12 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 33 m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 10 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 16 frames incl baseline)
idx=2   missing_texture    StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[47..54] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 47 m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 55 m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 16 frames incl baseline)
idx=3   missing_texture    StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[69..76] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 69 m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 11 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 77 m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

### M49_GEDGE_BL_SYN

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_BL_SYN\session_20260904-135302
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        NO ADMISSIBLE ENVELOPE
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         41 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[4..6] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=17 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 4 m_edge=0.0000 base=11 pairs=7/7 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 6, delta=+17; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 7 m_edge=0.0000 base=11 pairs=7/7 (mask extrapolated from frame 4, delta=-4; max distance, 3 frames incl baseline) (mask extrapolated from frame 6, delta=+17; max distance, 14 frames incl baseline)
      run[10..10] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 10 m_edge=0.0000 base=14 pairs=5/5 (mask extrapolated from frame 10, delta=+28; max distance, 21 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 11 m_edge=0.0000 base=14 pairs=6/6 (mask extrapolated from frame 10, delta=+28; max distance, 20 frames incl baseline)
idx=1   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[26..28] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=26 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 26 m_edge=0.0000 base=21 pairs=8/8 (mask extrapolated from frame 26, delta=-26; max distance, 15 frames incl baseline) (mask extrapolated from frame 28, delta=+17; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 29 m_edge=0.0000 base=21 pairs=7/7 (mask extrapolated from frame 26, delta=-26; max distance, 14 frames incl baseline) (mask extrapolated from frame 28, delta=+17; max distance, 14 frames incl baseline)
      run[32..32] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 32 m_edge=0.0000 base=23 pairs=5/5 (mask extrapolated from frame 32, delta=+28; max distance, 30 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 33 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 32, delta=+28; max distance, 29 frames incl baseline)
idx=2   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[48..50] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 48 m_edge=0.0000 base=23 pairs=8/8 (mask extrapolated from frame 48, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 50, delta=+17; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 51 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 48, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 50, delta=+17; max distance, 14 frames incl baseline)
      run[54..54] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 54 m_edge=0.0000 base=23 pairs=5/5 (mask extrapolated from frame 54, delta=+28; max distance, 30 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 55 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 54, delta=+28; max distance, 29 frames incl baseline)
idx=3   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[70..72] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 70 m_edge=0.0000 base=23 pairs=8/8 (mask extrapolated from frame 70, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 72, delta=+17; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 73 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 70, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 72, delta=+17; max distance, 14 frames incl baseline)
      run[76..76] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=19 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 76 m_edge=0.0000 base=20 pairs=5/5 (mask extrapolated from frame 76, delta=-19; max distance, 26 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 77 m_edge=0.0000 base=20 pairs=6/6 (mask extrapolated from frame 76, delta=-19; max distance, 25 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 8  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

### M49_GEDGE_MT_SYN

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_MT_SYN\session_20260904-135223
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        NO ADMISSIBLE ENVELOPE
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   missing_texture    StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[3..10] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 3 m_edge=0.0000 base=12 pairs=7/7 (mask id unresolved - sole value used) (mask extrapolated from frame 3, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 11 m_edge=0.0000 base=12 pairs=9/9 (mask extrapolated from frame 10, delta=+28; max distance, 16 frames incl baseline)
idx=1   missing_texture    StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[25..32] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 25 m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 12 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 33 m_edge=0.0000 base=21 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 25, delta=-12; max distance, 10 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 16 frames incl baseline)
idx=2   missing_texture    StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[47..54] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 47 m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 55 m_edge=0.0000 base=24 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 47, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 16 frames incl baseline)
idx=3   missing_texture    StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[69..76] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 69 m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 11 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 77 m_edge=0.0000 base=22 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 69, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

### A2L_LEGA

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\A2L_LEGA\session_20260904-180418
  frames / labels / events 90 / 90 / 7
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        NO ADMISSIBLE ENVELOPE
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         15 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
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
idx=3   missing_texture    B_Manny_C_7            UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=drawn obs=measured
      run[39..46] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=26 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 39 m_edge=0.0024 base=11 pairs=9/9 (mask extrapolated from frame 39, delta=-5; max distance, 5 frames incl baseline) (mask extrapolated from frame 46, delta=+26; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 47 m_edge=0.0024 base=11 pairs=9/9 (mask extrapolated from frame 46, delta=+26; max distance, 17 frames incl baseline)
idx=4   corrupted_texture  Cube2_8                UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=drawn obs=measured
      run[51..58] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 51 m_edge=0.0001 base=12 pairs=9/9 (mask extrapolated from frame 51, delta=-5; max distance, 5 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 59 m_edge=0.0001 base=12 pairs=9/9 (mask extrapolated from frame 58, delta=+28; max distance, 16 frames incl baseline)
idx=5   missing_texture    Cube2_8                UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=drawn obs=measured
      run[75..82] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=14 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 75 m_edge=0.0000 base=15 pairs=9/9 (mask extrapolated from frame 75, delta=-14; max distance, 14 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 5 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 83 m_edge=0.0000 base=15 pairs=9/9 (mask extrapolated from frame 75, delta=-14; max distance, 12 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 7 frames incl baseline)
idx=6   lod_popping        B_Manny_C_7            UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=projected obs=unmeasured
      UNASSESSABLE(non-manifested/empty event or no explicit frame_indices)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 7  READING 0  (of 7 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 8  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 7 unassessable, 0 readings)
```

### LYRA_SMOKE_01

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\LYRA_SMOKE_01\session_20260904-151503
  frames / labels / events 90 / 90 / 6
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        NO ADMISSIBLE ENVELOPE
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         24 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              Cube7                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[16..17] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=16 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 16 m_edge=0.0036 base=13 pairs=7/7 (mask extrapolated from frame 16, delta=-16; max distance, 16 frames incl baseline) (mask extrapolated from frame 17, delta=+1; max distance, 1 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 18 m_edge=0.0036 base=13 pairs=6/6 (mask extrapolated from frame 16, delta=-16; max distance, 15 frames incl baseline) (mask extrapolated from frame 17, delta=+3; max distance, 3 frames incl baseline)
      run[21..22] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=21 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 21 m_edge=0.0036 base=13 pairs=6/6 (mask extrapolated from frame 21, delta=-21; max distance, 17 frames incl baseline) (mask extrapolated from frame 22, delta=+1; max distance, 1 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 23 m_edge=0.0036 base=13 pairs=7/7 (mask extrapolated from frame 21, delta=-21; max distance, 15 frames incl baseline) (mask extrapolated from frame 22, delta=+5; max distance, 5 frames incl baseline)
idx=1   corrupted_texture  Cube4                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[27..34] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=27 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 27 m_edge=0.0612 base=13 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 27, delta=-27; max distance, 18 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 35 m_edge=0.0612 base=13 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 27, delta=-27; max distance, 14 frames incl baseline) (mask extrapolated from frame 34, delta=+5; max distance, 5 frames incl baseline)
idx=2   blink              Cube4                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[40..41] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 40 m_edge=n/a base=1 pairs=7/7 (mask extrapolated from frame 40, delta=-28; max distance, 7 frames incl baseline) (mask extrapolated from frame 41, delta=+1; max distance, 1 frames incl baseline)
        end   UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 42 m_edge=n/a base=1 pairs=6/6 (mask extrapolated from frame 40, delta=-28; max distance, 3 frames incl baseline) (mask extrapolated from frame 41, delta=+3; max distance, 3 frames incl baseline)
      run[45..46] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 45 m_edge=n/a base=1 pairs=6/6 (mask extrapolated from frame 45, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 46, delta=+28; max distance, 3 frames incl baseline)
        end   UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 47 m_edge=n/a base=1 pairs=7/7 (mask extrapolated from frame 45, delta=-1; max distance, 1 frames incl baseline) (mask extrapolated from frame 46, delta=+28; max distance, 7 frames incl baseline)
idx=3   missing_texture    B_Manny_C_14           UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[51..58] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=26 (including baseline)
        onset UNASSESSABLE(mask missing at frame 53) label 51 m_edge=n/a base=0 pairs=7/9 (mask id unresolved - sole value used) (mask extrapolated from frame 51, delta=-4; max distance, 4 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 59 m_edge=0.2117 base=11 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 58, delta=+26; max distance, 16 frames incl baseline)
idx=4   corrupted_texture  Cube3                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[63..70] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=14 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 63 m_edge=0.0187 base=11 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 63, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 70, delta=+14; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 71 m_edge=0.0187 base=11 pairs=9/9 (mask extrapolated from frame 70, delta=+14; max distance, 14 frames incl baseline)
idx=5   missing_texture    Cube3                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[87..89] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=14 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 87 m_edge=0.0291 base=11 pairs=7/7 (mask id unresolved - sole value used) (mask extrapolated from frame 87, delta=-14; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(edge truncated by the session boundary) label 90 m_edge=0.0291 base=11 pairs=3/3 (mask id unresolved - sole value used) (mask extrapolated from frame 87, delta=-14; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 6  READING 0  (of 6 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 8  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 6 unassessable, 0 readings)
```

### A1L_LEGA

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\A1L_LEGA\session_20260904-165004
  frames / labels / events 90 / 90 / 6
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        NO ADMISSIBLE ENVELOPE
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         30 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              Cube7                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=measured
      run[4..5] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=7 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 4 m_edge=0.0000 base=4 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 5, delta=+7; max distance, 6 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 6 m_edge=0.0000 base=4 pairs=7/7 (mask extrapolated from frame 4, delta=-1; max distance, 1 frames incl baseline) (mask extrapolated from frame 5, delta=+7; max distance, 7 frames incl baseline)
idx=1   corrupted_texture  Cube6_0                UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=measured
      run[15..22] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=26 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 15 m_edge=0.1036 base=15 pairs=9/9 (mask extrapolated from frame 15, delta=-7; max distance, 7 frames incl baseline) (mask extrapolated from frame 22, delta=+26; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 23 m_edge=0.1036 base=15 pairs=9/9 (mask extrapolated from frame 15, delta=-7; max distance, 5 frames incl baseline) (mask extrapolated from frame 22, delta=+26; max distance, 17 frames incl baseline)
idx=2   blink              Cube6_0                UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=measured
      run[28..29] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=20 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 28 m_edge=0.1083 base=15 pairs=7/7 (mask extrapolated from frame 28, delta=-20; max distance, 10 frames incl baseline) (mask extrapolated from frame 29, delta=+19; max distance, 13 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 30 m_edge=0.1083 base=15 pairs=6/6 (mask extrapolated from frame 28, delta=-20; max distance, 6 frames incl baseline) (mask extrapolated from frame 29, delta=+19; max distance, 15 frames incl baseline)
      run[33..34] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 33 m_edge=0.1081 base=16 pairs=6/6 (mask extrapolated from frame 33, delta=-25; max distance, 8 frames incl baseline) (mask extrapolated from frame 34, delta=+28; max distance, 15 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 35 m_edge=0.1081 base=16 pairs=7/7 (mask extrapolated from frame 33, delta=-25; max distance, 6 frames incl baseline) (mask extrapolated from frame 34, delta=+28; max distance, 16 frames incl baseline)
idx=3   corrupted_texture  Cube2_8                UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=measured
      run[51..58] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 51 m_edge=0.0192 base=23 pairs=9/9 (mask extrapolated from frame 51, delta=-14; max distance, 14 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 59 m_edge=0.0192 base=23 pairs=9/9 (mask extrapolated from frame 51, delta=-14; max distance, 12 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 16 frames incl baseline)
idx=4   missing_texture    B_Manny_C_12           UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=measured
      run[75..82] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 75 m_edge=0.4275 base=16 pairs=9/9 (mask extrapolated from frame 75, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 5 frames incl baseline)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 83 m_edge=0.4275 base=16 pairs=9/9 (mask extrapolated from frame 75, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 7 frames incl baseline)
idx=5   lod_popping        B_Manny_C_12           UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=unmeasured
      UNASSESSABLE(non-manifested/empty event or no explicit frame_indices)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 6  READING 0  (of 6 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 6  READING 0  (of 6 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 6 unassessable, 0 readings)
```

### M50L_LG9

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M50L_LG9\session_20260904-195949
  frames / labels / events 90 / 90 / 6
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        NO ADMISSIBLE ENVELOPE
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         24 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              BakedStaticMeshActor_C_UAID_E04F43E623D48FEB00_1131370154 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=2} bbox=projected obs=unmeasured
      run[16..17] READING (bbox-only)
        onset UNASSESSABLE(threshold unsatisfiable: tau=1.1837 >= 1.0) label 16 m_edge=0.4385 base=24 pairs=7/7 (bbox-only)
        end   UNASSESSABLE(threshold unsatisfiable: tau=1.1837 >= 1.0) label 18 m_edge=0.4385 base=24 pairs=6/6 (bbox-only)
      run[21..22] READING (bbox-only)
        onset UNASSESSABLE(threshold unsatisfiable: tau=1.1535 >= 1.0) label 21 m_edge=0.4376 base=24 pairs=6/6 (bbox-only)
        end   UNASSESSABLE(threshold unsatisfiable: tau=1.1535 >= 1.0) label 23 m_edge=0.4376 base=24 pairs=7/7 (bbox-only)
idx=1   corrupted_texture  BakedStaticMeshActor_C_UAID_E04F43E623D418F900_1959252093 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=1} bbox=projected obs=unmeasured
      run[27..34] READING (bbox-only)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 27 m_edge=0.2710 base=24 pairs=9/9 (bbox-only)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 35 m_edge=0.2710 base=24 pairs=9/9 (bbox-only)
idx=2   blink              BakedStaticMeshActor_C_UAID_E04F43E623D47EF200_1779408643 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=2} bbox=projected obs=unmeasured
      run[40..41] READING (bbox-only)
        onset UNASSESSABLE(threshold unsatisfiable: tau=1.0507 >= 1.0) label 40 m_edge=0.4180 base=24 pairs=7/7 (bbox-only)
        end   UNASSESSABLE(threshold unsatisfiable: tau=1.0507 >= 1.0) label 42 m_edge=0.4180 base=24 pairs=6/6 (bbox-only)
      run[45..46] READING (bbox-only)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 45 m_edge=0.4223 base=24 pairs=6/6 (bbox-only)
        end   UNASSESSABLE(region covers the picture at frame 51) label 47 m_edge=n/a base=0 pairs=6/7 (bbox-only)
idx=3   missing_texture    BakedStaticMeshActor_C_UAID_E04F43E623D408E000_1155742006 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=1} bbox=projected obs=unmeasured
      run[51..58] READING (bbox-only)
        onset UNASSESSABLE(region covers the picture at frame 47) label 51 m_edge=n/a base=0 pairs=0/9 (bbox-only)
        end   UNASSESSABLE(region covers the picture at frame 55) label 59 m_edge=n/a base=0 pairs=1/9 (bbox-only)
idx=4   corrupted_texture  BakedStaticMeshActor_C_UAID_E04F43E623D418F900_1266499077 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=1} bbox=projected obs=unmeasured
      run[63..70] READING (bbox-only)
        onset UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 63 m_edge=0.4473 base=24 pairs=9/9 (bbox-only)
        end   UNASSESSABLE(NO ADMISSIBLE ENVELOPE) label 71 m_edge=0.4473 base=24 pairs=9/9 (bbox-only)
idx=5   missing_texture    BakedStaticMeshActor_C_UAID_E04F43E623D417F900_1074739879 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=1} bbox=projected obs=unmeasured
      run[87..89] READING (bbox-only)
        onset UNASSESSABLE(threshold unsatisfiable: tau=1.2314 >= 1.0) label 87 m_edge=0.4426 base=24 pairs=7/7 (bbox-only)
        end   UNASSESSABLE(edge truncated by the session boundary) label 90 m_edge=0.4426 base=24 pairs=3/3 (bbox-only)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 6  (of 6 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 8  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 0 unassessable, 6 readings)
  coverage                 UNREAD-BBOX-ONLY
```

## Appendix A2 — complete pre-calibration readings at cap0.42 (diagnostic)

### M49_GEDGE_BL_NAT

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_BL_NAT\session_20260904-135206
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        0.42
                           079-10 v3 default cap 0.42 is PROVISIONAL: NO DISAGREEING CELL OBSERVED; floored from max m_edge=0.42753695, extrapolation not a boundary. Three masked sessions; N=162 perturbation cells, not independent edges; 178/572 scored with guard off, 164/572 with guard on. With wrong cells the rule is a two-significant-figure cut-off strictly below their smallest m_edge.
  clean-frame pool         41 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
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
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_CT_NAT\session_20260904-135137
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        0.42
                           079-10 v3 default cap 0.42 is PROVISIONAL: NO DISAGREEING CELL OBSERVED; floored from max m_edge=0.42753695, extrapolation not a boundary. Three masked sessions; N=162 perturbation cells, not independent edges; 178/572 scored with guard off, 164/572 with guard on. With wrong cells the rule is a two-significant-figure cut-off strictly below their smallest m_edge.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
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
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_MO_NAT\session_20260904-135147
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        0.42
                           079-10 v3 default cap 0.42 is PROVISIONAL: NO DISAGREEING CELL OBSERVED; floored from max m_edge=0.42753695, extrapolation not a boundary. Three masked sessions; N=162 perturbation cells, not independent edges; 178/572 scored with guard off, 164/572 with guard on. With wrong cells the rule is a two-significant-figure cut-off strictly below their smallest m_edge.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
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
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_MT_NAT\session_20260904-135030
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        0.42
                           079-10 v3 default cap 0.42 is PROVISIONAL: NO DISAGREEING CELL OBSERVED; floored from max m_edge=0.42753695, extrapolation not a boundary. Three masked sessions; N=162 perturbation cells, not independent edges; 178/572 scored with guard off, 164/572 with guard on. With wrong cells the rule is a two-significant-figure cut-off strictly below their smallest m_edge.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
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
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_BL_SYN\session_20260904-135302
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        0.42
                           079-10 v3 default cap 0.42 is PROVISIONAL: NO DISAGREEING CELL OBSERVED; floored from max m_edge=0.42753695, extrapolation not a boundary. Three masked sessions; N=162 perturbation cells, not independent edges; 178/572 scored with guard off, 164/572 with guard on. With wrong cells the rule is a two-significant-figure cut-off strictly below their smallest m_edge.
  clean-frame pool         41 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              StaticMeshActor_49     PARTIAL runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=1 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[4..6] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=17 (including baseline); assignment: onset 4 (nearest of {4, 7}), end 7 (nearest of {7, 10})
        onset TRANSITION 4 d=0.5454 tau=0.0040 label 4 delta=+0 ring d=0.3156 look at frames 3..5 note: whole-region change (lighting/camera?); other peaks in window: 7 m_edge=0.0000 base=11 pairs=7/7 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 6, delta=+17; max distance, 12 frames incl baseline)
        end   TRANSITION 7 d=0.5457 tau=0.0040 label 7 delta=+0 ring d=0.3144 look at frames 6..8 note: whole-region change (lighting/camera?); other peaks in window: 10 m_edge=0.0000 base=11 pairs=7/7 (mask extrapolated from frame 4, delta=-4; max distance, 3 frames incl baseline) (mask extrapolated from frame 6, delta=+17; max distance, 14 frames incl baseline)
      run[10..10] PARTIAL - one edge observed; the other is unassessable; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(all peaks in window assigned to earlier edges: 7) label 10 m_edge=0.0000 base=14 pairs=5/5 (mask extrapolated from frame 10, delta=+28; max distance, 21 frames incl baseline)
        end   NO-TRANSITION [10..15] tau=0.0040 label 11 m_edge=0.0000 base=14 pairs=6/6 (mask extrapolated from frame 10, delta=+28; max distance, 20 frames incl baseline)
idx=1   blink              StaticMeshActor_49     PARTIAL runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=1 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[26..28] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=26 (including baseline); assignment: onset 26 (nearest of {26, 29}), end 29 (nearest of {29, 32})
        onset TRANSITION 26 d=0.5457 tau=0.0040 label 26 delta=+0 ring d=0.3148 look at frames 25..27 note: whole-region change (lighting/camera?); other peaks in window: 29 m_edge=0.0000 base=21 pairs=8/8 (mask extrapolated from frame 26, delta=-26; max distance, 15 frames incl baseline) (mask extrapolated from frame 28, delta=+17; max distance, 12 frames incl baseline)
        end   TRANSITION 29 d=0.5453 tau=0.0040 label 29 delta=+0 ring d=0.3143 look at frames 28..30 note: whole-region change (lighting/camera?); other peaks in window: 32 m_edge=0.0000 base=21 pairs=7/7 (mask extrapolated from frame 26, delta=-26; max distance, 14 frames incl baseline) (mask extrapolated from frame 28, delta=+17; max distance, 14 frames incl baseline)
      run[32..32] PARTIAL - one edge observed; the other is unassessable; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(all peaks in window assigned to earlier edges: 29) label 32 m_edge=0.0000 base=23 pairs=5/5 (mask extrapolated from frame 32, delta=+28; max distance, 30 frames incl baseline)
        end   NO-TRANSITION [32..37] tau=0.0040 label 33 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 32, delta=+28; max distance, 29 frames incl baseline)
idx=2   blink              StaticMeshActor_49     PARTIAL runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=1 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[48..50] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 48 (nearest of {48, 51}), end 51 (nearest of {51, 54})
        onset TRANSITION 48 d=0.5455 tau=0.0040 label 48 delta=+0 ring d=0.3158 look at frames 47..49 note: whole-region change (lighting/camera?); other peaks in window: 51 m_edge=0.0000 base=23 pairs=8/8 (mask extrapolated from frame 48, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 50, delta=+17; max distance, 12 frames incl baseline)
        end   TRANSITION 51 d=0.5455 tau=0.0040 label 51 delta=+0 ring d=0.3164 look at frames 50..52 note: whole-region change (lighting/camera?); other peaks in window: 54 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 48, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 50, delta=+17; max distance, 14 frames incl baseline)
      run[54..54] PARTIAL - one edge observed; the other is unassessable; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(all peaks in window assigned to earlier edges: 51) label 54 m_edge=0.0000 base=23 pairs=5/5 (mask extrapolated from frame 54, delta=+28; max distance, 30 frames incl baseline)
        end   NO-TRANSITION [54..59] tau=0.0040 label 55 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 54, delta=+28; max distance, 29 frames incl baseline)
idx=3   blink              StaticMeshActor_49     PARTIAL runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=1 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[70..72] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 70 (nearest of {70, 73}), end 73 (nearest of {73, 76})
        onset TRANSITION 70 d=0.5457 tau=0.0040 label 70 delta=+0 ring d=0.3158 look at frames 69..71 note: whole-region change (lighting/camera?); other peaks in window: 73 m_edge=0.0000 base=23 pairs=8/8 (mask extrapolated from frame 70, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 72, delta=+17; max distance, 12 frames incl baseline)
        end   TRANSITION 73 d=0.5456 tau=0.0040 label 73 delta=+0 ring d=0.3146 look at frames 72..74 note: whole-region change (lighting/camera?); other peaks in window: 76 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 70, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 72, delta=+17; max distance, 14 frames incl baseline)
      run[76..76] PARTIAL - one edge observed; the other is unassessable; masks: in-span actual, edges extrapolated <=19 (including baseline)
        onset UNASSESSABLE(all peaks in window assigned to earlier edges: 73) label 76 m_edge=0.0000 base=20 pairs=5/5 (mask extrapolated from frame 76, delta=-19; max distance, 26 frames incl baseline)
        end   NO-TRANSITION [76..81] tau=0.0040 label 77 m_edge=0.0000 base=20 pairs=6/6 (mask extrapolated from frame 76, delta=-19; max distance, 25 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 4  UNASSESSABLE 0  READING 0  (of 4 events)
  run coverage             CONSISTENT 4  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 4  UNASSESSABLE 0  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 4 partial, 0 unassessable, 0 readings)
```

### M49_GEDGE_MT_SYN

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_MT_SYN\session_20260904-135223
  frames / labels / events 90 / 90 / 4
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        0.42
                           079-10 v3 default cap 0.42 is PROVISIONAL: NO DISAGREEING CELL OBSERVED; floored from max m_edge=0.42753695, extrapolation not a boundary. Three masked sessions; N=162 perturbation cells, not independent edges; 178/572 scored with guard off, 164/572 with guard on. With wrong cells the rule is a two-significant-figure cut-off strictly below their smallest m_edge.
  clean-frame pool         37 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
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
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\A2L_LEGA\session_20260904-180418
  frames / labels / events 90 / 90 / 7
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        0.42
                           079-10 v3 default cap 0.42 is PROVISIONAL: NO DISAGREEING CELL OBSERVED; floored from max m_edge=0.42753695, extrapolation not a boundary. Three masked sessions; N=162 perturbation cells, not independent edges; 178/572 scored with guard off, 164/572 with guard on. With wrong cells the rule is a two-significant-figure cut-off strictly below their smallest m_edge.
  clean-frame pool         15 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
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
      run[39..46] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=26 (including baseline); assignment: onset 39 (nearest of {39, 43}), end 47 (nearest of {45, 47})
        onset TRANSITION 39 d=0.9998 tau=0.0150 label 39 delta=+0 ring d=0.0516 look at frames 38..40 note: whole-region change (lighting/camera?); other peaks in window: 43 m_edge=0.0024 base=11 pairs=9/9 (mask extrapolated from frame 39, delta=-5; max distance, 5 frames incl baseline) (mask extrapolated from frame 46, delta=+26; max distance, 12 frames incl baseline)
        end   TRANSITION 47 d=1.0000 tau=0.0150 label 47 delta=+0 ring d=0.1734 look at frames 46..48 note: whole-region change (lighting/camera?); other peaks in window: 45 m_edge=0.0024 base=11 pairs=9/9 (mask extrapolated from frame 46, delta=+26; max distance, 17 frames incl baseline)
idx=4   corrupted_texture  Cube2_8                CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=drawn obs=measured
      run[51..58] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 51 (nearest of {47, 51}), end 59 (nearest of {56, 59})
        onset TRANSITION 51 d=0.9999 tau=0.0040 label 51 delta=+0 ring d=0.1421 look at frames 50..52 note: whole-region change (lighting/camera?); other peaks in window: 47 m_edge=0.0001 base=12 pairs=9/9 (mask extrapolated from frame 51, delta=-5; max distance, 5 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 59 d=1.0000 tau=0.0040 label 59 delta=+0 ring d=0.1516 look at frames 58..60 note: whole-region change (lighting/camera?); other peaks in window: 56 m_edge=0.0001 base=12 pairs=9/9 (mask extrapolated from frame 58, delta=+28; max distance, 16 frames incl baseline)
idx=5   missing_texture    Cube2_8                CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=drawn obs=measured
      run[75..82] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=14 (including baseline); assignment: onset 75 (nearest of {75}), end 83 (nearest of {81, 83})
        onset TRANSITION 75 d=1.0000 tau=0.0040 label 75 delta=+0 ring d=0.1202 look at frames 74..76 note: whole-region change (lighting/camera?) m_edge=0.0000 base=15 pairs=9/9 (mask extrapolated from frame 75, delta=-14; max distance, 14 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 5 frames incl baseline)
        end   TRANSITION 83 d=0.9999 tau=0.0040 label 83 delta=+0 ring d=0.1189 look at frames 82..84 note: whole-region change (lighting/camera?); other peaks in window: 81 m_edge=0.0000 base=15 pairs=9/9 (mask extrapolated from frame 75, delta=-14; max distance, 12 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 7 frames incl baseline)
idx=6   lod_popping        B_Manny_C_7            UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=projected obs=unmeasured
      UNASSESSABLE(non-manifested/empty event or no explicit frame_indices)
------------------------------------------------------------------------------
  CONSISTENT 3 (3 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 7 events)
  run coverage             CONSISTENT 3  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 5  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (3 consistent (3 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

### LYRA_SMOKE_01

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\LYRA_SMOKE_01\session_20260904-151503
  frames / labels / events 90 / 90 / 6
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        0.42
                           079-10 v3 default cap 0.42 is PROVISIONAL: NO DISAGREEING CELL OBSERVED; floored from max m_edge=0.42753695, extrapolation not a boundary. Three masked sessions; N=162 perturbation cells, not independent edges; 178/572 scored with guard off, 164/572 with guard on. With wrong cells the rule is a two-significant-figure cut-off strictly below their smallest m_edge.
  clean-frame pool         24 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              Cube7                  CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[16..17] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=16 (including baseline); assignment: onset 16 (nearest of {16, 18}), end 18 (nearest of {18, 21})
        onset TRANSITION 16 d=0.8414 tau=0.0212 label 16 delta=+0 ring d=0.0263 look at frames 15..17 note: whole-region change (lighting/camera?); other peaks in window: 18 m_edge=0.0036 base=13 pairs=7/7 (mask extrapolated from frame 16, delta=-16; max distance, 16 frames incl baseline) (mask extrapolated from frame 17, delta=+1; max distance, 1 frames incl baseline)
        end   TRANSITION 18 d=0.6574 tau=0.0212 label 18 delta=+0 ring d=0.0040 look at frames 17..19; other peaks in window: 21 m_edge=0.0036 base=13 pairs=6/6 (mask extrapolated from frame 16, delta=-16; max distance, 15 frames incl baseline) (mask extrapolated from frame 17, delta=+3; max distance, 3 frames incl baseline)
      run[21..22] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=21 (including baseline); assignment: onset 21 (nearest of {21, 23}), end 23 (nearest of {23})
        onset TRANSITION 21 d=0.6329 tau=0.0211 label 21 delta=+0 ring d=0.0077 look at frames 20..22; other peaks in window: 23 m_edge=0.0036 base=13 pairs=6/6 (mask extrapolated from frame 21, delta=-21; max distance, 17 frames incl baseline) (mask extrapolated from frame 22, delta=+1; max distance, 1 frames incl baseline)
        end   TRANSITION 23 d=0.6366 tau=0.0211 label 23 delta=+0 ring d=0.0122 look at frames 22..24 m_edge=0.0036 base=13 pairs=7/7 (mask extrapolated from frame 21, delta=-21; max distance, 15 frames incl baseline) (mask extrapolated from frame 22, delta=+5; max distance, 5 frames incl baseline)
idx=1   corrupted_texture  Cube4                  CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[27..34] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=27 (including baseline); assignment: onset 27 (nearest of {23, 27}), end 35 (nearest of {35})
        onset TRANSITION 27 d=0.9999 tau=0.1334 label 27 delta=+0 ring d=0.4296 look at frames 26..28 note: whole-region change (lighting/camera?); other peaks in window: 23 m_edge=0.0612 base=13 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 27, delta=-27; max distance, 18 frames incl baseline)
        end   TRANSITION 35 d=0.9999 tau=0.1334 label 35 delta=+0 ring d=0.3668 look at frames 34..36 note: whole-region change (lighting/camera?) m_edge=0.0612 base=13 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 27, delta=-27; max distance, 14 frames incl baseline) (mask extrapolated from frame 34, delta=+5; max distance, 5 frames incl baseline)
idx=2   blink              Cube4                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[40..41] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 40 m_edge=n/a base=1 pairs=7/7 (mask extrapolated from frame 40, delta=-28; max distance, 7 frames incl baseline) (mask extrapolated from frame 41, delta=+1; max distance, 1 frames incl baseline)
        end   UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 42 m_edge=n/a base=1 pairs=6/6 (mask extrapolated from frame 40, delta=-28; max distance, 3 frames incl baseline) (mask extrapolated from frame 41, delta=+3; max distance, 3 frames incl baseline)
      run[45..46] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 45 m_edge=n/a base=1 pairs=6/6 (mask extrapolated from frame 45, delta=-3; max distance, 3 frames incl baseline) (mask extrapolated from frame 46, delta=+28; max distance, 3 frames incl baseline)
        end   UNASSESSABLE(baseline: only 1 valid clean pairs, need 3 (23 unavailable)) label 47 m_edge=n/a base=1 pairs=7/7 (mask extrapolated from frame 45, delta=-1; max distance, 1 frames incl baseline) (mask extrapolated from frame 46, delta=+28; max distance, 7 frames incl baseline)
idx=3   missing_texture    B_Manny_C_14           PARTIAL runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=1 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[51..58] PARTIAL - one edge observed; the other is unassessable; masks: in-span actual, edges extrapolated <=26 (including baseline)
        onset UNASSESSABLE(mask missing at frame 53) label 51 m_edge=n/a base=0 pairs=7/9 (mask id unresolved - sole value used) (mask extrapolated from frame 51, delta=-4; max distance, 4 frames incl baseline)
        end   TRANSITION 63 d=0.8767 tau=0.3376 label 59 delta=+4 ring d=0.0609 look at frames 62..64 m_edge=0.2117 base=11 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 58, delta=+26; max distance, 16 frames incl baseline)
idx=4   corrupted_texture  Cube3                  CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[63..70] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=14 (including baseline); assignment: onset 63 (nearest of {59, 63}), end 71 (nearest of {71})
        onset TRANSITION 63 d=1.0000 tau=0.0580 label 63 delta=+0 ring d=0.0837 look at frames 62..64 note: whole-region change (lighting/camera?); other peaks in window: 59 m_edge=0.0187 base=11 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 63, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 70, delta=+14; max distance, 12 frames incl baseline)
        end   TRANSITION 71 d=1.0000 tau=0.0580 label 71 delta=+0 ring d=0.5494 look at frames 70..72 note: whole-region change (lighting/camera?) m_edge=0.0187 base=11 pairs=9/9 (mask extrapolated from frame 70, delta=+14; max distance, 14 frames incl baseline)
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
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\A1L_LEGA\session_20260904-165004
  frames / labels / events 90 / 90 / 6
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        0.42
                           079-10 v3 default cap 0.42 is PROVISIONAL: NO DISAGREEING CELL OBSERVED; floored from max m_edge=0.42753695, extrapolation not a boundary. Three masked sessions; N=162 perturbation cells, not independent edges; 178/572 scored with guard off, 164/572 with guard on. With wrong cells the rule is a two-significant-figure cut-off strictly below their smallest m_edge.
  clean-frame pool         30 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              Cube7                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=measured
      run[4..5] UNASSESSABLE - whole-span change at frame 4 d=0.6067 > tau=0.0040; masks: in-span actual, edges extrapolated <=7 (including baseline)
        onset NO-TRANSITION [1..6] tau=0.0040 label 4 m_edge=0.0000 base=4 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 5, delta=+7; max distance, 6 frames incl baseline)
        end   NO-TRANSITION [4..10] tau=0.0040 label 6 m_edge=0.0000 base=4 pairs=7/7 (mask extrapolated from frame 4, delta=-1; max distance, 1 frames incl baseline) (mask extrapolated from frame 5, delta=+7; max distance, 7 frames incl baseline)
idx=1   corrupted_texture  Cube6_0                CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=measured
      run[15..22] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=26 (including baseline); assignment: onset 15 (nearest of {12, 15}), end 23 (nearest of {23})
        onset TRANSITION 15 d=1.0000 tau=0.3372 label 15 delta=+0 ring d=0.1651 look at frames 14..16; other peaks in window: 12 m_edge=0.1036 base=15 pairs=9/9 (mask extrapolated from frame 15, delta=-7; max distance, 7 frames incl baseline) (mask extrapolated from frame 22, delta=+26; max distance, 12 frames incl baseline)
        end   TRANSITION 23 d=1.0000 tau=0.3372 label 23 delta=+0 ring d=0.0654 look at frames 22..24 m_edge=0.1036 base=15 pairs=9/9 (mask extrapolated from frame 15, delta=-7; max distance, 5 frames incl baseline) (mask extrapolated from frame 22, delta=+26; max distance, 17 frames incl baseline)
idx=2   blink              Cube6_0                CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=measured
      run[28..29] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=20 (including baseline); assignment: onset 28 (nearest of {28, 30}), end 30 (nearest of {30, 33})
        onset TRANSITION 28 d=1.0000 tau=0.3780 label 28 delta=+0 ring d=0.2324 look at frames 27..29; other peaks in window: 30 m_edge=0.1083 base=15 pairs=7/7 (mask extrapolated from frame 28, delta=-20; max distance, 10 frames incl baseline) (mask extrapolated from frame 29, delta=+19; max distance, 13 frames incl baseline)
        end   TRANSITION 30 d=0.9997 tau=0.3780 label 30 delta=+0 ring d=0.2322 look at frames 29..31; other peaks in window: 33 m_edge=0.1083 base=15 pairs=6/6 (mask extrapolated from frame 28, delta=-20; max distance, 6 frames incl baseline) (mask extrapolated from frame 29, delta=+19; max distance, 15 frames incl baseline)
      run[33..34] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 33 (nearest of {33, 35}), end 35 (nearest of {35, 39})
        onset TRANSITION 33 d=0.9997 tau=0.3440 label 33 delta=+0 ring d=0.2281 look at frames 32..34; other peaks in window: 35 m_edge=0.1081 base=16 pairs=6/6 (mask extrapolated from frame 33, delta=-25; max distance, 8 frames incl baseline) (mask extrapolated from frame 34, delta=+28; max distance, 15 frames incl baseline)
        end   TRANSITION 35 d=0.9993 tau=0.3440 label 35 delta=+0 ring d=0.2268 look at frames 34..36; other peaks in window: 39 m_edge=0.1081 base=16 pairs=7/7 (mask extrapolated from frame 33, delta=-25; max distance, 6 frames incl baseline) (mask extrapolated from frame 34, delta=+28; max distance, 16 frames incl baseline)
idx=3   corrupted_texture  Cube2_8                OFFSET-NOTE runs{CONSISTENT=0 OFFSET-NOTE=1 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=measured
      run[51..58] OFFSET-NOTE(+4,+0) - a target transition sits off the label - possible label offset OR an unrelated change (lighting, occlusion, neighbouring event); a person must look at the listed frames k-1..k+1; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline); assignment: onset 55 (nearest of {55}), end 59 (nearest of {59})
        onset TRANSITION 55 d=0.9855 tau=0.0763 label 51 delta=+4 ring d=0.1229 look at frames 54..56 note: whole-region change (lighting/camera?) m_edge=0.0192 base=23 pairs=9/9 (mask extrapolated from frame 51, delta=-14; max distance, 14 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 59 d=0.9903 tau=0.0763 label 59 delta=+0 ring d=0.0184 look at frames 58..60 m_edge=0.0192 base=23 pairs=9/9 (mask extrapolated from frame 51, delta=-14; max distance, 12 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 16 frames incl baseline)
idx=4   missing_texture    B_Manny_C_12           UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=measured
      run[75..82] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(regional change m=0.427537 > cap=0.420000) label 75 m_edge=0.4275 base=16 pairs=9/9 (mask extrapolated from frame 75, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 5 frames incl baseline)
        end   UNASSESSABLE(regional change m=0.427537 > cap=0.420000) label 83 m_edge=0.4275 base=16 pairs=9/9 (mask extrapolated from frame 75, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 7 frames incl baseline)
idx=5   lod_popping        B_Manny_C_12           UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=unmeasured
      UNASSESSABLE(non-manifested/empty event or no explicit frame_indices)
------------------------------------------------------------------------------
  CONSISTENT 2 (0 with caveat)  OFFSET-NOTE 1  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 3  READING 0  (of 6 events)
  run coverage             CONSISTENT 3  OFFSET-NOTE 1  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 2  READING 0  (of 6 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (2 consistent (0 with caveat), 1 offset-notes for human review, 0 partial, 3 unassessable, 0 readings)
```

### M50L_LG9

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  D:\IntrusiveAnomalies\_bench_sessions_bank\M50L_LG9\session_20260904-195949
  frames / labels / events 90 / 90 / 6
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        0.42
                           079-10 v3 default cap 0.42 is PROVISIONAL: NO DISAGREEING CELL OBSERVED; floored from max m_edge=0.42753695, extrapolation not a boundary. Three masked sessions; N=162 perturbation cells, not independent edges; 178/572 scored with guard off, 164/572 with guard on. With wrong cells the rule is a two-significant-figure cut-off strictly below their smallest m_edge.
  clean-frame pool         24 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              BakedStaticMeshActor_C_UAID_E04F43E623D48FEB00_1131370154 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=2} bbox=projected obs=unmeasured
      run[16..17] READING (bbox-only)
        onset UNASSESSABLE(threshold unsatisfiable: tau=1.1837 >= 1.0) label 16 m_edge=0.4385 base=24 pairs=7/7 (bbox-only)
        end   UNASSESSABLE(threshold unsatisfiable: tau=1.1837 >= 1.0) label 18 m_edge=0.4385 base=24 pairs=6/6 (bbox-only)
      run[21..22] READING (bbox-only)
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
      run[45..46] READING (bbox-only)
        onset UNASSESSABLE(regional change m=0.422312 > cap=0.420000) label 45 m_edge=0.4223 base=24 pairs=6/6 (bbox-only)
        end   UNASSESSABLE(region covers the picture at frame 51) label 47 m_edge=n/a base=0 pairs=6/7 (bbox-only)
idx=3   missing_texture    BakedStaticMeshActor_C_UAID_E04F43E623D408E000_1155742006 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=1} bbox=projected obs=unmeasured
      run[51..58] READING (bbox-only)
        onset UNASSESSABLE(region covers the picture at frame 47) label 51 m_edge=n/a base=0 pairs=0/9 (bbox-only)
        end   UNASSESSABLE(region covers the picture at frame 55) label 59 m_edge=n/a base=0 pairs=1/9 (bbox-only)
idx=4   corrupted_texture  BakedStaticMeshActor_C_UAID_E04F43E623D418F900_1266499077 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=1} bbox=projected obs=unmeasured
      run[63..70] READING (bbox-only)
        onset UNASSESSABLE(regional change m=0.447317 > cap=0.420000) label 63 m_edge=0.4473 base=24 pairs=9/9 (bbox-only)
        end   UNASSESSABLE(regional change m=0.447317 > cap=0.420000) label 71 m_edge=0.4473 base=24 pairs=9/9 (bbox-only)
idx=5   missing_texture    BakedStaticMeshActor_C_UAID_E04F43E623D417F900_1074739879 READING runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=1} bbox=projected obs=unmeasured
      run[87..89] READING (bbox-only)
        onset UNASSESSABLE(threshold unsatisfiable: tau=1.2314 >= 1.0) label 87 m_edge=0.4426 base=24 pairs=7/7 (bbox-only)
        end   UNASSESSABLE(edge truncated by the session boundary) label 90 m_edge=0.4426 base=24 pairs=3/3 (bbox-only)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 6  (of 6 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 8  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 0 unassessable, 6 readings)
  coverage                 UNREAD-BBOX-ONLY
```

## Appendix B — full new ruled-fixture readings

### adjacent_late_second_onset

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  <USERPROFILE>\AppData\Local\Temp\m49_consistency_selftest_b8l6hp6h\adjacent_late_second_onset
  frames / labels / events 100 / 100 / 1
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        1.0
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         87 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   missing_texture    SynthTarget            OFFSET-NOTE runs{CONSISTENT=1 OFFSET-NOTE=1 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[10..11] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges actual; assignment: onset 10 (nearest of {10, 12}), end 12 (nearest of {12, 15})
        onset TRANSITION 10 d=0.0400 tau=0.0040 label 10 delta=+0 ring d=0.0000 look at frames 9..11; other peaks in window: 12 m_edge=0.0000 base=24 pairs=7/7
        end   TRANSITION 12 d=0.0400 tau=0.0040 label 12 delta=+0 ring d=0.0000 look at frames 11..13; other peaks in window: 15 m_edge=0.0000 base=24 pairs=7/7
      run[16..16] OFFSET-NOTE(-1,+0) - a target transition sits off the label - possible label offset OR an unrelated change (lighting, occlusion, neighbouring event); a person must look at the listed frames k-1..k+1; does not establish cause; masks: in-span actual, edges actual; assignment: onset 15 (nearest of {15, 17}), end 17 (nearest of {17})
        onset TRANSITION 15 d=0.3600 tau=0.0040 label 16 delta=-1 ring d=0.0000 look at frames 14..16; other peaks in window: 17 m_edge=0.0000 base=24 pairs=6/6
        end   TRANSITION 17 d=0.3600 tau=0.0040 label 17 delta=+0 ring d=0.0000 look at frames 16..18 m_edge=0.0000 base=24 pairs=6/6
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 1  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 1 events)
  run coverage             CONSISTENT 1  OFFSET-NOTE 1  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 2 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 1 offset-notes for human review, 0 partial, 0 unassessable, 0 readings)
```

### r6_blink_aligned

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  <USERPROFILE>\AppData\Local\Temp\m49_consistency_selftest_b8l6hp6h\r6_blink_aligned
  frames / labels / events 100 / 100 / 1
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        1.0
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         88 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   missing_texture    SynthTarget            CONSISTENT runs{CONSISTENT=2 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[4..5] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges actual; assignment: onset 4 (nearest of {4, 6}), end 6 (nearest of {6, 8})
        onset TRANSITION 4 d=1.0000 tau=0.0040 label 4 delta=+0 ring d=0.0000 look at frames 3..5; other peaks in window: 6 m_edge=0.0000 base=24 pairs=6/6
        end   TRANSITION 6 d=1.0000 tau=0.0040 label 6 delta=+0 ring d=0.0000 look at frames 5..7; other peaks in window: 8 m_edge=0.0000 base=24 pairs=5/5
      run[8..9] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges actual; assignment: onset 8 (nearest of {8, 10}), end 10 (nearest of {10})
        onset TRANSITION 8 d=1.0000 tau=0.0040 label 8 delta=+0 ring d=0.0000 look at frames 7..9; other peaks in window: 10 m_edge=0.0000 base=24 pairs=5/5
        end   TRANSITION 10 d=1.0000 tau=0.0040 label 10 delta=+0 ring d=0.0000 look at frames 9..11 m_edge=0.0000 base=24 pairs=7/7
------------------------------------------------------------------------------
  CONSISTENT 1 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 1 events)
  run coverage             CONSISTENT 2  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 2 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (1 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 0 unassessable, 0 readings)
```

### r6_blink_late_second_onset

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  <USERPROFILE>\AppData\Local\Temp\m49_consistency_selftest_b8l6hp6h\r6_blink_late_second_onset
  frames / labels / events 100 / 100 / 1
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        1.0
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         88 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   missing_texture    SynthTarget            OFFSET-NOTE runs{CONSISTENT=1 OFFSET-NOTE=1 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[4..5] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges actual; assignment: onset 4 (nearest of {4, 6}), end 6 (nearest of {6, 8})
        onset TRANSITION 4 d=1.0000 tau=0.0040 label 4 delta=+0 ring d=0.0000 look at frames 3..5; other peaks in window: 6 m_edge=0.0000 base=24 pairs=6/6
        end   TRANSITION 6 d=1.0000 tau=0.0040 label 6 delta=+0 ring d=0.0000 look at frames 5..7; other peaks in window: 8 m_edge=0.0000 base=24 pairs=6/6
      run[9..9] OFFSET-NOTE(-1,+0) - a target transition sits off the label - possible label offset OR an unrelated change (lighting, occlusion, neighbouring event); a person must look at the listed frames k-1..k+1; does not establish cause; masks: in-span actual, edges actual; assignment: onset 8 (nearest of {8, 10}), end 10 (nearest of {10})
        onset TRANSITION 8 d=1.0000 tau=0.0040 label 9 delta=-1 ring d=0.0000 look at frames 7..9; other peaks in window: 10 m_edge=0.0000 base=24 pairs=5/5
        end   TRANSITION 10 d=1.0000 tau=0.0040 label 10 delta=+0 ring d=0.0000 look at frames 9..11 m_edge=0.0000 base=24 pairs=6/6
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 1  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 1 events)
  run coverage             CONSISTENT 1  OFFSET-NOTE 1  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 2 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 1 offset-notes for human review, 0 partial, 0 unassessable, 0 readings)
```

### r6_blink_missing_peak6

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  <USERPROFILE>\AppData\Local\Temp\m49_consistency_selftest_b8l6hp6h\r6_blink_missing_peak6
  frames / labels / events 100 / 100 / 1
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        1.0
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         88 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   missing_texture    SynthTarget            OFFSET-NOTE runs{CONSISTENT=0 OFFSET-NOTE=1 NO-TRACE=0 PARTIAL=1 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[4..5] OFFSET-NOTE(+0,+2) - a target transition sits off the label - possible label offset OR an unrelated change (lighting, occlusion, neighbouring event); a person must look at the listed frames k-1..k+1; does not establish cause; masks: in-span actual, edges actual; assignment: onset 4 (nearest of {4}), end 8 (nearest of {8})
        onset TRANSITION 4 d=1.0000 tau=0.0040 label 4 delta=+0 ring d=0.0000 look at frames 3..5 m_edge=0.0000 base=24 pairs=6/6
        end   TRANSITION 8 d=1.0000 tau=0.0040 label 6 delta=+2 ring d=0.0000 look at frames 7..9 m_edge=0.0000 base=24 pairs=5/5
      run[8..9] PARTIAL - one edge observed; the other is unassessable; masks: in-span actual, edges actual; assignment: onset 10 (nearest of {10}), end none (nearest of {})
        onset TRANSITION 10 d=1.0000 tau=0.0040 label 8 delta=+2 ring d=0.0000 look at frames 9..11 m_edge=0.0000 base=24 pairs=5/5
        end   UNASSESSABLE(all peaks in window assigned to earlier edges: 8, 10) label 10 m_edge=0.0000 base=24 pairs=7/7
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 1  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 1 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 1  NO-TRACE 0  PARTIAL 1  UNASSESSABLE 0  READING 0  (of 2 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 1 offset-notes for human review, 0 partial, 0 unassessable, 0 readings)
```

### r6_blink_one_frame_second

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-12)
  session                  <USERPROFILE>\AppData\Local\Temp\m49_consistency_selftest_b8l6hp6h\r6_blink_one_frame_second
  frames / labels / events 100 / 100 / 1
  semantics                masks identify pixels, not cause; no output certifies a label as correct
  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING
  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=28 frames (window+N_BASE=4+24)
                           This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist.
  signal                   fraction with an RGB-channel difference >8/255; edge tau=max(median+6*MAD,0.004)
  assessability cap        1.0
                           079-12 v3 default: NO ADMISSIBLE ENVELOPE; smallest wrong-cell m_edge=0. Three masked sessions; N=268 baseline-bearing perturbation cells, not independent edges. Guard off: 296/572 scored, 22 wrong; guard on: 0/572 scored, all 572 unscored. Zero scored cells do not validate recovery. The unchanged rule requires a two-significant-figure cut-off strictly below the smallest wrong-cell m_edge; no positive cap is defensible, so every otherwise-assessable edge is refused.
  clean-frame pool         87 (guard 2); nearest 3..24 valid per-pair regions for EACH edge
  search                   +/-4, inclusive labelled bounds only; local peaks >=1.5 x both neighbours (outside window=0)
  assignment               per target/mode, labelled order: nearest unused peak; tie earlier; no separate ordering constraint
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   missing_texture    SynthTarget            PARTIAL runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=1 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[4..6] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges actual; assignment: onset 4 (nearest of {4, 7}), end 7 (nearest of {7, 10})
        onset TRANSITION 4 d=1.0000 tau=0.0040 label 4 delta=+0 ring d=0.0000 look at frames 3..5; other peaks in window: 7 m_edge=0.0000 base=24 pairs=7/7
        end   TRANSITION 7 d=1.0000 tau=0.0040 label 7 delta=+0 ring d=0.0000 look at frames 6..8; other peaks in window: 10 m_edge=0.0000 base=24 pairs=7/7
      run[10..10] PARTIAL - one edge observed; the other is unassessable; masks: in-span actual, edges actual
        onset UNASSESSABLE(all peaks in window assigned to earlier edges: 7) label 10 m_edge=0.0000 base=24 pairs=5/5
        end   NO-TRANSITION [10..15] tau=0.0040 label 11 m_edge=0.0000 base=24 pairs=6/6
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 1  UNASSESSABLE 0  READING 0  (of 1 events)
  run coverage             CONSISTENT 1  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 1  UNASSESSABLE 0  READING 0  (of 2 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 1 partial, 0 unassessable, 0 readings)
```

## Appendix C — final regression summary

```text
LABEL-PIXEL OBSERVATIONS SELFTEST
  clean                                       CONSISTENT                       exit0 OK
  clean_masked                                CONSISTENT                       exit0 OK
  label_late_1                                OFFSET-NOTE                      exit0 OK
  label_early_1                               OFFSET-NOTE                      exit0 OK
  end_late_1                                  OFFSET-NOTE                      exit0 OK
  end_early_1                                 OFFSET-NOTE                      exit0 OK
  blank_region                                NO-TRACE                         exit2 OK
  moving_clean                                CONSISTENT                       exit0 OK
  moving_fast                                 CONSISTENT                       exit0 OK
  moving_label_late_1                         OFFSET-NOTE                      exit0 OK
  moving_label_early_1                        OFFSET-NOTE                      exit0 OK
  moving_end_late_1                           OFFSET-NOTE                      exit0 OK
  moving_end_early_1                          OFFSET-NOTE                      exit0 OK
  moving_blank_region                         NO-TRACE                         exit2 OK
  moving_fullframe_region                     UNASSESSABLE                     exit0 OK
  moving_unsat                                UNASSESSABLE                     exit0 OK
  moving_over_cap                             UNASSESSABLE                     exit0 OK
  bboxonly_clean                              READING                          exit0 OK
  bboxonly_label_late_1                       READING                          exit0 OK
  codex_quiet_prefix                          READING                          exit0 OK
  codex_small_region                          READING                          exit0 OK
  codex_alternating                           READING                          exit0 OK
  codex_quiet_prefix_shift1                   READING                          exit0 OK
  codex_small_region_shift1                   READING                          exit0 OK
  codex_quiet_prefix_noburst_shift1           READING                          exit0 OK
  codex_small_region_noburst_shift1           READING                          exit0 OK
  codex_masked_negative_False                 OFFSET-NOTE                      exit0 OK
  codex_masked_negative_True                  OFFSET-NOTE                      exit0 OK
  c07_burst_aligned                           READING                          exit0 OK
  c07_burst_late                              READING                          exit0 OK
  c07_static_aligned                          READING                          exit0 OK
  c07_static_late                             READING                          exit0 OK
  c07_onset_early_refusal                     READING                          exit0 OK
  c07_previous_end_refusal                    READING                          exit0 OK
  c07_partial_multirun                        READING                          exit0 OK
  c07_missing_edge_frames                     READING                          exit0 OK
  c07_early_refusal_control                   READING                          exit0 OK
  c07_previous_end_control                    READING                          exit0 OK
  c07_below_tau                               READING                          exit0 OK
  c07_burst_aligned_masked                    CONSISTENT                       exit0 OK
  c07_burst_late_masked                       OFFSET-NOTE                      exit0 OK
  c07_onset_early_refusal_masked              UNASSESSABLE                     exit0 OK
  c07_previous_end_refusal_masked             PARTIAL                          exit0 OK
  c07_previous_end_control_masked             OFFSET-NOTE                      exit0 OK
  c07_partial_multirun_masked                 CONSISTENT                       exit0 OK
  c07_mixed_runs                              CONSISTENT                       exit0 OK
  c07_missing_edge_frames_masked              UNASSESSABLE                     exit0 OK
  c07_below_tau_masked                        NO-TRACE                         exit2 OK
  c07_zero_cap_static_masked                  CONSISTENT                       exit0 OK
  c07_zero_cap_burst_masked                   CONSISTENT                       exit0 OK
  c07_no_envelope_masked                      UNASSESSABLE                     exit0 OK
  c08_lighting_aligned                        OFFSET-NOTE                      exit0 OK
  c08_lighting_late_onset                     CONSISTENT                       exit0 OK
  c08_occluder_aligned                        OFFSET-NOTE                      exit0 OK
  c08_static_aligned                          CONSISTENT                       exit0 OK
  c08_static_late_onset                       OFFSET-NOTE                      exit0 OK
  c08_missing_mask61                          PARTIAL                          exit0 OK
  c08_missing_rgb61                           PARTIAL                          exit0 OK
  c08_precise_mask_ring_burst                 CONSISTENT                       exit0 OK
  c08_dilated_mask_ring_burst                 OFFSET-NOTE                      exit0 OK
  c08_precise_mask_target_move                OFFSET-NOTE                      exit0 OK
  c08_lagged_mask_target_move                 CONSISTENT                       exit0 OK
  c08_foreign_mask_value                      UNASSESSABLE                     exit0 OK
  c08_no_detected_end                         PARTIAL                          exit0 OK
  c08_mixed_run                               PARTIAL                          exit0 OK
  c08_one_frame_aligned                       UNASSESSABLE                     exit0 OK
  c08_one_frame_late                          OFFSET-NOTE                      exit0 OK
  c08_one_frame_early                         OFFSET-NOTE                      exit0 OK
  adjacent_control                            CONSISTENT                       exit0 OK
  adjacent_late_second_onset                  OFFSET-NOTE                      exit0 OK
  adjacent_early_second_onset                 OFFSET-NOTE                      exit0 OK
  mixed_late_second_onset                     OFFSET-NOTE                      exit0 OK
  r6_blink_aligned                            CONSISTENT                       exit0 OK
  r6_blink_late_second_onset                  OFFSET-NOTE                      exit0 OK
  r6_blink_missing_peak6                      OFFSET-NOTE                      exit0 OK
  r6_blink_one_frame_second                   PARTIAL                          exit0 OK
  blank_whole_span                            NO-TRACE                         exit2 OK
  blank_missing_interior                      UNASSESSABLE                     exit0 OK
  interior_change                             UNASSESSABLE                     exit0 OK
  unresolved                                  UNASSESSABLE                     exit0 OK
  unsupported_class                           UNASSESSABLE                     exit0 OK
  empty_mask                                  PARTIAL                          exit0 OK
  unreadable_mask                             PARTIAL                          exit0 OK
  rgb_dimensions                              PARTIAL                          exit0 OK
  equal_luma_colour                           CONSISTENT                       exit0 OK
  missing_first_rgb                           PARTIAL                          exit0 OK
  missing_last_rgb                            PARTIAL                          exit0 OK
  span_masks_control                          CONSISTENT                       exit0 OK
  span_masks_blank                            NO-TRACE                         exit2 OK
  span_masks_missing_interior                 UNASSESSABLE                     exit0 OK
  span_masks_missing_onset                    PARTIAL                          exit0 OK
  span_masks_missing_end                      PARTIAL                          exit0 OK
  consistent_with_unassessable_run            CONSISTENT                       exit0 OK
  consistent_with_partial_run                 PARTIAL                          exit0 OK
SELFTEST: OK - 94 cases; observations, whole-span coverage and run eligibility
```

**Chat review REQUIRED for this evidence and independent Code review before merge.** P2′ and one supplied fixture prediction still miss; the branch remains unmerged and all release holds remain.

VERDICT: NEEDS-DECISION
