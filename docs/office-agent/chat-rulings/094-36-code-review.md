# 094-36 — Code's short review (C5 unqualified: a protocol bug, interference that looks like scene drift, and real-scene visual failures in all four anomalies; census parity, custom depth, view and schema pass)

**Date:** 2026-11-01, about 16:40 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:**
- **Codex's report:** collected byte-identical as `_reviews\094-36-codex-report.md` (sha256 `18C4C7B2…`). Exit 0 after 10,349 s (13:17:54 → 16:10:23), one resume try. Verdict **NEEDS-DECISION**.
- **Commits:**
  - `b1710b7`, `a4a0df6` and `9a785e0`: built;
  - `05c55e2` and `2b9dc19`: source-only, unbuilt;
  - `df5dfdc`: docs.
- **The session:** one street-map session, `C5\01`, 15:11:47–15:23:01 IST, 6,336 frames, 0 dropped.
- **My rules:** fixed at 13:22 in `D:\IA_Europa\Receipts\CodeReview09436\review-rules.md`, before any session existed. I added one switch test after seeing that my silhouette attribution has no power; its rule is in its own output file.
- **Receipts and scripts:** `CodeReview09436\` and `code-receipts-094-36\`.

## 1. The diff against the cited 5.1 code
- **Batch holding.**
  - What changed: the per-frame `FlushRenderingCommands` + `RestoreAll` is gone. Each batch stays tagged until `Observe` sees its own RequestId; `ReleaseUnused` then restores every actor that no live event or in-flight batch holds.
  - Parity: at most 2 batches in flight, which equals 5.1's `MaxInFlightBatches=2` (`AnomalyCensus.h:88`, release at `AnomalyCensus.cpp:629-699`).
  - ⚠ Gap: there is no `BATCH LOST` timeout (5.1 `LostAfterTicks=8`) and no no-pass or polluted discard. One lost readback would stall the 4.25 census permanently. This was moot here (0 drops).
- **Event targets.** Bindings are keyed by id + StartFrame and held for the live fire; `Tag()` returns early if the actor is already tagged.
  - ⚠ Deviation: an event adopts an existing census value, where 5.1's m44 rule asserts the event's own value.
  - The masks use the binding's values, so they stay correct. This counts as a 4.25-only reduction in re-tagging.
- **The game's own custom-depth actors.**
  - They are snapshotted at census start, then excluded in four places: the provider (`host_custom_depth_untouched`), `Tag()`, event binding and batch fill.
  - G498 logs the deviation and the 5.1 follow-up.
- **The bench census switch.** `IAI.Capture.BenchCensusOff` defaults to 0, acts only with `-IAIBench` (checked in both the setter and `Advance`), and resets at `Restore`. It is **off by default**.
- **The annotation values.**
  - `coverage_ratio` is the mean clipped projected-box share over affected frames. 5.1 averages over on-screen frames (`AnomalyCaptureSubsystem.cpp:10076-10084,10270`); here the two sets give identical values (gap 0).
  - `bbox_source` is "drawn" if any frame has drawn pixels, as in 5.1 (`:10097-10103,10272`).
  - Near and far come from the view's projection.
  - ⚠ An infinite far plane is written as **`far: 0`**, where 5.1 writes `GAnomalyDefaultFarPlane = 1,000,000` (`:594`, `:10291`). Only the rows carry `far_infinite`; `annotation.json` does not.
  - ⚠ `coverage_pct` is −1 on all 36 events. The fix is in `2b9dc19`, which is unbuilt.
- **The FR3 reader** now checks the freeze against the first scored-pass PNG's write time.
- **The grain.** I verified the citations in `E:\EpicGames\UE_4.25` source:
  - `RemapPermutation` forces grain quantization on except for linear output devices (`PostProcessTonemap.cpp:215-219`), overriding the CVar read at 810-812;
  - `GetFrameIndex(8)` returns `FrameIndex % 7` (`ScenePrivate.h:1047-1051`).

  So FT3's phase-match fallback is legitimate.
- **Comments:** none in the 267 added source lines.

## 2. The session, from the raw files
- **FR3 passes:** all 50 frozen files are timestamped ≤ 15:17:08.047; the first scored-pass PNG is 15:17:08.403.
- **The grain.** `grain_phase` is on every row (0–6); AA and eye adaptation are 0 on every row.
  - Phase matching removes the 2-level grain: the share of stable pixels whose range across phases is 2 levels is 5 / 46 / 26 % at poses 2 / 3 / 4, and 0 within a phase.
  - ⚠ Static pixels are **not bit-stable** within a phase: 76 / 99 / 97 % of stable pixels range by 1 level. That is within the frozen ±1 tolerance.
- **The poses.** My recompute equals Codex's frozen references, maps, self-check counts and interference counts exactly, at all six poses.

| Pose | Stable | Self-check max | Census-on max px / share | Kept |
|---|---:|---:|---:|---|
| 0 | 33.6 % | 36.5 % FAIL | 1,168 / 0.377 % FAIL | no |
| 1 | 70.7 % | 0.391 % FAIL | 1,830 / 0.281 % FAIL | no |
| 2 | 85.4 % | 0.031 % | 659 / 0.084 % | yes |
| 3 | 52.3 % | 2.99 % FAIL | 55 / 0.011 % | no |
| 4 | 77.5 % | 0.001 % | 3 / 0.0004 % | yes |
| 5 | 69.8 % | 0.102 % FAIL | 780 / 0.121 % FAIL | no |

- **G500 is real.** The first census-on frame at every pose ran with the census off: 6 rows with `enabled=false` and 0 owners. Frame 3168 (the first scored-pass frame) is at yaw 300.
- **The interference failures look like scene drift, not the census.** I used one baseline across the switch: the census-off first half's map and references.
  - The change count rises smoothly through the census-off second half and simply continues after the switch.
  - The first census-on frame, which has no owners yet, already sits at the census-on level: 1,290 against the last 40 census-off frames' mean of 1,274 (pose 0); 2,583 against 2,117 (pose 1); 692 against 539 (pose 5).
  - The step at the switch is no larger than the drift inside the census-off half: 769 / 595 / 141 against 805 / 1,767 / 405.
  - The stable pose 4 shows a step of −0.1 px.
  - **094-35's wall strip is gone:** rows 686–719 have 0 changed pixels on every census-on frame at pose 3.
  - No z-fighting claim was made. Silhouette attribution can't decide this: the dilated tag silhouettes cover 68–100 % of stable pixels.
  - **By the frozen line (≤ 0.001 at every pose), the gate still FAILS at poses 0, 1 and 5.**
- **The scored pass (kept poses 2 and 4).** My recompute agrees with Codex's numbers.

| Anomaly | Fired / scored | C2 mismatch / ambiguous | What the pixels show |
|---|---:|---:|---|
| blinking | 14 / 9 | 13 / 41 | Hidden frames: IoU 1.000, precision 1.000, spill 0. After each reveal, the target region keeps a residual decaying 8 → 6 → 4.5 % over 3–4 frames, while labelled clean. |
| lighting | 6 / 6 | 15 / 47 | The change ramps from 0.039 to 0.20 over the 13 labelled frames, then decays after. The mask is 782k–916k px of whole receivers, so precision is ≤ 0.20 (line 0.80). |
| LOD | 12 / 10 | 130 / 0 | Labelled frames change 0.004–0.011 % of the target region: the swap is invisible. |
| missing_object | 4 / 0 | – | The targets are bushes and an awning, with stable share 0–0.17. |

- **Clean scored frames:** 384 lie outside every event window, and none carries a mask.
  - Pose 2: all 192 exceed 0.001 (median 0.004), from drift since the pre-pass about 2.5 minutes earlier.
  - Pose 4: 15 exceed 0.001.
  - Together these are the reader's 207 mismatches outside events.
- **Negatives:** blinking's shifted mask fails 17 of 17 edge tests and the wrong target 67 of 67. Neither negative is qualified for all four anomalies.
- **The census.**
  - Drawn actors per pose: 141 / 116 / 37 / 2 / 27 / 92, so more than 55 at three poses.
  - The ledger was never exhausted; at most 57 owners per frame; 203 was never assigned.
  - **Late-entering targets:** some scored targets were drawn neither at pose 0 nor at pose 5, which was the actual first scored view (ProvaStrada11_63, SM_Alberiera_LP3, SM_Tronco_LP3, SM_PietreAngolo2_LP20). But none of them has a clean C2 result.
- **Custom depth passes.**
  - The 1,980 components are equal before and after.
  - All 6,336 mid-session samples obey "on = tagged or already on".
  - `CasaEliminare_8` (203) and `DefaultPawn_0` (18, write mask 2) were never tagged and never changed.
- **The view:** on all 6,336 frames the player view equals the rendered view, with zero deltas, on `DefaultPawn_0`.
- **The annotation:** 56 keys in 5.1's order, schema 2.1, and affected/injected sets that agree with the rows on all 36 events.
- **The masks I opened** (`c5-masks-sheet.png`):
  - blinking: exactly the hidden pavement piece (51,256 px);
  - LOD: the street patch (105,840 px), with no visible change in the frame;
  - missing_object: the bush behind the tree (11,222 px);
  - lighting: 916,185 px, 99 % of the frame (wall, tree, ground and sky).

## 3. Safety
- **Crash:** none. Exit 0, `LogExit: Exiting.`, 0 error lines, no new crash folder.
- **Upload:** none. The only upload-like line is the crash client's DataRouterUrl print, with its receiver disabled. No map save.
- **The Paris host**, by my own hashes before against after: Content (1,456 files, including `SampleMap.umap` `9D35E89E…`), plugin Content, Config, the descriptor and the `.uproject` are hash-, size- and mtime-equal.
- **Binaries:** only the host exe/pdb and `UE4Editor-AnomalyCapture.dll`/`.pdb` changed, each with our UBT/cl/link alive at BelowNormal and 2 actions. The manifest is 27 of 27 equal.
- **Git:**
  - `m51` equals P9 at start, prelaunch and exit.
  - The feature branch is clean at `df5dfdc`, with W4 dates, no origin branch and no tags.
- **Foreign activity:** a UE 5.7 DCDemo cook with its ShaderCompileWorkers, and ffmpeg. D: dipped to 12.6 and then 19.3 GiB, but not from the Europa folders; the kit waited 57.6 minutes for space.

## 4. What's left before an office capture smoke on Europa's fork
1. **A build-budget ruling,** then a rebuild with `05c55e2` and `2b9dc19` and a C5 re-run.
2. **Rulings on the real-scene definitions:**
   - drift-proof interference (for example, interleaved census-off and census-on blocks);
   - references taken near the scored hold;
   - the temporal settle after a reveal and around a light change;
   - lighting masks as changed receiver pixels;
   - a visibility gate for LOD;
   - missing_object targets that avoid animated foliage.
3. **Only then** the FS3 office card.
