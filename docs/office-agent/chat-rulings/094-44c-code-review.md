# 094-44c — Code's review: the fixture fix works. Both textures pass G1–G3 on static and skeletal targets (IoU 1.0, 0 pixels outside the mask) and both runtime can-fails fail. Codex stopped at G5 on missing live `r.CustomDepth` replies, so G4, the Paris regression and camera_clipping are unrun. Nothing pushed; card unchanged

**Date:** 2026-11-04, about 10:05 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Resume:** thread `01a14334`, detached, with W4 (the child saw no `GIT_*_DATE`), after a dry run of my gates. The prompt is the 094-44 request verbatim, then GH1–GH6 verbatim, then my context, then the 094-43r implementer block (`resume-prompt-094-44c.txt`, 19,862 B, `EC33E3CE`).
- **Run:** 07:53:54 → 09:49:46 (6,952 s), exit 0, one try. Codex's verdict is **NEEDS-DECISION**.
- **Report:** collected byte-identical as `_reviews\094-44c-codex-report.md`.
- **My files:** receipts in `code-receipts-094-44c\`; readers in `D:\IA_Europa\Temp\CodeReview09444c\`.
- **Polls:** 39 lines, 2.5–3.5 min apart. Sampling stopped for up to about 2.5 min while I ran readers. Every build and session was still sighted.

## 0. GH2 / GH3: the fixture fix
- **The bench settings are runtime-only** (`freeze.json`, 09:25:32, before the first fire at about 09:30):
  - bloom 0;
  - eye-adaptation quality 2 with method 3 (manual), physical-camera exposure off, exposure −2.1875;
  - AA, motion blur and dynamic resolution 0; screen percentage 100;
  - point light 0, plus six shadowless white fills at intensity 3;
  - FOV 30, distance 1,300, static scale 1.3;
  - each target's baseline material swapped to `WorldGridMaterial`.
- **Codex's 4.25 source reason for the earlier failure:** quality 0 clears the eye-adaptation show flag and forces exposure compensation to 1, so `r.ExposureOffset` did nothing in 094-44r.
- **The pre-fire checks** (Codex, calibration previews):

  | Target | Baseline mean | Checker contrast | Pink mean |
  |---|---|---|---|
  | skeletal | 51.48 | 40.18 | (214.2, 0, 202.0) |
  | static | 51.68 | 40.18 | (214.5, 0, 202.2) |

- **My reader on the actual fires:**
  - baseline luminance 0.2008–0.2018 of full scale;
  - checker two-means gap 43.6–43.8 levels;
  - pink median (217, 0, 204), mean (213.6–213.9, 0, 201.5–201.7);
  - 94.5–95.3 % of mask pixels have R, B ≥ 200 and G ≤ 80.
- ⚠ **All three pass, but by thin margins:** the baseline is 0.2–0.7 levels above the 0.2 floor, and pink B is 1.5–2.2 levels above 200.
- **GH3:** the static target is now `OF_LOD_B`, the cube-based one, at runtime scale 1.3. No fixture asset changed.
  - Drawn share: static **7.149 %** (65,886 px), skeletal **6.685 %** (61,609 px). Both are inside 0.5–25 %, and the ceiling is unchanged.
  - The annotation's bounds-based `coverage_pct` reads 9.96 for the static target and 31.74 for the skeletal one. Census selection uses the drawn share.
- **FE3b:** 9 sessions, each with one UBT child, all `-Mode=ValidatePlatforms -allplatforms`, read at creation; 0 children never read; no fallback re-run.

## 1. Port diff against `d7eee97` (my `portdiff44r.py` at `90d4275`)
- **No Source change in this resume** (`2c99a6f..90d4275` touches only docs, `CLAUDE.md` and the kit).
- **The texture files are still d7eee97 plus only the compat include** and the two `OverrideMaterials[i].Get()` → `AnomalyCompat::OverrideMaterial` reads, in each file. The `.h` files are identical; behaviour, names and keys are unchanged.
- **camera_clipping** is still `#if ANOMALY_ENGINE_51`.
- **The kit change** (`7c3303e`, `job_scope.py`): it only adds the 09444c job root to three existing scope lists. No limit changed. Self-test: 3 positive and 3 negative cases.

