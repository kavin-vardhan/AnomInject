<!-- Saved by chat Claude on 2026-11-13 from the owner's paste. Verbatim final message of Claude Code's owner-requested
     read-only review session of 2026-11-11 (not a mailbox brief). Chat's rulings on it: _reviews\095-00-ruling.md -->

REPORT TO CHAT - AnomalyInjector code review and capture frame-skip analysis
Date: 2026-11-11.  Author: Claude Code (review session, owner-requested).
Type: READ-ONLY ANALYSIS.  Chat review REQUIRED before any change proposed in
section 7 is implemented (items 1 and 6 change capture cadence and output
meaning).  Chat review NOT REQUIRED for the analysis itself.

0. AUTHORIZATION AND STATE
- Owner request: review the plugin for issues and improvements, and check
  whether capture performance can be improved. Owner reports frequent frame
  drops when capturing in Concorde; the ML team requires no frame skips.
- Nothing was built, run, captured, measured or modified. No repo write.
- Reviewed ref: fix/followup-capture @ 7754fd2 (descendant of
  release/2026-10-05 @ d7eee97). The checked-out worktree is m51 @ 53bf725,
  about 27,000 lines behind, and was NOT the review target.
- Source was read from git objects exported to a temp folder. The m51
  worktree and index are untouched; index SHA-256 still 526aba7f...9ea35e.
  One plain "git status" was run before switching to object-only reads.
- Method: main session read the frame-grab path (FrameCapturer, SveCapturer,
  SceneViewExtension, AsyncWriter, RunLog, pacing and tick state machine).
  Six read-only sub-reviewers covered the capture subsystem (two halves),
  mask/census/stencil, injector core, anomaly implementations, and control
  server plus writers. Claims marked [V] were re-checked by the main session
  against the code and the UE 5.1.1 source. Claims marked [R] are
  reviewer-reported and not re-checked.
- All line numbers refer to fix/followup-capture @ 7754fd2.
  SUB = Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp.
- UNKNOWN: which build Concorde runs (release or follow-up) and its settings
  (delivery mode, run log, output height, census, dashboard attached).

1. HEADLINE
Frame skips have three structural causes present in the shipped release:
(C1) the capture never writes the settle frames around each anomaly,
(C2) a mask-wait defect whose fix exists only on unmerged branch m51,
(C3) frame-writing cost is unbounded, so a heavy host falls below target rate.
Two more exist only on the follow-up branch (C4, C5).

2. FRAME-SKIP CAUSES

C1. Settle frames are rendered but never captured. [V] Release and follow-up.
- SUB:1160-1163 and SUB:1200-1203 decrement the settle counter without
  calling CaptureCurrentFrame. Entered at SUB:5601-5602 and SUB:5619-5620.
- Defaults AnomalyCaptureSubsystem.h:471-474: settle 2, pre 4, positive 8,
  post 4. Release header has the same values. PostGap goes straight to
  BeginFire (SUB:1224), so steady state is 16 engine ticks per 12 written
  frames: a 2-frame gap at every onset and every offset.
- The dashboard cannot change this: capture_start only calls StartRun
  (AnomalyControlServerSubsystem.cpp:668-721). No ini key; console only.
- Frame files are numbered contiguously, so the gap is not visible in the
  output. The gaps coincide exactly with label boundaries.
- Fixed timestep is always forced on (SUB:4224-4228) and the fps stamp is
  computed from ticks, not frames (SUB:8915-8922), so the assembled video is
  about 25 percent shorter than real time.
- run_summary.ticks_per_captured_frame is GameTicks / TotalFrames
  (AnomalyLabelWriter.cpp:1303-1304) and should read about 1.33. The Bates
  reading 1.3556 recorded as unexplained fits this; not proven for Bates.

