# 094-35 — Code's short review (C5 unscored: the pre-pass dropped all six poses for three distinct reasons, and one of them is our own census churn)

**Date:** 2026-11-01, about 10:25 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:**
- Codex's report, collected byte-identical as `_reviews\094-35-codex-report.md` (sha256 `D8C4C88C…`). Codex exited 0 after 2,603 s (09:17:00 → 10:00:23 IST), one resume try. Verdict **NEEDS-DECISION**.
- Commits on `feat/europa-port`: `120493b`, `18235f3`, `54e11c9`, `2143215`.
- One street-map session, `C5\01` (09:37:12–09:45:39 IST).
- My receipts: `code-receipts-094-35\` and `D:\IA_Europa\Receipts\CodeReview09435\`. My scripts: `D:\IA_Europa\Temp\CodeReview09435\`.

## 1. The diff against the cited 5.1 code
- **Census floor and ceiling:** `AnomalyCensusPolicy425.h` carries 5.1's constants: floor 0.5 % and ceiling 25 %, both inclusive; headroom 8; batch `min(free/2, free−8)`; window `ticks+8`, at least 12.
  - It registers through `SetCensusProvider`.
  - The queue is rebuilt from the census prefilter at the current view on every cycle, so actors that come into view later are eligible.
  - Ledger exhaustion only counts; it never fails the session.
- ⚠ **Parity deviation.** 5.1 holds each batch until its own readback resolves (`AnomalyCensus.cpp:629-699`), and an event target keeps its tag for the event.
  - In 4.25, `FAnomalyMasks425::Advance` runs on every captured frame. It calls `FlushRenderingCommands`, then `RestoreAll`, then re-tags the live-fire targets plus a new batch of up to 27 actors.
  - So every tagged actor's custom-depth flag and render proxy are reset on every frame, including the event target's.
  - Section 2 shows this probably changes rendered pixels.
- **Ledger restore:** `Release` calls `AnomalyStencilTag::RestoreAll` and restores the saved write masks. This is the same path as 5.1's `AnomalyStencilTag.cpp:385-392`.
- **Player view:** forcing the first camera actor now needs `IAI.Capture.BenchCamera` (default 0) and `-IAIBench`. C5 captures the player controller's own view target (`DefaultPawn_0`).
- **Annotation schema:** `AnomalyAnnotation425.cpp` emits the same 56 keys, in the same order, as 5.1's `WriteSessionAnnotation` (5.1 line 1459). "Affected" means injected and target mask pixels above 0, which is 5.1's meaning.
  - ⚠ The values differ in three places:
    - `coverage_ratio` is written as observable/injected *frames* (5.1: screen share);
    - `bbox_source` is always `"drawn"`;
    - the camera `far` is a fixed 100000.
- **Off by default:** `IAI.Capture.C5` refuses without `-IAIBench`, and the street driver exists only under it.

## 2. The C5 session, from the raw files
All checks below follow rules I fixed before reading the results (in each script).

**Session basics:**
- 3,996 frames, 0 dropped.
- D3D11, format 18, 1280×720.
- AA 0, and `eye_adaptation_quality` 0 on every row.
- Exit 0.

**FR3 holds:**
- The protocol was written at 09:38:10.
- The references and unstable maps were written at 09:39:47.36–48.07, and `prepass-frozen.json` at 09:39:48.12.
- The first scored-pass frame (828) was written at 09:39:48.52. The code requests frames from 828 on only after the freeze.
- Codex's reader prints `references_frozen_before_scoring=false` only because `scored-start.json` is written when auto-injection first turns on, which never happened.

**The poses.** My recomputed shares equal the frozen ones on all six (40.58 / 41.53 / 26.20 / 36.35 / 38.94 / 41.33 %). The pose-3 unstable PNG equals my recomputation. **All six drops follow the 5 % rule.**

The causes, split label-free (`prepass-grain-*.json`, `prepass-period2.json`, `strip-pose3.json`):
1. **Tonemapper grain, period 7 frames.** Same-pixel changes drop from about 0.65 at lag 1 to 0.20–0.36 at lag 7. About 29 % of pose 3's pixels range by exactly 2 levels, so the frozen "max−min > 1" rule counts the grain itself as instability.
2. **A 2-frame flicker that tracks our census (pose 3).**
   - It covers rows 686–719: 35,420 px, 3.84 % of the frame.
   - Over 570 still frames, it alternates every frame in lockstep with which census-tagged actor wins custom depth there: the street `ProvaStrada5` or the wall `SM_MuroCasa5_LP2_73`.
   - The strip's mean colour is G 29.4 on the street's 285 frames and G 17.9 on the wall's 285.
   - With the grain and this period both removed (phase-locked at 14), pose 3 has **0.0 %** of pixels above 8 levels and 2.2 % above 1. It would have been kept.
   - This is consistent with draw-order z-fighting flipped by the per-frame proxy re-registration. **Not proven:** the same pose with the census off is the control.
3. **Real scene motion at poses 0, 1, 2, 4 and 5.** Even phase-locked, 12.7 % (pose 0) and 8.2 % (pose 2) of pixels change by more than 8 levels. The causes are wind materials (`SimpleGrassWind` in `MM_Basic01` and `MM_Bush`), the fog panner and the sky. The pose-0 map is white over whole sky and fog regions.

**The scored pass:**
- **Scored events:** 0 for each anomaly. Nothing fired; 0 rows carry an anomaly or a mask.
- C2, C3, IoU, precision, spill, completeness and both negatives were therefore **not measurable**. Codex's coverage fix (`2143215`) correctly stops the reader calling that a PASS.
- No masks exist to open. `c5-sheet.png` shows the pose-0 street, the pose-3 wall, and frames 2460 and 2461 with the bottom band switching.

**Census (`census-drawn.json`):**
- Distinct drawn actors within 12 still frames: 168, 125 and 116 at poses 0, 1 and 5. Of those, 36, 32 and 42 are inside the 0.5–25 % band.
- The wall covering 93 % of the frame is excluded by the ceiling.
- Ledger exhaustion is 0 on all 3,996 rows; tags run 200–227, with at most 27 census owners per frame.
- **No scored late-entering target**, since there were no events.
- The injector's own visible list peaked at 20.

**Custom depth (`rows-check.json`):**
- **Before:** 1,980 components, 2 of them on. `CasaEliminare_8` is on with value 203, inside the ledger's range. The DefaultPawn mesh is on with value 18 and write mask 2. `Foglietto_6` is off with value 19.
- **After:** equal to before on all 1,980.
- **Mid-session:** all 3,996 samples satisfy "on = tagged or already on", and untagged components never change.
- **The reserved value:** 203 never appears as an owner tag, and `host_reserved` is [203] on every row.
- ⚠ The census does tag the 203 actor itself, for one frame per cycle with value 200, and restores it after. 5.1 behaves the same way.

**The view:**
- On all 3,996 frames, the player's point of view equals the rendered view: location, rotation and FOV deltas are 0. The view target is `DefaultPawn_0`.
- The holds sit at exactly 0, 60, …, 300° yaw.

**The annotation:** the root block matches 5.1, but `anomalies` is empty, so live agreement between the affected/injected sets and the rows is unproven.

## 3. Safety
- **Crash: none.** Exit 0, `LogExit: Exiting.`, 0 error lines, no new crash folder (the only two are from 10-27). The kit's `crash_count` is 0.
- **Upload: none.** `upload-check` passes. The only upload-like line is the crash client's DataRouterUrl print.
- **The Paris host**, by my own hashes before against after: Content (1,456 files, including `SampleMap.umap` `9D35E89E…`), plugin Content (5), Config (18), the descriptor and the `.uproject` are hash-, size- and mtime-equal.
- **Git:**
  - `m51` equals P9 at start, prelaunch, exit and final.
  - master, release and the 30 tags are unchanged.
  - The feature branch is clean at `2143215`, with W4 dates, no origin branch and no tags.
- **Binaries:**
  - Only the host exe/pdb (plus `.exp`/`.target`) and `UE4Editor-AnomalyCapture.dll`/`.pdb` changed.
  - Every change came with UBT/cl/link alive, at BelowNormal, with 2 parallel actions.
  - The manifest is 27 of 27 equal.

## 4. What's left before an office capture smoke on Europa's fork
1. **Make the 4.25 census hold tags like 5.1:** keep each batch until its readback, and keep event targets tagged for the event. Then run pose 3 with the census on and off to prove or clear the flicker.
2. **Make the pre-pass workable on a real map:** grain-phase-matched references or tolerance, and either poses or regions without wind and fog motion, or a ruling on animated pixels. Then rerun C5 to get scored events.
3. **Only then** the FS3 office card.
