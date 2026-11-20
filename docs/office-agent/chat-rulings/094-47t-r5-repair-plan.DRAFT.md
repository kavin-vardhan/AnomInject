# 094-47t r5 proposed repair phase — DRAFT, NOT RELEASED

Prepared by root while r4 remains active, 2026-11-06. This is review preparation, not a second brief, worker or in-flight instruction. Read the complete r4 final report and independently verify native/CLI closure and exit protection before releasing any continuation. Keep the same coherent 4.25/AnomDash workstream and sole ownership.

The owner has delegated technical decisions, requested overnight delivery, removed RAM admission waits and prioritized 4.25 plus dashboard over further 5.1 qualification. The next step should repair measured implementation defects if the remaining time, attempts and measured storage authorization permit it. Do not treat a failed test as a reason to erase its evidence, relax the expected behavior or retry unchanged code.

## Verified starting evidence

- Plugin feature f78a9c47eca51855cbf0e309982126e1ccce2422; native source and packaged executable c8aedf4dcc5b39f63cfb4da6dc773695a920dc99. The two later commits affect smoke tooling/docs only.
- Existing Development package has 34 files, 2369780355 bytes. Main executable SHA256 01f3e4d90bc3342cfd05709eb798a3caa619937ecea2e32f528c435a235e62b7; pak d926673c1e26886f69292162a87bdfcfbfaade283ef83b21e71e48c8dc940f56. Keep this complete package and its source/binary provenance.
- Stage B D3D11 console and distinct C D3D11 console full captures passed independent pixel/label readers. C dashboard D3D11 completed 2352 frames with zero drops and passed independent diagnostic pixel checks, but failed exact receipt equivalence. Both D3D12 full legs remain unrun.
- Root rehashed all frozen files in the three full captures. The dashboard pair has 8324 console and 8325 dashboard files, with zero mismatches. Root c-d3d11-equivalence-independent-diagnostic.json independently reproduces 11420 differences under the original sealed comparator; correcting modulo8 to the engine's modulo7 alone leaves 11359 differences. These failures remain failures under that comparator.
- Random-token lifecycle retry failed occupied-port refusal. A fresh LISTENING announcement occurred while the test's owned exclusive loopback socket held8077. The closed log contains no refusal, so this is not merely delayed log flushing.
- Configured-token lifecycle protocol passed authentication, warmup cancellation, same-game second start, active reconnect and final status. Its early-stop artifacts failed:39 requested,35 written,4 dropped. Mask/pacing restored, but restoration does not erase dropped frames.
- Dashboard branch b8ffaca1d88a2c89d1ed69adaac85501cc2fc0b5 remains clean, with69 tests and TypeScript passed. The original desktop artifact opened a file-URL directory listing. The job-only relative frontendDist correction compiled successfully and preserved the old artifact. New executable11190784bytes SHA c8120946ff5bb4daa713286510f091b75760ae41f0895480adea1a14e481be28, live and archived at J/dashboard-build-r4/AnomalyDashboard.exe. Root independently rehashed both, all15 frontend files and corrected configuration; no owner config embedded. Actual connection check is still active/pending at preparation time.

All source inspections by root use Git objects with GIT_OPTIONAL_LOCKS=0. Root evidence is RUN/root-receipts-094-47t-r4, especially runtime-failure-source-review.json, dashboard-repaired-build-review.json, independent-c-d3d11-dashboard-pixel-diagnostic.json and c-d3d11-equivalence-independent-diagnostic.json. Worker must not write root receipts.

## Repair 1: truthful listener startup

Primary file: Source/AnomalyControlServer/Private/Compat/AnomalyControlTransport425.cpp. Inspect associated transport state/tests and Build.cs only if the implementation requires them. Keep the 5.1 adapter untouched.

Start currently treats a non-null lws_create_context as proof of a listener. Installed headers identify libwebsockets3.0.0. Upstream3.0.0 server.c:74-120 and201-210 allows deferred nonfatal binding; that is a source-backed explanation consistent with the measurement, not proof that the installed static library is unmodified. Merely switching to an explicit vhost or checking the requested vhost port is insufficient unless it proves a real bound/listening socket.

Require evidence of the actual owned IPv4 loopback listener at the requested port before reporting success. If creation leaves a deferred/unbound context, destroy that context and return failure; it must not become a delayed listener after the blocker disappears. Avoid a probe-only bind followed by an unchecked race. Preserve game-thread dispatch, raw framing/reassembly, bounded queues, single teardown, loopback-only behavior and Shipping exclusion. Do not replace the transport or upgrade/install a library.

Verify held exclusive socket -> refusal/no LISTENING/no latent takeover; release socket -> explicit restart succeeds; old random token rejected and fresh token accepted; normal configured token still works. Preserve the failed r4 result and exact attempt counts.

## Repair 2: early stop drains accepted capture work

Primary files to inspect: Source/AnomalyCapture/Private/Compat/AnomalyCaptureSession425.{h,cpp}, AnomalyCaptureBackend425.{h,cpp}, Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp and the control server stop-response path. Add only the smallest state/ownership changes needed.

Current Session Stop destroys the backend before polling pending readbacks. Backend Stop counts PendingAA.Num as dropped and clears it. The later wait covers writer futures only. This explains the39/35/4 measurement and must not be repaired by hiding those drops or rewriting failed receipts.

