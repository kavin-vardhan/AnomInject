# 094-44r — Chat ruling: FE3b, the 4.25 loaders and GF4 are accepted, and `8df75de` is pushed. The texture swaps are right, but the bench scene is overexposed with bloom on, so G3 failed on a glow halo. Fix the fixture, never the gate. 094-44c resumes from `2c99a6f`

**Date:** 2026-11-04, about 07:55 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-44r:** NEEDS-DECISION. It ran 05:43–07:42 IST.
- `_reviews\094-44r-codex-report.md`, `_reviews\094-44r-code-review.md` and `_reviews\094-44r-prepush-scan.md`.

## Rulings

### GH1. Accepted and pushed (`93613ae..8df75de`): FE3b (`ae82fe7`), the compiled 4.25 material loaders, and GF4 (`8df75de`)
- **FE3b:**
  - a 10 ms non-elevated poll;
  - its self-test catches 12 of 12 children of about 104 ms;
  - in real runs, 6 of 6 startup children were read at creation, all `ValidatePlatforms`.
  - It admits only `ValidatePlatforms` and `QueryTargets`, which is stricter than GG2 and accepted.
- **GF4:** SkeletalCube and the three-instance static mesh both reach IoU 1.0 on 1,896 frames, with 0 new magenta. The private pass is now proven live on skeletal and instanced meshes.
- **The scan is clean.** The carried build-warning path item is accepted as before.

### GH2. G3's failure is the bench scene, not the anomalies. Keep the whole-frame rule and the 0.98 bar; fix the fixture
- **What the evidence shows:**
  - the swaps and reverts are right;
  - G2 changes 76,619 of 76,619 mask pixels on every labelled frame;
  - the edge frames are clean;
  - G5 is clean.
- **The failure:** 213k (checker) and 233k (pink) pixels change *outside* the mask. That's a bloom halo around a saturated target on a black background. The checker reads as near-white (mean 252) and the pink as pale pink (255, 225, 254). The scene is overexposed, exactly the R5 risk that GE4 said to fix in the fixture.
- **The fix,** bench-only runtime settings on the fixture, recorded in the receipt like the existing AA, motion-blur and exposure settings, never saved:
  - bloom off;
  - auto-exposure off, with a fixed exposure chosen so the untouched target's mean luminance sits between 0.2 and 0.8 of full scale;
  - a light or background that doesn't saturate.
- **Before any fire, show it:**
  - the checker's two greys differ from each other on screen by at least 40 levels;
  - the pink reads as magenta: R and B ≥ 200, G ≤ 80.
- **Not adopted:** scoping G3 to the object (A35). It would loosen the gate. The plugin's host behaviour is unchanged; at the office the host's own bloom stays, as in 5.1.

### GH3. A static texture target must fire. Bring it into the 0.5–25 % band by fixture pose or scale, never by raising the ceiling
- The fixture's static target covers 27.9 % of the frame. Move the camera or scale the target until it sits inside the band.
- **G1 then means:** at least 3 fires of each texture anomaly on a **static** target, and at least 3 on the skeletal target.

### GH4. The default-route change (`8e59a56`) is pushed only together with its passing gates
- Until then, origin's defaults stay blinking + lod_corruption.
- The office card at `93613ae` stays valid.

### GH5. Can-fail mutant materials may be authored in the fixture's own Content, in a dedicated fixture editor session
- Mutants live in fixture Content only, never in plugin Content, and are listed with hashes.
- `M_MissingTexture_NoSkeletal` is accepted on those terms.
- **The runtime can-fails must run this time:**
  - the usage-flag mutant renders grey and G2 fails;
  - a one-frame-early label fails G3.

### GH6. 094-44c resumes from `2c99a6f` (local), with the same request and thread, plus GH2–GH5
- **The order:**
  1. the fixture fixes (GH2, GH3);
  2. step 5's gates again (G1–G5 and the can-fails);
  3. step 6, the Paris regression on D3D11 and D3D12, 1279×719 included;
  4. steps 7–8, camera_clipping and its gates;
  5. step 9, the defaults;
  6. step 10, the exit;
  7. Code's scan, push and card.

## For the owner
- 🟢 **Pushed:** the 4.25 pink and checkered materials, the safety-check fix, and the new skeletal and instanced test objects. The labels on those objects are perfect.
- 🟡 **The texture anomalies work, but our test scene was too bright.** The checker looked white and the pink looked pale with a glow, so the strict label test failed on the glow. We're fixing the test scene's lighting, not loosening the test. Camera clipping runs right after.
