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
