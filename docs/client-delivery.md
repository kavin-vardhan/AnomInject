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

---

## `m43` — THE TARGET ID MASK: what ships, and how to read it

Every captured frame gets an **8-bit grayscale PNG** at `target_mask/frame_NNNNN.png`, numbered by the
same **`session_index`** as `Actual_Frames/`, at **exactly the picture size**.

- **pixel value 0** = background.
- **any non-zero value** = an **anomaly target** visible in that frame, identified by that value.
- **`mask_map.json`** (session root) maps `mask_value` + event → `target_name`, `anomaly_type`,
  `first_frame`, `last_frame`. ⚠ **Values are REUSED across events**, so key on `mask_value` *together
  with* the frame range, never on the value alone.
- **`labels.jsonl`** gains **three** keys: `mask_file` and `mask_state` on the frame row, `mask_value` on each anomaly row.
- **`run_summary.json`** gains **three**: `target_mask_frames_measured`, `_hidden_blank`,
  `_unavailable`.
- ⛔ **`annotation.json` is unchanged.**

### 🔑 `mask_state` — three values, and a file exists IFF it has content (m44)

| `mask_state` | file | meaning |
|---|---|---|
| **`present`** | yes | measured, a target was visible; `mask_file` names it |
| **`empty`** | **no** | measured, the target drew nothing |
| **`unmeasured`** | **no** | **no measurement exists**; no claim about visibility |

🚨 **`empty` and `unmeasured` must not be merged** — one is a measurement whose answer is zero, the
other is the absence of a measurement. ⛔ **No all-zero PNG is ever written**, and `mask_map.json`
lists only masks that exist.
📌 `target_mask_frames_hidden_blank` counts `empty` rows — **the name is kept deliberately** (renaming
a key silently breaks a reader) — and `target_mask_frames_unavailable` counts `unmeasured` rows. The
three counters sum to the captured frame count.
🔻 **SUPERSEDED BY `m45` (below): "hidden-object anomalies have no mask yet" was true when this
section was written and is FALSE now.** `missing object` and `blinking` get the target's would-be
silhouette on every labelled hidden frame. Corrected in place rather than deleted, because the
sentence shipped once and a reader may have it.

The run's own echo states it, and this line prints on every run:

```
=== Capture(m43): TARGET MASK ON FOR THIS RUN - requested on, from COMPILED DEFAULT (on), output dir
'<session>/target_mask' === READ THIS LINE, NOT THE INI. One 8-bit grayscale PNG per captured frame,
numbered by SESSION INDEX; non-zero pixel values are the stencil tags of the ANOMALY TARGETS visible in
that frame and 0 is background. mask_map.json maps value+event to target and anomaly type. m44: A FILE
EXISTS IF AND ONLY IF IT HAS CONTENT; mask_state is present|empty|unmeasured. mask_file:null means NOT
MEASURED - they are different facts. It reuses the m26 pass and does NOT change the m26 measurement, the
veto, or annotation.json. Delivery mode does NOT suppress it.
```

### ⛔ SCOPE — what the mask is NOT

- **It is a mask of the ANOMALY TARGETS ONLY**, not of every object in the scene. Everything else is
  background by design.
- **Translucent targets never appear.** They are excluded from selection and cannot write custom depth.
- **Nanite-rendered targets never appear** — the same limit the anomaly measurement has.
- **Multi-target frames are unverified.** Read it as *"one value per anomaly target present in the
  frame"*; no capture here has yet shown two distinct values in one PNG.

### The mask is provably the silhouette the labels were judged on

Per measured frame and per target, the PNG's pixel count for that value is checked against the same
per-tag reduce table the anomaly veto reads. On the shipping gate: **29 checks, 0 mismatches.**

### ⚠ COST — and the first knob to turn off

