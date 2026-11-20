# 094-47 — Code's review: Stage A stopped at GQ2. The 4.25 control transport and shared C5 entry are written and compile on 4.25 Editor and Game; the 5.1 compile on ReGate51 links (UBT exit 0) but prints 14 first-header errors that the build gate rejects. Those 14 come from an include order introduced on 2026-10-31 (a2cc30d) that is already on origin and was never 5.1-compiled before this job. Protocol smoke, packaging, the dashboard change and Stage C are unrun. Nothing pushed; no dashboard branch exists

**Date:** 2026-11-05, about 16:45 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Base attestation first (OD2):** `code-receipts-094-47\base-attestation-094-47.md` / `.json`, written before launch. BASE_ATTESTED=True: the 09446r Editor and Game builds (build commit cd2d60c) exit 0 / PASS with 0 error lines; all 27 live binaries equal the 09446r archive; cd2d60c..f3fbba7 touches only tools/docs (35 files, no Source/Shaders/Content/Config/descriptor/Build.cs/Target.cs); host Source and .uproject predate the build.
- **Launch:** resume of thread `01a14334` at 14:36:53 IST after a dry run. Detached, W4 (the child saw no `GIT_*_DATE`; one secret-named variable removed by name only), sandbox `danger-full-access`, `gpt-6-astra`, effort max (read from the live process's command line).
- **Prompt:** `resume-prompt-094-47.txt`, 43,489 B, `B2507FDE…`. Order: job list (the brief restated, with the two added write areas: dashboard branch `feat/europa-425` only, and `E:\IA_BuildCache\09447-package`), the plan verbatim (whole file), GQ1–GQ5 + stages verbatim, OD1–OD6 verbatim, a precedence paragraph (OD over older ordering/push clauses; supersedes Chat-only reservation of these decisions), my context, then the 094-43r implementer block (byte-equal to the tail of the 094-43r and 094-46s prompts). Receipt: `code-receipts-094-47\prompt-receipt.txt`.
- **Preserved before launch:** 094-46s's 9 report outputs (`code-receipts-094-47\preserved\`, all copy-equal).
- **Run:** 14:36:55 → 16:26:45 (6,590 s), exit 0, one try. Codex's verdict: **NEEDS-DECISION**. Its report: `_reviews\094-47-codex-report.md` (body byte-equal to `report.md`, `57BD89BD…`).
- **The PC was in modern standby from 15:30:45 to 16:13:50 IST** (System log Kernel-Power 506/566 enter, 507 exit). Codex had already stopped at 15:23 (stop.json) and was writing its report; events.jsonl resumed at 16:16:46. No build or session was in flight during standby. No power setting was touched.
- **My files:** receipts and polls (every ~2.5–3.3 min, UTC epoch per line; one 43-min gap for the standby, noted in `midjob-notes.txt`) in `code-receipts-094-47\`; readers in `D:\IA_Europa\Temp\CodeReview09447\`.

**Commits (local `feat/europa-port`, nothing pushed):**

| Commit | What |
|---|---|
| `2af16c0` | transport interface, 4.25 libwebsockets transport, 5.1 wrapper, wire helper + selftest, shared C5 entry, capture_start/refusal/capture_active, include fix, smoke script |
| `dfd11ca` | version-macro includes at public boundaries (after Editor attempt 1 failed, exit 6) |
| `a231a57` | transport platform headers isolated; token redaction for copied logs; tools |
| `a2250fd` | docs and stop receipts only (no build input changed since a231a57) |

- Author and committer dates are real, equal times (W4). Gotchas G556 and G557 were used; **the next free number on feat is G558**.

## 1. Stage A — the code, by reading the diff against the plan (GQ1)

**Transport (`AnomalyControlTransport425.cpp`, `AnomalyControlWire.h`):**
- **Loopback bind:** `Info.iface="127.0.0.1"` with `LWS_SERVER_OPTION_DISABLE_IPV6`; peers are also still checked by `IsLoopbackAddr` on the address from `lws_get_peer_simple`.
- **Raw binary, no prefix:** `lws_write(..., LWS_WRITE_BINARY)` from an `LWS_PRE`-offset buffer; no length header anywhere in the send path.
- **Reassembly:** fragments accumulate until `lws_is_final_fragment && lws_remaining_packet_payload==0`.
- **Bounds:** 1 MiB inbound per connection, 16 MiB / 512 messages outbound per connection, 8 connections (`AcceptPeer(Peers.Num())`), an aggregate inbound cap and 1,024 queued events; only path `/` is accepted; plain HTTP is refused.
- **Game-thread dispatch:** `Tick()` calls `lws_service(Context,0)`, moves the event list, then dispatches Open/Message/Close after service returns; `check(IsInGameThread())` on Start/Tick/Send/Stop.
- **Single teardown:** `Stop()` sets `Stopping` and clears callbacks before `lws_context_destroy`; the static callback returns 0 while `Stopping`; the destructor calls `Stop()`.
- **5.1 wrapper:** keeps `CreateServer`/`Init`/`Tick`/`Send(...,false)` and the receive/closed/error callbacks, now keyed by connection id. One 5.1 log line changes: the socket-error callback no longer logs "Control: socket error." (it routes to close). 5.1 runtime is unrun (GQ2 defers it).
- **The token announcement is unchanged** (`=== Control server token: … ===`, now followed by a log flush), so the dashboard's log discovery can still read it.

**C5 bridge (`AnomalyCaptureLabels425.cpp`, `AnomalyCaptureSubsystem.*`, `AnomalyCaptureSession425.*`):**
- **One shared path:** the console `IAI.Capture.C5` now calls `StartC5Run(…, ExitOnFinish=true)`; `capture_start` calls it with `false`.
- **Cancellable ticker:** the warmup ticker handle is stored; `StopRun` → `Cancel` removes it; a generation counter makes an obsolete lambda return without starting. Warmup rules unchanged (10 s minimum, 2 s quiet, 120 s timeout).
- **Refused starts return an error:** the capture subsystem returns `OutError`; `capture_start` replies `{"type":"error","code":"capture_refused"}`; mutations during an active capture get `capture_active` (list/subscribe/request_frame/status/stop stay available).
- **Keep-alive:** `Finished()` exits only when `ExitOnFinish`; the C5 admission failure in `Labels::Start` no longer calls `RequestExit`.
- **Preset (GQ1/OD3):** native PNG only (format/anomaly/target/outputHeight/targetArgs/outputWidth refused); routes = the auto-pool's enabled ids; seed defaults to 4242; `maxFrames` must equal the C5 schedule; `dir` must be absolute and gets a fresh `session_<local>_<ticks>` folder.
- **Counts:** `capture.json` `requested` is now the real requested count (was the planned count), plus new `planned`, `complete` and `result` (`PASS`/`PARTIAL`/`FAIL`); labels result can be `PARTIAL`.
- **Review notes (runtime unverified):** (a) `StartC5` now runs `AnomalyHostDepth425::Admit` before warmup and `Labels::Start` runs it again after warmup, so a C5 run writes two admission receipts (run root and `epoch01`) and could refuse earlier than before; (b) the console path now refuses a relative or reused run directory; (c) the lever audit's new R6 line (`AnomalyCaptureLabels425.cpp:423`) is the moved inline `-IAIBench` test.

**Include fix:** `AnomalyViewMetadata425.h` includes `SceneRendering.h` before `SceneViewExtension.h`, inside the 4.25-only `ANOMALY_CAPTURE_FRAMES425` guard.

**Tests:** the standalone wire selftest passes 543 checks; the four mutants (length prefix, early fragment, unbounded queue, after stop) each fail at a named check (`focused-tests.json`). These cover the pure C++ wire helper only; the libwebsockets integration (bind, framing on the wire, fragment delivery, teardown) has no live evidence because the smoke did not run.

## 2. Stage A — the builds (my `builds47.py`, from each result.json and log)

| Build | Commit | Exit | Gate | Seconds | Compiler/linker error lines | First-header lines |
|---|---|---:|---|---:|---:|---:|
| 4.25 Editor attempt 1 | 2af16c0 | 6 | — | 136.2 | 18 | 0 |
| 4.25 Editor | dfd11ca | 0 | PASS | 101.2 | 0 | 0 |
| 4.25 Game | dfd11ca | 0 | PASS | 110.4 | 0 | 0 |
| **4.25 Editor (final)** | a231a57 | 0 | PASS | 22.6 | 0 | 0 |
| **4.25 Game (final)** | a231a57 | 0 | PASS | 33.1 | 0 | 0 |
| **5.1 ReGate51 Editor** | a231a57 | 0 | — (rejected) | 213.3 | 0 | 28 (14 files) |

- Engine actions 0 on every build; the only other diagnostics are the 2 accepted user-XML warnings. Live plugin DLLs equal the final builds (AnomalyCapture `8036d3cd…`, AnomalyControlServer `ddf6af98…`), archived at `E:\IA_BuildCache\09447-evidence\binary-archive` (38 files, 0.907 GiB).
- **Host and plugin binaries changed only while a build process was alive** (13 timeline entries, 15:03–15:13 IST, each with live `cl`/`link`/UBT). No game, editor, ShaderCompileWorker or crash-client process was ever seen: **no runtime launch happened**.

## 3. GQ2's stop, independently checked
- **The 14 errors** are UBT's "Expected X.h to be first header included" on 14 `AnomalyCapture/Private/*.cpp` files, each of which starts with `#include "Compat/AnomalyCaptureFeatures.h"` before its own header. Codex's statement that these lines are unchanged from entry `f3fbba7` is correct.
- **They are older than this job, and older than origin's tip:** that include order was introduced by `a2cc30d` (2026-10-31, "add guarded 4.25 native frame capture and G488"), which is an ancestor of origin `c5d0919` (and of the first push `d1cdc7b`). At `40a8eaf`, ReGate51's checkout, those files began with their own header.
- **The last 5.1 compile of the port was 094-21 at `40a8eaf`** (`R\09421\builds\51port-editor` and `51port-game`: exit 0, PASS, 0 first-header lines; my `firsthdr47.py` over every 5.1 result.json in `D:\IA_Europa\Receipts`). So feat has carried a 5.1 compile diagnostic since 2026-10-31 that no gate saw, and 094-47's GQ2 compile is the first to see it.
- Both 094-21 and 094-47 compiled AnomalyCapture in unity mode, so the difference is the include order, not the build mode.
- **Scope:** fixing it means editing 14 5.1-compiled AnomalyCapture files, beyond GQ2's "only as far as the shared transport interface needs". Codex stopped rather than edit or reinterpret the gate. I agree that is the ruled scope; whether to accept, fix or separate it is a decision (below).

## 4. Stages A-smoke, B and C: UNRUN
- No editor `-game` smoke, no package (`E:\IA_BuildCache\09447-package` was never created), no packaged C5, no dashboard branch, tests or desktop build, no equivalence, early-stop, reconnect, second capture or desktop-exe connection. Nothing to recompute.
- **Capacity, my estimate (not a measurement):** after the binary archive, free space is C: 22.24, D: 53.20, E: 51.34 GiB, i.e. **about 1.34 GiB above E:'s 50 GiB floor and 3.2 GiB above D:'s** (the kit's 3 GiB launch reserve comes on top). Existing C5 sessions cost about **1.6–1.8 GiB of E: evidence each** (09445r D3D12-1279: 1.586 GiB; 09446s positive/control: 1.695/1.821 GiB). Stage A's smoke (a second C5 start), Stage B (package plus one packaged C5) and Stage C (at least four full C5s plus an early stop and a second capture) need several times the remaining E: headroom before the package itself is counted. **Under the current floors, no deletion and no new compression (OD3/OD5), the remaining stages do not fit**, independently of GQ2.

## 5. Safety
- **Crash / upload sign:** none (no runtime log exists; no new dump, WER entry or crash-client file since launch).
- **Token:** no listener ever started, so no token value was generated. My leak reader (`token_leak47.py`) found one regex match, in `events.jsonl`; it is a script pattern string, not a token. New commits contain no token; `control_server_425_smoke.ps1` writes `tokens='<masked>'`.
- **Paris host:** its own Content (1,456), Config (18), the descriptor and `.uproject` hash-equal before and after; plugin Content (7) unchanged. Host and plugin Binaries and Intermediate changed only through the builds (§2). `Saved` unchanged (36 files).
- **Materials:** the 4.25 pink (`b88d8861`) and checker (`5222b3ee`) equal the `8df75de` blobs in both copies and at every ref through the tip; both 5.1 materials equal `d7eee97`'s blobs everywhere.
- **Fixture:** nine assets, `Engine.ini`, Config and `.uproject` equal to 094-46s's exit.
- **Deletions and P9:** my tree lists (D:\IA_Europa, the run folder, E:\IA_Fixtures) show **0 files removed**, 0 reparse points, 0 P9 hash changes; drive roots unchanged. Codex receipted 6 makefile deletions (rebuilt by the next build) and removed 378 files its own temporary ReGate51 checkout had created (`regate51-restore-intent.json`).
- **ReGate51 is left in a mixed state:** its source is restored to `40a8eaf`, clean, but `UnrealEditor-AnomalyCapture.dll` and `UnrealEditor-AnomalyControlServer.dll` are now a231a57's unqualified build (`dc3195fd…`, `4e83f6ca…`). The 40a8eaf builds they replaced (`0e14f06a…`, `3e740f3b…`, recorded in 09409–09412 qualification receipts) were hashed but not copied; they are rebuildable from 40a8eaf. Rebuild ReGate51 before relying on its binaries.
- **Lever audit (Codex's, retained as failures):** 12 R6 inline `-IAIBench` tests (11 identical at entry, 1 moved into the shared C5 entry) and 32 B2 source/binary registration mismatches (Codex says the audit compares unfiltered 5.1 registrations with the 4.25 binary union).
- **m51:** passes (HEAD `53bf725`, index `526aba7f`, 15 untracked) at start, dry run, prelaunch and exit.
- **Refs:** local feat `a2250fd`, origin `c5d0919` (ls-remote equal, an ancestor); spikes local; backup ref unchanged; fixture worktree detached at `8e59a56`, clean. Dashboard unchanged: master `7ea2e51`, clean, no `feat/europa-425`, same 7 refs and build outputs.
- **Foreign activity:** the owner's Codex desktop app, two blender-mcp processes and the MCP node servers, present throughout and untouched. No foreign UE editor appeared.
- **Compression:** none in this job.

## 6. Pushes and cards
- **Plugin:** not pushed (OD4). Scan written for the record: `_reviews\094-47-prepush-scan.md` (PUSH HELD).
- **Dashboard:** nothing to push; no branch or commit exists. `_reviews\094-47-dashboard-prepush-scan.md` says so.
- **Card:** `_reviews\094-47-office-card-europa-dashboard.md`, marked LOCAL VALIDATION / NOT OFFICE-READY. The shipped card stays `_reviews\094-45r-office-card-europa.md`.

## Decisions (for root Codex / the owner; OD1–OD6 stand)
1. **GQ2's first-header errors (new finding).** Options:
   - **(a)** Authorize fixing the include order in the 14 AnomalyCapture files on feat (put each file's own header first, keeping the feature guard working on both engines), then rerun the 4.25 and 5.1 compiles. This is a 5.1-compiled change beyond GQ2's transport scope, so it needs the scope ruling.
   - **(b)** Accept "UBT exit 0 + link + only these 14 pre-existing first-header diagnostics" as GQ2's compile result for this job, recording them as an inherited 5.1 debt to fix before any 5.1 merge. This changes what the gate accepts, so it is a gate-meaning decision.
   - **(c)** Add evidence first: a 5.1 compile of `f3fbba7` (entry) on ReGate51, to show 094-47 adds 0 new 5.1 diagnostics. Information only; it does not by itself clear GQ2.
2. **Capacity (new finding, my estimate):** E: is about 1.3 GiB above its floor; the remaining smoke, package and Stage C sessions need several times that. With no deletion and no new compression allowed, the remaining stages need more free space on E:/D:, a different evidence location with room, or fewer full sessions — an owner decision.
3. **ReGate51's mixed binaries:** rebuild from 40a8eaf before any use of that tree.
4. **Inherited (unchanged by this job):** camera_clipping D3D12 1279×719 D4 and strict texture G3 fail, GO3 unrun; the plugin-push hold stays (OD4); 5.1 runtime regression required before any 5.1 release merge (GQ2).
