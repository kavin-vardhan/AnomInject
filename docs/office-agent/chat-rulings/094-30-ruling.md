# 094-30 — Chat ruling: the Phase C capture-port plan is approved, with Code's three amendments. C1 (plugin frames on 4.25) starts now (094-31)

**Date:** 2026-10-31, about 10:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's plan:** `docs/europa/PHASEC-capture-port-plan.md`, commit `4f46a9c` (docs only).
- **Codex's report:** `_reviews\094-30-codex-report.md`.
- **Code's review:** `_reviews\094-30-code-review.md`. It checked 9 inventory rows against 4.25 source and confirmed all 9.

## Verdict
**The plan is accepted:**
- three stages in the owner's order: C1 frames, then C2 labels in sync, then C3 masks;
- 36 inventory rows;
- the 4.25 differences have source anchors: texture readback without a pitch/height `Lock`, the absence of `OnWorldTickEnd` (PostActorTick instead), the RDG handoff, the SceneViewExtension hooks, and custom stencil;
- a C1 file-by-file list, with shaders and ControlServer kept off in C1.

**There was no measured compile.** Isolating a scratch build would have broken the lean profile, so C1's first build is the real measurement.

## Rulings

### FL1. The plan is approved as the Phase C route, amended by FL2–FL4

### FL2. C2 must catch a one-frame offset at every kind of edge (Code's risk 1)
- **The negative tests:** a separate one-frame shift mutation for apply edges, for revert edges, and for blinking's interior toggles. **Each must fail on its own.**
- **The pacing-off block:** the C2 session includes one block with capture pacing off (`IAI.Capture.Pace 0`). That is where 5.1's arm→present pairing once read −1 (P9).
- **The gate stays:** zero mismatched frames, and an ambiguous edge fails.

### FL3. The readback adapter survives an incomplete map (Code's risk 2)
- **The rule:** a staging map that returns 0×0, or a size or pitch that doesn't match the request, is dropped and counted. It is never written as a frame.
- **Recorded per session:** the RHI in use (D3D11 or D3D12), and the dropped count.
- **Later:** the office smoke records the renderer too.
- **The test:** the C1 negative test covers padded, undersized and 0×0 maps.

### FL4. C3 gets a metric per anomaly, frozen before C3 runs (Code's risk 3)
**The masks per anomaly:**
- **blinking:** the target's silhouette while it is drawn;
- **missing_object:** the counterfactual silhouette, that is, where the object would be;
- **lod_corruption:** the target's footprint. Whatever the visible change is, it must lie inside the mask;
- **lighting_mismatch:** the affected receivers' footprint, not the light actor.

**The thresholds:** IoU, recall and spill are proposed in C3's request, frozen there, and never lowered after the results are seen.

### FL5. Housekeeping
- **The new gotcha is renumbered G488.** G475 is already used on another branch. This goes in C1's first commit.

### FL6. 094-31: C1, now, under the lean profile (FJ1)
**Implement the plan's C1 file list:**
- capture is compiled in on 4.25 for non-Shipping builds;
- `SHADERS` and `CONTROL_SERVER` stay 0;
- FL3's adapter rule applies;
- the 5.1 paths stay intact behind their guards;
- the keyword-Rotator check and the Python 2.7 harness cover any changed in-editor script.

**Build the Paris host's two 4.25 targets** (Editor Development and Game Development):
- this is authorized to replace the host's plugin and host binaries;
- re-bind the runtime kit's binding manifest to the new hashes;
- start UnrealBuildTool at below-normal process priority, so the owner's PC stays usable; that is not a machine setting;
- keep the existing build memory gate;
- if a compile error survives two fix attempts, report it.

**The C1 gate (the plan's), in two real sessions under FJ1's gate:**
- Editor PIE on EA_Core, and standalone `-game` on EA_Travel;
- each requests 60 plugin PNGs at native 1280×720 within 90 s;
- **each must show:**
  - the exact count, with no missing or duplicate index;
  - no black frames and no unknown format;
  - the correct pixels in the check region;
  - the RHI and dropped count recorded (FL3);
  - a normal exit.

**The negative test:** a padded, undersized or 0×0 readback is refused.

**Report:**
- the compile-error count per module on the first build, which is the real measurement;
- the fixes;
- both sessions' results.

**The window for real sessions:** until 07:00 IST on 11-01.

## For the owner
- **Nothing to do.**
- **The push:** the branch tip is now `4f46a9c`, which adds only the plan document. If you push today, the office test uses that, and its source is the same as the tested `abb240b`.
