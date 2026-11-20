# 094-47r owner-delegated ruling — compatibility and audit prerequisites

2026-11-05, 16:55 IST. Author: root Codex, today's owner-authorized orchestrator. This is an owner-delegated ruling, not a Chat ruling. Authority remains the owner's instruction, "You take the best decisions for all of these yourself", with no gate relaxation and the approximately 20:30 IST checkpoint.

Read 094-47-codex-disposition.md first. OD1-OD6 remain in force. The specific scope extensions below take precedence over GQ2's former transport-only edit limit; none changes what a passing build, capture or release means.

## OD7. Fix the first-header diagnostics

Authorize include ordering and strictly necessary include/feature-guard placement in these 14 Source/AnomalyCapture/Private translation units and their matching headers:
AnomalyAsyncWriter, AnomalyCaptureSubsystem, AnomalyCensus, AnomalyChangeFamilyData, AnomalyChangeStage, AnomalyLabelWriter, AnomalyMaskMeasure, AnomalyMaskSceneViewExtension, AnomalyMeasurability, AnomalyPreviewTee, AnomalySceneViewExtension, AnomalyStencilTag, AnomalySveCapturer, AnomalySveKeyRing.

The matching header must be first. Preserve runtime bodies, defaults, fields, types, engine/feature guards and behavior. Prefer ordering-only changes. No camera, shader, asset or host Content/Config change. No disabling IWYU, suppressing diagnostics or accepting exit 0 with first-header errors. Both 4.25 builds and the required 5.1 changed-module build must pass on the new source. No extra old-entry 5.1 build is required merely to reproduce the known defect.

## OD8. Repair the audit prerequisites without widening admission

Authorize a narrow canonical 4.25 capture-bench predicate in AnomalyBenchGate.h/.cpp and replacement of the 12 raw IAIBench checks in these Compat files: AnomalyCameraSession425.cpp, AnomalyCaptureLabels425.cpp, AnomalyCaptureSession425.cpp, AnomalyHostDepth425.cpp, AnomalyMasks425.cpp, AnomalyPrivatePass425.cpp.

The existing IsEnabled() and its IAIBench-or-IAIBenchFixture behavior stay unchanged. The new predicate requires IAIBench itself and is false in Shipping. Preserve all companion flags, maps, routes, timing, near-plane, private-pass and admission conditions. Prove no flag=false, IAIBench=true, IAIBenchFixture alone=false, both=true, and Shipping=false. This is centralization of the existing predicate, not a camera correction or wider bench admission.

Authorize tools/lever_audit.py and its focused tests to use explicit supported engine/configuration profiles derived from source/build declarations. Root's 32-location evidence is in 094-47-codex-lever-profile-analysis.json; reproduce independently. Include ANOMALY_BENCH, engine and capture macros and actual target/module configuration. Bind binary audits to the exact built source/profile and artifact hashes.

Retain R1-R6 and B1-B3, conservative unknown-condition treatment, Shipping exclusions, registration/extra/missing/echo checks and nonempty expected inventories. No lever-name exceptions, deleted rules, swallowed failures or empty-parse passes. Retain existing rule mutants on engine-active branches, and prove active missing registration, extra binary lever, missing echo and wrong profile are rejected. A mutant in compiled-out code is INVALID, not evidence of gate sensitivity.

The existing full strict-include obligation is retained, using the authorized kit and resource bounds. Any additional code defect beyond this precise scope must be reported, not silently fixed by a broad refactor or waived.

## OD9. Capacity and ordering

Keep C: 10 GiB and D:/E: 50 GiB floors, all kit start/reserve/stop rules, the 3 GiB D: runtime launch reserve, and the existing C: per-session growth safeguard. Do not use weaker legacy constants as authority. No owner-process, machine/power/security, reparse-point or elevation change.

No general cleanup, old-evidence deletion, relocation to an overflow root, binary/raw-image/package compression or reduced number/length of required sessions is authorized. Only this job's disposable scratch may be removed by its exact creation/deletion receipt, as already allowed. Preserve every pre-existing receipt, raw capture, archive and protected asset.

For the new binary manifest, an unchanged artifact may reference an existing immutable, rehashed archive copy instead of duplicating it. Every changed artifact still requires the normal preserved evidence and provenance. This removes redundant copying only; it does not remove any artifact from the inventory or qualify an unbuilt binary.

After Stage A passes, complete the minimal independent dashboard implementation, existing tests and desktop build before starting Stage B's package. OD4 already permits that independently passing component to be retained/pushed if another component stops. This ordering does not clear package, runtime or Stage C gates.

Measure per-volume peak writes before cooking: package, host Saved/Intermediate/DDC, archives, complete required sessions and review outputs, including simultaneous copies and reserves. Current E: headroom is inadequate; do not assume it improves. Use only the authorized new E: package/evidence paths. If a concrete budget cannot fit, mark package/physical continuation HELD FOR CAPACITY and finish the independently authorized work and review. Known impossible budgets need no three-hour idle wait; GA5 remains the maximum admission bound. No false completion or automatic cleanup to force progress.

## OD10. Provenance, ownership and checkpoint

Entry: clean local feat/europa-port a2250fda7a9b2f3754fd13c6061fe632f45b9d6e; origin c5d0919e5074404d26da8c16ccf845b360bde383; dashboard master 7ea2e51bf55e19c2949641bbff2ec72afb4ae493. Code verifies the 094-47 final 4.25 compiled base and source equality before relay resume.

ReGate51 starts with source 40a8eaf but mixed, unqualified binaries as recorded in the disposition. Rebuild the selected candidate before claiming a compile, archive/identify the resulting artifacts, and restore the source through the existing receipted mechanism. Do not call the restored tree a qualified baseline or use its artifacts as 40a8eaf binaries. No 5.1 runtime.

A fresh job configuration may register 09447r under the same standing FM1 incremental policy (two actions, at most three attempts per target), FK1/GA5 runtime limits, write fence and process ownership. Only exact new job/path registrations needed by these policies are allowed; no policy reduction. Strict-include work must fit its applicable existing admission and attempt rules. Old attempts remain recorded.

Code uses a fresh default session for this substantial follow-up: the closed session measured 66% of 1M, near its 70% rollover boundary. One watcher, one brief, one relay worker. Code launches/reviews; relay implements/builds/captures; root reviews and orchestrates. No in-flight steering or competing work.

At 20:30 IST (15:00 UTC) on 2026-11-05, finish any active child safely, then checkpoint before another physical stage/leg absent owner extension. Do not infer a Chat takeover or extend the window for standby.

Plugin push remains prohibited. Dashboard feat/europa-425 alone may be pushed by Code after its independent tests, desktop build, review and clean scan under OD4/GQ3. Preserve the 094-45r shipped card; any new card says LOCAL VALIDATION / NOT OFFICE-READY. Camera D4/G3 failures and unrun GO3 remain open; no new camera session or correction here.

Chat review NOT REQUIRED for OD7-OD10 or work within this scope. The next mandatory owner/Chat checkpoint is changing qualification meanings, thresholds or release authority; none is authorized. Existing release/merge reviews remain mandatory.
