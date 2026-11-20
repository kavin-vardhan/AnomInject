# 079-08 — third independent merge review

**Disposition: CHANGES REQUIRED. Do not merge `1cbc07e`.**  
Reviewed 2026-09-20. Candidate: `fix/verifier-field-validity`, **`1cbc07e0fb21224f6a136871270766d8155481ce`**.

The reported regressions and bank results reproduce, including all 46 self-tests. The new mask-mode boundary still admits wrong verdicts. Accurate masks do not establish which change was caused by the anomaly; the implementation also reuses a fixed mask across each search, promotes an undetected edge to PASS, and misassigns transitions between nearby runs. Six actionable findings follow.

**Chat review REQUIRED for disposition of F1–F6 and the corrected design/evidence before merge.** This is the next mandatory checkpoint. Chat retains orchestration under the operating contract's 2026-09-15 correction; this report does not dispatch implementation work.

## Findings

Finding numbers below belong to **this** review. Source links point to frozen candidate files, because the live checkout remains `m51`.

### F1 — [P1] A correct mask still lets unrelated lighting certify a wrong label

**Location:** [verify_capture.py:1049](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:1049), [candidate ranking:865](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:865).

The new `attributed` flag means only that a binary mask exists. A change to pixels on the target can be caused by lighting, ordinary animation or motion, so that flag cannot license the claimed anomaly-edge verdict.

Independent fixture: a stationary 40×40 target exists throughout 100 frames, with an exact 1,600-pixel mask on every frame. A 10×10 texture anomaly is present on frames **60–67**. A global lighting step of +40 occurs at **61** and persists. The masks remain exact; no occluder or mask error is involved.

| Labels | Required result | Candidate result |
|---|---|---|
| 60–67, correct | Alignment or explicit refusal | **`ONSET-SHIFT(+1)`, HIGH, exit 2** |
| 61–67, onset one frame late | Recover onset −1 or refuse | **`PASS (1 of 1 events fully checked)`, HIGH, exit 0** |

Both edges have `m_edge=0`, `tau=0.004`. The real onset changes `0.0625` of the target; lighting changes `1.0`, winning by **16×**. The end is correctly located at 68. Removing the lighting step makes the controls PASS and `ONSET-SHIFT(-1)`, respectively.

**Required for closure:** prevent this lighting transition from becoming an anomaly timing verdict, or narrow the advertised result to a measurement that the available evidence actually establishes. Mask presence plus baseline stillness and dominance is insufficient. A smaller nonnegative cap cannot reject these `m_edge=0` cases. The header, README and G263 claim that masks resolve attribution must be corrected with the design.

Evidence: `lighting_aligned`, `lighting_late_onset`, `static_aligned`, `static_late_onset` in the [compact evidence](D:/IntrusiveAnomalies/_reviews/079-08-evidence-summary.json).

### F2 — [P1] Searches use one boundary mask and ignore missing masks inside the window

**Location:** [boundary-region construction:1513](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:1513), [fixed-region comparison:774](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:774), [coverage loop:853](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:853).

The onset search uses the mask at labelled `rs`; the end search uses the mask at labelled `re`. Each binary region is then reused for every candidate pair and baseline pair. These are boundary snapshots, **not the target's silhouette at each compared frame**. The `missing` check covers RGB images/differences, not the masks needed to support attribution.

Two independent reproductions:

1. **Accurate occlusion masks:** the same target/anomaly as F1, without lighting. An occluder covers the target's left half on frames 61–65. Every mask correctly excludes those 800 occluded pixels; the 100-pixel anomaly on the right remains visible throughout 60–67. The candidate nevertheless counts the occluder through the full masks from frames 60 and 67. It reports onset **+1** at 61 and end **−2** at 66, each with `d=0.5`, versus the genuine edges' `0.0625` (**8×** dominance). Correct labels produce `ONSET-SHIFT(+1)`, exit 2.
2. **Missing mask:** delete only the mask PNG for frame 61, retaining its label reference and all RGB frames. The candidate reports **full PASS**, `attributed=true`, `missing=0` on both edges. Deleting RGB frame 61 instead correctly refuses the onset with two missing pairs and returns PARTIAL. The RGB coverage repair works; the claimed mask coverage does not.

An exact moving-target mask also produces a false onset +1 in the additional control. Requested fault injections reproduce further failures: a 10-pixel mask dilation admits a background-ring step and produces a false +1; a one-frame-lagged mask with a late onset label produces full PASS. **The dilation/lag cases deliberately corrupt the supplied masks** and do not prove failure on valid inputs by themselves. The accurate occlusion and movement cases do.

