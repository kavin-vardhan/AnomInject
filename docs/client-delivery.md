# Shipping a client build — delivery mode (m12)

This is the owner handoff for shipping AnomalyInjector to an external client who runs capture in their
own build. There is no post-processing step between the client's capture and the client — **whatever
capture writes to disk IS what the client receives.** Delivery mode limits a run's output to only the
client-facing files.

---

## ⛔ THE SHIP RULE — check `speed_ratio` before delivering ANY session (m21, HARD GATE)

**Label correctness is RATE-DEPENDENT. `run_summary.json`'s `speed_ratio` is the per-session trust check —
read it before every delivery.**

| `speed_ratio` (with `paced: true`) | Verdict |
|---|---|
| **≤ ~1.05** | ✅ **SAFE TO DELIVER** — frame-exact (proven at 1.000 and 1.052: labels/annotation match the pixels 0/100 mismatches) |
| **≳ 1.1** | ⛔ **DO NOT SHIP.** Lower `IAI.Capture.Fps` until the run sustains ≈ 1.0, then **re-capture**. |

**Keep pacing ON** (`IAI.Capture.Pace 1`, the default) — it is what makes real-time (wall-clock) client content
play at the correct speed (m11). *(Pace-off no longer corrupts labels as of m21, but it still produces
wrong-speed video for wall-clock titles.)*

**Why:** a capture run that cannot sustain its target rate starves the render thread, and the presented backbuffer
starts carrying a **stale scene** — the pixels lag their own frame index. m21 made the arm→present pairing
deterministic (fixing the pace-off and mild-overrun cases outright), but under **deep starvation (ratio ≳ 3)** the
GPU presents stale content and **no arm-side fix can repair pixels the present never contained** — see the known
limitation below. A starved session is silently mislabeled, which is exactly the contamination this whole
delivery path exists to prevent.

**Audit already-delivered sessions:** any session whose `run_summary.json` shows `speed_ratio > 1.02` carries
shifted labels and should be re-checked or re-captured. The field has shipped in `run_summary.json` since m11, so
this is checkable retroactively on every session ever delivered.

**Also see:** `capture-fps.md` (the two-clock model and how to pick a sustainable `IAI.Capture.Fps`).

---

## ⚠ KNOWN LIMITATION — deep starvation presents a stale scene (ratio ≳ 3; tracked for m22)

Under deep starvation the defect is **below** the capture layer and is NOT a labeling bug:
- **Materials** (e.g. `missing_texture`): a one-time mid-run slip leaves the presented content one frame behind
  its index for the rest of the run — while the arm→present pairing telemetry stays *perfect* (0 drift, 100/100).
