# Gotchas (append-only)

Non-obvious lessons. Newest at the bottom. Never delete entries — supersede them.

---

### G1 — StackOBot is Blueprint-only but has stale `StackOBot` module binaries
The shipped project has no `Source/`, but `Binaries\Win64\UnrealEditor-StackOBot.dll`,
`StackOBotEditor.target`, and `UnrealEditor.modules` exist with no matching source. Launching
as-is makes the editor try to load a missing module → "modules missing, rebuild?" prompt.
**Fix:** delete project-root `Binaries/` + `Intermediate/` (not `Saved/`), then add the
Route A game module and build. (Discovered 2026-06-08.)

### G2 — `GetActorLabel()` is editor-only
It is compiled out (`WITH_EDITOR`) and absent in cooked builds. We print the label in
`IAI.ListActors` guarded by `#if WITH_EDITOR`, but **matching must never use the label** —
`IAI.HideActor` matches actor Name or Class only, keeping it forward-compatible with cooked
builds.

### G3 — `SetActorHiddenInGame` hides in game/PIE, not the editor viewport
The anomaly is only visible when playing (PIE or standalone), which is exactly the data-gen
context. Don't expect the editing viewport to change when toggling hidden state.

### G4 — The Bot is runtime-spawned
StackOBot spawns the player Bot at play time, so it is not in the world at PIE start.
`IAI.HideActor Bot` only matches after the Bot exists. For a deterministic smoke test, hide a
**persistent level prop** (a `StaticMeshActor` in `MainWorld`) instead.

### G5 — `UTickableWorldSubsystem::GetStatId()` is pure-virtual
It must be overridden or the subsystem won't compile. Implement with
`RETURN_QUICK_DECLARE_CYCLE_STAT(UAnomalyInjectorSubsystem, STATGROUP_Tickables);`.

