# 094-44r — Code's review: FE3b, the compiled 4.25 loaders and GF4 pass; the texture port is exact, but its first gate run fails G3 on a bloom halo and has no static-target fire, so the regression and camera_clipping are unrun. Pushed `93613ae..8df75de` only; card unchanged

**Date:** 2026-11-04, about 07:45 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:** resume of thread `01a14334`, detached, W4 (the child saw no `GIT_*_DATE`), after a dry run of my gates. Prompt = the 094-44 request verbatim + GG1–GG3 verbatim + my context + the 094-43r implementer block (`resume-prompt-094-44r.txt`, 18,694 B, `200E91C7`). 05:49:46 → 07:31:53 (6,127 s), exit 0, one try. Codex verdict **NEEDS-DECISION**; report collected byte-identical as `_reviews\094-44r-codex-report.md`. Receipts `code-receipts-094-44r\`; my readers `D:\IA_Europa\Temp\CodeReview09444r\`. 32 poll lines, 2.3–4.2 min apart; every build and session was sighted.

## 0. FE3b (`ae82fe7`)
- **Diff:** new `runtime_creation.py`: a background thread polls the owned tree every 10 ms and reads each new child's command line at first sight (ordinary process handles; no elevation, no machine setting). `RuntimeTree` now derives from it; `write_ledger.run_child` passes a per-child `child-NNN-creations.jsonl`.
- **Rule:** FE3 is unchanged and one stricter line is added: a read UBT line whose mode is not exactly `ValidatePlatforms` or `QueryTargets` stops the leg (the ruling also allows "another query mode"; Codex admits only those two). `short_child_retry` implements fallback (a): once, only for an unreadable short-lived child, only with the binary snapshot unchanged, never after a crash.
- **Self-test, Codex and my own re-run:** PASS. 12 children of about 104 ms each, all read 7–34 ms after creation; 6 `ValidatePlatforms` admitted, 6 `Mode=Build` refused; unreadable/clean/no-mode/unknown cases and the five retry cases behave as written. The worktree stayed clean.
- **Real children this run (my reader over the six creation logs):** 34 children, 0 never read. Each of the 6 editor/game sessions had one UBT child, all `-Mode=ValidatePlatforms -allplatforms`, read about 0.2 ms after first sight while still live. **No (a) re-run was needed.**

## 1. The port against `d7eee97`
- **`Anomaly_MissingTexture.cpp`, `Anomaly_CorruptedTexture.cpp`:** the only differences from `d7eee97` are the compat include and the two `OverrideMaterials[i].Get()` reads replaced by `AnomalyCompat::OverrideMaterial(Mesh, i)` (4.25: raw pointer; 5.1: `.Get()`). Behaviour, names, label keys and config keys are unchanged. The `.h` files are identical.
- **Callers:** registration ungated (both engines now register both); `SupportsAnomaly` +2 ids; the 4.25 auto pool lists `lod_corruption` where 5.1 lists `lod_popping` (FZ4's client set); C4/C5 default routes become blinking, lod_corruption, missing_texture, corrupted_texture.
- ⚠ **That default-route change is in `8e59a56`, before the textures' gates passed** (D5 says "once their gates pass"). It is why I did not push past `8df75de` (§8).
- `camera_clipping` is untouched: still `#if ANOMALY_ENGINE_51`.

## 2. Materials
- Both 4.25 assets are hash-equal to `9c87836` in Paris, the fixture and git (pink `b88d8861`, checker `5222b3ee`). The 5.1 `.uasset`s equal `d7eee97`'s blobs in all five copies and at every ref.
- **The loader lines compiled:** both Paris targets (Editor, Game) and both fixture targets built `ae82fe7`, exit 0. They ran in all six fixture `-game`/editor sessions with 0 `Failed to find`, 0 default-material and 0 missing-shader-map lines. No Paris session ran this job.

## 3. GF4 (my own readers, fixture, before the texture port)
- **H2 IoU** (legacy reference vs private-off, 2,352 pairs, 0 signature or owner mismatches): **EA_Skeletal** (`SkeletalMeshComponent`) 1,896 frames, IoU 1.0 on every frame, 5,839,680 px each side; **EA_Instanced** (`InstancedStaticMeshComponent`, `LiveInstances`) 1,896 frames, IoU 1.0, 75,199,971 px each side. All other objects 1.0; 222 hidden-blink frames non-empty and equal.
- **H1 new magenta** (private outline weight 0 vs 1, same seed): **0 on all 2,352 frames**, 0 magenta on either side; frames differ only by grain (≤2 levels).
- Availability: 9 components verified, 0 unavailable.