- **Render-state changes** (e.g. `blinking`'s `SetActorHiddenInGame`): can fail to appear in **any** presented
  frame at all. Measured at ratio 3.2: an 8-tick hide window, game-state-proven via the annotation, never showed
  up in a single pixel.
- Staleness is therefore **change-type-dependent**, which no arm-side pairing can fix.

**Mitigation today:** the ship rule above — never deliver a session with `speed_ratio ≳ 1.1`.
**Fix:** m22 — a scene-identity marker (minimal SVE) that publishes which tick's scene each present carries, so a
frame can be *marked* rather than mislabeled when the present is stale.

**Two theories that were investigated and REFUTED — do not re-chase them:**
- **Delivery mode** is NOT a variable: full-fidelity vs delivery A/B on the same scene were both frame-exact.
  `bDeliveryMode` gates only `run.json`, the label-record write and a `run_summary` flag.
- **Content clock (wall vs game)** is NOT a variable: `ContentClock` only ever reaches the fps *stamp*
  (`LastRunPacing.StampedFps`); frame indices are a plain counter. game@240 and wall@240 both shift; game@30 and
  wall@30 are both exact. The variable is **`speed_ratio`**, nothing else. (G82.)

## What delivery mode does

`IAI.Capture.Delivery 1` (or the packaged default below) makes each capture run write **only**:

```
<session>/
  Actual_Frames/frame_%05d.png    the image sequence
  Video_Clip/<session>.mp4         the encoded video (produced host-side by encode_watcher.py)
  run_summary.json                 run counts + video-timing + delivery_mode flag (the encoder's done-signal)
  annotation.json                  the labeled ground truth (client's primary artifact)
```

and **suppresses** (never writes):

```
  labels.jsonl    per-frame label sidecar (internal / QA)
  run.json        run manifest — and this is where the SEED lives
```

The label ground truth is still fully **computed** in delivery mode (annotation.json is complete) — it
is simply not written to the per-frame sidecar. Default is OFF (full fidelity); OFF output is identical
to normal capture except that annotation.json no longer carries the internal `schema_version` /
`source_id` tags (removed in both modes as of m12).

## Content clock (m15) — default is wall (client titles are wall-clock, RESOLVED)

The shipped default is `IAI.Capture.ContentClock wall`. Owner-tested wall vs game on the actual office
machine (m15, 2026-07-13): the client titles (Until Dawn, Concorde) are **wall-clock** — wall produces
correct-SPEED videos for them. Their video LENGTH varies with the real capture duration, which is
correct for wall-clock content (natural playback speed is the criterion). This RESOLVES the m14
open question (it briefly shipped as game pending this test).

**Do NOT set `ContentClockDefault=game` in a CLIENT build** — for wall-clock titles that stamps a slow
run at the target and plays their videos ~2× FAST (the Issue-2 regression). Client build = wall default,
no action needed. `game` is only for game-clock content like StackOBot, set in that build's own ini.
See `capture-fps.md` for the full model.

## How to set it before packaging a client build

Add to the **project's** `Config/DefaultGame.ini`:

```
[AnomalyCapture]
bDeliveryModeDefault=True
```

The subsystem reads this at startup, so a packaged Development build ships in delivery mode with no
editor and no console command. Note: GConfig caches the ini at startup — if you edit it while the editor
is open, restart the editor for it to take effect.

At runtime, `IAI.Capture.Delivery <0|1>` overrides the default for the current session (mid-run changes
are refused; stop first). `IAI.Capture.Status` shows the current `delivery=` state.

## What the client receives — and does not

- Receives: the frames, the mp4, the run summary, and the full labeled annotation.json.
- Does NOT receive: the per-frame labels.jsonl, the run manifest, or the **seed**. Because the seed lives
  only in run.json (suppressed), **a delivered session is not client-reproducible** — this is intentional;
  reproduction metadata stays owner-side. If you need a reproducible archive of a delivered run, capture it
  once in delivery-OFF mode for yourself (that keeps run.json + the seed), or read the seed from the
  "Capture run STARTED" log line (logged in both modes).

## Our QA tooling and delivery sessions

`host-tools/overlay_watcher.py` and `tools/verify_capture.py` read labels.jsonl and therefore **do not
process delivery sessions** (by design). Verify capture correctness in the default (delivery-off) mode;
ship in delivery mode. `host-tools/encode_watcher.py` is unaffected — it keys off run_summary.json +
annotation.json only, so the client's mp4 still encodes from a delivery session.

## run_summary.json fields (shipped in delivery mode — reviewed client-safe)

`type`, `schema_version`, `total_frames`, `positive_frames`, `bursts_done`, `zero_match_bursts`,
`end_frame` (raw engine frame counter), `target_fps`, `sustained_wall_fps`, `speed_ratio`, `stamped_fps`,
`paced`, `delivery_mode`. No seed; nothing owner-sensitive.

## Dashboard token — zero copy-paste for the client (m16)

The control server needs a token before the dashboard can drive it. In-editor the server logs a random
per-session token to the Output Log — useless for a client with only a packaged build (no console). For a
client build we bake a **fixed shared token** into BOTH the game and the dashboard so the dashboard connects
automatically with nothing to read, copy, or type.

Set it in two places, and they MUST match:

1. The **game** — `Config/DefaultGame.ini` (same file as the delivery-mode / content-clock keys):

   ```
   [AnomalyControlServer]
   Token=<pick-a-long-random-value>
   ```

   `StartListening` reads this at startup. Present + non-empty → it is the token. Absent/empty → the server
   falls back to the random per-session token + the existing log line (this is why the owner's own dev build,
   which sets no key, is unchanged).

2. The **dashboard** — set the same value as `controlToken` in the `config.json` that ships beside the built
   app (`<delivery root>/dashboard/config.json`):

   ```json
   { "controlToken": "<the-same-value>", "capturesRoot": "", "serverUrl": "ws://127.0.0.1:8077" }
   ```

   When a token is present the dashboard pre-fills it and **auto-connects** to `ws://127.0.0.1:8077` on
   load — no clicks. `capturesRoot` is filled in by `Setup.bat` on the client's machine; `serverUrl` is
   optional and defaults to `ws://127.0.0.1:8077`.

   > **This replaced a build-time bake (M2, 2026-07-21).** The token used to come from a gitignored `.env`
   > (`VITE_CONTROL_TOKEN`) compiled into the JS bundle, which meant assembling a delivery from a clean
   > checkout produced a **silently tokenless** dashboard, and changing the token required a rebuild. It is
   > now read at startup from a plain, visible, hand-editable file: no rebuild to change it, and a missing
   > or wrong token is something you can open and fix. There is deliberately **no env fallback** — one
   > source of truth.
   >
   > Degradations are all non-fatal: **no `config.json`** (or one the server answers with a page) → the
   > dashboard opens its manual connect screen; **malformed JSON** → same, plus a console warning; a
   > **present-but-rejected** token → the distinct "token rejected" screen from M1, whose copy names
   > `config.json` as the file to fix. The dashboard also still remembers the last token you typed
   > (localStorage), so the owner's own PIE workflow is unchanged.
   >
   > ⚠ **`dist/config.json` carries the DEV token after a build** — Vite copies `public/config.json` into
   > `dist/`. Overwrite it with the client's token when assembling. See `PRE-DELIVERY-CHECKLIST.md` §2.

**Security tradeoff (owner-accepted):** the baked token is a STATIC shared secret embedded in the two
privately-shipped client artifacts (recoverable from the cooked ini and plaintext in the JS bundle). It is
NOT per-session-random. It is still worth having: browser `ws://` connections ignore CORS, so without a token
any website the client visits while the game runs could drive the control server and pull viewport JPEGs; the
baked token stops any origin that doesn't know the value. This is a localhost-only research tool shipped to one
client — worst case if both artifacts leak is unwanted local injection / a viewport-screenshot on the client's
own machine; there is no network exposure. Do **not** disable auth to get auto-connect — that removes the only
defense against arbitrary local web origins.

## Capture Start waits for game-window focus (m16)

Clicking **Start** in the dashboard (a browser window) would otherwise record idle frames during the moment
the game window is unfocused and the client is clicking back to it. So a Start now **ARMS immediately** but
holds the **first frame** until the game window has foreground focus. The client hits Start, alt-tabs to the
game, and the run begins on focus — the lead-in frames are clean. The log shows `Capture ARMED — waiting for
game-window focus`; `IAI.Capture.Stop` cancels an armed run (nothing is written). Default ON; override with
`IAI.Capture.FocusGate 0` or the packaged default `[AnomalyCapture] bFocusGateDefault`. If there is no game
window (headless), the gate is skipped and the run starts immediately; a safety timeout also starts it anyway
if focus never arrives.

## Preview auto-pauses during capture (m16)

While a capture is active (armed or running), the server stops generating the live preview JPEGs entirely, so
the preview never competes with the capture for the game thread on a loaded machine. The dashboard preview
simply freezes on its last frame during the run and resumes automatically when the run ends. This is engine-side
and authoritative — it does not depend on the dashboard reacting in time.
