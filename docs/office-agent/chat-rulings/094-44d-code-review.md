# 094-44d — Code's review: G4 and G5 pass on the fixture. Paris D3D11 fails texture G3 because EA_Core is overexposed and bloomed, the same condition GH2 fixed on the fixture. Both D3D12 legs are unrun, nothing is pushed, and the card is unchanged

**Date:** 2026-11-04, about 10:55 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Resume:** thread `01a14334`, detached, with W4 (the child saw no `GIT_*_DATE`), after a dry run of my gates.
- **Prompt:** the 094-44 request verbatim, then GI1–GI7 verbatim, then the GI7 scope line, my context and the 094-43r implementer block (`resume-prompt-094-44d.txt`, 21,570 B, `F3EDA709`).
- **Run:** 10:09:02 → 10:50:14 (2,472 s), exit 0, one try. Codex's verdict is **NEEDS-DECISION**.
- **Report:** collected byte-identical as `_reviews\094-44d-codex-report.md`.
- **My files:** receipts in `code-receipts-094-44d\`; readers in `D:\IA_Europa\Temp\CodeReview09444d\`.

## 0. Frozen settings, children, `-BENCHMARK`
- **The outline-on run reused 094-44c's freeze exactly (GI6):**
  - `texture-setup.json` and `pose-setup.json` are byte-equal to 094-44c's off-run.
  - The launch argv differs only in log and receipt paths: exposure −2.1875, bloom 0, eye quality 2 / method 3, AA, motion blur and dynamic resolution 0, screen percentage 100.
  - The fixture binaries (7 files) equal 094-44c's archive C and are unchanged since 08:30:37, before 094-44c's off-run.
  - The driver differs from 094-44c's only in starting the depth observer.
  - The outline log reads `weight=1` here against `weight=0` in the off-run.
- **`-BENCHMARK`** and `-nowrite` are on the fixture launch. Paris ran `-game -nowrite -d3d11`.
- **FE3b:** one UBT child per session (2 sessions), both `-Mode=ValidatePlatforms -allplatforms`, read at creation. No fallback re-run.

## 1. G4: outline on against outline off, paired, seed 4242
- **Reader:** `newmag44d.py`. Off is 094-44c's `texture-off\01`, on is 094-44d's `texture-on\01`.
- **Pairing:** 2,352 pairs. Anomaly tuples (id, target, affected, mask px), poses and cameras are equal on every row. Only grain phase, timing, absolute event frame numbers and census tag values differ.
- **New magenta: 0 on all 2,352 frames**, in both the strict class and a loose class sized for the exposure-darkened pink.
  - The 4-px ring around the texture masks is also 0.
  - Every frame differs from its pair, but by grain only: at most 2 levels, and 0 pixels above 8 levels.
- **Texture frames covered:**
  - checker: static 169, skeletal 117;
  - pink: static 65, skeletal 92.
- **Can-fail (reader):** 1–2 px magenta rings injected around real masks are caught on 40 of 40 frames, in both pure (255,0,255) and darkened (214,0,202) colours.
- **The on-run also passes G1–G3 by my reader:** missing_texture 22 events and corrupted_texture 13; IoU 1.000; 0 px outside the mask; edges 0.

## 2. G5
- **Three readings of `r.CustomDepth`, all 1:**
  - at startup (`"1" LastSetBy: Constructor`);
  - two live readings, taken in-process with `SystemLibrary.get_console_variable_int_value` when saved indices 81 and 2300 existed (engine frames 5968 and 8212).
- **The observer** (`capture_depth_read09444d.py`, 35 lines) only reads and logs; it sends nothing to a window.
- **Per-row state (on-run):** 0 rows with custom depth on, tagged or unexpected; 0 polluted; 0 legacy-reference rows; depth before = after (28 candidates, all off); 35 reverts, all with residual 0.

## 3. Paris regression (`-game`, EA_Core, default routes, seed 4242)
- **D3D11 1280×720: H4's earlier gates all pass by my `h4check44d.py`** (the 094-43r checker, which still reproduces 094-43r's D3D11 pass):
  - 2,352 frames, contiguous, 0 zero-byte; 780 masks = 780 affected frames;
  - events: blink 15, LOD 19, missing 22, corrupted 13; every LOD event changes ≥14,456 in-mask px;
  - census lost, no-pass, polluted and exhausted 0; player view = captured view; no set violations;
  - self-check `1280x720`, format 18, D3D11, `GameViewportClient`; 0 fatal lines; upload PASS;
  - scanner admitted with no host write; private pass blocking, depth and stencil flags 0;
  - **unavailable 0** (5 static components verified);
  - three `r.CustomDepth` replies, all 1; depth before = after.
- **Texture G1 passes:** missing_texture fires 22 times (EA_BlinkCube 3, EA_HideCube 12, EA_LOD 7) and corrupted_texture 13 (4, 5, 4). EA_Core has no skeletal actor.
- **Texture G2 passes:** every texture frame changes in-mask pixels against a grain-matched clean frame at the same pose.
  - Pink changes 100 % of the mask on the two cubes and 98.4 % on EA_LOD.
  - The checker changes 40 % on the two cubes and 96 % on EA_LOD.
- **Texture G3 FAILS:**
  - Whole-frame IoU minimum: **0.0889** (mine), 0.0879 pink / 0.0956 checker (Codex). Up to **260,214 px** change outside the mask.
  - The timing is right. On every event the frame before the fire and the frames 1, 2 and 4 after revert change **0 px**, and all 70 edge frames are clean.
  - The outside change spreads far. At each event's middle frame, the share lying more than 64 px from the object is about 52 % (checker on the cubes), 89 % (pink on the cubes) and 91 % (EA_LOD). On EA_LOD, 92k–119k px lie more than 256 px away.
  - **The scene is overexposed:** 33.3 % of each reference frame is saturated (max channel ≥ 250). The untouched targets' mean luminance is **0.948–0.975 of full scale**, against GH2's 0.2–0.8 band. The host's bloom is on (EA_Core ran eye quality 0 and no bloom pin, as in 094-43r's H4).
  - My panel (`example-paris-d3d11.png`) shows a white scene with halos: a magenta glow around the pink cube and a bright halo around EA_LOD. The after-revert column is black.
  - ⛔ I have not established the mechanism. The pattern (same-frame onset and end, far spread, saturated target) matches 094-44r's fixture bloom halo.
- **D3D12 1280×720 and D3D12 1279×719: unrun.** Codex stopped launches on the failed gate, as the stop rule says.

## 4. Defaults
- **Local `4cc8b45`:** blinking, lod_corruption, missing_texture, corrupted_texture.
- **Origin `8df75de`:** blinking + lod_corruption.
- **camera_clipping:** untouched, still `#if ANOMALY_ENGINE_51`. No Source change in `90d4275..4cc8b45`.

