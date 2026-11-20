# 094-45r — Code's review: Stage 1 (textures on Paris) passes by my recompute and is PUSHED at `c5d0919`, with the new office card. Stage 2 (camera_clipping) is ported but fails D4 strictly on 1–3-pixel differences in unlabelled frames; D3D12 and a default-route regression are unrun. Stage 2 is NOT pushed

**Date:** 2026-11-04, about 19:30 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Launch:** resume of thread `01a14334` at 16:23:04 IST, after a dry run. It ran detached, with W4: the child saw no `GIT_*_DATE`, and one secret-named variable was removed by name.
  - m51 and the refs passed at start, dry run, prelaunch, the Stage 1 handshake, after the push, and at exit.
- **Prompt:** `resume-prompt-094-45r.txt`, 27,386 B, `F68A26AC…`. It holds the 094-44 request, then GL1–GL5 and GM1–GM3 verbatim, then the scope line, my context (with the handshake) and the 094-43r implementer block.
- **Run:** 16:23:04 → 19:17:58 (10,494 s), exit 0, one try. Codex's verdict: **NEEDS-DECISION**.
- **Handshake:**
  - `STAGE1-READY.txt` appeared at 18:15 (`c5d0919…`).
  - I reviewed, scanned and pushed it, then wrote `STAGE1-PUSHED.txt` at about 18:24.
  - Codex confirmed the push with `ls-remote` before starting Stage 2.
