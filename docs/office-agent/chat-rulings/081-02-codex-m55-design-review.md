# 081-02 — Codex independent design review: m55 change evidence

**Date:** 2026-09-21  
**Design disposition: CHANGES REQUIRED.** The measurement is worth building, but the design at `287ad3e` is not ready to implement as written.

**Chat review REQUIRED before approving the revised design for implementation.** The request reserves the findings and plan choices for Chat's ruling. A later change to the meaning of `observable` also needs the owner/client decision required by the operating contract; unchanged JSON keys do not waive that decision.

## 1. Scope and evidence

Reviewed the complete 713-line prediction and 148-line journal at **`287ad3ea137aa15f16d1dc68ce929e899245fc24`**, against their actual base **`031a103b18b90b2ce9dd626b22e9498717c60815`**. Their diff contains only those two documents. Read the relevant capture, mask, writer, anomaly-state and verifier source from that exact Git object, plus the 080-04/080-06 records and 079 closure handoff.

The live plugin checkout is **`m51` / `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`**, not the reviewed branch. Plugin implementation links below point to a [frozen review snapshot](D:/IntrusiveAnomalies/_reviews/081-02-evidence/preflight.json), so they cannot accidentally cite m51's different source. `CURRENT.md` still describes the earlier 079-13 step; the new request, closure handoff and live refs supersede that saved task state.

Validation performed:

- Read-only source/API inspection, including the installed engine's RDG declaration.
- [Synthetic counterexamples](D:/IntrusiveAnomalies/_reviews/081-02-evidence/review_probes.json), using the reviewed verifier's existing pixel statistic and a literal model of the proposed latch.
- [Read-only measurements](D:/IntrusiveAnomalies/_reviews/081-02-evidence/bank_probe.json) of the first four labelled pairs for the three G270 events. Input SHA-256 hashes are recorded.
- Final repository/input preservation checks are recorded in [postflight.json](D:/IntrusiveAnomalies/_reviews/081-02-evidence/postflight.json).

This is a **design review**, not an implementation or release gate. No build, cook, capture, mailbox brief, checkout change or plugin/CaptureBench repository edit was made. Review outputs are under `_reviews`.

## 2. Findings

### F1 — P1: Missing onset evidence can become ABSENT; first-K does not solve gradual change

**Location:** [plan §3.3, lines 282–309](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:282), [§2.2, lines 199–204](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:199).

The proposed rule makes an event `UNMEASURED` only when **none** of its first K frames was measured. Suppose the real transition occurred at the first labelled frame, whose pair is missing, late, identity-refused or control-refused. Frames 2–4 show the already changed, steady picture and measure zero. The input `[unmeasured, 0, 0, 0]` becomes **ABSENT**, and the later true-positive frames lose `observable`. The crucial transition was never observed.

The gradual-arrival mitigation is also mathematically insufficient. Taking the maximum of adjacent-pair fractions cannot recover change spread below the per-pixel threshold. With all target pixels progressing through byte values `0, 3, 6, 9, 12`, a pixel threshold of 8 gives four adjacent fractions of zero, but a fraction of 1.0 against the pre-onset image. These are illustrative counterexample values, **not proposed calibration constants**. A separate failure occurs when each step changes a disjoint, sub-floor fraction of the target.

080-06 fixes when arms are eligible. A game-thread held predicate is not proof of an exact delivered-image transition. The mip predicate tests resident count below baseline ([source](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp:892)); render correspondence and temporal coverage still need proof.

**Required change:** Define the observation interval, required pairs, coverage and refusal reasons. One measured quiet pair must not certify an unobserved onset as absent. Use an explicit pending state while collecting evidence, and retain an indeterminate/unmeasured outcome when required evidence is missing. For gradual steady-state classes, either add a qualified comparison with the event's own pre-onset reference or explicitly exclude them from negative gating. K=4 can bound work experimentally; matching the veto's arm budget does not establish that four observations cover the visual transition.

