# 079-07 — Independent merge re-review

**Disposition: CHANGES REQUIRED. Candidate: `bff1b0058688feb1dfe427f840e1c1ce8eac4a37`.**

The original three counterexamples now refuse, the pinned bench remains green, and the reported 780-cell tables reproduce. However, a new motion burst confined to the labelled window produces a false shift on correct labels and a full PASS on a deliberately late onset. Both happen at `m_edge = 0`. The corrected guard therefore still permits the failure this workstream is meant to address.

**Chat review REQUIRED for disposition of F1–F8 below and revised correction/evidence before merge.** This is a review report; no brief was sent to Code. Chat retains orchestration. The existing client, Section G, campaign and m51 holds remain in force.

Request: [079-06 re-review request](D:/IntrusiveAnomalies/_reviews/079-06-chat-request-for-codex-rereview.md). Reviewed 2026-09-15. Line links below point to frozen candidate bytes because the live checkout remains on m51.

## Findings

### F1 — [P1] A quiet baseline still admits an unrelated transition as the anomaly edge

The [local baseline](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:946) excludes the event window. The [cap and dominance checks](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:979) establish neither which in-window change belongs to the anomaly nor that the largest change is that anomaly.

Independent fixture: 100 frames, a 50×50 textured region in a 160×120 static picture, and a 20×20 white patch present exactly on frames **60–67**. The background inside the region scrolls once, by 10 pixels at frame **61**, then stays still. Every clean baseline pair is unchanged.

| Same RGB; label change only | Required interpretation | Candidate result |
|---|---|---|
| Labels 60–67, aligned | Alignment or refusal | **`ONSET-SHIFT(+1)`, FAIL, exit 2** |
| Labels 61–67, onset one frame late | Onset −1 or refusal | **`PASS (1 of 1 events fully checked)`, exit 0** |

Both edges have `m_edge=0`, `tau=0.004`, and 24 baseline pairs. The real onset changes `0.1600` of the region; the unrelated burst changes `0.4960` and wins by **3.1×**, comfortably clearing dominance. Removing the burst gives PASS on aligned labels and the correct `ONSET-SHIFT(-1)` on late labels.

**Required:** keep these cases unread unless the transition can be attributed to the target. Merely lowering the cap does not fix them: the aligned burst still falsely shifts with **cap 0**. The [“DIRECT CURE FOR ‘SOMETHING ELSE WON THE ARGMAX’” claim](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:745) is refuted. This finding does not prescribe motion compensation as the implementation.

Evidence: `burst_aligned`, `burst_onset_label_late`, and static controls in [probe readings](D:/IntrusiveAnomalies/_reviews/079-07-validation-probes.json); `zero_cap_burst` in [follow-up readings](D:/IntrusiveAnomalies/_reviews/079-07-validation-followups.json).

### F2 — [P1] Search constraints still allow one transition to serve two different edges

Search-before-refusal fixes the regional-cap path, but [whole-frame regions](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:935), insufficient baselines and unsatisfiable thresholds still return before searching. Their `best_k` remains absent, so the [end lower bound](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:1297) is still released.

Reproduction: aligned patch at 10–11, with a stronger background transition at onset 10. A whole-frame onset box is refused; the valid 50×50 end box then finds **onset 10 as clear 12**, reporting **`END-SHIFT(-2)`**. With the same RGB and a readable onset box, the tool correctly bounds the end search and reports PASS.

The sibling problem also exists across runs. The [next onset is bounded by the preceding labelled end](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:1279), never by its detected clear. In a fixture with RGB runs 10–12 and 15–16, and the first labelled end deliberately early at 11, clear 13 is reused as the second onset: **`ONSET-SHIFT(-2)` on the aligned onset 15**. This occurs both when the previous end is refused and when it is correctly read as `END-SHIFT(+1)`.

**Required:** preserve valid ordering/exclusion constraints across both boundaries; where they cannot be established, refuse the dependent reading. Moving only the regional-cap refusal below the search is incomplete.

Evidence: `onset_early_refusal`, `previous_end_refusal` in [probes](D:/IntrusiveAnomalies/_reviews/079-07-validation-probes.json), paired controls in [follow-ups](D:/IntrusiveAnomalies/_reviews/079-07-validation-followups.json).

### F3 — [P1] An empty edge search is called readable and fails the labels

When no candidate clears the threshold, [lines 985–988](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:985) set `status="read"` without requiring any frame pair to have been scanned. The [run combiner](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:1041) then emits NOT-VISIBLE when both empty searches take that path.

Reproduction: keep valid annotation/labels and ample clean frames, but remove the RGB files covering both edge searches. The output reports **`edges=2/2-read`**, **`0 of 0 frame(s) above tau`** at both edges, then **NOT-VISIBLE / FAIL / exit 2**.

