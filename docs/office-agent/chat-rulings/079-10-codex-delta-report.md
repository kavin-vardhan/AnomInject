# 079-10 — Codex delta implementation report

**NEEDS-DECISION.** The authorized delta is implemented and pushed on `fix/verifier-field-validity`. The final self-test passes, but R1's pinned-bench prediction is missed: **32 of 64 edges observed**, with both blink legs still UNASSESSABLE under the retained inclusive-window and duplicate-claim rules. All 64 edges have tau=0.0040. No bounds or thresholds were tuned to make P2 pass.

**Chat review NOT REQUIRED for the authorized implementation and feature push, now complete. Chat review REQUIRED for this evidence, the P2 miss and Claude Code's independent review before merge.** That is the next mandatory Chat checkpoint. Codex implemented; Code reviews next; Chat orchestrates. No review brief or mailbox message was issued. Client/Section G, m51 campaign and release gates remain held.

## 1. Scope, commits and evidence

- Base: `7d5b6e919f9bca3506b9057a56a4349ea300ae64`.
- Predictions-first commit (AMENDMENT 1): `97a94ef823dfdfa0f3def74ff118b65113ac06ee`.
- Implementation/local-and-remote feature: `e6351ce396c53ede5a7acd8186497811776e9e34`.
- Restored checkout: m51 `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`.
- Master/origin-master unchanged: `ac13700d99fe116056f5aa11611f8931a563a3f2`.
- CaptureBench unchanged HEAD: `472a409dc70ae78d01a38d08fc3bddb15fe41c58`; only its permitted, already-untracked `tools/m079_shift_recovery.py` changed. That repository has no remote; its scorer is retained locally and frozen for review, not included in the plugin branch push.

Source/, Shaders/ and tools/measure_label_offset.py are unchanged from 7d5b6e9. The ten pre-existing plugin handoffs and every other CaptureBench untracked file retain their preflight hashes. No merge, tag, build, cook, capture, office-data transfer or owner-app change.

Read the [predictions and AMENDMENT 1](D:/IntrusiveAnomalies/_reviews/079-10-frozen/2026-09-20-verifier-consistency-semantics.md), [session journal](D:/IntrusiveAnomalies/_reviews/079-10-frozen/2026-09-20-079-10-codex-delta.md), [source/hash manifest](D:/IntrusiveAnomalies/_reviews/079-10-frozen/manifest.json) and [final boundary audit](D:/IntrusiveAnomalies/_reviews/079-10-evidence/final/boundary.json). The live checkout is m51 again; review the frozen tools or the pushed feature ref.

## 2. Ruling-by-ruling implementation

| Ruling | Implementation and proof |
|---|---|
| R1 boundary masks | [verify_capture.py:679](D:/IntrusiveAnomalies/_reviews/079-10-frozen/verify_capture.py:679): actual masks first; missing out-of-span target masks use this run's onset/last-labelled mask within window+24, default 28. Pair union retained; no recursive fallback. Missing in-span masks, corrupt PNGs and dimension errors remain errors. Seven new permanent fixtures and five new contract tests cover the rule. |
| R1 identity | [verify_capture.py:597](D:/IntrusiveAnomalies/_reviews/079-10-frozen/verify_capture.py:597): both event/run anchors and per-frame identity require the named target. A sole unrelated row entry cannot supply its ID. The new named-boundary regression failed before this fix and passes afterward. |
| R1 disclosure | [verify_capture.py:1003](D:/IntrusiveAnomalies/_reviews/079-10-frozen/verify_capture.py:1003): source and maximum signed extrapolation distance on each affected edge, including baseline masks; complete source/delta lists in structured evidence; maximum distance on the run. NO-TRACE names extrapolated edge pairs. |
| R2 lighting | [verify_capture.py:1093](D:/IntrusiveAnomalies/_reviews/079-10-frozen/verify_capture.py:1093): CONSISTENT retains the whole-region-change caveat on the RUN line; event summaries count consistent events with a caveat. Lighting fixtures pass. |
| R3 thresholded absence | [verify_capture.py:953](D:/IntrusiveAnomalies/_reviews/079-10-frozen/verify_capture.py:953): no change above tau across the span and both windows. The one-pixel fixture remains NO-TRACE; an above-tau interior change blocks it. Header and README explain tau as a pixel fraction. |
| R4 aggregation | [verify_capture.py:993](D:/IntrusiveAnomalies/_reviews/079-10-frozen/verify_capture.py:993): NO-TRACE > OFFSET-NOTE > PARTIAL > CONSISTENT from runs only. U/READING run counts stay visible; blank_missing_interior is U, consistent+U is C with U coverage, consistent+partial is P. |
| R5 retained contracts | Class allowlist remains a producer contract; inclusive labelled windows, duplicate refusal, RGB-channel difference, endpoint extent and batch exits retained. Scorer eligibility now reflects the partly masked run correction. |

This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist. The exact distinction is in the module/output header and the appended G263/G264 correction. Boundary extrapolation still assumes that silhouette applies at another frame; the tag discloses the assumption.

Predeclared interpretations: N_BASE=BASELINE_MAX_FRAMES=24; absent means missing reference/file, empty valid PNG or absent requested ID outside the span. Corruption is not absence. Partly masked runs stay in mask mode even with a missing boundary; a genuinely bbox-only run remains READING. Caveat totals count CONSISTENT events with at least one caveated consistent run. Console uses ASCII `delta`, `<=`, and `k-1..k+1` for the specification's typographic symbols.

### Diff summary

