# Handoff — External Control Dashboard (Tier-2) — built for real

**Session date:** 2026-06-21
**Scope of this session:** Designed and built the local external control dashboard end-to-end — the runtime in-build control server (plugin-side), the standalone dashboard app, and the host-side overlay-automation watcher. This was a separate/parallel chat from the main milestone sequence and from the frame-capture/labeling chat.

**Read this first if you're cold:** This doc carries the decisions + rationale that are NOT yet in the repo's journals. For on-disk operational detail, have Code read `CLAUDE.md` + the latest session journals + `gotchas.md` (see Pointers at the end). **And read the Reconciliation warning at the very bottom before touching git** — two chats share one repo.

> **Post-wrap update:** after the milestone was first wrapped, three bug fixes landed (a frozen-dashboard/competing-drivers bug, the capture↔auto-pool workflow it exposed, and the poll-radius debug sphere appearing in captures). All owner-verified fixed. See §7 "Post-wrap fixes" — and note the **critical workflow fact** there: capture injects FROM the AutoPanel's enabled pool.

---

## 1. Current state — what's built & validated

The dashboard is **complete and operator-usable**. The full loop works hands-off: **capture → labels written → bounding boxes drawn**, driven from the dashboard cockpit.

**Three pieces, two boundaries (the architecture):**
- **Dashboard app** — external, its own git repo at `D:\IntrusiveAnomalies\anomaly-dashboard`. React + Vite + TypeScript + Zustand. Host tooling, NOT under the plugin repo. Talks to the game only over a localhost WebSocket.
- **Control server** — a NEW runtime module `AnomalyControlServer` *inside* the plugin, walled off from the anomaly core. Build-define `ANOMALY_CONTROL_SERVER` (off in Shipping); listener dormant by default (started via `IAI.Server.Start`). WebSocket-only transport (`WebSocketNetworking`); localhost bind + token handshake.
- **Anomaly core** — untouched except 4 additive read-only getters (below). Deps still `Core/CoreUObject/Engine/InputCore`. `IAnomaly` still locked.

**The 4 core read-back additions** (additive, `IAnomaly` untouched, deps unchanged — validated byte-clean in Stage 1):
- **A1 `GetAnomalyCatalog()`** (`AnomalyCatalogTypes.h`) — id/desc/usage **+ scope + structured typed arg schema**, sourced from a registration-time authored table (NOT parsed from `GetUsage()`).
- **A2 `GetActiveAnomalies()`** — `{id, args, secondsActive}` via an **inert** `ActiveRecords` side-table (stamped after a *successful* Apply; cleared on no-op/Revert/RevertAll). Zero effect on Apply/Revert returns, match counts, or `IsActive`.
- **A3** auto cadence getters + structured `GetLiveFires()`.
- **A4 `GetVisibleRenderableActorInfos()`** — visible set + class/comp/distance/normalized screen-rect in **one pass** (shared `FirstRenderableVisibleComponent` worker). `GetVisibleRenderableActors` is **literally unedited / byte-identical** — the hard regression gate, confirmed `MATCH` empirically (93 vs 93).

**Three server command groups** (commit `af44e7d`, gate-green over real WS dispatch):
- `set_poll_radius {cm}` → `AnomalyViewport::SetPollRadius` (cm≤0 = OFF); snapshot `session.pollRadius`.
- `auto_config` / `auto_run` (Enable-then-Run) / `auto_step` / `auto_fire_once` → existing auto setters.
- `capture_start` / `capture_stop` / `capture_status` → wired to m7's `UAnomalyCaptureSubsystem`. **Fork A**: added a read-only `UAnomalyCaptureSubsystem::GetStatus()` getter (additive, zero behavior change) so the panel can show frame count + run dir.

