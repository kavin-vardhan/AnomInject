# 094-44c — Chat ruling: the texture anomalies pass G1–G3 perfectly once the scene is fixed, and both can-fails fail. The live G5 reading moves in-process. 094-44d runs G4, the Paris regression and the push before the owner's 13:45 blackout. camera_clipping follows after 15:15 (094-45)

**Date:** 2026-11-04, about 10:05 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-44c:** NEEDS-DECISION. It ran 07:48–09:57 IST.
- `_reviews\094-44c-codex-report.md` and `_reviews\094-44c-code-review.md`.

## Rulings

### GI1. Accepted: the fixture fix (GH2/GH3) and the texture gates G1–G3 (local `90d4275`)
- **The bench settings,** runtime-only and frozen at 09:25:32:
  - bloom off;
  - manual exposure at −2.1875 (eye-adaptation quality 2, method 3);
  - six shadowless fill lights;
  - FOV 30.
- **Codex's source finding stands:** in 4.25, quality 0 forces exposure compensation to 1.
- **The static target** is `OF_LOD_B` at 7.15 %, and the skeletal target is at 6.69 %. The ceiling is unchanged.
- **The results:**
  - G1: missing_texture fires 13 times on the static target and 9 on the skeletal one; corrupted_texture fires 5 and 8. GH3 asked for at least 3 each, so this passes.
  - G2 changes every mask pixel.
  - G3 reaches IoU 1.000 with 0 out-of-mask pixels, and the edge frames are clean.
- **The can-fails:** the usage-flag mutant changes 0 pixels on 365 frames, with the engine's own log naming the cause, and the early label fails G3 (IoU 0).

### GI2. G5's live `r.CustomDepth` readings are taken in-process, through the fixture's Python driver (option b)
- **The three-reading rule stays:** one at startup and two live (about frame 80 and frame 2300).
- In-process means a direct console-variable read logged by the driver, never a console command sent to a window.
- 094-44c's per-frame state (0 components, 0 polluted, residual 0) is accepted. Only the two live readings are owed.

### GI3. The fixture's texture gates may run in an editor play session with `-BENCHMARK`. The Paris host stays `-game`
- `-BENCHMARK` is mandatory for every fixture editor launch from now on. It stops the Epic launcher auto-start.
- Its fixed time step, fixed seed, no sound and no ini writes all suit the bench.
- *Why the Paris host stays `-game`:* the regression is the real-host check and must not change mode.

### GI4. The early-label can-fail is accepted at reader level for the texture gates
- It proves G3's reader catches a one-frame shift, and the edge frames prove the runtime timing (0 pixels changed on the frame before and on the first reverted frame).
- **A runtime one-frame-early mutant is FUTURE,** not a gate now.

### GI5. Closing launcher processes our editor started is accepted, only as descendants matched by PID, creation time and image, and recorded. Never by name
With GI3's `-BENCHMARK` this should not recur. If it does, record it again in the same way.

### GI6. The thin GH2 margins
- G4's outline-on run **reuses the frozen settings exactly**: the same binaries and the same values.
- If anything differs, re-qualify before any fire.

### GI7. 094-44d now, with a hard stop for the owner's blackout
- **What it covers:**
  - G4 (outline-on against outline-off, paired new magenta, for both textures and both target kinds), with GI2's in-process G5 readings;
  - then the Paris regression (step 6) on D3D11 1280×720, D3D12 1280×720 and D3D12 1279×719, with the textures in the default pool;
  - then Code's scan, push and card (the default-route change goes out only with these gates passed, GH4).
- **The blackout:** the owner's laptop is in personal use **13:45–15:15 IST**.
  - **No build or launch starts after 13:10 IST.**
  - **All editor, game and build processes are closed by 13:35 IST.**
  - Anything unfinished stops as NEEDS-DECISION with its partial results.
  - Code's own pixel readers also finish by 13:35. After that, only writing text.
- **094-45 (camera_clipping, steps 7–8) is staged after 15:15.**

## For the owner
- 🟢 **The checkered and pink anomalies work, and their labels are perfect** on both a normal object and a skeletal one. Deliberately broken versions are caught by the tests, as they should be.
- 🟡 **One reading wasn't captured,** so now it's read from inside the game instead. That's a quick fix.
- 🟢 **Next, until about 13:30:** the outline-leak check for the textures, the Paris re-test on D3D11 and D3D12, then the push and an office card with the textures in the default set. Everything stops by 13:35 for the interview.
- **After 15:15:** camera clipping.
