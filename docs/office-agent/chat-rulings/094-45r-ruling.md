# 094-45r — Chat ruling: Stage 1 is accepted and pushed. The textures pass strict G3 on all three Paris legs, and `c5d0919` is on origin with the new card. camera_clipping works, but D4's "unlabelled frames match" failed on 1–3 px of ±3 levels. Measure the session noise floor first, then judge against it. 094-46 runs after the owner's trip

**Date:** 2026-11-04, about 19:30 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-45r:** NEEDS-DECISION. It ran 16:18–19:22 IST.
- `_reviews\094-45r-codex-report.md`, `_reviews\094-45r-code-review.md`, `_reviews\094-45r-stage1-prepush-scan.md` and `_reviews\094-45r-office-card-europa.md`.

## Rulings

### GN1. Accepted: Stage 1. **Phase B textures are DONE**
- **The bench fixes:**
  - a mid-blue base material on the four targets;
  - the point light off, with six shadowless fills;
  - the exposure recalibrated to −1.875 EV (all 15 brighter values fail);
  - the camera held on `EA_Camera`;
  - the pre-fire swap visibility at 100 % on every target. GM2's fallback was never needed.
- **The three Paris legs** (D3D11 1280×720, D3D12 1280×720, D3D12 1279×719): 2,352 frames each, 0 drops.
  - H4 passes: 780 masks, and every LOD event changes its target.
  - The textures reach **strict whole-frame IoU 1.000**, with 0 spill, 0 of 140 edge frames changed, and 0 of 35 reverts with residue.
  - G5 is clean, and `unavailable` is 0.
- **The push:** fast-forward `8df75de..c5d0919`, confirmed with `ls-remote`, and the scan is clean.
- **The default pool on origin** is now blinking, lod_corruption, missing_texture and corrupted_texture (D5).
- **The office card is `_reviews\094-45r-office-card-europa.md`.** It replaces 094-43's.

### GN2. camera_clipping: the behaviour is right. D4's "unlabelled frames match" is judged against a measured noise floor
- **What passed:**
  - the three slab-positive spans are labelled exactly (624–911, 1200–1487, 1776–2063), and each changes at least 902k px against the control;
  - the baseline-near can-fail labels 0 frames;
  - the near plane is restored;
  - the slab self-test passes 154 checks.
- **What failed:** 914 of 1,488 unlabelled frames differ from the control by 1–3 px of exactly ±3 levels, at 8 fixed screen positions. On top of that, about 40 % of pixels differ by ±1–2 levels on every unlabelled frame. That is session-to-session render noise, not a clip.
- **The rule, in this order:**
  1. **First, measure the floor:** a control pair, two sessions at the baseline near plane with the same seed and the same bench settings. Record the per-pixel |Δ| distribution and the count of pixels above each level, frame by frame.
  2. **"An unlabelled frame matches" means:** its difference from the control stays within the floor. Every pixel's |Δ| is no more than the control pair's maximum |Δ|, and the count of pixels at that maximum is no more than the control pair's maximum count for any frame. Both thresholds are recorded before the comparison.
  3. **Labelled frames** must still differ far above the floor. 902k px against a few pixels is already conclusive.
- *Why:* D4 asks whether a frame shows the clip. Two clean sessions already differ by a few pixels, so "identical" is a stricter bar than two controls can meet. The floor is measured, never assumed.

### GN3. The Paris regression after Stage 2 runs **the default routes plus camera_clipping requested**, on all three legs
- **Why:** Stage 2 touches the shared capture path. `run_summary.json` is now written on every run, and every fire gains a `whole_frame` key.
- **The gates:**
  - all of H4 and the textures' G1–G3 still pass;
  - camera_clipping's D4 holds under GN2;
  - a default run without camera_clipping is **additive-only** against `c5d0919`'s outputs: no existing key is removed or changed, and only new keys appear;
  - the office card's read commands still work.

### GN4. Key parity with 5.1
- **Port the missing run-summary counter** `label_transition_camera_clipping_unconfirmed_entries` with 5.1's name and meaning. Names are kept unchanged (D1).
- **The three added `session_global_*` keys** are accepted as additive compatibility keys, documented in `docs/europa/`.

### GN5. 094-46 is queued, not staged. The owner's laptop shuts down at 20:45 IST for travel
- **What it covers:**
  - GN4's counter;
  - GN2's control pair, then D4 judged against the floor on D3D11 and D3D12;
  - GN3's Paris regression on three legs;
  - Code's scan, push and card update (how to request camera_clipping for a held session).
- **When:** stage it when the owner is back and has restarted the watcher, starting from the local `c706b7c` on top of `c5d0919`.

## For the owner
- 🟢 **The checkered and pink textures are done and pushed,** in the default set, passing every strict test on Paris with D3D11 and D3D12. **New office card: `_reviews\094-45r-office-card-europa.md`.**
- 🟡 **Camera clipping works.** It clips exactly where it should and nowhere else. The only problem is that our "nothing changed" test was stricter than two normal runs can ever be, with a few pixels of noise. After your trip, we measure that noise first and judge against it, then re-test Paris and push.
