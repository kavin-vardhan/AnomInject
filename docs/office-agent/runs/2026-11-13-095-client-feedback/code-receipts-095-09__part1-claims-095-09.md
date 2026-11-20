# 095-09 Part 1 - claim verification (read-only, 2026-11-14)

Plugin source: `master` = `ebc34ed` (the delivered release line). The four LOD files
(`Anomaly_LodCorruption.cpp/.h`, `Anomaly_LodPopping.cpp/.h`, `AnomalyLod.cpp/.h`) are byte-identical on
`integ/m56-m58` (`git diff --stat master 9bf6c7e` on them is empty).
Engine: `D:\UESource\UnrealEngine`, Build.version 5.1.1. `UE` = `Engine\Source`.

## Claim 1 - a hierarchical-instanced component ignores a forced LOD: CONFIRMED

- `UHierarchicalInstancedStaticMeshComponent : UInstancedStaticMeshComponent : UStaticMeshComponent`
  (`UE/Runtime/Engine/Classes/Components/HierarchicalInstancedStaticMeshComponent.h:136`,
  `InstancedStaticMeshComponent.h:87`); `UFoliageInstancedStaticMeshComponent` derives from HISM
  (`UE/Runtime/Foliage/Public/FoliageInstancedStaticMeshComponent.h:20`). None overrides `SetForcedLodModel`.
- `UStaticMeshComponent::SetForcedLodModel` stores the value and `MarkRenderStateDirty()`
  (`UE/Runtime/Engine/Private/Components/StaticMeshComponent.cpp:2270-2277`): the proxy is rebuilt.
- A non-Nanite HISM always gets `FHierarchicalStaticMeshSceneProxy` (`HierarchicalInstancedStaticMesh.cpp:3179-3191`);
  a Nanite one gets a Nanite proxy (no discrete LODs either).
- That proxy turns static relevance OFF and dynamic relevance ON (`HierarchicalInstancedStaticMesh.cpp:936-938`), and its
  `DrawStaticElements` emits batches only for runtime-virtual-texture passes (`:949-957`). So the base
  `FStaticMeshSceneProxy::DrawStaticElements` forced-LOD branch (`StaticMeshRender.cpp:1165-1167`) never draws the main pass.
- Its own LOD pick, in `GetDynamicMeshElements` (`:1506`), starts from `ClampedMinLOD`, then the debug cvars
  `foliage.MinLOD` / `foliage.ForceLOD` (`:1748-1762`), then the per-cluster `CalcLOD` traversal (`:1147-1230`).
  `ForcedLodModel` / `ForcedLOD` has **zero references** in `HierarchicalInstancedStaticMesh.cpp` (grep).
  Only the `bRenderFirstInstanceOnly` path (`:1508-1511`, not a game-view path) falls back to the ISM code.
- **Plain ISM honours it, for the whole component:** `FInstancedStaticMeshSceneProxy` does not override
  `DrawStaticElements` (static path = `StaticMeshRender.cpp:1165-1167`, one forced LOD for every instance) and its dynamic
  path calls `GetLOD(View)` (`InstancedStaticMesh.cpp:1327`) = `FStaticMeshSceneProxy::GetLOD`, which returns the forced LOD
  (`StaticMeshRender.cpp:2320-2323`); `GetLODMask` likewise (`:2359-2361`). A multi-LOD ISM therefore changes visibly.

## Claim 2 - lod_popping can select a HISM outside foliage and labels it: CONFIRMED

- Selection filter `AnomalyViewport::IsRenderableComponent` (`master:Source/AnomalyInjector/Private/AnomalyViewport.cpp:794-823`)
  excludes only components owned by `AInstancedFoliageActor` (`:801-807`), excluded-name patterns (`:809`), and an ISM/HISM
  with zero instances (`:814-820`); a HISM passes `IsRenderableGeometryComponent` as a `UStaticMeshComponent` (`:825-829`).
  So an actor whose visible component is a non-foliage HISM with instances (level-placed, blueprint or packed-level
  actor) is an auto-pool candidate. Runtime non-Nanite landscape grass is not: it reports zero instances while it draws
  (G574, on integ), so the zero-instance guard drops it; a targeted fire on a landscape proxy still reaches its grass.
- Apply resolves `=ActorName` to every `UStaticMeshComponent` and `USkinnedMeshComponent` on the actor
  (`AnomalyLod.cpp:11-30`, `AnomalyTargeting.h` `FindComponentsMatching`: `GetComponents<T>`, no visibility filter), HISM
  included. Per component it checks only (`Anomaly_LodPopping.cpp:114-197`): runtime LOD count >= 2 (a multi-LOD HISM mesh
  passes), then auto-pool only: actor poll distance, actor bounds coverage, predicted current LOD from the component's
  combined bounds. There is no instancing check. A targeted fire (`AnomalyAutoInjectorSubsystem.cpp:700-840`, no pool check)
  skips everything but the LOD count, so a named foliage actor is reachable too.
- `Tick` writes `ForcedLodModel` popped/baseline every half-period (`Anomaly_LodPopping.cpp:238-263`); on a HISM nothing
  changes on screen.
- Label: `lod_popping` is `AnomalyState` (`AnomalyActiveSource.h:26`), membership = the active bit alone
  (`AnomalyLabelSync.h:72-74`), active = `bActive && bPoppedPhase` (`Anomaly_LodPopping.h:20`). **Every popped frame is a
  positive label with no visible change** when every qualifying component is a HISM. If the actor also holds a plain
  multi-LOD static mesh, that one pops and the label is true.
