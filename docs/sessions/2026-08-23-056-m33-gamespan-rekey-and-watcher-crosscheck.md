# 2026-08-23 — session 056 — m33: the GameSpan re-key and the watcher timebase cross-check

## §1 Goal and verdict context

m33 as ruled by chat (plan APPROVED; the literal `(ArmedFrames−1)×(1/VideoFps)` form explicitly
WITHDRAWN in favour of the tick-span form after the settle-gap flag). Rides Monday's delivery cook,
on `master`. **⛔ NO TAG — m31 remains the open milestone awaiting Concorde V-3/V-4; m34 stays
parked behind this per its A3.**

| commit | repo | what |
|---|---|---|
| `0298143` | plugin | predictions pre-declared (G-A/G-B/G-W/G-C) BEFORE any leg |
| `491eca5` | plugin | AMENDMENT 1 — watcher estimator corrected pre-measurement (§3) |
| `03b0b7a` | plugin | the fix + riding docs (capture-fps, 054 §8 correction, client-readme) |
| `132d27d` | AnomDash | encode_watcher timebase cross-check |

## §2 THE FIX AS BUILT

`speed_ratio`'s denominator is now PLUGIN-OWNED: `(CaptureGameTicks at last arm − at first arm) ×
(1/VideoFps)`, both tick stamps taken inside `StampArmWallClock` — the same two call sites as the
wall stamps, so the operands stay paired. `CheckEarlyPacingWarning` and `ComputeRunPacing` both
consume it. The old world-clock ratio is still computed and emitted as
**`run_summary.game_clock_speed_ratio`** (+1 field, the ONLY artifact delta; `annotation.json`
keyset untouched — **P6 does not move**). The finalize log line gains `legacyRatio=` and
`armTicks=`. The `content_clock=game` stamp branch, the 0.02 tolerance, the one-sided
faster-than-target rule and the stamp→annotation hand-off are untouched.

**Why:** on the pinned decoupled fork the world clock advances WITH WALL (owner artifact: labels
`t` span 34.219865 s vs `t_wall` span 34.220319 s against a fixed-step prediction of 3.967 s), so
the old `WallSpan / worldClockSpan` read ≈1.000 at ANY starvation — the honest stamp never fired
and starved captures shipped `fps=30` MP4s playing 2–5.75× fast. A tick count is a value no host
can redefine (the m31 invariant applied to this instrument). On a stock fixed-step host the two
denominators are identical by construction — measured at G-A as a 1e-7 difference.

## §3 🚨 AMENDMENT 1 — THE RULED WATCHER EXPRESSION HAD THE SAME DISEASE, CAUGHT AT FIXTURE PREP

The ruled backup formula `fps_effective = (N−1)/(t_wall span)` under-reads every healthy GAPPED
session for exactly the reason the plugin formula was withdrawn: settle gaps are real wall time
that is deliberately uncaptured. **Measured before any G-W run, on the fixture source itself
(banked `M30_FPS30`, certified healthy, ratio ≈1.000): 22.99 measured vs 30 stamped — a 23 %
false MISMATCH that would have "corrected" every healthy client video to play ~30 % slow.**

Estimator as implemented: **`1/median(consecutive t_wall delta)` over rows matched to on-disk
frames** — in-burst deltas are the majority, gap deltas are outliers the median ignores. Every
other ruled semantic unchanged (2 % band · one behaviour · loud line naming both numbers and the
winner · vacuity fallbacks). ⚠ **Robustness limit, named: a config whose gaps outnumber in-burst
intervals defeats the median; the shipped `2 4 8 4 0` is in-burst-majority by a wide margin.**

🔴 **CHAT-DECISION REQUIRED (ratify-or-revert): the deviation is from the ruled EXPRESSION, not
the ruled intent.** Pre-registered as AMENDMENT 1 (`491eca5`).
⚠ **Ordering deviation, recorded:** the amendment was WRITTEN before the G-W run but COMMITTED
immediately after, in the same session — transcript-attested, not laundered.

## §4 GATES — all home gates GREEN as pre-declared (`docs/predictions/2026-08-23-m33-gates.md`)

All four legs (2 controls on the pre-m33 exe, G-A, G-B): **VALID on attempt 1, pose gate PASS
(modal_rot (0,0,0), pose_match=True), `start_frame=1`** — A63 comparable throughout.

| gate | result |
|---|---|
| G-A field | `game_clock_speed_ratio` present in GA, ABSENT in both controls ✅ |
| G-A identity | worst difference **1e-7** (tolerance 0.02) ✅ |
| G-A stamp | stamped 30 exactly; `annotation.video.fps` 30 ✅ |
| G-A armTicks | **119** vs 89 frame-intervals ⇒ the withdrawn form would have read **1.337** on this healthy leg — the flag, quantified in the artifact ✅ |
| G-A subset | exit **1** with EXACTLY the one declared extra (`run_summary/game_clock_speed_ratio`); invariant core ALL IDENTICAL (event count 8, canonical gapped cadence, stamped 30) — the m25-S4-3/G158 shape ✅ |
| G-B lever | `CaptureBench.Stall.GameMs 40`; ratio **1.2374** in the pre-declared band [1.10, 1.45]; the banked model predicted 1.239 — within 0.13 % ✅ |
| G-B identity | relative disagreement **5e-5** ✅ |
| G-B honest stamp | FIRED end to end: warning line verbatim, `stamped_fps 24.244 == round(30/ratio,3) == annotation.video.fps` (fractional) ✅ |
| G-W (a) | healthy copy: NO mismatch line, encoded 30.0 ✅ |
| G-W (b) | t_wall ×2: ONE loud `TIMEBASE MISMATCH` line, encoded **14.998** (predicted ≈15.0) ✅ |
| G-W (c)/(d) | note + annotation fps (no labels / 1 matched row) ✅ |
| G-C | **NOT RUN AT HOME — rides the owner's Concorde pass post-cook**, per the ruling |

⚠ **G-B condition 1 (`PROBE` counter read) SATISFIED BEHAVIOURALLY, stated not relabelled:** the
probe's `stallcounters` line prints at module teardown, which the harness's process kill preempts,
so the counter never reaches the log. The lever's firing is established by the command-line echo
(`CaptureBench.Stall.GameMs 40` in the leg's own `LogInit` line) plus the measured ratio landing
on the banked model's prediction. *"A counter that never printed is not a counter that printed 0"*
— and a ratio that moved from 1.0000 to 1.2374 on the same box minutes apart is the behavioural
echo A48 prefers anyway.

⚠ **Runbook §8.2's A44 example control (`IsHideTypeAnomaly`) is STALE** — that symbol was renamed
at session 053 (`ResolveAnomalyActiveSource`), so it now reads 0/0 on a correct binary. The m33
scan's soundness rests on its three POSITIVE hits (`game_clock_speed_ratio`, `legacyRatio`,
`armTicks`, all utf16=1). Runbook deliberately not edited in the feat commit; flagged for the next
docs pass.

## §5 ALSO RIDING (per ruling)

- `capture-fps.md`: formula section re-written to the tick basis · m21 ship-rule scope note (the
  gate is VACUOUS on the pinned fork for pre-m33 binaries; wall math only there) · `Pace 0` note.
- Journal 054 §8: dated correction appended — the pinned ratios were clock-agreement, not health.
- `client-readme.md` §6: the capture-resolution recommendation (owner Option B wording, verbatim).
  Bundle inheritance verified at the manifest level (`PLUGINFILE docs/client-readme.md README.md`,
  `bundle_manifest.txt:54`); the built bundle is checked at the make_delivery step per the ruling.
- Comment stripper: 0 changed / 84 no-change (plugin), 0 / 40 (AnomDash). Diffstats matched intent.

## §6 ENVIRONMENT

Staged bench exe **`757A5DD4`** (= built, hash-verified); predecessor archived at
`_binary_baselines\StackOBot.exe.session055-DCF9C192` (A62-verified). Container unchanged — still
the session-051 quartet; **the delivery cook picks m33 up by pull.** Legs banked:
`M33_CTRL_A`, `M33_CTRL_B`, `M33_GA`, `M33_GB`. G-W fixtures are synthetic, reproducible from
`make_gw_fixtures.py` against the banked `M30_FPS30` source (script in the session scratchpad;
outputs not banked — the transcripted watcher output and the pre-registration carry the evidence).

## §7 NOT DONE, NAMED

- ⛔ **NO TAG** — m31 open; m33 closes only after G-C on Concorde.
- The watcher-estimator deviation awaits chat's ratify-or-revert (§3).
- Stray historical watcher copies (`D:\IntrusiveAnomalies\host-tools\`, `_M2Smoke\`) untouched —
  canonical copy only, per the standing host-tools rule.
- m34 untouched; branch `feature/mask-gpu-reduce` not yet cut (A3: after m33).
- `P6` did not move (subset gate is the measurement) · `feature/stencil-capture` untouched at
  `76cac74` · no force-push · no ratio, no threshold anywhere (the 2 % band is chat's ruled
  tolerance, mirroring the existing `GFpsStampTolerance`).
