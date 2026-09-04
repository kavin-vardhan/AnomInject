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
settled it: **the client titles (Bates, Concorde) are WALL-clock** — wall produces correct-SPEED videos for them (their
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

### G87 — a packaged StackOBot run ALWAYS boots MainMenu, and MainMenu looks exactly like gameplay

`Config/DefaultEngine.ini`: `GameDefaultMap=/Game/StackOBot/UI/MainMenu/MainMenu.MainMenu`
(`EditorStartupMap` is MainWorld, which is why PIE and a package disagree).

⚠ **The trap is not the config line — it is that StackOBot's MainMenu is a full 3D scene** with the Bot
posed in a lit environment. A capture from it is visually indistinguishable from gameplay. A whole turn was
lost to exactly this: a captured frame was declared "a REAL GAMEPLAY LEVEL" from the picture, while the
`annotation.json` from the same session already contained
`/Game/StackOBot/UI/MainMenu/MainMenu.MainMenu:PersistentLevel.StaticMeshActor_0`. The owner caught it.

**RULE: check the level NAME, never the picture.** `CaptureBench.Probe 1` now emits it every tick:
`PROBE,postactortick,gfc=…,level=MainMenu,actors=38`.

**The redirect is ACTIVE, not a startup race — do not try to out-run it.** Measured:
- `StackOBot.exe /Game/StackOBot/Maps/MainWorld` → `LoadMap MainWorld` then immediately `LoadMap MainMenu`.
- Deferred `UGameplayStatics::OpenLevel` after the menu settles → travels to MainWorld, then bounces back
  to MainMenu. (`GEngine->Exec(World,"open …")` does nothing at all in a packaged build — no LoadMap is
  even attempted; `OpenLevel` is the API that works.)
⇒ Every "get in early / get in late" approach is dead. The title pulls MainWorld back to the menu.

**Consequence for the record:** every packaged validation this project ran before 2026-08-06 — the m22
gates, the S1 packaged A/B matrix, B3' — necessarily ran in MainMenu. There was no way to leave it. Treat
any packaged measurement from that era as menu-bound until argued otherwise. (2026-08-06.)

---

🚨 **CORRECTION, 2026-08-19 — THE OBSERVATION STANDS, THE MECHANISM DOES NOT. `MainWorld` IS NOT IN ANY
STAGED BUILD, AND THAT ALONE EXPLAINS EVERYTHING ABOVE.**

The container index of every staged build was read directly (UTF-16 strings in `StackOBot-Windows.utoc`):

| build | exe | cooked maps |
|---|---|---|
| `Builds\BenchGate` (m25) | `101AFEA4` | `CB_GateLevel`, `Entry`, **`MainMenu`** |
| `Builds\MidRepro` | `3814E080` | `Entry`, **`MainMenu`** |
| `Builds\Windows` | `B3A49D82` | `Entry`, **`MainMenu`** |

**`MainWorld.umap` is in NONE of them. Neither is any `Structures/Struct_00*`.** `Builds\Windows`'
exe is dated the same day G87 was written, so it is almost certainly the build G87 measured — and it
contains two maps, neither of them MainWorld.

⇒ `StackOBot.exe /Game/StackOBot/Maps/MainWorld` → `LoadMap MainWorld` **fails because the map is not
in the pak**, and the engine does what it always does on a failed browse: **falls back to
`GameDefaultMap`**, which is MainMenu. That is the "immediately loads MainMenu" in the log. It is
**engine fallback, not an in-game redirect.**

**Corroboration from source — an exhaustive search for the redirect found that it does not exist:**

- the **only** `OpenLevel` in the framework is in **`HUD_MainMenu`**, and it travels **MainMenu →
  MainWorld** (the Play button, with `BPW_LoadingScreen`). Nothing travels the other way.
- `GI_StackOBot` is a **save-game manager** — `InitSaveGame` / `GetCurrentLevelName` / orb persistence.
  Its `LevelName` is a **save-slot key**, not a travel target. No `OpenLevel` anywhere in it.
- `GM_InGame` carries only `ReceiveBeginPlay`. `MainWorld.umap`'s level blueprint (`MainWorld_C`)
  carries no `OpenLevel` / `MainMenu` / travel strings at all.

⛔ **WHY THIS MATTERS MORE THAN A WRONG DETAIL: G87's conclusion FORECLOSED A ROUTE THAT WAS NEVER
CLOSED.** It reads *"Every 'get in early / get in late' approach is dead. The title pulls MainWorld back
to the menu"*, and on that basis MainWorld was treated as unreachable for ~13 days. The real situation
is a **cook-scope** problem with a known fix (cook the map in), not an unwinnable fight with the title.
The deferred-`OpenLevel` result is equally explained: `OpenLevel` to a map that is not in the pak fails
and returns to the default map.

✅ **CONFIRMED BY MEASUREMENT, 2026-08-19 (G-3 probe).** The predicted settlement was run on the
unchanged m25 binary `101AFEA4` — one launch, one log read, **no cook and no re-stage**:

```
LogNet:       Browse:  /Game/StackOBot/Maps/MainWorld?Name=Player
LogLoad:      LoadMap: /Game/StackOBot/Maps/MainWorld?Name=Player
LogStreaming: Warning: LoadPackage: SkipPackage: /Game/StackOBot/Maps/MainWorld
              - THE PACKAGE TO LOAD DOES NOT EXIST ON DISK OR IN THE LOADER
LogLoad:      Error:   Failed to enter /Game/StackOBot/Maps/MainWorld:
                       Failed to load package '/Game/StackOBot/Maps/MainWorld'.
LogExit:      Exiting.
```

**"The package to load does not exist on disk or in the loader."** The map is not in the build. The
"active redirect" is **confirmed to be nothing of the kind**. Log banked at
`_bench_sessions_bank/G3_MAINWORLD_BROWSE_PROBE/`.

⚠ **One difference NOT claimed as a further correction:** under `-unattended` this build **exits**
rather than falling back to MainMenu, where G87 recorded a fallback. That is plausibly an
`-unattended` artifact and was not isolated. **The CAUSE is what is settled**, and the cause is the
missing package either way.

**The re-cook that would put MainWorld in a build is THE SAME re-cook `G118` closure needs** — see
G118's sequencing note. Two debts collapse into one operation, and both retire `101AFEA4`.
→ **G120**.

### G88 — a loose `Config/DefaultEngine.ini` next to a package is IGNORED; the cooked config in the pak wins

Attempting G87's workaround by creating
`<Package>/StackOBot/Config/DefaultEngine.ini` with
`[/Script/EngineSettings.GameMapsSettings] GameDefaultMap=…/MainWorld.MainWorld`
had **no effect whatsoever** — `LoadMap` went straight to MainMenu and MainWorld was never attempted. The
directory did not previously exist; creating it changes nothing. Cooked config lives inside the `.pak`/
`.ucas` and takes precedence.

**It fails SILENTLY** — no warning, no error, the run just behaves as before. Easy to believe the override
"worked" and misattribute the resulting measurements. To change a packaged default map you must change the
project config and **re-cook**. (2026-08-06.)

### G89 — the CaptureBench gate level: a synthetic, owned, deterministic scene (F4)

Because of G87/G88 there is no way to gate in a real StackOBot gameplay level from a package without a
re-cook, so the ratio/keying gate matrix uses a **purpose-built bench level** instead.

- Authored **headlessly and reproducibly** by
  `Plugins/CaptureBench/tools/make_gate_level.py`, run via
  `UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script="<path>"`. Re-run it to rebuild the level from
  scratch; do not hand-edit the map.
- Asset: `/Game/CaptureBenchGate/CB_GateLevel`.
- Contents, chosen against the gate constraints: a 6×6 deterministic grid of static-mesh targets
  (Cube/Sphere/Cylinder/Cone, fixed positions — **no randomness anywhere**), a floor, one **skeletal** target
  (`SKM_Bot`), a static directional light + sky light (no time-of-day drift), and a stationary `PlayerStart`
  framing the grid, so camera motion is a variable we introduce deliberately rather than inherit. Targets sit
  clear of the marker strip's top band.
- ⚠ **SCOPE — do not mistake this for a general validation environment.** It exists to measure
  **frame↔label alignment across ratio regimes**, which is a property of the capture path, not of scene
  content. Work where scene content is the subject — the invisible-anomaly / selection-quality track — needs
  REAL scenes and a synthetic level would be the wrong instrument. (2026-08-06.)

**How to build and reach it.**
- Author: `UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script="Plugins/CaptureBench/tools/make_gate_level.py"`.
  The script deletes and rebuilds the asset, then **asserts** the asset exists and that the CB_-prefixed
  actor count equals what it just authored, raising loudly on mismatch — because a failed author run once
  **presented as success while leaving the OLD `.umap` in place** (see the API traps below).
- Cook: `RunUAT BuildCookRun -project=… -platform=Win64 -clientconfig=Development -cook -stage -pak -archive
  -archivedirectory=Builds\BenchGate -build -map="/Game/CaptureBenchGate/CB_GateLevel+/Game/StackOBot/UI/MainMenu/MainMenu"`.
  **Cold cook (with a binary rebuild) is the long pole; a WARM content-only re-cook is ~1.3 minutes**, so
  iterating the level by measurement is cheap.
- Reach it by **command-line map argument**: `StackOBot.exe /Game/CaptureBenchGate/CB_GateLevel …`.
  The default map is deliberately NOT changed, so the host project config is never touched (G88).
  Verified it *stays*: 6,443/6,443 ticks reported `level=CB_GateLevel`, zero MainMenu loads. It does not
  suffer StackOBot's MainWorld→MainMenu redirect because it contains no blueprint logic to redirect it.
- ⚠ `Builds\BenchGate` is **NOT deliverable-shaped** — bench-only map, restricted cook. The client artifact
  is `Builds\Windows`. Never send BenchGate.

**Two UE 5.1 Python traps hit while authoring it.**
1. `LevelEditorSubsystem.new_level` **REFUSES if the asset already exists** ("Failed to validate the
   destination") — and the script then exits non-zero **while the old `.umap` survives untouched**. A
   careless "the script ran" reading re-cooks the stale level. Delete the asset first, and assert after.
2. `ADirectionalLight` / `APointLight` / `ASkyLight` have **no** `directional_light_component` /
   `point_light_component` / `sky_light_component` attribute. The accessor is **`.light_component`**.

**The level rendered BLACK at first, and the benchmark hid it.** A script-authored level is never opened in
the editor, so **lighting is never baked**; static/stationary lights + unbuilt lighting render black, and
there was no sky actor so even the background was 0. Measured: mean luminance **0.000**, max **0**,
0.0000% non-black pixels. **Fix = fully MOVABLE lights (no bake needed) + a `SkyAtmosphere`.**
⚠ The trap is that *shading nothing is cheap*: the black level benchmarked at **141.52 fps**, which reads as
"a fast, light level" rather than "a broken one".

**Exposure must be PINNED, and it is not the same problem as overexposure.**
- Auto-exposure is ON by engine default (neither the project nor the engine config overrides it). MEASURED:
  frame-to-frame mean-luminance spread **5.752 → 0.108** (~53×) when pinned. A deliberately deterministic
  level was drifting ~5.75 luminance levels over 12 frames — enough to make luminance readings
  irreproducible run to run.
- Pin with `r.DefaultFeature.AutoExposure 0` + `r.EyeAdaptationQuality 0` on every gate run.
- ⚠ **Auto-exposure was NOT the cause of the blown-out image**: clipping was slightly *worse* pinned
  (58.02% vs 54.69%). That was light intensity. Two separate problems, one of which looks like the other.
- Verified inert against the case that actually matters: hiding a bright object does **not** shift
  whole-frame brightness. Anomaly run vs a zero-fire control (same seed/level/frames, targeted zero-match
  so cadence is identical) diverged by **mean −0.18, max 0.39** on non-event frames vs **mean +1.66, max
  4.67** on event frames — ~12× separation, confined to events.

**Converged light values, and which lever actually moved cost.**
`sun 1.8 lux · 3 movable point lights @2500 cd r1200 · dynamic_shadow_distance 4000 · floor =
WorldGridMaterial`. The sweep that produced them:

| | sun | point lights | shadow dist | floor | cost | clipped |
|---|---|---|---|---|---|---|
| start | 6.0 | 4 @40000 r1800 | 12000 | white | 29.5 ms* | 58.0% |
| attempt 1 | 3.0 | 3 @4000 r1200 | 12000 | white | 29.8 ms* | 26.5% |
| attempt 2 | 1.8 | 3 @2500 r1200 | **4000** | **WorldGrid** | 23.7 ms* | 9.2% |

⚠ **`*` those costs are CONTAMINATED — they were measured with PNG encoding running.** Clean
(capture-only) natural cost is **8.52 ms**. Encode contention is not scene cost. The 18–25 ms convergence
target these numbers were chased against has since been **retired** (see below).
⚠ **Shadow DISTANCE was the cost lever, not the lights** — attempt 1 cut point-light intensity 10× and
dropped one light for **zero** cost change. **ATTRIBUTED, NOT ISOLATED:** attempt 2 moved four variables at
once, so the shadow-distance attribution rests on attempt 1 having already cleared the lights. Anyone
re-tuning cost should know that evidential standing before leaning on it.
**White-on-white:** targets use the plain `BasicShapeMaterial`, so a white floor left them unreadable for a
pixel oracle even at correct exposure. The grid floor material is what makes them legible.

**Content checks are leg-validity conditions, and the visual step is NOT redundant with them.**
Three crude, loud, leg-invalidating checks in `FCaptureBenchCapturer`: luminance floor `mean < 2.0/255`
(black), flatness floor `sd < 5.0` (uniform), clip ceiling `>35%` at ≥250 (blown out). Healthy reference:
the converged level clips **9.2%** against the 35% ceiling — the ceiling was calibrated against a
known-good scene, not picked.
⚠ **The luminance floor PASSED a 58%-blown, unusable frame** — correctly, since it only separates
"rendered" from "did not render". Only *looking at a frame* caught that. **Eyes, then number, then
benchmark** is a required order, not a nicety.
⚠ Portability: all three thresholds assume a deliberately lit, deliberately varied scene. If they ever
migrate toward production, that assumption travels with them and stops being safe.

**A game-thread stall does NOT interact with scene cost — the "heavy gate level" premise is dead.**
MEASURED: `frame_time ≈ stall + ~1.3 ms` (stall 76→77.1, 85→86.3, 99→100.1). The render thread runs
concurrently, so a game-thread busy-wait simply replaces the frame time and the scene never enters. The
**knee therefore sits at ~30–34 ms of stall regardless of scene weight**, not at `(1/VideoFps − natural cost)`.
Cross-check: the model also fits the older MainMenu table away from the knee (60 ms→1.847 vs model 1.84;
105 ms→3.198 vs 3.19) — two different scenes, one model.
⇒ Making the gate level heavier buys nothing for this lever. **The 18–25 ms cost target is RETIRED**; the
level's cost is a *recorded property* (8.52 ms), not a bar to clear. A light gate level is acceptable, and
the game-stall table is **synthetic by nature and irreducibly so** — which is also exactly what makes it
deterministic and repeatable across machines. Both halves are true.
Scene cost can only enter via the RENDER thread, which is why a render-thread lever and a GPU-load leg are
the shapes where level weight would matter.

**⚠ A lever must PROVE it executed.** The render-thread stall was originally called from CaptureBench's
SceneViewExtension, which is only active while CaptureBench's *own* capturer is running — so during
production-capture legs it silently no-opped, and four legs at 20/40/70/110 ms returned a perfect
`ratio 1.000`. That reads exactly like the major finding "speed_ratio is blind to render-side starvation",
and it would have been **wrong**. Levers now carry execution counters (`gameStallFired` / `renderStallFired`)
reported per run. **A lever that never fired proves nothing about the metric it failed to move.**
(2026-08-06.)

⚠ **CORRECTED 2026-08-16 — the counter did not make that catch, and the reason matters.** The rebuild
that moved the stall out of the SVE into an `ENQUEUE_RENDER_COMMAND`, *and the execution counters
themselves*, were committed at CaptureBench `163dd12` **after the render legs had already run** — and the
packaged binary they ran against had not been rebuilt since 2026-08-06 01:33:46. A UTF-16 string scan of
that exe settles it: `CaptureBench.Stall.RenderMs` (the CVar) is **present**, so the knob appeared to
work, while `CaptureBenchRenderStall`, `PROBE,renderstall` and `PROBE,stallcounters` are all **absent**.
⇒ **The render lever's root cause is that the fix was not in the binary under test** (see **G92** — it
had in fact been *compiled*, just never *staged*), and `stall_fired = 0` cannot have been a reading —
there was no counter to read. What actually made the catch was **ratio arithmetic**: at 110 ms a firing
render stall would force `ratio ≈ 3.3` by the concurrency model, and the ratio did not move. Remember it
as **"a counter that never printed is not a counter that printed 0."** (The **game** lever's execution is
proven by stronger evidence than a counter anyway: a busy-wait that never ran cannot move frame time from
33.3 ms to 100.1 ms.)

✅ **CONFIRMED the same day by re-staging and re-running, with ZERO probe edits.** On the freshly staged
package the lever fires immediately — `PROBE,renderstall,fired=780,ms=40.0` at a 40 ms stall, and the
counter logs **zero** lines at a 0 ms stall (negative control). **`speed_ratio` is therefore NOT blind to
render-side starvation** — the hypothesis the dead lever nearly certified as a major finding is refuted by
measurement. Sweep (same A32 conditions as the game table, targeted zero-match, 60 frames):
`0→1.0000 · 10→1.0001 · 20→1.0000 · 30→1.1161 · 40→1.4069 · 70→2.3076 · 110→3.5065`.
**One model covers both levers:** `frame_time ≈ max(1/VideoFps, stall + residual)` with residual
**1.3 ms game / 6.9 ms render** ⇒ knee **32.0 ms game / 26.4 ms render**. The larger render residual is
physical: that thread still renders the scene and services the capture readback (recorded natural cost
8.52 ms). ⚠ **Corollary for reading client telemetry: both levers move the ratio through the same form,
so a client's `speed_ratio` of 1.2 says frame time ≈ 40 ms and says NOTHING about which thread starved.
The metric cannot attribute.** The A41 counter is now **paid for** — its first real observation.
(2026-08-16.)

**UPDATE 2026-08-16 — what the gate level was finally used for, and what it got wrong about itself.**

*Leg classification bands, declared in advance so binning can never become goalpost-moving:*
`nominal [1.00–1.02]` · `mild (1.02–1.10]` best-effort · `client [1.15–1.35]` · `deep ≥2.80` ·
`pacing-off` (own category, ratio recorded not banded). ⚠ **The mild band is not reachable by dialling a
stall** — its window sits inside the 1–2 ms run-to-run noise, and a leg that lands at 1.000 while
*labelled* 1.05 is a **silently passing leg**. (It was covered anyway, by accident, at 1.0558.)

*Two standing rules the gate legs are run under:* every leg states **targeted vs auto-pool and why**
(auto-pool fires concurrent anomaly types and has silently shaped a measurement twice); and the pixel
oracle keys on pixels **INSIDE the target bbox**, never whole-frame stats, because hiding an object also
removes its cast shadow and brightens pixels *outside* the bbox (measured +1.66 mean / +4.67 max at event
frames vs −0.18 / 0.39 on non-event frames).

⚠ **The level is content-deterministic, NOT camera-deterministic — the "stationary PlayerStart" claim
above is wrong in practice.** The pawn spawns above the floor, falls and settles, so:
- the projected bbox **moves** for the first ~0.5–1 s (one leg showed 11 distinct bboxes, settling only at
  captured frame 30);
- whole-frame luminance **ramps 34 → 111** over the first ~16 captured frames (a raw in-bbox luminance
  reading is swamped by this; normalise by the whole-frame mean);
- **the settled camera differs between runs**: five legs rested with the same target at 306×235 px, one
  rested at 178×291 px — at stall 39 vs stall 40, i.e. a **run-to-run bifurcation**, not a function of the
  input. ⇒ **every leg must compute its OWN settled bbox and settle window; never assume a fixed bbox
  across legs.**

⚠ **The m16 focus gate makes leg start time nondeterministic.** Its 30 s timeout fires on some launches
and not others, so some legs capture the warm-up and camera-settle and some begin fully warmed (one leg
sat 31 s before `start_frame`). Legs are therefore **not condition-uniform** unless you set
`IAI.Capture.FocusGate 0` or insert a fixed warm-up delay before starting.

**The marker survives PRODUCTION capture** (checked because production had previously kept a DrawDebug
poll-radius sphere out of saved frames): 60/60 frames decoded, strictly increasing. ⚠ **But the decoder
returns a CONFIDENT WRONG ANSWER on markerless frames** — value `0`, row `105`, spread `95.3`, past its own
`spread ≥ 40` gate, latching onto scene contrast. **A valid marker read is a strictly increasing decoded
series, never a decode count.** Re-decoding the banked calibration session shows exactly that false
signature on all 60 frames ⇒ **the banked stall→ratio legs were run marker-OFF** and carry no
frame-identity evidence of their own (the table itself re-measured fine on the same binary: 99 ms → 3.0027
vs banked 3.004; 39 ms → 1.2148 vs 1.204). (2026-08-16.)

### G90 — the packaged `StackOBot.exe` at the package root is a LAUNCHER; killing it leaves the game running

`Builds\<Package>\Windows\StackOBot.exe` is ~217 KB. The real game is the ~240 MB
`Windows\StackOBot\Binaries\Win64\StackOBot.exe`, spawned as a **separate process**. So
`Start-Process -PassThru` (or anything else that tracks the process it launched) hands you the **launcher**,
which exits almost immediately — and killing it leaves the game alive.

⚠ **This silently contaminates the NEXT measurement**, because the previous leg's game is still competing
for the CPU you are trying to measure. MEASURED: a nominal leg (stall 0, pacing on) read `speed_ratio`
**1.483** with a leftover instance alive; the identical leg on a verified-idle box read **1.0000**. Nothing
in the output says "another instance is running" — you just get a plausible wrong number.

**Rule:** any automated harness must `Stop-Process -Name StackOBot` and then **assert a zero-instance idle
box** before every launch, and kill by name again after. (2026-08-16.)

### G91 — `TryFireSpecific` prepends `=` itself; passing `=Name` silently zero-matches

`UAnomalyAutoInjectorSubsystem::TryFireSpecific` builds its match token as
`FString(TEXT("=")) + ActorName` (`AnomalyAutoInjectorSubsystem.cpp:283`). So the `targetActor` argument to
`IAI.Capture.Start` / WS `capture_start` must be the **bare actor name**. Passing `"=StaticMeshActor_49"`
becomes `"==StaticMeshActor_49"` and matches nothing.

⚠ **It fails quietly and looks like a healthy run**: the capture completes, writes all its frames, and
reports `positive_frames: 0` with `zero_match_bursts` equal to the burst count. Only
`Auto.FireSpecific: '<id>' on '<name>' -> 0 matched.` in the log names the cause.

Related, and the reason the mistake is easy to make: **actor LABELS are editor-only.** A level authored by
script with `set_actor_label("CB_Target_07")` exposes **no such name at runtime** — in a package the
matchable names are `StaticMeshActor_NN`. Pick a runtime target by measurement (`IAI.DumpCoverage`), never
by the editor label. (2026-08-16.)

### G92 — a packaged build can be COMPILED and never STAGED; the archive keeps serving the old exe

The most expensive hour of the render-lever saga was this: the fix **was compiled**. The project-side
`<Project>\Binaries\Win64\StackOBot.exe` was dated **2026-08-16 11:53:58**, thirty-six seconds before the
first leg that was supposed to exercise it, and it contained every symbol. What never ran was
`BuildCookRun`'s **stage/archive** step, so `Builds\BenchGate\Windows\...\StackOBot.exe` went on serving a
binary from **2026-08-06** — and the legs launched *that*.

⚠ **This is worse than forgetting to build, because you have a green build in hand.** UBT reports success,
the CVar you are toggling exists in the old binary too (it predates the change), and the run completes
normally. Nothing anywhere says "you are running a different binary than you compiled."

⚠ **A timestamp check alone can mislead in BOTH directions here.** The archived exe inherits the *compile*
time, not the archive time — after the corrective re-stage this session, `BuildCookRun` compiled nothing
for the `StackOBot` target (4 actions, all `StackOBotEditor`) and the newly archived exe still read
`11:53:58`. A newer-looking file is not proof it was staged, and an older-looking one is not proof it
wasn't.

**Rule (A44): prove the change is PRESENT in the binary under test with a symbol/string scan, not a
timestamp.** Cheap and decisive:

```
python -c "import mmap; f=open(r'<pkg>\...\Binaries\Win64\<Game>.exe','rb'); m=mmap.mmap(f.fileno(),0,access=mmap.ACCESS_READ); print(m.find('MyNewLogString'.encode('utf-16-le'))>=0)"
```

`TEXT()` literals land in the exe as UTF-16LE, so any `UE_LOG` format string, `ENQUEUE_RENDER_COMMAND`
name, or CVar name added by the change works as the probe. Scan for something the change **adds**, and
also for a control string that existed **before** it, so a false negative from a bad scan is visible.

⚠ **AND BEFORE YOU RE-STAGE: the archive step WIPES the package tree, including `<Package>\Windows\
<Game>\Saved\`.** That is where a packaged run writes its capture sessions, so every banked session
under it is destroyed by the fix. Move them out first. The bench sessions for this project now live at
**`D:\IntrusiveAnomalies\_bench_sessions_bank`** (evacuated 2026-08-16, 1347.2 MB, 10 directories:
`CAL CAL2 CaptureBench Config I10 Logs RSW SW T2 T2C`) — that is where the A17/A19 retroactive audit's
raw evidence lives, and it is outside both git repos, so nothing but this note records it.
(2026-08-16.)

---

📏 **MEASURED 2026-08-19 on a FULL `-cook -stage -pak -archive -build` — NOTHING WAS WIPED.** The
MainWorld/G118 re-cook was run with every precaution taken first (baselines evacuated and hash-verified,
bank swept 91 → 100 dirs). Afterwards:

| tree | before | after |
|---|---|---|
| `Builds\BenchGate\…\Binaries\Win64\` leg output dirs | 56 | **56** |
| `Builds\BenchGate\…\Binaries\Win64\*.baseline` exes | 4 | **4, all present** |
| `Builds\BenchGate\…\Saved\` dirs | 23 | **23, incl. `M23B`** |

⚠ **DO NOT READ THIS AS "THE WARNING WAS WRONG."** It is **one** cook, with **this** flag set
(`-cook -stage -pak -archive -build`, **no `-clean`**), archiving **into an existing tree**. The
2026-08-16 wipe was observed and is not being retracted. What is now known is that the archive step is
**not unconditionally destructive**, so the difference is in the flags or the state, and **which** is
not established. **The precaution stays: evacuate first.** It costs one copy and the failure it
prevents is unrecoverable. *(The `Binaries\Win64\` question was open because G92 only ever spoke about
`Saved\`; it is now answered for this flag set and only this one.)*

---

### G121 — a content-only re-cook leaves the EXE HASH UNCHANGED, so the exe hash does not identify the build

The MainWorld + G118 re-cook changed **what the build contains and what it enforces**, and left the
binary byte-identical:

| | before the cook | after the cook |
|---|---|---|
| **exe SHA-256** | **`101AFEA4`** | **`101AFEA4`** — *identical* |
| exe mtime | 2026-08-19 12:32:39 | **2026-08-19 12:32:39** — *identical* |
| cooked maps | `CB_GateLevel`, `Entry`, `MainMenu` | **+ `MainWorld`** |
| enforced control-server token | **`TESTVALUE123`** (placeholder) | **64-char rotated** |
| `StackOBot-Windows.utoc` | 194,996 bytes | **268,036 bytes** |
| `StackOBot-Windows.ucas` | 125,071,408 bytes | **284,469,920 bytes** |

**Same exe hash. Different build. Different maps. Different secret.** No code changed, so `BuildCookRun`
compiled nothing for the `StackOBot` target and the archived exe kept its **compile** time (G92 already
warns that the archived exe inherits compile time, not archive time — this is the same fact biting from
the other side).

⛔ **Consequence, and it reaches backwards: every A44 hash reference in this project identifies only
HALF the artifact.** `CLAUDE.md`'s *"staged exe `101AFEA4` = m25"* was true and is still true, and it
is **no longer sufficient** — two builds now answer to it. A leg banked "against `101AFEA4`" does not
say which content it ran on.

**RULE: a build's identity is `exe hash + pak identity`. Record both.** The cheap pak identity is the
`.utoc` — its size, mtime and hash:

```
StackOBot-Windows.utoc  939B9C9B   268,036 bytes   2026-08-19 17:00:27   (4 maps)
StackOBot-Windows.ucas  8A602D4D   284,469,920 bytes
StackOBot-Windows.pak   7CAE22DD    10,115,703 bytes
```

⚠ **The reverse case is the dangerous one and it is the reason this is a gotcha rather than a note:** a
**code-only hot-swap (G103)** changes the exe hash and leaves the pak alone — so the two halves move
**independently**, and *either* can change while the recorded identity says nothing happened. A
same-hash comparison is not a same-build comparison in either direction.

*(Related: G92 — compiled is not staged; G118/G119 — check what the artifact ENFORCES, not what its
source says. This is that principle applied to the artifact's own fingerprint.)*

**⛔ THE RETROACTIVE CONSEQUENCE, STATED PLAINLY: THE BASELINE CHAIN PRESERVES EXES ONLY, AND AN EXE IS
HALF AN ARTIFACT.** `_binary_baselines\StackOBot.exe.m25-baseline` (`101AFEA4`) **does NOT reconstruct
the build H4 was measured on** — that build's other half (3 maps, `TESTVALUE123`, `.utoc` 194,996 B)
was overwritten by the 2026-08-19 cook and **is gone**. Same for `.s4-2`, `.s4-0`, `.m24`, `.m23`.

⚠ **THE LOSS IS BOUNDED AND THE RECEIPT IS THE G-2 DEBT SWEEP** (journal 045 §30), run **before** the
cook precisely so this could be said: every claim still owed against `101AFEA4` was enumerated and
**the list was EMPTY**. ⛔ **Do NOT attempt to reconstruct any pre-cook pak.** Record it as a bounded
loss, not an open hole.

**A BASELINE IS NOW A QUARTET — anything less is not a baseline:**

```
exe hash + StackOBot-Windows.utoc hash + .ucas hash + .pak hash
```

**The current PATH-(a) MEASUREMENT BUILD is preserved complete** at
`D:\IntrusiveAnomalies\_binary_baselines\` (exe) + `pathA-measurement-build-paks\` (containers),
hash-verified at that location after copying (A62):
`exe 101AFEA4 · utoc 939B9C9B (268,036 B, 4 maps) · ucas 8A602D4D · pak 7CAE22DD`.
Path (a) will be measured against **that exact container**, and a future cook overwrites the live one
**silently** — a known failure mode now, not a hypothetical. (2026-08-19.)

---

🚨 **THE RUNNING COUNT, BECAUSE THE PATTERN IS THE FINDING AND NOT ANY SINGLE INSTANCE:
UNBANKED EVIDENCE HAS NOW BEEN FOUND SITTING IN THE PROJECT TREE FIVE SEPARATE TIMES.**
Two of those five were on **2026-08-20 alone**.

| # | when | what was sitting unbanked |
|---|---|---|
| 1 | 2026-08-14 | `RESCUE_H4_WSECHO` — and the bank already held a **DIFFERENT session** of the same NAME (`…-140533` vs `…-170238`). **A name-based sweep would have destroyed the only copy while reporting a clean duplicate.** |
| 2 | 2026-08-19 | 21 PIE-era sessions / 3.89 GB in `StackOBot\Saved\AnomalyCaptures`, **including `session_20260817-132214` — the `m23` OWNER PLAY-GATE SMOKE**, the first confirmation of that fix in real gameplay. |
| 3 | 2026-08-19 | `Saved\M23B` + **eight** exe-side leg outputs, **four of which are the raw evidence behind `m25`'s S4-3 and S4-4 claims** (bank 91 → 104). |
| 4 | 2026-08-20 | Three unbanked smoke sessions + the **rolled** smoke log, rescued as `I11B_SMOKE_RESCUE`. |
| 5 | 2026-08-20 | **BOTH owner play-gate smoke sessions for `m27`** (`…-211024`, `…-211345`) + `StackOBot.log`. |

**WHAT THE FIVE HAVE IN COMMON, and it is the actionable part:**

1. **Every one was found by matching SESSION ID, never by directory name.** A name-based
   sweep found **none** of them, and in case 1 would have actively destroyed evidence.
2. **The most valuable item is repeatedly an OWNER-PLAYED run, not a bench leg** (cases 2
   and 5). Those are produced outside the harness, so nothing banks them automatically —
   **the harness banks its own legs and creates exactly the blind spot.**
3. 🚨 **CASE 5 ADDS A NEW ONE: THE LOG CAN BE THE ONLY COPY OF A RESULT.** A vetoed event
   leaves **NO trace in `annotation.json`** by design (`m26`), so the `VETOED-OBJECT` lines
   recording *which three objects were deleted* in `session_20260820-211024` existed
   **nowhere else on disk**. Banking the sessions and not the log would have preserved the
   artifacts and lost the finding. ⇒ **BANK THE LOG ALONGSIDE ANY SESSION WHOSE RESULT IS
   PARTLY LOG-ONLY**, and note that UE **rotates** the log on the next launch (case 4 lost
   one that way and had to rescue a `-backup-` file).

**RULE: after ANY owner-played or out-of-harness capture, sweep by SESSION ID before doing
anything that touches the package tree, and bank the log with it.** Cheap; the alternative
is unrecoverable.

### G93 — `FocusGate 0` + a high `VideoFps` corrupts the camera; neither alone does it

Turning the m16 focus gate OFF at `VideoFps` 120/240 produced captures in which the player camera settled
to a **wrong, fps-dependent rotation** and held it there for the whole run, aiming the target off screen.
MEASURED: **0 of 150 label rows had a valid bbox** on both legs, with final view rotations
`[332.9, 45.1]` at 120 fps and `[347.2, 51.2]` at 240 fps (the correct rest rotation is `[0, 0, 0]`).

⚠ **It is the COMBINATION.** Isolated with a 2×2 rather than guessed:

| `VideoFps` | `FocusGate` | valid bbox | final rot |
|---|---|---|---|
| 30 | **0** | 59/59 | `[0,0]` |
| **120** | 1 | 99/99 | `[0,0]` |
| **120** | **0** | **0/150** | `[332.9, 45.1]` |
| **240** | **0** | **0/150** | `[347.2, 51.2]` |

**Mechanism INFERRED, not proven:** `StartRun` calls `FApp::SetFixedDeltaTime(1.0 / VideoFps)`
(`AnomalyCaptureSubsystem.cpp:672`). With the gate ON that call is deferred into `BeginActualRun` at
focus-in, so pawn possession finishes at normal dt first; with the gate OFF a 4–8 ms fixed step engages
*during* possession. At 30 fps the step is 33 ms and nothing breaks.

**Rule: keep `IAI.Capture.FocusGate` ON for any capture above 30 fps.** The failure is quiet everywhere
people look — the run completes, writes all its frames and reports the expected positive count; only
`bbox_valid: false` in `labels.jsonl` (or an oracle finding nothing) reveals it. (2026-08-16.)

### G94 — the annotation writer FABRICATES positives when a hide-type event never manifests

`WriteSessionAnnotationFile` decides an event's kind from the **outcome of sampling**, then falls back:

```cpp
AnomalyCaptureSubsystem.cpp:1466   const bool bHideType = HiddenIdx.Num() > 0;
                          :1467   TArray<int32> FrameIndices = bHideType ? MoveTemp(HiddenIdx) : Ev.AffectedFrames;
```

`HiddenIdx` holds the frames actually sampled hidden (`SampleDeferredHidden` :1139-1164 reads real actor
state at :1162). So if a hide-type anomaly **never actually hides** — for any reason — `HiddenIdx` is
empty, `bHideType` is **false**, and the event silently emits `Ev.AffectedFrames` instead: **every frame
where the actor merely projected on screen, relabelled as positive.**

⚠ **MEASURED consequence:** at `VideoFps` 120/240 this produced **99 labelled-positive frames per leg with
the target plainly visible in all of them**, across 49/49 events (journals 032/033). A non-event becomes a
full-window block of poisoned labels.

**Fingerprint for spotting it in existing data:** genuine sampled hide sets are **GAPPED**
(`[4,5,9,10]`); the fallback shape is a **CONSECUTIVE run covering the whole positive window**
(`[3,4,5,6,7,8,9,10]`). A blink event whose `frame_indices` are perfectly consecutive is suspect.

⚠ **It is anomaly-agnostic** — nothing about it is specific to `blinking`; any hide-type event that fails
to manifest gets the same treatment. Hide-type identity must come from the routing that already sends an
event into `SampleDeferredHidden`, never from whether sampling found anything. (2026-08-16.)

### G95 — a second capturer's write load starves the production writer; and overlapping two captures needs the focus gate managed

Running CaptureBench's SVE capture alongside a production `IAI.Capture.Start` in the same process is the
only way to compare grab points on **the same frames**, and it works — but two traps sit in front of it.

1. **Write starvation.** CaptureBench writes a PNG for **every rendered frame** (~720 KB each at
   1280×720). With a 4000-frame budget at `VideoFps 120` this saturated the shared write path and
   **production wrote 0 PNGs** while still producing a complete `run_summary.json` — i.e. it looks like a
   successful run with an empty `Actual_Frames/`. Keep the bench budget just large enough to span the
   production window.
2. **Window alignment.** CaptureBench starts at tick 1; production's start is held by the m16 focus gate
   (up to a 30 s timeout). Measured: the SVE captured `gfc 1..900` while production did not begin until
   frame **2156** — **zero overlap, so no frame could be compared.** Since `FocusGate 0` is barred above
   30 fps (**G93**), the fix is to bring the game window to the **foreground** after launch so the gate
   releases immediately; production then starts near frame 1 and both captures overlap.

**Matching method that works regardless:** decode the in-scene marker from both sets and match on
`GFrameCounter` — do not assume the two capturers' frame numbering corresponds. (2026-08-16.)

### G96 — an oracle's blindness is only ever exposed by known-answer controls: three instances, one principle

Every pixel-oracle used on this project has been blind in some regime, and in **all three cases the
blindness was invisible in the results and visible only when the oracle was run against a leg whose
answer was already known**. The verdicts it produced in the blind regime were confident and wrong.

1. **Fixed-K robust-sigma goes blind under A47 camera drift.** A `|x−median| > K·MAD` test needs a
   stable baseline. When the camera settles slowly the baseline walks through the analysis window, MAD
   inflates, and a real signal drops under the threshold. **Tell: MAD inflation** — 0.0102 at 30 fps
   rising to 0.0481 at 90 fps on the same target. It reported ABSENT on legs that were not absent.
2. **A neighbour-window local-contrast statistic goes blind on CONTIGUOUS claimed sets.** With
   neighbours taken at ±2, every interior frame of an 8-consecutive claimed run has only claimed
   neighbours, so it scores nothing: **2 of 8 frames evaluable**, and a "≥ half above threshold" rule
   became 1-of-2. **Tell: evaluable-frame starvation.** Fatal here because 8-consecutive *is* the P3b
   fallback shape — the oracle was blind in exactly the shape it existed to judge. Fix: take the
   nearest non-claimed frames flanking the **event**, not each frame.
3. **A calibration set can BRACKET a regime without CONTAINING it, so its floor is not derivable.**
   Deriving a decidability floor from an ALIGNED control (sharp signal) and an ABSENT control (no
   signal) yields nothing valid for a *spread* signal in between: one construction is contaminated
   (pseudo-events' ±1 shifts land on real signal — **tell: the "no-signal" median lands at ≈TAU**), the
   other measures numerical precision rather than discrimination. Two defensible floors gave opposite
   verdicts on the same legs. → **A57**: certify only what is invariant across all defensible
   constructions.

**Practice this earns:** every oracle change re-verifies against **one known-ALIGNED and one
known-ABSENT control** before its results are used (**A53**), and a control that cannot see the
original bug certifies nothing. (2026-08-16.)

---

### G97 — the MCP bridge attaches to whichever editor is listening, and a second UE project on this box will silently capture it

**This is a permanent environmental fact of this workstation, not an incident.** The owner runs a
second Unreal project alongside StackOBot. The `unreal-mcpython` bridge connects to whatever editor
holds the port — it does **not** resolve a project. A bridge session that answers every call
successfully may be answering about a different game on a different engine version.

Observed 2026-08-17 while gathering P6 evidence: the bridge was live and responsive, and
`Paths.project_dir()` returned `../../../../../Unreal Projects/HeistCrewUE/` with
`SystemLibrary.get_engine_version()` = **5.7.4**, not StackOBot on **5.1.1**. The tell was indirect
and would have been easy to miss: the level was `L_StackCurlTest`, `EditorAssetLibrary.
does_asset_exist("/Game/StackOBot/Blueprints/Character/BP_Bot")` returned **false**, and an asset
registry query for `BP_Bot` returned an empty list — while the same `.uasset` sits on disk. A reader
who assumed the bridge meant StackOBot would have concluded the asset was missing.

**Rule (A59): no measurement taken over the MCP bridge is attributed to this project until
`Paths.project_dir()` AND the engine version have been read back and stated alongside the result.**
A bridge connection is never evidence of which project answered. This is the A44 principle
(prove the artifact under test is the one you think it is) applied to a live editor instead of a
binary. (2026-08-17.)

---

### G98 — `AffectedFrames` is a PROJECTION-FILTERED SET, not a frame range: a fabricated label window can legitimately contain gaps

The pre-m23 P3b fallback emitted `Ev.AffectedFrames` verbatim as an event's positive frames
(`AnomalyCaptureSubsystem.cpp`, pre-m23: `const bool bHideType = HiddenIdx.Num() > 0;
TArray<int32> FrameIndices = bHideType ? MoveTemp(HiddenIdx) : Ev.AffectedFrames;`). It is tempting —
and it is **wrong** — to read that as "the fabricated set is the full contiguous burst window",
because in every banked session it *is* contiguous.

`AffectedFrames` is accumulated one index at a time and **only on frames that pass a projection
test**:

```
if (AnomalyViewport::ProjectActorBoundsToScreenRect(View, FActor, Min, Max))
{
    Ev->AffectedFrames.Add(SessionIndex);
}
```

`ProjectActorBoundsToScreenRect` (`AnomalyViewport.cpp:653-685`) returns **false** when the view is
invalid, when the actor has no `UStaticMeshComponent`/`USkinnedMeshComponent` with a valid bounds
box, when **no** bounds corner survives the `Clip.W > SMALL_NUMBER` test (whole box behind the
camera), or when the projected rect does not intersect the screen. The enclosing
`if (const AActor* FActor = F.TargetActor.Get())` adds a fourth: an actor that goes invalid mid-window
is skipped too. Any of these failing on an interior frame while the frames either side pass leaves a
**gap** in `AffectedFrames`. Note it is *not* occlusion-gated (no trace), so only frustum/screen
exits and actor validity can do it — which is exactly what a turning camera does.

**Why the corpus does not show it:** 0 of 1,367 non-empty events across all four corpora has a gapped
non-hide (= `AffectedFrames`-verbatim) set. That null is **confounded, not reassuring** — every
banked leg is a static-camera run (`CB_GateLevel`, and even the MainWorld smoke reports an identical
`camera.global_position` on all 8 events), which is the one regime in which the target cannot leave
the view. The client captures moving-camera gameplay.

**Consequence, and this is the load-bearing part:** "gapped ⇒ genuine" is **not** a sound
discriminator for fabricated labels. A fabricated event whose target briefly left frame would be
*blessed* as genuine — a false negative in the dangerous direction, and the same "unknown falls
through to safe" failure that Ruling C corrects elsewhere. Contiguity of `AffectedFrames` is an
**empirical property of static-camera capture**, never a mechanical guarantee. (2026-08-17.)

---

### G99 — the level-authoring script is DESTRUCTIVE BY DEFAULT, and the asset it destroys is the frozen gate instrument

`CaptureBench/tools/make_gate_level.py` opens with a delete-first block:

```
if unreal.EditorAssetLibrary.does_asset_exist(LEVEL_PATH):
    unreal.EditorAssetLibrary.delete_asset(LEVEL_PATH)
```

That block exists for a good reason — `new_level` refuses if the asset exists — but it means the
**default** behaviour of running the script is to **destroy** whatever is at `LEVEL_PATH`, and
`LEVEL_PATH` shipped pointing at `/Game/CaptureBenchGate/CB_GateLevel`: the **frozen, verified gate
instrument** that every banked leg, every A54 calibration and every I10/HF/M23 result is measured
against.

**Why "just re-run the script" does not recover it.** The script reproduces the *geometry*. It does
not reproduce the convergence — the black-level fix, the exposure pin and the cost model
(**G87–G89**) live in the capture configuration and in the banked baselines, not in the level. A
rebuilt level is a *new* instrument, and every prior result would need re-baselining against it.

**The trap is how ordinary the mistake is:** the obvious way to make a variant level is to open this
script and run it. Nothing in the old file said stop.

**Guarded 2026-08-18:** the script now **refuses** to author over any path in `FROZEN_LEVEL_PATHS`
unless `--allow-overwrite-frozen` is passed (or the module constant is flipped), and the refusal
names what is at stake, points at the sibling-level route, and reminds the reader to re-bank first
(**G92**). A sibling level simply uses a different `LEVEL_PATH` and is unaffected. Verified three
ways: default refuses, the override yields, a sibling path passes.

**Generalisable:** a tool whose normal mode deletes a load-bearing artifact should refuse by default
and require the destructive intent to be stated, not the safe intent. (2026-08-18.)

---

### G100 — `AnomalyCapture` now compiles against a Renderer **PRIVATE** include path, and an engine bump breaks it far from the failure site

S3a-1 added to `AnomalyCapture.Build.cs`, **inside the non-Shipping block only**:

```
PrivateDependencyModuleNames += "Renderer"
PrivateIncludePaths.Add(Path.Combine(GetModuleDirectory("Renderer"), "Private"));
```

It is required because the SVE post-process hook takes `FPostProcessMaterialInputs`, declared in
`PostProcess/PostProcessMaterial.h`, which lives in **Renderer/Private** — engine-internal, with no
API or deprecation contract. The same file also forces the `class FViewInfo;` forward declaration
that sits, apparently pointlessly, between the includes in `AnomalySceneViewExtension.cpp`. **Do not
"tidy" that line away** — the private header references the type.

**Why this is G86-shaped.** The pin in force is a **source build of UE 5.1** at
`D:\UESource\UnrealEngine` (`Build.version` 5.1.1, `++UE5+Release-5.1`) with **MSVC 14.38.33130**.
An engine bump can move, rename or restructure that private header, and the failure surfaces as a
**compile error inside our own capture module** — nowhere near the `Build.cs` line that caused it,
and with nothing in the error naming the private-path dependency. A reader who has not seen this
entry will debug the wrong file.

**Blast radius is bounded and worth stating:** it is `PublicDefinitions ANOMALY_CAPTURE=0` in
Shipping, so **Shipping never compiles it**; the `AnomalyInjector` core module is untouched, so the
game-agnostic invariant is unaffected; and the backbuffer path has no such dependency, so the
fallback survives an engine bump even if the SVE path does not.

**If it breaks:** the choice is to re-find the header in the new layout, or to build the pass
against a public surface if 5.x ever provides one. Do not vendor a copy of the private header.
(2026-08-18.)

---

### G101 — `IAI.Capture.Start`'s `outDir` is relative to the PROCESS CWD, not to `Saved`

A packaged leg launched as

```
...\Builds\BenchGate\Windows\StackOBot\Binaries\Win64\StackOBot.exe ... -ExecCmds="... IAI.Capture.Start S3A2_BASE png 777 90 blinking StaticMeshActor_49"
```

wrote its session to **`...\Binaries\Win64\S3A2_BASE\session_<ts>\`** — beside the executable —
**not** to `...\StackOBot\Saved\`. The log line says only
`=== Capture run STARTED: S3A2_BASE/session_<ts> ...`, a relative path, which tells you nothing about
the root.

**Why this bites:** the banked legs (`I10\`, `HF\`, `M23\`, `NEG2\` …) all sit under `Saved\`, so
`Saved\` looks like "where captures go". Searching there for a fresh run's output finds **nothing**,
and the natural conclusion — "the run did not write" — is wrong. It wrote somewhere else.

**Rule:** resolve the output root from the **process working directory**, or pass an absolute
`outDir`. When a run appears to have produced nothing, search by session-id across the tree before
concluding it failed. (2026-08-18.)

---

### G102 — appending a block after a closing brace can SILENTLY STEAL an `else`, and it compiles clean

The S3a-2 gate failure was a **data-destroying** defect with a one-word cause. `FinishRun` was:

```cpp
if (bRunBegun) { ...write session...  "FINISHED" }
else           { DeleteDirectory(RunDir);  "CANCELLED before focus" }
```

A new block was appended immediately after the closing brace of the `if` body. The next token in the
file was `else`, so the result was:

```cpp
if (bRunBegun) { ...write session...  "FINISHED" }

if (SveCapturer.IsValid()) { SveCapturer->SetActive(false); }   // <- inserted
else { DeleteDirectory(RunDir); "CANCELLED before focus" }       // <- else changed owner
```

**The `else` re-parented from `if (bRunBegun)` to the inserted `if`.** Effect: every run whose SVE
capturer was absent — i.e. **every run with the feature switched OFF** — wrote a complete 90-frame
session and then **deleted it**, in the same call. The `FINISHED` and `CANCELLED` log lines landed
20 ms apart, and **the 20 ms was the recursive delete**.

**Nothing catches it.** `if/else` is valid C++ with or without the insertion: no warning, no type
error, clean compile, clean link. Neither branch is dead code. It is invisible to every automated
check we run.

**Rules earned:**
- When inserting after a `}`, **read the next token**. If it is `else`, you are inside an if/else
  chain and appending there changes its meaning.
- Prefer anchoring an insertion to the **start** of the following construct rather than the **end**
  of the previous one.
- Blocks whose `else` performs a **destructive** action (delete, truncate, overwrite) deserve the
  brace-and-comment treatment, or restructuring to an early return, precisely because a silent
  re-parent is unreviewable at the diff level. (2026-08-18.)

---

### G103 — staging a code-only change is an EXE HOT-SWAP; no cook, and G92's archive-wipe is not involved

The plugin compiles **into the monolithic game executable**, so a code-only change reaches the
packaged build by rebuilding the game target and **copying one file**:

```
Build.bat StackOBot Win64 Development -project=...\StackOBot.uproject
copy  <Project>\Binaries\Win64\StackOBot.exe
   -> Builds\BenchGate\Windows\StackOBot\Binaries\Win64\StackOBot.exe
