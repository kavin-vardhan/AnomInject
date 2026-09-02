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

## ⚠ KNOWN LIMITATION — deep starvation presents a stale scene (BACKBUFFER PATH; ratio ≳ 3)

> **SCOPE, added at S4.** Everything in this section is a **BACKBUFFER-PATH** property — it is about
> what the *present* carries. **The default grab point since S4 is the SVE path**, which reads scene
> colour before Slate and was certified **ALL-ALIGNED at ratio 3.03** at `m24`.
> ⛔ **THE SHIP RULE ABOVE IS NOT RELAXED.** That certification is on a **synthetic bench level**
> (G89) at **`VideoFps` 30** (A52), and neither licenses loosening a delivery gate on real client
> content. Check `speed_ratio` before delivering, exactly as before.

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

## Display scaling and launch resolution (S4-1, measured)

**Packaged builds are DPI-unaware.** Windows reports 96 DPI to the process regardless of desktop
scaling and composites the output afterwards. **Captures are therefore at the LAUNCH resolution, not
the scaled desktop resolution.** All four rect sources — the delivered PNG, `labels.jsonl`
width/height, `annotation.video.resolution` and `run.json` viewport — agree in **both** regimes
(measured, S4-1 legs M1a / M1b / M1c: desktop at 150 %, once at the engine default and once with the
process forced DPI-aware and verified as such, including a non-multiple 1001×721 rect).

⚠ **RECOMMEND EVEN LAUNCH DIMENSIONS.** With odd dimensions the **mp4 is 1 px larger than the frames**:
h.264 requires even width and height, so the encoder pads (`pad=ceil(iw/2)*2:ceil(ih/2)*2`) — which is
why the encode succeeds at all. **Measured** on a 1281×721 leg: PNGs 1281×721,
`annotation.video.resolution` 1281×721, **mp4 1282×722**, 90 frames at 30 fps.
**This is a constraint of odd launch dimensions, not a defect**, and the encoder is deliberately
unchanged (crop-vs-pad is its own decision, not taken here).

## What a delivered frame CONTAINS — the grab point (S4)

The capture path has two grab points and the choice is what puts UI in the image or keeps it out:

| | **SVE / scene colour** | **backbuffer** |
|---|---|---|
| console | `IAI.Capture.SVE 1` | `IAI.Capture.SVE 0` |
| content | scene colour post-tonemap, **pre-Slate ⇒ NO UI** | the presented player frame, **UI INCLUDED** |

⚠ **The limit on that claim, verbatim, and it does not get promoted by repetition:**

> **UI exclusion is verified in pixels against Canvas AHUD output in `CB_GateLevel` at 1280×720
> windowed. Exclusion of Slate/UMG follows from compositing order and is REASONED, NOT MEASURED.
> Not verified in a gameplay level.**

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
machine (m15, 2026-07-13): the client titles (Bates, Concorde) are **wall-clock** — wall produces
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
Since `S4`: `capture_path` and the five `key_ring_*` counters.
🆕 **Since `m26`: `mask_probe_arms`, `mask_residual_discards`, `mask_nopass_discards`** — see the
Nanite limitation below. **`mask_probe_arms` must read `0` on every delivered session** (it counts
deliberate bench-only probe arms; the pre-delivery checklist has the check).

## `annotation.json` → `mask.provided` (m26 slice 2) — what it means to a client

Every event has always carried `mask: {provided: …}`. Since `m26` it carries a **real value**:

| `mask.provided` | meaning |
|---|---|
| **`true`** | the target's drawn pixels **were measured** for this event |
| **`false`** | **NOT measured** — the measurement did not run or could not see the target |

🚨 **`false` NEVER means "the target drew nothing".** It means no measurement exists. The common
causes are the Nanite limitation below, a target that left the camera frustum, and a hide-type
target with no measurable frame. **A `false` event carries exactly as much evidence as it did
before `m26`: none from this measurement.** ⚠ **No field reports the measured AMOUNT — that would
be a change to the annotation contract and is deliberately not made.**

## `m26` REMOVES SOME EVENTS — what is removed, what is NOT, and the accepted cost

**An event is removed from `annotation.json` IF AND ONLY IF its target was MEASURED and drew
ZERO pixels** (and it manifested). Removed events are counted in `run_summary.json` →
**`vetoed_events`**.

