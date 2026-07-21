# Anomaly Injection — Setup & Usage Guide

This package lets you inject labeled visual anomalies into the game and capture them as a dataset: per-frame images, a video clip, and an `annotation.json` describing each anomaly. This guide takes you from a fresh machine to your first captured session.

There are three pieces that work together, delivered together under one folder:

1. **The game build** — the packaged game with the anomaly plugin.
2. **The dashboard** — a small web app to control capture (pick anomalies, start/stop, tune targeting).
3. **The host tools** — a Python script that turns captured frames into an MP4.

The delivery folder looks like this — the two launchers sit at the top:

```
<delivery folder>/
  Setup.bat            run once, first
  Run.bat              starts everything (encoder + dashboard + browser)
  dashboard/           the dashboard app
  host-tools/          the encoder script
  (your game build)    see the Launch section below
```

## 1. Prerequisites

Install this once, before first setup:

* **Python** (3.10+) — runs the dashboard and the video encoder. https://python.org — during install, tick **“Add Python to PATH”**. No extra Python packages are needed.

That's the only prerequisite. You do **not** need Node.js, and you do **not** need to download ffmpeg yourself — `Setup.bat` fetches ffmpeg for you (or uses one already on your PATH).

## 2. First-time setup (once)

Double-click **`Setup.bat`** in the delivery folder and answer its prompts. It is a one-time configurator — it launches nothing. It will:

* **Find or download ffmpeg.** If ffmpeg is already on your PATH (or was downloaded by a previous run) it uses that. Otherwise it offers to download a build for you; say **Yes** and it fetches and unpacks it automatically. (Say No only if you prefer to install ffmpeg yourself and add it to PATH — then re-run `Setup.bat`.)
* **Find Python** on your PATH.
* **Ask where captures should be saved.** Enter any folder you like (Setup creates it, and any missing folders above it, if it doesn't exist yet), e.g. `D:\AnomalyCaptures`. This one folder is used by **both** the video encoder (it watches here for finished captures) **and** the dashboard (the game is told to write captures here), so the two can't drift apart. **You only enter this once** — nothing else ever has to be hand-edited.
* **Save your answers** to a small `config.bat` next to `Setup.bat` (and point the dashboard at that same captures folder), which the run scripts read automatically.

Re-run `Setup.bat` any time your paths change (new game build, moved captures folder) or the dashboard is updated.

**Token (connects the dashboard to the game).** The dashboard and the game share a token so only your dashboard can control your game. This is already configured in the build you received — you do not need to enter or paste anything. (For reference it lives in `dashboard\config.json`; if the dashboard ever reports that the token was rejected, that is the file to check.)

## 3. Running a capture session

Do these in order each time you want to capture.

### Step 1 — Launch the game

<!-- LAUNCH: paste your build's game-launch steps here (e.g. RunServer.bat -> RunClient.bat). -->

> **⟨ LAUNCH — build-specific; fill this in ⟩**
>
> _Start the game and its in-game control server here, then continue to Step 2. The launch steps for your specific build are maintained separately and dropped into this section._

### Step 2 — Start capturing (encoder + dashboard)

Double-click **`Run.bat`**. It opens two windows and your browser:

* **Anomaly Watcher** — watches for finished captures and turns them into MP4s automatically.
* **Anomaly Dashboard** — serves the control app; your browser opens to it automatically at `http://127.0.0.1:5180`.

`Run.bat` then prints a short **status check** — dashboard, watcher, and game server — so you can see at a glance whether anything is missing. If the game server line says *NOT RUNNING YET*, go back to Step 1 and start the game; the dashboard will connect on its own once it is up.

The dashboard connects to the game automatically — no token to enter. You should see a green “connected” dot and a live preview of the game. **Leave both windows open while you capture; close them both when you're done.**

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
```

The MP4 appears a few seconds after capture finishes (as long as the Anomaly Watcher window from Step 2 is running).

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

**Tip — warm up first:** the very first run after launching the game is slower while it compiles shaders. Do one short throw-away capture first; your real captures will then be more consistent.

## 7. Troubleshooting

* **Dashboard won't connect** — make sure the game is running and its control server is up (see the Launch section, Step 2); check with `IAI.Server.Status` in the console, or read the status check `Run.bat` prints. The dashboard connects to `127.0.0.1:8077` on this machine only.
* **Dashboard says the token was rejected** — the game and the dashboard disagree about the shared token. Check `controlToken` in `dashboard\config.json` against the build you were given, then reload the page. (You can also paste a token straight into the dashboard's connect screen for a one-off.)
* **Dashboard won't start** — run `Setup.bat` first, then `Run.bat`. If the Anomaly Dashboard window reports that the port is in use, an older dashboard window is probably still open; use that one, or close it and re-run `Run.bat`.
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

## 8. What's in a session (for reference)

* **`annotation.json`** describes the session: a `video` block (path, resolution, fps, frame count) and an `anomalies` array — one entry per anomaly event, each with which frames it affected, the affected object(s), and the camera/engine state at that moment.
* **`Actual_Frames/`** holds the source images, numbered from `frame_00000` in capture order — frame N in the folder is frame N in the video and frame N in the annotation's frame indices.
* **`Video_Clip/`** holds those frames encoded to MP4 at the fps recorded in `annotation.json`.
* **`run_summary.json`** is a small technical summary (frame counts, timing) — you can ignore it, but don't delete it before the MP4 appears; the encoder uses it to know the session is complete.

Note: bounding-box / pixel-mask detail is not included in this release.