**Required for closure:** define and enforce attribution for the actual compared frame pairs, including unavailable/empty masks and motion/occlusion. A boundary mask cannot silently stand in for all those observations. Merely loading a different mask per candidate will not, by itself, solve F1's lighting problem.

Evidence: `occluder_aligned`, `missing_mask61`, `missing_rgb61`, `precise_mask_target_move`, `dilated_mask_ring_burst`, `lagged_mask_target_move` in the [compact evidence](D:/IntrusiveAnomalies/_reviews/079-08-evidence-summary.json).

### F3 — [P1] Event aggregation upgrades an undetected end to full PASS

**Location:** [_event_token:1258](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:1258), especially lines 1265–1266.

The new coverage aggregation counts every `status == "read"` edge, including `found=false, offset=None`. When all edges have that status and no run is NOT-VISIBLE, it returns PASS even when the run itself returned PARTIAL.

Fixture `no_detected_end`: exact masks, anomaly **60–90**, labels **60–67**. Onset 60 is found and aligned. The end search 64–72 finds no above-threshold transition. The run correctly says **PARTIAL**, but the event says **PASS**, and the session says **`PASS (1 of 1 events fully checked)`**, exit 0. No claim about the true end at 91 is justified by that window.

The same conflation affects partial coverage: `one_frame_early` has one no-signal edge and one ambiguous/refused edge, with **no confirmed aligned edge**, yet reports `PASS-PARTIAL`.

**Required for closure:** preserve the distinction between an observed search window, a detected/aligned edge, a no-signal result and a refusal throughout run, event and session aggregation. Full PASS requires every required edge to be found and aligned; an undetected edge must not supply agreement.

Evidence: `no_detected_end` and `one_frame_early` in the [compact evidence](D:/IntrusiveAnomalies/_reviews/079-08-evidence-summary.json).

### F4 — [P1] A later run's onset can become the preceding run's clear and suppress its own detection

**Location:** [next-label upper bound:1532](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:1532), [unconditional `prev_clear`:1536](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:1536), [next onset lower bound:1518](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:1518).

The preceding end search stops at `next_start - 1`, but `next_start` is the label being tested. When that label is late, the search includes the next run's genuine onset. Its argmax is then accepted as the earlier clear and used to exclude that same transition from the next onset search.

Fixture uses the candidate's own `_codex07_fixture`: one stationary 50×50 target, exact full-object masks, true texture-anomaly runs **10–11 and 15–16**, patch sides **10 and 30**. With correct labels, all four edges align. Change only the second onset label **15→16**:

| Edge | Ground truth | Candidate |
|---|---|---|
| First onset | 10, aligned | 10, aligned |
| First clear | 12, aligned | **15, `END-SHIFT(+3)`** |
| Second onset | 15, offset −1 | **No signal; search clipped to 16–16** |
| Second clear | 17, aligned | 17, aligned |

The false clear's `d=0.36` beats the actual clear's `0.04` by **9×**, at `m_edge=0`; all four edges are reported read. The event gives a HIGH-confidence wrong `END-SHIFT(+3)`. Moving the second onset label one frame early instead correctly returns +1.

The mixed-mode variant also confirms the stated isolation rule is not implemented across runs: make the first run bbox-only and leave the second masked. The first run prints READING, but its unattributed `best_k=15` still clips the masked run's search to frame 16. The result is PASS-PARTIAL, with the real late onset missed.

**Required for closure:** prevent misassociation of one run's transition with another and preserve uncertainty from disputed/unattributed bounds. Correctness must be checked per edge in both directions, including a bbox-only predecessor. Reinstating `max(labelled, detected)` does not solve this problem and would reintroduce D1's one-frame exclusion.

Evidence: [ordering results](D:/IntrusiveAnomalies/_reviews/079-08-validation-ordering.json), generated by [ordering probes](D:/IntrusiveAnomalies/_reviews/079-08-ordering-probes.py).

### F5 — [P1] An explicitly different mask ID is treated as the target

**Location:** [_region_from_mask:657](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:657), especially the unconditional sole-value fallback at 660–662; newly trusted by [the attribution flag:1049](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:1049).

The function documents a fallback for unresolved `mask_value=0`, but the fallback also runs when a **positive requested ID is absent**. A label requesting 222 with a PNG containing only 111 is accepted as `mask(sole v111)` and declared attributed.

