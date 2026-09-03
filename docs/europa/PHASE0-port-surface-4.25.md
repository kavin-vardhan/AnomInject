# EUROPA — PHASE 0: PORT-SURFACE AUDIT FOR UE 4.25

**2026-09-03, session 069 brief 23. READ-ONLY: no source was changed.** Target codename **EUROPA** — a
title on a **UE 4.25 (possibly modified)** engine, on the owner's office PC.

## 0. WHAT THIS DOCUMENT IS, AND ITS ONE HONEST LIMIT

It enumerates every engine dependency the plugin has, at `file:line`, and marks each for 4.25.

🚨 **THERE IS NO 4.25 TREE ON THIS BENCH.** Every 4.25 status below is either (a) read out of **our own
code**, which is certain, or (b) **my knowledge of the 4.x line**, which is not. Column 3 is graded:

| grade | meaning |
|---|---|
| **PRESENT** | I am confident this exists in 4.25 in the same form |
| **CHANGED** | exists but differs; the difference is named |
| **ABSENT** | I am confident it does not exist in 4.25 |
| **UNKNOWN** | I will not guess — the question that settles it is written out |

⛔ **NOTHING HERE IS A MEASUREMENT OF 4.25.** Treat every PRESENT/CHANGED/ABSENT as a *prediction to be
checked against the real tree*, exactly as `G120` requires: an unverified mechanism must never become a
scope decision. The `UNKNOWN` rows are the ones that decide the plan, and §D is the list that must be
answered before Phase 1 starts.

---

## 1. `AnomalyInjector` — the injector half

