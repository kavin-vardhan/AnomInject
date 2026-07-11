# 017 — m11: Capture pacing + honest fps stamping (2026-07-11, gates run; COMMIT PENDING owner eyeball)

## Goal
Make the DELIVERED VIDEO play at natural speed on real-time-clock-driven client games (the Issue-2
office 2x symptom), while keeping game-clock-driven captures (StackOBot) exact. Fix = real-time frame
pacing during capture (game == wall == video clock), with one-sided honest `video.fps` stamping as the
fallback when a box can't hold the target rate. Settled diagnosis + approved plan per the owner's
2026-07-11 rulings (two-clock model; `video_speedup ≈ VideoFps / sustained_wall_fps`).

## What was built (all comment-free; full editor-closed rebuild — Live Coding distrusted for the new
static console command, per the Item-2 lesson)
- **D1 pacing (default ON):** `PaceThisTick()` at the top of `UAnomalyCaptureSubsystem::Tick` right
  after the `bRunning` gate, every tick LeadIn→DrainTail: drift-free accumulator
  (`NextPaceWallTarget += 1/VideoFps`; behind schedule → clamp to now, no catch-up), coarse
  `FPlatformProcess::SleepNoStats(remaining − 1.5 ms)` + spin to target; first tick initializes only.
  `IAI.Capture.Pace <0|1>` (default 1, mid-run guarded); `IAI.Capture.Status` idle line shows fps+pace.
- **D2 measure:** `FirstArmWallSeconds`/`LastArmWallSeconds` stamped at the SAME sites as the game-time
  stamps (async arm site; sync parallel block); `FCaptureSnapshot.WallSeconds` at ARM; per-row `t_wall`
  next to `t` in `BuildFrameLabelRecord` (both paths flow through it). `CaptureLabeledShot` gained a
  `WallSeconds` param (manual `IAI.Capture.Shot` passes now-wall).
- **D3 one-sided honest stamp:** `ComputeRunPacing()` at FinishRun (before the writers):
  `speed_ratio = wallSpan/gameSpan` over the same first/last ARMED frames (settle gaps cancel);
  `sustained = VideoFps/ratio`; tolerance `GFpsStampTolerance = 0.02` (constexpr). ratio > 1.02 →
  `video.fps` = sustained (3 decimals) + Warning; ratio < 0.98 → Info only, fps stays VideoFps
  (never stamp faster-than-target). `video.target_fps` always written (internal; slicer drops it).
  NO duplication/VFR — 1:1 mapping inviolate.
- **N3 split:** `run.json` += `target_fps`, `paced` (start); `run_summary.json` += `target_fps`,
  `sustained_wall_fps`, `speed_ratio`, `stamped_fps`, `paced` (finalize).
