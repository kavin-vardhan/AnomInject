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
| 🆕 **✅ MEASURED (stock 4.25)** | **added 2026-09-03, session 070.** Read out of the real stock 4.25.4 tree at `E:\EpicGames\UE_4.25`, with an engine `file:line`. **A fact about STOCK 4.25 — and still only a PREDICTION for Europa.** |

⛔ **NOTHING IN THE ORIGINAL GRADES IS A MEASUREMENT OF 4.25.** Treat every unmarked PRESENT/CHANGED/ABSENT
as a *prediction to be checked against the real tree*, exactly as `G120` requires: an unverified mechanism
must never become a scope decision. The `UNKNOWN` rows are the ones that decide the plan, and §D is the
list that must be answered before Phase 1 starts.

> 🆕🆕 **2026-09-03, SESSION 070 — A STOCK UE 4.25.4 TREE NOW EXISTS ON THE BENCH, AND MANY ROWS BELOW ARE
> NOW MEASURED. Rows carrying `✅ MEASURED (stock 4.25)` were read from `E:\EpicGames\UE_4.25`; the
> original prediction is KEPT BESIDE the measurement so the audit can be scored. Full working:
> `docs/europa/PHASE1-bench-audit-and-plan.md`.**
>
> 🚨 **THE DISTINCTION THAT GOVERNS EVERY SUCH ROW: the bench host `StylizedParisStreet` on a stock 4.25
> is a BENCH FIXTURE, NOT EUROPA.** Europa is a **possibly-modified** 4.25 on the office PC and remains
> entirely unmeasured. A stock-tree fact is excellent evidence about *the 4.x engine surface* and **no
> evidence at all** about *a fork's modifications*. ⛔ **Nothing below says "we measured Europa".**
>
> 📊 **SCORE: 5 rows got BETTER, 3 got WORSE, 1 new item was found that this document never enumerated.**
> · 🔻 **WORSE:** `OnWorldTickEnd` is **ABSENT** (the highest-risk row, and it went the bad way) ·
> `PrePostProcessPass_RenderThread` is **ABSENT** (this document asserted 4.25 has it) ·
> `bCastContactShadow` is **ABSENT** (this document graded the m45 flag set "PRESENT (likely)").
> · 🎯 **BETTER:** `FRDGBuilder(FRHICommandListImmediate&)` + `Execute()` exist, so **the mask pass is a
> PORT, not a rewrite** · `FPixelShaderUtils`/`FComputeShaderUtils`/`FRHIGPU*Readback` all present in
> **public** headers · the `PPI_*` enum is **line-for-line identical** to 5.1 · the LOD accessors are
> **unchanged** (zero work) · `bVisibleInRayTracing` is **present and unguarded**.
> · 🆕 **NEW ITEM:** 4.25's `UWorldSubsystem` has **no `DoesSupportWorldType`** — the Game+PIE restriction
> must move to `USubsystem::ShouldCreateSubsystem(Outer)`. A *correctness* item, not a compile item.

---

## 1. `AnomalyInjector` — the injector half