### F2 — P1: The proposed frame identity is not sufficient, and its metadata lookup expires

**Location:** [plan §4.3](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:389) and [§7.1](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:495).

Three concrete source facts defeat “same rendered frame” and “unchanged by construction”:

1. **The previous snapshot normally disappears.** The plan resolves both retained request IDs through `PendingSnapshots`, but the drain removes a snapshot immediately after enqueueing its writer job ([source, line 4153](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:4153)). When N−1 has already drained, the proposed lookup cannot resolve it.
2. **Session-index equality does not join render identity.** The mask callback consumes **all** pending arms into the next eligible mask pass ([source](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp:120)), whereas colour consumes wanted requests FIFO ([source](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalySveCapturer.cpp:45)). Mask counts are cloned to served IDs; only one request receives the pixel payload ([source](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp:512)). Assigning those results an intended session index does not prove that colour, mask and denominator depict that same render.
3. **Family frame number alone does not identify a view or camera.** Key lookup does not consume the entry ([source](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalySveKeyRing.cpp:132)). In a multi-view family, another callback can overwrite retained colour under the same request ID; the next frame's first view can then pass N−1 and extent checks against the other view's pixels. A same-size camera/world change also passes the stated checks.

The CPU fallback needs an explicit ordering design too. The render-thread drain scans in-flight requests backwards and skips unready entries; `PopCompleted` returns the resulting arrival order, and the subsystem processes that batch without sorting ([drain](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalySveCapturer.cpp:285), [consumer](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:3892)). “Previous buffer processed on the CPU” is therefore not necessarily N−1.

**Required change:** Retain an immutable, bounded receipt with the pixels: session/world epoch, capture request and session index, actual render/view identity, camera/cut information, dimensions/rect mapping, format and timing. Join colour, mask and counts using the actual observation receipt. Reorder by identity with a bounded wait, or refuse when the required predecessor is unavailable. Define successful capture versus successful delivery and account for failures after GPU retention. Do not keep old `PendingSnapshots` indefinitely as a workaround.

The detailed missing paths and required outcomes are in §4 below. m51's F1 commit is absent from this base, but **master already has shared held-batch logic** ([source](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:3937)). Neither that interaction nor the existing mask/colour uncertainty can be dismissed simply because F1 is unmerged. m55 need not solve every m51 issue, but uncertified paths must remain unmeasured.

### F3 — P1: Progressive output, provisional admission and “event verdict” are inconsistent

**Location:** [plan §3.2–3.4](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:269).

The text first says the onset verdict is available when the first row is built, then says the first K rows are provisionally admitted until the verdict resolves. The schema has no `pending` value. It also promises event-wide max and median inside progressively written frame entries without specifying whether they mean “so far” or final values.

With K=4, an otherwise m49-positive event lasting four labelled frames keeps **all four** positive frames even if every measured change is zero. A longer absent event still retains its first four positives. Thus the latch cannot produce the promised empty observable set for these cases. Earlier rows and `AccumulateFrameEvents` have already recorded those decisions ([source](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:4132)).

There is a delivery hole as well: the proposed event verdict lives only inside `labels.jsonl`, but that file can be disabled in delivery mode ([line 4150](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:4150)). Run-level totals cannot explain an individual event to that consumer.

**Required change:** Specify a streaming state machine, finalisation on short event/end/drop/teardown, and a durable final event record. Name running aggregates as running, or publish final aggregates separately. State whether evidence belongs to the whole fire or each contiguous anomalous phase; blinking and LOD toggles need that distinction. Specify the actual artifact delivered to the client.

For the first version, retain m49 `observable` unchanged. Any later gating design must explicitly choose how provisional frames affect the final annotation; it cannot promise retrospective removal while forbidding it. Keeping schema 2 and the same key counts does not preserve semantics when the predicate changes. The [operating contract §3](D:/IntrusiveAnomalies/StackOBot/Plugins/AnomalyInjector/docs/OPERATING-CONTRACT.md:17) reserves meaning changes for an owner/client decision.