🚨 **THE ACCEPTED COST, STATED PLAINLY:**

> **`m26` vetoes only targets measured at ZERO drawn pixels. A target that OVER-CLAIMS — measured
> non-zero but far below its claimed extent, such as the `InstancedFoliageActor` measured at
> 5,689–13,342 px against a claimed 921,600 px (the entire frame) — IS NOT VETOED and ships as a
> valid label. `m26` is a PARTIAL cure for `H5`: it removes the zero-contribution case and leaves
> the over-claim case. The over-claim rule requires a calibration campaign including
> complex-silhouette legitimate targets, which do not exist in the current measured set.**

⚠ **AND ONE RULING THAT BITES A VETOED CASE, recorded as a decision rather than an oversight:** a
target measured at zero SILHOUETTE can still have indirect visual effect — shadow, GI, reflection.
`BP_SplineSpawn_C`'s banked hide showed a small in-bbox luma change (**0.0175**) while the mask
reads exactly zero. **`m26` vetoes it anyway, because the label points at the OBJECT and not at its
shadow.**

**LIMITS carried, stated not discovered:**
- **`L1`** the captured **frames are already on disk and are NOT un-written** — the veto removes the
  EVENT, not the PNGs. `video.total_frames` is unchanged.
- **`L2`** 🚨 **a post-`m26` event count is NOT comparable with a pre-`m26` one.**
  `vetoed_events` carries the delta; read the two together or the comparison is wrong.
- **`L3`** `labels.jsonl` (delivery OFF only) is **prebuilt and cannot be corrected**, so
  **delivery OFF and delivery ON WILL DISAGREE on event content.** Stated, not reconciled.

🚨 **`L3` IN ITS SHARPER, OBSERVED FORM — and it bites OWNER-SIDE TOOLING, not the client:**

> **IN A SINGLE DELIVERY-OFF SESSION FOLDER, `annotation.json` AND `labels.jsonl` NOW DISAGREE ON
> EVENT CONTENT.** The veto edits the in-memory accumulator before `annotation.json` is written;
> `labels.jsonl` is prebuilt per frame and cannot be corrected. **A fully-vetoed session ships an
> empty `anomalies` array beside 59 label rows asserting `anomaly_present` and `visible_positive`.
> NO CLIENT IMPACT — delivery mode does not write `labels.jsonl` — but OWNER-SIDE TOOLING THAT
> READS `labels.jsonl` WILL DRAW BOXES FOR VETOED EVENTS.**