**Required:** distinguish unavailable observations from observed absence of above-threshold change. Enforce sufficient actual search coverage before judging an edge. This is a failure of the new judgeability claim even though the underlying missing-input behavior is inherited.

Evidence: `missing_edge_frames` in [probe readings](D:/IntrusiveAnomalies/_reviews/079-07-validation-probes.json).

### F4 — [P2] Unsupported perturbations still shrink the declared fixed cohort

The scorer returns `None` when a perturbation removes/merges a run or cannot construct its coherent payload. [The sweep skips that entire session/variant](D:/IntrusiveAnomalies/_reviews/079-07-frozen/m079_shift_recovery.py:393), adding no cells to the denominator.

Reproduction: one legitimate one-frame run has two original edges and 13 exercised perturbation/variant combinations: **26 planned cells**. Onset +1 and end −1 remove the run; both variants are skipped. The scorer returns **18 cells**, not 26. One invalid run can also discard the other runs in that session's combination.

**Required:** retain unsupported cells with an explicit reason, or define and publish an immutable eligibility denominator before scoring. Do not make measured coverage determine the denominator. **The supplied four-session 780-cell artifact does not lose cells**; the general “never shrinks” guarantee is nevertheless false.

Evidence: `cohort_skip` in [probe readings](D:/IntrusiveAnomalies/_reviews/079-07-validation-probes.json).

### F5 — [P2] The scorer counts returned no-signal verdicts as refusals

[Scorer lines 298–303](D:/IntrusiveAnomalies/_reviews/079-07-frozen/m079_shift_recovery.py:298) classify every `status="read", found=False` edge as `refused`. That differs from Amendment 4 S3's definition, **NOT-MEASURABLE with a reason**. The gate can instead combine these edges into NOT-VISIBLE and FAIL.

Of the reported **385 guard-off “refused” cells, 130 are `no-change-above-tau`**; the remaining 255 carry actual refusal reasons. A known aligned one-pixel change inside a 50×50 region (`d=0.0004 < tau=0.004`) reproduces the semantic mismatch: the gate reports **NOT-VISIBLE / FAIL**, while the scorer reports **0 wrong / 2 refused**. The README explicitly acknowledges that subthreshold real changes can produce NOT-VISIBLE, but the [final line still asserts that labels and pixels disagree](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:1387).

**Required:** retain no-signal returns separately from actual refusals, and report them when interpreting correctness. Reconcile the FAIL wording with the documented detection limit. “Zero wrong returned offsets” must not become “zero wrong client verdicts.” This does **not** change the supplied guard-on 52/0/728 counts; that table has no no-signal cells.

Evidence: [cohort audit](D:/IntrusiveAnomalies/_reviews/079-07-validation-cohort.json), `below_tau` and `below_tau_score` in [follow-ups](D:/IntrusiveAnomalies/_reviews/079-07-validation-followups.json).

### F6 — [P2] The zero-motion calibration boundary crashes, and cap zero does not refuse all

[`floor_2sf(0)` returns a scalar](D:/IntrusiveAnomalies/_reviews/079-07-frozen/m079_shift_recovery.py:308), while [`derive_cap` unpacks a pair](D:/IntrusiveAnomalies/_reviews/079-07-frozen/m079_shift_recovery.py:327). Supplying a wrong cell at `m_edge=0` raises **`TypeError: cannot unpack non-iterable float object`** before the advertised no-defensible-cap result. F1 now supplies precisely that boundary case.

There is a second problem with the fallback: the scorer describes cap zero as “every edge is refused,” but [the gate refuses only `med > region_cap`](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:979). Zero-motion edges still pass, including F1's false shift.

**Required:** make zero-valued calibration inputs valid and represent the no-admissible-envelope outcome explicitly. A numeric zero must not be described as refusing everything while accepting the failing zero-motion case.

Evidence: `zero_cap_derivation`, `zero_cap_static` in [probes](D:/IntrusiveAnomalies/_reviews/079-07-validation-probes.json), `zero_cap_burst` in [follow-ups](D:/IntrusiveAnomalies/_reviews/079-07-validation-followups.json).

### F7 — [P2] A fully read run plus an unread run is summarized as wholly unread

[Event aggregation](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:1323) takes the worst run token. PASS plus NOT-MEASURABLE therefore becomes NOT-MEASURABLE instead of PARTIAL.

Reproduction: one event with run 10–11 fully read/aligned, and run 25–26 refused for whole-frame boxes. The detail correctly prints **`edges=2/4-read`** and the first run's PASS. The event nevertheless reads NOT-MEASURABLE and the session prints **`UNREAD (0 of 1 events checked — every event is an unread surface)`**.

