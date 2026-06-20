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
`#include "Anomalies/Anomaly_Flicker.h"`. Public headers (`IAnomaly.h`, `AnomalyTargeting.h`)
include bare. (2026-06-09.)

### G11 — `SetGlobalTimeDilation` is clamped by WorldSettings (M1)
`UGameplayStatics::SetGlobalTimeDilation` is clamped to `AWorldSettings` `MinGlobalTimeDilation` /
`MaxGlobalTimeDilation` (defaults ~0.0001 .. 20). Extreme scales are silently clamped, so the
`time_dilation` anomaly reads the value back after setting and warns if it differs from the request.
Revert restores the **captured pre-Apply baseline** (AMB-3 ruling — overrides the brief's literal
"set back to 1.0"), so a non-1.0 game baseline is preserved; it falls back to 1.0 only if nothing
was captured. (2026-06-09.)

### G12 — Actor-scoped anomalies share the single `bHidden` flag (last-writer-wins) (M1)
`missing_object` and `flicker` both drive `SetActorHiddenInGame` on their targets. If two such
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
  invariant **subsumes BOTH conflict groups** — bHidden (`missing_object`/`flicker` both `SetActorHiddenInGame`) AND
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
object-scoped anomalies are hide / flicker / forced-LOD, none of which produce a meaningful, labelable corruption on a
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
- **Debug sphere (dev):** when `R > 0`, a `UDebugDrawService("Game")` delegate draws the radius as a yellow sphere
  centered on the LIVE pawn, re-resolved **every frame** (never cache a position at registration — the pawn moves).
  Registered/unregistered on the OFF↔ON boundary by `SetPollRadius` (G25 hygiene). Like all `UDebugDrawService` HUDs
  it draws **only in real Play** (non-editor game view), not a Simulate/editor viewport. Accepted minor: a module
  unload *while a radius is set* leaks the handle (a teardown hook would mean touching the module `.cpp`, out of this
  fix's "touch only AnomalyViewport" scope) — fine for a dev-only viz.
- **`DrawDebug*` from a `UDebugDrawService` delegate needs a POSITIVE LifeTime (persistent batcher), NOT -1.** The
  delegate fires during the **post-scene** canvas/HUD draw — after the world line batcher has already rendered for
  the frame. A one-frame line (`LifeTime <= 0`, the per-frame batcher) queued there is cleared before the next
  frame's scene pass and **never renders** (the sphere shipped invisible for exactly this reason). A small positive
  lifetime routes the lines to the **persistent** batcher, which survives into the next scene pass; re-added every
  frame the shape is continuously visible (~1 frame latency). We use `max(2 * World->GetDeltaSeconds(), 0.05)` to
  refresh with minimal motion smear while surviving low framerates. **Contrast:** the selector's `DrawDebugBox`
  uses `LifeTime=-1` fine because it is drawn from the subsystem **Tick** (PRE-scene-render); a free-function
  helper like `AnomalyViewport` has no Tick, so the persistent-batcher route is the correct fix. (Fixed 2026-06-20.)
- **No new dep** (`IConsoleManager`/`FVector` = Core; `UDebugDrawService`/`DrawDebugSphere`/`APawn` = Engine). (Viewport
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
`missing_object`/`flicker` **hide** the actor, so by capture time it has left the renderable-visible set
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
