# 2026-06-30 · 016 · Stencil-capture milestone — Stage 2 (custom-stencil tagging + gate viz)

Branch `feature/stencil-capture`. Second stage: tag the anomaly target into the custom-stencil buffer
so the GPU rasterises its real silhouette (the input Stage 3 reduces to the occlusion-correct box).
Engine-only, no render deps (Renderer still deferred to Stage 3).

## What was done
- **`AnomalyCapture/Private/AnomalyStencilTag.{h,cpp}`** (gameplay-thread, Engine-only):
  TagActor/RestoreActor/RestoreAll tag each renderable mesh primitive (SM/SK) with
  `SetRenderCustomDepth(true)` + `SetCustomDepthStencilValue(tag)`, SAVING each component's prior
  `{bRenderCustomDepth, CustomDepthStencilValue}` and restoring EXACTLY (never assume 0 — host may use
  custom stencil). Reserved base `200`; per-fire tag = `200 + fire index`. `EnableCustomStencil`/
  `DisableCustomStencil` set `r.CustomDepth 3` (EnabledWithStencil), REFCOUNTED so the run + viz +
  manual tagging don't fight (G58).
- **Capture-flow integration** (UAnomalyCaptureSubsystem): tag the live fire's actor at `BeginFire`
  (`TagLiveFires`, base+index in GetLiveFires() order); untag at `BeginRevert`; `StartRun` enables
  custom stencil + safety untag-all; `FinishRun` untag-all + disable.
- **Gate viz (Option A, NOT in the output data path — owner LOCK-2):**
  `tools/create_stencil_viz_material.py` authors `/AnomalyInjector/Debug/M_AnomalyStencilViz` (a Post
  Process material painting custom-stencil==TagValue pixels green). `IAI.Capture.StencilViz <0|1> [tag]`
  adds it as an unbound post-process blendable + drives TagValue; `IAI.Capture.TagActor <name> [tag]`
  / `IAI.Capture.UntagAll` give a controlled manual eyeball.
- **No new module dep** (Engine-only tagging); Renderer still deferred; core untouched; `IAnomaly`
  untouched; catalog stays 8. Clean 5.1 Dev-Editor compile (core not rebuilt — Stage 2 is all in
  AnomalyCapture).

## Problems → resolutions (gate iterations)
1. **Author tool errored on a guessed enum.** `BlendableLocation.BL_SCENE_COLOR_AFTER_TONEMAPPING` does
   not exist. Verified against engine source: `BL_AFTER_TONEMAPPING` (BlendableInterface.h),
   `SceneTextureId.PPI_CUSTOM_STENCIL` / `PPI_POST_PROCESS_INPUT0` (MaterialSceneTextureId.h). Also
   replaced the If node (fragile `"A > B"` pins) with plain math nodes.
2. **Material authored but failed to COMPILE** (float4-vs-float3 on the Lerp). `PostProcessInput0.Color`
   is float4; the green is float3. Masked the scene color to RGB so both Lerp inputs are float3, output
   float3 → Emissive (float3); the stencil-match alpha chain stays scalar. (MaterialEditingLibrary does
   not expose the shader-error list to Python → type-checked the graph by construction.)

## Decisions banked for Stage 3 (owner-locked this turn)
- **LOCK-1 (hide-type box source):** render-type fires (object renders) → the STENCIL box; hide-type
  fires (`missing_object`, flicker while hidden) → the m7 PROJECTED box ("the hole"). Settled by the
  Stage-2 eyeball: a hidden object shows NO green (empty stencil) — correct. Detection: `missing_object`
  hides via `SetActorHiddenInGame(true)` → `IsVisible()==false`, so "is the target currently rendering"
  is a clean runtime signal (no IAnomaly change, no id table needed).
- **LOCK-2 (no viz in output):** the green StencilViz is a DORMANT debug toggle only. Stage 3 produces
  BOXES in labels.jsonl; the captured PNG stays the clean player frame.

## State / hand-off
- Owner eyeball GREEN: stencil silhouette tracks live targets + the animated skeletal Bot, clears on
  untag, auto-tags during runs, missing_object shows no green (correct).
- Committed on the branch as one atomic `feat(capture)` (no tag, no version bump), incl. the authored
  `M_AnomalyStencilViz.uasset`.
- **Next: Stage 3** — re-add Renderer + Renderer/Private + the stencil/depth Scene View Extension (the
  one deleted in Stage 1, brought back for the MASK pass): read CustomStencil/CustomDepth/SceneDepth →
  R8 tag-valued mask → async readback → per-tag pixel-AABB → write the occlusion-correct box (render-
  type) / projected box (hide-type) into the existing labels.jsonl schema. Two grab points (backbuffer
  color + SVE mask) joined by submit frame id; the ViewRect→CaptureRect alignment via bbox_norm is the
  headline risk to eyeball. Staged 3a (box) / 3b (alignment + gating + all-types).