| dependency | file:line | 4.25 | replacement / question |
|---|---|---|---|
| `UTickableWorldSubsystem` base ×3 | `AnomalyInjectorSubsystem.h:12`, `AnomalyAutoInjectorSubsystem.h:35`, `AnomalySelectorSubsystem.h:12` | ✅ **MEASURED (stock 4.25): ABSENT** — prediction CONFIRMED | `Runtime/Engine/Public/Subsystems/WorldSubsystem.h` is **24 lines** and declares only `UWorldSubsystem : public USubsystem` with a single `GetWorld()` override. Replacement unchanged: `UWorldSubsystem` + hand-rolled `FTickableGameObject`. 🆕 **AND A NEW ITEM THIS DOCUMENT MISSED: 4.25's `UWorldSubsystem` has NO `DoesSupportWorldType`.** Our Game+PIE restriction rides that virtual; on 4.25 it must move to **`USubsystem::ShouldCreateSubsystem(UObject* Outer)`** (`Public/Subsystems/Subsystem.h:61`). **A different virtual with a different signature — a CORRECTNESS item, not a rename.** If dropped, the subsystems instantiate on editor-preview worlds. |
| `FWorldDelegates::OnWorldTickEnd` | `AnomalyCaptureSubsystem.cpp:349,350,557,562` | 🚨 **MEASURED (stock 4.25): ABSENT** — the high-risk row went the BAD way | `Classes/Engine/World.h:3671-3679` declares only `OnWorldTickStart`, `OnWorldPreActorTick`, `OnWorldPostActorTick`. A recursive grep of **all** of `Engine/Source/Runtime` returns **ZERO** hits. 🎯 **BUT THE FALLBACK IS BETTER THAN THIS ROW FEARED: `OnWorldPostActorTick` occupies m40's anchor POSITION on 4.25.** In `Private/LevelTick.cpp`, every tickable ticks at `FTickableGameObject::TickObjects` **`:1630`** inside `if (bDoingActorTicks)` opened at **`:1545`** (closing `:1688`), and `OnWorldPostActorTick.Broadcast` is at **`:1694`** inside `if (bDoingActorTicks)` at **`:1690`** — **same guard, immediately after the tickable block, still inside `UWorld::Tick`, still pre-draw.** ⚠ Still needs `m40`'s alignment gate re-run on 4.25 — that is an **E2 entry condition**. |
| `FWorldDelegates::OnWorldPreActorTick` | `AnomalyInjectorSubsystem.cpp:166,177` | ✅ **MEASURED (stock 4.25): PRESENT** | `World.h:3675-3676`; broadcast `LevelTick.cpp:1515`. 🎯 ⇒ **`IAI.Bench.SynthTickOrder` SURVIVES on 4.25, so both tick orders stay testable and the standing both-orders rule is not weakened by the port.** |
| `FWorldDelegates::OnWorldPostActorTick` | `AnomalyViewport.cpp:991,996` | ✅ **MEASURED (stock 4.25): PRESENT** | `World.h:3678-3679`; broadcast `LevelTick.cpp:1694`. Guard `bDoingActorTicks` defined `:1495-1498` = `(TickType!=LEVELTICK_TimeOnly) && !bIsPaused && (netdriver clause)` — **the same expression 5.1 uses at `:1448-1451`**. |
| `FCoreDelegates::OnEndFrame` | `AnomalyCaptureSubsystem.cpp:348,552` | ✅ **PRESENT** | Long-standing. |
| `SetActorHiddenInGame`, `GetComponentsBoundingBox`, `LineTraceSingleByChannel` | `AnomalyViewport.cpp` (2 traces), targeting | ✅ **PRESENT** | Stable across 4.x. |
| **m45 hide flags**: `bRenderInMainPass`, `bRenderInDepthPass`, `SetCastShadow`, `bCastContactShadow`, `bAffectDynamicIndirectLighting`, `bAffectDistanceFieldLighting`, `bReceivesDecals` | `AnomalyHiddenClass.cpp:102-125` | 🔻 **MEASURED (stock 4.25): 7 of 8 PRESENT — `bCastContactShadow` is ABSENT** | `Classes/Components/PrimitiveComponent.h`: `bRenderInMainPass` **:331** · `bRenderInDepthPass` **:335** · `CastShadow` **:377** · `bAffectDynamicIndirectLighting` **:381** · `bAffectDistanceFieldLighting` **:385** · `bReceivesDecals` **:339** · `bVisibleInRayTracing` **:327**. 🚨 **`bCastContactShadow`: ZERO hits across ALL of `Source/Runtime/Engine`.** Must be compiled out on 4.25 (a declaration-site guard on a save/restore field). ⚠ **NOT cosmetic:** contact shadows are a separate shadowing path, so a hidden target on 4.25 cannot have them silenced — **an open pixel question for the 4.25 identity arbiter, not something to assume away.** 🎯 **AND THE ROW'S REAL QUESTION — "the RENDERER BEHAVIOUR behind them" — IS NOW ANSWERED AND m45 SURVIVES:** `bRenderCustomDepth` is honoured with `bRenderInMainPass=false`, on three links — the proxy takes it straight from the component (`Private/PrimitiveSceneProxy.cpp:144`, accessor `PrimitiveSceneProxy.h:495`); `bHasCustomDepthPrimitives` is set from it **alone** (`Private/SceneVisibility.cpp:2069-2071`, propagated `:2443`); and the relevance gates are **disjunctions** (`SceneVisibility.cpp:2248`, `:2704`, `:2709`), with the pass gating on the proxy (`Private/CustomDepthRendering.cpp:58`). |
| `bVisibleInRayTracing` | `AnomalyHiddenClass.cpp:108,124` | 🎯 **MEASURED (stock 4.25): PRESENT AND UNGUARDED** — prediction REFUTED, in our favour | `Classes/Components/PrimitiveComponent.h:327`, a plain `UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = Rendering) uint8 bVisibleInRayTracing : 1;` — **no `#if RHI_RAYTRACING`**. **No flag to drop; zero work.** |
| `SetRenderCustomDepth` / `SetCustomDepthStencilValue` | `AnomalyStencilTag`, probe spawn | ✅ **PRESENT** | Custom depth/stencil is 4.x-era. |
| `SetMaterial`, per-slot restore | `Anomaly_MissingTexture.cpp:123,205,216,236`, `Anomaly_CorruptedTexture.cpp` same | ✅ **PRESENT** | |
| `ULightComponent::SetCastShadows` | `Anomaly_LightingMismatch.cpp:81,107` | ✅ **PRESENT** | |
| Forced LOD (`AnomalyLod::SetForcedLod/GetForcedLod`, `GetNumLODs`) | `Anomaly_LodCorruption.cpp:62-84`, `Anomaly_LodPopping.cpp:253,268` | 🎯 **MEASURED (stock 4.25): ALL PRESENT, UNCHANGED SHAPE** — "CHANGED (likely)" REFUTED | `Classes/Components/SkinnedMeshComponent.h`: `GetNumLODs()` **:673** · `SetForcedLOD(int32)` **:689** · `GetForcedLOD()` **:693** · `ForcedLodModel` deprecated **since 4.24** (`:406`), *exactly as on 5.1*. `Classes/Components/StaticMeshComponent.h`: `ForcedLodModel` **:176** (public, **not** deprecated) · `SetForcedLodModel` **:381**. `Classes/Engine/StaticMesh.h`: `GetNumLODs()` **:1073**. ⇒ **every symbol `AnomalyLod.cpp:34-41,53-57,64-72,84-125` uses exists. ZERO WORK.** 📌 The `m3` isolation still paid off — it made this a five-minute check instead of a survey. |
| `FAutoConsoleCommandWithWorldAndArgs` ×36, `IConsoleManager` ×5 | throughout | ✅ **PRESENT** | |
| `UDebugDrawService` ×4, `WasInputKeyJustPressed` ×8 | selector HUD/input | ✅ **PRESENT** | |
| `ConstructorHelpers::FObjectFinder` ×3 | `AnomalyInjectorSubsystem.cpp:38,42` | ✅ **PRESENT** | The `m8` cook guarantee for the two Content materials. |
| `TFunction`-based census provider contract | `AnomalyCensusProvider.h:32-34` | ✅ **PRESENT** | Pure C++; no engine surface. |
| `Foliage` module (`AInstancedFoliageActor`) | `AnomalyInjector.Build.cs` | ✅ **PRESENT** | Runtime module in 4.x. |
| `UPROPERTY` (only 2 uses), no `UFUNCTION` reflection tricks | | ✅ **PRESENT** | |
| **C++ level** — `if constexpr` **0**, `std::` **0**, structured bindings **0**, `FVector3f`/`FVector3d`/`UE::Math` **0**, `TStringBuilder` **0** | measured across all injector + capture `.cpp` | ✅ **NO RISK** | 🎯 **This is the single best piece of news in the audit.** The code is conservative C++ and does **not** use LWC types, so the 5.x double-precision `FVector` migration is a **non-issue** in both directions. |

