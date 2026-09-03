# EUROPA — PHASE 1 STAGE 0: BENCH AUDIT + E1 PLAN

**2026-09-03, session 070 brief 01. READ-ONLY: no source was changed, nothing was built, nothing was
written into the 4.25 host project.** Deliverable of a planning brief; **E1 is not started.**

---

## 0. THE DISTINCTION THAT GOVERNS THIS WHOLE DOCUMENT

🚨 **`StylizedParisStreet` on a stock UE 4.25.4 is a BENCH FIXTURE. It is NOT EUROPA.**

Europa is a title on a **possibly-modified** 4.25 on the owner's office PC and remains **entirely
unmeasured**. Everything below that was read out of the stock 4.25 tree is therefore **two different
statements at once**, and this document keeps them in two columns:

| column | meaning |
|---|---|
| **MEASURED (stock 4.25)** | a fact about `E:\EpicGames\UE_4.25`, with an engine `file:line` citation. Certain. |
| **PREDICTION (Europa)** | what that fact implies for Europa. **A prediction, not a measurement.** |

⛔ **No sentence in this document says "we measured Europa".** Where a stock-tree fact cannot constrain
Europa at all (anything downstream of "is the renderer modified?"), the Europa column reads
**UNKNOWN — office PC only**, and §4 lists those.

⚠ **`G120` is the reason for this shape:** an unverified mechanism must never become a scope decision.
A stock-4.25 measurement is excellent evidence about *the 4.x engine surface* and **no evidence at all**
about *a fork's modifications*.

📌 The bench fixture needs no codename. Europa's codename discipline is unchanged.

---

## A. TOOLCHAIN GO/NO-GO

### A.0 VERDICT

> ✅ **YES — C++ can be built against 4.25 on this machine TODAY, with no install, provided TWO PINS are
> set. Without the pins the default selection is wrong and the build will fail.**

The pins are **configuration, not installation**, and they belong in the 4.25 host's `*.Target.cs`
(§C.1) so they cannot leak into 5.1 builds:

```csharp
WindowsPlatform.Compiler          = WindowsCompiler.VisualStudio2017;
WindowsPlatform.CompilerVersion   = "14.16.27023";
WindowsPlatform.WindowsSdkVersion = "10.0.17763.0";
```

### A.1 The 4.25 install

| item | value |
|---|---|
| **Root** | `E:\EpicGames\UE_4.25` |
| **Version** | `4.25.4`, Changelist `14469661`, CompatibleChangelist `13144385`, `BranchName ++UE4+Release-4.25`, `IsPromotedBuild 1`, `IsLicenseeVersion 0` (`Engine/Build/Build.version`) |
| **Install type** | 🎯 **LAUNCHER-INSTALLED (an Installed Build), not a source build** |
| **How determined** | `Engine/Build/InstalledBuild.txt` **present**; `Setup.bat` **absent**; `GenerateProjectFiles.bat` **absent**; `.git` **absent**; and the Epic launcher manifest `C:\ProgramData\Epic\UnrealEngineLauncher\LauncherInstalled.dat` lists `UE_4.25 → E:\EpicGames\UE_4.25`, `AppVersion 4.25.4-14469661+++UE4+Release-4.25-2023.1-Windows`. Four independent signals agree. |

⚠ **`HKLM\SOFTWARE\EpicGames\Unreal Engine` does NOT list 4.25** — it lists 4.26, 4.27, 5.1–5.6 pointing
at `D:\EpicGames\UE_*`, **and none of those directories exist** (`D:\EpicGames` contains no `UE_*`
folders). **Those registry entries are STALE.** The live installs are recorded only in
`LauncherInstalled.dat`: `UE_4.25` on `E:`, plus `UE_5.7`/`UE_5.8` on `E:`. 📌 Recorded because a cold
session reading that registry key would conclude 4.25 is not installed.

### A.2 Visual Studio / MSVC / Windows SDK inventory

**Visual Studio installations** (from `vswhere -all -prerelease -products *`):

| product | version | path |
|---|---|---|
| Visual Studio **Community 2022** | `17.12.35707.178` | `C:\Program Files\Microsoft Visual Studio\2022\Community` |
| Visual Studio **Build Tools 2017** | `15.9.34031.82` | `C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools` |

**MSVC toolsets** (all verified to contain `bin\Hostx64\x64\cl.exe`):

| toolset | under | 4.25-usable |
|---|---|---|
| `14.16.27023` | VS2017 BuildTools | 🎯 **YES — and it is on 4.25's own preferred list** |
| `14.36.32532` | VS2022 Community | ⚠ not a 4.25 toolset |
| `14.38.33130` | VS2022 Community | ⚠ not a 4.25 toolset |
| `14.42.34433` | VS2022 Community | ⚠ not a 4.25 toolset |

**Windows 10 SDKs** — `10.0.17763.0` and `10.0.22621.0`. **Both complete**: `Include\um`,
`Include\ucrt`, `Lib\um\x64`, `Lib\ucrt\x64` all present for both. `NETFXSDK\4.8` present.

Also present, not relevant: `D:\UnrealToolchains\v23_clang-18.1.0-rockylinux8` (a Linux cross-toolchain).

### A.3 What 4.25's UBT will actually accept — read from source, not release notes

All citations `E:\EpicGames\UE_4.25\Engine\Source\Programs\UnrealBuildTool\Platform\Windows\UEBuildWindows.cs`.
✅ **UBT source ships with this launcher install** (`Engine/Source/Programs/UnrealBuildTool/` present), as
does the prebuilt `Engine/Binaries/DotNET/UnrealBuildTool.exe`.

1. 🚨 **`enum WindowsCompiler` (`:19`) STOPS AT `VisualStudio2019` (`:55`). There is no `VisualStudio2022`.**
   Command-line switches are `-2015`/`-2017`/`-2019` only (`:117-119`).
2. **VS discovery** (`FindVSInstallDirs`, `:946-1024`) enumerates every local instance through the
   `SetupConfiguration` COM API — **the same API `vswhere` uses, so Build Tools SKUs are found**. The
   only version filter is `:990-998`:
   - `VisualStudio2019` requires `Version >= 16` — **there is NO upper bound**;
   - `VisualStudio2017` requires `Version < 16`.
   ⇒ VS2017 BuildTools (15.9) is accepted as *2017*. 🚨 **And VS2022 (17.x) satisfies the *2019* filter.**
3. **Toolchain discovery** (`FindToolChainDirs`, `:1110-1132`) walks `<InstallDir>\VC\Tools\MSVC\*` and
   validates each via `IsValidToolChainDirDeep2017or2019`. `14.16.27023` under VS2017 BuildTools passes.
4. **Preferred toolchains** (`:618-621`) — the list literally contains
   `14.24.28315`, `14.22.27905`, `14.16.27023.2`, and **`14.16.27023`** with the comment *"fallback to
   VS2017 15.9 toolchain"*. 🎯 **The installed toolset is on 4.25's own preferred list.**
5. 🚨 **`GetDefaultCompiler` (`:816-890`) tests `HasCompiler(VisualStudio2019)` FIRST (`:864`).**
   Combined with (2), **the default on this machine resolves to VS2022's MSVC 14.42** — UE 4.25 compiled
   with a 2022 toolset, which is not a supported combination. **This is why pin #1 is mandatory.**
   ⚠ It fails *silently in selection* and loudly later in compilation, which is the worst shape.
