# 094-44f — Code's review: GK2 picks −5.0 EV and my reader agrees, but the D3D11 leg's own preflight came up with a different camera view, so no Paris leg was scored. Nothing pushed; the card is unchanged

**Date:** 2026-11-04, about 12:40 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Resume:** thread `01a14334`, detached, with W4 (the child saw no `GIT_*_DATE`; one secret-named variable removed by name), after a dry run of my gates. m51 and refs passed at start, prelaunch, exit.
- **Prompt:** the 094-44 request verbatim, then GK1–GK3 verbatim, GJ5 verbatim, GI7 verbatim, the GK3 scope line, my context and the 094-43r implementer block (`resume-prompt-094-44f.txt`, 22,330 B, `8F06BFD2`).
  - ⚠ My context said the resume started "about 12:12 IST". It started at 12:05:34, so the stated time left was 7 minutes short (the safe direction).
- **Run:** 12:05:34 → 12:32:35 (1,621 s), exit 0, one try. Codex's verdict: **NEEDS-DECISION**.
- **My files:** receipts in `code-receipts-094-44f\` (10 poll lines, 2.5–3 min apart); readers in `D:\IA_Europa\Temp\CodeReview09444f\`.

## 0. GK2: the EA_Core recalibration
- **Method (Codex):** a new reader re-read 094-44e's retained 65-exposure sweep (`E:\IA_BuildCache\09444e-evidence\Calibration\01`, raw files untouched, 73 sealed files rehash equal) under GK2. Result `D:\IA_Europa\Receipts\09444f\calibration-result.json`, frozen in `freeze.json`.
- **Chosen: −5.0 EV.** All 40 brighter tested values fail. Codex's numbers at −5.0:

  | Target | Untouched mean | Channel-255 px | Pink (min R/G/B) | Checker gap |
  |---|---:|---:|---|---:|
  | EA_HideCube | 151.89 (0.60) | 0 | 147.7 / 27.8 / 147.9 | 50.65 |
  | EA_BlinkCube | 152.44 (0.60) | 0 | 148.3 / 27.8 / 148.4 | 49.73 |
  | EA_LOD | 101.56 (0.40) | 0 | 245.9 / 90.6 / 232.0 | 21.09 |

- **My recheck agrees** (`calib44f.py` on the same raw frames; whole-silhouette masks from the pink-vs-original difference, 16,815 / 16,814 / 25,850 px; saturation = any channel at 255 in any frame of the untouched preview; checker = two-means luminance gap):
  - Brightest passing: **−5.0 EV**. Qualifying band −5.0 to −7.375; below −7.375 the cubes' pink drops under R,B ≥ 64.
  - At −5.0: means 154.0 / 154.6 / 101.5; 0 channel-255 px on all three; pink (149.8, 28.2, 149.9), (150.4, 28.2, 150.5), (245.9, 90.6, 232.0); checker gaps 115.2 / 115.4 / 40.2.
  - **The binding check is EA_LOD's pink hue:** at −4.875 its B/G is 234.5 / 99.4 = 2.36 < 2.5. The cubes' last channel-255 pixel goes at −4.125 (at −4.0: 1 px each).
  - ⚠ EA_LOD's checker gap is 21.09 by Codex's fixed front-face patch (1 level above the floor) and 40.2 by my whole-silhouette split. Both pass; Codex's is the thin one.
- **Bench-only and runtime-only:** the settings file (`E:\IA_BuildCache\09444f-evidence\D3D11-1280\01\settings.json`) reads bloom 5→0, eye quality 0→2, method −1→3, exposure 0→−5, physical-camera exposure off on 1 volume, `custom_depth` 1 (read, never set). No fills were used. The session ran `-nowrite`. The Paris host's Content (1,456), Config (18), plugin Content (7), descriptor and `.uproject` are hash-equal before and after (`PROTECTED_SETS_EQUAL=True`).
- **Launch reserve:** 3 GiB (`calibration-prediction.json`).
- **FE3b children this run:** one, UBT `-Mode=ValidatePlatforms -allplatforms` under the game process (first seen 06:47:45 UTC, read after exit from the cache).

## 1–4. The stop, the Paris legs, G5 and the defaults
- **The stop:** the D3D11 1280×720 leg launched at 12:17:42 IST with `Europa.TextureBench … run -5`. It wrote 3 previews / 24 frames / 0 drops, then the reader refused before C5 (`calibration_reader09444f.py:52`: 0 usable checker pixels on every target).
  - **Measured:** the three target centres projected to (2556, 638), (2254, 1438), (3019, 1272) in a 1280×720 capture, against the sweep's (502, 436), (778, 436), (640, 384). I looked at both frames: the leg's frame is black with one lit floor wedge on the right; the sweep's shows the three cubes centred.
  - The two launch command lines differ only in their output paths. The helper sets `SetControlRotation(FRotator(-12,0,0))` in both modes (`EuropaTextureBench09444e.cpp:69`); the level holds an `EA_Camera` actor (probe "initialized" record in both sessions). ⛔ **Why the view differed is not established** (`G120`); Codex says the same.
- **Paris regression:** D3D11 1280×720 not scored (preflight refused, 0 fires, no C5 masks or annotation); D3D12 1280×720 and D3D12 1279×719 not run. So there are no new H4, texture G1–G3, `unavailable` or G5 per-row numbers.
- **G5 where recorded:** `r.CustomDepth` read 1, never set.
- **Defaults on local `983f429`:** blinking, lod_corruption, missing_texture and corrupted_texture; camera_clipping untouched and still compiled out. `d33689f..983f429` (`786a168`, `983f429`) touches only `tools/`, `docs/` and `CLAUDE.md` (16 files, +1,375/−3). Origin stays at blinking and lod_corruption.

## 5. Stop times (IST)
- No build ran (no build process in any poll).
- **Last launch:** 12:17:42. The game ended 12:18:19, the kit 12:18:24. My 12:20:34 poll sighted the game and its CrashReportClient inside its window, neither alive at its end; no process of ours at any later poll. Codex's identity audit at 12:24:16 found 0.
- **Both limits kept by a wide margin** (13:10 launches, 13:35 processes). My readers and final gates finished by 12:36.

## 6. Safety
- **"1 crash" in the kit's result is the kit's own stop, by my reading.** The kit recorded a stop intent for PID 23160 at 06:48:19.631 UTC with an empty `AssertionError` reason and ended the root 0.2 s later; `runtime_exit.py:16` labels an empty-reason exit a crash. Evidence: `Runtime.log` ends mid-run with 0 fatal lines; 0 new files in every crash, WER and dump location I check; no crash-report folder. Upload check PASS; the one crash/upload term is the command line's `bSendUnattendedBugReports=False`. The classifier and the receipt were left as they are.
- **Paris:** host and plugin Binaries unchanged (0 changes in every poll). Intermediate: 20 AssetRegistryCache files rewritten by the launch. Saved (36) unchanged.
- **Fixture:** nine assets, `Engine.ini` (`64d45f80`), `DefaultEngine.ini` and `.uproject` equal. No fixture launch or build.
- **Materials:** the 4.25 materials equal `8df75de`'s blobs at every ref through `983f429`; the 5.1 `.uasset`s equal d7eee97's in every copy and at every ref.
- **Deletions:** none. My tree lists of D:\IA_Europa, the run folder and E:\IA_Fixtures: 0 removed, 127 added, 35 changed (20 AssetRegistryCache, 6 plugin docs/tools, 6 run-folder files, `Boot.ddc`, the two `Launch\Europa425-Runtime` files the kit restored). 0 reparse points; P9 files unchanged; Codex's full P9 `main()` PASS (358.8 s).
- **m51 and refs:** `m51` passes at start, prelaunch, exit and final (HEAD `53bf725`, index `526aba7f`, 15 untracked). Only local feat moved, `d33689f` → `983f429`. origin stays `8df75de` (`ls-remote`). The spikes are local; the fixture worktree is detached at `8e59a56`, clean.
- **Reading, not a protected item:** C: free fell 24.1 → 21.5 GiB between my 12:14:49 and 12:20:34 polls (the game session) and stayed there. Cause not established.

## 7. Push and card
- **Nothing pushed, no card update.** No Paris gate passed. Every commit after `8df75de` sits on `8e59a56`, which adds the textures to the default routes, so nothing stands alone (GH4).

## Decisions for chat
1. **The camera view in run mode.** The helper's preflight came up facing away from the targets, with the same command line that gave a centred view in 094-44e's sweep. Should the next job make the helper verify the view (centres inside the frame) before the previews and retry or stop, and should it bind the view explicitly (for example to `EA_Camera`)? The cause isn't established.
2. **The kit's crash label.** An empty-reason owned stop is counted as a crash. Should 094-45/next correct `runtime_exit.py` (with its can-fail tests), while leaving this receipt as recorded?
3. **When the three Paris legs run:** after 15:15, with −5.0 EV frozen, or after a fresh calibration in run mode?
