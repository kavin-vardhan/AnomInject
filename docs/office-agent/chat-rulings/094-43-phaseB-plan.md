# 094-43 — Phase B plan: camera_clipping, corrupted_texture (pink) and missing_texture (checkered) on 4.25, as in 5.1

**Date:** 2026-11-04, about 02:50 IST · **Author:** Claude Code (read-only: git objects, the installed 4.25 engine source, the Paris plugin's on-disk assets) · **A plan for chat, not a ruling.** File:line cites are `75ca580` (feat/europa-port) unless marked `d7eee97` (release/2026-10-05) or `4.25` (`E:\EpicGames\UE_4.25\Engine\Source\Runtime`).

## 0. Two facts first
- **Which 5.1.** The brief says "master / 53bf725". Both carry the *older* code: `master` (b5f15a3) is an ancestor of `release/2026-10-05`, which has 289 more lines in these six files (`090-09` installed-state + full revert + residual count; `086-02`/`084-07` camera_clipping slab rule v4 with triangle confirmation). The client's 5.1 is the release line, and **the port branch already carries the release code**, compiled out on 4.25 by `#if ANOMALY_ENGINE_51` (`Anomaly_CameraClipping.cpp:2-3`, `Anomaly_CorruptedTexture.cpp:2-3`, `Anomaly_MissingTexture.cpp:2-3`). **Recommended source: d7eee97.** (Decision D1.)
- **The 5.1 material assets cannot load on 4.25.** Measured header of the Paris copies of `M_CorruptedTexture_Pink.uasset` / `M_MissingTexture_Checker.uasset`: legacy version **−8**, UE4 522, UE5 1008 (4.25 writes −7 / 518). 4.25 zeroes the versions for legacy < −7 (`PackageFileSummary.cpp:87,95-100`) and the linker then fails the load (`LinkerLoad.cpp:1175-1178`). **Phase B needs two 4.25-authored materials.**

## 1. The three anomalies in 5.1 (what must be ported unchanged)
| | corrupted_texture / missing_texture | camera_clipping |
|---|---|---|
| Scope, args | Object, no args (`AnomalyInjectorSubsystem.cpp:220-227`); usage `<substring>` | Global (`:206-210`); `[near]` (default 100, 1–100000, `.h:51-53`) or `=<actor> [near]` (targeted, proximity-triggered) |
| Fires | auto pool (`AnomalyAutoInjectorSubsystem.cpp:39-50`); 5.1 default-enabled with blinking + lod_popping (`d7eee97:51-57`) | **never by the auto pool** (`:318` skips globals); held for the whole session from StartRun to FinishRun (`d7eee97 AnomalyCaptureSubsystem.cpp:5710-5807`) |
| Changes | every slot of every static/skinned mesh component on the target → shipped material (`Corrupted…cpp:100-143`, components via `AnomalyLod::ResolveLodComponents`) | `r.SetNearClipPlane <near>` (`CameraClipping.cpp:25`); targeted mode pushes/restores by poll distance ≤ trigger radius (`:120-145`) |
| Restores | m17/090-09: re-find component by name, restore original / default-reset / leave a game-replaced slot, sweep the owner, count residual (`:152-303`) | restore the pre-Apply near plane (`:218-244`) |
| Label | `FireWindow` + installed (`d7eee97 AnomalyActiveSource.h`); installed = a captured slot **renders ours now** (`:305-346`); type = subtype = id (`d7eee97 AnomalyCaptureSubsystem.cpp:652-664`) | `AnomalyState`: a frame is positive only when geometry lies in the view slab between the two planes, decided after that frame's camera update (bounds broad phase + complex-trace confirm, else `camera_clipping_unconfirmed`); whole-frame extent, no target, no mask (`:5882-6006`) |
| Mask | target's drawn silhouette | none |
| Config | shared selection keys only | `[AnomalyInjector] CameraClippingTriggerRadiusCm` (200 cm, 1–1e6, `AnomalyDefaults.h:121-123`, `.cpp:1141-1143`); console `IAI.Anomaly.CameraClipTriggerRadius <cm\|default>` (`.cpp:1567-1589`) |
| Row / summary keys | `mask_*`, `target_pixels` as other object ids | `camera_clipping.slab/.sphere_proxy/.slab_primitives/.eye_inside_box/.bounds_candidate/.clipped_ray_fraction/.confirm_*/.unconfirmed/.near_overridden` (`d7eee97 AnomalyLabelWriter.cpp:351-367`); `camera_clipping_*` run-summary counters (`:1373-1432`) |
| Tests / bench | `tools/install_state_selftest.cpp`; `create_anomaly_materials.py`, `set_material_usage_flags.py` | `tools/camera_clipping_slab_selftest.cpp`; bench `IAI.Bench.CameraSchedule cc_v1` (wall + props, 200 poses; `d7eee97 AnomalyBenchModule.cpp:265`) |

Shared helpers they use are **already built on 4.25**: `AnomalyLod::ResolveLodComponents` (`AnomalyLod.cpp:22-40`), `AnomalyViewport::FilterVisibleComponents`, `FindVisibleActorsMatching`, `GetActorPollDistanceCm`, `EvaluateNearClipSlab`, `IsGeometryWithinNearClipRadius` (`AnomalyViewport.h:108,123,180-185`), with the 4.25 compat for the camera cache, near plane, frustum and PhysX tri-meshes (`AnomalyEngineCompat.cpp`).

## 2. 4.25 API differences (checked in the 4.25 source)
- `UMeshComponent::OverrideMaterials` is `TArray<UMaterialInterface*>` (4.25 `Engine/Classes/Components/MeshComponent.h:28`): the two `OverrideMaterials[i].Get()` calls in each texture file (`:69`, `:285`) do not compile → one `AnomalyCompat::OverrideMaterial(Mesh, i)` helper. `UMaterialInstance::Parent` is a raw pointer (`MaterialInstance.h:309`), so `IsPinkDerived` compiles as is.
- `SetMaterial` → `MarkRenderStateDirty` (4.25 `MeshComponent.cpp`), as in 5.1: the swap shows on the frame after Apply. `ShouldRender()` exists (`SceneComponent.h:1045`).
- **Usage flags decide what renders in `-game`:** a missing flag logs "Default Material will be used in game" and draws grey (`Material.cpp:1597-1643`). The 4.25 materials need StaticLighting, SkeletalMesh, InstancedStaticMeshes, SplineMeshes, MorphTargets and Clothing; 4.25 has no Nanite flag, so the 5.1 `USAGE_FLAGS` tuple (`create_anomaly_materials.py`) needs that one entry dropped.
- No `GetCameraCacheView` (4.25 has `GetCameraCachePOV`, `PlayerCameraManager.h:313`): `Anomaly_CameraClipping.cpp:159` must use the existing `AnomalyCompat::CameraCachePOV`. 4.25 `FMinimalViewInfo` has no perspective near-clip override (only `OrthoNearClipPlane`, `CameraTypes.h:64`), so `HasNearClipOverride=false` is correct there. `r.SetNearClipPlane` exists and does `FlushRenderingCommands()` then sets `GNearClippingPlane` (min 1) (`UnrealEngine.cpp:14458-14468`).
- The 4.25 camera update (`LevelTick.cpp:1645`) runs before `OnWorldPostActorTick` (`:1694`), where the C5 snapshot is taken, so evaluating the slab there is "after the camera update" as 5.1 requires.

## 3. Masks from the private ID/depth pass (FZ2), not custom depth
- **Texture swaps:** the event mask is GD3 *visible coverage* of the target (private depth == scene depth). Nothing else is new: the targets are what GD5 covers (static, ISM/HISM, skeletal); anything else is already `unavailable` and ineligible. **The private pass must take the proxy fresh from the component every frame:** a swap recreates the proxy, so any per-proxy cache goes stale (and a masked → pink swap changes the silhouette).
- **camera_clipping:** no mask (`mask.provided=false`, as 5.1). The private pass inherits the pushed `GNearClippingPlane` through the view, so census masks stay consistent while it is held.

## 4. Files on `feat/europa-port`
1. `Source/AnomalyInjector/Private/Anomalies/Anomaly_{CorruptedTexture,MissingTexture,CameraClipping}.{h,cpp}`: drop the 5.1-only gates; use the `OverrideMaterial` and `CameraCachePOV` compat; no behaviour edits.
2. `Compat/AnomalyCapabilities.cpp:5`: add the three ids to `SupportsAnomaly`. `AnomalyEngineCompat.{h,cpp}`: `OverrideMaterial`.
3. `AnomalyInjectorSubsystem.cpp:46-54, 83-103, 285-293, 1227`: ungate the includes, `Register` and `GetCameraClippingFrameEvaluation`; give 4.25 its own CDO finders for the 4.25 material paths (D2).
4. **New 4.25 assets:** two materials authored by a py27 port of `create_anomaly_materials.py` (same graphs and constants, 4.25 usage flags, verified on readback), in a 4.25-only content folder (D2). The 5.1 `.uasset`s stay untouched.
5. **Capture, texture swaps:** no special case; `FAnomalyMasks425::Labelled` already resolves `FireWindow` + installed (`AnomalyMasks425.cpp:314-323`), and `Observe` keeps `Injected && Pixels>0` (`AnomalyCaptureLabels425.cpp:241`).
6. **Capture, camera_clipping** (`AnomalyCaptureLabels425.cpp`, `AnomalyMasks425.cpp`, `AnomalyAnnotation425`): hold enabled global routes from C4/C5 `Start` to `Stop` (5.1's `ApplySessionGlobals`/`RevertSessionGlobals`); add a whole-frame fire row when `IsCurrentlyAnomalous` (today `Labelled` returns false with no target, `AnomalyMasks425.cpp:317`, and `Observe` needs mask pixels); write the `camera_clipping.*` row keys and run-summary counters under the 5.1 names; event `camera_clipping@<start>|` with no nodes.
7. **Bench (4.25 only, `-IAIBench`):** a near-wall pose schedule on EA_Core, because 5.1's `cc_v1` lives in AnomalyBench, which is a stub on 4.25 (`AnomalyBench.Build.cs`: enabled only `Is51`). Also a skeletal target for the texture gates: EA_Core is all static basic shapes (`author_425_fixture.py`: EA_Floor, EA_HideCube, EA_BlinkCube, EA_LOD, EA_Light); 4.25 ships `/Engine/EngineMeshes/SkeletalCube` (D3).
8. **Kit and readers:** `capture_c5_kit.py:44,154` and the `ROUTES` tuples in `capture_c4_reader.py:8` / `capture_c5_reader.py` hard-code four routes; add the three, with per-route references (texture: target-region change; camera_clipping: same-seed baseline-near control).
9. Self-tests: run `install_state_selftest.cpp` and `camera_clipping_slab_selftest.cpp` unchanged (pure headers); plus a compile probe for the compat helper and a py27 harness for the authoring script.

## 5. Gates (all through the armed kit, `-nowrite` on fixtures, both RHIs where H4 runs)
- **G1 fires:** each anomaly ≥ 3 events on EA_Core. For camera_clipping, a session global is **one** event per session, so read "fires" as ≥ 3 separate slab-positive runs (D4).
- **G2 pixels:** every texture fire changes the target region against the frame before the swap; every labelled camera_clipping frame differs from a same-seed control at the baseline near plane, and every unlabelled frame matches it.
- **G3 labels = pixels:** texture masks IoU ≥ 0.98 against the colour-change silhouette, both edges; camera_clipping labelled frames == control-differing frames, no early or late frame.
- **G4 no leak:** outline-on fixture run vs outline-off, same seed: 0 new magenta. ⚠ The fixture outline is `float3(1,0,1)` (`author_outline425.py`) and the pink is `LinearColor(1,0,1)`: **only a paired, same-seed "new magenta" count works**; an absolute magenta count fires on every pink fire (G226's colour axis).
- **G5 state:** `polluted=0`, `r.CustomDepth` unchanged, 0 custom-depth components, residual 0 after every revert, near plane restored at Stop.
- **Can-fails:** a material with one usage flag removed (grey, so G2 must fail); a mutant labelling one frame early; a camera_clipping run at near = baseline (0 labelled).

## 6. Order of work
0. **Prerequisite:** 094-43 GREEN (FZ2 in, H1–H5). Phase B is blocked on it. ⚠ 094-43 stopped tonight at its entry gate (see the review).
1. 4.25 materials (D2) + readback verification.
2. missing_texture, then corrupted_texture (code, compat, registration, routes, kit).
3. Their gates on EA_Core and the outline fixture, then H4-style Paris regression on D3D11 and D3D12.
4. camera_clipping (session hold, whole-frame rows, bench pose), its gates, then Paris regression.
5. Office card update; default routes stay FZ4's until chat rules (D5).

## 7. Risks
- **R1 assets:** 5.1 `.uasset`s are unloadable (§0); two pak-able 4.25 assets must ship, and the 5.1 line must not see them.
- **R2 silent grey:** a missing usage flag renders the default material in `-game` with the label still "installed" (`Material.cpp:1643`). G2's can-fail is the guard.
- **R3 first-use shader compiles:** new materials compile on first launch (uncooked `-game`). C5 waits for shader jobs before starting (`AnomalyCaptureLabels425.cpp:395-400`), but a cold DDC changes the "0 Missing cached shader map on a warm launch" reading: run a warm-up launch first.
- **R4 the outline colour is the pink** (G4).
- **R5 exposure:** EA_Core frames have read saturated; the gray/white checker may barely change a gray cube. Measure in G2; fix the fixture exposure, not the anomaly.
- **R6 camera_clipping reach:** the C5 street view rarely comes within 100 cm of geometry, and a host camera with collision may never do so; positives need the bench pose. Each push/restore is a full render flush (`UnrealEngine.cpp:14465`), a hitch, and `GNearClippingPlane` is global (scene captures too).
- **R7 materials and instances:** swapping a `UMaterialInstanceDynamic` slot keeps the m17 rules (the game's MID is left alone once re-taken); ISM swaps change every instance (5.1 does the same).
- **R8 skeletal on EA_Core:** none exists; GD5's skeletal path is untested until a skeletal target is added (D3).

## 8. Decisions for chat
- **D1:** port the release (d7eee97) versions, not master/53bf725?
- **D2:** where the two 4.25 materials live (a 4.25-only folder, e.g. `Content/Engine425/Materials/`, with 4.25-only finders), and whether authoring them in the Paris plugin copy is allowed (today any Paris Content change is a STOP).
- **D3:** add a SkeletalCube target: to EA_Core (re-author) or to the outline fixture?
- **D4:** the camera_clipping "≥ 3 fires" wording (three slab-positive runs in one held session?).
- **D5:** stay opt-in routes, or match 5.1's default pool (blinking, missing_texture, corrupted_texture, lod_popping)?
