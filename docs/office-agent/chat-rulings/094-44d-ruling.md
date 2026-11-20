# 094-44d — Chat ruling: G4 and G5 pass for both textures (0 new magenta, three live readings). Paris D3D11 failed texture G3 on the same overexposure and bloom halo, now on EA_Core. Apply GH2's bench fix to EA_Core and rerun the three Paris legs (094-44e), then push

**Date:** 2026-11-04, about 11:05 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-44d:** NEEDS-DECISION. It ran 10:03–10:56 IST.
- `_reviews\094-44d-codex-report.md` and `_reviews\094-44d-code-review.md`.

## Rulings

### GJ1. Accepted: G4 and G5 for both textures (fixture, frozen settings)
- **G4:** 0 new magenta on 2,352 paired frames. That covers checker 169 static / 117 skeletal and pink 65 static / 92 skeletal. The reader catches 40 of 40 injected rings.
- **G5:** `r.CustomDepth` reads 1 at startup and at frames 81 and 2300, in-process. The state is clean, and all 35 reverts leave residual 0.

### GJ2. Accepted on Paris D3D11: every earlier H4 gate, and texture G1/G2
- 2,352 frames, 780 masks, LOD changes of at least 14,456 pixels, 0 census errors, 0 `unavailable`.
- missing_texture fires 22 times and corrupted_texture 13 times on three cubes. Every fire changes its target.
- Timing is exact: the frame before each fire and the frames after each revert change 0 pixels.

### GJ3. Option (a): apply GH2's bench-only fix to EA_Core, keep the gate, rerun the three Paris legs
- **The cause:** EA_Core is overexposed, with 33 % of the frame saturated and the targets at 0.95–0.98 of full scale, and the host's bloom is on. This is the same halo GH2 fixed on the fixture.
- **What to apply:** the same runtime-only settings, active only under `-IAIBench` on EA_Core, recorded in the receipt, never saved, and nothing in the Paris host's Content or Config:
  - bloom off;
  - manual exposure (eye-adaptation quality 2, method 3) set so that the untouched targets' mean sits between 0.2 and 0.8 of full scale;
  - no saturated target.
- **The pre-fire checks** must pass: the checker's greys at least 40 levels apart, and the pink with R and B ≥ 200 and G ≤ 80.
- **Kept as they are:** `-game`, the whole-frame G3 rule and the 0.98 bar.
- **Not adopted:** (b) fixture-only G3, and (c) textures out of the default pool.
- **The host's own look stays untouched outside the bench.** At the office, the game's own bloom and exposure apply, as in 5.1.

### GJ4. The launch disk reserve goes back to 3 GiB
- D: has about 55 GiB free, so 3 GiB costs nothing.
- If 3 GiB ever blocks a launch, stop and report rather than lowering it.

### GJ5. 094-44e now, before the blackout. camera_clipping stays 094-45 after 15:15
- **What it covers:**
  1. GJ3's EA_Core bench fix, with its pre-fire checks;
  2. the Paris regression on D3D11 1280×720, D3D12 1280×720 and D3D12 1279×719: H4's gates plus texture G1–G3, and G5 where it applies;
  3. if all three legs pass, Code's scan, push and office card, with the textures in the default pool.
- **The textures' default-route change ships on its own gates (GH4).** camera_clipping is opt-in and independent, so the push does not wait for it.
- **The blackout rules stand:** no build or launch after 13:10 IST, and everything closed by 13:35.

## For the owner
- 🟢 **No outline leak from the textures:** 0 leaked pixels on 2,352 frames.
- 🟡 **Paris failed one test for the same reason as before:** our bench level there is too bright, with glow. We're fixing that bench level's lighting (bench-only; your game's look isn't touched) and re-running Paris on D3D11 and D3D12. It should finish well before 13:35.
- If it passes, the textures get pushed as part of the default set, with a new office card.
