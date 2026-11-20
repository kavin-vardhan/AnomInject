# REPORT for 084-02-m52-fix-source-build.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-02-m52-fix-source-build.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 14:04:11 | finished: 2026-09-28 16:00:10 | duration: 116 min | exit code: 0
context at last turn: 948058 tokens (95% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-02-m52-fix-source-build.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

The fix is built and pushed, and nothing was launched or staged. Codex's source review came back CHANGES-REQUIRED, so chat needs to rule before part B runs.

## Summary for the owner
- The blurry-texture labels now follow what each captured frame actually drew, and stay on until the texture is sharp again. That targets your 2-frames-late start and ~5-frames-early end.
- A texture is only blurred if nothing else in the loaded level uses it, which targets the glove/wall case.
- None of this has been run yet. Codex found real edge cases that could still mislabel, so chat decides whether tonight's bench runs this build as-is or after corrections.

## 1. Wrong-object cause (from source, at `b5f15a3`)
The old shared-texture check (`Anomaly_StuckLowMip.cpp:118-144`, `:406-417`) had four gaps:
- **Only on-screen users counted.** It looked only at actors that passed frustum, occlusion, the 18 m poll radius and the 6 % screen-coverage cull at fire time.
- **Only some components counted.** Within those actors it skipped hidden, foliage, pattern-excluded and zero-instance components, and anything that isn't a static or skeletal mesh (landscape, decals).
- **Targeted fires skipped it entirely.**
- **It ran once, at `Apply`.**

So a wall off-screen, occluded, far away or under 6 % coverage was invisible to the check, and got blurred while the mask named the glove. Which of these excluded the office wall is not established; there is no office data.

For part B, the bench rock's three textures are used by exactly one placed actor in MainWorld (name-table evidence), so the rock still fires under the new rule.

## 2. The fix
- **Render record (new pure header plus the SVE capturer and scene view extension).** At arm time the frame gets a watch list of held textures. In the colour post-process callback, for that exact request, the render thread reads each texture's resident mips (`GetState()`). A frame is held if any texture was drawn below baseline. Missing, uninitialised or partially-resident resources count as unknown, and unknown is labelled, never clean.
- **One authority (capture subsystem).** At drain, that record sets the label, `stuck_mip.held`, `observable`, the annotation window and the mask. Masks are armed on every frame of the event and trimmed per frame by the record. m55's per-frame change evidence is queued in frame order. The old game-thread value stays as `stuck_mip.held_gt_mirror` for diagnosis.
- **Event outlives the revert.** A new captured `RestoreTrail` phase keeps the event attached until the record shows every texture back at baseline on 2 consecutive frames, plus `IAI.StuckMip.SettleTailFrames` (default 0). The next burst waits for it.
- **Timeout.** After the existing restore timeout (120 captured frames) the event is marked `restore_unresolved`. Further m52 fires are refused with that reason and the target is reserved; frames stay labelled while the record says held.
- **Purity (m52 anomaly).** A texture is held only if it has exactly one user component across all loaded actors: every primitive type plus decals, visibility ignored, targeted fires included.
- **Also in the m52 anomaly:**
  - a texture whose streaming is mid-flight at `Apply` is refused (`baseline_pending`);
  - the "already back but still streaming" race from 084-01 is now tracked at revert.
- **Supporting changes.** Injector: a named per-id refusal and a reserved-actor set. Label writer: new `run_summary` keys, emitted only on runs that fired m52.
- **Bench levers**, registered only under `-IAIBench` and inside the capture code that is compiled out of Shipping:
  - `IAI.Bench.StuckMipLegacyTiming` restores today's timing;
  - `IAI.Bench.StuckMipLegacyPurity` restores the old sharing rule;
  - each m52 fire also logs an independent texture-user count (`TEXUSERS`).

## 3. Builds
- **Game:** exit 0 on a new scratch host, `E:\IA_BuildCache\_r84_host`. It has no content link to the real project, and it is kept warm for rebuilds.
- **Editor:** exit 0, 28 actions.
- **Warnings:** none in plugin code. The only two are old deprecation warnings in the host project's `MidReproActor.cpp`, which I didn't touch.
- **Exe `562EE7A4`:** code-only, pairs with container `67EA1FE0`. A string scan finds every new token, with existing strings as controls.
- **Archive:** `_binary_baselines\m52fix-562EE7A4\`.
- **Staged set:** not touched; still m53 (`2FCDF059`/`20DA6F98`/`534C5863`).

## 4. Offline and unit proofs
- **Unit test** (`tools/m52_window_selftest.cpp`, standalone MSVC, warnings as errors): **41 checks, 0 failures**. It covers:
  - the onset against a game-thread value that lags one frame;
  - the offset with tail 0 and tail 3;
  - a flap and an unknown frame;
  - timeout and burst gating;
  - purity sets, including the off-screen-user case that the old rule fires and the new one refuses.
- **Non-m52 labels unchanged:** this is a source-level argument, not a replay, which would need the engine. Every changed code path is listed in journal §5 with what guards it. The only output difference in a run with no m52 is one extra log line.

## 5. Part B harness (`_reviews\084-03-*`, instructions in `084-03-README.md`)
- **Reuses the S1 pieces:** the launcher (plus one `-ExtraArgs` parameter), quiet and person-present checks, and banking with hardlink aliases.
- **Staging:** the m53 set is backed up, then the baseline is staged, then the fix, each with a hash receipt. The m53 set is always restored and verified at the end.
- **Legs:** B0/B5/B3 on both builds; B9 (anti-aliasing off) on the baseline; the timing-lever leg and two purity legs on the fix; B7 optional.
- **Gates:**
  - (i) sync from pixels, with thresholds from the null run;
  - (ii) the render-record window equals the label and the mask;
  - (iii) purity from the independent `TEXUSERS` counts.
- **Premise audit** per leg: focus, camera, streamer settings from the log, run log present, dither phase, and gap frames reported as censored rather than passed.
- **Dry run** (stubbed launch, banked sessions) is clean. On banked pixels it reproduces the diagnosed baseline: blur 1 frame before the label, 7 blurred unlabelled frames after.
- **Can-fail proofs so far:**
  - doctored labels (onset +2 and offset −5): proven on real pixels — the pixel-repaired window passes, both doctored versions fail;
  - purity-gate parser: proven both ways on known-answer lines;
  - timing lever and old purity rule: these legs run tonight.
- **Pixel reference:** it had to become the flat pre-event level. The texture streamer keeps sharpening the rock for about 150 frames on its own, which fooled the earlier versions. Recorded as gotcha G344.

## 6. Codex (Relay, 933 s): CHANGES-REQUIRED
Collected to `_reviews\084-02-codex-m52-fix-review.md` with a ledger row; not acted on.
- **F1 (P1):** the trail closes on 2 clean frames without checking the restore finished. A late stream-out could blur a frame after the event has detached.
- **F2 (P1):** frames can arrive split across drain batches, giving a clean / held / clean order that closes the trail early. Closure never reopens.
- **F3 (P1):** a frame with an m52 fire but no watch list is treated as "before Apply", i.e. clean. That is unproven if a later frame serves the request.
- **F4 (P1):** purity misses inactive-but-loaded levels and unregistered components, and isn't re-checked after `Apply`. `TEXUSERS` has the same blind spot.
- **F5 (P1):** an unresolved restore is forgotten at run stop and start. A frame-cap cut during a live window isn't counted.
- **F6 (P2):** m55 can be sent a provisional "not labelled" on force-flush, and late queued entries are lost at shutdown.
- **F7 (P2):** a deferred mask's age resets each time it is serviced, so the 16-tick drop never fires.
- **Shipping:** the lever registration is correctly excluded. The legacy-purity flag and its branch still compile into Shipping.
- **Render hook:** reading the record in that callback is sound for the captured frame's ordinary textures. Replacement of a texture's render resource is not detected.
- **Harness counterexample:** gates (i) and (ii) pass four extra clean-but-labelled frames after recovery (via the settle tail). Over-labelling past the last visible frame isn't gated.

## 7. 084-03 invocation
`C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py live`, optionally with `--with-optional`.

Expect 45–60 minutes, up to about 90 with retries, and about 18 GB of banked sessions on D:.

## 8. Commits pushed (`fix/m52-label-timing`, now `749a3bd` == origin)
- `67afa94`: the fix.
- `749a3bd`: journal `docs/sessions/2026-09-28-084-02-code-m52-fix-source-build.md`, the `CLAUDE.md` status block, and gotcha G344.

No tag, no merge. `master` is still `b5f15a3` and `m51` is still `53bf725`, clean.

## 9. Decisions needed
1. **Does 084-03 run tonight on `562EE7A4` as the ordinary-path measurement** (baseline vs fix, plus the settle-tail number)? Or are F1–F7 corrected and rebuilt first (the plugin rebuilds in about 85 s on the warm host)?
2. **The gate contract for the end edge:** should the pixel sync gate also reject labelled frames after the last visible frame, as the addendum's exact-last-frame rule implies? The ruling's gate is one-sided there, and the settle tail would over-label by design.
3. **Purity scope:** include inactive-but-loaded levels, unregistered components and users that stream in after the fire, and if so, how to handle a user that appears during the hold.

Unrelated: several MCP connectors (GitHub, Slack, Notion and others) need authorising via `claude mcp` or claude.ai connector settings, and the PDF viewer failed to connect. Nothing here depended on them.
api-equivalent cost (not billed on Max): $38.46 | turns: 273 | running total across all briefs: $3,048.03