| dependency | file:line | 4.25 | replacement / question |
|---|---|---|---|
| `UTickableWorldSubsystem` base ×3 | `AnomalyInjectorSubsystem.h:12`, `AnomalyAutoInjectorSubsystem.h:35`, `AnomalySelectorSubsystem.h:12` | 🚨 **ABSENT** | `UTickableWorldSubsystem` arrived in **4.26**. 4.25 has `UWorldSubsystem` (added 4.24) but **not** the tickable variant. Replacement: `UWorldSubsystem` + hand-rolled `FTickableGameObject` (multiple inheritance), which is exactly what `UTickableWorldSubsystem` is. **~4 classes, mechanical.** |
| `FWorldDelegates::OnWorldTickEnd` | `AnomalyCaptureSubsystem.cpp:349,350,557,562` | ⚠ **UNKNOWN — HIGH RISK** | This is `m40`'s label anchor and `m44`'s record-birth point. **Q: does `FWorldDelegates::OnWorldTickEnd` exist in 4.25, and does it fire after every tickable?** If absent, the fallback is `FWorldDelegates::OnWorldPostActorTick` (which we already use elsewhere) — but that is a *different point in the frame* and would need `m40`'s alignment gate re-run to certify. |
| `FWorldDelegates::OnWorldPreActorTick` | `AnomalyInjectorSubsystem.cpp:166,177` | ⚠ **UNKNOWN** | Only used by the bench lever `SynthTickOrder`; if absent, the lever is dropped on 4.25 and the alignment gates run native-order only there. **Not ship-blocking.** |
| `FWorldDelegates::OnWorldPostActorTick` | `AnomalyViewport.cpp:991,996` | ⚠ **UNKNOWN** | Same question. |
| `FCoreDelegates::OnEndFrame` | `AnomalyCaptureSubsystem.cpp:348,552` | ✅ **PRESENT** | Long-standing. |
| `SetActorHiddenInGame`, `GetComponentsBoundingBox`, `LineTraceSingleByChannel` | `AnomalyViewport.cpp` (2 traces), targeting | ✅ **PRESENT** | Stable across 4.x. |
| **m45 hide flags**: `bRenderInMainPass`, `bRenderInDepthPass`, `SetCastShadow`, `bCastContactShadow`, `bAffectDynamicIndirectLighting`, `bAffectDistanceFieldLighting`, `bReceivesDecals` | `AnomalyHiddenClass.cpp:102-125` | ✅ **PRESENT** (likely) | All are long-standing `UPrimitiveComponent` properties. ⚠ **But the RENDERER BEHAVIOUR behind them is the real question** — see `bVisibleInRayTracing` below and §D. |
| `bVisibleInRayTracing` | `AnomalyHiddenClass.cpp:108,124` | ⚠ **UNKNOWN / likely ABSENT-or-guarded** | Ray tracing in 4.25 is behind `RHI_RAYTRACING`. **Q: does `UPrimitiveComponent::bVisibleInRayTracing` exist in 4.25?** If not, drop that one flag — the hide is unaffected on a non-RT title. |
| `SetRenderCustomDepth` / `SetCustomDepthStencilValue` | `AnomalyStencilTag`, probe spawn | ✅ **PRESENT** | Custom depth/stencil is 4.x-era. |
| `SetMaterial`, per-slot restore | `Anomaly_MissingTexture.cpp:123,205,216,236`, `Anomaly_CorruptedTexture.cpp` same | ✅ **PRESENT** | |
| `ULightComponent::SetCastShadows` | `Anomaly_LightingMismatch.cpp:81,107` | ✅ **PRESENT** | |
| Forced LOD (`AnomalyLod::SetForcedLod/GetForcedLod`, `GetNumLODs`) | `Anomaly_LodCorruption.cpp:62-84`, `Anomaly_LodPopping.cpp:253,268` | ⚠ **CHANGED (likely)** | `USkinnedMeshComponent::GetNumLODs()` and `ForcedLodModel` semantics shifted across 4.x/5.x. **Q: confirm both static and skinned accessors on 4.25.** Contained: the dispatch is already isolated in `AnomalyLod` (that helper was created for exactly this reason at `m3`). |
| `FAutoConsoleCommandWithWorldAndArgs` ×36, `IConsoleManager` ×5 | throughout | ✅ **PRESENT** | |
| `UDebugDrawService` ×4, `WasInputKeyJustPressed` ×8 | selector HUD/input | ✅ **PRESENT** | |
| `ConstructorHelpers::FObjectFinder` ×3 | `AnomalyInjectorSubsystem.cpp:38,42` | ✅ **PRESENT** | The `m8` cook guarantee for the two Content materials. |
| `TFunction`-based census provider contract | `AnomalyCensusProvider.h:32-34` | ✅ **PRESENT** | Pure C++; no engine surface. |
| `Foliage` module (`AInstancedFoliageActor`) | `AnomalyInjector.Build.cs` | ✅ **PRESENT** | Runtime module in 4.x. |
| `UPROPERTY` (only 2 uses), no `UFUNCTION` reflection tricks | | ✅ **PRESENT** | |
| **C++ level** — `if constexpr` **0**, `std::` **0**, structured bindings **0**, `FVector3f`/`FVector3d`/`UE::Math` **0**, `TStringBuilder` **0** | measured across all injector + capture `.cpp` | ✅ **NO RISK** | 🎯 **This is the single best piece of news in the audit.** The code is conservative C++ and does **not** use LWC types, so the 5.x double-precision `FVector` migration is a **non-issue** in both directions. |

**Injector-half verdict: SMALL.** One real structural change (`UTickableWorldSubsystem` → `UWorldSubsystem` + `FTickableGameObject`), one real unknown (`OnWorldTickEnd`), one contained unknown (LOD accessors), one droppable flag.

---

## 2. `AnomalyCapture` — the capture half

