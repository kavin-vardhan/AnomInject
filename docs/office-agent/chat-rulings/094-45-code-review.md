# 094-45 — Code's review: GL3 is fixed and GL2's view lock works, but Stage 1 stops on D3D11. Texture G3 fails on EA_Core because part of each swap renders the same as the original, and C5 fired on EA_Floor, which was never pre-qualified. Nothing pushed; no handshake file written; Stage 2 not started; the card is unchanged

**Date:** 2026-11-04, about 16:30 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Launch:** resume of thread `01a14334` at 15:15:19 IST (after the 15:15 blackout end; my wrapper refused any launch before epoch 1793785500). It was detached, with W4 (the child saw no `GIT_*_DATE`; one secret-named variable removed by name). m51 and refs passed at start, dry run, prelaunch, exit and final.
- **Prompt:** the 094-44 request verbatim, then GL1–GL5 verbatim, then the brief's scope line, my context (including the Stage 1 handshake) and the 094-43r implementer block. File `resume-prompt-094-45.txt`, 23,294 B, `F859832A`.
- **Run:** 15:15:19 → 16:07:58 (3,159 s), exit 0, one try. Codex's verdict: **NEEDS-DECISION**.
- **Handshake:** Codex stopped on its own failed gates before Stage 1 was ready, so it never wrote `STAGE1-READY.txt`, and I wrote neither `STAGE1-PUSHED.txt` nor `STAGE1-FAILED.txt`. The run folder holds no handshake file.
- **My files:** receipts in `code-receipts-094-45\` (20 poll lines, 2–3 min apart while builds or sessions were possible); readers in `D:\IA_Europa\Temp\CodeReview09445\`.

## 1. GL3: `runtime_exit.py` (commit `96d8f01`)
- **The change:** one predicate. `not intent.get('reason')` is dropped. The PID match, `root_running`, and the start ≤ intent ≤ root-end window stay.
- **Tests, from a `git archive` copy:** 9 of 9 pass. The new cases are:
  - an empty-reason owned stop is classified `owned-stop` (and `outcome` is not a crash);
  - an intent from another PID, or no intent at all, is still classified `crash`.
- **My mutants (`gl3review45.py`):** each one fails:
  - restoring the old empty-reason rejection fails 1 test;
  - dropping the PID check fails 2;
  - dropping the `root_running` check fails 1.
- **Replay:** 094-44f's own `execution.json` (PID 23160, reason `''`, root running, exit 125) now classifies as `owned-stop`. Its `result.json` is byte-unchanged (`500AF0C4…`).

## 2. GL2: the view lock and the `-game` re-qualification
- **Helper (`93e7435`):** holds `EA_Camera` at (−550, 0, 480), pitch −12, FOV 50, with camera management off. The host copy and `tools/europa` copy are byte-equal (`4DFDB471`).
- **Plugin capture change:** a bench-only no-fire probe, `IAI.Capture.BenchGeometry`. It is gated by `!UE_BUILD_SHIPPING`, `-IAIBench`, map `EA_Core` and no auto run.
  - The fix (`422ac34`) moves its early return after `Prepared = Next`. The first probe session stalled without it: 0 frames, 0 fires, closed by the kit with exit 0 and classified `normal`.
  - The only other `Source` change in the job is `AnomalyCaptureLabels425.cpp` (+22/−1). No anomaly, Content or descriptor changed. The probe reaches the office plugin copy, but it is inert there without `-IAIBench` and EA_Core.
- **My recompute** (`geom45.py`, my own ASP1 decoder, visible = tag and custom depth ≥ scene depth − 4/(2^24−1), last 8 rows of each of the three probes): one camera, `EA_Camera`, in every row.

  | Actor | Visible px | Drawn share | Centroid (x, y) | In frame / band |
  |---|---:|---:|---|---|
  | EA_HideCube | 78,390 | 8.5059 % | 331.5, 509.4 | yes / yes |
  | EA_BlinkCube | 78,390 | 8.5059 % | 947.5, 509.4 | yes / yes |
  | EA_LOD | 118,616 | 12.8707 % | 639.5, 406.7 | yes / yes |
  | **EA_Floor** | 216,502 | **23.492 %** | 639.5, 594.3 | yes / yes |

  The cubes match Codex exactly. EA_Floor is not in the helper's actor list (`EuropaTextureBench09444e.cpp:146`), so it was never previewed. Its geometry would pass; its material checks were never measured.
- **GK2 in `-game` at −5 EV** (`gk2game45.py`; 8 frames per preview; same-session silhouettes; GK2 rules):

  | Target | Untouched mean | Channel-255 px | Pink mean RGB | Checker gap |
  |---|---:|---:|---|---:|
  | EA_HideCube | 143.86 | 0 | 139.5 / 25.3 / 139.8 | 121.6 |
  | EA_BlinkCube | 144.42 | 0 | 140.1 / 25.4 / 140.4 | 121.8 |
  | EA_LOD | 99.80 | 0 | 244.9 / 88.1 / 231.0 | 41.6 |

  - All checks pass for the three cubes. Codex's numbers agree, with front-patch checker contrasts of 49.9 / 48.9 / 21.0.
  - No `-game` sweep was needed.
- ⚠ **The previews already show the G3 problem.** The fraction of each silhouette that changes by more than 2 levels, against the untouched preview:
  - **checker on the cubes: 0.4927 / 0.4935**;
  - pink on the cubes: 0.9862;
  - EA_LOD: 1.0000 / 1.0000.

  GK2's checks don't test whether the swap differs from the original, so nothing stopped C5.

## 3. The Paris legs
- **D3D11 1280×720 (scored session `…\H4\D3D11-1280\02`, frames on `E:\IA_BuildCache\09445-evidence\D3D11-1280\02`):** recomputed with `h4check45.py` and `edges45.py`.
  - **H4 passes:**
    - frames and labels: 2,352 frames, contiguous, 1280×720, 0 zero-byte, 0 dropped;
    - masks: the 780 affected frames equal the 780 mask files equal the labels' present and mask sets; mask PNG pixel counts match the rows;
    - census: lost, no-pass, polluted and exhausted are all 0, legacy reference never used;
    - custom depth: on, tagged and unexpected are all 0; depth before equals after; `r.CustomDepth` = "1" (Constructor only); host custom-depth result `none`;
    - private pass: `unavailable` 0 of 5 verified components;
    - LOD: all 19 events change target pixels (min 69,409);
    - log: SELF_CHECK on D3D11 / GameViewportClient; 0 fatal lines; 0 missing cached shader maps; upload check PASS.
  - **Fires (G1):** blink 15, lod_corruption 19, missing_texture 22, corrupted_texture 13.
    - missing_texture: HideCube 8, Floor 7, LOD 5, BlinkCube 2.
    - corrupted_texture: BlinkCube 4, Floor 4, LOD 3, HideCube 2.
  - **G2:** every texture frame changes its target. **Edges:** 140 of 140 edge frames are clean (the frame before each fire, and +1, +2 and +4 after each revert). Residual 0 on all 35 reverts. **0 changed pixels outside any mask.**
  - **G3 fails.** Whole-frame IoU per type and target (frames failing below 0.98):

    | | EA_LOD | EA_HideCube | EA_BlinkCube | EA_Floor |
    |---|---:|---:|---:|---:|
    | missing_texture | 1.0000 | 0.4926 (all) | 0.4935 (all) | **0.4065** (all) |
    | corrupted_texture | 1.0000 | 0.9862 | 0.9862 | **0.8131** (all) |

    Codex counts the same thing: 221/286 checker frames and 40/157 pink frames fail.
  - **What the unchanged pixels are**, measured with no mechanism claimed (`diag45.py`, `diag45b.py`):
    - **Cubes, checker:** 39,771 of 78,390 px change by ≤ 2 levels. The rest change by 33–64.
    - **Floor, pink:** 40,450 px are exactly (0,0,0) in both frames, outside every other target's silhouette. They are shadows, not a mask over-claim.
    - **Floor, checker:** those black pixels plus about 88,000 px where the checker matches the floor's original (unchanged-pixel mean 127.9 before, 127.6 after).
  - **C: per session:** 20.46 → 20.47 GiB (setup) and 20.45 → 20.45 GiB (scored), by Codex. My polls read 20.5 → 20.4. No drop reached 1 GiB.
- **D3D12 1280×720 and D3D12 1279×719:** not run.
- **Stage 2 (camera_clipping):** not started. camera_clipping is still compiled out on 4.25 and absent from the pool.

## 4. Safety
- **Crash:** none. Both sessions exited 0, classified `normal`, `crash_count` 0. No crash or WER folder and no dump since launch. The 11 CrashReportClient config `.ini` rewrites per session are the same as in 094-44d/e.
- **Upload sign:** none. The upload check passed on both logs. The one matching term is the command line's `bSendUnattendedBugReports=False`.
- **Paris host:** its own Content (1,456), Config (18), descriptor and `.uproject` are hash-equal before and after. Plugin Content (7) is unchanged. Saved (36) is unchanged.
- **Builds:** host and plugin Binaries changed only while UBT, cl or link were alive (11 timeline entries). The file counts are equal (16 + 11).
- **Materials:** the 4.25 materials equal the `8df75de` blobs in both copies and at every ref through `2d93e67`. The 5.1 materials equal d7eee97's blobs everywhere.
- **Fixture:** nine assets, `Engine.ini` (`64d45f80`), `DefaultEngine.ini` and `.uproject` are equal.
- **Deletions and P9:** my tree lists show 0 removed, 0 reparse points and 0 P9 hash changes.
- **m51:** passes at start, prelaunch, exit and final (HEAD `53bf725`, index `526aba7f`, 15 untracked).
- **Refs:** only local feat moved, `983f429` → `2d93e67`. origin is `8df75de` by `ls-remote`. The spikes are local. The fixture worktree is detached at `8e59a56`, clean.
- **Foreign activity:** the owner's ChatGPT desktop `codex exec-server` (not ours, untouched). No foreign UE process was seen.
- **GA5:** not approached; gate waits were 9 s.

## 5. Push and card
- **Nothing pushed.** Local `2d93e67` holds the GL3 fix, the view lock and the stop record, all on top of `8e59a56` (the textures in the default routes). It stays local until the textures pass on Paris.
- **No card update.** The office card stays `_reviews\094-43-office-card-europa.md` (origin `8df75de`: blinking, lod_corruption).

## Decisions for chat
1. **G3 on EA_Core with the bench as it stands.**
   - **What's measured:** at −5 EV under EA_Core's lighting, one checker grey renders within 2 levels of the cubes' and floor's original surface. The cubes' shadows on the floor render (0,0,0) under any material. So whole-frame IoU cannot reach 0.98 for the checker anywhere but EA_LOD, nor for pink on the floor. Meanwhile nothing changes outside the masks, the edges are exact, and the mask is each object's visible coverage.
   - **My recommendation:** don't change G3. Use GK2's own fallback on the bench, as runtime-only bench settings (the fixture recipe that made GH2/G3 pass):
     - a baseline material on the EA_Core targets that renders unlike both checker greys;
     - shadowless fills, so no target region is black.
   - Then add one pre-fire check: each swap must change at least 98 % of its target's silhouette in the `-game` preview. It would have stopped this leg before C5 (checker 0.49).
2. **EA_Floor:**
   - **What's measured:** the bench's C5 fires textures on EA_Floor (23.49 % drawn, inside the band), but the helper's previews cover only the three cubes.
   - **The choice:** either add the floor to the previews and every pre-fire check, or keep the bench from selecting it.
   - Under item 1's fills, the floor's shadow pixels would be lit.
3. **Next job:** rerun Stage 1 (D3D11, then both D3D12 legs) after items 1–2, with the same handshake. Stage 2 is unchanged.