### F4 — P1: The load-bearing no-effect gate cannot reach the proposed verdict

**Location:** [plan G-1](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:610).

`IAI.Bench.StuckMipNoHold 1` deliberately produces `held:false`, `observable:false` and **no labelled frame**; the implementation explicitly documents that behaviour ([source](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp:511)). The AnomalyState label predicate consults `IsCurrentlyAnomalous`, and target-mask arming skips unlabelled fires ([predicate](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:4845), [arming](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:1156)).

Under its intended no-hold behaviour, this lever never supplies the first K labelled, measured target pairs. It tests failure to engage, not “engaged/labelled with a valid region but no visible effect.” An unrelated residency drop would not make it a controlled null-effect test.

**Required change:** Keep this lever as a no-label/unmeasured control. Add a separately specified bench case with a known unchanged image, a valid target mask, and the same evidence-window eligibility as the positive case. Pair it with a known image-changing case through the same path. Declare any new bench-only label/arm instrumentation and its scope before implementation; the current no-anomaly-source-change assumption cannot silently supply it.

### F5 — P1: Calibration uses verifier outcomes as visibility truth, and G270 does not mean zero change

**Location:** [plan §8](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:576), [G-3](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:612).

The plan says the known answer must be injected, then uses `NO-TRACE`/`CONSISTENT` as the anchors. These are outputs of another temporal pixel statistic. They are useful corroboration, but neither independent visibility labels nor causal ground truth. The closed 079 record explicitly retains wrong picks inside CONSISTENT runs and no certified moving-camera ground-truth session.

The G270 record itself reports adaptive fraction thresholds of **0.7439, 0.0040 and 0.5086**. “No change above that threshold” is not “no pixels changed.” I measured the delivered onset pairs using each **current** target mask and the existing verifier's strict max-RGB byte threshold **>8**:

| Banked event | Pair | Target changed fraction | Outside-target changed fraction | Target mean magnitude /255 |
|---|---:|---:|---:|---:|
| 0, Cube5 | 22 → 23 | 3.2161% | 2.1789% | 0.005918 |
| 1, Cube4 | 63 → 64 | 0.1774% | 0.2687% | 0.003237 |
| 3, Cube4 | 141 → 142 | 11.3463% | 10.8893% | 0.018934 |

Event 3 reaches **12.9160%** target change within its first four labelled pairs. All twelve inspected current-mask pixel counts equal their recorded `target_pixels`. These are **artifact arithmetic checks**, not certification of renderer pairing, visible anomaly effect or cause. The [data and input hashes](D:/IntrusiveAnomalies/_reviews/081-02-evidence/bank_probe.json) make that distinction reproducible.

Thus G-3 must be an experiment with predeclared interpretation, not a requirement to force all three events into ABSENT. Depending on independently selected policy, a high-control event may correctly be ineligible/unmeasured instead.

The noise procedure also needs a failure branch. The maximum per-pixel difference on an anomaly-free moving scene can be 255. A threshold above it has no usable range, and an order-higher threshold is impossible. Predeclaring that algorithm does not make the resulting discriminator sound. Two stored threshold counts and a sum also cannot reconstruct counts for arbitrary new pixel thresholds; those sweeps require the saved images or an adequate histogram.

**Required change:** Separate instrument correctness, numeric-change detection and perceptual visibility validation. Predeclare eligible capture conditions, independent positive/negative examples, calibration versus held-out validation sets, sample coverage, false-negative acceptance criteria, the control rule, and **no separation ⇒ no gating**. Enforce valid threshold ranges and record effective values/provenance. Do not treat verifier outcomes as class labels or adjust thresholds to make G270 pass. Floors without supported calibration remain uncalibrated; changing their source to a compiled default does not supply the missing evidence.