- **My files:** receipts in `code-receipts-094-45r\` (polls every 2–3 min), readers in `D:\IA_Europa\Temp\CodeReview09445r\`.

## 1. Stage 1 — GM2 on Paris (EA_Core)
- **The bench additions** (`c356b7e`, home-host helper only; the host Source copy and the `tools/europa` copy are byte-equal, `5F47CA15`):
  - **base material:** a runtime `BasicShapeMaterial` MID with Color (0.05, 0.2, 0.5, 1), read back, on all four targets: EA_HideCube, EA_BlinkCube, EA_LOD (both slots) and EA_Floor;
  - **lights:** EA_Light set to 0, plus six spawned shadowless directional fills at intensity 3;
  - **gating:** `!UE_BUILD_SHIPPING`, `-IAIBench`, map EA_Core and a game world only, with nothing saved;
  - **EA_Floor:** included, with no exclusion (`texture_exclusions:false`).
  - Plugin Source is unchanged in Stage 1. The Paris host's own Content (1,456 files), Config (18), descriptor and `.uproject`, and the plugin Content (7), are hash-equal before and after.
- **Calibration, recomputed by my `cal45r.py` from the raw sweep** (66 groups: the −5 EV probe plus 0 to −8 EV in 0.125 steps):
  - The brightest exposure that passes GK2 on all four targets is **−1.875 EV**. All 15 brighter values fail; at −1.75 the binding check is EA_Floor's pink hue (B/G 233.9/94.2 = 2.48).
  - At −1.875: untouched means 104.07 / 104.10 / 110.13 / 157.27, 0 saturated pixels, front checker gaps 57.99 / 57.84 / 46.65 / 25.53, and pink hue passing.
  - **Swap visibility is 1.0000 for both textures on all four targets.** That is the per-frame minimum over 8 frames, counting pixels changing by more than 2 levels against the untouched preview. **So no target used GM2's fallback.**
  - −5 EV, the old GL1 value, now fails, because the cubes are too dark under the fills (mean 18.1).
- **View lock:** one camera, EA_Camera at (−550, 0, 480), pitch −12, FOV 50, in every calibration and preview row, and in **all 2,352 C5 frames of each leg**.
- **The three legs, recomputed from the raw frames** (`h4check45r.py`, `edges45r.py`, `cal45r.py` on each leg's own `-game` preflight):

  | Leg | Frames / drops | Events (blink / LOD / checker / pink) | Masks = affected | Texture IoU min | Out-of-mask px | Edge frames changed | Residual ≠ 0 | LOD min px | `unavailable` | Pre-fire visibility |
  |---|---|---|---|---|---|---|---|---|---|---|
  | D3D11 1280×720 | 2352 / 0 | 15 / 19 / 22 / 13 | 780 = 780 | 1.000 | 0 | 0 of 140 | 0 of 35 | 68,036 | 0 of 5 | 1.0000 ×8 |
  | D3D12 1280×720 | 2352 / 0 | 15 / 19 / 22 / 13 | 780 = 780 | 1.000 | 0 | 0 of 140 | 0 of 35 | 68,036 | 0 of 5 | 1.0000 ×8 |
  | D3D12 1279×719 | 2352 / 0 | 15 / 19 / 22 / 13 | 780 = 780 | 1.000 | 0 | 0 of 140 | 0 of 35 | 67,886 | 0 of 5 | 1.0000 ×8 |

  - My edge frames are: the frame before each fire, and +1, +2 and +4 after each revert, each against the matched clean frame (same pose, grain phase and sweep phase).
  - The same reads on every leg:
    - census lost / no-pass / polluted / exhausted all 0, and the legacy reference unused;
    - custom depth on, tagged and unexpected all 0, depth before = after, host custom-depth `none`;
    - `r.CustomDepth` = "1" three times per session;
    - 0 fatal lines, 0 missing cached shader maps, 0 "Default Material" lines, upload check PASS;
    - SELF_CHECK RHI and size correct.
  - The texture fire targets are identical on all three legs: checker on HideCube 8, Floor 7, LOD 5 and BlinkCube 2; pink on BlinkCube 4, Floor 4, LOD 3 and HideCube 2.
- **C: per session** (kit receipts; matches Codex): changes of −0.0013, −0.0262, +0.0338, −0.0017 and −0.0025 GiB. No drop reached 1 GiB.
- **One deviation, disclosed by Codex:**
  - Calibration slot 01 hit the inherited 900 s C5 bound partway through its 198 previews. It was closed by an owned stop before any fire, and its receipt is kept.
  - Slot 02 got 2,700 s under a recorded no-fire retry predicate. Scored legs kept 900 s.
  - No measured gate was retried.

## 2. Stage 1 — scan, push and card
- **Scan:** `_reviews\094-45r-stage1-prepush-scan.md`, over `8df75de..c5d0919`. It is clean.
  - The one item is the carried owner profile path: 356 lines, mostly kit C: cache-size paths and UBT warnings in receipts.
  - The 39 name = value hits are all `authority` labels.
  - The single dense root-name line is ordinary words.
- **Push:** `git push --porcelain origin c5d0919…:refs/heads/feat/europa-port`, exit 0 at 18:22:17, `8df75de..c5d0919`.
  - `ls-remote` = `c5d0919`; the remote still has 20 heads; no tags, backup ref, spikes or force.
  - Receipt: `code-receipts-094-45r\push-stage1-receipt.txt`.
- **Card:** `_reviews\094-45r-office-card-europa.md`, which replaces card 094-43.
  - It covers `c5d0919` and the default pool of blinking, lod_corruption, missing_texture and corrupted_texture.
  - It adds checker and pink eyeball rows, a "Default Material" count, texture revert and `residual=0` counts, and events by type. All reads are numbers or fixed words.

## 3. Stage 2 — camera_clipping (local `c706b7c`, NOT pushed)
- **The port diff against `d7eee97`:**
  - `Anomaly_CameraClipping.{h,cpp}` differ only by two includes and `GetCameraCacheView` → `AnomalyCompat::CameraCachePOV`.
  - `AnomalyDefaults.{h,cpp}` (the `CameraClippingTriggerRadiusCm` key and `IAI.Anomaly.CameraClipTriggerRadius`) and `camera_clipping_slab_selftest.cpp` are byte-equal.
  - The slab selftest passes 154 checks (Codex).
  - The 4.25 adapter is new: `Compat/AnomalyCameraSession425.{h,cpp}`. It holds the anomaly from C4/C5 Start to Stop, writes the 12 `camera_clipping.*` row keys under the 5.1 names, and accumulates the 5.1 summary block verbatim.
  - **Keys, compared mechanically** (`keys45r.py`):
    - 55 of 5.1's 56 `camera_clipping`-named keys are present.
    - 5.1's run-summary counter `label_transition_camera_clipping_unconfirmed_entries` is not emitted; the 4.25 path records those transitions in the annotation instead.
    - The 4.25 path adds `session_global_baseline_near`, `session_global_restored_near` and `session_global_near_restored`, plus a separate `camera-session-start.json` receipt.
  - ⚠ **Default-path changes, made by Stage 2 to every C5 run:**
    - `run_summary.json` is now written on every run, holding just the three `session_global_*` keys when camera_clipping is off. No such file existed before on 4.25.
    - Every sampled fire state gains a `"whole_frame"` key.
    - `Observe` and `Labelled` gain a whole-frame branch.
- **D4 on D3D11, recomputed by my `cc45r.py`:** same seed; control held at near 10, positive at near 100; 0 pose mismatches across 2,352 index pairs.
  - **Spans:** 3, exactly the three near holds, frames 624–911, 1200–1487 and 1776–2063 (864 frames).
  - **Agreement:** row `camera_clipping.slab` = annotation affected frames = `anomaly_present`.
  - **Labelled frames:** each changes at least **902,398 px** (of 921,600) against the control.
  - **Can-fail:** the baseline session labels 0 frames and has 0 events.
  - **Near plane at Stop:** restored to 10 in both sessions.
  - Custom depth, census, `unavailable` and upload checks are clean, with no crash.
  - ❌ **D4 fails as written: 914 of the 1,488 unlabelled frames differ from the control by more than 2 levels**, each on 1 to 3 pixels.
    - **What those differences are** (`ccdiag45r.py`, no mechanism claimed):
      - **every** unlabelled frame, in the far prepass and in the far holds, differs at some level on about 319,000–384,000 px;
      - the per-pixel deltas are ±1–3 levels, and the mean signed shift is about 0 (−0.001 to +0.0007);
      - only **8 distinct pixel positions** ever exceed 2 levels, each by exactly 3 levels, at fixed screen spots: three near (2–17, 422–435), three near (275–298, 585) and two near (1105–1107, 661–662).
    - No control pair at the *same* near plane exists, so whether two sessions at the same near plane are byte-equal is **unmeasured**. The cause is not established either way: the near plane's depth precision, or run-to-run noise.
- **D3D12 camera legs and a Paris regression with the route requested: not run.** Codex stopped at the failed gate.
  - ⚠ Note for the next job: Codex's design used camera-only routes (`IAI.Capture.C5 … 4242 camera_clipping`), so even a passing run would not have exercised the default routes on the Stage 2 code. That leaves the default-path changes above without a runtime check.
- **D5 holds at `c706b7c`:**
  - the 4.25 default-enabled set is blinking, missing_texture, corrupted_texture and lod_corruption;
  - camera_clipping sits in `GAutoPool` but `TryFireOnce` skips session globals (`IsSessionGlobalId`), as in 5.1;
  - the kit's default routes are unchanged.
- **Builds (FM1):**
  - Stage 2 Editor failed attempt 2 on a `ToSharedRef()` compile error and passed attempt 3, the last allowed. Game passed attempt 2.
  - Plugin Binaries changed only while UBT, cl or link were alive (12 timeline entries, 0 outside builds).

## 4. Safety (both stages)
- **Crash:** none. All 7 sessions exited 0, with no crash or WER files since launch. The only matching term is the command line's `bSendUnattendedBugReports=False`.
- **Upload sign:** none. All 7 upload checks PASS with 0 hits.
- **Paris host:**
  - its own Content, Config, descriptor and `.uproject` are hash-equal;
  - plugin Content is unchanged; only our branch is involved;
  - host and plugin Binaries changed only during builds.
- **Materials:** the 4.25 materials equal the `8df75de` blobs in both copies and at every ref through the tip. The 5.1 materials equal `d7eee97`'s blobs everywhere.
- **Fixture:** nine assets, `Engine.ini` (`64d45f80`), `DefaultEngine.ini` and `.uproject` equal.
- **Deletions and P9:** my tree lists show 0 removed files (excluding the relay's own `exit-code.txt` move), 0 reparse points and 0 P9 hash changes. The drive roots are unchanged.
- **m51:** passes at every check (HEAD `53bf725`, index `526aba7f`, 15 untracked).
- **Refs:**
  - local feat = `c706b7c`, origin = `c5d0919` (an ancestor);
  - the spikes are local; the backup ref is unchanged;
  - the fixture worktree is detached at `8e59a56`, clean.
- **C:** 20.43 → 20.40 GiB over the job.
- **Foreign activity:** none seen. **GA5:** not approached; gate waits were 7–240 s.

## Decisions for chat
1. **D4's "every unlabelled frame matches".** The measured mismatch is 1–3 px per frame at exactly 3 levels on 8 fixed screen positions, plus ±1–2-level differences across about 40 % of every frame, against labelled frames changing over 900,000 px.
   - Options: a defined tolerance (for example, a maximum delta or a pixel count), or a same-near control pair that first measures session-to-session noise.
   - The cause is not established.
2. **"Paris regression with the route requested":** should the next job run the default routes plus camera_clipping on both RHIs? Stage 2 changes the default capture path: `run_summary.json` on every run, a `whole_frame` key on every fire, and the whole-frame branches.
3. **The 5.1 run-summary key `label_transition_camera_clipping_unconfirmed_entries`** is absent on 4.25, and three `session_global_*` keys are added. Is that acceptable as compat-level?
