# 094-44e — Code's review: the EA_Core bench fix is in and runtime-only, but no exposure passes all of GJ3's pre-fire checks, so no Paris leg ran. Nothing pushed; the card is unchanged

**Date:** 2026-11-04, about 11:58 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Resume:** thread `01a14334`, detached, with W4 (the child saw no `GIT_*_DATE`), after a dry run of my gates.
- **Prompt:** the 094-44 request verbatim, then GJ1–GJ5 verbatim, then GI7 verbatim, then the GJ5 scope line, my context and the 094-43r implementer block (`resume-prompt-094-44e.txt`, 21,803 B, `049D60C0`).
  - ⚠ My context said the resume started "about 11:25 IST". It actually started at 11:10, so the stated time left was 15 minutes short. That is the safe direction.
- **Run:** 11:10:06 → 11:52:02 (2,516 s), exit 0, one try. Codex's verdict is **NEEDS-DECISION**.
- **Report:** collected byte-identical as `_reviews\094-44e-codex-report.md`.
- **My files:** receipts in `code-receipts-094-44e\`; readers in `D:\IA_Europa\Temp\CodeReview09444e\`.

## 0. GJ3: the EA_Core bench fix
- **Where it lives:** a new host-probe file, `Source\EuropaPhaseAProbe\EuropaTextureBench09444e.cpp`. It is behind `!UE_BUILD_SHIPPING`, `-IAIBench` and the exact `EA_Core` map. A byte-equal copy is committed as `tools/europa/EuropaTextureBench09444e.cpp` (blob `bca7e20e`).
  - Editor attempt 1 failed with C4668 (`UE_BUILD_SHIPPING` was read before its include). Editor attempt 2 and Game attempt 1 passed (FM1, BelowNormal, at most 2 actions).
- **Settings, recorded in `settings.json` and read back at runtime:**
  - `r.BloomQuality` 5 → 0;
  - `r.EyeAdaptationQuality` 0 → 2;
  - `r.EyeAdaptation.MethodOverride` −1 → 3;
  - `r.ExposureOffset` stepped from 0 to −8 EV in 0.125 steps;
  - physical-camera exposure off on the one post-process volume (in memory only);
  - pitch −12.
  - `r.CustomDepth` was read as 1 and never set.
- **Runtime-only:** the session ran `-nowrite`. The Paris host's Content (1,456 files), Config (18), plugin Content (7), descriptor and `.uproject` are hash-equal before and after. The Saved listing (36 files) is unchanged.
- **Pre-registered before launch** (`calibration-prediction.json`, 05:44 UTC):
  - the target mean in 51–204 (0.2–0.8 of full scale), with 0 saturated pixels (≥ 250);
  - checker contrast ≥ 40;
  - pink R,B ≥ 200 and G ≤ 80;
  - the first common qualifying exposure is frozen;
  - the launch reserve is 3 GiB (GJ4 restored).
- **The calibration:** 195 previews, 1,560 frames, 0 drops, no injector fires. The camera was static (all 195 projected target centres are identical).
- **Result: no exposure qualifies.** My recheck (`calib44e.py`) works from the raw preview frames, with silhouettes taken from the pink-vs-original difference (16,815 / 16,814 / 25,850 px). It finds **0 qualifying exposures**, agreeing with Codex:

  | Target | Pink R,B ≥ 200, G ≤ 80 | 0 saturated px | Mean in 51–204 |
  |---|---|---|---|
  | EA_HideCube | −3.0 to −3.75 | −5.125 and darker | −3.75 and darker |
  | EA_BlinkCube | −3.0 to −3.75 | −5.0 and darker | −3.75 and darker |
  | EA_LOD | −5.25 to −6.0 | −2.125 and darker | −2.625 to −6.25 |

  - **The cubes can't pass at any exposure.** At −3.75 their pink is (207, 46, 201), but about 2,796 of 16,815 target px (17 %) are saturated at baseline. By −5.125 nothing saturates, but the pink is (144, 27, 145).
  - **EA_LOD's pink window doesn't overlap the cubes'.** At −3.0 EA_LOD's pink reads (255, 198, 253): it is still in the tonemapper's white shoulder.
- ⚠ **A measurement difference with no effect on the outcome.** My checker contrast is a two-means split over the whole silhouette; Codex used a fixed front-face patch, declared before launch. By mine, EA_LOD alone passes everything at −5.25 to −6.0. By Codex's, EA_LOD's contrast reaches 40 only after its baseline drops below 51. Either way the cubes fail.
- **My reader can fail:** run on 094-44d's failed D3D11 leg, it reads targets at 0.948–0.975, 49 % saturated, checker gap 15.9 (0 on EA_LOD) and pink (255, 198, 253), and fails every check.
- ⛔ I haven't established why the cubes saturate on part of their silhouette. The numbers fit uneven lighting across faces (one face 250+ while the mean is about 200). The fixture avoided this with six shadowless fills (GI1), which GJ3 did not list for EA_Core.

## 1–4. G5, the Paris regression and the defaults
- **No scored leg ran.** The stop rule applied before any fire: D3D11 1280×720, D3D12 1280×720 and D3D12 1279×719 are all **unrun**. So there are no new H4, texture G1–G3 or `unavailable` numbers, and no G5 per-row state.
  - Calibration only: `r.CustomDepth` 1; 0 fatal lines; 0 `Default Material` lines.
- **Defaults:**
  - local `d33689f`: blinking, lod_corruption, missing_texture and corrupted_texture;
  - origin `8df75de`: blinking and lod_corruption.
  - camera_clipping is untouched and still `#if ANOMALY_ENGINE_51`.
  - `4cc8b45..d33689f` changes no Source, Shaders, Content, descriptor or Config: 20 files, all tools and docs.