Fixture `foreign_mask_value` puts the real target/anomaly at the usual location on 60–67. Its labels explicitly request **222**. The mask contains only **111**, over a separate object which changes on 61–67. The candidate reports **`ONSET-SHIFT(+1)` for SynthTarget**, HIGH, exit 2, while both edge sources say `mask(sole v111)`. The input contains an observable identity mismatch; it must not authorize a target verdict.

**Required for closure:** reject an explicit nonzero ID mismatch from the attributed path. Keep any justified unresolved-ID compatibility behavior distinct from contradicting a known requested identity. Although this fallback predates the patch, the new verdict boundary now relies on it as proof of attribution.

### F6 — [P2] The scorer credits an edge from a run that the gate declares READING

**Location:** [m079_shift_recovery.py:348](D:/IntrusiveAnomalies/_reviews/079-08-frozen/m079_shift_recovery.py:348).

The gate correctly chooses mask mode **per run**, requiring both edge regions to be attributed. The scorer checks only the individual edge's `attributed` flag. It can therefore count a recovered/wrong offset from a run that produced no verdict.

Fixture `mixed_run` has the correct 60–67 labels and an exact onset mask, but no mask at the labelled end frame 67. The gate reports **READING**, `edges=0/2-read`, `UNREAD-BBOX-ONLY`. Scoring the unchanged annotation reports **onset recovered**, end refused. That is the same half-masked-run inconsistency A5.5 identifies for the event combiner, still present in the calibration consumer.

**Required for closure:** use the gate's run-level eligibility when assigning all five scoring classes. Neither edge of this READING run can supply a successful verdict. This does **not** change the reproduced 572-cell table: the current nominated sessions have no mixed runs. It matters when the tool is used to rederive the cap on additional captures.

Evidence: `mixed_run` and `mixed_run_score` in the [compact evidence](D:/IntrusiveAnomalies/_reviews/079-08-evidence-summary.json).

## Answers to D1 and D2

**D1 — the objection to `max(labelled, detected)` is right; general soundness is not established.** For a true one-frame run at 60 with both labels late at 61, the true clear is 61. A floor of labelled onset +1 would exclude it. The independent `one_frame_late` case retains and correctly reports end −1 under the current rule; its onset is explicitly refused as ambiguous. The aligned one-frame control passes. The early-labelled counterpart does not recover both edges and exposes F3's coverage wording problem. F4 demonstrates a separate failure across runs. Accept the rationale for the deviation, but not a claim that detected-else-labelled makes the complete ordering algorithm sound.

**D2 — acceptable transparent correction, with the original failed prediction retained.** The fixture truly has its first clear at 13 while its label puts that edge at 12, so `END-SHIFT(+1)` is the known answer. A5.4 preserves the erroneous table and appends the correction. Its per-edge expectations are stronger than an event-token check and reproduce exactly: `+0,+1,+0,+0` for the control; `+0,r,+0,+0` for the refused-end case. This is a disclosed correction after observing the result, not a successful original predeclaration. It creates no separate merge blocker.

## What independently passed

### Q1 — existing fixtures and bank readings

The candidate's **46-case self-test passes**. This includes the unmasked 079-05/079-07 cases returning READING, the two burst-free masked negative controls returning onset −1, the masked burst twins returning `0,0` and `−1,0`, and the five strengthened per-edge assertions.

I also ran the original review generators against the frozen candidate: three original 079-05 cases and eight original 079-07 cases all give READING / UNREAD-BBOX-ONLY under the revised scope. The original RGB/label construction was retained; obsolete 079-05 post-gate baseline diagnostics were omitted. [Original regression evidence](D:/IntrusiveAnomalies/_reviews/079-08-validation-originals.json).

All ten nominated bank sessions reproduce:

| Sessions | Independent result |
|---|---|
| Six `M49_GEDGE_*` legs | PASS 4 each; **64/64 edges mask-attributed**, all offsets zero |
| `A2L_LEGA` | PASS 2, NOT-MEASURABLE 5; PASS-PARTIAL, 2 fully / 0 partly / 5 unread |
| `LYRA_SMOKE_01` | NOT-MEASURABLE 6; UNREAD |
| `A1L_LEGA` | NOT-MEASURABLE 6; UNREAD |
| `M50L_LG9` | READING 6; UNREAD-BBOX-ONLY, 0/16 edges attributed |

All ten exit 0 and produce no SHIFT or NOT-VISIBLE. These results establish the reported regression behavior, not correctness on F1–F4's additional valid-mask cases. [Bank evidence](D:/IntrusiveAnomalies/_reviews/079-08-validation-bank.json).