Stop new requests, retain ownership of already accepted requests, drain their image/mask/label/write work, and finalize an honest PARTIAL only when every accepted request is terminal with no unexplained drops. Preserve the original planned2352 count and exact committed indices. Bound draining and preserve explicit failure on timeout, write/readback failure or teardown. No new captures during drain; no orphaned warmup, delayed start, stale completion or cross-session reply. Warmup cancellation remains zero-frame cancellation. Normal full completion stays2352/2352/zero drops/PASS.

If drain must continue across ticks, the public running/status/stop-response behavior must remain truthful. Do not send a terminal capture_stopped while pending work can still change final counts. Preserve the existing dashboard protocol and keep disconnected-client cleanup safe. Treat object/world teardown separately from a normal user-requested stop; never spin the game thread indefinitely waiting for work that requires its next tick.

Verify early stop during actual frames, immutable final counts after drain, retained partial artifacts and restoration, same-listener second capture, active/final reconnect and warmup cancellation on both RHIs. Retain hard caps and zero-drop qualification. This repairs the implementation to its promised behavior; it does not change label/schema meanings.

## Repair 3: comparable inputs and source-correct receipt reader

The console default route array is [blinking,lod_corruption,missing_texture,corrupted_texture]; dashboard uses GetEnabledIds pool order [blinking,missing_texture,corrupted_texture,lod_corruption]. Both enable the same four routes, but the original exact-order gate fails. Make the shared C5 entry or its caller produce one explicit stable sequence for equivalent enabled inputs, preserving every selected route, default membership, seed4242, camera opt-in and the schedule. Do not sort the saved failed receipts or declare their original exact-order result passing. Source files to inspect are AnomalyControlServerSubsystem.cpp, AnomalyAutoInjectorSubsystem.cpp and Compat/AnomalyCaptureLabels425.cpp. Avoid changing injector scheduling or rendering algorithms.

The reader's modulo8 is factually wrong: UE4.25 ScenePrivate.h:1050 and plugin AnomalyStreetBench425.cpp identify modulo7. Fix the reader with wrap controls and preserved old seal/results. Full captures satisfy grain_phase==family_frame modulo7 and family_frame-sample_frame==2 on every row.

The original one-offset-per-session assumption also encounters source-defined readiness waits: seven relative clock shifts in the current C pair occur at clean-prepass pose/census boundaries; scored rows share one fixed offset. Inspect StreetBench425::Ready and its caller before choosing a clock model. Any correction must remain within the approved paths/IDs/absolute-frame-offset/wall-clock normalization and preserve per-row timing/identity/actor bindings. Reject a one-frame error within a captured segment, wrong object, missing event, changed mask pixel, wrong schema, wrong seed/route, failed private-pass measurement, spill and dropped frame. Never introduce per-row arbitrary offsets to force equality.

Raw census scheduling observations (cycles, in_flight_batches, verified) also differ and must remain in the report. Establish their source meaning and existing four-rule/telemetry authority before deciding whether they are comparison gates or diagnostic notes. Do not silently drop fields, normalize away a correctness failure or infer label equivalence from aggregate counts. If existing authority does not cover the intended comparison, keep the gate open and identify that exact decision. A reader correction must have a new presealed prediction and active negative controls before new native legs.

## Validation and resource decision still to resolve before release

Preserve all r4 outputs, source/binary pins, packages, raw evidence, attempts and cumulative charges. No automatic new job or attempt reset. Audit the actual per-target/session counts, current wall deadline and full report before issuing a repair brief. Some initial qualification limits are spent (including normal Editor restore3, package UAT3, random desktop2); do not disguise an additional physical attempt under a new name.

A new native candidate needs matching normal/strict compile proof, source-backed transport/drain controls, protocol validation, a newly pinned staged Development artifact and fresh runtime evidence on that artifact. Retain the old package. No shader/content/config changes are proposed, so do not recook unchanged resources without a demonstrated need. Verify new package staging and unchanged cooked resources explicitly.

The current C primary evidence is about5.16GB under an11GiB cap. Five fresh full legs for a repaired binary cannot fit that remaining allocation. Before any such native work, root must make an explicit resource/attempt disposition under owner authority; do not evade the cap by renaming evidence as receipts, silently relocating it, compressing raw data or deleting old captures. A potential separate D primary allocation is only a proposal here, not authorized by this draft. Initial measured D free is about44GiB; any proposal must preserve25GiB floor+3GiB buffer plus the complete proved remaining peak, retain C/E floors and all incidental/protection limits, and name exact newly owned paths. E cache deletion is conditional on OD18 eligibility and measured need; no deletion is currently necessary or approved by path.

Keep one compiler job/action, no RAM admission wait, severe-pressure stops/drain, one worker/brief, protected trees/index and owner settings untouched. Further5.1 stays deferred. Keep all full2352-frame, strictIoU1/zero-spill, same-package paired comparison, lifecycle and actual-desktop gates. No release/office-ready claim while anything is open. CameraD4/G3 remain failed andGO3 unrun; plugin push/merge/tag held. Dashboard-only feature publication remains root-only after independent review.

Chat review **NOT REQUIRED** for preparing this source-backed repair proposal. Release of the continuation still requires root's completed r4 review and an explicit resource/attempt disposition within the owner's authority. Any actual change to capture/label meaning, correctness tolerance or release authority remains the next mandatory owner/Chat checkpoint; this draft authorizes none.

DRAFT — NOT RELEASED
