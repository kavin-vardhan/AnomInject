# 2026-07-01 — 017 — Stencil-capture Stage 3a (occlusion-correct box + last-known-stencil + foliage exclusion)

Branch `feature/stencil-capture` (off `master` `d4a77db`). NOT on `master`. No tag (BB milestone tag waits for the full close).

## Goal
Replace the m7 projected 2D-AABB label box with a **GPU-truth, occlusion-correct** box derived from the rendered silhouette,
for render-type anomalies; keep the projected "hole" box for genuinely-hidden ones (LOCK-1). Fold in the async backbuffer
color path from Stage 1. Owner-eyeball-gated.

## What was done
- **Renderer re-added + Scene View Extension.** `FAnomalyStencilSceneViewExtension` (after-tonemap) dispatches a global mask
  shader `AnomalyVisibleMask.usf`: for each pixel, keep the Custom-Stencil tag iff Custom-Depth passes the Scene-Depth
  (reversed-Z) occlusion compare, else 0. `PF_R8_UINT` mask RT → `FRHIGPUTextureReadback` → render-thread reduce to a per-tag
  pixel-AABB (`FAnomalyMaskResult`). Color (Stage-1 backbuffer) + mask are two grab points joined by submit `GFrameCounter`,
  with a bounded hold if the mask resolves a frame late. Module `LoadingPhase` moved `Default → PostConfigInit` (a module that
  defines a global shader must load early enough to register the shader-source-dir mapping — else `!bInitializedSerializationHistory`).
- **Isolation (H1 vs H2).** Before changing box logic, isolated whether the loose flicker box was the stencil path or the
  projected fallback. Analyzed an existing self-labeling run (missing_texture always-visible ⇒ guaranteed stencil box; flicker
  alternates) — **H1 confirmed**: the stencil reduction is correct (tight box, goes invalid at 0 px); the loose boxes were ONLY
  the projected fallback on flicker's hidden frames.
- **Fix A — box computation moved into the subsystem + last-known-stencil cache.** The per-fire box decision now lives in
  `UAnomalyCaptureSubsystem::ResolveFireBoxes` (owns the snapshot + mask + a per-fire `TMap<id+target, normalized box>` cache);
  the writer's `BuildFrameLabelRecord` is now a **pure serializer** of pre-resolved boxes (schema byte-identical — `verify_capture.py`
  unchanged). Rule: rendering → live stencil (cache it); hidden+`flicker` → cached last-good stencil (projected only if no cache
  yet); hidden+`missing_object` → projected. Cache cleared at StartRun / FinishRun / Deinitialize.
- **Fix B — clean captures (LOCK-2).** A run **force-disables StencilViz** (restore prior + tag on finish) so the debug green
  never bakes into the post-Slate backbuffer. Confirmed StencilViz is the ONLY plugin path that reaches captured pixels (it's a
  scene post-process → scene color → backbuffer); all other plugin visuals are Slate/canvas and already suppressed (Stage 1).
- **Foliage exclusion (pulled forward from 3b).** `AInstancedFoliageActor` excluded from the renderable-visible pool in the
  shared classifier `ClassifyRenderableVisibleLive` (selector + auto + dashboard inherit). Matched by reflected class name
  (no Foliage-module dep). Two reasons: level-spanning bounds + HISM ignores actor-hide (G59). Scoped to the foliage container
  only — standalone ISM/HISM stay eligible.

## Problem → Resolution
- Editor crash on launch (`!bInitializedSerializationHistory`, Shader.cpp) → `AnomalyCapture` `LoadingPhase` → `PostConfigInit`.
- `AsShared()` cast (`TSharedRef<FSceneViewExtensionBase>` → `TWeakPtr<derived>`) → `StaticCastSharedRef`.
- Loose flicker box → NOT a stencil bug (H1); the projected fallback on hidden flicker frames → last-known-stencil (Fix A).
- Green in captured frames → StencilViz left on → force-off during a run (Fix B).
- Owner re-eyeball kept landing on `InstancedFoliageActor` (grotesque projected box + persistent green when "hidden") → the
  foliage exclusion pulled forward so auto picks clean single-mesh targets.
- Anon-namespace serializer couldn't see `AnomalyLabel::FResolvedFireBox` unqualified → qualify it (compile fix).

## State
- **Clean 5.1 Dev-Editor compile (exit 0)** for Fix A/B (full Build.bat); the foliage exclusion (core `.cpp`-body change) patched
  via **Live Coding** ("Live coding succeeded") into the owner's open editor. `IAnomaly` untouched; catalog stays 8.
- Committed on `feature/stencil-capture` as one `feat(capture)` commit (Fix A + Fix B + foliage exclusion + docs). No tag.
- **Ship split:** owner needed the async capture on `master` urgently → `master` fast-forwarded to **Stage 1 only** (`468ed6b`);
  Stage 2 + Stage 3a + remaining BB work stay on `feature/stencil-capture`, unmerged.

## Hand-off / next
- Owner re-eyeball on CLEAN targets: partially-occluded box excludes hidden geometry; tight box on the skeletal Bot; pixel-for-pixel
  alignment; flicker tight across the blink cycle (Fix A); clean PNGs (Fix B).
- Then Stage 3b (alignment hardening + all-types), Stage 4 (multi-actor + tag). BB milestone tag at the full close.
- Banked (G59): foliage-as-target would need per-instance hide handling — out of scope for v1.