**Required:** aggregate coverage over all event edges. This code follows Amendment 4's declared severity order, but that rule conflicts with its PARTIAL definition and the [README's statement that UNREAD means nothing could be read](D:/IntrusiveAnomalies/_reviews/079-07-frozen/client-readme.md:220). Correct the design rule as well as the implementation.

Evidence: `partial_multirun` in [probe readings](D:/IntrusiveAnomalies/_reviews/079-07-validation-probes.json).

### F8 — [P2] Flattened relative paths still collide and overwrite reports

[Replacing separators with `__`](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:1798) is not a unique encoding when a path component already contains `__`. Reports are still opened with [mode `"w"`](D:/IntrusiveAnomalies/_reviews/079-07-frozen/verify_capture.py:1860).

Reproduction: `a__b/session_same` and `a/b__session_same` both become **`a__b__session_same.txt`**. One session fails and one passes. The summary repeats the same identifier with opposite verdicts, and only the PASS report survives. The missing-label session in the same batch correctly preserves exit 3 under report-only.

**Required:** use a collision-safe path representation and verify uniqueness before writing. This implements the recipe declared in Amendment 4; the recipe itself does not provide the claimed guarantee.

Evidence: `batch` in [probe readings](D:/IntrusiveAnomalies/_reviews/079-07-validation-probes.json).

## Answers to Q1–Q7

### Q1 — Original probes and new examples

**Original corrections reproduced:** all three 079-05 fixture generators now return NOT-MEASURABLE at the shipped cap. Their regional medians are `0.12`; the alternating case has `tau=0.48`. All **24 candidate self-tests** pass, including both burst-free shifted controls returning `ONSET-SHIFT(-1)` with the guard explicitly disabled. The independent stationary late-onset control also recovers −1 with the shipped cap.

**The mechanism is not closed:** F1 gives aligned false SHIFT and shifted false PASS at zero baseline motion; F3 gives NOT-VISIBLE with no search observations.

### Q2 — Cohort and classification audit

I reconstructed the expected cell keys from the four unperturbed source annotations and checked both saved tables. Each contains **780 unique keys**, with no missing or extra keys. The cohort is **60 original eligible run-edges × 13 combinations**; coherent delta zero is deliberately omitted as redundant. It is not literally 7×2 combinations per edge.

| Session | Original edges | Cells per guard |
|---|---:|---:|
| A2L_LEGA | 16 | 208 |
| LYRA_SMOKE_01 | 16 | 208 |
| A1L_LEGA | 12 | 156 |
| M50L_LG9 | 16 | 208 |
| **Total** | **60** | **780** |

| Guard | Recovered offsets | Wrong offsets | Scorer's refusal bucket |
|---|---:|---:|---:|
| OFF | 376 | 19 | 385 |
| ON | 52 | 0 | 728 |

I independently re-ran **28 session/perturbation/variant groups, 432 cells**, covering **all 19 OFF wrong cells and all 52 ON recovered cells**. Expected offsets, returned offsets, regional medians, thresholds, classifications and reason strings match the saved artifact exactly. Arithmetic and signed-offset classification checks found no discrepancy.

The two qualifications are F4's unsupported-perturbation omission and F5's no-signal/refusal conflation. Also, agreement with the injected offset is a perturbation test: it does not independently establish that the original real label or selected RGB edge was correct. The scorer already states that limitation, appropriately.

### Q3 — Cap and the meaning of N

The derivation reproduces **`0.00014000000000000001`, N=736**. The binding cell is A1L event 0, original run 4–5, onset, annotation-only both +1: expected **−1**, returned **+1**, `m_edge=0.00014459224985540774`. The exact gap below that cell is about **3.18% of its value**.

“Provisional heuristic” is appropriate calibration provenance, but **N=736 means repeated perturbation cells that reached a baseline, not 736 independent real edges or successful validations**. All 52 retained successes are the same **four original edges from A2L events 4 and 5**, exercised across 13 combinations. Their clean-pair medians range from **0 to 0.00005820721769499418**. The observed positive support is therefore essentially static regions between events, from two events in one session.

State that scope in the header/README. Prefer “cutoff floored below the smallest observed wrong-offset cell” to “largest measured level with no wrong answers.” Quiet clean pairs do not establish quiet or attributable changes inside the labelled window; F1 demonstrates the distinction.

### Q4 — Search-before-refusal

**Incomplete.** The cap/dominance refusal paths retain a candidate; earlier refusals do not. Next-run onset bounds ignore even a successfully detected preceding clear. F2 reproduces both directions.

### Q5 — Wording

The threshold-bound correction, producer-metadata legend, bbox-only explanation, 90% rectangle rule, provisional-cap caveat and removal of the old “no pixel change at all” definition are improvements. All-refused gameplay sessions now correctly print UNREAD rather than PASS.