```text
CLAUDE.md                                          |  32 ++-
 docs/client-readme.md                              |  89 ++++---
 docs/gotchas.md                                    |  35 +++
 docs/office-rdp-card.md                            |  45 ++--
 .../2026-09-20-verifier-consistency-semantics.md   | 113 +++++++++
 docs/sessions/2026-09-20-079-10-codex-delta.md     | 117 +++++++++
 tools/test_verify_capture_consistency.py           |  68 +++++-
 tools/verify_capture.py                            | 270 ++++++++++++++-------
 8 files changed, 618 insertions(+), 151 deletions(-)
```

README Step 6 and the prospective Section G note were updated; earlier readings and prediction bodies remain history. G263/G264 received a dated append. CLAUDE.md and the new session journal record current ownership and the P2 miss.

## 3. Per-prediction readings

### P1 — self-test and ruling regressions

**90/90 permanent fixtures pass; 9/9 focused contract tests pass.** The prior 83 cases are retained with R1-R4's amended known answers, plus seven span-mask/aggregation cases. Semantic fixtures explicitly use cap 1.0; separate tests exercise numeric/zero/refuse-all caps. This prevents calibration from turning every semantic control into an unread run.

[all self-test readings](D:/IntrusiveAnomalies/_reviews/079-10-evidence/final/selftest_full.txt) · [structured details](D:/IntrusiveAnomalies/_reviews/079-10-evidence/final/selftest_details.json) · [contract output](D:/IntrusiveAnomalies/_reviews/079-10-evidence/final/contracts.txt) · [named-target failure before fix](D:/IntrusiveAnomalies/_reviews/079-10-evidence/named-target-before.txt)

```text
  consistent_with_partial_run                 PARTIAL                          exit0 OK
SELFTEST: OK - 90 cases; observations, whole-span coverage and run eligibility
```

**lighting_late_onset** (exit 0):

```text
idx=0   missing_texture    SynthTarget            CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[61..67] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges actual
  CONSISTENT 1 (1 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 1 events)
  run coverage             CONSISTENT 1  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 1 runs; unassessable/readings never promote an event)
```

**blank_missing_interior** (exit 0):

```text
idx=0   missing_texture    SynthTarget            UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[40..70] UNASSESSABLE - whole-span check: mask missing at frame 55; masks: in-span actual, edges actual
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 1  READING 0  (of 1 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 1  READING 0  (of 1 runs; unassessable/readings never promote an event)
```

**c07_below_tau_masked** (exit 2):

```text
idx=0   missing_texture    SynthTarget            NO-TRACE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=1 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[60..67] NO-TRACE - no change above the noise floor (tau=0.0040) on this target across frames 60..67; the label claims a visible change; masks: in-span actual, edges actual
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 1  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 1 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 1  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 1 runs; unassessable/readings never promote an event)
```

**interior_change** (exit 0):

```text
idx=0   missing_texture    SynthTarget            UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[40..70] UNASSESSABLE - whole-span change at frame 55 d=0.0625 > tau=0.0040; masks: in-span actual, edges actual
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 1  READING 0  (of 1 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 1  READING 0  (of 1 runs; unassessable/readings never promote an event)
```

**span_masks_blank** (exit 2):

```text
idx=0   missing_texture    SynthTarget            NO-TRACE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=1 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[40..70] NO-TRACE - no change above the noise floor (tau=0.0040) on this target across frames 40..70; the label claims a visible change; edge pairs use extrapolated masks; masks: in-span actual, edges extrapolated <=27 (including baseline)
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 1  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 1 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 1  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 1 runs; unassessable/readings never promote an event)
```

**consistent_with_unassessable_run** (exit 0):

```text
idx=0   missing_texture    SynthTarget            CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[10..17] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges actual
      run[40..47] UNASSESSABLE - neither edge observed; masks: in-span actual, edges actual
  CONSISTENT 1 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 1 events)
  run coverage             CONSISTENT 1  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 1  READING 0  (of 2 runs; unassessable/readings never promote an event)
```

**mixed_run** (exit 0):

```text
idx=0   missing_texture    SynthTarget            PARTIAL runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=1 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[60..67] PARTIAL - one edge observed; the other is unassessable; masks: in-span actual, edges actual
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 1  UNASSESSABLE 0  READING 0  (of 1 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 1  UNASSESSABLE 0  READING 0  (of 1 runs; unassessable/readings never promote an event)
```

P1's old ‘never CONSISTENT’ lighting expectation was wrong: a stronger lighting change can coincide with the label and satisfy the observation rule. AMENDMENT 1 records the correction before measurements.

### P2 — six pinned legs: prediction missed