**Dashboard app** (built in slices A→B→C→D, all gate-green; Slice A commit `af7d1ba`, B/C/D landed after — confirm exact SHAs via git at next bootstrap):
- Transport `AnomalyClient.ts` + `protocol.ts` (framing-agnostic — see §2), Zustand `store.ts` with snapshot-diff event derivation, `ConnectScreen`, `SessionBar` (conn/FPS/seed/revert-all/scoping+HUD toggles/poll-radius slider), `PreviewCanvas` (JPEG→canvas, overlay rects, click-to-select smallest-area-wins, near-fullscreen rects non-clickable), `TargetsPanel` (filter + two-way selection sync), `InjectPanel` + generic `ArgControls`, `ActivePanel`, `AutoPanel`, `CapturePanel`, `EventLog`.
- **Optimistic UI** is a standing rule: controls update locally on input; the snapshot reconciles as source-of-truth.
- The throwaway `RawDump` debug box was **removed** in Slice D; the plain-language `EventLog` replaces its useful role.

**Host-side overlay watcher** — `D:\IntrusiveAnomalies\host-tools\overlay_watcher.py` (+ README). Zero-dep stdlib poll loop; triggers on `run_summary.json`; runs `verify_capture.py --dir <run>` → annotations into `<run>/annotated/`; de-dups via `.overlay_done` marker; **backfills existing runs on startup**; fail-soft. Run with the Pillow interpreter `C:\Python313\python.exe`. A `start_overlay_watcher.bat` double-click launcher is being added (brief issued at session end — confirm it landed).

**What's NOT done:** see Open Questions (§4) — all are deferred niceties, none block the working system.

---

## 2. Decisions made this session (rationale — the important part)

**Build the REAL runtime server, not a throwaway bridge prototype.** Owner call. The plugin was mature enough (IAnomaly locked through 5+ milestones, primitives proven) to invest properly. **This SUPERSEDES** the prior framing that the dashboard track was "exploratory / out of the main sequence." It is now production: the Tier-2 runtime control server was pulled forward and built for real.

**One WebSocket carries everything; the binary-frame reality.** *Critical, non-obvious.* libwebsockets sends **every** server→client message — including JSON — as a WS **binary** frame. Clients MUST be **framing-agnostic**: read leading bytes; `"AIF1"` magic ⇒ preview frame (16-byte header `{magic, u32 frameId, u32 epoch, u16 w, u16 h}` + JPEG); otherwise decode the whole payload as UTF-8 JSON. **Never assume a text opcode.** The Slice-0 "JSON arrives as text" note was WRONG (it only worked because the spike client was already framing-agnostic). Reference impls: `WebClient/spike-client.html` and the app's `AnomalyClient.ts`. The server is correct — this is a client rule.

**Preview = `FViewport::ReadPixels` on the game viewport.** This **SUPERSEDES** the ratified `OnBackBufferReadyToPresent` + screenshot-delegate pair — both failed their constraints in practice: in docked PIE the backbuffer path captured the *whole editor window*, and the screenshot delegate writes a PNG to disk per frame. `ReadPixels` is game-view-only, no disk, works docked or windowed, matches packaged. Accepted cost: a synchronous render-flush, bounded by keeping cadence low (~2–5 fps; a control-preview, not gameplay). The render-thread async readback is the **documented deferred upgrade** (higher fps + the packaged-build capture smoke-test). Bonus: the server's `Build.cs` **dropped** `RHI/RenderCore/Slate/SlateCore` because `ReadPixels` didn't need them.

**Catalog carries `scope` + a typed arg schema → the inject UI is generic.** The dashboard builds inject controls from the catalog (slider for ranged float, dropdown for enum, etc.), and the target picker is shown only for `scope:"object"`. **Consequence the owner specifically asked about: adding new anomalies later (texture, animation, …) needs ZERO dashboard/server changes** — a new anomaly that describes itself in the catalog renders correctly automatically. `scope` was also needed *now* for correctness (globals like `time_dilation`/`camera_clipping` must not show an object picker; `lighting_mismatch` is component-scoped). Sourced plugin-side via an authored table; `IAnomaly` untouched.