```

**~85 s to build, one copy to stage.** No cook, no `BuildCookRun`, no archive step — and therefore
**the G92 hazard (the archive step wiping the `Saved` tree) is not in play at all** for a code-only
leg. Content changes still need the full path; code changes do not.

**Two things this does NOT excuse.** The hot-swap **is** the stage step, so G92's actual lesson —
*compiled is not staged* — applies unchanged: **A44-scan the staged artifact after copying**, not the
build output. And back the previous exe up first if the old binary is a baseline you still need; the
m23 gate binary had to be restored intact after the S3a-2 leg. (2026-08-18.)

---

### G104 — this box stops giving the game window focus MID-SESSION, and it silently invalidates cross-binary comparisons

**Environmental fact of this workstation, not an incident.** The packaged legs are launched the same
way every time:

```
StackOBot.exe /Game/CaptureBenchGate/CB_GateLevel -windowed -ResX=1280 -ResY=720 -ExecCmds="..." -unattended -nosplash
```

All morning that produced `run.json start_frame = 1` — the window took foreground focus on creation and
capture began on the first tick. By the afternoon the **identical command** began producing
`start_frame ≈ 2100–2560`: the window no longer took focus, so the m16 focus gate held the run in
`ArmedPending` for the **full 30 s safety timeout** before starting anyway. Nothing in the harness, the
command, or the build changed.

**Why it is expensive.** Everything absolute shifts — `start_frame`, `end_frame`,
`engine/ticks_msec` (30270 ms *is* the timeout), and the label rows' frame/time fields. A leg that rode
the timeout **is not comparable** to one that did not, and the difference looks exactly like a code
regression in a subset diff. It also inflates SVE `key_ring_published` counts (2228+ vs 121), because
the extension activates in `StartRun` **before** the focus branch and publishes throughout the wait.

**The rule it earned: A63** — `start_frame` must match across legs in a cross-binary comparison, or the
leg is **INVALID** and re-run. *A leg is discarded for how it ran, never for what it showed.*

**The remedy that works — use it, do not rediscover it.** Force focus at launch: `Start-Process -PassThru`,
then call `WScript.Shell.AppActivate($p.Id)` on a short loop (~12 × 400 ms) while the window comes up.
One attempt was enough to get `start_frame = 1` back. **Verify `start_frame` from `run.json`; never
assume the leg was valid.**

**The general lesson, and it is the argument for validity conditions over one-time calibration:** the
confound was **moving**. A calibration taken in the morning certified nothing about the afternoon. Only
a per-leg validity check catches a variable that drifts. (2026-08-18.)

---

### G105 — a zero-valued metric that is ALSO the pass condition: the fourth instance of G96's principle, and the first caught after acceptance

**The metric:** `overlap = (frames missing from disk) ∩ (frames claimed positive)`. It was used to ask
whether a forced-drop sweep had ever dropped a **positive** frame, and it read **0**.

**Why it is blind:** when the label path handles a dropped positive **correctly**, the frame disappears
from the files **and** from the claims — so the intersection is empty. **Zero overlap is simultaneously
"the case was never exercised" and "the case was exercised and handled perfectly."** The metric cannot
separate them, in exactly the region it was introduced to judge.

**The sound discriminator: did the CLAIMED SET SHRINK against the clean run?** Under it, phase 0
genuinely dropped no positive (30 of 30 claims intact) and phase 2 dropped 7 (23 of 30). Same artifacts,
opposite conclusions.

**This is G96's principle a fourth time** (after fixed-K sigma under drift, the ±2 neighbour-window LC on
contiguous sets, and bracket-not-contain floor non-derivability) — **and the first caught *inside* a
result that had already been reported, accepted and committed.** The first three were caught before use.

**The escalation is the lesson:** a blind metric survives review when **its output looks like the answer
you expected**. "Never exercised" was a plausible, mildly disappointing result, so nobody pushed on it —
including the person who wrote it. Ask of any zero: *what else would produce this exact zero?*
(2026-08-18.)

### G106 — an analysis instrument that grades a certified result is a COMMITTED ARTIFACT; prose is not a spec

**What happened.** S3b Stage 2a went to run the A54 oracle against the bank and found **there was no A54
oracle**. `TAU = 0.04684` and the canonical definition existed only in prose — `gotchas.md`, the S2
handoff, journal 034, `PRE-DELIVERY-CHECKLIST.md`. The committed tools were `compare_sessions.py`,
`subset_gate.py`, `decode_marker.py`, `compare_traces.py`, `exposure_shift.py`, `frame_stats.py`,
`make_gate_level.py`, `verify_capture.py`. **No oracle, no ±1 shifter, no A56 checker.**

⇒ **Every frame-alignment certification this project holds — I10 and m23 — was graded by scripts that no
longer exist.** Those results are not wrong. They were **NOT REPRODUCIBLE**, which is a quieter problem
and a worse one, because nothing announces it.

**Why the prose was not enough — measured, not asserted.** Rebuilding from the written definition left
**two load-bearing points under-specified**, and the most natural reading was wrong on both:

| under-specified point | the attractive reading | what actually reproduces |
|---|---|---|
| do the event flanks move with the shift hypothesis? | **fixed** — "they are the clean baseline" | **they move.** A wrong shift must drag a hidden frame into its own reference so its score collapses; fixed flanks leave a wrong shift at exactly half the right one |
| which shifts are scored? | −2…+2 | **−1, 0, +1 only.** With ±2 in the set, ±2 becomes the runner-up and **every margin halves** |

Either error alone changes R30's median margin from the published **0.10737** to **0.0548 / 0.0505**, and
its decidability from **12/12** to **0/12** — *while leaving every per-event verdict ALIGNED and the
headline "12 ALIGNED / 0 non-ALIGNED" intact*. **A silently different instrument that agrees on the
verdicts and disagrees on the confidence is the worst of both worlds:** it passes a casual re-check and
it corrupts every decidability annotation A55 depends on.

**Both were caught only by reproducing published NUMBERS, not published CONCLUSIONS.** "It says ALIGNED,
and ALIGNED is what the journal says" would have shipped the wrong ruler twice over.

**The rule.** Any script whose output appears in a certification — an oracle, a comparator, a floor
construction, a positive control — is **committed with the result it graded**, in the repo, before the
result is cited. Document the rule in prose by all means; the prose is the *explanation*, never the
*definition*. **A verdict whose grader no longer exists is not reproducible, however well it was
written up.**

*Corollary, and it is the expensive half:* when an instrument must be rebuilt from prose, **reproducing
the published verdicts is not sufficient evidence that you rebuilt the same instrument.** Reproduce the
published *quantities* — medians, margins, decidability counts — or state plainly which ones you could
not.

**This is G96's principle a FIFTH time, and the sharpest.** The previous four were instruments **blind**
in some region — they eventually surface as unevaluable or suspiciously tidy results. This one is not
blind. It is **confidently wrong about how much it knows**, and it surfaces as *nothing at all*: the
verdicts are right, the headline is right, and only the confidence is silently corrupted.

⚠ **SCOPE OF THE REBUILT ORACLE (`CaptureBench/tools/a54_oracle.py`), recorded here as well as in the
file:** certified at **VideoFps 30 only** — R30 (12/12 decidable, margins to 2.4 %) and I10HF HF1
pre-fix (ALIGNED = 0). **Margin scale is NOT reproduced above 30 fps** (×0.98 sharp → ×2.05 spread).
**Verdicts reproduce at every tested regime; margins do not.** Any use above 30 fps requires re-gating
first. → **P7**. (2026-08-18.)

### G107 — a frozen ABSOLUTE threshold silently inherits every dependency of the quantity it thresholds

**TAU (0.04684) is an absolute in-bbox luminance difference.** How big that difference *is* depends on
**where the camera came to rest** — the A47 bifurcation sets the bbox and the background behind the
target. TAU was frozen from calibration legs that all happened to settle in the **modal** pose.

Measured on two legs of the *same* I10 set, same binary, same target, same seed:

| leg | camera rest pose | hidden − visible | A54 verdict |
|---|---|---|---|
| `L6_client40` | modal | **+0.1126** | ALIGNED |
| `L3_client39` | **bifurcated** | **−0.0383** | **ABSENT — and it is FALSE** |

L3's hide is **real, perfectly aligned and perfectly separated**: claimed frames 0.8153–0.8180,
non-claimed 0.8531–0.8559, **zero overlap**, every claimed frame on the low side. The oracle returns
ABSENT for one reason only — `0.0383 < 0.04684`. Note also the **sign flip**: the bifurcated pose makes
the hidden side *darker*, matching journal 031's recorded "hidden side: low" for L3 against "high"
everywhere else.

**Why this is the dangerous direction, not the harmless one:** A50 treats **ABSENT as reproduction of
the defect**. So a false ABSENT does not read as "inconclusive" — it reads as **evidence of a defect
that is not there**, on the client band, which is the band that matters most.

**A56 does not catch it, and cannot as written.** A56 asks whether a leg is *self-consistent* (modal-crop
coverage, distinct-bbox count). L3 passes A56 comfortably once the settle window is applied — 97.6 %
modal, 2 distinct. It never asks the question that matters: **is this leg's pose the pose TAU was
calibrated on?**

**A57 recurring.** The calibration set **brackets** the phenomenon without **containing** it — nothing
in it exhibits a bifurcated pose, so no pose-invariant floor was derivable from it and none was noticed
to be missing.

**The general lesson, which outlives this oracle:** a threshold frozen in *absolute* units carries every
hidden dependency of the units. Before freezing one, ask **what else changes its scale** — and if
anything does, either normalise it away (a separability statistic is scale-free where a raw difference
is not) or make the dependency an explicit precondition of use. → **P8**. (2026-08-18.)

### G108 — a STALLED process fails foreground activation, and "the box is being difficult" is the wrong first conclusion

**What happened.** The S3b deep leg (`CaptureBench.Stall.GameMs 99`) rode the 30 s focus gate **three
times in a row** — `start_frame` 298 / 299 / 300, which is *exactly* 30 s at ~100 ms/frame. The harness
declared an **environmental halt**, which is what its own rule said to do.

**It was not environmental. It was the harness.** A game thread busy-waiting 99 ms per tick reads as an
**unresponsive window**, and Windows' foreground lock refuses `WScript.Shell.AppActivate` against one.
The other legs acquired focus in ~1 s; the deep leg never acquired it at all. Same box, same session,
same operator behaviour — **the stall lever was the variable.**

**Fix:** a synthetic **ALT tap** (`keybd_event 0x12` down/up) releases the foreground lock, followed by a
direct `SetForegroundWindow` + `BringWindowToTop`. The leg then started at **frame 1, 0.1 s, first
attempt**. This is a **harness** mechanism — it does not touch the system under test, and the focus gate
itself is untouched (**G93** stands).

⛔ **VIRTUAL-DESKTOP ISOLATION — INVESTIGATED AND REJECTED. Do not re-propose it.** Launching the game on
a separate virtual desktop *sounds* like the clean fix for focus contention. It is the opposite:
**moving a window to another desktop REMOVES foreground focus**, which is precisely what the focus gate
waits for. It would **guarantee the timeout it was meant to prevent.**

**The general lesson:** when automation that normally works fails only under load, suspect the
**automation's assumptions about the target's responsiveness** before concluding the environment is at
fault. An "environmental" halt that only ever fires on the slowest configuration is not environmental.
(2026-08-18.)

### G109 — a threshold in FRAMES cannot generalise across regimes where frame time varies

The A63 auto-retry first detected the focus timeout as `start_frame > 100`. That constant is wrong in
both directions at once, and the arithmetic says why: the gate is **30 seconds**, so it expires at

| regime | frame time | timeout lands at |
|---|---|---|
| nominal | 33.3 ms | ~900 frames |
| client (stall 40) | 41.3 ms | ~726 frames |
| deep (stall 99) | 100.3 ms | **~299 frames** |

A single frame-count threshold is therefore **simultaneously too loose for the nominal leg** (a genuine
timeout at 900 frames is caught, but so is a healthy 4 s acquisition at 120) **and only accidentally
right for the deep one**. It happened to catch the deep leg's 298 because 298 > 100 — for the wrong
reason.

**Time is the invariant; frames are not.** The correct form is
`start_frame / sustained_wall_fps >= 20 s`, both quantities read from the artifact.

*Generalisation:* any harness constant expressed in frames, ticks, or iterations inherits the frame
time of the regime it was calibrated in. Before freezing one, ask **what the underlying quantity
actually is** — here it was always seconds — and express it in those units. Same family as **G107**
(a frozen absolute threshold inheriting its units' hidden dependencies). (2026-08-18.)

### G110 — a summary figure that contradicts the document's own table survives review, because nobody re-adds the column

Journal 031 §4.5 published, in a single paragraph:

```
label.frame_index - marker_gfc  ==  0   on  532 / 534 decoded frames
per leg: L1 85, L2 90, L3 87, L4 90, L5 90, L6 90.
The only two exceptions are pre-window warm-up frames in L3 with no marker drawn
```

**The headline and the table disagree, and the table is right.** `85+90+87+90+90+90 = 532` over a
`6 × 90 = 540` corpus ⇒ **eight** non-matching rows, in **L1 (five) and L3 (three)**. The stated
denominator `534` is `532 + 2`, written to fit the "two exceptions" sentence; and that sentence names
**only L3**, while **L1 — which holds the majority of them — is not mentioned at all.**

An independent re-measurement months later reproduced the per-leg distribution **exactly**: L1 5, L3 3,
all others 0. **The data was always correct. Only the summary was wrong**, and it sat unchallenged
through a milestone tag and two handoffs.

**Why it survived:** the headline was the number people quoted, the table was the number nobody re-added,
and the two lived four lines apart. **Nothing about the paragraph looks wrong** — it looks like a
carefully-reported result, because most of it is one.

⚠ **A PLAUSIBLE WRONG EXPLANATION NEARLY LANDED ON TOP OF IT.** The first hypothesis offered for the gap
was that the analysis windows had filtered the denominator — mechanically sensible, consistent with a
concern already live in the project, and **false**. Its own per-leg counts (85 and 87) **exceed** those
legs' published window sizes (74 and 60), so they cannot be window-filtered. It was caught only because
a **reconciliation was demanded before the correction was written.**

**The rules that follow:**

1. **State a ratio's denominator basis with the ratio** — what was enumerated, over what set. `532/534`
   with no basis cannot be checked against a corpus later.
2. **Re-add your own columns before publishing a summary over them.** A total that does not equal its
   parts is the cheapest possible error to catch and the most expensive to leave.
3. **When a published figure does not reconcile, reconcile it or record it as UNRECONCILED (A60).**
   Do not fit a mechanism to the gap — the plausible one here was wrong, and it was *more* plausible than
   the truth (a slip of two).

*Companion to G96's family: this is not a blind instrument, it is a correct instrument with a mis-stated
summary — which review handles even less well, because the underlying work is sound.* (2026-08-18.)

---

### G111 — a working-agreement written for ONE agent, stored in a SHARED doc, will be executed by ANY agent that reads it

`docs/CHAT-HANDOFF-s3-m24-capture-migration.md` §13 "Working agreements" describes **chat-Claude's**
output conventions to the owner: mark must-read items with 🔴, route ACT/DECIDE/HEADS-UP items to the
top of the reply, always lead with a plain-language summary.

A fresh Claude Code session was told to read that handoff as part of its cold bootstrap. It read §13,
found no addressee named anywhere in it, and reasonably concluded the conventions were **its own**. It
duly produced an S4 plan headed by four 🔴 markers, two of which routed genuine **technical** decisions
to the owner — who by the very same section "does **not** make technical decisions".

**Nothing in the section was wrong. It simply never said who it was for.**

Two distinct harms, and the second is the one that lasts:

1. **The marker inflates and stops meaning anything.** 🔴 means "the owner must read this line". Once it
   appears in routine implementation output it is noise, and the next genuine ACT is skimmed past.
2. **A decision reaches the wrong desk.** The owner's stated role is executive judgement on
   product/scope tradeoffs, with design and technical calls fully delegated. A convention that invites
   an implementer to hand him a technical fork quietly reverses that delegation — and it reverses it
   *while looking like compliance*.

**RULE: a convention stored in a shared doc must name its addressee, in the section itself, not in the
reader's head.** Scoping now lives at the top of §13 and is mirrored in `CLAUDE.md`.

⚠ **This generalises past markers and past agents.** Any shared doc accumulating "how we work" text —
handoffs, `CLAUDE.md`, journals — is read by every agent that cold-starts, and each one applies what it
finds. Effort settings, report formats, escalation paths, commit conventions: if a rule applies to one
role, the doc has to say so. *The failure mode is not disobedience — it is obedience to an instruction
that was never addressed to you.* (2026-08-19.)

---

### G112 — a gate/test artifact written into a file OUTSIDE version control will silently return

`m16` recorded the `[AnomalyControlServer] Token=TESTVALUE123` gate artifact as **reverted** from
`StackOBot\Config\DefaultGame.ini`. On 2026-08-19 it was **back**, and it was found only by unrelated
inspection while planning S4's client-config section — not by any check, and not by the pre-delivery
checklist, which already carried a "Token is set to a long random value" box.

**It did not come back because a script rewrote it.** The generator question was asked explicitly and
the answer is: **nothing in the workspace writes that file.** What kept the value alive is that the
dev pair had **three legs and only one of them was tracked**:

| leg | tracked? |
|---|---|
| `StackOBot\Config\DefaultGame.ini` | **no** — StackOBot is not a git repository at all |
| `anomaly-dashboard\public\config.json` | **no** — explicitly gitignored (`.gitignore:6`) |
| `CaptureBench\tools\verify_lastrundir.ps1` | **YES** — hardcoded `TESTVALUE123` as a *parameter default* |

Reverting one leg left the other two asserting the value, and the pair only works when all three match —
so the ini was re-established to make dev auto-connect work again. **A revert that leaves the
convention intact is not a revert.**

⚠ **A KNOWN DEFAULT IS WORSE THAN NO DEFAULT.** The committed tool's `[string]$Token = "TESTVALUE123"`
is the sharpest part: it is in version control, it is silent, and it survives every rotation of the
actual secret. It now reads the token from `DefaultGame.ini` instead, refuses a placeholder, and warns
below 32 chars — so the tool has exactly one source of truth and git has none.

**RULE: untracked config needs a DETECTOR, not a memory.** A checkbox that says "confirm X" is a memory
with better formatting. `PRE-DELIVERY-CHECKLIST.md` §1 now carries a **runnable** check that exits
non-zero and names the fault (no `TESTVALUE|CHANGEME|placeholder`, plus a positive `>= 32 chars`
assertion). It was verified against four cases before being trusted: the live config **PASS**, the
historical `TESTVALUE123` **FAIL**, a 6-char token **FAIL**, an absent key **FAIL**.

---

🚨 **AMENDMENT, 2026-08-19 — THIS DETECTOR CHECKS THE WRONG ARTIFACT AND RETURNS PASS ON THE UNSAFE
CASE.** The check above reads `StackOBot\Config\DefaultGame.ini`. **That file is not what any built
game enforces.**

A packaged build reads the **COOKED** copy of `DefaultGame.ini` baked into its pak at cook time. Editing
or rotating the source ini changes **nothing** for a build already cooked. Measured on the m25 staged
binary while collecting an unrelated read-back: the source ini carried a rotated 64-character token,
the build **rejected** it, and the build's own startup log said

```
=== Control server token: TESTVALUE123 (from DefaultGame.ini [AnomalyControlServer] Token) ===
```

⚠ **The parenthetical is the trap.** The binary says *"from DefaultGame.ini"* and means **the cooked
one**. A reader who greps the source ini, sees a strong token, and matches it against that log line
concludes the rotation took. It did not.

**So the detector above returns PASS on a build that is enforcing a 12-character literal present in
this repo's history.** That is strictly worse than having no detector, because it retires the vigilance
that would otherwise notice — which is the same complaint this gotcha makes about a checkbox, now
turned on its own fix.

**The check is DEMOTED: passing the source ini is NECESSARY, NOT SUFFICIENT.** The sufficient check is
a read-back from the running build — start it, read `Control server token:` out of its own log, and
assert *that* value is not a placeholder and is ≥ 32 chars. See **G118** for the full write-up and
**G119** for the general shape. `PRE-DELIVERY-CHECKLIST.md` §1 carries both checks now, in that order.

⚠ **There is no pre-cook or pre-stage script in this project** — cooking and staging are run by hand
from `setup-runbook.md` §8 — so the check has nowhere automatic to live and one was deliberately **not
invented**. If a build wrapper is ever written, this check is its first line.

**Generalises past tokens.** `GameDefaultMap` (G88) sits in the same untracked host config and the
checklist already flags it with *"nothing in git will catch it"* — that note was right, and this is what
"nothing in git will catch it" looks like when it actually happens. Any value that a **cook consumes**
and **git does not see** needs an executable check, whatever the value is. (2026-08-19.)

---

### G113 — the Bash tool exits 1 on every call in this environment while producing correct output

Environmental, this workstation, not a project defect. Every `Bash` tool invocation returns **exit code
1** with a trailing
`bash.exe: /c/Users/.../Temp/claude-XXXX-cwd: No such file or directory`,
**while the command itself runs and its stdout is correct**. The failure is in the wrapper's
working-directory save step, after the command has already succeeded.

**Why it matters here specifically:** this project gates on exit codes
(`subset_gate.py` 0/1/2, `run_leg.ps1` 0/2, `resolution_delta.py` 0/1/2). A tool that reports failure
while succeeding is the exact shape that gets a green result recorded as red — or, worse, gets a real
red dismissed as "that's just the Bash thing".

**Workaround: use the PowerShell tool for everything.** It reports exit codes faithfully. All S4-0 work
was done through it.

*General form, and it is the mirror of G92: verify the CHANNEL before trusting what it reports. G92 was
a binary that was compiled but never staged; this is an exit code that is emitted but never earned.
In both cases the tooling is confidently wrong and nothing in the output says so.* (2026-08-19.)

---

### G114 — a packaged UE game runs DPI-UNAWARE, so a display-scale change never reaches it, and the null that produces is an ARTIFACT

S4-1's DPI leg was set up as: put the desktop at 150 %, run once at the engine default, run again with a
per-application **`~ DPIUNAWARE`** override, compare. Both legs returned **dW = dH = 0** on every rect.
Read naively that is *"DPI scaling does not move the capture rects"*.

**It is not. The process never saw the 150 %.**

An independent read-back — `GetProcessDpiAwareness` against the live PID, run **with and without** the
override — returned **`PROCESS_DPI_UNAWARE` (0) in BOTH cases.** The packaged `StackOBot.exe` is already
DPI-unaware, so:

- Windows **virtualises** it: the process is told 96 DPI whatever the desktop is set to, and its output
  is stretched by the compositor after the fact.
- the `~ DPIUNAWARE` override was a **no-op** — it forced a state that was already true;
- the two legs were **one regime measured twice**, not two regimes;
- and **the DPI axis was not probed at all**, by either of them.

⚠ **`EnableHighDPIAwareness` defaults to 1** in `GenericPlatformApplicationMisc.cpp` and is easy to read
as "games are DPI-aware". The measured behaviour of the packaged game target is the opposite. **Read the
process, not the cvar default.**

**The fix is the OPPOSITE override.** `~ HIGHDPIAWARE` flips it to **`PROCESS_PER_MONITOR_DPI_AWARE` (2)**,
verified by the same probe before the leg ran. Only then does the process observe the scaled display, and
only that leg is evidence about DPI. (It also came back dW = dH = 0 — but now that is a *measurement*
rather than an artifact of insulation.)

> **This is G96's principle applied to the LEVER rather than to the oracle.** Every previous instance was
> a blind *instrument*; this was a blind *manipulation*. A lever that does nothing produces a clean null
> that looks exactly like a clean result — and unlike a blind oracle it leaves no unevaluable output to
> notice. **The only thing that exposed it was a both-directions control on the lever itself: set it,
> read the state back, clear it, read again, and require the two readings to DIFFER.**

**RULE: an environmental lever must be verified to have CHANGED SOMETHING before any leg run under it is
evidence.** A48 already says report the effective config from an independent read-back rather than what
was requested; this extends it from in-process cvars to **OS-level and per-application state**, where
there is no log line to echo and the failure is silent by construction.

*(Related: G92 — compiled is not staged. Same family: the thing you believe you changed is not the thing
under test.)* (2026-08-19.)

---

### G115 — a shell round-trip re-encodes the WHOLE file, and the tell is the DIFFSTAT, not the text

Writing a tracked file through a shell round-trip — `Get-Content -Raw` → `Out-File` / `Set-Content` —
**re-encodes the entire file**: every non-ASCII line rewritten, plus a BOM added. **The content still
reads correctly**, so nothing in the text says anything is wrong.

**The tell is the diffstat.** Two instances on 2026-08-19, both while making small edits:

| file | intended | what `git diff --stat` actually said |
|---|---|---|
| `architecture.md` + `capture-fps.md` | a handful of lines | **377 insertions / 377 deletions** |
| `CLAUDE.md` | two blocks | **700 insertions / 685 deletions** |

Both were caught by *reading the diffstat before committing*, reverted with `git checkout --`, and
redone through the editor tool — landing at 35/10 and 34/13 respectively, no BOM. Nothing corrupted
reached a commit.

**RULE: use the editor tool for edits to tracked files. A diffstat disproportionate to the intended
change is an ENCODING SMELL and must be checked BEFORE committing.**

⚠ **Treat this as a TOOL-REFLEX, not an attention lapse — that is the load-bearing part.** The first
instance was written up as a correction, *and the second happened roughly two minutes later, in the same
turn, on a different file.* **A written note did not defend against it**, because the reach for a shell
one-liner is automatic when the edit looks mechanical. The defence is the diffstat check at commit time,
which is mechanical too and therefore actually survives contact.

⛔ **Deliberately NOT automated.** A hook or wrapper for this is more surface than the fault is worth.
(2026-08-19.)

#### 🆕 AMENDMENT (2026-08-20) — **DIAGNOSE THE DIFF BEFORE FIXING IT**, and what actually caught it

**This fired on the author of the rule.** A `Get-Content -Raw` → `Set-Content` round-trip for **one**
PART-INDEX line returned **`2940 ++++----` on a ~130-line addition**.

🚨 **AND THE MORE INSTRUCTIVE HALF: TWO REPAIRS WERE ATTEMPTED BEFORE THE CAUSE WAS NAMED, AND BOTH
WERE WRONG.** *(1)* strip the BOM — the BOM was real but was not the diff; *(2)* convert LF→CRLF —
**backwards**: `core.autocrlf=true` and the stored blob is **LF, no BOM**, so line endings were never
the difference at all. The real cause was only found by comparing a single line against `HEAD`:
**`⚠` had become `Ã¢Å¡Â ` — double-encoded.** Every non-ASCII line in a 2,900-line journal.

⇒ **RULE: a large diffstat has SEVERAL possible causes — BOM, line endings, re-encode, or genuine
content — and THE WRONG REPAIR IS INDISTINGUISHABLE FROM THE RIGHT ONE UNTIL THE CAUSE IS NAMED.**
Diagnose first: check the first three bytes, check `core.autocrlf` and the **stored blob** (not the
working copy), and diff **one known line** against `HEAD`. Only then repair — and when the content is
corrupt the repair is `git checkout --` plus re-applying through the editor tool, never another
round-trip.

🚨 **RECORD WHAT CAUGHT IT: the MECHANICAL PRE-COMMIT DIFFSTAT CHECK, not vigilance.** The rule was
known, written by the same hand, and violated anyway. **That is the entire argument for mechanical
checks over remembered rules** — the check does not depend on remembering the rule at the moment it
matters.

---

### G116 — a SHORT-CIRCUIT chain collapses four distinct causes into ONE artifact string, so a signature is only "unique" if the other clauses are excluded BY CONSTRUCTION

`EvaluateSelectionProvenance` (`AnomalyViewport.cpp:540-576`) writes `valid:false`, `0/0` samples,
`coverage_pct -1`, `poll_distance -1` — a single, distinctive-looking string. It is produced by **four
different clauses**, evaluated in this order inside `IsComponentRenderableVisibleInternal`:

```
C1 renderable (SM/SK ∧ IsVisible)
C2 poll radius   dist(pollOrigin, bounds.origin) − sphereRadius > GPollRadius   (default 1800 cm)
C3 frustum
C4 occlusion
```

**H4's cause signature was pre-declared as `valid:false` + `0/0`, "unique to H4" — i.e. C4.** The bank
already contained that exact string **21 times out of 780 records**, every one on `StaticMeshActor_49`,
which is **unoccluded on all 9 rays**. Recomputing the frustum test at each record's actual banked
camera rotation explained all 21: **21/21 out of frustum (C3), 0/21 occluded (C4).** They are anchor
frames that landed while the camera was still slewing through the A47 settle, 22°–116° of yaw off modal.

**The fix is not a better threshold; it is a SECOND, INDEPENDENT quantity that the clauses disagree on.**
Here `labels.jsonl`'s `bbox_valid` at the same anchor frame does it, because
`ProjectActorBoundsToScreenRect` shares C3 (frustum/screen) but **not** C4 (it runs no trace):

| clause | provenance | `bbox_valid` at the anchor frame |
|---|---|---|
| C3 frustum | `valid:false` 0/0 | **false** — both paths agree |
| C4 occlusion | `valid:false` 0/0 | **true** — the paths DIVERGE, which is the whole hypothesis |

All 21 banked cases read `bbox_valid:false`, so the bank holds **zero** instances of the divergence —
neither corroboration nor refutation, but the incumbent producer of the signature, named.

**RULE: before pre-declaring a signature as diagnostic of one cause, enumerate every branch that writes
it. A short-circuit chain is the classic generator, because the artifact records the RESULT and not the
CLAUSE.** Then either exclude the other clauses by construction (choose a target inside the poll radius;
require the camera settled) or pair the signature with a quantity the clauses disagree on.

> This is **G96's principle moved one step earlier**. G96 is about an oracle being blind in a regime;
> G114 is about a lever that does nothing. This is about a **signature that is not the discriminator it
> was declared to be** — and it is the cheapest of the three to catch, because the evidence was already
> sitting in the bank and cost one read. **Grep the bank for the signature BEFORE the run.** A signature
> that already occurs, on a case where its declared cause is impossible, is disqualified on the spot.

(Found in H4's pre-flight; the run was stopped before it could be misread. → journal 045 §3. 2026-08-19.)

---

### G117 — `CALIB_BBOX` is frozen against a TARGET as well as a RESOLUTION, so the pixel oracle cannot judge a leg fired at any other actor

S4-1 established that `CALIB_BBOX` is frozen **in pixels** at 1280×720 and therefore fails an
off-calibration **resolution** for reasons unrelated to pose. The same constant is equally frozen
against the **target**, and that had not been stated anywhere.

`CALIB_BBOX = (0.0, 485.2, 306.1, 234.8)` **is `StaticMeshActor_49`'s bbox**, and `pose_match(modal)` is
a **conjunct of `a56_check`**. So a leg fired at any other actor exits **2 / `NOT-A54-CERTIFIABLE`** —
the oracle declines to judge, correctly and safely, but **for a reason it then misattributes**: its
failure text prints *"P8: this leg's camera settled in a pose TAU was NOT calibrated on."* The pose may
be perfectly modal. The cause is the target.

**Consequence, and it is a design constraint rather than a bug:** any experiment that fires at an actor
other than `StaticMeshActor_49` **has no A54 verdict available to it**, in any run design. Discovered
when H4's pre-declared H4-CONFIRMED branch turned out to require "A54 = ABSENT" on a different target.

Two further consequences worth having written down:

- **A54 also cannot grade a MULTI-TARGET leg.** `a56_check` pools every `anomalies[]` entry of every
  label row into one modal bbox. Two simultaneous targets ⇒ modal coverage ≈ 0.50 against
  `A56_MIN_MODAL = 0.90` ⇒ not certifiable **before a single pixel is read**. The header already says
  *"the bbox is the leg's MODAL bbox, taken once per leg"*; this is what that costs.
- **The honest substitute is the RAW in-bbox series, reported AS a raw series.** Precedented: S3 read
  L3's raw series before reporting when the oracle declined (→ P8/G107). It licenses less than an A54
  verdict, and saying so is the point.

⛔ **`CALIB_BBOX`, `TAU`, `POSE_TOL_PX` and the A54 definition stay UNTOUCHED.** The generalisation —
per-leg calibration, or NDC-normalising `CALIB_BBOX` — is a **definition change to a certified
instrument** and needs its own eight-control gate under A53. It is filed alongside `B1`-NDC and `B2`,
**not** to be done inside a measurement turn.

> **The general lesson: a frozen calibration constant is scoped by EVERY dimension of the leg it was
> measured on, not just the one you were thinking about when you froze it.** `CALIB_BBOX`'s own comment
> says *"SCOPED to 1280×720 / CB_GateLevel / StaticMeshActor_49"* — all three were written down, and only
> the resolution was ever treated as load-bearing. **Read the scope line as a conjunction of
> preconditions, not as provenance.** (2026-08-19.)

**Addendum, measured the same day.** Fired at `StaticMeshActor_100`, `check_pose.py`'s reporting-only
output read `ratio m/CALIB = (None, 0.9161, 0.3777, 0.394)` — **non-uniform**, with `modal_rot` stable
at `(0,0,0)` and `distinct=1, modal=100%`. Under the printed discriminator that is **neither** of the
two causes it enumerates ("uniform ratio + camera still ⇒ resolution scope"; "non-uniform + modal_rot
displaced + collapsed width ⇒ genuine A47"). It is a **third** cause — a different target — and the
discriminator does not name it. ⚠ **A discriminator that lists two causes invites the reader to pick
one.** Anything it does not enumerate reads as the nearest listed option, which here would have been
"genuine A47 bifurcation" on a leg whose camera was provably motionless.

---

### G118 — the COOKED build enforces its own token, so a placeholder guard on the SOURCE ini validates the wrong artifact

`Config/DefaultGame.ini` in the project carries a rotated 64-character control-server token.
**The staged build rejects it.** Its own startup log says why:

```
=== Control server token: TESTVALUE123 (from DefaultGame.ini [AnomalyControlServer] Token) ===
```

The staged build was cooked **before** the rotation and still enforces the placeholder it was cooked
with. **The parenthetical is the trap:** the binary says *"from DefaultGame.ini"* and means the
**cooked** one, which is a different file from the one on disk. A reader who greps the project ini,
sees a strong 64-char token, and matches it against that log line will conclude everything is fine.

**Why this is worse than an ordinary staleness bug:** G112 installed a placeholder guard, and
`verify_lastrundir.ps1` applies it — to the **source** ini. The source ini is clean. **The guard fires
on the artifact that enforces nothing and stays silent about the one that is actually listening.**
A build cooked before any future rotation inherits the same hole, silently.

- **`TESTVALUE123` is a 12-character literal that appears in this repo's history.** Anyone who can
  reach `ws://127.0.0.1:8077` on a box running a stale cooked build is authenticated.