### F6 — P2: The proposed verifier agreement compares different quantities, regions and pairs

**Location:** [plan lines 99–101](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:99) and [§5](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:459).

The verifier's `d` is a **fraction of pixels whose max-channel difference exceeds a byte threshold**, not mean magnitude ([binarisation](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/tools/verify_capture.py:525), [fraction](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/tools/verify_capture.py:561)). On 100 pixels each changing by 10/255, its d is **1.0** and m55's proposed mean is **0.039216**.

Even replacing mean with fraction is insufficient:

- The verifier uses the **union of the two masks**; m55 uses the current mask ([source](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/tools/verify_capture.py:746)). A changed vacated pixel can give 0 under the current mask and 0.5 under a two-pixel union.
- The verifier reports its **selected transition pair**, which may differ from the first labelled pair.
- Its adaptive regional `tau` is not m55's per-pixel `tau_lo`; the pixel threshold must match independently.

**Required change:** Add an independent decoded-image arithmetic comparison at explicit producer pair IDs, with exactly the declared region, pixel conversion and threshold. Compare count/fraction to count/fraction and mean to mean. Report coverage and unavailable comparisons. Preserve the verifier's existing peak selection and outcomes separately. Numeric agreement can validate arithmetic/transport; it does not strengthen a label into a correctness or cause verdict.

### F7 — P2: The GPU pass does not yet measure the delivered byte representation, and retention is not free

**Location:** [plan §7.1](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:513), [cost §4.2](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:369).

`OwnTexture` preserves the source pixel format ([source](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalySceneViewExtension.cpp:139)). The writer later converts it to BGRA8, including 10-bit truncation and half-float rounding/clamping ([conversion](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp:266)). The supported write path also includes JPEG.

A 10-bit red change from 0 to 35 is about 8.724 byte units when normalized directly, but the writer emits a byte change of **8**. At a strict >8 threshold those classifications differ. A GPU float mean is likewise not necessarily the mean of decoded PNG bytes. Merely choosing the later hook does not establish equality.

The installed engine describes `ConvertToExternalTexture` as forcing allocation and increasing memory pressure ([RenderGraphBuilder.h:269](D:/UESource/UnrealEngine/Engine/Source/Runtime/RenderCore/Public/RenderGraphBuilder.h:269)). It can avoid an explicit copy, but its retention/allocation cost is not zero. The 8.29 MB figure assumes a four-byte format and omits other in-flight references; PF_FloatRGBA needs eight bytes per pixel.

**Required change:** Define the canonical delivered RGB conversion and encoding scope. CPU and eventual GPU results must reproduce that conversion, including threshold ties, or report that format as unsupported. Restrict an initial exact comparison to supported PNG/no-resample paths; explicitly handle lossy delivery. Budget retained bytes, mask ownership, work in flight and reset/drain lifetime. For CPU use, `Result.MaskPixels` is moved during mask service and `Frame.RawBytes` during writer enqueue; they are not freely available indefinitely.

### F8 — P2: G-4 makes a known non-necessary depth condition mandatory

**Location:** [plan G-4](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:613), [hide discussion](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:180).

The text correctly preserves G258's asymmetry, but the gate then requires zero drawn pixels on **every** later held frame. Current source explicitly records a correctly hidden image with `drawn == count`, due to depth-pass behaviour, and deliberately leaves `observable` unchanged ([source](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:4023)).

Conversely, `drawn == 0` does not prove the object was visibly present immediately before hiding. If it was fully behind another object, the occlusion-correct silhouette can be empty too, leaving no denominator ([mask shader](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/Shaders/Private/AnomalyVisibleMask.usf:39)). Occluder/camera changes can also coincide with a hide.

**Required change:** Treat drawn-depth zero as supporting absence evidence with valid mask/visibility context, never a necessary condition for a correct hide. Keep the known depth-silencing counterexample as an accepted case. Add already-occluded onset, missing pre-onset evidence, and repeated blink phases. A disappearance claim requires evidence of pre-onset visibility and a valid transition; do not infer that history from current zero counts.