| Leg | Observed / total edges | Event reading | Exit |
|---|---:|---|---:|
| M49_GEDGE_BL_NAT | 0/16 | CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 events) | 0 |
| M49_GEDGE_CT_NAT | 8/8 | CONSISTENT 4 (4 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 events) | 0 |
| M49_GEDGE_MO_NAT | 8/8 | CONSISTENT 4 (4 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 events) | 0 |
| M49_GEDGE_MT_NAT | 8/8 | CONSISTENT 4 (4 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 events) | 0 |
| M49_GEDGE_BL_SYN | 0/16 | CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 events) | 0 |
| M49_GEDGE_MT_SYN | 8/8 | CONSISTENT 4 (4 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 0  (of 4 events) | 0 |


Each non-blink leg has four CONSISTENT events, all with the diagnostic ring caveat. Both blink legs have four UNASSESSABLE events (eight unread runs each). Their short inclusive windows select competing transitions or fail dominance. Missing edge regions are repaired; unique association is not. All 64 tau readings are exactly 0.0040. The exact P2 readings are reproduced in Appendix A.

Example retained refusal (no threshold tuning):

```text
      run[4..5] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=18 (including baseline)
        onset UNASSESSABLE(transition 6 claimed by two edges (3 claimants)) label 4 m_edge=0.0000 base=11 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 5, delta=+18; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(transition 6 claimed by two edges (3 claimants)) label 6 m_edge=0.0000 base=11 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 3 frames incl baseline) (mask extrapolated from frame 5, delta=+18; max distance, 14 frames incl baseline)
```

### P3 — masked gameplay and bbox-only control

No NO-TRACE in any of the three masked sessions. Final default cap is 0.42. M50 remains six READING events and UNREAD-BBOX-ONLY. Appendix B prints every line for all four sessions.

**A2L_LEGA**, exit 0:

```text
CONSISTENT 2 (2 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 1  UNASSESSABLE 4  READING 0  (of 7 events)
VERDICT                  NO FAILURE FOUND (2 consistent (2 with caveat), 0 offset-notes for human review, 1 partial, 4 unassessable, 0 readings)
```

**LYRA_SMOKE_01**, exit 0:

```text
CONSISTENT 2 (2 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 6 events)
VERDICT                  NO FAILURE FOUND (2 consistent (2 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

**A1L_LEGA**, exit 0:

```text
CONSISTENT 1 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 5  READING 0  (of 6 events)
VERDICT                  NO FAILURE FOUND (1 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 5 unassessable, 0 readings)
```

**M50L_LG9**, exit 0:

```text
CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 0  READING 6  (of 6 events)
VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 0 unassessable, 6 readings)
coverage                 UNREAD-BBOX-ONLY
```

### P4 — scorer v3 and rederived cap

Immutable denominator: **572 planned keys** (A2L 208, LYRA_SMOKE 208, A1L 156), 13 perturbation/variant combinations per original run-edge. Each guard's scored and unscored key sets reconcile exactly, with no duplicates or omitted keys.

| State | Scored | Unscored | Recovered | Wrong | No-transition | Unassessable | Unsupported |
|---|---:|---:|---:|---:|---:|---:|---:|
| OFF (cap 1.0) | 178 | 394 | 134 | 0 | 2 | 42 | 0 |
| ON (cap 0.42) | 164 | 408 | 122 | 0 | 2 | 40 | 0 |


Exact table/cap/support lines:

```text
IMMUTABLE PLANNED COHORT BEFORE SCORING: 572 keys
Per session: {'A2L_LEGA': 208, 'LYRA_SMOKE_01': 208, 'A1L_LEGA': 156}; each original run-edge has 13 perturbation/variant combinations
GUARD OFF
PLANNED 572 = SCORED 178 + UNSCORED 394
  recovered        134 (23.4% of planned)
  wrong              0 (0.0% of planned)
  no-transition      2 (0.3% of planned)
  unassessable      42 (7.3% of planned)
  unsupported        0 (0.0% of planned)
  unscored run keys 394; READING/UNASSESSABLE runs supply ZERO scored cells
POSITIVE SUPPORT: 16 original edges: [('A1L_LEGA', 1, 0, 'end'), ('A1L_LEGA', 1, 0, 'onset'), ('A1L_LEGA', 3, 0, 'end'), ('A1L_LEGA', 4, 0, 'end'), ('A1L_LEGA', 4, 0, 'onset'), ('A2L_LEGA', 3, 0, 'onset'), ('A2L_LEGA', 4, 0, 'end'), ('A2L_LEGA', 4, 0, 'onset'), ('A2L_LEGA', 5, 0, 'end'), ('A2L_LEGA', 5, 0, 'onset'), ('LYRA_SMOKE_01', 0, 1, 'end'), ('LYRA_SMOKE_01', 1, 0, 'end'), ('LYRA_SMOKE_01', 1, 0, 'onset'), ('LYRA_SMOKE_01', 4, 0, 'end'), ('LYRA_SMOKE_01', 4, 0, 'onset'), ('LYRA_SMOKE_01', 5, 0, 'onset')]
DERIVED CAP 0.42; N=162 perturbation cells, not independent edges; provisional - NO DISAGREEING CELL OBSERVED, so the cap is floored from the largest m_edge seen; extrapolation, not a boundary
GUARD ON uses 0.42
GUARD ON
PLANNED 572 = SCORED 164 + UNSCORED 408
  recovered        122 (21.3% of planned)
  wrong              0 (0.0% of planned)
  no-transition      2 (0.3% of planned)
  unassessable      40 (7.0% of planned)
  unsupported        0 (0.0% of planned)
  unscored run keys 408; READING/UNASSESSABLE runs supply ZERO scored cells
