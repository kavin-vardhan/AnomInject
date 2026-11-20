# 079-05 — Independent Codex merge review

**Disposition: CHANGES REQUIRED. Do not merge or deliver `57e91cd` as a validated client verifier.**

**Chat review REQUIRED for disposition of these findings and the corrected verifier/calibration before merge.** The new evidence contradicts the previous merge-ready ruling. Chat remains the orchestrator; this report issues no brief or execution authorization. The separate m51 design, campaign and release holds remain in force.

The unsatisfiable-threshold refusal is useful, and I reproduced the ten reported bank readings. However, the new motion guard still permits false NOT-VISIBLE and false SHIFT readings below its cap. More seriously, the sole supposedly fully recovering real session does **not** recover every run and edge: the calibration checker loses those failures when it scores the event summary. The evidence therefore does not support the claimed validity envelope.

## Scope, exact state and verification

Request: [079-04 review request](D:/IntrusiveAnomalies/_reviews/079-04-chat-request-for-codex-merge-review.md). Reviewed on 2026-09-15:

- Candidate: `fix/verifier-field-validity`, **`57e91cd8c9f5d3e46a455b7de0b94964f036c0ec`**; local remote-tracking ref matches. Base/master/origin-master: `ac13700d99fe116056f5aa11611f8931a563a3f2`.
- Live checkout remained **m51, `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`** throughout. I read candidate blobs directly from Git; I did not switch branches. Tracked tree clean; ten pre-existing untracked handoffs retained.
- Full 079-01 through 079-04 reports and journals, predictions followed by Amendments 1–3, changed gotchas, current operating contract, tool diff, checker and client Step 6 read. The amendments' commits precede their implementation commits. Historical rejected proposals are not treated as the current design.
- Candidate changes eight files: seven documentation files and `tools/verify_capture.py`. No diff under `Source/` or `Shaders/`; `measure_label_offset.py` is byte-identical to master. `git diff --check` passes. CaptureBench remains tracked-clean at `472a409`; its checker is untracked and matches the banked copy.
- Python 3.13.1 / Pillow 12.2.0. Both existing black-frame selftests and all 17 label-pixel selftests pass. The latter disable the cap for most moving fixtures, as documented.
- Re-ran the candidate on all ten nominated bank sessions. **Complete output matches the 079-04 evidence after removing only the session path lines.** Independently ran master's unchanged gate on all six pinned legs: same counts/verdicts, all `PASS 4`, `tau=0.0040`. This is verdict/statistic equivalence, not byte equality of the newly expanded report text.
- Additional deterministic offline fixtures exercise the unchanged candidate functions. They are generated test images, **not UE captures or new real-footage evidence**. A separate A2L shift audit observes `_dominant_edge` return values without changing them, to inspect results hidden by the event summary.

All source links below point to exact frozen copies under `_reviews/079-05-frozen`, because the live checkout contains a different version. SHA-256:

| File | SHA-256 |
|---|---|
| Candidate `verify_capture.py` | `F0A96DB0A3AEE669FB83BE36F8D9F9446BA55B487DCC70A4E815514CE0234C3F` |
| Unchanged `measure_label_offset.py` | `15538658DDA9C14DF791B6D4B1E746C4FFEF94519AC9F7B59989DB62AB9933D1` |
| `m079_shift_recovery.py` | `9FA26D4F4172F887E3194D227F1FE7032337086B9359449A792B0A7A7C580EBE` |

## Findings

### 1. [P1] The motion guard does not establish that an event is measurable

[Candidate lines 926–934](D:/IntrusiveAnomalies/_reviews/079-05-frozen/verify_capture.py:926) compute one median from the **first** 24 eligible clean pairs. [Lines 1021–1036](D:/IntrusiveAnomalies/_reviews/079-05-frozen/verify_capture.py:1021) instead calibrate each event on its **nearest** 24 pairs. Every event then reuses the opening motion value at line 1046. A quiet opening can authorize verdicts on a later moving scene.