### F9 — P2: Control refusal must preserve the raw measurement and expose its policy

**Location:** [plan §1.4](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:109), [rule/schema](D:/IntrusiveAnomalies/_reviews/081-02-evidence/snapshot/docs/predictions/2026-09-20-m55-change-evidence.md:282).

No subtraction is the right choice. However, an accurately paired image difference remains a numeric measurement when the background changes. Setting `change_measured:false` and publishing unmeasured sentinels conflates invalid pixels with a policy decision that those pixels are unsuitable for an event inference. It also removes data needed to assess the control rule.

A quiet whole-frame complement does not certify a clean target: local occlusion or parallax can be large within the target and tiny across the rest of the image. A true anomaly's spill outside the silhouette can do the reverse. In particular, high control should prevent a negative decision, not cause one through the partial-coverage rule in F1.

**Required change:** Separate pair validity/raw statistics from evidence eligibility/refusal. Publish the applicable reason, measured counts, window coverage and effective control policy. Guard **both** target and control denominators; `Count[t] > 0` does not prevent `W*H - Count[t] == 0`. State what a small/empty control means. Preserve raw values for valid pairs even when inference is refused. A low control value is not a causal certificate.

## 3. Answers to the review questions

| Question | Disposition |
|---|---|
| **Q1 — Is onset enough?** | Enough for the narrow statement “a qualifying temporal change was observed near engagement,” once identity and coverage are proven. **Not enough to close F2** or establish later visibility. For slow/steady classes, an own-pre-onset reference is useful under a stable or independently qualified view/region; screen-coordinate subtraction under camera/object motion is not a safe general replacement. Missing/reference-invalid evidence must remain indeterminate. No observable gating until F1–F5 and independent validation are resolved. |
| **Q2 — Previous-frame identity?** | N−1 is necessary under the proposed strict policy, but insufficient. Use durable observation receipts, actual render/view joins and explicit invalidation. See §4. |
| **Q3 — Control refusal?** | **Agree with no subtraction.** Accept control as a predeclared inference-refusal signal, preserving valid raw measurements. No numeric X is justified by this plan. Broad lighting/shadow effects and anomaly spill can make an outside-target control uninformative; local confounds can evade it. Exemption or refusal must be explicit per supported scope. |
| **Q4 — CPU or GPU first?** | **CPU first**, after the identity/format/ordering design is corrected. Reuse existing readback, establish a decoded-PNG arithmetic reference, and measure cost. This is a correctness/prototyping choice as well as avoiding a cook. GPU can follow as a profiled optimisation with identical numeric semantics. Do not put an unbounded full-frame scalar scan on the game thread. See §5. |
| **Q5 — Hide?** | Onset plus later zero drawn-depth counts is useful supporting evidence only with valid pre-onset visibility and pairing. It does not establish cause, and nonzero drawn-depth counts cannot veto a correct hide. Fully occluded onset provides no visible-disappearance proof; an empty mask is not a zero-change measurement. |
| **Q6 — Honest wording?** | The cause disclaimer is good; “make observable mean measured visible change” and “F2 is closed” overstate the result. State the measured image pairs, region, threshold and coverage. A present transition is not proof of persistent visible state or perceptual salience. Suggested wording appears below. |
| **Q7 — Floors?** | **Not sufficient as written.** Predeclaration alone cannot repair invalid ground truth, non-separating distributions or partial coverage. Use independent cases and held-out validation, preserve an explicit no-separator outcome, and do not derive visibility truth from verifier verdicts. |
| **Q8 — Design merge readiness?** | **CHANGES REQUIRED.** Resolve F1–F9 in the design, with executable acceptance cases specified before implementation. The architectural direction does not warrant RED; implementation as written does not warrant GREEN. |

