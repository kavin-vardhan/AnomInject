# 094-46r — Code's review: GO2 passes on D3D11 and fails on D3D12 by pixel count only. Both near-plane floors are sealed and recompute exactly; D3D11's 094-46 positive now matches on every unlabelled frame (254 px at 2 levels against a 255-px floor). D3D12's positive stays within 2 levels everywhere, but 6 unlabelled frames carry 255 px at 2 levels against its sealed 240. The 1279×719 legs, GO3's default-only run and the card read check are unrun. NOT pushed; the card stays 094-45r

**Date:** 2026-11-05, about 11:45 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Launch:** resume of thread `01a14334` at 09:37:15 IST, after a dry run. Detached, W4 (the child saw no `GIT_*_DATE`; one secret-named variable removed by name only), sandbox `danger-full-access`, `gpt-6-astra`, effort max (verified on the live process's command line).
- **Prompt:** `resume-prompt-094-46r.txt`, 21,369 B, `5A5F400F…`. Order: chat's job list (restated), GO1–GO4 verbatim from `094-46-ruling.md`, GN1–GN5 verbatim from `094-45r-ruling.md` (byte-equal to the 094-46 slice), steps 7–10 verbatim from `094-44-request.md`, my short context, then the 094-43r implementer block (byte-equal to the tail of the 094-43r and 094-46 prompts). Receipt: `code-receipts-094-46r\prompt-receipt.txt`.
- **Preserved before launch:** 094-46's report outputs (9 files, `code-receipts-094-46r\preserved\`, all copy-equal; `report.md` = the body of `_reviews\094-46-codex-report.md`).
- **Run:** 09:37:15 → 11:25:39 (6,503 s), exit 0, one try. Codex's verdict: **NEEDS-DECISION**.
- **My files:** receipts and polls (every 2–2.5 min, UTC epoch per line) in `code-receipts-094-46r\`; readers in `D:\IA_Europa\Temp\CodeReview09446r\`. Codex's report: `_reviews\094-46r-codex-report.md` (body byte-equal to `report.md`, `3B10C93E…`).

**Commits (local `feat/europa-port`, nothing pushed):**

| Commit | What | Sessions ran on it |
|---|---|---|
| `cd2d60c` | `-IAICameraFarOnly`: the combined schedule's 2,352-frame default section with no near-wall tail | no |
| `15cea38` | kit, readers, floor sealer | **all five new sessions** |
| `886e1a0` | results and evidence index: docs only | no |

- The only Source change is `AnomalyCameraSession425.{h,cpp}` (+9/−3): `FarOnly` is set only inside the existing `Bench` branch (`!UE_BUILD_SHIPPING`, EA_Core, `-IAIBench`, `-IAICameraSlabBench`) and only with `-IAICameraRegression`. It drops the six tail holds from `FrameCount` and keeps `Prepare` at the far pose; it adds a `far_only` receipt field. Without the switch, the combined and camera-only schedules are unchanged.
- New gotcha G554; the next free number on feat is G555. Author and committer dates are real times (W4).

## 1. GO2, D3D11 1280×720: PASSES
**The near-plane floor pair** (my `floor46r.py`): the new far-only near-100 session against 094-46's accepted control A (near 10), all 2,352 default-section indices.
- 0 pose mismatches; the far-only session has **0 camera events, 0 slab rows, 0 frames at the near-hold location**, and its camera never leaves (−550,0,480).
- **Max |Δ| = 2 levels; max count at 2 = 255 px** (per frame: median 160, p99 212). **Equal to Codex's sealed `{2, 255}`** (sealed 04:37:58 UTC; combined floor 04:38:23; re-comparison 04:40:23).
- **The binary difference:** the far-only session ran on `15cea38`'s binaries (AnomalyCapture `dd44fd0b…`), the reused control and positive on `afa3ea0`'s (`9c5a4846…`). My cross-check, the new near-100 session against 094-46's near-100 positive on the same 2,352 indices: **max 1 level, max count 295,682**, inside the session floor (1 / 560,155). So the rebuild adds no 2-level pixel; the 2-level pixels follow the near-plane difference.

**D4 against the larger floor** (my `d4_46r.py`; larger = the higher max delta, its own count; equal to Codex's rule):

| Check | D3D11 | D3D12 |
|---|---|---|
| Session floor (delta / count) | 1 / 560,155 (094-46) | 1 / 559,838 (mine = Codex's, sealed 05:12:03 UTC) |
| Near-plane floor | 2 / 255 | 2 / 240 (mine = Codex's, sealed 05:28:23 UTC) |
| Positive launched after both seals | reused positive; re-judged 04:40:23 UTC | yes: capture command written 05:29:24 UTC |
| Spans | 3, equal to the schedule (2352–2399, 2448–2495, 2544–2591) | the same 3 |
| Slab rows = annotation affected = injected = camera state rows | yes (144) | yes (144) |
| Labelled frames: pixels above the larger floor | 921,600 on every frame | 921,600 on every frame |
| Baseline controls: camera events / slab rows | 0 / 0 | 0 / 0 |
| Near plane | positive 100, controls 10; all restored to 10 at Stop | the same (`restored near clip 10.000` in all five new logs) |
| Pose mismatches positive vs control | 0 | 0 |
| **Unlabelled frames within the larger floor** | **2,496 of 2,496: PASS** | **2,490 of 2,496: FAIL** |
| Unlabelled max \|Δ\| | 2 (0 px above 2) | 2 (0 px above 2) |
| Unlabelled max count at 2 | 254 | **255** (bound 240) |
| Shifted-label mutants (early / late) | fail 6 / 6 | fail 12 / 12 |

- Codex's numbers are the same on both RHIs (`D3D11-rejudged-full.json`: max count 254; `stop.json`: frames 1156, 1163, 1996, 2003, 2087, 2094 at 255).

## 2. GO2, D3D12 1280×720: FAILS on pixel count, not on amplitude
- **The six failing frames** are one repeated render state: each has exactly 255 px at 2 levels and 290,763 px at 1 level, at default-section frames 7 apart (1156/1163, 1996/2003, 2087/2094). All are far-pose texture-section frames.
- **The positive's level-2 counts are not higher than the floor pair's** (my `d4fail46r.py`, unlabelled frames):

| Pair | n | mean | median | p90 | p99 | max |
|---|---|---|---|---|---|---|
| D3D12 positive vs control A | 2,496 | 154.0 | 157 | 187 | 212 | 255 |
| D3D12 near-floor pair (sealed) | 2,352 | 163.3 | 172 | 185 | 227 | 240 |
| D3D11 positive vs control A | 2,496 | 159.8 | 171 | 191 | 232 | 254 |
| D3D11 near-floor pair (sealed) | 2,352 | 151.5 | 160 | 186 | 212 | 255 |

- **Information only, computed after the stop and not a gate:** the same far-only session against the *other* control (B) gives **2 / 254 on D3D12** and **2 / 254 on D3D11**. So one pair's sealed maximum moves by about 15 px depending on which control it uses; D3D12's 255 sits 1 px above even that second pair.
- No mechanism is claimed. Codex stopped at this first failed measured gate and launched nothing further, as the stop rule says; no threshold was retuned and nothing was retried.

## 3. The legs: all five new sessions pass H4, strict texture G1–G3 and G5 (my `h4check46.py` + `edges46.py`)

| Session | Frames / drops | Fires (blink / LOD / checker / pink) | Camera events / frames | Masks = non-camera affected | Texture IoU min | Out-of-mask px | Edge frames changed | Residual ≠ 0 | LOD min px | `unavailable` | Failures |
|---|---|---|---|---|---|---|---|---|---|---|---|
| D3D11 near-floor | 2352 / 0 | 15 / 19 / 22 / 13 | 0 / 0 | 780 = 780 | 1.000 | 0 | 0 of 140 | 0 of 35 | 68,034 | 0 | none |
| D3D12 control A | 2640 / 0 | 15 / 19 / 22 / 13 | 0 / 0 | 780 = 780 | 1.000 | 0 | 0 of 140 | 0 of 35 | 68,036 | 0 | none |
| D3D12 control B | 2640 / 0 | 15 / 19 / 22 / 13 | 0 / 0 | 780 = 780 | 1.000 | 0 | 0 of 140 | 0 of 35 | 68,036 | 0 | none |
| D3D12 near-floor | 2352 / 0 | 15 / 19 / 22 / 13 | 0 / 0 | 780 = 780 | 1.000 | 0 | 0 of 140 | 0 of 35 | 68,034 | 0 | none |
| D3D12 positive | 2640 / 0 | 15 / 19 / 22 / 13 | 1 / 144 | 780 = 780 | 1.000 | 0 | 0 of 140 | 0 of 35 | 68,034 | 0 | none |

- Every session: player view = held view; census lost / no-pass / polluted / exhausted 0; custom depth unexpected and tagged 0, depth before = after; `r.CustomDepth = "1"` (Constructor) only; 0 fatal lines; 0 "Default Material" lines; upload check PASS; routes `blinking,lod_corruption,missing_texture,corrupted_texture,camera_clipping`.
- Switches per leg (from `prepared.json`): controls `-IAICameraBaseline`, floors `-IAICameraFarOnly`, all with `-IAIBench -IAICameraSlabBench -IAICameraRegression -IAICaptureFallback -game -nowrite`, 1280×720, `-d3d11` or `-d3d12` as named.
- **GN4:** `label_transition_camera_clipping_unconfirmed_entries` is 0 in all five sessions, equal to the annotation reason sums and to the positive-and-unconfirmed row counts (`gn4_46r.py`).
- **Unrun:** D3D12 1279×719 combined, D3D12 1279×719 default-only (GO3), and the card's read commands.
- **Info only, not the GO3 gate** (`keys46.py`: D3D12 control A, which requests camera_clipping at the baseline near, against Stage 1's `c5d0919` D3D12 1280×720 output): **no key path removed and no type changed** in any shared file. Only new keys appear (`whole_frame`, the `camera_clipping.*` row keys, `camera-session-start.json`, tail evidence files). Value differences are run identity and timing, census tag owners and the frame count.

## 4. D5
- `AnomalyAutoInjectorSubsystem.cpp` is byte-equal between `c5d0919` and `886e1a0`; the default-enabled set is blinking, missing_texture, corrupted_texture and lod_corruption; `TryFireOnce` still skips session globals, so camera_clipping is never fired by the pool.
- The C4/C5 default routes are unchanged since `eea2acd` (the only Source diff is the camera session file pair). No Content change.

## 5. Builds (FM1)
- Codex: Editor attempt 1 in 52.6 s, Game attempt 1 in 66.9 s, 0 link errors; Shipping not rebuilt.
- Host and plugin Binaries changed only between 09:48:22 and 09:50:55 IST (5 timeline entries). The last entry saw no live build process, but the files' times (09:50:21.7–09:50:41.9) precede the Game build child's final records (09:50:42.07–42.11), so the writes were inside the build. No Binaries change after that, so all five sessions ran on the same binaries.

## 6. Safety
- **Crash:** none. All five sessions closed their logs normally; 0 fatal lines; no dump or WER entry since launch. The only crash-term match per log is `bSendUnattendedBugReports=False`. The 4.25 crash monitor rewrote its own `CrashReportClient\Saved\Config\WindowsNoEditor\*.ini`, as on every exit.
- **Upload sign:** none (five upload checks PASS, 0 hits).
- **Paris host:** its own Content (1,456), Config (18), the descriptor and the `.uproject` hash-equal before and after; plugin Content (7) unchanged. Only Binaries and Intermediate build files changed (§5).
- **Materials:** the 4.25 pink (`b88d8861`) and checker (`5222b3ee`) equal the `8df75de` blobs in both copies and at every ref through `886e1a0`; both 5.1 materials equal `d7eee97`'s blobs everywhere.
- **Fixture:** nine assets, `Engine.ini` `64d45f80`, `DefaultEngine.ini` and the `.uproject` equal.
- **Deletions and P9:** my tree lists show 0 removed files, 0 reparse points and 0 P9 hash changes; drive roots unchanged.
  - **New practice, disclosed by Codex:** 13 text receipts this job created (for example `D3D11-rejudged-full.json`) were stored compressed with `compact`, to keep the kit's 3 GiB launch reserve. Their sizes and SHA-256 are unchanged (I rehashed all 13: 0 mismatches) and nothing was deleted.
  - One kit invocation came before arming finished; it launched no engine (Codex's disclosure).
- **m51:** passes (HEAD `53bf725`, index `526aba7f`, 15 untracked) at start, dry run, prelaunch and exit.
- **Refs:** local feat `886e1a0`, origin `c5d0919` (ls-remote equal, an ancestor); that is the only ref change. Spikes local, backup ref unchanged, fixture worktree detached at `8e59a56`, clean.
- **C:** no session dropped ≥ 0.01 GiB (Codex's per-session table). Over the whole job C: fell from 23.67 to 23.2 GiB, between sessions (the largest step, 23.66 → 23.27, falls between the D3D11 near-floor session and D3D12 control A); not attributed, and far above GL4's 10 GiB. Final: C: 23.2, D: 53.0, E: 54.9 GiB.
- **Foreign activity:** the owner's Codex desktop app and two blender-mcp processes, present throughout and untouched. No foreign UE editor appeared.
- **GA5:** not approached; every admission gate cleared in 2–3 s.

## 7. Push and card
- **Not pushed:** D3D12's D4 failed and GO3 is unrun, so the push condition is not met. No pre-push scan was written. Origin stays `c5d0919`.
- **The office card stays `_reviews\094-45r-office-card-europa.md`.**

## Decisions for chat
1. **D3D12 fails GO2's count bound by 15 px on 6 frames (one repeated render state), with no pixel above 2 levels.** A single pair's maximum count is itself a sample maximum: the same far-only session reads 240 or 254 depending on which control it is paired with, and D3D11 passed by 1 px (254 against 255). Options:
   - **(a)** Keep GO2 as written. camera_clipping then fails D4 on D3D12.
   - **(b)** Seal the near-plane floor over more than one pair (for example the far-only session against both controls, the maximum over pairs). Still a sample maximum; as information only, it would read 254 here and D3D12 would still fail by 1 px.
   - **(c)** Bound amplitude only for unlabelled frames when the floors differ in level: no pixel above the larger floor's max delta (both RHIs: 0 such pixels), with labelled frames still far above (921,600 px).
   - **(d)** Compare distributions instead of maxima (for example the positive's level-2 count quantiles against the floor pair's; on D3D12 the positive is lower at the median and p99).
2. **After that ruling, what remains:** D4 on D3D12 under the ruled rule, the D3D12 1279×719 combined leg, GO3's default-only run with the key diff and card reads, then my scan, push and card.