- **RULE: verify the token against the RUNNING PROCESS, not against the ini you edited.** The log line
  above is the read-back; it is the A44 principle (prove the artifact under test is the one you think
  it is) applied to a secret instead of a binary, and A48's (report the effective value from an
  independent read-back) applied to something with no cvar to echo.
- `ws_scoping_echo.ps1` now reads the enforced token from the live log, prints source-vs-enforced side
  by side, and shouts when the enforced one is a placeholder. **Reading it from the log is a
  READ-BACK, not a hardcoded secret** — the value never enters a tracked file.
- ⛔ **NOT fixed here.** The fix is a re-cook (and a re-stage, which wipes `Saved` — G92 — so the bank
  must be refreshed first), and that would replace the exact binary `101AFEA4` that every m25 result is
  measured against. **Filed, not done.** It must not ride inside a measurement turn.

*(Found while collecting H4's corroborating A48 echo; entirely orthogonal to H4, and recorded rather
than absorbed. Related: G92 — compiled is not staged; G112 — placeholder tokens; G114 — a lever that
does nothing.)* (2026-08-19.)

**CLOSURE IS SEQUENCED, NOT OPTIONAL — and closing it RETIRES the m25 measurement binary.**
`G118 CLOSURE = re-cook + re-stage + re-bank (G92 wipes `Saved`) + a re-run of the A44 hash scan.`
It runs **AFTER the current measurement sequence and NEVER inside one**, because a re-cook replaces
staged exe **`101AFEA4`** — the binary every m25 result, and H4's own two legs, are measured against.
⛔ **Any result still owed against `101AFEA4` must land BEFORE closure.** Sequencing is the owner's
call, not the implementer's.

---

### G119 — a guard that checks the SOURCE of a baked artifact validates a copy the running system never reads

**Third instance of the same shape**, and the pattern is now clear enough to name:

| # | gotcha | the channel that was trusted | what was never verified |
|---|---|---|---|
| 1 | **G92** | "it compiled" | that the binary was **staged** |
| 2 | **G113** | an **exit code** | that the code was **earned** rather than emitted |
| 3 | **G118 / G112-amended** | the **source** config | what the built artifact **enforces** |

Each time, a signal that *represents* the property was checked instead of the property. Each time the
signal was real, correctly produced, and about the wrong object.

**The general rule: for anything BAKED — cooked config, embedded resources, compiled-in defaults,
generated headers, packaged assets — the source file is an INPUT, not the artifact. Check what the
artifact ENFORCES, by reading it back out of the running system.**

Practical form, and it is the same three words every time: **read it back.** A48 already says this for
in-process cvars; G114 extended it to OS-level levers; G118 extends it to secrets. The unifying
question is *"what would I observe if the thing I edited never reached the thing under test?"* — and
if the answer is *"exactly what I am observing now"*, the check is not a check.

⚠ **A guard that PASSES the unsafe case is worse than no guard.** All three instances share this: the
false pass is silent, it looks like diligence, and it retires the suspicion that would otherwise catch
the fault. When demoting such a guard, say **NECESSARY BUT NOT SUFFICIENT** rather than deleting it —
the source check still catches a source-side regression, it just cannot certify a build. (2026-08-19.)

---

### G120 — an UNVERIFIED MECHANISM attached to a real observation FORECLOSES routes that were never closed

G87 observed something true: a packaged launch at `MainWorld` ends up in `MainMenu`. It then attached a
mechanism — *"the redirect is ACTIVE, not a startup race"*, *"the title pulls MainWorld back to the
menu"* — and drew a conclusion from the mechanism rather than from the observation:

> *"Every 'get in early / get in late' approach is dead."*

**The mechanism was never verified, and it is wrong.** `MainWorld` is **not cooked into any staged
build**; the engine was falling back to `GameDefaultMap` on a failed browse. An exhaustive source search
finds **no** MainWorld→MainMenu travel anywhere — the only `OpenLevel` in the project runs the other
way. MainWorld was treated as unreachable for ~13 days on the strength of an explanation nobody tested.

**This is the mirror image of the failures already catalogued here, and the mirror is the point:**

| shape | example | what it costs |
|---|---|---|
| a **false positive** from an unverified mechanism | G116 — a signature with an incumbent producer | you believe something that is not there |
| a **false null** from an unverified lever | G114 — a lever that changed nothing | you believe nothing is there |
| **a false FORECLOSURE from an unverified mechanism** | **this** | **you stop looking** |

The first two get caught eventually, because someone re-runs the measurement. **A foreclosure is not
re-run by anyone** — that is what foreclosing means. It is the most durable of the three and the least
likely to be revisited, so it has to be caught at writing time.

**RULE: an observation and its explanation are separate claims and must be recorded separately.** Write
the observation as fact; write the mechanism as a hypothesis with its evidence, or as *"cause not
established"*. ⛔ **Never derive a SCOPE decision — "X is impossible", "that approach is dead", "do not
try Y" — from an unverified mechanism.** Scope decisions may only rest on the observation itself, which
here would have supported the far weaker and far more useful *"a packaged launch at MainWorld ends in
MainMenu; cause not established"*.

*(This is the same discipline `CaptureBench/tools/check_pose.py` already applies to a failing gate —
print the numbers, name the discriminator, and let the reader attribute — applied to prose instead of a
tool. And note the near-miss: G87's own headline rule, "check the level NAME, never the picture", is
correct and was never in question. The defect is entirely in the paragraph that explains why.)*
(2026-08-19.)

🆕 **INSTANCE (2026-08-26, owner-ruled) — THE POSITIVE FORM OF THE SAME RULE: A NAMED GAP IS A FACT;
A RECONSTRUCTED RECORD IS A FABRICATION.** `master`'s tip `9f52cab` landed on **2026-08-25**, a day
with **no session journal** — sessions run 060 (08-24) then 061 (08-26). The tidy move is to write the
missing journal from the commit and the surrounding record. **Ruled: do not.** What went in instead
was an explicit gap entry — *no journal for this commit; it is nonetheless recorded in the m34 gate
file's A2.1 table and in `CLAUDE.md`'s four-item staging line; its branch twin is `5495aa6`.*

A reconstructed journal is indistinguishable, to every later reader, from one written at the time —
so it inherits full trust while carrying none of the evidence, which is `G120`'s foreclosure failure
with the polarity flipped: instead of stopping people looking, it stops them **knowing they should**.
**Write the absence down and say where the commit IS recorded.** A named gap can be closed later by
someone with the evidence; a fabrication cannot even be detected.

---

### G122 — an ASSET census and a RUNTIME census are DIFFERENT NAMESPACES; a claim proven in one is UNPROVEN in the other until they are joined by an exact key

`G-1` established, per **external actor FILE**, which `BP_Stomper` instances leave `Trigger` empty:
**5 of 7**. A runtime motion leg then fired at a **runtime UAID name** and measured it perfectly
static — camera identical to one decimal across 10 frames, target bbox identical on all of them.

That is a clean measurement of the **wrong instance.** The one targeted was
`…_2086831169`, which joins to file `0E5JK19NZI4C74C00ZZ7N`: **one of the two trigger-BOUND
Stompers.** A bound Stomper standing still with nobody on the pressure plate is the **expected**
result and tests nothing — yet it matched a pre-declared *"MOVERS LOADED BUT STATIC"* halt exactly,
and reporting it would have sent the next stage down a different road on a false foreclosure (G120,
one turn after that gotcha was written).

