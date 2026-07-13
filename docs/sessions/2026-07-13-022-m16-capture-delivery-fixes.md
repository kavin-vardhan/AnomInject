# 2026-07-13 — 022 — m16: three capture-delivery fixes (token auto-populate + focus-gated start + preview-pause hardening)

Base: plugin m15 `bc10272` (both trees clean, confirmed at start), dashboard `8958ed7`.
Design → build in one turn (three well-scoped fixes). **No commit this turn** — owner eyeball first.

## Goal

Three fixes that together make the plugin+dashboard cleanly deliverable to a client running a packaged build:

1. **Client token auto-population** — a client with only a packaged build (no editor console) cannot read
   the random per-session token the server logs. Auto-fill the dashboard token with zero copy-paste.
2. **Focus-gated capture start** — clicking Start in the (browser) dashboard must not record idle frames
   during the click-and-move-back gap; the run's first frame must wait for game-window focus.
3. **Preview-pause hardening** — the live preview's synchronous game-thread JPEG generation must stop the
   instant a capture is active (not after a snapshot round-trip), so it can't drag sustained fps.

## What was done

### Item 1 — token auto-populate (Option A, owner-chosen: static baked token)

Mirrors the delivery-mode / content-clock client-vs-owner ini split exactly.

- **Server** (`AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp`): `StartListening` now reads
  `[AnomalyControlServer] Token` from `GGameIni` via GConfig. Present + non-empty → that fixed value is the
  token; absent/empty → the existing `FGuid::NewGuid()` random token. The listening log names the source
  (`from DefaultGame.ini` vs `random per-session`). Added `Misc/ConfigCacheIni.h` + `CoreGlobals.h` includes
  (Core, no new dep). Owner in-editor (no ini key) is byte-unchanged: random token, logged, manual paste.
- **Dashboard**:
  - `src/config.ts` (new) — `BAKED_TOKEN = import.meta.env.VITE_CONTROL_TOKEN`, storage helpers.
  - `src/vite-env.d.ts` (new) — `VITE_CONTROL_TOKEN` typing (manual `ImportMetaEnv`/`ImportMeta` merge; no
    `vite/client` triple-slash so the file stays comment-free).
  - `src/store.ts` — initial `token`/`wsUrl` from localStorage; `setCreds` persists them (owner stops
    re-typing the random token every reload).
  - `src/App.tsx` — mount effect: if `BAKED_TOKEN` present and disconnected → `setCreds` + `client.connect`
    (auto-connect; URL is the fixed `ws://127.0.0.1:8077` default).
  - `src/components/ConnectScreen.tsx` — token field pre-fills from `BAKED_TOKEN || persistedToken`;
    placeholder/hint updated.
  - `.env.example` (new) + `.gitignore` `.env` — so a real client token is never committed.

### Item 2 — focus-gated capture start (all engine, `AnomalyCapture`)

- New phase `ECapturePhase::ArmedPending`. `StartRun` now does arm-setup (validate world, pause auto,
  clean-slate reverts, RunDir/SessionId, viewport size, counter/timing resets), sets `bRunning = true` +
  `bRunBegun = false`, then decides: if `bFocusGate && HasGameWindow && !IsGameWindowFocused` →
  `ArmedPending` (+ "Capture ARMED — waiting for focus" log); else → `BeginActualRun()`.
- **`BeginActualRun()`** (new) holds the timing-critical bundle moved OUT of `StartRun`: `StartFrame`, the
  run manifest write, the fixed-timestep override, overlay suppression, `Phase = LeadIn`, `bRunBegun = true`,
  the "Capture run STARTED" log. So the run's frame indexing / pacing / manifest all start at the REAL first
  frame, not at arm.
- **`Tick`** handles `ArmedPending` before pacing/capture: focus (or no window) → `BeginActualRun`; else a
  periodic (2 s) waiting log + a 30 s safety timeout that begins anyway with a Warning. Returns while pending
  (no pacing/capture).
- **`FinishRun`** guards artifact writes with `bRunBegun`: begun → normal (drain / pacing / annotation /
  run_summary / FINISHED log); cancel-before-focus → delete the empty session dir, write nothing, log
  CANCELLED. Common teardown (bRunning/phase reset, fixed-step restore [already guarded], overlay restore,
  auto resume) always runs.
- **Focus signal:** `World->GetGameViewport()->Viewport->IsForegroundWindow()` — Engine-only `FViewport`
  API, packaged-safe in Development/Test (where `ANOMALY_CAPTURE=1`); no Slate needed for the gate itself.
  Added `#include "UnrealClient.h"` to guarantee `FViewport` is a complete type.
- **Headless / Simulate safety:** the gate applies ONLY when a game window exists (`GetGameViewport() &&
  ->Viewport`). MainWorld Simulate over the MCP bridge has no game viewport → gate skipped → run begins
  immediately, so the owner's Simulate capture smoke-gates do NOT deadlock. Plus `IAI.Capture.FocusGate 0`
  session override, `[AnomalyCapture] bFocusGateDefault` packaged default, and the safety timeout.