- m26 veto deletes only `MEASURED_ZERO` (`AnomalyCaptureSubsystem.cpp:8749`, `:8980-8986`); a HISM draws pixels, so the
  event is kept. The census measures drawn coverage, which the HISM has.

## Claim 3 - lod_corruption: CONFIRMED (single-LOD and HISM), with the reach narrower than "auto pool"

- **Auto pool: not reachable on master.** `lod_corruption` is not in `GAutoPool` (`AnomalyAutoInjectorSubsystem.cpp:35-46`)
  nor the selector's `GAnomalyChoices` (`AnomalySelectorSubsystem.cpp:27-34`).
- **Targeted fire: reachable.** `TryFireSpecific` (`AnomalyAutoInjectorSubsystem.cpp:700-840`) has no pool check: a
  targeted capture (`IAI.Capture.Start ... lod_corruption <actor>`, dashboard `capture_start {anomaly,target}`) applies
  `=Actor` plus any forwarded args. Nanite targets are refused there by default (`:757`). `IAI.Apply` reaches Apply too but
  creates no live fire, so it writes no label.
- **Apply has no guard at all** (`Anomaly_LodCorruption.cpp:49-68`): every resolved static/skinned component is captured
  and forced. `GetWorstLod` = `max(NumLODs, 1)` (`AnomalyLod.cpp:32-44`); `ResolveTargetLod` returns it (`:143-151`).
  - single-LOD static or skeletal mesh: forced 1 = LOD index 0 = itself: no visible change.
  - HISM (incl. foliage HISM): forced value ignored (claim 1): no visible change.
  - plain ISM, multi-LOD: whole component forced to the coarsest LOD: visible change (no defect). Single-LOD ISM: as above.
- Label: `lod_corruption` is `FireWindow` (`AnomalyActiveSource.h:32`): member = on screen AND installed
  (`AnomalyLabelSync.h:72-74`); installed = `IsVisualConditionHeld` default = `IsActive` (`IAnomaly.h:44-46`) = `bActive`.
  **Every on-screen frame of the hold is a positive label with no visible change.** Mask veto: pixels are drawn, so kept.
- 4.25 for comparison (`feat/europa-port` git objects, read only): `Anomaly_LodCorruption.cpp:60` skips a single-LOD mesh
  **in auto-pool selection only**; `:64-70` the 4.25 predictor (`AnomalyLod.cpp:165-203`, `ComputeStaticMeshLOD` on the
  game-thread view) picks a forced value that differs from the drawn LOD, auto pool only and only without an explicit
  index. No instancing check there either; a targeted 4.25 fire keeps both gaps. Not changed by this brief.

## Claim 4 - skeletal: which frame's render shows a plugin-set forced LOD: CONFIRMED (one frame late, order-dependent)

- `USkinnedMeshComponent::SetForcedLOD` stores the value and notifies streaming only; no render-state or dynamic-data
  dirty (`SkinnedMeshComponent.cpp:3393-3403`).
- The value is consumed by `UpdateLODStatus_Internal` (`:3543-3546`), called from the component's `TickComponent`
  (`:1186`), tick group `TG_PrePhysics` (`SkeletalMeshComponent.cpp:208`). `SendRenderDynamicData_Concurrent` sends
  `GetPredictedLODLevel()` (`SkinnedMeshComponent.cpp:969-995`).
- World tick order (`LevelTick.cpp`): `OnWorldPreActorTick` `:1468` -> `TG_PrePhysics` `:1537` -> tickables `:1606` ->
  `OnWorldTickEnd` `:1814` (label sample, m40).
- Native order: anomaly `Tick` runs from the injector tickable (`AnomalyInjectorSubsystem.cpp:491-494`), after the
  component consumed the old value -> **render N+1**. `IAI.Bench.SynthTickOrder`: dispatch at `OnWorldPreActorTick`
  (`:538-545`), before the component tick -> **render N**. Apply and Revert run from the capture tickable
  (`BeginFire`/`BeginRevert`) in both orders -> **render N+1**.
- Labels per anomaly (both sampled at tick end of frame N):
  - `lod_corruption`, both orders: the frame armed on the `BeginFire` tick is captured and labelled
    (`master:AnomalyCaptureSubsystem.cpp:1133-1135`, LeadIn captures then fires in the same tick; m18 finalises its label
    after the fire) while its picture still shows the old LOD: **one false positive per onset**; the `BeginRevert` tick frame
    (`:1172-1175`) is unlabelled while still showing the forced LOD: **one false negative per offset**. The settle ticks
    (uncaptured on master, flagged on integ) come after these edge frames, so they do not cover them.
  - `lod_popping`: native order, every pop and unpop edge: the label leads the picture by one frame; synth order: aligned.
    Revert (capture tick, both orders) while popped: one false negative.
- Derived from source, never measured. The bench's only skeletal meshes are SKM_Bot (single-LOD, G20), SKM_Backpack (LOD
  count not established) and the m53 fixture `SK_TC_Cube`.

## Not in scope, noted
- Nanite: forced LOD does nothing on a Nanite proxy [R]; targeted/auto fires refuse Nanite by default, and with
  `IAI.Targets.AllowNanite 1` Nanite frames are not annotation members (`AnomalyLabelSync.h:96-98`).
- `lod_corruption` on a multi-LOD mesh already showing its coarsest LOD (far away), or with an explicit `lod-index 1`
  (forces LOD 0) on a near object, also changes nothing. Graded, not structural; not refused here.