**Injector-half verdict: SMALL.** One real structural change (`UTickableWorldSubsystem` → `UWorldSubsystem` + `FTickableGameObject`), one real unknown (`OnWorldTickEnd`), one contained unknown (LOD accessors), one droppable flag.

> 🆕 **REVISED 2026-09-03 (session 070) against the stock 4.25 tree — STILL SMALL, and the WORK MOVED
> rather than shrank.** The verdict stands; three of its four items resolved and two new ones appeared.
> · ✅ **structural change — CONFIRMED**, and unchanged in size.
> · 🔻 **`OnWorldTickEnd` — ABSENT.** But `OnWorldPostActorTick` sits in m40's anchor position, so this
> costs a **binding-site `#if`**, not a redesign. ⚠ Its **alignment gate must be re-run on 4.25 (E2).**
> · 🎯 **LOD accessors — MEASURED UNCHANGED. Zero work.**
> · 🎯 **`bVisibleInRayTracing` — MEASURED PRESENT. Nothing to drop.**
> · 🔻 🆕 **`bCastContactShadow` is ABSENT and must be dropped instead.**
> · 🆕 **`UWorldSubsystem` has no `DoesSupportWorldType`** ⇒ move the Game+PIE restriction to
> `ShouldCreateSubsystem`. **A correctness item this document did not enumerate.**
>
> ⇒ **The original "1–2 days if `OnWorldTickEnd` exists; +2–3 days if not" is REVISED DOWN: it does not
> exist, and the fallback turned out to be positional rather than semantic, so the `+2–3 days` penalty is
> not incurred in E1** — it is deferred to E2's alignment re-gate, where it belongs. ⛔ **Estimate, not a
> measurement; nothing has been compiled on 4.25.**

---

## 2. `AnomalyCapture` — the capture half