There is also a spatial problem even if sampling becomes local in time: a whole-frame changed-pixel fraction can be small while the target region changes strongly. It is a content-dependent image-change proxy, not a camera displacement measurement or a bound on regional interference.

**Reproduced with aligned labels covering frames 60–67:**

| Offline fixture | Reported `M_med` | Nearest-24 whole-frame median | `tau` | Candidate verdict |
|---|---:|---:|---:|---|
| Quiet opening, motion begins at frame 36; larger background movement at 61 | 0.0000 | 0.1250 | 0.1200 | **ONSET-SHIFT(+1)** |
| Moving 50×50 region in a static 320×240 picture | 0.00390625 | 0.00390625 | 0.1200 | **ONSET-SHIFT(+1)** |
| Same small region, alternating scroll increments | 0.00390625 | 0.005859375 | 0.4200 | **NOT-VISIBLE (bbox-only)** |

The anomaly patch is explicitly drawn only on frames 60–67. In the first two tests, the correct onset/clear differences are 0.2640/0.2480, but a background change wins the onset search. In the third, both true edges change 10% of the region; they remain below `tau=0.4200`, which is satisfiable, so the new `tau >= 1` refusal cannot help. The first two print LOW/CONTAMINATED but still fail the label; the third prints MED with no contamination warning.

These are counterexamples to the claimed protection, not estimates of real-world error frequency. The spatial examples are below **both 0.040 and 0.0049**. Merely lowering the constant does not repair this mechanism; a zero-motion opening also survives a zero cap because the comparison is strict `>`.

**Required:** validate the actual temporal and regional support used for every judged run/edge, including motion bursts and changing scene content. Refuse unsupported attribution before emitting a label failure. Add these independently aligned positive controls to the tests, alongside the existing genuine negative controls. Neither restoring the rejected ring subtraction nor a producer-declared provenance veto follows from this finding.

Evidence: [reproducible offline probe](D:/IntrusiveAnomalies/_reviews/079-05-review-validation.py), [full probe readings](D:/IntrusiveAnomalies/_reviews/079-05-validation-probes.json).

### 2. [P1] “FULL RECOVERY” hides failed runs and incorrect end edges in A2L itself

[Checker lines 101–115](D:/IntrusiveAnomalies/_reviews/079-05-frozen/m079_shift_recovery.py:101) request quiet output and retain only the fourth token of each `idx=` line. [Lines 125–180](D:/IntrusiveAnomalies/_reviews/079-05-frozen/m079_shift_recovery.py:125) score that token, omit NOT-MEASURABLE events, and call the session FULL when every remaining event token matches. They do not inspect every contiguous run or both edges. Candidate [lines 1109–1146](D:/IntrusiveAnomalies/_reviews/079-05-frozen/verify_capture.py:1109) can hide an incorrect end behind an onset shift, and combine a successful shifted run with an unread run into one shifted event.

I repeated the annotation-only shifts on **`A2L_LEGA/session_20260904-180418`**, using the existing `_shifted_copy_of` helper and cap 1.0, matching the calibration's disabled guard. Actual readings:

| Label delta | Published event score | Correct onset offsets | Correct end offsets |
|---|---|---:|---:|
| 0 | 6/6 | 8/8 runs | 8/8 runs |
| +1 | 6/6 | 8/8 runs | **7/8 runs** |
| −1 | 6/6 | **6/8 runs** | **6/8 runs** |

Exact counterexamples:

- **Delta +1, event 2, shifted run [29..30]:** expected onset/end offsets `(-1,-1)`; returned **`(-1,+2)`**. The event still reports `ONSET-SHIFT(-1)`.
- **Delta −1, event 0, shifted run [8..9]:** expected `(+1,+1)`; returned **`(-2,-1)`**. The detailed run is NOT-MEASURABLE beyond its range, while the event reports `ONSET-SHIFT(+1)` from its other run.
- **Delta −1, event 2, shifted run [27..28]:** expected `(+1,+1)`; returned **`(-4,-1)`**. Again, an unread run is hidden by the event's expected shift token.