**Suggested client wording:**

> Change evidence reports pixel differences between identified captured images within the recorded target region. “Present” means the declared change threshold was met during the stated observation window. “Absent” means the required eligible observations were completed without meeting it. Missing or unsuitable observations are reported separately. This does not establish what caused the change, whether the effect remains visible later, or whether a person can perceive it. The existing observable flag retains its documented m49 meaning until a separately approved change.

That wording presupposes the coverage and eligibility fixes. Until a policy is calibrated, describe it as uncalibrated rather than giving an authoritative present/absent event verdict.

## 4. Required previous-frame and lifetime cases

| Path | Why the current checks are insufficient | Required design outcome |
|---|---|---|
| First run frame / insufficient pre-roll | No valid predecessor exists; “always available” is conditional. | No onset-negative decision; seed valid history with explicit coverage. |
| Previous row already drained | Its `PendingSnapshots` entry is removed. | Keep a bounded immutable identity receipt with retained pixels. |
| Out-of-order readbacks or held batches | Arrival order differs from capture order. | Identity-indexed bounded join; never advance history from “last item processed.” |
| Capture skipped before retention | N−1 can detect the gap if indices genuinely skip. | Refuse that pair and reseed; preserve reason and gap. |
| Readback/encode/write failure after retention | Session indices are allocated on **arm**, not successful delivery; a retained GPU frame may never become a delivered image. | Declare the measurement stage and success receipts. A delivered-pair claim must account for both images' delivery; never silently bridge a failed predecessor. |
| Pending mask arms coalesced / lost pixel owner | Intended session index may name another render or have counts without mask bytes. | Actual render/view/mask receipt match, or unmeasured. Test the coalescing path explicitly. |
| Repeated callback / another view | Same family frame and request can recur; retained view may change. | One supported view/capture owner, or view-specific state; refuse unsupported multi-view paths. |
| Same-size camera cut, level/world transition, reset | Extent equality and N−1 can both hold across unrelated pictures. | Epoch/cut invalidation; reset colour, mask holder, receipt and latch together; reject late results from the old epoch. |
| Letterbox/rect or projection changes | Dimensions alone omit rect origin/mapping and camera state. | Carry mapping/projection identity and apply a declared validity policy. |
| First capture after a hitch | Consecutive session indices can span a long time, including an unseen onset/recovery. | Record render/capture times and temporal coverage. A numerically valid difference can still be ineligible as onset evidence. |
| Missing/late change result or final drain | A later result must not mutate an already finalised negative or leak into the next run. | Bounded waits, terminal unmeasured reasons, ownership cleanup and late-result rejection. |

A camera can move while remaining the same camera: camera identity alone does not remove motion confounds. Conversely, retaining only a full pose equality rule would make much of moving-camera use unsupported. The design must state which claim remains valid on that path rather than silently choose one.

## 5. Cost at 1080p and recommended first implementation shape

For **1920×1080 = 2,073,600 pixels**, assuming two BGRA8 images and one R8 mask:

| Quantity | Approximate amount |
|---|---:|
| Minimum full-frame scan inputs | **18.66 MB per captured frame**: 2 × 8.29 MB colour + 2.07 MB mask |
| Input traffic at 30 / 60 captured fps | **0.56 / 1.12 GB/s**, before extra copies, conversion, aggregation and cache effects |
| One retained canonical colour image | **8.29 MB** |
| One retained R8 mask, if needed by the join/reference | **2.07 MB** |
| GPU 8×8 dispatch | **32,400 groups** plus group initialization/reduction/atomic work |
| Proposed GPU summary readback | **4,096 bytes** plus resource/readback overhead |

The paired whole-frame control means a **bbox-only scan is not equivalent**. One whole-frame pass can aggregate all tags and the total; it need not repeat the scan for each target. At 720p the same minimum inputs are 8.29 MB per pair. A retained raw PF_FloatRGBA frame is 16.59 MB at 1080p.