POSITIVE SUPPORT: 16 original edges: [('A1L_LEGA', 1, 0, 'end'), ('A1L_LEGA', 1, 0, 'onset'), ('A1L_LEGA', 3, 0, 'end'), ('A1L_LEGA', 4, 0, 'end'), ('A1L_LEGA', 4, 0, 'onset'), ('A2L_LEGA', 3, 0, 'onset'), ('A2L_LEGA', 4, 0, 'end'), ('A2L_LEGA', 4, 0, 'onset'), ('A2L_LEGA', 5, 0, 'end'), ('A2L_LEGA', 5, 0, 'onset'), ('LYRA_SMOKE_01', 0, 1, 'end'), ('LYRA_SMOKE_01', 1, 0, 'end'), ('LYRA_SMOKE_01', 1, 0, 'onset'), ('LYRA_SMOKE_01', 4, 0, 'end'), ('LYRA_SMOKE_01', 4, 0, 'onset'), ('LYRA_SMOKE_01', 5, 0, 'onset')]
```

Configured default: **0.42**, N=162 perturbation cells with baseline readings. Derivation note: **provisional - NO DISAGREEING CELL OBSERVED, so the cap is floored from the largest m_edge seen; extrapolation, not a boundary**. There is no observed wrong-cell boundary in this cohort; this provisional value is not a validated universal envelope. The mechanism is unchanged, including the no-wrong-cell fallback. No cap was chosen to improve P2.

[full scorer stdout, including every refusal reason](D:/IntrusiveAnomalies/_reviews/079-10-evidence/scorer_v3.txt) · [complete cells and unscored ledgers](D:/IntrusiveAnomalies/_reviews/079-10-evidence/cohort_v3.json)

### Source provenance for the sweep

The full 78-group off/on sweep ran before the final named-target anchor correction and default-cap update. Its exact source snapshot is retained in [calibration-tools manifest](D:/IntrusiveAnomalies/_reviews/079-10-evidence/calibration-tools/manifest.json). All 39 unique shadow inputs were rebuilt: event/run anchor selections and per-frame mask identities are identical between that snapshot and the final code. The two sweeps pass cap 1.0 / the newly derived 0.42 explicitly, so changing the configured default does not alter them. The remaining source differences are documentation/comments. The exact [snapshot-to-final diff](D:/IntrusiveAnomalies/_reviews/079-10-evidence/final/calibration-to-final.diff) and [equivalence audit](D:/IntrusiveAnomalies/_reviews/079-10-evidence/final/complete_checks.json) are retained. All final fixtures, contracts, CLI checks, scorer self-tests and ten unperturbed bank readings ran afterward. No second full sweep of the final source is claimed.

### P5 — batch and exit contracts

Flattened-path collisions retain separate reports; forced identifier collision returns 3 before report writes. Execution error 3 survives report-only with other successful sessions. A previous-mask-only pixel remains in the union. Direct CLI controls: NO-TRACE=2, report-only=0, execution failure with report-only=3. Scorer controls exercise recovered, wrong, no-transition and unassessable cells; its 26-key tiny cohort retains eight unsupported cells. Bbox-only gives zero scored/two unscored cells; the missing-boundary PARTIAL gives two scored cells.

[CLI/class/equivalence evidence](D:/IntrusiveAnomalies/_reviews/079-10-evidence/final/complete_checks.json) · [scorer self-test](D:/IntrusiveAnomalies/_reviews/079-10-evidence/final/scorer_selftest.txt)

### P6 — independent review and limits

Claude Code's independent review is pending. This implementation's checks are not that review. The revised wrong-NO-TRACE falsifier is an above-tau pair on the selected target region in the span or either edge window. The below-tau one-pixel fixture is an expected NO-TRACE; the above-tau interior fixture is unassessable and blocks NO-TRACE. The class allowlist remains a documented producer contract, not independent proof that an injection must visibly change pixels. Extrapolated silhouettes and incomplete coverage remain disclosed limits.

## 4. Reproduction

Use the frozen tools after restoration; these commands read banked captures. Output paths below are examples for a new review run.

```powershell
python -B 'D:\IntrusiveAnomalies\_reviews\079-10-frozen\verify_capture.py' --label-pixel-gate --selftest
Push-Location 'D:\IntrusiveAnomalies\_reviews\079-10-frozen'
python -B -m unittest test_verify_capture_consistency -v
Pop-Location
$env:IAI_VERIFIER_TOOLS = 'D:\IntrusiveAnomalies\_reviews\079-10-frozen'
python -B 'D:\IntrusiveAnomalies\_reviews\079-10-frozen\m079_shift_recovery.py' --selftest
python -B 'D:\IntrusiveAnomalies\_reviews\079-10-frozen\m079_shift_recovery.py' --json 'D:\IntrusiveAnomalies\_reviews\079-10-reviewer-sweep.json'
python -B 'D:\IntrusiveAnomalies\_reviews\079-10-frozen\verify_capture.py' --label-pixel-gate --dir 'D:\IntrusiveAnomalies\_bench_sessions_bank\M49_GEDGE_BL_NAT\session_20260904-135206'
Remove-Item Env:\IAI_VERIFIER_TOOLS
```

Every bank path is also recorded in bank.json. The report scripts and validation drivers are in _reviews; their live-source paths intentionally refer to the feature checkout, so use the frozen commands above after restoration.

## 5. Time and completed boundary

First recorded clock: **2026-09-20 07:50:23 UTC**. Final audited boundary: **2026-09-20 08:31:36 UTC**. Elapsed wall time: **41m 13s**. This includes reading, implementation, tests, calibration, documentation, commits and push; it is not CPU time. Monetary/token billing is unavailable and no cost is inferred.

All owned test/sweep processes have finished and their scratch directories/links are cleaned. No replacement watcher was started. CURRENT.md records this completed boundary and the next review owner. The same coherent workstream can continue; no Codex session restart or context-setting change is recommended.

## Appendix A — exact final P2 readings

### M49_GEDGE_BL_NAT

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-10)
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
  search                   +/-4, inclusive labelled bounds only; duplicate claims refuse both edges (short-run coverage cost retained)
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[4..5] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=18 (including baseline)
        onset UNASSESSABLE(transition 6 claimed by two edges (3 claimants)) label 4 m_edge=0.0000 base=11 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 5, delta=+18; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(transition 6 claimed by two edges (3 claimants)) label 6 m_edge=0.0000 base=11 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 3 frames incl baseline) (mask extrapolated from frame 5, delta=+18; max distance, 14 frames incl baseline)
      run[9..10] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(transition 6 claimed by two edges (3 claimants)) label 9 m_edge=0.0000 base=14 pairs=6/6 (mask extrapolated from frame 9, delta=-9; max distance, 5 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 16 frames incl baseline)
        end   UNASSESSABLE(ambiguous: best=0.5455 second=0.5453 ratio=1.00 < 1.50) label 11 m_edge=0.0000 base=14 pairs=7/7 (mask extrapolated from frame 9, delta=-9; max distance, 3 frames incl baseline) (mask extrapolated from frame 10, delta=+28; max distance, 17 frames incl baseline)
idx=1   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[26..27] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=26 (including baseline)
        onset UNASSESSABLE(transition 28 claimed by two edges (3 claimants)) label 26 m_edge=0.0000 base=21 pairs=7/7 (mask extrapolated from frame 26, delta=-26; max distance, 15 frames incl baseline) (mask extrapolated from frame 27, delta=+18; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(transition 28 claimed by two edges (3 claimants)) label 28 m_edge=0.0000 base=21 pairs=6/6 (mask extrapolated from frame 26, delta=-26; max distance, 14 frames incl baseline) (mask extrapolated from frame 27, delta=+18; max distance, 14 frames incl baseline)
      run[31..32] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(transition 28 claimed by two edges (3 claimants)) label 31 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 31, delta=-18; max distance, 14 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 16 frames incl baseline)
        end   UNASSESSABLE(ambiguous: best=0.5457 second=0.5457 ratio=1.00 < 1.50) label 33 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 31, delta=-18; max distance, 12 frames incl baseline) (mask extrapolated from frame 32, delta=+28; max distance, 17 frames incl baseline)
idx=2   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[48..49] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(transition 48 claimed by two edges (2 claimants)) label 48 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 48, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 49, delta=+18; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(transition 48 claimed by two edges (2 claimants)) label 50 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 48, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 49, delta=+18; max distance, 14 frames incl baseline)
      run[53..54] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(transition 55 claimed by two edges (2 claimants)) label 53 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 53, delta=-18; max distance, 14 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 16 frames incl baseline)
        end   UNASSESSABLE(transition 55 claimed by two edges (2 claimants)) label 55 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 53, delta=-18; max distance, 12 frames incl baseline) (mask extrapolated from frame 54, delta=+28; max distance, 17 frames incl baseline)
idx=3   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[70..71] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(ambiguous: best=0.5457 second=0.5453 ratio=1.00 < 1.50) label 70 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 70, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 71, delta=+18; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(transition 75 claimed by two edges (3 claimants)) label 72 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 70, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 71, delta=+18; max distance, 14 frames incl baseline)
      run[75..76] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=18 (including baseline)
        onset UNASSESSABLE(transition 75 claimed by two edges (3 claimants)) label 75 m_edge=0.0000 base=20 pairs=6/6 (mask extrapolated from frame 75, delta=-18; max distance, 14 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(transition 75 claimed by two edges (3 claimants)) label 77 m_edge=0.0000 base=20 pairs=7/7 (mask extrapolated from frame 75, delta=-18; max distance, 12 frames incl baseline) (mask extrapolated from frame 76, delta=+13; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 8  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

### M49_GEDGE_CT_NAT

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-10)
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
  search                   +/-4, inclusive labelled bounds only; duplicate claims refuse both edges (short-run coverage cost retained)
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
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-10)
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
  search                   +/-4, inclusive labelled bounds only; duplicate claims refuse both edges (short-run coverage cost retained)
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
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-10)
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
  search                   +/-4, inclusive labelled bounds only; duplicate claims refuse both edges (short-run coverage cost retained)
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
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-10)
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
  search                   +/-4, inclusive labelled bounds only; duplicate claims refuse both edges (short-run coverage cost retained)
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[4..6] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=17 (including baseline)
        onset UNASSESSABLE(transition 7 claimed by two edges (3 claimants)) label 4 m_edge=0.0000 base=11 pairs=7/7 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 6, delta=+17; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(transition 7 claimed by two edges (3 claimants)) label 7 m_edge=0.0000 base=11 pairs=7/7 (mask extrapolated from frame 4, delta=-4; max distance, 3 frames incl baseline) (mask extrapolated from frame 6, delta=+17; max distance, 14 frames incl baseline)
      run[10..10] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(transition 7 claimed by two edges (3 claimants)) label 10 m_edge=0.0000 base=14 pairs=5/5 (mask extrapolated from frame 10, delta=+28; max distance, 21 frames incl baseline)
        end   UNASSESSABLE(ambiguous: best=0.5455 second=0.5454 ratio=1.00 < 1.50) label 11 m_edge=0.0000 base=14 pairs=6/6 (mask extrapolated from frame 10, delta=+28; max distance, 20 frames incl baseline)
idx=1   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[26..28] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=26 (including baseline)
        onset UNASSESSABLE(transition 26 claimed by two edges (2 claimants)) label 26 m_edge=0.0000 base=21 pairs=8/8 (mask extrapolated from frame 26, delta=-26; max distance, 15 frames incl baseline) (mask extrapolated from frame 28, delta=+17; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(transition 26 claimed by two edges (2 claimants)) label 29 m_edge=0.0000 base=21 pairs=7/7 (mask extrapolated from frame 26, delta=-26; max distance, 14 frames incl baseline) (mask extrapolated from frame 28, delta=+17; max distance, 14 frames incl baseline)
      run[32..32] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(transition 33 claimed by two edges (2 claimants)) label 32 m_edge=0.0000 base=23 pairs=5/5 (mask extrapolated from frame 32, delta=+28; max distance, 30 frames incl baseline)
        end   UNASSESSABLE(transition 33 claimed by two edges (2 claimants)) label 33 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 32, delta=+28; max distance, 29 frames incl baseline)
idx=2   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[48..50] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(ambiguous: best=0.5455 second=0.5455 ratio=1.00 < 1.50) label 48 m_edge=0.0000 base=23 pairs=8/8 (mask extrapolated from frame 48, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 50, delta=+17; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(ambiguous: best=0.5457 second=0.5455 ratio=1.00 < 1.50) label 51 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 48, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 50, delta=+17; max distance, 14 frames incl baseline)
      run[54..54] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(transition 55 claimed by two edges (2 claimants)) label 54 m_edge=0.0000 base=23 pairs=5/5 (mask extrapolated from frame 54, delta=+28; max distance, 30 frames incl baseline)
        end   UNASSESSABLE(transition 55 claimed by two edges (2 claimants)) label 55 m_edge=0.0000 base=23 pairs=6/6 (mask extrapolated from frame 54, delta=+28; max distance, 29 frames incl baseline)
idx=3   blink              StaticMeshActor_49     UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[70..72] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(transition 70 claimed by two edges (2 claimants)) label 70 m_edge=0.0000 base=23 pairs=8/8 (mask extrapolated from frame 70, delta=-28; max distance, 17 frames incl baseline) (mask extrapolated from frame 72, delta=+17; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(transition 70 claimed by two edges (2 claimants)) label 73 m_edge=0.0000 base=23 pairs=7/7 (mask extrapolated from frame 70, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 72, delta=+17; max distance, 14 frames incl baseline)
      run[76..76] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=19 (including baseline)
        onset UNASSESSABLE(ambiguous: best=0.5456 second=0.5456 ratio=1.00 < 1.50) label 76 m_edge=0.0000 base=20 pairs=5/5 (mask extrapolated from frame 76, delta=-19; max distance, 26 frames incl baseline)
        end   UNASSESSABLE(ambiguous: best=0.5456 second=0.5452 ratio=1.00 < 1.50) label 77 m_edge=0.0000 base=20 pairs=6/6 (mask extrapolated from frame 76, delta=-19; max distance, 25 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 0 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 4 events)
  run coverage             CONSISTENT 0  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 8  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (0 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

### M49_GEDGE_MT_SYN

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-10)
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
  search                   +/-4, inclusive labelled bounds only; duplicate claims refuse both edges (short-run coverage cost retained)
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

## Appendix B — exact final P3 readings

### A2L_LEGA

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-10)
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
  search                   +/-4, inclusive labelled bounds only; duplicate claims refuse both edges (short-run coverage cost retained)
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
idx=3   missing_texture    B_Manny_C_7            PARTIAL runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=1 UNASSESSABLE=0 READING=0} bbox=drawn obs=measured
      run[39..46] PARTIAL - one edge observed; the other is unassessable; masks: in-span actual, edges extrapolated <=26 (including baseline)
        onset TRANSITION 39 d=0.9998 tau=0.0150 label 39 delta=+0 ring d=0.0516 look at frames 38..40 note: whole-region change (lighting/camera?) m_edge=0.0024 base=11 pairs=9/9 (mask extrapolated from frame 39, delta=-5; max distance, 5 frames incl baseline) (mask extrapolated from frame 46, delta=+26; max distance, 12 frames incl baseline)
        end   UNASSESSABLE(ambiguous: best=1.0000 second=0.6836 ratio=1.46 < 1.50) label 47 m_edge=0.0024 base=11 pairs=9/9 (mask extrapolated from frame 46, delta=+26; max distance, 17 frames incl baseline)
idx=4   corrupted_texture  Cube2_8                CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=drawn obs=measured
      run[51..58] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset TRANSITION 51 d=0.9999 tau=0.0040 label 51 delta=+0 ring d=0.1421 look at frames 50..52 note: whole-region change (lighting/camera?) m_edge=0.0001 base=12 pairs=9/9 (mask extrapolated from frame 51, delta=-5; max distance, 5 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 14 frames incl baseline)
        end   TRANSITION 59 d=1.0000 tau=0.0040 label 59 delta=+0 ring d=0.1516 look at frames 58..60 note: whole-region change (lighting/camera?) m_edge=0.0001 base=12 pairs=9/9 (mask extrapolated from frame 58, delta=+28; max distance, 16 frames incl baseline)
idx=5   missing_texture    Cube2_8                CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=drawn obs=measured
      run[75..82] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=14 (including baseline)
        onset TRANSITION 75 d=1.0000 tau=0.0040 label 75 delta=+0 ring d=0.1202 look at frames 74..76 note: whole-region change (lighting/camera?) m_edge=0.0000 base=15 pairs=9/9 (mask extrapolated from frame 75, delta=-14; max distance, 14 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 5 frames incl baseline)
        end   TRANSITION 83 d=0.9999 tau=0.0040 label 83 delta=+0 ring d=0.1189 look at frames 82..84 note: whole-region change (lighting/camera?) m_edge=0.0000 base=15 pairs=9/9 (mask extrapolated from frame 75, delta=-14; max distance, 12 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 7 frames incl baseline)
idx=6   lod_popping        B_Manny_C_7            UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=projected obs=unmeasured
      UNASSESSABLE(non-manifested/empty event or no explicit frame_indices)
------------------------------------------------------------------------------
  CONSISTENT 2 (2 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 1  UNASSESSABLE 4  READING 0  (of 7 events)
  run coverage             CONSISTENT 2  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 1  UNASSESSABLE 5  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (2 consistent (2 with caveat), 0 offset-notes for human review, 1 partial, 4 unassessable, 0 readings)
```

