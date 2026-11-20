# 081-04 — Codex m55 Stage 1 report: pre-implementation delta check

**2026-09-21 — NEEDS-DECISION. Stage 1 implementation has not started.**

Input: [081-03 implementation request](D:/IntrusiveAnomalies/_reviews/081-03-codex-implement-m55-v1.md). Reviewed v2 at **`ae2a9677edf9efcb49e315b47966489606747ac6`**, equal to origin. Only the active v2 text is cited; the superseded fold is excluded.

## 0. Delta check of v2

**The stop condition is met: F2 is NOT RESOLVED.** The request says: “If anything there is NOT RESOLVED, stop and report NEEDS-DECISION before building.” This report fulfils that precheck; it does not claim completion of the Stage 1 implementation or gates.

The newer request controls over the older plan wherever it differs. In particular, I accept the explicit rulings on unavailable pre-onset visibility, same-size camera cuts, the split ramp test, the shipped Lit material, catalogue 10→12, and adding `cam_delta`. None of those accepted choices is being reopened.

### 0.1 F1–F9 disposition

“RESOLVED” here means the design addresses the original finding for measurements-only v1. It does not mean runtime validation has passed.

| Finding | Status | Active v2 lines and assessment |
|---|---|---|
| **F1 — missing onset / gradual change** | **RESOLVED** | [7–12](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:7), [400–452](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:400), [822–823](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:822). No present/absent verdict or observable change; phase onset coverage is separate from later quiet pairs; reference comparison and K's limited meaning are explicit. Producer ramp remains UNEXERCISED as accepted in 081-03. |
| **F2 — identity and expired metadata** | **NOT RESOLVED** | [256–305](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:256), [345–351](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:345), [810–820](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:810). Durable receipts and ordering fix the expired-snapshot concept, but the epoch is stamped too late and the mask join still lacks unique serving-view identity. See blockers D1–D3. |
| **F3 — progressive output / final event record** | **PARTIAL** | [307–343](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:307), [460–478](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:460), [483–558](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:483). Separate sidecar, running names and phase scope resolve the core output problem. Finalisation ordering, reset persistence and the zero-phase reason field remain inconsistent or unspecified (D4–D5). |
| **F4 — unreachable no-effect gate** | **RESOLVED** | [667–737](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:667), [794–803](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:794). New eligible bench twins replace the wrong null control; StuckMipNoHold is correctly a no-labelled-frames case. Its final-record schema still needs the small correction in D5. |
| **F5 — calibration / verifier outcomes as truth** | **PARTIAL** | [741–768](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:741), [831–835](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:831). Calibration and forced G270 verdicts are removed. The new promise that eight bins permit arbitrary later threshold sweeps is false; see D6. This is a data-capability wording issue, not an objection to the accepted eight bins. |
| **F6 — unlike verifier comparisons** | **RESOLVED** | [626–661](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:626). Separate oracle uses explicit pair indices, current mask, byte >8, count/count and mean/mean, with a limited arithmetic/transport claim. No change to peaks or 079 outcomes. |
| **F7 — delivered bytes / lifetime / cost** | **PARTIAL** | [59–107](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:59), [307–396](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:307), [837–856](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:837). Canonical encoder bytes, supported formats, PNG scope and CPU worker resolve representation. Freezing the mask before filtering contradicts the immutable-buffer contract; admission/back-pressure and actual retained-byte accounting need clarification (D7). |
| **F8 — mandatory hide-depth zero** | **RESOLVED** | [573–595](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:573), [827–828](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:827). Depth-silencing case remains accepted; empty silhouette is unmeasured. 081-03 explicitly accepts that v1 establishes no pre-onset visibility. |
| **F9 — control refusal destroys raw data** | **RESOLVED for v1** | [109–192](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:109). No control threshold or subtraction; validity and eligibility are separate; denominator checks are explicit. Correct the overclaim about excluded spill in D6. |

### 0.2 Blocking identity defects

#### D1 — Capture epoch/cut identity must be bound before asynchronous work

[Lines 275–277](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:275) add render metadata, then say the **rest** of the receipt is stamped in the drain, where the current run epoch is available. That includes `run_epoch` and `cut_counter`. [Lines 347–351](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:347) promise to reject older results using those same fields.

