# REPORT for 094-44c-europa-phaseB-textures-cameraclip.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-44c-europa-phaseB-textures-cameraclip.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-04 07:48:00 | finished: 2026-11-04 09:57:48 | duration: 130 min | exit code: 0
context at last turn: 620233 tokens (62% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-44c-europa-phaseB-textures-cameraclip.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:**
1. With the test scene's lighting fixed, the checkered and pink textures now work and label correctly on both a static object and a skeletal object. Every labelled pixel changes, nothing outside the label changes, and both deliberate-failure tests fail as they should.
2. Codex stopped before the last state check, the outline-on leak check and camera clipping. One required reading (the custom-depth setting, taken live during the run) never reached the engine's console in the editor session the tests now use.
3. Paris was not re-tested this run.
4. Nothing was pushed and the office card is unchanged, because the default-setting change may only go out with all its tests passed.

**The fixture fix (GH2/GH3).** All changes were runtime-only and frozen at 09:25:32, before the first fire.
- **Settings:** bloom off; manual exposure (eye-adaptation quality 2, method 3, physical-camera exposure off) at −2.1875; the point light replaced by six shadowless fill lights; FOV 30.
  - Codex found the cause in the 4.25 source: quality 0 forces exposure compensation to 1, so the old exposure offset did nothing.
- **Pre-fire checks (Codex):** baseline 51.48 / 51.68 of 255; checker contrast 40.18; pink about (214, 0, 202). My reader on the real fires agrees: baseline 0.2008–0.2018 of full scale, checker gap 43.6–43.8, pink median (217, 0, 204). All pass, but the margins are thin.
- **Static target (GH3):** now the cube `OF_LOD_B` at scale 1.3, with drawn share 7.149 %. The skeletal target's drawn share is 6.685 %. The 25 % ceiling is unchanged.

**Per anomaly** (fixture, D3D11, seed 4242, 2,352 frames, 0 drops; recomputed with my own readers):

| | missing_texture | corrupted_texture |
|---|---|---|
| **G1** fires, static / skeletal | 13 / 9 | 5 / 8 |
| **G2** target changes | every mask pixel, every frame | same |
| **G3** IoU (min) and pixels outside the mask | 1.000, 0 (286 frames) | 1.000, 0 (157 frames) |
| G3 edge frames | 0 px changed | 0 px changed |
| **G4** outline-on leak check | unrun | unrun |
| **G5** state | components 0, polluted 0, 35 reverts with residual 0 | same |

- **G5's gap:** `r.CustomDepth` was read once, at startup (`"1" Constructor`). The two live readings sent to the editor window at frames 80 and 2300 got no reply.
- **Can-fails:**
  - The usage-flag mutant fails G2: 29 skeletal events, 365 frames, 0 pixels changed. The log confirms why: "missing bUsedWithSkeletalMesh … Default Material will be used in game".
  - The early label fails G3: IoU 0 on all 75 events. This is a shift of real masks in the reader, not a runtime mutant.
- **Example frames:** in frames 719/935 (pink) and 743/839 (checker), two dark-grey cubes turn solid magenta or a sharp grey/white checker, exactly inside the mask, with no glow.
- **Port:** no source changed this run. The texture files are still `d7eee97` plus the compat helper. The materials are hash-equal to `8df75de`, the 5.1 `.uasset`s are unchanged, and no new mutant was authored.

**Paris regression:** unrun on D3D11, D3D12 and 1279×719.

**camera_clipping:** unrun, still compiled out.

**Defaults:** local `90d4275` uses blinking, lod_corruption and both textures; origin `8df75de` still uses blinking + lod_corruption.

**Push and card:** nothing pushed, and no scan was needed. Everything after `8df75de` sits on top of `8e59a56`, the default-route change (GH4). The card stays `_reviews\094-43-office-card-europa.md`. My review is `_reviews\094-44c-code-review.md`; Codex's report is collected byte-identical as `_reviews\094-44c-codex-report.md`.

**Safety:**
- 0 crashes; the upload check passed on all 9 sessions.
- Paris Content, Config, descriptor and `.uproject` are hash-equal. Only fixture source and scripts changed, plus two editor autosave/screenshot files.
- `m51` passed at every check; only local feat moved. Binaries changed only during builds. 0 files deleted and 0 reparse points.
- The longest admission wait was 213 s.
- ⚠ The first editor play session silently started the Epic Games Launcher and its EOS helpers, about 5 minutes alive. After the editor exited, Codex closed the 12 still running, each matched as our editor's descendant by PID, creation time and image. No owner process was touched.
- ⚠ Later launches added `-BENCHMARK` to stop that launcher start. In 4.25 it also forces a fixed time step and a fixed seed, and turns off sound and ini writes.

**Decisions for chat:**
1. **G5's live readings in the editor's play mode.** Options:
   - accept the startup reading plus the per-frame state;
   - take the two readings from inside the process, through the fixture's Python driver (my recommendation, since it keeps the three-reading rule);
   - move the measured legs back to `-game`.
2. **The test environment.** Confirm the fixture texture gates may run in an editor play session with `-BENCHMARK`, instead of the `-game` sessions every earlier gate used. Paris would stay `-game`.
3. **The early-label can-fail.** It shifts real masks in the reader. Decide whether that meets GH5's "runtime" wording.
4. **The Epic launcher.** Decide whether closing launcher processes our editor started is acceptable, and whether `-BENCHMARK` becomes mandatory for editor launches.
5. **The thin GH2 margins:** baseline +0.2 levels and pink B +1.5 levels above their limits. The outline-on run must reuse the frozen settings or re-qualify them.
6. **Then resume from `90d4275`:** G4, the Paris regression on both RHIs, camera_clipping and its gates, the defaults, and the exit.
api-equivalent cost (not billed on Max): $19.62 | turns: 149 | running total across all briefs: $5,684.02
