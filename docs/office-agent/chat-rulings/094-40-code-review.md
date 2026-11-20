# 094-40 — Code's short review (FX2 and FX3 hold; the EA_Core smoke passes every gate by my own numbers; FV6 verified; pushed `d1cdc7b`)

**Date:** 2026-11-02, about 05:05 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:**
- **Codex's report:** collected byte-identical as `_reviews\094-40-codex-report.md` (sha256 `0BEC402D…`). Exit 0 after 1,612 s (04:06:10 → 04:33:03), one resume try, thread `01a14334`. Verdict **GREEN**.
- **Commits:** `08fe652` (FX2/FX3, built) and `d1cdc7b` (docs); after FV6 the built commit is `252b335`.
- **My receipts:**
  - `code-receipts-094-40\`: gates, Paris lists, tree lists, polls, push receipt, `prepush\`.
  - `D:\IA_Europa\{Receipts,Temp}\CodeReview09440\`: `review-01.json` (my gate recompute), `masks-sheet-01.png`, `binscan.json`, `fv6-check-b.json`, `rootnames-new-vs-base.json`.

## 1. The diff
- **FX2: correct.**
  - The bench's `Wanted` is now `!Tail && (Started || Eligible)` (`AnomalySessionRun425.h`, used at `AnomalyCaptureLabels425.cpp:93`). Once the first scored hold starts the injector, it keeps running across the remaining holds and turns, and stops 24 frames before the end.
  - The seed is still set once in setup (`SetSeed(CaptureSeed425)`), and the shared 5.1 `SetRunning` is unchanged.
  - **At runtime:** exactly one `IAI.Auto.Run -> ON (seed 4242…)` and one OFF. The 69 draws are numbered 1–69 with no restart, and the draw sequence no longer repeats a block of eight.
- **FX3: correct.**
  - `AnomalyAutoLod425::ForcedModel` returns the coarsest model unless the coarsest is drawn, then model 1 (LOD0). It refuses fewer than 2 LODs and unknown predictions.
  - `PredictDrawnLod425` builds the player's `CalcSceneView` on the game thread and calls stock `ComputeStaticMeshLOD`, honouring min LOD, the strip-min-LOD editor emulation, the current first LOD, `ForcedLodModel`, `r.ForceLOD` and `r.StaticMeshLODDistanceScale`.
  - It runs only on 4.25 auto picks without an explicit index; FW3's ≥2-LOD guard is kept.
  - **The 4.25 citations check out against `E:\EpicGames\UE_4.25`:**
    - `SceneManagement.cpp:451-461` is `ComputeBoundsScreenRadiusSquared`, and `:543-568` is `ComputeStaticMeshLOD`.
    - `StaticMeshRender.cpp:151-174` is the proxy's clamped min LOD, and `:2081-2150` is `GetLODMask` (force / forced model / show-flag / inverse distance scale / first-LOD clamp).
  - **Two small differences from the proxy, both harmless here:** the proxy also clamps min LOD to the first LOD with vertices, and it has a dithered two-sample path.
  - Logged as a deliberate 4.25 addition in `09438-auto-parity.json` (`09440_amendments`), with G510/G511.
- **The rebuilt targets contain both.**
  - Editor attempt 1 (56 s) and Game attempt 1 (75 s) built `08fe652`, exit 0, 0 errors, BelowNormal, 2 parallel actions.
  - The new `UE4Editor-AnomalyInjector.dll` (`46d212e0…`) and the exe (`6d402db1…`) contain the `Auto.LodChoice` format and `engine-screen-size-game-thread` (UTF-16). FX2 adds no string; its proof is the single run start above.
  - The 90 added source lines contain no comments.

## 2. The smoke, from the raw files (my own numbers; they equal Codex's)
**The map: EA_Core, as FX6 requires.** Slot 01 ran 04:20:20–04:22:26 IST. It produced 2,352 frames, 0 dropped, at 1280×720 (D3D11, format 18, AA 0, eye adaptation 0), with a normal exit. **The selection floor was 0.5 %, the bench-only value** (`IAI.SetMinScreenCoverage 0.5` in the argv, echoed `0.50% (cull ON)`). The retry slot is unused.

| Gate | My result |
|---|---|
| Fires ≥ 3 each | PASS: blinking 22 (9 on `EA_HideCube`, 13 on `EA_LOD`), lighting 13, LOD 19, **missing_object 15** (10 on `EA_LOD`, 5 on `EA_HideCube`) |
| Every LOD event changes target pixels | PASS: **19 of 19**. Every event predicts `drawn=1 count=2` and forces model 1 (LOD0, the sphere). On all 13 affected frames of every event, **14,455–14,475 of 15,022** target-mask pixels differ by more than 2 levels from the immediately preceding capture. The minimum per-event maximum is 14,462 |
| Masks | PASS: 731 affected frames, each with target-tag pixels (blinking 132, lighting 157, LOD 247, missing 195); the PNG equals the raw mask channel on every one. 1,621 clean frames have no mask PNG and 0 raw mask pixels |
| Annotation | PASS: schema 2.1; affected/injected sets equal the rows on all 69 events; far plane 1,000,000 on every event. Coverage: 56 measured; the 13 lighting events are `-1` with `valid=false` |
| Census | PASS: peak 3 drawn; 0 exhausted, lost, no-pass or polluted. The wall recompute reads 0 changed pixels |
| Custom depth | PASS: 14 components equal before and after; 0 mid-session failures; host-on `EA_BlinkCube` and `DefaultPawn_0` (203) never tagged or changed |
| View | PASS: player view on 2,352 of 2,352 frames (yaw 0, pitch −12, `DefaultPawn_0`) |
| Self-check + UI proof | PASS: 2 lines (start, 300), equal to Codex's verbatim text. Requests 1–8 at sample frames 1676–1683: each control shows 8/8 marker cells and its paired capture 1/8 (the black cell on black); every capture reads 1/8 |
| Seed | PASS: one `IAI.Auto.Seed -> 4242`, one Run ON |

- **The masks I opened** (`masks-sheet-01.png`):
  - **blinking:** exactly the hidden left cube (17,051 px).
  - **lighting:** floor, left cube and centre cube (356,721 px). The right cube darkens but stays outside the mask, because `EA_BlinkCube` owns custom depth (FX5, accepted).
  - **LOD:** the forced sphere (15,022 px), visibly a sphere where the reference shows the cube.
  - **missing_object:** the hidden centre cube's would-be silhouette (25,850 px); the cube and its shadow are gone in the frame.

## 3. FV6
- **Backup ref:** `refs/backup/europa-pre-fv6-09440` → `08fe652`. It is local only: not in `ls-remote`, and not contained in any remote-tracking branch.
- **The rewrite, pair by pair:**
  - 39 commits from `4f46a9c` are unchanged; 26 are rewritten (`47a6972`…`08fe652`).
  - Each rewritten tree differs from its original **only** in `docs/europa/09434-live-capture-receipts.json`, with the same author, date, message and parent count. 0 violations.
  - The new tip `d1cdc7b` adds only `CLAUDE.md`, `09440-office-capture-receipts.json` and the close-out journal.
  - `Source`, `Shaders` and the descriptor are equal between `08fe652` and `d1cdc7b`.
- **Drive-root names, against 189 non-generic names (my step-0 roots list):**
  - The pre-rewrite blob `c3741a56…` (control) holds all 189.
  - The only version left in the branch (`91427e49…`) holds 4, each an ordinary path word: `/Game/…`, `.cpp`, `builds\`, `"config"`.
  - Across all 445 changed blobs since `4f46a9c`, no file holds more than 7, and those are common words. Names new relative to `4f46a9c` and origin are only ordinary words (Builds, Captures, Config, Cpp, Game, Logs, Misc, Models, Python313, renders).

## 4. Safety
- **Crash:** none.
  - Exit 0 with `LogExit: Exiting.`; 0 Fatal / Assertion / GetLastError / Ensure lines.
  - The crash client reports `Editor/ExitCode:0`, `NoTempCrash`.
  - No new `Saved\Crashes` entry (latest 10-27).
- **Upload:** none. The only matching runtime-log line is the command line. The crash-client log shows only startup, the configured DataRouter URL and the monitor; the kit's upload check passes on both logs.
- **The Paris host, by my own hashes before against after:** Content (1,456 files), plugin Content (5), Config (18), the descriptor and the `.uproject` are hash-, size- and mtime-equal.
- **Binaries:** only `StylizedParisStreet.exe/.pdb/.exp/.target`, `UE4Editor-AnomalyCapture/Injector.dll/.pdb` and the editor `.target` changed, and every mtime falls inside a build window.
  - Editor: 04:16:12–04:17:09.
  - Game: 04:17:36–04:18:51; the last file is written at 04:18:50.
  - My 04:19:32 sighting is a late poll of those writes.
- **Git:**
  - `m51` equals P9 at start, prelaunch, exit, pre-push and final.
  - `master` `b5f15a3`, `release/2026-10-05` `d7eee97` and the 30 tags are unchanged.
  - The feature worktree is clean at `d1cdc7b`.
- **Deletions:** 0 files and 0 directories gone under `D:\IA_Europa` and the run folder (154,046 → 162,021 files); 0 reparse points.
- **Foreign load:** none of note. No foreign UE or build process; minimum free commit 6.99 GiB in the session and 6.39 GiB in the Editor build (kit readings; my polls read 7.3 at their lowest).

## 5. Pushed and written
- **Pushed:** `git push origin feat/europa-port` → `* [new branch]`. `ls-remote` = `d1cdc7b956db…`, equal to the local tip; no tags, no backup ref, no force.
- **Written:** `_reviews\094-40-prepush-scan.md` (clean) and `_reviews\094-40-office-card-europa-capture.md` (pinned to `d1cdc7b`).
- ⚠ **Not yet exercised at home:** the card's office route runs at the shipped 6 % floor on a real level. At 6 %, 094-39's EA_Core slot fired lighting only. A `0` count for an anomaly at the office is therefore listed in the card as a finding, not a stop.