6. **Blacklist:** only `14.23` is explicitly rejected (`:1266`, a known codegen bug). `14.16` is clean.
7. 🚨 **Windows SDK selection** — `PreferredWindowsSdkVersions` (`:627-630`) is
   **`10.0.18362.0` then `10.0.16299.0`. NEITHER IS INSTALLED.** `TryGetWindowsSdkDir` (`:1742-1746`)
   then falls through to `CachedWindowsSdkDirs.OrderBy(Key).Last()` — **the newest installed, i.e.
   `10.0.22621.0`**, a 2022-era SDK. **This is why pin #2 is mandatory**; `10.0.17763.0` is installed,
   complete, and contemporary with the 14.16 toolset.

### A.4 Registry and disk

- `HKCU\Software\Epic Games\Unreal Engine\Builds` — **exactly one entry**:
  `{B34F356C-4AE7-256A-F0E1-318A632BB902} → D:/UESource/UnrealEngine` (the 5.1 source build).
  ⚠ **A launcher-installed engine needs NO entry here** — `StylizedParisStreet.uproject` carries
  `"EngineAssociation": "4.25"`, a version string that resolves through `LauncherInstalled.dat`, not a
  GUID. ⇒ 🎯 **Phase 0 §E's warning that "a second engine means a second registry entry" DOES NOT APPLY
  to a launcher install.** No registry change is needed and none should be made.
- **Free space: `C:` 29.31 GB · `D:` 188.21 GB · `E:` 365.13 GB.**
  ⚠ `C:` is the tightest and is where MSVC/SDK live; nothing here needs to grow it.
  🎯 **Phase 0 §E budgeted "~150–250 GB for a source-built 4.25 engine". That cost is NOT incurred** —
  the engine is a launcher install already on disk. The only new disk is the host project's
  `Intermediate`/`Binaries`/DDC on `E:`, order tens of GB.

---

## B. PHASE 0'S UNKNOWNS, TURNED INTO MEASUREMENTS

Engine root for every citation: `E:\EpicGames\UE_4.25\Engine`.

### B.1 — D-1: the injector half

| # | question | MEASURED (stock 4.25) | PREDICTION (Europa) | vs Phase 0 |
|---|---|---|---|---|
| **D-1 q1** | `FWorldDelegates::OnWorldTickEnd` — exists? where broadcast? | 🚨 **ABSENT.** `Source/Runtime/Engine/Classes/Engine/World.h:3671-3679` declares only `OnWorldTickStart`, `OnWorldPreActorTick`, `OnWorldPostActorTick`. A recursive grep of **all** of `Engine/Source/Runtime` returns **ZERO hits** for `OnWorldTickEnd`. | ABSENT (a fork is overwhelmingly unlikely to *add* it) | 🔻 **WORSE — confirmed the bad branch** |
| **D-1 q1b** | 🎯 **the replacement anchor** | ✅ **`OnWorldPostActorTick` is m40's anchor position.** In `Private/LevelTick.cpp`: every tickable ticks at `FTickableGameObject::TickObjects` **`:1630`**, inside `if (bDoingActorTicks)` opened at **`:1545`**; that block closes at `:1688`; `OnWorldPostActorTick.Broadcast` is at **`:1694`**, inside `if (bDoingActorTicks)` at **`:1690`**. **Same guard, immediately after the tickable block, still inside `UWorld::Tick`, still pre-draw.** | HIGH confidence — `UWorld::Tick`'s spine is not usually forked | 🎯 **BETTER than the fallback Phase 0 feared** |
| **D-1 q2** | `OnWorldPreActorTick` / `OnWorldPostActorTick` | ✅ **BOTH PRESENT.** `World.h:3675-3676` / `:3678-3679`. Broadcast at `LevelTick.cpp:1515` / `:1694`. | PRESENT | ✅ resolved |
| **D-1 q3** | `UTickableWorldSubsystem` | 🚨 **ABSENT.** `Public/Subsystems/WorldSubsystem.h` is **24 lines** and declares only `UWorldSubsystem : public USubsystem` with a single `GetWorld()` override. ⚠ **And it has NO `DoesSupportWorldType`** — that virtual does not exist on 4.25's `UWorldSubsystem`. `USubsystem::ShouldCreateSubsystem(UObject* Outer)` **does** exist (`Public/Subsystems/Subsystem.h:61`). | ABSENT | ✅ confirmed, **plus one extra item Phase 0 missed** (§B.1a) |
| **D-1 q4** | LOD accessors | ✅ **ALL PRESENT, unchanged shape.** `USkinnedMeshComponent::GetNumLODs()` `SkinnedMeshComponent.h:673`; `SetForcedLOD(int32)` `:689`; `GetForcedLOD()` `:693`; `ForcedLodModel` deprecated since **4.24** (`:406`) exactly as on 5.1. `UStaticMeshComponent::ForcedLodModel` `StaticMeshComponent.h:176` (public, **not** deprecated); `SetForcedLodModel` `:381`. `UStaticMesh::GetNumLODs()` `StaticMesh.h:1073`. **Every symbol `AnomalyLod.cpp` uses (`:34-41,53-57,64-72,84-125`) exists.** | PRESENT | 🎯 **BETTER — "CHANGED (likely)" → measured unchanged; ZERO work** |
| **D-1 q5** | `UPrimitiveComponent::bVisibleInRayTracing` | ✅ **PRESENT AND UNGUARDED.** `Classes/Components/PrimitiveComponent.h:327`, a plain `UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = Rendering) uint8 bVisibleInRayTracing : 1;` — **no `#if RHI_RAYTRACING`**. | PRESENT | 🎯 **BETTER — "likely ABSENT-or-guarded" REFUTED** |

#### B.1a — 🆕 A NEW ITEM PHASE 0 DID NOT ENUMERATE

🚨 **4.25's `UWorldSubsystem` has no `DoesSupportWorldType`.** Our three subsystems restrict themselves to
Game+PIE through that override (CLAUDE.md records it as the M0 design and it has been load-bearing since).
On 4.25 the equivalent is **`USubsystem::ShouldCreateSubsystem(UObject* Outer)`** (`Subsystem.h:61`),
where `Outer` is the `UWorld` — so the same restriction is expressible, but it is **a different virtual
with a different signature**, not a rename.

⚠ **This is a genuine correctness item, not a compile item.** If it is simply dropped, the subsystems
would instantiate on editor-preview worlds — the exact thing the original design forbids. It is
**cheap** (one override per subsystem) but it **must be on the E1 list**, and it is not on Phase 0's.

### B.1b — D-1 extension: the `m45` hide surface

Our surface is `AnomalyHiddenClass.cpp:102-125` (8 saved flags, 7 written).

| flag | MEASURED (stock 4.25) |
|---|---|
| `bRenderInMainPass` | ✅ `PrimitiveComponent.h:331` |
| `bRenderInDepthPass` | ✅ `:335` |
| `CastShadow` (via `SetCastShadow`) | ✅ `:377` |
| 🚨 `bCastContactShadow` | 🚨 **ABSENT.** Recursive grep of **all** of `Source/Runtime/Engine` returns **ZERO hits**. |
| `bAffectDynamicIndirectLighting` | ✅ `:381` |
| `bAffectDistanceFieldLighting` | ✅ `:385` |
| `bVisibleInRayTracing` | ✅ `:327` |
| `bReceivesDecals` | ✅ `:339` |

⇒ **7 of 8 present; `bCastContactShadow` must be compiled out on 4.25.** Phase 0 graded the whole set
"PRESENT (likely)" — 🔻 **one flag is WORSE than predicted.** It is a *declaration-site* guard on a
save-and-restore field, so it stays inside the §B-3 architecture rule.