**Affected tooling, named:** **`tools/verify_capture.py`** — it reads `labels.jsonl` and draws the
boxes — and **`overlay_watcher.py`, which invokes it automatically on every completed run**.
⚠ **`overlay_watcher.py` exists in THREE copies**: `host-tools\`, `anomaly-dashboard\host-tools\`,
`_M2Smoke\host-tools\`. ✅ **The Dashboard application itself is NOT affected — it does not read
`labels.jsonl`.**
⛔ **NOT FIXED, deliberately:** correcting `labels.jsonl` is a separate change with its own gates,
and `L3` was accepted as a limit before the veto was built. **If you overlay a vetoed session and
see boxes, that is this — not a labelling regression.**

## ⛔ KNOWN LIMITATION (`m26`) — THE CURE CANNOT SEE NANITE GEOMETRY, AND ON A NANITE-HEAVY TITLE THAT IS MOST OF THE LEVEL

`m26` decides whether a labelled target actually drew anything by rasterising it into a
**custom-depth / stencil mask**. On **UE 5.1 a Nanite primitive cannot write custom depth at all** —
the Nanite scene proxy never sets `bRenderCustomDepth` and the custom-depth pass has no Nanite path.
Setting the flag on a Nanite component **succeeds and verifies, and never reaches a pixel.**

**What that means for a delivered session:** a Nanite target is measured as `NOT_MEASURED` and is
therefore **ALWAYS ADMITTED, never removed.** That is safe — the cure can never delete a good
event — **and it is also the cure not working on that target.**

🚨 **THE SCOPE, STATED PLAINLY BECAUSE IT IS THE MORE IMPORTANT HALF:**

> **The two `H5` instances this cure was built from are reachable BECAUSE OF WHAT THEY HAPPEN TO BE
> MADE OF, not because `H5` favours measurable geometry.** On StackOBot, authored structural
> geometry — walls, floors, platforms, pillars, pipes, crates, doors, ramps — is **overwhelmingly
> Nanite and therefore unmeasurable by this cure on UE 5.1**, while foliage and simple planes are
> not. **Measured on this title, not projected.** On a Nanite-heavy host title **the cure is inert
> for most authored geometry**: those targets are always ADMITTED, never vetoed, which is safe and
> is also the cure not working there.

*(No percentage is quoted deliberately — an asset-count ratio would include non-mesh assets and
mislead. The PATTERN is the finding.)*

**How to see it in a delivered session:** `run_summary.json` → **`mask_nopass_discards`** counts
frames where the custom-depth pass did not produce for the target.
🚨 **IT IS NOT A NANITE COUNTER.** Its causes include Nanite geometry (above), **frustum culling**,
and any other route by which the target is absent from the view's relevant set — e.g. a target that
drifted off screen. In every case the frame is discarded and the event tends toward `NOT_MEASURED`,
**which ADMITS**. *(A high count therefore means "the measurement could not see the target on those
frames", not "the target is Nanite" and not "the target drew nothing".)*
⚠ **Scoped to UE 5.1** — a later engine with Nanite custom-depth support changes the Nanite half of
this, not the culling half.

🚨 **CORRECTED 2026-08-20 — "a Nanite target is ALWAYS ADMITTED" IS NOT UNCONDITIONAL.** `I11-A`
measured that a target the mask cannot see can reach `MEASURED_ZERO` and be **vetoed**, because the
pass-ran precondition is a **VIEW-LEVEL** property used per target. The paragraph above states the
safe case; it is the safe case **only while nothing else in the scene writes custom depth.**
⛔ **MECHANISM CLAIM ONLY — PIE, no incidence claim, and whether the shipping path supplies its own
writer is OPEN (`I11-B`).** **Do not read this as a statement about any delivered session.**
→ **the full correction of record is `docs/invisible-anomaly-mechanisms.md`, "SAFETY-PROPERTY
CORRECTION — the admit bias is sound at the enum and unsound at the assignment".** It is stated
once, there.

⚖ **RULED 2026-08-20 — `H6` IS DOCUMENTED, NOT FIXED, AND THE DECISION RESTS ON A HOST SETTING.**
The near-term ship target has **SUPPORT NANITE DISABLED at project level**, which makes a
Nanite-flagged mesh render through the conventional path and therefore **measurable** — so the
high-harm route is **inert as configured**. ⚠ **That is a property of the HOST PROJECT, not of this
plugin, and it can be undone by one checkbox.** → **the full entry, the second cvar it also depends
on, and what would reopen it: `docs/invisible-anomaly-mechanisms.md`, "`H6` — DOCUMENTED, NOT
FIXED".** ⛔ **Two pre-delivery boxes now exist for it — see `PRE-DELIVERY-CHECKLIST.md` §1.**

## `m27` — TURNING THE CURE ON IN A DELIVERED BUILD, AND HOW TO SEE WHAT IT REMOVED

🚨 **WITHOUT THIS BLOCK THE `m26` CURE DOES NOTHING.** `bMaskMeasure`'s compiled default is
**false**, deliberately, so a build with no ini key labels exactly as `m25` did — which is the
invisible-anomaly behaviour the client reported. **The ini carries the delivered behaviour.**

Paste into the **host project's** `Config/DefaultGame.ini` **before the cook**:

```ini
[AnomalyCapture]
bMaskMeasureDefault=True
bDeliveryModeDefault=True
bSelectionCensusDefault=True
CensusMinDrawnCoveragePctDefault=0.5
CensusMaxDrawnCoveragePctDefault=25.0
CensusMaxVerdictAgeTicksDefault=12
bCensusExcludeTranslucentDefault=True
```

🆕 **`m41` CHANGED WHAT THIS BLOCK IS FOR. READ THIS BEFORE DELETING A "REDUNDANT" KEY.**
At `m41` the **compiled** defaults for BOTH the mask and the selection census are `true`
(`bCensusEffective = census && mask && async`, so shipping the census ON requires the mask ON). So
`bMaskMeasureDefault` and `bSelectionCensusDefault` **no longer change behaviour — they are now a
PROVENANCE READOUT.** Keeping them is what makes the run's own echo say `from DefaultGame.ini …`
rather than `from COMPILED DEFAULT (on)`, and that echo is the only way to confirm the key reached the
cook. ⛔ **Do not "tidy away" a key because it matches the compiled default — that removes the only
evidence the cook consumed your config and re-opens `G88`.**
🔑 **The safety this buys: before `m41`, a cooked ini that lost `bMaskMeasureDefault` silently reverted
the build to `m25` labelling — invisible anomalies back, with no artifact difference at all. Now a lost
key downgrades PROVENANCE, never behaviour.**
⚠ **`CensusMaxVerdictAgeTicksDefault` is the FLOOR of the freshness window at `m41`, not a fixed age** —
the effective window is `max(this, lastCompletedCycleTicks + 8)`. Setting it to `0` still expires
everything (that is a deliberate diagnostic lever, not a tuning value).

Those keys, plus `bDeliveryModeDefault`, are the delivered configuration.
`bSveCaptureDefault` is deliberately **omitted** — the compiled default is already the UI-free SVE
path since `S4`, and leaving the key absent is what makes the grab point immune to `G88`.
`bFocusGateDefault` and `ContentClockDefault` are already correct for client titles.

🚨 **`G88` — WHERE IT MUST LIVE.** In a packaged build the ini that counts is the **COOKED**
`DefaultGame.ini`. **A loose ini beside the package is a SILENT NO-OP.** The key must be in the
project config **before the cook**, and the only trustworthy confirmation is the run's own log:

```
=== Capture(mask): EFFECTIVE FOR THIS RUN - mask ON (measure, report and veto), default from
    DefaultGame.ini [AnomalyCapture] bMaskMeasureDefault === READ THIS LINE, NOT THE INI.
