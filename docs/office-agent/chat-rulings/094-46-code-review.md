# 094-46 — Code's review: GN4's counter is ported and correct. On D3D11 the combined default-plus-camera leg passes H4, strict texture G1–G3 and G5, but camera_clipping's D4 fails GN2's measured floor by one level. The control pair never differs by more than 1 level; the positive differs from it by 2 levels on up to 254 scattered pixels per unlabelled frame. D3D12, the default-only additive run and the card check are unrun. NOT pushed; the card stays 094-45r

**Date:** 2026-11-05, about 09:40 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Launch:** resume of thread `01a14334` at 08:01:06 IST, after a dry run. It ran detached, with W4: the child saw no `GIT_*_DATE`, and one secret-named variable was removed by name only. Sandbox `danger-full-access`, model `gpt-6-astra`, effort max.
  - m51 and the refs passed at start, the dry run, prelaunch and exit.
- **Prompt:** `resume-prompt-094-46.txt`, 18,614 B, `66B3657F…`. Order: chat's job list (restated), then GN1–GN5 verbatim from `094-45r-ruling.md`, then steps 7–10 verbatim from `094-44-request.md`, then my short context, then the 094-43r implementer block (byte-equal to the tail of the 094-43r and 094-45r prompts). Receipt: `code-receipts-094-46\prompt-receipt.txt`.
- **Run:** 08:01:06 → 09:19:32 (4,706 s), exit 0, one try. Codex's verdict: **NEEDS-DECISION**.
- **My files:** receipts and polls (every 2–3 min) in `code-receipts-094-46\`; readers in `D:\IA_Europa\Temp\CodeReview09446\`. Codex's report: `_reviews\094-46-codex-report.md` (body byte-equal to `report.md`, `D19B8CB4…`).

**Commits (local `feat/europa-port`, nothing pushed):**

| Commit | What | Sessions ran on it |
|---|---|---|
| `1e65b6f` | GN4 counter; `IAICameraRegression` combined schedule; bench helper hand-off | no |
| `afa3ea0` | kit, readers, floor sealer | **all three sessions** |
| `eea2acd` | results and evidence index: docs only (my diff `afa3ea0..eea2acd`: 7 files, all docs) | no |

- Author and committer dates are real times (W4). New gotchas G552 and G553; the next free number is G554.

## 1. GN4 — the counter: ported, correct, value 0 everywhere
- **The change:** `AnomalyTransitionCounts425.h` sums the `camera_clipping_unconfirmed` reason count over the session's transitions. `AnomalyCaptureLabels425.cpp:330` writes it into `run_summary.json` as `label_transition_camera_clipping_unconfirmed_entries`.
- **Against 5.1:** `d7eee97` `AnomalyLabelWriter.cpp:1350` writes `LabelSync->ReasonEntries[3]`. `CountEntryReasons` (`AnomalyCaptureSubsystem.cpp:7742`) adds one per frame entry whose reason bits hold that reason.
  - On 4.25, `AnomalyCaptureLabels425.cpp:191` adds one per frame where the camera fire is positive and the row says `camera_clipping.unconfirmed`. That is the same per-frame entry unit. The name is unchanged (D1).
- **Values, by my `gn4_46.py`:** 0 in all three sessions. That equals the annotation's per-event reason sums and the count of positive-and-unconfirmed rows (0, 0, 0). A nonzero value is unexercised at runtime; Codex's compiled helper test covers accumulation, and its assignment mutant fails.
- **Key parity, by my `keys51_46.py`:** all **56** of 5.1's `camera_clipping`-named keys are now emitted (094-45r: 55).
  - The 4.25 extras are the three accepted `session_global_*` keys (GN4) and the `camera-session-start.json` receipt fields.
  - The anomaly class, `AnomalyDefaults` and the slab self-test source are unchanged since `c706b7c`.

## 2. GN2 — the floor, then D4 on D3D11 1280×720: FAILS by one level
**Design (Codex):**
- Each combined session is the original 2,352-frame default-route section at the held EA_Camera view, then six 48-frame holds alternating near/far, starting near: 2,640 frames. The auto pool stops at frame 2328, before the tail.
- Two baseline sessions (near 10, `-IAICameraBaseline`, seed 4242) were run first. The floor was sealed, then the positive (near 100) was run.
- The far holds use EA_Camera's own transform, so 2,496 frames per session are unlabelled far frames.

**The floor — my `floor46.py`, independent of Codex's reader:**
- control A against control B, 2,640 frames, 0 pose mismatches, 0 slab rows in either;
- **maximum |Δ| = 1 level; the largest count of pixels at 1 level on any frame = 560,155** (a near-hold frame; far frames at most 295,709);
- 152 texture-section frames are byte-identical.
- **Identical to Codex's sealed thresholds** `{max_delta: 1, max_count_at_max_delta: 560155}`.
- Codex sealed at 03:22:02.80 UTC (`floors\D3D11-1280.json`, 08:52:03 IST). The positive launched at 03:23:29.80 UTC and my polls first saw it at 08:53:54 IST. **So the floor was recorded before the comparison.**

**D4 — my `d4_46.py`, positive against control A, judged against that floor:**

| Check | Result |
|---|---|
| Spans | 3: 2352–2399, 2448–2495, 2544–2591, equal to the schedule |
| Slab rows = annotation affected = injected = camera sampled-state rows | yes (144 frames) |
| Labelled frames: minimum pixels above the floor | **921,600 of 921,600** (every pixel, every labelled frame) |
| Baseline can-fail: camera events and slab rows in control A and control B | 0 and 0 |
| Near plane | positive held 100, controls held 10; all three restored to 10 at Stop |
| Pose mismatches; non-camera events (type, target, frames) positive vs control | 0; 69 of 69 equal (event ids differ only by the engine frame number) |
| **Unlabelled frames within the floor** | **56 of 2,496: FAIL** |
| Unlabelled maximum \|Δ\| | **2** (no unlabelled frame has any pixel above 2) |
| Unlabelled maximum count at 1 level | 296,540 (count bound holds: ≤ 560,155) |
| Unlabelled maximum pixels above the floor (at 2 levels) | 254 |

- Codex's numbers are the same (2,440 / 2,496, maximum 2, maximum 254, maximum count 296,540).
- **What the 2-level pixels look like** (`d4diag46.py`; no mechanism claimed):
  - mean 160 px per failing frame;
  - **4,704 distinct positions**, all in the lower 60 % of the frame (y 267–719), with no position failing in even half the frames;
  - mixed channels and signs, with the frame-mean signed shift between −0.0016 and +0.0005;
  - present in the far texture section (2,296 of 2,352 frames) and the far tail holds (144 of 144) alike.
  - **The positive against control B gives the same picture** (2,496 of 2,496 frames, maximum 238 px).
  - Two sessions at near 10 never differ by 2 levels anywhere.
  - So the 2-level differences appear only when the near plane differs; their cause is not established.
- For comparison only: under 094-45r's frozen ">2 levels" reading, this leg has **0** failing unlabelled frames. 094-45r's camera-only pair had 3-level spots; this combined bench has none.
- **D3D12 1280×720: unrun.** Codex stopped at the first failed measured gate, as the request's stop rule says.

## 3. GN3 — the combined Paris legs: D3D11 passes its object and texture gates; the rest is unrun
**My recompute on all three D3D11 sessions** (`h4check46.py` and `edges46.py`, on the 2,352-frame default section; references drawn only from that section; camera frames are excluded from mask sets):

| Session | Frames / drops | Fires (blink / LOD / checker / pink) | Masks = non-camera affected | Texture IoU min | Out-of-mask px | Edge frames changed | Residual ≠ 0 | LOD min px | `unavailable` | Failures |
|---|---|---|---|---|---|---|---|---|---|---|
| control A | 2640 / 0 | 15 / 19 / 22 / 13 | 780 = 780 | 1.000 | 0 | 0 of 140 | 0 of 35 | 68,036 | 0 | none |
| control B | 2640 / 0 | 15 / 19 / 22 / 13 | 780 = 780 | 1.000 | 0 | 0 of 140 | 0 of 35 | 68,036 | 0 | none |
| positive | 2640 / 0 | 15 / 19 / 22 / 13 (+1 camera event, 144 frames) | 780 = 780 | 1.000 | 0 | 0 of 140 | 0 of 35 | 68,034 | 0 | none |

- Fire targets are identical to Stage 1's D3D11 leg.
- On every session: player view = held view on all default-section frames; census lost / no-pass / polluted / exhausted all 0; custom depth on, tagged and unexpected all 0, depth before = after; `r.CustomDepth` = "1" (Constructor) only; 0 fatal lines; 0 "Default Material" lines; upload check PASS.
- My two texture-look heuristics read exactly as on Stage 1's accepted legs (checker gap minimum 25.55 on EA_Floor, pink median G up to 84). They are not gates.
- **Unrun:** D3D12 1280×720, D3D12 1279×719, the default-only additive comparison against `c5d0919`, and the office-card read commands.
  - Codex had planned **one** default-only run (D3D12 1279×719), reading GN3's "a default run" as singular, to keep disk reserve. It never launched.
- **Info only, not the GN3 gate** (`keys46.py`: control A, which requests camera_clipping at the baseline near, against Stage 1's `c5d0919` D3D11 output):
  - No key path removed and no type changed in any shared file.
  - Only new keys appear: `whole_frame`, the `camera_clipping.*` row keys, `camera-session-start.json`, `run_summary.json` and tail evidence files.
  - Value differences are run identity and timing (event ids carry the engine frame number; `t`, `t_wall`, `sample_frame`, `family_frame`, `grain_phase`, tick and readback-cost fields), census allocator tag owners, and the frame count.

## 4. D5
- `AnomalyAutoInjectorSubsystem.cpp` is byte-equal between `c5d0919` and `eea2acd`.
  - The 4.25 default-enabled set is blinking, missing_texture, corrupted_texture and lod_corruption.
  - camera_clipping is in `GAutoPool`, but `TryFireOnce` skips session globals (`:322`).
- The C4/C5 default routes are unchanged (`AnomalyCaptureLabels425.cpp:52`, `AnomalyCaptureAdmission425.h:20`).
- `IAICameraRegression` takes effect only with a non-Shipping build, `-IAIBench`, EA_Core, `-IAICameraSlabBench` and a requested camera route (`AnomalyCameraSession425.cpp:19-27`). Without it, the camera-only and default lengths are unchanged.

## 5. Builds (FM1)
- Editor passed attempt 1 in 76.3 s and Game attempt 1 in 72.5 s (the commit is `1e65b6f`), with 0 link errors. The Shipping binary was not rebuilt.
- Host and plugin Binaries changed only during those builds (5 timeline entries, 14 file changes, 08:17:55–08:21:11).
  - One poll entry, at 08:21:11, saw changes after the build processes had gone. The files' times (08:20:52–53) match the Game build child's last record (08:20:52–53), so the writes were inside the build.
- The host bench helper `Source\EuropaPhaseAProbe\EuropaTextureBench09444e.cpp` changed and is byte-equal to the plugin's `tools/europa` copy (`9DEA02EF…`). The host's own Content and Config are untouched.

## 6. Safety
- **Crash:** none. All three sessions closed their logs normally; 0 fatal lines; no dump or WER entry since launch. The only crash-term match is each command line's `bSendUnattendedBugReports=False`. The 4.25 crash monitor rewrote its own `LocalAppData\CrashReportClient\Saved\Config\WindowsNoEditor\*.ini` at session end (08:56:49), as it does on every exit.
- **Upload sign:** none (three upload checks PASS, 0 hits).
- **Paris host:**
  - its own Content (1,456), Config (18), the descriptor and the `.uproject` are hash-equal before and after;
  - plugin Content (7) is unchanged.
- **Materials:**
  - the 4.25 pink (`b88d8861`) and checker (`5222b3ee`) equal the `8df75de` blobs in both copies and at every ref through `c706b7c`;
  - both 5.1 materials equal `d7eee97`'s blobs everywhere.
- **Fixture:** nine assets, `Engine.ini` `64d45f80`, `DefaultEngine.ini` and the `.uproject` equal.
- **Deletions and P9:** my tree lists show 0 removed files, 0 reparse points and 0 P9 hash changes. The drive roots are unchanged.
- **m51:** passes (HEAD `53bf725`, index `526aba7f`, 15 untracked).
- **Refs:**
  - local feat `eea2acd`, origin `c5d0919` (an ancestor); that is the only ref change;
  - the spikes are local; the backup ref is unchanged;
  - the fixture worktree is detached at `8e59a56`, clean.
- **C:** before → drop per session: 23.815 → 0.0037 GiB, 23.790 → 0.0013 GiB, 23.780 → 0.0023 GiB. Final: C: 23.77, D: 53.08, E: 64.10 GiB.
- **Foreign activity:** the owner's Codex desktop app and two blender-mcp processes were present throughout and untouched. No foreign UE editor appeared.
- **GA5:** not approached; every admission gate cleared within seconds.

## 7. Push and card
- **Not pushed:** D4 failed and GN3 is incomplete, so the push condition is not met. No pre-push scan was written. Origin stays `c5d0919`.
- **The office card stays `_reviews\094-45r-office-card-europa.md`.**

## Decisions for chat
1. **D4 under GN2 fails by exactly one level.** The 2-level pixels appear only when the near plane differs, on up to 254 scattered pixels per unlabelled far frame, while every labelled frame changes all 921,600 pixels. Cause not established. Options:
   - **(a)** Keep GN2 as written. camera_clipping then cannot pass D4 on this bench.
   - **(b)** Also measure a near-plane floor before the positive: a held near-100 session whose schedule has **no** near holds (no slab-positive frames possible), paired against a near-10 control. Judge unlabelled frames against the larger of the two sealed floors. This keeps "measured, never assumed", but needs one more guarded bench switch and two more sessions per RHI.
   - **(c)** A fixed tolerance, for example "no unlabelled pixel above 2 levels". This D3D11 leg meets that with 0 failing frames.
2. **After that ruling, what remains:**
   - D4 on D3D12 1280×720;
   - the combined D3D12 1280×720 and 1279×719 legs;
   - the default-only additive run(s);
   - the card read-command check.

   Should one default-only run (Codex planned D3D12 1279×719) satisfy GN3's "a default run", or one per leg?
