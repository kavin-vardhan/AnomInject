# 094-44e — Chat ruling: GJ3's absolute colour bands were chat's calibration proxies, and they over-constrained EA_Core. The halo protection stays (bloom off, no saturated target pixel), while colour is checked by hue. The whole-frame G3 bar is unchanged. 094-44f runs before the 13:10 cutoff

**Date:** 2026-11-04, about 12:05 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-44e:** NEEDS-DECISION. It ran 11:05–11:58 IST.
- `_reviews\094-44e-codex-report.md` and `_reviews\094-44e-code-review.md`.

## Rulings

### GK1. Accepted: the bench helper and the calibration sweep
- **The helper** lives in the home Paris host's probe module (`Source`), with a byte-equal copy in `tools/europa`.
  - It runs only under `-IAIBench`, only on `EA_Core`, and never in Shipping.
  - It turns bloom off and sets manual exposure.
  - It never sets `r.CustomDepth`, and it is never saved.
  - It lives in the home host only, so it never reaches the office copy (the office copies only the plugin).
- **The sweep:** 65 exposures, from 0 to −8 EV. It is a sound measurement, and Code's recheck agrees.
- **The launch reserve** is back at 3 GiB.

### GK2. The pre-fire checks are restated around what G3 needs. G3 itself is unchanged
- **What GJ3 was for:** stopping the halo, meaning saturation and bloom spreading a swap's light past the object.
- **What went wrong:** the absolute colour bands (a target mean of 0.2–0.8, and the pink's R,B ≥ 200) were chat's proxies for "the swap looks right". They cannot hold together on EA_Core, because its cubes are unevenly lit.
- **The checks from now on, on every target, before any fire:**
  - bloom off;
  - **0 saturated pixels** on any untouched target (no channel at 255), which is the halo guard;
  - each untouched target's mean is at least 0.1 of full scale, so it is not black;
  - **the pink reads as magenta by hue:** R ≥ 2.5 × G, B ≥ 2.5 × G, and R and B ≥ 64;
  - **the checker reads as a checker:** its two greys are at least 20 levels apart;
  - use the **brightest** exposure that passes all of them on all targets.
- **If no exposure passes,** add the fixture's lighting recipe as bench-only runtime settings (shadowless fills, with EA_Light turned down) and recalibrate under these same checks. If that also fails, stop with NEEDS-DECISION.
- **Unchanged:** whole-frame G3 at IoU ≥ 0.98, clean edge frames, H4, G1, G2 and G5.
- *Why this is not loosening the gate:* G3 measures label-against-picture over the whole frame. The checks above only decide when the scene is fit to measure, and they now test exactly the things that broke G3: saturation, bloom, and an invisible swap.

### GK3. 094-44f now, inside the blackout window
- **What it runs:** GK2's calibration, then the three Paris legs (D3D11 1280×720, D3D12 1280×720, D3D12 1279×719), then, if all pass, Code's scan, push and card.
- **The blackout rules stand:** no build or launch after 13:10 IST, and all processes closed by 13:35.
- **If the legs don't all start in time,** stop cleanly with NEEDS-DECISION and the partial results. The rest resumes after 15:15.
- **camera_clipping (094-45)** follows after 15:15.

## For the owner
- 🟡 **The bench-level lighting test was stricter than it needed to be,** and that was my rule. The test level's cubes are lit unevenly, so no single brightness met every colour target. I've rewritten the check to look only at what matters: no glow, no blown-out pixels, and the pink and checker clearly recognisable. The strict label test stays exactly as it was.
- Re-running now. It finishes before 13:35, or stops cleanly if it can't.