## 5. Stop times (IST)
- **Builds:** 11:19:51 (Editor 01), 11:21:12 (Editor 02), 11:22:27 (Game 01); the last binary change was at 11:23:08.
- **Last launch:** the calibration session at 11:25:46. Its log closed at 11:39:48.
- **Processes:** none of ours were alive at my 11:40:18 poll. Codex's PID and creation-time audit at 11:42:46 found all five identities gone. Nothing of ours has run since.
- **Both limits kept by a wide margin:** 13:10 for launches and 13:35 for processes. My readers finished by 11:55.
- **FE3b:** one UBT child, `-Mode=ValidatePlatforms -allplatforms`, read at creation.
- **My polls:** 15 lines, 2.2–4.3 min apart. Every build and the session were sighted.

## 6. Safety
- **Crashes and uploads:** 0 crashes; no new crash, WER or dump files; upload check PASS; 0 fatal lines.
  - The one "crash/upload term" hit is the command line's `bSendUnattendedBugReports=False`, as in 094-44d.
  - CrashReportClient rewrote its own `Saved\Config` inis (known behaviour).
- **Paris:**
  - Host Binaries changed 7 files (`StylizedParisStreet.exe/.exp/.pdb`, two `.target` files, `UE4Editor-EuropaPhaseAProbe.dll/.pdb`), all while UBT was alive (5 timeline entries, 0 outside a build). Plugin Binaries are unchanged.
  - Host Source gained one file, the helper. Host Source isn't on the protected list; it is recorded here.
  - Intermediate changed (45 host entries, including AssetRegistryCache, plus 18 plugin build entries).
- **Fixture:** the nine assets, `Engine.ini` (`64d45f80`) and `.uproject` are equal. No fixture launch or build.
- **Materials:** the 4.25 materials equal `8df75de`'s blobs; the 5.1 `.uasset`s equal d7eee97's in every copy and at every ref.
- **Deletions:** none by command, except the three Makefile.bin cache refreshes Codex receipted during the authorized builds. My tree lists show 0 removed files in D:\IA_Europa, the run folder and E:\IA_Fixtures. 0 reparse points; P9 files unchanged; full P9 `main()` PASS (386.9 s).
- **Evidence location:** the calibration frames went to `E:\IA_BuildCache\09444e-evidence\`, to keep D:'s 3 GiB reserve. That folder is outside my tree-list roots.
- **m51 and refs:**
  - `m51` passes at start, prelaunch, exit and final.
  - Only local feat moved, `4cc8b45` → `d33689f` (`cfe8e06`, `ee56edc`, `cc8c2d5`, `d33689f`).
  - origin stays `8df75de` (`ls-remote`). The spikes are local, and the fixture worktree is detached at `8e59a56`, clean.
- **Gotchas:** G539 added; the next free number on feat is **G540**.

## 7. Push and card
- **Nothing pushed.** Every commit after `8df75de` descends from `8e59a56`, the default-route change, so none stands alone without the texture gates (GH4).
- **No pre-push scan was written**, because there was nothing to push.
- **The office card stays** `_reviews\094-43-office-card-europa.md` (`93613ae`; origin's defaults blinking + lod_corruption).

## 8. Decisions for chat
1. **GJ3's pre-fire checks can't all be met on EA_Core by exposure alone.** Options:
   - **(a)** Add GI1's fixture lighting recipe as more bench-only runtime settings on EA_Core: shadowless fills, with EA_Light reduced, so the cubes' faces light evenly. Recalibrate under the same pre-registered checks.
     - **I recommend (a).** It is GH2's own rule ("fix the fixture, never the gate"), and even lighting is the main lighting difference between the fixture that passes and EA_Core.
     - ⚠ Whether fills make EA_LOD's and the cubes' pink windows overlap is unmeasured.
   - **(b)** Change how the checks are read: a front-face patch for the saturation and pink checks, or per-object passes at one frozen exposure. This would loosen GJ3.
   - **(c)** The earlier options, previously not adopted: fixture-only whole-frame G3, or the textures out of the default pool.
2. **Timing.** A new job needs about 5 min to build (if the helper changes), about 15 min to calibrate and about 3 min per leg. Ruled before about 12:30 IST, it could still clear 13:10. Otherwise it waits for 15:15, alongside 094-45 (camera_clipping).