## 2. Materials
- **The 4.25 materials are hash-equal to `8df75de`** in the Paris and fixture copies: pink `b88d8861`, checker `5222b3ee`.
- **The 5.1 `.uasset`s equal d7eee97's blobs** in all five copies and at every ref.
- **No mutant was authored.** `M_MissingTexture_NoSkeletal` (`0e517277…`) is still in fixture Content only and unchanged since 094-44r. All nine fixture assets are unchanged.

## 3–4. Gates (fixture, D3D11, 1280×720, seed 4242, outline off, four default routes, 2,352 frames, 0 drops)

| | missing_texture | corrupted_texture |
|---|---|---|
| **G1** static / skeletal fires | **13 / 9** | **5 / 8** |
| **G2** every labelled frame changes the target | PASS: all mask px change on every frame | PASS |
| **G3** IoU vs whole-frame change (≥0.98) | **1.000 min**, 0 px outside, 286 frames | **1.000 min**, 0 px outside, 157 frames |
| G3 edges (2 before, 1 after each event) | 0 px changed, none labelled | 0 px changed |
| **G4** paired new magenta (outline on) | **unrun** | **unrun** |
| **G5** | see below | see below |

- **G5:**
  - 0 polluted, lost, no-pass or exhausted rows;
  - 0 custom-depth on, tagged or unexpected rows; legacy reference off on every row;
  - 35 reverts, all with residual 0.
  - ⚠ **`r.CustomDepth` is read only once:** at startup, `"1" LastSetBy: Constructor`, matching the scanner's `custom_depth_mode 1`. The two live probes (frames 80 and 2300) were posted to the editor window and produced no reply. That is why Codex failed G5.
  - At `90d4275`, the only 4.25 access outside `Legacy()` is a read (`AnomalyHostDepth425.cpp:87`).
- **Can-fails, recomputed:**
  - **The usage-flag mutant:** 29 skeletal events / 365 frames, 0 target pixels changed, so G2 fails.
    - The log gives the cause: `missing bUsedWithSkeletalMesh=True! Default Material will be used in game`. PIE counts as game in 4.25.
    - The default material equals the bench baseline, which is why exactly 0 pixels change.
    - The same run's 40 static events pass.
  - **The early label:** every event's two pre-apply frames show 0 changed pixels, so a label one frame early scores IoU 0 on all 75 events (Codex: 35 + 40).
  - ⚠ The early label is a reader-side shift of real masks, not a runtime mutant.
- **Example frames** (`example-44c.png`): the two dark-grey cubes (51/255) on black.
  - Pink frame 719 (skeletal) and frame 935 (static): solid magenta.
  - Checker frames 743 and 839: a sharp grey/white checker (luminance p10/p90 159/206).
  - Masks cover them exactly, with no glow and no changed pixel outside.

## 5. Paris regression · 6. defaults · camera_clipping
- **Paris D3D11, D3D12 and 1279×719:** unrun. No Paris build or launch happened.
- **Defaults:**
  - **local `90d4275`:** blinking, lod_corruption, missing_texture, corrupted_texture;
  - **pushed `8df75de`:** blinking + lod_corruption, with the textures unregistered on 4.25.
- **camera_clipping:** unrun and not enabled.