### LYRA_SMOKE_01

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-10)
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
  search                   +/-4, inclusive labelled bounds only; duplicate claims refuse both edges (short-run coverage cost retained)
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              Cube7                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=n/a
      run[16..17] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=16 (including baseline)
        onset UNASSESSABLE(transition 16 claimed by two edges (2 claimants)) label 16 m_edge=0.0036 base=13 pairs=7/7 (mask extrapolated from frame 16, delta=-16; max distance, 16 frames incl baseline) (mask extrapolated from frame 17, delta=+1; max distance, 1 frames incl baseline)
        end   UNASSESSABLE(transition 16 claimed by two edges (2 claimants)) label 18 m_edge=0.0036 base=13 pairs=6/6 (mask extrapolated from frame 16, delta=-16; max distance, 15 frames incl baseline) (mask extrapolated from frame 17, delta=+3; max distance, 3 frames incl baseline)
      run[21..22] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=21 (including baseline)
        onset UNASSESSABLE(ambiguous: best=0.6553 second=0.6366 ratio=1.03 < 1.50) label 21 m_edge=0.0036 base=13 pairs=6/6 (mask extrapolated from frame 21, delta=-21; max distance, 17 frames incl baseline) (mask extrapolated from frame 22, delta=+1; max distance, 1 frames incl baseline)
        end   UNASSESSABLE(ambiguous: best=0.6366 second=0.6329 ratio=1.01 < 1.50) label 23 m_edge=0.0036 base=13 pairs=7/7 (mask extrapolated from frame 21, delta=-21; max distance, 15 frames incl baseline) (mask extrapolated from frame 22, delta=+5; max distance, 5 frames incl baseline)