For scale only, moving 18.66 MB at an effective 10–30 GB/s takes about **0.62–1.87 ms**. That is illustrative bandwidth arithmetic, **not a benchmark or promise**; the scalar loop, conversion and contention can cost more. A GPU time cannot be inferred from the 4 KB readback size. The mask is a third texture input, and the plan's groupshared aggregation has costs beyond the two colour loads.

Prefer a bounded ordered CPU measurement stage sharing immutable canonical pixels with the writer, with explicit byte/work limits and honest refusal when its budget cannot be met. Do not double ownership accidentally after `MoveTemp`; do not block indefinitely waiting for an image/mask that cannot arrive. The exact placement must respect when the row is finalised.

Measure with the declared pacing-off A/B/B/A procedure at both resolutions. Report game-thread time, worker time, end-to-end capture/writer throughput, queue/byte high-water marks, drops and tails. An effect hidden inside measurement noise is **unresolved cost**; it does not prove the ≤1 ms requirement if the uncertainty itself exceeds that budget. The GPU version should be considered only after these measurements identify a worthwhile bottleneck.

## 6. Concrete design revision and acceptance list

Before implementation, the revised plan should contain:

1. **One numeric contract:** delivered colour conversion, current-mask versus other region basis, exact thresholds/comparisons, denominators, supported encoding/format/geometry, and raw versus inference-eligible fields.
2. **One identity/lifetime contract:** actual same-render colour/mask/count join; retained predecessor receipt; ordered CPU processing; supported view ownership; epoch resets; bounded waits/bytes; writer-failure and final-drain policy.
3. **One temporal contract:** candidate onset, pending state, required coverage, gradual-change/reference behaviour, phase/event scope, terminal short-event behaviour and final artifact.
4. **A staged semantics decision:** additive measurements first with m49 observable unchanged. An event verdict still needs an explicit calibrated policy; merely postponing observable gating does not validate an uncalibrated verdict.
5. **Independent calibration and comparison:** corrected G-1, G270 as an observation rather than forced truth, a same-pair/same-region decoded-image arithmetic oracle, independent held-out examples and a no-separation stop.
6. **Revised gates** that include missing onset plus later quiet frames; gradual sub-threshold change; short events; same-size cut/reset; out-of-order completion; mask coalescing/payload loss; predecessor write failure; threshold/format ties; full-target/empty-control frames; already-occluded hides; the accepted hide-depth counterexample; and both tick orders. Exercise the relevant failure paths, not just the one-index-stale lever.
7. **A truthful cost/invariance gate:** existing reduce source unchanged is a useful constraint, but does not prove output or timing unchanged. Measure the new pipeline, declare intended new fields, and retain the AA-off image identity checks. Adjust GPU/cook-specific gates if CPU is selected.

### Disposition of the plan's own Q1–Q6

- **Q1 subtraction:** reject subtraction; retain both raw measurements and explicit refusal policy.
- **Q2 GPU/cook:** CPU first, subject to the corrected identity/ordering/conversion design.
- **Q3 observable:** preserve m49 now. Calibrated evidence and an owner/client semantics decision precede later gating.
- **Q4 K=4:** acceptable as an experimental resource/window choice, **not accepted as a proven onset-completeness bound**.
- **Q5 light:** keep deferred to m53's defined region source.
- **Q6 mip default-on:** leave unchanged. G-3 alone, even if it met the proposed expectation, would not settle default-on visibility/readiness.

**Chat review REQUIRED for the next action: rule on these findings and the revised design before authorising implementation.** No implementation brief was issued. The next mandatory checkpoint after any approved telemetry implementation is review of its identity, arithmetic, coverage and cost evidence; changing `observable` remains a separate owner/client decision and review checkpoint.

The design review is complete. Implementation, campaign gates and release readiness remain unrun/unapproved by this report.

VERDICT: NEEDS-DECISION