| dependency | file:line | 4.25 | replacement / question |
|---|---|---|---|
| `FSceneViewExtensionBase` + `FAutoRegister` | both SVEs, 4 sites | ✅ **PRESENT** | The SVE mechanism predates 4.25. |
| `IsActiveThisFrame_Internal(const FSceneViewExtensionContext&)` | `AnomalySceneViewExtension.h:25`, `AnomalyMaskSceneViewExtension.h:47` | ✅ **MEASURED (stock 4.25): CHANGED as predicted** | `FSceneViewExtensionContext` **ABSENT** (zero `.h` hits in `Runtime`). 4.25 has `virtual bool IsActiveThisFrame(class FViewport* InViewport) const` at **`Runtime/Engine/Public/SceneViewExtension.h:146`** ⚠ (note: **Engine**, not Renderer). Mechanical override-signature change on both SVEs, as predicted. |
| `SubscribeToPostProcessingPass(EPostProcessingPass, FAfterPassCallbackDelegateArray&, bool)` | `AnomalyMaskSceneViewExtension.cpp:91-98` (Tonemap), `AnomalySceneViewExtension.cpp` (VisualizeDepthOfField) | 🚨 **MEASURED (stock 4.25): ABSENT** — confirmed. 🔻 **AND SO IS `PrePostProcessPass_RenderThread`, which this row asserted 4.25 HAS** | Both return **zero** `.h` hits in `Runtime`; so do `EPostProcessingPass` and `FSceneViewExtensionContext`. **4.25's ACTUAL hook set** (`Runtime/Engine/Public/SceneViewExtension.h`): `SetupViewFamily` **:91** · `SetupView` **:96** · `SetupViewPoint` **:101** · `SetupViewProjectionMatrix` **:106** · `BeginRenderViewFamily` **:111** · `PreRenderViewFamily_RenderThread` **:116** · `PreRenderView_RenderThread` **:121** · `PostRenderBasePass_RenderThread` **:126** · `PostRenderViewFamily_RenderThread` **:131** · `PostRenderView_RenderThread` **:136**. 🚨 **EVERY render-thread hook takes `FRHICommandListImmediate&`; NONE takes an `FRDGBuilder`.** ⚠ **Which hook to use is an E2 decision needing a measurement:** custom depth renders at `Private/DeferredShadingRenderer.cpp:1826` (before `RenderBasePass` **:2014**) **or** at **:2149** (after it), by engine setting — so `PostRenderBasePass_RenderThread` is **not guaranteed** to have custom depth ready, while `PostRenderView(Family)_RenderThread` is. |
| `FRDGBuilder`, `FRDGTextureDesc::Create2D`, `AddEnqueueCopyPass` | `AnomalyMaskSceneViewExtension.cpp:144,186,196`, `AnomalySceneViewExtension.cpp:139` | 🎯🎯 **MEASURED (stock 4.25): RDG IS USABLE FROM AN SVE HOOK. THE MASK PASS IS A PORT, NOT A REWRITE.** | **This row's own question is the one that decided the size of the whole capture half, and it answers well.** `class RENDERCORE_API FRDGBuilder` — `Runtime/RenderCore/Public/RenderGraphBuilder.h:16` — has **`FRDGBuilder(FRHICommandListImmediate& InRHICmdList);` at :20** and **`void Execute(); ` at :167** (holding `FRHICommandListImmediate& RHICmdList;` at `:173`). ⇒ inside any `*_RenderThread` hook (all handed an `FRHICommandListImmediate&`) the 4.25 backend constructs its **own** builder, adds the existing passes, and `Execute()`s. **Shader, parameter struct, GPU reduce and readback ALL SURVIVE.** |
| `FScreenPassTexture`, `FPostProcessMaterialInputs`, `EPostProcessMaterialInput` | `AnomalyMaskSceneViewExtension.cpp:101-104` | ✅ **MEASURED (stock 4.25): PRESENT but RENDERER-PRIVATE** | `FScreenPassTexture` → `Runtime/Renderer/Private/ScreenPass.h:42`; `FPostProcessMaterialInputs` → `Runtime/Renderer/Private/PostProcessMaterial.h:51`. ⚠ They exist, but the **handoff** does not — no 4.25 hook passes them in. The 4.25 backend binds the scene-texture uniform buffer directly instead. |
| `AddDrawTexturePass` | `AnomalyMaskSceneViewExtension.cpp:37` | ✅ **MEASURED (stock 4.25): PRESENT, renderer-private** | `Runtime/Renderer/Private/ScreenPass.h:422` and `:432`. Same private-include exposure as 5.1 (`G100`). |
| `FPixelShaderUtils::AddFullscreenPass` | `:160` | 🎯 **MEASURED (stock 4.25): PRESENT AND PUBLIC** | `struct RENDERCORE_API FPixelShaderUtils` — **`Runtime/RenderCore/Public/PixelShaderUtils.h:18`**, with **`AddFullscreenPass` at :80** and `DrawFullscreenTriangle` at `:36`. **No manual fullscreen draw needed.** |
| `FComputeShaderUtils::AddPass` / `GetGroupCount` | `:181-182` | 🎯 **MEASURED (stock 4.25): PRESENT AND PUBLIC** | `Runtime/RenderCore/Public/RenderGraphUtils.h:99`. ⇒ **the `m34` GPU reduce ports.** The CPU-reduce fallback (`IAI.Capture.MaskReduce cpu`) stays available but is **no longer required**. |
| 🚨 `#include "SceneRendering.h"` + `static_cast<const FViewInfo&>(View)` | `AnomalyMaskSceneViewExtension.cpp:22,36,153,154,216`; enabled by `PrivateIncludePaths.Add(GetModuleDirectory("Renderer")/Private)` at `AnomalyCapture.Build.cs:355` | ✅ **MEASURED (stock 4.25): PRESENT.** 🚨 **STILL HIGH RISK FOR EUROPA — and this row's real subject is the FORK, which the bench cannot touch.** | **Stock half answered both ways:** the launcher install **does** ship the private headers (`Runtime/Renderer/Private/SceneRendering.h` and `ScenePrivate.h` both present), and `class FViewInfo : public FSceneView` is at **`SceneRendering.h:860`** with **`FIntRect ViewRect;` at :865** — same member name as 5.1. ⚠ **A partial escape hatch exists:** `FSceneView::UnscaledViewRect` (`Runtime/Engine/Public/SceneView.h:880`) and `UnconstrainedViewRect` (`:883`) are **public**. ⛔ **NONE OF THIS CONSTRAINS EUROPA** — a fork can move, rename or restructure anything under `Renderer/Private`, and that remains the single highest-risk item in the port (`G100`, and §D q6 is unchanged). |
| `FRHIGPUTextureReadback` | `AnomalyFrameCapturer.cpp:317`, `AnomalyMaskSceneViewExtension.cpp:195`, `AnomalySceneViewExtension.cpp:147` | 🎯 **MEASURED (stock 4.25): PRESENT AND PUBLIC** | **`Runtime/RHI/Public/RHIGPUReadback.h:92`.** The family had landed by 4.25. **No `RHIReadSurfaceData` fallback needed.** |
| `FRHIGPUBufferReadback` | `AnomalyMaskSceneViewExtension.cpp:184` | 🎯 **MEASURED (stock 4.25): PRESENT AND PUBLIC** | **`Runtime/RHI/Public/RHIGPUReadback.h:75`.** |
| `OnBackBufferReadyToPresent` | `AnomalyFrameCapturer.cpp:177,188,222` | 🎯 **MEASURED (stock 4.25): PRESENT WITH A BYTE-IDENTICAL SIGNATURE** | 4.25 `Runtime/SlateCore/Public/Rendering/SlateRenderer.h:261`: `DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBackBufferReadyToPresent, SWindow&, const FTexture2DRHIRef&);` — **the same line as 5.1's `:299`**; accessor `:262` vs `:300`. ⇒ **the backbuffer capture path ports with ZERO signature change**, confirming this row's prediction outright. |
| `IMPLEMENT_GLOBAL_SHADER`, `SHADER_USE_PARAMETER_STRUCT`, `BEGIN_SHADER_PARAMETER_STRUCT` | `AnomalyVisibleMaskShader.h:15`, `AnomalyMaskReduceShader.h:14` | ✅ **PRESENT** | Shader parameter structs exist in 4.25. |
| `FSceneTextureShaderParameters` + `.usf` `#include "/Engine/Private/SceneTexturesCommon.ush"`, `CalcSceneCustomStencil` | `AnomalyVisibleMask.usf:13,31` | 🎯 **MEASURED (stock 4.25): A ONE-LINE SUBSTITUTION, NOT A SHADER REWRITE** | `Engine/Shaders/Private/SceneTexturesCommon.ush` **EXISTS**, and `SceneTexturesStruct.SceneDepthTexture` is used throughout it (`:43,:62,:72`). ⚠ **`CalcSceneCustomStencil` is ABSENT** (zero hits across all of `Engine/Shaders/Private`) — 4.25 reads custom stencil explicitly as `SceneTexturesStruct.CustomStencilTexture.Load(int3(PixelPos,0)) STENCIL_COMPONENT_SWIZZLE` (`Shaders/Private/DeferredShadingCommon.ush:635`, `:704`). ⇒ **only `AnomalyVisibleMask.usf:31` changes.** ✅ **Lines `:38-39` need NO change** — they already use `SceneTexturesStruct.CustomDepthTexture.Load` / `SceneTexturesStruct.SceneDepthTexture.Load`, which is 4.25's own idiom. 🚨 **But `G129` means this cannot ride E1:** `IMPLEMENT_GLOBAL_SHADER` is not gated by any cvar, so a failed global shader is **fatal at engine init** with every feature switch off. ⇒ **`AnomalyShaders` must be stubbed for E1 and the `.usf` port belongs to E2.** |
| `FMaterialShaderMap::UsesSceneTexture(PPI_*)` (the host-PP preflight) | `AnomalyCaptureSubsystem.cpp:1667-1686`, `5346-5362` | 🎯 **MEASURED (stock 4.25): PRESENT, AND THE ENUM IS LINE-FOR-LINE IDENTICAL TO 5.1** | `UsesSceneTexture(uint32 TexId)` at **`Runtime/Engine/Public/MaterialShared.h:1228`**. `ESceneTextureId` in `Runtime/Engine/Public/MaterialSceneTextureId.h` is **identical between 4.25 and 5.1** — all 31 entries, same order, **same line numbers**; `PPI_CustomDepth` **:45** and `PPI_CustomStencil` **:69** in *both*. ⇒ **`m41`'s preflight ports UNCHANGED; nothing needs dropping.** |
| `GetCachedPostProcessBlends`, `GetCameraCacheView` | `AnomalyCaptureSubsystem.cpp:1721,1732` | ⚠ **UNKNOWN** | Same preflight; same droppable status. |
| `ENQUEUE_RENDER_COMMAND` ×4, `FlushRenderingCommands` ×8 | throughout | ✅ **PRESENT** | |
| `FScopeLock` ×53, `FThreadSafeCounter` ×25, `ESPMode::ThreadSafe` ×31, `Async()` | throughout | ✅ **PRESENT** | |
| `IImageWrapper` / `IImageWrapperModule` (PNG, gray-8) | label/preview writers | ✅ **MEASURED (stock 4.25): PRESENT, same ordinal** | `Runtime/ImageWrapper/Public/IImageWrapper.h:44-49` — `enum class ERGBFormat : int8 { RGBA = 0, BGRA = 1, **Gray = 2** }`, identical to 5.1. **The `m43` gray-8 mask format is expressible.** |
| `FJsonObject` / `TJsonWriter` | label writer | ✅ **PRESENT** | |
| `FOutputDevice` (the `m38` run log) | `AnomalyRunLog` | ✅ **PRESENT** | |

