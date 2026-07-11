# Capture frame rate — why the video plays at the right speed

This documents how the session-capture pipeline handles frame rate, the bug it fixes, and the
behaviour you'll see while capturing. It supersedes the earlier "fps = fixed 30 metadata" assumption.

## The symptom

Capturing 120 frames at a stated 30 fps produced a 4-second mp4, but the motion in it was
**sped up** — a take that covered ~12 seconds of actual gameplay was crammed into 4 seconds
(≈3× fast). Importing the raw frames into Blender and setting 10 fps looked correct but ran long.

## Root cause

The capture arms **exactly one frame per engine tick** — it already captures at the game's frame
rate, not some independent throughput. The problem was the **time base written into the video**, not
the frames themselves.

On the host, PIE rendered at ~10 fps wall-clock during capture, so 120 consecutive engine frames
spanned ~12 s of game time. The mp4 was then encoded at a hard-coded **30 fps**, so those 120 frames
replayed in 4 s → a 3× fast-forward. Blender at 10 fps looked right because ~10 fps was the *true*
rate of that session. The frame data and labels were always correct; only `video.fps` lied.

## Options considered

1. **Keep a fixed 30 and duplicate frames to real time** — rejected. Padding breaks the 1:1 mapping
   between the mp4 frame count, `Actual_Frames/`, and `affected_frames` indices, which the client
   clip mapping depends on.
2. **Measure the real rate and write it into `video.fps`** — honest metadata. Implemented first
   (commit `c5d58b0`): stamp the world-time of the first/last armed frame and write
   `fps = (frames-1) / (t_last - t_first)`. The mp4 then plays at true gameplay pacing, but the rate
   is whatever the machine happened to hit (fractional, e.g. 9.83), and varies run to run.
3. **Make every frame an exact 1/fps slice (fixed timestep)** — the chosen fix. Instead of
   *describing* whatever rate happened, *force* a native rate. This is the same mechanism UE's movie
   render pipeline uses for deterministic offline rendering.

Option 3 superseded option 2 (commit `500eac7`). The measured-rate calculation is kept only as a
**finalize sanity log**, not written to the annotation.

## The fix: fixed timestep (native fps)

For the duration of a capture run the subsystem switches the engine to a fixed timestep of
`1 / VideoFps` (default 30):

- `StartRun`: save `FApp::UseFixedTimeStep()` / `FApp::GetFixedDeltaTime()`, then
  `FApp::SetUseFixedTimeStep(true)` and `FApp::SetFixedDeltaTime(1.0 / VideoFps)`.
- `FinishRun`: restore both saved values.

Every engine tick now advances game time by exactly `1/VideoFps` regardless of how long the frame
actually took to render. Consequences:

- **Exact output.** 120 frames at 30 = exactly 4.0 s of game time; the mp4 encodes at exactly 30 fps
  and plays at natural speed on **any** machine. `annotation.json` `video.fps` is the fixed rate,
  exactly (an integer), so the client "fps matches the encoded video" rule holds.
- **Time-deterministic runs.** A 5 Hz blink toggles every exactly 3 frames; holds, settles, and
  pre/post-roll are exact frame counts. The same session config produces the same timing on the host
  and the remote machine.
- **Scope.** `SetUseFixedTimeStep` is app-wide — the same scope movie capture uses. It's saved and
  restored around each run, so nothing leaks past `FinishRun`.

## Behaviour you will see while capturing

Fixed timestep decouples game-time from wall-clock and (as in benchmark mode) removes the frame-rate
cap during the run, so the **live game feels sped up or slowed down while capturing**:

```
feel = (frames the machine renders per real second) / VideoFps
```

- Machine renders **faster** than `VideoFps` (e.g. a strong PC at 60+ fps) → game looks **sped up**.
- Machine renders **at** `VideoFps` → **real-time**, no difference.
- Machine renders **slower** (e.g. a loaded editor at ~10 fps) → game looks **slow-motion**.

This is expected and is the fixed timestep working — the **output time base is exact either way**.
One side effect: your hand-driven camera input is time-warped in the take (you move in real time while
the world runs fast or slow), while world motion (bot, physics, anomalies) is exact. For a dataset
this is normally fine.

An optional future toggle, `IAI.Capture.Pace <0|1>`, would sleep each frame to hold ≥ `1/VideoFps`
wall-clock so a fast machine feels real-time during capture (slow machines can't be sped up). The
output is byte-identical with or without it. Not yet implemented.

## Knobs

- `IAI.Capture.Fps <fps>` — native capture/playback rate, default 30, clamped 1–240. Guarded mid-run
  (stop first). Also settable per run; the fixed timestep uses this value.

## Host-side encoder

`host-tools/encode_watcher.py` reads `video.fps` from `annotation.json` and passes it to ffmpeg's
`-framerate`. It parses fps as a float with a 30.0 fallback (commit `c803fe8` in the dashboard repo's
`host-tools/`), so it works whether the annotation carries the exact integer (fixed-timestep path) or,
historically, a fractional measured value.

## Commit trail

- `c5d58b0` — measured session fps written to `video.fps` (superseded).
- `500eac7` — native-fps capture via fixed timestep (current behaviour); measured rate demoted to a
  finalize sanity log.
- `c803fe8` (dashboard `host-tools/`) — encoder accepts fractional fps.

## Pre-fix sessions

Sessions captured before `500eac7` still have their old `video.fps` baked into `annotation.json`
(either a fixed 30 or a measured value). Recapture is simplest; alternatively hand-patch `video.fps`
and delete the `.mp4_done` marker so the encode watcher re-encodes at the corrected rate.
