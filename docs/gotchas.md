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
`GDP.ListActors` guarded by `#if WITH_EDITOR`, but **matching must never use the label** —
`GDP.HideActor` matches actor Name or Class only, keeping it forward-compatible with cooked
builds.

### G3 — `SetActorHiddenInGame` hides in game/PIE, not the editor viewport
The anomaly is only visible when playing (PIE or standalone), which is exactly the data-gen
context. Don't expect the editing viewport to change when toggling hidden state.

### G4 — The Bot is runtime-spawned
StackOBot spawns the player Bot at play time, so it is not in the world at PIE start.
`GDP.HideActor Bot` only matches after the Bot exists. For a deterministic smoke test, hide a
**persistent level prop** (a `StaticMeshActor` in `MainWorld`) instead.

### G5 — `UTickableWorldSubsystem::GetStatId()` is pure-virtual
It must be overridden or the subsystem won't compile. Implement with
`RETURN_QUICK_DECLARE_CYCLE_STAT(UGDPAnomalyInjectorSubsystem, STATGROUP_Tickables);`.

### G6 — Plugin enablement via `EnabledByDefault: true` is project-plugin scoped
We enable the plugin through `"EnabledByDefault": true` in `GDPAnomalyInjector.uplugin` so the
`.uproject` needs no `Plugins[]` entry. This is fine while it lives in a **project's**
`Plugins/` folder. **Revisit if this ever becomes an engine plugin** (installed under the
engine's `Plugins/`): `EnabledByDefault` would then auto-enable it for *every* project on that
engine, which is almost certainly not what we want.

### G7 — Restrict the subsystem to Game + PIE worlds
`DoesSupportWorldType` returns true only for `EWorldType::Game` and `EWorldType::PIE`, so the
subsystem never instantiates or ticks in the editor preview/editing world. Consequence: the
`GDP.*` console commands resolve a null subsystem when run outside a game world — they
null-guard and log a clear warning rather than crashing.

### G8 — MCP bridge: UnrealMCPython is a 5.6/5.7 plugin; needs a local patch to build on 5.4
To drive functional smoke tests via the `unreal-mcpython` MCP, the `UnrealMCPython` editor
plugin (from the RatBurglar project, descriptor `EngineVersion 5.7.0`) was copied into
`StackOBot/Plugins/unreal-mcp/` (host tooling — NOT part of the GDP plugin repo). Building it
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

### G9 — `TUniquePtr<IGDPAnomaly>` member in a UCLASS needs an out-of-line destructor (M1)
The subsystem owns `TMap<FName, TUniquePtr<IGDPAnomaly>>`. The `TUniquePtr` deleter needs the
**complete** `IGDPAnomaly` type at the point the map is destroyed. Declare the destructor in the
header (`virtual ~UGDPAnomalyInjectorSubsystem();` — no `override`; destructors can't be marked
`override`) and define it `= default` in the `.cpp`, which `#include`s the concrete anomaly headers.
Without this you get incomplete-type errors at the implicitly-generated destructor. (2026-06-09.)

### G10 — UBT does not auto-add `Private/` subfolders to the include path (M1)
UnrealBuildTool puts only the module's `Public/` and `Private/` roots on the include path, **not**
subfolders. The concrete anomalies live in `Private/Anomalies/`, so every include of them — in their
own `.cpp` and in the subsystem `.cpp` — must be path-relative from `Private/`:
`#include "Anomalies/GDPAnomaly_Flicker.h"`. Public headers (`IGDPAnomaly.h`, `GDPTargeting.h`)
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
which is addable **without touching the `IGDPAnomaly` interface**. Flagged, not built. (2026-06-09.)
