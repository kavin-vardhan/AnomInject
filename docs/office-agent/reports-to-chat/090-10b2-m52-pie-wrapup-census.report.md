# REPORT for 090-10b2-m52-pie-wrapup-census.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-10b2-m52-pie-wrapup-census.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-01 21:06:55 | finished: 2026-10-02 01:38:36 | duration: 272 min | exit code: 0
context at last turn: 876603 tokens (88% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-10b2-m52-pie-wrapup-census.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner**
1. Pull **`feat/m53-uv-normal-corruption` at `d7e396d`**. Its code is the same as `61d186a`, exe `E2BCE76B`, and every game run tonight used exactly that build.
2. In Play-In-Editor at home, texture-coordinate (UV) corruption and normal-map corruption both visibly corrupt 6 of 6. A target that would draw the default material, or whose shaders don't exist yet, is refused with its own reason and never labelled. Blurry textures visibly blur on the MainWorld rock and pass the label-timing gate (17/17 in PIE, 16/16 staged); auto-pool blurred 5 of 5.
3. The blurry-textures refusal you saw in PIE on the test level is deliberate: that test object shares its texture with a neighbour on purpose. It was not caused by the editor's copy of the world, so there was nothing to fix there.

**Part 0: state**
- Heads matched origin at the start (m53 `0a69ea3`, fix `cf422d5`). `0a69ea3` is the source of exe `65607703`: committed 16:14:20, built 16:14–16:18, relinked by the strict pass at 16:28, and it matches the archive 6/6.
- Nothing of 090-10b's was still running. Its watcher (PID 27364) and all 68 PIDs its launcher recorded were gone.
- 090-10b's interrupt left two things behind: the staged package held `65607703` instead of the S1 set, and the m53 build host held editor DLLs from an older build that the build tool would have reused. I set those DLLs aside (not deleted) before building. Both are recorded in the journal.

**Part A: 090-10b written up, plus the other direction**
- **090-10b journal** (`docs/sessions/2026-10-01-090-10b-office-nothing-corrupts.md`), written from its own run records:
  - **Cause:** S6 required the editor's whole-shader-map "complete" flag, which stays 0 in the editor for materials that draw fine.
  - **Proof:** every earlier m53 build shows 0 events in PIE (`shader_map_incomplete`) and 6/6 staged; `65607703` shows 6/6 in both.
  - **Can-fail:** the office exe `8131479A` fires 0 events in PIE.
  - It also covers the yield, startup and no-visible log lines, `anomaly_refusal_counts.py`, and the bounded census.
- **Gaps I found in `0a69ea3`:**
  - It still admitted on that cached "complete" flag.
  - It accepted any shader, not specifically the shaders the main draw pass needs.
  - It checked spline meshes against the wrong vertex type.
  - It checked identity after readiness and gave both failures one name.
- **New order (`5fba7df`):**
  1. Metadata.
  2. **Identity** (`default_material_path`): the mesh needs a material usage the material lacks, so the game draws the default material.
  3. **Draw readiness:** the component's vertex type must have the main draw pass's vertex and pixel shaders. Still compiling → `shader_map_incomplete:compile_pending`; missing → new `draw_shaders_missing`.
  - Pure test 15/0; four mutants fail (whole-map only, the `0a69ea3` rule, any-shader, identity-after-readiness).
- **In PIE** (a spline mesh summoned into the PIE world, because Python can't build an instanced mesh):
  - **Identity:** the engine logs `missing bUsedWithSplineMeshes=True! Default Material will be used` and the plugin refuses `default_material_path:spline_mesh` every time, 0 events.
  - **Shaders genuinely absent:** refused `shader_map_incomplete:compile_pending` while the editor compiled them on demand (`MaterialShared.cpp:3348-3388`), then admitted; all 13 events visibly corrupt.
  - **Healthy:** 28/28.
  - **Not shown in-engine:** a permanent `draw_shaders_missing`. It needs a material whose cached flag reads complete, and my attempt to build one didn't form; that case rests on the pure test and the engine source.

**Part B: blurry textures in PIE**
- **The premise was wrong.** The PIE log scanned 1 level (98 components); the two users of the shared texture are `_77` and `_78` in the PIE world. The editor world holds two more copies, and they were not counted (the log says 2 users, not 4). The test level builds that pair on purpose (`make_texcorrupt_fixture.py:1443-1446`, "m52 target (EXCL)").
- **So no counting logic changed.** The fix branch (`c60de04`, `a5f0a06`) only makes the log name its world, corrects the summary wording ("shared with a visible component" was wrong), keeps the office counting tool in step, and adds a two-world unit test that an "all worlds" mutant fails.
- **Label timing gate** (rock, AA off, against its matched null): PIE PASS 17/17; staged PASS 16/16, the same as the 084-09 baseline. The first null run was invalid (the null switch needs `-IAIBench` on the command line); it was discarded and re-run.

**Part C: the census can't freeze the game**
Declared limit: no frame over 100 ms while the census runs.

| scene | where | longest frame | result |
|---|---|---|---|
| MainWorld | PIE | 36.7 ms | PASS |
| MainWorld | staged | 18.0 ms | PASS |
| office size (15,343 actors) | PIE | 63.9 ms, done in 1.7 s | PASS |
| MainWorld, office exe, staged | staged | 150.8 ms | can-fail fires |
| MainWorld, office exe, after a capture (the office's order) | PIE | 192.3 ms (`shader_map_incomplete:68`) | can-fail fires |

- The office-size run finished before the first 2-second progress line, so no progress line appeared there; the 40,000-actor run printed 11.
- ⚠ At 40,000 actors the very first frame takes 195 ms, because listing all actors is not split across frames. There was also one 650 ms frame whose cause is not established.

**Builds, archives, commits (all pushed, fast-forward)**
- **m53** `0a69ea3..d7e396d`:
  - strict build 0 errors / 0 warnings;
  - log-text differences vs the last archive: 35, all explained (new strings or one-byte artefacts);
  - bench-switch audit 47/47; C++ tests 7/7, mutants 27/27 failing; Python 27/27;
  - archived to `m53-0910b2-E2BCE76B` (6/6 re-hashed).
- **fix** `cf422d5..d3c9423`:
  - strict build 0/0; log-text differences 12, all explained;
  - bench-switch audit 34/34; C++ tests 5/5, mutants 20/20 failing plus the new all-worlds mutant; Python 19/19;
  - archived to `m52fix-0910b2-5E93A74E`.
- The project's content files are unchanged (2,207, nothing changed). Staged package is back on the S1 set. Evidence banked to `D:\IA_BankOverflow\090-10b2-evidence` (10.6 GB, verified). New gotchas G443–G444, and 090-10b's G439–G442 are now committed. The checklist rule "visibly worked in PIE and staged" is §0 of `PRE-DELIVERY-CHECKLIST.md`.

**NEEDS-DECISION**
1. **Part B:** the requested fix doesn't apply because its cause was refuted. Why blurry textures never showed at the office is still unknown; the office has given no blurry-texture counts yet.
2. **Census above ~30,000 actors:** splitting the actor listing across frames is a small change, but every check would need re-running. Do it now or with 090-10c?
3. **PIE-only timing glitch:** on about one event per PIE run, the picture stays corrupted one frame after the label ends. It also happens on the older `0a69ea3` build (side-by-side run: 2 and 1 such events), and not in the packaged game. Chase it or file it?
4. The office's `runtime_lod_bias` refusals (14,454 of 15,906) are 090-10c's subject.
api-equivalent cost (not billed on Max): $47.36 | turns: 333 | running total across all briefs: $4,255.65