- New `SetFocusGate` + `IAI.Capture.FocusGate <0|1>` console command; GConfig read in `Initialize`; init /
  idle-status logs extended with `focusgate=`. (Fixed a pre-existing cosmetic typo in the idle-status log:
  `async\backbuffer` → `async/backbuffer`, since I was editing that same statement.)

### Item 3 — preview-pause hardening (control server)

- `PushFrames` early-returns when `Cap->IsCaptureActive()` (== `bRunning`, true from arm through finish),
  regardless of `bForce`. Preview JPEG generation stops the instant Start is accepted (armed-pending
  included); snapshots keep flowing so the dashboard still tracks state. The client-side unsubscribe on
  `capture.running` stays as belt-and-suspenders.

### Shared "capture-active" signal

`bRunning` is set at ARM (before the focus decision) and cleared at finish → one flag spans
arm→armed-pending→running→finish. `IsCaptureActive()` (new, `= bRunning`) is what the control server checks.
Both features key off it; armed-pending already reads as `capture.running=true` in the snapshot, so the
preview is suppressed across the exact focus-in moment the focus-gate is protecting. No new snapshot/WS field
was added (deliberate — minimal protocol churn; the engine log gives the owner armed-pending visibility, and
the client is looking at the game window at that moment, not the dashboard).

## Files touched

Plugin: `AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp` (token + preview suppression);
`AnomalyCapture/Public/AnomalyCaptureSubsystem.h` + `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`
(focus gate). Docs: `architecture.md`, `client-delivery.md`, `capture-fps.md`, `gotchas.md` (G71–G73),
this journal, `CLAUDE.md` status.
Dashboard: `src/config.ts` (new), `src/vite-env.d.ts` (new), `src/store.ts`, `src/App.tsx`,
`src/components/ConnectScreen.tsx`, `.env.example` (new), `.gitignore`, `README.md`.

## Invariants held

- `IAnomaly` / injector / anomalies / leaf-helpers BYTE-UNCHANGED; catalog stays **8**.
- No new module dependency: GConfig = Core; focus via Engine `FViewport`; AnomalyCapture already links
  Slate/SlateCore/ApplicationCore in non-Shipping.
- Ships-as-a-build: token ini read, focus gate, preview suppression all work packaged with no editor.
- Owner in-editor unchanged where it should be (random token + log with no ini key; Simulate not deadlocked).

## Gates

- **Dashboard build (self-run, GREEN):** `npm run build` (tsc typecheck + vite build) clean, 60 modules.
  With `VITE_CONTROL_TOKEN=TESTVALUE123 npm run build` the value is inlined into the bundle (1 match) →
  the baked-token auto-connect path compiles the token in as expected. Rebuilt clean afterwards.
- **Plugin C++ compile:** NOT run this session (no editor/UBT in this fresh headless session). Self-reviewed
  the changed regions; flagged for the owner's build.
- **Engine/PIE behavior gates (owner-run — need the real editor + a real window + browser):**
  - Item 1a (owner path): no ini key → server logs a random token; dashboard with no baked token → manual
    paste still connects; persisted token pre-fills on reload.
  - Item 1b (client path): set `[AnomalyControlServer] Token=TESTVALUE` in DefaultGame.ini + build dashboard
    with `VITE_CONTROL_TOKEN=TESTVALUE` → dashboard AUTO-CONNECTS with no typing.
  - Item 1c: token mismatch → connection rejected (token still enforced). Revert the ini key after.
  - Item 2: `capture_start` while the game window is NOT focused → run stays armed-pending, no frames, log
    says waiting; focus the window → run begins normally, lead-in intact; `IAI.Capture.Stop` cancels an
    armed run (empty dir removed). Confirm Simulate/bridge capture path is NOT stalled (should skip the gate:
    no game viewport).
  - Item 3: on a heavy scene with preview active, start capture → preview generation stops immediately
    engine-side; sustained fps not dragged; preview resumes after. Confirm armed-pending is also suppressed.
  - Combined: dashboard Start (unfocused) → armed-pending + preview suppressed → focus game → clean run →
    preview resumes. Both async + sync capture paths honor Items 2+3; zero async drops.

## Owner-eyeball items remaining

1. Real-browser auto-connect on a client-shaped build (baked token → connects, no typing).
2. Real-window focus-gate feel (Start from the dashboard, alt-tab to the game, run begins on focus; lead-in
   frames clean; Stop cancels cleanly).
3. Preview-pause on the loaded office box (preview freezes during capture, sustained fps not dragged, resumes).
4. Plugin C++ compile (Development-Editor, 5.1) + the Simulate capture smoke-gate not deadlocking.

## State

All code + docs written; dashboard builds clean; plugin compile + engine gates pending owner. **No commit.**
On owner acceptance: strip comments (`python _strip_comments.py` on both repos), commit plugin as one
`feat` (tag `m16`) + dashboard as its own `feat` (untagged, per dash precedent), update CLAUDE.md/journal to
as-built.
