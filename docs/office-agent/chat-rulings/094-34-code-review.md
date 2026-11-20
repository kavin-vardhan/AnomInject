# 094-34 — Code's short review (C4 passes on my own numbers at both resolutions; FR2 holds; the static scan is clean; the binding is still session-start and scene-scale limited)

**Date:** 2026-11-01, about 08:40 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:**
- Codex's report, collected byte-identical as `_reviews\094-34-codex-report.md` (sha256 `5C3267B5…`). Codex exited 0 after 3,591 s (07:06:48 → 08:06:39 IST), one resume try. Verdict **GREEN**.
- Commits on `feat/europa-port`: `f16256b`, `93fec56`, `455506e`, `47a6972`.
- Four sessions: 1280×720 `01` (failed) and `02` (accepted); 1600×900 `01` (stopped) and `02` (accepted).
- My receipts: `code-receipts-094-34\` and `D:\IA_Europa\Receipts\CodeReview09434\`. My scripts: `D:\IA_Europa\Temp\CodeReview09434\`.

## 1. FR2, from the kept 094-33c files (`fr2-rescore.json`)
Completeness is the share of the union of receiver silhouettes (stencil 128–131, front-most custom depth) that the mask covers, across 42 labelled lighting frames:

| Mask | Completeness | Frames ≥ 0.95 |
|---|---|---|
| The real mask | 1.000 | 42 of 42 |
| Best single object (tag 131) | 0.4401 | 0 of 42 |
| All but the smallest (drop tag 128) | 0.8406 | 0 of 42 |

The real mask has 0 pixels outside the receivers. **FR2 holds**, and the numbers equal Codex's.

## 2. FR4: the diff and the static check
**The three 5.1 citations match the source** at `47a6972`:
- `AnomalyCaptureSubsystem.cpp:1686-1690`: the arm reads `Auto->GetLiveFires()`;
- `AnomalyStencilTag.cpp:75-91`: `IsAssignable`/`IsFree` over HostReserved, EventClaimed, CensusClaimed and Quarantined;
- `:4378-4392`: the census registers through `Auto->SetCensusProvider`.

**What changed on 4.25:**
- `AnomalyMasks425` takes each live fire's `TargetActor`. Lighting's receivers are the tagged actors that a light on the target `AffectsPrimitive`.
- Values come from the shared ledger, 200 upward and skipping host-reserved values.
- The shader selects by a 64-bit set relative to `ReservedStencilBase`.
- Every size comes from the view or viewport.
- Fixture tags, names, the `EA_Travel` hop and the 128–131 range are gone.

**My scan of `47a6972`** (`git archive`, 168 Source and Shader files): **0 fixture names; no fixed mask tags.** The remaining hits are unrelated:
- a 10-bit midpoint `static_assert` (128);
- a "128 MiB" help string;
- a reason code (`ReasonCaptureUnpaired = 128`);
- 5.1's 1920×1080 no-viewport fallbacks in stuck_low_mip and TexCorrupt, which are outside the smoke set.

Against that, the base `03bdef0` had 15 fixture-name hits and 13 fixed-tag hits.

**One reader change after a result.** `455506e` (07:47) changed the wrong-target negative, after the 1280×720 retry had been scored at 07:39. The control must now be a non-empty silhouette; before, it could pick an empty one and "fail" vacuously. That tightens the check rather than loosening it, and the session was re-read under it.

## 3. The two C4 sessions, from the raw files (`c4-independent-*.json`, `c2-label-free.json`)
**Both sessions:**
- 1,440 frames, 0 dropped, D3D11, format 18, `r.EyeAdaptationQuality = "0"` (Console) at frame 1, AA 0;
- 58 events, all complete;
- PNG equals the GPU evidence on 621 of 621 frames; my reduction table equals the file on 1,440 of 1,440;
- the row `mask`/`mask_state` is consistent on 1,440 of 1,440;
- mask tags are a subset of the event's tags on 621 of 621;
- **819 clean frames with 0 mask pixels and no PNG.**

**C2, by my label-free check.** The rule, fixed before I read the result: a frame is pixel-active if more than 0.1 % of its pixels differ by more than 8 levels from session frame 0.
- **0 mismatches** in each session, with 154 edges equal to the labels (38 of them interior blink toggles).
- Clean frames: max **0** changed pixels. Active frames: at least 123,118 (1280×720) and 194,018 (1600×900). So no revert leaves residue.
- All 116 per-event edge-shift negatives fail in each session.
- ⚠ My first split (the largest gap) cut between the LOD and lighting change sizes and reported 478 mismatches. That was my classifier's error, not the capture's, and the frame-0 series shows it.

**Masks**, scored against the frozen FQ2 gates plus FR2, with both a frame-0 and a pre-apply reference. The two references agree to within 0.001, and no frame fails under either:

| Anomaly | Events (720 / 900) | Labelled frames | IoU | Precision 720 | Precision 900 | Spill | Lighting completeness |
|---|---|---|---|---|---|---|---|
| blinking | 19 / 19 | 114 | 1.000 | 0.9388–1.000 | 0.9374–1.000 | 0 | — |
| lighting_mismatch | 11 / 11 | 143 | 1.000 | 1.000 | 1.000 | 0 | 1.000 |
| lod_corruption | 16 / 16 | 208 | 1.000 | 0.9786–0.9794 | 0.9798–0.9803 | 0 | — |
| missing_object | 12 / 12 | 156 | 1.000 | 0.9388–0.9789 | 0.9370–0.9857 | 0 | — |

The precision columns use the pre-apply reference, and the labelled-frame counts are the same in both sessions.

**Negatives:**
- **Mask shift:** 154 frames are inconsistent each way, in each session.
- **Wrong target:** another owner's non-empty silhouette passes on 0 frames, for blinking, missing_object and LOD. For lighting, the largest single receiver and all-but-the-smallest pass on 0 of 143 frames.

**Same seed (4242):**
- The accepted 1280×720 and 1600×900 sessions give the identical 58 (anomaly, target) sequence. The 1280×720 run's Auto.Fire log equals its receipt.
- 1600×900 attempt 1, which ran on the same binary as the accepted 1280×720 session, matches the first 46 events before its stop.
- Only 1280×720 attempt 1 (pre-census binary) diverges, at event 3, on `DefaultPawn_0`.
- ⚠ The accepted pair differs in resolution, and in the write-queue bound only (720 on `93fec56`, 900 on `455506e`). It is not a byte-identical re-run.

**One mask per anomaly** (`c4-mask-sheet-720.png`):
- **blinking on EA_BlinkCube (frame 78):** the hidden cube's would-be silhouette, without its shadow;
- **blinking on EA_Floor (frame 102):** the floor around the cubes in front;
- **missing_object on EA_HideCube (frame 292):** the missing cube's silhouette;
- **lod_corruption (frame 28):** the current low-LOD sphere;
- **lighting (frame 52):** all four lit receivers.

## 4. Safety
- **Crash: none.**
  - No new crash-report folder.
  - No error, assert or ensure line.
  - The accepted sessions exit 0 with `LogExit: Exiting`.
  - 1600×900 attempt 1 was the kit's owned-tree stop at its 4 GiB commit floor (monitor first, exit 125). Its log's last line submits frame 1,119, and Codex counts 1,020 PNGs written: the unbounded write backlog that `455506e` fixes.
- **Upload: none.** `upload-check` passes on all four attempts. The only upload-like line is the crash client's DataRouterUrl print.
- **The Paris host** (my own hashes, before against after): Content (1,456), plugin Content (5), Config (18), the descriptor and the `.uproject` are hash-, size- and mtime-equal. The C:, D: and E: drive roots are unchanged.
- **Binaries:** only the host exe/pdb and the three editor plugin DLLs/PDBs changed (plus `.target`/`.exp`). Every change came with UBT/cl/link alive, at BelowNormal, with `-MaxParallelActions=2`. The 27-file manifest is equal to disk.
- **Git:**
  - `m51` equals P9 at start, prelaunch and exit; master, release and the 30 tags are unchanged.
  - The feature branch is clean at `47a6972`, with W4 dates, no origin branch and no tags.
- **My polls saw all four editor sessions** (17340, 10096, 17432, 34620).

## 5. What's left before an office capture smoke on Europa's fork
1. **The binding holds at the test scene's scale, not yet at an office scene's:**
   - it tags every visible actor once, at session start, for the whole session;
   - `Start` fails outright when there are more visible actors than the 55 free ledger values;
   - an actor that comes into view later can never be a target;
   - admission is "any drawn pixel", without 5.1's census floor and ceiling;
   - custom depth stays on for every tagged actor, a host-interference risk on titles that read custom depth.
2. **The bench command `IAI.Capture.C4` forces the view to the first camera actor,** so the smoke must capture the player's own view. The 4.25 `annotation.json` is minimal, missing 5.1's affected/injected frame sets.
3. **Then the read of Europa's fork:** its viewport, render target, `PostRenderViewFamily` and tick mode.
