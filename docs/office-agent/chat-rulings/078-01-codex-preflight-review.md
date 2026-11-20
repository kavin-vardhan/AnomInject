# 078-01 predictions review — Codex — 2026-09-06

**Verdict: CHANGES REQUIRED. No implementation or leg is released.**

Reviewed draft: `docs/predictions/2026-09-06-m51-release-hardening.md`, SHA256 `4A676AD2B08F859C3A5F147CD75C42FA29DAA69DF83086F6217C4F656BC2BDED`, and the accompanying 078 preflight journal. Source checks below use `ac13700`. This is static review of the plan and source, not a new capture measurement. The final Code report and documentation commit will be linked in the reviewer request once the runner finishes.

The source audit, conflict map, amended-schema awareness and explicit unknown states are useful. The plan is not yet a safe implementation contract. These corrections are required before execution; existing owner/client rulings are not reopened by listing them as forks.

## 1. Restore the actual Phase B observable predicate

Predictions §2, rows H/I, allow a hide positive without explicitly requiring the condition to remain held and the silhouette count to meet the configured threshold. Row H even permits zero target pixels. Source `AnomalyCaptureSubsystem.cpp:3866–3890` computes `bLabelled && bHeld && Px >= ObservableMinPixels`, after the unmeasured check. DrawnCount is a separate diagnostic.

Use that same predicate for the measured cases, independent of DrawnCount. Include below-threshold, zero-count, condition-lost and unlabelled cases. DrawnCount > 0 must neither establish nor veto observable. A zero-target-pixel frame with the ordinary positive threshold is negative even when DrawnCount is zero. Do not substitute a hardcoded `> 0` for the configurable threshold.

Also specify the row-level `anomaly_present` and `visible_positive` aggregates, globals/sync dispositions, and mixed-fire shader-pending example. A per-fire unknown must not make a co-active valid fire unknown or silently change the meaning of the frame's shared mask file.

## 2. Reject fork F-b: zero-observable events remain in the file

Predictions §3 says these events remain, but §3.1 and commit 6 propose deleting them when all measured counts are zero. That contradicts the operating contract and the amended R1, even when the veto reads current per-frame evidence.

Remove the destructive event veto. A fully measured zero-observable event remains with its injection window, empty affected window and observations. Zero measured retains the legacy fallback; partially measured uses only its observable subset.

Keep `vetoed_events` as a compatibility field if required, documenting that the removed veto no longer increments it. Zero events deleted is the correct count after removing event deletion; G96 does not require preserving a bug to keep its counter nonzero. Do not quietly repurpose the field to count retained events. Declare expected differences on the specific affected P-C7 legs.

## 3. R8 needs a real stale-arm path; ForceMiss is an additional test

`AnomalySveKeyRing.cpp:16–27` says ForceMiss corrupts a key after a family has been published; it does not prevent that tick's render submission. `AnomalySceneViewExtension.cpp` performs lookup in the render callback. A lookup failure and an unpublished stale arm reach different branches. Testing the former does not prove cancellation in `ConsumeWantedForPublish` or cleanup of unpublished mask arms.

Keep a ForceMiss recovery leg, with a distinct lookup-failure disposition/counter; do not call a rendered-but-unreadable frame an unrendered tick. Add a bench device that lets game ticks/arms advance while deliberately withholding the relevant render submissions for >32 opportunities, then resumes normal submissions. Observe the actual branch reached, terminal cancellations, pending capacity and subsequent committed frames. If that device requires a fixture/engine change, state the concrete proposal and its scope before execution.

ForceMiss's argument is a periodicity, not a burst count: 1 corrupts every key, N>1 every Nth key. Specify an independent tick-bounded on/off interval when using it.

The applied-pin arm is unavailable on the two current fixtures. That limitation remains OPEN; it is neither silently waived nor covered by reversing subsystem tick order.

## 4. M-F9 names the wrong failure device

`AnomalyInjectorSubsystem.cpp:861–883` defines HideOmitDepthPassSilencing as leaving the target's depth prepass enabled while silencing its main pass. It does not disable the custom-depth measurement pass. Phase B deliberately measures its nonzero DrawnCount. Expecting all its rows to become `target_pixels:-1` would turn a correct Phase B result into a test failure.

Keep that lever for its existing identity/DrawnCount diagnostic gates. For F9, deliberately invalidate the actual mask pass or its recorded validity metadata and read back which invalidity occurred. Define separate valid-zero and invalid-pass controls; do not call absence of main-pass pixels a missing custom-depth pass.

## 5. F2 needs real readback delay and an independent geometry reference

`AnomalyCaptureSubsystem.cpp:5927–5930` explicitly describes ViewLag as a legacy/sync projection-view lag. It is not a GPU/readback-delay knob. Moving the camera sample is a confound, not a latency manipulation.

Add an actual completion/drain-delay lever that preserves the captured render state. Keep the useful latency-invariance check, but it is insufficient alone: a permanently stale or constant box would also be invariant. Use an independent same-frame projection reference or a calibrated unoccluded fixture with predeclared tolerance, as the brief requires. Freeze scene/actor/camera state for comparisons; identical seeds alone do not establish identical trajectories under changed timing. Retain the destroyed-after-render-before-readback case and assert both row and event membership.

## 6. Correct writer accounting and declare an actual sustained-load gate

PNG output is not bounded by raw byte count: incompressible data has encoding/container overhead. Account for the source buffer, decoded/resampled buffers, mask buffers, encoded outputs and codec working memory under an explicit bound or separate reserve. `PF_A2B10G10R10` is 4 bytes per pixel; half-float RGBA is 8. Declare what happens when a single job exceeds the budget, worker/byte/readback setting validation, and the invalid/zero capture-rate case for the wait deadline.