C2. Mask-wait defect; fix unmerged. [V] Release and follow-up.
- SUB:5667 sets Snap.bTargetMask from the REQUESTED flag. SUB:3793 makes the
  mask EFFECTIVE only when mask measure and async are on and output height
  is 0. SUB:4983-4994 holds each frame up to 4 ticks (SUB:584) on a
  subsystem-wide counter waiting for an outcome.
- Every outcome producer needs the effective flag or a mask result
  (SUB:1356, 1374-1390, 1879, 1968, 1975, 5696, 7761). When the mask is
  requested but not effective, no outcome can arrive, each frame waits 4
  ticks, the held list grows with raw pixels, and the remainder is dropped
  at run end ("did not resolve by run end", SUB:5339-5344).
- Release has the same logic at lines 4737-4746 and 5392.
- Trigger: output height non-zero, or mask measure off. The dashboard Setup
  panel has an output-height dropdown (SetupPanel.tsx:178, sent at :325).
- Fix is 6713f51 "m51 F1 - wait only for a mask outcome that can exist"
  (2026-09-06). "git branch --contains 6713f51" lists only m51.
- Existing measurement, from the plugin status block (session 078): 33 of 90
  frames written with 57 dropped before, 90 of 90 after. That reading used
  TargetMask 1 with Mask 0 at native resolution. F1 was never merged, M-F1b
  (backbuffer path) is unrun, and the m51 campaign verdict is NEEDS-DECISION.

C3. Unbounded frame-writing cost. [V] structure; impact not measured.
- AnomalyAsyncWriter.cpp:22-32 issues one Async(ThreadPool) job per frame on
  the engine shared pool. No queue limit, no back-pressure, GetPending has
  no caller.
- Each job converts to a new BGRA array (AnomalyLabelWriter.cpp:954-956),
  PNG-encodes at zlib level 3 (engine PngImageWrapper.cpp:197-221, quality
  argument 0 at AnomalyPreviewCapture.cpp:68-69), then opens, appends and
  closes labels.jsonl (AnomalyLabelWriter.cpp:973-978).
- The pacer sleeps the game thread to 1/fps (SUB:8868-8899) and never
  catches up (SUB:8897). Game time is pinned at 1/30 s per tick, so on a
  host slower than target, real-time-driven content jumps between frames.
- Last recorded Concorde figure found: about 5.2 fps effective, 23 s for a
  4 s video, at 6.4 MP (journal 059 section 5, 2026-08-24, before the m34
  GPU mask reduce). No later Concorde throughput reading was found.

C4. View-target or viewport-size change. [V] path; not timed. Follow-up only.
- Owner identity includes GetViewTarget() and viewport size (SUB:4681-4686,
  AnomalyCaptureOwnership.h:114-119).
- On change mid-run (SUB:4732-4736): InvalidateOwnerRequests drops every
  in-flight request (AnomalySveCapturer.cpp:59-75), then ResetEpoch calls
  CloseAndPersist, a 1 ms sleep loop on the game thread
  (AnomalyChangeStage.cpp:622-666) bounded by a 5.0 s deadline (:605, :446).
- Not in the release tree (no InvalidateOwnerRequests there).

C5. Arm must be rendered in its own engine frame. [V] Follow-up only.
- AnomalySveCapturer.cpp:288-299 and AnomalyCaptureOwnership.h:141-144:
  a request whose arm frame differs from the current frame is dropped with
  no picture and no row. Counted in capture_unpaired_dropped.

3. GAME-THREAD STALL SOURCES DURING CAPTURE
H1. Census. [V] AnomalyCensus.cpp:389-397 restarts a cycle in the same tick
    the last one ends: no minimum period. Batches of up to 27 actors, 2 in
    flight (:492, AnomalyCensus.h:88). TagActor (:783) and RestoreActor
    (:549) each flip bRenderCustomDepth, and the engine marks render state
    dirty on that (PrimitiveComponent.cpp:4075-4082). SetCustomDepthStencil
    Value is a render command only (:4084-4097). [R] two full-world walks
    per cycle, including the hygiene check (SUB:1996-2048).
