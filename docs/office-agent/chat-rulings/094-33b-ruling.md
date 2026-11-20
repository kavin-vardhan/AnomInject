# 094-33b — Chat ruling: the masks are ported. They sit exactly on the right objects, and the labels still match. Two of my FO3 rules were wrong, so C3 is re-scored on corrected rules (094-33c)

**Date:** 2026-11-01, about 05:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-33b:** NEEDS-DECISION. It ran 03:32–04:22 IST.
- **Codex's report:** `_reviews\094-33b-codex-report.md`.
- **Code's review:** `_reviews\094-33b-code-review.md`. Code recomputed every number from the raw files.

## Verdict
**The 4.25 mask path works:**
- **The descriptor fix** (shader-read in `TargetableFlags`) is correct, cited, and tested, and no other descriptor has the same mistake;
- **the masks:** IoU 1.000 and spill 0 for all four anomalies, against the plugin's own stencil and depth;
- **the negatives:** the one-frame mask shift fails at the event edges, and the wrong target fails for blinking, missing_object and LOD;
- **C2 holds on the same session:** 240 of 240 frames, with 0 mismatches;
- **no crash and no upload,** and the Paris host and `m51` are unchanged.

**But two of my FO3 rules were wrong:**
1. **Coverage of all changed pixels is ill-posed.** An anomaly's visible change spreads beyond its target:
   - shadows;
   - bloom glow on the backdrop;
   - for LOD, the outline the old mesh vacated.

   No target mask can cover those, so lighting's coverage came out at 0.56 and LOD's at 0.05. **This is not a threshold to lower: the metric measured the wrong thing.**
2. **The blinking clause put masks on drawn frames.** Drawn frames are labelled clean, and 5.1's `m44` rule forbids a mask on a frame labelled clean. Code caught it.

**Code's further point stands:** IoU 1.000 only shows that the shader agrees with a CPU recompute of the same stencil. It is not an independent visual check, and the precision check below supplies that.

## Rulings

### FQ1. Accepted: the 4.25 mask shaders, the descriptor fix, the IoU and spill results, and C2 holding

### FQ2. Replace coverage with an independent precision check (frozen here, before any measurement)
**Precision** is the fraction of a mask's pixels whose colour visibly changed between the clean pre-apply frame and the anomaly frame, beyond the ±1-level grain.

**The scoring session locks auto-exposure** through a command-line override, cited from 4.25 source, with no config edit. Then "changed" means a real local change, and the wrong-target negative stays meaningful.

| Anomaly | Gate |
|---|---|
| blinking (frames labelled anomalous only) | IoU(mask, the counterfactual silhouette) ≥ 0.90, and precision ≥ 0.90 |
| missing_object | IoU(mask, the counterfactual silhouette) ≥ 0.90, and precision ≥ 0.90 |
| lod_corruption | IoU(mask, the current silhouette) ≥ 0.90, and precision ≥ 0.80 |
| lighting_mismatch | mask pixels only on receivers (≤ 1 % outside their silhouettes dilated by 1 px), and precision ≥ 0.80 |
| all four | spill ≤ 1 % |

**The negatives:**
- **the one-frame shift** must fail;
- **the wrong target** must fail for all four. Precision now catches it for lighting too.

**These numbers are frozen.** They are set before precision has been measured, and are never lowered after.

### FQ3. Blinking follows 5.1's `m44`: no mask on a frame labelled clean
- **Hidden frames,** which are labelled anomalous, carry the counterfactual silhouette.
- **Drawn frames carry no mask.** The gate is 0 mask pixels on every frame labelled clean.
- FO3's drawn-frame clause is withdrawn.

### FQ4. 094-33c: fix blinking's masks, add precision, rebuild, and re-run C3
- **The C3 gates are FQ2 and FQ3.**
- **Plus:** the C2 reader on the same session, and FO3's spill.
- **The session:** one, plus one retry slot.
- **The rest:**
  - FM1's build gate;
  - FJ1's lean profile;
  - **the window for real sessions:** until 23:59 IST on 11-01.

## For the owner
- **Nothing to do.**
- **The masks line up on 4.25.**
- **The scoring rule was my mistake.** It is corrected and frozen before the re-run.
