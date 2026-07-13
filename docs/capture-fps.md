# Capture frame rate — the two-clock model, pacing, and honest fps stamping

This documents how the session-capture pipeline handles frame rate as of m11 (capture pacing +
honest fps stamping). It supersedes the fixed-timestep-only description (which itself superseded
the "fps = fixed 30 metadata" assumption).

## The two-clock model

A UE game's visible content advances on one of two clocks:

- **Game-clock-driven** content (StackOBot): world motion — the bot, physics, our anomaly toggles —
  advances by the world's delta seconds. Pin the game clock and you pin the motion.
- **Real-time-driven** content (the client games this plugin targets): significant visible content —
  sequencer-driven scenes, platform-clock/audio-synced systems — advances on the WALL clock,
  regardless of what the world's delta seconds say.

`FApp::SetUseFixedTimeStep` + `SetFixedDeltaTime(1/fps)` (the m10-era fix) pins only the **game**
clock: every tick advances game time by exactly `1/fps` no matter how long the frame really took.
That is sufficient for game-clock content — which is why StackOBot captures played correctly — but
on real-time-driven content each captured frame holds however much REAL time the frame took to
render, while the annotation claims `1/fps`. When a machine sustains fewer fps than the target, the
mp4 plays fast by exactly:

```
video_speedup ≈ VideoFps / sustained_wall_fps
```

Office example (settled diagnosis, 2026-07-11): 157 ticks in 5.364 s wall at a 60-fps target →
sustained ≈ 29.3 fps → ≈ 2.05x fast video, with zero frame drops and a perfectly exact fixed step.
The frames and labels were always correct; wall time and game time simply disagreed.

## The fix: real-time frame pacing (`IAI.Capture.Pace`, default ON)

While a run is active, the capture subsystem holds every engine tick to **at least `1/VideoFps` of
wall time** (a drift-free sleep at the top of its Tick — coarse sleep + short spin; if the machine
falls behind, it does not try to catch up). Fixed timestep already pins game time to exactly
`1/VideoFps`, so with pacing:

```
game clock == wall clock == video clock
```

and the delivered video plays at natural speed for BOTH clock families. Two side effects, both
desirable: the live game runs at **1x while capturing** (no more sped-up feel on a fast machine),
and GPU readbacks get a full frame budget (fewer end-of-run stragglers).

Note UE's own frame-rate limiter (`t.MaxFPS`, smoothing) is **bypassed under fixed timestep**
(benchmark behavior), so this sleep is the plugin's own — there is no double-limiter, and no host
project setting is touched.

## The fallback: honest fps stamping (one-sided)

Pacing cannot speed up a machine that renders SLOWER than the target. For that case the run
measures the truth and stamps it:

- Every armed frame is wall-stamped (`t_wall` per row in `labels.jsonl`, next to the game-time `t`);
  the run keeps first/last armed wall stamps on both the async and sync paths.
- At finalize: `speed_ratio = wallSpan / gameSpan` between the SAME first/last armed frames (settle
  gaps cancel exactly); `sustained_wall_fps = VideoFps / speed_ratio`.