**Capture-half verdict: LARGE**, and concentrated in exactly three things: the **SVE post-process hook**, **RDG availability**, and **renderer-private `FViewInfo`**.

> 🆕 **REVISED 2026-09-03 (session 070), against the stock 4.25 tree — TWO of the three shrank, ONE did not:**
> · 🎯 **RDG availability: RESOLVED IN OUR FAVOUR.** `FRDGBuilder(FRHICommandListImmediate&)` + `Execute()`
> exist, and `FPixelShaderUtils`/`FComputeShaderUtils`/`FRHIGPUTextureReadback`/`FRHIGPUBufferReadback` are
> all in **public** headers. **The mask pass is a PORT, not a rewrite.**
> · 🎯 **`FViewInfo::ViewRect`: PRESENT on stock, and the launcher install ships the private headers.**
> ⛔ **But this item's real subject is whether EUROPA's renderer is modified, and that is untouched.**
> · 🔻 **The SVE hook: UNCHANGED IN SIZE, and slightly worse** — `PrePostProcessPass_RenderThread` does not
> exist either, so the hook must be chosen from 4.25's actual set, and *which* one is an E2 decision gated
> on where custom depth lands in the frame.
>
> ⇒ **The honest revised estimate for the capture half is the LOWER half of the original 1.5–4 week range**,
> because the two largest unknowns resolved well and the shader turned out to be a one-line change.
> ⛔ **It is still an ESTIMATE, still gated on §D-2's fork questions, and NOTHING has been compiled.**

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
| `WebSocketNetworking` plugin | `.uplugin` `Plugins[]` | ✅ **MEASURED (stock 4.25): PRESENT and usable** | `Engine/Plugins/Experimental/WebSocketNetworking/WebSocketNetworking.uplugin` — `Type: Runtime`, `EnabledByDefault: false`, `Installed: false`. **Ships BOTH `Source/` and prebuilt `Binaries/Win64/UE4Editor-WebSocketNetworking.dll`** (with `.precompiled` markers). ⛔ **Says nothing about a modified fork, which may have removed it** — §D q18 stands for Europa. 📌 Moot for E1 either way: the control server is stubbed. |
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
| `AnomalyCapture.Build.cs` **tick-pin fork probe** — scans the engine tree for `sUseFixedGameTickWithVariableRenderTick_Net`, walks `Source/Runtime/Core`, `Source/Editor`, `Plugins` | ✅ **MEASURED (stock 4.25): DEGRADES CORRECTLY, DOES NOT MISFIRE** | Routes A/B: the literal `sUseFixedGameTickWithVariableRenderTick_Net` has **zero hits** across `Source/Runtime/Core`. Route C (`NameLooksLikeFork`, `AnomalyCapture.Build.cs:108-113`, matching dirs named `FWNet*`/`Firewalk*`): **zero matches** under `Engine/Plugins`. ⇒ probe reports **NOT FOUND**, `ANOMINJECT_FW_TICKPIN=0`, the pin compiles out, and the build log says so. ⛔ **Europa is the possibly-modified 4.25 this probe was BUILT for — its echo must still be read on the first Europa build.** |
| `.uplugin` `EngineVersion` | ✅ **MEASURED (stock 4.25): LEAVE IT UNSET — that is CORRECT, not an oversight** | In 4.25's UBT it is an optional string (`PluginDescriptor.cs:115`), parsed at `:246`, and only written back when non-empty (`:365-367`). **There is no build-time compatibility enforcement.** ⇒ **no separate `.uplugin` is needed, and setting the field would PIN the plugin to one engine and defeat the one-codebase goal.** |

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
1. ⛔ **STRUCK 2026-09-03 (session 070) — THIS STEP CANNOT STAND AS WRITTEN.** *"`.uplugin` reduced to
   `AnomalyInjector` only"* is impossible: **there is ONE `.uplugin` in ONE tree, and reducing it breaks
   the 5.1 host on the same branch.** ✅ **Replacement, measured:** keep the 4-module `.uplugin` and add
   **one condition per non-injector `Build.cs`** — `Target.Version.MajorVersion >= 5` ANDed into the
   existing `!= Shipping` gate — reusing the **already-shipped** `ANOMALY_CAPTURE=0` /
   `ANOMALY_CONTROL_SERVER=0` compile-out that every Shipping build exercises. `IMPLEMENT_MODULE` sits
   **outside** those guards (`AnomalyCaptureModule.cpp:24`, `AnomalyControlServerModule.cpp:24`), so the
   modules still load and log *"compiled out"*. `Target.Version.MajorVersion` is valid on 4.25
   (`TargetRules.cs:193`, `BuildVersion.cs:272`). **`AnomalyShaders` needs the same treatment** — `G129`
   makes its global shaders a fatal boot failure otherwise. Full working: PHASE1 §D.