## 5. Stop times (IST)
- **Builds:** none.
- **Launches:**
  - fixture editor at 10:22:19;
  - Paris game at 10:30:40 (manifest 10:30:20), the last start; its log closed at 10:32:53.
- **Process audit:** 0 owned processes alive at 10:41:10 (Codex). My polls show none from 10:35:47 on.
- **Both limits kept by a wide margin:** 13:10 for launches, 13:35 for processes. My readers finished by about 10:53.

## 6. Safety
- **Crashes and uploads:** 0 crashes; no new crash, WER or dump files; upload checks PASS on both sessions; 0 fatal lines. CrashReportClient rewrote its own `Saved\Config` inis (known behaviour).
- **Paris:**
  - Content (1,456), plugin Content (7), Config (18), host and plugin Binaries (16 + 11), the descriptor and the `.uproject` are hash-equal.
  - Only `Intermediate\AssetRegistryCache` changed (20 files), from the launch.
- **Fixture:**
  - the nine assets, `Engine.ini` (`64d45f80`), `DefaultEngine.ini` and the `.uproject` are equal;
  - added `Scripts\texture_pose09444d.py` and `capture_depth_read09444d.py`.
- **Materials:** the 4.25 materials equal `8df75de`'s blobs; the 5.1 `.uasset`s equal d7eee97's in every copy and at every ref.
- **Binaries:** changed only in builds. There were no builds, and my sampler saw 0 binary changes.
- **Deletions and links:** 0 removed files in D:\IA_Europa, the run folder and E:\IA_Fixtures; 0 reparse points; P9 files unchanged.
- **m51 and refs:**
  - `m51` passes at start, prelaunch, exit and final.
  - Only local feat moved (`90d4275` → `4cc8b45`; commits fa207fb, 035ec20, 4cc8b45; tools and docs only).
  - origin stays `8df75de` (`ls-remote`). The spikes are local, and the fixture worktree is detached at `8e59a56`, clean.
- ⚠ **A disclosed kit change:** the Paris launch reserve dropped from 3 to 2 GiB (`host-capacity-rule.json`, frozen before launch; prior sessions max out at 1.470 GiB). The 50 GiB D: floor is unchanged. D: was 54.1 GiB.
- ⚠ **My sighting gap:** no poll from 10:26:17 to 10:35:47, while I ran readers. The Paris game (10:30–10:33) was not sighted by me. Its identity and exit rest on Codex's creation records and audit. There were 12 poll lines; the other 10 gaps were 1.2–4.6 min.
- Gotchas on feat now reach G538, so the next free number is G539.

## 7. Push and card
- **Nothing pushed.** Every commit after `8df75de` descends from `8e59a56`, which changes the default routes. Under GH4, nothing stands alone without the texture gates.
- **No pre-push scan was written.** The office card stays `_reviews\094-43-office-card-europa.md` (at `93613ae`).

## 8. Decisions for chat
1. **Paris texture G3 on an overexposed, bloomed bench scene.** EA_Core's targets sit at 0.95–0.98 of full scale, so the swap's light spreads far outside the object.
   - (a) Apply GH2's bench-only runtime fix to EA_Core for the regression: bloom 0, and a fixed exposure that puts the targets in 0.2–0.8. Keep `-game`, the whole-frame rule and 0.98. Then rerun D3D11, D3D12 and 1279×719. **I recommend (a):** it is GH2's own rule ("fix the fixture, never the gate"), and EA_Core is our bench map inside the Paris project.
   - (b) Carry whole-frame G3 on the fixture only, and hold Paris to G1, G2, the timing edges and the residuals (A35-like; GH2 declined this).
   - (c) Keep the textures out of the default pool until a host-mode G3 is defined.
2. **The kit's launch reserve, 3 → 2 GiB:** confirm or revert it.
3. **Then:** 094-45 (camera_clipping) after 15:15, and the D3D11/D3D12/1279×719 texture regression under the ruling. The default-route push still waits on both.