Remaining corrections:

- Scope PASS/SHIFT claims to what the measurement establishes; F1 contradicts their present label-correctness interpretation. Remove the dominance “direct cure” assertion.
- Separate no detected signal from established label disagreement in the final FAIL line (F5).
- Make PARTIAL/UNREAD coverage match all event edges (F7).
- Correct the header's 7×2 shorthand, explain N, and expose the narrow retained positive sample (Q2–Q3).
- The [Section G note at line 1269](D:/IntrusiveAnomalies/_reviews/079-07-frozen/office-rdp-card.md:1269) says exit 3 means **“NOTHING WAS READ”** immediately after discussing both single and batch modes. In a mixed batch, exit 3 means **at least one session could not run**; others may have been read. Our mixed batch proves this. The later “UNREAD means nothing at all” statement also depends on fixing F7.

The prospective card note correctly preserves the original predeclared field reading and the held status. No office reading was performed here. A small evidence-count correction is also due: the six nominated pinned sessions produce **64 detailed edges**, all with `m_edge=0`, rather than the 56 stated in Code's final report.

### Q6 — Batch tests

The missing-label/report-only reproduction now returns **3**, closing the earlier exit-suppression finding. The reported tests cover useful execution paths, but are insufficient to establish identifier uniqueness: F8 still overwrites an independently generated report. The code's exception and gate-code-3 paths preserve failure status on inspection; I did not independently rerun every one of Code's twelve batch cases.

### Q7 — Merge readiness

**CHANGES REQUIRED: F1–F8.** The 52/0/728 result is reproducible evidence about this calibration cohort. It does not establish the claimed reliability of client verdicts, and F1 directly supplies a wrong PASS and wrong SHIFT under the shipped settings.

**Chat review REQUIRED for these findings, the corrected design where indicated, and revised evidence before merge.** The next mandatory checkpoint is Chat's disposition of this report; a corrected candidate then needs the independent merge re-review that Chat commissions. No Code brief or implementation authorization is issued here.

## Verification, evidence and final state

- All 24 self-tests pass. All six pinned `M49_GEDGE_*` sessions return **PASS 4 / SHIFT 0 / NOT-VISIBLE 0 / PARTIAL 0 / NOT-MEASURABLE 0**, `tau=0.004` at every edge, exit 0. [Ten full bank readings](D:/IntrusiveAnomalies/_reviews/079-07-validation-bank.json) also reproduce A2L's **2 fully / 0 partly / 5 unread of 7**, and UNREAD on LYRA, A1L and M50L.
- [Validation script](D:/IntrusiveAnomalies/_reviews/079-07-review-validation.py) loads candidate Git/frozen bytes without changing the checkout and disables bytecode writes. Modes: `probes`, `followups`, `cohort`, `bank`. [Initial predictions](D:/IntrusiveAnomalies/_reviews/079-07-probe-predictions.json) and [follow-up predictions](D:/IntrusiveAnomalies/_reviews/079-07-followup-predictions.json) were saved before their respective fixture runs. RGB and label construction, complete outputs and structured per-edge returns are retained in the linked evidence.
- Candidate tool SHA-256: `A77CC41C64C5A4DB940C1E1B177C0DB9B8C14736BB8817B2A2FF32F20123D691`. Scorer v2: `D98CB5AE4375DB06FFA170F8A4CEAABC46DD32BB4EC1C44B38E0D61777E7512E`. Unchanged `measure_label_offset.py`: `15538658DDA9C14DF791B6D4B1E746C4FFEF94519AC9F7B59989DB62AB9933D1`.
- `git diff --check 57e91cd bff1b00` passes. `Source/`, `Shaders/` and `measure_label_offset.py` are unchanged against both the previous candidate and master. The candidate and local origin-tracking ref remain `bff1b00`; master/origin-master remain `ac13700`; live checkout remains **m51 `53bf725`**, tracked-clean with the same ten untracked handoffs. CaptureBench remains tracked-clean at **`472a409`** with its existing untracked tools; its scorer matches the frozen reviewed bytes. No remote fetch was required or performed.
- All review processes finished and this review's scratch directories/links were removed. Bank frames and labels were opened read-only. No plugin/CaptureBench edits, build, capture, cook, checkout, merge, push, mailbox brief or external message. Last watcher observation: **idle, PID 18520**.
- The mailbox report's wrapper is **UNDECLARED** because its parser did not accept the embedded bold GREEN text as a literal verdict line. That pre-existing queue hold was left untouched; Chat owns its disposition. It is separate from the correctness findings above.

This remains the same coherent review workstream; no fresh Codex session or context-setting change is recommended. Documentation/test improvements do not release the campaign or client gates.