2. Subsystem base swap: `UTickableWorldSubsystem` → `UWorldSubsystem` + `FTickableGameObject` (×3, ×4
   with capture later). 🆕 **AND the world-type restriction with it** — 4.25's `UWorldSubsystem` has no
   `DoesSupportWorldType`; use `USubsystem::ShouldCreateSubsystem(Outer)` (§1).
3. Resolve the three delegate questions (§D-1) and pick the tick anchor. ✅ **ANSWERED:** `OnWorldTickEnd`
   is **absent**; **`OnWorldPostActorTick` is the 4.25 anchor** (§1). ⛔ **Do NOT move 5.1 onto it** — that
   re-opens `P9`, closed and validated on Bates at `m40`. `#if` at the **binding site** only.
4. LOD accessors behind `AnomalyLod` (already isolated) — fix or stub. 🎯 **MEASURED: NO CHANGE NEEDED.**
5. Drop `bVisibleInRayTracing` if absent. 🎯 **MEASURED PRESENT AND UNGUARDED: NO CHANGE NEEDED.**
   🔻 **But `bCastContactShadow` IS absent and must be dropped instead** (§1) — the work did not vanish, it
   moved.

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

> 🆕 **2026-09-03, SESSION 070 — STATUS OF THIS LIST.** Everything a **stock 4.25** tree can settle is now
> **MEASURED** and marked ✅ below, with engine `file:line` in §1–§5 and full working in
> `PHASE1-bench-audit-and-plan.md` §B. ⛔ **The rows marked 🔒 are OFFICE-PC-ONLY: they are properties of a
> possibly-modified fork or of the title's own content and configuration, and NOTHING on the bench can
> answer them.** ⚠ **A ✅ here means "measured on stock 4.25", which is a PREDICTION for Europa — never a
> measurement of it.**

