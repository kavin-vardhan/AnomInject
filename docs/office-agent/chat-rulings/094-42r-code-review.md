# 094-42r — Code's review: the guard and routes are in and the EA_Core smoke passes by my numbers; the fixture authoring stopped on FF2 before any asset existed, so repro, guard and FZ6(b) are unmeasured

**Date:** 2026-11-03, about 12:30 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:** I resumed thread `01a14334` with `-OwnerApprovedFullAccess` (FZ8) at 11:11:05; a new turn started on the first try. Codex exited 0 at 11:51:20 (2,414 s) with `NEEDS-DECISION`. Its report is `_reviews\094-42r-codex-report.md`. My receipts, scripts and polls are in `code-receipts-094-42r\`.

## 1. The diff (`d1cdc7b..f7b32f0`: 8 commits, 18 files, +768/−34; source in 7 files)
- **FZ3.1, plants only on fixture `EA_` actors.** `AnomalyStreetBench425.cpp:48` skips every candidate unless `BenchFixture(World)` holds (`-IAIBench` plus the package is EA_Core, EA_Travel, or Outline425's `EA_Outline`; `AnomalyHostDepth425.cpp:93-99`) and the actor carries an `EA_` tag (`:100-105`). The old `N==3` start condition is gone. In the smoke, the planted components are exactly `EA_BlinkCube`, `EA_Floor` and `EA_HideCube`. `DefaultPawn_0` is no longer planted.
- **FZ3.2, the scanner's results.** `Classify` (`AnomalyCaptureAdmission425.h:9-12`) returns reader if any reader, else unknown, else writer, else none. The scan covers every PostProcessVolume, the cached camera blends and the camera POV settings (`AnomalyHostDepth425.cpp:38-73`). A non-material blendable or a missing shader map counts as unknown; zero-weight readers are kept (`4fdfe84`). The native test asserts all four classes and the bench-only bypass (`tools/europa/capture_fz425_test.cpp`).
- **FZ3.3, refusal before any tag.** In `FAnomalyCaptureLabels425::Start`, `Admit` runs at line 89, before `Street->Start` (`:97`, the plants) and `MaskOwner->Start` (`:100`, `EnableCustomStencil` plus census and event tags). C4 and C5 both reach it through `AnomalyCaptureSession425.cpp:42`. `Admit` refuses on reader or unknown, logs `C5 refused: host custom-depth <result>`, requires zero ledger tags, and never touches the volumes. Bypass needs `-IAIBenchBypassHostDepth` on a recognized fixture. **Limit:** the scan runs once, at start, so a reader added later is not seen; the receipt's own scope string says so.
- **FZ4.** The default is `blinking,lod_corruption` (`Admission425.h:20`). An explicit list must be non-empty, have no duplicates and lie inside `GetPoolIds()` (4.25-supported ids), or the whole capture refuses (`Labels425.cpp:58-73`; the tests cover `""`, `unknown`, a trailing comma, a double comma, a duplicate and a mixed list). The routes are logged (`Capture425 routes=…`), written to `routes.json` and to the labels receipt. C4 takes them as a 4th argument, C5 as a 3rd. The kit passes them through a stock `exec` macro so `-ExecCmds` cannot split on the commas.

## 2. EA_Core smoke (slot 01) by my own checker (`smoke_check42r.py` → `smoke-check42r.json`)
| Gate | My reading |
|---|---|
| Frames | 2,352 PNGs, indices 0..2351, all 1280×720, 0 zero-byte; 2,352 label rows join 1:1 |
| Routes | `routes.json` = blinking, lighting_mismatch, lod_corruption, missing_object; the log line is identical |
| Fires (≥3 each) | blink 22 · lighting 13 · LOD 19 · missing 15 (69 events) |
| Masks | 731 affected frames = 731 mask PNGs; 0 clean frames with a mask; 0 empty masks; PNG pixel counts = row `mask_pixels` on all 731 |
| LOD changes pixels | all 19 events: minimum 14,456 changed in-mask pixels per frame (>2 levels against a clean frame of the same pose and grain phase) |
| Annotation | schema 2.1; sets ⊆ and counts agree; far = 1,000,000 on 69/69; `coverage_pct` 0–2.90 on 56; −1 on the 13 lighting events, whose provenance is `valid:false` (the unmeasured sentinel the reader accepts) |
| Census and depth | lost/no-pass/polluted/exhausted 0 on every row; tag 203 never a census tag; 0 unexpected; 0 untagged host-component changes; depth-before = depth-after |
| Scanner | `none`, readers/writers/unknown 0, admitted, no bypass, 0 tagged after the scan, `r.CustomDepth` 1 at the scan |
| View and UI | player view = captured view on 2,352/2,352; UI marker in 0 frames; 8/8 controls positive; SELF_CHECK start and 300 lines present (D3D11, format 18) |
| Exit | the log ends `Exiting`; the crash-report client's own log reads `Editor/ExitCode:0 … NoTempCrash`; upload check: 0 hits in both logs |

Builds (FM1, BelowNormal, two parallel actions): Paris Editor attempt 1 and Game attempt 1, both exit 0 at `4fdfe84`.

## 3. The fixture
- **Where:** `E:\IA_Fixtures\Outline425\`, outside every protected tree. Its plugin is a detached worktree at `4fdfe84`, clean.
- **Authoring script:** `Scripts\author_outline425.py`, SHA-256 `6E4174A1…44891C` (my hash; it matches Codex's).
- **Builds:** Editor attempt 1 failed on a fixture-only line (exit 6, `R.ToSharedRef()`); attempt 2 passed. Game attempt 1 passed. FM1 budget used: Editor 2/3, Game 1/3.
- **The stop:** the first authoring launch's argv had no `-nowrite`, while the Paris smoke argv has it. At 11:39:51 the engine wrote `Saved\Config\Windows\Engine.ini` (1,502 B: default `[Core.System] Paths` and accessibility keys). The kit stopped the editor and monitor it owned: `FAIL`, "FB2 protected file changed during session", exit 125. There was no crash and no upload hit. In 4.25, `-nowrite` gates only config writes (`ConfigCacheIni.cpp:1478`), so adding it does not block package saves.
- **Repro gate:** NOT MEASURED. **Guard gate:** NOT MEASURED. There are no fixture assets, and no repro or guard session ran.

## 4. The spike
- **(a):** I opened all 39 cited ranges in `E:\EpicGames\UE_4.25` (`citecheck-fz6a.txt`). Every claim holds once read; the 7 token misses were my own literal choices.
  - The depth-only types are registered by name (`DepthRendering.cpp:76-82`) and compile for the default material, masked, WPO and translucency-writing-custom-depth materials (`DepthRendering.h:74-79`).
  - The engine draws opaque without WPO with the default material's position-only VS (`.cpp:973-981`), and masked or WPO with the material's own `TDepthOnlyVS<false>` (`:983-998`).
  - The lookup is public and exported: `FindShaderTypeByName` (`Shader.h:2195`), `FShaderMapContent::GetShader` (`:1765-1771`), `GetMeshShaderMap` (`MaterialShared.h:1193`). The `FMaterial::GetShader` overload at `:1992` is under `private:` (`:1908`), as Codex says.
  - Both shader classes' `GetShaderBindings` are inline in the private header (`DepthRendering.h:82-92`, `:183-193`) and forward to the exported `FMeshMaterialShader`.
  - **Verdict: buildable by source, with no new shader type.** Not compiled. One link risk remains for the modular editor: no template may touch the shaders' unexported `StaticType`. There is also an 8-bit ID cap per pass, and tessellated materials need HS/DS.
- **(b):** NOT RUN; no numbers. No spike branch exists (local refs, worktrees and `origin` checked), so none is merged or pushed.

## 5. Safety: none
- **Crash and upload:** no new crash folder (Paris `Saved\Crashes`, engine, `%LOCALAPPDATA%`); upload hits 0 in all four session logs.
- **Paris host:** Content 1,456, plugin Content 5, Config 18, the descriptor and the `.uproject` are hash-equal before and after (my lists). `Saved\` received no file.
- **Binaries:** they changed only while UBT ran (11:17–11:21 Paris; 11:33–11:38 fixture).
- **`m51`:** equal to P9 at start, at both prelaunches and at exit (HEAD `53bf725`, index `526aba7f`, 15 untracked). `master` `b5f15a3`, `release/2026-10-05` `d7eee97`; tag refs are hash-equal.
- **Deletions:** 0 files removed across `D:\IA_Europa`, the run folder and `E:\IA_Fixtures`.
- **No push:** `ls-remote` shows `feat/europa-port` = `d1cdc7b`; local = `f7b32f0` (8 unpushed commits). No foreign load was seen.
- **Cosmetic:** the killed 094-42 attempt wrote no ledger row, so `…-resume47` is absent from `ledger.csv`; this run is `…-resume48`.
- **Observation:** the kit's runtime admission now has no deadline (`DEADLINE=None`), so a blocked gate waits without bound, as in 094-42.