⚠ **It is not cosmetic.** Contact shadows are a *separate* shadowing path from `CastShadow`; on 4.25 they
are simply not per-primitive-controllable, so a hidden target on 4.25 **cannot have contact shadows
silenced**. Whether that leaves a visible residue is an **open pixel question for the 4.25 identity
arbiter**, not something to assume away. 📌 On 4.25 contact shadows are also off by default in more
configurations than on 5.1, which is why this may cost nothing in practice — **but that is a prediction.**

#### 🎯 `m45`'s LOAD-BEARING ASSUMPTION HOLDS ON 4.25 — MEASURED

The question: *is `bRenderCustomDepth` honoured for a primitive with `bRenderInMainPass = false`?*

**Answer: YES, on three independent links of the chain.**

1. `Private/PrimitiveSceneProxy.cpp:144` — the proxy initialises
   `bRenderCustomDepth(InComponent->bRenderCustomDepth)` **straight from the component**, and
   `PrimitiveSceneProxy.h:495` exposes `inline bool ShouldRenderCustomDepth() const { return bRenderCustomDepth; }`.
   **No reference to main-pass state.**
2. `Private/SceneVisibility.cpp:2069-2071` — `if (ViewRelevance.bRenderCustomDepth) { bHasCustomDepthPrimitives = true; }`,
   set **independently** of `bRenderInMainPass`; propagated at `:2443`.
3. The relevance gates are **disjunctions**, not conjunctions:
   `SceneVisibility.cpp:2248` — `&& (ViewRelevance.bRenderInMainPass || ViewRelevance.bRenderCustomDepth || ViewRelevance.bRenderInDepthPass)`;
   same pattern at `:2704` and `:2709`.
   And the pass itself gates on the proxy: `Private/CustomDepthRendering.cpp:58` —
   `if (PrimitiveSceneProxy->ShouldRenderCustomDepth())`.

⇒ **A primitive with `bRenderInMainPass=false, bRenderInDepthPass=false, bRenderCustomDepth=true` still
reaches the custom-depth pass on stock 4.25.** The `m45` mechanism transfers.

🎯 **And `G134` does not exist on 4.25** — there is no Nanite, so there is no class of geometry that is
structurally invisible to the custom-depth pass. **Every target is potentially maskable.** That is a real
improvement over the 5.1 host, and it was Phase 0's §D-3 q16 prediction, now resting on the stronger fact
that the only custom-depth exclusion path we ever measured was Nanite-specific.

### B.2 — D-2: the renderer

| # | question | MEASURED (stock 4.25) | vs Phase 0 |
|---|---|---|---|
| **q9** | which SVE hooks exist; is any handed an `FRDGBuilder`? | **`Runtime/Engine/Public/SceneViewExtension.h`** (⚠ **Engine**, not Renderer). Hooks: `SetupViewFamily` `:91` · `SetupView` `:96` · `SetupViewPoint` `:101` · `SetupViewProjectionMatrix` `:106` · `BeginRenderViewFamily` `:111` · `PreRenderViewFamily_RenderThread` `:116` · `PreRenderView_RenderThread` `:121` · `PostRenderBasePass_RenderThread` `:126` · `PostRenderViewFamily_RenderThread` `:131` · `PostRenderView_RenderThread` `:136` · `IsActiveThisFrame(FViewport*)` `:146`. 🚨 **EVERY render-thread hook takes `FRHICommandListImmediate&`. NONE takes an `FRDGBuilder`.** | see below |
| **q9a** | `SubscribeToPostProcessingPass` | 🚨 **ABSENT** (zero `.h` hits in `Runtime`) | ✅ confirmed |
| **q9b** | `PrePostProcessPass_RenderThread` | 🚨 **ABSENT** (zero `.h` hits in `Runtime`) | 🔻 **WORSE — Phase 0 §2 asserted 4.25 has this hook. It does not.** |
| **q9c** | `FSceneViewExtensionContext`, `EPostProcessingPass` | 🚨 **ABSENT** (zero hits each) | ✅ confirmed |
| **q10** | `FRHIGPUTextureReadback` / `FRHIGPUBufferReadback` | ✅ **BOTH PRESENT AND PUBLIC** — `Runtime/RHI/Public/RHIGPUReadback.h:92` and `:75` | 🎯 **BETTER — UNKNOWN → present in a PUBLIC header** |
| **q11** | how a 4.25 `.usf` reads CustomStencil / CustomDepth / SceneDepth | ✅ `Shaders/Private/SceneTexturesCommon.ush` **EXISTS**, and `SceneTexturesStruct.SceneDepthTexture` is used throughout it (`:43,:62,:72`). Custom stencil is read as `SceneTexturesStruct.CustomStencilTexture.Load(int3(PixelPos,0)) STENCIL_COMPONENT_SWIZZLE` (`Shaders/Private/DeferredShadingCommon.ush:635`, `:704`). ⚠ **`CalcSceneCustomStencil` is ABSENT** (zero hits across `Engine/Shaders/Private`). | 🎯 **BETTER — a ONE-LINE substitution, not a shader rewrite** |
| **q12** | `FPixelShaderUtils` / `FComputeShaderUtils` | ✅ **BOTH PRESENT AND PUBLIC.** `struct RENDERCORE_API FPixelShaderUtils` at `Runtime/RenderCore/Public/PixelShaderUtils.h:18`, **with `AddFullscreenPass` at `:80`**. `FComputeShaderUtils` at `Runtime/RenderCore/Public/RenderGraphUtils.h:99`. | 🎯 **BETTER — UNKNOWN → present, public, with the exact entry points we call** |
| **q13** | `FMaterialShaderMap::UsesSceneTexture` + `PPI_*` | ✅ **PRESENT, AND THE ENUM IS IDENTICAL.** `UsesSceneTexture(uint32 TexId)` at `Runtime/Engine/Public/MaterialShared.h:1228`. `ESceneTextureId` in `Runtime/Engine/Public/MaterialSceneTextureId.h` is **line-for-line identical between 4.25 and 5.1** — all 31 entries, same order, same line numbers; `PPI_CustomDepth` `:45`, `PPI_CustomStencil` `:69` in **both**. | 🎯 **BETTER — the `m41` host-PP preflight ports UNCHANGED** |
| **q6 (stock half only)** | does `Renderer/Private/SceneRendering.h` define `FViewInfo` with `ViewRect`, and does a launcher install ship private headers? | ✅ **YES to both.** The launcher install ships `Runtime/Renderer/Private/SceneRendering.h` and `ScenePrivate.h`. `class FViewInfo : public FSceneView` at `SceneRendering.h:860`, **`FIntRect ViewRect;` at `:865`**. ⚠ Also noted: `FSceneView::UnscaledViewRect` (`Engine/Public/SceneView.h:880`) and `UnconstrainedViewRect` (`:883`) are **public** — a partial escape hatch from the private include. | 🎯 **BETTER on the stock tree.** ⛔ **Says NOTHING about Europa — q6's real subject is the fork.** |

#### 🎯 THE BIGGEST SINGLE RESULT IN THE AUDIT

Phase 0 §2 posed the decisive question as: *"is RDG usable from an SVE hook on 4.25, or must the mask
pass be written against `FRHICommandListImmediate` with manual RTs? **This decides whether the mask pass
is a port or a rewrite.**"*

