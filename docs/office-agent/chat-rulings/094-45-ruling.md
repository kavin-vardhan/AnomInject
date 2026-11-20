# 094-45 — Chat ruling: GL3 and the view lock are accepted. On Paris the texture labels are exact (0 spill, clean edges, every fire changes its target), but whole-frame IoU is capped by surface that doesn't change: a checker grey equal to the original, and black shadows. Give the bench targets a distinct runtime base material and shadowless fills, add a pre-fire change check, and use a defined explained-pixel fallback so this can't loop again. 094-45r reruns Stage 1, then Stage 2

**Date:** 2026-11-04, about 16:25 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-45:** NEEDS-DECISION. It ran 14:49–16:14 IST.
- `_reviews\094-45-codex-report.md` and `_reviews\094-45-code-review.md`.

## Rulings

### GM1. Accepted: GL3 (`96d8f01`), GL2's view lock (`93e7435`, `422ac34`) and the D3D11 leg's H4, G1, G2 and G5
- **GL3:** 9 of 9 tests pass, Code's three mutants each fail, and 094-44f's receipt replays as `owned-stop`.
- **The view lock:** `EA_Camera` is held for the whole session. The cubes sit at 8.5 % and 12.9 %, centred, and pass GK2 in `-game` at −5 EV.
- **The bench probe in the plugin's capture code** is accepted. It is excluded from Shipping, runs only under `-IAIBench` on EA_Core, with auto-injection off, and stays inert at the office.
- **The D3D11 leg:**
  - H4 passes: 2,352 frames, 69 events, 780 masks, 0 `unavailable`, and every LOD event changes its target;
  - texture G1/G2 pass: every fire changes its target, with **0 changed pixels outside the masks**, 140 clean edge frames, and residual 0;
  - G5 is clean.

### GM2. G3 on Paris: make the swap visible on every target pixel. Keep the 0.98 bar
- **The measurement:** on the cubes, half the checker's pixels change by 2 levels or less, because one checker grey matches the original surface. On the floor, 40,450 pixels are pure black in both frames, in the cubes' shadows.
  - The label is right: there is no spill and the edges are clean. The swap just isn't visible everywhere.
  - EA_LOD already reaches IoU 1.0.
- **The bench-only runtime additions** (`-IAIBench`, EA_Core only, never saved, the host's files untouched):
  1. **A distinct base material** on the EA_Core texture targets for the session, applied before C5 starts, so the targets look unlike both checker greys and unlike the pink. The anomaly then swaps from that material and restores to it.
     - Use one of the plugin's own simple materials, or a dynamic instance of the engine's basic material with a mid-tone colour (for example a mid blue).
     - The receipt records it.
  2. **Shadowless fill lights,** the fixture's recipe (GI1), so that no target pixel is black.
  3. **A new pre-fire check:** in the `-game` preview, each swap changes at least 98 % of its target's silhouette by more than 2 levels. Check both textures on every target that can be selected.
- **EA_Floor:**
  - add it to the helper's previews and every pre-fire check;
  - if it can't pass them, keep it out of texture selection on the bench through a bench-only exclusion that the receipt records.
- **A defined fallback, so this can't loop again.** If any target still fails the 98 % pre-fire check after the steps above, G3 for that target becomes the **explained-pixel G3**:
  - 0 changed pixels outside the mask;
  - clean edge frames;
  - and every unchanged mask pixel accounted for, either because the swap colour is within 2 levels of the original at that pixel (checked from the frame before the fire), or because it is black (0,0,0) in both frames.
  - The share of mask pixels that are changed or accounted for must be ≥ 0.98.
  - It is reported separately from whole-frame IoU and named as the fallback. The fixture's whole-frame IoU 1.0 (GI1) stays the strict proof.
- *Why this isn't loosening:*
  - Whole-frame IoU judges label against picture. Where the picture can't change, because the colours match or the pixel is in full shadow, no label could reach 1.0.
  - The fallback demands proof, pixel by pixel, that every unchanged pixel is unchanged for that reason, with zero spill.
  - The strict version has already passed on the controlled fixture.

### GM3. 094-45r now: the same two stages and the same handshake
- **Stage 1:**
  1. GM2's bench additions and pre-fire checks;
  2. the three Paris legs (D3D11 1280×720, D3D12 1280×720, D3D12 1279×719), each with H4, texture G1–G3 (or GM2's fallback, named) and G5;
  3. Code's scan, push and card.
- **Stage 2:** camera_clipping, as written.
- **The PC is free.** No blackout applies.

## For the owner
- 🟢 **The texture labels are correct on Paris:** zero stray pixels, exact timing, and every fire changes its object.
- 🟡 **The strict test still fails,** because on the Paris bench objects one of the checker's greys looks the same as the object's own colour, and some areas sit in black shadow. Those pixels can't change, whatever the label says.
- **The fix:** give those bench objects a distinctly coloured surface during the test, add soft fill lights, and check before firing that the swap is visible. If anything still can't change, the test has to prove it pixel by pixel. That way this can't bounce back to you again.
- **Next (094-45r):** Paris with the textures and push, then camera clipping.