```

That line prints on **every** run, mask on or off, and names where the value came from. If it says
`COMPILED DEFAULT (off)` while your ini says `True`, the key did not reach the build.

### THE BISECT — `IAI.Capture.Mask 0`

If captures look wrong, set it to `0` and **re-capture**. That returns the build to `m25` labelling
in about thirty seconds with no rebuild, and comparing the two sessions says whether the cure is
implicated. ⚠ **It takes effect BETWEEN RUNS, not mid-run** — stop the run first. (The same is true
of `IAI.Capture.SVE 0`, the grab-point bisect.)

### SEEING WHAT THE VETO REMOVED — 🔎 GREP FOR `VETOED-OBJECT`

**A vetoed event leaves NO trace in `annotation.json`** — that is the whole point of it. So the only
record is `run_summary.json` and the log.

- `run_summary.json` → **`vetoed_events`** (how many), **`translucent_vetoes`** and
  **`translucency_unknown_vetoes`** (the diagnostic below).
- The log → **one line per removed object, each beginning `VETOED-OBJECT`.** That single token
  appears nowhere else in the codebase, so `findstr VETOED-OBJECT` on the log lists exactly the
  objects that went, with `asset=`, `componentClass=`, `state=`, `maxCount=` and the translucency
  verdict on each.

⚠ **`translucency=UNKNOWN` IS NOT `opaque`.** It means the target was gone or carried no material
slot when the line was written, and it is counted separately for that reason. A translucent target
cannot write custom depth on UE 5.1 unless its material ticks *Allow Custom Depth Writes*, so a zero
measured on one may mean **the mask could not see it**, not that it drew nothing —
route (e) in `docs/invisible-anomaly-mechanisms.md`, an accepted cost.
⛔ **These are DIAGNOSTIC.** They feed nothing, gate nothing, and must never become a filter.

### `m27` ALSO REMOVES FOLIAGE FROM SELECTION

`AInstancedFoliageActor` is no longer selectable, so **delivered datasets contain no foliage
anomalies at all**. Its label boxed the entire frame for a ~1.4 % effect and was unusable.
→ the full reasoning, and the correction to the reason previously given for it, is in
`docs/invisible-anomaly-mechanisms.md`, **"FOLIAGE EXCLUDED FROM SELECTION"**.

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