- If `speed_ratio > 1.02` (couldn't hold the rate): `annotation.video.fps` is stamped with the
  **sustained** rate (fractional, 3 decimals — the encode watcher float-parses fps). The mp4 then
  plays at true speed on real-time-driven content.
- Otherwise `video.fps = VideoFps` exactly (clean integer). A run FASTER than target wall-clock
  (only possible with `Pace 0`) also keeps `VideoFps` — stamping the faster wall rate would make
  game-clock content play fast; this direction is deliberately one-sided (Info log only).
- `annotation.video.target_fps` always records the requested rate (internal field; the client
  slicer drops it). `run.json` records `target_fps` + `paced` at start; `run_summary.json` records
  `target_fps` / `sustained_wall_fps` / `speed_ratio` / `stamped_fps` / `paced` at finalize.

The 1:1 mapping between `Actual_Frames/`, `labels.jsonl` rows, `affected_frames` indices, and mp4
frame numbers is sacrosanct: NO frame duplication, NO variable frame rate, ever.

Warnings: at ≥30 armed frames a one-shot warning fires if the running ratio exceeds tolerance
("sustaining ~X of F fps — the video will be stamped at the true rate; lower IAI.Capture.Fps or run
a packaged build"), and the finalize log always states pacing/target/sustained/ratio/stamped. The
dashboard shows a post-run badge when the stamp fell back ("couldn't hold F fps — video stamped at
X fps (true speed)").

## The content-clock setting (m14) — which clock the honest stamp follows

The one-sided stamp above is only correct when the video's playback rate should equal the SUSTAINED
wall rate. That is true for **real-time-driven** content (sequencer/audio-synced titles), where each
frame holds ~`1/sustained` wall-seconds of motion. It is WRONG for **game-clock-driven** content
(StackOBot world under fixed step), where each frame holds exactly `1/target` GAME-seconds of motion
regardless of how slow the machine ran — there the natural stamp is **target**, and stamping
sustained makes the video play `speed_ratio`× too SLOW:

```
game-clock content stamped at sustained  →  plays  (target / sustained) = speed_ratio  times slow
```

(Observed: 120 frames @ target 60 on a box that sustained 11.64 → stamped 11.64 → a 10.3 s mp4
playing 5.16× slow; the correct stamp is 60 → a natural 2.0 s mp4.)

Because the plugin cannot tell which clock the visible content followed, a **setting** picks it:

- **`IAI.Capture.ContentClock <game|wall>`** (mid-run guarded), default **wall**.
- Packaged default: `DefaultGame.ini [AnomalyCapture] ContentClockDefault=game|wall` (GConfig at
  Initialize, same mechanism as delivery mode). When the key is ABSENT the default resolves to
  **wall**. The console command overrides per session.

Behaviour per mode at finalize:

- **wall** (default): UNCHANGED from the one-sided rule above — `ratio > 1.02` stamps sustained, within
  tolerance stamps target, faster-than-target stays target. This is the m11 real-time-title path, and
  the client titles are wall-clock (see below).
- **game**: `video.fps` is stamped at **target** at ANY ratio (the frames are exact `1/target`
  game-slices), so game-clock content always plays natural. A high ratio in game mode means only that
  the **live capture ran slow** — a capture-time performance issue, not a video defect; the warnings
  say so ("live capture ran slow … video stamped at target F and plays natural").

`run_summary.json` records `content_clock` alongside
`target_fps`/`sustained_wall_fps`/`speed_ratio`/`stamped_fps`/`paced` (annotation stays client-clean;
its `video.fps` already encodes the decision).

### Default = wall — the client-vs-StackOBot clock question, RESOLVED (m15, 2026-07-13)

The default is **wall**. This was tested and settled on the actual office machine (it briefly shipped as
`game` in m14 pending that test — now closed):

- **The client titles (Until Dawn, Concorde) are WALL-clock.** Owner-tested wall vs game on the office
  machine: **wall** produces correct-SPEED videos for them. Their video LENGTH varies with the real
  capture duration — that is CORRECT for wall-clock content (natural playback SPEED is the criterion,
  and wall passes it), not a defect. The earlier `Fps` 120/240 "slow motion" was an
  extreme-forced-ratio artifact, not game-clock evidence.
- **StackOBot is GAME-clock** — set `game` in the StackOBot build (one ini line), where a slow capture
  then stamps target and plays natural.
- **Do NOT flip the default to `game` "to be helpful."** For the client's wall-clock titles that would
  stamp the target on a slow run and play their videos ~`speed_ratio`× (≈2×) FAST — the Issue-2
  regression. This is settled fact, not conjecture.
- **Per-build mechanism, nobody types anything at runtime:** the client build uses the wall default (no
  action); the owner's StackOBot build sets `[AnomalyCapture] ContentClockDefault=game` in its
  `DefaultGame.ini`. The console command exists for ad-hoc override only.
- **Wall-clock video-length property (not a bug):** for wall-clock content the delivered mp4's LENGTH =
  the real capture duration (varies with machine/scene) while the SPEED is natural. A longer clip on a
  slow box is expected and correct.

This supersedes the m14 "default = game, mixed-clock UNRESOLVED, client FAST-risk open" framing
(journal 020's open item is now CLOSED — see journal 021).

**Pre-m14 game-clock sessions** captured before this setting have their sustained rate baked into
`video.fps` on a slow run — use the re-encode rescue below (patch `video.fps` to the target and delete
`.mp4_done`), which is exactly what makes a game-clock capture play natural.

## Behaviour you will see while capturing

With **Pace 1** (default): the live game runs at **1x** while the machine sustains the target rate,
and drops into slow-motion (never sped-up) below it. The output time base is exact either way.

With **Pace 0** (escape hatch, old behavior): the engine free-runs —

```
feel = (frames the machine renders per real second) / VideoFps
```

fast machine → sped-up live feel, slow machine → slow-motion. Stamping stays honest either way,
with the one-sided rule above.

## Knobs

- `IAI.Capture.Fps <fps>` — native capture/playback rate, default 30, clamped 1–240. Guarded
  mid-run (stop first). The fixed timestep, the pacer, and the stamp all use this value.
- `IAI.Capture.Pace <0|1>` — real-time frame pacing during runs, default **1**. Guarded mid-run.
- `IAI.Capture.ContentClock <game|wall>` — which clock the honest stamp follows on a slow run,
  default **wall** (see the content-clock section). Guarded mid-run; packaged default via
  `DefaultGame.ini [AnomalyCapture] ContentClockDefault`.

## Operational guidance

- **Wall-clock capture takes `frames / sustained_fps`, NOT `frames / VideoFps`.** Pacing holds each
  frame to *at least* `1/VideoFps` of wall time — it cannot speed a slow box UP, only slow a fast one
  down to real time. On a box that sustains below the target the run takes longer in wall time (and
  the live game runs in slow motion); the delivered video is honest either way (played at the stamped
  rate). Budget capture time against the sustained rate, not the target.
- **The dashboard's live preview measurably drags sustained fps during capture.** Its per-frame
  ReadPixels stacks on the capture path — observed dropping the sync path from ~18 fps to ~10.7 fps in
  one run. For time-sensitive or delivery captures, disconnect the preview or use a packaged build.
  (An async-preview upgrade + a packaged-build smoke test are the tracked future threads.)
- **Choose `fps` ≤ what the box sustains** in that project/view mode — pacing then delivers exact
  real-time capture with an integer stamp. `IAI.Capture.Status` shows the configured fps/pace.
- **Warm up before judging**: the first run after an editor boot renders slow (shader compilation)
  and skews sustained-fps measurements. Run a throwaway capture first.
- **Packaged Development builds sustain far more than PIE** — for heavy titles at 60, prefer a
  packaged build.
- **Re-encode rescue for old sessions**: hand-patch `video.fps` in `annotation.json` (to
  `target_fps / observed_speedup`) and delete the `.mp4_done` marker — the encode watcher
  re-encodes at the corrected rate.

## Host-side encoder

`host-tools/encode_watcher.py` reads `video.fps` from `annotation.json` and passes it to ffmpeg's
`-framerate`. It parses fps as a float with a 30.0 fallback (dashboard repo `c803fe8`), so integer
(healthy) and fractional (fallback) stamps both encode correctly. Unchanged in m11.

## Preview no longer drags capture (m16)

The control server's live preview does a synchronous `CaptureGameViewportJpeg` (ReadPixels) on the game
thread each push. On a loaded machine that competed with the capture and dragged the sustained fps at the
start of a run (the dashboard only unsubscribed after learning `capture.running` over the snapshot
round-trip). As of m16 the server suppresses all preview-frame generation while a capture is active
(`Cap->IsCaptureActive()`, true from arm through finish) — engine-side, immediate, no round-trip. So the
preview cannot inflate `speed_ratio` during a run; the sustained-fps measurement reflects the capture alone.

## History

- `c5d58b0` — measured session fps written to `video.fps` (superseded).
- `500eac7` — native-fps capture via fixed timestep (game clock pinned; the office 2x symptom on
  real-time-driven titles led to the two-clock diagnosis).
- m11 — real-time pacing (default ON) + one-sided honest stamping (this document).

Sessions captured before m11 on real-time-driven content have their target fps baked into
`video.fps` even when the box couldn't hold it — use the re-encode rescue above.
