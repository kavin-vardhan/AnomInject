# 094-47 plan — Chat ruling: Codex's control-server plan is approved as the design, with five bounds. 094-47 implements it in gated stages after 094-46s lands

**Date:** 2026-11-05, about 12:20 IST · **Author:** chat Claude (orchestrator)
**Input:** `_reviews\094-47-codex-controlserver-plan.md`, a read-only Codex plan from the owner's own thread, saved by chat.

## Rulings

### GQ1. The design is approved
- **A plugin-owned 4.25 WebSocket transport** on the bundled libwebsockets 3.0.0:
  - loopback-bound;
  - raw binary frames with no 4-byte prefix;
  - fragments reassembled;
  - bounded queues;
  - serviced on the game thread, with dispatch after `lws_service`.
- **A transport interface** shared with a 5.1 wrapper.
- **One shared C5 entry** (`StartC5Run`) for console and dashboard:
  - a cancellable warmup ticker;
  - the dashboard keeps the game alive;
  - starts that are refused return an error.
- **`capture_start` on 4.25 accepts only** the C5-compatible preset: native PNG, the auto-pool routes, seed 4242 by default, and `maxFrames` equal to the C5 schedule.
- **The dashboard change:** show **LOD corruption** (the owner's instruction).
- **The office include-order fix** is folded in.
- **Shipping is excluded,** as on 5.1.

### GQ2. The 5.1 path: behaviour unchanged, compile-checked, runtime regression deferred
- The changes live on `feat/europa-port` only. `master` and `release/2026-10-05` are untouched, so nothing the client runs on 5.1 changes.
- **5.1-compiled code may change only as far as the shared transport interface needs.** The 5.1 wrapper keeps the existing calls.
- **Gate:** a 5.1 Development **compile** of the changed modules on the existing 5.1 tree (`Baseline51` or `ReGate51`, whichever builds the feat branch; say which). No 5.1 launch.
- **FUTURE:** the 5.1 dashboard runtime regression runs before this transport is ever merged toward a 5.1 release.

### GQ3. The dashboard repo
- **Branch:** commit the change on a new branch `feat/europa-425` in `D:\IntrusiveAnomalies\anomaly-dashboard`, from `7ea2e51`.
- **Tests and build:** run its existing tests, then build the desktop executable locally.
- **The push:** Code pushes that branch only, under the 094-39 rules (no tags, no force, fast-forward only, confirmed with `ls-remote`). `master` is untouched.
- If the desktop build toolchain is missing, stop that stage with NEEDS-DECISION. Never install system-wide toolchains without saying so.

### GQ4. Known limitations, accepted for now (FUTURE)
- No preview stream on 4.25: the dashboard shows status and controls, but no live picture.
- No arbitrary durations, JPEG, resizing or targeted captures.
- Non-auth errors aren't surfaced in the dashboard UI.

### GQ5. The packaged build
- **What:** the Paris host packaged as **Win64 Development** with the plugin (`RunUAT BuildCookRun`, installed 4.25), into a new folder on E: with its free-space floor kept.
- **Cooked:** the 4.25 materials and the private-pass resources.
- **Gates:**
  - a console-started C5 **from the packaged exe** passes H4 and the strict texture gates on EA_Core (the bench flags still apply);
  - the dashboard connects to that same packaged exe.
- **Paris host's own Content and Config:** any change is still a STOP. Packaging writes only to its output folder and the project's `Saved`/`Intermediate`, which are recorded.

## Stages for 094-47
- **Stage A, the code:**
  - transport, bridge, include fix and tests;
  - 4.25 Editor and Game compile, and the 5.1 compile (GQ2);
  - a protocol smoke against the **editor `-game`** build (connect, hello, catalog, a bad-token refusal, then start, stop and a second start of C5).
- **Stage B, the package:**
  - GQ5's packaged Development build;
  - console C5 from the packaged exe, then the gates;
  - the dashboard change and desktop build (GQ3).
- **Stage C, equivalence:**
  - per RHI (D3D11 and D3D12 at 1280×720), on the packaged exe, a console-started C5 against a dashboard-started C5, under the plan's gate 4;
  - a separate early-stop test;
  - reconnect and survival for a second capture.
- **Push:** the plugin `feat/europa-port` and the dashboard `feat/europa-425`, after Code's scans. Plus a short card: how to package, start the listener, connect the dashboard and run the preset.

## For the owner
- 🟢 **Codex's plan is solid, and I've approved it.** The dashboard gets a 4.25 connection that speaks exactly the same language as on 5.1, and starting a capture from the dashboard runs the very same checked capture as the console.
- 🟡 **One limitation for now:** no live preview picture in the dashboard on 4.25. Start, stop, status and the anomaly list all work.
- **The job starts as soon as camera clipping is pushed.**
