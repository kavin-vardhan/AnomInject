# External release-readiness review of AnomalyInjector — 2026-09-06

**Provenance:** an independent static review of the plugin (4 modules, 93 files, ~24,000 lines) received by the owner on 2026-09-06. Static analysis plus small in-memory control-flow models; no Unreal runtime; no files changed. Reviewed tree = the working tree as of 2026-09-06 morning (branch `m49-phase-b`, tip `5e45339`, Phase B uncommitted hunks present) — line references below are against that tree.

**Chat spot-check (2026-09-06, read-only, cited lines only):** findings 1, 2, 3, 4, 5, 7, 8, 12 were read at the cited lines and every one is as described. The remaining findings were NOT spot-checked by chat; m51 step 0 verifies all sixteen.

**Response:** milestone **m51 (release hardening)** — see brief 078-01 and `docs/predictions/…-m51-*`.

---

**AnomalyInjector is not release-ready yet.** I found several paths that can lose frames, misalign labels, or publish annotations without reliable pixel evidence.

I reviewed the four plugin modules, anomaly implementations, shaders, and verification tooling. The source inventory is 93 files and approximately 24,000 lines. This was a static analysis pass with small, in-memory control-flow checks—not an Unreal runtime certification. I changed no files. Your existing uncommitted changes remain intact.

The most important findings follow, ordered roughly by release impact. P1 means a release blocker; P2 means a significant correctness or reliability issue.

1. **[P1] Disabling effective target masks can stall frame processing and discard captures.**
   `StartRun` disables effective target masks when output height is nonzero or mask measurement is disabled. However, each snapshot stores the *requested* `bTargetMask`, and completed frames consequently wait for outcomes that will never exist.
   The timeout holds the entire remaining batch behind the first unresolved frame. After releasing that frame, the next frame starts another wait. Finalization eventually empties the unresolved snapshots.
   Trigger: leave target masks requested and use downscaled output, or disable mask measurement.
   An in-memory model of this exact waiting logic processed only 29 of 120 completed images after 120 drain calls, leaving 91 held. This demonstrates the mechanism, not a measured Unreal capture rate.
   Required correction: distinguish mask requested, mask available, and mask actually armed. An intentionally unavailable mask must resolve immediately without delaying its image.
   References: effective configuration `AnomalyCaptureSubsystem.cpp:2965`, snapshot flag `AnomalyCaptureSubsystem.cpp:4195`, blocking drain `AnomalyCaptureSubsystem.cpp:3801`.

2. **[P1] Moving actors receive bounding boxes calculated from a later pose.**
   The snapshot retains weak actor references. When GPU readback completes, `BuildFrameLabelRecord` projects the actor's current bounds using the previously captured view. Event accumulation also reads current bounds, component identity, and selection provenance.
   Symptoms: boxes lead or trail moving objects; an actor moving offscreen loses an earlier valid annotation; destroying an actor before readback invalidates a label for pixels already captured.
   Capturing `FirePos` does not fix this: the bounding-box calculation still dereferences the live actor.
   Required correction: capture geometry, identity, and provenance by value at the frame boundary. Completed-frame processing should not query live actors.
   References: late projection `AnomalyLabelWriter.cpp:106`, late event metadata `AnomalyCaptureSubsystem.cpp:5466`.

3. **[P1] The event stencil allocator inevitably exhausts during sufficiently long runs.**
   Every new measurable event adds a value to `EventClaimed`. Those claims are cleared at run end, with no corresponding release when an event finishes.
   The pool contains only 55 values: 200–254. With no host reservations, event 56 cannot obtain a tag—even if every previous event has ended. Census reservations can make availability worse, and the census stops allocating batches once only its eight-value event headroom remains.
   Symptoms: masks and observability work initially, then deteriorate into unmeasured results; census selection stops refreshing.
   Required correction: separate historical event records from live tag ownership. Release tags after the event and all associated rendering/readback work finish.
   References: event allocation and cleanup `AnomalyMaskMeasure.cpp:56`, census headroom `AnomalyCensus.cpp:714`.

4. **[P1] Images and masks are not bound to one specific rendered view.**
   The color extension consumes one pending request per eligible view family. The mask extension consumes all pending mask requests in the next eligible render callback.
   Neither extension restricts itself to its owning world and viewport. The color key is also reusable across views within a family.
   Triggers: multiple PIE worlds, another eligible editor viewport, split-screen/stereo, or multiple game ticks before a render.
   Consequences: the wrong view can consume a request; several mask requests can receive one later scene state while their images follow a different queue.
   Required correction: bind image, mask, and label state to the same identified world, viewport, view, and render submission. Epic exposes world identification through `FSceneViewExtensionContext`.
   References: color eligibility and request consumption `AnomalySceneViewExtension.cpp:43`, mask request consumption `AnomalyMaskSceneViewExtension.cpp:103`.

