# REPORT for 094-44d-europa-phaseB-textures-g4-paris.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-44d-europa-phaseB-textures-g4-paris.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-04 10:03:19 | finished: 2026-11-04 10:56:05 | duration: 53 min | exit code: 0
context at last turn: 559283 tokens (56% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-44d-europa-phaseB-textures-g4-paris.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:**
- 🟢 The checkered and pink textures leave no outline leak. With the outline on, 0 new magenta pixels appear on all 2,352 test frames.
- 🔴 Paris on D3D11 fails one test with the textures on by default. The bench scene there is overexposed (white, with glow), so the swap's light spreads well past the object. The anomalies themselves behave correctly. The two D3D12 tests didn't run, because the job stops at a failed test.
- ⛔ Nothing is pushed, and the office card is unchanged. Everything was finished and closed by 10:50, long before the 13:45 blackout.

**G4 and G5 (fixture, seed 4242, same settings and binaries as 094-44c):**
- **Same setup:** the outline-on run's setup files are byte-identical to 094-44c's outline-off run. The fixture binaries equal 094-44c's archive and haven't changed since 08:30. The outline was really on (`weight=1` against `weight=0`), and the launch carried `-BENCHMARK` and `-nowrite`.
- **G4 passes:** 0 new magenta on all 2,352 paired frames, with the pairs matching on every row's anomalies, pose and camera. Differences between the runs are tonemapper grain only (at most 2 levels).
  - Frames covered: checker 169 static and 117 skeletal; pink 65 static and 92 skeletal.
  - My reader catches injected magenta rings on 40 of 40 frames, so it can fail.
- **G5 passes:** `r.CustomDepth` reads 1 at startup and at saved frames 81 and 2300, read directly inside the process.
  - 0 rows with custom depth on, tagged or unexpected; 0 polluted.
  - Depth state before equals after; all 35 reverts leave residual 0.

**Paris regression (`-game`, EA_Core, the four default routes):**
- **D3D11 1280×720:**
  - **Earlier H4 gates:** all pass by my own checker. 2,352 frames; masks equal affected frames (780); every LOD event changes at least 14,456 pixels; census errors 0; unavailable 0 (5 static components); 0 fatal lines; upload check PASS.
  - **Texture G1 and G2 pass:** missing_texture fires 22 times and corrupted_texture 13, across three cubes. EA_Core has no skeletal actor.
  - **Texture G3 fails:** whole-frame IoU is at least 0.088 (mine; Codex 0.0879 pink, 0.0956 checker), with up to 260,214 changed pixels outside the mask.
    - The timing is correct: the frame before each fire and the frames after each revert change 0 pixels.
    - EA_Core is overexposed: 33 % of each frame is saturated, and the targets sit at 0.95–0.98 of full scale against GH2's 0.2–0.8 band. The host's bloom is on.
    - The cause has not been isolated. The pattern matches the bloom halo GH2 fixed on the fixture.
- **D3D12 1280×720 and 1279×719:** not run.

**Push and card:** nothing pushed. Every local commit after `8df75de` descends from the default-route change, so under GH4 none can go out alone, and no pre-push scan was needed. Local feat is now `4cc8b45`; origin stays `8df75de`; the office card stays `094-43-office-card-europa.md`.

**Stop times (IST):** no builds ran. The last launch started at 10:30:40 (the Paris game), and the last process closed at 10:32:53. Codex exited at 10:50, and my readers finished by about 10:53.

**Safety:**
- **No crashes or upload signs.** No new crash or error-report files.
- **Paris host:** its own Content, Config, descriptor, `.uproject` and Binaries are hash-equal. Only its asset-registry cache changed, from the launch.
- **Fixture and materials:** the fixture's assets and config are equal, and both 5.1 material `.uasset`s are unchanged everywhere.
- **Branches and files:** `m51` passes at every check, and only local feat moved. Nothing was deleted, and there are no reparse points.
- ⚠ Codex lowered the Paris launch disk reserve from 3 to 2 GiB before launching. The 50 GiB floor is unchanged.
- ⚠ My polls had one 9.5-minute gap (10:26–10:35) while I ran readers, and the Paris game ran inside it. Its process record rests on Codex's receipts.

**Decisions chat must make:**
1. **Paris texture G3 on an overexposed bench scene.**
   - (a) Apply GH2's bench-only runtime fix to EA_Core: bloom off, and a fixed exposure that puts the targets in the 0.2–0.8 band. Keep `-game`, the whole-frame rule and the 0.98 bar, then rerun D3D11, D3D12 and 1279×719.
   - (b) Check whole-frame G3 on the fixture only.
   - (c) Keep the textures out of the default pool.

   I recommend (a): it applies GH2's rule ("fix the fixture, never the gate"), and EA_Core is our own bench map.
2. **The launch reserve:** confirm or revert the 3 → 2 GiB change.
3. **Sequencing:** 094-45 (camera_clipping) can still go after 15:15. The default-route push waits on the texture regression and on camera_clipping.

My review is `_reviews\094-44d-code-review.md`, and Codex's report is `_reviews\094-44d-codex-report.md`.
api-equivalent cost (not billed on Max): $15.13 | turns: 117 | running total across all briefs: $5,699.15