**MEASURED ANSWER: RDG IS USABLE. IT IS A PORT, NOT A REWRITE.**

- `class RENDERCORE_API FRDGBuilder` — `Runtime/RenderCore/Public/RenderGraphBuilder.h:16`
- **`FRDGBuilder(FRHICommandListImmediate& InRHICmdList);`** — `:20`
- **`void Execute();`** — `:167`
- and it holds `FRHICommandListImmediate& RHICmdList;` — `:173`

⇒ Inside any `*_RenderThread` SVE hook (all of which are handed `FRHICommandListImmediate&`), the 4.25
backend can construct its **own** builder, add the existing passes with `FPixelShaderUtils::AddFullscreenPass`
and `FComputeShaderUtils::AddPass`, enqueue the existing `FRHIGPUTextureReadback` / `FRHIGPUBufferReadback`,
and `Execute()`. **The shader, the parameter struct, the GPU reduce and the readback all survive.**

**What actually changes on 4.25 is three things, and they are bounded:**
1. **The hook** — `SubscribeToPostProcessingPass(Tonemap)` → a `_RenderThread` hook + our own builder.
2. **Where the textures come from** — no `FPostProcessMaterialInputs` / `FScreenPassTexture` handoff; the
   pass binds the scene-texture uniform buffer directly.
3. **One `.usf` line** — `AnomalyVisibleMask.usf:31` `CalcSceneCustomStencil((uint2)P)` →
   `SceneTexturesStruct.CustomStencilTexture.Load(int3(P,0)) STENCIL_COMPONENT_SWIZZLE`.
   ✅ **Lines `:38-39` need no change** — they already use `SceneTexturesStruct.CustomDepthTexture.Load` /
   `SceneTexturesStruct.SceneDepthTexture.Load`, which is 4.25's own idiom.

⚠ **Which hook** is a real design choice and is NOT settled here. Custom depth renders at
`Private/DeferredShadingRenderer.cpp:1826` (`RenderCustomDepthPassAtLocation(RHICmdList, 0)`, before
`RenderBasePass` at `:2014`) **or** at `:2149` (location 1, after base pass), selected by the engine's
custom-depth-order setting. ⇒ **`PostRenderBasePass_RenderThread` is NOT guaranteed to have custom depth
ready; `PostRenderViewFamily_RenderThread` / `PostRenderView_RenderThread` are.** This is an **E2**
decision that must be made against a measurement, not a guess.

🎯 **A side benefit worth recording: a pre-post-process hook runs at INTERNAL resolution**, so `m46`'s
output→internal rect mapping may be **unnecessary** on the 4.25 backend rather than harder. **Prediction,
not a measurement.**

### B.3 — D-3 q18 and the build system