Thus the reported six-event summaries are reproducible, but **the only allegedly fully recovering real session already loses localization at `M_med` 0.0043–0.0048**. This is stronger evidence than merely having a small sample near the cap. It does not retroactively change the predeclared event-token P3 count; it rejects the broader inference that this count validates every run and edge.

**Required:** score a fixed eligible cohort of individual runs and both edges. Publish coverage and reasons for every excluded/refused edge; loss of coverage under a perturbation must not improve “full recovery.” Exercise onset-only and end-only perturbations too. Correct the A2L full-recovery claim and re-evaluate the cap after the scorer is fixed. Preserve the original results as history with a correction.

Evidence: [complete shift audit, including every returned edge](D:/IntrusiveAnomalies/_reviews/079-05-validation-recovery.json). No inference about why the original real labels were produced is required for this finding.

### 3. [P2] Client wording overstates both the readings and the cap's evidence

[Step 6, line 170](D:/IntrusiveAnomalies/_reviews/079-05-frozen/client-readme.md:170) defines NOT-VISIBLE as **“no pixel change at all across the claimed window.”** The code only searches near the edges for changes exceeding a learned threshold. The third fixture above changes 10% at both correct edges and still produces that verdict. The definition repeats the misleading absence claim that motivated this work.

[Header lines 126–129](D:/IntrusiveAnomalies/_reviews/079-05-frozen/verify_capture.py:126) say the cap is “MEASURED, not chosen” and is the level where recovery disappears. [Step 6, line 186](D:/IntrusiveAnomalies/_reviews/079-05-frozen/client-readme.md:186) says the boundary was measured “exactly.” No real session was measured at 0.040; Amendment 3 explicitly **rules** that number, and finding 2 undermines its positive anchor. The header's “never calls a correct label NOT-VISIBLE” assertion at lines 155–156 is also refuted.

All-refused sessions still print **`PASS 0 ... NOT-MEASURABLE 6` followed by `VERDICT PASS`** ([lines 1192–1201](D:/IntrusiveAnomalies/_reviews/079-05-frozen/verify_capture.py:1192)). This aggregate behavior is inherited, but it becomes central to the new refusal-based product. The following explanatory sentence does not make the headline a successful validation. The README currently defines PASS as alignment without distinguishing event PASS from this aggregate PASS.

**Required:** describe NOT-VISIBLE as no detectable above-threshold edge in the searched region/windows, with no guarantee of absence. Explain `(bbox-only)` explicitly: it used the supplied box rather than an event mask. Mark any retained cap as a provisional heuristic; remove exact-boundary and universal-correctness assertions. Distinguish all-unread/partly-read summaries from fully checked PASS, independently of compatible process exit codes. Fix the stale ring-normalized function docstring at lines 838–841.

### 4. [P2] Batch reports overwrite distinct sessions with the same folder name

[Lines 1503 and 1519](D:/IntrusiveAnomalies/_reviews/079-05-frozen/verify_capture.py:1503) use only the final folder component as both the summary identifier and the output filename, opening it with `"w"`.

Reproduction: `a/session_same` passes; `b/session_same` fails. Both run, but only **one `session_same.txt` survives**, containing the second session. The summary lists the identical name twice, so it cannot identify which path passed. A read-only inventory of the real bank finds **1,409 session-shaped directories and 509 colliding basename groups**. For example, `A1F_A1F_NAT/session_20260904-163120` and its `_try1` copy collide. Some bank copies may be identical; that does not make basename uniqueness a valid assumption for client batches.

**Required:** use a stable unique relative session path in summaries and output paths, and retain a complete report for every attempted session. Test duplicate folder names with different results. Evidence is in the batch section of [the probe readings](D:/IntrusiveAnomalies/_reviews/079-05-validation-probes.json).

### 5. [P2] Batch `--report-only` silently changes the CANNOT-RUN exit contract

[Line 1541](D:/IntrusiveAnomalies/_reviews/079-05-frozen/verify_capture.py:1541) returns zero whenever `report_only` is true, including after missing labels, exceptions or gate code 3. Single-session mode deliberately preserves code 3 at [line 1619](D:/IntrusiveAnomalies/_reviews/079-05-frozen/verify_capture.py:1619).