A concrete failure sequence is:

1. A colour request/readback is issued under epoch/cut E.
2. The bench reset or world transition advances the epoch/cut and clears change-stage state.
3. The old readback completes afterwards.
4. Receipt assembly stamps the **current** epoch/cut E+1 onto those old pixels.
5. The stale-result check sees E+1 and cannot reject their actual origin.

Two delayed consecutive captures can both acquire E+1 and satisfy the N−1 check. This is a specification flaw, not a claim that I ran and observed the proposed code. The current capturer has asynchronous in-flight state; clearing the new change-stage queue does not inherently cancel that upstream state.

**Needed ruling/correction:** Bind run/cut generation when the request is created, carry it unchanged through colour and mask submissions, and reject old-generation work before assembling/admitting a new receipt. A writer's later success should be a separate immutable completion record or a newly constructed final receipt, not mutation of a previously published immutable receipt. Gate a reset with an **old colour readback and old mask result still in flight**, not only an already assembled old change-stage result.

#### D2 — Family frame + view index does not uniquely identify the serving view

[Lines 298–305](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:298) reduce the mask receipt addition to `family_frame` and `view_index`. No common colour/mask capture token, unique family identity or selected view owner is specified. A view index is local to its family. Moreover, “index 0” is not a check that the family contains only one view.

This is confirmed by the installed engine: [SceneRendering.cpp:4552](D:/UESource/UnrealEngine/Engine/Source/Runtime/Renderer/Private/SceneRendering.cpp:4552) assigns the **same** scene frame number to every family in the supplied `ViewFamilies` collection. Multiple families can therefore each have view index 0 and the same frame number. The proposed tuple can match different views. The existing key lookup is also reusable, not consuming.

**Needed ruling/correction:** Select/enforce a supported capture view/family and carry a unique actual serving-render identity shared by its colour, mask and counts. Retain the independent request IDs and an explicit correlation token where necessary; colour and mask currently have separate request serials, so merely comparing their raw IDs is not a solution. Include epoch and geometry in the mask receipt. Reject repeated/unsupported view callbacks rather than accepting the first index of each family.

This does **not** reverse the accepted same-size-camera-cut limitation. Camera-motion interpretation and proving that two buffers came from the same serving view are different obligations.

#### D3 — G-COALESCE does not force coalescing, and G-VIEW misses the alias above

[G-COALESCE, line 815](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:815) assumes two simultaneous fires make the mask pass serve several arms. But [ArmTargetMaskOwn:1246–1251](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:1246) issues **one arm after collecting the live tags**. Several targets in that one arm do not prove `PendingArms.Num() >= 2`.

The gate can therefore pass with no coalescing or pixel-owner loss exercised. Likewise, a synthetic index-1 refusal in [G-VIEW, line 816](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:816) does not test two distinct families both at index 0, or a duplicate callback under one capture ID.

**Needed ruling/correction:** Force two distinct capture-mask requests to remain pending until one serving pass consumes them. Record served-request count, request IDs, actual render identity and payload owner; require the counter proving at least two served requests to be nonzero. Separately force the same-frame/index-0 wrong-family and duplicate-callback cases. These are required discriminating gates for F2, not a request to ship multi-view support.

### 0.3 Other holes and contradictions to settle

#### D4 — Closure, flushing and reset need one ordering contract

[Lines 347–351](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:347) clear pending phase records and the queue on reset, including FinishRun's epoch increment. [Lines 460–471](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:460) instead require bounded flush, phase closure and durable records, with later results rejected. Specify the order so reset cannot discard the records that must be finalised.

Also distinguish **a captured phase ending** from **its evidence record becoming immutable**. The game thread can see phase/event end before the unordered writer finishes earlier window frames. Closing immediately and rejecting all later writer results would discard normal evidence on short events. Queue a closure watermark and resolve admitted work through it, bounded by an explicit terminal deadline, before final closure; otherwise declare the intended loss and reason. Teardown must persist final records too, not merely clear memory when FinishRun was not reached.

The closed enum has no specified reason for a failed **current** writer, or a normal end/teardown with unresolved work. `predecessor_undelivered` is defined specifically for the previous writer (line 510). Choose a complete mapping or revise the enum; the implementer should not silently emit a thirteenth value or mislabel the failed current frame as its predecessor.