**D-1 — engine surface (settles the injector half)**
1. ✅ **ANSWERED — ABSENT.** `OnWorldTickEnd` has zero hits in all of `Engine/Source/Runtime`. 🎯 **The
   replacement is `OnWorldPostActorTick`, which broadcasts at `LevelTick.cpp:1694` under the SAME
   `bDoingActorTicks` guard (`:1690`) that encloses `TickObjects` at `:1630` (opened `:1545`)** — i.e. after
   every tickable, still pre-draw. m40's structural argument transfers; its **alignment gate must still be
   re-run on 4.25, which is an E2 entry condition.**
2. ✅ **ANSWERED — BOTH PRESENT.** `World.h:3675-3679`; broadcast `LevelTick.cpp:1515` and `:1694`.
   🎯 ⇒ `IAI.Bench.SynthTickOrder` survives; both tick orders stay testable.
3. ✅ **ANSWERED — ABSENT; compose `UWorldSubsystem` + `FTickableGameObject`.** 🆕 **And a NEW item: 4.25's
   `UWorldSubsystem` has no `DoesSupportWorldType`** — the Game+PIE restriction moves to
   `USubsystem::ShouldCreateSubsystem(Outer)` (`Subsystem.h:61`).
4. ✅ **ANSWERED — ALL PRESENT, UNCHANGED.** Zero work (§1).
5. ✅ **ANSWERED — PRESENT AND UNGUARDED** (`PrimitiveComponent.h:327`). Zero work.
   🔻 **6 (new). `bCastContactShadow` is ABSENT** and must be compiled out — the one m45 flag that moved
   the wrong way.

**D-2 — renderer (settles whether the capture half is a port or a rewrite)**
6. 🔒 **OFFICE-PC ONLY. Is the renderer MODIFIED?** ⚠ *Stock half only:* the launcher install **does** ship
   `Renderer/Private`, and `FViewInfo : public FSceneView` (`SceneRendering.h:860`) has `FIntRect ViewRect`
   (**`:865`**). 🚨 **That does NOT constrain a fork.** Still the highest-risk item in the port.
7. 🔒 **OFFICE-PC ONLY. Forward or deferred?** A title's choice.
8. 🔒 **OFFICE-PC ONLY, AND STILL THE HIGHEST-VALUE QUESTION IN THE LIST. Does the game already use custom
   depth/stencil, and for what?** The `200–254` reservation and `m26`'s collision detectors depend on the
   answer. Purely a property of the title's content and code.
9. ✅ **ANSWERED.** 4.25's hook set is `SetupViewFamily`/`SetupView`/`SetupViewPoint`/
   `SetupViewProjectionMatrix`/`BeginRenderViewFamily`/`PreRenderViewFamily_RenderThread`/
   `PreRenderView_RenderThread`/`PostRenderBasePass_RenderThread`/`PostRenderViewFamily_RenderThread`/
   `PostRenderView_RenderThread` (`Engine/Public/SceneViewExtension.h:91-136`). 🔻 **`PrePostProcessPass_RenderThread`
   does NOT exist** (this document assumed it did). 🚨 **No hook is handed an `FRDGBuilder`** — every one
   takes `FRHICommandListImmediate&`. 🎯 **But that is sufficient: `FRDGBuilder` is constructible FROM one**
   (`RenderCore/Public/RenderGraphBuilder.h:20`) with `Execute()` at `:167`, **so the mask pass is a PORT.**
10. ✅ **ANSWERED — BOTH PRESENT, in a PUBLIC header** (`RHI/Public/RHIGPUReadback.h:92` and `:75`).
11. ✅ **ANSWERED — a ONE-LINE change.** `SceneTexturesCommon.ush` exists; `CalcSceneCustomStencil` does not;
    4.25's idiom is `SceneTexturesStruct.CustomStencilTexture.Load(int3(P,0)) STENCIL_COMPONENT_SWIZZLE`
    (`DeferredShadingCommon.ush:635`). Only `AnomalyVisibleMask.usf:31` changes; `:38-39` already match.
12. ✅ **ANSWERED — BOTH PRESENT AND PUBLIC.** `FPixelShaderUtils` (`RenderCore/Public/PixelShaderUtils.h:18`,
    `AddFullscreenPass` `:80`), `FComputeShaderUtils` (`RenderCore/Public/RenderGraphUtils.h:99`).
13. ✅ **ANSWERED — PRESENT, and the `PPI_*` enum is LINE-FOR-LINE IDENTICAL to 5.1.** Nothing to drop.