### Q2 — mask semantics and observation coverage

**No:** it is one boundary mask reused through a search, not per-frame silhouette attribution. **No:** an interior missing mask does not trigger the new coverage refusal. **Yes:** missing RGB pairs in the final search window are now refused, and the original empty-window test no longer becomes NOT-VISIBLE. See F2.

### Q3 — scorer v2.1 and calibration

The saved table has exactly the independently enumerated **572 unique planned keys**, with no omissions or extras in either guard state: 44 original run-edges × 13 perturbation/variant combinations. Expected offsets and returned-offset classifications reconcile.

| Guard | Recovered | Wrong | Refused | No-signal | Unsupported | Total |
|---|---:|---:|---:|---:|---:|---:|
| OFF | 368 | 17 | 175 | 12 | 0 | 572 |
| ON, 0.00014 | 52 | 0 | 520 | 0 | 0 | 572 |

Independent reruns covered **33 complete session/perturbation groups, 508 cells**: 300 guard-OFF and 208 guard-ON. All `expected`, `m_edge`, `tau`, `offset`, `outcome` and `reason` values matched the banked JSON exactly. This reran **all 17 wrong cells, all 12 no-signal cells and all 52 retained successes**; it was not a new exhaustive execution of both 572-cell sweeps.

| Session | Wrong cells rerun | No-signal cells rerun |
|---|---:|---:|
| `A2L_LEGA` | 4 | 4 |
| `LYRA_SMOKE_01` | 7 | 5 |
| `A1L_LEGA` | 6 | 3 |

The cap rederives to **0.00014000000000000001**. Its binding cell remains A1L event 0, original run 4–5, onset, annotation-only both +1: expected **−1**, returned **+1**, `m_edge=0.00014459224985540774`. The 52 successes are 13 variants of each of **four original edges**, both edges of A2L events 4 and 5, in **one session**. N=572 is the number of perturbation cells reaching a baseline, not 572 independent confirmations.

Additional checks:

- Tiny one-frame cohort retains **26/26** planned cells: 9 recovered, 8 refused, 1 no-signal, **8 unsupported**. The denominator no longer shrinks.
- A wrong cell at `m_edge=0` returns **NO ADMISSIBLE ENVELOPE**, without the old exception. A recovered-only zero cell yields numeric zero with the extrapolation note.
- The explicit sentinel refuses both otherwise valid static edges and produces UNREAD. Numeric cap zero still permits static controls, as now documented.
- The published 079-06 re-split correctly separates its historical 130 no-signal cells, including 118 from M50L. It is historical evidence, not the current mask cohort.

The remaining scorer issue is F6. Separately, F1's new wrong result at zero shows why the reproduced bank-derived cap cannot establish general verdict correctness. [Cohort audit and reruns](D:/IntrusiveAnomalies/_reviews/079-08-validation-cohort.json).

### Q4 — batch IDs and exit contract

**Pass for the reviewed changes.** The collision fixture retains both differently classified reports:

- `a__b__session_same__b4aabc5f.txt` → `a__b/session_same`, PASS.
- `a__b__session_same__51fcd56f.txt` → `a/b__session_same`, NOT-VISIBLE.

The summary prints both source paths. A third missing-label session preserves **exit 3 under report-only**. I additionally forced `_batch_key` to return the same identifier for every session: the pre-write uniqueness check returned 3, listed collisions and wrote **no report files**. That injected test bypassed only the already-passed self-test; production naming and gate logic were unchanged in the ordinary collision test. Source inspection and the supplied healthy/failing batch evidence agree with normal exits 0/2 and report-only suppression of 2 only.

### Q5 — wording

The corrected FAIL phrase, above-threshold detection-limit explanation, Section G batch exit-3 correction, bbox-only notice, 44×13 denominator and four-edge positive-support disclosure are supported by the reviewed evidence.

The mask-sufficiency statements in the header, README and G263 remain materially incorrect under F1/F2/F5. Full-PASS/partial-agreement descriptions are contradicted by F3. These need to follow the corrected semantics, not just softer confidence wording.

Two smaller wording corrections can accompany that work:

- [Header:201](D:/IntrusiveAnomalies/_reviews/079-08-frozen/verify_capture.py:201) and [README:234](D:/IntrusiveAnomalies/_reviews/079-08-frozen/client-readme.md:234) still say **four sessions**; the current cap is rederived on three masked sessions.
- Describe 0.00014 as the **two-significant-figure cut-off strictly below the smallest observed wrong-cell m_edge**. `derive_cap` floors a number; it does not find the largest measured safe level or the largest real value below that boundary. The current scorer/header wording still blurs those claims.