**Neither census was wrong. Nothing joined them.** The file half keys on opaque GUID basenames
(`0E5JK19NZI4C74C00ZZ7N`); the runtime half keys on `BP_Stomper_C_UAID_<HEX>_<N>`; and **actor labels
do not exist in a cooked build** (`(no-label)`), so the human-readable name that would have bridged
them is gone by design (G91's *"targeting is label-free"*).

**The exact key exists and is cheap:** every one-file-per-actor `.uasset` contains its own object path
`…MainWorld:PersistentLevel.<Class>_C_UAID_<HEX>_<N>`, and that substring **is** the runtime name.
One scan joins the namespaces exactly.

**RULE: before believing a runtime measurement about an instance whose PROPERTY was established
asset-side (or vice versa), JOIN THE NAMESPACES BY AN EXACT KEY AND SAY WHICH INSTANCE YOU HIT.**
A per-instance property does not transfer between namespaces on class name, count, or proximity.

⚠ **And the join must be checked in BOTH directions (G96).** The same exercise showed `BP_Fan` keys on
**`Triggers` (an ARRAY)**, not the scalar `Trigger`: keyed on the scalar, Fans read **4-of-4 empty
while 2 of 4 referenced a `BP_PressurePlate`** — the two signals **disagreed**, which is the only
reason the wrong key was caught. `mainworld_join.ps1` now reports any such disagreement, and the
committed table `mainworld_instance_join.md` carries **class · file · runtime name · trigger status ·
whether runtime motion is OBSERVED / REFUTED / UNTESTED per instance.** ⛔ **`UNTESTED` is not
"presumed" anything** — the asset comment *"when no trigger is referenced it move constantly"* is
already **refuted at runtime for one unbound Stomper.** (2026-08-19.)

---

### G123 — a code path labelled REPORTING ONLY that can terminate the run is not a reporting path

`check_pose.py`'s B1-detail block carries the comment **"REPORTING ONLY — no verdict, no exit code, no
definition depends on anything below."** It then did this:

```
ratio = tuple((round(m / c, 4) if c else None) for m, c in zip(bbox, O.CALIB_BBOX))
TypeError: 'NoneType' object is not iterable
```

on any leg where `bbox is None` — i.e. **no bbox rows in the settle window**, which is the *normal*
case off-calibration, where the camera settles looking somewhere the target is not. The exception
propagated out of python, `run_leg.ps1` runs with `$ErrorActionPreference = "Stop"`, and **the harness
died — after the capture artifacts had already been written to disk.**

**Three properties made this worse than an ordinary crash, and they generalise:**

1. **The block's own header asserts it cannot do this.** A reader auditing the harness for
   result-affecting code would skip it *because of the comment*, which is exactly backwards.
2. **It fired on the NORMAL case of a newly-supported configuration**, not on a rare one. The `-Map`
   parameter had just made off-calibration levels reachable; the first such leg hit it.
3. **The artifacts already existed.** The run had succeeded; only the report died. A less careful
   reading would have recorded "the leg failed" for a leg that produced a complete, valid session.

**RULE: a reporting path must be unable to change the outcome — including by raising.** If a block is
labelled reporting-only, it must degrade to a printed message on every input the surrounding code can
hand it, *especially* the "nothing to report" input. **The absence of a thing to report is the input
most likely to be untested**, because the author was looking at a case that had one.

*(Fixed by a `bbox is None` branch that says plainly that B1 has nothing to judge — explicitly NOT a
pose reading and NOT a pose failure, which is the honest label where "failed" would name a cause the
gate has not established. Same family as G113 — an exit code emitted but never earned.)* (2026-08-19.)

---

### G124 — an AGGREGATE component's bounds defeat EVERY bounds-based guard at once, and the same property makes its label wrong

Measured on `InstancedFoliageActor_0_0_0` in MainWorld, shipping defaults, on the m25 build:

| guard | intended job | what it did |
|---|---|---|
| **poll-radius cull** (`GPollRadius` 1800 cm) | reject actors too far to matter | **`poll_distance = −5396.0` — NEGATIVE.** It is `dist(pollOrigin, B.Origin) − B.SphereRadius`, and the cluster's bounds sphere (~17,000 cm) **exceeds the distance to it**, so the value is negative **from anywhere in the level**. The cull can never fire. |
| **screen-coverage floor** (`GMinScreenCoveragePct` 6 %) | reject actors too small on screen | **`coverage_pct = 100`.** The union-bounds rect fills the frame. Vacuous. |
| **`IsUnoccluded`** (9 rays, first clear wins) | reject actors nothing can see | **`1/9` — the exact minimum.** Rays are traced to the **cluster's** AABB corners, which are hundreds of metres apart and mostly in open air. |
| **label rect** (`ProjectActorBoundsToScreenRect`) | say where the anomaly is | **`(0, 0, 1280, 720)` — the entire frame**, `coverage_ratio 1.0`, on 59/59 rows. |

**One property causes all four: the bounds describe the CLUSTER, not the drawn geometry.** A 252 m ×
217 m × 67 m box makes "how far", "how big on screen", "can anything see it" and "where is it" all
answer about a volume the size of a district.

**Why this is a gotcha and not just a fact about foliage:** ⚠ **the guards look independent and are
not.** A reader satisfied that *"distance, coverage and occlusion all agree it is a valid target"* has
three readings of **one number**. `UInstancedStaticMeshComponent` derives from `UStaticMeshComponent`
and `UHierarchicalInstancedStaticMeshComponent`/`UFoliageInstancedStaticMeshComponent` from that, so
they pass the type test **trivially**, and **nothing downstream treats them differently** — the only
ISM-aware line in the whole filter is an instance-count `> 0` check.

**Measured consequence:** hiding it changed the frame by **0.0069 mean luma** against a proper hide's
**0.1023–0.1116** — while the label claimed **100 % of the frame**. The change was real but lived in
**4 of 64 grid cells**; the other 60 were flat. **A bbox-only reading (A35's rule, which is correct
when the label points at the object) reports a real change and calls it a manifest hide.**

**RULE: before trusting a bounds-derived guard, ask whether the component AGGREGATES.** For ISM / HISM
/ foliage / spline-spawned meshes, `Bounds` is a container, not an object, and **every guard computed
from it inherits that.** → **`H5` class (ii)**. (2026-08-19.)

🚨 **GENERALISED, MEASURED, SAME DAY — AND IT IS NOT ABOUT AGGREGATION.** Sweeping every banked
`poll_distance`, **3 of 13 non-foliage selectable actors are NEGATIVE**:

| actor | component_class | `poll_distance` |
|---|---|---|
| **`BP_SpawnPad_C`** | **`StaticMeshComponent`** — *plain, not instanced* | **−114.8** |
| `BP_SplineSpawn_C` | `InstancedStaticMeshComponent` | **−19405.5** |
| `RoomBuilderSquare_C` | `InstancedStaticMeshComponent` | **−1737.8** |

⛔ **`BP_SpawnPad_C` IS A PLAIN `StaticMeshComponent`.** The mechanism is **OVERSIZED BOUNDS**;
aggregation is merely the most common way to get them. **A blacklist of instanced/foliage component
types would miss it entirely while appearing to close the hole.**

⚠ **`poll_distance == −1` is the STRUCT SENTINEL, not a measurement** — provenance returned
`valid:false` and no distance was computed. Those actors are **UNMEASURED, not small**. A genuine value
of exactly −1.0 would be indistinguishable from the sentinel; not observed, stated because the field
cannot tell them apart.

---

### G125 — a frame-identity MARKER is a per-frame differencer's contaminant, and it looks exactly like a finding

The `CaptureBench` marker exists to prove frame identity (A10), so **it changes every frame BY
CONSTRUCTION**. A claimed-versus-flank pixel differencer measures exactly that, so the marker registers
as a large, perfectly reproducible change — **in the same cells, at the same magnitude, in every leg.**

It was reported in a result before it was caught. A grid of per-cell change on an `H5` leg read
*"FOUR cells carry the change (0.1800, 0.1510, 0.1228, 0.0860)"* — **the first two were the marker.**

**What exposed it was running a SECOND leg and comparing grids:**

| leg | level | target | cells (0,1) and (0,2) |
|---|---|---|---|
| foliage | MainWorld | `InstancedFoliageActor` | 0.1800, 0.1510 |
| spawn pad | MainWorld | `BP_SpawnPad_C` | 0.1797, 0.1528 |
| control | **CB_GateLevel** | `StaticMeshActor_49` | 0.1808, 0.1570 |

**Three levels, three targets, three different anomalies — the same two cells within 0.006.** No real
signal does that. `CaptureBench.Marker.Top` is `0.80` of the half-height, i.e. near the top edge, and
`run_leg.ps1` defaults `-Marker 1`.

**FIXED AT SOURCE, NOT MASKED IN ANALYSIS.** The leg was re-run with **`-Marker 0`** rather than
excluding cells in the reader — masking would have hidden any *real* change that happened to fall
there. Corrected numbers: whole-frame mean 0.0069 → **0.0059**, grid peak 0.1800 → **0.1242**, top row
0.1800/0.1510 → **0.0018–0.0072**. ✅ **The finding STRENGTHENED** — the real change is smaller and more
concentrated than first reported.

**RULES.**
1. **Any instrument that writes into the frame must be OFF for a pixel measurement.** The marker earns
   its place proving frame identity; it has no business in a differencer's input.
2. **A signal identical across different subjects is an instrument, not a finding.** Cheapest possible
   check: run one more leg on a *different* target and compare. A per-cell map makes this visible;
   a single scalar would have hidden it inside the mean forever.
3. ⚠ **A35's in-bbox rule would NOT have caught this here** — the contaminated cells were *outside* the
   bbox on the CB_GateLevel control, so the in-bbox score was clean and the contamination only surfaced
   when the measurement was widened to the whole frame for `H5`. **Widening a measurement can import
   contaminants a narrower one excluded by luck.** (2026-08-19.)

### G126 — `WasRecentlyRendered()` is TRUE for a SHADOW-ONLY contributor, so "the engine says it rendered" is not "it put pixels on screen"

Costing `C-3` as an `H5`/`H4` cure candidate meant reading what the engine actually writes. It is not
one signal — **`LastRenderTime` is bumped from three places with two different meanings.**

| writer | flag | meaning |
|---|---|---|
| `SceneVisibility.cpp:2493` | `bUpdateLastRenderTimeOnScreen=**true**` | visible **and** `PrimitiveDefinitelyUnoccludedMap` set |
| `ShadowSetup.cpp:1672` | `bUpdateLastRenderTimeOnScreen=**false**` | **in a shadow-casting pass** |
| `ShadowSetup.cpp:1909` | `bUpdateLastRenderTimeOnScreen=**false**` | ditto |

`AActor::WasRecentlyRendered` reads `LastRenderTime` (`Actor.cpp:1989-2000`), which the shadow path
**does** bump. ⇒ **an object contributing only a shadow — no direct pixels — reads as "recently
rendered".**

🚨 **This is not hypothetical here: `SM_Ramp2` is exactly that shape** — peak change **OUT** of its
bbox **0.2955** against **IN** **0.1785**, marker-off, on a legitimate target.

⚠ **The finer-grained field exists and is NOT the same one:** `LastRenderTimeOnScreen` is bumped only
by the `true` path. `WasRecentlyRendered()` does **not** use it.

**RULES.**
1. **`WasRecentlyRendered()` answers "did the renderer touch this", not "did the viewer see it".**
   For a drawn-extent question it is the wrong instrument at any tolerance.
2. **It is BINARY** — it cannot distinguish a target drawing 4 pixels from one drawing 400,000, which
   is the entire `H5` class-(ii) question.
3. ⚠ **It is LATENT in two stacked ways** — written from the render thread, *"up to a frame behind
   the game thread"* (`PrimitiveComponent.h:810-814`), **plus** occlusion-query buffering
   (`FOcclusionQueryHelpers::GetNumBufferedFrames`). *"Recently"* is a tolerance, never an instant.
4. ✅ **What it IS good for:** it is the **cheapest** thing that is genuinely occlusion-aware, because
   the `true` path is gated on `PrimitiveDefinitelyUnoccludedMap`. That makes it a candidate for
   `H4`'s question and **not** for `H5`'s. (2026-08-19.)

### G127 — the FILTER ships in every configuration; every PIXEL MEASUREMENT is compiled OUT of Shipping. They are not in the same module and cannot simply call each other.

The `H5` filter under study — `IsRenderableComponent`, `IsUnoccluded`, the selector, every anomaly —
lives in **`AnomalyInjector`**: deps `Core`/`CoreUObject`/`Engine`/`InputCore`, **no render deps**,
built in **every** configuration including Shipping.

Every candidate that measures pixels lives in **`AnomalyCapture`**, which declares
`ANOMALY_CAPTURE=**0**` in Shipping and pulls `Renderer`/`RHI`/`RenderCore`/`Slate` **plus the
Renderer PRIVATE include path** only when `Target.Configuration != Shipping`
(`AnomalyCapture.Build.cs:24-43`).

⇒ **"just have the selector ask the mask" is a MODULE-SHAPE DECISION, not an implementation
detail.** Either the cure is non-Shipping-only, or `AnomalyInjector` grows render dependencies.
*(CLAUDE.md's invariant contemplates those deps, so this is a cost to price — not a prohibition.)*

⚠ **AND THE SECOND HALF, WHICH IS EASIER TO MISS: a pixel measurement CANNOT INFORM A SAME-FRAME
PICK-TIME DECISION.** The SVE runs on the **render thread** after post-processing and returns by
**async GPU readback**; `FAnomalySveCapturer::Drain_RenderThread` polls `IsReady()` and simply skips
a frame that is not ready. `feature/stencil-capture` budgets **12 frames** before abandoning a mask
(`HeldAges[i] > 12`). **Any pixel-derived cure is therefore a PRE-FLIGHT — arm, wait, decide — never
a predicate the selector can call inline.** Design accordingly, or the cure silently decides on
last-frame's answer. (2026-08-19.)

### G128 — DELIVERY MODE gates REPORTING, not MEASUREMENT — and one provenance field already crosses into the client artifact

It is natural to read *"the provenance sidecar is only written when delivery is OFF"* as *"provenance
is not computed in delivery mode"*. **Source says otherwise, and the distinction decides whether a
whole class of cure is viable.**

| line | what it does | delivery-gated? |
|---|---|---|
| `AnomalyCaptureSubsystem.cpp:1599` | `EvaluateSelectionProvenance(...)` — the 9 traces, the coverage rect, the poll distance | ⛔ **NO — unconditional** |
| `:1691` | `Out.CoveragePct = Ev.Provenance.CoveragePct` | ⛔ **NO** |
| `AnomalyLabelWriter.cpp:404` | writes `coverage_pct` into **`annotation.json`** | ⛔ **NO — both modes** |
| `AnomalyCaptureSubsystem.cpp:1720` | writes **`selection_provenance.json`** | ✅ **YES** |

⇒ **the measurement always runs and one of its outputs already reaches the client.** Only the
*internal sidecar file* is suppressed. **A cure is not blocked by delivery mode; it is blocked only
if it needs a channel that does not already exist** — and `annotation.json` is written in both modes.

🆕 **Related, and it removes a second assumed blocker:** `annotation.json` already emits
**`mask: {provided: false}`** and **`depth: {provided: false}`** on every event, hardcoded, in every
delivered artifact today (`AnomalyLabelWriter.cpp:452-459`). **The slots a mask- or depth-based cure
would report through are already in the shipped contract**, so populating them changes a **value**,
not the field **shape**. Adding sub-fields underneath them would be a shape change.

⚠ **The real constraint is SHIPPING, not delivery** (→ `G127`) — and capture is already compiled out
of Shipping, so a client capturing at all is on a non-Shipping build. (2026-08-19.)

### G129 — a new GLOBAL SHADER cannot ride the code-only hot-swap, and a default-OFF switch does NOT make it inert

**G103** established that a code-only change stages as an **exe hot-swap** — ~85 s build, one file
copy, **no cook**. That is true for C++ and it is **false the moment the change adds an
`IMPLEMENT_GLOBAL_SHADER`.**

`m26` slice 1 added `FAnomalyVisibleMaskPS` (`/Plugin/AnomalyInjector/Private/AnomalyVisibleMask.usf`).
The exe hot-swapped cleanly and **A44 confirmed every new symbol in the staged binary, both
encodings** — including the shader's virtual path. **The build then died at startup, 3 of 3 attempts,
with no artifact:**

```
Fatal error: [ShaderCompiler.cpp] [Line: 6931]
Missing global shader FAnomalyVisibleMaskPS's permutation 0, Please make sure cooking was successful.
```

**Global shaders live in the cooked container, which the hot-swap does not touch. The exe half moved;
the shader half did not** — `G121`'s quartet, biting from the other side.

🚨 **TWO PROPERTIES THAT MATTER MORE THAN THE FAILURE ITSELF:**

1. **IT FIRES AT ENGINE INIT, before anything runs.**
2. 🚨 **IT FIRES WITH THE FEATURE'S SWITCH *OFF*.** Global shader-map verification **does not consult
   a runtime cvar.**

⇒ **`IMPLEMENT_GLOBAL_SHADER` IS NOT GATED BY ANY RUNTIME SWITCH. A default-OFF console variable does
not make a global shader optional — the binary cannot boot without it.**

⚠ **AND THAT RETIRES A PRECEDENT FOR THIS CLASS OF CHANGE.** `S3a` earned its strongest gate from
*"switch-OFF inertness is STRUCTURAL — there is no way to reach the code."* **A global shader has no
such state.** Its cost is paid at load, unconditionally. **"Inert when off" is UNOBTAINABLE by a
switch here**, so a gate of the form *"byte-identical with the switch off"* must instead be a
**CONTROL PAIR against a build that does not contain the shader at all**.

**RULES.**
1. **Adding a global shader is a COOK-CLASS change, not a hot-swap-class one.** Budget the cook, and
   remember it retires the pak half of the build identity every prior measurement was taken on.
2. **A44 passing is NOT sufficient for a shader change.** The scan proved the symbol reached the
   binary and the build still could not boot — **the binary is not the whole artifact** (`G119`'s
   principle, on the half `G121` names).
3. **Do not plan switch-OFF inertness for anything that participates in a cooked map** — shaders,
   cooked assets, generated tables. Ask *"does this exist before my switch is read?"* before
   promising inertness. (2026-08-19.)

### G130 — an operation's WORKING SET is not its OUTPUT SIZE, and running out of room mid-way yields a half-written artifact behind a system that still starts

**Disk became a blocker twice in six parts.** Both times the estimate came from the **output**: the
cooked `.ucas` is **284 MB**, so a cook "needs a few hundred MB". **It does not.** The cook also
writes `Saved\Cooked`, `Saved\StagedBuilds` **and** the archive copy — a transient requirement of
**multiple GB**, none of which survives into the artifact you measured.

**Measured 2026-08-19, and the shape is worth keeping:**

| | |
|---|---|
| free after the PART TWELVE prune | **19.12 GB** |
| free six parts later | **0.94 GB** |
| what consumed it | two full UBT builds with UHT, three engine-fatal launches, shader-compile attempts, eight capture legs |
| `StackOBot\Intermediate` alone | **14.54 GB** |

🚨 **THE FAILURE MODE IS THE POINT, AND IT IS NOT "IT STOPS".** A cook that exhausts the disk part-way
leaves a **HALF-WRITTEN CONTAINER BEHIND A BUILD THAT STILL BOOTS.** The exe is fine, the game starts,
and the container is short — **an artifact that presents as healthy.** This project already has the
vocabulary for that: it is `G118`'s *"a guard that PASSES the unsafe case is worse than no guard"*,
and `m19`'s *"gate on PIXELS, not on a counter"*, applied to storage.

**A second-order trap, hit this turn:** the plan was *"bank the unbanked evidence FIRST, then free
space."* **That ordering was impossible** — banking 3.89 GB needs 3.89 GB, and only 0.94 GB existed.
⇒ **When preservation and cleanup contend for the same resource, free ONLY what contains no evidence
first** (`Intermediate`, `.vs`), **then preserve, then free the verified duplicates.** The intent —
*never delete evidence before it is safe* — is preserved; the literal order is not achievable.

**RULES.**
1. **Check free space BEFORE starting a cook, a full rebuild, or a bulk copy** — a go/no-go floor, not
   a glance afterwards. Runbook §8.6 step 0 carries the figures.
2. **Estimate from the WORKING SET, never from the output.** Ask *"what does this write that it later
   deletes?"*
3. **A copy that runs out of space leaves a PARTIAL directory that looks like a real one.** Delete the
   partial immediately and re-verify the source — a half-copied bank entry is indistinguishable from
   a complete one by name alone. *(Measured here: 652 files / 891 MB of a 21-session copy landed
   before the disk filled.)*
4. **Report free space after each freed tree, not only at the end** — a single before/after number
   hides which step actually bought the room. (2026-08-19.)

### G131 — a plugin that declares a GLOBAL SHADER must load at `PostConfigInit`, and a SUCCESSFUL COOK is not evidence the shader reached the container

**Two distinct failures, in sequence, both of which report success at the step that caused them.**

#### (1) The cook ran on STALE EDITOR BINARIES — `G47`, hit again

`m26` slice 1 added `IMPLEMENT_GLOBAL_SHADER(FAnomalyVisibleMaskPS, …)`. The full cook reported
**`BUILD SUCCESSFUL`, ExitCode=0, 39m 26s**, cooked *"16.51 MB for 629 Global shaders"*, and the map
gate passed on all four maps. **The packaged build then died at engine init on
`Missing global shader FAnomalyVisibleMaskPS's permutation 0`.**

**Cause, measured not inferred — A44 applied to the EDITOR dll:**

| symbol | stale `UnrealEditor-AnomalyCapture.dll` (18-08, 473,600 B) |
|---|---|
| `AnomalyVisibleMask` | **0** |
| `/Plugin/AnomalyInjector` | **0** |
| `IAI.Capture.Mask` | **0** |
| `IsHideTypeAnomaly` | **1** ⇒ **the scan is SOUND, not blind** |

**The cook commandtlet is `UnrealEditor-Cmd` — it loads EDITOR dlls.** Ours were two days old, so
`AddShaderSourceDirectoryMapping` never ran, the `.usf` was never found, and the shader was never
compiled — **while the cook reported success.** `Build.bat StackOBotEditor …` fixed it in **45 s /
22 actions**, taking the dll to **590,336 B** with every symbol present.

⚠ **`G47` already said this since `m8`. The RUNBOOK did not** — §8.6's recipe builds only the GAME
target. **The knowledge existed and the recipe didn't carry it**, which is how it was missed. §8.6
now has an explicit editor-rebuild step.

#### (2) 🚨 The real blocker: `LoadingPhase`

With fresh editor binaries the cook **crashed at commandlet startup**, and the engine named the fix:

```
Assertion failed: !bInitializedSerializationHistory  [RenderCore/Private/Shader.cpp:246]
Shader type was loaded after engine init, use ELoadingPhase::PostConfigInit on your module
to cause it to load earlier.
```

*(The `EXCEPTION_ACCESS_VIOLATION` in `UClassRegisterAllCompiledInClasses()` underneath it is
downstream noise — the assertion above it is the cause.)*

**Global shader types must register BEFORE the shader serialization history is initialised.**
`AnomalyCapture` is `"LoadingPhase": "Default"`, which is **after** engine init.
⚠ **And it is not a one-line flip: `AnomalyCapture` depends on `AnomalyInjector`, also `Default`, and
a module cannot load before its dependency.**

**RULES.**
1. **Adding a global shader to a plugin is a PLUGIN-DESCRIPTOR change, not just a code change.**
   Budget the `LoadingPhase` question before writing the shader.
2. **Before any cook that must pick up new code, REBUILD THE EDITOR TARGET and A44-scan the EDITOR
   DLL** — not only the staged game exe. **`BUILD SUCCESSFUL` is not evidence the shader is in the
   container**; only booting the packaged build is.
3. **The standard pattern is a small dedicated module at `PostConfigInit` that declares only the
   shader**, leaving existing modules' load order untouched. Flipping a module that other modules
   depend on changes load order for all of them. (2026-08-20.)

### G132 — `GFrameCounter` increments BEFORE `OnEndFrame` broadcasts, so an end-of-frame sampler keyed on it looks at the NEXT frame's id

An `FCoreDelegates::OnEndFrame` handler that filters work by `Id == GFrameCounter` matches nothing,
every frame, silently. The engine increments the counter mid-`FEngineLoop::Tick` — **after** the
world tick, **before** the end-frame broadcast:

```
LaunchEngineLoop.cpp:5568   GFrameCounter++;                        // after world tick + render kick-off
LaunchEngineLoop.cpp:5623   FCoreDelegates::OnEndFrame.Broadcast(); // counter is already N+1 here
```

So a request stamped with `GFrameCounter` during a subsystem `Tick` (value N) can never be found at
`OnEndFrame` by comparing against `GFrameCounter` (value N+1). Hit on the first M-4 instrument
build (journal 045 §170): **0 `M24 ENDFRAME` lines against 30 `M23 PASS` lines** — the pre-declared
B4 branch ("instrument did not report") is what surfaced it before any conclusion was drawn from
the silence.

**RULES.**
1. **Do not re-derive "what happened this tick" from `GFrameCounter` at `OnEndFrame`** — record the
   ids explicitly at the point of action and consume that list in the handler.
2. **The two sides of a cross-phase join must be verified to EMIT before the join is read** — an
   empty join side is indistinguishable from "nothing to report" unless a control quantity (here,
   the PASS line count) says the other side was live. Same family as G96: the blindness was visible
   only because the expected count was known. (2026-08-20.)

### G133 — the StencilDummy 255 detector fires on AT MOST ONE PIXEL, and whether that pixel fires depends on what the camera happens to show

D-3 established that when custom depth is not produced the engine binds `StencilDummy` — a **1×1**
texture filled with 255 — and §149 said the shader read "returns 255 AT EVERY PIXEL." **The mask's
detector never saw that.** Two gates sit between the binding and the detector: `.Load` on a 1×1
texture returns **0 for every out-of-bounds pixel**, so 255 exists only at texel (0,0); and the
mask shader also requires the custom-DEPTH comparison to pass there, which against the depth dummy
happens only where the scene shows the far plane. **Measured across the whole milestone: every
single 255 fire was `unassignedCount=1` — one pixel — and on the `P26_FIX2_RAMP` leg, four armed
frames whose pass never ran produced NO fire at all** (the top-left pixel was not far-plane), so
four zeros from an unproduced pass were CONTRIBUTED and the event read **`MEASURED_ZERO` on the
A35 control** — the exact clean-looking false zero Ruling 1 warned about, produced by the
detector's own environmental dependence.

**RULES.**
1. **A detector whose FIRING is contingent on view content cannot certify anything by SILENCE.**
   The pass-ran discriminator is the extent datum (`customStencilExtent` 1×1 vs view-sized) —
   direct, per frame, content-independent, and already collected since M-2.
2. **A measurement may CONTRIBUTE only when the instrument is PROVEN to have run for that frame**
   — "no anomaly signal" and "instrument did not run" must never share the value 0. This is the
   whitelist-polarity rule (a missing check must never read as a passed check) applied one level
   deeper: to the renderer pass itself, not just our sampler.
3. When quoting an engine fallback's value, state where it can actually REACH your reader — "the
   read returns 255" was true of `CalcSceneCustomStencil` and false of the mask output, and the
   gap between those two sentences held a silent false zero. (2026-08-20.)

✅ **CLOSED 2026-08-20 (journal PART TWENTY-SEVEN, `3beb3ba`):** `customStencilExtent` (1×1 dummy vs
view-sized real) is now a **CONTRIBUTION PRECONDITION** — a frame contributes only on positive
evidence the pass ran, in its own disjoint bucket `framesNoPass`, and the 255 detector is
**demoted to a SECONDARY signal**. Verified on the banked failure: `SM_Ramp2` went from
`MEASURED_ZERO` on event 1 to **`NOT_MEASURED` on all 8 events**, with L1–L4 unchanged.

### G134 — Nanite primitives cannot write custom depth in UE 5.1, so a custom-depth instrument is STRUCTURALLY BLIND to Nanite geometry — and a non-Nanite bench level can never show it

The `SM_Ramp2` control (F-6 item 2) came back with the custom-depth pass produced on **0 of 29
armed frames** while the target was tagged (verified), un-hidden (bracket samples 0/29), in-frustum
(projected bbox valid) — **and DRAWING: the PART ELEVEN hide-measurement `CM_CM_RAMP` ran at the
identical camera pose and hiding the ramp changed in-bbox pixels by 0.1785.** The chain, read from
source:

| # | fact | source |
|---|---|---|
| 1 | `SM_Ramp` serialises a non-default `NaniteSettings` with `bEnabled` — the Nanite-enabled signature | `Content\...\Modular\SM_Ramp.uasset` |
| 2 | **`Nanite::FSceneProxy::GetViewRelevance` NEVER sets `bRenderCustomDepth`** — both branches | `NaniteResources.cpp:941-1010` |
| 3 | `View.bHasCustomDepthPrimitives` rises ONLY from that relevance flag | `SceneVisibility.cpp:2470` |
| 4 | the 5.1 custom-depth pass has **no Nanite path** — it rasterises classic mesh draw commands only | `CustomDepthRendering.cpp` (zero Nanite references) |

⇒ **setting `bRenderCustomDepth` on a Nanite component succeeds, verifies on read-back, and can
never reach a pixel.** The property write is not the capability.

⚠ **The trap's shape is G124's: the loudness is environmental.** The calibration control
(`StaticMeshActor_49`) lives in the script-built gate level on plain meshes, where the instrument
works perfectly — an entire milestone of green controls on a bench that could not exhibit the
blindness. Only the second control, on real level content, could. **A control set must span the
PRIMITIVE CLASSES of the content it certifies for, not just the geometry sizes and poses.**

**RULES.**
1. **Before adopting a rendering-feature-based instrument, enumerate which primitive classes feed
   that feature in the TARGET ENGINE VERSION** — Nanite, landscape, water, skeletal, ISM each have
   their own paths, and support tables move between versions.
2. **A target's measurability is a property to CHECK, not assume** — for C-1 on 5.1, a Nanite mesh
   is selectable, taggable, verifiable, and permanently unmeasurable; without a pass-ran
   precondition (G133) it reads as a clean zero, the false-veto direction.

🚨 **DO NOT READ `framesNoPass` AS A NANITE COUNTER — IT IS NOT ONE, AND THE DISTINCTION WAS ONE
REPORT AWAY FROM BEING LOST.** The counter's definition, fixed here:

> **`framesNoPass` counts frames where the custom-depth pass did not produce for this target.
> Causes include Nanite geometry (this gotcha), frustum culling, and any other route by which the
> target is absent from the view's relevant set. It is NOT a Nanite counter. In all cases the frame
> is discarded and the event tends toward `NOT_MEASURED`, which ADMITS.**

**Measured (journal PART TWENTY-NINE §200.1), on a DELIVERY-OFF leg with a plain non-Nanite
`Cylinder`:** as the camera drifted and the target left the frustum, `coverage_ratio` fell
`0.0685 → 0.0379 → 0, 0, 0, 0` while `framesNoPass` rose `0 → 3 → 4, 4, 4, 4` **in lockstep** —
the same mechanism (`SceneVisibility.cpp:2470`: relevance runs over the VISIBLE set) reached by
culling rather than by Nanite. **Had that gone unrecorded, `G134` would have inherited a wrong
denominator and every future `framesNoPass > 0` would have been read as evidence of Nanite.**
3. On a Nanite-heavy host title this limit is the COMMON CASE, not the corner — it belongs in the
   cure's scope statement, not its footnotes. (2026-08-20.)

**🆕 SCOPED 2026-08-20 (journal PART TWENTY-SEVEN), and the scoping cuts both ways.** An asset-side
name-table sweep classified the target set: **both `H5` instances are NON-Nanite** (`SM_Bush`,
`SM_GenericPlane`), so the cure reaches the cases that motivated it; and the discriminator closes —
**`StaticMeshActor_49` is `/Engine/BasicShapes/Cube`, and `make_gate_level.py:54-58` builds the
whole calibration level from `/Engine/BasicShapes/`.** The signature predicted measurability on
**5 of 5** targets. ⚠ **THE BAD HALF: StackOBot's own authored structural geometry — walls, floors,
platforms, pillars, roofs, pipes, fences, crates, doors, ramps — is overwhelmingly Nanite (46
assets carry the signature), while foliage and simple planes are not. So "common case" is MEASURED
ON THIS TITLE, not projected about a hypothetical one, and the two `H5` instances are reachable
because of what they happen to be made of, not because `H5` favours non-Nanite geometry.**
⚠ **A second-order consequence worth its own line: every A35-shaped legitimate target this project
has measured is Nanite, so the A35 over-fire risk cannot be tested on this bench at all** — the
`N-2` control's A35 property goes into the tag as UNTESTED rather than being quietly dropped.
📌 **Method note: the classifier is `CaptureBench/tools/nanite_signature_scan.py`, and it is
EVIDENCE, not a measurement** — it prints its own two weaknesses (the struct can serialise for a
different sub-field; `bEnabled` is a generic name), and it is load-bearing only as a DIFFERENTIAL
across a set whose measured behaviour is already known. The editor bridge was refused
(`Connection refused`), so `A59` corroboration was **abandoned rather than worked around**.

---

### G135 — a calibration environment built from a RESTRICTED ASSET SET cannot exhibit defect classes that depend on assets outside it, and the blindness presents as a CLEAN PASS

`CB_GateLevel` is built by `make_gate_level.py:54-58` **entirely from `/Engine/BasicShapes/`** —
Cube, Sphere, Cylinder, Cone. Every one is non-Nanite. The `m26` mask instrument was therefore
green on that level for an entire milestone **while being structurally incapable of measuring
Nanite geometry (`G134`)** — a limit that only appeared when a control was finally run on real
level content (`SM_Ramp2`, journal PART TWENTY-SIX).

**The failure mode is the point: the bench did not report a gap, it reported success.** Nothing in
a run on BasicShapes geometry can distinguish *"the instrument works"* from *"the instrument works
on everything this level contains."*

🚨 **THE TENSION IS REAL AND IS NOT RESOLVED HERE, DELIBERATELY.** The properties that make
`CB_GateLevel` a good instrument — fully controlled, script-authored, deterministic, frozen — are
**the same properties that make it unrepresentative of shipped content.** You cannot have both in
one level. ⛔ **Do NOT "fix" this by changing `CB_GateLevel`**: it is frozen, `m25`'s certifications
are expressed in it, and `G99` guards it. **The correct response is knowing what it cannot show,
and running the ship-gate controls somewhere else.**

**RULES.**
1. **Enumerate what your calibration environment is MADE OF, not just what it is shaped like** —
   asset features (Nanite, landscape, water, skeletal, instanced, translucent, WPO) are defect-class
   dimensions, and a level that contains one value of each certifies only that value.
2. **A ship gate needs at least one control on REAL CONTENT**, precisely because the calibration
   level cannot surprise you. `N-2` on `SM_Ramp2` is what caught `G134`; it earned its place in one
   leg.
3. **Say what the environment cannot show, in the certification** — an instrument certified on a
   restricted set is certified FOR that set until proven wider.

*(Same family as `G107` — a calibration set that brackets a regime without containing it — and `P8`
— TAU is not pose-invariant. All three are "the control was valid and the conclusion was not."*
*Found in journal 045 PART TWENTY-SEVEN by asking why the control worked rather than being*
*satisfied that it did.)* (2026-08-20.)

---

### G136 — an ABSENCE-OF-FINDING is only as good as the SURFACE that was searched

Code reported that Play-In-Editor cannot be started from a script, having enumerated
`unreal.LevelEditorSubsystem` and the `unreal` namespace: only `editor_play_simulate()` exists and
there is no `editor_request_begin_play`. **The conclusion was correct. The check was not.**

**The `unreal` namespace is the wrong surface for a CUSTOM BRIDGE.** `unreal-mcpython` is bespoke,
was extended for this project on 5.1, and a start-PIE endpoint would live C++-side over
`GEditor->RequestPlaySession` — reachable from a bridge and invisible to a Python-namespace scan.
The owner challenged the foreclosure from project history (*"the M0 gate split was explicit — the
owner presses Play; Code drives everything after that"*) and named the surface that should have been
searched.

**Re-run properly: all 62 advertised bridge endpoints enumerated by category** — actor 17, asset 2,
behavior_tree 13, blueprint 11, editor 6, game 3, material 10, util 3. **None starts Play or
Simulate.** The finding SURVIVED — but it survived a test it had not previously been given.

**RULES.**
1. **An absence-of-finding is a WEAKER claim than a positive measurement, and it is only as good as
   the surface searched.** State which surface, and why that is the surface where the thing would
   live.
2. **For anything custom — a bridge, a plugin, a wrapper — enumerate ITS OWN advertised surface,
   not the surface of the thing it wraps.**
3. ⚠ **This is `G120` with the safety catch removed.** A false positive gets re-run; a false null
   gets re-run; **a false FORECLOSURE is not re-run, by definition.** Here the only thing that
   tested it was the owner's memory of a receipt. Do not rely on that being available next time.

*(Contrast, from the same session: the 982-component custom-depth-writer census, the five-stage
stencil-inertness confirmation and the direct `nanite_settings.enabled` reads are POSITIVE
measurements and were never exposed to this failure mode.)* (2026-08-20.)

---

### G137 — a VIEW-LEVEL property used as a PER-TARGET precondition admits evidence that does not exist

`m26`'s mask decides whether a frame may contribute by testing `Mask.CustomStencilExtent` — 1x1
means the engine's `StencilDummy` is bound, view-sized means custom depth was produced. That answers
**"was custom depth produced AT ALL for this view this frame"**. It is used as **"did THIS TARGET
write custom depth"**.

Those are different questions, and the gap is not academic: the extent goes view-sized if **ANY**
primitive in the scene writes custom depth — it need not be a tagged target — and with
`AnomalyStencilTag` never untagging until `RestoreAll()` at `EndRun`, tags also accumulate across a
run.

**MEASURED (`I11-A`, journal 046), five legs, two independent routes, every gate passed:**

```
lever OFF   framesNoPass=4  framesContributed=0             -> NOT_MEASURED -> ADMITTED
lever ON    framesNoPass=0  framesContributed=4  maxCount=0 -> MEASURED_ZERO -> VETOED
```

Same target, same map, same seed, same session shape, pose matched to 0.175 deg. **The only change
was `bRenderCustomDepth` on one unrelated lamp.** Four frames carrying no evidence about the target
were contributed as clean zeros, and the event was deleted from `annotation.json`.

**RULES.**
1. **"The instrument was running" and "the instrument saw THIS subject" are different claims.** A
   per-frame or per-view answer substituted for a per-subject one converts *no evidence* into
   *measured zero* — silently, and in the data-destroying direction.
2. **State the SCOPE of every precondition next to its name** — per frame, per view, per target.
   `bPassRan` reads as per-target and is per-view.
3. ⚠ **A structural safety argument can be TRUE while the safety property is UNSOUND.** `m26`'s
   *"no code path lets a magnitude move an event between the two zeros"* is correct and is NOT
   withdrawn. What failed is the assumption underneath it — that an event reaching `MEASURED_ZERO`
   had been measured at all. **Check the ASSIGNMENT into a state, not only the transitions between
   states.** (2026-08-20.)

---

### G138 — an expectation written for one environment, read in another; write validity gates as CATEGORIES

`I11-A`'s pre-declaration required the lever to change `customStencilExtent` and wrote the
expectation as *"1x1 vs 1280x720"*. The legs ran in a PIE panel, whose view rect is **876x872**.

**The gate passed anyway, and that is the whole lesson: it was written CATEGORICALLY — "the extent
MUST DIFFER between lever-ON and lever-OFF" — with the numbers as illustration.** 1x1 versus 876x872
differs unambiguously. Had the gate been written as *"the extent must read 1280x720"* it would have
FAILED on a correct instrument in a valid run, and the failure would have looked like a lever that
did not fire.

**RULES.**
1. **Write a validity gate as the CATEGORY you actually mean.** "Must differ", "must be non-empty",
   "on screen or not" survive an environment change; a literal value does not.
2. **A number carried into a pre-declaration inherits the environment it was measured in** — the
   same shape as `G117` (`CALIB_BBOX` scoped to a resolution AND a target) and `G107`. Resolution,
   viewport, windowing and DPI are all environment.
3. **Report the mismatch even when the gate survives** — the next reader needs to know the written
   expectation and the measurement were not the same number. (2026-08-20.)

---

### G139 — a default that can come from an ini must echo its EFFECTIVE VALUE **and its PROVENANCE**, or "the key did not take" is indistinguishable from "deliberately off"

`m26` shipped the mask behind `bMaskMeasure`, default **false**, with no ini key. `m27` added
`bMaskMeasureDefault`. Writing the key is the easy half. **The half that matters is that until
`m27`, THE LOG SAID NOTHING AT ALL WHEN THE MASK WAS OFF** — the only mask banner
(`AnomalyCaptureSubsystem.cpp`, the "m26 SLICES 1+2+3 ACTIVE" block) is *inside* `if (bMaskMeasure …)`.

So a delivered session with a missing, misspelt, or **silently ignored** key produced a log that was
byte-for-byte identical to one where the mask was deliberately off. **The failure and the intended
state were indistinguishable in the only artifact the client returns.**

⚠ **And `G88` makes the silent case the LIKELY one, not the exotic one:** in a packaged build
`GGameIni` resolves to the **COOKED** `DefaultGame.ini`. A loose ini beside the package is a **no-op**.
A client can set the key correctly, in a real file, and have it do nothing.

**RULES.**
1. **Echo the EFFECTIVE value, read back from the getter, not the value you think you set** (A48) —
   and echo it **UNCONDITIONALLY**, on the off path as loudly as on the on path. A feature that
   reports itself only when enabled cannot be diagnosed when it is not.
2. **Echo the PROVENANCE beside it** — ini key vs compiled default vs console override. "Off" and
   "off because your key never arrived" are different facts and only provenance separates them.
3. **State where the setting must live to take effect**, in the same line, because the reader
   holding the wrong file is exactly who needs telling.
4. This generalises past inis: any default with more than one possible source needs both halves.
   `bSveCaptureDefault` already did this and is the pattern worth copying.
(2026-08-20.)

---

---

⛔ **FILED DEFECT, DELIBERATELY NOT FIXED AT `m27` — `IAI.Capture.Mask`'s CONSOLE HELP STILL
CONTRADICTS THIS GOTCHA AND ITSELF. DELIVERABLE A3 IS *PARTIAL*.**

**The exact string, first line of the help in
`Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`:**

```
"m26 SLICES 1+2 - MEASURE AND REPORT (default OFF). ON: tag each fired target into custom stencil using "
```

**What A3 DID deliver** (verified in the shipped string): the help now describes the
**slice-3 zero-only veto** accurately — *"an event is removed from `annotation.json` IF AND
ONLY IF it is manifested AND its target was MEASURED at ZERO drawn pixels … there is NO
ratio and NO threshold"* — and it states **"Mid-run changes are ignored (stop first)"**.

**What A3 did NOT deliver, and both are in that one opening line:**
1. **`"SLICES 1+2"` is stale** — the body of the same help goes on to describe **slice 3**.
   ⚠ **A reader therefore hits an INTERNAL INCONSISTENCY rather than a plain error**, which
   is the harder kind to trust your way out of: the header and the body disagree, and
   nothing says which is current.
2. **`"(default OFF)"` is now only the COMPILED default.** After `m27` the **ini decides**,
   so a build with `bMaskMeasureDefault=True` runs with the mask **ON** while its own console
   help says OFF. **That is this gotcha's exact failure mode surviving inside the fix for it.**
3. The word **BISECT** never appears, though that is what the switch now is.

**WHY IT WAS NOT FIXED — the trade, recorded so it is not re-litigated as sloppiness:** a
one-character source change forces a rebuild **and a re-cook**, which **moves the exe hash and
INVALIDATES ALL FOUR `m27` GATE-3 LEGS** — for a string **no gate ever read**. Trading
certified evidence for cosmetics is the wrong direction. **Same trade already made
deliberately for `G118`'s cooked placeholder token and for the `m26` bench binary
(`5EA6AB92`, shipped one commit behind to preserve the binary nine gate legs ran on).**

🧭 **THE RULE FOR CLEARING IT: FOLD IT INTO THE NEXT MILESTONE THAT ALREADY REQUIRES A COOK.**
It costs nothing there and must not motivate a cook of its own. **`G118`'s cooked-placeholder
item travels the same way and should be cleared in the same pass.**
(Filed 2026-08-20, `m27`.)

---

⛔ **SECOND FILED ITEM, DELIBERATELY NOT FIXED AT `m28` — THE CAPTURE FORMAT IS NEVER ECHOED
BACK TO THE OPERATOR (`W2`).**

`capture_start` accepts a `format` field and the engine honours it end to end
(`AnomalyControlServerSubsystem.cpp` → `bFormatPng` → the writer). **But nothing reports it
back.** `ControlSnapshot.cpp` carries no capture-format field and the `capture_stopped` reply
does not include one, so **the dashboard's format select is fire-and-forget**: the operator
cannot confirm from the UI which format the run actually used.

It IS recorded after the fact in `run.json`'s `"format"` — so this is a **latency of
feedback**, not a loss of the record, which is why it is filed rather than fixed.

⚠ **WHY IT IS FILED HERE AND NOT IN `docs/invisible-anomaly-mechanisms.md`:** that ledger is
for **invisible-anomaly MECHANISMS** only. A missing echo is this gotcha's own subject —
*report the EFFECTIVE value* — so `G139` is the correct anchor. **Owner-ruled, `m28`.**

📌 **`m28` DID fix the other half (`W1`)**: the same parser used to fall back to PNG **silently**
on any unrecognised string, and it now warns. **The value is reported when it is WRONG; it is
still not reported when it is RIGHT.** See **`G144`**.
(Filed 2026-08-20, `m28`.)

---

### G144 — A PARSER THAT MAPS "EVERYTHING ELSE" TO A DEFAULT TURNS A TYPO INTO A SILENT BEHAVIOUR CHANGE

Until `m28` the control server chose the capture image format like this:

```cpp
const bool bPng = !Format.Equals(TEXT("jpeg"), ESearchCase::IgnoreCase);
```

**Three separate defects live in that one line, and all three are invisible at runtime.**

1. **`"jpg"` SILENTLY PRODUCED PNG.** The console command accepted **both** spellings
   (`AnomalyCaptureSubsystem.cpp`, and `IAI.Capture.Shot` likewise), so the SAME WORD meant
   different things depending on which surface you typed it into. Nothing anywhere said so.
2. **EVERY unrecognised string became PNG**, with no warning — a typo, a future caller, a
   different client, all silently redirected to the default.
3. **The failure is INVISIBLE IN THE ARTIFACT**, because `run.json` faithfully records `"png"` —
   the value that was actually used. The record is honest; it just records the wrong intent.
   ⚠ **A correct-looking artifact is the worst place for a defect to hide.**

**RULES.**
1. **Never write `bFlag = !(x == "oneValue")`.** Enumerate the accepted values explicitly and give
   the no-match case its own branch. A boolean derived by negating one comparison silently claims
   that everything in the universe except one string means the other thing.
2. **THE NO-MATCH BRANCH MUST BE LOUD**, and the warning must name **the unrecognised value AND
   the fallback taken** — "unrecognised format" alone does not tell the operator what they got.
3. **DISTINGUISH ABSENT FROM WRONG.** An omitted field is a caller legitimately taking the
   default and must stay silent; a present-but-unparseable field is an error and must not. `m28`
   treats empty/absent as silent and everything else non-matching as a warning.
4. **Accept every spelling your other surfaces accept, or make them all reject it.** The
   divergence is worse than either policy.
5. ⚠ **This is `G96` again:** the fallback branch existed for the whole life of the feature and
   had **never once been observed to fire**, because firing it produced no output. **A guard that
   cannot be seen firing is not a guard, and neither is a fallback.** Gate G for `m28` therefore
   requires the warning to be *demonstrated* firing, not merely present in the source.
(2026-08-20, `m28`.)

---

### G145 — PowerShell STRIPS EMBEDDED DOUBLE QUOTES from any argument it hands a NATIVE command

⚠ **THIS IS NOT A GIT GOTCHA.** It was first found on `git commit -m` and was originally written that
way; the third instance had **nothing to do with git** and forced the widening. **Any native
executable invoked from PowerShell 5.1 — `git`, `python`, `ffmpeg`, anything — receives its arguments
through Windows argument re-parsing, which consumes inner `"` characters.** A literal here-string
(`@'…'@`) does **not** protect you: it defeats `$`-expansion and the `G141` BOM trap, and neither of
those is this problem.

**MEASURED, THREE TIMES, ALL DURING `m28`:**

| # | what was written | what arrived |
|---|---|---|
| 1 | `git commit -m` body containing ``!Format.Equals("jpeg")`` | `!Format.Equals(jpeg)` — **committed silently** |
| 2 | `git commit -m` body containing `maps "everything else" to a default` | **the argument SPLIT**; git errored `pathspec 'else to a default …' did not match any file(s)` |
| 3 | `python ws_send.py <token> '{"type":"capture_start","format":"jpg"}'` | `{type:capture_start,format:jpg}` — **invalid JSON, silently rejected by the server** |

🚨 **THE QUIET FAILURES ARE THE EXPENSIVE ONES, AND TWO OF THE THREE WERE QUIET.** #2 errored and cost
nothing. #1 landed a commit *about string-literal matching* with exactly the characters carrying the
meaning removed, caught only by a deliberate read-back. #3 looked like a successful send — the client
printed `TX`, the socket closed cleanly — and the gate leg simply produced nothing, which is
indistinguishable from a feature that does not work.

**RULES.**
1. **PASS THE PAYLOAD IN A FILE, NOT AS AN INLINE ARGUMENT.** `git commit -F <file>`;
   `ws_send.py <token> <msgfile>`. This is the fix shape for the whole class, not a git trick.
   Author the file with the editor tool, never `Set-Content`/`Out-File` (`G141` — BOM).
2. **An inline argument is acceptable ONLY when it contains no `"` at all.** In this project — which
   quotes identifiers, log strings, ini keys and JSON constantly — that is almost never true.
3. **VERIFY THE PAYLOAD ARRIVED INTACT, at the receiving end.** `git log -1 --format=%B` after a
   commit; have the sender echo the exact bytes it transmitted. Same mechanical habit as `G115`'s
   diffstat, and it exists for the same reason: the failure is silent.
4. **A validating parse at the sender catches it before the wire.** `ws_send.py` now runs
   `json.loads` on every message before connecting, so a mangled payload fails loudly and locally
   instead of becoming an empty test result.
5. **Amending is correct for #1 and is not an exception to the prefer-a-new-commit rule:** the commit
   was **unpushed**, so nothing was rewritten for anyone else, and a follow-up commit that only fixes
   prose is worse than a clean amend. The rule guards shared history, not local drafts.
(2026-08-20, `m28`; widened from git-only to all native commands 2026-08-21 after the third instance.)

---

### G146 — a gate whose pass condition is an EQUALITY needs a companion that FAILS ON EMPTY INPUT

**`m28`'s `GATE D` is the control the whole milestone rests on:** run the same seed native and
downscaled, and every `bbox_norm` must be IDENTICAL, because `bbox_norm` never sees a pixel dimension.
If one moves, a pixel dimension reached the projection path and the design is wrong.

**IT PASSED. IT WAS MEANINGLESS.** Both legs contained **zero anomalies** — `positive_frames=0`,
`annotation.anomalies=0`, every burst logging *"fired nothing (zero-match / empty)"*. Comparing two
empty sets returns equal. 🚨 **THE EMPTIEST POSSIBLE RUN PRODUCED THE GATE'S CLEANEST PASS.**

The emptiness itself was an environment property, not a defect: `IAI.DumpCoverage` reported **69**
renderable-visible actors while `IAI.DumpVisible` reported **0**, because in an 875×869 PIE panel no
actor reaches the default 6 % screen-coverage threshold. With the cull off, the re-run pair carried
**19 valid bboxes per leg** and the gate became real.

**WHAT CAUGHT IT WAS NOT IN THE PRE-DECLARATION** — an added counter-check: *`bbox_px` rows differing
MUST be > 0, else nothing was actually rescaled.* `bbox_px` reading zero differences **alongside
`width`/`height` that plainly differed** is what exposed the hole.

**THE RULE (owner ruling, 2026-08-21).**

> **A gate whose pass condition is an EQUALITY needs a companion condition that FAILS ON EMPTY INPUT.
> Otherwise the emptiest possible run is its cleanest pass.**

**HOW TO APPLY IT.**
1. When drafting a gate, ask **"what is the emptiest input that satisfies this?"** If the answer is
   *"no data at all"*, the gate is not finished.
2. **Pair every "these must be IDENTICAL" with a "these must DIFFER"** on a quantity the change is
   supposed to move. One without the other is half a control.
3. **Put a NON-VACUITY MINIMUM in the gate text itself** — *"at least N valid rows per leg"* — so the
   check lives in the pre-declaration and not in whoever happens to read the output.
4. ⛔ **This is NOT a tolerance and does not reopen the ratio/threshold ruling.** *"More than zero
   data points"* is a categorical precondition on the INSTRUMENT, not a magnitude test on the SUBJECT.

⚠ **THIRD INSTANCE OF THE ORACLE SHAPE** — `G106` (the A54 oracle existed only in prose), `G142` (two
defects in a verification script, found while reporting a pass), and now this. **What all three share:
the INSTRUMENT was wrong while the PRODUCT was fine, and every time the wrongness presented as a CLEAN
RESULT rather than as an error.** That is why these keep costing a session each: nothing about a pass
invites a second look.
(2026-08-21, `m28`.)

---

### G147 — the host encoder PADS odd frames to even and MUST NOT be changed to scale or crop

`encode_watcher.py` runs ffmpeg with `-vf pad=ceil(iw/2)*2:ceil(ih/2)*2` because H.264 + `yuv420p`
require even dimensions and a PIE viewport is routinely odd (measured: **875×869**, and `1238×585`
in the original note).

🚨 **THE CHOICE OF *PAD* OVER *SCALE* OR *CROP* IS LOAD-BEARING AND THE REASON WAS NEARLY LOST.**
Padding adds ≤1 px at the **bottom/right**, which **leaves the top-left origin fixed** — and therefore
leaves **every `bbox_px` coordinate in `labels.jsonl` and `annotation.json` valid on the encoded
video**. A `scale` would move every coordinate; a `crop` from anywhere but bottom-right would move the
origin. Either would silently invalidate the labels against the mp4 while producing a video that looks
perfectly fine.

⚠ **HOW IT WAS NEARLY LOST, and this is the transferable half:** that reasoning existed **only as a
source comment**, and the canonical copy of the script (`anomaly-dashboard/host-tools/`) has been
comment-stripped. It survives today only in the owner's older working copy at
`D:\IntrusiveAnomalies\host-tools\`. **A rule whose only record is a comment in one of three
divergent copies is one cleanup away from being deleted by someone who thinks `scale` is tidier.**
Recorded here so the constraint outlives the comment.

📌 **`m28` makes the pad a no-op for downscaled runs** — `DeriveOutputSize` always yields an even pair
— but it is still required for NATIVE runs at an odd viewport, which is the common case in PIE.
(2026-08-21, `m28`.)

---

### G148 — NEVER CHANGE THE LEVEL FROM THE MCP BRIDGE: it executes Python INSIDE a world tick, and the load destroys the world that is ticking

**MEASURED — it crashed the editor outright (2026-08-21, `m28` smoke setup).** The call was an
ordinary-looking one:

```python
if ues.get_game_world() is None:                      # PIE not running - looked safe
    les.load_level("/Game/StackOBot/Maps/MainWorld")
```

```
Assertion failed: !LevelList.Contains(TickTaskLevel)
  FTickTaskManager::FreeTickTaskLevel()  <-  UWorld::FinishDestroy()
  <- IncrementalPurgeGarbage()  <-  UWorld::Tick()
  <- FMCPythonTcpServer::ProcessDataOnGameThread()  <-  FPythonScriptPlugin::EvalString()
```

**THE GUARD WAS NOT THE PROBLEM AND ADDING A BETTER GUARD WILL NOT FIX IT.** PIE genuinely was not
running. The problem is *where the bridge runs Python*: `FMCPythonTcpServer::ProcessDataOnGameThread`
evaluates the string **from inside `UWorld::Tick`**, so a level load tears down the very world whose
tick group is on the stack, and the tick task level is still registered when GC frees it.

**RULES.**
1. ⛔ **Do not call `load_level`, `new_level`, `open_level`, or anything else that destroys or swaps
   the world, from `util_execute_python`.** Console commands, property reads/writes, actor queries and
   `execute_console_command` are all fine — they do not destroy the ticking world.
2. **A map change is an OWNER action**, like pressing Play. That is not a limitation of the bridge to
   work around; it is the same `M0` split (**`G136`**) applied to the other end of the session.
3. If a deferred load is ever genuinely needed, it must be scheduled **off** the tick —
   `unreal.register_slate_post_tick_callback` or a timer — and that has **not been tried here**, so
   treat it as unverified rather than as the recommended workaround.
4. ✅ **The blast radius was zero and that is worth recording too:** every artifact was already
   committed, pushed and banked before the call. **The habit of banking and pushing at each stage is
   what turned an editor crash into an inconvenience.** (`G92`'s discipline paying off in the other
   direction.)
(2026-08-21, `m28`.)

### G140 — changing the SELECTABLE SET changes SEEDED SELECTION, so banked runs stop being comparable across the change

`m27` excludes `AInstancedFoliageActor` from `IsRenderableComponent`. That predicate feeds
`GetVisibleRenderableActors`, which is **the input to the seeded draw stream**.

⇒ **THE SAME SEED NOW PICKS DIFFERENT TARGETS.** This is correct and expected, and it is not a
defect — but it silently invalidates a comparison the project relies on constantly.

🚨 **EVERY BANKED MAINWORLD AUTO-POOL RUN IS NON-COMPARABLE TO ANYTHING CAPTURED AFTER `m27`.**
A post-change run at seed 0 will not reproduce the play-gate smoke's target list, and **that is the
change working, not a regression.** `m22`'s same-seed byte-identity gate is satisfied **WITHIN** the
new behaviour, never **ACROSS** the change.

**RULES.**
1. **Any edit to the selection predicate is a comparability boundary.** Date it, name it, and say
   which banked evidence it retires — the way a binary hash or a cook does.
2. **Do not diagnose a post-change seed mismatch as a bug.** Check the boundary first.
3. ⚠ **The blast radius is wider than the selector**: the same predicate serves the auto-injector
   **and** the dashboard's `GetVisibleRenderableActorInfos`, so the visible-set read-back moves too.
   That is by design (`G33`'s one-definition ruling) and is why `IAI.DumpVisible`'s set-identity
   assertion has to be re-checked after any such edit rather than assumed.
4. Targeted fire is **unaffected** — `TryFireSpecific` carries no viewport predicate, so the `=name`
   escape hatch still reaches an excluded actor deliberately.
(2026-08-20.)

---

### G141 — PowerShell's `-Encoding utf8` WRITES A BOM, and it has now corrupted files in this project twice in one session

`Set-Content`/`Out-File -Encoding utf8` in **Windows PowerShell 5.1 emits UTF-8 WITH a BOM**. The
bytes `EF BB BF` land at the head of the file and nothing complains.

**TWICE IN ONE SESSION (2026-08-20):**
1. A shell round-trip to add one parameter to `AnomalyLabelWriter.{h,cpp}` prefixed a BOM to both.
   Caught by the standing pre-commit diffstat habit (`G115`) — the diff showed
   `-#pragma once` / `+<BOM>#pragma once`, a change to line 1 nobody asked for. Reverted and redone
   with the editor tool.
2. A hand-written test `config.json` for the dashboard verifier, written with `Out-File -Encoding
   utf8`, made the verifier fail with *"Unexpected UTF-8 BOM (decode using utf-8-sig)"*.

⚠ **THE SECOND ONE IS THE INSTRUCTIVE ONE, BECAUSE THE TOOL WAS RIGHT.** A BOM'd `config.json`
breaks the browser's `JSON.parse` exactly as it broke Python's. The verifier catching it was the
guard WORKING, not a false alarm, and treating it as noise would have discarded a real signal.
*(The shipped `write_config.py` writes clean UTF-8 — verified, no BOM — so only hand-written files
are exposed.)*

**RULES.**
1. **Never write a tracked source or config file through `Set-Content`/`Out-File`.** Use the editor
   tool, which preserves encoding and line endings.
2. When a script genuinely must write a file, use
   `[System.IO.File]::WriteAllText($path, $text, (New-Object System.Text.UTF8Encoding($false)))`
   — the `$false` is "no BOM" and is the whole point.
3. **`G115`'s diffstat check is what catches this**, because a BOM shows as a change to line 1 of a
   file whose line 1 you did not touch. Read the diffstat before every commit.
4. A parser rejecting a BOM is reporting a REAL defect in the file. Fix the file, not the parser.
(2026-08-20.)

🆕 **THIRD SURFACE (2026-09-01): COMMIT MESSAGES.** A message written with
`Set-Content -Encoding utf8` and handed to `git commit -F` put a literal BOM at the **start of the
commit subject**, which then went to origin. The two messages authored through the editor tool in the
same session were clean; the one written by PowerShell was not. **Commit messages go through the
editor path only — never a PowerShell-written file.**

🚨 **AND THE PART THAT MAKES THIS WORSE THAN AN UGLY SUBJECT: THE SHELL THAT WROTE THE BOM STRIPS IT
WHEN READING IT BACK, SO IT HIDES THE DEFECT FROM ITS OWN TEST.** The check
`git log -1 --format=%s <sha>` piped into PowerShell and byte-tested returned **BOM = False** — a
false negative — because PowerShell removes U+FEFF while decoding git's stdout. The BOM was only
visible by reading the raw object outside the shell:

```
git cat-file commit <sha>   ->  body starts b'\xef\xbb\xbfdocs(r'
```

⇒ **Verify message bytes with `git cat-file commit`, never with a shell-decoded `git log`.** The
general form is the one that keeps recurring here: *a producing tool is not a witness to its own
output.* Same family as `G183`/`G188`/`G190` — in this workspace the shell is a defect surface, and
the defect and the blindness to it can come from the same place.
📌 Fix path used: `commit --amend` (message only — the tree OID was captured before and compared
after, identical) plus a lease-checked force push of that one branch. Standing rule now: chat may
authorise a lease-checked amend of an **unmerged branch's TIP** until the office has pulled it;
**master and tags never; anything deeper is STOP.**

### G142 — a VERIFICATION SCRIPT is a defect surface of its own, and its failures wear the costume of a BUILD failure

🚨 **BOTH DEFECTS BELOW WERE IN THE CHECKER, NOT IN THE BUILD, AND EITHER WOULD HAVE
MANUFACTURED A FALSE `COUNTS DISAGREE — STOP`** on a gate that was working perfectly.
Caught 2026-08-20 while running `m27`'s owner play-gate smoke — **while reporting a PASS**.

⚠ **WHY THIS IS ITS OWN GOTCHA AND NOT A FOOTNOTE: A FALSE FAILURE IS MORE EXPENSIVE THAN
A MISSED ONE HERE.** It costs the owner a round trip, and — worse — **it teaches him to
distrust a gate that was correct**, which is the one thing a gate cannot survive. `G118`
already records that a guard passing the unsafe case is worse than no guard; this is the
mirror image, and it is not obviously cheaper.

**DEFECT 1 — THE SCRIPT ASSUMED ONE CAPTURE RUN PER LOG. A LOG CAN HOLD MANY.**
The owner captured **twice in one game session**. A whole-log
`Select-String VETOED-OBJECT` therefore counted **run A's three lines** against **run B's
`vetoed_events = 0`** and would have reported a mismatch. Nothing about the log announces
that it spans two runs.

**DEFECT 2 — THE OBVIOUS WINDOW ANCHOR IS WRONG IN BOTH DIRECTIONS.** Scoping a run to
"from `Capture run STARTED` to `Capture run FINISHED`" is wrong at **both** ends, and the
two errors point opposite ways, so neither cancels the other:

| line | where it actually is | first symptom |
|---|---|---|
| the per-run mask ECHO (`READ THIS LINE, NOT THE INI`) | **BEFORE** the `STARTED` banner | reported the echo **MISSING** — read as "the ini did not take" |
| `M27 VETO SUMMARY`, `M26S3 G-11` | before `FINISHED` ✅ | (correctly scoped by accident) |
| the per-event `M26S1 EVENT` lines | **AFTER** the `FINISHED` banner | printed the **PREVIOUS run's** events under this run's heading |

Measured line numbers from that log, as the proof: run A `STARTED` 1859 · veto summary
2211 · `G-11` 2212 · `FINISHED` 2214 · **`M26S1 EVENT` 2216–2224**; run B `STARTED` 2245
· summary 2575 · `FINISHED` 2578 · **`M26S1 EVENT` 2580–2592**.

**THE CORRECT SCOPING RULE — write it into any future checker rather than re-deriving it:**

> A run's evidence spans **from after the PREVIOUS run's last `M26S1 EVENT` line, to this
> run's last `M26S1 EVENT` line.** The `STARTED` banner sits in the MIDDLE of its own run's
> evidence, not at the start of it. Anchoring on `STARTED` silently truncates both ends.

Safer still where it is available: **key on the SESSION ID**, which appears in both banners,
and take the Nth occurrence of each marker. Safest of all for a manual check: **restart the
game between captures** so one log holds one run.

**THE GENERAL RULE.** When a check fails, the FIRST hypothesis is that **the check** is
wrong, not the artifact — especially a check written in the same session as the thing it
checks, because it has never been exercised against a known-good input. That is `G96`'s
principle (a guard that has never fired is not a guard) applied to the **checker** instead
of the product. **Two known-answer inputs cost minutes: one run that SHOULD trip it and one
that should NOT.** Here the known-good input existed for free and was not used — the owner's
two runs happened to be exactly that pair, and the script only survived because the numbers
were read by hand afterwards.
(2026-08-20.)

---

### G143 — `git tag -l --format='%(objectname:short)'` PRINTS THE TAG OBJECT, NOT THE COMMIT, AND OUR TAGS ARE ANNOTATED

A cold-start instruction sheet asked for tag verification with:

```
git tag -l m26 m27 --format='%(refname:short) %(objectname:short)'
```

It printed `m26 4328961` and `m27 1756f52`, against expected commits `d6bee7a` and `4a92962`.
**Both numbers were wrong and the repository was perfectly correct.** For an **annotated** tag
`%(objectname)` is the hash of the TAG OBJECT — a real object with its own message, tagger and
date — and the commit hangs off it. `%(*objectname)`, with the asterisk, is the dereferenced
commit.

⚠ **THE COST IS NOT THE CONFUSION, IT IS THE HALT.** Bootstrap contracts here say *"if any of
that does not match, STOP and report before continuing"* — correctly, because a repo in an
unexpected state is exactly when work should not proceed. So a defect in the CHECK spends the
session's most expensive response on a non-event. This is **`G142`'s shape one level up**: the
checker was wrong, not the build, and it wore the costume of a build failure.

**RULES.**
1. **Verify a tag with `git rev-parse --short <tag>^{commit}`.** It is unambiguous, it cannot be
   got subtly wrong, and it reads identically for annotated and lightweight tags.
2. **A hash that "looks like a hash" is not evidence it is the RIGHT KIND of hash.** Both values
   above are valid 7-hex object names. Nothing about the output announced that it was answering a
   different question from the one asked.
3. ⚠ **Do not silently "fix" a bootstrap mismatch by adjusting the expectation.** Establish which
   side is wrong first. Here the instruction was wrong; had the tag genuinely moved, the same
   output would have meant something entirely different and far worse.
4. This generalises to every `--format` field that has a `*`-prefixed sibling: the unprefixed form
   answers *about the ref's own object*, not *about what it eventually points to*.
(2026-08-20, m28 Stage 0.)

---

## G149 — a guard drawn on a PROXY for the property you care about stops exactly where the proxy stops

`m29` gave `lod_popping` a **≥2 LOD guard**: a matched component qualifies only if its runtime LOD
count is at least 2. The reasoning is sound — a single-LOD mesh forced to a LOD pops **to itself**,
producing a positive label with no visible change.

**But LOD COUNT is a PROXY.** The property that actually matters is *"would forcing this LOD change
what is drawn, at this target's current on-screen size?"* — which depends on the mesh, the distance,
and what auto-LOD was already selecting. The proxy is only sound at the extreme: `count == 1` is
certainly invisible; `count >= 2` is **not** certainly visible.

**MEASURED on `SM_rock` (4 LODs, non-Nanite, MainWorld):** forced LOD 1 vs forced LOD 4, event-matched
across five events, differ by **~0.4 % of the silhouette** (≈110 px) — systematic and one-directional,
so the LOD IS applied; it just does not move the outline. A direct pixel diff across a toggle
(half-period 1 frame, camera static to 0.05 cm, both frames labelled positive) shows the change on
MainWorld's moving platform and fans and **nothing on the rock**. Good LODs cut triangles and preserve
the silhouette — which is precisely why forcing one is invisible.

🚨 **NOTHING DOWNSTREAM CATCHES IT.** The `m26` mask veto cannot: the object still draws, so it reads
`MEASURED_NONZERO` and the event survives. And it could never catch it in principle — the mask measures
the **silhouette**, and the silhouette is exactly what does not change.

⚠ **THE PART THAT TRAVELS: a high refusal rate reads as protection.** Across three auto-pool legs the
guard fired **5 times out of 7 draws**, which looks like a guard doing its job. The two that got through
were the invisible ones. **A guard's fire count says nothing about the cases it admits.**

(2026-08-21, m29 — G-P1 failed on this; no fix designed, diagnosis and fix do not share a turn.)

---

## G150 — adding a pool member re-rolls every seeded auto-pool draw

`m29` added two ids to `GAutoPool`, so `Eligible[Stream.RandHelper(Eligible.Num())]` draws from a
larger set: **the same seed now yields a different id/target sequence.** Every banked MainWorld
auto-pool run is therefore **NON-COMPARABLE across this commit**.

This is **`G140`'s shape, second instance** — there the selectable ACTOR set changed (the foliage
exclusion), here the ANOMALY set does. Both re-roll the same stream.

⇒ **Any regression leg for an existing anomaly must be TARGETED, never auto-pool.** An auto-pool
before/after comparison across a pool-membership change is measuring the draw, not the anomaly.

(2026-08-21, m29.)
---

## G149 — AMENDMENT (2026-08-21, same day): the missing variable is ON-SCREEN SIZE, not LOD quality

The entry above stands on two of its three claims and is CORRECTED on the middle one.

**STANDS.** LOD COUNT is a proxy; count 1 is certainly invisible; count >= 2 is NOT certainly visible.

**CORRECTED.** The original entry attributed the invisibility to LOD authoring quality - "good LODs
cut triangles and preserve the silhouette, which is precisely why forcing one is invisible". That is
wrong as stated. **A good LOD preserves the silhouette AT THE SIZE IT WAS AUTHORED FOR.** Close
enough, the difference is plainly visible. Measured on the same mesh, LOD 1 vs LOD 4, two legs,
identical camera, whole-frame pixels differing by >= 8/255:

    bounds coverage 33.04%  ->  66,615 px   VISIBLE
    bounds coverage  9.35%  ->  12,489 px   VISIBLE
    farther rung            ->      14 px   not visible
    farthest rung           ->       8 px   not visible

**The 0.4% best-vs-worst delta in the original entry is a reading AT THAT DISTANCE, not a property
of the mesh.** Three orders of magnitude separate the visible rungs from the invisible ones.

**STANDS.** Nothing downstream catches the admitted case: the m26 mask measures the SILHOUETTE and
reads MEASURED_NONZERO either way. The gate must be at PICK TIME.

**WHY THE ORIGINAL WAS WRONG, and it is the transferable part:** every leg behind it ran under the
shipped 18 m pawn-anchored poll radius only, with the target at ~3% of frame. The condition the
anomaly is FOR - the player being near the object - was never in the test. G135's shape: a bench leg
that structurally cannot exhibit the effect returns a clean negative, and a clean negative reads like
a finding.

**ALSO CORRECTED, a method error worth keeping:** the original entry's MainWorld evidence compared
ADJACENT FRAMES WITHIN ONE LEG at half-period 1, assuming they straddled a toggle. They did not. The
sound instrument is TWO LEGS at fixed different LODs with a matched camera, diffed at the same frame
index. Re-measured that way, the MainWorld target shows 2,133 strong pixels inside its own bbox
against an out-of-bbox control channel - i.e. it DOES change, contradicting the original reading.

(2026-08-21, m29. The bounds-coverage proxy over-reads: that same target reads 11.83% bounds coverage
while drawing 2.78% of frame, ~4x. A threshold on bounds coverage is a proxy for a proxy.)

---

## G151 — a black frame and a null result are the same number, and the mask cannot tell them apart

A synthetic calibration level authored by script rendered **100% BLACK** in the packaged build:
`mean_luma 0.0000`, **zero non-zero pixels of 921,600**. LOD 1 vs LOD 4 frames from it were
**byte-identical**, and that was read as "the anomaly produces no visible change".

**It was black-vs-black.** The owner caught it by looking at the screen.

🚨 **THE PART THAT MATTERS: A CONTRADICTION WAS SITTING IN THE DATA AND WAS READ AS NOISE.** The
custom-depth mask reported a small but SYSTEMATIC, one-directional difference between the two LODs on
every event, while the colour frames showed nothing at all. Both readings were correct:
**custom depth does not need lighting, so the mask saw the real geometry change; the colour frames
carried no light.** The discriminator was in hand and was dismissed.

**RULE: a luma check is the FIRST thing run against any new or rebuilt capture environment, before
any measurement taken in it is trusted.** `mean_luma > 0` and `nonzero% > 0` - one line, no
threshold to argue about. This is m19's "gate on PIXELS, not on a counter" in its third instance, and
the first where the misleading number was a DIFFERENCE rather than a count.

**Isolation before diagnosis:** on the SAME build, MainWorld read `mean_luma 107.95 / 99.14% nonzero`
while the synthetic level read `0.0000`. That is what established the build was healthy and only the
level was dark - a one-command check that prevented a wrong and expensive conclusion about the cook.

**Cause, for anyone authoring a bench level:** it lacked the movable point lights `make_gate_level.py`
spawns, and its directional light was not flagged as the atmosphere sun light. A script-authored level
is never opened in the editor, so nothing is ever baked - the lighting must be MOVABLE and it must
actually reach the geometry.

(2026-08-21, m29.)
---

## G152 — a guard that is skipped in the mode you test it in passes for the wrong reason

m30's brief specified a non-interference gate for the new SESSION-GLOBAL anomaly against a TARGETED
capture leg: prove that camera_clipping being active all session does not disturb any other anomaly's
labels.

**Session globals are deliberately skipped in targeted mode.** So that gate would have run with
camera_clipping never applied at all, compared clean labels against clean labels, and PASSED - while
testing nothing. G96's shape, arriving through a different door: not a blind oracle this time, but a
correct test pointed at a configuration where the condition cannot occur.

**Caught by asking what the leg would look like if the feature were completely broken.** The answer
was "identical", which is the diagnostic G119 already states: *what would I observe if the thing I
edited never reached the thing under test?* If that answer matches what you are observing, it is not
a check.

**Non-interference was instead evidenced where the anomaly is positive on EVERY frame** - the
close-pose leg - with the other anomalies' burst spans unchanged at the canonical cadence.

⚠ The general form: when a feature has a mode gate, a test written before that gate existed can
silently land on the wrong side of it. Re-read the test's PRECONDITIONS after adding any mode guard,
not just the test's assertions.

(2026-08-21, m30.)
---

## G153 — console arg quotes survive into the path: a quoted outDir becomes literal quote characters in RunDir

Relayed from the first Concorde run (m31, office box): `IAI.Capture.Start` with outDir
`"E:\Captures"` carried the QUOTE CHARACTERS into `RunDir` as literal path characters, and the run
ended with `Warning: Capture: failed to write annotation.json`. A CLIENT-FACING SHARP EDGE: quoting a
path is the natural thing to type, the run appears to proceed, and the failure surfaces only at
write time as a warning that does not name the cause.

**FILED, NOT FIXED.** The fix candidate (strip surrounding quotes when parsing the outDir token) is
to be FOLDED INTO THE MILESTONE THAT FIXES THE SVE WANTED-HANDSHAKE — the next milestone that already
requires a cook — together with the two other fold-ins riding the same rule: `IAI.Capture.Mask`'s
stale help string (m27 RULING 2 / Deliverable A3 PARTIAL, filed at G139's addendum) and G118's cooked
placeholder token. Same cook, one binary swap, per the m27/G139 precedent that a string fix never
retires a gated binary on its own.

(2026-08-21, m31 open — relayed observation, not reproduced on this box.)

---

## G154 — a drop that logs at Verbose is invisible by default, and it made two different failures identical

The snapshot-pairing drop in ProcessCompletedFrames ("completed frame id=N has no pending
snapshot") logged at VERBOSE — suppressed at default verbosity. Consequence, realised during m31:
a BROKEN PAIRING (frames completing under ids the snapshot map does not hold) produces exactly the
same observable as a path that never submitted at all — zero PNGs, no explanation — so the one
diagnostic that separates "nothing arrived" from "things arrived and were thrown away" was mute
precisely when it was needed.

**The rule: any code path that DISCARDS work product must announce it at a verbosity that is ON by
default.** A drop is not debug detail — it is the system reporting that it destroyed something. The
m31 fix promotes that drop to a Warning with its own unique grep token, and the run-end handshake
summary names which stage lost frames so a zero localises itself.

(2026-08-21, m31.)

---

## G153 — ADDENDUM (2026-08-21, same day): FIXED in m31

The quoted-outDir sharp edge is closed by the m31 build: IAI.Capture.Start now strips one wrapping
pair of quotes from the outDir argument, REFUSES loudly at StartRun (log Error, run never starts,
auto-injector resumed) if a quote character survives inside the path, and REFUSES loudly if the run
directory cannot be created — the failure surfaces at start time with its own token, never at
annotation-write time. The entry above stands as the record of the sharp edge and its cost.

## G155 — a detector calibrated on a synthetic patch is calibrated on the easy case, and the real one can fail in the opposite direction

Two pixel detectors written for the label-offset instrument PASSED a synthetic gate and FAILED on
real frames, each for a different reason, and both were only caught because real pixels were looked
at afterwards.

**MAGENTA.** An absolute-brightness test (`min(R,B) > 128`) sailed through a saturated synthetic
patch. On real frames the corruption material is LIT, so its pixels sit well below any absolute
floor: the same test read 0.203 and 0.046 of the region on frames that are unmistakably magenta to
the eye. The shipped test is RELATIVE — `G < 0.60·min(R,B)` and `|R−B| < 0.40·max(R,B)` — and reads
those frames at 0.373 / 0.198 while still reading 0.000 on checkered ones.

**CHECKER.** The absolute route (achromatic + bimodal luminance + edge energy) failed for a more
instructive reason: **a bbox holding a DARK OBJECT ON A BRIGHT BACKGROUND is strongly bimodal
whether or not a checker is present.** The anomalous and the clean frame were nearly
indistinguishable on every statistic the detector used — separation 0.739 vs 0.720, concentration
0.914 vs 0.943. The only quantity that moved at all was edge energy, 0.0081 → 0.0108. A DIFFERENTIAL
route against the same region's edge energy in the baseline frame is what catches it.

⚠ **THE RULE: a synthetic fixture proves the detector fires on the ideal case. It cannot show what
the detector CONFUSES.** Where the quantity being measured is a property of a REGION rather than of
an object, the background is inside the measurement, and a differential test against a known-clean
frame of the same region is worth more than any absolute threshold. Validate on real pixels you have
actually looked at, and keep the synthetic as a regression guard, not as the calibration.

## G156 — delivery mode removes the bbox, and that costs CORRECTNESS, not merely confidence

Delivery mode does not write `labels.jsonl`, and `labels.jsonl` is the ONLY carrier of the per-frame
bbox — `annotation.json` has no bbox field at all. Any tool that localises a measurement to the
anomaly's region therefore degrades to WHOLE FRAME, and loses the ambient ring that rejects camera
motion at the same time.

Measured on delivery-mode fixtures built from sessions whose true offset was KNOWN:

| fixture | true offset | delivery-mode reading |
|---|---|---|
| blink | +0 | median +0 but per-event range +0..+7 |
| missing_texture | +0 | **median startΔ +6 — WRONG**, half the events unmeasurable |
| missing_texture | +1 | **all eight events UNMEASURABLE** |
| missing_object | +1 | median +1, only 2 of 8 events survive |

In-region contrast collapses about fifteenfold (peak/T 33.9 → 2.3).

⚠ **The failure is not a uniform loss of confidence — a texture event reports a CONFIDENT WRONG
NUMBER.** An offset of 1–6 frames is NOT reliably detectable in delivery mode. **THE CLIENT'S SHIPPED
CONFIG CANNOT SELF-VERIFY; ANY DIAGNOSTIC CAPTURE MUST SET `IAI.Capture.Delivery 0`.** This is the
same shape as `L3` — delivery mode changes what evidence exists, not just what is convenient.

## G157 — a project setting can make a defect STRUCTURALLY unreproducible, and the clean logs look like health

Both shipped materials were missing `bUsedWithStaticLighting`. On Concorde this drew the ENGINE
DEFAULT MATERIAL on statically-lit static meshes while the skeletal weapon drew our magenta — one
anomaly type, two appearances. On this box the defect had been present the whole time and produced
NOTHING: not a warning, not a wrong pixel.

The mechanism is a single project line: **`r.AllowStaticLighting=False` in StackOBot's
`DefaultEngine.ini`.** The engine gate (`StaticMeshRender.cpp:2225`) only consults
`MATUSAGE_StaticLighting` when the section has `bHasSurfaceStaticLighting`, which needs a
lightmap/shadowmap. With static lighting disallowed no primitive here has one, so the usage is never
queried, the flag's absence costs nothing, and no warning is emitted.

Evidence that the silence was real and not filtered: **eleven packaged home logs contain ZERO
`LogMaterial` lines of any verbosity** while `LogRHI`, `LogAnomalyCapture`, `LogConfig` and a dozen
others log freely, and no suppression is configured.

⚠ **THE RULE: before concluding "our build is fine", ask which HOST SETTING decides whether the code
path under test runs at all.** A green home run is evidence about this project's configuration, not
about the plugin. And ⚠ **the log only warns for usages actually EXERCISED at runtime — the absence
of a warning for some other flag means that path was not hit, NOT that the flag is set.** Do not
narrow a fix to the one flag you happened to see warned.

## G158 — frame byte-identity is not available between two runs of the same binary, so a single-frame difference proves nothing on its own

Re-measured while validating the tick-mode pin: **90 of 90 frames differ between two runs of the
SAME binary** on the settled CB_GateLevel bench. This is A47's bifurcation and its relatives, and it
means a byte-diff of frames carries zero information about a code change in either direction.

It bites at the margin. A post-change leg showed two of seven events at agreement 0.9375 instead of
1.000 — ONE extra boundary frame crossing the manifestation threshold — which read like a regression.
A SECOND post-change leg read 1.000 on all seven, identical to both pre-change legs.

⚠ **THE RULE: when a difference is one frame wide, run the leg again before attributing it.** The
sound instrument for a cross-binary comparison is the CONTROL PAIR plus the INVARIANT CORE
(`subset_gate.py`), never frame bytes. And note the corollary for that gate: **when a change
deliberately ADDS artifact fields, the subset gate EXITS NONZERO by construction** — it flags any
field the control pair does not exhibit. Report that as it printed and show the extras are exactly
the declared additions; do not relabel it a pass.

## G159 — a string scan of a cooked container is blind to serialized property flags, and only a known-TRUE control reveals it

Attempting to confirm that `bUsedWithStaticLighting` reached the COOKED container, the obvious
instrument is to scan `StackOBot-Windows.ucas` for the property name. **Run the control first, and
here the control killed the instrument:**

- `M_CorruptedTexture_Pink` and `M_MissingTexture_Checker` — PRESENT in the pre-fix container.
- `bUsedWithStaticLighting`, `bUsedWithClothing` **and `bUsedWithSkeletalMesh`** — ALL ABSENT.

`bUsedWithSkeletalMesh` was KNOWN TRUE before the fix, because m29's `G-4S` proved skeletal pink
renders out of the cooked artifact. So an "absent" reading is BLINDNESS, not a negative. Cooked
packages do not carry these flags as searchable strings.

⚠ **The instrument was rejected rather than its result reported.** What remains is corroboration and
was labelled as such: the `.ucas` grew 364,557,872 → 364,594,736 bytes while the `.pak` stayed
BYTE-IDENTICAL and `global.*` unchanged, consistent with extra vertex-factory permutations from two
new usage flags on two materials. **A size delta is not a flag read-back.** This is `G96`'s principle
applied to a container scan: a search that cannot find a value you KNOW is there tells you about the
search.

## G160 — `preFrames` is a one-time lead-in; `postFrames` governs the gap between annotated windows

`IAI.Capture.Config <settleK> <preFrames> <positiveFrames> <postFrames> <burstCount>` governs the
burst schedule on BOTH the targeted and the auto-pool path — the capture FSM is targeting-agnostic
and `bTargetedMode` only selects which fire route `BeginFire()` takes. But the parameter that widens
the CLEAN GAP between annotated windows is **`postFrames`, not `preFrames`**, because `LeadIn` runs
ONCE PER RUN rather than once per burst.

Measured, after the wrong knob was recommended first:

| config | resulting windows | min clean gap |
|---|---|---|
| `2 14 8 4 0` | 13-20, 27-31, 39-43, 49-56, 61-68, 85-92, 97-104, 109-116 | **4 — unchanged** |
| `2 4 8 14 0` | 3-10, 26-32, 48-54, 69-76, 91-98 | **14** |

Raising `preFrames` from 4 to 14 left the burst period at 12 and the gap at 4.

⚠ **Why this matters beyond the knob name: the clean gap is the CEILING on any offset measurement
that derives its baseline from the annotation.** On the standard `2 4 8 4 0` config that ceiling is
about ±2 frames. `tools/measure_label_offset.py` prints the achieved gap and `--require-gap N` exits
nonzero below a bar — **the correction above was found because the tool PRINTED the number, not
because anyone re-derived it.** Let the instrument enforce the precondition rather than trusting
arithmetic about the schedule.

---

## G161 — a join between two artifacts needs the key they SHARE, not the one that looks like it

`annotation.json`'s `affected_frames.frame_indices` and `labels.jsonl`'s `frame_index` are both
"frame numbers" and they are NOT the same space. `labels.jsonl` carries BOTH:

* **`session_index`** — the 0-based capture index. `session_index` N is
  `Actual_Frames/frame_000NN.png`, frame N of the video, **and** frame N of `annotation.json`'s
  frame indices. This is the join key.
* **`frame_index`** — the ARM-TIME `GFrameCounter`. It starts elsewhere and counts elsewhere.

Diagnosing the overlay's "phantom boxes" (2026-08-22) the first pass joined on `frame_index` and
produced **14,399 bogus "outside window" hits plus 15 annotation frames with no label row at all** —
a result that looked like a real product defect. Re-keyed on `session_index`, annotation is a
**strict subset** of the label rows with **zero orphans**, and the true distribution is
90.1 % by-design / 9.9 % vetoed.

🚨 **The tell was the impossible direction:** annotation claiming frames `labels.jsonl` did not
have at all. A label file that is missing rows the annotation cites is a much bigger claim than the
one being investigated — **when a diagnostic produces a finding LARGER than the bug you went looking
for, suspect the diagnostic first.** `G142`'s shape: the CHECKER was wrong, not the build.

---

## G162 — `labels.jsonl` ROW ORDER IS NOT DETERMINISTIC, and a positional comparison will call that a regression

The async writer appends rows in COMPLETION order, not `session_index` order. Neighbouring frames
routinely swap. Measured on two runs of the SAME binary: **4 positional mismatches**; across a
binary change: **8**. In both cases **0 field differences once sorted by `session_index`** — the
content is identical, only the order moves.

`CaptureBench/tools/subset_gate.py` compares `labels.jsonl` LINE BY LINE, so it reads that jitter as
differing fields. On the m32 change set it produced **15 `labels.jsonl` extras** and exited 1 while
the only real artifact change was one declared `run_summary` field. **The control pair exhibits the
same phenomenon — it had simply under-sampled it.**

⛔ **Filed, deliberately NOT fixed** (verification tooling, not a build defect). Rules:
* Compare `labels.jsonl` **keyed or sorted by `session_index`**, never positionally.
* ⚠ **And do not check it with a SET comparison either.** PowerShell `Compare-Object` treats its
  inputs as sets and reported "0 differing lines" on files the gate called different — it was blind
  to order in the exact place order was the whole question. Two instruments disagreeing meant one of
  them was wrong about what it measured, not that the data was ambiguous.
* This is now CLIENT-VISIBLE: `labels.jsonl` ships in delivery mode from m32, so `client-readme.md`
  states the ordering rule for data consumers.

---

## G163 — a cross-repo build input must never be derived from the repo you happen to be standing in

`make_delivery.py` resolved its `PLUGINFILE` entries from
`<dashboard repo>/../StackOBot/Plugins/AnomalyInjector`. That path is correct **on the development
box and nowhere else.** The machine that actually packages client bundles has the dashboard at
`D:\AnomDashboardV1\AnomDash` and **no plugin tree at all**, so packaging refused to build the night
before a delivery.

**The refusal was CORRECT** — an allowlist entry that cannot be resolved must not silently vanish
from a bundle. **The defect was the derived default**, which encoded "these two repos are
siblings" as if it were a fact about the world rather than a fact about one workstation.

Rule: **a dependency on a SECOND repository is opt-in and explicit, never inferred from the first
one's location.**
* Not given ⇒ do the work you can, **exit 0**, and say loudly and specifically what is missing,
  where it belongs and where to get it. The success line itself must carry the caveat
  (`9/11 ... dashboard-only; 2 plugin-side file(s) NOT included`) so nobody reads "success" and
  infers completeness they do not have.
* Given ⇒ resolve it, and **fail loudly** if it cannot be satisfied. An explicit request that
  cannot be met is still an error.
* ⛔ Never invent a placeholder, never create the directory, never keep a second copy in the first
  repo "to be safe" — a second copy is a second thing to drift.

Same family as `G88` (a loose ini beside a package is a no-op) and `G119` (read it back out of the
artifact): **an assumption about the filesystem layout is an assumption about a machine you are not
standing on.**

## G164 — a killed build leaves a truncated exe that the next build calls "up to date" at exit 0

(2026-08-22, session 055.) A killed build task left `StackOBot.exe` TRUNCATED at ~2 MiB against the
real ~240 MB — and the next `Build.bat` reported "up to date" at **EXIT 0 in 2.8 s**. UBT trusts the
output's TIMESTAMP; it never checks that the artifact is whole. This is `G119`'s shape applied to
the build system itself: **an exit code plus "up to date" is not evidence the artifact exists in
full.**

Rule: **after any killed or interrupted build task, verify the output binary's SIZE (or hash)
against a known-good build before trusting it.** Recovery is cheap — delete the exe+pdb and rebuild
(objects survive; ~90 s relink). The dangerous path is the silent one: a 2 MiB stub with a fresh
timestamp satisfies every downstream step that keys on "the build succeeded".

---

# Session 061 (2026-08-26) — the m35 readback sub-rect lessons

⚠ **HONEST NOTE ON THE COUNT.** The close-out brief asked for "all nineteen lessons". The canonical
numbering is maintained chat-side; what follows is **every lesson this session established with a
receipt on this box**, written out in full. Where a lesson reinforces an existing gotcha it is
recorded as an instance of that gotcha rather than minted as a new number, and where the brief names
a lesson I cannot reconstruct without guessing it is recorded as a **named gap**, not filled in
(`G120`: never write an unverified mechanism as fact). Expect the numbering here to differ from
chat's; the substance is what travels.

## G165 — a crash inside a new code path is an implementation error, not a verdict on the design

m35's first Build B leg died at capture start with `State != D3D12_RESOURCE_STATE_COMMON`. The
temptation is to read that as "the design does not work". It was one missing creation flag: the
**D3D12 transient resource allocator derives a resource's initial state solely from RenderTarget,
DepthStencil and UAV flags**, so an RDG texture created with `TexCreate_ShaderResource` alone has no
derivable initial state. Adding `TexCreate_RenderTargetable` fixed it outright.

Two things travel. First, **classify a crash before reacting to it** — an allocator-contract error and
a design refutation demand opposite responses, and only one of them means stop. Second, **the fix
created a deliberate asymmetry that must not be tidied away**: the flag is added to the RDG (SVE)
texture only, because the backbuffer path's texture is a *persistent* `FTextureRHIRef` that declares
`SetInitialState(ERHIAccess::CopyDest)` directly and needs no render-target flag. That asymmetry is
now named in the backbuffer path's own log line, precisely so a future reader does not "harmonise" the
two and reintroduce the crash on the path that was never broken.

## G166 — a guard leg that writes ZERO frames is the PASS, and zero frames without drops is the FAIL

`G-M3` proved the new readback bounds guard by breaking it on purpose
(`IAI.Bench.ReadbackGuardInflate 1`). The result: **90 `READBACK-GUARD FIRED` lines, 0 frames on
disk, `total_frames 0`, no crash, and the run completed and wrote its artifacts.** An empty output
folder is exactly what a *successful* guard test looks like.

The trap is that an empty output folder is also what a *broken build* looks like. **The discriminator
is the drop count, never the emptiness** — 0 frames *with* 90 drops is a pass; 0 frames *without*
drops is a failure. The gate text was written that way before the leg ran, which is the only reason
the reading was unambiguous. Knob back to 0 restored silence, so the guard is proven **both ways**
(`G96`); a guard that has only ever been silent is not a guard.

## G167 — pre-declare the number that will look like a regression, or it will be read as one

Under m35 the plugin owns the readback texture, so `bufferHeight` stops reporting the engine's
staging height (869) and reports the picture height (344). **That drop is the fix working.** Read cold,
it looks exactly like "this became a 5.2+ engine", which is the misreading that would have sent
someone chasing an engine-version hypothesis through a working build.

The gate file carried the sentence *"`bufferHeight` WILL NOW READ 344, NOT 869, AND THAT IS THE FIX
WORKING"* **before any Build B leg ran**. Generalisation: when a change will move an observable in a
direction that resembles a defect, **write down the expected new value and its meaning in advance**.
A prediction made before the measurement costs one line; the same claim made afterwards is
indistinguishable from rationalising a result.

## G168 — `G158`'s THIRD INSTANCE, now with the noise floor quantified

`G158` already says frame byte-identity is unavailable between two runs of the same binary. This
session measured how unavailable, which is what makes it actionable:

- **MainWorld:** mean |Δ| **4.42**, max **225**, **78 %** of pixels differ. Unusable for any frame
  comparison.
- **CB_GateLevel, same binary** (`M33_CTRL_A` vs `M33_CTRL_B`): mean |Δ| **0.00117**, max **3**,
  **0.116 %** of pixels — concentrated in the **lower half**, **top four grid rows exactly 0.000**,
  and **no corner box**, so it is **not** `G125`'s CaptureBench frame marker.

Consequence adopted: the m35 pixel gates are reported **NOT OBTAINED, never FAILED**, and the
replacement instrument compares **two readbacks of the same frame inside one run** (`G-M9`) instead of
two runs. **A pose-matched pair is necessary and not sufficient for byte identity** — and the
control pair is what turns "the frames differ" from a finding into a noise reading, which is why it
must never be deleted.

## G169 — the m11 pacer makes a capture hook's cost unmeasurable, so perf must be asked with pacing OFF

`G-M6` wanted the cost of m35's added per-frame copy. There is **no hook-cost field in any artifact**,
so the available proxy is the median consecutive `t_wall` delta from `labels.jsonl` — and it read
**0.03334 on all five legs with `paced=True`**. That is `1/30` to four decimals. The **m11 pacer pins
each tick to `1/VideoFps`**, so it absorbs any sub-budget hook cost completely.

⇒ **A paced leg cannot measure a sub-budget hook, and no rebuild changes that.** Any perf question
about a capture-path hook must be asked with **pacing OFF**, or it is being asked of the pacer instead
of the code. Corollary that saved a rebuild here: the prior is obtainable by **swapping an archived
predecessor exe** against the current one, order-matched, hashes re-verified at each swap — which is
why archived exes are load-bearing artefacts and not housekeeping.

🆕 **SECOND INSTANCE, SAME DISEASE IN A DIFFERENT COSTUME (2026-08-26, owner-ruled).** Turning pacing
OFF removes the pacer, but it does not make the instrument infinitely sensitive — and the failure mode
on the other side is identical in shape. **`G-M6` must report the WITHIN-BUILD SPREAD ACROSS POSITIONS
beside the BETWEEN-BUILD DIFFERENCE, and if the A/B difference is not larger than that spread, the
honest statement is "BELOW THE RESOLUTION OF THIS INSTRUMENT" — never "no cost".**

The two mistakes are the same mistake: above, a number produced by the pacer was nearly read as a
number about the code; here, a difference smaller than the instrument's own noise would be read as an
absence. **A null is a statement about the instrument until it is shown to be a statement about the
world.** ⇒ never report an unresolved difference as zero, and never let a perf null harden into "free"
without the spread printed next to it.

## G170 — a lever that silently no-ops produces a clean null indistinguishable from a clean result

This is `G114`'s principle, hit again on a new lever. The packaged letterbox leg returned
**`LETTERBOX REFUSED - view target 'SpectatorPawn_2147482483' has no UCameraComponent`**, and that
refusal is the only reason the leg was not read as a pass: its `READBACK-LAYOUT` reported
`rect=(0,0)-(1280,720)` and its bbox was **identical to the un-letterboxed leg**. Had the lever
shrugged and done nothing, the artifacts would have been a perfectly clean un-letterboxed run
presented as evidence about letterboxing.

**Rule: a lever must refuse loudly and name the reason, never no-op.** The leg is banked **VOID by the
pre-declared rule**, not as a pass. Environmental fact worth carrying: **the packaged bench pawn under
`-unattended` is a `SpectatorPawn` and has no `UCameraComponent`**, so any lever that acts on a camera
component is structurally unavailable in the unattended harness and its gate must run in PIE.

## G171 — a row-only checker is blind to a horizontal defect

The letterbox lever moves `Rect.Min.Y`, so every checker built for it examines **rows**. `Rect.Min.X`
has been **zero in every leg ever run**, which means the X half of the sub-rect origin has never been
tested and a row checker could not detect a failure there if it were.

The fix is not a better row checker; it is **a column checker, and a pillarboxed leg to point it at**.
Written down because the coverage gap was invisible from inside the passing gates — every gate was
green and half the parameter space was untouched.

## G172 — a checker can print "MATCHED" while comparing nothing, and that is how a vacuous gate ships

My pose checker read `annotation["camera"]`. That key does not exist at the top level — camera data
lives under `anomalies[]`. So it compared `None` against `None`, found them equal, and printed
**"MATCHED"** on every leg. The verdict was not wrong about the data; there was no data.

Corrected to read `view.rot` / `view.origin` from `labels.jsonl`. This is the receipt behind the
standing owner rule that now governs everything here: **EVERY CHECKER IS PROVEN AGAINST A KNOWN ANSWER
BEFORE ITS VERDICT IS READ.** `G146`'s vacuity problem, arriving through a missing key rather than an
empty result set — and note that the vacuous output was *the answer I expected*, which is what stops
it being noticed.

## G173 — when a probe reads empty, suspect the probe's key before the build

A probe of mine reported `max_frames` empty and I nearly read that as a config not being applied. The
real key is **`frame_cap`** — `max_frames` does not exist. **The checker was wrong, not the build.**

Same family as `G161` (join on the key the artifacts share) and `G142` (a verification script is a
defect surface of its own). The cost asymmetry is what makes it worth a number: a false failure here
teaches the owner to distrust a gate that was working, which is more expensive than the missing
measurement.

## G174 — a bare class-name target matches nothing on UAID actors, and the leg reads as a clean empty run

Firing at `SM_Ramp2` produced `zero_match_bursts = 8` and `positive_frames = 0`. Nothing errored. The
run completed, wrote its artifacts, and looked like a session in which simply nothing happened. It is
**INVALID — not a pass and not a failure** — because it sits below the ≥3-counted-events validity
floor.

**Why only some targets need the long form:** `CB_GateLevel`'s actors were script-spawned with only
`set_actor_label()`, so their `GetName()` genuinely is `StaticMeshActor_<n>` and a bare name matches.
MainWorld's editor-placed and Blueprint actors carry a runtime `_UAID_…` suffix, so a bare class name
matches nothing there. The replayable tokens are:

```
SM_Ramp2_UAID_B42E9936F5429ADA00_2086822137
BP_SplineSpawn_C_UAID_A85E45CFE40412DE00_1511100424
```

🚨 **They existed in replayable form ONLY inside banked `run.json` files** — a discovery problem for
every future session, which is why they are now in the journal, the gate file §12 and the runbook.
**Grep the bank for a target token before designing a leg around it** (`G116`'s habit, applied to
targets).

## G175 — attribute code to commits per file, never per branch

I reported that m34 had touched four files, including the one m35's fix rewrites around. Per-file
`git log` said otherwise: **`AnomalySceneViewExtension.cpp` was touched by `b05066f` — the
stale-present display fix — and m34 (`0fc00ef`) never touched it at all.**

That matters because `feature/mask-gpu-reduce` is **not "the m34 branch"**; it is a four-item staging
line (m34 · `b05066f` · `5495aa6` · m35). Reading a shared branch as one milestone's work misattributes
every interaction on it, and here it would have pointed a gate at the wrong milestone. **On a shared
branch, `git log --follow <file>` is the only sound attribution.**

## G176 — "per armed frame" and "per captured frame" are different rates in this codebase

The added sub-rect copy was described for several turns as a **per-armed-frame** cost. It is
**per CAPTURED frame**. `CaptureCurrentFrame()` (`AnomalyCaptureSubsystem.cpp:1736`, called at
`:569`/`:579`/`:589`) mints one RequestId (`:1752`), arms one readback (`:1769` SVE / `:1773`
backbuffer) and increments `SessionFrameIndex` (`:1777`) — which **names the output PNG** at `:1790`
and is what `FrameCap` is tested against at `:551`. ⇒ **one arm == one captured frame == one PNG,
1:1**, i.e. 30× per second at 30 fps.

The phrase was true of a *different* subsystem: **the MASK arms a few times per burst.** This
session's own legs separate the rates — **90** `READBACK-GUARD FIRED` for a 90-frame cap against
**29** `M23 PASS` mask arms on the same family of leg. **When two subsystems in one file arm at
different rates, name the subsystem in the sentence**, or the cost estimate silently changes by 3×.

## G177 — an engine check that exists can still be absent, because it is compiled out

Asked whether the backbuffer copy would catch a format mismatch, the answer looked like yes:
`FValidationRHIUtils::ValidateCopyTexture` (`RHIValidationUtils.h:10-45`) carries
`checkf(bValidCopyFormats, ...)` plus source and destination bounds checks. **But the whole file sits
behind `#if ENABLE_RHI_VALIDATION` (line 5), which is OFF in a default Development build**, and
D3D12's own `RHICopyTexture` (`D3D12Texture.cpp:2868+`) checks only block alignment — **no format
check at all.**

⇒ on the shipped path a format mismatch is **undefined behaviour**: no assert, no loud drop, no
graceful failure. This inverts the conclusion: the structural guarantee (read the format from
`BackBuffer->GetFormat()` every frame; recreate the owned texture *before* the copy in the same
straight-line block; hand the drain `Item.Format = SrcFormat`) is not belt-and-braces — **it is the
only protection there is.** `G119`'s rule pointed at the engine: **finding the check in source is not
evidence the check runs.** Check its compile gate.

## G178 — verify an instrument's premise before you build the instrument, not after it disagrees

`G-M9` compares two readbacks of the same frame, which is only sound if both passes observe identical
source contents. That rests on RDG executing passes in handle order without reordering. **That was
verified from source before a line of the comparator was written** — RDG has no sort or reorder, and
both passes are added consecutively and are read-only with respect to the source.

The payoff is in the failure mode: a non-zero diff now has **two** possible causes — the owned copy is
wrong, or the added pass broke the adjacency the premise rests on — and **a broken premise makes the
comparison meaningless rather than failing**. So the comparator's output line is required to name both
causes and instruct the reader to **check adjacency first**. An instrument that cannot distinguish "I
found a defect" from "I am no longer measuring" sends people hunting defects in working code.
**The comparator is its own guard.**

## G179 — the padding in a row pitch can be ZERO, which is what kills a pitch-based layout sniff

The rejected design would have sniffed the engine's staging layout by comparing `bufferHeight` and
`rowPitchInPixels` against the picture's dimensions. Measured here: `rowPitch 832` against
`width 821` = **11 px of padding** (832 × 4 = 3328 = 13 × 256, D3D12 256-byte row alignment).

🚨 **At any width whose byte stride is already 256-aligned, the padding is ZERO and `rowPitch ==
width`.** So at a narrow pillarbox, and at every aligned width, the two candidate engine layouts are
**numerically indistinguishable** and the sniff **fails silently inside its own blind spot** — the
dangerous direction, because it returns a confident answer. ⚠ A literal `rowPitch == 0` has never been
observed here; the finding is about the *padding*, and the distinction is recorded so nobody
"corrects" it later.

Adopted consequence: **do not detect the environment; make the environment irrelevant.** The shipped
design copies the sub-rect into a plugin-owned texture at (0,0) and reads back the whole texture with
no rect, so `bufferHeight == picture height` on every engine and there is nothing left to sniff.

## G180 — compiling against three engine versions is not evidence their semantics agree

The readback staging layout **changed at UE 5.2** (5.1 allocates full-source-size staging and copies
the sub-rect to its own position; 5.2+ allocates rect-sized staging with dest 0,0). **UE 5.3 keeps a
`FResolveRect` compatibility overload, so the call sites compile unchanged on 5.1, 5.2 and 5.3.**

That is the whole trap: the code builds everywhere and is correct in only one place. A field report of
"it works on our 5.3 host with the offset removed" and a field report of "it crashes on our 5.1 host"
were **both true simultaneously**, and neither was a build problem. **When a host reports
version-dependent behaviour, check whether the API kept a compat shim** — a silent semantic change
behind a stable signature is invisible to every compiler in the chain.

## G181 — hashing a redirected file may hash the redirection, not the content

The insurance diff of the uncommitted fix was written twice from the same working tree and produced
**18,374 B / `sha256 8479FFE7…`** and then **18,756 B / `sha256 7A0CC269…`** — different sizes,
different hashes, **identical content** (both are the same 8 files at 192 insertions / 16 deletions).
The delta comes from how the shell wrote the file, not from what git produced.

⇒ **A hash of a shell-redirected artifact certifies the byte stream, which includes the shell's
encoding decisions.** Verify *content* identity with a content instrument (here: the diffstat and the
file list), and treat a moved hash on an unchanged input as an encoding question until proven
otherwise. Same family as `G141` (PowerShell `-Encoding utf8` writes a BOM) and `G115` (a shell
round-trip re-encodes a whole file while the text still reads correctly).

🚨 **UPDATE 2026-08-26 — THE ENCODING QUESTION WAS THE WRONG QUESTION, AND THE RIGHT ONE HAD A WORSE
ANSWER: NEITHER COPY EVER APPLIED.** "The diffstat matches, so the code is the same" was true and
**irrelevant**. The question insurance has to answer is *does it apply*, and it was never asked.
Measured, `git apply --check` against the pre-fix tree:

| artifact | bytes | form | `git apply --check` |
|---|---|---|---|
| the surviving copy | 18,756 | **BOM + CRLF** + trailing NL | ❌ `patch does not apply`, **all 8 files** |
| same, BOM stripped | 18,753 | CRLF | ❌ all 8 files |
| same, BOM kept, CRLF→LF | 18,378 | BOM + LF | ✅ applies |
| same, BOM + CRLF stripped | 18,375 | LF | ✅ applies |
| **the earlier copy**, reproduced exactly | **18,374** | LF, **no trailing newline** | ❌ **`corrupt patch at line 378`** |
| `git diff --output=` | 18,375 | LF + trailing NL | ✅ applies |

⇒ **CRLF is the sole killer of an otherwise valid patch** — the BOM is inert to `git apply`, which was
the opposite of the expected culprit. And **the earlier 18,374-byte copy was broken too**, by a
*missing final newline*: a patch whose last line is unterminated is `corrupt`, not merely untidy.

**The 382-byte delta is now fully accounted for by counting, not reasoning:** 3 (BOM) + 378 (one CR
per line, and the file has exactly 378 lines) + 1 (trailing newline) = **382**. Confirmed by
reconstruction — truncating git's canonical output by its final byte reproduces the earlier artifact's
recorded `sha256 8479FFE7…` exactly.

**RULE: let git write the patch — `git diff --output=<file>`** — never a shell redirect, `>`,
`Out-File` or `Set-Content`. Git emits LF and no BOM regardless of `core.autocrlf`.

🚨 **THE REUSABLE HALF, AND IT IS BIGGER THAN PATCH FILES: A BACKUP IS VERIFIED BY RESTORING IT, NEVER
BY INSPECTING IT.** The recorded check here was *"the diffstat matches at 192/16 over the same 8
files"* — a true statement, carefully measured, and **evidence for the wrong proposition.** It
established that the patch **describes** the right change. Insurance has to **apply**, and nobody
asked. A hash proves a file has not changed since you wrote it; a diffstat proves it describes what
you meant; **neither says the artifact can ever be used**, and only restoring it does.

This is `G-M3`'s guard rule pointed at backups: *a guard that has only ever been silent is not a
guard* ⇒ **a backup that has never been restored is not a backup.** Same family as `G96`. ⇒ **at the
moment a backup artifact is created, restore it — `git apply --check` against a clean tree, from its
final location — and record the exit code beside the hash.** For the whole life of this fix, the
recorded "insurance" was decorative, and every check that had been run on it passed.

⚠ **AND A SECOND-ORDER CATCH FROM THE SAME SESSION, because it nearly buried the finding:** the
verification script printed the label **`(=> the ORIGINAL 18374 B artifact WAS valid insurance)`**
next to the command that tested it — a label written **before** the measurement returned. It returned
`corrupt patch at line 378`. **A label written before its measurement is a VERDICT, not a
description**, and a reader skimming the output would have taken the sentence over the exit code.
`G172`'s vacuity problem in the output layer rather than the logic layer: write the label from the
result, or write no label at all.

## G182 — PowerShell 5.1: a here-string handed to `git commit -F -` becomes a pathspec

`git commit -F - @'…'@` does **not** pipe. PowerShell passes the here-string as an *argument*, git
takes it as a pathspec, and the commit fails with `pathspec '<your entire commit message>' did not
match any file(s) known to git` — measured this session, exit 1, no commit made.

**Write the message to a file and use `git commit -F <file>`.** Same family as the already-recorded
trap where a `git commit -m` message containing embedded double quotes silently becomes pathspecs. Two
instances now: **in PowerShell 5.1, never construct a git commit message inline.**

## G183 — the Bash tool's git can hang in this workspace; PowerShell is the working form

`git` invoked through the Bash tool hung to the 2-minute timeout and then reported
`/c/Users/.../claude-…-cwd: No such file or directory` — the tool's working-directory file had
vanished, and `D:\IntrusiveAnomalies\StackOBot` (the shell's cwd) is not itself a git repository.
Plain `pwd` worked; a `git`-leading compound command did not, and an attempted heredoc append wrote
nothing at all (the target file was verified untouched afterwards).

**Use `git -C <plugin-path> --no-pager …` from PowerShell in this workspace, and verify that any
shell-driven file write actually landed before assuming it did.** The second half is the real lesson:
the failed heredoc reported an error *after* printing earlier output, which reads like partial
success. It wasn't — but only checking the file established that.

## G184 — a leg payload transcribed by hand into a doc is a defect surface with no known-answer control on it

The session-061 journal recorded an `ExecCmds` payload for re-running the `StaticMeshActor_73`
known-answer leg, and named `M34_R3_CYL73` as the datum it would be graded against. Reading that
banked leg's own `_leg_geometry.json` and `run.json` instead of the prose, the two disagree on **four
axes**: anomaly `missing_texture` vs **`blinking`**, seed `4242` vs **`777`**, `=StaticMeshActor_73`
vs the **bare** form, and — the one that matters — **`IAI.Capture.MaskReduce both` omitted entirely**.

🚨 **THAT LAST OMISSION MAKES THE LEG UNGRADEABLE RATHER THAN WRONG, WHICH IS WORSE.**
`IAI.Capture.Mask 1` measures the mask; only `MaskReduce both` runs the CPU and GPU reductions side by
side and emits the `MASK-REDUCE COMPARE` line the gate reads. Without it the leg **does not fail** —
it completes, writes its artifacts, and simply has **no verdict available on it**. That is `G174`'s
shape arriving through a missing *command* instead of a missing *target*, and `G142`'s point — a
verification script is a defect surface of its own — applied to **launch lines**.

**RULE: a re-run leg's payload is DERIVED FROM THE BANKED LEG'S OWN RECORDED CONFIG, never transcribed
by hand into a doc.** A doc payload has no control on it; the banked record is the known answer and
carries its own conditions. ⇒ **Before running ANY leg graded against a banked datum, diff the
intended payload against that datum's recorded config on EVERY axis and report the diff — including
when it is empty.** An empty diff reported is evidence; an unreported diff is an assumption.

⚠ **AND THE BANKED RECORD ITSELF HAS A BLIND SPOT, so "derive it" is not automatically complete.** The
two files are complementary and **both** are needed: `_leg_geometry.json` (19 fields) holds
anomaly / target / map / geometry / `extra_execcmds`, `run.json` holds `seed` / `frame_cap` / `paced` /
`start_frame` — and `run.json`'s own `target_anomaly` and `target_actor` are **empty**. **`CaptureBench.Marker`
is in NEITHER**, so it is an un-diffable axis: harmless for a log-line verdict, live for anything
graded by pixels, where `G125` says the marker changes every frame by construction.

## G185 — "protect the work first" must never put UNPUSHABLE work beneath PUBLISHABLE work

Session 061 closed by protecting an uncommitted 8-file fix as a `WIP` commit that must not be pushed,
and then writing the close-out docs **on top of it**. Both instincts were right; the **order** was
wrong. The result: the docs — the cold-start contract a fresh session depends on — could not be
published without also publishing a commit explicitly marked *do not push*, and the WIP was no longer
the tip, so amending it into the real fix commit would have needed a rebase.

**RULE: when a `WIP` commit that must not be pushed coexists with docs, the WIP GOES ON TOP.** Docs
then publish freely, and the WIP stays a trivially amendable tip needing no force-push. The paths were
disjoint (8 source files vs `docs/` + `CLAUDE.md`), so the correction was conflict-free — but that was
luck of the layout, not of the sequencing.

🚨 **THE SECOND LESSON IS THE ONE THAT NEARLY GOT MISSED.** Re-parenting the WIP **changes its SHA by
construction** — and the docs commit being published named that SHA in **five places**, including a
parent-chain sentence that was wrong post-reorder regardless of hash. There is no arrangement in which
docs published *beneath* an amendable commit can name it correctly: its hash depends on them.
⇒ **identify an amendable commit by SUBJECT AND POSITION — `git log --oneline -1`, "the tip", "ahead
exactly 1" — never by SHA**, and say *in the doc* why the SHA is deliberately absent, or a later reader
helpfully restores one. Same family as the stale-`master`-SHA incidents: **a hash written into prose is
a fact with an expiry date and no expiry stamp.**

## G186 — `A,B,A,B` is BIASED when the disturbance is monotonic; counterbalance as `A,B,B,A`

Asking for an A/B hook cost with a repeat of each build, the obvious ordering is `A,B,A,B`. It is
wrong here, and the reason generalises. **Warm-up (`G66`) makes EARLIER legs slower.** In `A,B,A,B` the
A-side occupies positions {1,3}, mean **2.0**, and B occupies {2,4}, mean **3.0** — so **A sits
earlier on average, a monotonic slow→fast gradient inflates A and flatters B, and the design
systematically hides the very cost the measurement exists to find.** In `A,B,B,A` both builds sit at
mean position **2.5** and any monotonic order effect cancels.

The gradient here is measured, not assumed: Build A's own two legs read `speed_ratio` **1.1677** on the
leg that ran FIRST against **1.0123** on the second — a **15 % spread, with the *smaller* copy on the
*slower* leg**, i.e. the ordering effect was larger than and opposite to the effect under test.

Two riders. **Precede the series with one leg DECLARED A DISCARD BEFORE IT RUNS** — never banked as a
measurement — to flatten the steepest part of the gradient; declaring it in advance is what keeps it
from being a result someone later chooses to drop. And **counterbalancing removes the bias, not the
noise**: report the within-build spread across positions beside the between-build difference, because
a difference smaller than that spread is *unresolved*, not *absent* (`G169`).

⚠ Owner-ruled, 2026-08-26. Recorded because the flawed ordering is the one that looks most obviously
"fair", and a design that is biased toward the null is the hardest kind to notice from inside its own
clean-looking output.

## G187 — "SUBMITTED" IS A GAME-THREAD CLAIM; THE FAILURE LANDS ONE HOP LATER ON THE RHI THREAD

**The named gap this replaces is now CLOSED FROM THE LOG, not from recollection.** The Build B crash
run's log survives as `Saved\Logs\StackOBot-backup-2026.08.26-10.27.55.log` — matched to the banked
crash session by UTC start (`run.json` `start_time_utc` `10:27:49.239Z` against the first capture line
at `10.27.49:265`), viewport `821x869`, seed `4242`. The sequence, verbatim and in order:

```
[135] 10.27.49:265  Capture(sve): keyed frame id=1 submitted (rtframe=1136, fmt=18, rect=821x344).
[136] 10.27.49:287  SVE-WANT-TRACE arm 2/64 requestId=2 gameFrame=1135
[136] 10.27.49:287  Error: appError called: Assertion failed: State != D3D12_RESOURCE_STATE_COMMON
[137] 10.27.55:784  Capture(sve): keyed frame id=2 submitted (...)        <- 6.5 s later, crash handling
       Callstack: GetInitialResourceState() <- HandleTransientAliasing() <- RHIEndTransitions()
       LogThreadingWindows: Error: Runnable thread RHIThread crashed.
```

**THE LESSON THAT SURVIVES, and it is the strong one: a `submitted` line proves nothing about
execution.** `keyed frame id=1 submitted` printed cleanly, with the **correct** sub-rect
(`821x344` — the clamp passed) and the correct format (`fmt=18`), and the process died **22 ms later**
on the **RHI thread**, executing that same graph, in the D3D12 **transient** allocator. Submission is
a game/render-thread statement; the RHI thread is one hop further on, and everything that matters
about a GPU resource contract happens there. ⇒ **never read a submit/enqueue log line as evidence a
path works. The evidence is the artifact on disk** — here, `run.json` alone and **zero frames**.

🚨 **SECOND LESSON, AND IT COST A CORRECTION: A RECOLLECTION OF A LOG IS NOT THE LOG.** The account
carried into the next session was that the assert fired **on the second armed frame**, from which
followed *"a smoke test capped at one armed frame would have passed."* **The log refutes both.** The
assert fired after arm 1 was submitted and **before arm 2 was ever submitted** — arm 2's `submitted`
line is timestamped 6.5 s later, inside crash handling, with the modal dialog already up. A
one-armed-frame smoke test **would have crashed**. The factual half of the account was exact (the
quoted line is correct to the character); the **frame index and the inference built on it were not**,
and the inference was the part that would have shaped a gate. **Re-read the log; do not re-run the
memory of it.** Note also what this removes: "why did frame 1 survive?" was never a real question, so
the transient-allocator-reuse candidate offered to answer it explains nothing — `G120`, avoided by one
grep.

*(Root cause itself is unchanged and is recorded at `G165`: the RDG texture was created with
`TexCreate_ShaderResource` alone, so the transient allocator could derive no initial state.)*

## G188 — PowerShell variable names are CASE-INSENSITIVE, so `$r` silently destroys `$R`

Measured here while checking the insurance diff. A script held the repo path in `$R` and captured a
command's output into `$r`:

```powershell
$R = 'D:\...\AnomalyInjector'
$r = git -C $R apply --check --cached -- $diff     # <- this is the SAME variable
git -C $R diff ...                                 # fatal: cannot change to '--no-pager'
```

**`$r` and `$R` are one variable.** The capture overwrote the path; and because the successful check
produced *no output*, `$R` became empty, so every later `git -C $R <verb>` silently read its own verb
as the path. The tell is the shape of the error — `cannot change to 'read-tree'`, `cannot change to
'apply'` — a git verb appearing where a directory belongs **always** means the `-C` argument expanded
to nothing.

Two riders. **Never use a single letter and its own case-variant in one script** — the collision is
invisible and PowerShell issues no warning. And the failure mode is worst on the *success* path: a
command that fails leaves stderr in the variable and the script limps on; a command that succeeds
leaves it **empty**, which is when the damage is total. Same environment family as `G182` (here-string
becomes a pathspec) and `G183` (Bash-tool git hangs): **in this workspace, assume the shell is a
defect surface and verify what a command actually received.**

## G189 — A FIXER REUSED AS A VERIFIER IS VACUOUS BY CONSTRUCTION

The hostname/codename scrub tool substitutes forbidden terms and then reports whether any remain. Run
as the **verifier** that licensed deleting the only pre-scrub backup, it passed everything. Its check
was:

```python
out = raw
for pat, rep in SUBS:
    out, n = pat.subn(rep, out)     # runs even in DRY RUN
residual = RESIDUAL.findall(out)    # <-- the SUBSTITUTED text
```

It answered *"would my substitutions leave residue?"* — always no, for any file it can fix — instead of
*"does this file contain the terms?"*. **It could not fail on anything it was capable of fixing.**
Pointed at the known-positive pre-scrub tree it printed the substitution counts AND `[CLEAN]`,
`no residual`, **exit 0**.

**RULE: a fix step and a check step must read DIFFERENT inputs.** The fixer reads the input and
produces output; the verifier must read **the artifact as it stands**, not the fixer's projection of
what it would become. One line: `subject = out if apply_changes else raw`.

Generalisation, and it is the reusable half: **substitution-then-check answers a different question
from check-only, and the difference is invisible because both print the same words.** `G172`'s
vacuity, arriving through a reused code path rather than a missing key. This is also why `G96`'s
prove-it-both-ways rule is written as *before its verdict is read* — it is the only thing that catches
an instrument whose green means nothing.

## G190 — AN EXIT CODE SURVIVES A COMMAND THAT NEVER RAN

Verifying a scrubbed tree, the file list was passed as absolute paths under a long scratch directory.
Windows rejected the command line as too long, python **never launched**, and the wrapper still
printed `exit=0`. `$LASTEXITCODE` held a value from an **earlier** command.

The only reason it was caught: the run was supposed to print a `VERDICT:` line and there wasn't one.

⇒ **Assert on a POSITIVE ARTIFACT OF THE RUN — a line the run must emit, a file it must write, a
counter it must move — never on the exit code alone.** An exit code is a claim about a process that
may not exist. Corollary for this workspace: prefer relative paths from inside the tree under test;
the same 192 files passed relatively fit comfortably.

## G191 — AN EXTENSION LIST IS A BLIND SPOT; SCAN THE WHOLE TREE

The first scrub pass built its work-list with `git grep -- '*.md' '*.py' '*.cpp' '*.h' '*.ini' '*.txt'`.
It missed **`Source/AnomalyCapture/AnomalyCapture.Build.cs`**, which held three hits, because `.cs` was
not in the list. The tree-wide pass caught it; the hand-written glob never would have.

⚠ **The miss was not random — it was in the one file where the strings were LOAD-BEARING** (build-time
fork-detection needles), so the blind spot and the highest-stakes hit coincided. That is the usual
shape: the file you forgot to include is the one nobody thinks of as documentation.

**RULE: enumerate from the repository, not from a guess** — every tracked file plus the untracked docs,
decide text-vs-binary by attempting the decode rather than by extension, and **print the counts**
(`scanned N; skipped M binary; K exclusions`) so a shrinking denominator is visible. Any file
deliberately out of scope is an explicit, dated, **printed** exclusion — never an absence from a glob.
Silence and exemption must not look the same.

## G192 — A DIFFERENTIAL TEST RUN WHERE THE TWO PATHS REDUCE TO THE SAME EXPRESSION PROVES NOTHING

`G-M9` compares the m35 owned-copy readback against the pre-m35 whole-source readback on the same
frame. Its first comparator leg reported **90 frames, 3,686,400 bytes each, zero differing** — a clean,
categorical, completely uninformative pass. The leg ran at `rect=(0,0)-(1280,720)`, and at a **zero
origin** the two drains are the *same arithmetic*:

```
new     Base +  (int64)y                  * RowPitch          * BPP
legacy  Base + ((int64)(Rect.Min.Y + y)   * RowPitch + Rect.Min.X) * BPP
```

With `Rect.Min == (0,0)` the second collapses into the first. **The instrument was comparing an
expression to itself** and would have returned IDENTICAL even if the offset handling were wholly wrong
— which is the entire defect m35 exists to fix.

The gate was only discharged by driving the origin off zero: `rect=(0,92)-(1280,628)` and
`rect=(280,0)-(1000,720)`, the latter the first non-zero `Rect.Min.X` ever produced in this project.

**RULE: for any differential test, identify the parameter values at which the two sides DEGENERATE
into one, and make sure the gate is not standing on them.** A differential passes trivially at its
degenerate point, and the degenerate point is usually the default — zero offset, empty list, single
element, identity transform — so it is exactly where a leg lands if nobody chose otherwise. Ask what
value would make the comparison vacuous, then check whether that is the value you ran.

## G193 — A NEGATIVE RESULT CAN BE A PROPERTY OF THE FIXTURE, NOT OF THE HARNESS

`G170` measured that `IAI.Bench.Letterbox` refuses under `-unattended`:
**`LETTERBOX REFUSED - view target 'SpectatorPawn_2147482483' has no UCameraComponent`**. True, and
correctly recorded. The **inference** attached to it — *"`G-M7`, `G-M8` and `G-M9`'s both-origins half
must run in PIE"* — was wrong, and it stood for a week, scoping work onto a scarce manual session.

The refusal was a property of **`CB_GateLevel`**, whose `-unattended` pawn is a `SpectatorPawn`. Run
the same packaged harness against **`MainWorld`** and the view target is camera-bearing:
**`LETTERBOX APPLIED on BP_SpawnPad_C_… / Camera`**, giving `rect=(0,92)` and `rect=(280,0)` unattended.
The note *"UNTESTED AND CHEAP: a packaged MainWorld leg might supply a camera-bearing pawn"* was sitting
in the same section the whole time.

**RULE: when a capability is declared unavailable, name the FIXTURE the negative was measured on, and
vary it before accepting the scope decision.** `G120`'s foreclosure failure with a new face: an
observation about one map became a claim about the whole harness. Cheapest guard: any "X is impossible
here" line must carry the map, the pawn, the target and the config it was measured under — then the
next reader can see which of those to change.

## G194 — A CAPPED, SORTED LISTING HIDES EXACTLY THE CLASS AT ITS TAIL

The census's per-cycle `DRAWN-COVERAGE` listing capped at 60 entries **sorted by drawn pixels
DESCENDING**. On the 77-candidate bench the truncated tail was therefore not a random sample — it
was **every `MEASURED_ZERO` candidate**, because zeros sort last by construction. `P-C1`'s control
IS a zero, so the one listing that could show its verdict structurally could not show it, and the
line ended `(+17 more)` while looking complete enough to read.

**RULE: an absence read off a capped listing is not a reading, and a SORTED cap is worse than a
random one — the sort key decides WHICH class is systematically hidden, and it is usually the
extreme class, which is usually the interesting one.** Before reading absence from any listing,
check for the truncation marker; before trusting a listing at all, ask what the sort order pushes
into the tail. The fix here was to raise the cap AND add a companion line that names the
non-measured classes explicitly, so absence became decidable from either direction. (2026-09-01,
m36 S2 pre-flight — caught by reading a banked log BEFORE the gate ran, which is the only reason it
is a gotcha and not a false pass.)

## G195 — A MEASURED-CONTROL AND A SELECTED-CONTROL CANNOT BE THE SAME ACTOR ON ONE LEG

`P-C2` asked one control (`StaticMeshActor_49`) to be `MEASURED_NONZERO` **on every census cycle**
AND to be **selectable by the seeded draw**. Once selection was wired, those became mutually
exclusive: selection fires on the control, the fired anomaly HIDES it, and a hidden actor is
`NOT_MEASURABLE` — so the act of satisfying the second conjunct destroys the first. Measured, not
argued: on the main leg the Cube was measured on 8/91 cycles (fired on continuously, sole eligible
candidate at floor 6.0); on the companion leg, where the floor refused it so nothing fired,
**30/30**; and on the same legs `StaticMeshActor_100`, which is never fired on, read 91/91 and
26/26.

**RULE: when a prediction asks a control to be both OBSERVED-STABLE and ACTED-UPON, split it into
two legs (or two actors) at writing time — the action is usually what perturbs the observation.**
The wider shape: an instrument's control must not be the system's own consumable. (2026-09-01,
m36 S2, ruled PASS-WITH-READING; the wording is corrected for m37, not retro-edited into the
closed predictions file.)

## G196 — WRITE GEOMETRY PREDICTIONS AS MEASUREMENTS, NOT DIRECTIONS

`P-C13` conjunct 3 predicted the known-visible control's drawn SHARE would **rise** under a
pillarbox, reasoning: same drawn pixels, smaller denominator. The share **fell** (3.183 → 1.792 %,
2.263 → 1.648 %), because the pillarbox also CROPS — the drawn pixel counts fell by 0.317× and
0.410×, harder than the denominator's 0.5625×. The premise "drawn px invariant under crop" was
never stated, so it was never examined.

**RULE: a geometry prediction that names a DIRECTION quietly asserts every term it did not
mention is constant. Predict the MEASURABLE QUANTITY instead** — here, `frame_px == rect area`,
which conjunct 1 pinned at 518,400 vs the zero-origin control's 921,600 and which carried the whole
gate on its own. A direction is the RATIO of two predictions; get either denominator-side premise
wrong and the direction inverts while the mechanism under test is perfectly healthy. (2026-09-01,
m36 P-C13 — conjunct 3 refuted as a prediction while the gate's actual claim passed decisively.)

## G197 — A TRUNCATED SEARCH RESULT IS AN UNREAD SURFACE (G136'S SHAPE, PAGINATION EDITION)

A repo grep for the census host-tag command truncated at 25 results — with the pagination notice
printed right in the output — and the absence of the command from those 25 was read as "item 5 is
missing". A second implementation was then written, and the build caught it as a symbol
redefinition (`C2374`) because S1 had already shipped `IAI.Capture.CensusHostTag`.

**RULE: an absence-of-finding is only as good as the surface actually read, and a paginated result
IS a partial surface — the tool said so.** Before acting on "X does not exist", confirm the search
was exhaustive: no result cap hit, no pagination marker, no extension filter (G191's half of the
same lesson). The cheap discipline: any conclusion of the form "absent" must cite a COMPLETE
enumeration, not the first page of one. This is the same failure as G194 wearing a different tool —
there the log line truncated, here the search result did. (2026-09-01, m36 S2; cost one reverted
duplicate implementation and would have cost a redefinition on every future build.)

## G198 — `struct Foo*` INSIDE A NAMESPACE DECLARES A NEW TYPE, NOT A REFERENCE TO THE GLOBAL ONE

Writing `const struct FAnomalyCensusCounters* Census` in a parameter list **inside
`namespace AnomalyLabel`** compiled clean at the declaration — because it silently declared a NEW
incomplete type `AnomalyLabel::FAnomalyCensusCounters` — and failed only at the point of use, as
`use of undefined type`, in a different file, pointing at code that looked correct.

**RULE: an elaborated-type-specifier (`struct X`) in a scope where `X` is not yet declared
DECLARES `X` in that scope. Inside a namespace that means a namespace-local type that shadows the
global one you meant.** Fix: forward-declare at GLOBAL scope before the namespace opens, and
qualify the use (`::FAnomalyCensusCounters`). The trap's signature is the distance between cause
and symptom: the declaration is legal, so the error surfaces wherever the pointer is first
dereferenced, which can be another translation unit entirely. (2026-09-01, m36 S2, run_summary
census plumbing.)

## G199 — A RUNBOOK STEP THAT NEVER ENABLES ITS OWN PRECONDITION READS LIKE A CLEAN RESULT

The RDP card's Section B told the office to run a census leg with `IAI.Capture.Census 1` — and
never issued `IAI.Capture.Mask 1`. The census is only effective when the mask AND async capture are
both on (`bCensusEffective = census && mask && async`), so the leg would have completed, written
every artifact, and emitted **no census at all**: no echo, no counters, no histogram. On a sealed
host read over RDP by a non-technical operator, that is indistinguishable from a clean null —
`G139`'s exact failure mode, printed on the card that exists to prevent it.

**RULE: a runbook is code and its preconditions are dependencies — audit every card/recipe step by
asking "what would the operator see if the thing under test never ran?", and if the answer is "what
they would see on success", the card is broken.** The build's own warning line
(`census was REQUESTED but is INACTIVE`) is the backstop, but only if the card tells the reader to
look for it — the correction added both the missing command and the read that catches its absence.
(2026-09-01, caught while rewriting Section B, before any office leg ran.)

## G200 — AN INLINE `-m` COMMIT MESSAGE WITH QUOTES CAN SWALLOW THE COMMIT WHOLE

A `git commit -m "..."` issued through PowerShell with embedded double quotes broke the argument
quoting mid-message: git received the message's own words as separate arguments, interpreted them
as PATHSPECS (`error: pathspec 'how' did not match any file(s)`), and **the commit did not
happen** — while the surrounding script carried on. The staged state survived, so the failure was
recoverable, but a script that read only the last exit line would have reported a commit that does
not exist.

**RULE: commit messages go through a FILE (`git commit -F`), written by the editor tool, verified
by `git cat-file commit` — never inline `-m` through a shell, and never a shell-written file
(G141's BOM).** This was already the standing rule; the instance is recorded because the rule was
broken by the person who wrote it down, on the very next long message, which is the strongest
argument that it must be mechanical rather than remembered. (2026-09-01, RDP-card commit; redone
via the file path, byte-verified.)

## G201 — AN ARTIFACT HASH IS NOT CONTENT IDENTITY ACROSS TWO LINKS

The inert-merge proof was first attempted as "master's built exe should hash equal to the branch's
exe". It does not: `D2BB25A5` vs `70F6B72C` at **byte-identical size** (241,026,048), from
**byte-identical `Source/`** — MSVC embeds timestamps/build ids, so two links of the same source
differ. A hash-equality proof would have read as a FAILED merge on a merge that was perfect; and
its mirror image is worse — G121 already records two DIFFERENT builds sharing one exe hash. The
hash can miss in both directions.

**RULE: an exe hash identifies a BUILD ARTIFACT (G121's quartet role: "is this file the file I
staged/archived?"), never SOURCE CONTENT across separate links. An inert-merge proof is two other
things: SOURCE identity (`git diff branchA branchB -- Source/` empty, tree OIDs matching the
`merge-tree` prediction) plus BEHAVIOURAL identity (the two builds' artifacts compared by the
established control-pair method).** Both halves ran here and both passed; the hash comparison is
recorded only so nobody reaches for it next time. (2026-09-01, m36 merge to master; proof form
ratified by the owner.)

## G202 — A SELF-PROVING INSTRUMENT ECHOES THE SECRET IT PROVES ITSELF AGAINST

The codename scrub verifier runs a **mandatory self-test on every invocation**: it writes a synthetic
fixture containing every forbidden term, requires the check to FIRE, then requires the mapping to
CLEAR it. That design is exactly right — it is what made deleting the pre-scrub backup branch safe
(`G189`).

🚨 **AND IT PRINTS THE DECODED TERM TABLE TO STDOUT WHILE DOING SO.** The fixture's before/after
lines name every term in plaintext, along with the substitution patterns. The term table is stored
**base64-encoded on disk** precisely so no plaintext copy exists in any file — so journal 064's claim
is intact for FILES and silently untrue for **the tool's own output**.

**RULE: never paste raw verifier output into any doc, journal, commit message, handoff, OR a reply to
the owner.** ⚠ **The reply is the one people forget, and it is the widest channel** — replies get
pasted into chat, chat gets pasted into handoffs, and handoffs get committed. **Quote the
`SELFTEST ok` line and the `VERDICT:` line only.** Both are counts and adjectives; neither carries a
term.

⛔ **NO FIX TO THE VERIFIER IS PROPOSED, and that is deliberate:** suppressing the echo would weaken
the only evidence that the check can fail, which is the whole reason the self-test exists. **The
containment is a handling rule, not a code change.**

**Generalises past this tool:** any instrument that proves itself by *exhibiting* the thing it
detects — a scrubber, a secret scanner, a credential linter, a PII detector — **emits the sensitive
material as part of its passing output.** The safer the instrument, the louder its receipts. Ask of
every self-proving check: *what does its success message contain, and where does that message end
up?* (2026-09-02, session 067 — found by running the verifier, not by reading it.)

## G203 — THE SAME THING HAS TWO NAMES IN TWO ARTIFACTS, AND A JOIN ON THE NAME RETURNS NOTHING

The `P9` reader had to join `annotation.json` events to `labels.jsonl` rows. The natural key looked
like the anomaly's identity, and the predictions file specified exactly that:

```
anomalies[].id == anomaly_type   AND   target_name == nodes[primary].name
```

**The first conjunct is FALSE IN THE DATA.** Measured on banked `M23\R30_regress`:

```
annotation.json   "anomaly_type": "blink"        <- the CLIENT-FACING vocabulary
labels.jsonl      "id": "blinking"               <- the INTERNAL anomaly id
```

Two vocabularies **by design** — `annotation.json` is the delivered artifact and goes through the
client mapping; `labels.jsonl` carries the internal id. Requiring equality **joins nothing**, and
every event would have landed `UNDECIDABLE`.

⚠ **The failure is SAFE, and that is the trap rather than the consolation.** It fails to
*undecidable*, never to a wrong answer — so it produces a full page of plausible output with a
uniform verdict, and "every event undecidable" reads like a property of the DATA rather than a
broken join. A wrong answer would have been caught faster.

**RULE: before joining two artifacts on a name, PRINT BOTH SIDES OF THE KEY FROM REAL DATA.** One
`json.dumps` of one row from each file settles it. Never infer that two fields describing the same
thing hold the same string — especially across a delivered/internal boundary, which exists precisely
to rename things. Join on the identity that is not translated: here, the **target actor name**, which
both sides take verbatim from `AActor::GetName()`.

**Corollary for this project:** `MapAnomalyToClient` is a translation layer, so **any** join, filter
or grep that crosses `annotation.json` ↔ `labels.jsonl` on an anomaly identifier inherits this.
(2026-09-02, session 067 — caught while implementing, before any measurement; the spec defect was
annotated in the tool header and the journal, and the predictions file was NOT edited.)

## G204 — A SWALLOWED TRACEBACK PLUS A GUARD ON THE WRONG KEY PRINTS AN EMPTY TABLE AND EXIT 1

Second instance in two sessions of `G190`'s family, and this one is worth its own entry because the
failure looked like a *result*.

The `P9` reader's control run was invoked as `python p9_hidden_set.py <dir> --synth-shift 1 2>$null`
— stderr redirected, because the run was otherwise noisy with a Pillow `DeprecationWarning`. Inside,
`read_event` set `out["observed"]` **before** the anchor check returned early, so an
anchor-unreliable event carried `observed` but no `best_k`; the printer's guard tested
`"observed" not in r` and fell through to `r["best_k"]`. `KeyError`.

**What the operator saw:** the header lines, the column header, **then nothing**, then exit 1.

```
  ev  outcome          k   claimed   observed   missing  extra  sep/spr  minMrg
exit=1
```

**An empty table under a correct header reads exactly like "no events matched" — a clean negative.**
It was only caught because a *different* control had just produced twelve rows, so zero rows was
conspicuous. On a first run it would have been believed.

**RULES, three, and the third is the general one:**
1. **Never redirect stderr on a run whose result you are about to trust.** Filter the known-noisy
   lines instead — `Where-Object { $_ -notmatch 'DeprecationWarning' }` keeps tracebacks visible.
2. **Guard on the key the consumer actually dereferences**, not on a proxy for it. The printer needs
   `best_k`, so `best_k` is the guard. A guard on `observed` was testing a *different* question.
3. **An early return must not leave a half-populated record behind.** Publish the fields that make a
   record readable **after** every gate that can reject it, never before — otherwise downstream code
   cannot distinguish "rejected" from "complete".

⇒ And the reason it belongs beside `G190`: **assert on a POSITIVE artifact of the run.** This tool
now prints a `summary` line unconditionally, so a run that produces no rows still has to say so in
words. Absence of output is not a reading. (2026-09-02, session 067.)

## G205 — `& script.ps1 @{...}` PASSES THE HASHTABLE POSITIONALLY, AND THE LEG LOOKS FINE

Two `P9` legs were launched as:

```powershell
& .\run_leg.ps1 @{ Label='P9_Ap'; Map='/Game/StackOBot/Maps/MainWorld'; Anomaly='blinking'; ... }
```

**Splatting requires a VARIABLE.** `@{...}` written inline is a hashtable **literal**, so `@` is not
the splat operator there — the whole hashtable is passed as **one positional argument**, landing in
`$Label`. Every other parameter silently took its **default**.

🚨 **AND THE RUN SUCCEEDED.** Both legs completed, passed `A63`, passed the `B1` pose gate — of
course they did, they were running the harness's default `CB_GateLevel` / `StaticMeshActor_49`
calibration leg — and **banked a plausible-looking session** under the label
`System.Collections.Hashtable`. Nothing errored. The only visible tell was the label, and a label is
easy to skim past.

⚠ **The same shape bites `-File`, differently and more loudly:**
`powershell -File .\run_leg.ps1 -Anomaly '' ...` **drops empty-string arguments entirely** and fails
with *"Missing an argument for parameter 'Anomaly'"*. That one is safe because it is loud. The
splat-literal is dangerous **because it is quiet**.

**RULES:**
1. **Splat from a variable, always:** `$p = @{...}; & .\script.ps1 @p`. Never `& .\script.ps1 @{...}`.
2. **THE LEG'S OWN `_leg_geometry.json` IS THE ARTIFACT THAT CONVICTS IT — verify it PER LEG, EVERY
   LEG**, against the intended config on every axis. It is written by the harness from the
   parameters it actually received, so it cannot repeat the caller's mistake. That file is why the
   two bad legs were provably not the legs they claimed to be (`map=CB_GateLevel`,
   `target=StaticMeshActor_49`) rather than merely suspected.
3. **Record every axis a leg can be wrong on in that file.** It now carries `letterboxed_fixture`,
   `require_modal_rot_zero`, `seed`, `max_frames` and `bank_prefix` as well — a guard that only
   covers some axes lets the rest through.

**Generalises:** any launcher that accepts a permissive first positional parameter will absorb a
malformed argument bundle and run *something*. **Ask what a wrong invocation would produce, and make
the run write down what it actually ran.** (2026-09-02, session 067 — caught by the label, confirmed
by the geometry files, both legs discarded and re-run.)

## G206 — EVERY REQUIREMENT SATISFIABLE ALONE, THE CONJUNCTION SATISFIABLE BY NOTHING

The `P9` v1 plan needed a fixture with **two** properties:

| requirement | fixture that supplies it |
|---|---|
| a **non-zero view-rect origin** (Bates letterboxes; the whole point) | `MainWorld` — the letterbox lever refuses `CB_GateLevel`'s `SpectatorPawn` (`G193`) |
| a **settled camera** (every certified pixel result this project owns) | `CB_GateLevel` — its unattended camera is motionless |

**Each requirement was individually satisfied, checked, and written into the plan. The CONJUNCTION
was satisfied by neither fixture, and nobody asked.** Five legs ran and returned `UNDECIDABLE` on
every event: `MainWorld`'s intro camera moves during capture (32 distinct origins over 90 frames,
pitch −20°→0°), so the per-event bbox changed every frame and `A56` collapsed to modal 1-in-8.

⚠ **The warning was already in the record and was misfiled.** Journal 065 §11 noted `MainWorld`'s
"intro-camera→Bot switch" heterogeneity — carried forward as a *bbox* caveat for `P-C13`, not as a
*blocker* for any bbox-scoped oracle. **A caveat about one gate is a blocker for another.**

**RULE: check requirements as a CONJUNCTION, against a NAMED fixture, at plan time.** Write the
fixture's name beside the requirement list and ask *"does THIS ONE satisfy ALL of them?"* — a list of
individually-satisfied requirements is not a fixture, and a plan that never names one has not been
checked. `G135`'s family: the instrument environment could not exhibit the case, and the blindness
presented as a plan that read perfectly well.

✅ **The resolution is worth carrying too, because it inverts the usual move:** rather than making
the moving-camera fixture hold still, give the **settled** fixture the missing property. Two engine
console commands (`Set PlayerCameraManager bDefaultConstrainAspectRatio true` /
`Set PlayerCameraManager DefaultAspectRatio 2.39`) letterbox `CB_GateLevel` with **no source change
and no cook**, because `APlayerCameraManager::UpdateViewTarget` applies those defaults
**view-target-agnostically** (`PlayerCameraManager.cpp:352-355`) and a camera-less `CalcCamera` never
overwrites them (`Actor.cpp:3085`). **Ask which half of the conjunction is cheaper to ADD, not which
is easier to work around.** (2026-09-02/03, session 067.)

## G207 — PowerShell 5.1's `-Encoding UTF8` WRITES A BOM, AND `git commit -F` EATS IT AS THE SUBJECT

A commit message was written with `Set-Content -Encoding UTF8` and passed to `git commit -F`. **PS
5.1's `UTF8` means UTF-8 *with BOM*.** Git took the BOM as the first character of the **subject
line**, so `git log --oneline` renders:

```
6fb1215 ﻿docs(067): fixture-v2 legs - P9 did not appear on any gradeable event...
```

— an invisible mark before `docs(`. Every subject-line grep, changelog scrape and
`--grep='^docs'` now misses that commit.

🚨 **THE TRAP HAS TWO SIDES AND NEITHER DEFAULT IS RIGHT:** `Set-Content` **without** `-Encoding`
falls back to the **system ANSI codepage** and mangles non-ASCII; `Set-Content -Encoding UTF8`
**adds the BOM**. The obvious fix for one is the cause of the other.

**RULE: write commit messages with `-m`, or with a genuinely BOM-free writer** —
`[System.IO.File]::WriteAllText(path, text, [System.Text.UTF8Encoding]::new($false))`, or
`Out-File -Encoding utf8NoBOM` on PS 6+, or a non-PowerShell file writer. **Then read the subject
back** (`git log -1 --format=%s | Format-Hex | Select-Object -First 1`) before pushing anything whose
subject matters.

⛔ **A PUSHED BOM IS ANNOTATED FORWARD, NEVER FORCE-PUSH-FIXED.** Amending a pushed commit means
rewriting published history, which costs far more than one cosmetic mark. `e66fb1b` records it.
`G188`'s family — a PowerShell behaviour that survives review because the command reads correctly.
(2026-09-02, session 067.)

## G208 — `Set <class> <prop> <value>` REACHES LIVE INSTANCES AND IS NOT SHIPPING-GATED

The engine's `Set` exec command is a **legitimate zero-cook bench lever**, and it is worth knowing
precisely because it looks like an editor-only convenience:

```
Obj.cpp:3937   else if( FParse::Command(&Str,TEXT("SET")) )
Obj.cpp:3939       PerformSetCommand( Str, Ar, true );
Obj.cpp:3435   PerformSetCommand -> GlobalSetProperty(Str, Class, Property, ...)
```

Two properties that make it powerful:

1. **It applies to LIVE INSTANCES**, via `GlobalSetProperty` — not merely to a CDO, so it changes the
   behaviour of objects already ticking.
2. 🚨 **IT IS NOT SHIPPING-GATED.** The `#if !UE_BUILD_SHIPPING` block begins at `Obj.cpp:3947`,
   **after** `SET` and `SETNOPEC`. So it is live in Development and Test configurations.

✅ **What it bought here:** `Set PlayerCameraManager bDefaultConstrainAspectRatio true` +
`Set PlayerCameraManager DefaultAspectRatio 2.39` letterboxed `CB_GateLevel` — a fixture the
purpose-built lever **refuses** (`G193`) — with **no source change, no recompile and no cook**,
because `UpdateViewTarget` applies those defaults view-target-agnostically
(`PlayerCameraManager.cpp:352-355`) and a camera-less `CalcCamera` never overwrites them
(`Actor.cpp:3085`). It saved an entire cook and kept the legs on the same binary.

⛔ **RULES, AND THE FIRST IS NOT NEGOTIABLE:**
- **NEVER in a client-facing payload, config, ini, delivery script or client-facing doc.** It is a
  bench device. It edits engine state by name at runtime and depends on a non-Shipping build.
- **Every use is NAMED IN THE LEG'S READ-BACKS.** A fixture produced by a console command is
  invisible in the artifact unless something echoes it — here the `READBACK-LAYOUT` rect and the
  harness's own `-LetterboxedFixture` assertion carry it, and the assertion **invalidates the leg**
  if the rect origin is zero.
- **Reach for the purpose-built lever first.** This is the fallback for when the lever's precondition
  cannot be met, not a shortcut around writing one.

⚠ **And the general lesson: a capability you assumed was editor-only may be live in your packaged
build.** Check the guard, not the reputation. (2026-09-02/03, session 067.)

## G209 — `Get-Content -Raw` ON A **0-BYTE** FILE RETURNS `$null`, NOT `''`, AND `Test-Path` DOES NOT SAVE YOU

The idiom looks safe and is not:

```powershell
$stderr = if (Test-Path $err) { Get-Content $err -Raw -Encoding UTF8 } else { '' }
...
if ($stderr.Trim().Length -gt 0) { ... }        # <-- throws
```

`Test-Path` is **true** for a 0-byte file, so the `else { '' }` branch never runs — and
`Get-Content -Raw` on an **empty** file returns **`$null`**, not the empty string. The next `.Trim()`
throws *"You cannot call a method on a null-valued expression."*

**Measured, both directions:**

| probe | `Get-Content -Raw` is `$null` | `.Trim()` |
|---|---|---|
| 0-byte file | **True** | **THROWS** |
| non-empty control | False | returns the string |

🚨 **THE DANGEROUS PART IS *WHICH* CASE IS EMPTY: IT IS THE HEALTHY ONE.** The file that is reliably
0 bytes is a **stderr capture from a run that succeeded**. So the guard fires on **clean** runs and
never on failing ones — the exact inverse of what anybody testing it would try first, and it means
the failure only appears once everything else is working.

📌 **MEASURED INSTANCE, and it cost seven duplicated headless sessions.** The `_mailbox` watcher's
`run_brief.ps1` reads its stderr capture this way. Claude Code exited **`rc=0`**, the final message
was captured to `_final_<name>.txt` (5,107 B, intact), and then post-processing threw on the empty
stderr file — **before** the line that writes `to_chat\<name>.report.md` and **before** the line that
moves the brief to `processed\`. The brief therefore stayed in `to_code\` and the watcher re-fired
the identical brief six more times, each one a full session at high effort. **Nothing was wrong with
the work; the report simply never reached its reader, and the same brief kept arriving.**

**RULES:**
1. **Normalise immediately after the read**, never at the point of use:
   `if ($null -eq $s) { $s = '' }`.
2. Or make the call null-safe: `if ($s -and $s.Trim().Length -gt 0)`.
3. `Test-Path` answers *"does it exist"*, **not** *"is there anything in it"*. When the next thing
   you do is call a method, guard on **content**, not existence.

⚠ **And the second-order lesson, which is the expensive one: WHEN A HARNESS SWALLOWS ITS OWN OUTPUT,
THE WORK LOOKS UNDONE.** The failure presents as *"the task was never completed"* to whoever is
reading downstream, because the only channel that could say otherwise is the broken one. **Before
re-issuing a task that appears not to have been done, check whether it was done** — here,
`CLAUDE.md`'s status block, `git log`, and the staged exe hash all said it had been, three commits
earlier. `G142`'s family: a defect in the tooling around a result, wearing the result's clothes.
(2026-09-02, session 068.)

---

## G210 — DELIVERY MODE SILENTLY MIRRORS THE RUN LOG **OFF**, AND THE ECHO IS THE ONLY TELL

**Measured on Bates, 2026-09-02, during the `m40` validation (card `SECTION D`).** The owner ran the
leg exactly as carded and `anomaly_log.txt` **was not there**. Nothing failed, nothing warned, and
the capture itself was perfect.

**Cause, and it is by design.** `m38`'s run log defaults to **auto**, and *auto* **MIRRORS
`run.json`** — on when delivery is OFF, off when delivery is ON. **The Bates project runs DELIVERY
MODE**, so the run log was auto-OFF and the file was never written. `IAI.Capture.RunLogVerbose 1`
does **not** turn the log on; it only raises `LogAnomaly` to Verbose for the run, so setting it on a
delivery-shaped build buys nothing on its own.

**The fix on the box is one line, typed before `IAI.Capture.Start`:**

```
IAI.Capture.RunLog 1
```

🎯 **WHY THIS COST MINUTES INSTEAD OF A SECOND OFFICE VISIT: the `Capture(runlog)` echo states the
EFFECTIVE value AND its provenance, unconditionally, on every run** — including when the answer is
*off*. The owner read it, forced the log, and re-ran. **That is `G139`/`A48` earning its keep on a
host nobody here can inspect: an absent artifact that explains its own absence is a reading, not a
mystery.**

⚠ **THE GENERAL SHAPE, which is what travels: A DEFAULT THAT IS DERIVED FROM ANOTHER SETTING IS
INVISIBLE AT THE SITE WHERE IT BITES.** Nobody typing `IAI.Capture.RunLogVerbose 1` is thinking about
delivery mode. **When a diagnostic's default is a function of an unrelated switch, the diagnostic
must say so out loud every time it resolves** — and if it can be absent, its absence must be
explainable from something that IS present.

📌 **AND IT IS A LIVE DATA POINT, NOT ONLY A GOTCHA:** the run log's **client** default is still an
open owner question, and the first real delivery-shaped host needed it **forced**. Recorded, not
decided. (2026-09-02, session 068.)

---

## G211 — an ini key that duplicates the compiled default stops being a correctness dependency and becomes a PROVENANCE READOUT. Say so, or someone deletes it.

**2026-09-03, `m41`.** Before `m41`, `bMaskMeasureDefault=True` in the client's cooked ini was
load-bearing: without it the delivered build silently reverted to `m25` labelling — invisible anomalies
back, **with no artifact difference at all**. `m41` flipped the compiled default to `true`, so the key
now changes nothing about behaviour.

⚠ **The tempting conclusion — "it's redundant, delete it" — removes the only evidence the cook consumed
your config.** The run's echo says `from DefaultGame.ini [AnomalyCapture] bMaskMeasureDefault` when the
key is present and `from COMPILED DEFAULT (on)` when it is not; with the key gone, `G88` (a loose ini
beside a package is a silent no-op) becomes unanswerable again from the log alone.

🔑 **RULE: when a compiled default is raised to match the shipped ini value, KEEP THE KEY and write down
that it is now a provenance readout.** The safety gained is real and worth stating too: **a lost key now
downgrades provenance, never behaviour.**

---

## G212 — a "requested but inactive" WARNING becomes noise the moment the requested state is the compiled default

**2026-09-03, `m41`.** `bCensusEffective = census && mask && async`. Before `m41` a warning fired when
the census was requested but the mask was off — a genuine, rare event, because requesting the census
meant someone had typed a command or set a key.

**Flipping the census's compiled default ON would have made that warning fire on every run of the
shipped default on any host without an ini** — which inverts the loud-inert rule into noise. **A warning
that always fires is a warning nobody reads**, and the next real one is invisible inside it.

Two fixes exist and only one is right: gate the warning (it then cannot say what it means), or **flip
the sibling default too so the state it warns about is genuinely exceptional.** `m41` did the second and
reworded the line to name the cause — *"THE MASK IS OFF … reaching this line means something TURNED THE
MASK OFF: console or an ini key"*.

🔑 **RULE: before flipping a default ON, grep for every diagnostic that fires on that state and ask what
it will say on a default run.**

---

## G213 — a synthetic level with no post-process reads `0/0/0` on a preflight, and blindness looks exactly like a clean read. THE SCANNED COUNTS ARE THE INSTRUMENT.

**2026-09-03, `m41`, and it caught a real defect.** The `m41` host-PP preflight reports
`HOST-PP CUSTOM-DEPTH READERS = N` **plus the counts of what it scanned**. On `CB_GateLevel` it read
`= 0 (scanned 0 volume(s), 0 camera blend(s), 0 material(s))`.

**Both halves of that are true and they mean different things.** `CB_GateLevel` authors **no**
`APostProcessVolume` (its build script spawns only meshes, lights, sky and a `PlayerStart`), and the
`-unattended` `SpectatorPawn` pushes no camera blend — `ClearCachedPPBlends()` runs at the top of
`ApplyCameraModifiers` (`PlayerCameraManager.cpp:281`) and only modifier code ever calls
`AddCachedPPBlend`. So `V = 0` and `C = 0` are the **true** answers there: **blindness by fixture.**

🚨 **AND THE SAME INVESTIGATION FOUND THE SCAN WAS INCOMPLETE.** The engine builds a view's
post-process from **THREE** sources (`LocalPlayer.cpp:866-881`): volumes · the cached blends it labels
*"CameraAnim override"* · and **`View->OverridePostProcessSettings(ViewInfo.PostProcessSettings, …)`
under `// CAMERA OVERRIDE`** — where a `UCameraComponent`'s `PostProcessSettings` actually arrive, i.e.
**the most ordinary way a host applies a full-screen effect.** The first cut scanned two of the three.

📌 **A preflight printing only `READERS = 0` would have been GREEN on every level and would have shipped
the missing source.** The gate failed only because it demanded the scanned counts, and the pre-declared
clause called `0/0/0` **blindness, not a clean read**.

🔑 **TWO RULES:**
1. **Any "I looked and found nothing" instrument must report HOW MUCH IT LOOKED AT.** A count of zero
   findings beside a count of zero things examined is not a result.
2. **When a fixture cannot exercise an instrument, build a LEVEL-INDEPENDENT probe rather than trusting
   the fixture's silence** — here `IAI.Bench.ProbeSceneTextureUsage`, which drives one named material
   through the identical code path and prints the bits.
⚠ **And do not attribute the shortfall before measuring it:** `MainWorld` then read `V=1 C=1 M=0`, which
had been pre-declared as a code defect. Adding a discriminator first — **blendable ENTRIES reported
separately from resolved MATERIALS** — returned `entries = 0` ⇒ those settings carry no blendable at
all. **Content, not a broken walk. The pre-declared failure branch was refuted by measurement.**

---

## G214 — comparing two binaries: compare absolute counters as DELTAS that must be ONE CONSTANT. "Zero differences" fails for the wrong reason.

**2026-09-03, `m41`'s `P-C7` re-anchor.** The gate demanded `labels.jsonl` **0 row diffs** between the
new binary (census OFF) and a pre-change control. Measured: **90/90 rows differing**, on `t`,
`frame_index`, `view` and `anomalies` — while the **event set was string-identical** (same types, same
targets, same spans, same `frame_indices`).

**Cause, measured not argued:** the first control leg's whole run was **rigidly translated by 4 engine
frames**. Launch-to-first-arm over five legs — **m41: 1, 1, 1 · m40: 5, 1** — i.e. **the OLD binary
produced BOTH values**, so the offset is **run-to-run startup variance, not a property of the change**.
The arm **span was 119 on all five**.

⛔ **Widening the run-unique set to excuse it was refused** (`P30`'s laundering shape), and so was
re-running the control until it agreed.

🔑 **THE REPLACEMENT RULE (`P-C7 v2`), and it is STRONGER, not looser:**
- absolute counters (`t`, `frame_index`) are compared as **deltas** that must be **ONE constant across
  every row**;
- `view` and pose-derived fields must be identical after that constant is removed, **or differ by a
  single constant pose delta that is itself constant across rows**;
- everything else byte-identical; the run-unique set stays `{t_wall}`.

**It forbids DRIFT, which "0 row diffs" only forbade incidentally.** In the event, the pose-matched
cross-binary pair came back with **every delta zero** — byte-identity except `t_wall` — while the
non-pose-matched pair showed a **yaw drifting 0 → −0.175 → −0.35 → −0.525** and correctly failed the
pose conjunct (`A64`: pose match is a **precondition** of the pair comparison, not part of its verdict).
⚠ **Report both pairs and keep both legs. Reporting only the matching one is picking, however sound the
rule you applied afterwards.**

---

## G215 — check what a shipped GATE LEVER does under your new semantics before you change them

**2026-09-03, `m41`.** `IAI.Capture.CensusMaxAge` was a fixed verdict-age limit and its help text says
*"0 expires everything and is the `P-C11` loud-inert control"* — a documented, shipped diagnostic lever.

`m41` changed the semantics to `Window = max(knob, lastCompletedCycleTicks + 8)`. **Under a bare
`max()`, a knob of 0 yields a window of `cycleTicks + 8`, and `P-C11`'s lever silently stops working** —
no error, no warning, just a gate lever that quietly no longer does the thing its own help text
promises.

Caught by re-reading the knob's help text while editing it; fixed by special-casing knob `<= 0` to a
window of 0, and pre-declared in the predictions addendum **before** it was measured.

🔑 **RULE: when you change the semantics of a knob, enumerate its documented special values and check
each one still means what the docs say.** A lever that has silently stopped working is worse than one
that is gone: the next person to use it gets a clean-looking null.

---

## G216 — a WRONG BLANK is worse than a MISSING FILE. An artifact that states something false beats one that states nothing.

**2026-09-03, `m43` attempt 1.** The target mask wrote a blank (all-zero) PNG on every captured frame
where `m26` had declined to arm. A blank mask **asserts** *"measured, and no target was visible"* —
real ground truth for a hide-type anomaly. But `m26` declines for its **own budget** reasons
(`MaxArmsPerEvent = 4`), not because anything is hidden, so **71 of 90 frames were written as blank
while most of them had a plainly visible target.**

⚠ **A consumer cannot tell a wrong blank from a right one.** A *missing* file says "no information" and
is self-announcing; a *blank* file says something specific, and if it is wrong it is indistinguishable
from correct ground truth. **The absence was the safer artifact and the code chose the assertion.**

🔑 **RULE: before writing a value that ASSERTS something, check that the condition you are asserting is
the one you actually measured — not a proxy that happens to correlate.** *"The measurement subsystem
declined"* is not *"the object is not visible"*.
📌 Same family as `m26`'s `NOT_MEASURED` ≠ `MEASURED_ZERO`, one level down: there it is per event, here
it is per frame.

---

## G217 — a single render served FIFO STARVES whichever consumer arrives last, and the starvation is invisible until you count arms against passes

**2026-09-03, `m43` attempts 2–3, and it found a defect in ALREADY-SHIPPED code.** The `m26` visible-mask
pass renders **once per frame** and consumed **exactly one** pending arm (`RequestId = PendingArms[0]`).
By `m41` there were three consumers: `m26`'s ≤4 arms per event, the census's ~0.8 arms per frame, and
the new target mask.

**The arithmetic is the whole diagnosis:** 90 captured frames · **106** mask passes · `m26` **24** arms
· census **72** arms ⇒ 96 of 106 slots already spoken for, and the newcomer got **10**.

🚨 **And it was not only the newcomer.** On shipped `m41` with the census ON, **2 of 24 `m26` arms were
never served at all** and the rest ran up to **3 frames late**; with the census OFF, **24/24 at lag 1**
(two control legs). ⇒ **`framesContributed` — a VETO INPUT — was coupled to census cycle length.** One
event measured `fc 2 → 4` once fixed. ⚠ **Latent: no verdict was observed to change.**

🔑 **TWO RULES:**
1. **When several subsystems share one per-frame resource, count REQUESTS against SERVICES.** A queue
   that never drains looks identical to one that is merely busy.
2. **If the served result does not depend on who asked, SERVE EVERY PENDING REQUEST FROM ONE
   PRODUCTION.** Here the RT's content depends only on which actors are tagged at render time and each
   consumer already filters by its own tag set, so one render is the *same answer* delivered to each
   asker — exact, not an approximation, and it removes the contention rather than prioritising within it.

---

## G218 — a derived artifact must take its LIVENESS from the same source as the labels it accompanies

**2026-09-03, `m43` attempt 3.** The target mask decided "is a target live this frame?" from *"did `m26`
arm?"*. But `m26`'s records **outlive their fire window** — it keeps arming to spend its budget — so on
2 frames the mask carried a target silhouette while the label row for that same frame read
`anomalies = 0`.

⚠ **A mask that contradicts its own labels is worse than a missing mask**, and it is the only failure a
segmentation consumer cannot detect: both files are present and internally well-formed.

✅ **The fix is not "add a check" — it is to READ THE SAME THING.** Liveness and the write filter now
come from `Auto->GetLiveFires()`, the identical call `FinalizeArmedLabel` builds the label row from, in
the same tick, with the underlying state mutated only in the injector's `Tick` which has already run.
**So the two cannot disagree by construction** — and, deliberately, **without depending on the
`OnWorldTickEnd` multicast order**, which is precisely the undeclared-ordering assumption `P9` was made
of.

🔑 **RULE: when artifact B must agree with artifact A, derive B from A's source, not from something
that usually tracks it.** A second source is a second chance to be wrong, and the disagreement will be
silent.

---

## G219 — a gate predicate can be OVER-STRICT on a counter that exists to absorb the thing you are measuring

**2026-09-03, `m43` gate D.** The predicate was *"`tagOvertaken` unchanged or lower"*. It read **0 → 1**,
fully attributed (same binary, target mask the only variable) — and `tagOvertaken` is precisely the
counter the census built for this class, its own log text calling a re-tag by the event mask *"the
expected case"*. Everything the counter guards stayed put: `framesPolluted` 0, `batchesLost` 0, cycle
histogram identical, verdict set identical.

⚖ Ruled **PASS-WITH-READING** (`P-C2` precedent), with the corrected predicate recorded in the journal
and **the closed predictions file left unedited**.

⚠ **The trap this is NOT:** the fix is not "loosen the gate when it fires". It is that the predicate
named the wrong quantity — it constrained an *observation counter* instead of the *outcomes* that
counter exists to protect. **Write gate predicates against what must not change, not against every
number that might move.**
📌 **And a mechanism of mine was refuted the same day:** I attributed the rise to the target mask's
tag/restore cycle; the `tagFlips` counter added afterwards read **0 on every leg**, so that mechanism
does not stand. **Recorded as refuted rather than quietly dropped** (`G120`).

---

## G220 — `-testexit` matches the engine's OWN echoed command line, so a self-naming trigger quits instantly

**2026-09-03, `m43` gate (x).** A graceful-shutdown leg was launched with
`-testexit="Capture run FINISHED"`. The process exited with code **0**, zero asserts — and **had
captured nothing**: `LogInit: Command Line:` echoes the full argument string, which *contains the
trigger*, so the engine quit on its own first log line.

🚨 **It would have read as a clean pass.** Exit 0, no asserts, no session — the emptiest possible run
producing the greenest possible result (`G146`'s shape again, and `G96`'s: the check never ran).
⚠ A second attempt with a multi-word trigger failed the same way, because the unquoted spaces split the
argument.

🔑 **RULES:** never use a `-testexit` / log-trigger string that appears in the command line itself;
prefer a marker the *work* emits. And **always confirm the run produced its artifact before reading its
exit code** — an exit code describes how a process ended, never whether it did anything.
📌 The leg that finally counted ran the capture to completion, polled for `run_summary.json`, then
closed the window (`WM_CLOSE`): exit **0**, `Object subsystem successfully closed.`, **0** asserts,
**30/30** masks flushed, folder **deletable**.

---

## G221 — a MONOLITHIC packaged build cannot see a missing `MODULE_API`. The EDITOR target is the modular control, and it is the one the cook runs on.

**2026-09-03, owner-found on the Bates editor build; five milestones after it was introduced.**

`AnomalyInjectorLog.h` declared `DECLARE_LOG_CATEGORY_EXTERN(LogAnomaly, Log, All);` **without a module
export**. From `m38` onward the `AnomalyCapture` module logs to `LogAnomaly` and references the category
**object** across the module boundary. The dependency was correctly declared
(`AnomalyCapture.Build.cs` lists `"AnomalyInjector"`); what was missing was **symbol visibility**.

**Measured, pre-fix:**
```
Module.AnomalyCapture.cpp.obj : error LNK2001: unresolved external symbol
"struct FLogCategoryLogAnomaly LogAnomaly" (?LogAnomaly@@3UFLogCategoryLogAnomaly@@A)
UnrealEditor-AnomalyCapture.dll : fatal error LNK1120: 1 unresolved externals   [exit 6]
```
**Fix:** `ANOMALYINJECTOR_API DECLARE_LOG_CATEGORY_EXTERN(LogAnomaly, Log, All);` — the standard idiom
(`CORE_API DECLARE_LOG_CATEGORY_EXTERN` in `CoreGlobals.h`). **exit 0.**

🚨 **THE PART THAT GENERALISES: THE BENCH COULD NOT HAVE CAUGHT THIS.** Every gate here runs the
**PACKAGED Development** build, which is **MONOLITHIC** — no DLL boundaries, so `MODULE_API` expands to
nothing and a missing export is **invisible by construction**. The **editor** target is **modular**, one
DLL per module, and is the only configuration where the symbol must cross a boundary.
⇒ **Not "we forgot to check". No amount of packaged-build gating could have found it.**

🚨 **And it was on a collision course with the client cook: the cook runs on EDITOR binaries** (`G47`,
runbook §8.6 step 3.5). The next cook would have failed at link, inside the delivery window, five
milestones downstream of the commit that caused it.

🔑 **RULE, now permanent: every feat milestone builds BOTH the packaged Development target AND the
Editor target on the bench, and both must exit 0.** In `CLAUDE.md`'s milestone gate template and in
`PRE-DELIVERY-CHECKLIST.md` §1.1.

🔑 **THE WIDER LESSON: when two build configurations differ STRUCTURALLY, a gate that only ever runs one
of them is blind to a whole defect class — and its blindness looks exactly like a pass.** Same family as
`G213` (a preflight that scanned nothing reporting zero) and `G119` (reading the source instead of what
the artifact enforces). Ask what the *other* configuration would have to do differently, and whether
anything ever runs it.

📌 **Audit note:** the grep found 79 `ANOMALYINJECTOR_API` occurrences across 10 public headers, so the
rest of the surface was already exported — **but the decisive check is the linker, not the grep.** The
pre-fix editor link reported **exactly one** unresolved external and the post-fix link reported none;
that is the tool whose job it is, answering directly.

---

## G222 — "Deimos" was a DUPLICATE codename for Bates. There are exactly TWO office hosts: Concorde and Bates.

**Owner correction, 2026-09-03: "Bates and Deimos are one and the same."**

For several milestones this project's docs read as though there were **three** office hosts. There were
always **two**. `Deimos` was a second codename that entered the record for the machine already called
`Bates`, and it is **RETIRED as of 2026-09-03**.

🔑 **HOW TO READ THE HISTORY: "Deimos" in any journal or predictions file MEANS BATES.** Those files are
**append-only history and are deliberately NOT retro-edited** — the same rule that keeps every other
superseded claim visible. Only the **living** docs were corrected (`CLAUDE.md`, `office-rdp-card.md`);
`docs/sessions/*` and `docs/predictions/*` still say Deimos and are correct as records.

⛔ **Never reintroduce the name.** The codename-only invariant now reads **(Concorde, Bates)**.

⚠ **One claim was WITHDRAWN with it, not migrated.** The RDP card's `A-9` carried a bullet reading
*"**Deimos**, if reachable: same A-5/A-6 sequence … **Being 5.2+**, it is the host where the pre-`m35`
code would have been wrong at a non-zero origin."* That bullet **described a third host that does not
exist**, and its engine-version claim rested on that host being a different machine. **It is deleted
rather than re-pointed at Bates** — re-pointing it would silently transfer an unverified property
(`5.2+`) onto a host whose lineage is recorded elsewhere. 📌 **`G120`'s shape: a stale entity carried an
unverified attribute, and deleting the entity has to delete the attribute too, not relocate it.**

🔑 **THE TRANSFERABLE PART: a duplicate name for one thing is worse than a wrong name for it.** A wrong
name is eventually questioned; a duplicate quietly doubles the apparent size of the world, and every
plan written against it budgets for work that does not exist. **When a codename is minted, check it is
not a second label for something already named.**

## G223 - a record born in the drain is one frame late, and anything keyed off it inherits the lag
`m43`'s target mask tags off an `FAnomalyMaskRecord`. Records were created ONLY by `FindOrAddRecord`,
called ONLY from `AccumulateFrameEvents`, called on the async path ONLY from the readback DRAIN - one
frame after the arm. So on a fire's FIRST frame no record and no tag existed, and the mask was blank
BY CONSTRUCTION, in any tick order. The shipped symptom was a systematic `+1` between the first
labelled frame and the first mask frame.
**The general shape: when a structure is created as a side effect of consuming an async result, every
consumer that reads it synchronously is silently one cycle behind.** Ask where a structure is BORN,
not only where it is read.
**And the fix is not automatic:** creating the record earlier is NECESSARY and NOT SUFFICIENT here -
a newly applied stencil tag is not in that same frame's custom-depth pass (journal 069 section 8.3),
measured, mechanism deliberately not asserted.

## G224 - a tie gate and a count gate are both satisfied by a uniformly late artifact
`m43` shipped the `+1` above behind a green gate set. Gate (ii) proved the delivered PNG's per-tag
pixel count equals the reduce table's count the veto reads - BIT-EXACT, 29 lines, 0 mismatch. Gate
(iii) proved the frame counts reconcile. **Both are true of a mask that is uniformly one frame late,
because both compare the artifact against ITSELF.** Nothing compared the artifact against the LABELS.
**A per-frame artifact needs a gate that joins it to the thing it claims to describe** - here,
first-labelled-frame vs first-mask-frame vs first-differing-picture-frame. Self-consistency is not
alignment.
**Corollary, and it fired immediately:** the first `m44` build produced ZERO masks and 90
`unmeasured` rows, and every gate still said PASS - the onset gate was `0/0`, the blank-PNG gate had
no files to be blank, the subset gate had no files to be stray. `G146` again: the emptiest possible
result produced the cleanest tick. **A gate whose subject can be empty needs a non-empty
precondition.**

## G225 - a post-upscale pass that samples pre-upscale scene textures with unscaled coordinates
**FIXED IN `m46` (2026-09-03)** - the pass now maps output pixels through the internal view rect,
nearest and clamped. Measured at `r.ScreenPercentage 50`: pairing CURRENT **0 of 26 before, 35 of 35
after**. It needed a FULL COOK (`G129`). The lesson below stands as written; only the defect is closed.

`AnomalyVisibleMask.usf:23` computes `P = SvPosition.xy + ViewRectMin` and loads CustomStencil,
CustomDepth and SceneDepth at `P`. The pass runs AFTER tonemap, so `SvPosition` is in OUTPUT space,
while the scene textures are at INTERNAL (pre-upscale) resolution. At 100% screen percentage the two
are the same and nothing is visibly wrong. **At any other ratio every sample lands in the wrong
place** - measured on the bench with `r.ScreenPercentage 50`: internal rect 640x360 against an output
rect of 1280x720, and the mask probe reads WRONG on 25 of 26 decidable frames (0 correct).
**Dynamic resolution, a non-100 screen percentage and temporal upsamplers are all ordinary
shipped-game settings**, so this is a host-configuration landmine, not a bench curiosity. It also
means the region beyond the internal rect reads whatever the POOLED texture still holds, which is not
cleared outside the current view rect.
**The general rule: any pass that reads scene textures must map its coordinates through the INTERNAL
view rect, and must never assume its own output space matches.**
Not the cause of the bench observations it was hypothesised to explain (journal 069 section 10) - it
is a separate real defect found while falsifying that hypothesis.

## G226 - a bench fixture that shares a namespace with the system under test will be mistaken for it
A mask-pairing probe was given stencil tag **250** and the plugin's magenta anomaly material. Both
choices collided with the system it was measuring:
- 250 is inside the tag allocator's range (`ReservedStencilBase 200` .. `AssignableStencilMax 254`),
  so the census - which tagged 78 candidates over 16 cycles - both handed 250 to another actor AND
  re-tagged the probe, because the probe is an ordinary visible static mesh and therefore an ordinary
  census candidate.
- the magenta material is the one `corrupted_texture` swaps its target to, so the picture-side
  detector merged the probe with the anomaly target (pixel count 8,000 -> 17,000 on exactly those
  frames).
Together they produced a confident, detailed and entirely false reading: "the mask carries content
the picture does not, on a quarter of frames, and is absent on another quarter." **It was the
fixture.**
**Pick fixture identifiers OUTSIDE every range the system can allocate** - here `ReservedStencilMax`
(255), which `AllocateTag` can never return - **and do not reuse an appearance the system under test
also produces.** The corrected probe reads 40/40 correct in both tick orders.
The tell was available and was nearly missed: the probe's own telemetry showed the census tagging 78
candidates out of a 55-value pool while the probe held a fixed value inside it.
## G227 - two consumers of one resource disagreeing about ownership, and only one of them asserting it
The mask pass feeds three consumers. `m26`'s ArmIfMeasurable always calls TagActor with its own value,
so it ASSERTS ownership every frame. The target mask's ArmTargetMaskOwn instead did
`if (IsAnyComponentTagged(Actor)) { ++TaggedCount; continue; }` - it accepted "somebody has tagged
this" as "it is tagged for me". The census, a third writer, skips actors that are already tagged, so
its guard is "already tagged" and not "under a live fire", and on a fire's FIRST frame the target is
not yet tagged and the census can take it.
Result: on the first labelled frame of an event the actor could be carrying a census value, or the
PREVIOUS event's value on that same actor, and the reduce - which filters on the event's own tag -
found nothing. That is a systematic one-frame-late mask, shipped behind a green gate set.
**When several subsystems write one shared per-object attribute, exactly one must own it at a time and
the owner must ASSERT the value, never test for "is it set".** "Already set" cannot distinguish "set
by me" from "set by someone else".
Measured: 4 of 4 events, two carrying a census value (204, 242) and two carrying the previous event's
value (224, 226). Turning the census off cured only the first two - which is why the census-only
reading of this was half right and would have shipped a half fix.

## G228 - at the delivered configuration, cross-run picture comparison has a ~9% floor
Two runs of the SAME configuration, same binary, same seed, identical camera pose, identical frame
alignment and identical event sets differ by **9.1612%** of pixels (>8/255), worst frame 15.40%. A
deliberate violation read 9.5381% and the correct fix 8.5619% - all one band, so the comparator could
not tell a correct hide from a broken one. Cause: temporal accumulation (TSR/TAA, Lumen) whose history
depends on startup timing. G169's shape at a much larger scale than journal 061's 0.116%.
**The cure is to remove the temporal confound, not to widen the tolerance:** with
`r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0,
r.ReflectionMethod 0` the control floor collapses to **ZERO of 60 frames**, so the sensitivity becomes
one pixel and the gate can decide.
**Therefore: any gate asking "did this change what renders" runs at the AA-off arbiter BY DESIGN.** It
answers that question exactly; it does NOT prove identity of the delivered temporal-AA picture, and no
such claim is made. A within-frame comparator (G-M9's shape) is what would.
⚠ `IAI.Bench.SynthTickOrder` is nondeterministic even at AA-off (control 5.95%), so the arbiter is
UNOBTAINABLE in that order - proven to be the lever's property, because the OLD hide is equally
nondeterministic there.

## G229 - the base-pass gather accepts a main-pass-off mesh; the PROCESSOR is what refuses it
Reading `SceneVisibility.cpp:2708` alone says a `bRenderInMainPass=false` primitive is still added to
`EMeshPass::BasePass` - the enclosing condition at `:2634` is an OR with `bRenderCustomDepth`, so the
command slot really is created. It is `FBasePassMeshProcessor` that refuses it, at
`BasePassRendering.cpp:1831` (`!PrimitiveSceneProxy || PrimitiveSceneProxy->ShouldRenderInMainPass()`).
**Gather and processor are two different filters and only reading the first one misleads in the
dangerous direction** - it would have said "the object still draws" and killed a design that works.
Same shape on the depth side: `ShouldRenderInDepthPass() = bRenderInMainPass || bRenderInDepthPass`
(`PrimitiveSceneProxy.h:613`), so ONE flag silences two passes and the second flag is not redundant.
## G230 - `IAI.Bench.SynthTickOrder` can host an ALIGNMENT gate but never a PIXEL arbiter
The lever relocates the injector's dispatch to `OnWorldPreActorTick`. Measured: with it on, two runs of
the SAME configuration and the SAME hide differ on **60 of 60 frames (mean 5.95%)** even at the AA-off
configuration where the native order's control is **0 of 60**. Poses, origins and `frame_index` are
identical across those legs, so it is not `A47` - the lever itself makes the run nondeterministic.
**Consequence, and it is a SCOPE rule rather than an exemption:**
- **alignment gates** (labels, masks, onset, mask-picture pairing - "is frame N's artifact about frame
  N?") run in **BOTH** orders, because that is exactly the class of defect the lever exists to expose;
- **pixel arbiters** ("does this change what renders?") run **native order, at the AA-off
  configuration**, because no cross-run pixel comparison can decide anything under that lever.
⛔ Do not read this as "m45 skipped a gate". The old hide is equally nondeterministic there - proven by
its own control - so the blindness belongs to the lever, and the lever never ships.
## G231 - editor on-demand shader compilation is REAL, and its trigger is PER VERTEX FACTORY
`FMaterialRenderProxy::GetMaterialWithFallback` (`MaterialShared.cpp:4207-4229`): if a material's
shader map is not complete it walks `GetFallback()` until one IS, and calls
`SubmitCompileJobs_RenderThread`. So in an EDITOR build a material really can draw the engine fallback
while it compiles, kicked from the render thread at draw time. The chain terminates at
`UMaterial::GetDefaultMaterial(Domain)` - the **grey grid**, not black.
**But completeness is asked of the WHOLE shader map while a draw needs ONE vertex factory.** Our two
swap materials carry 7 usage flags, so they compile for ~7 VF families; `FLocalVertexFactory` (a plain
static mesh) is compiled first. Measured on `m47`'s `E2b` leg with `-nomaterialshaderddc`: both
materials `IsComplete() == false` on **90 of 90 armed frames** with **3760 jobs outstanding**, and the
`corrupted_texture` target still drew correct magenta on every frame including every event's first -
target luminance 106.9-124.7 against a warm control's 106.9-127.0, zero black frames.
**So the mechanism is SUPPORTED and the symptom is NOT REPRODUCED at maximum forcing, and the
difference between those two statements is the VF.** The consequence is a prediction, not a claim: the
fallback window is wide for the VFs that compile LATE - skeletal, Nanite, cloth, morph - which is
where a host's symptom would sit, and it is where `m30`'s usage-flag defect sat too.
A packaged build cannot enter the path at all: `IAI.Bench.ForceAnomalyShaderRecompile` REFUSES BY NAME
there, because `ForceRecompileForRendering` is `WITH_EDITOR` only.
**The prewarm (`IAI.Capture.ShaderPrewarm`, `UMaterialInterface::EnsureIsComplete()`) is likewise
`WITH_EDITOR` in its body - measured cost in a packaged run 0.0017 ms. It is an editor fix.**
- A warm derived-data cache defeats a lever that only clears the in-memory map:
  `ForceRecompileForRendering()` gave `incomplete BEFORE=0 AFTER=0`. The lever that worked was the
  ENGINE switch `-nomaterialshaderddc`, which simulates a cold DDC on first encounter - which is also
  the owner's reported shape ("first use per combination, cached afterwards"). Keep the plugin lever
  anyway: its packaged REFUSAL is its own measurement.

## G232 - a process-global counter is not a per-frame gate input, and "IsComplete" is not "will draw"
`m47` was briefed to mark a frame `render_state: "shaders_pending"` when
`GShaderCompilingManager->GetNumRemainingJobs() > 0`. That counter is **process-wide**. Measured: on
the first editor run after a build it read **157 falling to 73 across all 90 captured frames** while
this plugin's materials were complete and every pixel was correct. The briefed gate would have marked
**90 of 90 frames** of a perfectly healthy leg - true, useless, and it teaches the reader to ignore
the key, which is worse than not having it.
**So the shipped gate keys on THIS PLUGIN'S OWN materials** (`CountIncompleteAnomalyMaterials()`), and
the global number ships beside it as a reported reading.
⚠ **And even the specific predicate over-fires, which is stated rather than papered over.**
`UMaterial::IsComplete()` checks `IsGameThreadShaderMapComplete()` per feature level - the whole map -
so it reads false while the vertex factory that actually draws is ready (G231). It means "something
about this material is still compiling", NOT "this draw will fall back". It errs toward marking good
frames suspect, which is the safe direction, and the log line says so in its own words.
**General form: before keying a gate on a counter, ask what SCOPE the counter has and whether that
scope is the thing the gate is about.** Global-vs-mine and whole-map-vs-this-draw are two different
scope errors and this one change contained both.
- Second half, from the same milestone: `frames_shaders_pending == 0` in a PACKAGED build is
  structurally zero, because the engine body it depends on is `WITH_EDITOR`. That is a READING, never
  a gate that passed (`G146`'s vacuity shape). The packaged evidence is the m47 BLACK-FRAME PIXEL GATE
  (`verify_capture.py --black-frame-gate`), per m19's standing "gate on PIXELS, not on a counter".

## G233 - EVERY BENCH LEG EVER RUN FORCED AUTO-EXPOSURE OFF; THE DELIVERED BUILD RUNS IT ON

**Measured 2026-09-03, session 069 brief 26 (`m47b`), eight legs, editor and packaged.**

Every leg this project has ever run issued `r.DefaultFeature.AutoExposure 0, r.EyeAdaptationQuality 0`
(`run_leg.ps1:187`, `run_leg_editor.ps1:78`, `verify_lastrundir.ps1:61`). **Nothing the plugin ships
does that** - `grep -i exposure Source/` finds one log string and no code - and **StackOBot's
`DefaultEngine.ini` sets no exposure key at all**, so a delivered build runs the ENGINE DEFAULT, which
is `r.DefaultFeature.AutoExposure = 1` (`SceneView.cpp:165-170`) with `Min 0.03 < Max 8.0`
(`Scene.cpp:468-469`), i.e. genuinely adaptive rather than the `Min == Max` fake-manual case.

🚨 **THE ASYMMETRY IS LARGE, AND IT WAS NEVER MEASURED UNTIL NOW.** Matched packaged pair, same
binary / seed / target / config, only those two cvars differing:

| | AE OFF (every historical leg) | AE ON (what ships) |
|---|---|---|
| whole-frame mean luminance | 102.488 | **77.907** (−24 %) |
| whole-frame spread over 90 frames | 7.100 | **32.020** (4.5×) |
| max drop vs the previous 8 frames | 2.04 % | **9.01 %** |
| the SAME anomaly's own target luminance | 123.7 – 128.1 (pinned) | **117.8 → 90.5** (−23 % across one session) |

**Four AE-OFF legs produced ZERO frames dropping more than 3 % below their own recent mean; four
AE-ON legs produced 13-21 each.** No overlap on any statistic.

🔑 **THE CONSEQUENCE FOR READING ANY OLD NUMBER: every luminance, black-frame and "dark first frame"
figure in this project's history was taken at PINNED exposure.** They are correct for what they
measured and they do NOT describe the delivered configuration. The m47 black-frame threshold 6.0 was
derived from a pinned-exposure darkest frame of 59.992; under AE the darkest legitimate frame is
72.419, so the threshold still holds with 12× margin - **that one survives, but it survives by luck of
direction, not because the regime was considered.**

⚠ **AND IT CONFOUNDS ONSET READINGS.** Under AE the first event of a session fires while exposure is
still converging, so its target reads **117.8 against a session mean of 95.0** - brightest first,
falling. That looks exactly like an onset effect and **is not one**: it is where in the session the
event fired. Any "first frame differs" reading taken with AE live must control for session position.

**General form: a cvar your harness sets on every leg is part of your fixture, not part of the
product.** If the shipped default differs, every number you have is from a regime the client never
runs. The tell is cheap and permanent - **echo the EFFECTIVE value (A48) on BOTH sides of the switch**,
so a leg's regime is read off its own log rather than inferred from the flag that was passed.

## G234 - AN ONSET READING TAKEN UNDER LIVE AUTO-EXPOSURE MUST CONTROL FOR SESSION POSITION

`m47b` measured the game's own auto-exposure and found the dominant effect is a **session-start
convergence transient** - whole-frame mean falls 103.9 -> ~74 over the first ~30-40 captured frames -
not an event-locked dip. The steady-state sawtooth is only 1.4-2.1%.

The consequence is a trap for every "does the anomaly look different on its first frame?" reading:
under live auto-exposure **the first event of a session fires while exposure is still converging**,
so its target reads 117.8 against a session mean of 95.0. **That looks like an onset effect and is
not one.** The same anomaly, same material, same target, later in the same session reads 90.5.

`m48` inherits the shape and states its own version of it: `exposure_dip` compares a frame against
the rolling mean of the previous 8 CAPTURED frames, so **the first 8 frames of a session can never
be marked**, and the marks that do appear cluster at session start because that is where the
transient is - measured on the `m48` AE-ON gate leg, 10 marks at session_index 8-19.

RULE: an onset or first-frame appearance claim measured with auto-exposure live is only sound if the
compared frames sit at COMPARABLE SESSION POSITIONS, or if exposure is pinned for that comparison and
the pinning is stated. Every exposure-pinned bench leg is a valid instrument for onset and an invalid
description of the delivered configuration; the AE-ON leg is the reverse. Neither alone is enough.

See also G233 (the standing asymmetry), G135 (a null bounded by what the fixture can exhibit).

## G235 - "IT MOVED NOTHING vs THE PREVIOUS BINARY" IS NOT A GATE RESULT. THE BASELINE IS THE LAST **PASSING** READING.

A permanent gate had a recorded passing baseline: MASK-PICTURE-PAIRING, `NEITHER == 0` AND
`PREVIOUS == 0`, read as **33/33 N0 P0** (native) and **35/35 N0 P0** (synth) at 069-22 A4 on the
m46 container, and **33/33 N0 P0** at 069-16 P6 before that.

Two milestones later the same gate read **`NEITHER 54` of 79 decidable**. The response was to run
an A-side on the **previous** binary. It read `NEITHER 54` too — so the change was reported as
*"m48 moved nothing, which is what 'pairing unchanged' asked for"*, with the band called *"a
pre-existing property of this fixture, reported not attributed"*.

**Both binaries were already off-baseline. The A-side compared two failing cells and found them
equal.** A bisect (069-28) then put the m46 binary — **the very binary that had produced the passing
33/33 N0** — in front of the newer leg recipe, and it reproduced `79 · 25 · 0 · 54 · 11` frame for
frame. The binary was innocent, the analyser was byte-unchanged, and **the LEG RECIPE had moved**:
the probe leg had begun firing `corrupted_texture`, whose material is the *same magenta asset the
probe itself wears*, so the picture centroid averaged two objects (G226 on the colour axis).

**RULE.** For any gate with a recorded passing reading:

1. **The baseline is the last reading that PASSED, on the recipe that passed** — never "the previous
   binary" and never "the last thing we built". Quote it with its leg recipe, not just its number.
2. **An A-side is only a control if the A-side itself is on-baseline.** Verify that before reading
   the comparison; otherwise "unchanged" is a statement about two unknowns.
3. **A gate clause that does not pass is a STOP, even when the delta is zero.** "Unchanged and
   failing" and "unchanged and passing" are different facts and must not share a sentence.
4. **The FIXTURE and the INSTRUMENT are part of the gate and are versioned with it.** If which
   anomaly, seed or frame-cap the leg fires can change the verdict, that recipe belongs in the
   gate's own artifact — not in whoever types the command. It regressed here precisely because it
   lived nowhere.

**AND THE HONEST HALF:** 069-27's restraint was *correct* — it refused to call the band an m48
regression and it refused to wave it through, and it said in as many words that the clause did not
pass. That is why the bisect was possible at all. **The defect was not the caution; it was accepting
a same-as-last-binary reading as evidence that a FAILING clause was fine.**

See also G226 (a fixture sharing a namespace with the system under test), G169 (a difference inside
the instrument's spread is below its resolution, never "no cost"), G121 (an exe hash does not
identify a build), G119 (read it back out of the artifact, do not trust the input you edited).

---

## G236 - a SHARED-ENGINE second host must mount by `git worktree`, not by junction, and the mount point must be EMPTY of `.uplugin` (2026-09-04, session 072)

**MEASURED.** Lyra's `.uproject` carries `EngineAssociation {B34F356C-4AE7-256A-F0E1-318A632BB902}`
- **the same GUID as StackOBot**, i.e. the same source-built UE 5.1. The 4.25 host may keep its
junction because its build outputs cannot collide with 5.1's; a SAME-ENGINE host is the opposite
case. Junctioning the plugin folder into a second 5.1 project makes both projects write the **same**
`Plugins/AnomalyInjector/Binaries` and `Intermediate`, so a build for host B can invalidate host A's
byte-identity anchors (`P-C7`) with nobody attributing it.

=> `git worktree add --detach <host>/Plugins/AnomalyInjector <sha>`. **Detached, deliberately:** a
second worktree cannot check out `master` while the main checkout holds it, and a detached HEAD
cannot be committed to in a way that moves a branch.

**AND THE TRAP THAT IS NOT ABOUT GIT AT ALL: UE SCANS `Plugins/` RECURSIVELY FOR `.uplugin`.**
A pre-existing copy parked *aside within* `Plugins/` is a **fatal duplicate-plugin-name error**, not
a harmless leftover. Move it OUT of the project tree. (Session 072 found a June-2026 clone already
sitting at the mount point - a real `.git` **directory**, no remote, 14 dirty paths. It was moved,
not deleted, only after its HEAD was proved an ancestor of `master` and every dirty path proved
present in `master` today - one had merely been **renamed**
(`create_missing_texture_materials.py` -> `create_anomaly_materials.py` at `1ccfca1`). `G92`: check
before you clear, the check is cheap.)

Precondition to verify BEFORE mounting: the plugin's `.gitignore` must already cover `Binaries/` and
`Intermediate/`, or the host's build output becomes untracked churn in a shared repo.

---

## G237 - a pre/post SNAPSHOT check cannot attribute a difference, and on a host that legitimately writes the same state it reports OUR failure (2026-09-04, session 072)

**MEASURED, on Lyra, with the discriminator already printed and simply not used.**
`CENSUS-HYGIENE final DIFF n=3 first=B_Hero_ShooterMannequin_C_12/CharacterMesh0 gained
bRenderCustomDepth (value 0) ... otherwise it is a hygiene defect and the leg FAILS P-C6.`
The leak probe was OFF, so by its own wording the leg failed. **It almost certainly leaked nothing:
the stencil value is `0`, and the plugin only ever writes `200..254`.** Lyra's hero characters enable
custom depth themselves, for the outline system - the host's own designed behaviour.

=> **A check built as "compare the world to a snapshot taken before we started" answers *did this
change?*, never *did WE change it?*** On a fixture that never touches the state (StackOBot) the two
questions coincide and the gap is invisible; on a host that writes the same state as part of playing
the game they diverge, and the check accuses the plugin. **The fix is not a wider tolerance - it is
to use the evidence already in hand** (here: the VALUE, which is outside our reserved range).

**Transferable form:** whenever a guard's evidence is "state differs from before", ask what else in
the process is entitled to write that state. If anything is, the guard needs an **attribution**
term, and a guard that fires on the host's legitimate behaviour will be trained away as noise -
which is worse than not having it. Cf. `G118` (a guard that passes the unsafe case), `G226` (a
fixture sharing a namespace with the system under test), `G120` (observation vs mechanism).

---

## G238 - "the map is in that plugin's folder" is an INFERENCE; the package path is a fact the project's own ini already carried (2026-09-04, session 072)

Session 072 launched Lyra at `/ShooterMaps/Maps/L_ShooterGym` because a recursive `.umap` listing had
been read as putting the file under `ShooterMaps`. The engine answered
`LogUObjectGlobals: Warning: Failed to load '/ShooterMaps/Maps/L_ShooterGym': Can't find file.`
**`L_ShooterGym` lives in `ShooterCore`.** Lyra's own `DefaultGame.ini` said so verbatim
(`+CommonEditorMaps=/ShooterCore/Maps/L_ShooterGym.L_ShooterGym`) and that line had been read and
**dismissed as stale** in favour of the inference.

=> A plugin mounts at `/<uplugin filename>/`, so the package path is derivable - but derive it from
the `.uplugin`'s own location, and **prefer the host project's own recorded path over any
derivation.** This is `G87`'s rule from the other side: judge by the name the host writes down, not
by the picture you assembled. Cost here: one 15-minute cold shader-compile cycle.

**Second lesson from the same failure, and it is about our harness:** a runner that DELETES the
previous log at launch destroys the evidence of the run that just failed. `lyra_leg.ps1` did, and
attempt 2's log - which held the map error, the plugin's clean init lines and a Lyra shutdown
callstack - survived only because it had already been read into the report. **Archive, never delete.**

---

## G239 - a process-global counter reads 7,301 on a real game; keying the mark on OUR OWN materials is what makes it usable (2026-09-04, session 072)

`m47` section `G232(a)` rejected the briefed shader-readiness predicate because
`GetNumRemainingJobs()` is **process-global**: on a healthy StackOBot editor leg it read 157->73
across all 90 frames, so a mark keyed on it would have flagged **90 of 90 good frames**. The shipped
mark instead keys on this plugin's own two swap materials, with the global number printed beside it
as a reading.

**Lyra puts a number on how large that mistake would have been:** `SHADERS pending=7301
incomplete=0` on **all 90 captured frames**, with `frames_shaders_pending = 0` and
`shader_prewarm_ms = 33,195` (StackOBot packaged: 0.0017 ms). A real game with TSR, Lumen, hardware
ray tracing and Nanite keeps thousands of shader jobs outstanding for the whole session as ordinary
background work.

=> **A counter that aggregates the whole process cannot answer a question about one subsystem's
objects, and the gap between the two grows with the size of the host - which is exactly the
direction in which the reading matters.** Corroborates `G232(a)` on an independent host, at roughly
50x the magnitude that motivated it.
---

## G240 - `FMulticastDelegateBase::Broadcast` iterates in REVERSE registration order, so two handlers on ONE delegate run back-to-front (2026-09-04, session 072/073)

Two handlers were bound to `FWorldDelegates::OnWorldTickEnd` in `Initialize`: the MASK block first,
the SAMPLE block second. The sample block searched the mask records the mask block creates - and on
every fire's FIRST captured frame it found none, defaulting `mask_value` to `0`.

The cause is in the engine and it says so itself:

    // call bound functions in reverse order, so we ignore any instances that may be added by callees
    MulticastDelegateBase.h:163

**Bound first == invoked LAST.** So "I registered A before B, therefore A runs first" is exactly
backwards for a multicast delegate. Measured: the `mask_value == 0` entry was each fire's first
fire-active captured frame **and no other entry** - 6 of 6 on Lyra, 6 of 6 on StackOBot, in every
banked session since `m43`, i.e. systematic rather than the "sporadic" journal 071 called it.

**THE FIX IS NOT TO SWAP THE TWO `AddUObject` CALLS.** That would work by relying on
reverse-broadcast continuing to hold, and this project has already ruled - journal 068 section 8, the
`P9` tick-order finding - that **an ordering guarantee cannot be requested, only constructed.** One
handler now calls the two blocks in explicit written order; the bodies are unchanged. The ordering
became a statement in our own code instead of a property of the engine's delegate container.

**Generalises:** any time two callbacks on the SAME delegate have a producer/consumer relationship,
that relationship is invisible at the registration site, silent at compile time, and reversed from
what the reading order suggests. If one must run before the other, put them in one handler.

---

## G241 - a per-frame predicate must be SAMPLED on its frame; the same read taken later in the drain is the stale read you just fixed (2026-09-04, session 073)

`observable`'s active term needed `IsFireLabelledThisFrame`. That function reads LIVE state
(`IsLogicallyHidden`, the anomaly's own state), and the natural place to call it - the loop that
builds the label row - runs in `ProcessCompletedFrames`, i.e. **in the async readback drain, several
ticks after the frame it describes.** Calling it there compiles, reads plausibly, and answers a
question about the WRONG FRAME.

It is sampled into `Snap->FireLabelled` in `SampleDeferredActiveState`, beside `FireActive`, at the
same tick-end where `ArmTargetMaskOwn` consults the same predicate to decide whether that frame's
mask is armed at all. **So the term that decides `observable` is the same term, on the same frame,
that decided whether `target_pixels` could exist.**

**The trap is that the fix and the defect look alike.** The onset-join defect (`G240`) was a
too-early read of a per-frame fact; the tempting implementation of its fix was a too-late read of a
different per-frame fact, one function over. Ask of every per-frame field: **on which tick was this
value true?**

---

## G242 - a term that is MEANINGLESS for a class is not merely unused; the moment you consult it, it is FALSE (2026-09-04, session 073)

`ComputeFireActive`'s fallthrough returns `IsLogicallyHidden(actor)`. For `FireWindow` ids (the
texture swaps) the actor is never hidden, so it returns **false** - and journal 071 section 3.1 had
already written that the value "is never consulted for them". A1 consulted it. Result:
`affected_frames` EMPTY on all four `FireWindow` events while `injected_frames` was correct, on a
build whose every other gate was green.

**Two lessons, and the second is the expensive one.**

1. A field documented as "not consulted for X" is a **latent false**, not a blank. Documentation that
   a value is unused does not make it safe to use; it marks it as a trap. If a value is meaningless
   for a class, the honest shapes are an assert, a `TOptional`, or a name that says so.

2. **A dead term takes its downstream counters with it.** `FramesConditionLost` was incremented
   inside `if (bActive && !bHeld)`, so the same false `bActive` made it **unable to fire for exactly
   the two types whose `IsVisualConditionHeld` override exists**. Its `0` on the failing leg was
   BLINDNESS, not a clean read (`G96`), and it read `0` beside the failure it was supposed to
   explain. **When a predicate is found to be wrong, audit everything guarded by it before trusting
   any of their zeros.**

---

## G243 - `target_pixels 0` with `mask_file: null` is CORRECT, and a checker that calls it a mismatch is the checker's bug (2026-09-04, session 073)

`m44` shipped the rule "no all-zero PNG is ever written - a file exists iff it has content". So a
frame whose target was MEASURED and drew nothing carries `mask_state: "empty"`, `mask_file: null`
and `target_pixels: 0`. My tie-checker asserted "a positive-or-zero `target_pixels` implies a mask
file" and reported **10 MISMATCHes on Lyra** - a confident, detailed, entirely false finding about a
build that was correct.

Also correct and equally confusing, on a probe leg: **`mask_state: "present"` with
`target_pixels: -1`.** `mask_state` is a FRAME-level fact and the bench pairing probe (tag 255) forces
the arm, so the frame's mask is present because of the PROBE, while the event's own tag was not in
`EventTags` that frame and therefore has no count. **The two fields answer different questions - one
about the frame, one about the anomaly - and they are allowed to disagree.**

Both were caught only because the numbers were read against what the code actually guarantees.
`G142`'s rule again: a verification script is a defect surface of its own, and a FALSE failure is
expensive because it teaches the reader to distrust a gate that was right.

---

## G244 - `A47` says the eye POSITION is invariant; that is a claim about a settled camera, not about every accepted leg (2026-09-04, session 073)

`A47` was amended on measured evidence: camera eye position invariant at `(-1500,0,260)` on
**369/369** banked gate samples, with the bifurcation living in ROTATION. Two `r.ScreenPercentage 50`
attempts this session failed the B1 pose gate with `rot` exactly `(0,0,0)` and eye origin
**Z = 273.3 against the settled 260** - a POSITION difference on a leg the rotation criterion would
have called clean.

They were still-settling legs, so this does not overturn `A47`: the invariance holds for a SETTLED
camera, which is what the 369 samples were. **But "modal_rot is (0,0,0)" is not by itself evidence
that a leg is at the calibration pose**, and a pose gate keyed on rotation alone would have accepted
those two. B1's pixel bbox comparison caught them; the rotation criterion did not.

When a pose check disagrees with `modal_rot`, **read `view.origin` before attributing anything** -
the harness prints the ratio and the discriminator precisely so the reader attributes rather than the
gate (`G123`).
---

## G245 - a STATIC actor silently refuses SetActorLocation, and a lever that logs its INTENT instead of reading the result produces a clean null (2026-09-04, session 074)

`IAI.Bench.TeleportTargetOffscreenAt` was built to make m49's OBS-2 gate able to FAIL: move the
live fire's target off screen mid-window so target_pixels reads a MEASURED ZERO. It logged a
confident line naming the actor, the old position and `-> +1,000,000 cm in Z` - and produced an
artifact **identical to the OFF control**, because `StaticMeshActor_49` is STATIC mobility and
`SetActorLocation` on a static component does nothing.

**The gate would have read GREEN on a lever that had done nothing.** Every row still had pixels,
every event still read observable, and the only thing saying otherwise was a log line asserting an
outcome nobody had checked.

The fix is not "remember that actors can be static". It is: **a lever must READ BACK the state it
claims to have changed and report the measurement, not the request.** The shipped line now prints
`from=`, `to=`, `promotedToMovable=` and **`MOVED=`**, with `MOVED=0` declared in the
lever's own text to mean *the leg is INVALID, not passed*.

This is `G119`'s rule ("read it back out of the running system") applied to a TEST LEVER rather
than to a build artifact, and `G114`'s failure mode ("a lever that does nothing produces a clean
null indistinguishable from a clean result") on a new axis. **A can-fail lever is itself something
that must be proven to fire.**

---

## G246 - the census hands out stencil values a LIVE EVENT is holding, and MASK-TIE structurally cannot see it (2026-09-04, session 074)

Found by `P-C7 v3` reporting `anomalies.target_pixels` differing across two runs on rows with the
SAME `mask_value`. Ground truth from the delivered mask PNGs: on those frames the event's tag has
**TWO connected components** - the target (66,837 px) plus a second object (18,330 px).

The census's own log says why:

    Census: ARM cycle=1 batch id=... size=27 tags=200..226 ...
    Census: ARM cycle=1 batch id=... size=27 tags=227..253 ...

**It allocates across the whole reserved range 200..254, including the value a live event currently
holds.** `m44`'s ownership rule is not violated on the anomaly side - that leg logs
`TAG-OWNERSHIP ... alreadyTagged=1 foreignValue=0` - the census simply gave the same value to
somebody else, and the per-tag reduce then counts both.

🚨 **MASK-TIE CANNOT CATCH IT.** The tie compares the reduce table's count against the delivered
PNG's own count of that value; the intruder is in BOTH, so it reads `tableCount=85167
pngCount=85167 MATCH`. A gate that compares two things which share a fault is self-consistent, not
correct - `G96` in a passing gate rather than a silent one.

Measured incidence across 16 legs / 448 tag-instances: **22 (4.9 %), 12 on one binary and 10 on the
next** => pre-existing and not binary-attributable. ⚠ "more than one connected component" is a
PROXY - a legitimately disjoint silhouette reads the same way - so the number is an upper bound on
that fixture, and the MECHANISM comes from the census's log, not from the proxy.

Consequences to carry: `target_pixels` and `bbox_drawn_px` both inherit it, and any future
"the mask is over-claiming" report should check the component count before blaming the mask pass.

🔻 **AMENDED 2026-09-04, session 075 - THE OBSERVATION STANDS, THE MECHANISM DOES NOT.** The
`tags=200..226` line above prints `Batch.Tags[0]` and `Batch.Tags.Last()`, i.e. **FIRST and LAST, not
the set** (`AnomalyCensus.cpp:794-797`), and the allocator skips non-free values inside that range -
so it does not show that a live event's value was issued. The census allocator DOES consult
`FAnomalyStencilTagLedger::IsFree`, which excludes `EventClaimed`, and `EventClaimed` is never
released mid-run. **The two components, the 4.9 % and the MASK-TIE blindness are unchanged and still
stand; "the census hands out a value a live event holds" is WITHDRAWN as established and is now one
candidate among four.** See `G249` and journal 075 §3.1.

---

## G247 - when a schema TIGHTENS a set, every gate keyed on that set is now asking a different question (2026-09-04, session 074)

`m49` redefined `annotation.json`'s `affected_frames` from "the injected subset" to "the
OBSERVABLE subset", and added `injected_frames` carrying the old meaning. `m44`'s **G7** - "a
mask file may exist only on a frame labelled anomalous" - keys on `affected_frames`. The target
mask is armed on every frame the event is INJECTED on, so from schema v2 on, a frame that is
injected-but-not-observable **legitimately** carries a mask file and G7 calls it stray.

It stayed invisible through every settled-bench leg (stray 0 everywhere, because nothing was ever
dropped) and fired the moment a leg deliberately dropped frames: `FAILED G7, stray 5`, all five
inside the injected window of the event whose visual condition a lever had just removed.

🔑 **The repair is to point the gate at the set it always MEANT.** `injected_frames` IS the pre-m49
`affected_frames`, unchanged, so re-keying **RESTORES** the original predicate - it does not loosen
it. Keying G7 on the observable subset would be a NEW and stricter predicate the code was never
designed to satisfy, and it would fail every correct run that drops a frame.

**Generalises:** a field that changes MEANING while keeping its NAME breaks consumers silently, and
it breaks them in the direction of a FALSE FAILURE on a correct build - which is expensive because
it teaches the reader to distrust a gate that was right (`G142`). When a schema tightens a set,
enumerate every gate reading it and ask, for each, *which of the two sets was it always about?*

---

## G248 - `injected_frames` is projector-gated for FireWindow ids and projector-independent for ActorHidden ids, so a can-fail lever's shape depends on the class it fires on (2026-09-04, session 074)

OBS-2 teleports a live target off screen and expects the frames to leave `affected_frames` while
STAYING in `injected_frames`. On a `missing_texture` leg the lever worked - rows read
`target_pixels 0` / `observable false` exactly as pre-declared - and **`injected_frames`
shrank too**, while three later events vanished entirely, vetoed by `m26` as MEASURED_ZERO.

Not a defect. For **FireWindow** ids `injected_frames` IS the pre-m49 `AffectedFrames`, which is
appended only when `ProjectActorBoundsToScreenRect` succeeds; a target moved wholly off screen is
therefore removed by the PROJECTOR and then by the zero veto, both **upstream of observability**.
For **ActorHidden** ids it comes from `ActiveByIndex` and never consults the projector.

That is exactly why Lyra's `blinking` event kept `injected_frames [4,5,9,10]` while
`affected_frames` shrank to `[4,5]`, and why the same lever on a texture swap cannot show that
shape. Re-run on `blinking`, OBS-2 reproduced the Lyra result on the settled bench in both tick
orders.

**Before designing a can-fail lever, ask which CLASS of id can even exhibit the shape you are
trying to force** - three mechanisms (the projector, the m26 veto, observability) can each remove a
frame, and only the last one is the one under test.
---

## G249 - a log line that summarises a SET as a RANGE cannot be read as a set, and a mechanism built on that reading is an over-read (2026-09-04, session 075)

`G246` attributed a real, measured defect - one stencil value carrying two objects - to "the census
hands out the whole reserved range, including live event values", on the strength of its own log:

    Census: ARM cycle=1 batch id=... size=27 tags=200..226 ...

That line is `FString::Printf(... "tags=%d..%d" ..., Batch.Tags[0], Batch.Tags.Last())`
(`AnomalyCensus.cpp:794-797`). It prints the FIRST and LAST value issued. The allocator walks a
cursor and **skips every value that is not free**, so the printed endpoints say nothing about which
values inside them were handed out. Reading `200..226` as "all of 200 through 226" is reading a
summary as an enumeration.

**The reading was wrong in the direction that matters: it made a fix look obvious.** The proposed fix
- give the allocator an ownership ledger so it cannot take a value a live fire holds - was already
implemented and had been for milestones: `FAnomalyStencilTagLedger` carries `EventClaimed` and
`CensusClaimed`, `IsFree()` excludes both, both allocators call it, and `EventClaimed` is never
released mid-run. **Building the "fix" would have changed no behaviour and shipped a green tick over
a live defect** - `G118`'s shape, reached through a log line rather than through a guard.

Two rules, and the second is the one that generalises:

1. **When a log line is load-bearing for a MECHANISM, print the set.** A range, a count, a min/max or
   a "first and last" is a summary; it answers "how big" and never "which". One `FString::Join` is
   the difference between a diagnostic and a guess.
2. 🚨 **Before building a fix, READ THE CODE THE FIX IS SUPPOSED TO CHANGE AND CONFIRM IT IS NOT
   ALREADY THERE.** The observation was real, the incidence was measured on two binaries, the gate
   blindness was real - everything except the causal sentence held up, and the causal sentence was
   the only part the fix depended on. **An observation and its explanation are separate claims**
   (the standing invariant, `G120`): here the observation survived and the explanation did not, which
   is exactly the split that invariant exists to make cheap.

⚠ **The tell that should have prompted the re-read: the proposed fix was one line and the defect had
survived two milestones of a project that gates everything.** A defect that cheap to fix, in code
this heavily instrumented, is more likely to be a misread than an oversight.

---

## G250 - a released resource is not a free resource, and the record that says so is often already in the codebase, wired to the wrong consumer (2026-09-04, session 075)

Chasing `G246`'s real cause turned up this shape. The census releases a stencil value at collect:

    AnomalyCensus.cpp:550-554
        Ledger->CensusClaimed.Remove(Tag);
        RecentlyReleased.Add(TPair<uint8, uint64>(Tag, GFrameCounter));

`RecentlyReleased` exists **because the project already knew a released tag lingers** - the restore
is a deferred proxy recreate, so the value can still be in the pixels for a few frames. But it is
consumed by `GetLegitTags()` only (`:217-225`), which feeds the POLLUTION DETECTOR's allowed set. It
stops the detector complaining about the lingering value. **It does not stop the value being
re-issued.** `IsFree()` never sees it.

So the codebase contained the fact "this value is not really free yet", correctly derived and
correctly expiring, wired to a consumer that only needed it to stay QUIET - and not wired to the
consumer that needed it to stay CORRECT.

**The transferable check:** whenever you find a "recently released" / "in flight" / "pending
teardown" list, ask **which consumers read it**, and then ask **which consumers make DECISIONS that
depend on the same fact**. If those two sets differ, the gap between them is a defect waiting for the
timing to line up. A quarantine that suppresses a warning and a quarantine that blocks reallocation
are the same fact serving two purposes, and only one of them was implemented.

⚠ **And price the fix before writing it:** folding the quarantine into `IsFree` shrinks the usable
pool (here: 55 assignable values against a census that has run 77 candidates in one leg). The
allocator's exhaustion path exists and is loud, but **a gate must read it**, or the fix trades a rare
wrong number for a frequent one.

---

## G251 - a "last resort" that re-issues a resource it has just proven is unavailable is not a fallback, it is the defect (2026-09-04, session 076)

`G246` measured one stencil value carrying two objects, so `target_pixels` and `bbox_drawn_px`
described two objects. `m50` step 0 found the whole of it in one line:

    AnomalyMaskMeasure.cpp, AllocateTag
      for every assignable value: if (IsFree(Tag)) { claim it; return Tag; }
      // no value was free:
      const int32 Fallback = ReservedStencilBase + (NextTagOffset % Span);
      UE_LOG(..., "TAG-POOL EXHAUSTED - ... Re-assigning %d; the collision detectors are the
             backstop and affected frames discard toward NOT_MEASURED, which ADMITS.")
      return Fallback;

The loop's entire job is to establish that no value is free. The line after it hands one out anyway.
**The comment even names the safe outcome it believes will follow - and that outcome does not
follow**, because the named backstops (verify read-back, unassigned-tag detection) both check that
the value is *ours*, and it is: it belongs to the census. The reduce then counts both objects and
the label ships a confident wrong number.

Measured, three ways, in increasing directness: the exhaustion count tracks the defect count leg for
leg across 16 banked legs (2->5, 1->5, 1->2, 0->0 x13); every one of the 12 affected tag-instances
carries a value that line re-issued, exactly 3 ticks later; and once the line was made to print the
ledger, it read **`Re-assigning 230, which is NOT FREE: censusClaimed=[...,230,...]`**.

**The transferable check: when a guard proves a precondition false and the code proceeds anyway,
the guard is a comment.** Grep for the shape - a validity loop that falls through to an
unconditional assignment - and ask what the caller does with the value. If the honest answer is
"nothing, it just uses it", the guard is decoration. **The repair is not a better fallback value;
there isn't one. It is to return failure and let the system take its already-existing safe path** -
here, no tag, no arm, state stays `NOT_MEASURED`, the veto ADMITS, and the event ships labelled
"not measured" instead of measured-wrongly.

---

## G252 - a gate built on a system's BOOKKEEPING inherits the bookkeeping's blind spots; gate on the live state (2026-09-04, session 076)

`m50`'s single-owner gate was first built as an **ownership-log join**: parse the allocator's own
ARM / RELEASE / assign lines, reconstruct who held each stencil value when, and assert one owner per
value per frame. It selftested green in both directions on synthetic logs.

Then the can-fail lever fired - a non-target actor deliberately given a live event's stencil value -
and the gate read:

    si=26 value=227 ncomp=2 sizes=[66837, 48578]  SINGLE-OWNER ::
      exactly one owner (event 'StaticMeshActor_49') - the extra component is that owner's
      own disjoint silhouette, not a collision

**It was correct about its own data and wrong about the world.** The intruder was tagged *outside*
the allocator, so it appears in no allocator line. Every log the gate reads was accurate; the union
of them was not the truth.

The rebuilt gate reads `FAnomalyStencilTagLedger`'s live tag map - the actual `bRenderCustomDepth`
value on the actual components at the moment the frame renders - and emits one `TAG-OWNERS` line per
captured frame. It sees a collision **however it was produced**, and it turned out to be **~7x more
sensitive** than the connected-component proxy as well (22 violated frames vs 4 on the same leg).

**Two rules:**

1. **Prefer the system's own live state over its records of that state.** Records are written by the
   code paths you know about; the state is written by all of them.
2. 🚨 **This is why a can-fail lever must BYPASS the mechanism under test, not exercise it.** A lever
   that produced the collision *through* the allocator would have been caught by the log join and
   the blind spot would have shipped behind a green tick. `G96` says prove the detector can fire;
   this adds: **prove it can fire on an input the detector's own instrumentation never saw.**

---

## G253 - pricing a fix before writing it can REFUTE it, not merely qualify it (2026-09-04, session 076)

`G250` closed with *"price the fix before writing it: folding the quarantine into `IsFree` shrinks
the usable pool (55 assignable values against a census that has run 77 candidates in one leg)"*, and
the `m50` plan carried that forward as a pre-declared read on every leg: `TAG-POOL EXHAUSTED`,
`tagOvertaken`, `census_fires_*`.

Step 0 then measured that read **already non-zero on the A-side**, and measured the exhaustion path
to be the *entire* defect. ⇒ **the quarantine would have shrunk the pool that was already being
exhausted, and made the defect it was meant to fix more frequent.** It was not built.

**The price was not a caveat to attach to the fix. It was the argument against it, and it had been
written down before the measurement existed.** A cost stated in advance is a falsifier: if the
measurement lands on the cost rather than on the benefit, the design is refuted and a note in the
"known limitations" section is the wrong response.

⚠ **And the shape recurs: this is the second consecutive session in which a fix was stopped by
reading the code it was supposed to change** (`G249` was the first - the ownership ledger already
existed). Both stops were cheap. Both would have shipped a green tick over a live defect.

---

## G254 - when two consumers share a fixed pool, the ELASTIC one must leave headroom for the INELASTIC one (2026-09-04, session 076)

The stencil pool has 55 assignable values and two consumers:

  - the **census**, which tags dozens of candidates per batch and can always arm fewer and requeue -
    it already had that path, and short batches are ordinary;
  - the **event allocator**, which needs exactly one value per anomaly event and has **no** graceful
    smaller request: an event either gets a tag or cannot be measured.

They were competing symmetrically, first-come-first-served, and the census - arriving in batches of
27 against 55 - routinely took the lot. The fix is one test in the census's allocation loop
(`NumFree() <= EventTagHeadroom` -> stop, requeue) and it changed no census semantics.

**The asymmetry to look for is not "who is more important" but "who can take less".** A consumer
that can degrade gracefully should be the one that degrades; a consumer whose only failure mode is
total should never be the one that hits the wall. Partitioning the pool was considered at `m44` and
correctly rejected (55 values against 77 candidates), and **headroom is not partitioning** - the
census may still use every value, it just may not take the last N.

⚠ **Price it and gate it.** Here: headroom 8 of 55, the census cut short 9-16 times per normal leg
and 108 times at a deliberately narrowed pool, `census_fires_*` unchanged, cycle length not
measurably affected, and `TAG-POOL EXHAUSTED` is a gate read on every leg rather than a log line
nobody looks at.