**Labels are engine-side, frame-locked, authoritative. Never label from the preview stream.** The bbox *coordinates* are written to `labels.jsonl` the instant each frame is captured — that IS the dataset, complete the moment a run stops. `verify_capture.py` only **draws** boxes onto PNG copies for human inspection; it is NOT a label producer. The preview overlay (rects from a ~10 Hz snapshot on a ~12 Hz frame, best-effort `epoch` correlation) is fine for *monitoring* but WRONG as a ground-truth basis. So capture stays in-engine; the browser never harvests labels (it can't write disk anyway, and pushing full-res frames + masks over WS to label in JS is a non-starter). UE stays a pure data producer.

**Overlay automation is host-side, decoupled.** Auto-drawing boxes on stop is done by the host-side watcher polling the capture folder — NOT by UE shelling out to Python (which would couple the plugin to a Python install + script path and break the ships-as-a-build invariant). The `run_summary.json` completion signal already existed. The watcher is a convenience — nothing is lost if it's delayed or skipped, because labels are already written.

**Pure-client discipline.** The app and the watcher never silently modify the server (it's gate-validated and feature-complete); any gap gets flagged, not patched. (Honored throughout — several real spec gaps were flagged, not edited.)

**"Do not modify `UAnomalyCaptureSubsystem`" was clarified** to mean *don't change its behavior* — a purely additive read-only getter (`GetStatus`) is allowed and was added (Fork A).

---

## 3. Forward plan / sequencing

1. **`.bat` launcher for the watcher** — brief issued at session end; confirm it landed (`D:\IntrusiveAnomalies\host-tools\start_overlay_watcher.bat`).
2. **(Deferred, optional) Auto-launch the watcher with the dashboard** — an `npm run dev` variant that starts the watcher alongside the dev server, so the owner never starts it manually. The `.bat` is the lighter option chosen for now.
3. **(Deferred) Packaged-build capture smoke-test + render-thread async readback preview upgrade** — the `ReadPixels` path is PIE-validated only; packaged validation + higher-fps preview are tied together as the deferred upgrade.
4. **(If wanted) small server additions** — universal `active[]` countdown (`secondsRemaining`), a `request_snapshot` command, an absolute `runDir`. All currently worked around client-side; only add if the workaround chafes.
5. **Main-sequence roadmap threads continue as before** (new anomaly types — texture/animation/etc., the separate replay/paired-capture plugin, etc.). New anomaly types now flow into the dashboard for free. These are tracked in project memory, not this session's scope.

---

## 4. Open questions (need a call) vs Locked decisions

**Locked (settled — don't relitigate):**
- Build the real runtime server (not throwaway). Three-layer architecture: external app (own repo) / server (own plugin module, compiled out of Shipping) / core untouched bar read-only getters.
- WebSocket-only transport; clients are framing-agnostic (binary-frame reality).
- `FViewport::ReadPixels` preview path for v1; async readback is the deferred upgrade.
- Catalog carries scope + typed arg schema; inject UI generic; new anomalies need zero dashboard edits.
- Labels engine-side, frame-locked, authoritative; `verify_capture.py` is human-viewing only; never label from the preview stream.
- Overlay automation is host-side (the watcher); engine stays a pure producer.
- Pure-client discipline; gaps flagged, never silently patched.

**Open (deferred, not blocking):**
- Universal `active[]` countdown — currently client-derived from `auto.liveFires` by id; manual/global injects show no countdown. (`active[]` has no `secondsRemaining`.)
- `request_snapshot` command — doesn't exist (only `request_frame`); steady cadence + optimistic UI cover confirmation. Only needed for sub-interval confirmation.
- Capture `runDir` is engine-relative (`../../../…`) in the reply/snapshot; the app normalizes to basename for display (full path on hover). A server-side absolute path is a possible future nicety.
- Auto-launch the watcher with the dashboard (see §3.2).
- The `manual` single-shot capture dir has **no** `run_summary.json`, so the watcher ignores it by design — overlay by hand if ever wanted.

---

## 5. Corrections / things that changed (so stale understanding doesn't carry forward)

- **SUPERSEDED:** "the parallel dashboard track is exploratory / out of the main sequence / main sequence unchanged" → it is now **production**; the Tier-2 runtime control server was built for real this session.
- **SUPERSEDED:** Slice-0's "JSON arrives as WS text" → **wrong**; all server→client messages are WS **binary**; clients must be framing-agnostic.
- **SUPERSEDED:** the ratified preview path (`OnBackBufferReadyToPresent` + screenshot-delegate) → replaced by `FViewport::ReadPixels` (game viewport).
- **comp tag is now SM/SK only** — VFX was removed from the renderable-visible set by the labeling/other track (commit `9bbd398`, reverses the earlier G29 VFX inclusion). The A4 `"FX"` branch is now dead code (cleanup deferred; server untouched).
- The "don't modify `UAnomalyCaptureSubsystem`" instruction was clarified (behavior, not additive read-only getters).

---

## 6. Pointers — what a new chat should have Code read

- **`CLAUDE.md`** — always, on cold bootstrap.
- **`gotchas.md`** — new this session: the cross-module log-category `LNK2001` (each module needs its own `DEFINE_LOG_CATEGORY`; core `LogAnomaly` vs module `LogAnomalyServer`), plus (to be appended at next journal pass) the **binary-frame protocol finding** and the **`ReadPixels` game-view-only preview path**. Confirm exact G-numbers via Code — the other track also added entries around VFX removal / poll-radius. *(Code reported the log-category gotcha as G32; the file is append-only and shared across tracks, so numbers may differ — verify.)*
- **Session journals** — this whole dashboard arc (spike → command groups → app slices → watcher) is **not yet in a session journal** (journals are held to milestone boundaries). At next session, have Code write/confirm the journal(s) for this arc. The labeling track's journals (e.g. 012) + the 011 vfx-removal/poll-radius handoff hold adjacent on-disk detail.
- **Dataset format** — `run_summary.json` / `labels.jsonl` / `run.json` schemas live with the m7 capture work (labeling track). Read `verify_capture.py` + the m7 journal for the format (`bbox_px`, `visible_positive`, etc.).
- **The dashboard repo** — `D:\IntrusiveAnomalies\anomaly-dashboard\README.md` has the dev-run story (`npm install` → `npm run dev` → `http://localhost:5173` → `IAI.Server.Start` → paste token → connect `ws://127.0.0.1:8077`).

---

## 7. Post-wrap fixes (landed after the initial wrap — all owner-verified)

Three bugs surfaced and were fixed after §1–§6 were first written. All on our side; the labeling track's m7 capture subsystem was NOT edited.

**Bug 1 — frozen dashboard / "both stopped" while the engine kept running.** With AutoPanel `auto_run` ON, `capture_start → stop → start` made the dashboard show both auto and capture as stopped while the engine kept running both, and controls went dead. Root cause was a stack of three, not a crash:
- Capture's per-frame `ReadPixels` + the server's preview `ReadPixels` each do a synchronous render flush → ~8 FPS → the **snapshot stream starved**.
- The client only expired optimistic entries inside `setSnapshot`, so a stalled stream **never reconciled** the displayed "stopped".
- `client.send()` **silently dropped** commands issued while the socket wasn't OPEN (so `capture_stop` never executed).

Fixes (all pure-client except Bug 3's A1): timer-based optimistic-entry expiry in `store.ts` (not only in `setSnapshot`); `send()` reports sent/dropped and a dropped command surfaces an error with NO fake optimistic success; a "stream stalled / state unknown" banner after ~2 s of no snapshot + controls disabled when disconnected/stalled + a manual reconnect; and **while `capture.running`, the client unsubscribes from preview frames** (drops the redundant per-cycle `ReadPixels`) to ease the starvation. Recovery from the wedge was always just reconnect / `IAI.Server.Stop`+`Start` (no PIE-kill needed). The deeper `ReadPixels`→async-readback upgrade stays the documented deferred item.

**Bug 2 / CRITICAL WORKFLOW FACT — capture is NOT self-contained; it injects FROM the AutoPanel pool.** Capture reuses the auto-injector's **enabled pool** (the AutoPanel checkboxes) + its **seeded stream** to decide what to inject: capture's `BeginFire` calls `Auto->TryFireOnce`, which draws id/target/hold from `EnabledIds` + the seeded `Stream`; capture sets that seed via `StartRun → Auto->SetSeed`. **So the AutoPanel pool IS the user's injection selection for capture.** Owner capture workflow = **tick the bugs in the AutoPanel → leave `auto_run` OFF → Start capture**; capture fires those bugs in clean labeled bursts. **Empty pool = capture injects nothing** (clean/negative frames, no boxes) — legitimate, not a bug.

**Bug 3 / product decision A1 — capture pauses the AutoPanel auto-injector (control-server-side).** Running continuous `auto_run` AND capture = two competing drivers on the one-instance-per-id registry (corrupts frame labels). Fix: `capture_start` records `bAutoWasRunning = Auto->IsRunning()` and if true calls `Auto->SetRunning(false)` **only** — which stops the free-running `Tick→AdvanceTime` loop + `RevertAllLiveFires`, but does NOT touch `EnabledIds`/`Seed`/`bEnabled`, so capture's own `TryFireOnce` keeps injecting. Restore `Auto->SetRunning(true)` on `capture_stop` AND when the server Tick observes `Cap` no longer running (finite runs/teardown). A1 must NEVER `SetEnabled(false)` / clear the pool / reset the seed, and the UI must NOT lock the pool checkboxes during capture (they're the selection) — only auto's RUN is suppressed. Snapshot reflects `auto.running=false` while capture owns injection. Implemented in the control-server layer — no m7 capture-subsystem edit.

**Bug 4 — poll-radius debug sphere baked into captures.** The `IAI.SetPollRadius` debug sphere (session 011 / `ae57b69`, drawn in `AnomalyViewport`) was rendering into the game view, so capture's `ReadPixels` baked it into dataset PNGs. Fixed so the sphere is **absent from captured frames** while still visible for **live monitoring**, and the poll-radius **cull stays active** (visual suppressed, cull untouched). The sphere lives in the other track's poll-radius code — the fix was kept minimal/coordinated; confirm the exact mechanism + SHA via git.

**New open item (owner decision pending): the capture seed side-effect.** `StartRun` overwrites the auto-injector's seed via `SetSeed` (pre-existing m7 behavior), so after a capture run the AutoPanel seed is capture's, not the user's original. A1 doesn't worsen it. Decision deferred: restore the user's original auto seed after a capture, or treat the seed as capture's? **Defaulted to leave-as-is.**



The **dashboard track** (this chat) and the **frame-capture/labeling track** (a separate chat) share **one** plugin git repo. They have already entangled once: the labeling track committed THIS track's Step-2 server WIP **verbatim** at `ff1be3c`, then shipped **m7** (`tag m7 = f5125ae`) on top of it.

**Therefore, at any cold boot, before building anything:** have Code run `git log --oneline -20`, `git status`, `git describe --tags`, and **distinguish committed vs working-tree state per track.** Do NOT trust this session's last-reported SHAs as the live state — verify against the repo. Known anchors as of this session: HEAD was around `22cf34f` (`m7-1-g…`) before the command-group work; the three server command groups landed at `af44e7d`; the dashboard app is its own independent repo (untagged, separate from the plugin's `m*` tags). Confirm all of these live.

**Cross-track coordination still owed:** this session added a read-only `GetStatus()` getter to `UAnomalyCaptureSubsystem` (the labeling track's m7 file). When next in the labeling chat, tell it: *the dashboard track added a read-only `GetStatus` to `UAnomalyCaptureSubsystem` — reconcile at SHA `f5125ae`.* Also: the watcher *calls* `verify_capture.py`; if the labeling track changes that script's CLI, the watcher's invocation needs a tweak.