5. **[P1] Late mask results can cross session boundaries.**
   Mask-extension `Reset()` clears pending arms and results but leaves `InFlight` intact. The final drain explicitly permits unresolved GPU work to remain. Meanwhile, the next run resets target-mask and census request serials to zero.
   Trigger: stop with unresolved GPU readbacks, then start another capture in the same world.
   Consequence: an old result can arrive under a request ID reused by the new session.
   Required correction: include a session generation in every request and reject obsolete completions. GPU resource retirement must also have an explicit lifecycle.
   References: incomplete reset `AnomalyMaskSceneViewExtension.cpp:80`, request serial reset `AnomalyCaptureSubsystem.cpp:2994`.

6. **[P1] Event-level measurements can erase valid per-frame positives.**
   The older event measurement samples up to four logically visible ticks and can continue sampling historical events after their anomaly has reverted. Finalization removes an entire event when that measurement reports zero.
   That veto does not check whether the newer per-frame measurements already recorded observable positive frames.
   Trigger: an actor is visible during the anomaly but becomes occluded or moves offscreen during the separate event-measurement ticks.
   Consequence: valid image/mask evidence can survive in the frame outputs while the corresponding event disappears from `annotation.json`.
   Required correction: derive event summaries from the committed per-frame evidence. An observation from another time must not overrule that evidence.
   References: separate measurement timing `AnomalyMaskMeasure.cpp:255`, whole-event deletion `AnomalyCaptureSubsystem.cpp:4948`.

7. **[P1] Successful image writes are treated as successful labeled-frame writes.**
   Both synchronous and asynchronous writers ignore the return value from appending `labels.jsonl`. Event accumulation happens before asynchronous image-writing success is known.
   Mask-write failures are counted internally, but those failure counters are not consumed by the subsystem to repair labels or the summary.
   Triggers: disk full, permissions, file locks, encoding failures.
   Consequences: missing label rows, annotations referencing unwritten frames, and `mask_file` pointing to a nonexistent PNG. Dropped images also leave holes in session indices while `video.total_frames` reports the successful-write count.
   Required correction: publish per-frame completion receipts and derive final artifacts from successful commits. Define an explicit failed/incomplete session state.
   References: ignored label-write result `AnomalyLabelWriter.cpp:533`, accumulation before writing `AnomalyCaptureSubsystem.cpp:3998`, mask-write failure `AnomalyAsyncWriter.cpp:40`.

8. **[P1] Writer timeout does not mean the session has finished writing.**
   `FlushPending(5.0)` returns after its timeout while worker jobs continue. Finalization then publishes annotations and the summary. A subsequent run reuses the same writer and resets its counters.
   Consequences: files can appear after the completion signal; the published counts can be stale; late jobs from the previous run can increment the next run's counters.
   There is also no queue or byte limit on pending encoding jobs.
   Required correction: give each session its own writer state, apply bounded backpressure, and publish completion only after all accepted jobs reach a terminal outcome.
   References: timeout behavior `AnomalyAsyncWriter.cpp:114`, writer reuse `AnomalyCaptureSubsystem.cpp:2914`.

9. **[P1] Per-frame target masks bypass validity checks used by other mask consumers.**
   Event measurement and census processing reject results when the custom-depth pass did not run. `ServiceTargetMask` checks pixel-buffer dimensions but does not apply the equivalent pass-validity or contamination checks before interpreting counts.
   Consequence: absence of valid measurement can become `target_pixels: 0`, `observable: false`, and removal from affected frames.
   Required correction: use one shared validity assessment before any consumer interprets a count as evidence.
   Reference: target-mask result handling `AnomalyCaptureSubsystem.cpp:1168`.

10. **[P1] Global-anomaly transitions still depend on subsystem tick order.**
    `FinalizeArmedLabel` decides which session-global anomalies exist in the snapshot before deferred end-of-world-tick sampling. Deferred sampling only revisits entries already present.
    Furthermore, `ComputeFireActive` returns active unconditionally for `AnomalyState` entries without a target actor—the representation used by session globals.
    Trigger: camera clipping changes its proximity-trigger state after capture's tick.
    Consequence: an entering transition can be omitted, and an already-added global entry cannot reliably have its active state corrected.
    Required correction: capture global membership and active state together at the final sampling boundary.
    References: early membership `AnomalyCaptureSubsystem.cpp:4379`, null-actor active shortcut `AnomalyCaptureSubsystem.cpp:4651`.

11. **[P2] Projection assumes a camera configuration that the renderer need not use.**
    The plugin reconstructs a perspective projection, explicitly disables aspect constraints, and obtains aspect ratio from the entire viewport.
    Affected configurations: orthographic cameras, constrained/letterboxed cameras, custom projections, and split-screen. A static actor can therefore have incorrect boxes even without readback latency.
    The corner-projection helpers also discard behind-camera corners without clipping intersecting box edges, producing incorrect bounds near the camera plane.
    Required correction: use the captured render view's actual projection and rect, with proper clipping.
    Reference: projection construction `AnomalyViewport.cpp:166`.