The offline batch fixture includes a discovered session without `labels.jsonl`. It prints **CANNOT RUN**, sets `worst=3`, then returns **0**. This differs from the equivalent individual invocation and can make an unread batch appear successfully completed to its caller.

**Required:** retain execution/refusal errors under `--report-only`, consistently with individual mode. Suppress only the verdict-failure code that report-only already suppresses. Test missing labels, malformed/unreadable inputs and a mixture of valid sessions and errors.

## Answers to the seven questions

### Q1 — Is the diagnosis complete?

**No.** The `tau >= 1` impossibility is valid and merits its explicit refusal. It explains the two unsatisfiable banked events, not every potentially false reading. Findings 1–2 establish additional mechanisms: regional motion hidden by whole-frame averaging, motion omitted by opening-only sampling, and a largest-change search that can select background motion. These operate while `M_med <= 0.040` and `tau < 1`.

There is also a source-level calibration mismatch to preserve as a limitation: tau is learned on the event's anchor region, but later runs and the end can use different masks/boxes (lines 1024–1035 versus 1071–1085). A moving target can change the sampled content without recalibrating that region. I did not isolate that as the cause of the banked edge misses.

Two precision corrections: tau is **not bounded to d's ceiling**, rather than mathematically unbounded—its inputs are bounded, so it has a finite bound. And without office readings of tau/motion, this home reproduction does not establish the complete cause of the owner's M2 observations. No real unshifted label failure is newly attributed here.

### Q2 — Is shift injection a sound known-answer test; what does it not prove?

**Useful and capable of failing, but insufficient as implemented.** Moving an annotation by delta should move a correctly identified fixed pixel edge's reported offset by minus delta. This is a useful sensitivity check. It does not establish that the original selected pixel edge was the correct anomaly edge or that the unshifted real label was independently correct. Requiring PASS at delta zero is a test expectation, not independent ground truth.

The copy changes annotation windows only; `labels.jsonl` and masks remain at original indices. Consequently region selection, clean-frame membership and even the measurable range can change. A2L's 0.0043–0.0049 variation is deterministic **between different perturbed inputs**, not observed repeat-run drift on immutable input. Test annotation-only errors and coherent per-frame-label errors separately, while keeping RGB ground truth independent.

It also does not prove all-run/all-edge localization, sensitivity to smaller anomalies, robustness across camera/content regimes, freedom from false positives, or producer-to-RGB pairing correctness. The current scorer's full-recovery claim is specifically refuted in finding 2. Rejecting the original circular mask-onset proposal was correct; that does not make its replacement complete.

### Q3 — Is 0.040 defensible?

**Not as a validated client acceptance envelope.** Halving the first sampled failure assumes a relationship between motion and recovery that these heterogeneous scenes do not establish. There is no tested real interval from 0.0049 to 0.040, and now the positive anchor itself fails the stronger per-run check.

I would not approve shipping merely by changing to **0.0049 + “provisional”**: the counterexamples already sit below that number. A narrower provisional guard can be a diagnostic policy, but it is not evidence that labels are judgeable below it. Keep client validation held until the regional/temporal validity rule and scorer are corrected.

Evidence that would settle the decision: independently established real RGB onsets/ends; a fixed run/edge cohort; onset-only, end-only and combined ±1 perturbations; recovery and refusal coverage reported separately; held-out sessions spanning the proposed boundary; and varied target sizes, contrast, sparse texture, moving targets, parallax and motion bursts. Synthetic controls must cover those failure mechanisms, but cannot substitute for the real cohort. Recalibrate without tuning away recorded failures.

### Q4 — Where does whole-frame motion under-read?

A camera moves along a largely flat wall or beneath a uniform sky, with a small textured foreground object in the label box. Most background pixels barely change in grayscale; a static HUD adds more unchanged area. The foreground has strong parallax and texture changes, so it can dominate regional d while whole-frame M stays low. A short turn during the labelled interval is also excluded from the clean pool. The small-region fixtures demonstrate the averaging mechanism; the quiet-prefix fixture demonstrates the time-selection mechanism. They are analogues, not claims that these exact scenes occur in the bank.