**D-3 — runtime configuration (settles which gates are even applicable)**
14. 🔒 **OFFICE-PC ONLY. Screen percentage / dynamic resolution in use?**
15. 🔒 **OFFICE-PC ONLY. TAA method, and is it temporally accumulating?**
16. ✅ **CONFIRMED — no Nanite on 4.25** ⇒ 🎯 **`G134`'s limitation does not exist on Europa.** Strengthened
    by measurement: the only custom-depth exclusion path we ever established was Nanite-specific, and
    4.25's relevance gates OR `bRenderCustomDepth` with the main/depth-pass bits (`SceneVisibility.cpp:2248`,
    `:2704`, `:2709`). **Every target is potentially maskable** — a genuine improvement over the 5.1 host.
17. 🔒 **OFFICE-PC ONLY for Europa** — but ✅ **measured on the bench fixture: the probe returns NOT FOUND and
    does not misfire** (neither the symbol in `Source/Runtime/Core` nor a `FWNet*`/`Firewalk*` directory
    exists on stock). ⛔ **Europa is exactly the modified case the probe exists for; read its echo there.**
18. ✅ **ANSWERED for stock — PRESENT with source and binaries.** 🔒 A fork may have removed it; moot for E1,
    which stubs the control server.
19. 🔒 **OFFICE-PC ONLY (from §F). Does Europa cook to `.utoc`/`.ucas` or `.pak` only?**
20. 🆕 🔒 **OFFICE-PC ONLY (new, and made urgent by q1's answer): did the fork touch `UWorld::Tick`?** The
    whole 4.25 anchor argument rests on `OnWorldPostActorTick` broadcasting after `TickObjects` under one
    guard. **Stock-verified, Europa-unverified**; E1's alignment gate is what would catch a difference.

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

> 🆕 ✅ **RESOLVED 2026-09-03 (session 070) — IT IS DONE, AND IT COST LESS THAN THIS SECTION BUDGETED.**
> The engine is **`E:\EpicGames\UE_4.25`, a LAUNCHER-INSTALLED 4.25.4** (`Engine/Build/InstalledBuild.txt`
> present; no `Setup.bat`, no `GenerateProjectFiles.bat`, no `.git`; and
> `C:\ProgramData\Epic\UnrealEngineLauncher\LauncherInstalled.dat` records
> `UE_4.25 → E:\EpicGames\UE_4.25`, `4.25.4-14469661+++UE4+Release-4.25-2023.1-Windows`).
> The host project is **`E:\Unreal Projects\StylizedParisStreet`** (BP-only, `EngineAssociation "4.25"`).
> 🚨 **It is a BENCH FIXTURE, NOT EUROPA.**
>
> · 🎯 **The "~150–250 GB source build" cost is NOT incurred** — a launcher install is already on disk.
> Free space measured: **`C:` 29.31 · `D:` 188.21 · `E:` 365.13 GB.**
> · 🎯 **The registry warning DOES NOT APPLY to a launcher install.** `HKCU\...\Builds` still holds
> **exactly one** entry — the 5.1 source build's GUID. A launcher engine is addressed by the **version
> string** `"4.25"` through `LauncherInstalled.dat`, not by a GUID. **No registry change is needed and
> none should be made.**
> · ⚠ **A separate correction: `HKLM\SOFTWARE\EpicGames\Unreal Engine` lists 4.26/4.27/5.1–5.6 under
> `D:\EpicGames\UE_*`, and NONE of those directories exist. Those entries are STALE** — a cold session
> reading that key would wrongly conclude 4.25 is not installed.
> · ✅ **Separate `Intermediate`/DDC is preserved automatically**, and the shared plugin tree does **not**
> collide: 4.25 writes `Binaries\Win64\UE4Editor-*.dll` and
> `Intermediate\Build\Win64\{UE4Editor,UE4}\…` while 5.1 writes `UnrealEditor-*` and
> `Intermediate\Build\Win64\UnrealEditor\…` (both measured). `.gitignore` already covers both wholesale.
> ⚠ **One operational residual: a hand-deletion of `Intermediate\` destroys BOTH engines' state.**
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
3. ✅ **ANSWERED 2026-09-03 (session 070): YES, THE BENCH HOSTS 4.25.** A launcher-installed **4.25.4** at
   `E:\EpicGames\UE_4.25` plus a BP-only host project `E:\Unreal Projects\StylizedParisStreet`.
   🎯 **Both feared costs evaporated:** no ~150–250 GB source build (it is a launcher install) and **no
   second registry mapping** (a launcher engine resolves by version string, not GUID — `HKCU\...\Builds`
   still holds exactly one entry). ⇒ **five of the six E1 gates become gates I can run headlessly**;
   only `E1-G3`'s eye half needs the owner. 🚨 **And it is a BENCH FIXTURE on a STOCK engine — it does not
   reduce Europa's fork risk by one bit** (NEEDS-DECISION 1 and 2 below are untouched by it).
   ✅ **Toolchain verified the same day: buildable TODAY with no install**, using the already-present
   VS2017 Build Tools 15.9 / MSVC **14.16.27023** — which is on 4.25's own `PreferredVisualCppVersions`
   list (`UEBuildWindows.cs:618-621`) — provided the compiler and Windows-SDK are **pinned**
   (`10.0.17763.0`), because 4.25's defaults would otherwise select VS2022's 14.42 and the 22621 SDK.
   Full working: `PHASE1-bench-audit-and-plan.md` §A.