idx=1   corrupted_texture  Cube4                  CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[27..34] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=27 (including baseline)
        onset TRANSITION 27 d=0.9999 tau=0.1334 label 27 delta=+0 ring d=0.4296 look at frames 26..28 note: whole-region change (lighting/camera?) m_edge=0.0612 base=13 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 27, delta=-27; max distance, 18 frames incl baseline)
        end   TRANSITION 35 d=0.9999 tau=0.1334 label 35 delta=+0 ring d=0.3668 look at frames 34..36 note: whole-region change (lighting/camera?) m_edge=0.0612 base=13 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 27, delta=-27; max distance, 14 frames incl baseline) (mask extrapolated from frame 34, delta=+5; max distance, 5 frames incl baseline)
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
        end   UNASSESSABLE(ambiguous: best=0.9983 second=0.9086 ratio=1.10 < 1.50) label 59 m_edge=0.2117 base=11 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 58, delta=+26; max distance, 16 frames incl baseline)
idx=4   corrupted_texture  Cube3                  CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=n/a
      run[63..70] CONSISTENT (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1) - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=14 (including baseline)
        onset TRANSITION 63 d=1.0000 tau=0.0580 label 63 delta=+0 ring d=0.0837 look at frames 62..64 note: whole-region change (lighting/camera?) m_edge=0.0187 base=11 pairs=9/9 (mask id unresolved - sole value used) (mask extrapolated from frame 63, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 70, delta=+14; max distance, 12 frames incl baseline)
        end   TRANSITION 71 d=1.0000 tau=0.0580 label 71 delta=+0 ring d=0.5494 look at frames 70..72 note: whole-region change (lighting/camera?) m_edge=0.0187 base=11 pairs=9/9 (mask extrapolated from frame 70, delta=+14; max distance, 14 frames incl baseline)
