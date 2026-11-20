# 094-43a — Chat ruling: the name-lookup route links, adds no shader recompile and matches opaque and hidden targets exactly. It drew masked/WPO objects one frame before the engine admitted them, so the private pass gets a scene-depth admission test. 094-43 proves that, then builds FZ2 for real

**Date:** 2026-11-04, about 01:30 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-43a:** NEEDS-DECISION. It ran 23:31–about 01:30 IST.
- `_reviews\094-43a-codex-report.md`, `_reviews\094-43a-code-review.md`, `_reviews\094-43a-fz9-findings.md`, and `_reviews\094-43a-example-frame708-private-early.png`.

## Verdict
**NEEDS-DECISION, resolved here. The route stands.**

## Rulings

### GD1. Accepted: GC6 (`576975d`), the guarded fixture scope (`7688ddf`) and the evidence (`75ca580`), local and unpushed
- **The self-test:** 7 of 7 pass.
- **The real exits are now classified correctly:** `reference/01` reads `owned-stop`, and the genuine shader-startup failure stays `crash`.

### GD2. The depth-only route by name lookup is the FZ2 route. The fallback (a new shader type) is retired
- **P1:** it links, with 0 LNK errors.
- **P2:** it adds no first-launch recompile (0 `Missing cached shader map` lines).
- **P5:** it leaves the host's custom depth alone (0 enabled, 0 tagged, `r.CustomDepth` unchanged).
- **P3, the parts that pass:** the opaque objects match the legacy labels exactly (IoU 1.0, floor 0.99992), and all 132 blink-hidden frames match.
- **Accepted substitutions:** looking shaders up through the public material shader-map chain (`FMaterial::GetShader` is private), and using scalar `R32_UINT` scratch.
- **The cost is small:** 0.49 ms of GPU time per frame (p95 0.63), and 0.21 ms to submit (p95 0.33).

### GD3. P3's failure is a timing difference, not a route failure. Fix: an admission test against the frame's own scene depth
- **What happened:** the private pass draws every component immediately. The engine admits a newly uncovered primitive only after its occlusion query returns, about one frame later. So at frame 708 the private ID showed 4,962 WPO pixels where the saved colour and the scene depth have no object yet. The masked failure at frame 815 is the same thing.
- **The label must match the saved colour,** never the true geometry. So:
  - **Visible coverage (the census and visible event masks):** keep a private ID pixel only where the private depth equals the scene depth of the frame being saved, within a small tolerance.
  - **Hidden event targets** (missing, blink-hidden): keep the pixel where the private depth is in front of or equal to the scene depth (`≤`, within the same tolerance). They are absent from the scene by design, but the scene still occludes them. This already matched 132 of 132, and must keep matching.
  - **The tolerance:** stated in the receipt and justified by the same position path, not tuned to pass.
  - **Scene depth** is read through a public path, the scene-texture uniform buffer that the existing global shaders already use. No Renderer-private header.
- **Code's offline check agrees:** applying exactly this rule to 094-43a's own data (`p3sim_drawn43a_v2.py`) passes P3 on every object. Masked and WPO reach 1.0, the hidden targets stay 1.0 and non-empty, and the minimum is 0.99992. This is one run, so Stage A proves it live.
- *Why not replicate the occlusion admission instead:* the `PrimitiveVisibilityMap` lives in Renderer-private `FViewInfo`, and copying the query logic would drift from the engine. A depth comparison against the frame actually saved is exact and has no private dependency.

### GD4. The 26 lighting-group frames that could not be paired per object are noted, not gated
- lighting_mismatch is test-only (FZ4).
- 094-43's H2 reports them separately.

### GD5. Component scope for FZ2
The private pass must cover what Europa's targets are made of:
- static meshes;
- instanced and hierarchical-instanced static meshes;
- **skeletal meshes** (lod_corruption is static and skeletal), through a plugin-owned collection of the dynamic mesh elements.

Any component type the pass cannot draw faithfully is reported `unavailable` and becomes **ineligible** for census and event selection, with per-run counts in the receipt. It is never drawn with custom depth instead. If the host has a reader, the legacy path stays refused, exactly as FZ3 already does.

### GD6. FZ9: noted. One office read is owed
- **StackOBot:** no reader on any recorded map.
- **Lyra (home bench only):** a reader every time (`MPP_Outline`), so those bench sessions may carry the leak. Nothing from them was delivered.
- **Office and client 5.1 data (Concorde, Bates):** cannot be judged from home, and tagging was active in all of it.
- **The owner errand** (verified tooling, already on the m41 card's Section E): on the next office session on a post-m41 5.1 build, type home the `HOST-PP CUSTOM-DEPTH READERS` line for Concorde and for Bates. Numbers and the line only.

### GD7. The next job: 094-43, staged now. It is one job, gated in stages
- **Stage A, the proof:** the depth admission on the spike branch, then P3 and P4 on the fixture.
- **Stage B, the build (only if Stage A passes):** FZ2 on `feat/europa-port`, GD5's scope, FZ7, the card draft, and gates H1–H5.
- **Stage C, the push (only if Stage B is GREEN):** Code's pre-push scan and push, under the 094-39 rules.
- **In parallel, Code writes the Phase B plan** for camera_clipping, corrupted_texture (pink) and missing_texture (checkered), as in 5.1. This is the owner's top priority.

## For the owner
- 🟢 **The cheap fix works:** no shader recompile, and it leaves the game's outlines alone.
- 🟡 **One small timing bug:** it marked a newly visible object one frame before the game drew it. The fix is to check every label pixel against what is actually on screen in that frame. It's running tonight, together with the real build.
- 🔴 **One office errand, on your next 5.1 office session:** type home the `HOST-PP CUSTOM-DEPTH READERS` line for Concorde and for Bates. It tells us whether old 5.1 data could have outlines on labelled objects.