### G6 — Plugin enablement via `EnabledByDefault: true` is project-plugin scoped
We enable the plugin through `"EnabledByDefault": true` in `AnomalyInjector.uplugin` so the
`.uproject` needs no `Plugins[]` entry. This is fine while it lives in a **project's**
`Plugins/` folder. **Revisit if this ever becomes an engine plugin** (installed under the
engine's `Plugins/`): `EnabledByDefault` would then auto-enable it for *every* project on that
engine, which is almost certainly not what we want.

### G7 — Restrict the subsystem to Game + PIE worlds
`DoesSupportWorldType` returns true only for `EWorldType::Game` and `EWorldType::PIE`, so the
subsystem never instantiates or ticks in the editor preview/editing world. Consequence: the
`IAI.*` console commands resolve a null subsystem when run outside a game world — they
null-guard and log a clear warning rather than crashing.

### G8 — MCP bridge: UnrealMCPython is a 5.6/5.7 plugin; needs a local patch to build on 5.4
To drive functional smoke tests via the `unreal-mcpython` MCP, the `UnrealMCPython` editor
plugin (from the RatBurglar project, descriptor `EngineVersion 5.7.0`) was copied into
`StackOBot/Plugins/unreal-mcp/` (host tooling — NOT part of the AnomalyInjector plugin repo). Building it
against source **UE 5.4.4** surfaced one linker error:
`LNK2019 UBehaviorTreeGraphNode_SimpleParallel::GetPrivateStaticClass` (only 1 unresolved).
Cause: in 5.4 that editor graph-node class is `UCLASS()` with **no `BEHAVIORTREEEDITOR_API`**,
so it isn't exported and can't be linked from another module (it was exported in >= 5.6).
**Fix:** in `Source/UnrealMCPython/Private/MCPythonHelper.cpp`, route the SimpleParallel case
through the generic `UBehaviorTreeGraphNode_Composite` path (SimpleParallel runtime nodes are
composites) and drop the unused `bIsSimpleParallel` local (else `-WarningsAsErrors` trips).
Also set the copied `.uplugin` `"Installed": false` and remove its `EngineVersion` so 5.4
doesn't reject it on load. After this, `StackOBotEditor` builds clean and the MCP server
starts on `127.0.0.1:12029` when the editor launches. (Discovered 2026-06-09.)
**If the engine is ever upgraded to >= 5.6, revert the MCPythonHelper patch** to restore the
dedicated SimpleParallel graph node. This is a local patch to a third-party plugin copy; the
RatBurglar original is untouched.

**Update (M2.6, UE 5.1, 2026-06-10) — full diagnosis + the 5.1 fix (severed the BehaviorTree dep).**
The bridge (GenOrca **UnrealMCPython**, GitHub `GenOrca/unreal-mcp`) **officially targets UE 5.6+**.
On 5.1 it does **not** build as-is. Working through it: first `BehaviorTreeEditor.h` and the
`BehaviorTreeGraphNode*.h` / `EdGraphSchema_BehaviorTree.h` headers aren't on the include path
(they live in the engine module's `Private/` + `Classes/`, not `Public/`); adding those include
paths cleared that, then `MCPythonTcpServer.cpp` needed an explicit `#include "Async/Async.h"`
(`AsyncTask` was transitively included on 5.4). After both, linking failed with **7 `LNK2019`
unresolved externals** — `UBehaviorTreeGraphNode`, `UBehaviorTreeGraph`, and `_Root/_Task/_Decorator/
_Service/_SubtreeTask`: these `BehaviorTreeEditor` UCLASSes have **no `BEHAVIORTREEEDITOR_API`
export macro before 5.6**, so they're unexported and unlinkable from another module. This is the
original G8 SimpleParallel issue **generalized to 7 symbols** — a hard 5.6→5.1 back-port wall that
would need either an engine-source export patch (rejected: per-sync tax on the shared engine that
also builds the real target games) or severing the feature.
**Resolution — severed the `BehaviorTreeEditor` dependency (route a):** the bridge's BT-graph
*authoring* features are unused by this project (we need console exec, Output-Log read, actor/
component state reads). The coupling was isolated to **one Python router + three C++ functions**.
The sever: dropped `"BehaviorTreeEditor"` from `UnrealMCPython.Build.cs`; removed the editor-graph
`#include`s from `MCPythonHelper.cpp`; `#if 0`-compiled-out `GetSelectedBTNodes`,
`CreateBTGraphNodeRecursive`, and `BuildBehaviorTree`; replaced the two public UFUNCTIONs with
"unsupported on 5.1" JSON stubs; and marked the matching Python tools unsupported in
`behavior_tree_router.py`. Kept the `Async/Async.h` fix. **Dropped 2 tools:** `build_behavior_tree`,
`get_selected_bt_nodes`. **Retained the 4 runtime-BT read tools** (`get_behavior_tree_structure`,
`get_bt_node_details`, `list_bt_node_classes`, `set_blackboard_to_behavior_tree` — they use AIModule
runtime types, which are exported). After the sever the bridge **compiles, links, and loads clean
on 5.1**; `LogMCPython: TCP server started at 127.0.0.1:12029`; it drove the full M2.5 re-gate.
**To restore BT authoring on >= 5.6:** re-add the dep + includes and delete the stubs + the
`#if 0 ... #endif` guard. The bridge folder is **not under version control** (host tooling); these
edits are recorded here, not committed to the plugin repo. (M2.6, 2026-06-10.)
**Forward decision (flagged, not a TODO):** keeping the bridge unversioned holds only while it stays
*regenerable third-party tooling* (known upstream + this exact recipe). When the capture/labeling/replay-
harness milestone starts adding **bespoke C++ to the bridge** (e.g. GPU buffer / object-mask readback
UFUNCTIONs), the bridge becomes **original project work** — at that point revisit versioning: `git init`
the **host root** `D:\IntrusiveAnomalies\StackOBot` as a single host-substrate repo (scaffolding +
`.uproject` + bridge), with a `.gitignore` for `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`.
That C++ addition is the natural trigger; until then, docs-as-source-of-truth holds. (Flagged M2.6, 2026-06-10.)

### G9 — `TUniquePtr<IAnomaly>` member in a UCLASS needs an out-of-line destructor (M1)
The subsystem owns `TMap<FName, TUniquePtr<IAnomaly>>`. The `TUniquePtr` deleter needs the
**complete** `IAnomaly` type at the point the map is destroyed. Declare the destructor in the
header (`virtual ~UAnomalyInjectorSubsystem();` — no `override`; destructors can't be marked
`override`) and define it `= default` in the `.cpp`, which `#include`s the concrete anomaly headers.
Without this you get incomplete-type errors at the implicitly-generated destructor. (2026-06-09.)

### G10 — UBT does not auto-add `Private/` subfolders to the include path (M1)
UnrealBuildTool puts only the module's `Public/` and `Private/` roots on the include path, **not**
subfolders. The concrete anomalies live in `Private/Anomalies/`, so every include of them — in their
own `.cpp` and in the subsystem `.cpp` — must be path-relative from `Private/`:
`#include "Anomalies/Anomaly_Blinking.h"`. Public headers (`IAnomaly.h`, `AnomalyTargeting.h`)
include bare. (2026-06-09.)

### G11 — `SetGlobalTimeDilation` is clamped by WorldSettings (M1)
`UGameplayStatics::SetGlobalTimeDilation` is clamped to `AWorldSettings` `MinGlobalTimeDilation` /
`MaxGlobalTimeDilation` (defaults ~0.0001 .. 20). Extreme scales are silently clamped, so the
`time_dilation` anomaly reads the value back after setting and warns if it differs from the request.
Revert restores the **captured pre-Apply baseline** (AMB-3 ruling — overrides the brief's literal
"set back to 1.0"), so a non-1.0 game baseline is preserved; it falls back to 1.0 only if nothing
was captured. (2026-06-09.)

### G12 — Actor-scoped anomalies share the single `bHidden` flag (last-writer-wins) (M1)
`missing_object` and `blinking` both drive `SetActorHiddenInGame` on their targets. If two such
anomalies target the **same** actor, they fight over one boolean: **last-writer-wins**. This is
acceptable for M1 because the terminal state after `RevertAll` is always visible (every hide-revert
ends in `SetActorHiddenInGame(false)`); only intermediate frames during deliberate concurrent use
are wrong, and M1's test plan applies one actor anomaly at a time.
**Revisit when we inject simultaneous/compound anomalies** (a likely future need for richer training
data): the fix is a **subsystem-level "hidden-by" coordinator** (ref-count / owner-set per actor),
which is addable **without touching the `IAnomaly` interface**. Flagged, not built. (2026-06-09.)
**Superseded for auto-injection v1 (m6):** the auto-injector sidesteps this entirely with a
**one-anomaly-per-actor** scheduler invariant (G30) — never two anomalies on one actor, so the shared
`bHidden` (and forced-LOD) are never contended on the auto path. The coordinator above is still the
correct path for **deliberate compound/stacked** same-actor anomalies (the deferred compound milestone),
just no longer the *first* need. (Note added m6, 2026-06-19.)

### G13 — `r.SetNearClipPlane` is a console COMMAND, not a console VARIABLE (M2)
The near clip plane has **no** `IConsoleVariable`. In 5.4 source, `r.SetNearClipPlane` is an
`FAutoConsoleCommand` (`UnrealEngine.cpp`) whose handler calls `SetNearClipPlaneGlobals(max(v, 1.0))`;
the state lives in the **CORE global `GNearClippingPlane`** (`CoreGlobals.h`, default 10), mirrored to
the render thread by RenderCore's `SetNearClipPlaneGlobals`. So `IConsoleManager::FindConsoleVariable("r.SetNearClipPlane")`
returns **null** — the briefed "FindConsoleVariable → GetString/Set" cvar mechanism cannot drive it.
This is why the generic cvar helper (AnomalyCvar / A2) was **deferred** (its sole would-be M2 consumer can't
use it; it has zero real consumers, failing the ≥2-consumers bar). `camera_clipping` instead captures the
baseline by reading `GNearClippingPlane` (Core, free) and applies/reverts via the `r.SetNearClipPlane`
**console command** (`GEngine->Exec`, Engine) — **no `RenderCore` dependency**, and the command path
correctly syncs the render-thread copy. The command clamps to `>= 1`, so restoring a sub-1 baseline would
be clamped (default 10 round-trips cleanly). AnomalyCvar lands with its first genuine `IConsoleVariable`
anomaly (post-process / scalability milestone). (2026-06-09.)

### G14 — Runtime light mutation is only visible on Movable (and partially Stationary) lights (M2)
`lighting_mismatch` mutates `ULightComponent` state (intensity / color / visibility / cast-shadow). For
**Static/baked** lights the component property changes (the state-read gate passes) but the **rendered
image does not** — the lighting is baked into lightmaps. Movable lights change fully at runtime;
Stationary change partially (direct lighting dynamic, indirect baked). So the visual gate needs a
**Movable** light target. Confirm a target's mobility live via `Light->Mobility == EComponentMobility::Movable`
(MainWorld byte-scan suggested ~2 movable light actors exist; try the DirectionalLight first for the
biggest visible effect). If none exists, add a Movable PointLight to a test sublevel, or accept
state-only validation. (2026-06-09.)

### G15 — `lod_corruption` only shows a visible change on multi-LOD meshes; stock MainWorld has no clean visual target (M2)
Forcing a LOD does nothing visible if the target `UStaticMesh` ships a **single runtime LOD** — the
state-read gate still passes (`ForcedLodModel` set + logged), but the image is unchanged.
**Verified live in PIE MainWorld (2026-06-09)** — supersedes the planning-time byte-scan guess (`Boulder`):
- Regular static props are matchable by name (e.g. ramp actors are named `SM_Ramp2/3_UAID_*`, so substring
  `SM_Ramp` matches 2 components) **but report only 1 runtime LOD** → state proves out, no visual.
- The **rocks** (`SM_Boulder`, `SM_RockFlats_01/02`) are placed on **`InstancedFoliageActor`** (auto-named
  `InstancedFoliageActor_*`, **not** matchable by mesh-name substring) **and** report only 1 runtime LOD —
  so `Boulder`/`RockFlats` as a substring matches **zero** actors. (The byte-scan's "LOD7" token did not
  reflect the cooked/runtime LOD count.)
- The **only** multi-LOD meshes in the streamed level are `SM_Bush` and `SM_Tree` (2 LODs each), also on
  `InstancedFoliageActor` → reachable via substring **`Foliage`** (5 actors), which forces their instanced
  components to the low-detail LOD. As *instanced* foliage the forced-LOD visual is subtle/unreliable.
**Conclusion:** there is **no deterministic visual LOD target** in stock MainWorld. Validate `lod_corruption`
by **state-read on `SM_Ramp`** (`forced_lod 0→1`, reverts to 0) plus a best-effort `Foliage` visual; for a
guaranteed visual, import a multi-LOD mesh placed as a regular (non-instanced) `StaticMeshActor` into a test
sublevel. Confirm any candidate live with `GetStaticMesh()->GetNumLODs() > 1`. (2026-06-09.)

### G16 — Static-mesh vs skeletal-mesh forced-LOD are different APIs on different base classes (M2)
`UStaticMeshComponent::SetForcedLodModel(int32)` (prop `ForcedLodModel`) vs
`USkinnedMeshComponent::SetForcedLOD(int32)` / `GetForcedLOD()` (inherited by `USkeletalMeshComponent`).
Both are **1-based** (0 = auto/off; N forces LOD N-1), but the method names and owning classes differ, and
`FindComponentsMatching<T>` forces the call site to pick `T`. `lod_corruption` v1 is **static-mesh-only**
(the deterministic MainWorld targets are static; the only skeletal candidate, the Bot, is runtime-spawned /
non-deterministic — G4). Skeletal forced-LOD is a documented **M3 follow-up**. (2026-06-09.)

### G17 — 5.1 host-target build constants are `BuildSettingsVersion.V2` / `EngineIncludeOrderVersion.Unreal5_1` (M2.5)
The host scaffolding's `*.Target.cs` was written for 5.4 with `DefaultBuildSettings = BuildSettingsVersion.V5`
and `IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4`. **Neither value exists in 5.1** —
building against 5.1 fails immediately with `'BuildSettingsVersion' does not contain a definition for 'V5'`
and `'EngineIncludeOrderVersion' does not contain a definition for 'Unreal5_4'`. Verified against the 5.1
UBT source (`Engine/Source/Programs/UnrealBuildTool/Configuration/TargetRules.cs`): 5.1 defines
`BuildSettingsVersion` only up to **`V2`** (members `V1`, `V2`, `Latest`) and `EngineIncludeOrderVersion`
only up to **`Unreal5_1`** (`Unreal5_0`, `Unreal5_1`, `Latest = Unreal5_1`). So the 5.1 host targets use
`DefaultBuildSettings = BuildSettingsVersion.V2;` and `IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;`
(`V2` is also what UBT's own upgrade hint resolves to: `Latest - 1`). This is **host scaffolding only** —
the plugin `Build.cs` pins no `CppStandard` and inherits 5.1's C++17 default unchanged; the plugin source
needed **zero** changes to compile on 5.1. (M2.5, 2026-06-10.)

### G18 — after switching the source engine to a new branch, rebuild ShaderCompileWorker (env, M2.6)
Switching `D:\UESource\UnrealEngine` to the **5.1** branch and rebuilding produced a fresh `UnrealEditor.exe`
but left the **stale 5.4-era `ShaderCompileWorker.exe`** (the rebuild didn't include the SCW program).
Launching the editor then throws the modal **"Expecting ShaderCompileWorker output version 8, got 20
instead! Forgot to build ShaderCompileWorker?"** — shaders can't compile, editor unusable. The SCW binary
was ~1 month older than `UnrealEditor.exe`. **Fix (engine tooling, NOT an engine-source patch):**
```
& "D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat" ShaderCompileWorker Win64 Development -waitmutex
```
then relaunch (first launch recompiles all shaders since none are cached). Other engine **Programs**
(`UnrealLightmass`, `InterchangeWorker`, …) can be similarly stale if their features are exercised —
rebuild the same way if a version-mismatch modal names them. **Re-run this whenever the source engine is
re-synced/rebuilt.** (M2.6, 2026-06-10.)

### G19 — skeletal forced-LOD: the settled 5.1 accessors + common base (supersedes G16's static-only scope) (M3)
`lod_corruption` is now static **OR** skeletal, and `lod_popping` is new; both go through the shared
`AnomalyLod` helper. The 5.1 facts that pin the dispatch (all verified against `D:\UESource\UnrealEngine`,
Release-5.1):
- **Setter/getter — `USkinnedMeshComponent::SetForcedLOD(int32)` / `GetForcedLOD()`**
  (`Components/SkinnedMeshComponent.h:844/848`). **1-based**, identical semantics to static
  `UStaticMeshComponent::SetForcedLodModel(int32)` / `ForcedLodModel`: `0 = no forced LOD (auto)`,
  `N` forces LOD `N-1`, valid range `[1, NumLODs]`. `USkeletalMeshComponent` inherits both from
  `USkinnedMeshComponent` (the base that owns them).
- **LOD-count accessor (the AMB-1 decision) — use `USkinnedMeshComponent::GetNumLODs()`**
  (`:828`), whose impl returns the **runtime render-data count** `RenderData->LODRenderData.Num()`
  (`SkinnedMeshComponent.cpp:3412`). This is the true analog of the static
  `UStaticMesh::GetNumLODs()` we already use, and the right number for G15's runtime-LOD reasoning.
  **Do NOT use the asset-level `USkinnedAsset::GetLODNum()`** (`Engine/SkinnedAsset.h:138`) — that is
  the *authored* LOD count and can diverge from cooked/stripped runtime render data. (Note also that
  in 5.1 the old `USkinnedMeshComponent::SkeletalMesh` UPROPERTY is **deprecated** in favour of
  `GetSkinnedAsset()` / `USkeletalMeshComponent::GetSkeletalMeshAsset()`; using the component-level
  `GetNumLODs()` sidesteps that entirely.)
- **Common base = `UMeshComponent`.** `UStaticMeshComponent : UMeshComponent` and
  `USkinnedMeshComponent : UMeshComponent` are disjoint siblings, so a single capture record keyed to
  `TWeakObjectPtr<UMeshComponent>` covers both, and `AnomalyLod` recovers the concrete type via `Cast<>`
  to pick the right getter/setter (AMB-2). Resolving both families = `FindComponentsMatching<UStaticMeshComponent>`
  + `<USkinnedMeshComponent>` merged (no overlap). All types are in the **Engine** module — **no new
  module dependency** (`Core`/`CoreUObject`/`Engine` unchanged).
- **Python verification note:** the skinned component's `get_num_lods()` is **not bound** in the editor
  Python API (only `get_forced_lod` / `set_forced_lod` / the `forced_lod_model` property are). Read the
  runtime LOD count off the C++ log line `lod_corruption: '<comp>' forced LOD N of M` instead — `M` is
  exactly what the helper's `GetNumLODs()` returned. (This supersedes the M2 "static-mesh-only" note in
  G16; G16 stays as the historical record of why v1 was static-only.) (M3, 2026-06-13.)

### G20 — the Bot DOES spawn in a Simulate session, carries mixed components, and is single-LOD (M3)
Live scout over the bridge in a 5.1 `MainWorld` **Simulate** session (the only PIE-start exposed to the
bridge is `LevelEditorSubsystem.editor_play_simulate`):
- **The player Bot spawns in Simulate too.** Contrary to the usual "Simulate has no player pawn"
  expectation, StackOBot's GameMode spawns `BP_Bot_C_0` in a Simulate session, so the **real** Bot was
  available for the skeletal state gates with **no manual spawn and no full Play session**. (G4 still
  holds for *timing* — it appears only after play begins, not at editor/MainWorld load.)
- **`BP_Bot_C_0` carries 1 static + 2 skinned components:** `StaticMeshComponent_0` (static),
  `CharacterMesh0` (skinned, `SKM_Bot`), `Jetpack` (skinned). So substring **`Bot`** is intrinsically a
  **heterogeneous** target set — one `IAI.Apply lod_corruption Bot` captures/forces/reverts a static
  mesh component *and* the skeletal Bot together (proves the AMB-2 single-record convention across
  component types in one apply, no contrived cross-actor substring needed).
- **`SKM_Bot` is single-LOD** (asset `lod_info` length 1; the helper read its runtime render-data count
  as **1** live — log "forced LOD 1 of 1"). Per G15's logic (which applies to skeletal too), forcing a
  LOD on a single-LOD mesh changes **no pixels** — so skeletal `lod_corruption`/`lod_popping` are
  **state-validated only; the Bot is not a deterministic visual target.** Posture per G15: state-validated.
- **Static multi-LOD targets, re-confirmed:** only `SM_Bush` (2 LODs) and `SM_Tree` (2 LODs), both on
  `InstancedFoliageActor` (substring **`Foliage`**). Rocks are single-LOD. Same as G15 → **no new
  deterministic visual for M3**; the instanced-foliage pop is the only (subtle/unreliable) visual.
  (M3, 2026-06-13.)

### G21 — module rename ⇒ DLL rename ⇒ must clean stale Binaries/Intermediate (refactor)
Renaming the module (`GDPAnomalyInjector` → `AnomalyInjector`) renames its output DLL
`UnrealEditor-GDPAnomalyInjector.dll` → `UnrealEditor-AnomalyInjector.dll`. The **old DLL** and the stale
`Intermediate/` UHT manifests still name the old module, so a clean start is required or the editor tries to
load the vanished old module ("modules missing, rebuild?") / links against stale objects. **Fix:** delete the
project-root **and** plugin `Binaries/` + `Intermediate/` (never `Saved/`), then rebuild `StackOBotEditor /
Development / Win64`. The subsystem `.generated.h` (the only `UCLASS`) regenerates under the new name
automatically, and the `*_API` macro `ANOMALYINJECTOR_API` is derived from the module name by UHT — no manual
edit. Verified clean compile (exit 0) + bridge re-gate green. (Session 007, 2026-06-18.)

### G22 — primitive render-time reflects occlusion, but only `GetLastRenderTimeOnScreen()` is shadow-clean (viewport)
The viewport layer's occlusion question is "did the renderer actually draw this in the player's view." Pinned
against 5.1 source for the AMB-V1 backend decision:
- The **component-level** render time (`UPrimitiveComponent::LastRenderTime` / `LastRenderTimeOnScreen`, read by
  `GetLastRenderTime()` / `GetLastRenderTimeOnScreen()` / `WasRecentlyRendered()`) is written by
  `FPrimitiveSceneInfo::UpdateComponentLastRenderTime(t, bUpdateLastRenderTimeOnScreen)` — `LastRenderTime`
  always, `LastRenderTimeOnScreen` only when the bool is true (`PrimitiveSceneInfo.cpp:2135`).
- In the **main view** that update is gated on `View.PrimitiveDefinitelyUnoccludedMap[BitIndex]` with
  `bUpdateLastRenderTimeOnScreen=true` (`SceneVisibility.cpp:2491`) — i.e. it genuinely reflects **occlusion**
  (the comment says "This signals that the primitive is visible"), not just frustum/distance cull.
- BUT the **shadow-depth** passes call the same updater with `bUpdateLastRenderTimeOnScreen=false`
  (`ShadowSetup.cpp:1672, 1909`), so a primitive that is fully occluded yet casts a shadow into view still bumps
  `LastRenderTime`. Therefore `GetLastRenderTime()` / `WasRecentlyRendered()` (which read it) are
  **shadow-contaminated**; only **`GetLastRenderTimeOnScreen()`** is the occlusion-clean main-view signal. The
  other `=true` site is ray-tracing-dynamic-geometry only (`DeferredShadingRenderer.cpp:1202`, gated on an RT
  cvar; off in StackOBot). `UpdateComponentLastRenderTime` also writes `OwningActor->LastRenderTime`, so
  `AActor::GetLastRenderTime()` is contaminated too.
- **Decisive consequence (AMB-V1):** render-time is a property of *the view the renderer drew*; it **cannot be
  evaluated for an arbitrary synthetic view**, which collides with the locked "core is deterministically
  state-gatable with a synthetic view." So v1's occlusion backend is the **multi-sample camera-to-bounds line
  trace** (synchronous, deterministic, synthetic-gatable, Engine-only), private behind `AnomalyViewport`.
  `GetLastRenderTimeOnScreen()` (with a custom tolerance `World->TimeSince(...) <= max(Tol, DeltaTime+eps)`,
  AMB-V2) is the verified drop-in **live** backend for the future capture/live-injection milestone — a .cpp-only
  swap, where render-fidelity matters and a per-frame cache is in play. The trace over-includes on
  no-collision / translucent occluders (accepted v1 trade-off; safe direction — never drops a visible target).
  (Viewport milestone, session 008, 2026-06-18.)

### G23 — a StackOBot Simulate session DOES expose a usable local-player view (viewport)
Planning feared Simulate had no usable player camera (so live viewport scoping would degrade to unscoped there).
Verified live over the bridge: StackOBot's Simulate session has `PlayerController_0` **with** a
`PlayerCameraManager` whose POV is valid (e.g. loc (3074,4336,1646), yaw -40, fov 90 — near the level start),
so `AnomalyViewport::GetActiveViewInfo` (which uses `APlayerController::GetPlayerViewPoint` + `GetFOVAngle`)
**resolves a real view in Simulate** — `IAI.SetViewportScoping 1` then `IAI.Apply missing_object SM_Ramp`
matched both ramps (both in that camera's cone) with **no** "no local player controller" warning. Caveats:
- That POV is the **auto/spawn camera POV**, NOT the editor perspective viewport the user sees in Simulate, and
  it isn't something to set/know a priori. So the **deterministic** core gate still uses the synthetic-view
  command `IAI.TestVisibility` (a pure function of an explicit view + world); the live resolver's
  off-screen/on-screen discrimination is owner-eyeballed in **real Play**.
- The **no-view treat-as-unscoped degrade** (AMB-V3: `GetActiveViewInfo` returns false → return the full
  matched set + one warning) is **code-verified, NOT bridge-triggerable** — precisely *because* StackOBot Simulate
  always exposes a usable local-player view (above), so `GetActiveViewInfo` never returns false there and the
  degrade branch can't be reached over the bridge. The branch **errs safe**: returning the full matched set is
  exactly the scoping-**OFF** behavior, so on-but-no-view can only ever *over*-affect (act on every match), never
  silently drop a target. It covers genuinely view-less contexts (dedicated server, very early frames). Deliberately
  **no** `WITH_EDITOR` editor-viewport fallback was added (no UnrealEd dep). (Viewport milestone, session 008, 2026-06-18.)

### G24 — reversed-Z is the one footgun assembling a synthetic view-projection; validate near/far empirically (viewport)
For the synthetic-view core, assemble the VP exactly as the engine's live path does (verified 5.1) so synthetic
and live agree for the same pose/FOV/aspect:
- **Projection (reversed-Z, infinite far):** `FMinimalViewInfo::CalculateProjectionMatrix()` →
  `FReversedZPerspectiveMatrix(max(0.001, FOV)*PI/360, AspectRatio, 1, NearClip)` where `FOV` is the **full
  horizontal** FOV in degrees (`CameraStackTypes.cpp:89`). `SceneView.h:44`: "clip space Z=1 is the near plane,
  Z=0 is the **infinite far** plane."
- **View rotation (basis swap):** `FInverseRotationMatrix(Rotation) * FMatrix(FPlane(0,0,1,0),(1,0,0,0),(0,1,0,0),(0,0,0,1))`
  (`LocalPlayer.cpp:1139`).
- **Compose:** `VP = FTranslationMatrix(-Origin) * ViewRotationMatrix * ProjectionMatrix` (==
  `FSceneViewProjectionData::ComputeViewProjectionMatrix`, `SceneView.h:82`).
- **Frustum:** `GetViewFrustumBounds(Frustum, VP, /*bUseNearPlane=*/true, /*bUseFarPlane=*/false)` — under
  reversed-Z the near plane (clip Z=1) rejects behind-camera geometry; the far plane is infinite/degenerate so it
  is **skipped** (distant-but-in-cone objects stay in frustum). Then `Frustum.IntersectSphere/IntersectBox`
  against `UPrimitiveComponent::Bounds`.
- **Empirically validated** via `IAI.TestVisibility` in the synthetic gate (session 008): camera *behind* a known
  target → `frustum=0` (near-plane works); target *53k units away but in cone* → `frustum=1` (far correctly not
  clipping); in-cone clear → `frustum=1 unoccluded=1`. If anything behind the camera or absurdly far tests as
  in-frustum, suspect the near-plane flag / VP assembly first. (Viewport milestone, session 008, 2026-06-18.)

### G25 — a game-agnostic Canvas HUD = `UDebugDrawService::Register("Game")`; it draws WITHOUT the host's HUD class (m5)
The selector UI must draw an on-screen overlay in PIE/standalone **without** the host cooperating (no setting the
game's `AHUD`/GameMode HUD class — that would break the game-agnostic invariant). The settled 5.1 path:
- `static FDelegateHandle UDebugDrawService::Register(const TCHAR* Name, const FDebugDrawDelegate&)`
  (`Debug/DebugDrawService.h:23`, module **Engine**); the delegate is
  `DECLARE_DELEGATE_TwoParams(FDebugDrawDelegate, UCanvas*, APlayerController*)` (`:16`).
- It is a **global static** delegate list drawn unconditionally by the engine viewport path:
  `UGameViewportClient::Draw` → `UDebugDrawService::Draw(ViewFamily.EngineShowFlags, InViewport, View, DebugCanvas, DebugCanvasObject)`
  (`GameViewportClient.cpp:1820`). `Draw` fires every registered delegate whose **show flag** is set
  (`DebugDrawService.cpp:84-111`). **Zero dependence on the project's HUD/GameMode class.**
- Register under the **`Game`** show flag (`ShowFlagsValues.inl:267`), which is forced ON for any non-editor view:
  `SetGame(InitMode != ESFIM_Editor && InitMode != ESFIM_VREditing)` (`ShowFlags.h:430`). So the delegate fires in
  PIE and standalone game.
- **Delegate hygiene (load-bearing):** register on enable; unregister on **both** disable **and**
  `Deinitialize`/teardown; **guard against double-register** (`IAI.SelectorUI 1` twice must not stack delegates — a
  dangling/duplicate debug-draw delegate is a crash/UB risk). The selector stores the `FDelegateHandle` and no-ops
  `SetUIEnabled` when the state is unchanged.
- **Rejected alternatives:** an `AHUD` post-render delegate needs the host to set our HUD class (not game-agnostic);
  a `UGameViewportClient` Slate viewport widget needs Slate widget injection + pulls Slate/SlateCore (heavier).
  `UDebugDrawService` is strictly minimal and host-blind. (m5, session 009, 2026-06-19.)

### G26 — `IsInputKeyDown`/`WasInputKeyJustPressed` read RAW key state, independent of the game's input mappings (m5)
For game-agnostic input the selector polls keys directly, with **no** project Action/Axis mapping required:
- `APlayerController::IsInputKeyDown(FKey)` (`PlayerController.h:1563`) and `WasInputKeyJustPressed(FKey)` (`:1567`)
  route to `PlayerInput->IsPressed(Key)` / `WasJustPressed(Key)` (`PlayerController.cpp:5530-5538`), which read the
  raw **`KeyStateMap`**: `IsPressed` returns `KeyStateMap.Find(InKey)->bDown` (`PlayerInput.cpp:1807-1809`);
  `WasJustPressed` returns `…->EventCounts[IE_Pressed].Num() > 0` (`:1691-1693`). `KeyStateMap` is populated from raw
  key events, so polling `Tab` works even though StackOBot never binds it. `Shift+Tab` is composed at poll time:
  `WasInputKeyJustPressed(Tab) && IsInputKeyDown(LeftShift||RightShift)`.
- `FKey`/`EKeys` are in the **InputCore** module (`EKeys::Tab` etc. defined `InputCoreTypes.cpp:32/68/124`). The local
  PC is reachable each tick via `World->GetFirstPlayerController()` (same call `AnomalyViewport` uses; G23 — a
  Play/Simulate session always exposes one).
- **Default keybinds (S7):** Tab (next) / Shift+Tab (prev) / C (cycle anomaly) / G (inject) / H (revert), all
  rebindable via `IAI.SelectorBind <action> <KeyName>` (validated with `EKeys::GetKeyDetails(...).IsValid()`).
  **Caveat:** in a **Steam-launched** build the Steam overlay grabs **Shift+Tab**, so the default prev gesture is
  shadowed there (fine in PIE/standalone — it's an informed default; rebind `prev` to a dedicated key to escape it).
  (m5, session 009, 2026-06-19.)

### G27 — the selector adds exactly ONE module dep (`InputCore`); immediate-mode HUD avoids Slate/UMG entirely (m5)
The roadmap anticipated the first UI milestone would pull Slate/SlateCore/UMG. It did **not** — by choosing an
**immediate-mode** HUD over a UMG widget: `UDebugDrawService`, `UCanvas` (`Engine/Canvas.h`), `FCanvasTextItem`
(`CanvasItem.h`) and `DrawDebugBox` (`DrawDebugHelpers.h`) are **all in the Engine module** (already a dep). The only
non-Engine type is `FKey`/`EKeys` (**InputCore**). InputCore is already a **public** dependency of Engine
(`Engine.Build.cs` PublicDependencyModuleNames), so it is transitively available — *strictly, zero new deps are
required to compile* — but it is declared explicitly in `AnomalyInjector.Build.cs` for **IWYU hygiene**. Net: deps go
`Core`/`CoreUObject`/`Engine` → **+`InputCore`** (the first addition since M0); **no Slate/SlateCore/UMG**. A custom-
depth glowing outline (deferred, not v1) would later need the project Custom Depth-Stencil setting + a shipped
post-process material + `UPrimitiveComponent::SetRenderCustomDepth`/`SetCustomDepthStencilValue`
(`PrimitiveComponent.h:1758/1762`) — a render-pipeline + content surface the `DrawDebugBox` highlight sidesteps.
(m5, session 009, 2026-06-19.)

### G28 — the `=` exact-match sentinel in `AnomalyTargeting`; exact-name is the v1 identity ceiling (m5)
`AnomalyTargeting::FindActorsMatching` matches by **substring** (`GetName().Contains` / class-name; `:28-30`). Passing
the selected actor's `GetName()` as a plain substring is **not** safe for arbitrary actors: a selected `Cube` is a
strict substring of `Cube2`, so an inject would also corrupt the numbered sibling — the exact mislabeled-but-present
frame the viewport layer exists to prevent. (Stock StackOBot placed actors are `_UAID_…`-suffixed → effectively
unique, so the risk is content-dependent, but the selector targets *arbitrary* user-picked actors so we can't rely on
that.) **Fix (S1, pre-authorized by verify-item 5):** a leading-`=` sentinel in `FindActorsMatching` — strip the `=`
and match by `GetName().Equals(rest, ESearchCase::IgnoreCase)`. Because **every** object-scoped path funnels through
`FindActorsMatching` (directly, or via `FindComponentsMatching<T>` / `AnomalyLod` / the `AnomalyViewport` finders), this
single edit makes exact targeting available to all four anomalies with **zero anomaly edits, zero `IAnomaly` change,
and full backward-compatibility** (object names never contain `=`; the substring path is byte-identical with no `=`).
`InjectSelected()` passes `"=" + Actor->GetName()`. **This is load-bearing beyond m5** — the future auto-injection path
(which also targets arbitrary on-screen actors) uses the same primitive. **Accepted ceiling:** exact-*name* uniquely
identifies within a level but not across streamed sublevels that hold duplicate names; perfect pointer identity would
require widening `IAnomaly::Apply` past its string-arg contract (rejected — the M1 lock). Document, don't solve.
(m5, session 009, 2026-06-19.)

### G29 — "visible set" must mean "renderable-visible set": a frustum+occlusion-only test passes non-rendering primitives (m5)
The live eyeball found the selector's visible set included **non-renderable** actors — `RuntimeVirtualTextureVolume`,
`PlayerStart`, `GameplayDebuggerCategoryReplicator`, `LandscapeStreamingProxy`, `RoomBuilderSquare`. Cause: the m4
visibility test is frustum AND occlusion over **any** `UPrimitiveComponent`, and these actors carry primitives that
pass it but draw no useful geometry (collision boxes, capsules, bounds boxes). Injecting on them is the
unlabeled-but-invisible sample the viewport layer exists to prevent. The fix is a **renderability predicate** added to
`AnomalyViewport` (the shared source of truth — the selector AND future auto-injection consume the same corrected set).
Pinned against 5.1 source:
- **Predicate (R1) = `IsVisible()` AND a base-TYPE allowlist:** `IsRenderableComponent(Comp)` =
  `Comp->IsVisible() && (Comp->IsA<UStaticMeshComponent>() || Comp->IsA<USkinnedMeshComponent>() || Comp->IsA<UFXSystemComponent>())`.
  A capability/type test, **not a class blocklist** (a blocklist rots on another title; this stays game-agnostic).
  **SUPERSEDED (VFX inclusion only) by G33 (2026-06-20):** the `|| IsA<UFXSystemComponent>()` clause was removed —
  VFX/particles are no longer in the renderable-visible set. The rest of G29 (the SM/SK allowlist, `IsVisible()` over
  `ShouldRender()`, the empty-ISM guard, the no-view contract, the landscape extension point) stands unchanged.
- **`IsVisible()`, not `ShouldRender()` (R3):** `USceneComponent::IsVisible()` returns false if `bHiddenInGame`, else
  `GetVisibleFlag() && level-visible` (`SceneComponent.cpp:3140-3149`) — deterministic, view-independent. `ShouldRender()`
  has a **non-shipping branch** that returns true for hidden collision components when
  `World->bCreateRenderStateForHiddenComponentsWithCollsion` is set (`SceneComponent.cpp:3075-3104`) — a determinism
  footgun for the synthetic gate. Avoided.
- **VFX with NO new dep (R2 budget held):** `UFXSystemComponent : public UPrimitiveComponent` is **ENGINE_API**
  (`Particles/ParticleSystemComponent.h:355`) and is the common base of `UNiagaraComponent` (Niagara **plugin**,
  `NiagaraComponent.h:36`) and `UParticleSystemComponent` (`:459`). `IsA<UFXSystemComponent>()` catches both with only
  the Engine header → deps stay `Core`/`CoreUObject`/`Engine`/`InputCore`.
- **Why the RVT volume slipped in (precisely):** its RVT component is `URuntimeVirtualTextureComponent : USceneComponent`
  (`RuntimeVirtualTextureComponent.h:17`) — **not** a primitive, so it was never in the set. The false-positive primitive
  is the volume's bounds box `UBoxComponent "Bounds"` (`RuntimeVirtualTextureVolume.cpp:18`), excluded by the allowlist
  (not SM/SK/FX). Same shape excludes PlayerStart (capsule) and the debug/streaming actors.
- **Landscape (R5):** `ULandscapeComponent` renders but is excluded (not in the allowlist) — matching the requirement.
  **Extension point:** add `|| IsA<ULandscapeComponent>()` (one line, still a type test) to make terrain selectable on a
  future title — documented in `IsRenderableComponent`, intentionally inactive.
- **Empty-instance guard (R1 refinement, confirmed by the live gate):** a `LandscapeStreamingProxy` still leaked in via
  its **grass** — `GrassInstancedStaticMeshComponent` (a `UStaticMeshComponent` subclass) which IS allowlisted and
  `IsVisible()`, but the live enumeration showed **instance count = 0** (empty — draws nothing). So `IsRenderableComponent`
  additionally requires `Cast<UInstancedStaticMeshComponent>(Comp)->GetInstanceCount() > 0` for instanced meshes
  ("renders nothing => not renderable"). HISM derives from ISM so the Cast catches grass/foliage uniformly; real foliage
  and populated ISMs (count > 0) stay. Capability refinement, not a blocklist. **Counter-case kept:** `RoomBuilderSquare`
  has 4 ISMs with 19/4/25/195 = **243 real instances** → it genuinely renders and is **intentionally retained** as a valid
  target (the "no visible geometry" assumption was wrong; excluding it would reintroduce name-based special-casing the
  allowlist exists to avoid). Corrected definition: **renderable = a visible SM/SK/FX component that actually draws
  something (instanced ⇒ instance count > 0).**
- **Ordering = perf (R1):** the renderability check runs FIRST in the per-component test, before the occlusion line
  traces, so non-targets are rejected without tracing.
- **Additive only (R2):** new entry points `IsRenderableComponent` / `IsActorRenderableVisible` /
  `FilterRenderableVisibleActors` / `GetVisibleRenderableActors`. The m4 functions (`IsComponentVisible`,
  `IsActorVisible`, `FilterVisibleActors`, `FindVisible*`) are **byte-identical** — the scoping-ON path + all prior gates
  keep their guarantees without re-proving.
- **No-view contract (R6):** `GetVisibleRenderableActors` returns **empty** on no resolvable view (offer nothing, never
  inject blind) — **deliberately distinct** from the `FindVisible*Matching` finders' treat-as-unscoped. Two callers, two
  safe directions: console finders serve an explicit human instruction (act, don't silently drop); the selector /
  auto-injection must never offer or inject blind (no legitimate visible set ⇒ nothing). A future reader must NOT
  "reconcile" them. (m5 follow-on, session 009, 2026-06-19.)

### G30 — auto-injection v1 is concurrent-but-collision-free BY CONSTRUCTION via a ONE-ANOMALY-PER-ACTOR scheduler invariant (m6)
`UAnomalyAutoInjectorSubsystem` fires multiple anomalies at once but never collides — **without a ref-count
coordinator** — by holding two scheduler invariants:
- **(i) one live fire per id.** The injector registry holds exactly **one instance per id** (`TMap<FName,
  TUniquePtr<IAnomaly>>`; re-Apply of a live id reverts-then-reapplies — `AnomalyInjectorSubsystem.cpp:250-263` +
  each anomaly's `if (bActive) Revert();`). So the scheduler **never re-fires a still-live id** (the natural
  concurrency ceiling = distinct enabled-id count, and clean revert accounting — it only ever reverts what it fired).
- **(ii) one anomaly per actor.** `Candidates = V − {actors hosting ANY live fire}` (`TryFireOnce`). This single
  invariant **subsumes BOTH conflict groups** — bHidden (`missing_object`/`blinking` both `SetActorHiddenInGame`) AND
  forced-LOD (`lod_corruption`/`lod_popping` both `AnomalyLod::SetForcedLod`) — **and** the hide-masks-LOD case (a hide
  hiding a LOD change = an invisible/mislabeled sample, the exact failure the viewport layer exists to prevent). Among
  the 4 pool ids all 6 cross-pairs are either same-resource or visibility-masks-LOD, so one-per-actor is strictly
  simpler than a per-conflict-group guard and needs **no id→group table**.
- **The registry's one-instance-per-id is thus repurposed as the concurrency limiter** — the same property that makes
  manual re-injection "last-writer-wins" (G12) is exactly what bounds and de-conflicts the auto path here.
- **Determinism:** all randomness is one `FRandomStream` seeded once per run; draws happen on a fixed schedule
  independent of `ApplyAnomaly`'s result (skip-paths consume 0 draws; Candidates-empty consumes only the Id draw; a real
  attempt draws Id/Target/Hold then registers on success). The seed reproduces the choices given the same visible-set
  sequence + Step granularity (full run reproducibility is a capture-pipeline concern, not v1).
- **The deferred ref-count "hidden-by" coordinator + per-(id,target) registry keying (G12) remain the path for
  DELIBERATE compound/stacked same-actor anomalies** — v1 sidesteps the need entirely via one-per-actor. (m6, session
  010, 2026-06-19.)

### G31 — conflict-group resource identity is NOT machine-readable from `GetUsage()`; v1 needs no group table anyway (m6)
`IAnomaly::GetUsage()` returns a **human hint string** (e.g. `"<name-substring> [hz]"`, `"<substring> [lod-index]"` —
`IAnomaly.h:36`), NOT the mutated resource (bHidden vs forced-LOD). So a conflict-group guard could never have read the
resource from it. Under G30's one-anomaly-per-actor invariant the auto-injector needs **no group table at all** (the
guard never reads a group). **Forward note:** IF selective per-group same-actor stacking is ever built (the deferred
compound-anomaly milestone), encode groups in a **tiny internal id→group table** about the plugin's own ids — never
parse `GetUsage()`, and don't add a resource accessor to `IAnomaly` (the M1 lock). (m6, session 010, 2026-06-19.)

### G32 — UE log categories do NOT export across module/DLL boundaries; each module declares its own (control server, Slice 0)
A new module that reuses another module's `DECLARE_LOG_CATEGORY_EXTERN` category **compiles but fails to LINK**. The
new control-server module (`AnomalyControlServer`) first reused the core's `LogAnomaly` (declared in the core's public
`AnomalyInjectorLog.h`, defined `DEFINE_LOG_CATEGORY(LogAnomaly)` in the core module). Result:
`LNK2001: unresolved external symbol "struct FLogCategoryLogAnomaly LogAnomaly"` → `LNK1120` (the Slice-0 build, exit 6).
- **Cause:** `DEFINE_LOG_CATEGORY(X)` emits a plain global symbol with **no `*_API` (dllexport) decoration**, so the
  category object is **not exported** from the defining module's DLL and cannot be resolved by a dependent module's DLL.
  (UCLASS/USTRUCT reflected types and `*_API`-decorated functions export; bare log-category globals do not.) Including
  the header is not enough — the *definition* lives unexported in the other DLL.
- **Fix (idiomatic):** every module declares **its own** category. The control server uses `LogAnomalyServer`
  (`DECLARE_LOG_CATEGORY_EXTERN` in a private `AnomalyControlServerLog.h`, `DEFINE_LOG_CATEGORY` in
  `AnomalyControlServerModule.cpp`); the core keeps `LogAnomaly`. Rebuild clean (exit 0). Bonus: per-surface categories
  keep Output-Log filtering clean (`LogAnomaly` vs `LogAnomalyServer`).
- A genuinely shared cross-module category would need a custom `*_API`-decorated category declaration — not worth it;
  per-module categories are the UE norm. (Slice 0 / control server, 2026-06-20.)

### G33 — VFX removed from the renderable-visible set (reverses the G29/R1 VFX inclusion) (viewport)
The `IsRenderableComponent` allowlist originally admitted **three** families — static mesh, skeletal/skinned mesh, **and
VFX** (`UFXSystemComponent`, the common Engine base of Niagara + Cascade; G29/R1). The VFX clause is now **removed**: the
renderable-visible set is **SM ∥ SK only**. Rationale: particles are not useful *injectable geometry* targets — the four
object-scoped anomalies are hide / blinking / forced-LOD, none of which produce a meaningful, labelable corruption on a
pure-particle actor (the LOD pair can't even match one — no mesh LODs), so offering particles in the selectable/auto/
dashboard set only invites unlabeled or no-op samples. This reverses the prior R1 ruling deliberately.
- **Single change, single source of truth:** drop `|| Component->IsA<UFXSystemComponent>()` from the allowlist in
  `AnomalyViewport::IsRenderableComponent`. Because the set is the one source of truth, this propagates in lockstep to
  **all** consumers — the M5 selector, the m6 auto-injector, the control-server A4 read-back
  (`GetVisibleRenderableActorInfos` → dashboard), and the `IAI.DumpVisible` set-identity gate. (Also removed: the dead
  `"FX"` branch in `ClassifyRenderableComponent` and the now-unused `#include "Particles/ParticleSystemComponent.h"`.)
- **Escape hatch intact (scope boundary):** the change is confined to the renderable-visible **set**. The console
  by-name finders (`AnomalyTargeting::FindActorsMatching` / `FindComponentsMatching<T>`, incl. the `=name` exact match)
  do **NOT** route through `IsRenderableComponent`, so `IAI.Apply <id> =<VfxActorName>` still reaches a VFX actor. The
  explicit human-named escape hatch is preserved; only the auto-offered set narrows.
- **Gate consequence:** the old "select a pure-VFX actor → LOD anomaly → 0 matched" scenario (R4 zero-match surfacing)
  is no longer reachable *through the set* (the VFX actor is no longer offered). The `0 matched` HUD/log plumbing is
  unchanged; the zero-match gate is re-pointed to the console escape hatch (`IAI.Apply lod_corruption =<VfxName>`).
- Hard remove, no toggle. (Viewport fix, 2026-06-20.)

### G34 — changeable poll-radius distance cull: pawn-origin, sentinel-OFF, applied at BOTH live entry points (viewport)
An optional distance cull on the LIVE renderable-visible poll: an actor is in the set iff renderable AND within radius
**R** of the poll origin AND in-frustum AND unoccluded. Console `IAI.SetPollRadius <value>` (cm), shared state in
`AnomalyViewport.cpp`. The non-obvious parts:
- **Origin = the player PAWN, not the camera (locked).** `ResolvePollOrigin` reads the first PC's possessed pawn
  location; the camera origin is only a fallback when there is no pawn (spectator / pawn-less Simulate). Don't "fix"
  this to the camera. (The dashboard's `FRenderableActorInfo::Distance` is separately CAMERA-relative — two metrics,
  two purposes; deliberately not reconciled.)
- **Metric = sphere-approximated bounds distance:** `Dist(PollOrigin, Bounds.Origin) - Bounds.SphereRadius <= R`.
  `Component->Bounds` is a cached member (O(1)) — the same one the frustum/occlusion tests read, so there is no
  double-computation to dedupe. Cull order: renderable type-test → distance → frustum → occlusion (cheapest-first;
  out-of-range actors are rejected before any line trace). Order is a pure short-circuit choice (the set is an AND).
- **Default OFF via a sentinel:** `R <= 0` disables the cull entirely → behavior BYTE-IDENTICAL to no-cull (the
  regression guarantee). No separate enabled flag.
- **Applied to BOTH live poll entry points** (`GetVisibleRenderableActors` AND `GetVisibleRenderableActorInfos`) with
  the identical pawn-origin + radius, so the `IAI.DumpVisible` set-identity gate still passes. The cull is threaded as
  a parameter through the shared chokepoint `IsComponentRenderableVisibleInternal`; the **explicit-view** functions
  (`IsActorRenderableVisible` / `FilterRenderableVisibleActors`) pass radius **0** so the synthetic-gate surface stays
  byte-identical even when a radius is set. (Reading the global *inside* the chokepoint would have wrongly culled the
  synthetic surface too — hence the parameter.)
- **Debug sphere (dev):** when `R > 0`, a yellow sphere of radius R is drawn centered on the LIVE pawn, re-resolved
  **every frame** (never cache a position at registration — the pawn moves). The draw hook is (un)registered on the
  OFF↔ON boundary by `SetPollRadius`. It draws **only in real Play** (Game/PIE world, gated by `World->IsGameWorld()`),
  never a Simulate/editor viewport. Accepted minor: a module unload *while a radius is set* leaks the handle (a
  teardown hook would mean touching the module `.cpp`, out of this fix's "touch only AnomalyViewport" scope).
  **Capture interaction (G44):** the sphere is a line-batcher draw, so it bakes into game-viewport captures —
  `SetDebugSphereSuppressed(bool)` (a cull-independent visual-only toggle) lets the capture run hide it; the cull is
  unaffected.
- **Drawing the 3D sphere needs a PRE-scene-render hook — a `UDebugDrawService` delegate does NOT work (the real
  bug).** First attempt drew the sphere from a `UDebugDrawService("Game")` delegate; it never appeared. That delegate
  fires during the **post-scene** canvas/HUD draw, *after* the world line batcher has already rendered for the frame,
  so a one-frame debug line queued there is cleared before the next scene pass. **A positive (persistent-batcher)
  lifetime did NOT fix it either** — still invisible (the persistent batcher's render state isn't reliably picked up
  from that late phase). **The fix that works: draw from `FWorldDelegates::OnWorldPostActorTick`** (fires during the
  world tick, BEFORE the scene render) with `LifeTime=-1` — the **same phase** as the selector's Tick-driven
  `DrawDebugBox`, which always rendered correctly. Lesson: a one-frame `DrawDebug*` must be issued from a pre-render
  per-frame context (subsystem `Tick` or a world-tick delegate); a free-function helper like `AnomalyViewport` (no
  Tick) uses the world-tick delegate. The `UDebugDrawService` path is for **canvas (2D)** HUD draws, not 3D line-batcher
  shapes. (Corrected 2026-06-20 after two failed attempts.)
- **No new dep** (`IConsoleManager`/`FVector` = Core; `FWorldDelegates`/`DrawDebugSphere`/`APawn` = Engine). (Viewport
  fix, 2026-06-20.)

### G35 — UE unity builds merge .cpp files into ONE TU; anonymous-namespace helpers must be file-unique (m7)
A UE module's `.cpp` files are concatenated into a unity blob (`Module.<Name>.cpp`), so two files' `namespace { }`
helpers share one translation unit. `AnomalyLabelWriter.cpp` defined `Num`/`Vec3` anonymous-namespace helpers
identical to `ControlSnapshot.cpp`'s → ODR redefinition (`C2084: '...Num(double)' already has a body`), failing only
the unity build (the core module compiled fine). **Fix:** give file-local helpers file-unique names (`LabelNum`/
`LabelVec3`). Lesson: in a UE module never reuse a common anonymous-namespace helper name across `.cpp` files — the
unity blob will collide even though each file is individually legal. (m7, 2026-06-20.)

### G36 — image↔label alignment: capture + ground-truth snapshot in the SAME game-thread tick, stamped `GFrameCounter` (m7)
The labeled-capture writes, per frame, the game-viewport image PLUS a label sourced from the auto-injector's own live-fire
ground truth (L1). Both are taken in ONE game-thread call (`AnomalyLabel::CaptureLabeledShot`): the **synchronous**
`FViewport::ReadPixels` and `Auto->GetLiveFires()` snapshot run on the same tick, and image + JSONL record are stamped
with the same **`GFrameCounter`** ("Steadily increasing frame counter", `CoreGlobals.h:474` — the game-thread counter,
not `GFrameNumber` which is render-oriented). The synchronous readback is what makes the co-thread, same-frame stamp
exact (L3). The render-thread async path (G40) breaks this co-thread assumption and must re-establish alignment via the
frame the backbuffer carries. (m7, 2026-06-20.)

### G37 — a game-thread mutation reaches the rendered frame >=1 frame later; the capture-burst settle-K must be SYMMETRIC (m7)
`r.OneFrameThreadLag` defaults to **1** (`UnrealEngine.cpp:388`), so the render thread trails the game thread by a frame:
a fire or a revert applied on the game thread does not appear in the captured pixels for ~1 frame. A burst therefore has a
lag window at **both** boundaries — just after `FireOnce` the anomaly is not on-screen yet (capturing then mislabels
clean pixels as `present=true`); just after revert it is still on-screen (mislabels corrupted pixels as `present=false`).
The capture subsystem **skips K frames after BOTH the fire AND the revert** before sampling the labeled window (default
`K=2` = 1-frame lag + 1 margin). Settle-K is the **temporal**-lag knob and is **distinct** from the **spatial** view-lag L
(G41) — never conflate them. Gate-2 validated the revert→negative boundary specifically. (m7, 2026-06-20.)

### G38 — the 2D bbox projects the fired actor's PERSISTED bounds by TYPE, NOT the renderable-visible set (m7)
`missing_object`/`blinking` **hide** the actor, so by capture time it has left the renderable-visible set
(`GetVisibleRenderableActorInfos` excludes it — `IsVisible()==false`). But the label box must mark **where the now-hidden
object is** (the correct missing-object label). So `AnomalyViewport::ProjectActorBoundsToScreenRect` unions the actor's
static/skeletal-mesh component `Bounds` selected by **TYPE ONLY** (`IsA<UStaticMeshComponent>() || IsA<USkinnedMeshComponent>()`),
**deliberately NOT gated on `IsVisible()`**, and projects that AABB directly via the shared reversed-Z VP path — NOT by
reading the dashboard's screen-rect (which is absent for a hidden actor). `SetActorHiddenInGame` does not change a
component's `Bounds`, so "where the hole is" projects correctly. This is why the actor-carrying readback (`FAutoLiveFireInfo::
TargetActor`) is essential, not cosmetic. (m7, 2026-06-20.)

### G39 — the game backbuffer's alpha is ~0; force opaque or PNG captures render fully TRANSPARENT ("empty") (m7)
`FViewport::ReadPixels` returns BGRA; the game backbuffer's alpha channel carries no meaningful opacity (typically 0). PNG
**preserves** alpha, so a captured frame opens fully transparent in any alpha-honoring viewer even though the RGB is
correct — it looks "empty". (`Image.open(...).convert("RGB")` silently masks it by dropping alpha, which is why the
overlay script showed the scene while the raw PNG looked empty.) JPEG has no alpha channel, so the dashboard preview path
never exhibited it — the bug surfaced only when PNG became the dataset default. **Fix:** force every pixel `A=255` after
readback (`CaptureGameViewportRaw`). Dataset images default to **PNG (lossless)** — JPEG blocking near high-contrast edges
could be mislearned as corruption by a bug detector; JPEG stays behind a format flag for bandwidth. (m7, 2026-06-20.)

### G40 — `ReadPixels` is a synchronous flush (observer-effect); the async backbuffer path is the deferred exact superseder (m7)
`FViewport::ReadPixels` = `FRenderTarget::ReadPixels` enqueues a `ReadSurfaceData` of the viewport RT then
`FlushRenderingCommands()` (`UnrealClient.cpp:51-89`) — a synchronous game-thread stall (it reads the last
render-thread-completed frame; it does NOT trigger a fresh draw). Acceptable for v1: capture-driven bursts bound the
stall, and the four object-scoped anomalies don't measure framerate. **But it is incompatible with framerate-bug
anomalies** (the flush would corrupt the very framerate label) **and** is not pixel-exact under frame-time variance for
motion view-matching. The render-thread async path — `FSlateRenderer::OnBackBufferReadyToPresent` (`SlateRenderer.h:299`)
+ a staged `FRHIGPUTextureReadback` — is the documented **exact superseder**, now motivated by BOTH framerate-bug
anomalies AND exact motion view-matching. That path has its OWN lag characteristic: **re-derive L there; do NOT assume
the L=0 of G41 carries over.** (m7, 2026-06-20.)

### G41 — capture view-lag **L=0 is CORRECT (not "zero lag")**: world tickables tick BEFORE the camera update (m7)
Stage-3 predicted L=1 (project the bbox with the view 1 frame back, to match the 1-frame-lagged render). **Empirically L=0
matches and L=1 over-corrects** (box trails the object under motion). Why: the capture subsystem (a
`UTickableWorldSubsystem` = `FTickableGameObject`) ticks at `LevelTick.cpp:1606`, **before** `UpdateCameraManager` at
`:1621` (comment: "Update cameras last ... after all actors have been ticked"). So `GetActiveViewInfo` at the capture tick
returns the **previous** frame's camera POV (frame N's camera not yet recomputed) — exactly the view that rendered the
pixels `ReadPixels` returns (frame N-1, `r.OneFrameThreadLag=1`). The two 1-frame lags **cancel** → L=0 matches. **"L=0" is
a ring-origin / tick-order convention for "one render-frame back," NOT zero render lag.** Consistency: spatial L=0 and
temporal settle-K (G37) both account for the **same** ~1-frame render lag (coherent). **FPS-invariant:** it is a fixed
frame-COUNT relationship (tick order + 1-frame render lag), so L=0 holds at any framerate by construction — no low-FPS
re-test. The `IAI.Capture.ViewLag` knob + the per-tick view ring stay (default 0) to future-proof the async path (G40),
which re-derives its own L. (m7, 2026-06-20.)

### G42 — under motion a fired actor can leave the viewport mid-hold: `present=True` + `bbox_valid=false` is NOT a visible positive (m7)
`anomaly_present` = "an anomaly is applied in game-state this frame" (game-state truth; it drives the validated temporal
transitions — keep it independent of the projection). Under camera motion a fired actor can leave the viewport **during its
hold**, so `present=true` with every bbox off-screen (`bbox_valid=false`). **These frames are KEPT** (legitimate hard
negatives — anomaly active but not visible), **not dropped**. The detection-relevant positive is the top-level
**`visible_positive` = `anomaly_present && (>=1 bbox_valid)`** written into each JSONL record — consumers filter on it for
the detection set. The in-frustum-but-**OCCLUDED** sub-case (actor on-screen yet hidden behind geometry) is NOT caught by
bounds projection — that's the deferred `GetLastRenderTimeOnScreen` refinement (G22), **now also motivated by per-frame
visible-positive accuracy**, in addition to fire-time visibility gating. (m7, 2026-06-20.)

### G43 — at cold-boot, distinguish COMMITTED vs WORKING-TREE state per track before building a milestone on a "primitive" (process, m7)
m7's cold-boot read the control server's capture primitive (`AnomalyPreview::CaptureGameViewportJpeg`) from the **working
tree** and treated it as the committed baseline. It was actually a **parallel track's uncommitted Slice-1 WIP** (the
committed HEAD had a different Slice-0 `FAnomalyPreviewCapture` class). m7 was then built on top of it, entangling the
milestone with another track's uncommitted work — surfaced only at commit time. **Resolved via Plan A:** a separate commit
promoting the Slice-1 WIP (`ff1be3c`), then m7 path-scoped on top (this commit). **Lesson:** when a repo has multiple
in-flight tracks, at bootstrap run `git status` / `git log` and distinguish committed vs working-tree state **per track**
before treating any "existing primitive" as a stable foundation — a working-tree primitive may be another track's
uncommitted WIP. Standalone-buildability of each commit (no forward-references) is what makes a clean post-hoc split
possible. (m7, 2026-06-20.)

### G44 — debug-draw (line batcher) IS baked into game-viewport `ReadPixels` captures; suppress it during a run (capture)
The poll-radius debug sphere (G34, drawn via `DrawDebugSphere` from `FWorldDelegates::OnWorldPostActorTick`) was
appearing **inside captured dataset frames**. Cause: capture grabs frames with `FViewport::ReadPixels` on the **game
viewport** (`AnomalyPreview::CaptureGameViewportRaw`), which returns the final **composited backbuffer** — and engine
debug-draw (the world `ULineBatchComponent`) renders into the scene, so it is part of that surface. There is **no
readback-time show-flag exclusion** on the `FViewport::ReadPixels` path; you get exactly what the player sees, debug
overlays included. (Contrast `UDebugDrawService` *canvas* HUD text, which is a separate 2D pass — but the sphere is a
3D line-batcher draw, so it IS in the readback.)
- **Fix = capture SUPPRESSES the visual, not the feature (Option B).** A tiny additive, cull-independent toggle in
  `AnomalyViewport` — `SetDebugSphereSuppressed(bool)` — gates only the sphere draw hook; the poll-radius **cull**
  (`GetVisibleRenderableActors`/`...Infos`) never reads it, so the renderable-visible SET is unchanged. `SetPollRadius 0`
  was NOT an option (it would disable the cull too — both were gated on `GPollRadius > 0`).
- **Lifecycle:** `UAnomalyCaptureSubsystem::StartRun` suppresses; `FinishRun` restores. `FinishRun` is the single
  run-exit (Stop, burst-count auto-finish, and `Deinitialize` teardown all route through it), so the live sphere always
  comes back. Mirrors the A1/A2 auto-injector coordination (`a12d16e`). Set-at-StartRun (processed before that frame's
  `OnWorldPostActorTick`) means no sphere leaks into even the first pre-roll frame.
- **General lesson:** anything you draw via `DrawDebug*` / the line batcher will land in a game-viewport pixel capture.
  A capture/dataset path must suppress dev debug overlays for the duration of a run (or capture from a separate view
  with its own ShowFlags). (Capture follow-up, 2026-06-21.)

### G45 — cook guarantee for plugin Content: a soft-ref does NOT cook; a CDO hard-ref does (m8)
`missing_texture` ships the plugin's **first** `Content/` asset (the checker material). Getting a plugin-owned asset to
**cook into a packaged build** is non-obvious:
- A bare `FSoftObjectPath` / `TSoftObjectPtr` is just a path string — the cooker does NOT follow it, so the asset is
  **omitted** from the package and `LoadSynchronous` returns null at runtime (silent no-op). Soft-ref alone is insufficient.
- **The working mechanism (Option B, plugin-self-contained):** a `static ConstructorHelpers::FObjectFinder<UMaterialInterface>`
  in the `UAnomalyInjectorSubsystem` constructor assigning a **non-transient `UPROPERTY() TObjectPtr<>`**. The CDO then carries
  a hard reference the cooker follows → the asset cooks. (`UPROPERTY` must NOT be `Transient` — a transient ref isn't
  serialized into the CDO, so the cooker won't see it.) No host `DefaultGame.ini` / `DirectoriesToAlwaysCook` edit — the
  guarantee lives entirely in the plugin (a host-config line is forgettable in a fresh project → silent cook failure).
- Also required: flip the `.uplugin` `"CanContainContent": true` (was false) so the `/AnomalyInjector/` content mount registers
  (needs an editor restart to take effect). Empirically validated: cooked `.uasset` present + the CDO refs resolve **non-null at
  runtime in a packaged Development build** (a temporary `Initialize` log proved it; removed before commit). (m8, 2026-06-21.)

### G46 — `SetMaterial` per-component override = clean object isolation; revert needs the override-flag (m8)
`missing_texture` swaps a mesh component's material slots via `UMeshComponent::SetMaterial(i, M)`, which writes the
component's **`OverrideMaterials`** array — a **per-component** override, never the shared `UStaticMesh`/`USkeletalMesh` asset
or the source `UMaterial`. So two actors sharing one mesh+material are isolated: recoloring one never bleeds to the sibling
(gate-verified on `SM_RockFlats_02`/`M_Rock`). **Exact revert needs two things captured per slot:** the original
`UMaterialInterface*` (`GetMaterial(i)`) **and** whether it was an explicit override (`OverrideMaterials.IsValidIndex(i) &&
OverrideMaterials[i] != nullptr`). On revert: if it WAS an override, `SetMaterial(i, captured)`; else `SetMaterial(i, nullptr)`
to clear the override back to the asset default. Both branches validated — the StackOBot Bot's skinned slots hold runtime MIDs
(override → restore ptr); placed props use asset-default materials (no override → clear to null). `OverrideMaterials` is a
public member of `UMeshComponent`. Static and skeletal are identical here (both derive `UMeshComponent`). (m8, 2026-06-21.)

### G47 — the cook runs on EDITOR binaries: rebuild the editor target before cooking, or a CDO/content change is invisible (m8)
The first `missing_texture` package built clean (exit 0) but contained **no material** — the cook commandlet runs in
`UnrealEditor-Cmd` (the **editor** target), and the editor binaries had NOT been rebuilt with the new CDO `FObjectFinder`
(the change existed only as an in-memory Live Coding patch, lost when the editor closed; the package build used `-nocompileeditor`).
So the cook ran against old code with no material reference. **Fix:** `Build.bat StackOBotEditor …` (or otherwise refresh the
editor DLLs) **before** `RunUAT BuildCookRun`. After rebuilding the editor, the material cooked. Lesson: a CDO/content reference
only cooks if the binaries the **cook commandlet** loads contain it — that's the editor target, not the game target. (m8, 2026-06-21.)

### G48 — UE 5.1 packages cooked assets into IoStore (`.ucas`/`.utoc`), NOT the `.pak`; verify by runtime load (m8)
After the editor rebuild, `UnrealPak -list` on `StackOBot-Windows.pak` showed only the plugin's `.uplugin` — no material — yet
the packaged game **loaded the material fine** (the subsystem CDO ref resolved non-null at runtime). Reason: UE 5.1 default
packaging uses **IoStore** — cooked asset data lives in `StackOBot-Windows.ucas` (+ `.utoc` index), and the `.pak` carries only
loose files (`.uplugin`, ini). So a missing-from-`.pak` asset is NOT missing from the package. **Verify packaged content by
running the build and checking a runtime load** (a log line / actual use), not by `UnrealPak -list` on the `.pak`. (m8, 2026-06-21.)

### G49 — a material swapped onto arbitrary meshes at runtime needs ALL mesh USAGE FLAGS pre-authored, or it renders default-gray (m8)
The `missing_texture` material applied cleanly (override set, gate-green on state) but **rendered gray, not the intended colour**.
Cause: **material usage flags.** UE can only add a usage flag (e.g. `bUsedWithSkeletalMesh`, `bUsedWithNanite`) during an
**editor recompile** — at RUNTIME it cannot, so it substitutes the **default gray material** for any component type the material
wasn't compiled for. StackOBot's ramps are **Nanite** (`Invalid material usage for Nanite static mesh … forcing default material`)
and the Bot is **skeletal** (`Material with missing usage flag … skeletal mesh`) — both rendered gray. **Fix:** author the material
with every mesh usage the anomaly can target set true — `used_with_skeletal_mesh`, `used_with_nanite`,
`used_with_instanced_static_meshes`, `used_with_morph_targets`, `used_with_spline_meshes` (settable via `set_editor_property` at
author time; baked into the `.uasset`). This applies to a packaged build too — found only because the visual gate looked at pixels,
not just state. (m8, 2026-06-21.)

### G50 — unlit + emissive lights the Lumen scene ("glow"); a flat colour that must NOT light the room uses Lit base-colour (m8)
The first `missing_texture` look was **Unlit + Emissive** magenta (flat, lighting-independent). Owner feedback: it "reflects
magenta onto surrounding surfaces, like a glowing object." Cause: an emissive surface feeds **Lumen's surface cache** and bounces
its colour onto neighbours (and blooms). `UPrimitiveComponent::bEmissiveLightSource` is NOT the lever — it **defaults false**
(PrimitiveComponent.cpp:321), so setting it false is a no-op; emissive still contributes. There is no clean per-material "exclude
emissive from Lumen" in 5.1. **For a flat colour that renders but does NOT light the scene, use a LIT material with the colour on
Base Color (matte: rough=1, spec=0), emissive=0** — it shades like a normal untextured surface and emits nothing. This is also the
canonical missing-texture look (a flat colour on a lit surface) and more realistic for the dataset. v1 ships the **checker** look
(lit gray/white); the flat-**magenta** variant + a `mode` arg are **deferred** (owner revisiting the look — the lit-vs-uniform
tradeoff). (m8, 2026-06-21.)

### G51 — the two LIVE renderable-visible entry points are NOT a single shared loop; an actor-level cull must go through a shared classifier (screen-coverage)
`GetVisibleRenderableActors` and `GetVisibleRenderableActorInfos` each run their **own** `TActorIterator` loop; before the
screen-coverage cull they shared only the **per-component** worker `FirstRenderableVisibleComponent` (the actor decision was
`!= nullptr` in one, the match in the other). So there was **no single actor-aggregation point** to hang an actor-level cull on.
The poll-radius cull got away with being a per-component param because it's a per-component test; **screen-coverage is actor-level**
(union of the actor's renderable-visible components' bounds), so it needed a real shared actor decision. Fix: a new shared
`ClassifyRenderableVisibleLive(Frustum, ViewProj, …, MinCoveragePct, …, OutFirstMatch)` that **both** loops now call — that is
what keeps `IAI.DumpVisible`'s set-identity assertion true with the cull ON. Two load-bearing properties: **(1) OFF is
byte-identical in result AND cost** — when `MinCoveragePct <= 0` the classifier just calls `FirstRenderableVisibleComponent`
(the old short-circuit), no union pass / no projection, so the default path adds zero traces; **(2) ON does a single union pass**
that yields both the first match and the union bounds (do NOT call `FirstRenderableVisibleComponent` *and then* re-iterate — that
double-traces). Also: `GetVisibleRenderableActors` now builds the VP explicitly and derives the frustum from it (was
`BuildFrustum(View)`) so it passes the **identical** projector `GetVisibleRenderableActorInfos` already used — required for set
identity, and byte-identical because it's the same VP + near=on/far=off flags. Coverage reuses the **clamped**
`ProjectBoundsToScreenRect` (the dashboard/A4 rect projector, already in `AnomalyViewport`) fed the visible-component union —
**not** the m7 `ProjectActorBoundsToScreenRect` (that one is type-only + unclamped, for the label box "where the hole is"); coverage
wants the true clamped on-screen footprint (clamp-before-area). No capture-module dependency, no dedup needed (both projectors live
in `AnomalyViewport`). Console `IAI.SetMinScreenCoverage <pct>` (plain global cmd) + `IAI.DumpCoverage` (tuning diagnostic).
Touch only `AnomalyViewport.{h,cpp}` + docs; no dep / no `IAnomaly` change. (2026-06-22.)

### G52 — the player-visible frame (GAME UI IN) is ONLY at the post-Slate backbuffer, NOT the scene-view-extension after-tonemap (stencil-capture S1)
The contract is "capture the REAL player frame, game UI included, minus only our own dev overlays." The SVE post-processing
pass (`SubscribeToPostProcessingPass` @ `Tonemap`) gives the final 3D color but it is **pre-HUD/Slate** — it drops the game's
UI too. The only UI-inclusive, async-readback-able surface is **`FSlateRenderer::OnBackBufferReadyToPresent`** (render thread,
after Slate composite, before present). Clip to the game-viewport rect via the engine's **FFrameGrabber pattern**: resolve
`World->GetGameViewport()->GetGameViewportWidget()` → `FSlateApplication::FindWidgetWindow` → store the `SWindow*` as
`TargetWindowPtr`, compute `CaptureRect` from the arranged widget's `GetAbsolutePosition/Size`, and in the callback ignore any
window where `&SlateWindow != TargetWindowPtr`. This clips to the PIE viewport sub-rect EVEN in docked PIE → never captures
editor chrome (the m7 backbuffer-grabs-whole-editor trap, G40, avoided). Consequence for later stages: color (backbuffer,
post-Slate) and custom-stencil/depth (mid scene-render, pre-Slate, SVE) are now **two grab points in the same frame**, joined by
the submit `GFrameCounter`. (2026-06-30.)

### G53 — async readback that doesn't stall the game thread: keep ONLY the lock-copy-out on the render thread; encode+write go to a worker (stencil-capture S1)
"Reasoned non-blocking" is not "measured non-blocking." The first cut polled `IsReady()` correctly and never blocked on the GPU,
but it did the **format-convert on the render thread and the PNG encode + file write on the GAME thread** (in the per-tick
drain). Per captured frame that is tens of ms of encode + disk I/O on the game thread; with `positive=8` it's 8 consecutive
hitches, and since **animation is game-thread-advanced, the game-thread stall IS visible animation judder.** Fix: the render
thread does ONLY the mandatory lock-copy-out (`Lock` is IsReady-gated; then a stride-removed `memcpy` of the raw native bytes;
`Unlock`) — no convert, no encode. A thread-pool worker (`Async(ThreadPool)`, `FAnomalyAsyncWriter`) does convert + encode +
write + jsonl-append (append serialized by a lock; counters atomic, mirrored to the game thread). The label RECORD is built on
the **game thread** because it projects the target actor's bounds (UObject access is game-thread-only). Preload the ImageWrapper
module on the game thread so workers never module-load. Only flush at run end, never per frame. (2026-06-30.)

### G54 — `AddOnScreenDebugMessage` has an on-screen LIFETIME; a stop-new-adds gate cannot evict an already-displayed one (stencil-capture S1)
The heartbeat used `AddOnScreenDebugMessage(key, 2.5s, …)`. Gating the ADD when overlays are suppressed stops NEW heartbeats,
but a message added just before a capture run keeps DISPLAYING for its 2.5 s lifetime — leaking onto the lead-in frames. Evict
it actively: call `GEngine->RemoveOnScreenDebugMessage(key)` **every tick while suppressed** (cheap; no-op if absent) so the
lingering message is gone within one frame of the run starting. (2026-06-30.)

### G55 — `ULevelEditorPlaySettings::ShowMouseControlLabel` is a one-shot SHOW-gate, NOT a live toggle (stencil-capture S1)
The PIE "Shift+F1 for Mouse Cursor" hint is shown ONCE at PIE start (`SLevelViewport::StartPlayInEditor`, ~`:4035`) and on
viewport swap (`:4327`), both gated by `ShowMouseControlLabel`. The widget is then persistent and **self-fades** via per-frame
opacity/visibility delegates. So flipping the setting false mid-PIE does NOT hide an already-shown/fading label — a run-start
flip is a no-op for the live label (it only prevents future swap re-shows). The reliable fix is to disable the setting at the
capture subsystem's **Initialize** (PIE-world bring-up, before the viewport shows the label) and restore at Deinitialize →
suppressed for the whole PIE session (an accepted trade for a capture tool). Transient (never `SaveConfig`), `WITH_EDITOR`-only,
needs the `UnrealEd` editor dep (`if (Target.bBuildEditor)`). Relies on Initialize running before the label show (normal PIE
order); if it doesn't hold, a force-hide of the live widget is the fallback. (2026-06-30.)

### G56 — the LAST async-captured frame presents the NEXT frame, after a same-tick end-of-run flush → dropped; a DrainTail phase fixes it (stencil-capture S1)
The final burst's last frame is armed on the tick the run wants to finish, but its backbuffer presents only the NEXT frame —
after `FinishRun`'s bounded `FlushRenderingCommands` already ran — so its readback is never even enqueued and the frame is
dropped ("1 frame did not resolve by run end"). A bigger flush can't help (you can't present a new frame synchronously inside a
tick). Fix: a **`DrainTail` FSM phase** entered instead of finalizing — it ticks `max(10, ViewLag+4)` more frames; the per-tick
`ProcessCompletedFrames` drains the tail as the last presents + GPU readbacks resolve naturally, and `FinishRun` fires the moment
`PendingSnapshots==0`. A clean burst-count run drops ZERO frames. (Manual `IAI.Capture.Stop` keeps the bounded flush and may drop
one in-flight frame on an abort — acceptable.) (2026-06-30.)

### G57 — adaptive non-unity build exposes missing transitive includes a relocated/new .cpp relied on (stencil-capture S1)
UBT's adaptive build uses `git status` to pull CHANGED/untracked files OUT of the unity blob and compile them individually. A
relocated file (e.g. `AnomalyLabelWriter.cpp` moved into the new module) then loses includes a unity neighbor used to provide,
so it fails standalone (here: `TCondensedJsonPrintPolicy` undeclared). The build that "passed" earlier only did so because the
file was still unity-blobbed; the isolation set shifts as files change. Make every file **self-contained** (added explicit
`Policies/CondensedJsonPrintPolicy.h` / `PrettyJsonPrintPolicy.h`). This is also why a clean non-unity compile is a stronger
gate than a unity one. (2026-06-30.)

*(G58/G59 reserved — the unmerged `feature/stencil-capture` branch already numbers its own gotchas up to G59; master
continues at G60 so the rebase can absorb the branch entries without renumbering.)*

### G60 — the console tokenizer delivers a quoted empty placeholder as the LITERAL 2-char string `""` (m10)
`IAI.Capture.Start "" png "" 60 ...` does not give the handler empty strings for slots 0/2 — each `""` arrives as a
2-character string of two double-quotes (and `''` as two single-quotes), so naive `Args[0]` use produces a literal `""`
output dir (`""/session_...`). Any positional console command that supports skip-this-arg placeholders must NORMALIZE in
the parser: treat empty, `""`, and `''` all as "use the default" (the `Slot()` lambda in the Start command). Runtime-verified
2026-07-11: the placeholder run lands in `Saved/AnomalyCaptures/session_<ts>` with an auto seed. (2026-07-10.)

### G61 — targeted fire is visibility-INDEPENDENT and assumes `IAI.SetViewportScoping` OFF (m10)
`TryFireSpecific` targets by `=`-exact name via `AnomalyTargeting` — it does NOT consult the renderable-visible set the
auto-pool draw uses, so a targeted capture can fire on an off-screen actor (frames are then honest hard negatives:
`present=true`, no box). Corollary: with the m4 `IAI.SetViewportScoping 1` toggle ON (non-default), the underlying apply
itself becomes visibility-gated and an off-screen targeted fire will zero-match instead. Targeted capture is specified
for the default scoping-OFF configuration; keep it OFF during capture runs. (2026-07-10.)

### G62 — capture pause/resume of the auto-injector must be CENTRALIZED in StartRun/FinishRun + teardown-guarded (m10)
The m9 shape (WS handler pauses; control-server Tick polls to resume; console path only warns) split the policy across
entry points and broke parity. Centralize: `StartRun` records `bAutoWasRunning` + pauses; `FinishRun` resumes. The resume
MUST be guarded by a `bDeinitializing` flag set at `Deinitialize()` before its `StopRun()` call — PIE teardown funnels
through the same FinishRun, and an unguarded resume would SetRunning(true) on a subsystem in a dying world. With the
guard, teardown finalizes artifacts but never resumes. (2026-07-10.)

### G63 — the pre-run clean slate needs `RevertAllActive()`, not just `RevertAllLiveFires()` (m10)
`Auto->RevertAllLiveFires()` only clears the auto-injector's OWN live fires. Anomalies injected manually (console
`IAI.Apply`, selector, dashboard-era inject) — object, component, or global scope — are tracked by the injector
subsystem, not the auto layer, and would persist into the captured frames as UNLABELED contamination (labels source only
the auto layer's fire list). `StartRun` therefore also calls `Injector->RevertAllActive()` before frame 0. Gate: manual
`missing_object` + `time_dilation`, then targeted start → `IAI.DumpActive` = 0 immediately after StartRun; the session
shows only the targeted anomaly. (2026-07-10.)

### G64 — a game's visible content advances on ONE of two clocks; fixed timestep pins only the GAME clock (m11)
Game-clock-driven content (StackOBot world motion, our anomaly toggles) follows the world's delta seconds — fixed
timestep captures it exactly. Real-time-driven content (sequencer scenes, platform-clock/audio-synced systems — the
client games) advances on the WALL clock regardless of world delta. With fixed timestep alone, each captured frame
then holds however much REAL time the frame took while the annotation claims 1/fps, and the mp4 plays fast by
`VideoFps / sustained_wall_fps` (office: 60/29.3 ≈ 2.05x, with ZERO drops and an exactly-held fixed step — the frames
and labels were never wrong, the two clocks just disagreed). The fix is to make the clocks agree: pace every capture
tick to >= 1/fps of wall time (`IAI.Capture.Pace`, default ON) → game == wall == video, correct for BOTH families;
when the box can't hold the rate, stamp `video.fps` with the SUSTAINED rate (one-sided: never stamp faster-than-target
— that would speed up game-clock content). (2026-07-11.)

### G65 — UE's own frame-rate limiter is BYPASSED under UseFixedTimeStep: any capture pacing must be self-implemented (m11)
`UEngine::UpdateTimeAndHandleMaxTickRate` skips the MaxTickRate/smoothing wait entirely when `FApp::UseFixedTimeStep()`
is on (benchmark semantics) — `t.MaxFPS` does nothing during a capture run, so it can neither pace a run NOR serve as a
gate lever to throttle one. The m11 pacer is therefore the plugin's own drift-free sleep (coarse `SleepNoStats` +
short spin at the top of the capture subsystem's Tick), and it is the ONLY limiter active during a run — no
double-sleep. Measured precision on Windows: 33.2–33.6 ms held against a 33.33 ms target. (2026-07-11.)

### G66 — sustained-fps measurements are skewed by first-run warm-up AND by the editor-backgrounded state (m11)
Two big skews when judging what rate a box "sustains": (1) the first run(s) after an editor boot carry shader/PSO
compilation — the very first m11 gate run measured 2.2 fps sustained on a box that later held 30.0 exactly; warm up
with a throwaway run before judging. (2) A NOT-FOREGROUND editor is throttled: `UEditorEngine::ShouldThrottleCPUUsage`
sleeps ~100 ms/frame when unfocused ("Use Less CPU in Background", `bThrottleCPUWhenNotForeground`) — and that check is
DISABLED while shaders compile, so a cold busy editor can measure FASTER than a warm idle one. A fully OCCLUDED window
throttles presents besides. Capture with the editor foreground/visible (the real workflow); headless/bridge sessions
must foreground the window or flip the preference or their ratios are meaningless. (2026-07-11.)

### G67 — delivery mode suppresses labels.jsonl + run.json → our own QA tools no-op on delivery sessions BY DESIGN (m12)
With `IAI.Capture.Delivery 1` a run writes only the client-facing set (Actual_Frames/ + Video_Clip/ + run_summary.json
+ annotation.json) and never creates labels.jsonl or run.json. Consequence: our host QA tools that read the per-frame
label sidecar — `overlay_watcher.py` (hard-requires labels.jsonl; logs "no labels.jsonl" and skips) and
`tools/verify_capture.py` (overlays boxes from labels.jsonl) — will NOT process a delivery session. This is intended, not
a bug: delivery mode is for client OUTPUT, not our verification. Verify capture correctness in the DEFAULT (delivery-off)
mode; ship in delivery mode. The host `encode_watcher.py` is unaffected — it keys off run_summary.json (its done-signal) +
annotation.json only, never labels.jsonl/run.json, so the client's mp4 still encodes. (2026-07-12.)

### G68 — delivery mode withholds the seed (it lives only in run.json) → a delivered session is not client-reproducible (m12)
The injection seed is written ONLY to run.json (run_summary.json deliberately does not carry it). Delivery mode suppresses
run.json, so a shipped session has no seed and the client cannot deterministically reproduce it. This is an intentional
property, not an oversight — reproduction metadata stays owner-side. If you need a reproducible record of a delivered run,
capture it once in delivery-OFF mode (which keeps run.json + the seed) for your own archive, then re-run in delivery mode
for the client, or note the seed from the STARTED log (it is logged in both modes). (2026-07-12.)

### G69 — the packaged delivery default is a GConfig read at Initialize, NOT a UDeveloperSettings/UPROPERTY (m12)
The delivery default that survives into a packaged Development build (no editor) is read at subsystem Initialize via
`GConfig->GetBool(TEXT("AnomalyCapture"), TEXT("bDeliveryModeDefault"), ..., GGameIni)` — the owner/client sets it in the
PROJECT's `Config/DefaultGame.ini`:
```
[AnomalyCapture]
bDeliveryModeDefault=True
```
Chosen over a UDeveloperSettings subclass to avoid a new module dependency + UCLASS. Two consequences to remember:
(1) GConfig caches the ini at editor/app STARTUP — editing DefaultGame.ini while the editor is running does NOT take effect
until a restart (the value is re-read at the next subsystem Initialize, but from the cached ini). To gate-test the default,
edit the ini then RESTART the editor. (2) The console `IAI.Capture.Delivery <0|1>` overrides the seeded default for the
session and does NOT SaveConfig — the durable default lives only in the ini the owner edits. Absent the key, the code
default (OFF) stands. (2026-07-12.)

### G70 — game-clock content + sub-target sustained ⇒ MUST set ContentClock game, or the mp4 plays ratio× SLOW (m14)
The m11 honest stamp (a run with `speed_ratio > 1.02` stamps the SUSTAINED wall rate) is correct for REAL-TIME-driven
content but WRONG for GAME-CLOCK-driven content (StackOBot world under fixed timestep). Under fixed step every frame is an
exact `1/target` GAME-second slice of motion regardless of how slow the machine ran (proven: labels game t-deltas stay
exactly `1/target`), so the natural playback rate is TARGET; stamping sustained makes the video play
`target / sustained = speed_ratio` times SLOW (owner case: 120 @ 60 on a box sustaining 11.64 → stamped 11.64 → 10.3 s mp4
at 5.16× slow; correct is 60 → 2.0 s natural — the owner's own hand-patch to 60 confirmed it). Fix: `IAI.Capture.ContentClock
game` (or the `[AnomalyCapture] ContentClockDefault=game` ini key) makes game mode stamp TARGET at any ratio.
**Default is `wall`** (RESOLVED m15, 2026-07-13; the member default + the GConfig-absent fallback both resolve to wall). It
briefly shipped as `game` in m14 pending validation; the owner then tested wall vs game on the ACTUAL office machine and
settled it: **the client titles (Until Dawn, Concorde) are WALL-clock** — wall produces correct-SPEED videos for them (their
LENGTH varies with real capture duration, which is correct for wall-clock content; natural playback SPEED is the criterion).
The earlier `Fps` 120/240 "slow motion" was an extreme-forced-ratio artifact, not game-clock evidence. **StackOBot is
GAME-clock** — set `game` in its build's ini. **Do NOT flip the default to game "to be helpful":** for the client's
wall-clock titles that stamps a slow run at target and plays their videos ~`speed_ratio`× (≈2×) FAST = the Issue-2
regression (this is settled fact now, not conjecture). Per-build mechanism: client build = wall default (no action); owner's
StackOBot build = one ini line `ContentClockDefault=game`; nobody types anything at runtime. The stamp is a single fps for
the whole video, so a hypothetical title whose game-clock and real-time layers truly diverge still couldn't be right for
both (a per-clock-layer stamp would be needed) — but the actual client titles are wall-clock, so this is not currently in
play. In game mode a high ratio is only a live-capture perf issue, not a video defect. (2026-07-13.)

### G71 — the control-server token can be a STATIC baked secret via GConfig; owner path stays random (m16)
`StartListening` reads `[AnomalyControlServer] Token` from `GGameIni` (same GConfig-at-read pattern as the delivery-mode /
content-clock defaults — G69). Present + non-empty → that fixed value is the token; absent/empty → the existing
`FGuid::NewGuid()` random per-session token (owner in-editor is BYTE-UNCHANGED — still random, still logged). The client build
sets the ini key AND builds the dashboard with a matching `VITE_CONTROL_TOKEN`, so the dashboard auto-fills + auto-connects with
zero copy-paste. This is a **static shared secret** scoped to the privately-shipped client build (owner accepted the tradeoff:
localhost-only tool, `ws://` ignores CORS so the token still stops an arbitrary web origin from driving the server; worst case
if the two artifacts leak = local viewport-screenshot/injection, no network exposure). Do NOT drop/loopback-bypass auth to
achieve auto-connect — the token is the only thing gating arbitrary local web origins. (2026-07-13.)

**PARTIALLY SUPERSEDED (M2, 2026-07-21) — dashboard side only:** the `VITE_CONTROL_TOKEN` build-time bake
described above is gone; the dashboard now reads its token at startup from a runtime `config.json` served
beside the app. The ENGINE half of this entry (GConfig read, random-when-absent, the security tradeoff and
the do-NOT-drop-auth rule) is unchanged and still current. See **G84**.

### G72 — focus-gated capture start: viewport foreground signal; SKIP when no game window so headless/Simulate can't deadlock (m16)
A capture Start ARMS immediately (clean-slate reverts + auto-pause happen now) but holds the first frame until the game window
has focus, via a new `ECapturePhase::ArmedPending` resolved in `Tick`. Focus signal = `GameViewport->Viewport->IsForegroundWindow()`
(Engine-only `FViewport` API — packaged-safe in Development/Test where capture is compiled in; no Slate dependency needed for the
gate itself, though AnomalyCapture already links Slate in non-Shipping). The gate is applied ONLY when a game window exists
(`GetGameViewport() && ->Viewport`); with no window (headless / MainWorld Simulate over the MCP bridge) it is skipped and the run
begins immediately — so the owner's Simulate smoke-gates do NOT stall waiting for a "focus" that never comes. Two more escape
hatches: `IAI.Capture.FocusGate 0` (session override; packaged default `[AnomalyCapture] bFocusGateDefault`) and a 30 s safety
timeout that starts the run anyway (with a Warning) if focus never arrives. `IAI.Capture.Stop` cancels an armed-pending run, which
deletes the empty session dir and writes NO artifacts (guarded by a new `bRunBegun` — the manifest/StartFrame/fixed-timestep are
deferred out of `StartRun` into `BeginActualRun`, fired at focus-in, so timing/frame-indexing start at the real first frame). (2026-07-13.)

### G73 — one "capture-active" signal (`bRunning`, set at ARM) drives both the focus-gate and preview suppression (m16)
`bRunning` is set true in `StartRun` at arm time (before the focus decision) and cleared in `FinishRun` — so it is true across the
whole arm→armed-pending→running→finish span, NOT just while frames flow. This single flag is the shared signal: (1) `Tick` proceeds
into the armed-pending / phase machine while it is true; (2) the control server's `PushFrames` early-returns while
`Cap->IsCaptureActive()` (== `bRunning`) — so the synchronous per-frame `CaptureGameViewportJpeg` preview generation STOPS the
instant Start is accepted (armed-pending included), engine-side, without waiting for the dashboard to learn `capture.running` over
the snapshot round-trip (the client-side unsubscribe stays as belt-and-suspenders). Only preview FRAMES are suppressed; snapshots
keep flowing so the dashboard still tracks state. The preview simply freezes on its last frame during a capture and resumes when
`bRunning` clears (the client re-subscribes on the next `running=false` snapshot). Because armed-pending already reads as
`capture.running=true`, the preview can't drag the very focus-in moment the fix is protecting. (2026-07-13.)


### G74 — a saved material pointer is NOT a valid restore target on runtime/modular characters (m17)

`missing_texture` originally saved `TWeakObjectPtr<UMeshComponent>` + `TWeakObjectPtr<UMaterialInterface>` at Apply and
restored through both at Revert. That is sound only for content whose materials are shared assets that never die and whose
components are never re-created — i.e. props and StackOBot. On a character whose OWN runtime logic owns its materials
(Concorde's `FWMasterSkeletalMeshComponent`; a merged/modular skeletal mesh assembled at runtime), TWO independent things go
stale during the hold: (1) the component is re-created (outfit/merge/LOD rebuild) and `OverrideMaterials` is copied to the
successor, so our saved component pointer is dead and the corruption now lives on a component we never captured; (2) our own
`SetMaterial(i, checker)` evicted the game's runtime MID from the slot, dropping its only strong reference, so the saved
"original" is GC'd and restores as null. The old revert then silently skipped (dead component) or cleared the override
(dead original) — and reported success either way. **Rule: at revert, re-resolve the live component from the owning actor and
verify the slot before touching it; never treat a saved pointer as the restore target.** Reproduced locally in a package
(`MidReproActor` modes 1/2/3), not only on the client title. (2026-07-15.)

### G75 — only touch a slot that still holds YOUR material; reset dead originals to the mesh default (m17)

Two halves of the same lesson, both load-bearing and both proven by the local repro. **(a) Guard:** a character system
re-asserts its materials on its own schedule, so by revert time the slot may already hold the GAME's fresh material. Restoring
"our" saved original there — or worse, restoring a dead original as null — STOMPS live game state (m16 did exactly this:
observed destroying a live `MaterialInstanceDynamic` and dropping the slot to the asset default until the game's next
re-assert). Check `current == ours` (including a material instance whose PARENT CHAIN reaches ours — a game MID layered over
our checker is still our corruption) and skip otherwise. **(b) Dead original → mesh default, not "leave it":** clearing the
override (`SetMaterial(i, nullptr)`) makes the mesh's built-in default render and lets the owning system re-take the slot on
its next assertion. Do NOT try to make a restore "survive" the character system's next re-assertion — surviving means fighting
the owner; yielding the slot cleanly is the correct outcome. The guard also makes a post-revert sweep idempotent. (2026-07-15.)

### G76 — validate against a LOCAL PACKAGED BUILD, not just PIE (standing rule, m17)

Both first-smoke-test bugs (packaged black preview; missing_texture stuck revert) were invisible in the editor and only
appeared in a package — the editor composites a separate render target for the game viewport (so the preview's `ReadPixels`
works in PIE and returns black packaged), and PIE sessions rarely exercise the runtime-material/component churn that breaks a
saved pointer. **Standing baseline: features are validated against a local package under
`D:\IntrusiveAnomalies\StackOBot\Builds`, not only PIE.** A package can be driven fully headless with no editor and no office
box: `StackOBot.exe -windowed -ExecCmds="IAI.Server.Start, ..."` plus the control server's own WS surface (inject / revert_all /
capture_start) as the driver — a raw-socket client needs no dependencies. NOTE when writing such a client: the control server
sends ALL WS messages as BINARY frames (opcode 2), JSON included — classify by content (`AIF1` magic = a preview frame), not by
opcode. Iterative cook + an exe hot-swap into the archived build makes the edit→validate loop ~1 minute. (2026-07-15.)


### G77 — CLOSED/CONFIRMED (m17): the slot reset HOLDS on a real modular merged-proxy; the map below is now regression-history

**STATUS: CONFIRMED on the real title — closed 2026-07-15 (was OPEN for ~1 day).** The owner pulled + rebuilt `m17` on the
office box and ran `missing_texture` apply → revert on Concorde's **actual `FWMasterSkeletalMeshComponent`**: the body reverts
clean, both for an **immediate** revert and for the **CHURN** case (apply → ~30 s of play, letting the character system
re-create the body component mid-hold → revert) — the case that previously left the hand stuck. **The slot reset STICKS on the
real merged/master-pose proxy: the character system does not re-assert the checker back and does not leave it stuck.** So the
lesson of G74/G75 is validated on a real runtime-assembled modular character, not only on the model — and outcomes 2 and 3
below **did not occur**; they are kept as a diagnostic map in case a FUTURE title regresses this way. (The owner's report is
behavioral; which branch fired — `restored` vs `swept` — is readable from the revert log counters if ever needed.)

m17's revert hardening (G74/G75) was first **validated on a local StackOBot repro** of the mechanism — runtime-MID staleness and
component re-creation, reproduced in a package via the test-only `MidReproActor` (`D:\IntrusiveAnomalies\StackOBot\Source\
StackOBot\MidReproActor.{h,cpp}`, project game module, deliberately NOT in this repo; console `SOB.MidRepro.*`). At the time it
was **not yet confirmed on Concorde's real `FWMasterSkeletalMeshComponent`** — a custom, runtime-assembled merged/master-pose
proxy — which is why the map below was written. The question then open (**now answered: the reset sticks**): does our slot reset
**STICK** on that proxy, or does the character system **re-assert over it** on its next
update? Repro Mode 3 (master + master-posed sub-part, sub-part rebuilt while the master re-asserts) says one revert reaches
both, because the fix enumerates the actor's components **at revert time** rather than trusting the captured set — and the real
proxy has now been shown to behave like the model. **If a title ever shows this corruption again, read the revert log line
first** (`restored=/default-reset=/left-to-game=/unresolved=/swept=/re-found=`) — it names the branch that fired:
- `swept>0` and the hand clears → working as designed.
- The system re-asserts the **checker** back after our revert → the proxy rebuilt from a state snapshot taken BEFORE the
  revert. This is a TIMING problem (re-check after the system's next assertion, or reset on the driven sub-component instead
  of the merged proxy) — do NOT "fix" it by making our restore survive re-assertion; that fights the owner system (G75).
- `unresolved>0` with the hand still corrupted, or **no log activity at all** on the hand's components → the driven
  sub-components are not reachable from the matched actor. Then the place to change is the **per-owner sweep loop in
  `Anomaly_MissingTexture.cpp Revert()`** — the `Owner->GetComponents<UMeshComponent>(Components)` enumeration is the single
  point that decides which live components we may touch — and possibly targeting upstream
  (`AnomalyLod::ResolveLodComponents` → `AnomalyTargeting::FindComponentsMatching<T>` is per-actor, so a sub-part owned by a
  DIFFERENT actor is never captured at all). Not the captured-slot list, which by construction only knows Apply-time components.
(2026-07-15. Opened when m17 shipped repro-validated; **CLOSED the same day** — the owner's office-box Concorde test confirmed
the reset holds on the real `FWMasterSkeletalMeshComponent`, immediate and after ~30 s churn. Retained as a regression map.)


### G78 — the async label stamp must describe END-OF-TICK state; the label span sat one frame LATE at burst boundaries (m18)

**ROOT CAUSE, STATED FOR THE RECORD: an intra-tick ATTRIBUTION off-by-one at the span boundary — NOT a render-thread-lag
inconsistency.** The label's fire state was sampled at a point in `Tick` BEFORE the `BeginFire()`/`BeginRevert()` whose
effect the SAME frame's render includes, so the boundary frame was attributed the PRE-change state. It has nothing to do
with the label side "failing to apply the 1-frame lag that ViewLagFrames=0 applies": **L=0 applies no compensation** — it
means "use the view sampled this tick", which is correct on sync only because that view already IS the previous frame's
camera (a natural lag, not an applied one). The fix required **no lag arithmetic whatsoever**; the rule is purely
intra-tick — *the label must describe the state the frame renders, i.e. the end-of-tick world*. Do not re-tell this bug
as a render-lag story; that story is what sent the first fix attempt in the wrong direction (see below).

**The two capture paths grab DIFFERENT frames, and only one of them pairs with an arm-time label stamp.** Sync
(`CaptureLabeledShot` → `FViewport::ReadPixels`) returns the **previously presented** frame N-1 — that is the whole basis of
the validated L=0 derivation (G41: "exactly the view that rendered the ReadPixels frame; the two 1-frame lags cancel"). The
async backbuffer grab (`FAnomalyFrameCapturer`, the default since the Stage-1 capture work) returns the render of the **arm
tick itself** (frame N). Nothing downstream was re-derived for that shift — G41 even flags the `IAI.Capture.ViewLag` knob as
being "for the future async path", and that re-derivation never happened.

**Symptom (measured, packaged StackOBot, default burst config):** `CaptureCurrentFrame()` samples the label from
`Auto->GetLiveFires()` at mid-tick N, then the phase machine calls `BeginFire()`/`BeginRevert()` LATER IN THE SAME `Tick`.
So the frame armed on a transition tick renders the POST-transition world while its label describes the PRE-transition one.
The label's positive span ran one frame LATE at BOTH boundaries: pixels positive on [3..10], labels positive on [4..11].
- **Fire edge** — last lead-in/post-gap frame: labeled clean, pixels already anomalous = **false negative / contaminated
  negative** (the dangerous one: a "clean" training example containing the bug).
- **Revert edge** — last positive frame: labeled positive, pixels already clean = **false positive**.
Exactly 1 frame per boundary; steady-state fraction **2 / (PositiveFrames + PostFrames)** = **16.7%** at defaults (17/100
measured, 9 FN + 8 FP over 8 bursts). Independent of SettleFrames (settle ticks capture nothing) and of PreFrames.
**The direction is counter-intuitive** — the instinct is "the render thread lags, so pixels change one frame later, so the
label is early". That is WRONG here and was explicitly refuted: a state change made at the end of tick N lands in frame N's
OWN render; the render thread's lag is a THREAD lag, not a state lag. The label was LATE, not early, so the span had to move
one frame EARLIER. Always settle this direction with pixels (`align_check.py`), never by reasoning about thread lag.

**Fix (m18):** the async path arms + stores the snapshot WITHOUT the fire state; `FinalizeArmedLabel()` — the last statement
of `Tick`, after the phase switch — fills in `Fires`/`FireHidden`/`FirePos` from the post-transition world. Because
`anomaly_present`, the per-anomaly bbox/`bbox_valid`, `AffectedFrames` (non-hide `frame_indices`) and `HiddenIndices`
(hide-type `frame_indices`) ALL derive from that one sample, both anomaly families and `visible_positive` shift coherently
with no per-family special-casing. Sync is untouched (already correct). Phase timing and settle-K are byte-unchanged — the
captured frame cadence is identical pre/post fix, only the labels move. **Do NOT "fix" this by shifting the phase transition
or by index-arithmetic on the span**: the former changes when anomalies fire (they fire at the right time already), and the
latter would break the sync path, whose arm-time stamp is correct.

**Still open after m18:** the VIEW half of the same async shift (bbox projected with camera N-1 against pixels from camera N)
is untouched and predicted to be one frame stale under camera motion — the m18 scene has a static camera, so it could not be
measured. If it is confirmed, `IAI.Capture.ViewLag` is the knob G41 reserved for exactly this. Related, also unfixed: the
per-frame bbox is projected from the target's LIVE bounds at record-build time (`BuildFrameLabelRecord` →
`ProjectActorBoundsToScreenRect(View, Actor)`), which on the async path is several ticks after the arm — harmless for a
static target, wrong for a moving one. (2026-07-15.)


### G79 — the preview's viewport ReadPixels is BLACK in any packaged build (it was never editor-gated); tee the preview off the backbuffer (m19)

**Two myths to kill first, because both were believed and both are wrong.** (a) The preview was NOT "editor-only gated" —
there is no `WITH_EDITOR` guard anywhere in `AnomalyPreviewCapture.{h,cpp}` or its call site; the only guard is
`#if ANOMALY_CONTROL_SERVER`, which is =1 in every configuration except Shipping, so a packaged Development build compiled
and RAN the preview. (b) The frames were NOT "never generated" — measured on the packaged build: 117 frames in 20 s
(~5.8/s), frameId incrementing, every JPEG **exactly 15027 bytes**, decoding to a valid 1280×720 image of mean brightness
**0.00**. They were generated, encoded and SENT, and they were BLACK. ⇒ **A "frame counter increments" gate PASSES on the
broken build.** Gate the preview on PIXELS (decode it; assert non-black; assert JPEG sizes VARY — a constant size across
frames is the black-frame fingerprint), never on the counter.

**Cause:** a packaged game viewport renders straight to the swapchain and has NO separate render target, so
`FRenderTarget::ReadPixels` → `GetRenderTargetTexture()` is null → `FD3D12DynamicRHI::RHIReadSurfaceData` hits
`if (!ensure(InRHITexture)) { OutData.AddZeroed(...); return; }` — it ZERO-FILLS and returns, and ReadPixels reports
SUCCESS (`return OutImageData.Num() > 0`). The one-shot `Ensure condition failed: InRHITexture` is the only fingerprint,
and UE ensures fire once per site, so it does not bound how many frames failed. In PIE the game viewport DOES have its own
render target, which is exactly why the editor masked this for weeks. **The same root also makes the SYNC CAPTURE path
(`IAI.Capture.Async 0`) write a 100% BLACK dataset in a package with perfectly plausible labels — still unfixed as of m19.**

**Fix (m19):** the preview tees off the same `OnBackBufferReadyToPresent` stream capture uses. It CANNOT share capture's
grab — m16 suppression makes the two mutually exclusive in time (when capture grabs, preview must be silent, so there is
never a capture frame to tee from), and `FAnomalyFrameCapturer`'s arm-matching and completed queue are single-consumer.
So the preview owns a SECOND `FAnomalyFrameCapturer` instance on the same delegate; the capturer class is unmodified
(that is what keeps capture byte-identical). Both bindings early-out with no arm pending.
**Suppression must gate the ARM, not just the send** — gating only the send leaves the tee grabbing+encoding during a
capture, fighting it. But the pump must still DRAIN while suppressed, or an in-flight `FRHIGPUTextureReadback` armed just
before the capture start never unlocks. Encode goes to a background task (`ConvertTightToBGRA` + `EncodePixels`); the WS
send stays on the game thread (the socket is not thread-safe). No `FlushRenderingCommands` remains in the path.

**Format:** the preview now reuses `AnomalyLabel::ConvertTightToBGRA` (promoted out of a file-local anonymous namespace so
both paths share ONE conversion). Its `default:` branch returns BLACK, so an unhandled swapchain format blackens BOTH the
preview and the captures — meaning any title whose captured PNGs are correct will preview correctly. **If a title ever needs
a new format or an HDR tonemap, `ConvertTightToBGRA` is the single place to add it and it fixes capture + preview at once.**

**Honest perf:** this does NOT speed up capture — m16 already suppressed the preview during captures, so it could not have
been dragging them. The removed ~6 Hz game-thread flush + encode only cost anything OUTSIDE capture (live dashboard use).
On a light scene the difference is inside the noise floor; do not quote a capture-fps win. (2026-07-15.)


### G80 — targeting defaults are ENGINE-authoritative; the dashboard mirrors them and has no defaults of its own (m19)

**Changed defaults (m19):** poll radius `0 → 1800` cm (**18 m**), min screen coverage `0 → 6` %, auto-pool
default-enabled `{missing_object, blinking, missing_texture} → {blinking, missing_texture}`. All three were previously
"OFF/all-on" and are now the shipped starting point, because a **packaged client build with no dashboard must start
correct on the engine side alone**. All three are plain hardcoded constants — `GPollRadius` / `GMinScreenCoveragePct`
(`AnomalyViewport.cpp`, file-scope globals) and `GAutoPoolDefaultEnabled` (`AnomalyAutoInjectorSubsystem.cpp`, consumed
in `Initialize`). They are **NOT ini-backed** (unlike the m12/m14/m16 capture defaults, which use GConfig +
`DefaultGame.ini`); GConfig-backing is available as a follow-up if per-title tuning is ever wanted — the console
commands and the dashboard sliders already give per-session overrides.

**The parity question, answered: there is nothing to keep in parity.** The dashboard defines NO defaults for these —
its sliders and pool checkboxes are pure mirrors of the snapshot (`session.pollRadius`, `session.minScreenCoverage`,
and `auto.pool[id]` ← `Auto->IsAnomalyEnabled(id)` in `ControlSnapshot.cpp`); `store.ts` initialises `snapshot: null`
and `SessionBar` only renders after `everConnected`. So changing the engine constant is sufficient AND is the only
correct move: adding a matching dashboard-side default would create a SECOND source of truth that could silently drift
from the engine. (The `?? 0` fallbacks in `SessionBar` are pre-first-snapshot placeholders, not defaults.)

**UNITS — the two easy mistakes:** poll radius is **centimetres** (Unreal units), so "18 m" is `1800`, and the dashboard
slider is cm (`max=20000, step=100`) rendered through `metres()`. Coverage is a **percent 0-100**, not a 0-1 ratio, so
"6%" is `6.0f`, NOT `0.06` (the slider is `max=100, step=1`). Both new defaults land exactly on a slider step.

**Consequences worth knowing before tuning these — MEASURED on StackOBot's MainMenu (coverage swept with the poll cull
OFF, so each cull is isolated):**

| coverage | eligible targets | Bot (`SkeletalMeshActor_3`) in set? |
|---|---|---|
| 0 % (pre-m19) | 15 | yes |
| **6 % (the m19 default)** | **5** | **yes** |
| 10 % | 4 | no |
| 12 % (first proposal) | 4 | no |
| *poll 18 m alone, coverage off* | **1** | no |

- **The coverage cull is unforgiving near the threshold, and 6% was chosen for exactly that reason.** The Bot occupies
  **9.98 %** of the viewport (independently corroborated by `annotation.json`'s `coverage_ratio: 0.10026` for the same
  actor), so a 10 % or 12 % default **culls the hero character of the test scene** while 6 % keeps it with ~1.7×
  headroom. When picking this number for a title, measure the smallest thing you still want targeted
  (`IAI.DumpCoverage` prints per-actor coverage ascending) — do not guess.
- **The poll radius, not coverage, is what collapses the MainMenu set to 1 — and that is a MENU-MAP ARTIFACT, not a
  defect.** The cull is measured from the **player pawn** (G34), and a menu map's pawn is nowhere near the menu
  vignette's camera, so everything the camera sees is >18 m from the pawn and gets culled. (Proof: the camera sits at
  `[3837, 2525, 1700]`, ~3 m from the Bot — if the poll origin were the camera the Bot would pass comfortably.) In a
  real gameplay level — the actual use case — the pawn IS the player and the camera is on them, so 18 m is meaningful.
  **Do not tune or judge the poll radius in a menu map.**
- The poll-radius cull compares `Dist(PollOrigin, Bounds.Origin) - Bounds.SphereRadius > R`, i.e. it **subtracts the
  bounds sphere**, so very large actors (skybox / terrain / landscape) are never distance-culled at any sane radius —
  which is why the single MainMenu survivor sits 122 m away despite an 18 m radius.
- If a title's auto-pool starts zero-matching, suspect these two defaults first: `IAI.DumpCoverage` for the coverage
  side, and remember the poll side is pawn-relative.
- A non-zero *default* does **not** register the dev debug sphere: registration only happens inside `SetPollRadius` on an
  OFF→ON transition. Good for client builds (no wireframe sphere), but it means the sphere no longer appears for the
  owner unless the slider is taken to 0 and back. (2026-07-15.)


### G81 — a hide-type anomaly that ticks ITSELF is sampled one game tick stale; and never count transitions in arrival order (m20)

**Two distinct defects in annotation.json's hide-type data, plus one non-bug worth not "fixing".**

**(1) Self-ticking hide state was ONE GAME TICK STALE.** m18 samples the label at the end of the CAPTURE subsystem's
Tick (G78). That is correct for state changed by `Apply`/`Revert`, because those run inside our own Tick
(`BeginFire`/`BeginRevert`) — which is why `missing_object` was always right (m18 G3: 0/100). It is WRONG for an
anomaly that toggles in its OWN tick: `blinking` is ticked by the INJECTOR subsystem
(`AnomalyInjectorSubsystem.cpp:169-173`), a *different* `UTickableWorldSubsystem` that ticks AFTER us, so `IsHidden()`
at our sample point still holds the previous frame's value while the frame renders with the new one. Measured:
`annotation(gframe G) == pixels(gframe G-1)` on EVERY blink edge, every burst (`{4,5,6,10}` recorded vs `{4,5,9,10}`
rendered). At a burst tail this reads as "the last hidden frame is missing" — it is not a tail clip and **there is no
range end to fix**: `frame_indices` is emitted verbatim (`AnomalyLabelWriter.cpp:345-355`). **Rule: any state a foreign
system mutates must be sampled AFTER every subsystem has ticked — i.e. at the top of the NEXT tick, not at the end of
ours.** m20 defers only the hidden sample (`SampleDeferredHidden()`), which is safe precisely because `FireHidden`
never reaches labels.jsonl — it feeds only annotation's hidden set + transition count. Check that property before
moving any other sample.

**(2) Transition counting was ORDER-DEPENDENT → spurious "flicker".** `Transitions` was accumulated incrementally via
`LastHidden` in ARRIVAL order, but frames do not arrive in session order: `Drain_RenderThread` iterates `InFlight` in
REVERSE (`AnomalyFrameCapturer.cpp:128` → `:164`) while `PopCompleted` is FIFO (`:184`), so any drain with >=2 ready
frames (routine at DrainTail / under hitches) delivers them newest-first and invents transitions. **Rule: never derive
an ORDER-SENSITIVE statistic during async accumulation.** m20 stores `TMap<int32,uint8> HiddenByIndex` and derives both
the hidden set and the transition count from the SORTED keys at write time. NB the reported "single vanish → flicker"
did not reproduce locally: `DefaultHz = 5.0f` (6-frame period) means the default 8-frame positive window genuinely
holds ~2 hidden blocks, so "flicker" is CORRECT there — measure the blink rate against the window before calling it a
misclassification.

**(3) THE NON-BUG: annotation.json frame indices are 0-BASED and were already pixel-exact.** A reported "whole-range
shift one earlier" for missing_texture did NOT reproduce: annotation `[3..10]` == pixels == labels, zero disagreement,
and annotation index 3 <-> `frame_00003.png` <-> visibly corrupted. annotation and labels.jsonl are NOT separate paths
— `BuildLabelRecordForSnapshot` and `AccumulateFrameEvents` are ADJACENT LINES (`AnomalyCaptureSubsystem.cpp:800`,
`:802`) fed by the SAME m18-corrected snapshot. The whole pipeline is 0-based (`frame_%05d.png`; encode_watcher.py
docstring :17-18), so a 1-based video player displays index 15 as "frame 16" = exactly the reported +1. **The tell:
pre-m18 the range ran one LATE, which accidentally matched a 1-based reading; m18 made it 0-based-exact and "broke"
that reading.** Shifting it would re-introduce the m18 bug in the client deliverable. **Before treating any frame-index
report as a shift, ask which artifact the "actual" came from, and settle it by opening the two candidate PNGs.**
(2026-07-15.)


### G82 — "next present wins" is a race: arm→present pairing must ride the render command stream; and ratio≈1 masks everything (m21)

**The "-1 frame shift" (labels/annotation reading one earlier than the pixels) was never delivery-mode (refuted),
never content-clock (refuted — the clock only touches the fps stamp, and the dev box was already running wall), and
never an annotation bug (labels.jsonl shifts too). The variable is the CAPTURE RATE REGIME:** paced at a sustainable
rate (speed_ratio ~ 1) = frame-exact; rate-starved (ratio >> 1) OR pacing off (even ratio < 1, running FAST) = every
file's content lags its index by one.

**Root:** `ArmForCapture` added the arm to `PendingArms` directly on the GAME thread at a wall-clock moment, and the
backbuffer hook consumed the head arm at whatever present came next in wall-clock time. When the m11 pacer sleeps
(ratio~1), the render thread drains during the sleep and presents N-1 BEFORE the arm -> the next present is N ->
correct; that sleep was the only thing making the pairing right. Without it, present(N-1) fires after the arm ->
consumed one early. **Measured:** the arm-id -> consume-rtframe delta is d=2 (99%) at ratio~1, d=1 (100%) starved or
pace-off, and MIXES 88/12 within a single starved run -> no fixed-delta rule can ever pair these.

**Fix (m21):** registration is enqueued via ENQUEUE_RENDER_COMMAND, so it is FIFO-ordered after present(N-1)'s
broadcast (enqueued tick N-1) and before present(N)'s (enqueued end of tick N). "Next present wins" becomes
deterministic in every regime. Telemetry (`armRt` at registration vs `rtframe` at consume, printed in the armed-frame
log) proves it: consume-armRt = 0 for 100/100 frames even at ratio 3-5. Post-fix: pace-off and mild overrun
(ratio <= ~1.05) are frame-exact (0/100, previously -1); ratio~1 byte-identical.

**THE DEEPER RESIDUAL THE FIX EXPOSED (open, m22):** under DEEP starvation (ratio >~ 3) the presented backbuffer can
carry a STALE SCENE — a one-time mid-run event permanently shifts presented content one frame behind its index while
the pairing telemetry stays perfect; and render-STATE changes propagate even worse than material changes (an 8-tick
SetActorHiddenInGame window, game-state-proven by the annotation, never appeared in ANY presented frame). No arm-side
pairing can repair content the present never contained; the fix needs a scene-identity marker (the Stage-3 SVE) that
publishes which tick's scene each present carries. Until then: **deep-starved captures are unreliable and
self-identifying — `run_summary.speed_ratio > ~1.05 means do not ship the session; lower IAI.Capture.Fps and
re-capture.**

**Standing validation lesson:** capture-correctness gates must run ACROSS RATIO REGIMES (starved and pace-off, not
just ratio~1) — every m18/m19/m20 validation ran paced+sustainable, the one regime where this whole bug class is
invisible. And when a frame-pairing bug is suspected, settle the direction with PIXELS decoded from the files
(align_check/mode_audit), never by reasoning about thread lag; this is the third time the pixel check overturned a
plausible story. (2026-07-15.)

### G83 — UE 5.1 `INetworkingWebSocket` cannot CLOSE a socket; the error reply is the only bad-token signal (M1)

`INetworkingWebSocket` (5.1, `Engine/Plugins/Experimental/WebSocketNetworking`) exposes `Send`/`Tick`/`Flush`/
`SetXCallBack`/address getters and **no `Close` or `Destroy`**. The control server therefore has no way to hang up on a
peer: `HandleMessage`'s bad-token branch could only set `Conn.bRejected` and then silently ignore that connection
forever (the 5s `AuthTimeoutSeconds` sweep does the same for a peer that never sends `hello`). The socket stays OPEN
from the client's point of view.

**Consequence:** a client that sent a wrong token got NOTHING back — no reply, no close, no error. A browser client
waiting on the server's `welcome` waits forever, showing "connecting/authenticating" indefinitely, and a **token
mismatch is indistinguishable from a dead server**. This is exactly how the m16 baked-token footgun presented in the
field: the dashboard looked broken rather than mis-configured.

**Fix (M1, `3a46c1f`):** the bad-token branch now sends `{type:"error", code:"bad_token", message:"token rejected"}`
via the same `SendJson` as `welcome`, *before* setting `bRejected`. It is additive and backward-compatible (older
clients ignore unknown message types), so no `ControlProtocol::Version` bump. The no-hello auth-timeout branch is
deliberately unchanged — that peer never identified itself and may not speak our protocol.

**Client-side corollary (do not remove it):** because the server cannot close and a wrong-token peer is simply
ignored, the dashboard's **welcome timeout is load-bearing, not belt-and-braces**. The error reply covers "the server
is running and rejected me"; the timeout covers everything else (server wedged, message lost, an older server build
with no error reply). The dashboard implements both and lands on one distinct `auth_failed` state, with no auto-retry.
**Any future client for this server needs its own response timeout — never assume a reply will arrive.** (2026-07-21.)

### G84 — ship client config as a RUNTIME file, not a build-time bake; and the four traps found doing it (M2)

**The rule:** a value the client must have (here: the control-server token) must not be compiled into an
artifact from a **gitignored** source. The m16 design baked `VITE_CONTROL_TOKEN` from `.env` into the JS
bundle, so assembling a delivery from a clean checkout produced a **silently tokenless dashboard** — present,
launching, looking correct, unable to connect — and changing the token meant a rebuild. M2 moved it to a
`config.json` served beside the app, read at startup: hand-editable, visible, no rebuild, and a missing or
wrong value is a file you can open. **No env fallback was kept** — two sources of truth for one secret is the
confusion being removed. Degrade, never hang: absent/malformed config → the manual connect screen (the
rejected-token case is G83's `auth_failed`).

**Trap 1 — a UTF-8 BOM cost the shipped token.** `Setup.bat` stamps `capturesRoot` into `config.json` while
preserving `controlToken`. Windows editors (Notepad; PowerShell `Set-Content -Encoding utf8`) write a BOM;
`json.load` on a BOM'd file raises, and the naive `except → start from {}` fallback then **rewrote the file
without the token**. Read with **`utf-8-sig`**, and never let "unreadable" silently mean "discard": the
unparseable file is preserved as `.bak` with a loud warning. Same defence client-side (strip a leading BOM
before `JSON.parse`). *Any config a human may edit will eventually arrive with a BOM.*

**Trap 2 — `python -m http.server` resolves MIME from the WINDOWS REGISTRY.** A machine whose
`HKCR\.js` carries `Content Type=text/plain` serves JavaScript as text/plain, the browser refuses the ES
module, and the app is **blank** — on the client's box, where we cannot debug it. Our box is clean (no `.js`
Content Type; Python 3.13 serves `text/javascript`), which is exactly why this is a trap: *it passes here and
fails there.* Force types via `extensions_map` (consulted before the registry lookup). Don't gamble on a
machine you cannot inspect.

**Trap 3 — static servers cache; rewritten config keeps serving stale.** `http.server` sends
conditional/cacheable responses, so a `config.json` updated by a re-run of `Setup.bat` can keep serving the
OLD token until a hard refresh. Send `Cache-Control: no-store`. This bit during gating: a cached `index.html`
served an earlier bundle and briefly disguised a gate result (the give-away was the asset hash in the DOM).
**When verifying a static build, assert on the served asset hash, not just "the page works".**

**Trap 4 — two cmd.exe parsing hazards in the launchers.** (a) An `echo` containing **parentheses inside an
`if (...)` block** closes the block early → `. was unexpected at this time`; rephrase or escape as `^(`.
(b) `timeout /t N` **aborts when stdin is redirected** ("Input redirection is not supported") — use
`ping -n N 127.0.0.1 >nul` for a redirect-safe wait. Also note `.bat` files need CRLF (this repo enforces it
via `.gitattributes`; an editor writing LF breaks multi-line blocks). (2026-07-21.)

### G85 — wrapping the dashboard as a Tauri v2 desktop app: five lessons (M3)

Wrapping the web dashboard in a **Tauri v2** shell (double-click `Dashboard.exe`, system WebView2, ~8 MB,
no bundled Chromium) so a client needs no browser and no served build. The plugin/protocol/WS are untouched;
these are the non-obvious traps.

**1. `frontendDist` is COMPILED INTO the exe — never put `config.json` in `dist/`.** Tauri embeds the whole
frontend build into the binary. If `dist/config.json` is present it is baked in, silently re-creating the
M2 build-time-token footgun (G84). Two defences, both used: a `build:tauri` script deletes `dist/config.json`
before `tauri build`, AND the app never `fetch`es config in a Tauri build — a narrow `#[tauri::command]
read_config` reads `<exe_dir>/config.json` off disk (the loose, editable, runtime file). Detect the Tauri
context with `'__TAURI_INTERNALS__' in window` (guard `typeof window` so Node unit tests stay on the fetch
path). Proof it's not baked: delete the loose file → the app opens its manual connect screen.

**2. A set CSP must explicitly allow the control-server WS or it dies silently.** With
`app.security.csp` non-null, `connect-src` must list `ws://127.0.0.1:8077` (we also add `ipc:
http://ipc.localhost` for Tauri's own IPC, and `img-src blob:` defensively). The preview uses
`createImageBitmap(Blob)` directly (no `blob:` URL) so `connect-src` is the real requirement — but verify
live: a wrong CSP shows a rendered-but-disconnected app, not an error. Ours needed no loosening.

**3. Least privilege: your OWN commands need no capability grant.** A custom command registered via
`invoke_handler` is callable from the main window without any entry in `capabilities/*.json` — that gates
*plugin* commands (fs/shell/…), which we deliberately do NOT use. `read_config` + the scaffold's default
capability is the whole surface.

**4. Portable exe = `target/release/<productName>.exe` with `bundle.active:false`.** `tauri build` always
compiles the release binary; `bundle.active:false` just skips the MSI/NSIS packaging. Ship that raw exe in
the delivery folder (no installer, no Program Files/UAC). It is unsigned → first launch trips SmartScreen
("More info → Run anyway", once) — documented for the client; code-signing is out of scope.

**5. WebView2 + Rust MSRV.** The app renders in the system WebView2 (default on Win10 21H2+/Win11); detect
via `pv` under `HKLM\SOFTWARE\WOW6432Node\Microsoft\EdgeUpdate\Clients\{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}`
(+HKCU), silent-install the Evergreen bootstrapper with `/silent /install`. Tauri v2 needs **Rust ≥ 1.77.2**
(a stale 1.74.1 must be `rustup update stable`d). ⚠ **The silent-install path is UNTESTED against a machine
actually lacking WebView2** — the dev box has it and was not uninstalled; first WebView2-less client is the
watch-item (in PRE-DELIVERY-CHECKLIST §4). (2026-07-21.)

### G86 — a Visual Studio update silently invalidates the whole UBT cache AND breaks UE 5.1; pin `CompilerVersion`

Symptom, in this order: (1) a build that should be incremental suddenly queues **~2500 actions**; (2) it then
fails inside **engine** code, not project code:
`Engine/Source/Runtime/Core/Public/Experimental/ConcurrentLinearAllocator.h(29): error C4668: '__has_feature'
is not defined as a preprocessor macro` (plus C4067), preceded by
`Detected compiler newer than Visual Studio 2022, please update min version checking`.

Two independent facts, both worth knowing:
- **The compiler version is part of UBT's action key.** When VS auto-updates the MSVC toolset, every cached
  action key changes, so UBT rebuilds the entire engine even though no source changed. **The huge action
  count is the tell** — it is not your code, and re-running does not help.
- **UE 5.1 predates MSVC 14.42.** `__has_feature` is a Clang builtin; 14.42 does not define it, and 5.1's
  Core header uses it unguarded under warnings-as-errors. The engine cannot compile on that toolset.

**Fix — pin the toolchain** in `%APPDATA%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml`:
```xml
<WindowsPlatform>
  <CompilerVersion>14.38.33130</CompilerVersion>
</WindowsPlatform>
```
(Installed on this box: 14.36.32532, 14.38.33130, 14.42.34433; **14.38 builds 5.1 clean**.) ⚠ The pin does
**not** undo the cache invalidation — one full ~2500-action engine rebuild still runs; it just now *succeeds*.
Budget that wall time after any VS update.

**Companion trap seen the same session:** with the editor still open (~6 GB), UBT saw ~500 MB free of 24 GB
and self-capped to **one** parallel action (`Requested 1.5 GB free memory per action, 1.18 GB available`),
turning that rebuild into hours. **Close the editor before any large build** — and note the link would have
failed anyway on the loaded DLL (the Live-Coding class of failure). (2026-07-29.)
