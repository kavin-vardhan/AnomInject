# Anomaly Injection — Setup & Usage Guide

This package lets you inject labeled visual anomalies into the game and capture them as a dataset: per-frame images, a video clip, and an `annotation.json` describing each anomaly. This guide takes you from a fresh machine to your first captured session.

There are three pieces that work together, delivered together under one folder:

1. **The game build** — the packaged game with the anomaly plugin.
2. **The dashboard** — a small web app to control capture (pick anomalies, start/stop, tune targeting).
3. **The host tools** — a Python script that turns captured frames into an MP4.

The delivery folder looks like this — the two launchers sit at the top:

```
<delivery folder>/
  README.md            this guide
  Setup.bat            run once, first
  Run.bat              starts everything (encoder + dashboard)
  dashboard/           the dashboard itself, plus the config.json it reads on startup
  host-tools/          the encoder and helper scripts, and the label-sync check (Step 7)
  (your game build)    see the Launch section below
```

## 1. Prerequisites

Install this once, before first setup:

* **Python** (3.10+) — runs the video encoder, the overlay inspector and the dashboard server. https://python.org — during install, tick **“Add Python to PATH”**.
* **Pillow** — one Python package, needed only by the **overlay inspector** (section 3, Step 5). Install it once by opening a command prompt and running:

  ```
  python -m pip install --upgrade Pillow
  ```

  If you skip this, everything else still works — captures, labels and video are unaffected. The overlay window will simply tell you Pillow is missing, print the exact install line for your Python, and close.

That's the only prerequisite. The dashboard **opens in your normal web browser** — there is nothing to install for it. You do **not** need Node.js, and you do **not** need to download ffmpeg yourself (`Setup.bat` fetches ffmpeg for you, or uses one already on your PATH).

*(Python 3.7 is the hard minimum for the small server that shows the dashboard; 3.10+ is recommended and is what the encoder is tested against.)*

## 2. First-time setup (once)

Double-click **`Setup.bat`** in the delivery folder and answer its prompts. It is a one-time configurator — it launches nothing. It will:

