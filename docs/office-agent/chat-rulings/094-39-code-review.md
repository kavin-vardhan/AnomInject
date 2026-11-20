# 094-39 — Code's short review (the builds pass, FW3/FW4 hold at runtime; the EA_Core smoke fails one gate: missing_object never fired. No FV6, scan or card)

**Date:** 2026-11-02, about 02:55 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:**
- **Codex's report:** collected byte-identical as `_reviews\094-39-codex-report.md` (sha256 `11CD60A4…`). Exit 0 after 5,044 s (01:08:35 → 02:32:39), one resume try. Verdict **NEEDS-DECISION**.
- **Commits:** 3 on `feat/europa-port`, `8bdcb4b..0621027`. `d25b8eb` changes source, `d8f6ebf` changes only the kit's fallback argv, and `0621027` is docs.
- **My receipts:** `code-receipts-094-39\` (gates, Paris lists, tree lists, polls) and `D:\IA_Europa\{Receipts,Temp}\CodeReview09439\`: `review-01.json` and `review-02.json` (my own gate recompute), `masks-sheet-02.png`, `binscan-fv6.json` and `rootnames-check.json`.

## 1. The diff
- **FW3: correct.** An auto `lod_corruption` candidate now needs a component with `HasMultipleLods` (`AnomalyAutoInjectorSubsystem.cpp:453-459`). The auto apply also skips single-LOD components (`Anomaly_LodCorruption.cpp:57`). This is the same ≥2 predicate as `lod_popping`, and `WorstLodSentinel` is unchanged. An `Auto.LodTarget` log line records each pick.
  - **It ran in both directions:**
    - slot 01 rejected the only candidates: `0 of 2 candidates eligible - single_lod 2`;
    - slot 02 accepted `EA_LOD` 12 times, each `forced LOD 2 of 2`.
- **FW4: correct.**
  - The marker is drawn only on a frame whose `GFrameCounter` matches a registered request (the first 8).
  - A render-queue breadcrumb carries the request into the post-UI backbuffer control.
  - Each control is joined to its pre-UI capture by RequestId and SampleFrame.
  - `tick` and `tick_type` carry `*_source=build_constant`, and `tick_calls` stays a read.
  - The reader fails a shifted, missing or leaked pair.
- **FW5:** listed in `09438-auto-parity.json` (`09439_amendments`), with G506 for FW2 and FW3.
  - ⚠ The list names the interval, the hold and the concurrency, but **not the per-hold restart**. That restart is what failed this smoke (§2).
- **The Editor build contains 4dbcf9b.** `4dbcf9b` is an ancestor of `d25b8eb`, which is what was built. The new `UE4Editor-AnomalyCapture.dll` (`f840b2b0…`, 01:22 IST) contains `ui_paired`, `tick_source=build_constant` and `drawn_frames` (UTF-16, 1 each), and `UE4Editor-AnomalyInjector.dll` contains the `Auto.LodTarget` format.
- **The builds (all BelowNormal, 2 parallel actions):**
  - Editor attempts 1 and 2 hit FM1's memory stop (minimum free commit 1.2 and 2.9 GiB). A foreign UE 5.7 DCDemo `BuildCookRun` was running then.
  - Editor attempt 3 and Game attempt 1 passed, with 0 errors.
  - Both are spent for this job.
- **`d8f6ebf` (the retry fix):** it adds `IAI.SetMinScreenCoverage 0.5` to the fallback argv only. The log echoes `-> 0.50% (cull ON)`.
  - 5.1 applies the same 6 % bounds cull before the census (`d7eee97` `AnomalyAutoInjectorSubsystem.cpp:358` → `GetVisibleRenderableActors`; default 6.0 at `AnomalyViewport.cpp:54`). So 6 % is parity, and **the passing anomalies in slot 02 were obtained at a non-default selection setting.**
- **Comments:** none in the added source lines.

## 2. The smoke, from the raw files (my own numbers; they equal Codex's)
**The map: EA_Core (fallback).** The street map never reached 12 GiB free commit before the cutoff: the build pair finished at 01:25:09 IST, so the cutoff was 02:10:10 IST (+45 min), after a 2,470 s wait. The fallback was admitted at 10.2 GiB.
- Slot 01: 02:10:15–02:12:43 IST.
- Slot 02: 02:19:40–02:21:44 IST.
- Each session: 2,352 frames, 0 dropped, D3D11, format 18, 1280×720, AA 0, eye adaptation 0, normal exit.

| Gate (slot 02) | My result |
|---|---|
| Fires ≥ 3 per anomaly | **FAIL**: blinking 30 (18 on `EA_HideCube`, 12 on `EA_LOD`), lighting 6, LOD 12, **missing_object 0**. Slot 01: lighting 12 only. |
| Masks | PASS: 414 affected frames, every one with target-tag pixels (blinking 180, lighting 78, LOD 156); the PNG equals the raw mask channel on all of them. 1,938 clean frames have no mask PNG and 0 raw mask pixels. |
| Annotation | PASS: schema 2.1, affected/injected sets equal the rows on all 48 events, far 1,000,000 on every event. Coverage: 42 measured (blink 1.99 / 2.90, LOD 2.90) and 6 lighting `-1` with `valid=false`. |
| Census | PASS (fallback): peak 3 drawn; 0 exhausted, lost, no-pass or polluted. Over 55 and the wall strip are waived. My wall recompute still reads 0 changed pixels. |
| Custom depth | PASS: 14 components equal before/after; 0 mid-session failures; the host-on components `EA_BlinkCube` (18, mask 2) and `DefaultPawn_0` (203) never tagged or changed. |
| View | PASS: player = view on 2,352/2,352 frames (yaw 0, pitch −12, `DefaultPawn_0`). |
| Self-check + FW4 | PASS: 2 lines per session. Requests 1–8 at sample frames 1712–1719: each control has 8/8 marker cells, and each capture of the same request and frame has 1/8. That 1 is the black cell on the black backdrop; all 2,352 captures read 1/8. |
| FW3 | PASS (see §1). |

- **The masks I opened** (`masks-sheet-02.png`):
  - **blinking:** exactly the hidden left cube's silhouette (17,051 px).
  - **LOD:** the centre cube (25,850 px).
  - **lighting:** floor, left cube and centre cube (356,721 px). ⚠ The right cube visibly darkens but is **not** in the mask. It is `EA_BlinkCube`, which owns custom depth, and host-owned actors are never tagged (G498). So the lighting mask under-covers by one receiver.
  - missing_object: no event, so there is no mask to open.
- **The LOD visible change (information only):** 12 of 12 events show **0 target pixels changed** by more than 2 levels, on all 13 frames each. The forced-LOD frame looks identical to its reference.
  - At this distance auto-LOD already shows LOD1 (the cube; LOD0 is the sphere), so forcing the coarsest LOD is a no-op.
  - ⚠ **FW3's ≥2-LOD guard is necessary but not sufficient:** a target already drawn at its coarsest LOD still shows nothing (D3's class).
- **Why missing_object never fired: confirmed in source.**
  - 5.1's `SetRunning(true)` calls `Stream.Initialize(Seed)` (`d7eee97` `AnomalyAutoInjectorSubsystem.cpp:231`).
  - The 4.25 bench driver toggles running for every scored hold (`AnomalyCaptureLabels425.cpp:92-100`).
  - On a stationary view the six holds therefore replay one identical 8-draw block: LOD, lighting, blink, blink, blink, LOD, blink, blink.
  - On the street map the views differ, so the same sequence lands on different actors (094-36 fired missing_object 4 times).

## 3. FV6: not run (it needs a passing smoke)
- No backup ref and no `refs/original`. `4f46a9c`, `47a6972`, `07c853e` and `8bdcb4b` are all ancestors of `0621027`.
- The listing blob `c3741a56…` is unchanged; it is now carried by **25 commits** (`47a6972..0621027`).
- The three new commits add no drive-root listing. My scan matches only common path words (Builds, Game, Program Files, Cpp…).

## 4. Safety
- **Crash:** none. Both sessions exited 0 with `LogExit: Exiting.`, with 0 Fatal / Assertion / GetLastError lines and no new `Saved\Crashes` entry.
  - The kit adopted crash monitor 12968 for editor 22152 and 50908 for 44440.
  - The three LocalAppData CrashReportClient logs from tonight are UE 5.7 (`Release-5.7`), so they are foreign.
- **Upload:** none. The only matching line in each runtime log is the command line; the crash-client logs show only startup.
- **The Paris host, by my own hashes before against after:** Content (1,456 files, including `SampleMap.umap` `9D35E89E…`), plugin Content, Config, the descriptor and the `.uproject` are hash-, size- and mtime-equal.
- **Binaries:** 7 change events (18 file changes), every one with our UBT/cl/link alive. Codex's hashes match mine for the host exe and both editor DLLs.
- **Git:**
  - `m51` equals P9 at start, prelaunch and exit.
  - The feature branch is clean at `0621027`: 64 commits since `4f46a9c`, no origin branch, no push, no tags.
- **Deletions:**
  - files: one, a UBT `.cpp.txt.tmp` during an authorized build;
  - directories: one, the empty `Intermediate\DatasmithContentTemp`, removed by the engine during a session;
  - reparse points: 0.

  Codex's partial-build outputs are in `partial-build-quarantine` by receipt.
- **Foreign load:**
  - processes: UE 5.7 DCDemo `BuildCookRun` and its Shipping games, wlanext 6.2 GB, vmmem 4.0 GB;
  - minimum free commit: 0.8 GiB at 01:37;
  - the idle timer was reset by input until about 02:05.

## 5. Not written
The pre-push scan and the office card, because the smoke did not pass.

**Items for the card when it is written:**
- the self-check reads (`tick`/`tick_type` are constants);
- the selection prefilter setting (6 % default vs 0.5 %);
- the expectation that a LOD event can be invisible;
- a continuous scored hold, so that all four anomalies get a chance to fire.