| dependency | file:line | 4.25 | replacement / question |
|---|---|---|---|
| `FSceneViewExtensionBase` + `FAutoRegister` | both SVEs, 4 sites | ✅ **PRESENT** | The SVE mechanism predates 4.25. |
| `IsActiveThisFrame_Internal(const FSceneViewExtensionContext&)` | `AnomalySceneViewExtension.h:25`, `AnomalyMaskSceneViewExtension.h:47` | 🚨 **CHANGED** | `FSceneViewExtensionContext` is **5.x**. 4.25 has `IsActiveThisFrame(const FViewport*)`. **Mechanical but it is an override-signature change on both SVEs.** |
| `SubscribeToPostProcessingPass(EPostProcessingPass, FAfterPassCallbackDelegateArray&, bool)` | `AnomalyMaskSceneViewExtension.cpp:91-98` (Tonemap), `AnomalySceneViewExtension.cpp` (VisualizeDepthOfField) | 🚨 **ABSENT** | **This is the single largest port item.** The post-processing-pass subscription API is **5.0+**. 4.25's SVE has `PrePostProcessPass_RenderThread` / `PostRenderBasePass_RenderThread` / `PostRenderViewFamily_RenderThread` with **`FRHICommandListImmediate`, not RDG**. ⇒ **both the colour grab point and the mask pass need a different hook on 4.25.** See §B. |
| `FRDGBuilder`, `FRDGTextureDesc::Create2D`, `AddEnqueueCopyPass` | `AnomalyMaskSceneViewExtension.cpp:144,186,196`, `AnomalySceneViewExtension.cpp:139` | ⚠ **CHANGED** | RDG **exists** in 4.25 but is far less complete and the SVE hooks do not hand you a builder. **Q: is RDG usable from an SVE hook on 4.25, or must the mask pass be written against `FRHICommandListImmediate` with manual RTs?** This decides whether the mask pass is a port or a rewrite. |
| `FScreenPassTexture`, `FPostProcessMaterialInputs`, `EPostProcessMaterialInput` | `AnomalyMaskSceneViewExtension.cpp:101-104` | 🚨 **ABSENT** | 5.x screen-pass plumbing. Replaced by whatever the 4.25 hook provides. |
| `AddDrawTexturePass` | `AnomalyMaskSceneViewExtension.cpp:37` | ⚠ **UNKNOWN** | Part of the same 5.x screen-pass family. |
| `FPixelShaderUtils::AddFullscreenPass` | `:160` | ⚠ **UNKNOWN** | **Q: does `FPixelShaderUtils` exist in 4.25?** If not, a manual fullscreen draw. |
| `FComputeShaderUtils::AddPass` / `GetGroupCount` | `:181-182` | ⚠ **UNKNOWN** | Same question; the `m34` GPU reduce depends on it. Fallback: the pre-`m34` **CPU reduce**, which still exists in-tree behind `IAI.Capture.MaskReduce cpu` — 🎯 **a ready-made 4.25 path we already ship and gate.** |
| 🚨 `#include "SceneRendering.h"` + `static_cast<const FViewInfo&>(View)` | `AnomalyMaskSceneViewExtension.cpp:22,36,153,154,216`; enabled by `PrivateIncludePaths.Add(GetModuleDirectory("Renderer")/Private)` at `AnomalyCapture.Build.cs:355` | 🚨 **HIGH RISK** | We reach into **renderer-private** headers for `FViewInfo::ViewRect` (`m46`'s internal rect) and `AddDrawTexturePass`. `G100` already flags this as breaking on an engine bump *inside our module*. On a **modified** 4.25 engine this is the most likely hard failure. **Q: does `FViewInfo` exist with a `ViewRect` member on the Europa tree, and does the module still link against Renderer/Private?** |
| `FRHIGPUTextureReadback` | `AnomalyFrameCapturer.cpp:317`, `AnomalyMaskSceneViewExtension.cpp:195`, `AnomalySceneViewExtension.cpp:147` | ⚠ **UNKNOWN** | **Q: does `FRHIGPUTextureReadback` exist in 4.25?** I believe the `FRHIGPU*Readback` family landed around 4.25–4.26 and I will not guess which. If absent: `RHIReadSurfaceData` (which the pre-`m19` sync path already used — again, a path we have shipped before). |
| `FRHIGPUBufferReadback` | `AnomalyMaskSceneViewExtension.cpp:184` | ⚠ **UNKNOWN** | Same; only needed for the GPU reduce, which has the CPU fallback. |
| `OnBackBufferReadyToPresent` | `AnomalyFrameCapturer.cpp:177,188,222` | ✅ **PRESENT** (likely) | The `FSlateRenderer` delegate is 4.x-era. 🎯 **The backbuffer capture path is therefore the most likely thing to work on 4.25 unchanged** — and it is already a shipped, gated alternative (`IAI.Capture.SVE 0`). |
| `IMPLEMENT_GLOBAL_SHADER`, `SHADER_USE_PARAMETER_STRUCT`, `BEGIN_SHADER_PARAMETER_STRUCT` | `AnomalyVisibleMaskShader.h:15`, `AnomalyMaskReduceShader.h:14` | ✅ **PRESENT** | Shader parameter structs exist in 4.25. |
| `FSceneTextureShaderParameters` + `.usf` `#include "/Engine/Private/SceneTexturesCommon.ush"`, `CalcSceneCustomStencil` | `AnomalyVisibleMask.usf:13,31` | ⚠ **UNKNOWN — likely CHANGED** | The unified scene-texture uniform buffer is a 5.x consolidation. **Q: how does a 4.25 shader read CustomStencil/CustomDepth/SceneDepth?** This is the mask shader's core and may need rewriting even if the C++ ports. |
| `FMaterialShaderMap::UsesSceneTexture(PPI_*)` (the host-PP preflight) | `AnomalyCaptureSubsystem.cpp:1667-1686`, `5346-5362` | ⚠ **UNKNOWN** | **Q: does `FMaterialShaderMap::UsesSceneTexture` exist in 4.25, and are `PPI_CustomDepth`/`PPI_CustomStencil` the same enum values?** `m41`'s preflight is diagnostic-only — **droppable on 4.25 without losing product behaviour**, at the cost of that warning. |
| `GetCachedPostProcessBlends`, `GetCameraCacheView` | `AnomalyCaptureSubsystem.cpp:1721,1732` | ⚠ **UNKNOWN** | Same preflight; same droppable status. |
| `ENQUEUE_RENDER_COMMAND` ×4, `FlushRenderingCommands` ×8 | throughout | ✅ **PRESENT** | |
| `FScopeLock` ×53, `FThreadSafeCounter` ×25, `ESPMode::ThreadSafe` ×31, `Async()` | throughout | ✅ **PRESENT** | |
| `IImageWrapper` / `IImageWrapperModule` (PNG, gray-8) | label/preview writers | ✅ **PRESENT** | ⚠ **Q: does 4.25's PNG wrapper accept `ERGBFormat::Gray` at 8 bits?** That is the `m43` mask format. |
| `FJsonObject` / `TJsonWriter` | label writer | ✅ **PRESENT** | |
| `FOutputDevice` (the `m38` run log) | `AnomalyRunLog` | ✅ **PRESENT** | |

**Capture-half verdict: LARGE**, and concentrated in exactly three things: the **SVE post-process hook**, **RDG availability**, and **renderer-private `FViewInfo`**.

---

## 3. `AnomalyShaders`

| dependency | file:line | 4.25 | note |
|---|---|---|---|
| `AddShaderSourceDirectoryMapping("/Plugin/AnomalyInjector", …)` | `AnomalyShadersModule.cpp:18` | ✅ **PRESENT** | Virtual shader paths work in 4.25. |
| `LoadingPhase: PostConfigInit` for a shader-declaring module | `.uplugin` | ✅ **PRESENT** | The `m26` `G129` lesson (a global shader must load before engine init) applies identically. |
| `IMPLEMENT_MODULE` | `:28` | ✅ **PRESENT** | |
| Module deps `Core/Engine/RenderCore/RHI/Projects` | `AnomalyShaders.Build.cs` | ✅ **PRESENT** | |

**Small.** ⚠ But a shader change still needs a **full cook** on 4.25 (`G129` is engine-independent).

---

## 4. `AnomalyControlServer`

| dependency | file:line | 4.25 | note |
|---|---|---|---|
| `WebSocketNetworking` plugin | `.uplugin` `Plugins[]` | ⚠ **UNKNOWN** | **Q: is the `WebSocketNetworking` plugin present and enabled in the Europa engine?** It is an engine plugin in 4.25 but not always shipped/enabled in a modified fork. |
| `IWebSocketNetworkingModule::CreateServer`, `IWebSocketServer`, `INetworkingWebSocket`, `WebSocketNetworkingDelegates.h` | `AnomalyControlServerSubsystem.cpp:24-27,121-131,255,269` | ⚠ **UNKNOWN** | The API is old and stable; `G83` (5.1's `INetworkingWebSocket` has no `Close`) suggests it barely changed. |
| `Json` module, the dashboard protocol | | ✅ **PRESENT** | The protocol is our own JSON over WS — **engine-independent**. |

**Small, and it is the least important half:** the control server is `ANOMALY_CONTROL_SERVER`-gated and compiled out of Shipping already. **A 4.25 bring-up can ship with it disabled** and drive capture from the console (`IAI.Capture.Start`), which the client docs already describe as the two-monitor workflow.

---

## 5. Build system / `.uplugin`

| item | 4.25 | note |
|---|---|---|
| `PCHUsage = UseExplicitOrSharedPCHs` (all 4 modules) | ✅ **PRESENT** | |
| Module `Type: Runtime`, `LoadingPhase: Default`/`PostConfigInit` | ✅ **PRESENT** | |
| No `CppStandard` set anywhere | ✅ **NO RISK** | We never opt into C++17/20, and we use no feature that needs it (measured, §1). |
| No `bUseUnity` override | ✅ | |
| `PrivateIncludePaths` → `GetModuleDirectory("Renderer")/Private` | ⚠ **CHANGED-or-RISK** | `GetModuleDirectory` exists in 4.25; whether the Renderer private layout matches is §2's `FViewInfo` question. |
| `AnomalyCapture.Build.cs` **tick-pin fork probe** — scans the engine tree for `sUseFixedGameTickWithVariableRenderTick_Net`, walks `Source/Runtime/Core`, `Source/Editor`, `Plugins` | ⚠ **WORKS BUT RE-CHECK** | It is a *content* probe with route A/B/C and a cap; it degrades gracefully to "not found". ⚠ **On a modified 4.25 fork it may fire or mis-fire — read its build-log echo on the first Europa build** (the runbook already requires this). |
| `.uplugin` `EngineVersion` | ⚠ | Not currently set. A 4.25 build may want a separate `.uplugin` or no version pin. |

---

## A. HONEST SIZE ESTIMATE

⚠ **Estimates, not measurements — no 4.25 tree was compiled.** Ranges reflect the UNKNOWNs.

| half | scope | estimate |
|---|---|---|
| **INJECTOR HALF** — 9 anomalies, selector, auto-injector, viewport/visibility, targeting, LOD, census provider, hidden-class registry | subsystem base swap ×4, LOD accessor check, one droppable RT flag, delegate questions | **SMALL — 1–2 days** if `OnWorldTickEnd` exists; **+2–3 days** if it does not (the label anchor must be re-established and `m40`'s gate re-run) |
| **CAPTURE HALF** — SVE capture, mask SVE, GPU reduce, census, `m26`, label writer, async writer, run log, letterbox, preview tee, stencil tag, key ring | SVE hook rewrite, RDG-or-RHI decision, `FViewInfo` access, scene-texture shader rewrite, readback API | **LARGE — 1.5–4 weeks**, dominated by three unknowns; the spread is honest, not padding |

🎯 **The asymmetry is the point: the injector half is nearly portable as-is, and it is the half that
produces anomalies.** A 4.25 build that injects but captures via the **backbuffer** path (already
shipped, already gated) is a plausible early deliverable.

---

## B. ARCHITECTURE FOR ONE CODEBASE — the brief's proposal, evaluated

**The proposal:** engine-version shims for the injector half; a small backend interface for the
render-touching parts of capture (capture hook, mask pass, reduce, readback) with a 5.1 backend and a
4.25 backend; everything else shared.

✅ **I agree with the shape, and the seam is already where it needs to be.** `m45` proved the plugin
survives a change of hide mechanism because `AnomalyHiddenClass` owns it behind one interface; the same
move applies here. The existing module split already isolates render code in `AnomalyCapture` +
`AnomalyShaders`, and `ANOMALY_CAPTURE=0` in Shipping proves the rest compiles without it.

**What I would do differently — four things:**

1. 🚨 **Do NOT put the backend seam at "capture hook / mask pass / reduce / readback" as four
   interfaces. Put it at ONE: `IAnomalyMaskBackend` + `IAnomalyFrameGrabBackend`.** Reduce and readback
   are *implementation details of the mask backend* — the CPU reduce already exists behind
   `IAI.Capture.MaskReduce cpu` and is not a separate axis. Four seams means four places for the two
   engines to drift; two seams means the 4.25 backend can use RHI-only code end-to-end without ever
   touching RDG types.
2. 🚨 **The seam must be drawn ABOVE `FViewInfo`, not below it.** The 5.1 backend may reach into
   renderer-private headers; the *shared* code must never see `FViewInfo`. Today
   `AnomalyMaskSceneViewExtension.cpp` mixes both. Concretely: the backend returns a plain
   `struct FAnomalyMaskFrame { FIntRect InternalRect; FIntRect OutputRect; TArray<uint8> Pixels;
   TMap<uint8,int32> Counts; }` and nothing above it knows how those were obtained.
3. ⚠ **Version shims by `#if ENGINE_MAJOR_VERSION`/`ENGINE_MINOR_VERSION`, but ONLY in the injector
   half and ONLY at declaration sites** (the subsystem base, the LOD accessors, the one RT flag).
   ⛔ **Never `#if` inside a gate-bearing algorithm** — a preprocessor branch inside `ArmTargetMaskOwn`
   or the census would mean the two engines are running different logic behind one green gate table,
   which is `G224`'s shape.
4. 📌 **Keep the artifact contract engine-independent and gate it as such.** `labels.jsonl`,
   `annotation.json`, `mask_state`, `mask_map.json` must be **byte-comparable across engines** for the
   same fixture. That is the strongest available proof the port did not change meaning, and it costs
   nothing to declare now.

⚠ **The risk this shape does not remove:** if 4.25 cannot produce an occlusion-correct custom-depth
mask at all, the 4.25 backend cannot be a backend — it would have to return `unmeasured` for every
frame, and `m26`'s veto, the census and the target mask all degrade together. **That is a product
question, not an architecture one** (§D, and NEEDS-DECISION 1).

---

## C. PHASE 1 — the walking skeleton

**Goal: `AnomalyInjector` alone compiles and runs on Europa, and ONE anomaly fires.** Capture,
mask, census and control server are **out of scope** and disabled.

**Steps**
1. New target/branch; `.uplugin` reduced to `AnomalyInjector` only (drop `AnomalyShaders`,
   `AnomalyCapture`, `AnomalyControlServer` and the `WebSocketNetworking` dependency).
2. Subsystem base swap: `UTickableWorldSubsystem` → `UWorldSubsystem` + `FTickableGameObject` (×3, ×4
   with capture later).
3. Resolve the three delegate questions (§D-1) and pick the tick anchor.
4. LOD accessors behind `AnomalyLod` (already isolated) — fix or stub.
5. Drop `bVisibleInRayTracing` if absent.

**GATES — pre-declared here so Phase 1 cannot be graded after the fact**

| gate | predicate |
|---|---|
| **E1-G1** | `AnomalyInjector` compiles on Europa, editor **and** game target, both exit 0 (the `G221` both-targets rule is engine-independent) |
| **E1-G2** | `IAI.ListAnomalies` prints **9** ids, sorted — the catalog is intact |
| **E1-G3** | `IAI.Apply missing_object <actor>` hides it and `IAI.Revert` restores it, **verified by eye**, and `IAI.DumpActive` reads 0 after |
| **E1-G4** | 🚨 **PROVE-IT-CAN-FAIL:** `IAI.Apply` on a **non-existent** actor name reports the AMB-2 zero-match refusal and changes nothing. *A bring-up where everything "works" is the shape that hides a no-op.* |
| **E1-G5** | The auto-injector fires once (`IAI.Auto.FireOnce`) and auto-reverts, with the seeded draw reproducible across two runs at one seed |
| **E1-G6** | ⛔ **NO artifact is produced and none is claimed.** Phase 1 ships no labels; any label-shaped output at this stage would be unvalidated by construction |

⚠ **Explicitly NOT in Phase 1:** any pixel gate, any mask, any identity arbiter. Those need the capture
half and the engine questions answered.

---

## D. QUESTIONS THAT MUST BE ANSWERED FROM THE REAL EUROPA TREE BEFORE PHASE 1

**D-1 — engine surface (settles the injector half)**
1. Does `FWorldDelegates::OnWorldTickEnd` exist, and does it fire after every tickable? *(the `m40`/`m44` anchor)*
2. Do `OnWorldPreActorTick` / `OnWorldPostActorTick` exist?
3. Is `UTickableWorldSubsystem` present, or must we compose `UWorldSubsystem` + `FTickableGameObject`?
4. `USkinnedMeshComponent::GetNumLODs()` and forced-LOD accessors — present, renamed, or moved?
5. `UPrimitiveComponent::bVisibleInRayTracing` — present?

**D-2 — renderer (settles whether the capture half is a port or a rewrite)**
6. 🚨 **Is the renderer MODIFIED?** If so, where — and does `Source/Runtime/Renderer/Private/SceneRendering.h` still define `FViewInfo` with a `ViewRect` member?
7. **Forward or deferred?** A forward renderer changes what custom depth even means for the mask.
8. **Does the game already use custom depth/stencil, and for what?** 🚨 **This is the highest-value question in the list:** the plugin reserves stencil values `200–254` and `m26`'s collision detectors assume nothing else writes there. A host already using custom stencil breaks that assumption on day one.
9. Which SVE hooks exist? Specifically: `PrePostProcessPass_RenderThread`, `PostRenderBasePass_RenderThread`, and is any of them handed an `FRDGBuilder`?
10. Do `FRHIGPUTextureReadback` / `FRHIGPUBufferReadback` exist?
11. How does a 4.25 `.usf` read CustomStencil / CustomDepth / SceneDepth? *(`SceneTexturesCommon.ush` + `CalcSceneCustomStencil` may not apply)*
12. `FPixelShaderUtils` / `FComputeShaderUtils` — present?
13. `FMaterialShaderMap::UsesSceneTexture` and the `PPI_*` enum — present, same values?

**D-3 — runtime configuration (settles which gates are even applicable)**
14. **Screen percentage / dynamic resolution** in use? *(`m46` exists precisely for this; if Europa runs dynamic res, the mapping is mandatory from day one, not an optimisation)*
15. **TAA method** — and is it temporally accumulating? *(decides whether `G228`'s ~9 % cross-run floor applies there too, and therefore whether an AA-off identity arbiter is available)*
16. Nanite: **obviously absent on 4.25** ⇒ 🎯 **`G134`'s Nanite limitation does not exist on Europa.** Every target is potentially maskable — a genuine *improvement* over the 5.1 host.
17. Is the game's tick decoupled (the `ANOMINJECT_FW_TICKPIN` fork shape)? Read the build-log probe echo.
18. `WebSocketNetworking` plugin present and enabled?

---

## E. THE TWO-ENGINE GATE MATRIX, AND HOSTING 4.25 ON THE BENCH

🚨 **THE GATE MATRIX DOUBLES, AND THAT IS THE REAL ONGOING COST — not the port itself.** Every
per-frame alignment gate already runs in **two tick orders**; adding a second engine makes it
**2 orders × 2 engines = 4 legs per gate**, and the identity arbiter adds its control and can-fail leg
on each. A milestone that costs 6 legs today costs ~12.

**Mitigations worth deciding on now, not later:**
- **Engine-independent artifact comparison** (§B-4) is the cheapest cross-engine gate: one fixture, two
  engines, compare `labels.jsonl` and `mask_map.json`. It catches meaning-drift without doubling the
  pixel gates.
- **Not every gate needs both engines.** Pixel arbiters are engine-specific by nature (`G230`'s scope
  rule already says arbiters are narrower than alignment gates). Proposed: **alignment gates on both
  engines; pixel arbiters on the engine being changed.**

**Hosting a 4.25 engine here:**
- **Disk:** currently **188 GB free on `D:`, 381 GB on `E:`**. A source-built 4.25 engine plus DDC is
  ~150–250 GB. It fits, but only on `E:` — and `E:` already holds every archive behind the junctions.
- ⛔ **Separate install, separate `Intermediate`, separate DDC. Nothing shared.** `Intermediate\**\*.dep.json` stores absolute paths, and a shared DDC across engine versions is a corruption risk.
- ⚠ **The `.uproject` GUID → engine mapping lives in `HKCU\Software\Epic Games\Unreal Engine\Builds`** — a second engine means a second registry entry and a second host project. **That registry key is in no repo and no backup** (already recorded in the runbook's `E:`-move scope note).
- ⚠ **`CB_GateLevel` cannot be shared.** It is a cooked 5.1 asset; Europa needs its own gate fixture,
  authored by `make_gate_level.py` **against 4.25** — and `G99`'s freeze applies separately to each.

---

## F. TOOLING THAT ASSUMES 5.x

| tool | assumption | status |
|---|---|---|
| `CaptureBench` plugin | `CAPTURE_BENCH` gated, 5.1 marker/stall cvars | ⚠ Needs its own 4.25 port; it is bench-only and **not** on the delivery path |
| `run_leg.ps1` | UE 5.1 packaged layout, `-ExecCmds`, engine paths | ⚠ Parameterise the engine root; otherwise engine-agnostic |
| `m44_gates.py`, `m44_pairing_probe.py`, `m45_iou.py`, `m45_identity.py` | 🎯 **read only `labels.jsonl`, `annotation.json`, `run_summary.json`, `target_mask/*.png`** | ✅ **ENGINE-INDEPENDENT — they port for free**, and that is a direct consequence of the artifact contract. **They become the cross-engine gate.** |
| `verify_capture.py`, overlay tooling | same artifact contract | ✅ Engine-independent |
| `verify_cooked_maps.ps1` | IoStore `.utoc` container index | ⚠ **UNKNOWN** — 4.25 supports IoStore but a fork may pak-only. **Q: does Europa cook to `.utoc`/`.ucas` or to `.pak` only?** |

---

## G. NEEDS-DECISION

1. 🚨 **What is the 4.25 deliverable?** The injector half is ~2 days; the capture half is 1.5–4 weeks
   and is gated on renderer questions that could make an occlusion-correct mask impossible there. Three
   coherent products: **(a) injection only** — anomalies fire on Europa, the client captures by their
   own means; **(b) injection + backbuffer capture + labels, no masks** — uses only paths we have
   already shipped and gated, and needs almost none of §D-2; **(c) full parity incl. masks** — needs
   every §D-2 answer first. ⛔ **I would not commit to (c) before D-2 is answered**, and (b) is a real
   product that (c) can be built on top of.
2. ⚠ **Does the Europa title already use custom depth/stencil?** (§D-2 q8.) If it does, the
   `200–254` reservation is not safe there and the tag scheme needs a design change **before** any
   capture work. This is the one question that could invalidate the architecture rather than just size
   it.
3. ⚠ **Does the bench host the 4.25 engine, or does Europa work happen only on the office PC?** Hosting
   it here costs ~150–250 GB on `E:` and a second registry mapping, and it is the difference between
   gates I can run and gates only the owner can run.