### Q5 — Are the refusals and bbox tag understandable?

**The numeric reasons are useful; the surrounding claims need correction.** `threshold unsatisfiable` is precise. `camera motion` should identify a measured image-change proxy and its sampled scope, not imply it measures physical camera speed. `region covers the picture` should state the configured 90% bounding-rectangle exclusion; the implementation does not compare with an outside ring. Explain `(bbox-only)` as described in finding 3.

`bbox=... obs=...` is reported producer metadata, not an independent measurement made by this checker. It does not report per-frame completeness. M3 visibility fields may help inspect a capture, but are not an independent proof of RGB/label alignment; the held m51 pairing/identity work is not closed by recommending those fields.

### Q6 — Any behavior beyond the stated scope?

- **Yes:** new batch report collisions and code-3 suppression (findings 4–5).
- Single-session label-gate return logic remains 0/2/3; its `--report-only` still preserves 3. All-NOT-MEASURABLE yielding aggregate PASS/0 is inherited, now exposed much more often; it must not be described as successful alignment validation.
- The nearest-24 baseline is an intentional behavior change even when the cap does not fire. This branch is not merely an unsatisfiable-threshold guard or the last one-constant commit.
- Overlay functions and black-frame gate/selftest are structurally unchanged; black-frame tests pass. No change to `measure_label_offset.py`, capture source, shaders or schema.
- Section G's document is byte-unchanged, but its command invokes this changed verifier, so its readings can change. The current operating-contract hold on field Section G readings remains; no office run was made, no old prediction was rewritten and no client/M3 gate was waived. The current card's old header names and “always exits 0” prose are stale and should receive a prospective correction before renewed use.

### Q7 — Merge readiness and exact required changes

**CHANGES REQUIRED.** Before a new merge-gate review:

1. Repair or conservatively refuse the temporal/regional cases in finding 1, with aligned positive controls and meaningful negative controls. A cap-only edit is insufficient.
2. Make recovery scoring cover every required run and both edges with a fixed denominator; preserve refusal coverage. Correct the A2L claim, rerun the declared evidence, and re-evaluate the envelope from that result.
3. Correct client/header semantics and cap provenance, explain the bbox-only limitation, and distinguish unread/partial session summaries from fully checked PASS.
4. Make batch report identifiers and filenames collision-free, with complete per-session evidence.
5. Preserve CANNOT-RUN/error exit status in batch report-only mode, matching individual mode.

The required scope is correction and re-review of the verifier and its evidence. It does not authorize capture changes, ring subtraction, a provenance veto, another physical campaign, merge, delivery, push or a brief to Code.

## Evidence and completion boundary

- [Validation probe and deterministic fixture construction](D:/IntrusiveAnomalies/_reviews/079-05-review-validation.py) — run with `probes`, `bank` or `recovery`; frozen candidate loaded from Git with bytecode writes disabled.
- [New counterexamples and batch reproductions](D:/IntrusiveAnomalies/_reviews/079-05-validation-probes.json).
- [Ten candidate bank readings](D:/IntrusiveAnomalies/_reviews/079-05-validation-bank.json) and [six master readings](D:/IntrusiveAnomalies/_reviews/079-05-validation-master.json).
- [A2L shift audit, all edge returns](D:/IntrusiveAnomalies/_reviews/079-05-validation-recovery.json).

All review processes completed and temporary fixtures/copies were removed. The bank and plugin repositories were not edited; no capture/build, mailbox dispatch, merge or external message occurred. The watcher was last observed idle. Review artifacts are under `_reviews/079-05-*`; the continuity pointer records this bounded disposition.

**Chat review REQUIRED for the next verifier disposition and corrected evidence before merge, because the earlier acceptance evidence is contradicted.** Chat retains orchestration ownership. No fresh Codex session is needed for this completed review; no context setting was changed.