Do not promise "3x the byte budget in flight" under a byte-bounded writer. Cumulative work exceeding the budget proves neither saturation nor steady-state throughput. Predeclare exact rate, duration, warmup/measurement interval, event count, memory and queue high-water readings, wait-time bound and the numeric speed-ratio acceptance band. Both orders on both fixtures need sustained natural 1080p native PNG/mask/census runs, plus a deliberate saturation run with counted failure. A roughly 100-frame run is not justified as a steady-state test by the supplied arithmetic.

## 7. Completeness and failure outcomes must be internally consistent

Fork F-a was already resolved YES in brief 078-01: a promised mask artifact failing makes the session incomplete even when the valid RGB/label commit survives. Remove the redundant fork.

Predictions §6.7 incorrectly expects M-F16 to pass as a complete session while §6.1 rejects the unsupported image. Missing requested RGB is not merely unavailable measurement. The deliberate unsupported-format session must fail integrity when it creates a requested-frame hole.

Replace the four-counter "if and only if" definition with an explicit reconciliation of requested indices, terminal outcomes, committed frame receipts, required artifact writes and final annotation/summary publication. Either classify every other failure under those counters or add an explicit failure status; a lost callback or failed final JSON write must not produce `capture_complete:true` with all four counters zero. Specify how incomplete publication is reported when the summary itself cannot be written.

## 8. Test each writer failure boundary and late completion explicitly

One write-failure row does not cover F7/F8. Separate mask encode/write failure, RGB encode/write failure, JSONL append failure, saturated enqueue and a delayed worker that crosses FinishRun/start-next-session. Require the mask-failure row AND event observation downgrade; RGB survives only in that case. Check no successful commit without its required row/image, no annotation index without a committed frame, and no writes or counter mutation after the session-completion notification. Predeclare each auditor exit and the expected counts.

## 9. Strengthen the lifecycle design before porting it

BeginRenderViewFamily is the game-thread publication boundary here, not the render callback; correct the ownership table against the engine call site. Session Generation does not reject a late completion for an already-cancelled request in the SAME session. Specify per-request terminal-state checking in addition to generation, including duplicate completion and capacity release exactly once.

The journal promises tag release after associated work terminates, but the imported function releases on fire end. Specify the actual lease references and renderer/proxy ordering that bridge that gap. A generation check alone also does not isolate two tag leases in the same session. M-F3 needs to exercise delayed work plus reuse, not only sequential counters. Merely reducing the pool to 8 does not necessarily exhaust sequential correctly released leases; specify achieved occupancy or a retirement-bypass falsifier and confirm >=3 counted events.

Replacing Published.Reset with FIFO eviction plus a counter is still insufficient if evicted batches retain live snapshot capacity. Every affected request/consumer needs an explicit cancellation outcome and resource retirement. The same-family colour/mask join must preserve both generation and request identity, including late completion after cancellation.

## 10. Complete the executable gate matrix and provenance

The table explicitly lists only StackOBot for most new gates despite the both-fixtures rule. It also lacks concrete commands and numeric relations for most regressions, omits an explicit M-F8 late-worker leg, and leaves global/sync, orthographic/near-plane and per-fire shader attribution coverage incomplete. Inventory exact existing script paths/commands; specify the new scripts/levers, fixture configuration, independent oracle, known-answer checker failures, output bank path and stop criteria for every required case. Do not substitute static rejection for an unrun F4 runtime gate or accept F-c as a waiver. Unsupported multi-view/stereo configurations need explicit disposition rather than unexplained capture holes; assess interference when another capture context is also active.

P-C7's section currently says "declared per leg" but supplies a general partial key list and future candidates. Produce the actual per-leg allowlists, including row/event/summary additions and only the justified membership changes. Preserve absolute-counter offset and tag-bijection rules. Do not rebuild an A-side by "reverting commit 1" during a frozen-binary campaign: identify a preserved baseline or an in-binary fault lever, with exact provenance and its limitations.

The journal gives full hashes for staged bench artifacts but only prefixes for archived A-side artifacts and no full Lyra binary/module manifest. Finish the full-path/full-hash anchors and verify the relevant binaries stayed unchanged. A plugin checkout SHA alone does not establish which binary an editor loads.

## 11. Correct the provenance claims, without changing the integration strategy

An endpoint diff `master..be16977` displays Phase B additions as deletions because that old branch lacks them. This is not a prediction of a normal three-way Git merge: merge uses the common ancestor, retaining nonconflicting changes from both sides. Correct the draft/journal claim that merging necessarily deletes Phase B. Wholesale file replacement or applying the endpoint diff would be dangerous; the authorized deliberate port onto master remains the chosen strategy.

The source branch's empty shader diff supports a no-shader-change plan, not proof that the entire future milestone needs no cook. New fixture assets or a later necessary shader/parameter change could alter that conclusion. Preserve the no-cook scope fence and stop for an explicit release if an actual implementation or fixture dependency crosses it.

Supplemental source checks: the local UE 5.1 `Engine/Source/Runtime/Engine/Public/SceneViewExtension.h` documents the game-thread callback at lines 136–141; `Engine/Source/Runtime/Core/Private/Misc/PixelFormat.cpp:44,52` records FloatRGBA as 8 bytes per pixel and A2B10G10R10 as 4.

## Review handoff

Outgoing chat Claude should review the exact draft together with these objections, especially the R1 event-retention ruling and the distinction between stale arms and render-key failures. The next Code brief will be a bounded plan revision against consolidated written review; it will not silently release implementation or any leg. Scope and release evidence remain owned by the operating contract.
