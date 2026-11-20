# 094-47 root Codex source review - working, not acceptance

Recorded 2026-11-05 about 15:14 IST. Root remains the sole orchestrator and independent reviewer. Code is monitoring/reviewing the authorized relay worker; this note does not steer that worker. Read OD1-OD6 and the AUTHORIZED brief for authority. Plugin source was read through git objects with GIT_OPTIONAL_LOCKS=0. No root build, launch or source edit.

Observed local commits: 2af16c00932c61dea86ce827d2d88388093cca7f (transport/C5 implementation), dfd11cadc7ca194cf47900e01c9718a791286ad9 (version-header boundary correction), a231a577905b3ea6163500b5a5982f108c8c2183 (platform-header isolation, smoke and receipt-token masking). The worker is still active; review the final tip and exact binary provenance again before acceptance.

## Source observations

- The 4.25 transport binds 127.0.0.1 and accepts the root URI. It sends LWS_WRITE_BINARY with LWS_PRE storage, without the old networking length prefix. Receive fragments accumulate until the final fragment has no remaining payload. Open/message/close events dispatch after lws_service returns on the game thread. Stop suppresses callbacks before context destruction and clears peers/events. Bounds are 1 MiB per inbound message, 16 MiB outbound bytes and 512 outbound messages per peer, 8 peers, and an aggregate event/byte bound. These observations do not substitute for runtime smoke.
- The shared C5 bridge initializes seed, schedule, directory and session status before warmup. Pending warmup is capture-active; the world and generation are checked. Stop/deinit remove the pending ticker. The dashboard passes exit=false. Console and dashboard call the same StartC5Run path. Stop refreshes completed/requested counts after the session drains its pending writer futures.
- The 4.25 control handler refuses mutations during capture while retaining subscription, catalog, status and stop. It rejects unsupported format/size/target fields, invalid seeds, nonmatching schedule counts and relative/empty base directories. GQ1's seed 4242 default is implemented. Dashboard sessions get fresh names under the selected base directory.
- Session receipts add planned/complete/result and report an early stop as PARTIAL unless a failure/drop is present; they do not claim incomplete work is complete. Full completion and failure restoration still require runtime evidence.
- Snapshot/capture_status/capture_stopped obtain authoritative subsystem frame totals. Disconnect handling removes only the connection, not the capture.
- The smoke driver uses ClientWebSocket text requests, requires raw binary JSON responses, fragments hello, exercises bad-token refusal, catalog defaults, active mutation refusal, warmup cancel/no delayed start, a fresh second session and reconnect. Tokens are read in memory and not placed in its result. This is a script inspection, not a smoke PASS.
- The dashboard's current Start control sends the same field names used by the server. The qualified preset remains a user-selected custom count of 2352, native PNG and the four default routes. GQ4's non-auth-error UI limitation remains explicit.

## Evidence seen so far

The Stage A predictions receipt predates the physical stages. focused-tests.json reports 543 checks PASS, with length-prefix, premature-fragment, unbounded-queue and post-stop mutations all failing as expected; scope can-fails reject weakened floors. Code's live note reports the first Editor compile failed for missing version includes and the second passed. The Game result, final rebuilt source, 5.1 compile and protocol smoke still require final receipts/review. Do not read this note as a Stage A verdict.

## Outstanding acceptance work

Read Code's full final report and independent review, then verify all claimed stages against original receipts. In particular: final source-to-binary identity; ReGate51 compile only; editor-game smoke; package capacity and preserved floors; cooked resources; packaged H4 and strict texture IoU 1.000/zero-spill checks; configured and random tokens, restart/port conflict and loopback; real built desktop discovery/auth; same-package per-RHI console/dashboard equivalence; early-stop artifacts, warmup cancellation, reconnect and second capture; host/material/P9 preservation; exact dashboard push and plugin no-push status.

Camera 094-46s D4/G3 failures and GO3 remain OPEN. No source observation or passing Stage A/B/C result clears them. Plugin push remains HELD; no release acceptance here.

Chat review NOT REQUIRED for this read-only review or the active OD1-OD6 development work. The next mandatory owner/Chat checkpoint is any proposed qualification-meaning, threshold or release-authority change; none is made here. Existing release/merge review checkpoints remain.
## 15:29 IST correction and Stage A stop

ReGate51's UBT child returned exit code0 and linked, but its log contains14 first-header errors (child-004.log lines16-29). The unchanged error gate therefore rejected it. Root initially described the exit-code result as a pass in a progress update; that statement was corrected promptly. It is NOT a clean 5.1 compile pass. stop.json records Stage A/GQ2/A1 NEEDS-DECISION and zero runtime launches. Smoke and Stages B/C remain unrun.

Root read the affected files' leading includes through feat/europa-port Git objects. All14 put Compat/AnomalyCaptureFeatures.h before the matching header;13 files are byte-unchanged from the job's entry according to git diff --name-only, and the14th is the modified capture subsystem whose leading include issue is separately recorded unchanged in stop.json. The matching private headers already have the advanced-feature guards; the subsystem public header now includes the engine-version header. A narrow include-order repair is a concrete follow-up candidate, with fresh 4.25 Editor/Game and5.1 compile validation and no build-gate suppression. Do not stage until Code's full final review is read and the previous job is idle.

The final4.25 child receipts are editor-03 exit0 (22.428s) and game-02 exit0 (32.823s). Those logs contain no compiler errors or Windows macro warnings in the root's focused scan. Final source/binary/P9 verification still awaits the completed reports. The ReGate51 checkout restoration receipt says40a8eaf983518fe1992e172434aa8caf347654f1 detached, with the built binaries retained and archived.

The archive requires974343342bytes and reduced E: to about51.3GiB. Capacity must be remeasured; nothing here permits deletion, new archive/package compression or lower floors.