12. **[P1] Texture restoration can lose the original runtime material to garbage collection.**
    Both texture anomalies store `OriginalMaterial` as `TWeakObjectPtr`. Replacing the component's material can remove the last strong reference to a runtime material instance. A collection during the anomaly can destroy that original.
    Revert then resets to the mesh default, losing the previous material and its parameters. The warning acknowledges this outcome but does not prevent it.
    Required correction: retain originals through a GC-aware strong ownership mechanism until restoration completes. Weak object pointers do not keep objects alive.
    Reference: captured material ownership `Anomalies/Anomaly_MissingTexture.h:24` (and `Anomaly_CorruptedTexture.h`).

13. **[P2] One dropped preview frame can freeze preview permanently.**
    Preview increments `Outstanding` when arming and decrements it only when a completed frame arrives. The underlying capturer has several paths that drop a request without returning a completion.
    After such a drop, `IsBusy()` remains true and prevents further preview arms.
    Trigger: resize-related extent failure, failed readback, or another dropped capture.
    Required correction: every accepted request needs a success, failure, or cancellation completion.
    References: preview accounting `AnomalyPreviewTee.cpp:33`, busy gate `AnomalyCaptureSubsystem.cpp:4331`.

14. **[P2] Synchronous capture has different labeling behavior.**
    Its frame-label writer reads only auto-injector fires, omitting session globals. Its event-state sampling uses `Actor->IsHidden()` rather than `AnomalyHiddenClass::IsLogicallyHidden()`, so the default main-pass hiding mechanism is not recognized correctly.
    Consequences: missing-object/blink annotations can report no manifestation, and global anomalies can disagree between JSONL and event annotations.
    Required correction: both capture backends must consume the same immutable label snapshot.
    References: synchronous label collection `AnomalyLabelWriter.cpp:445`, synchronous active-state sampling `AnomalyCaptureSubsystem.cpp:4264`.

15. **[P2] Capture has no exclusive ownership of anomaly mutations.**
    During capture, WebSocket `inject`, `revert`, `revert_all`, and auto-control commands remain available. Core application methods do not enforce a capture ownership boundary.
    Consequence: another command can replace an anomaly while the auto-injector still records the previous target, or introduce an anomaly absent from capture's live-fire list. The clean slate at capture start does not protect the rest of the run.
    Required correction: route mutations through session ownership or explicitly invalidate the capture when an external mutation occurs.
    Reference: unrestricted control handlers `AnomalyControlServerSubsystem.cpp:398`.

16. **[P2] Unsupported pixel formats silently become successful black images.**
    The pixel decoder handles four formats. Its default returns black, and the encoding path can then successfully write and count that black image.
    Required correction: reject unsupported formats explicitly and define the supported color-space/HDR conversion contract.
    Reference: decoder fallback `AnomalyLabelWriter.cpp:218`.

There are additional release concerns beyond those defects:

- **Anomaly state is not consistently visual evidence.** LOD popping reports its internal phase without verifying a different LOD actually rendered. Camera clipping uses a collision sphere, which can detect geometry behind the camera and miss non-colliding visible geometry. These need clearly stated evidence levels.
- **Component anomalies do not fit the generic actor-box model.** A targeted light can change the scene while providing no mesh bounds for the current event-labeling path.
- **Control responses obscure failures.** `capture_start` sends an acknowledgment regardless of whether `StartRun` succeeds, and passes an empty anomaly-argument array. Unknown commands also receive acknowledgments.
- **Exposure telemetry assumes ordering that readback does not guarantee.** Both capturers drain ready items in reverse order, while exposure analysis expects predecessor samples to exist.
- **Shipping configuration disables capture, shaders, and the control server.** This can be an intentional Development-build product, but it needs an explicit release/support contract.
- **The capture module depends on private Renderer headers and a fork-symbol build probe.** Engine-version and renderer support require a tested matrix; the plugin is not insulated by public APIs alone.

Architecturally, the module separation is useful: injection, capture, control, and shaders are distinct. The owned sub-rectangle readback textures and explicit unmeasured states are also sound foundations.

The main weakness is inside capture: orchestration, measurement policy, event interpretation, persistence, diagnostics, and compatibility handling share one large subsystem. Several layers independently reinterpret the same event.

Architectural changes recommended before release:

| Boundary | Required responsibility |
|---|---|
| Capture session | Own configuration, mutation permissions, session identity, and lifecycle |
| Frame snapshot | Hold immutable actor state, geometry, camera, and event identities |
| Render submission | Bind color and mask to one identified view and submission |
| Measurement assessment | Produce one validity/evidence decision shared by every consumer |
| Frame writer | Bound memory, report terminal outcomes, and commit related artifacts |
| Event aggregation | Derive annotations exclusively from committed frame records |

The release gate should include moving and destroyed targets; constrained cameras; mask-on/off and output-resizing combinations; more than 55 events; rapid stop/start with delayed readbacks; disk and encoding failures; preview resize recovery; forced GC during texture swaps; and multiple views/worlds.

For every successful session, verify that frame files, label rows, masks, event indices, and final counts agree—and that nothing continues writing after completion is published.

**Suggested first fix order: disabled-mask drain, immutable frame snapshots, shared render identity, session isolation, and committed-write accounting.** Those address the most consequential capture and labeling failures before further anomaly tuning.