The mask adds a **GPU→CPU readback per fire-active frame** (**921,600 bytes** at 1280×720; it scales
with your capture resolution) plus one PNG encode on a worker thread. On the dev box at the shipped
paced 30 fps the pacer absorbed it entirely (`speed_ratio` 1.0000001 against a control's 1.0000009) —
⚠ **that is HEADROOM, NOT FREE.**

**`run_summary.speed_ratio` is the instrument.** If it rises on your machine, or capture hitches,
**the target mask is the FIRST thing to turn off**:

```
IAI.Capture.TargetMask 0
```

It takes effect **between runs**, writes no directory and adds no keys.

### The output-height refusal

If `IAI.Capture.OutputHeight` is non-zero the mask is **refused outright** and says so, because the mask
is view-rect sized while the written frame is resampled — and **a label mask must never be filtered**
(interpolation would invent values that identify no target). `mask_file` is `null` on every row and
`target_mask_frames_unavailable` equals the frame count. Set the output height to `0` to get masks.

### 🆕 `m45` — hidden-class masks, and the identity arbiter that gates them

For **missing object** and **blinking**, every labelled frame now carries the target's **would-be
silhouette** (occlusion-aware). The hide keeps custom depth while dropping the target from the main and
depth passes and silencing shadows, Lumen, distance fields, ray tracing and decals.

**COOK-TIME GATES (all of these, at the next cook):**

- [ ] **IDENTITY ARBITER — the new hide changes NOT ONE PIXEL.** Run the leg twice under
      `IAI.Bench.HideMode 0` and once under `1`, at the **AA-off configuration**
      (`r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0,
      r.ReflectionMethod 0`), **native tick order**. **Control must be 0 frames differing** — that is
      what makes the reading mean anything — **and the test must be 0.**
- [ ] 🚨 **ITS CAN-FAIL LEG, in the same session.** `IAI.Bench.HideOmitDepthPassSilencing 1` must make
      the identity comparison **FAIL** (bench: 20 of 60 frames, worst 4.69 %). *A gate that has only
      ever passed is not a gate.*
- [ ] **G7 becomes EQUALITY for hidden-class:** mask files == labelled frames exactly, stray 0.
- [ ] **ONSET 6/6** — every event, hidden-class included, first mask == first label.

⚠ **A LIMITATION, STATED RATHER THAN IMPLIED: identity is proven at the AA-off arbiter. At the
DELIVERED configuration a cross-run picture comparison has a nondeterminism floor (~9 % of pixels
between two runs of the SAME build), so NO pixel-identity claim is made there.** The AA-off arbiter
answers "does the hide change what renders", which is the question; it does not certify the delivered
temporal-AA frames.
⚠ **`IAI.Bench.SynthTickOrder` cannot host a pixel arbiter** — it is nondeterministic even at AA-off
(its own old-hide control differs on 60 of 60 frames). It hosts ALIGNMENT gates only.
### 🆕 `m46` — the mask pass maps through the internal view rect

The pass runs after tonemap (OUTPUT space) and samples scene textures at INTERNAL resolution. It now
maps `P_out → InternalRectMin + clamp(round(P_out × InternalSize / OutputSize), 0, InternalSize−1)`,
nearest and clamped. **At 100 % this is the identity; at any other ratio it is the fix.**

- [ ] 🆕 **MASK-PICTURE-PAIRING gains a 50 % leg.** Run the probe at
      `r.ScreenPercentage 50, r.DynamicRes.OperationMode 0`: **NEITHER 0 and PREVIOUS 0**, and the
      `M23 PASS` line must show `internalViewRect` **differing** from `viewRect` (otherwise the leg
      did not exercise the mapping at all and its pass is vacuous).
      *Bench: 35 of 35 CURRENT after; 0 of 26 CURRENT with 25 NEITHER before.*
- [ ] The 100 % leg must be **unchanged**.

⚠ **This change is a GLOBAL SHADER parameter-struct change: it needs a FULL COOK (`G129`).** A
code-only hot-swap against a stale container is fatal at engine init with
`parameter structure has changed without recompilation`.

## 🆕 `m48` — EXPOSURE-DIP MARKING, AND THE ONE SMOKE LEG THAT MUST RUN AT THE GAME'S OWN EXPOSURE

🚨 **EVERY BENCH LEG THIS PROJECT HAS EVER RUN FORCED AUTO-EXPOSURE OFF; THE DELIVERED BUILD RUNS IT
ON.** The plugin never touches exposure and the project ini sets no exposure key, so a delivered cook
runs the engine default `r.DefaultFeature.AutoExposure = 1` with `Min 0.03 < Max 8.0` — adaptive.
Measured on a matched packaged pair with only those two cvars differing: whole-frame mean
**102.5 → 77.9 (−24 %)**, spread **7.1 → 32.0 (4.5×)**, and the same anomaly's own target luminance
falling from a pinned 123.7–128.1 to 117.8 → 90.5 across one session.
⇒ **Every luminance figure in this project's history was taken at PINNED exposure and does not
describe the delivered configuration.**

**What `m48` ships:** a frame row gains **`exposure_dip: true`** — additive, emitted only when true —
when its whole-picture mean luminance falls more than **4.0 %** below the rolling mean of the
**previous 8 CAPTURED frames**; `run_summary.json` gains **`frames_exposure_dip`**. The threshold is
**DERIVED TWO-SIDED, not chosen**: every exposure-pinned leg's maximum drop is **≤ 2.39 %** and every
auto-exposure-ON leg reaches **7.27–9.01 %**.
⛔ **THE PLUGIN MUST NOT FORCE EXPOSURE — the dataset should look like the game.** Do not "fix" the
dip by pinning exposure in a delivered build; that would ship a dataset the client's own game never
produces.

### The cook-time smoke run gains ONE leg at the game's own exposure defaults

🔑 **All other gate legs stay exposure-pinned, deliberately — determinism is what makes them
comparable.** But a gate set that pins exposure everywhere is structurally blind to the delivered
configuration, so **exactly one leg runs with auto-exposure at the game's defaults** and three things
are read off it:

| read | expected |
|---|---|
| `run_summary.frames_exposure_dip` | **> 0**, and typically a handful clustered near session start. **A 0 here with auto-exposure proven live means the DETECTOR is broken, not that the game is steady** — that is a FAIL. |
| black-frame gate (`verify_capture.py --black-frame-gate`) | **BLACK FRAMES 0** |
| DARK FIRST FRAMES | **0** |

**Auto-exposure must be proven LIVE on that leg before its dip count is read**, and the black-frame
gate's own luminance line is the proof: a pinned leg spans ~7 units of whole-frame mean, an
auto-exposure-ON leg spans ~32. **A dip count read without that proof is a number, not a reading.**


## The label-vs-pixel gate (`m49`) — run it on the HOST's own smoke session

One command, on the machine that captured the session:

```
python host-tools/verify_capture.py --label-pixel-gate --dir <sessionDir>
```

It reads `annotation.json`, `labels.jsonl` and the frames, and for every event checks two things
against the pixels: that the **first labelled frame is the first frame whose picture changes**, and
that the **frame after `end_frame` is the first clean one**. One line per event —
`PASS`, `ONSET-SHIFT(±n)`, `END-SHIFT(±n)`, `NOT-VISIBLE` or `NOT-MEASURABLE(<why>)` — then a summary
and an exit code: **0** when nothing disagrees, **2** on any shift or `NOT-VISIBLE`, **3** when the
session is unreadable. `--report-only` prints the readings and always exits 0.

🚨 **`NOT-MEASURABLE` is NOT a pass.** It is an unread surface — a truncated final event, a baseline
with too few clean frames, a missing region — and the reason is printed on the event's own line.

⚠ **It ships with `measure_label_offset.py` beside it and imports it** (one implementation of the
region/baseline/threshold code, not two). If that file is missing the gate REFUSES rather than
running degraded. Masks are used as the region when the session has them; without them it runs in
**bbox-only mode**, which still reads edge shifts but cannot tell a sliver from an occluded target.

**Prove it can fail before trusting a green run:**
`python host-tools/verify_capture.py --label-pixel-gate --selftest` — seven synthetic cases covering
both edges in both directions plus a `NOT-VISIBLE` case; it must print `SELFTEST: OK`.

---

# 📣 THE CHANGELOG PARAGRAPH FOR THE NEXT DROP

**Paste this into the delivery note.** It is written for the client, in the client's terms, and it
covers everything that changed since the last bundle. ⚠ **The `m50` paragraph is marked as such and
must be REMOVED if `m50` is not in the build being shipped** — a changelog that describes a fix the
binary does not carry is worse than no changelog.

> **What changed in this build**
>
> **1. Label timing is fixed on every host.** Several separate causes could make a label sit one
> frame away from the pixels it describes. They are all addressed: the per-frame label is now sampled
> at the end of the game tick, so it describes what the renderer is about to draw whatever order the
> game's subsystems happen to tick in; the target mask now belongs to the event that owns the object,
> so an event's first labelled frame carries its own mask instead of the previous event's; the first
> frame of a material swap is no longer at the mercy of on-demand shader compilation; and a frame
> whose whole picture darkens because the game's auto-exposure is re-adapting is now marked rather
> than left to look like a defect.
>
> **2. Labels now say whether the anomaly could actually be SEEN, per frame.** Every frame of every
> anomaly carries a measured pixel count and a visible/not-visible verdict, derived from that frame's
> own render. `annotation.json`'s `affected_frames` therefore now means *"the frames the anomaly is
> visible on"*, and a new `injected_frames` carries *"the frames it was applied on"* — which is
> exactly what `affected_frames` meant before. **If your tooling read `affected_frames`, point it at
> `injected_frames` and nothing changes.**
>
> **3. The label format is versioned.** `annotation.json` carries a root key `label_schema` whose
> value is `2`. A file with no such key is the previous format. **Nothing was removed and nothing was
> renamed** — every field you already read is still there, with its previous type. The full field
> table and a v1→v2 changelog are in section 8 of the README.
>
> **4. Objects that could never show a visible anomaly are no longer targeted.** Objects made
> entirely of translucent materials are excluded when the anomaly picks its target.
>
> **4b. For a DISAPPEARING anomaly, the renderer itself now confirms the object is gone.** Alongside
> the pixel count in (2), every frame carries `target_drawn_pixels`: of the pixels where the object
> would be, how many the renderer actually drew it at. A correct `blinking` or `missing_object` frame
> reads a positive `target_pixels` with `target_drawn_pixels: 0` — *"it should be here, and the
> picture does not contain it"* — and that verdict comes from the frame's own render rather than from
> what the plugin asked for. `run_summary.json` carries `frames_drawn_unexpected`, which counts frames
> where the renderer still had the object's own depth in front despite the frame being labelled as
> hidden.
>
> ⚠ **Read this number in ONE direction only.** `target_drawn_pixels: 0` is strong evidence the object
> is absent from the picture. **A value above `0` is not evidence it is present** — the measurement
> comes from the renderer's depth buffer, and an object can leave its depth behind for a frame while
> the picture correctly does not contain it. **It therefore does not change any label**, and it is
> offered as a number you can audit rather than as a verdict.
>
> ⚠ **On some titles `frames_drawn_unexpected` will not be zero, and that is expected.** On one real
> game we measured it at a few tenths of a percent of `target_pixels` on a small number of hidden
> frames, while an independent check of the picture confirmed the object *was* hidden on exactly those
> frames; the likely cause is the silhouette's edge on a title using a temporal upsampler such as TSR,
> which we have not established. **A residual of a fraction of a percent is a property of your
> renderer, not a failed hide.** The reading worth investigating is the opposite extreme — the drawn
> count approaching the full `target_pixels` on a frame labelled for a disappearing anomaly — and even
> then the picture itself is the arbiter.
>
> **4c. A disappearing anomaly no longer trips the exposure warning by itself.** Removing a bright
> object darkens the whole picture, which used to be enough to set `exposure_dip` on the anomaly's own
> frames. The brightness comparison is now made a second time with the target's silhouette excluded,
> and the frame is marked only if both comparisons agree — which is true of the game's exposure
> adapting and false of an object being removed. Marked rows say which comparison applied, in
> `exposure_dip_scope`, and `run_summary.json` counts the marks this removed in
> `frames_exposure_dip_suppressed`.
>
> **5. You can check the labels against the pixels yourself.** One command, on the machine that
> captured the session:
> `python host-tools\verify_capture.py --label-pixel-gate --dir <sessionDir>`
> For every anomaly it checks that the first labelled frame is the first frame whose picture changes,
> and that the frame after the last labelled one is clean. It prints one line per anomaly and exits
> **0** when nothing disagrees, **2** when something does, **3** when the session cannot be read.
> Add `--report-only` to print the readings without a failing exit code.
> ⚠ **`NOT-MEASURABLE` on a line is neither a pass nor a failure** — it means that anomaly could not
> be judged (a truncated final event, too few clean frames to measure against), and the reason is
> printed on the line itself.

## Ruling 1 — host post-process materials that read custom depth: DETECTED AND NAMED, NO RULE

Ruled 2026-09-04 (session 075) after the `m49` A2 measurement. **Include this paragraph in the
delivery note only if the client asks about outline/highlight effects; it is otherwise owner-facing.**

The plugin identifies its anomaly targets by writing a value into the custom **stencil** buffer. A
host game whose own post-processing **reads** custom depth or custom stencil — an outline system, a
selection highlight, a post-process mask — is sharing that buffer with us, and the question was
whether tagging an object could make such an effect draw on it and change the delivered picture.

**What ships:** the census counts those materials and **names them** in `run_summary.json` →
`census_host_pp_customdepth_readers` and `census_host_pp_customdepth_reader_names`, so a delivered
session states whether the host has any. On a real host with an outline post-process, that read is
**non-zero and correctly names the material.**

**What does NOT ship: any refuse-or-warn rule.** It was measured on that host with a purpose-built
fixture (`IAI.Bench.CensusMaskDump`) giving a tagged-vs-untagged comparison on ordinary scene objects
with **no anomaly anywhere in the comparison** — 21 treatment pairs against 25 controls, pose-matched,
one reference silhouette used for both classes. The two profiles agree within 7 % at every radius
from the silhouette edge, and in **both** classes the ring around the object is *dimmer* than the
ambient picture. On an armed frame 14 objects are tagged at once including a sky sphere covering a
third of the screen, so an outline would move the whole picture — and the ambient measure is **lower**
on the treatment side. ⇒ **NO DETECTABLE EFFECT, so no rule.**

⛔ **That is a statement about the measurement, not a claim that such a material never draws on our
tags.** It was measured on one host with one outline material. **If a client's host shows an effect,
the census is the thing to turn off** — `IAI.Capture.Census 0`, between runs — and the reader names in
`run_summary.json` are how you find out whether that host has such a material at all.

## Ruling 3 — Nanite and other unmeasurable targets: ADMITTED AS UNMEASURED (`m50`)

✅ **`m50` IS BUILT AND MERGED (2026-09-04, `7ecdf5f`). This paragraph is shippable in any build at
or after that commit.** Verified A/B on two hosts: on the bench the same leg went `anomalies: []` →
**4 events delivered**; on a real Nanite-heavy game, **0 → 6 events**, all
`observability_measured: false`, `vetoed_events` **0**.
⚠ **Still remove it if you ship a build older than `m50`** — a changelog describing a fix the binary
does not carry is worse than no changelog.
📌 `run_summary.json` gains one key, **`unmeasurable_targets_admitted`**: how many events shipped
labelled-but-unmeasured rather than being deleted. **A zero is a reading, not a problem** — on a
project with Nanite disabled it is the correct answer.

**The problem, measured on a real Nanite-heavy game (session 074, `LG-9`):** on UE 5.1 a Nanite
primitive cannot write custom depth, so the measurement cannot see it. That was believed safe,
because an unmeasured target is admitted rather than deleted. **It is not safe when something ELSE in
the scene writes custom depth**, which is the ordinary case: the pass then runs, the Nanite target
contributes nothing to it, and the zero-pixel veto reads that as *"measured, drew nothing"* and
**deletes the event.** Measured: **87 % of that map's candidates were Nanite**, all six fired events
were vetoed, and a 90-frame capture with 43 positive frames delivered **`anomalies: []`** — no labels
at all, from a session that plainly contained anomalies.

**What `m50` changes:** a target that is *known* to be unmeasurable before the fire — Nanite today,
the same test the census already uses — is classified **NOT MEASURED at arm time** rather than being
allowed to reach a measured zero. Such an event is **admitted**, and its labels say exactly what is
known and nothing more:

| field | value | meaning |
| --- | --- | --- |
| `target_pixels` (per frame) | `-1` | no measurement exists |
| `observable` (per frame) | `null` | no claim either way |
| `observability_measured` | `false` | the visibility layer could not run for this event |
| `affected_frames` | == `injected_frames` | the honest fallback: what was applied |
| `bbox_source` | `"projected"` | the boxes come from bounds, not from measured pixels |
| `mask.provided` | `false` | unchanged meaning: **not measured**, never "drew nothing" |

🔑 **The reasoning, stated because it is a judgement and not a derivation: honest labels without pixel
evidence beat no labels at all.** A dataset of correctly-boxed anomalies marked *"visibility not
measured"* is usable; an empty `anomalies` array is not. **It is not a claim the anomaly was
visible** — `observability_measured: false` is the field that says so, and a consumer who needs
pixel-confirmed labels should filter on it.

⚠ **Scoped to UE 5.1.** A later engine with Nanite custom-depth support makes those targets measurable
and this path stops being reached for them. The classification is by *measurability*, not by "Nanite",
so it follows the engine rather than a hard-coded list.