H2. Dashboard snapshots. [V] PushSnapshots has no capture gate
    (AnomalyControlServerSubsystem.cpp:823-864); PushFrames has one (:877).
    Default interval 0.2 s (header :74). [R] each snapshot walks all actors
    with up to 9 line traces per component (AnomalyViewport.cpp:1613-1656).
H3. Run log. [V] AnomalyRunLog.cpp:98 flushes after every line on the
    logging thread; the engine flush is FlushFileBuffers
    (WindowsPlatformFile.cpp:755-759). Per-frame Log lines exist on the game
    thread (SUB:5663) and render thread (AnomalySveCapturer.cpp:438).
    [R] run log is on whenever delivery mode is off (SUB:4044-4045).
H4. camera_clipping. [V] Anomaly_CameraClipping.cpp:24 executes
    r.SetNearClipPlane; the engine handler calls FlushRenderingCommands
    (UnrealEngine.cpp:16958-16968). [R] full-world component walk every
    frame while active (AnomalyViewport.cpp:1260-1274).
H5. stuck_low_mip hold route. [V] Anomaly_StuckLowMip.cpp:1388 calls
    ScanHoldForNewUsers every tick; it iterates every primitive and decal
    component in the process (:1808-1815). Synchronous full streaming update
    at revert (:1321).
H6. Mask pixels. [R] two to three full-resolution passes on the game thread
    per labelled frame (SUB:1892-1906, 1941-1945, 1527-1540).
H7. Readbacks. [V] a new FRHIGPUTextureReadback per frame
    (AnomalySceneViewExtension.cpp:268); each allocates its own staging
    texture on first copy (RHIGPUReadback.cpp:154-164). Full-frame
    allocation and row copy per frame on the render thread
    (AnomalySveCapturer.cpp:685-692).

4. DROPS THAT ARE NOT REPORTED
D1. [V] Writer encode or write failure: one end-of-run warning only
    (SUB:5349-5353); not in run_summary.
D2. [V] GetMasksDropped has no caller; the row still names the mask file.
D3. [V] Run end waits at most 5 s for the writer (SUB:5346,
    AnomalyAsyncWriter.cpp:140-153), 2 s at teardown (SUB:1021), then
    finalises counts with jobs still pending.
D4. [V] Frame cap counts armed frames (SUB:1111, :5728): every drop shortens
    the capture and leaves an index gap.
D5. [V] labels.jsonl append result is ignored (AnomalyLabelWriter.cpp:
    973-978). [R] the append opens the file exclusively, so a reader holding
    it loses that row silently.
D6. [R] pending_at_stop (SUB:9942) is read after the drain emptied the map
    (SUB:5339-5344), so it always reads 0.
D7. [R] CAP-PAIR-DROP (SUB:4958-4966) and other-generation cancels
    (SUB:5688-5690) are log or diagnostic only.

5. OTHER FINDINGS
O1. [V] Control server listens on all interfaces (engine WebSocketServer.cpp
    :131-132). Plugin logs ws://127.0.0.1 (:174), rejects non-loopback peers
    only after connect and never closes them (:294-298), and logs the token
    in clear (:175, :213). [R] no message size cap before authentication.
O2. [V] Debug hotkeys 1-9, J, K are bound by default and polled every tick
    (AnomalyAutoInjectorSubsystem.cpp:110-120, 1017-1039), including during
    capture: they toggle pool membership, run state and seed.
O3. [V] Census stops for the rest of a run after 47 tagged events. It will
    not arm when free values are 8 or fewer (AnomalyCensus.cpp:735,
    AnomalyCensus.h:90); 55 values exist (200..254); EventClaimed is reset
    only at run boundaries (AnomalyStencilTag.cpp:121,
    AnomalyMaskMeasure.cpp:94).