idx=5   missing_texture    Cube3                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=n/a
      run[87..89] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=14 (including baseline)
        onset UNASSESSABLE(transition 87 claimed by two edges (2 claimants)) label 87 m_edge=0.0291 base=11 pairs=7/7 (mask id unresolved - sole value used) (mask extrapolated from frame 87, delta=-14; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(transition 87 claimed by two edges (2 claimants)) label 90 m_edge=0.0291 base=11 pairs=3/3 (mask id unresolved - sole value used) (mask extrapolated from frame 87, delta=-14; max distance, 13 frames incl baseline)
------------------------------------------------------------------------------
  CONSISTENT 2 (2 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 4  READING 0  (of 6 events)
  run coverage             CONSISTENT 2  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 6  READING 0  (of 8 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (2 consistent (2 with caveat), 0 offset-notes for human review, 0 partial, 4 unassessable, 0 readings)
```

### A1L_LEGA

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-10)
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
  search                   +/-4, inclusive labelled bounds only; duplicate claims refuse both edges (short-run coverage cost retained)
  producer metadata        bbox/observable flags are reported, not confirmed by this tool
  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows
  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies
------------------------------------------------------------------------------
idx=0   blink              Cube7                  UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=measured
      run[4..5] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=7 (including baseline)
        onset UNASSESSABLE(ambiguous: best=0.9962 second=0.6817 ratio=1.46 < 1.50) label 4 m_edge=0.0000 base=4 pairs=6/6 (mask extrapolated from frame 4, delta=-4; max distance, 4 frames incl baseline) (mask extrapolated from frame 5, delta=+7; max distance, 6 frames incl baseline)
        end   UNASSESSABLE(ambiguous: best=1.0000 second=0.9994 ratio=1.00 < 1.50) label 6 m_edge=0.0000 base=4 pairs=7/7 (mask extrapolated from frame 4, delta=-1; max distance, 1 frames incl baseline) (mask extrapolated from frame 5, delta=+7; max distance, 7 frames incl baseline)
idx=1   corrupted_texture  Cube6_0                CONSISTENT runs{CONSISTENT=1 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=measured
      run[15..22] CONSISTENT - consistent with the label; does not establish cause; masks: in-span actual, edges extrapolated <=26 (including baseline)
        onset TRANSITION 15 d=1.0000 tau=0.3372 label 15 delta=+0 ring d=0.1651 look at frames 14..16 m_edge=0.1036 base=15 pairs=9/9 (mask extrapolated from frame 15, delta=-7; max distance, 7 frames incl baseline) (mask extrapolated from frame 22, delta=+26; max distance, 12 frames incl baseline)
        end   TRANSITION 23 d=1.0000 tau=0.3372 label 23 delta=+0 ring d=0.0654 look at frames 22..24 m_edge=0.1036 base=15 pairs=9/9 (mask extrapolated from frame 15, delta=-7; max distance, 5 frames incl baseline) (mask extrapolated from frame 22, delta=+26; max distance, 17 frames incl baseline)
idx=2   blink              Cube6_0                UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=2 READING=0} bbox=n/a obs=measured
      run[28..29] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=20 (including baseline)
        onset UNASSESSABLE(transition 28 claimed by two edges (2 claimants)) label 28 m_edge=0.1083 base=15 pairs=7/7 (mask extrapolated from frame 28, delta=-20; max distance, 10 frames incl baseline) (mask extrapolated from frame 29, delta=+19; max distance, 13 frames incl baseline)
        end   UNASSESSABLE(transition 28 claimed by two edges (2 claimants)) label 30 m_edge=0.1083 base=15 pairs=6/6 (mask extrapolated from frame 28, delta=-20; max distance, 6 frames incl baseline) (mask extrapolated from frame 29, delta=+19; max distance, 15 frames incl baseline)
      run[33..34] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(transition 33 claimed by two edges (2 claimants)) label 33 m_edge=0.1081 base=16 pairs=6/6 (mask extrapolated from frame 33, delta=-25; max distance, 8 frames incl baseline) (mask extrapolated from frame 34, delta=+28; max distance, 15 frames incl baseline)
        end   UNASSESSABLE(transition 33 claimed by two edges (2 claimants)) label 35 m_edge=0.1081 base=16 pairs=7/7 (mask extrapolated from frame 33, delta=-25; max distance, 6 frames incl baseline) (mask extrapolated from frame 34, delta=+28; max distance, 16 frames incl baseline)
idx=3   corrupted_texture  Cube2_8                UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=measured
      run[51..58] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(ambiguous: best=1.0000 second=0.9855 ratio=1.01 < 1.50) label 51 m_edge=0.0192 base=23 pairs=9/9 (mask extrapolated from frame 51, delta=-14; max distance, 14 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 14 frames incl baseline)
        end   UNASSESSABLE(ambiguous: best=0.9903 second=0.9855 ratio=1.00 < 1.50) label 59 m_edge=0.0192 base=23 pairs=9/9 (mask extrapolated from frame 51, delta=-14; max distance, 12 frames incl baseline) (mask extrapolated from frame 58, delta=+28; max distance, 16 frames incl baseline)
idx=4   missing_texture    B_Manny_C_12           UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=1 READING=0} bbox=n/a obs=measured
      run[75..82] UNASSESSABLE - neither edge observed; masks: in-span actual, edges extrapolated <=28 (including baseline)
        onset UNASSESSABLE(regional change m=0.427537 > cap=0.420000) label 75 m_edge=0.4275 base=16 pairs=9/9 (mask extrapolated from frame 75, delta=-28; max distance, 16 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 5 frames incl baseline)
        end   UNASSESSABLE(regional change m=0.427537 > cap=0.420000) label 83 m_edge=0.4275 base=16 pairs=9/9 (mask extrapolated from frame 75, delta=-28; max distance, 14 frames incl baseline) (mask extrapolated from frame 82, delta=+7; max distance, 7 frames incl baseline)
idx=5   lod_popping        B_Manny_C_12           UNASSESSABLE runs{CONSISTENT=0 OFFSET-NOTE=0 NO-TRACE=0 PARTIAL=0 UNASSESSABLE=0 READING=0} bbox=n/a obs=unmeasured
      UNASSESSABLE(non-manifested/empty event or no explicit frame_indices)
------------------------------------------------------------------------------
  CONSISTENT 1 (0 with caveat)  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 5  READING 0  (of 6 events)
  run coverage             CONSISTENT 1  OFFSET-NOTE 0  NO-TRACE 0  PARTIAL 0  UNASSESSABLE 5  READING 0  (of 6 runs; unassessable/readings never promote an event)
  VERDICT                  NO FAILURE FOUND (1 consistent (0 with caveat), 0 offset-notes for human review, 0 partial, 5 unassessable, 0 readings)
```

### M50L_LG9

```text
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-10)
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
  search                   +/-4, inclusive labelled bounds only; duplicate claims refuse both edges (short-run coverage cost retained)
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

**Chat review REQUIRED before merge**, after Claude Code's independent review and disposition of the P2 miss. The authorized branch work is complete; no brief has been sent.

VERDICT: NEEDS-DECISION