* **Find or download ffmpeg.** If ffmpeg is already on your PATH (or was downloaded by a previous run) it uses that. Otherwise it offers to download a build for you; say **Yes** and it fetches and unpacks it automatically. (Say No only if you prefer to install ffmpeg yourself and add it to PATH — then re-run `Setup.bat`.)
* **Find Python** on your PATH.
* **Ask where captures should be saved.** Enter any folder you like (Setup creates it, and any missing folders above it, if it doesn't exist yet), e.g. `D:\AnomalyCaptures`. This one folder is used by **both** the video encoder (it watches here for finished captures) **and** the dashboard (the game is told to write captures here), so the two can't drift apart. **You only enter this once** — nothing else ever has to be hand-edited.
* **Save your answers** to a small `config.bat` next to `Setup.bat` (and point the dashboard at that same captures folder), which the run scripts read automatically.

Re-run `Setup.bat` any time your paths change (new game build, moved captures folder) or the dashboard is updated.

**Token (connects the dashboard to the game).** The dashboard and the game share a token so only your dashboard can control your game. This is already configured in the build you received — you do not need to enter or paste anything. (For reference it lives in `dashboard\config.json`; if the dashboard ever reports that the token was rejected, that is the file to check. `Setup.bat` checks that the dashboard can actually read it and stops if it cannot.)

**Build configuration.** The plugin's modules are not built for Shipping (all five code modules are excluded by the plugin descriptor); capture with Development or Test. A Shipping build of your game therefore has no capture, no anomalies and no dashboard connection. Only the code modules are left out: the plugin descriptor (with its content and its WebSocket networking dependency) is still part of a Shipping build, and any reference your own game code makes to plugin classes must be made conditional by you.

## 3. Running a capture session

Do these in order each time you want to capture.

### Step 1 — Launch the game

<!-- LAUNCH: paste your build's game-launch steps here (e.g. RunServer.bat -> RunClient.bat). -->

> **⟨ LAUNCH — build-specific; fill this in ⟩**
>
> _Start the game and its in-game control server here, then continue to Step 2. The launch steps for your specific build are maintained separately and dropped into this section._

### Step 2 — Start capturing (encoder + dashboard)

Double-click **`Run.bat`**. It opens three windows:

* **Anomaly Watcher** — watches for finished captures and turns them into MP4s automatically.
* **Anomaly Overlay Inspector** — watches for finished captures and draws the labels onto copies of the frames, so you can see what each frame is labelled as (section 3, Step 5). It prints its progress as it works.
* **Anomaly Dashboard Server** — a small local server that hands the dashboard to your browser.

Your **default browser then opens automatically** at `http://127.0.0.1:5180/`. If it does not, or you close the tab, just open that address yourself — the server window stays running. Nothing is published to the internet: it listens only on your own machine.

`Run.bat` then prints a short **status check** — dashboard, watcher, and game server — so you can see at a glance whether anything is missing. If the game server line says *NOT RUNNING YET*, go back to Step 1 and start the game; the dashboard will connect on its own once it is up.

The dashboard connects to the game automatically — no token to enter. You should see a green “connected” dot and a live preview of the game. **Leave the three windows open while you capture; close them when you're done.** (Closing the browser tab is harmless — reopen the address to come back.)

### Working on two monitors, or without alt-tabbing

If you are running the game on one monitor and the dashboard on the other, you can start a capture **entirely from the game's own console** and never touch the dashboard:

```
IAI.Capture.Start "" png 0 120 blinking
```

The arguments are the same ones the dashboard sets for you — output folder (`""` = the default), image format, seed (`0` = pick one), frame count, and optionally an anomaly and a target object. This matters because **capture waits for the game window to have focus before it records its first frame**: if you start a capture from the dashboard and then click back to the game, the first moments are spent waiting rather than capturing. Starting from the game's console means the game already has focus, so recording begins immediately and you never do the click-across-and-back dance. The dashboard still shows the run and the watcher still encodes it.

### Step 3 — Capture

In the dashboard's **Capture dataset** panel:

* **Pick what to capture.** Use **Targeted** mode to capture one specific anomaly on one specific object (pick the anomaly from the list, then pick the object — either from the dropdown of on-screen objects or by clicking it in the live preview). Or use **Auto-pool** mode to capture a random mix from the checked anomaly list in the **Capture pool** panel.
* **Set the frame count** (how many frames to capture — default 120; leave blank to capture until you press Stop).
* Optionally change the **image format** (PNG is lossless and the default; JPEG makes smaller files).
* Press **Start capture**. Then **click into the game window** — capture deliberately waits for the game window to be in focus before it begins, so you won't record idle frames of you switching windows. Play/move as you like while it captures.
* The live preview freezes while capture runs — **this is normal** (the preview is paused so it can't slow the capture down). It resumes when the run ends.
* Capture stops automatically at the frame count (or press **Stop capture**).

### Step 4 — Get your results

Each capture creates a folder under the captures folder you set in `Setup.bat`:

```
session_<date-time>/
  Actual_Frames/        the captured frames (frame_00000.png, frame_00001.png, …)
  Video_Clip/           the MP4 (produced by the host tools)
  run_summary.json      a small technical summary of the run
  annotation.json       the anomaly labels for the session
  labels.jsonl          the same labels again, one line per frame
  annotated/            copies of the LABELLED frames with the labels drawn on — numbering
                        has gaps because frames with nothing to draw are skipped (see Step 5)
```

The MP4 appears a few seconds after capture finishes (as long as the Anomaly Watcher window from Step 2 is running).

### Step 5 — Check the labels by eye (the overlay inspector)

Some anomalies are genuinely hard to spot — a missing texture on rocks lying on the ground, for instance, can look perfectly normal until you know where to look. The **overlay inspector** exists for exactly that: it draws the capture's own bounding boxes onto **copies** of your frames so you can confirm what the dataset says about a frame instead of squinting at it.

You do not have to run anything. `Run.bat` starts it, and a few seconds after each capture finishes you will find an **`annotated/`** folder inside that session. Open any of them.

**`annotated/` holds only the frames that have something drawn on them.** On a typical session most frames carry no anomaly, and an annotated copy of such a frame would be an identical duplicate of the original — so those are skipped, which saves a large amount of disk and makes the pass much faster. Two consequences worth knowing:

* **The numbering has gaps, and that is normal.** You might see `frame_00003_annotated.png`, then `frame_00026_annotated.png`. **The gaps are frames with nothing to draw, not missing data** — every captured frame is still in `Actual_Frames/`, and `annotation.json` and `labels.jsonl` still describe all of them.
* **The number in the filename is always the original frame index.** `frame_00045.png` becomes `frame_00045_annotated.png`. Nothing is ever renumbered, so you can always line an overlay up against `annotation.json` by that number.

The overlay window tells you exactly what it did after each capture, in the form *"96 frame(s) had boxes, 96 image(s) written, out of 300 total frame(s)"*, so you never have to wonder whether something went missing.

**What the colours mean:**

* **RED box — the delivered ground truth.** This anomaly **is in `annotation.json`** for this frame. Red is your dataset.
* **AMBER box — a candidate that is not a label on this frame.** Amber is *not* a rejected label and *not* an error. Each amber box is tagged with which of the cases below it is.

**You will see a lot of amber, and that is expected.** Across 389 measured sessions, of all the amber boxes drawn:

| Amber category | Share | What it means |
| --- | --- | --- |
| **`OUTSIDE-SUBSET`** | **90.1 %** | **Expected, by design.** For anomalies that hide an object (blink, missing object), `annotation.json` lists only the frames where the object was actually **hidden**. The overlay additionally marks the rest of that anomaly's window — the lead-in frame and the “visible again” halves of a blink. The label is correct; these frames are simply ones where the object was on screen normally. This is the dominant amber category by a wide margin. 🆕 **Under schema v2 this category also picks up frames the anomaly was applied on but could not be SEEN on** (the object left the screen, something moved in front of it): they are in `injected_frames` but not in `affected_frames`, so they draw amber rather than red. **That is the observability layer working, and it means you will see slightly more amber than the share below — which was measured on v1 sessions.** |
| **`VETOED`** | **9.9 %** | An event the plugin **removed** from `annotation.json` because the object was measured to draw **no visible pixels** in that view. This is the invisible-anomaly cure doing its job: rather than ship a label pointing at nothing, the event is dropped. These boxes are useful — they show you **where** something was dropped, so you can judge whether dropping it was right. |
| **`NON-MANIFESTED`** | ~0 % | The anomaly was triggered but never reached the picture, so it carries no positive frames. Not seen in any measured session; the tool reports it if it ever occurs. |
| **`UNMATCHED`** | ~0 % | Unexpected. If you see this, it is worth telling us. |

So the short version: **red is what you were given; the overwhelming majority of amber is the normal shape of a hide-type anomaly's window, and the rest is the system correctly declining to label something invisible.** Nothing is being quietly discarded — every dropped event is visible to you as an amber box.

**Two things it never does:** it never changes a captured frame (every annotated image is a new file in `annotated/`), and it never changes a label. The labels come from the game engine and are the authority; this tool only reads them and draws what is already there.

If the overlay window says Pillow is missing, run the install line it prints (see section 1) and start `Run.bat` again. Nothing else is affected in the meantime.

### Step 6 — Read pixel consistency around the labels

The checker reports **where a change was observed near a labelled frame**. No output
certifies that a label is correct. Even an exact target mask identifies the pixels,
not the cause: lighting, occlusion or animation can change those same pixels.

```
python host-tools\verify_capture.py --dir <sessionFolder> --label-pixel-gate --report-only
python host-tools\verify_capture.py --all <folderOfSessions> --label-pixel-gate --report-only
```

`--all` proves the instrument with its self-test first, then saves each session's
complete reading under a unique name and prints a summary. Read the coverage and
individual observations as well as the last line.

**Every run has an onset and an end observation:**

- `TRANSITION k d=… tau=… label s delta=…`: the nearest available local peak
  was at frame `k`; `delta` is `k − s`. `d` is the fraction of region pixels
  with a change greater than the configured pixel threshold in any RGB channel.
- `NO-TRANSITION [a..b]`: every pair in this window was observed, and none qualified
  as a local peak. Above-threshold changes can exist without a local peak; the
  separate whole-span check must still pass before a run can report NO-TRACE.
- `UNASSESSABLE(reason)`: the observation was unavailable. The line
  says why: missing RGB or masks, an absent mask ID, too little baseline evidence,
  excessive regional change, an unsatisfiable threshold, an oversized region, or
  peaks already assigned to earlier edges. Missing observations are never evidence of absence.

There is **no verdict at an edge**. Runs are summarised as follows:

| Run outcome | What the reading means |
|---|---|
| `CONSISTENT` | Both transitions sit on the two labelled edges. **Consistent with the label; does not establish cause.** **24 of the 314 scored cells read CONSISTENT while carrying a known one-frame label shift — a CONSISTENT run does not confirm the label.** A coincident whole-region change adds a caveat on the run line and asks you to inspect the neighbouring frames. |
| `OFFSET-NOTE(…)` | Both edges were observed and at least one transition sits off its label. A possible label offset **or an unrelated change**: look at frames `k−1..k+1`. This does not fail the session. |
| `PARTIAL` | Only one edge was observable, or one zero-delta transition accompanies a `NO-TRANSITION` observation. It cannot become CONSISTENT. |
| `UNASSESSABLE` | Neither edge was observable, or two NO-TRANSITION observations could not support the whole-span check. The run prints the reason. |
| `READING` | A run has no delivered in-span masks and uses bounding boxes. The same observation lines are printed with `(bbox-only)`; it cannot become CONSISTENT or NO-TRACE. A partly masked run retains mask mode, so a missing boundary inside its span remains unassessable. |
| `NO-TRACE` | The complete run span **and both windows** had usable, resolved masks and RGB pairs, but no target change exceeded the noise floor. In-span masks must be actual; missing outside-span masks may be extrapolated as described below. This is the only failing outcome. |

**NO-TRACE means:** “no change above the noise floor (tau=…) on this target
across frames s..e; the label claims a visible change”. The whole run span and
both edge windows were checked. This is a thresholded absence: `tau` is a
**fraction of the pixels in the compared target region** (the union of the two
frame masks). For example, one changed pixel in a 2,500-pixel region gives
`d=0.0004`, below the default `tau` floor `0.004` (0.4%). That case remains
NO-TRACE. A pixel must also exceed the configured RGB-channel difference threshold.
A wrong NO-TRACE means a compared in-span or edge-window pair exceeded `tau`.

NO-TRACE is restricted to the producer's pixel-changing class IDs:
`missing_object`, `blink` (`blinking` injector alias), `missing_texture`,
`corrupted_texture`, `lod_popping`, and `lod_corruption`. These come from the
injectors' `GetId()` methods and the producer's ID/active-source mapping in
`Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`
(`MapAnomalyToClient`, `ResolveAnomalyActiveSource`). Hidden runs of
blink are represented as separate active runs. `time_dilation`,
`lighting_mismatch`, `camera_clipping` and unknown classes cannot produce NO-TRACE.
Class membership is a producer contract, not an independent proof of visible effect.

`stuck_low_mip` is a **conditional** member of that class: it can earn NO-TRACE only
on runs where the producer's own `stuck_mip.held` flag is true on **every** labelled
frame. A run with an unheld frame, or with the flag absent, is left UNASSESSABLE —
the verdict the class had before it was admitted. The flag is the producer's, so this
narrows the tool rather than trusting it.

**Coverage and boundary masks:** each pair uses the union of its two target
masks, including the clean pairs used for the baseline. **Actual target masks
always take precedence.** The producer may write masks only on labelled frames.
When a frame outside this run has no target mask, the checker uses the nearest
labelled boundary of the same run: onset before the span, last labelled frame
after it. The distance is bounded by `edge_window + N_BASE`, where this verifier's
`N_BASE` is 24, giving 28 frames with the default window.

A real per-frame mask is never replaced by an extrapolated one: extrapolation is
used only where the producer wrote no mask at all, and every line it affects says
so. Extrapolation assumes the boundary silhouette
still represents the target's region; it cannot establish target geometry outside
the delivered masks. Every affected observation shows its boundary source and
largest signed distance (including baseline use); structured output retains all
source/delta pairs. The run summarises the maximum distance used.

Missing masks **inside the span**, including a missing onset or last-labelled
mask in a partly masked run, remain unassessable. Corrupt masks and invalid
dimensions are errors, not permission to extrapolate. An explicit absent ID is
an error inside the span; outside it, a valid PNG containing only other targets
may trigger the bounded fallback. ID 0/unresolved may use a sole value, visibly
tagged, but cannot support NO-TRACE. NO-TRACE is permitted with extrapolated edge
pairs and explicitly says `edge pairs use extrapolated masks` when that occurs.

Search windows remain bounded by labelled frames, inclusively. A peak must exceed
`tau` and be at least 1.5 times the **smaller** immediately neighbouring pair's
change; neighbours outside the scanned window count as zero. Adjacent equal
on/off changes both qualify. A positive flat plateau has qualifying edges but
no interior peaks. For each target and mode, edges in labelled order take the
nearest peak that an earlier edge has not used. Equidistant candidates prefer
the larger change `d`, then the earlier frame if strengths are equal; the edge
line prints both tied strengths and the choice. Bbox-only runs do not constrain
masked runs. All peaks already used means UNASSESSABLE with those frame numbers.
Selected transitions list alternative peaks, and multi-peak windows also print
the run's onset/end assignments.

This walk ensures unique assignments but does not establish cause or guarantee
increasing assigned frames when regions admit different peaks. A decrease prints
`note: assigned frames out of label order (k1 > k2)` on the affected run. A missing
transition can make an earlier edge take a later run's peak. A delayed one-frame
label can assign its actual end transition to its onset and leave its end unread.
Inspect the disclosed candidates and frame neighbourhoods when an association is
in doubt. NO-TRANSITION means no qualifying peak, not no above-tau change.

Every transition also prints the change fraction in a ten-pixel ring around its
region. `note: whole-region change (lighting/camera?)` helps a person inspect the
frames. It is diagnostic only and never changes an outcome. A CONSISTENT run
with that diagnostic carries a lighting caveat.

**Regional motion is a reading.** `m_edge` measures regional change on the nearest
3–24 valid clean pairs; `tau` is learned separately for each edge. There is no
regional-motion refusal or derived cap. If any edge has `m_edge > 0.42`, its run
prints `caveat: high regional change m=… — readings under motion are less reliable`,
using the largest available edge median. The historical 0.42 marker does not
affect detection or eligibility. Lighting and motion caveats can appear together.
Unsatisfiable thresholds, whole-frame regions, too few baseline pairs and missing
or invalid RGB/mask/identity coverage still prevent an observation.

The header links [verifier-characterisation.md](verifier-characterisation.md):
per-session recovery tables, every wrong cell and competing peaks from a fixed
perturbation cohort. All planned keys remain in scored or unscored ledgers.
These are measured limitations; recovery does not certify label correctness.
The retired `--region-cap`/`--motion-cap` options are no longer accepted.

Events now use **run outcomes only**, taking the worst of NO-TRACE, OFFSET-NOTE,
PARTIAL, CONSISTENT in that order. UNASSESSABLE and READING runs contribute coverage
counts but cannot promote an event through their edge observations. With no
assessable run, the event is UNASSESSABLE, or READING if every run is bbox-only.
Every event prints its run counts; a separate session run-coverage line makes an
unread sibling run visible even when the event has a CONSISTENT run.

The session counts `CONSISTENT n (c with caveat)`, `OFFSET-NOTE`, `NO-TRACE`,
`PARTIAL`, `UNASSESSABLE` and `READING` events. The caveat count is the subset of
CONSISTENT events containing a caveated consistent run. Any NO-TRACE ends
`FAIL: NO-TRACE …`; otherwise the last line is **NO FAILURE FOUND** with coverage
counts. That wording does not certify labels or mean every edge was observed.
An all-bbox session also prints **UNREAD-BBOX-ONLY**.

Normal exit codes: `0` for no NO-TRACE, `2` for NO-TRACE, `3` when execution cannot
complete. `--report-only` suppresses only `2`; execution errors remain `3`, also
in a batch whose other sessions may have produced valid readings.

M3 `observable`, `target_pixels`, and bbox provenance are **the producer's own
recorded evidence**. The checker copies their provenance into its output; it does
not independently measure or confirm those fields. Use the overlay in step 5 and
the source frames for human review when this instrument is unassessable.

### Step 7 — The label-sync check (numbers only)

`host-tools\label_sync_check.py` measures, from the saved frames themselves, how many frames each label edge
is off from the picture: the first and the last frame on which the anomaly is visible, against the first and
the last frame the labels mark. **0 means in sync; `+` means the picture lags the label.** It reads sessions
without changing them and prints **numbers only** (no folder, object or frame names). It needs Python 3.8 or
newer and nothing else; Pillow, if installed, only makes it faster (both decoders give identical numbers).
**`host-tools\OFFICE-CHECK.md` is its one-page recipe** — the capture settings (PNG, native size, a still
camera, `IAI.Capture.Config 2 40 8 30 0`), the two commands and what to read back.

```
python host-tools\label_sync_check.py --selftest
python host-tools\label_sync_check.py <a session folder, or a folder of sessions> --out numbers.txt
```

- **Run the selftest first.** It builds known-answer sessions (exact labels, labels one frame early or late,
  an effect three frames late, a faint ghost frame, a moving camera, a missing mask, a stray transition flag)
  and checks every reading; its last line must say `OK`.
- **The release reading is transition-aware at 50 % of the effect** (§8.6a): a frame flagged `transition`
  is excused only in the direction its reason allows (an onset frame the anti-aliasing history has not caught
  up with; the first frame after an object returns; a `stuck_low_mip` tail that decays), and a `partial`
  frame counts when it shows part of the blur, or when its render record shows a texture held. Raw, 10 %
  and strict readings are printed beside it. Sessions from builds without the flags are read raw, and the
  header says so.
- **An interrupted event** (`effect_interrupted`, §8.6a — the game removed the effect partway through) is
  judged at its own label edges: the last labelled frame against the picture at the interruption, 0 frames off
  to pass. If the effect came back, each labelled run is judged separately, and the frames between them must
  not show the effect. `interrupted` on the READ BACK line counts such events; it is not a failure by itself,
  and an `effect_interrupted` flag is never read as an anti-aliasing flag, whatever other reasons the frame carries.
- **An event the kit cannot fully judge** (for example a frame image missing from the copied folder) reads
  `unjudged`, never a pass; `unjudged` on the READ BACK line counts them. Re-copy the session folder.
- **`nanite N`** counts events on which the object started drawing Nanite while the event ran
  (`nanite_unmaskable`, §8.6a): the plugin stopped labelling and removed the effect, so that end is not a timing
  claim and reads `censored`.
- **Known limit:** a one-frame interruption between two labelled runs, or a gap too short to measure, reads
  `censored`, not failed. A censored count is therefore not proof of correct timing.
- **It cannot judge** `camera_clipping` (no target mask; its label is a whole-frame proxy), moving-camera
  events, effects too faint to measure, or frames the capture never saves. Its wrong-object count is an
  upper bound (shadows and reflections trip it too).

## 4. The dashboard, control by control

### Top bar

* **connected / reconnecting** — the link to the game. If it says reconnecting, check the game is running and its control server is up (see the Launch section).
* **FPS** — the game's current frames per second on your machine. Useful when choosing a capture rate (section 6).
* **seed / active** — the random seed in use, and how many anomalies are active right now.
* **Revert all** — instantly removes every active anomaly and returns the game to normal. Safe to press at any time.
* **scoping / selector HUD / auto HUD** — internal debug overlays and options. Leave these unchecked for normal use.

### Seed (what it is)

The capture uses a **deterministic random seed** to decide which objects and anomalies get selected. **The same seed always produces the same run** (same picks, same order) — so leave **seed** on “auto” for fresh variety each run, or set a specific number to reproduce an earlier run exactly.

### Poll radius and coverage sliders (how many objects get anomalies)

Anomalies are applied to objects that are currently visible on screen. These two sliders control **which of those objects count as candidates** — so they directly decide how many, and which, objects can receive anomalies:

* **poll** (poll radius) — only objects within this distance of your character can be picked, shown in meters. Default is 18 m. Slide it **right** to also include objects further away; slide it **left** to restrict to nearby objects. All the way left (0) turns the distance limit **off** entirely — any distance qualifies.
* **coverage** — an object must take up at least this percentage of the screen to be picked. Default is 6%. **Lower** it to include smaller/farther objects; **raise** it so only large, close, clearly-visible objects are picked. At 0 the size filter is **off** — any visible object qualifies, however tiny.

**Play with these two.** If capture keeps choosing the same one or two objects — or reports nothing to target — increase the poll radius and/or lower the coverage until more objects qualify. If anomalies land on tiny background objects you can barely see, lower the radius and/or raise the coverage. The target dropdown in Targeted mode reflects the current candidate set, so it's an easy way to see the effect of your changes live.

### Capture dataset panel

* **Auto-pool / Targeted** — the mode toggle described in Step 3 above.
* **anomaly / target (on-screen)** — in Targeted mode, what to inject and on which object. The target list shows the current candidate objects (see the sliders above). You can also click an object in the live preview to select it.
* **captures folder** — pre-filled with the folder you chose in `Setup.bat` (the same folder the video encoder watches), so captures land exactly where the MP4s are made. Leave it as-is; only change it for a deliberate one-off to a different folder (the encoder won't see that run unless you re-run `Setup.bat` for that folder).
* **format** — PNG (lossless, bigger files) or JPEG (smaller files).
* **seed** — leave on “auto” unless you were asked to reproduce a specific run (see **Seed** above).
* **frames** — how many frames to capture before stopping automatically. Blank = run until you press Stop.

### Capture pool panel

The list of anomaly types Auto-pool mode draws from — check the ones you want in the mix. **now firing** below it shows which anomalies are live right now, on which objects, and for how much longer.

**Nine anomaly types are available. Four are enabled by default:**

| Anomaly | Default | |
| --- | --- | --- |
| `blinking` | **on** | an object disappears and reappears |
| `missing_texture` | **on** | an object's material is replaced with a checker pattern |
| `corrupted_texture` | **on** | an object's material is replaced with solid magenta |
| `lod_popping` | **on** | an object flips to a much lower-detail version of itself |
| `missing_object` | **off** | an object is hidden for the whole burst, with no reappearance inside it |
| `camera_clipping` | **off** | the camera's near-clip plane is pushed out, slicing away close geometry |
| `stuck_low_mip` | **off** | an object's textures stay stuck at a low-resolution level, so it looks blurry while everything around it stays sharp |
| `uv_corruption` | **off** | an object's textures are repeated many times across its surface, or cut into cells and shuffled (§8.8) |
| `normal_corruption` | **off** | an object's normal map is inverted or its green channel flipped, so its surface detail is lit from the wrong side (§8.8) |

The five unticked ones are **available, not disabled** — tick any of them whenever you want it in the mix.

**`uv_corruption` and `normal_corruption` are new in this delivery and off by default.** Read §8.8 before relying on them: they decline any object they cannot corrupt completely, a normal-map change can be nearly invisible under flat lighting, and a **Targeted** capture of either needs the console, because the dashboard does not send their `mode` yet.

**`camera_clipping` is available but off by default**, because in Auto-pool mode it is held for the **whole session** rather than for a few frames — so on a first-person game the player's hands and weapon are sliced away in *every frame* of that capture. That is correct behaviour and it is what the anomaly looks like, but it is disruptive as a default. **Tick it whenever you want it** — see the explainer video and the note in section 7 first.

**`stuck_low_mip` is available but off by default.** The default `auto` route uses the existing streaming hold for pure textures and a private copy when an eligible texture is shared. The private copy blurs only the target's admitted components; other objects continue to use the original texture. Explicit `hold` retains the shared-texture refusal. See §8.3aa for route selection and the different label policies. Read the per-route fire counts and refusal counts before relying on your content for volume.

### Live preview

A live view of the game. In Targeted mode, clicking an object in the preview selects it as the target. The preview intentionally freezes while a capture runs and resumes afterwards.

## 5. Useful in-game console commands

The game has a built-in command line called the **console**. To use it:

1. Click into the game window.
2. Press the **`** / **~** key (directly below Esc). A text line appears at the bottom of the screen.
3. Type a command and press **Enter**. Commands are not case-sensitive, and the console suggests completions as you type.
4. Press the tilde key again (or Esc) to close it.

Commands you may actually need:

* **`IAI.Server.Start`** — starts the in-game control server the dashboard connects to. How and when it runs in your build is covered in the Launch section (Step 1).
* **`IAI.Server.Status`** — shows whether the server is running (and on which port). Useful if the dashboard won't connect.
* **`IAI.Capture.Fps 30`** — sets the capture rate in frames per second (default 30, allowed 1–240). See section 6 for how to choose the number. Can't be changed while a capture is running — stop first.
* **`IAI.Capture.Status`** — shows the current capture settings and whether a run is active.
* **`IAI.RevertAll`** — removes every active anomaly (same as the dashboard's Revert all button).
* **`IAI.TexCorrupt.Census`** (or **`IAI.TexCorrupt.Census all`**) — counts how many objects the two texture-corruption anomalies could use right now, and why the others are declined: the first reason per object, and a `subs` line with its detail (`reason/detail:count`). **`IAI.TexCorrupt.Census allreasons`** checks every rule for every object instead of stopping at the first, and prints each reason with how many objects it blocks and how many it blocks **alone**, the notes that no longer block, and the most common combinations. Read-only; prints counts, no names (§8.8).
* **`stat FPS`** — shows the game's frames per second in the corner of the screen. Type it again to hide it. (The dashboard's top bar shows the same number.)

Console equivalents of the dashboard sliders, in case you ever need them: `IAI.SetPollRadius 1800` (poll radius — note this one is in **centimeters**, so 1800 = 18 m; 0 = off) and `IAI.SetMinScreenCoverage 6` (coverage percentage; 0 = off).

Everything else (starting/stopping capture, choosing anomalies) is easier from the dashboard, so those commands are the whole list.

**Unsupported while a capture runs:** applying or reverting an anomaly by hand (`IAI.Apply` / `IAI.Revert`) for an anomaly type the capture is already firing. The capture's labels for that type follow whichever object the anomaly is currently on, so a manual re-apply to another object can make the capture's running event read as installed on the wrong object. Use `IAI.RevertAll` or the dashboard's Revert all button, which end the capture's events cleanly.

## 6. Capture rate & video speed

Your machine renders the game at some frame rate ("native fps") — check it in the dashboard's **FPS** readout (or with `stat FPS`). The capture rate defaults to **30 fps** and is set with `IAI.Capture.Fps <n>` in the console (there is no dashboard control for it).

* If you capture at a rate your machine can sustain, the game plays normally while capturing, and capture takes the time you'd expect (e.g. 120 frames at 30 fps ≈ 4 seconds).
* If you set a rate **higher** than your machine can sustain, the game runs in **slow motion during capture** and the capture takes longer in real time. The captured frames, the labels, and the video are still correct — the video is automatically stamped at the true rate so it **plays back at natural speed** — and the dashboard shows a notice after the run ("couldn't hold N fps — video stamped at X fps"). It just makes capturing slower and clunkier to play.

**Recommendation:** keep the capture rate at or below your machine's native fps. The default of 30 is fine on most machines.

**Capture resolution.** We recommend capturing at **1920×1080** or **1280×720** — the launcher provided with this build already starts the game at one of those two, so normally there is nothing to set. Higher resolutions (e.g. 3200×2000) produce correctly-labeled data, but frames take longer to process, so captures run slower and the review video becomes choppier. Quick health check after any settings change: time a short capture with a stopwatch — a 120-frame run should take about 4 seconds. If it takes noticeably longer, drop the resolution.

**Tip — warm up first:** the very first run after launching the game is slower while it compiles shaders. Do one short throw-away capture first; your real captures will then be more consistent.

## 7. Troubleshooting

* **Dashboard won't connect** — make sure the game is running and its control server is up (see the Launch section, Step 2); check with `IAI.Server.Status` in the console, or read the status check `Run.bat` prints. The dashboard connects to `127.0.0.1:8077` on this machine only.
* **Dashboard says the token was rejected** — the game and the dashboard disagree about the shared token. Check `controlToken` in `dashboard\config.json` against the build you were given, then reload the page. (You can also paste a token straight into the connect screen for a one-off.)
* **The browser doesn't open, or the page won't load** — check the **Anomaly Dashboard Server** window opened by `Run.bat`. If it says the port is already in use, another copy is already running: use that tab instead. Otherwise open `http://127.0.0.1:5180/` yourself.
* **The page loads but is blank** — that usually means `dashboard\config.json` is missing or unreadable. Re-run `Setup.bat`; it writes the file and then verifies the dashboard can actually fetch it, and stops if it cannot.
* **Pressed Start but nothing is recording** — click into the game window. Capture waits for the game to have focus before its first frame (so it doesn't start on a timeout after ~30 seconds otherwise).
* **The live preview froze** — if a capture is running, that's intentional; it resumes when the run ends. If no capture is running, check the connection dot.
* **No MP4 appears** — make sure the **Anomaly Watcher** window (opened by `Run.bat`) is still open. The most common cause is a wrong captures folder or a missing ffmpeg: re-run `Setup.bat` to re-enter the captures path and (re)install ffmpeg, then restart `Run.bat`. The watcher prints a line for every session it encodes — and a clear message if it can't find ffmpeg. It will encode any sessions it missed once the paths are right.
* **Game is in slow motion while capturing / capture takes ages** — your capture rate is above what the machine sustains. Lower it (`IAI.Capture.Fps`, section 6) and warm up first. The already-captured videos are still fine.
* **Few or no objects to target** — widen the poll radius and/or lower the coverage slider (section 4).
* **An anomaly seems stuck on screen** — press **Revert all** in the dashboard (or run `IAI.RevertAll` in the console).
* **Nothing captures / commands not recognized** — confirm you're running the provided build (capture features are included in this build).

### ffmpeg didn't download

On locked-down corporate networks the automatic ffmpeg download can be blocked outright — even the revocation-check retry that `Setup.bat` performs. This only stops the **video** step: your captures still record fine (frames, labels, `annotation.json`), and they encode to MP4 later once ffmpeg is in place. To install it by hand:

1. On any machine with internet, download an ffmpeg build (either link — both are `.zip`):
   * https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl.zip
   * backup: https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-essentials.zip
2. Unzip it. Inside you'll find a `bin` folder containing `ffmpeg.exe`.
3. In the delivery folder, create a `host-tools\ffmpeg\` folder if it isn't already there, and copy the unzipped build into it — so that `ffmpeg.exe` ends up somewhere under `host-tools\ffmpeg\` (the build's own subfolder is fine, e.g. `host-tools\ffmpeg\ffmpeg-master-latest-win64-gpl\bin\ffmpeg.exe`).
4. Run **`Setup.bat`** again. It detects the ffmpeg you placed ("found from a previous setup"), finishes setup, and records the path. Then use `Run.bat` as usual.

(Alternative: if ffmpeg's `bin` folder is on your system PATH, `Setup.bat` finds it there too — no copying needed.) Once ffmpeg is in place and the watcher is running, it encodes any sessions it missed while ffmpeg was absent.

## 8. What's in a session — the full label reference

This section is the reference for everything the labels contain. It is written to be read straight
through once, and then used as a lookup table.

### 8.0 The files

* **`annotation.json`** — the session's ground truth: a `video` block and an `anomalies` array, one entry per anomaly **event**. This is the primary artifact.
* **`Actual_Frames/`** holds the source images, numbered from `frame_00000` in capture order — frame N in the folder is frame N in the video and frame N in the annotation's frame indices.
* **`Video_Clip/`** holds those frames encoded to MP4 at the fps recorded in `annotation.json`.
* **`run_summary.json`** is a small technical summary (frame counts, timing) — you can ignore it, but don't delete it before the MP4 appears; the encoder uses it to know the session is complete.
* **`change_evidence.jsonl`** measures how much each anomaly target's pixels actually changed on the first frames of each event, with the rest of the picture as a control. Measurements only, no verdict — see section 9.
* **`labels.jsonl`** is one line per captured frame carrying the same labels per frame, including each anomaly's bounding box. It is what the overlay inspector reads.
* **`target_mask/`** and **`mask_map.json`** — one 8-bit grayscale PNG per frame that has content, marking exactly which pixels belong to an anomaly target. Full description in the target-mask section further down.
* **`annotated/`** holds the overlay inspector's output — a copy of **each frame that has a label drawn on it**, keeping the original frame number in its filename. Frames with nothing to draw are skipped, so the numbering has gaps; that is normal and no data is missing. Nothing else reads this folder — the video is always built from `Actual_Frames/` — so it is there purely for you to look at, and deleting it changes nothing.

### 8.1 🆕 Schema version 2 — read this first if you already have a parser

`annotation.json` now carries a root key **`label_schema`**, and its value is **`2`**.

* **A file with `label_schema: 2` is a version-2 file.**
* **A file with no `label_schema` key at all is a version-1 file** — that is every session delivered
  before this drop. There is no `label_schema: 1`; its absence is the version.

**Branch on that key, not on the delivery date.** The v1 → v2 changelog is §8.5, and the one change
that can silently alter a number you were already reading is **`affected_frames`** — read that row
first.

### 8.2 `annotation.json` — every field

**Root**

| Field | Type | Since | Meaning |
| --- | --- | --- | --- |
| `label_schema` | int | **v2** | Always `2`. Absent in v1 files. |
| `session_id` | string | v1 | The session folder's name. |
| `video` | object | v1 | See below. |
| `anomalies` | array | v1 | One object per anomaly **event**. May be empty — see §8.4. |

**`video`**

| Field | Type | Since | Meaning |
| --- | --- | --- | --- |
| `path` | string | v1 | Where the MP4 is written. |
| `frames_dir` | string | v1 | Where the source frames are (`Actual_Frames`). |
| `resolution` | `[w, h]` | v1 | The size of the **written frames**, read from the first frame actually written — not from the window size. |
| `fps` | number | v1 | The frame rate the MP4 is stamped at. May be fractional if the machine could not sustain the requested rate; the video still plays at natural speed. |
| `target_fps` | number | v1 | The rate that was *requested* (`IAI.Capture.Fps`). |
| `total_frames` | int | v1 | How many frames were captured. |

**Each entry of `anomalies` — one anomaly event**

| Field | Type | Since | Meaning |
| --- | --- | --- | --- |
| `anomaly_type` | string | v1 | `blink`, `missing_texture`, `corrupted_texture`, `lod_popping`, `missing_object`, `camera_clipping`, `stuck_low_mip`, `uv_corruption`, `normal_corruption`. ⚠ The blinking anomaly is written **`blink`** here, while `labels.jsonl` calls it `blinking` (its `id`); every other anomaly uses the same name in both files. |
| `anomaly_subtype` | string | v1 | A finer label for the same event, e.g. `disappear_reappear` for `blinking`. For `uv_corruption` and `normal_corruption` it is the mode: `tile`, `scramble`, `invert` or `green_flip` (§8.8). |
| **`affected_frames`** | object | v1, **meaning changed in v2** | **The frames on which the anomaly is judged to be VISIBLE.** See §8.3. |
| **`injected_frames`** | object | **v2** | **The frames on which the anomaly was APPLIED**, whether or not it could be seen. Same shape as `affected_frames`. This is exactly what `affected_frames` meant in v1. |
| **`bbox_source`** | string | **v2** | `"drawn"` = the boxes for this event come from measured drawn pixels. `"projected"` = they come from the object's projected bounds only. |
| **`observable_frame_count`** | int | **v2** | How many injected frames were measured **and** judged visible. Equals `affected_frames.frame_count` whenever `observability_measured` is `true`. |
| **`unmeasured_frame_count`** | int | **v2** | How many injected frames carry **no** visibility measurement at all. |
| **`observability_measured`** | bool | **v2** | `true` = at least one injected frame was measured, so `affected_frames` is a real measurement. `false` = **none** were, so `affected_frames` falls back to `injected_frames`. See §8.4. |
| `manifested` | bool | v1 | `false` = the anomaly was triggered but no captured frame ever sampled it as active; such an event carries zero frames. |
| `coverage_ratio` | number | v1 | Mean fraction of the picture the target's projected box covered while the event ran (0–1). |
| `coverage_pct` | number | v1 | The target's screen coverage at selection time, as a percentage. **`-1` means "not recorded"**, not "zero". |
| `affected_objects` | object | v1 | `count`, `primary_index`, and `nodes[]`. |
| `affected_objects.nodes[]` | array | v1 | Per object: `name`, `path`, `global_position[3]`, `asset_name`, `component_class`, `bounds{origin[3], extent[3]}`. ⚠ `bounds` is the **whole actor's** bounding box and can be much larger than the drawn object; use the per-frame boxes in `labels.jsonl` for geometry. |
| `camera` | object | v1 | `path`, `global_position[3]`, `near`, `far`, `rotation[3]` (pitch, yaw, roll), `fov_deg`, `aspect` — sampled at the event's first captured frame. |
| `engine` | object | v1 | `ticks_msec`, `name`, `version`, `project`. |
| `mask` | object | v1 | `provided`: `true` = the target's drawn pixels **were measured** for this event; `false` = **no measurement exists**. 🚨 `false` never means "the target drew nothing". |
| `depth` | object | v1 | `provided` — always `false`; reserved. |

### 8.3 `affected_frames` and `injected_frames` — the two frame lists

Both objects have **exactly the same five fields**:

| Field | Type | Since | Meaning |
| --- | --- | --- | --- |
| `frame_indices` | `[int, …]` | v1 | **The authoritative list.** Sorted, 0-based, and it may have GAPS. |
| `start_frame` | int | v1 | The **first** entry of `frame_indices` (`0` when the list is empty). |
| `end_frame` | int | v1 | The **last** entry of `frame_indices` (`0` when the list is empty). |
| `frame_count` | int | v1 | **The NUMBER OF ENTRIES in `frame_indices`.** It is a count, **not** a span. |
| **`span_frame_count`** | int | **v2** | `end_frame − start_frame + 1` — the width of the window, i.e. what you would get by counting from first to last inclusive. `0` when the list is empty. |

🚨 **`frame_count` and `span_frame_count` differ whenever the list has gaps, and that is normal.**
For `frame_indices: [4, 5, 9, 10]` → `start_frame 4`, `end_frame 10`, **`frame_count 4`**,
**`span_frame_count 7`**. Four frames are affected; they lie inside a seven-frame window.
*(`span_frame_count` exists because v1 gave you no way to say "the window" without recomputing it,
and `frame_count` was being read as the span. Both numbers are now stated.)*

**The difference between the two lists:**

* **`injected_frames`** = *"the anomaly was applied on these frames."* It is what the engine did.
* **`affected_frames`** = *"the anomaly is judged VISIBLE on these frames."* It is a subset of
  `injected_frames` — never larger, and often equal.

They differ when the anomaly was applied but could not be seen: the object walked off the edge of the
screen mid-window, something moved in front of it, or the game re-applied its own material over ours.
Those frames stay in `injected_frames` and drop out of `affected_frames`.

✅ **If you want "frames that show the bug", use `affected_frames`.**
✅ **If you want "frames the bug was switched on for", use `injected_frames`.**
✅ **In v1 there was only one list and it meant `injected_frames`.**

### 8.3a `stuck_mip.*` — the blurry-texture anomaly's own evidence (new in m52)

This subsection describes the **hold route**. Shared textures normally use the proxy route described in §8.3aa.

These keys appear **only inside a `stuck_low_mip` anomaly entry**. A frame with no such
anomaly, and a dataset produced by a build without this anomaly, carries none of them, so
an existing parser is unaffected and `label_schema` stays **2**.

`stuck_low_mip` holds one or more of the target's textures at a LOW resident mip for the
event, so the object looks blurry while the rest of the scene stays sharp. The keys are the
**engine's own streaming facts for that frame**, not an inference:

| key | type | meaning |
| --- | --- | --- |
| `stuck_mip.textures` | array | **one row per held texture** — `name`, `baseline_mips`, `forced_mips`, `resident_mips`, `resident_mips_at_onset`, `co_affected_visible`, `held`. This is the complete per-frame fact; every scalar below is a summary of it |
| `stuck_mip.held` | bool | **the authoritative per-frame fact: is ANY held texture below its baseline right now** |
| `stuck_mip.held_all` | bool | are they ALL below baseline. `held` true with `held_all` false means the hold engaged on some textures and not others, and `stuck_mip.textures` says which |
| `stuck_mip.primary_resident_mips` | int | how many mip levels of the **primary** held texture are resident THIS FRAME |
| `stuck_mip.primary_baseline_mips` | int | how many were resident immediately before the anomaly was applied |
| `stuck_mip.full_mips` | int | the primary texture's total mip count |
| `stuck_mip.floor_mips` | int | the lowest count the engine permits for it (its non-streaming LODs) |
| `stuck_mip.forced_mips` | int | the count the anomaly asked the streamer to hold |
| `stuck_mip.top_resident_px` | int | the width in pixels of the largest resident mip this frame — the number that decides how blurry it looks |
| `stuck_mip.textures_armed` | int | how many of the target's textures the anomaly is holding |
| `stuck_mip.textures_held` | int | how many of them are measurably below baseline this frame |
| `stuck_mip.co_affected_visible` | int | other visible components sampling the primary held texture (0 under the shipped default) |
| `stuck_mip.onset_latency_frames` | int | captured frames between the anomaly being applied and the first frame it was measurably held. `-1` until the hold engages |
| `stuck_mip.forced_top_px` | int | the width in pixels of the largest mip the anomaly will hold this texture at — the *intended* blur, fixed at pick time and constant for the event |
| `stuck_mip.ratio_at_pick` | float | the target's longest on-screen side divided by `forced_top_px`, measured when the target was chosen. Higher means the object out-resolves the held mip by more. `-1` when the target's on-screen box could not be measured |
| `stuck_mip.texture` | string | the primary held texture's name |

⚠ **The `primary_*` scalars describe ONE texture; `held` is an ANY-of over all of them.**
That is why they are named `primary_`: on a target with several textures you can legitimately
see `held: true` while the primary's own two numbers are equal, because a different texture is
the one that dropped. **Read `stuck_mip.textures` when the two look like they disagree** — it
names every texture and says which ones held. Measured example from the bench: 6 textures
armed, 4 held at the target mip and 2 still at baseline, `held: true`, `held_all: false`.

🔑 **A frame is labelled for this anomaly only while the mip is actually down**, and **the
labelled window STARTS at the first such frame.** The hold is performed by the engine's own
texture streamer on its own schedule, so it takes a number of frames to engage after the
anomaly is applied — measured at up to 19 captured frames on the bench and 19 on a second
game. Those frames are still captured and are labelled NEGATIVE; they do not consume the
event's window, so an event gets its frames rather than a truncated set. An event whose hold
never engaged is written with `manifested: false`, empty `injected_frames` and empty
`affected_frames` — the dataset never claims a frame that did not change.

🔑 **The anomaly restores what it held, and VERIFIES it.** On revert the streaming bias is
cleared and the plugin then re-asserts the stream-in every frame until the engine's own
resident-mip count is back at the baseline it recorded. Measured: up to 7 frames on the bench
fixture and up to **75** on a second game. While a texture is still climbing back, any new
anomaly on a target that uses it is REFUSED and counted in
`run_summary.stuck_mip_refused_not_restored`, so a later event can never record the
still-depressed count as its own baseline.

⛔ **The object must be big enough on screen for the blur to show.** The deepest hold the engine
permits still leaves a mip of a certain width, and if the target's on-screen box is not at least
**8×** that width the target is REFUSED and counted in
`run_summary.stuck_mip_refused_too_small_for_ratio` — it produces no event at all, rather than a
positive label you cannot see. (`stuck_mip_refused_imperceptible` is the same test applied to an
explicitly requested mip depth that was simply too shallow; on an automatically chosen target it
stays 0.)
⚠ **This is a size test, not a visibility guarantee.** It compares the object's on-screen box
against the held mip's width and knows nothing about what the texture contains: a low-frequency
surface — flat paint, a soft normal map — can pass it comfortably and still look unchanged. The
per-frame `observable` field, which is measured from the rendered frame, remains the authoritative
answer to "did this show".

⛔ **Not applicable to virtual textures.** A target whose textures are virtual-textured is
refused and counted in `run_summary.stuck_mip_refused_virtual`; it produces no event.
✅ That guard is **proven**: on both test fixtures a bench probe found genuinely virtual-textured
assets at runtime, ran them through the shipped eligibility test, and confirmed they are refused —
so a `0` in that counter means no such texture was encountered, not that the check was never made.

⚠ **If the world shuts down while a hold is still being restored**, the streaming bias is cleared
on every held texture — nothing is left holding the mip down — but the plugin cannot read back a
confirmation, because nothing ticks after the world is gone. Those textures are counted in
`run_summary.stuck_mip_unverified_at_teardown` rather than in the restored total, so the weaker
evidence is visible as such. This is a property of shutdown, not a leak: the runtime state dies
with the world and no asset is modified on disk at any point.

⛔ **If the target actor is destroyed while the anomaly is live**, the anomaly reverts immediately
and **no frame after that is labelled for it** — counted in
`run_summary.stuck_mip_revert_on_destroy`. This matters because the anomaly holds a *texture*, not
the actor: without it the texture would stay blurry after the object was gone and frames would
carry a label for something no longer in the scene.

⚠ **Two known delivery limitations of `stuck_low_mip`.** Both are rare, and each is either counted or
named here rather than silent.

- **A texture parameter changed on a runtime material instance during the hold.** Suppose that while
  an event is holding a texture, your game sets that same texture as a parameter on a *dynamic
  material instance* used by another object. That object then turns blurry as well, and neither the
  label nor the mask names it. UE 5.1 sends no engine-wide notification for a material-parameter
  change, so the plugin cannot see this without changing the engine, which it never does.
  The plugin **does** catch the other routes: a new object that uses the texture, a streamed-in
  level, a material swap on an existing object, or an object given the texture while unregistered
  that registers later. In each case the hold is reverted at once, and the event's frames from that
  point carry `stuck_mip.contaminated = 1`.
- **Amortized texture streaming.** Your project may enable `r.Streaming.AmortizeCPUToGPUCopy` with
  `r.Streaming.MaxNumTexturesToStreamPerFrame` above 0. If so, a mip copy the engine queued during
  the hold can still run after the revert. The event then stays open for two full streaming cycles
  before it may close. That covers the engine's normal schedule but is not a guarantee. Each such
  revert is counted in `run_summary.stuck_mip_streamer_fence_incomplete`. With the engine defaults
  (both off) this does not apply.

`run_summary` carries the per-session totals: `stuck_mip_fires_applied`,
`stuck_mip_textures_held`, `stuck_mip_frames_held`, `stuck_mip_onset_preroll_max`,
`stuck_mip_hold_timeouts`, `stuck_mip_restore_timeout`, `stuck_mip_restore_frames_max`,
`stuck_mip_textures_awaiting_restore`, `stuck_mip_revert_on_destroy`,
`stuck_mip_unverified_at_teardown`, and the refusal counters `stuck_mip_refused_shared` /
`_not_streamable` / `_virtual` / `_imperceptible` / `_too_small_for_ratio` /
`_no_eligible_textures` / `_not_restored` / `_already_held`.

⚠ **A `stuck_low_mip` event's window is one frame shorter than the configured burst length.** The
window opens at the first frame the hold is measurably down, and the burst's last tick reverts in
the same tick it captures, so that final frame is correctly not held and not claimed. This is a
property of the capture schedule, not of this anomaly; the count is always in
`affected_frames.frame_count`.

⚠ `stuck_mip_onset_preroll_max` and `stuck_mip.onset_latency_frames` measure **adjacent but
different intervals** and will normally differ by one. The per-frame key counts every captured
frame from the one the anomaly was applied on; the run counter counts only the frames the
WINDOW skipped, which begins one frame later.

### 8.3aa Proxy blur: route selection and labels

`IAI.Anomaly.StuckMipRoute auto|hold|proxy` selects the route for future events. `auto` is the default: an event uses private copies when an eligible candidate is shared in the loaded world, otherwise it uses hold. `hold` always uses the original purity-checked streaming route; `proxy` always uses private copies. The route is fixed for the event. `IAI.Anomaly.StuckMipRoute default` clears the console override. To configure it in `DefaultGame.ini`:

```ini
[AnomalyInjector]
StuckMipRoute=auto
```

The proxy's largest mip is the source mip selected by the existing blur depth policy. Telemetry `k` is that absolute source mip index; `resident_drop` is the additional drop from the source's resident first mip. Bench identity uses zero additional drop, matching the original resident chain. It keeps the remaining resident mip chain and samples at the original UVs. Source textures are read only. Material instances bind the copies only on admitted target components; the mask, label box and pixel count cover those same components. Texture uniformity, copy size and memory limits use the texture-corruption machinery (§8.8). The existing automatic-selection size and perceptibility checks still apply. A target cannot carry this anomaly and UV/normal corruption simultaneously.

Proxy labels use **FireWindow** membership: a frame is labelled while an installed proxy renders on the target within its fire window. There is no streaming onset wait. A displaced binding or lost target is `effect_interrupted`. Temporal AA uses the existing transition policy; PIE also carries the existing `pie_end_settle` precaution. The hold route retains its render-residency labels and restore tracking.

| Field | Meaning |
| --- | --- |
| `annotation.json` event `stuck_mip_route` | `hold` or `proxy` for this event |
| frame entry `stuck_mip.route` | the same route |
| `stuck_mip.k` | the first copied source mip, including the source's resident offset; primary texture summary |
| proxy `stuck_mip.textures` | per-copy `k`, `resident_first_mip`, `resident_drop`, `baseline_mips`, `forced_mips`, `forced_top_px` and source name |
| proxy `stuck_mip.held` | an expected proxy binding remains installed; it does not claim a shared source was streamed down |
| proxy `stuck_mip.onset_latency_frames` | `0`: onset is the bind frame |
| `run_summary.stuck_mip_hold_fires`, `stuck_mip_proxy_fires` | applied events per route |
| `run_summary.stuck_mip_label_source` | `proxy_installed_fire_window` for proxy-only runs, `per_event_route` for mixed runs, the existing source for hold-only runs |

`IAI.TexCorrupt.Census allreasons` includes `id=stuck_low_mip` and a `routes` line with `hold_eligible`, `proxy_eligible`, `auto_hold` and `auto_proxy`. The first two describe each route independently and can overlap; the last two partition eligibility under auto. `tools/anomaly_refusal_counts.py` reads those counts, and `--route-check` checks their accounting. Census reports potential eligibility across its scope; on-screen size and current renderer state can still affect a fire.

Blurry remains **available but OFF by default**. This feature branch does not change delivery approval or enable it in the default pool.

Eligibility and fire counts do not measure visible blur strength. Validate the pictures with a suitable clean control: movement alone can produce a large pixel difference on an animated target. The native hold-route visual evidence remains under review; the private-copy fixture proof is recorded separately.

### 8.4 Visibility — `observable`, `target_pixels`, and the honest "we don't know"

Every anomaly entry in `labels.jsonl` now carries two extra numbers, and the event-level fields above
are simply those numbers summarised.

**`target_pixels`** (int, per frame, per anomaly) — how many pixels of that frame the anomaly's target
actually drew, front-most, occlusion-aware.

| Value | Meaning |
| --- | --- |
| `> 0` | measured; the target drew this many pixels |
| `0` | **measured, and it drew nothing** — fully hidden, occluded, or off-screen |
| **`-1`** | **NOT MEASURED.** No measurement exists for this frame. It is **not** zero. |

**`target_drawn_pixels`** (int, per frame, per anomaly) — of those pixels, how many the target was
**actually drawn into the picture** at. It is measured on the GPU in the same pass and on the same
frame as `target_pixels`, and it is always a subset of it.

| Value | Meaning |
| --- | --- |
| `> 0` | the target's own surface is what the renderer put on screen at this many pixels |
| `0` | **measured, and the target was not drawn there** — the silhouette says it would have been visible, and the picture does not contain it |
| **`-1`** | **NOT MEASURED**, exactly as for `target_pixels`. Not zero. |

🔑 **Why both numbers exist, in one line: `target_pixels` says *where the target would be*, and
`target_drawn_pixels` says *whether it is there*.** For a **disappearing** anomaly (`blinking`,
`missing_object`) a correct frame reads `target_pixels > 0` **and** `target_drawn_pixels: 0` — that
pairing is the renderer's own statement that the object is missing from the picture, rather than a
statement about what the plugin asked for. For every other anomaly type the object is still drawn, so
the two numbers are equal.

⚠ **It is a statement about geometry, not about appearance.** A target drawn with the wrong material
— which is what `missing_texture` and `corrupted_texture` do — is still *drawn*, and reads
`target_drawn_pixels == target_pixels`. Use it to check that a **hide** took effect, not to check
that a **texture swap** looks right.

🚨 **AND IT IS ASYMMETRIC — read it in one direction only.** `target_drawn_pixels: 0` is strong
evidence the object is **absent** from the picture. **`target_drawn_pixels > 0` is NOT evidence that
it is present.** The measurement is taken from the renderer's *depth* buffer, and an object can leave
its depth behind for a frame while the picture correctly does not contain it — we have measured
exactly that on our own bench. ⛔ **So it does NOT affect `observable`, and you should not use it to
overrule a label either.** It is a reading you can audit, not a verdict.

⚠ **EXPECT A SMALL NON-ZERO RESIDUAL ON SOME TITLES, AND DO NOT READ IT AS A BROKEN LABEL.** On one
real game we measured `target_drawn_pixels` reading **0.3–0.8 % of `target_pixels`** on a small
number of hidden frames (about 0.3 % of rows in that run), while an independent check of the picture
itself confirmed the object *was* hidden on exactly those frames. The likely cause is the edge of the
silhouette on a title that uses a temporal upsampler such as TSR — **a candidate explanation, which
we have not established.** ⛔ **A residual of a fraction of a percent is a property of the host's
renderer, not a failed hide.** The reading that would genuinely concern us is the opposite extreme —
`target_drawn_pixels` approaching `target_pixels` on a frame labelled for a disappearing anomaly —
and even that we would confirm against the picture before calling it a fault. `run_summary.json`
reports the per-run count as `frames_drawn_unexpected` so you can see it rather than guess at it;
**it is a diagnostic, and it never changes a label.**

**`observable`** (per frame, per anomaly) — the verdict `target_pixels` produces:

| Value | Meaning |
| --- | --- |
| `true` | the frame is labelled for this anomaly, the anomaly's visual condition still holds, and `target_pixels` is at or above the threshold |
| `false` | measured, and one of those three is not satisfied |
| **`null`** | **unmeasured** (`target_pixels` is `-1`). It carries no claim either way. |

🚨 **`false` and `null` are different facts and must not be merged.** `false` is a measurement whose
answer is "not visible"; `null` is the absence of a measurement.

**The threshold** is `run_summary.json` → **`observable_min_pixels`**, and it is **absolute pixels**,
not a percentage. It ships at **1**, meaning *"any drawn pixel at all counts as observable"*. Raise it
with `IAI.Capture.ObservableMinPixels <n>` in the console (between runs, not mid-run) if you want to
exclude slivers — e.g. at 1920×1080, `2074` pixels is one tenth of one percent of the picture.

**When nothing could be measured.** If **not one** injected frame of an event carried a measurement,
the event reports `observability_measured: false` and **`affected_frames` falls back to
`injected_frames`** — the v1 behaviour, unchanged. That is the honest reading, not a claim that the
anomaly was visible. The common causes are the target mask being off, and target geometry the
measurement cannot see (see the scope notes in the target-mask section).

**An event with an empty `affected_frames` STAYS IN THE FILE.** It is not deleted. `injected_frames`
still tells you what was applied, and `observable_frame_count` reads `0`.

📌 **`run_summary.json` also gains `observable_frames`** (how many frame-anomaly pairs across the whole
session were judged observable) and **`frames_condition_lost`** (how many labelled frames lost the
anomaly's visual condition — e.g. the game re-applied its own material).

📌 **And three more, all readings rather than settings:** `target_drawn_pixels_measured` (how many
anomaly rows carried a real drawn count), **`frames_drawn_unexpected`** (labelled frames of a
disappearing anomaly where the renderer still drew some of the target — a diagnostic that never changes a
label: it reads `0` on our bench, a small residual is expected on some titles as described above, and a value
approaching the number of labelled hidden frames is the reading to report to us), and
`frames_exposure_dip_suppressed` (see the exposure-dip section).

### 8.4.1 One `blinking` event = one burst, which may contain more than one flicker

**One fire is one record.** A `blinking` event covers a single burst of the anomaly, and inside that
burst the object typically hides, reappears, and hides again. **That is one entry in `anomalies`, not
two**, and `frame_indices` lists exactly the frames on which the object was hidden.

So `frame_indices: [4, 5, 9, 10]` is **one blinking event containing two hide/show cycles**: hidden on
4–5, visible on 6–8, hidden again on 9–10. Each run of consecutive indices is one flicker. If you need
per-flicker records, split `frame_indices` on the gaps — the gaps are meaningful and are not missing
data.

The same applies to `missing_object`, which hides for the whole burst and therefore usually yields a
single unbroken run.

### 8.5 v1 → v2 changelog

| Key | v1 meaning | v2 meaning | Additive? |
| --- | --- | --- | --- |
| `label_schema` (root) | *absent* | `2` — the version marker | **Added** |
| `label_schema_minor` (root) | *absent* | `1` — additive transition export (schema 2.1) | **Added** |
| `event_id` | *absent* | Stable join with the entry in `labels.jsonl`: anomaly id, engine start frame and target name. Separate fires on one target have separate ids. | **Added** |
| `transition_frames` | *absent* | Sorted, unique session frame indices with `transition: 1` for this event, including onset, detached post-label tails and every transition reason. These can overlap `injected_frames` or lie outside it. Drop them from training: never use as negatives, never as positives. | **Added** |
| `transition_reasons` | *absent* | Map from each reason to its count of emitted transition entries for this event. An entry can have several reasons; the counts need not sum to the frame count. | **Added** |
| `transition_frame_count` (root) | *absent* | Number of distinct session frames carrying at least one transition entry; overlapping events count once. | **Added** |
| `affected_frames` | the frames the anomaly was **applied** on | the frames the anomaly is judged **visible** on (a subset of `injected_frames`) | ⚠ **MEANING CHANGED** — same key, narrower set |
| `affected_frames.frame_indices` | applied frames | visible frames | ⚠ **MEANING CHANGED** |
| `affected_frames.start_frame` / `end_frame` | first / last **applied** frame | first / last **visible** frame | ⚠ **MEANING CHANGED** |
| `affected_frames.frame_count` | number of entries in `frame_indices` | number of entries in `frame_indices` | Unchanged (it was never a span) |
| `affected_frames.span_frame_count` | *absent* | `end_frame − start_frame + 1` | **Added** |
| `injected_frames` | *absent* | **exactly what `affected_frames` meant in v1** | **Added** |
| `observable_frame_count` | *absent* | count of injected frames measured **and** visible | **Added** |
| `unmeasured_frame_count` | *absent* | count of injected frames with no measurement | **Added** |
| `observability_measured` | *absent* | whether `affected_frames` is a measurement or a fallback | **Added** |
| `bbox_source` | *absent* | `"drawn"` or `"projected"` | **Added** |
| `labels.jsonl` → `target_pixels` | *absent* | measured drawn pixels, `-1` = unmeasured | **Added** |
| `labels.jsonl` → `observable` | *absent* | `true` / `false` / `null` | **Added** |
| `labels.jsonl` → `bbox_drawn_px` | *absent* | measured drawn box, or `null` | **Added** |
| `labels.jsonl` → `target_drawn_pixels` | *absent* | of `target_pixels`, how many the target was actually drawn at; `-1` = unmeasured | **Added** |
| `labels.jsonl` → `exposure_dip_scope` | *absent* | `"frame"` or `"frame_minus_targets"`, **only on rows that carry `exposure_dip`** | **Added** |
| `run_summary.json` → `observable_frames`, `frames_condition_lost`, `observable_min_pixels` | *absent* | see §8.4 | **Added** |
| `run_summary.json` → `target_drawn_pixels_measured`, `frames_drawn_unexpected`, `frames_exposure_dip_suppressed` | *absent* | see §8.4 and the exposure-dip section | **Added** |
| everything else | — | — | Unchanged |

🔑 **The one-line migration:** *if your v1 code read `affected_frames`, point it at `injected_frames`
and nothing changes.* Then adopt `affected_frames` when you want the visible subset.

⛔ **Nothing was removed and nothing was renamed.** Every v1 key is still present with its v1 type.

### 8.6 `labels.jsonl` — every field

One JSON object per line, one line per captured frame.

| Field | Type | Since | Meaning |
| --- | --- | --- | --- |
| `session_index` | int | v1 | **The frame number, and the only join key.** `session_index` N is `Actual_Frames/frame_000NN.png`, frame N of the video, and frame N in `annotation.json`'s frame indices. |
| `frame_index` | int | v1 | The game engine's own internal frame counter. **Never join on this** — see the ordering note below. |
| `t` | number | v1 | Game-clock seconds since the run started. |
| `t_wall` | number | v1 | Real-world seconds since the run started. |
| `image` | string | v1 | The frame's filename. |
| `width`, `height` | int | v1 | The written frame's size in pixels. |
| `anomaly_present` | bool | v1 | Whether any anomaly was active on this frame. 🆕 **For the `stuck_low_mip` hold route, only a frame that is labelled for it counts** (it is in the event's `annotation.json` frame list); its frames before the blur takes hold and after the texture is back carry no positive entry. `camera_clipping`, a whole-session anomaly, adds its entry only on the frames where it is positive (§8.6b). Every other anomaly keeps the v1 meaning (an event is running). A transition-only entry (§8.6a) never sets it. Per anomaly: §8.7. |
| **`transition_present`** | `true` | **new** | **Only present when true.** At least one entry on this frame carries `transition: 1` — see §8.6a. |
| `visible_positive` | bool | v1, 🆕 **changed** | `anomaly_present` **and** at least one entry that is **labelled on this frame** (`labelled: true`, i.e. the frame is in that event's `annotation.json` frame list) has a valid box. 🆕 Before this build it only needed an active entry with a box, so a `blinking` frame whose object was visible, or a `lod_popping` frame between pops, read `true`; it now reads `false` there (see "Rows where an event is active but not in the picture" below). ⚠ **Sessions delivered before this build use the old rule.** Tell them apart by the entries: a file whose anomaly entries carry a `labelled` key uses the new rule; a file with no `labelled` key anywhere uses the old one, and for it the per-frame truth comes from `annotation.json`'s frame lists (§8.7). |
| `anomalies` | array | v1 | One object per active anomaly — see below. |
| **`label_rule`** | string | **new** | **Only present on a row written by the single-shot console command `IAI.Capture.Shot`**, and then always `"legacy_shot"`. Such a row is not part of a capture run: it has no `labelled` key and its `visible_positive` keeps the **old** meaning (an active entry with a valid box), because a single shot has no render record and no transition history to apply the run rule with. Capture-run rows never carry this key. |
| `mask_file` | string \| null | since target masks | The mask PNG for this frame, or `null` if there is none. |
| `mask_state` | string | since target masks | `present` / `empty` / `unmeasured`. |
| `exposure_dip` | `true` | since exposure marking | **Only present when true.** The picture darkened more than 4% below its recent average, **and the same drop survives with every live target's silhouette excluded** — the game's auto-exposure adapting, not the anomaly's own effect. |
| **`exposure_dip_scope`** | string | **v2** | **Only present alongside `exposure_dip`.** `"frame_minus_targets"` if a target silhouette was excluded from the second comparison, `"frame"` if there was nothing to exclude. |
| `view` | object | v1 | `origin[3]`, `rot[3]`, `fovDeg`, `aspect`, `valid` — the camera for this frame. |
| `render_state`, `anomaly_materials_incomplete`, `shader_jobs_pending` | — | — | **Editor-build diagnostics; should never appear in a delivered session.** If you see them, tell us. ⚠ `render_state` has been seen once in a packaged capture as a false alarm — see section 9.9. |

**Each entry of `anomalies`**

| Field | Type | Since | Meaning |
| --- | --- | --- | --- |
| `id` | string | v1 | The anomaly type. It matches `anomaly_type` in `annotation.json` except for one name: `labels.jsonl`'s `blinking` is `annotation.json`'s `blink`. |
| `target_name` | string | v1 | The object it was applied to. |
| `start_frame` | int | v1 | The engine frame counter when this fire began — an event identifier, **not** a `session_index`. |
| `seconds_remaining` | number | v1 | How long the fire had left at this frame. |
| `bbox_valid` | bool | v1 | Whether the projected box could be computed for this frame. |
| `bbox_norm` | `[x0,y0,x1,y1]` | v1 | The projected box, normalised 0–1, as **corners**. |
| `bbox_px` | `[x,y,w,h]` | v1 | The projected box in pixels, as **origin + size**. Derived from the object's bounds, so it can be larger than what is drawn. From build 090-10 the bounds are **frozen at the end of the game tick that rendered this frame** and projected with that frame's camera, so the box (and `bbox_valid`, `bbox_norm` and the on-screen test behind `labelled`) describes where the object was in **this** picture, even if it moves or is destroyed before the frame is written. Earlier builds re-read the object when the frame was written, one or two frames later. |
| **`bbox_drawn_px`** | `[x,y,w,h]` \| null | **v2** | The box around the pixels the target **actually drew** on this frame, from the mask. `null` when nothing was drawn or nothing was measured. ⛔ **It does not replace `bbox_px`** — both ship. |
| **`target_pixels`** | int | **v2** | See §8.4. `-1` = unmeasured. |
| **`target_drawn_pixels`** | int | **v2** | See §8.4. The subset of `target_pixels` the target was actually drawn at. `-1` = unmeasured. |
| **`observable`** | bool \| null | **v2** | See §8.4. `null` = unmeasured. |
| `mask_value` | int | since target masks | This anomaly's pixel value in `target_mask/`. |
| **`transition`** | `1` | **new** | **Only present when set.** This frame is uncertain for this event — why is in `transition_reason`; see §8.6a. |
| **`transition_reason`** | array of string | **new** | **Only present with `transition`.** One or more of `temporal_aa`, `hide_return`, `partial`, `unresolved`, `camera_clipping_unconfirmed`, `effect_interrupted`, `nanite_unmaskable`, `capture_unpaired`, `pie_end_settle` — see §8.6a. |
| **`capture_unpaired`** | `true` | **090-10** | **Row-level, only present when set** (with a row-level `transition_reason: ["capture_unpaired"]`). The frame was captured by the **synchronous** path, so its picture is not paired with its label: drop it (§8.6a). Never present with the shipped settings. |
| **`labelled`** | bool | **new** | Whether this frame is in this event's `annotation.json` frame list (`injected_frames`) — the anomaly is in the picture on this frame by the label's own rule. It is written by the same rule, per anomaly, that builds that frame list (for `missing_texture` / `corrupted_texture`: the object's projected box is on screen **and at least one slot the anomaly replaced still renders its material on this frame** — see `effect_interrupted`, §8.6a; for `blinking` / `missing_object`: the object is hidden; for `lod_popping` / `camera_clipping`: the anomaly is in its anomalous state; for `stuck_low_mip`: the frame's render record shows the hold). `false` on an entry means the event is running but its effect is not applied on this frame, or the entry is a transition-only one (§8.6a). What "applied" means for each anomaly is in §8.7. Absent on files from earlier builds and on single shots taken with `IAI.Capture.Shot` (those rows carry `label_rule: "legacy_shot"`). A frame captured by the synchronous path (`capture_unpaired`) never has a labelled entry. `run_summary.json` names the rule as `label_labelled_rule: "annotation_membership_per_policy_v4_sample_bound"` (090-10: the on-screen test uses the bounds frozen for this picture, and synchronous frames are never labelled). Earlier names: `"…_v3_effect_rendered_nanite_gated"` (090-09: partial replacement stays labelled, Nanite-drawing frames are not), `"…_v2_effect_installed"` (090-05), `"annotation_membership_per_policy_v1"` (before 090-05, which did not check that the texture anomaly's material was still on the object). |

**Rows where an event is active but not in the picture.** `anomaly_present` means *an event is active on this
frame*; whether its effect is in the picture is `labelled` on the entry (and `visible_positive` on the row). These
row families are expected and are the ONLY ones:

| Anomaly | Which frames | `anomaly_present` | entry `labelled` | `visible_positive` | `observable` / `target_pixels` |
| --- | --- | --- | --- | --- | --- |
| `blinking` | the burst's visible phases (between and around its hidden runs) | `true` | `false` | `false` (unless another entry is labelled) | `null` / `-1` (no mask is armed on these frames) |
| `lod_popping` | the burst's un-forced phases (between pops) | `true` | `false` | `false` (unless another entry is labelled) | `null` / `-1` |
| `missing_texture`, `corrupted_texture` | burst frames on which the object's projected box is entirely off screen (`bbox_valid: false`) | `true` | `false` | `false` (unless another entry is labelled) | `null` / `-1` |
| any | the frame after an object returns, or a `stuck_low_mip` off-window frame | as the table in §8.6a says | `false` | as above | `null` / `-1` |

The reverse combination also exists and is correct: a hidden-object frame (`blinking`, `missing_object`) whose object
is hidden but whose projected box is off screen carries `labelled: true` with `bbox_valid: false`, so the frame is in
the event's frame list while `visible_positive` stays `false` for want of a box.

Any other `anomaly_present` row outside the event's frame list is a defect — tell us.

⚠ **The two box fields use different conventions on purpose and always have:** `bbox_norm` is
`[x0, y0, x1, y1]` (corners, 0–1); `bbox_px` and `bbox_drawn_px` are `[x, y, width, height]` (pixels).

### 8.6a `transition` — frames that temporal anti-aliasing may smear

Labels follow **what the game rendered on that exact frame**. With a **temporal** anti-aliasing method
(TAA or TSR), the picture you see is blended with previous frames, so a change can look half-done for a
frame or two after it starts, and linger for a few frames after it ends. We measured this on our bench:
with anti-aliasing off the labels and pixels agree exactly at both ends; with TAA the blur reaches its
midpoint about **2 frames** after a blurry-texture event starts, falls below half of its depth **4 to 8 frames**
after it ends, and a faint trace (5–45 % of the depth) can linger to **13–29 frames** after it ends. That is
why older builds flagged 16 frames. The 091-03 E1 measurement found a tail above 10% of the event's
full strength lasting up to 26 frames under TSR and 36 under TAA. The default is **40** post-label frames:
36 measured frames plus four frames of margin. The configuration cap remains 64. The unchanged half-strength
E1 edge check is the release gate; smaller residuals above clean-to-clean variation are diagnostics, not a
pixel-for-pixel clean guarantee. Final proof and its exact scope are in the 091-03 journal.

Rather than move a label off the exact render, those frames carry an extra key, **`transition: 1`**, on
the anomaly's entry, and the frame carries **`transition_present: true`**:

| Case | `transition_reason` | Which frames | The entry | Sets `anomaly_present`? | In positive frame lists? |
| --- | --- | --- | --- | --- | --- |
| `stuck_low_mip`, UV/normal and other fire-window events: start | `temporal_aa` | the event's first **3** labelled frames | the normal entry, plus `transition: 1` | yes | yes |
| `stuck_low_mip`, UV/normal and other fire-window events: end | `temporal_aa` | the **40** captured frames after its last labelled frame | a transition-only entry (`target_pixels` −1, `observable` null; the frame's target mask does not include it) | **no** | **no** |
| `blinking`, `missing_object` | `hide_return` | the **first** captured frame after the object reappears | `transition: 1` (inside a `blinking` burst this is on the burst's own entry; after the event ends it is a transition-only entry) | only if the event is still running, as before | no |
| `stuck_low_mip`, either edge | `partial` | every labelled frame whose **render record** shows the held textures only part of the way down — some at their held (blurry) level, others still sharp. **With or without anti-aliasing.** | the normal entry, plus `transition: 1` | yes | yes |
| `stuck_low_mip`, any labelled frame | `unresolved` | a labelled frame whose render record cannot say whether the whole set is held — a texture's level could not be read, its held level is unknown, or the frame has no texture record — and no texture it *can* read proves the set partial. **With or without anti-aliasing.** | the normal entry, plus `transition: 1` | yes | yes |
| `camera_clipping` | `camera_clipping_unconfirmed` | a labelled frame where the geometry inside the clipped slab could not be confirmed triangle by triangle (see §8.6b) | the normal entry, plus `transition: 1` | yes | yes |
| `missing_texture`, `corrupted_texture`, `uv_corruption`, `normal_corruption`, `stuck_low_mip` proxy | `effect_interrupted` | every captured frame of a running event on which **none** of the slots it replaced still renders its material (for UV, normal and proxy blur: our material with its private texture copies still bound), judged on the object as it renders now: the mesh component still registered and visible, the slot still present on its current mesh, and our material still the one that slot resolves to. Causes: reverted early (for example by a manual `IAI.Revert`), the game replaced the material (a damage flash, a material swap), hid the component or swapped its mesh, or the object was removed. If the game replaced only **some** of the slots, the frame stays labelled (the object still shows the effect) and is counted in `run_summary.label_effect_partial_frames`. Checked on every captured frame, at the same point as the label itself. **With or without anti-aliasing.** | a transition-only entry (`labelled: false`; no target mask is drawn for it) | **no** | **no** |
| any anomaly on an object | `nanite_unmaskable` | with `IAI.Targets.AllowNanite 0` (the default), every frame on which the object draws a Nanite part — one that appeared while the event ran (a component added or made visible, a mesh change). The event is reverted at the next tick. **With or without anti-aliasing.** | a transition-only entry (`labelled: false`; no target mask) | **no** | **no** |
| any anomaly | `capture_unpaired` | every frame captured by the **synchronous** path: `IAI.Capture.Async 0`, or, with `IAI.Capture.SVE 0` (the UI-on option), a frame on which the game-viewport rectangle could not be found. That path reads the picture the game presented **before** the current tick while the anomaly state is read **during** it, so a change made in that tick is in the label and not in the picture. **With or without anti-aliasing. Never with the shipped settings** (asynchronous capture on, scene-colour capture on). | a transition-only entry on every anomaly of the frame (`labelled: false`, no target mask), plus row-level `capture_unpaired: true`; the frame is excluded from positive lists and included in schema 2.1 `transition_frames` | **no** | **no** |
| `missing_texture`, `corrupted_texture`, `uv_corruption`, `normal_corruption`, proxy-route `stuck_low_mip` (every fire-window type) | `pie_end_settle` | **Play In Editor only:** the first captured frame after each labelled run of the event. It is a precaution, not a correction: an earlier reading suggested that in PIE the picture stayed changed one frame after the label ended, and that reading was traced to the checking tool, not the labels (it compared against `affected_frames`, which leaves out a labelled frame whose target mask was not measured; against `injected_frames` no event ended late). The flag only marks a frame that is already unlabelled, so keeping it costs one frame per event in PIE and nothing else. **Never in a packaged or staged game**; `run_summary.json` says whether the capture ran in PIE (`pie_end_settle_active`) and counts the frames (`pie_end_settle_frames`). | a transition-only entry (`labelled: false`; no target mask) | **no** | **no** |
| every other anomaly | — | none | — | — | — |

- **`partial` is not an anti-aliasing effect.** A blurry-texture event holds several textures of one object (for
  example its colour, normal and roughness maps). The engine lowers them one by one, and which one goes first
  varies. We measured the first two labelled frames showing between **6 % and 78 %** of the full blur depending
  only on that order. The label is right that the event has started; the flag says the picture shows only part of
  it. `run_summary.json` counts these frames per edge: `stuck_mip_partial_onset_frames`,
  `stuck_mip_partial_offset_frames`, `stuck_mip_partial_mid_frames`, plus `stuck_mip_partial_events` (each event's
  partial and unresolved frames as exact ranges, e.g. `onset=[46-50] offset=[953]`, with no limit on how many are
  listed). The per-frame flag on the entry itself is always complete.
- **Expect it at the start of `stuck_low_mip` events.** An event's **first 1 to 5 labelled frames** can be partial:
  some of the object's textures already blurred, others not yet. They carry `transition: 1` with reason `partial`,
  and **the order in which the textures blur varies** from event to event and from build to build, so how visible
  those frames look varies with it — a partial frame can look almost unchanged or almost fully blurred. The flag is
  read from each texture's own resident level on that frame, compared with the level the anomaly holds it at. The
  last labelled frame of an event can be partial too (one texture already restored). `partial` is written only when
  the record **proves** it: a texture read below its baseline beside a texture read short of its held level.
- **`unresolved` is the honest "cannot tell".** When a texture's level could not be read on a frame, or its held
  level is unknown, and nothing the record can read proves the set partial, the labelled frame carries
  `unresolved` instead. Such a frame is still labelled (the record shows the event holding), but it is never taken
  as evidence that the whole set was held, so it never moves the event's partial frames between its start, middle
  and end. `run_summary.json` counts them in `stuck_mip_unresolved_frames` / `stuck_mip_unresolved_events`.
  This includes a frame whose render record had not arrived when the capture's wait limit forced a decision
  (counted in `stuck_mip_forced_authority_frames`, entry key `stuck_mip.forced_unknown`): it is labelled, reads
  `stuck_mip.held_set: "unresolved"` and carries the `unresolved` reason. (Builds before 084-08 wrote it with no reason
  and `held_set: "not_held"`.)
- **One limit, in the source:** "the level the anomaly holds it at" is the plugin's prediction of where the
  engine's streamer will settle, so on a game whose streaming settings push a texture deeper than predicted, frames on
  the way down past the prediction are not flagged.

**What to do with each reason:**

| `transition_reason` | What the frame is | Training use (schema 2.1) |
| --- | --- | --- |
| `temporal_aa` | the anti-aliasing history may still show the previous state (onset) or a fading copy during the declared off window (40 frames by default) | drop the frame |
| `hide_return` | the first frame after a hidden object reappears; the history may still show it missing | drop the frame |
| `partial` | `stuck_low_mip` is applied to only part of the object's textures | drop the frame |
| `unresolved` | `stuck_low_mip` is applied, but the record cannot say whether to all of the object's textures | drop the frame |
| `camera_clipping_unconfirmed` | the clipping label rests on bounding boxes only (see §8.6b) and may be an over-label | drop the frame |
| `effect_interrupted` | the event is still running but none of the slots it replaced renders its material on this frame. The effect may be fully or partly gone (a fading trace under temporal anti-aliasing, or parts of the object we did not record) | drop the frame |
| `nanite_unmaskable` | the object now draws a Nanite part, so no mask can be made for it; the effect is reverted at the next tick | drop the frame |
| `capture_unpaired` | captured by the synchronous path: the picture may be one game tick older than the label | drop the frame |
| `pie_end_settle` | Play In Editor only: the frame right after a label ends, which may still show the effect | drop the frame |

A frame can carry more than one reason. Drop every transition frame from training: never use it as a positive or a negative.
All the transition rows above appear in schema 2.1 `transition_frames`, including rows outside the positive lists.

**The synchronous capture path is unsupported for delivery.** The shipped defaults (asynchronous capture and
scene-colour capture both on, no ini key needed) never reach it; it is reached only by `IAI.Capture.Async 0`, or by
`IAI.Capture.SVE 0` (the UI-on option) on a frame whose game-viewport rectangle cannot be found. Every frame it
writes is flagged `capture_unpaired` as above, so a capture taken that way delivers no labelled frames from that
path; `run_summary.json` counts them in `capture_unpaired_frames`, and the log prints one
`CAPTURE-UNPAIRED` warning per run. If that count is not 0 on a capture meant for delivery, the capture settings
were changed: restore them and capture again.
- **Capture-time temporal detection** (for `temporal_aa` and `hide_return`). Each scene-colour capture carries the rendered
  view's AA method and temporal-upscaler evidence. Engine defaults are also re-read during the run, including the mobile
  feature-level path. TAA, TSR, a registered temporal upscaler or missing/unknown evidence enables temporal flags.
  Once observed, temporal protection stays on to run end so changing AA cannot erase a pending tail. This can add
  exclusions after a method change. `label_temporal_source` records the evidence; `label_aa_method` and `label_temporal_aa`
  report the method and effective temporal state. A confirmed AA-off run has zero temporal reasons/windows; independent
  reasons such as `partial`, `capture_unpaired` and PIE-only `pie_end_settle` still export exactly.
- **Across captures:** if a capture stops while a transition is still owed, the next capture's first frames
  carry it (counted in `label_transition_tracks_carried_in` and `label_hide_tracks_carried_in`), and so do the
  captures after that while it is still owed — a short capture in between, or one that wrote no frames, passes it
  on. This treats the gap between captures as zero frames, which can only add flagged frames, never remove a label.
- **The numbers are 3 / 40 / 1** under temporal anti-aliasing and **0 / 0 / 0** without it (091-03 measured offset;
  earlier builds used 3 / 16 / 1 and 3 / 8 / 1). They are console variables —
  `IAI.Label.TransitionOnFrames`, `IAI.Label.TransitionOffFrames`, `IAI.Label.TransitionHideFrames`
  (any negative value = the default; a value of 0 or more replaces it, capped at 64; without temporal or unknown evidence all three
  are 0 whatever is set) — and each capture reports the values it used in
  `run_summary.json` (`label_transition_on_frames`, `_off_frames`, `_hide_frames`, plus the `_cvar` values as set).
  The 40 covers the maximum measured E1 t10 tail of 36 frames with four frames of margin. The previous
  16-frame default failed t10 coverage. This exclusion window does not add positive labels.
  The half-strength gate in §8.7a remains the release rule. Residuals below t10 may outlast the window;
  their measured lengths are reported separately. Always drop every declared transition frame.

- **How to use it:** drop the union of schema 2.1 `transition_frames` before assigning training positives or negatives (§8.7).
  The unchanged half-strength gate in §8.7a remains the release rule. The offset window also covers the measured
  t10 tail plus margin; the journal separately records smaller residuals above clean-to-clean variation.
- `run_summary.json` also counts `label_transition_entries`, `label_transition_frames` and
  `label_entries_suppressed` (entries withheld because they were not labelled on that frame), the entries per
  reason (`label_transition_temporal_aa_entries`, `_hide_return_entries`, `_partial_entries`,
  `_camera_clipping_unconfirmed_entries`, `_unresolved_entries`, `_effect_interrupted_entries`,
  `_nanite_unmaskable_entries`, `_capture_unpaired_entries`, `_pie_end_settle_entries`), `pie_end_settle_active` and
  `pie_end_settle_frames` (Play In Editor only, see the table above), `label_active_unlabelled_entries` (the rows of the
  table above), `label_effect_partial_frames` (labelled frames on which the game had replaced some, not all, of a
  texture event's slots), `nanite_midevent_reverts`, `refused_nanite_probe_missing` and `capture_unpaired_frames`
  (frames written by the synchronous path; also reported as `sync_frames_written`). The rule name is
  `label_labelled_rule: annotation_membership_per_policy_v4_sample_bound`, and `label_geometry_source:
  bounds_frozen_at_tick_end_sample` says where the label's box comes from (see `bbox_px`, §8).

### 8.6b `camera_clipping` — how a frame becomes positive

A frame is labelled `camera_clipping` only when rendered geometry lies in the **slab** between the normal near
plane and the pushed one. Object bounds are only the first check: a hollow room or a large rock around the
camera has bounds that contain the camera while its surfaces are far away. So for each object whose bounds
touch the slab, the plugin traces a grid of rays **inside the slab only** against that object's own triangles
(its collision mesh; for landscape, its height field). A hit means geometry is really there.

- `camera_clipping.clipped_ray_fraction` — the share of a 16×9 grid of view rays whose slab segment hit
  geometry: roughly how much of the picture is sliced.
- `camera_clipping.bounds_candidate`, `.confirm_traces`, `.confirm_hits`, `.confirm_misses`,
  `.confirm_unresolved` — the per-frame bookkeeping.
- **When triangles cannot be checked** — a skinned character (only rough physics shapes exist), an object with
  no collision, an object whose collision is simplified, more than 16 candidate objects / 320 traces in one
  frame, an object that fewer than 6 valid traces reached, an object **welded** to another (its collision body
  holds the other object's shapes too), or an instance whose collision body the engine places differently from how
  it draws it (a rotated instance inside a component scaled differently along different axes) — the frame stays
  labelled on the bounds check and
  carries `transition_reason: ["camera_clipping_unconfirmed"]` and `camera_clipping.unconfirmed: true`. It is never
  silently a guess: an object can be read as "not clipped" only from traces the engine actually ran.
- **Flat objects are traced along the whole slab.** A perfectly flat object (a single-plane mesh) has a box with no
  depth, so the part of a ray inside its box has no length and the engine would not trace it. Such a ray is traced
  along its whole segment through the slab instead, against that object's own triangles only, so a flat wall across
  the slab is found. (`camera_clipping_confirm_full_slab_fallback_traces` counts these.)
- **Instanced objects are selected as drawn.** Each instance's box is built from the same matrix product the renderer
  draws it with (instance × component), so a rotated instance inside a component scaled differently along different
  axes is no longer dropped before the traces.
- **What it can miss** (each of these reads as "not clipped", with no flag):
  - a sliver of geometry thinner than the ray grid that belongs to a large object (for example a thin pipe inside a
    room mesh) can fall between rays. Small objects get their own finer grid;
  - one-sided collision seen from behind: a trace that starts on the back side of a one-sided collision surface does
    not hit it.
- **Instances whose collision body differs from the drawn instance are not traced.** The engine places an instance's
  collision body with a simpler transform than the one it draws with; when the two differ (a rotated instance inside
  a component scaled differently along different axes), the body can sit away from the drawn instance, so such an
  instance is flagged `camera_clipping_unconfirmed` instead of traced (`camera_clipping_confirm_instance_transform_unconfirmable`).
- **Welded objects are not traced.** An object welded to another shares one collision body with it, so a trace could
  hit the other object's geometry; such a candidate is flagged `camera_clipping_unconfirmed` instead
  (`camera_clipping_confirm_welded_unconfirmable`).
- **What it traces is the collision mesh, not the drawn mesh.** Where an object's collision triangles differ from
  what is drawn, the label follows the collision triangles; the landscape is traced against its collision height
  field, which can be coarser than the drawn landscape. Translucent and masked surfaces are traced as solid.
- **Cost:** the confirmation's own arithmetic (the ray grid and the clipping) measured **16 µs per frame at 16
  candidate objects** outside the engine. The cost of the traces themselves has **not been measured in the engine
  yet**; the budget is 0.5 ms mean per labelled frame, and every run reports what it spent
  (`camera_clipping_confirm_us_mean_per_labelled_frame`, `camera_clipping_confirm_us_max`).
- `run_summary.json` reports `camera_clipping_label_rule: "view_slab_bounds_then_triangle_confirm_v4"`, the
  confirmed / unconfirmed / rejected frame counts, the per-cause unconfirmed counts (including
  `camera_clipping_confirm_too_few_valid_trace_candidates`, `_welded_unconfirmable` and
  `_instance_transform_unconfirmable`) and the cost.

### 8.7 The per-frame label, anomaly by anomaly — and how to build a training label from it

**First exclude settling and uncertain frames.** In schema 2.1, drop the union of all events' `transition_frames`
before assigning positives or negatives: **never use as negatives, never as positives**. This includes frames after
an event's last label, when the picture can still be settling. `transition_reasons` explains the entries counted;
`transition_frame_count` counts their session-wide union. These fields exactly mirror `transition: 1` in `labels.jsonl`,
joined by `event_id`, including non-AA reasons such as `partial`, `capture_unpaired` and `pie_end_settle`. An event that
only contributes exclusions can have empty positive frame lists. For an older annotation without these fields,
use the per-frame transition entries; absence of the new fields does not prove the picture has settled.

Temporal detection reads the AA method and registered temporal upscaler from the rendered view paired with each
captured frame. Defaults are also re-read during capture, including the engine's mobile/forward selection. Unknown
view evidence is treated as temporal. Once temporal history is detected it remains protected until that run ends,
even if the game changes AA settings. `run_summary.label_temporal_source` and the EFFECTIVE log line show the observed
view-method bit mask (bit 2 = TAA, bit 4 = TSR), upscaler, unknown evidence and default method. Confirmed AA-off runs
retain zero temporal windows; non-AA uncertainty flags remain independent. UV/normal and other fire-window types
now receive temporal onset and offset flags as well as the existing PIE settle flag.

Five fields answer five different questions about one frame. Keep them apart:

| Field | Level | The question it answers |
| --- | --- | --- |
| `anomaly_present` | row | Is an anomaly **event active** on this frame? (Its fire is running — for the `stuck_low_mip` hold route, its render-held window; proxy uses its fire window.) |
| `labelled` | entry | Is this frame in **this event's frame list** in `annotation.json` (`injected_frames`) — is the anomaly **applied in the picture** by the label's own rule? |
| `visible_positive` | row | `anomaly_present` and at least one `labelled` entry with a valid box. |
| `observable` / `target_pixels` | entry | On a labelled frame, did the target actually **draw pixels** in this frame's render? (`null` / `-1` = not measured.) |
| `transition` / `transition_reason` | entry | Is this frame **uncertain** for this event, and why (§8.6a)? |

**Anomaly by anomaly** (the exact rule the plugin applies; "burst" = the event's fire, from apply to revert):

| Anomaly | `anomaly_present` is true on | `labelled` (in the frame list) on | Active but not labelled | Box | `observable` measured on | Reasons that can appear |
| --- | --- | --- | --- | --- | --- | --- |
| `blinking` | every burst frame | the frames on which the object is **hidden**, as the plugin's own hidden state stands after every game system has ticked for that frame | the burst's visible phases | the object's projected box, also on hidden frames (where it would be) | labelled frames (the would-be silhouette) | `hide_return` |
| `missing_object` | every burst frame | the hidden frames — normally the whole burst | none expected | as `blinking` | labelled frames | `hide_return` (after the event) |
| `missing_texture`, `corrupted_texture`, `uv_corruption`, `normal_corruption` | every burst frame | burst frames on which the object's projected box is on screen **and** at least one slot the event replaced still renders its material (partial replacement stays labelled) | frames with the box off screen; `effect_interrupted` frames | projected box | labelled frames | `effect_interrupted`, `nanite_unmaskable` |
| `lod_popping` | every burst frame | the frames on which the forced LOD is applied | the un-forced phases between pops | projected box | labelled frames | none |
| `camera_clipping` | only the frames on which it is positive (§8.6b) — a whole-session anomaly whose entry appears only then | the same frames | none | the whole frame | never (`null` / `-1`) | `camera_clipping_unconfirmed` |
| `stuck_low_mip`, hold route | only the frames whose render record shows the hold (§8.3a); the frames before the blur takes hold carry **no entry at all** | the same frames | none; under temporal anti-aliasing the off window after the last held frame (40 frames by default) carries **transition-only** entries that do not set `anomaly_present` | projected box | labelled frames | `temporal_aa`, `partial`, `unresolved` |
| `stuck_low_mip`, proxy route | every burst frame | burst frames with an installed proxy rendering on the target (§8.3aa) | off-screen or `effect_interrupted` frames | admitted components only | labelled frames | `effect_interrupted`, `temporal_aa`, `pie_end_settle` (PIE only) |

`temporal_aa` and `hide_return` appear only under temporal anti-aliasing (TAA or TSR); `partial`, `unresolved` and
`camera_clipping_unconfirmed` appear with any anti-aliasing setting.

**Masks and recycled mask values.** On a labelled frame whose target was measured, the target's pixels in
`target_mask/` carry the entry's `mask_value`. There are 55 values (200–254), so a long capture **re-uses** them:
a value moves to a new event only when every value is taken, only from an event that has ended and whose every
frame, mask and measurement has been read back, and only after every object tagged with it has been reset and
checked (a value that fails the check is set aside, never re-used). So one value never marks two events on the
same frame — but it can mark different events on different frames. (One known limit, never observed: an object
that already carried a value before the capture can have that value written back to it when the plugin releases it;
if the game later turns its custom-depth flag back on it would show that value. The plugin now detects every such
write-back of a value that is in use — `run_summary.json` `mask_prior_collision`, expected 0 — and sets that value
aside for the rest of the capture so it is never handed to another event; `mask_prior_collision_quarantined`
counts the values set aside.) **Read the value together with the frame**:
use the `mask_value` on that frame's own `labels.jsonl` entry, or key `mask_map.json` by value **and**
`first_frame`–`last_frame`, never by the value alone.

**How to build a per-frame training label.** Key every row by `session_index` (never by `frame_index`, never by line
order). For each frame and each anomaly type, the frame is **positive** when it lies in the `injected_frames.frame_indices`
of an event of that type in `annotation.json` — the same thing as an entry of that type with `labelled: true`
on a file that carries `labelled`, with one exception: an event the plugin **removed** because its object was
measured to draw no pixels (counted in `run_summary.json` → `vetoed_events`) is absent from `annotation.json` while its
`labels.jsonl` entries keep their per-frame values, so **`annotation.json` decides**. For a file without `labelled` (an
earlier delivery) use the frame lists only, because its `visible_positive` also counted active-but-unapplied frames. If you want "visible in this frame" rather
than "applied in this frame", use `affected_frames` instead, which is the measured-visible subset whenever the event's
`observability_measured` is `true`. Every other frame is **negative** for that type, including the active-but-not-labelled
rows in the table above: the event is running but its effect is not in the picture. Take the box from the entry
(`bbox_drawn_px` when it is not `null`, else `bbox_px`; the whole frame for `camera_clipping`) and the pixels from
`target_mask/` via the entry's `mask_value`. Finally, handle uncertainty: drop every frame with
`transition_present`, using the per-reason guidance in §8.6a. For training, schema 2.1 requires dropping the
`transition_frames` union as described above, even where the event also has a positive label. Do not use `anomaly_present` on its own as a positive
label — it says an event is running, not that its effect is on screen.

### 8.7a How far each label is proven, anomaly by anomaly — and which flags to drop

**What "proven" means here — the release rule, exactly.** We captured each anomaly on our own machine and, from the
saved frames themselves, measured where each event's change in the picture crosses **half of its full strength** on
the way in and on the way out (the "half-strength edge"). An event passes when those two crossings fall on the label's
first and last frame — **0 frames off at both edges** — on every event judged. The anti-aliasing flags (`temporal_aa`,
`hide_return`) excuse the edge they flag; `partial`/`unresolved`/`camera_clipping_unconfirmed` frames are labelled
frames whose extent is uncertain; `effect_interrupted` frames are **never** excused — the label end is judged
against the picture during the interruption, and an interruption that still shows the effect fails the event;
a `nanite_unmaskable` end is not a timing claim and reads censored. A capture passes when every judgeable event passes. The tools that did
this were first shown to fail on labels deliberately moved by one frame, so a pass is not the tool being blind. A proof
says the label is **in step with the picture** by that rule; it does not say the anomaly is easy to see (that is
`observable`, §8.4).

What the rule does **not** establish, stated rather than implied:

- **It is not "every visible pixel".** With temporal anti-aliasing (TAA or TSR) an event's change is blended over a few
  frames: its first frames can be short of full strength, and after it ends a faint trace (5–45 % of the change) can
  last well past the half-strength edge. The rule judges the half-strength edge; it does not say the frames around it
  are pixel-identical to a clean or a fully changed picture.
- **The onset blind spot under temporal anti-aliasing.** On a texture-swap event the picture reaches half strength one
  or two frames into the label on some events. Frames inside the flagged transition window are excused by construction,
  so a label moved **one frame earlier** at the start of such an event is not always distinguishable from the blur by
  this gate. Our can-fail checks catch a one-frame shift of the whole label; they do not prove a one-frame error at a
  flagged onset would be caught.
- **Weak effects.** An event whose change is too faint to measure is reported as not measurable and is not judged.

"Our bench level" is a purpose-built test level of simple shapes; "our large test level" is the main level of the
sample game we develop on, with ordinary game scenery. "Natural" and "synthetic" order are the two orders in which the
engine can update the game and the plugin within one frame (a game can use either); the synthetic order is forced with a
bench switch. **This historical table predates 091-03. Every capture below ran at 1280×720, paced at 30 frames a second, on our own machine; "AA on" means the
engine's TSR (`r.AntiAliasingMethod 4`) and "AA off" means method 0. Plain TAA (method 2), other resolutions and the
office hosts were not part of that historical table.** The 091-03 journal separately records the new TAA/TSR captures and their resolutions. The table lists exactly the combinations that were run and passed — a combination
not listed was not run.

| Anomaly | Captures that passed (content · tick order · anti-aliasing) | For a strict training set, drop frames carrying |
| --- | --- | --- |
| `blinking` | large test level · natural and synthetic · AA on; large test level · natural · AA off | `hide_return` (the first frame after the object reappears; temporal AA only) |
| `missing_object` | large test level · natural and synthetic · AA on; large test level · natural · AA off | `hide_return` (temporal AA only) |
| `missing_texture` | large test level · natural and synthetic · AA on; bench level · natural and synthetic · AA on | `effect_interrupted` (none occurred in these captures; see §8.6a) |
| `corrupted_texture` | large test level · natural and synthetic · AA on | `effect_interrupted` (none occurred in these captures) |
| `stuck_low_mip` | large test level (one object with its own textures) · natural · AA on, targeted and in an Auto-pool run; the same object · natural · AA off | `partial` and `unresolved` (an event's first labelled frames), and `temporal_aa` (its first 3 labelled frames and the current **40** frames after its last one; temporal AA only) |
| `camera_clipping` | bench level with a scripted camera, judged on the **confirmed** frames · natural and synthetic · AA on; natural · AA off | `camera_clipping_unconfirmed` (over-labels by design — on our bench those frames showed no slice at all) |
| `lod_popping` | a purpose-built test object whose detail levels differ strongly, about 23 pops per capture · natural and synthetic · AA on and AA off | nothing |
| `uv_corruption` (`tile`, `scramble`) | a purpose-built test object (`TC_UC1`), one capture per mode · natural and synthetic · AA on; natural · AA off; and a second, natural AA-on capture per mode. Each capture: **16 complete events, plus 1 whose end runs past the end of the capture (its start is checked, its end cannot be)**. **Not yet checked on ordinary scenery** (§8.8) | `effect_interrupted` (none occurred in these captures) |
| `normal_corruption` (`invert`, `green_flip`) | a second purpose-built test object (`TC_NN1`), the same combinations and the same counts as `uv_corruption`. **Not yet checked on ordinary scenery** (§8.8) | `effect_interrupted` (none occurred in these captures) |

Not run for any anomaly: the synthetic order with AA off, except `lod_popping`. For `uv_corruption` and `normal_corruption` the synthetic order ran with AA on only, and the second capture was natural order only. For `missing_texture` and
`corrupted_texture` there is no AA-off capture on the large test level. Earlier builds were also checked on our bench
level in both orders with AA off (the `m49` edge gate); those captures are older than the builds above and are not
counted in this table.

What each row leaves open, stated rather than implied:

- **`stuck_low_mip` — the start is flagged, not early.** The engine lowers an object's textures one by one, so an
  event's first labelled frames can show only part of the blur. On our bench that was **2 frames per event** (3 in the
  capture built to recycle mask values, **up to 5** on the first event after the game starts), each flagged `partial`.
  The label is right that the event has begun; the flag says the picture shows only part of it. With temporal AA the
  blur **fell below half strength 4 to 6 frames** after the label's last frame on the bench, inside the then-current 16 flagged
  frames. The current t10 exclusion policy is wider (§8.6a); with AA off it fell below half strength on exactly the label's last
  frame.
- **`stuck_low_mip` — how often it fires depends on your content.** It holds only textures that exactly one object uses
  (the Capture pool panel, section 4), so on scenes that share textures it fires rarely, and it is **off in Auto-pool by
  default**. `run_summary.json` counts every refusal by reason.
- **`camera_clipping`** — a flagged unconfirmed frame is a frame where the plugin could not check the object's own
  triangles (§8.6b). On our large test level the rule labelled 2 frames of a 200-frame leg, both flagged, where the
  previous rule had labelled all 200 with nothing visibly clipped.
- **`lod_popping`** — on ordinary scenery (the rocks of our large test level) the pop is too faint to find in the
  pixels, so there the label could not be checked against the picture. The mechanism does not depend on the content:
  the detail level is forced on the object in the same frame the label marks, which is what the test object proves.
- **Masks:** long captures re-use mask values (the `m43` section below). In a capture on our large test level built to
  force it, two events held values at the same time and values were recycled 6 times; all 199 labelled frames had
  their mask, and no frame carried two events under one value.
- **Nanite objects are skipped by default.** On Unreal Engine 5.1 the pass that draws the mask cannot see objects
  rendered with Nanite (an engine limit). The plugin therefore refuses, before applying anything, any target that draws
  **at least one** visible Nanite part — counting every part the object draws, including one a target-name exclusion
  pattern hides from selection — with the reason `nanite_unmaskable`: in the Auto-pool (the object is dropped from the
  candidates before one is drawn), on a targeted fire, and on a direct `IAI.Apply`. The Auto-pool logs each refused
  object once per capture as `REFUSED-NANITE`; a targeted fire or `IAI.Apply` logs every refusal. `run_summary.json`
  counts distinct refused objects as `refused_nanite`, with the setting in effect in `nanite_target_policy`.
  **If a Nanite part appears while an event runs** (a component added or made visible, a mesh change), every frame from
  then on is unlabelled with `nanite_unmaskable` and the effect is reverted at the next tick (`nanite_midevent_reverts`).
  **If the Nanite check itself is unavailable** (the capture module is not loaded), setting `0` refuses every target as
  `nanite_probe_missing`, with an error line and `run_summary.refused_nanite_probe_missing`: the "skip Nanite" decision
  holds even then. A spline mesh using a Nanite-enabled asset is drawn without Nanite on 5.1 and is **not** refused.
  What this gives you: with target masks enabled (they need asynchronous capture at native output size; see the target-mask section), no
  labelled frame comes from an object known to draw Nanite. It does not make every mask complete — occlusion, faint
  effects and the mask's own scope limits still apply.
  **The trade-off: on a Nanite-heavy scene far fewer objects are eligible**, and a capture there can fire rarely or not
  at all. The setting is **`IAI.Targets.AllowNanite`** (console, `0` = skip, the default; `1` = allow; `default` clears
  the console value) or `AllowNaniteTargets=True` under `[AnomalyInjector]` in `DefaultGame.ini`; the console wins over
  the ini. **With it set to `1`, the previous admission returns** (the same objects are eligible and drawn in the same
  order; the rest of the capture keeps this version's rules): anomalies fire on Nanite objects and are
  labelled normally, with the box from the object's projected bounds (`bbox_source: "projected"`), but their entries
  read `target_pixels: -1` and `observable: null`, the event reads `observability_measured: false` (so
  `affected_frames` equals `injected_frames`), the object never appears in `target_mask/`, and `run_summary.json`
  counts such events in `unmeasurable_targets_admitted`. `camera_clipping` has no target and is not affected.
- **Not covered by any row:** a moving camera (other than `camera_clipping`'s scripted one), effects too faint to
  measure from the pixels, and a texture anomaly interrupted by the game — the `effect_interrupted` rule (§8.6a) is
  unit-tested; capture proof pending (no capture of an early revert, a partial replacement or a real game's material
  swap has been run on this version yet).
  Step 7's label-sync check (section 3) measures the same thing on your own sessions.

### Reading `labels.jsonl` — the rows are not in order

**If you parse `labels.jsonl`, read this first.** The file has one JSON object per line, one line per captured frame. Every frame is present exactly once — **but the lines are not written in frame order.** They are written in the order the capture's background writer finished them, which varies from run to run, and neighbouring frames routinely swap places.

What to do:

* **Key or sort by `session_index`.** That is the frame number: `session_index` N is `Actual_Frames/frame_000NN.png`, frame N of the video, and frame N of `annotation.json`'s frame indices. It is the only field that ties the three together.
* **Do not sort or join on `frame_index`.** That field is the game engine's own internal frame counter. It counts different things and starts from a different place, so it is **not** interchangeable with `session_index` — joining on it will silently mismatch rows. It is kept for engine-side diagnostics; a data consumer should ignore it.
* **Do not assume line N is frame N**, and do not assume the file is sorted even if a particular run happens to come out that way.

Nothing is missing and nothing is duplicated — it is purely an ordering property. `annotation.json` is unaffected: its frame indices are always in order.

### A note on `camera_clipping`

`camera_clipping` is **available but switched off by default** in the Capture pool panel — tick it when you want it. It is a **whole-session** anomaly: it applies to the camera for the entire capture rather than to one object for a few frames. That is why it is not on by default, and it is the consequence worth knowing in advance: anything permanently close to the camera — a first-person viewmodel, a held weapon — sits in front of the pushed near plane for the whole run, so it will appear sliced or partly missing in **every frame** of that session. **This is expected behaviour, not a defect**, and it is what the anomaly is meant to look like.

**Which frames are labelled.** A frame carries a `camera_clipping` entry only when something the game actually draws on screen has geometry between the game's normal near plane and the pushed one, **inside the camera's view** (§8.6b has the full rule). The entry is a whole-frame box (`bbox_norm` `[0, 0, 1, 1]`), always `labelled: true`, with `target_pixels` `-1` and `observable` `null` — there is no single object to measure. So:

- geometry **behind or beside** the camera does **not** label the frame, even when it is very close;
- objects that are drawn but hidden from the player's own view (a mesh the game hides from its owner) do **not** count, and nor do objects not drawn in the main pass;
- **particle effects** are not counted, so a particle sliced by the near plane does not label the frame (an under-label);
- a large **hollow** object that surrounds the camera — a room shell, a big rock — no longer labels a frame on its bounding box alone: its triangles are traced inside the slab, and if none are there the frame is **not** labelled;
- objects whose triangles **cannot** be traced — the player's own **skinned** meshes (hands, weapon, body), objects **without collision** (many props, foliage), objects with simplified collision only — still label the frame on their bounding box, and the frame carries `transition_reason: ["camera_clipping_unconfirmed"]` and `camera_clipping.unconfirmed: true` so you can filter it.

Every frame of a `camera_clipping` session also carries diagnostic keys in `labels.jsonl`: `camera_clipping.slab` (equals the label), `camera_clipping.bounds_candidate` (the bounding-box first pass), `camera_clipping.clipped_ray_fraction`, `camera_clipping.confirm_traces` / `_hits` / `_misses` / `_unresolved`, `camera_clipping.slab_primitives`, `camera_clipping.eye_inside_box`, and `camera_clipping.sphere_proxy`. `camera_clipping.sphere_proxy` is the oldest labelling rule, kept only for comparison — **do not train on it**. The triangle-confirmation rule has been checked frame by frame against the pictures on our bench (§8.7a): every **confirmed** labelled frame shows the slice, with anti-aliasing on and off, and the label starts and ends on exactly the frames the picture does. The **unconfirmed** frames on that bench — an object with no collision in the slab — showed **no** visible slice at all: they are over-labels by design, which is why they are flagged. **Drop frames carrying `camera_clipping_unconfirmed`.** On our large test level, where the old rule labelled all 200 frames of a leg with nothing visibly clipped, the new rule labelled 2 frames, both flagged unconfirmed.

### 8.8 Texture corruption — `uv_corruption` and `normal_corruption` (new in this delivery)

Two anomalies change how an object's own textures are read, without changing its shape. Each has **modes**;
this delivery ships two modes per anomaly.

| Anomaly | Mode | What it looks like |
| --- | --- | --- |
| `uv_corruption` | `tile` | The texture is repeated N×N times across the same surface (its texture coordinates are multiplied by N; N = 8 by default), so the pattern looks shrunk and repeated. |
| `uv_corruption` | `scramble` | The texture is cut into a K×K grid of cells (K = 8 by default) and the cells are shuffled: the cell at position *i* moves to position *a·i + b* (mod K²). *a* and *b* come from the capture's seed and a per-anomaly attempt counter, so the same seed gives the same shuffle. Each event's log line and its `labels.jsonl` entries record them. The no-shuffle pair (*a* = 1, *b* = 0) is never used. |
| `normal_corruption` | `invert` | The normal map's X and Y are both negated: bumps read as dents and grooves as ridges, so the lighting on surface detail comes from the wrong side. |
| `normal_corruption` | `green_flip` | Only the normal map's Y (the green channel) is negated — the classic DirectX/OpenGL normal-map mismatch: detail looks lit from the wrong side along one direction. |

`uv_corruption` applies the same change to every texture of the object's material (colour, normal, roughness and so
on; a 1×1 single-colour texture is left as it is), so they stay aligned with each other. `normal_corruption` changes the normal maps only.

**Not in this delivery:** the modes `drift` and `swap` (`uv_corruption`) and `flat` and `noise`
(`normal_corruption`) are deferred to the next delivery. They are recognised by name and refused
(`mode_invalid:not_in_delivery:<mode>`), never replaced by another mode.

**Choosing the mode.**

- **Auto-pool** picks the mode at random from that anomaly's enabled list — one random draw per attempt, from the
  capture's seed, so a seed reproduces the modes as well. Both modes are enabled by default. To narrow the list, use
  `IAI.Anomaly.TexCorruptUvModes tile` (or `scramble`, or `tile+scramble`) and `IAI.Anomaly.TexCorruptNormalModes
  invert` (or `green_flip`, or `invert+green_flip`). `none` empties a list; every Auto-pool attempt on that anomaly
  is then refused `mode_invalid:no_mode_enabled`. To leave an anomaly out of the mix, untick it in the Capture pool
  panel instead.
- **Everywhere else** the mode is the argument after the target: `IAI.Apply uv_corruption <object> tile`,
  `IAI.Apply normal_corruption <object> green_flip`. Mode names are not case-sensitive. The anomaly list the game
  sends the dashboard declares a required `mode` argument (defaults `tile` / `invert`).
- **A Targeted capture with no mode** — every Targeted capture started from the dashboard — takes the anomaly's
  enabled modes in turn, one per burst (`tile`, `scramble`, `tile`, … from the same lists as Auto-pool, starting again
  at the first with each capture). Nothing random is drawn for it, so a seed reproduces the rest of the run exactly as
  before; the game log names the source of each mode (`mode_source=round_robin`). With an empty list the burst is
  refused `mode_invalid:no_mode`. To fix one mode instead, start the targeted capture from the console with the mode
  after the object's name (the log then reads `mode_source=argument`):

  ```
  IAI.Capture.Start <captures folder> png "" "" uv_corruption <object name> tile
  ```

  Use your captures folder (the one you gave `Setup.bat`; in quotes if it contains a space) and the object name as
  the dashboard's target list shows it. The two `""` keep the automatic seed and capture until you press Stop (or type
  `IAI.Capture.Stop`).

**Settings.** Console first, then `DefaultGame.ini` `[AnomalyInjector]`, then the built-in default. Every capture
prints the values in effect at its start. A value outside the allowed range is refused, never clamped, and
`default` returns a command to the ini or built-in value.

| Console command | ini key | Default | Allowed | What it sets |
| --- | --- | --- | --- | --- |
| `IAI.Anomaly.TexCorruptTileN` | `TexCorruptTileNDefault` | 8 | 2, 4, 8, 16 | `tile`'s N |
| `IAI.Anomaly.TexCorruptScrambleK` | `TexCorruptScrambleKDefault` | 8 | 2–64 | `scramble`'s K |
| `IAI.Anomaly.TexCorruptUvModes` | `TexCorruptUvModesDefault` | `tile+scramble` | `+`-joined modes, or `none` | the modes Auto-pool may draw, and a Targeted capture without a mode takes in turn, for `uv_corruption` |
| `IAI.Anomaly.TexCorruptNormalModes` | `TexCorruptNormalModesDefault` | `invert+green_flip` | `+`-joined modes, or `none` | the same for `normal_corruption` |
| `IAI.Anomaly.TexCorruptMaxRtBytes` | `TexCorruptMaxRtBytesDefault` | 134217728 (128 MiB) | 1 MiB – 1 GiB | the memory cap for corrupted texture copies, both anomalies together |
| `IAI.Anomaly.TexCorruptMinTexturePx` | `TexCorruptMinTexturePxDefault` | 64 | 1–16384 | a slot needs at least one texture at least this many pixels on both sides |
| `IAI.Anomaly.TexCorruptMaxTextures` | `TexCorruptMaxTexturesDefault` | 8 | 1–64 | the most textures one slot may need |

The two mode lists drive the Auto-pool draw and the Targeted turn order; a mode named explicitly may be any delivered
mode of that anomaly.

**Whole components, and only the parts that changed are labelled (since 090-10c).** The event first selects the
target's mesh components — every static or skeletal mesh component of the object, or, when viewport scoping
(`IAI.SetViewportScoping 1`) is on, only those in view at that moment. An empty slot and a translucent slot are left
alone and **no longer count against the object**. A component is corrupted only if **every one of its other slots
qualifies** — a component is never partly corrupted, because the mask cannot separate the slots of one component. A
component with a slot that does not qualify is skipped whole. The event applies if at least one component is corrupted;
when some are skipped, **the mask, the measured pixels and the projected box cover only the corrupted components**
(the projected box `bbox_px` / `bbox_norm` only since 090-10f2: before it, those two covered the whole object while the
mask, `bbox_drawn_px` and the measured pixels were already limited — measured on a two-part bench object), and the
event record says so: `texcorrupt.components_corrupted`, `texcorrupt.components_skipped`,
`texcorrupt.slots_corrupted_list` (each corrupted `Component[slot]`), and `texcorrupt.slots_untouched` (every other
slot with its reason; a qualifying slot of a skipped component reads `component_skipped`). If no component can be
corrupted the event is refused `partial_footprint:<qualified>/<slots>:<reason>` (the first slot that blocks it) or, if
no slot qualifies at all, with that slot's own reason, and nothing is touched. Within a corrupted component every
texture its slots need is corrupted together, and everything is rolled back if any step fails. What this does not
promise: a mesh component that is out of view under viewport scoping, or one added or replaced after the event was
applied, is not part of the event; and changing a slot's textures does not guarantee that every pixel of it changes (a
nearly uniform texture, or a normal map under flat lighting, can look almost the same — see "When the picture barely
changes" below). The box and the mask show **which components** the event is on, not that every pixel inside them
changed.

**The texture as it is drawn (since 090-10c).** Real projects carry texture LOD bias — a texture's own LOD bias, a
texture-group bias from the device profile (for example a lower texture-quality setting), cinematic mips — and a
streamed world keeps most textures only partly resident. None of these refuse the event any more. The corrupted copy is
made from the mips the game is drawing at that moment: the copy's top level is the highest mip that is in memory (the
event record's `snapshot_mip` names that mip of the original, and `cooked_mip_count` the original's full count), so the
corrupted object looks as sharp as the original did. If the game later streams sharper mips in, the original would have
become sharper; the copy stays as it was and stays visibly corrupted until the event ends. A global `r.MipMapLODBias`
is applied to the copy as it is to the original.

**Memory (since 090-10c).** If an event's copies would exceed the memory cap, the largest copies are made at half size
(again, if needed) until they fit — never below 512 pixels on the longer side. Such a copy is a little blurrier than the
original as well as corrupted; the corruption stays plainly visible and the label is unchanged. `snapshot_mip` then names
the smaller mip it was made from. Only if the copies still do not fit at that floor is the event refused `over_budget`.

*Example from our large test level:* its floor object has 30 material slots. In an earlier measurement, 12 of them
(21 on another run) qualified at the moment of the decision, so under this rule it is refused `partial_footprint`.
When all 30 qualify it needs 219,541,488 bytes of texture copies, more than the 128 MiB cap, so it is refused
`over_budget`. Across the objects in that level's usual starting view, **none qualified at the 128 MiB default** —
`uv_corruption`: `over_budget` 2, `partial_footprint` 1, `texture_not_parameter` 2; `normal_corruption`:
`partial_footprint` 3, `texture_not_parameter` 2. Raising the cap to 256 MiB (`IAI.Anomaly.TexCorruptMaxRtBytes
268435456`) made two objects eligible for `uv_corruption` (each needing about 214–220 MB of copies); `normal_corruption`
had no eligible object there at any cap. So on scenes built from a few large, many-material objects, expect few events
at the default, and run `IAI.TexCorrupt.Census` (below) before relying on these two for volume. A raised cap costs that
much video memory while an event is live.

**Refusals.** A refused event records no fire, changes nothing and gets no label. The game log names each refusal
in full (`uv_corruption: REFUSED partial_footprint:12/30:… (step …)`). `run_summary.json` counts refusals by reason
name as `texcorrupt_refused_<reason>`; each key is present with 0 when that reason did not happen, except
`slot_empty` and `slot_translucent`, which appear only when they occur. In the order they are checked:

| Reason | Meaning |
| --- | --- |
| `assets_unavailable` | the plugin's own corruption materials are missing from the build — a packaging fault; every event is refused |
| `corruptor_not_ready` | those materials' shaders are not compiled yet (early in a session) |
| `dxt5_normal_host` | `normal_corruption` only: the project stores normal maps in the DXT5 layout, which this version does not handle |
| `runtime_lod_bias` | **no longer refuses (090-10c).** A mip bias (global, texture group, per texture, cinematic, streaming) is handled by copying the mips the texture is drawn with; it is reported by `IAI.TexCorrupt.Census allreasons` as a note. Measured in 090-10f2 on a packaged Windows build: a texture's own LOD bias and a texture-group LOD bias from the cooked device profile are applied by the cook (the biased top mips are simply not in the packaged texture), so a packaged game shows no runtime bias for them; cinematic mips stay a runtime property. In the editor (Play In Editor) all of them are runtime |
| `mode_invalid:<detail>` | the mode: `no_mode` (none given), `unknown:<text>`, `family:<mode>` (a mode of the other anomaly, including a deferred one), `not_in_delivery:<mode>` (a deferred mode of this anomaly), `no_mode_enabled` (Auto-pool with an empty mode list) |
| `no_mesh` | no static or skeletal mesh matched the target |
| `nanite_unmaskable` | the target draws at least one Nanite part, and Nanite targets are skipped by default (`IAI.Targets.AllowNanite 0`, §8.7a) because their events could not carry a mask. With the setting at `1` this step is skipped and the per-slot rules below decide |
| a per-slot reason | why a slot did not qualify: `slot_empty`, `slot_translucent`, `host_mid` (the game already drives that slot through its own runtime material instance), `nanite_override`, `shader_map_unavailable`, `shader_map_incomplete` (the material's shaders are still compiling; tried again later), `draw_shaders_missing` (the material has no shaders for that kind of mesh, so the game would draw the default material instead), `default_material_path` (the mesh needs a material usage the material does not have, so the game draws the default material), `no_textures`, `no_normal_map` / `normal_unconnected` (`normal_corruption`), a texture reason (`virtual_texture`, `unsupported_type`, `excluded_group`, `texture_not_parameter`, `unsupported_encoding`, `mip_chain_shape`, `held_by_stuck_low_mip`, `resource_not_ready`, `not_fully_resident:unmappable` — the engine's resident mips cannot be matched to the texture's own; `streaming_pending` and plain `not_fully_resident` no longer refuse), `below_size_policy` (judged on the texture's own size, not on what is resident), `map_set_over_cap`, `texture_uniform` (since 090-10f2: every texture the slot would corrupt is measured uniform for the mode being drawn, so the corruption could not be seen — `texture_uniform:no_spatial_variation` for `tile` / `scramble` (no channel of the texture's resident top mip varies by more than 2 of 255 steps), `texture_uniform:flat_normal` for `invert` (the normal map's X and Y are flat at 0.5 ± 2 steps), `texture_uniform:flat_normal_y` for `green_flip` (Y flat); `texture_uniform:pending` while the measurement has not come back and `texture_uniform:unmeasurable` if it failed — both refuse, an unmeasured texture is never assumed to vary; a slot is admitted as soon as one of its textures is measured to vary). It is the event's reason when no slot qualifies, and the `<reason>` of `partial_footprint` when no component qualifies whole |
| `no_eligible_slot` | the target has no slot to judge |
| `partial_footprint` | some slots qualify, but no component has all of its slots qualifying (above) |
| `over_budget` | the texture copies would exceed the memory cap (128 MiB by default, both anomalies together) even at the 512-pixel floor (above) |
| `rt_alloc_failed`, `draw_precondition_failed`, `param_readback_mismatch` | a failure while applying. Everything already taken is released and nothing is left changed (counted in `texcorrupt_rollback_*`) |

**Labels.** Both are labelled like `missing_texture` and `corrupted_texture` (§8.7): `anomaly_present` on every frame
from apply to revert, and `labelled` (in `injected_frames`) on those frames where the object's projected box is on
screen **and** at least one corrupted slot still renders our corrupted material with its texture copies bound.
`anomaly_type` is `uv_corruption` / `normal_corruption` and `anomaly_subtype` is the mode. Like the other texture
types they can carry `effect_interrupted` (no corrupted slot renders our material on that frame: drop those frames,
never use them as negatives) and `nanite_unmaskable` (§8.6a); a frame on which the game replaced only some slots stays
labelled and is counted in `label_effect_partial_frames`. Application is all-or-nothing over the **selected** meshes
(this section's qualification lists which meshes are selected), so every selected slot starts corrupted; the target
mask covers the object and identifies it — it is not proof that every pixel changed.
`observable` and `target_pixels` say whether the object drew pixels on that frame with the corrupted material still
rendering on at least one slot; they do not say how much its appearance changed (see the next paragraph).
`texcorrupt.condition_held` is stricter telemetry: true only while **every** corrupted slot still holds our material
with all its bindings, so a partial frame is labelled with `texcorrupt.condition_held: false`. Each
`labels.jsonl` entry also carries `texcorrupt.*` keys: `texcorrupt.mode`, `texcorrupt.slots_corrupted` /
`texcorrupt.slots_total`, `texcorrupt.condition_held`, and `texcorrupt.tile` for `tile` or `texcorrupt.scramble_cells`
/ `_a` / `_b` for `scramble`. `run_summary.json` adds `texcorrupt_fires_applied` and `texcorrupt_rt_bytes_peak` (the
most memory the copies used at once).

**How far these labels are proven** (the method is in §8.7a):

- **Label in step with the picture: proven on two purpose-built test objects** (`TC_UC1` for `tile` and `scramble`,
  `TC_NN1` for `invert` and `green_flip`), by the release rule of §8.7a. Per mode, one capture each in natural order with
  AA on, natural order with AA off and synthetic order with AA on, plus a second natural AA-on capture. In every capture,
  **16 complete events start and end on the labelled frames (0 frames off at the half-strength edge), and a 17th starts
  on its labelled frame but ends after the capture stops, so its end could not be checked**. Deliberately delayed apply
  and restore were caught at the size we set. Not run: synthetic order with AA off; other resolutions; the office hosts.
- **The right mode is applied: proven on separate test objects, all four modes** — `TC_OracleUV16` and `TC_OracleUV64`
  for `tile` and `scramble`, which match the expected pattern on every judged event (the logged `scramble` pair
  recomputed on each), and `TC_OracleN` for `invert` and `green_flip`, read with lighting off, on every judged cell of
  the surface; every wrong mode fails the same check. These are not the objects the timing was measured on.
- **Never in the same frame as `stuck_low_mip`: 0 frames carrying both, in every capture** (the rule is below).
- **On ordinary game scenery: not checked from pixels yet.** On our large test level the only objects that could be
  framed and qualified are rendered with Nanite, and on Unreal Engine 5.1 Nanite objects get no mask (§8.7a), so the
  label could not be checked against the picture there — and with Nanite targets skipped by default (§8.7a) those
  objects are now refused `nanite_unmaskable` instead. Those captures ran with Nanite allowed (the previous behaviour). Their events were labelled exactly as the unmeasured contract
  says, and no change was seen on any other object. The mechanism is the same material swap, in the same frame, as
  `corrupted_texture`, whose label is proven on that level.

⚠ **When the picture barely changes — read this before training on these two.** An inverted or green-flipped
normal map changes the shading only where light falls across the surface at an angle. Under flat, head-on or back
lighting the picture barely changes, yet the frames are labelled: a false positive. `tile` and `scramble` on a nearly
uniform texture (a flat colour, a faint noise) behave the same way. `observable` does not catch this. How often it
happens, per mode: **not measured on realistic content.** On our purpose-built test object it happened on **0 of 17**
events in every mode; that object was built to show the change, so it says nothing about your scenes. On our large test level no object that could be framed qualified at the
default memory cap, and the two that qualified under a raised cap are rendered with Nanite, which the pixel
measurement cannot see (below), so the rate could not be read there. Read it on your own content with the filter below.
**To filter such events,** use `change_evidence.jsonl` (§9). For each `uv_corruption` / `normal_corruption` event,
divide its event line's `ref_gt8_max` by `chg_n` (the object's pixel count) from the same event's pair lines. That is
the share of the object that changed noticeably against the frame before the event began. Drop events where it is
small; the threshold is your choice. This works only where change evidence was measured (default capture settings,
§9.1); an event whose `ref_gt8_max` is `-1` has no reading.

**After an event ends — a shadow difference that is not a missed label.** On our large test level, the frames just
after one of these events can differ from a run in which the event never happened, in a small shadowed area, while
those frames are correctly labelled clean. Every corrupted slot is back on its original material, and
`corrupted_texture` (a plain material swap) and a render refresh that changes no material leave the same difference,
so it is not specific to these two anomalies. The cause is not established (the renderer's shadow cache is a
candidate). If you compare frames after an event with an untouched run, expect this; it does not mean the event
lingered.

**Never at the same time as `stuck_low_mip` — in the frames, not only in the game.** The blurry-texture anomaly and
these two never share a captured frame. `stuck_low_mip` counts as live from apply until its textures are seen back at
full resolution in the rendered picture **and** every frame it labels, or flags `transition` after its last labelled
frame, has been written **and** no render record it is still waiting on could show the blur again — which can be
several frames after its event ends (with temporal anti-aliasing, the declared off window (40 frames by default) for its `transition` tail,
including frames at the start of the next capture). `uv_corruption` / `normal_corruption` count as live from apply
until 2 frames after their revert **and** until every captured frame that carries their entry has been written — so if
one is reverted early during a capture (for example `IAI.Revert uv_corruption` in the console, or its object being
removed), the capture still carries its event until the event's scheduled end, and `stuck_low_mip` stays out until
then (`<state>` below reads `label_tail`). While one side is live the other is not started:

- in **Auto-pool**, it is left out of that draw before anything random is drawn, so a seed still reproduces the rest
  of the run;
- on **every other route** — a Targeted capture, `IAI.Apply` in the console, or any other way of applying an
  anomaly — the request is refused `excluded_partner_live:<partner>:<state>` (for example `excluded_partner_live:stuck_low_mip:trail_open`)
  and nothing is changed. `<state>` is `fire_live` (the partner's event is running), `trail_open` (its textures are
  not yet back), `trail_reopenable` (they look back, but a render record it is still waiting on could show the blur
  again), `label_tail` (frames it labels or flags `transition` are still to be written), `restoring` (textures not
  yet back, outside a capture) or `revert_settling` (the 2 frames after a texture corruption's revert).
- A refused re-apply of a running anomaly (for example `IAI.Apply uv_corruption` with the object missing) leaves the
  running one in place **and still counted as live**, so it keeps the other side out until it is reverted.

`run_summary.json` counts each refusal as `auto_excluded_uv_corruption`, `auto_excluded_normal_corruption` or
`auto_excluded_stuck_low_mip` (from any route), and `texcorrupt_m52_overlap_frames` counts captured frames that
carry both a `stuck_low_mip` entry (labelled, or flagged `transition`) and any texture-corruption entry (labelled or
not). **It must read 0**, except as in the second limit below. Two limits:

- **Outside a capture** (Auto-pool running with no capture), the game cannot read the rendered picture, so the
  `stuck_low_mip` side ends when the game has finished its restore, which can be a little before the picture is
  sharp again. A restore that never finishes stops blocking after the same timeout a capture uses
  (`StuckMipRestoreTimeoutFrames`), and each start after it is counted in `texcorrupt_admitted_after_unresolved`: the
  exclusion never lasts forever.
- **If a `stuck_low_mip` restore is declared unresolved** (its textures were not seen back at full resolution in
  time), texture corruption is allowed again and each such start is counted in
  `texcorrupt_admitted_after_unresolved`; the blur may still be in the picture on those frames, and such frames may
  carry both entries (they count in `texcorrupt_m52_overlap_frames`).

**Checking your content first — `IAI.TexCorrupt.Census`.** A read-only console command, in every build. It runs both
anomalies' decision — with the current memory cap and the all-or-nothing rule, without choosing a mode, and without
changing or allocating anything — over the objects Auto-pool could pick right now (`IAI.TexCorrupt.Census`), or over
every drawable object in the loaded levels (`IAI.TexCorrupt.Census all`). It prints exactly four lines, counts only,
with no object, texture, asset or map names:

```
IAI-TEXCORRUPT-CENSUS v1 scope=<view|all> candidates=<n> cap_bytes=<n> uv_modes=<list> normal_modes=<list>
IAI-TEXCORRUPT-CENSUS v1 id=uv_corruption eligible=<n> refused=<n> reasons=<reason>:<n>,...
IAI-TEXCORRUPT-CENSUS v1 id=normal_corruption eligible=<n> refused=<n> reasons=<reason>:<n>,...
IAI-TEXCORRUPT-CENSUS v1 end stats_unchanged=1
```

`eligible` counts objects the anomaly could apply to at that moment; `reasons` lists the refusal reasons by name
(the table above, without their details), or `-` when nothing was refused. A Nanite target is counted under its own
reason, `nanite_unmaskable`, while Nanite targets are skipped (the default); with `IAI.Targets.AllowNanite 1` it is
judged by the per-slot rules instead. For example: `reasons=nanite_unmaskable:7,partial_footprint:1`. The four-line
format itself is unchanged (`v1`). The answer depends on the view and on what
has finished loading, so run it at a few typical views. The command changes nothing — not even the counts a capture
running at the same time reports, and it prints none of the per-object messages other commands print when they first
meet an excluded object. It also prints none of the settings messages the game writes the first time it reads a
setting (for example `texcorrupt: IAI.Anomaly.TexCorruptUvModes = ...`), even when the census is the first command
after the game starts; those messages appear later, at the setting's first real use. So the four lines above are
everything the command prints. `stats_unchanged=1` on the last line is the command's own check that nothing changed;
`0` means something changed and should be reported to us.

**What ships in this delivery:** both anomalies, the four modes above, the settings, the census command and the
`stuck_low_mip` exclusion. Both are in the Capture pool panel, **off by default**. **Deferred:** `drift`, `swap`,
`flat` and `noise`.

⚠ **An "all" pool now includes these two.** `IAI.Auto.Pool all 1` enables `uv_corruption` and
`normal_corruption` too, so an Auto-pool run with every anomaly enabled draws a
**different sequence** of anomalies, objects and holds for the same seed than a build without them. A seed
reproduces runs of the same build, not runs across this change.

One internal test setting that your build does not have (a delayed texture restore, `IAI.Bench.*`) can leave a
corrupted texture on screen past the end of a capture; it cannot happen in a delivered build.

---

## `m43` — THE TARGET ID MASK: what ships, and how to read it

Every captured frame gets an **8-bit grayscale PNG** at `target_mask/frame_NNNNN.png`, numbered by the
same **`session_index`** as `Actual_Frames/`, at **exactly the picture size**.

- **pixel value 0** = background.
- **any non-zero value** = an **anomaly target** visible in that frame, identified by that value.
- **`mask_map.json`** (session root) maps `mask_value` + event → `target_name`, `anomaly_type`,
  `first_frame`, `last_frame`. ⚠ **Values are REUSED across events**, so key on `mask_value` *together
  with* the frame range, never on the value alone.
- 🆕 **Long captures keep their masks.** There are 55 values. Earlier builds gave each event its own
  value for the whole capture, so a session with more than about 55 events shipped its later events with
  labels but **no mask**. A value is now **recycled** when the pool would otherwise run out: it is taken
  back only from an event that has finished, whose every frame and mask has been processed, so **no frame
  ever carries two events under one value**. `run_summary.json` reports `mask_tag_recycles`,
  `mask_tag_peak_live` (the most values held by unfinished events at once) and `mask_tag_exhausted`
  (events that still got no value; expected 0). 🆕 Before a value moves, every object the plugin tagged
  with it is reset, including objects the game has temporarily hidden from the mask pass, and the plugin
  checks that nothing still carries it. If something does, that value is set aside for the rest of the
  capture instead of being reused (`mask_tag_retire_quarantined`; expected 0).
- **`labels.jsonl`** gains **three** keys: `mask_file` and `mask_state` on the frame row, `mask_value`
  on each anomaly row.
- **`run_summary.json`** gains **three**: `target_mask_frames_measured`, `_hidden_blank`,
  `_unavailable`.
- ⛔ **`annotation.json` is unchanged.**
- **`run_summary.json` also carries three shader-readiness keys** — `shader_prewarm_ms`,
  `shader_prewarm_incomplete` and `frames_shaders_pending`. **On a delivered (packaged) capture all
  three read 0**, and a frame row carries **no** extra key. They exist because in an *editor* build a
  material can be asked to draw before its shaders have finished compiling, and such a frame would
  show the engine's placeholder appearance while the label says an anomaly is present; when that
  happens the frame row gains **`render_state: "shaders_pending"`** so it is visible rather than
  silently labelled clean. **If you ever see that key in a delivered dataset, tell us** — it should
  not be reachable in a packaged build.
  ⚠ **It has since been seen once in a packaged build**, marking every frame of a capture that no
  pending shader could have affected — see section 9.9. Do not discard frames on that marker alone.

### 🆕 `exposure_dip` — the game's auto-exposure, made visible (m48)

The game's auto-exposure re-adapts for roughly a second at session start and after a large texture
anomaly appears. Frames whose whole-picture brightness drops more than 4% against the preceding
frames carry **`exposure_dip: true`**; **`frames_exposure_dip`** in `run_summary.json` counts them.
**The plugin never overrides the game's exposure — the dataset looks like the game.**

- The key is **additive and emitted only when true**, so a run with no dip gains no key at all.
- The comparison is against the **rolling mean of the previous 8 CAPTURED frames**, so **the first
  8 frames of a session can never be marked**. A session that opens mid-adaptation therefore reports
  fewer marked frames than the eye would count — a stated limit, not a defect.
- ⚠ **The mark is not a defect flag.** It says the picture got darker than its own recent history,
  which is the game's eye adapting. Use it to explain a dark-looking frame; do not treat a marked
  frame as unusable.
- 🆕 **THE ANOMALY'S OWN EFFECT NO LONGER COUNTS AS A DIP.** Hiding a bright object darkens the whole
  picture, and until this release that was enough to trip the 4% test — so a *disappearing* anomaly
  could mark its own frames as an exposure event. The comparison is now made **twice**: once over the
  whole picture, and once over the picture **with every live target's silhouette removed**. A frame is
  marked only if **both** comparisons see the drop, which is true of a real exposure change (it is
  global) and false of an object being removed (it is local). Marked rows carry
  **`exposure_dip_scope`**: `"frame_minus_targets"` when the second comparison was available,
  `"frame"` when there was nothing to exclude.
- **`frames_exposure_dip_suppressed`** in `run_summary.json` counts the frames the whole-picture test
  marked and the target-excluded test did not — i.e. the marks this rule removed. **Read it beside
  `frames_exposure_dip`:** zero-and-zero is a session with no exposure movement at all, while
  zero-and-non-zero is this rule doing its job.
- ⚠ **Stated limit:** a real exposure change occurring within 8 captured frames of the *first* time a
  target's silhouette is measured can be missed, because the second comparison's own history spans two
  different regions there. It errs toward **not marking**, which loses a warning rather than
  fabricating one.

### 🔑 `mask_state` — the three values, and what each one claims

Every frame row carries **`mask_state`**, and it is the field to branch on:

| `mask_state` | file on disk | what it means |
|---|---|---|
| **`present`** | yes | measured, and at least one anomaly target was visible. `mask_file` names it. |
| **`empty`** | **no** | measured, and the target contributed **no pixels** (fully occluded or off-screen). `mask_file` is `null`. |
| **`unmeasured`** | **no** | **no measurement exists** for this frame. `mask_file` is `null`. It carries **no** claim about visibility. |

🚨 **`empty` and `unmeasured` are different facts and must not be merged.** `empty` is a measurement
whose answer is zero; `unmeasured` is the absence of a measurement.

📌 **(090-10f2) A frame with a labelled anomaly gets a measured mask.** Before this fix, mostly in Play In Editor
(the first second or so of a capture) and rarely in a packaged game, a labelled frame could ship with `mask_state: "unmeasured"` (the mask for that
frame was taken by the previous frame's render). Each mask request is now served only by its own frame's render.
If you ever see a labelled frame (`labelled: true`) with `mask_state: "unmeasured"`, treat the mask as missing for
that frame and report it.

📌 **A mask file exists if and only if it has content.** No all-zero PNG is ever written, so you never
have to test a file to find out whether it says anything.

**`mask_map.json` lists only masks that exist.**

**Counter names, stated because one of them reads oddly:** `target_mask_frames_hidden_blank` counts
rows with `mask_state == "empty"`. **The name is kept from the previous build on purpose** — renaming a
key silently breaks anyone already reading it — but no blank file is written for those rows any more.
`target_mask_frames_unavailable` counts rows with `mask_state == "unmeasured"`.
The three counters sum to the captured frame count.

### ⏱ The first labelled frame of a texture anomaly can look subtle

The first labelled frame of a texture anomaly can look subtle to the eye because temporal
anti-aliasing settles over the following frames; the pixels already differ on the first labelled frame
(bench: 6–8 % of the picture differs against a ~0.5 % baseline). **The label and the mask are both
correct on that frame.**

### 🆕 Hidden-object anomalies DO get masks

For the two hidden-object types — **missing object** and **blinking** — every labelled frame carries a
mask of **where the object would have been**: its **would-be silhouette**, occlusion-aware, so anything
genuinely in front of it still cuts it away. It is not a bounding box.

- **Every labelled hidden frame has a mask file with content.**
- **The visible in-between frames of a blink carry NO mask** — those frames are labelled clean, and a
  mask there would contradict the label.
- ⛔ **Nanite-rendered targets cannot get a mask**, the same limit the anomaly measurement has. By default they are
  not targeted at all (`IAI.Targets.AllowNanite 0`, §8.7a); with the setting at `1` their events are labelled, with
  boxes, and no mask.
- ✅ **Masks are correct at any screen percentage**, including dynamic resolution and temporal
  upsamplers — the mask pass maps its samples through the render's internal view rect.

*Measured on the bench: the hidden-frame mask matches the same object's silhouette while visible, at
the same camera, to an IoU of **0.9969–0.9987**.*

The run's own echo states it, and this line prints on every run:

```
=== Capture(m43): TARGET MASK ON FOR THIS RUN - requested on, from COMPILED DEFAULT (on), output dir
'<session>/target_mask' === READ THIS LINE, NOT THE INI. One 8-bit grayscale PNG per captured frame,
numbered by SESSION INDEX; non-zero pixel values are the stencil tags of the ANOMALY TARGETS visible in
that frame and 0 is background. mask_map.json maps value+event to target and anomaly type. m44: A FILE
EXISTS IF AND ONLY IF IT HAS CONTENT - no all-zero PNG is written. labels.jsonl mask_state says which
fact a frame carries: 'present' (measured, a target was visible), 'empty' (measured, the target drew
nothing) or 'unmeasured' (no measurement exists). empty and unmeasured are DIFFERENT FACTS. It reuses
the m26 pass and does NOT change the m26 measurement, the veto, or annotation.json. Delivery mode does
NOT suppress it.
```

### ⛔ SCOPE — what the mask is NOT

- **It is a mask of the ANOMALY TARGETS ONLY**, not of every object in the scene. Everything else is
  background by design.
- **Objects made entirely of translucent materials are never targeted at all.** They are excluded when
  the anomaly picks its target, because they cannot be measured, so they never reach a mask. *(A
  translucent material that explicitly opts into writing custom depth CAN be measured and is not
  excluded.)* You can turn the exclusion off with `IAI.Select.AllowTranslucentOnlyTargets 1`, and
  `run_summary.json` → `translucent_only_excluded_targets` counts how many objects it refused.
- **Nanite-rendered targets never appear** — the same limit the anomaly measurement has. By default such targets are
  skipped before any anomaly is applied (`refused_nanite`, §8.7a); with `IAI.Targets.AllowNanite 1` their events are in
  `annotation.json` and `labels.jsonl`, with boxes, marked unmeasured.
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

## 9. Change evidence — `change_evidence.jsonl` (m55)

### 9.1 What it is

For each anomaly event, the capture measures **how much the picture actually changed** inside the
target's silhouette on the first frames of the event, and measures the same thing over the **rest of
the picture** as a control. The numbers go into one extra file per session,
`change_evidence.jsonl`, and a block of `change_*` keys in `run_summary.json`.

- **There is no verdict in this version.** Nothing in the file says "visible", "present" or "absent".
  It reports measurements; **you decide** what counts as a visible change for your purpose.
- **Nothing else changes.** `observable`, `affected_frames`, the labels, the masks and the frames
  follow exactly the same rules with this file on or off.
- **A measurement that could not be made is never reported as zero.** Such a row names its reason
  (one of 14, listed below), and the numbers it could not measure — the target numbers (`chg_*`) and
  the pre-onset comparison (`ref_gt8`, `ref_mean`) — are `-1` or `null`. The rest of the row can still
  hold real values: the control numbers (`ctl_*`) on an `empty_region` row whose surroundings could be
  measured, the reference frame's index (`ref_session_index`), the camera and capture bookkeeping, and
  the largest values measured earlier in the phase (`chg_gt8_max_sofar`, `chg_mean_max_sofar`).
- It is **on by default**. `IAI.Capture.ChangeEvidence 0` in the console, before starting a capture,
  turns it off for the following captures (no file, no `change_*` keys).
- A frame can be measured only with the default capture settings for this: the default image grab
  point, PNG frames, output height `0` (native size) and target masks on. Other settings produce rows
  refused `unsupported_delivery`; frames and labels are unaffected.

### 9.2 Reading a row

The file has two kinds of line. **`"kind": "pair"`** lines are one per measured window frame: the
**first four labelled frames of every phase** of every event (a `blinking` event has one phase per
hidden run). **`"kind": "event"`** lines summarise each event when it ends. Join a pair line to its
`labels.jsonl` row and its image by **`session_index`** (never `frame_index`).

For a pair line at frame N, each pixel's change is `d` = the largest of the red, green and blue byte
differences between frame N and frame N−1. The **target region** is where frame N's target mask equals
the line's `mask_value`; the **control region** is where the mask is `0` (every target excluded).

| Field | Meaning |
|---|---|
| `session_index`, `frame_file` | frame N and its image |
| `event`, `phase_ordinal`, `window_index` | the event (as in the labels), its phase, and the window 0–3 (0 = the phase's first labelled frame) |
| `mask_value` | the event's tag in frame N's target mask |
| `chg_measured` | `true` if the numbers below were measured (`chg_eligible` and `pair_valid` are equal to it in this version) |
| `reason` | `null` when measured, otherwise one of the 14 reasons below |
| `chg_n` | pixels in the target region |
| `chg_gt8` | target pixels with `d > 8` (the fixed threshold `tau_px`; not a calibrated visibility threshold) |
| `chg_sum`, `chg_mean` | sum of `d`, and `sum / n / 255` to four decimals |
| `chg_hist` | counts of `d` in eight bins: `0`, `1–2`, `3–4`, `5–8`, `9–16`, `17–32`, `33–64`, `65–255` |
| `ctl_n`, `ctl_gt8`, `ctl_sum`, `ctl_mean`, `ctl_hist` | the same over the control region (rest of the picture) |
| `ref_session_index`, `ref_gt8`, `ref_mean` | frame N compared with the frame **just before the phase began** (the pre-onset reference), over frame N's target region; `-1`/`null` when that frame is not available. On a refused row `ref_gt8`/`ref_mean` are `null` even when `ref_session_index` names the reference frame |
| `chg_gt8_max_sofar`, `chg_mean_max_sofar` | the largest values so far in this phase's window |
| `prev_target_pixels` | the previous window frame's own count of this tag; always `-1` on window 0, so it does **not** show whether the target was visible before the event |
| `cam_dpos_cm`, `cam_drot_deg`, `cam_dfov_deg`, `cam_moved` | how far the camera moved between N−1 and N: whole centimetres, and **tenths** of a degree for rotation and field of view; `cam_moved` is true if any is non-zero. A caveat only — the pair is measured either way. |
| `receipt`, `prev_receipt`, `mask_receipt`, latency fields | capture bookkeeping; you can ignore them |

**The control is there to be compared with the target, not subtracted from it.** The control
excludes every pixel of the target itself, but it **can include** light, shadow and reflections that
the target throws onto its surroundings, so part of the anomaly's own effect can show up there. A
quiet control with a large target change is the clean case. A busy control (the camera moved, the
lighting changed, or the target's own shadow or light spill changed) means part of the target's change
may not be the anomaly's own, and part of the control's change may be. Neither number establishes
what caused a change.

An **event line** carries `anomaly_type`, `target`, `phase_count` and, per phase (first eight):
`state` (`measured` if the phase's first-frame pair was measured, otherwise `indeterminate`),
`pairs_required` (up to 4), `pairs_measured`, the refusal counts in `reasons`, and the largest
`chg_gt8`, `chg_mean` and `ref_gt8` seen (`-1` when none). An event that never had a labelled frame
reads `reason: "no_labelled_frames"`.

**The 14 reasons a pair can be refused:**

| Reason | In plain words |
|---|---|
| `first_frame` | the first frame of the capture has nothing before it |
| `predecessor_missing` | the previous frame was not available (not captured, late, or not kept because of the memory cap) |
| `predecessor_undelivered` | the previous frame's image was never written |
| `out_of_order_timeout` | this frame's inputs arrived too late |
| `epoch_reset` | the game's view changed owner (e.g. a level change), so the two frames are not comparable |
| `view_mismatch` | the image or mask came from a different rendered frame than the one captured |
| `extent_mismatch` | the two frames (or the image and its mask) differ in size or format |
| `mask_payload_missing` | no target-mask pixels arrived for this frame |
| `unsupported_delivery` | the capture settings or the way the frame was rendered cannot be measured |
| `budget_exceeded` | keeping this frame for measurement would exceed the memory cap (9.4) |
| `empty_region` | the target is not in this frame's mask (for example fully hidden behind something), or nothing else is |
| `no_labelled_frames` | event lines only: the event never had a labelled frame |
| `current_undelivered` | this frame's own image or mask failed to be written; `stage` says which |
| `closure_timeout` | at the end of the event or the capture, this frame's inputs never arrived |

`run_summary.json`'s `change_reason_*` keys count these over **every** captured frame, not just the
window frames, so a large count there (typically `mask_payload_missing` on frames with no mask) does
not mean a measurement you needed was lost. Measured yield is `pairs_measured / pairs_required` from
the event lines.

### 9.3 A change that arrives late or gradually

Adjacent frames only show a change that happens **between** them. If an anomaly's visible change
arrives a few frames after its label begins, the first window pairs read quiet and the change appears
in the later pair where it happens — and in the `ref_*` comparison, which compares each window frame
with the frame before the phase began. In testing, an anomaly held back for three frames changed at
most 63 of its 66,837 target pixels (under 0.1 %) in each adjacent pair on windows 0–2. On window 3
it changed all 66,837, in the adjacent pair and in `ref_*`. The `ref_*` values on windows 1–2 were not
that small: because `ref_*` compares across several frames, it had already collected the scene's
ordinary small changes (156 to 819 of the 66,837 target pixels, up to about 1.2 %). So compare
`ref_*` with the control and with the anomaly's expected size rather than with zero. A change that
builds up **gradually**, in steps each smaller than the threshold, can read quiet in **every** adjacent
pair while `ref_gt8` grows with the accumulated change (shown on a synthetic test; not produced by any
current anomaly). **So read `ref_*` (and the event line's `ref_gt8_max`) alongside the adjacent-pair
numbers.** Only the first four labelled frames of each phase are measured; a change that arrives after
the fourth is not in the file.

### 9.4 Memory

The memory cap, `IAI.Capture.ChangeMaxBytes` (default **256 MiB**), covers **only the images and masks
this measurement keeps**, not the game's total memory. It is a hard cap: a frame that would exceed it
is not kept and its pair is refused `budget_exceeded` — **a refusal, never a wrong number**. Full yield
is not guaranteed at any resolution: a long event whose frames complete slowly keeps later frames
until it can close, so long events at high resolution can reach the cap.

- **Planning estimate** (not a guaranteed maximum): `(8 + 3) × image + 9 × mask`, where an image is
  width × height × 4 bytes and a mask width × height bytes — about 49 MB at 1280×720, 110 MB at
  1920×1080, 195 MB at 2560×1440 (within the cap, **not tested**), and 440 MB at 3840×2160 (**above
  the cap: expect `budget_exceeded`** unless you raise `IAI.Capture.ChangeMaxBytes` before capturing).
- **Measured** on our test machine and static test scene, in the captures described in 9.5 (paced
  30 fps, 600 frames): **19–20 MB** at 1280×720 and **56–68 MB** at 1920×1080. These peaks belong to
  that scene and load, not to the resolution: a capture on a second test game reached about **135 MB**,
  and an unpaced 1920×1080 capture reached the cap (9.5). Plan with the estimate above and the cap,
  not with these figures. Nothing guarantees full yield, and nothing bounds memory except the cap.

### 9.5 Cost — what we measured, on our machine, with paced capture

**This is a dated result for one test scene on one machine (2026-09-26), not a guarantee for yours.**
We compared the same build with change evidence off and on: a static test scene, the test anomaly
`solid_swap` (9.8), `IAI.Capture.Config 2 4 16 4 0`, 600-frame captures at 1280×720 and at 1920×1080,
the run log off, and paced capture (`IAI.Capture.Pace 1`, the default) set to 30 fps — on that machine
the capture actually took about **25.07 frames per second**. The figures below come from frames
60–599 of each capture, four captures per side.

- **Game thread.** The one-sided 95 % upper bound on the **mean game-thread CPU time per engine frame**
  (on minus off) was **−0.0718 ms** at 1280×720 and **+0.1100 ms** at 1920×1080. It counts the CPU
  cycles the game thread spent working, converted to milliseconds for that machine: time the thread
  spent **blocked or waiting is not included**, and as an average over the capture it does not show a
  rare or one-off delay.
- **Workload.** Every capture with change evidence on measured **120 of its 120** required pairs, and
  no frame was dropped. A denser workload — more events or measured pairs per second — does more work.
- **Image writer.** No loss of throughput was detected at this load; the writer kept up with it. That
  is not a measurement of its maximum capacity.
- **Background work.** The measurement itself runs on a background thread: about 2.5 ms per captured
  frame at 1280×720 and 7.3 ms at 1920×1080 on that machine.

A heavier scene, a denser workload or a slower machine can cost more. Watch `run_summary.speed_ratio`
and the `change_*` counters (`budget_exceeded`, yield from the event lines) on your own captures.

With pacing **off** (`IAI.Capture.Pace 0`) at 1920×1080 the image writer falls behind; kept frames pile
up to the memory cap and pairs are refused `budget_exceeded`. Under heavy unpaced load the target mask
can also arrive from a different rendered frame than the image, and those pairs are refused
`view_mismatch` or `unsupported_delivery` rather than measured wrongly. **Keep pacing on when you want
change evidence.**

### 9.6 Checking the numbers yourself

```
python host-tools\verify_capture.py --change-oracle <sessionFolder>
python host-tools\verify_capture.py --change-oracle <sessionFolder> --quiet --oracle-json detail.json
python host-tools\verify_capture.py --change-oracle --selftest
```

It recomputes every measured pair from the PNGs in `Actual_Frames/` and `target_mask/`, without using
the numbers in the file, and compares. It also checks that each measured row does not contradict
itself (no refusal reason, `chg_eligible` and `pair_valid` true, histogram and sum present), and that
each `empty_region` refusal agrees with its mask: a refusal whose mask shows both the target and its
surroundings is reported **DISAGREES**. A refusal with no mask image to check (none is written for an
empty mask) is reported unverifiable, not as a disagreement.

Exit code **1** — at least one comparison did not match, a measured row contradicts itself, or an
`empty_region` refusal disagrees with its mask; **3** — part of the file could not be read as
intended (a line that is not a JSON object, or a measured row with a field of the wrong type — each is
listed with its line number), or it could not run at all (no `change_evidence.jsonl`, an unreadable
file, or the Python `Pillow` package missing); **0** — every line was read and everything compared
matched (if nothing was compared, the output says coverage 0). When a mismatch and an unreadable line
occur together the exit code is 1, and the summary always shows both counts.

**It validates arithmetic and transport only**: that the numbers in the file are the numbers in the
delivered images. It uses each row's own frame numbers, so it does not show that the right frames were
paired; it does not check which frame was chosen as a phase's reference (another identical image
would pass), or every field of the file; and it does not show that a change is visible or what caused
it.

### 9.7 `positive_frames` in `run_summary.json`

`positive_frames` counts every written frame whose `labels.jsonl` row has `anomaly_present: true`. For
most anomalies that means an event was running, including frames where its effect is not showing —
for example a `blinking` burst's visible frames. 🆕 For the blurry-texture anomaly (`stuck_low_mip`) only
labelled frames count: its frames while the blur takes hold and after the texture is back no longer
count and carry no entry for it. **It is not a count of labelled frames in general**; use
`injected_frames` and `affected_frames` in `annotation.json` for that.

### 9.8 Two test entries in the anomaly list

**`null_effect`** and **`solid_swap`** are internal test fixtures. Your build does not list them:
`IAI.ListAnomalies` and the dashboard show them only when the game is launched with our internal test
switch, and even then they refuse to apply outside the test setup and never enter the capture pool, the
dashboard's selection or any default. The same switch hides every `IAI.Bench.*` console command: in your
build those commands are not recognised. If you ever see either name listed, tell us — it means the
build was launched with a test switch it should not have.

### 9.9 Known limits (not specific to change evidence)

- **An occasional unmeasured frame.** When one frame's mask render slips, the next frame's mask
  request is served by that render and comes out unmeasured (`mask_state: unmeasured`,
  `target_pixels: -1`, `observable: null`); its change-evidence pair is refused
  `mask_payload_missing`. The label is honest — an unmeasured frame is not counted as observable — so
  this costs coverage, not correctness.
- **The shader-readiness marker can over-flag frames in packaged builds.** In one packaged capture
  every frame carried `render_state: "shaders_pending"` (`frames_shaders_pending` equal to the frame
  count, `shader_prewarm_incomplete: 1`) although no shader work was pending and the active anomaly
  used no affected material. The marker is run-wide, not per anomaly. **Do not discard frames on that
  marker alone** — check the pictures.
- **`coverage_pct` is sampled at processing time**, from the live scene when the event's first frame
  is processed, not from that frame itself. It can differ slightly between two runs of the same capture.
- **Leave the run log off for production captures** (`IAI.Capture.RunLog 0`, or the default in
  delivery mode). With it on, the game thread was measured spending about 1 ms more CPU per frame
  (with pacing off); why is not yet established.