| item | MEASURED (stock 4.25) | vs Phase 0 |
|---|---|---|
| **q18** `WebSocketNetworking` | ✅ **PRESENT** at `Engine/Plugins/Experimental/WebSocketNetworking/WebSocketNetworking.uplugin` — `Type: Runtime`, `EnabledByDefault: false`, `Installed: false`. **Ships BOTH source (`Source/`) and prebuilt binaries (`Binaries/Win64/UE4Editor-WebSocketNetworking.dll`, plus `.precompiled` markers).** | 🎯 **BETTER — UNKNOWN → present and usable** |
| **`.uplugin` `EngineVersion`** | ✅ **Leave it UNSET — that is the correct choice, not an oversight.** In 4.25 UBT it is an optional string (`PluginDescriptor.cs:115`), read at `:246`, and only *written back* when non-empty (`:365-367`). **There is no build-time compatibility enforcement.** Setting it would pin the plugin to ONE engine and break the two-engine goal. | ✅ resolved — **no change** |
| **TICKPIN fork probe** (`AnomalyCapture.Build.cs`) | ✅ **It degrades correctly and will NOT misfire on the stock tree.** Route A/B look for the literal `sUseFixedGameTickWithVariableRenderTick_Net`: a recursive grep of `Source/Runtime/Core` returns **ZERO hits**. Route C looks for directories named `FWNet*`/`Firewalk*` (`AnomalyCapture.Build.cs:108-113`): **zero matches** under `Engine/Plugins`. ⇒ probe reports **NOT FOUND**, `ANOMINJECT_FW_TICKPIN=0`, the pin compiles out, and the build log says so. | ✅ **resolved for the bench fixture.** ⛔ **Europa is a possibly-modified 4.25 — its probe echo must still be read on the first build, exactly as the runbook requires.** |
| `ERGBFormat::Gray` (m43's mask PNG) | ✅ **PRESENT** — `Runtime/ImageWrapper/Public/IImageWrapper.h:49`, `Gray = 2`, same ordinal as 5.1 | ✅ resolved |
| `OnBackBufferReadyToPresent` | 🎯 **PRESENT WITH A BYTE-IDENTICAL SIGNATURE.** 4.25 `SlateCore/Public/Rendering/SlateRenderer.h:261`: `DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBackBufferReadyToPresent, SWindow&, const FTexture2DRHIRef&);` — 5.1 `:299` is the **same line**. Accessor `:262` / `:300`. | 🎯 **The backbuffer capture path ports with ZERO signature change** |

---

## 4. STILL UNKNOWN FOR EUROPA — office PC only

⛔ **Nothing on this bench can settle any of these. They are listed here so that no reader mistakes §B's
green for Europa readiness.**

| # | question | why the bench cannot answer it |
|---|---|---|
| **D-2 q6** | **Is Europa's renderer MODIFIED, and where?** Does `SceneRendering.h` still define `FViewInfo` with `ViewRect`? | The stock tree answers only for stock. A fork can move, rename or restructure anything in `Renderer/Private`. **This is the single highest-risk item in the whole port** (`G100`). |
| **D-2 q7** | **Forward or deferred renderer?** | A forward renderer changes what custom depth means for the mask. Stock 4.25 defaults to deferred; a title chooses. |
| **D-2 q8** | 🚨 **Does the Europa title already use custom depth/stencil, and for what?** | **The highest-value question in the list.** We reserve stencil `200–254` and `m26`'s collision detectors assume nothing else writes there. Purely a property of the game's content and code. |
| **D-3 q14** | Screen percentage / dynamic resolution in use? | Runtime configuration of the title. |
| **D-3 q15** | TAA method, and is it temporally accumulating? | Decides whether `G228`'s ~9 % cross-run floor applies and therefore whether an AA-off identity arbiter is available on Europa. |
| **D-3 q17** | Is Europa's tick decoupled (the `ANOMINJECT_FW_TICKPIN` fork shape)? | ⚠ **Stock 4.25 says NO** (§B.3) — **that is a fact about stock, and Europa is precisely the "possibly modified" case the probe exists for.** Read its build-log echo on the first Europa build. |
| **§F** | Does Europa cook to `.utoc`/`.ucas` (IoStore) or `.pak` only? | Determines whether `verify_cooked_maps.ps1` works there. |

⚠ **One more that Phase 0 did not list and that §B has now made urgent:** if Europa's fork touched
`UWorld::Tick`, the `OnWorldPostActorTick`-after-`TickObjects` ordering that §B.1 rests on could differ.
**It is stock-verified and Europa-unverified**, and E1's alignment gate is what would catch it.

---

## C. HOST PROJECT AND HOW THE PLUGIN IS MOUNTED

### C.1 Converting `StylizedParisStreet` to a code project — the minimal set

**Measured current state** (read-only): `E:\Unreal Projects\StylizedParisStreet\` contains exactly
`Config\`, `Content\`, `StylizedParisStreet.png`, `StylizedParisStreet.uproject`. The `.uproject` is
5 lines: `FileVersion 3`, `EngineAssociation "4.25"`, empty `Category`/`Description`.
**No `Source/`, no `Plugins/`, no `Modules` block.** ⇒ BP-only, confirmed.

**Five files to create, plus one `.uproject` edit. Nothing else.** ⛔ **NOT created in this brief.**

| # | path | content |
|---|---|---|
| 1 | `Source\StylizedParisStreet.Target.cs` | `TargetType.Game`, `DefaultBuildSettings = BuildSettingsVersion.V2`, `ExtraModuleNames.Add("StylizedParisStreet")`, **plus the two toolchain pins from §A.0** |
| 2 | `Source\StylizedParisStreetEditor.Target.cs` | `TargetType.Editor`, same settings, same pins |
| 3 | `Source\StylizedParisStreet\StylizedParisStreet.Build.cs` | `PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs`; deps `Core`, `CoreUObject`, `Engine`, `InputCore` |
| 4 | `Source\StylizedParisStreet\StylizedParisStreet.h` | `#pragma once` + `#include "CoreMinimal.h"` |
| 5 | `Source\StylizedParisStreet\StylizedParisStreet.cpp` | `IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, StylizedParisStreet, "StylizedParisStreet");` |
| 6 | *(edit)* `StylizedParisStreet.uproject` | add `"Modules": [{ "Name": "StylizedParisStreet", "Type": "Runtime", "LoadingPhase": "Default" }]` and `"Plugins": [{ "Name": "WebSocketNetworking", "Enabled": true }]` |

⚠ **The `WebSocketNetworking` entry is only needed if `AnomalyControlServer` is left enabled.** Under the
§D ruling it is **stubbed on 4.25**, so E1 **omits it** and the dependency disappears.
📌 The plugin's own `.uplugin` declares that plugin dependency (`"Plugins": [{"Name":"WebSocketNetworking","Enabled":true}]`);
that is a plugin-level declaration and is discussed in §D.4.

⛔ **Per the brief, NOTHING was written into `E:\Unreal Projects\StylizedParisStreet`. This is a plan.**

### C.2 MOUNTING — 🎯 **I CONFIRM THE JUNCTION RULING, with evidence**

**The ruling:** the 4.25 project's `Plugins\AnomalyInjector` is a Windows directory junction
(`mklink /J`) to `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`. One codebase, one git repo,
two hosts, zero copy drift.

**Evidence it works:**

- 🎯 **UBT has NO reparse-point handling at all.** A recursive grep of the entire 4.25
  `Engine/Source/Programs/UnrealBuildTool` for `ReparsePoint` / `IsSymlink` / `FileAttributes.ReparsePoint`
  returns **ZERO hits**. UBT enumerates plugins with ordinary .NET `Directory` APIs, which traverse
  junctions transparently. ⇒ **plugin discovery through a junction is not a special case for UBT; it is
  invisible to it.**

**Evidence the outputs do NOT collide — this is the risk the brief asked me to verify, and it is measured
on both sides:**

| | 5.1 (measured in our tree) | 4.25 (measured in the install) |
|---|---|---|
| plugin DLLs | `Binaries\Win64\` + **`UnrealEditor-`**`<Module>.dll` | `Binaries\Win64\` + **`UE4Editor-`**`<Module>.dll` — confirmed on `Engine\Plugins\2D\Paper2D\Binaries\Win64\UE4Editor-Paper2D.dll` |
| editor intermediates | `Intermediate\Build\Win64\`**`UnrealEditor`**`\Development\<Module>\` and `…\Inc\<Module>\UHT\` | `Intermediate\Build\Win64\`**`UE4Editor`**`\Development\<Module>\` and `…\Inc\<Module>\` |
| game intermediates | `Intermediate\Build\Win64\<TargetName>\…` | `Intermediate\Build\Win64\`**`UE4`**`\{Development,Shipping}\<Module>\` |

⇒ 🎯 **NO PATH COLLIDES.** The two engines are separated by the engine-name path segment
(`UnrealEditor`/`UE4Editor`, `UE4`) and by the DLL filename prefix. They can share `Binaries\Win64\` and
`Intermediate\Build\Win64\` as sibling contents.
📌 Note 4.25 also emits an extra `Inc` layout without the `UHT` sub-level — that difference is *inside*
each engine's own subtree and is therefore harmless.

**`.gitignore` covers both — measured.** It ignores `Binaries/`, `Intermediate/`, `Saved/`,
`DerivedDataCache/` **wholesale, with no engine-specific patterns**, so 4.25 artifacts are ignored by the
same four lines that already ignore 5.1's. ✅ **No `.gitignore` change is needed.**

**Residual risks I accept, stated rather than discovered later:**
1. ⚠ **A hand-deletion of `Intermediate\` destroys BOTH engines' state.** UBT's own clean is
   engine-scoped and safe; a human `rm -rf Intermediate` is not. Operational note for the runbook.
2. ⚠ **A junction is invisible in `git status`** — the plugin repo has no idea it is mounted twice. That
   is the point, and it is also why builds must state which engine they ran under (the existing
   shared-tree rule already demands both directions of any switch be reported).
3. ⚠ **`E:` → `D:` cross-volume junction.** Junctions support this (they are not symlinks and need no
   developer mode / elevation for directories on local volumes). ⛔ **Unverified by execution here** —
   creating it is an owner errand (§7).

**The named fallback, and why I do NOT recommend it:** a second `git worktree` on `E:`. It is still one
repo and one history, so it satisfies "one codebase" — **but it introduces a second working tree that can
sit at a different commit**, which is exactly the drift the junction eliminates by construction. It also
collides with the existing **shared-tree rule** ("worktrees are for read-only inspection and doc-only
commits on other branches; **never for builds**"). ⇒ **Junction. The worktree is the fallback only if the
cross-volume junction fails at creation.**

### C.3 The TICK-PIN probe against a stock 4.25 tree

Answered in §B.3: **it returns NOT FOUND and compiles the pin out, with a build-log line saying so.**
Neither the symbol nor a fork-named directory exists. ⇒ Phase 0's "⚠ may fire or mis-fire" is
**resolved for the bench fixture** and **still open for Europa**, which is a possibly-modified 4.25 and is
the exact case the probe was built for.

⚠ **One cost to expect:** the probe's route-B content scan walks `Source/Runtime/Core`. On this launcher
install that tree is present, so the probe runs its normal ~sub-second path rather than degrading.

---

## D. MODULE SEPARABILITY

### D.1 The measured dependency map

Every edge below is from a `Build.cs` declaration **and** was cross-checked against real `#include`s.

```
AnomalyControlServer ──> AnomalyInjector          (Build.cs :18)
        │            └─> AnomalyCapture           (Build.cs :19)
        │            └─> WebSocketNetworking, Json (Build.cs :20-21)
AnomalyCapture ────────> AnomalyInjector          (Build.cs :335)
        │            └─> ImageWrapper, Json       (Build.cs :336-337)
        │            └─> AnomalyShaders           (Build.cs :352, NON-SHIPPING ONLY)
        │            └─> RenderCore/RHI/Renderer/Slate/SlateCore/ApplicationCore (:346-351, NON-SHIPPING ONLY)
        │            └─> Renderer/Private include path (:355, NON-SHIPPING ONLY)
        │            └─> UnrealEd                 (:364, bBuildEditor ONLY)
AnomalyShaders ────────> Core/Engine/RenderCore/RHI/CoreUObject/Projects   (no Anomaly deps)
AnomalyInjector ───────> Core/CoreUObject/Engine/InputCore/Foliage         (NO Anomaly deps)
```

**Direction: every edge points DOWN, toward `AnomalyInjector`. There are no cycles and no upward edges.**
📌 This is the inverted-provider architecture working as designed — `AnomalyCensusProvider.h` puts the
contract in the *lower* module precisely so this edge never has to exist.

### D.2 Can `AnomalyInjector` compile and run with the other three compiled out, without touching the `.uplugin`?

> ✅ **YES. Measured, not argued.**

- **Build.cs:** `AnomalyInjector.Build.cs:9-19` declares `Core`, `CoreUObject`, `Engine`, `InputCore`,
  `Foliage` and **no Anomaly module**.
- **Source:** a recursive grep of all of `Source\AnomalyInjector` for
  `#include "Anomaly(Capture|ControlServer|Shaders|Sve|Mask|Frame|Label|Async|RunLog|Preview)…`,
  `ANOMALYCAPTURE_API`, `ANOMALYCONTROLSERVER_API`, `ANOMALYSHADERS_API`, `UAnomalyCaptureSubsystem`,
  `UAnomalyControlServerSubsystem` returns ***ZERO cross-module references***.

⇒ `AnomalyInjector` is a **leaf**. Phase 0's step 1 ("reduce the `.uplugin` to `AnomalyInjector` only")
was aimed at a real requirement — and the brief is right that **it cannot stand as written**: there is one
`.uplugin` in one tree, and reducing it breaks the 5.1 host on the same branch.

### D.3 🎯 THE MECHANISM — I CONFIRM THE BRIEF'S SHAPE, AND REFINE IT TO ONE THAT IS ALREADY SHIPPED

**The brief's proposal:** keep the single 4-module `.uplugin`; in each non-injector `Build.cs`, branch on
`Target.Version.MajorVersion == 4` to compile that module as an **empty stub** that still loads and does
nothing.

**✅ The predicate is valid on both engines — measured.** `Target.Version` is a `ReadOnlyBuildVersion` on
`TargetRules` (4.25 UBT `Configuration/TargetRules.cs:193`, accessor `:1808`), and `ReadOnlyBuildVersion`
(`BuildVersion.cs:216`) exposes `MajorVersion` (`:272`) and `MinorVersion` (`:277`).
⇒ **`Target.Version.MajorVersion == 4` is a legal Build.cs expression on 4.25 and on 5.1.**

**🎯 But do NOT hand-write a new empty stub. The exact stub already exists, and every Shipping build
exercises it.**

`AnomalyCapture` and `AnomalyControlServer` each already have a *compiled-out* mode:

- `AnomalyCapture.Build.cs:340-360` — `if (Target.Configuration != Shipping)` adds `ANOMALY_CAPTURE=1`
  **plus all the render dependencies and the `Renderer/Private` include path**; `else` adds
  `ANOMALY_CAPTURE=0`.
- `AnomalyControlServer.Build.cs:24-30` — the same shape for `ANOMALY_CONTROL_SERVER`.
- **13 of the 32 files in `AnomalyCapture` are guarded at the very top of the file** by `#if ANOMALY_CAPTURE`
  (`AnomalyAsyncWriter.cpp`, `AnomalyCaptureLetterbox.cpp`, `AnomalyCaptureModule.cpp`, `AnomalyCensus.cpp`,
  `AnomalyFrameCapturer.cpp`, `AnomalyLabelWriter.cpp`, `AnomalyMaskMeasure.cpp`,
  `AnomalyMaskSceneViewExtension.cpp`, `AnomalyPreviewTee.cpp`, `AnomalySceneViewExtension.cpp`,
  `AnomalyStencilTag.cpp`, `AnomalySveCapturer.cpp`, `AnomalySveKeyRing.cpp`), so they become empty
  translation units.
- 🎯 **And `IMPLEMENT_MODULE` sits OUTSIDE the guard.** `AnomalyCaptureModule.cpp` is 24 lines; only the
  log *string* is `#if`-selected, and the `#else` branch logs
  **`"AnomalyCapture module started (compiled out: ANOMALY_CAPTURE=0)."`**;
  `IMPLEMENT_MODULE(FAnomalyCaptureModule, AnomalyCapture)` is line **24**, unguarded.
  `AnomalyControlServerModule.cpp` is byte-for-byte the same shape (`IMPLEMENT_MODULE` at `:24`).

⇒ **With `ANOMALY_CAPTURE=0`, the module still loads, still implements `IModuleInterface`, announces in
the log that it is compiled out, and does nothing. That is precisely "an empty stub that still loads",
achieved by a switch with years of Shipping evidence behind it.**

**THE RECOMMENDED CHANGE — one condition per module, at a declaration site:**

```csharp
// AnomalyCapture.Build.cs  (was: Target.Configuration != Shipping)
bool bEnableCapture = Target.Configuration != UnrealTargetConfiguration.Shipping
                   && Target.Version.MajorVersion >= 5;
```

…and the identical shape in `AnomalyControlServer.Build.cs`. **Nothing else changes**: not the
`.uplugin`, not a single `.cpp`, not the 5.1 build (on 5.1 `MajorVersion` is 5, so the expression is
exactly what it is today — **byte-inert on the 5.1 side, which is itself a gate**).

⚖ **Why this is strictly better than a new stub:** a hand-written second compile-out path would be an
**untested** way to empty a module, sitting next to a **tested** one. Two mechanisms means one can rot
while the other covers for it — the same reasoning that refused a belt-and-braces string match beside the
`Foliage` type reference in the Invariants. **One switch, already proven, reused.**

✅ **And it stays inside the locked architecture rule (Phase 0 §B-3):** this is a **module-availability
decision at a declaration site**, not an `#if` inside a gate-bearing algorithm. No census, no
`ArmTargetMaskOwn`, no veto path acquires a preprocessor branch.

**`AnomalyShaders` is the one module with no such gate**, and it needs none for E1:
its dependencies (`Core`, `Engine`, `RenderCore`, `RHI`, `CoreUObject`, `Projects`) all exist on 4.25 and
it declares no Anomaly dependency. **But it declares two global shaders**
(`AnomalyVisibleMaskShader.cpp:5` → `/Plugin/AnomalyInjector/Private/AnomalyVisibleMask.usf`;
`AnomalyMaskReduceShader.cpp:5` → `AnomalyMaskReduce.usf`), and `AnomalyVisibleMask.usf:31` uses the
**absent** `CalcSceneCustomStencil` (§B.2 q11).
🚨 **`G129`: a missing/failed global shader is FATAL at engine init, not a warning** — and it fires with
every feature switch off, because `IMPLEMENT_GLOBAL_SHADER` is not gated by any cvar.
⇒ **E1 must give `AnomalyShaders` the same `MajorVersion >= 5` treatment** (stub the module, do not
declare the shaders) rather than attempt the `.usf` port. **The shader port belongs to E2, where it can be
gated.** ⛔ Leaving `AnomalyShaders` live on 4.25 would put a fatal boot failure in the middle of E1.

### D.4 What the injector loses when capture is stubbed — so E1-G6 is a code fact, not a hope

With `ANOMALY_CAPTURE=0` / `ANOMALY_CONTROL_SERVER=0` / `AnomalyShaders` stubbed:

| lost | consequence for E1 |
|---|---|
| `UAnomalyCaptureSubsystem` and its whole `Tick`/phase FSM | **No capture run can be started.** `IAI.Capture.*` commands are registered inside `AnomalyCapture` and simply do not exist. |
| `AnomalyLabelWriter`, `AnomalyAsyncWriter` | **`labels.jsonl`, `annotation.json`, `run_summary.json`, `run.json` are unreachable** — the code that writes them is an empty translation unit. |
| `AnomalyCensus` + the registered census provider | The injector's `AnomalyCensusProvider` contract is **never registered**, so selection falls back to the pre-census picker — the path `P-C7` measured as byte-identical. |
| the `m40`/`m44` `OnWorldTickEnd` anchor and `mask_state` | Both live in `AnomalyCaptureSubsystem.cpp`. **Not compiled.** |
| `target_mask/*.png`, `mask_map.json` | Unreachable. |
| the WS dashboard | Unreachable; drive by console, which the client docs already describe. |

⇒ 🎯 **E1-G6 ("NO artifact is produced and none is claimed") is enforced BY CONSTRUCTION: the writers are
not in the binary.** That is a statement about code, verifiable by `ANOMALY_CAPTURE=0` appearing in the
build log and by the module's own startup line — **not a hope, and not a discipline anyone has to
remember.**

⚠ **What the injector KEEPS, and it is everything E1 needs:** all 9 anomalies, the selector, the
auto-injector and its seeded draw, `AnomalyViewport` (frustum + occlusion + coverage + poll radius),
targeting, `AnomalyLod`, the hidden-class registry, and **45 `IAI.*` console commands** — measured by
enumerating every `TEXT("IAI.…")` literal in `Source\AnomalyInjector`, including `IAI.ListActors`,
`IAI.ListAnomalies`, `IAI.DumpCatalog`, `IAI.Apply`, `IAI.Revert`, `IAI.RevertAll`, `IAI.DumpActive`,
`IAI.Auto.FireOnce`, `IAI.Auto.Seed`, `IAI.Auto.Step`, `IAI.DumpVisible`, and
`IAI.Bench.SynthTickOrder`.

🎯 **`IAI.Bench.SynthTickOrder` survives E1** — it is injector-side and uses `OnWorldPreActorTick`, which
**exists on 4.25** (§B.1 D-1 q2). ⇒ **both tick orders remain testable on 4.25**, so the standing
both-orders rule is not weakened by the port.

---

## E. THE E1 PLAN

**Branch `europa-e1` off `master`. Conventional commits `feat(europa): …`. ⛔ NO TAG** — tags stay in the
pending office batch (`m31 → m33 → m34 → m35 → m36 → m37 → m38 → m40 → m41 → m43 → m44 → m45 → m46`).

**Goal, unchanged from Phase 0 §C:** `AnomalyInjector` alone compiles and runs on a 4.25 host, and one
anomaly fires. Capture, mask, census and control server are out of scope and **compiled out**.

### E.1 Ordered steps

| # | step | files | notes |
|---|---|---|---|
| **1** | **Host project conversion** | the 5 files + 1 edit in §C.1, **in `E:\Unreal Projects\StylizedParisStreet`** | ⛔ **OUTSIDE the plugin repo — not versioned here.** Carries the two §A.0 toolchain pins. |
| **2** | **Mount** | `mklink /J "E:\Unreal Projects\StylizedParisStreet\Plugins\AnomalyInjector" "D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector"` | owner errand (§7); no repo change |
| **3** | **Module availability switch** | `AnomalyCapture.Build.cs`, `AnomalyControlServer.Build.cs`, `AnomalyShaders.Build.cs` | the §D.3 one-condition change ×3. **Byte-inert on 5.1.** |
| **4** | **Subsystem base swap** | `AnomalyInjectorSubsystem.h:12`, `AnomalyAutoInjectorSubsystem.h:35`, `AnomalySelectorSubsystem.h:12` | `UTickableWorldSubsystem` → `UWorldSubsystem` + `FTickableGameObject`, behind one shim header. **Declaration sites only.** |
| **5** | 🆕 **World-type restriction** | same 3 classes | `DoesSupportWorldType` (5.x) ↔ `ShouldCreateSubsystem(Outer)` (4.25). **§B.1a — not on Phase 0's list and it is a correctness item, not a compile item.** |
| **6** | **Tick anchor** | `AnomalyInjectorSubsystem.cpp:166,177` region | 5.1 keeps `OnWorldTickEnd`; 4.25 binds `OnWorldPostActorTick`. **Binding site only.** ⛔ **Do NOT switch 5.1 to the shared anchor** — that would re-open `P9`, which was closed on Bates and validated at `m40`. |
| **7** | **Drop `bCastContactShadow` on 4.25** | `AnomalyHiddenClass.cpp:105,120` (+ the `FSavedPrimitive` field) | §B.1b. Declaration-site guard. |
| **8** | **LOD accessors** | `AnomalyLod.cpp` | 🎯 **NO CHANGE NEEDED — measured present** (§B.1 D-1 q4). Step retained only so the audit records it was checked. |
| **9** | **`bVisibleInRayTracing`** | `AnomalyHiddenClass.cpp:108,124` | 🎯 **NO CHANGE NEEDED — measured present and unguarded.** |

📌 Steps 8 and 9 were Phase 0 plan items 4 and 5. **Both are now measured no-ops.** The work Phase 0
predicted has been replaced by *different*, smaller work (steps 5 and 7).

### E.2 GATES — `E1-G1..G6`, adapted to this host

⚠ **`<Actor>` below is a placeholder resolved by `E1-G0` before any gate runs.**

| gate | predicate | exact command / action | who runs it |
|---|---|---|---|
| **E1-G0** *(precondition, new)* | An actor name in the ParisStreet level is obtained and recorded | `StylizedParisStreet.exe ... -ExecCmds="IAI.ListActors"` (editor: `-game`), read from the log. 🎯 **`IAI.ListActors` is defined in `AnomalyInjector`** so it survives the stub — measured in §D.4. ⚠ `GetActorLabel()` is editor-only, so a packaged run prints `(no-label)`; **match on `GetName()`**, which is what our targeting uses anyway. | **me, headless** |
| **E1-G1** | `AnomalyInjector` compiles on 4.25, **editor AND game target, both exit 0** (`G221`) | Editor: `E:\EpicGames\UE_4.25\Engine\Build\BatchFiles\Build.bat StylizedParisStreetEditor Win64 Development -Project="E:\Unreal Projects\StylizedParisStreet\StylizedParisStreet.uproject" -WaitMutex`<br>Game: same with `StylizedParisStreet` as the target name. ⚠ **The editor target is the one that can see a missing `MODULE_API` export** (`G221`) — the game target is monolithic and cannot. **Both, always.** | **me, headless** |
| **E1-G1b** *(new, 5.1 side)* | 🚨 **The 5.1 host still builds, both targets, exit 0, and `Source/` behaviour is unchanged** | The existing 5.1 both-targets build. **This is the "one branch, two engines" gate** — without it, step 3 could silently break `master`'s engine. | **me, headless** |
| **E1-G2** | `IAI.ListAnomalies` prints **9** ids, sorted | `-ExecCmds="IAI.ListAnomalies"`, count the `LogAnomaly` lines | **me, headless** |
| **E1-G3** | `IAI.Apply missing_object <Actor>` hides it, `IAI.Revert` restores it, **verified by eye**, and `IAI.DumpActive` reads 0 after | `-ExecCmds="IAI.ListActors, IAI.Apply missing_object =<Actor>, IAI.DumpActive"` then revert. ⚠ **The state half is headless; the EYE half is not.** | **state: me. Eye: OWNER** |
| **E1-G4** | 🚨 **PROVE-IT-CAN-FAIL:** `IAI.Apply missing_object =NoSuchActor_ZZZ` reports the AMB-2 zero-match refusal and changes **nothing** | `-ExecCmds="IAI.Apply missing_object =NoSuchActor_ZZZ, IAI.DumpActive"`. **Pass = the zero-match line is present AND `IAI.DumpActive` reads 0.** *A bring-up where everything "works" is the shape that hides a no-op.* | **me, headless** |
| **E1-G5** | The auto-injector fires once and auto-reverts, with the seeded draw **reproducible across two runs at one seed** | Run twice: `-ExecCmds="IAI.Auto.Seed 4242, IAI.Auto.FireOnce, IAI.Auto.Status"`. **Pass = the two runs name the same target and the same anomaly id, and `IAI.DumpActive` reaches 0 after the hold.** | **me, headless** |
| **E1-G6** | ⛔ **NO artifact is produced and none is claimed** | Two-part, and part (b) is the strong one: **(a)** the session output directory contains no `labels.jsonl` / `annotation.json` / `run_summary.json` / `target_mask/`; **(b)** 🎯 **the build log shows `ANOMALY_CAPTURE=0` and the runtime log shows `"AnomalyCapture module started (compiled out: ANOMALY_CAPTURE=0)."`** — §D.4 makes this a **code fact**. | **me, headless** |

⚠ **Explicitly NOT in E1** (unchanged from Phase 0): any pixel gate, any mask, any census, any identity
arbiter, any alignment gate. **Step 6 changes the label anchor on 4.25 and E1 does NOT certify it** —
there are no labels in E1 to certify it with. 🚨 **Re-running `m40`'s alignment gate on 4.25 is an E2
entry condition and must be written into E2's pre-declaration, not discovered there.**

### E.2a 🚨 E2 ENTRY CONDITIONS — PRE-DECLARED HERE, AS GATES WITH PREDICATES

**Ruled 2026-09-03 (session 070 brief 02, ruling 4). These are not notes. E2 does not begin until each is
either PASSED or explicitly waived in writing, and none may be graded after the fact.**

| gate | predicate | why it exists |
|---|---|---|
| **E2-E1 — the anchor re-gate** | 🚨 **`m40`'s label-alignment gate is re-run ON 4.25 and passes there, in BOTH tick orders, BEFORE any label or capture output is graded on that engine.** Pass = for every counted event, the first frame labelled positive equals the first frame whose picture differs, delta **0**, on both the native order and `IAI.Bench.SynthTickOrder`; with the in-leg positive control (a deliberate ±1 shift) reading SHIFTED so the instrument is proven able to fail. | 5.1 anchors the per-frame label at `FWorldDelegates::OnWorldTickEnd`; **4.25 has no such delegate** and must bind `OnWorldPostActorTick` instead. The two occupy the same *structural* position — `LevelTick.cpp:1694` sits after `TickObjects` at `:1630` under the same `bDoingActorTicks` guard (`:1545`/`:1690`) — but **that is an argument from engine source, not a measurement of our labels.** `P9` is the standing proof that an anchor argument which "obviously holds" can be wrong on a host we did not measure. |
| **E2-E2 — the contact-shadow residue** | On 4.25, a hidden-class target is compared against its 5.1 counterpart on the same fixture and **either** shows no contact-shadow residue, **or** the residue is measured and stated as a named 4.25 limitation. | `bCastContactShadow` is **absent** from 4.25's `UPrimitiveComponent` (measured), so `m45`'s hide cannot silence that shadowing path there. ⛔ Do not assume it is off by default — measure it. |
| **E2-E3 — the artifact contract** | For one fixture and one seed, `labels.jsonl` / `annotation.json` / `mask_map.json` are compared across the two engines and **every field except the declared run-unique set is identical**. | §B-4's engine-independent artifact contract is the cheapest cross-engine gate and the strongest available proof the port did not change *meaning* rather than just compiling. |

⛔ **E2-E1 is the blocking one.** Until it passes, **no label produced on 4.25 may be shipped, quoted, or
used to grade anything** — an unvalidated anchor is exactly the dataset-poisoning shape `P3` and `P9` were.

### E.3 Residual risks

**Accepted — I would start E1 with these open:**
1. The `m40` anchor change on 4.25 is **structurally argued from `LevelTick.cpp` and NOT gate-verified**.
   Acceptable because E1 produces no labels, so nothing can be mis-labelled by it.
2. `bCastContactShadow`'s absence leaves one shadowing path unsilenced on 4.25. Acceptable in E1 because
   E1 makes **no pixel claim at all**.
3. The bench fixture is stock, so every §B result is Europa-**predictive** only. Acceptable — that is the
   declared purpose of a bench fixture.
4. The `10.0.22621.0` SDK sits on the machine and would be picked by default. Acceptable **because the pin
   is explicit and in the Target.cs**, where a build cannot silently lose it.

**NOT accepted — I would stop rather than proceed:**
1. ⛔ **Leaving `AnomalyShaders` live on 4.25.** `G129` makes a missing global shader a **fatal boot
   failure**, immune to every feature switch. It must be stubbed in step 3 or E1 cannot start.
2. ⛔ **Switching the 5.1 host to `OnWorldPostActorTick` to get a single shared anchor.** It would re-open
   `P9` on the only host where it was ever closed and validated. **The `#if` at the binding site is the
   cheaper and safer trade.**
3. ⛔ **Any `#if ENGINE_MAJOR_VERSION` inside a gate-bearing algorithm** (`G224`'s shape). Every branch in
   this plan is at a declaration, binding or Build.cs site — that is a design constraint, not an accident.
4. ⛔ **Building without the §A.0 pins.** The default resolves to VS2022's 14.42 toolset; a failure there
   would be misread as "the port does not work" when it is a toolchain selection artifact.

---

## 7. OWNER ERRANDS — only a human at the machine can do these

1. 🚨 **Create the junction** (one elevated command, cross-volume `E:` → `D:`):
   `mklink /J "E:\Unreal Projects\StylizedParisStreet\Plugins\AnomalyInjector" "D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector"`
   (create `…\StylizedParisStreet\Plugins\` first). **Report whether it succeeded** — §C.2 residual 3 is
   the only unverified link in the mounting ruling.
2. **`E1-G3`'s EYE half** — confirm `missing_object` visibly removes the actor and `IAI.Revert` visibly
   restores it, in the ParisStreet level. Every other gate is headless.
3. *(optional, only if E2 keeps the control server)* Confirm the `WebSocketNetworking` plugin can be
   enabled in a 4.25 code project. Not needed for E1 — it is stubbed.

⛔ **Nothing else needs a human.** In particular: **no engine install, no VS component install, and no
registry edit** (§A.4).

---

## 8. WHAT CHANGED IN PHASE 0

`PHASE0-port-surface-4.25.md` is updated **in place** for the rows §B measured, each marked
**`✅ MEASURED (stock 4.25)`** with its engine `file:line`, and its grade legend gains a fifth row for that
marker. **Every original prediction is kept beside its measurement** so the audit can be scored:
five rows got better, three got worse, and one new item (§B.1a) was found that Phase 0 had not enumerated.
