# REPORT for 094-44r-europa-phaseB-textures-cameraclip.md
verdict: NEEDS-DECISION (derived: NEEDS-DECISION section) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-44r-europa-phaseB-textures-cameraclip.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-04 05:43:50 | finished: 2026-11-04 07:42:39 | duration: 119 min | exit code: 0
context at last turn: 621183 tokens (62% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-44r-europa-phaseB-textures-cameraclip.live.txt

---- FINAL MESSAGE (Claude Code) ----
1. **VERDICT: NEEDS-DECISION.** FE3b, the compiled 4.25 material loaders and GF4 pass by my own checks. The texture port matches the 5.1 release, but its first gate run fails G3 and had no static-target fire. Under the stop rule, the Paris regression and all of camera_clipping did not run. I pushed only the passing steps, `93613ae..8df75de`, and the office card is unchanged.

2. **For the owner:**
   - **Checkered and pink:** they swap and revert correctly, and the labels land on exactly the right frames and pixels. But the test scene is so bright that the checker shows as plain white and the pink as pale pink with a glow around it, so the strict label-vs-picture test fails on that glow. The scene's lighting needs fixing, not the anomalies.
   - **Camera clipping:** not started yet.
   - **Paris:** not re-run this time.
   - **Pushed:** the safe parts only (the 4.25 materials, the safety-check fix and the new skeletal and instanced test objects). The office card is unchanged and still valid at `93613ae`.

3. **What passed and what didn't:**
   - **FE3b (`ae82fe7`):** a background poll every 10 ms records each child process's command line when it starts; no elevation. Its self-test passes, run by Codex and again by me: 12 children of about 104 ms, each read 7–34 ms after it started; 6 query children admitted, 6 build children refused. In the 6 real sessions, each editor startup build-tool child was read at creation, all `-Mode=ValidatePlatforms`, and none was unreadable. No re-run under the fallback was needed. It is slightly stricter than GG2: only `ValidatePlatforms` and `QueryTargets` are admitted.
   - **Materials:** hash-equal to `9c87836`. The loaders compiled on both Paris and both fixture targets (exit 0) and ran in all 6 fixture sessions with no load errors. The 5.1 assets are hash-equal everywhere.
   - **GF4 (my readers):** both new targets match the reference exactly.
     - SkeletalCube: IoU 1.0 on 1,896 frames, identical pixel counts.
     - Three-instance static mesh actor: IoU 1.0 on 1,896 frames, identical pixel counts.
     - Outline on vs off: 0 new magenta on 2,352 frames.
   - **Port:** the two texture files differ from `d7eee97` only by the compat helper that replaces `OverrideMaterials[i].Get()`. ⚠ The same commit, `8e59a56`, also adds the textures to the default routes before their gates passed, so I held it.
   - **Textures, one fixture run (my reader agrees with Codex):**

     | Gate | missing_texture | corrupted_texture |
     |---|---|---|
     | G1 fires | 20, all on the skeletal target | 19, all on the skeletal target |
     | G2 target changes | pass: 76,619 of 76,619 mask pixels change on every labelled frame | pass, same |
     | G3 IoU (needs ≥ 0.98) | **fail, 0.264** | **fail, 0.247** |
     | Edges (frame before, first reverted) | 0 pixels changed | 0 pixels changed |
     | G4 paired new magenta | not run | not run |
     | G5 state | pass: polluted 0, custom depth unchanged, 39 reverts with residual 0 | pass |

     - **Why G3 fails:** 213k (checker) and 233k (pink) pixels change *outside* the mask: a bloom halo around a saturated target on a black background.
     - **Why no static fire:** the static target covers 27.9% of the frame, above the 25% census ceiling.
     - **Can-fails:** only the synthetic early-label test ran. The grey-material and one-frame-early runtime tests did not.
     - **Example frames:** pink frame 719 is a pale-pink cube (mean 255, 225, 254) with a magenta glow. Checker frame 767 is a near-uniform white cube (mean 252) with a grey glow.
   - **Paris regression (D3D11 and D3D12, including 1279×719):** not run.
   - **Push:** the scan of `93613ae..8df75de` is clean. The only carried item is 8 build-warning lines quoting your Windows profile path, the same kind as accepted before. The fast-forward push succeeded and `ls-remote` shows `8df75de`. `8e59a56`, `3457db1` and `2c99a6f` stay local.

4. **Safety:**
   - 0 crashes; upload check passed in all 6 sessions.
   - Paris Content, Config, descriptor and `.uproject` are hash-equal. Binaries changed only during builds.
   - The `m51` gate passes at the end; only feat and its origin ref moved.
   - Deletions were by receipt only: 4 regenerated fixture build caches (`Makefile.bin`), plus the two untracked material originals moved, hash-equal, into the receipts folder.
   - ⚠ Codex saved the can-fail mutant material (`M_MissingTexture_NoSkeletal`) in a separate fixture editor session, outside the request's literal authoring exception. It is fixture-only, not a protected change.
   - The longest admission wait was 244 s, far below the 3 h bound. D: 55.8 GiB, E: 89.1 GiB.

5. **Decisions for chat:**
   - **G3 on this fixture:** I recommend keeping the whole-frame rule and the 0.98 bar, and fixing the fixture instead: bloom off on the bench, and exposure or light low enough that the target isn't saturated. Scoping the comparison to the object (m26's A35 principle) is the other option, but it loosens the gate.
   - **Static target size:** bring a static texture target inside the 0.5–25% band by changing the fixture's pose or scale. Don't raise the ceiling.
   - **Push timing:** confirm `8e59a56` (which changes the default routes) is pushed only together with its passing gates.
   - **Extra fixture session:** accept the authoring session for the can-fail material, or rule it out next time.
   - **Next run:** resume from `2c99a6f` to re-run step 5, then steps 6–10 and camera_clipping.

Files are in `D:\IntrusiveAnomalies\_reviews\`:
- `094-44r-code-review.md`
- `094-44r-prepush-scan.md`
- `094-44r-codex-report.md`
api-equivalent cost (not billed on Max): $21.02 | turns: 165 | running total across all briefs: $5,664.40