The Section G notice preserves the historical predeclared reading and explicitly flags that bbox-only captures cannot satisfy it. That handling is acceptable; this review provides no new office/client reading.

## Exact merge disposition and prior-finding closure

**Q6: CHANGES REQUIRED — F1, F2, F3, F4, F5 and F6 above.** The first four include failures with accurate masks and known RGB timing. F5 is a detectable identity mismatch accepted as attribution; F6 is a demonstrated scorer/gate eligibility mismatch.

Relative to the previous review: old F4 denominator, F5 no-signal accounting, F6 zero-boundary handling and F8 batch naming are repaired. Old F3 is repaired for RGB coverage, with mask coverage still open. Old F1's bbox scope reduction works but its mask-mode replacement is insufficient. Old F2's original refusal cases pass but cross-run ordering remains unsound. Old F7's PASS-plus-refusal case passes, while the new aggregation introduces the false PASS in this report's F3.

**Chat review REQUIRED for these findings, any revised verdict boundary, and fresh evidence before merge.** The immediate next step is Chat's disposition of this report. No brief to Code was written or sent. Client/Section G acceptance, campaign/release readiness, `m51` and the separate paused <other-home-project> work are not released by these regression results.

## Reproducibility and state

Read [compact evidence](D:/IntrusiveAnomalies/_reviews/079-08-evidence-summary.json) first; it contains per-edge results, all 29 rerun wrong/no-signal cells, batch checks and bank summaries. Full new-fixture and self-test output is in [probe evidence](D:/IntrusiveAnomalies/_reviews/079-08-validation-probes.json). Predictions were written before the independent fixture runs. Fixture truth and constructors remain available after temporary images were removed.

Reproduction scripts:

```powershell
python -B D:\IntrusiveAnomalies\_reviews\079-08-review-validation.py probes
python -B D:\IntrusiveAnomalies\_reviews\079-08-review-validation.py cohort
python -B D:\IntrusiveAnomalies\_reviews\079-08-review-validation.py bank
python -B D:\IntrusiveAnomalies\_reviews\079-08-ordering-probes.py
python -B D:\IntrusiveAnomalies\_reviews\079-08-original-regressions.py
```

The scripts load frozen bytes, keep bank reads read-only, and clean their own scratch fixtures/links. Source references throughout this report are for `1cbc07e`, not the checked-out `m51` implementation.

| Reviewed file | SHA-256 |
|---|---|
| `verify_capture.py` | `def462109b5e687214e1ea91b3ed2ddec7d764b99628691b57ab2ef815c3c441` |
| `measure_label_offset.py` | `15538658dda9c14df791b6d4b1e746c4ffef94519ac9f7b59989db62ab9933d1` |
| Scorer v2.1 | `f94eaeef6e37b1e6a48c4d5be7ec174eff780e1bd70bd5855c42d58f057d7ded` |

The scorer hash matches the frozen copy, working untracked CaptureBench file and banked `m079_shift_recovery_v21.py`. `Source/`, `Shaders/` and `tools/measure_label_offset.py` have no diff between master and the candidate; `git diff --check` passes. AMENDMENT 5 commit `0e7d96c` precedes implementation `d7f4e40`.

Final observed repository state: AnomalyInjector **m51 `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`**, master/origin-master **`ac13700d99fe116056f5aa11611f8931a563a3f2`**, candidate and local origin-tracking candidate **`1cbc07e`**. CaptureBench remains **`472a409dc70ae78d01a38d08fc3bddb15fe41c58`**. Both tracked trees are clean; existing untracked handoffs/tools were preserved. No checkout, implementation, build, capture, cook, merge, push or external message occurred.

All review processes finished and this review's scratch directories/links were removed. The watcher status file is stale (`idle`, PID 18520, updated 2026-09-15 23:26:17); that PID was absent at the final check. No watcher was started or changed. The 079-07 mailbox report declares GREEN; that declaration is evidence submitted for review, not this review's disposition.

The separate Codex storage-migration status still reports its 2026-09-18 failure and retained originals. It was read only; no migration or backup cleanup was attempted. The current review boundary is recorded above the preserved earlier material in `ToCodex/CURRENT.md`. No fresh Codex session or context-setting change is recommended for this continuing workstream.