## 4. Textures (one fixture run, D3D11 1280×720, seed 4242, outline off; my reader `texrev44r.py`)
| | missing_texture | corrupted_texture |
|---|---|---|
| **G1 fires** | 20, all EA_Skeletal | 19, all EA_Skeletal |
| **G2** every labelled frame changes the target | PASS: 76,619 of 76,619 mask px change, every frame | PASS: same |
| **G3** IoU vs whole-frame change (≥0.98) | **FAIL 0.2636–0.2637** | **FAIL 0.2471–0.2472** |
| G3 edges (frame before apply, first reverted frame) | 0 px changed, every event | 0 px changed, every event |
| G4 paired new magenta | unrun | unrun |
| **G5** | polluted/lost/no-pass 0; 0 custom-depth on/tagged/unexpected rows; `r.CustomDepth "1" Constructor`; 39 reverts, residual 0 | same |
- **Why G3 fails:** 212,857–214,028 (checker) and 232,341–233,413 (pink) px change **outside** the mask: a bloom halo. The target is saturated before the swap (mean 228.9), on a black background, so its glow spreads about 3× the mask area (bbox y 0–575, x 14–875, against the mask's y 0–223, x 181–541). Even at a 16-level threshold, 65k–86k px change outside. The labels are exact: every mask pixel changes, both edges are clean, the camera is static over every span, and nothing is early or late.
- **Why there is no static fire:** the static target `OF_LOD_A` covers 27.92% of the frame, above the 25% census ceiling, so it was never selected. The same run's annotation has 2,352 frames and 51 events: blink 12, missing_texture 20, corrupted_texture 19, LOD 0.
- **Example frames:** pink `frame-000719`: the skeletal cube turns pale pink (mean 255, 225, 254), with a magenta right face and a dark-magenta glow over the black floor. Checker `frame-000767`: the cube turns near-uniform white (mean 252, sd 6.8) with a grey glow. The checker squares are barely visible: plan risk R5 (exposure) in practice, still at `r.ExposureOffset -2` with a 600-intensity light.
- **Can-fails:** synthetic only (an early label gives IoU 0). The missing-skeletal-usage material was authored (`M_MissingTexture_NoSkeletal`), but neither runtime can-fail ran.

## 5. Paris regression · 6. defaults · camera_clipping
- **Paris D3D11 / D3D12 / 1279×719:** unrun.
- **Defaults:** local `2c99a6f`: blinking, lod_corruption, missing_texture, corrupted_texture. **Pushed `8df75de`: blinking + lod_corruption**; the textures are still compiled out there.
- **camera_clipping:** port, gates and route are unrun; not enabled on 4.25.

## 7. Safety (my lists, before against after)
- **Crashes and uploads:** 0 crashes, 0 new crash folders, upload check PASS on all 6 sessions.
- **Admission:** the longest wait was 244 s (idle gate, GF4 private-off), far below GA5's 3 h.
- **Paris:** Content (1,456), Config (18), plugin Content (7), the descriptor and the `.uproject` are hash-equal; `Saved` unchanged. Binaries and Intermediate changed only in builds.
  - One change, at 06:09:56, fell between my 20 s samples. It matches Codex's 8 s fixture Editor-02 build (06:09:39–47).
- **Fixture:**
  - **Changed:** the map (step 3's two actors); `Outline425.Build.cs`.
  - **Added:** `M_LiveSurface` (step 3's session) and `M_MissingTexture_NoSkeletal`; two fixture `.cpp` helpers; two scripts; an editor map backup.
  - ⚠ The mutant material was saved in a separate fixture authoring session (07:13), outside the request's literal exception (step 2's materials, step 3's actor). It is fixture-only, not a protected change.
  - `Engine.ini` (`64d45f80`), `DefaultEngine.ini` and the `.uproject` are equal.
- **Deletions, by receipt only:** 4 fixture `Makefile.bin` caches (regenerated by UBT). The two untracked fixture material originals were moved into `Receipts\09444r\material-author-backup` (hash-equal) before the fixture worktree was checked out.
  - My tree list over `D:\IA_Europa`, the run folder and `E:\IA_Fixtures` shows 0 removed files. P9 files unchanged; 0 reparse points.
- **Refs and m51:**
  - `m51` passes at start, prelaunch, exit and final; `master`, `release`, 30 tags and the tag refs are unchanged.
  - Only feat (`9c87836` → `2c99a6f`) and origin feat moved. The spikes are local, and neither is an ancestor of feat. The fixture worktree is detached at `8e59a56`, clean.
- **Processes:** none left.
- **Disk:** D: 55.9 → 55.8 GiB; E: 99.4 → 89.1 GiB.

## 8. Push and card
- **Pushed `93613ae..8df75de`** (`9c87836` materials + finders, `ae82fe7` FE3b, `6ee4a0e` and `8df75de` docs). The scan is clean (`_reviews\094-44r-prepush-scan.md`), the push a fast-forward, and `ls-remote` = `8df75de`. These are the steps that passed and compiled, and they stand alone: on 4.25 at `8df75de`, the textures stay unregistered and the defaults are unchanged.
- **Not pushed:** `8e59a56`, `3457db1`, `2c99a6f`. Their gates failed, and `8e59a56` changes the default routes.
- **Card unchanged:** `_reviews\094-43-office-card-europa.md` stays pinned at `93613ae` and stays valid. No `094-44r` card was written, because its contents (textures in the default pool, the camera_clipping route) are not qualified.

## 9. Decisions for chat
1. **G3 on this fixture view.** The whole-frame silhouette rule cannot pass while bloom is on with a saturated target. Recommend: keep the rule and 0.98, and fix the fixture (R5): bench-only bloom off, plus exposure or light low enough that the target is not saturated, then re-run step 5. The other choice, scoping the change silhouette to the object, follows m26's A35 principle (the label is the object, not its glow), but it would loosen the gate.
2. **Static coverage:** put the static texture target inside the 0.5–25% census band (pose or scale on the fixture). Do not move the ceiling.
3. **Push timing for `8e59a56`:** confirm it is pushed only together with its passing gates (D5).
4. **Accept the extra fixture authoring session** for the can-fail material, or rule it out for next time.
5. **Then resume from `2c99a6f`** for step 5's re-run, steps 6–10, and camera_clipping.