#### D5 — The sidecar key is present, but several promised audit fields are not

The join exists in v2:

- Within the run directory, `pair.session_index` joins `labels.jsonl.session_index`.
- Within that row, `mask_value` identifies the target entry, and `event` can be checked against `anomalies[].id + "@" + anomalies[].start_frame`. The final event record supplies the target. The event-key construction is defined at [lines 402–405](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:402); pair fields are listed at [525–543](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:525).
- `session_index` and `prev_session_index` name the two `Actual_Frames/frame_%05d.png` files. The current index names `target_mask/frame_%05d.png`; §6.2 gives those exact templates.
- If labels are disabled, the PNG/sidecar join still works. A missing labels row cannot be independently validated as though it existed.

Thus **no new labels.jsonl key is required just to perform the join**. File names or row position alone must not supply event identity; the run directory is part of the key.

However:

1. The hitch row at [line 820](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:820) says both capture times are published, but the sidecar field table has **neither**. Epoch, cut, actual render/view identity and receipt geometry are also not exported there. Specify a compact receipt/audit block and whether times mean submit, drain or another stage.
2. The new 081-03 instruction adds `cam_delta`, superseding v2's “no raw pose delta.” Define its representation: centimetres and degrees cannot form one unqualified physical magnitude. Separate quantised translation, angular and FOV deltas are one clear option; none should become a validity rule.
3. G-NOLABEL requires an event-level `reason: no_labelled_frames`, but the event field table at [545–558](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:545) has no reason field. A zero-phase event cannot put it inside `phases[].reasons`.
4. The earlier R4 brief asks for final per-event records in **both run summary and sidecar**, whereas v2's run-summary block has only counters. Confirm sidecar-only event detail as the v1 contract or add the missing summary representation.

#### D6 — Histogram and control wording still overclaim

[Lines 744–746](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:744) say the eight-bin histogram can be swept at **any** other threshold without images. It cannot. Differences `[17,32]` and `[24,25]` have the same histogram, count, sum, mean and >8 count; their strict >25 counts are 1 and 0.

With the accepted bins, exact strict-> thresholds recoverable from the histogram are bin boundaries **0, 2, 4, 8, 16, 32, 64** (equivalently >=1,3,5,9,17,33,65). Arbitrary thresholds require finer data or the images. Keep the accepted bins and correct the capability claim rather than quietly expanding the numeric design.

[Line 119](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:119) says excluding all target masks excludes their spill too. It excludes their **mask pixels**; light, shadow or other changes outside those masks remain in the control. The later caveat already acknowledges that. Remove the stronger claim.

#### D7 — Immutable mask capture and budget accounting need precise boundaries

[Line 318](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:318) freezes Gray into `TSharedRef<const TArray<uint8>>` at the MoveTemp on source line 1295. But [source lines 1298–1304](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:1298) subsequently zero non-event tags. The numeric contract correctly wants the **filtered** delivered mask.

Filter the mutable local first, then freeze/share it before mask writer and change-stage handoff. Treat colour conversion and delivery completion similarly: immutable identity/pixels, separately recorded completion.

[Lines 330–334](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:330) do not establish the 64 MB cap by saying concurrent writer jobs bound allocation. The two-buffer limit is on admitted canonical buffers; other writer jobs can still retain buffers before admission, and mask jobs can already hold references/copies. Specify which bytes the m55 counter owns, when reservations occur, and how admission fails without retaining an uncounted extra reference. Back-pressure must not block all writer workers while the serial stage waits for the predecessor held by one of those workers. The existing writer's total memory bound remains a separate m51 limitation; do not claim this stage bounds it.

### 0.4 Every row of 081-02 §4: gate or UNEXERCISED

**No whole row is entirely omitted from v2's table.** Some compound paths have only nominal or incomplete coverage:

| Original row | v2 coverage | Delta result |
|---|---|---|
| First frame / insufficient pre-roll | line 810, G-FIRST | Declared. |
| Predecessor drained | 811, G-RECEIPT | Declared; receipt lifetime is the right approach. |
| Out-of-order / held batches | 812, G-ORDER | Declared; 081-03 strengthens it to two actual readbacks ready in one drain. Use that instruction, not only reversing worker admission. |
| Capture skipped | 813, G-SKIP | Declared **synthetic wrong-predecessor** test; does not actually skip capture or test cleanup of its missing completion. Add that path or explicitly mark it unexercised. |
| Readback/encode/write failure | 814, G-UNDELIVERED | Writer failure declared. Readback failure and mask-write failure are not separately exercised/declared; current-failure reason missing (D4). |
| Mask coalescing / lost owner | 815, G-COALESCE | Named but not forced; D3. |
| Repeated callback / another view | 816, G-VIEW | Index-1 test exists; index-0 alias/duplicate-callback paths uncovered; D2–D3. |
| Same-size cut / world transition / reset | 817–818, G-EPOCH + UNEXERCISED cut | Cut limitation accepted. Epoch late-result gate strengthened by 081-03, but stamp timing must change (D1). |
| Letterbox / rect / projection | 819, G-EXTENT | Synthetic rect perturbation declared; natural letterbox unavailable. Projection-only change is not tested by rect mismatch; keep as a published camera caveat and name coverage explicitly. |
| First capture after hitch | 820, UNEXERCISED | Declared; claimed exported times absent from schema (D5). |
| Missing/late result / final drain | [821, G-LATE](D:/IntrusiveAnomalies/_reviews/081-04-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:821) | Force-stop exists, but forcing a stop does not guarantee a pending result. Require an actual delayed completion and exact non-mutation counts; D4. |

No executed gate or synthetic model is being passed off as producer coverage.

## 1. Action taken and state

- Read the full active v2, its revision journal, Chat's disposition, the processed revision brief/report, and the new implementation instruction. Rechecked the cited source and installed engine.
- Created a [54-file frozen snapshot and preflight](D:/IntrusiveAnomalies/_reviews/081-04-evidence/preflight.json) directly from `ae2a967`; no checkout switch.
- Stopped before source edits, builds, staging, captures, cooks or runtime gates. **All Stage 1 runtime gates remain UNRUN.** Stages 2 and 3 remain unstarted.
- Plugin checkout remains **m51 / `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`**, tracked clean; eleven pre-existing untracked handoffs preserved.
- Master remains **`031a103b18b90b2ce9dd626b22e9498717c60815`**; feature remains **`ae2a9677edf9efcb49e315b47966489606747ac6`**, both matching origin.
- CaptureBench, the detached Lyra worktree, binaries and containers were not modified. No process was started for building/capture; none was killed.
- No commit, push, merge, tag or mailbox brief. This is the pre-implementation stop, so no implementation journal/field-reference change was committed. The report and CURRENT update record the boundary.

[Postflight verification](D:/IntrusiveAnomalies/_reviews/081-04-evidence/postflight.json) records repository/ref/untracked preservation and report-link checks.

## Questions for chat

1. **Identity:** approve binding epoch/cut at request creation and a shared serving-render/capture identity with an enforced single-view owner, plus forced coalescing and duplicate/wrong-family gates (D1–D3)?
2. **Closure/refusals:** confirm flush-through-watermark before immutable finalisation, persistence before reset, and the complete closed reason mapping for failed current delivery and terminal unresolved work (D4).
3. **Artifact contract:** confirm exported receipt/timing fields, the representation/units of `cam_delta`, the zero-phase event reason, and whether final event details belong only in the sidecar or also in run_summary (D5).
4. **Implementation clarifications:** accept filter-then-freeze mask ownership, explicit m55 admission/byte accounting, and the limited histogram/control wording corrections without changing the accepted measurement definitions (D6–D7).

## 2. Disposition and next action

**Chat review REQUIRED before resuming Stage 1 implementation**, because 081-03 explicitly makes an unresolved delta finding a stop. The next action belongs to Chat: rule on the items above through the owner. No new implementation permission is otherwise being requested; the existing Stage 1 authorization resumes once this specified precheck hold is resolved.

After Stage 1 implementation and its gates, stop again for the owner-carried Chat ruling before Stage 2, exactly as 081-03 requires. Claude Code's independent review remains after Stage 3, before any merge.

Same coherent workstream; no fresh session or context-setting change is recommended. No in-flight work is left under Codex ownership.

VERDICT: NEEDS-DECISION