- **D4 warnings:** one-shot early warning at ≥30 armed frames when running ratio > 1.02 ("sustaining
  ~X of F fps — the video will be stamped at the true rate; lower IAI.Capture.Fps or run a packaged
  build"); finalize summary line always (`pacing | target | sustained | ratio | stamped`). Short runs
  skip the early warning (N5).
- **D5 dashboard:** `FLastRunPacing` (bValid-gated) + const getter on the capture subsystem;
  `capture_stopped`/`capture_status` replies carry `{targetFps, stampedFps, speedRatio, paced}`;
  dashboard `CaptureStopped` type widened (store.ts), AnomalyClient passes the fields through,
  CapturePanel renders the post-run badge ONLY from the server payload, hidden when stamped == target.
  `tsc && vite build` clean.
- **D6 docs:** `docs/capture-fps.md` REWRITTEN (two-clock model, speedup law, pacing, one-sided
  stamping, corrected "Behaviour you will see", knobs Fps+Pace, stale "settable per run" line deleted,
  ops guidance incl. warm-up + packaged-build + re-encode rescue); gotchas **G64** (two-clock),
  **G65** (UE limiter bypassed under fixed timestep), **G66** (warm-up + background-editor skew);
  architecture.md header block. `encode_watcher.py` untouched.

## Gate results (rebuilt binaries, Simulate over the bridge, editor window foregrounded/visible)
- **G-P1 paced clean @30 (`session_20260711-164933`, 76 frames):** labels `t`-deltas EXACTLY 1/30
  (0.03333×63; 0.1×12 = 3-tick settles); wall cadence 33.2–33.6 ms (avg 33.35), settle boundaries
  99.8–100.0 ms (= exactly 3 ticks); ratio 0.99998; annotation `fps=30` (integer) + `target_fps=30`;
  run_summary paced=true sustained=30.001; NO warnings. mp4 encoded via encode_watcher at 30.0,
  ffprobe `30/1`, 76 frames. **PASS.**
- **G-P2 throttled @60 (`session_20260711-165240`, 76 frames; lever = maximized window +
  r.ScreenPercentage 400 + heavy shadow/AA cvars, pre-verified: unpaced probe sustained 53 fps
  → 18.9 ms > 16.7 ms):** early warning fired at ~30 arms ("sustaining ~58.8 of 60"); finalize warning;
  labels `t`-deltas EXACTLY 1/60 throughout (0.01667×63); annotation `fps=58.055` (fractional) +
  `target_fps=60`; run_summary target=60 sustained=58.055 ratio=1.0335 paced=true; WS `capture_status`
  carried `{targetFps:60, stampedFps:58.055, speedRatio:1.0335, paced:true}` (live socket, real auth);
  encode_watcher encoded AT 58.055 and ffprobe reports `r_frame_rate = 11611/200 = 58.055`, 76 frames
  — NOT the 30.0 fallback. **PASS** (both gate additions included).
- **G-P3 Pace 0 regression @30:** stamp stays 30 under the one-sided rule across FIVE unpaced runs
  (free rates 93–325 fps, ratios 0.09–0.32 → Info only, stamped=30.000, paced=false); sync-path
  Pace-0 run (`session_20260711-170132`, 40/40 frames) shows `t`-deltas EXACTLY 1/30 with free-run
  wall cadence ~14 ms (sustained 93.5 = the fast live feel); no badge (stamped == target). **PASS.**
- **G-P4 zero drops:** all four official gate runs wrote files == summary == labels (76/76, 76/76,
  40/40, 40/40) with no "did not resolve"/encode-failure warnings. **PASS.** (See observations for
  the two out-of-gate pathological runs.)
- **G-P5 sync path (`session_20260711-170056`, Async 0 + Pace 1 @30):** `t_wall` present + strictly
  monotonic; `t`-deltas exactly 1/30; the sync path's own ReadPixels cost (~55 ms/frame) exercised the
  fallback coherently: sustained 18.217, ratio 1.6468, stamped 18.217 fractional, annotation/summary
  agree. **PASS.**

## Observations / flags for the owner (none block the milestone)
1. **Unpaced free-running at extreme rates can mass-drop (pre-existing, NOT an m11 regression):** with
   Pace 0 on this box un-throttled (~300+ fps game thread), two async runs resolved 0/40 arms (loud
   "40 frame(s) did not resolve" warning, honest empty session). The m9-era arm→present pipeline was
   never exercised at 10x-target rates; pacing — the new DEFAULT — inherently prevents the regime.
   Recorded here rather than fixed (Pace 0 is an explicit escape hatch).
   **OWNER RULING (2026-07-11): no action** — the Pace-1 default prevents the regime; documented, closed.
2. **Tolerance sensitivity to single hitches on SHORT runs:** one 188 ms editor hitch in a 1.7 s
   40-frame run pushed the span ratio to 1.11 → fractional stamp on an otherwise-perfect run (the
   stamp is still SAFE — video plays true speed — just not the round number). Longer runs amortize
   (the 76-frame G-P1 stamped clean 30 with the same background noise). If this annoys in practice, a
   hitch-robust ratio (median per-frame) is a possible m11.1 refinement — owner's call, not built.
   **OWNER RULING (2026-07-11): DEFER to a possible m11.1; the 2% constexpr tolerance stands.**
3. **Editor-background throttling (G66):** headless/bridge capture sessions must foreground the editor
   window (or disable "Use Less CPU in Background") or the editor itself sleeps ~100 ms/frame and the
   measured ratios are environmental, not real. Discovered because `ShouldThrottleCPUUsage` is
   suppressed WHILE SHADERS COMPILE — which is why the previous day's cold runs measured fast and the
   warm ones slow. The office/owner workflow (foreground editor) is unaffected.
4. Gate-lever note: `r.ScreenPercentage` DOES apply to this Simulate viewport (initial "ignored"
   reading was an inference error — at the small window even SP 400 still exceeded 60 fps).
   The effective G-P2 lever was maximized-window + SP 400 + heavy shadow/AA cvars.

## Owner-eyeball gate — ALL GREEN (2026-07-11)
- G-P1: live game feels 1x during a paced @30 run; `session_20260711-164933` mp4 plays natural. ✓
- G-P2: `session_20260711-165240` mp4 (58.055 fps) plays natural speed. ✓
- G-P3: with `IAI.Capture.Pace 0`, the old fast live feel returns. ✓
- D5: dashboard badge shows after a sync-path fallback run and is absent after a clean run. ✓
  Owner badge-test log for the record: *"could not hold 30 fps wall-clock (sustained 10.745 fps,
  ratio 2.792) — video.fps stamped at the true rate 10.745."* The 10.7 (vs G-P5's 18.2 on the same
  sync path) = the **dashboard live preview's own sync ReadPixels stacking on the sync capture path** —
  expected, not a defect (see the ops-guidance note in capture-fps.md).

## State / Hand-off
- Working trees DIRTY BY DESIGN (m11 implementation + docs), NOT committed — the single m11 commit +
  tag happens in the commit turn after the eyeball. Plugin base `d4c5730` (m10), dashboard `6c67752`.
- Environment restored: Async 1, Pace 1, all gate cvars back to defaults, editor-background-throttle
  preference restored (transient CDO flip only), editor window un-topmost, play ended, editor open on
  the rebuilt binaries (they match this working tree).
- Files changed (plugin): `AnomalyCaptureSubsystem.{h,cpp}`, `AnomalyLabelWriter.{h,cpp}`,
  `AnomalyControlServerSubsystem.cpp`; (dashboard): `store.ts`, `AnomalyClient.ts`, `CapturePanel.tsx`;
  (docs): `capture-fps.md` (rewrite), `gotchas.md` (G64–G66), `architecture.md`, this journal.
  CLAUDE.md deliberately untouched until the commit turn.