O4. [V] AnomalyHiddenClass::Show calls SetActorHiddenInGame(false) for an
    actor it never hid (AnomalyHiddenClass.cpp:149-154). [R] Blinking revert
    calls Show for every target regardless of phase.
O5. [V] time_dilation revert writes the saved baseline unconditionally
    (Anomaly_TimeDilation.cpp:51). [R] camera_clipping does the same.
O6. [R] missing_texture and corrupted_texture hold the original material
    only as a weak pointer; a host dynamic instance can be collected.
O7. [R] Targets are re-resolved by object name (AnomalyTargeting.cpp:17-48);
    same-named actors in other loaded sublevels collide.
O8. [R] IsRenderableComponent tests only IsVisible (AnomalyViewport.cpp:
    791-820): actors hidden with SetActorHiddenInGame or owner-no-see are
    admitted to selection.
O9. [R] lod_corruption has no single-LOD guard; hierarchical instanced
    meshes ignore forced LOD. Both are labelled as applied.
O10. [R] labels.jsonl rows are appended in job-completion order.
O11. [R] Census release leaks a tag and re-issues its value when the
    still-tagged check fails (AnomalyCensus.cpp:530-566).

6. NOT VERIFIED
- No runtime measurement of any cost. Every magnitude is from reading.
- Concorde build, settings, resolution today, and how often its view target
  changes.
- Whether removing the settle skip is label-safe. Every gate on record ran
  with settle 2.
- Host tools (encode watcher, ffmpeg step) were not in scope.
- Items marked [R].

7. PROPOSED CHANGES, IN ORDER (none started)
1. Capture every tick: settle 0, or capture settle frames and flag them;
   stamp fps from captured frames. Needs the edge gate at settle 0 in both
   tick orders. CHAT REVIEW REQUIRED.
2. Carry 6713f51 to the release line.
3. Writer: bounded queue that blocks at arm time, dedicated low-priority
   threads, zlib level 1 or recompress in the host watcher, keep
   labels.jsonl open, ordered rows.
4. Census: minimum cycle period, cached prefilter, keep the custom-depth
   flag and rotate stencil values only, hygiene check at run end, return
   event tags when releasable.
5. Pause snapshots during capture, buffer the run log, pool readbacks, move
   mask loops off the game thread.
6. One frames_dropped total with missing indices in run_summary; fail loud
   on write errors. CHAT REVIEW REQUIRED (output schema).
7. Follow-up branch: make the owner-change reset non-blocking and take view
   target out of the generation.
8. Bind the control server to loopback; gate hotkeys during capture.

8. HOST READBACK TO DISCRIMINATE (numbers only, nothing leaves the host)
run_summary.json: ticks_per_captured_frame, capture_game_ticks, total_frames
against requested, speed_ratio, sustained_wall_fps, stamped_fps,
capture_unpaired_dropped, capture_unpaired_frames,
target_mask_frames_unavailable, owner_updates, epoch_resets.
Log line counts: "TARGET MASK NEVER RESOLVED", "did not resolve by run end",
"writer flush timed out", "failed to encode/write", "OWNERSHIP UPDATE",
"CLOSURE-TIMEOUT".
Console bisect levers, diagnosis only, not qualified for delivery:
IAI.Capture.Config 0 4 8 4 0; IAI.Capture.Census 0; IAI.Capture.RunLog 0;
IAI.Capture.ChangeEvidence 0; output height left at native; dashboard
closed during capture.

9. DECISIONS REQUESTED FROM CHAT
a. Is the settle skip still required for label correctness, given keyed
   frame pairing, end-of-tick label sampling and transition flags? If not:
   settle 0, or capture-and-flag, and which gate set qualifies it.
b. May 6713f51 be carried to the release line ahead of the m51 campaign,
   and with which gates.
c. Is blocking back-pressure at arm time acceptable under the fixed
   timestep (costs wall time, never a frame).
d. Owner to state which build and settings Concorde runs.