## 7. Safety (my lists, before against after)
- **Crashes and uploads:** 0 crashes, 0 new crash or WER files; upload check PASS on all 9 sessions; 0 fatal lines. CrashReportClient rewrote its own `Saved\Config` inis at monitor exit (known behaviour); it wrote no logs.
- **Paris:** Content (1,456), plugin Content (7), Config (18), host and plugin Binaries (16 + 11), the descriptor and the `.uproject` are hash-equal. The Saved and Intermediate listings are unchanged.
- **Fixture changes (all fixture-only):**
  - `Source\Outline425\Outline425.Build.cs` (`2634ae9b`);
  - added `Outline425Calibration09444c.cpp` (`22226428`);
  - changed `Outline425TextureBench09444.cpp` (`f6153f6d`);
  - added `Scripts\texture_pose09444c.py` (`40648e6a`);
  - the fixture's Binaries and Intermediate (3 + 3 FM1 builds, all passing);
  - editor artifacts `Saved\AutoScreenshot.png` (new) and `Saved\Autosaves\PackageRestoreData.json`.
  - Content, `Engine.ini` (`64d45f80`), `DefaultEngine.ini` and the `.uproject` are equal.
- **Binaries:** they changed only in builds. The one change my 20 s sampler could not pair with a live build (08:18:14) is Codex's 8 s Editor-02 build (08:18:05–08:18:13).
- **Deletions and links:** 0 removed files across `D:\IA_Europa`, the run folder and `E:\IA_Fixtures`; 0 reparse points; P9 files unchanged.
- **Refs and m51:** `m51` passes at start, prelaunch, exit and final. Only local feat moved (`2c99a6f` → `90d4275`). origin stays at `8df75de`. The spikes are local, and the fixture worktree is detached at `8e59a56`, clean.
- **Processes:**
  - ⚠ **The first editor play session (`preflight-pie`) silently started the Epic Games Launcher.** It started as our editor's child, with `com.epicgames.launcher:?silent=true`.
  - The launcher started the EOS helpers and the overlay; 31 descendants were created.
  - After the editor exited, Codex closed the 12 that were still alive, each matched by PID, creation time and image (`owned-descendant-cleanup.json`). The launcher was alive about 5 min.
  - The later launches added **`-BENCHMARK`**, which skips that start (`EditorEngine.cpp:619`). In 4.25 it also forces the engine's fixed time step, a fixed seed, no sound and no ini writes (`LaunchEngineLoop.cpp:1964-1974, 3545`; `UnrealEngine.cpp:1994, 2275`).
  - No owner process was touched, and nothing of ours is left running.
- **Admission and disk:** the longest admission wait was 213 s (the idle gate). D: 55.8 → 55.7 GiB; E: 89.1 → 87.3 GiB.

## 8. Push and card
- **Nothing pushed.** Every commit after `8df75de` descends from `8e59a56`, which changes the default routes. Under GH4, it goes out only with passing G4, G5, the regression and camera_clipping.
- No pre-push scan was needed. The office card stays `_reviews\094-43-office-card-europa.md` (at `93613ae`), which is still valid.

## 9. Decisions for chat
1. **G5's live readings in PIE.** The kit's console probe types into the window, and the PIE viewport doesn't route that to the console. The options:
   - (a) accept the startup reading, the scanner reading and the per-row component state;
   - (b) take the two live readings in-process, from the fixture's editor Python driver at frames 80 and 2300. This keeps the three-reading rule; I'd recommend it.
   - (c) return the measured legs to `-game`.
2. **The gate environment.** Confirm that the fixture's texture gates may run in an editor play session with `-BENCHMARK`, instead of the `-game` sessions every earlier H-gate used. Paris step 6 would still be `-game`.
3. **The early-label can-fail.** It shifts real masks in the reader. Does that satisfy GH5's "runtime" wording, or do you want a runtime mode that labels one frame early?
4. **The launcher.** The editor play mode starts the network-connected Epic launcher unless `-BENCHMARK` (or similar) is passed. Codex closed only our editor's descendants. Rule whether that is acceptable, and keep the flag mandatory for editor launches.
5. **The thin GH2 margins** (baseline +0.2 levels, pink B +1.5 levels). The outline-on run must use the frozen settings unchanged, or re-qualify them first.
6. **Then resume from `90d4275`:** G4 (outline on), the Paris regression on D3D11, D3D12 and 1279×719, camera_clipping and its gates, the defaults, and the exit.
