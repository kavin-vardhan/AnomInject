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
  host-tools/          the encoder and helper scripts
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
python tools\verify_capture.py --dir <sessionFolder> --label-pixel-gate --report-only
python tools\verify_capture.py --all <folderOfSessions> --label-pixel-gate --report-only
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

**Six anomaly types are available. Four are enabled by default:**

| Anomaly | Default | |
| --- | --- | --- |
| `blinking` | **on** | an object disappears and reappears |
| `missing_texture` | **on** | an object's material is replaced with a checker pattern |
| `corrupted_texture` | **on** | an object's material is replaced with solid magenta |
| `lod_popping` | **on** | an object flips to a much lower-detail version of itself |
| `missing_object` | **off** | an object is hidden for the whole burst, with no reappearance inside it |
| `camera_clipping` | **off** | the camera's near-clip plane is pushed out, slicing away close geometry |

The two unticked ones are **available, not disabled** — tick either whenever you want it in the mix.

**`camera_clipping` is available but off by default**, because in Auto-pool mode it is held for the **whole session** rather than for a few frames — so on a first-person game the player's hands and weapon are sliced away in *every frame* of that capture. That is correct behaviour and it is what the anomaly looks like, but it is disruptive as a default. **Tick it whenever you want it** — see the explainer video and the note in section 7 first.

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
* **`stat FPS`** — shows the game's frames per second in the corner of the screen. Type it again to hide it. (The dashboard's top bar shows the same number.)

Console equivalents of the dashboard sliders, in case you ever need them: `IAI.SetPollRadius 1800` (poll radius — note this one is in **centimeters**, so 1800 = 18 m; 0 = off) and `IAI.SetMinScreenCoverage 6` (coverage percentage; 0 = off).

Everything else (starting/stopping capture, choosing anomalies) is easier from the dashboard, so those commands are the whole list.

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
| `anomaly_type` | string | v1 | `blinking`, `missing_texture`, `corrupted_texture`, `lod_popping`, `missing_object`, `camera_clipping`. |
| `anomaly_subtype` | string | v1 | A finer label for the same event, e.g. `disappear_reappear` for `blinking`. |
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
disappearing anomaly where the renderer still drew the target — **it should be `0`, and a non-zero
value means those frames are labelled as a disappearance that did not happen**), and
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
| `anomaly_present` | bool | v1 | Whether any anomaly was active on this frame. |
| `visible_positive` | bool | v1 | An anomaly was active **and** at least one had a valid box. |
| `anomalies` | array | v1 | One object per active anomaly — see below. |
| `mask_file` | string \| null | since target masks | The mask PNG for this frame, or `null` if there is none. |
| `mask_state` | string | since target masks | `present` / `empty` / `unmeasured`. |
| `exposure_dip` | `true` | since exposure marking | **Only present when true.** The picture darkened more than 4% below its recent average, **and the same drop survives with every live target's silhouette excluded** — the game's auto-exposure adapting, not the anomaly's own effect. |
| **`exposure_dip_scope`** | string | **v2** | **Only present alongside `exposure_dip`.** `"frame_minus_targets"` if a target silhouette was excluded from the second comparison, `"frame"` if there was nothing to exclude. |
| `view` | object | v1 | `origin[3]`, `rot[3]`, `fovDeg`, `aspect`, `valid` — the camera for this frame. |
| `render_state`, `anomaly_materials_incomplete`, `shader_jobs_pending` | — | — | **Editor-build diagnostics; should never appear in a delivered session.** If you see them, tell us. |

**Each entry of `anomalies`**

| Field | Type | Since | Meaning |
| --- | --- | --- | --- |
| `id` | string | v1 | The anomaly type (matches `anomaly_type` in `annotation.json`). |
| `target_name` | string | v1 | The object it was applied to. |
| `start_frame` | int | v1 | The engine frame counter when this fire began — an event identifier, **not** a `session_index`. |
| `seconds_remaining` | number | v1 | How long the fire had left at this frame. |
| `bbox_valid` | bool | v1 | Whether the projected box could be computed for this frame. |
| `bbox_norm` | `[x0,y0,x1,y1]` | v1 | The projected box, normalised 0–1, as **corners**. |
| `bbox_px` | `[x,y,w,h]` | v1 | The projected box in pixels, as **origin + size**. Derived from the object's bounds, so it can be larger than what is drawn. |
| **`bbox_drawn_px`** | `[x,y,w,h]` \| null | **v2** | The box around the pixels the target **actually drew** on this frame, from the mask. `null` when nothing was drawn or nothing was measured. ⛔ **It does not replace `bbox_px`** — both ship. |
| **`target_pixels`** | int | **v2** | See §8.4. `-1` = unmeasured. |
| **`target_drawn_pixels`** | int | **v2** | See §8.4. The subset of `target_pixels` the target was actually drawn at. `-1` = unmeasured. |
| **`observable`** | bool \| null | **v2** | See §8.4. `null` = unmeasured. |
| `mask_value` | int | since target masks | This anomaly's pixel value in `target_mask/`. |

⚠ **The two box fields use different conventions on purpose and always have:** `bbox_norm` is
`[x0, y0, x1, y1]` (corners, 0–1); `bbox_px` and `bbox_drawn_px` are `[x, y, width, height]` (pixels).

### Reading `labels.jsonl` — the rows are not in order

**If you parse `labels.jsonl`, read this first.** The file has one JSON object per line, one line per captured frame. Every frame is present exactly once — **but the lines are not written in frame order.** They are written in the order the capture's background writer finished them, which varies from run to run, and neighbouring frames routinely swap places.

What to do:

* **Key or sort by `session_index`.** That is the frame number: `session_index` N is `Actual_Frames/frame_000NN.png`, frame N of the video, and frame N of `annotation.json`'s frame indices. It is the only field that ties the three together.
* **Do not sort or join on `frame_index`.** That field is the game engine's own internal frame counter. It counts different things and starts from a different place, so it is **not** interchangeable with `session_index` — joining on it will silently mismatch rows. It is kept for engine-side diagnostics; a data consumer should ignore it.
* **Do not assume line N is frame N**, and do not assume the file is sorted even if a particular run happens to come out that way.

Nothing is missing and nothing is duplicated — it is purely an ordering property. `annotation.json` is unaffected: its frame indices are always in order.

### A note on `camera_clipping`

`camera_clipping` is **available but switched off by default** in the Capture pool panel — tick it when you want it. It is a **whole-session** anomaly: it applies to the camera for the entire capture rather than to one object for a few frames. That is why it is not on by default, and it is the consequence worth knowing in advance: anything permanently close to the camera — a first-person viewmodel, a held weapon — sits inside the near-clip radius for the whole run, so it will appear sliced or partly missing in **every frame** of that session. **This is expected behaviour, not a defect**, and it is what the anomaly is meant to look like.

---

## `m43` — THE TARGET ID MASK: what ships, and how to read it

Every captured frame gets an **8-bit grayscale PNG** at `target_mask/frame_NNNNN.png`, numbered by the
same **`session_index`** as `Actual_Frames/`, at **exactly the picture size**.

- **pixel value 0** = background.
- **any non-zero value** = an **anomaly target** visible in that frame, identified by that value.
- **`mask_map.json`** (session root) maps `mask_value` + event → `target_name`, `anomaly_type`,
  `first_frame`, `last_frame`. ⚠ **Values are REUSED across events**, so key on `mask_value` *together
  with* the frame range, never on the value alone.
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
- ⛔ **Nanite-rendered targets are excluded**, the same limit the anomaly measurement has.
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
