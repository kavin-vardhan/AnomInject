# Architecture (living — current as-built)

> **Reflects:** M1 — Anomaly Abstraction & Registry. **Complete — all 8 stage gates passed** (clean
> headless compile + gates 2–7 verified live in PIE via the unreal-mcpython bridge + owner eyeball,
> 2026-06-09). Detail in `sessions/2026-06-09-003-m1-implementation.md`.
> **Maintenance:** update this file to match the code at the end of every milestone; describe only
> what is built. Forward plans and design rationale live in the session journals.

## Purpose
GDPAnomalyInjector injects labeled visual anomalies (graphics bugs) into a running UE5 game to
generate synthetic training data for bug-detection ML. Game-agnostic (public UE APIs only); tested
on the Stack O Bot sample.

## Module & load
- One **Runtime** module `GDPAnomalyInjector`, `LoadingPhase Default`, `EnabledByDefault: true`
  (project-plugin scoped — gotcha G6), `VersionName 0.2.0`. Build.cs deps: `Core`, `CoreUObject`,
  `Engine` (M1 added no new dependency — `UGameplayStatics` is in `Engine`).

## The anomaly abstraction — `IGDPAnomaly`  (`Public/IGDPAnomaly.h`)
Plain C++ polymorphic interface (NOT a UCLASS — dispatch needs no reflection). One instance per type.
```
GetId() / GetDescription() / GetUsage()   // identity + help
Apply(UWorld*, const TArray<FString>&)->bool   // true iff an observable effect was applied
Tick(float DeltaSeconds)                  // no-op default; override only for ticking anomalies
Revert()                                  // undo everything; leaves IsActive()==false
IsActive()->bool
```
Contract: `Apply` returns **false** (and stays inactive) when an actor anomaly matches zero actors
(AMB-2); re-applying an active anomaly **reverts-then-reapplies** (no state leak); anomalies cache
their targets/world as `TWeakObjectPtr` inside `Apply` (GC-safe) so `Tick` needs only `DeltaSeconds`.

## Core component — `UGDPAnomalyInjectorSubsystem` (the manager)
- A `UTickableWorldSubsystem` (UCLASS): one per world, auto-ticks, `GetWorld()`. **Game + PIE only**
  via `DoesSupportWorldType` (gotcha G7).
- **Owns the registry** `TMap<FName, TUniquePtr<IGDPAnomaly>>` — plain C++, not a UPROPERTY. Registers
  one instance of each anomaly type in `Initialize` (explicit, no self-registration macros). Needs an
  out-of-line destructor (gotcha G9).
- **Tick:** drives `Tick(Dt)` on the active anomalies; the 2 s heartbeat now reports `(active: N/Total)`.
- **Deinitialize:** `RevertAllActive()` then `Super` — the generalized auto-restore-on-teardown.
- **Re-entrancy:** dispatch is thin; each anomaly's `Apply` does the revert-then-reapply.

## Lifecycle shapes (why exactly three anomalies)
The trio deliberately spans the axes the interface must cover, proving it generalizes:
| anomaly | scope | ticks? | proves |
|---|---|---|---|
| `missing_object` | actor | no | the static, actor-scoped baseline (re-homes the M0 hide) |
| `flicker` | actor | **yes** | the `Tick` path |
| `time_dilation` | world-global | no | the interface does **not** assume actor-scoping |

## Shared targeting — `GDPTargeting::FindActorsMatching`  (`Public/GDPTargeting.h`)
Free function (deliberately not a base class). Matches by `Actor->GetName()` **or**
`Actor->GetClass()->GetName()` `.Contains(substring)` (case-insensitive), **never** `GetActorLabel()`
(editor-only — gotcha G2). Returns weak-ptrs. Used by `missing_object` and `flicker`.

## Control surface (console commands)
Module-scoped `FAutoConsoleCommandWithWorldAndArgs`, resolved from the console's world, null-guarded
(warns outside Game/PIE). Output → Output Log, category `LogGDPAnomaly`.
- `GDP.ListActors` — log `Class | Name | Label` per actor (targeting aid, not an anomaly).
- `GDP.ListAnomalies` — list registered anomalies as `id - description - usage` (sorted).
- `GDP.Apply <id> <args...>` — look up id, apply (reverts-then-reapplies if active).
- `GDP.Revert <id>` — revert one active anomaly.
- `GDP.RevertAll` — revert all active anomalies.
*(M0's `GDP.HideActor` / `GDP.ShowAllActors` were removed — superseded by `GDP.Apply missing_object`
/ `GDP.RevertAll`.)*

## Anomaly catalog
| id | shape | usage | effect | revert | status |
|----|-------|-------|--------|--------|--------|
| `missing_object` | static, actor-scoped | `GDP.Apply missing_object <sub>` | `SetActorHiddenInGame(true)` on matches | un-hide / RevertAll / teardown | **as-built (M1)** |
| `flicker` | ticking, actor-scoped | `GDP.Apply flicker <sub> [hz]` | toggle hidden each half-period (default 5 Hz, clamp 60) | restore visible (any phase) | **as-built (M1)** |
| `time_dilation` | world-global, no tick | `GDP.Apply time_dilation <scale>` | `SetGlobalTimeDilation(scale)` (clamped — G11) | restore captured baseline (AMB-3) | **as-built (M1)** |

## How to add an anomaly
1. Implement `IGDPAnomaly` in `Private/Anomalies/GDPAnomaly_<Name>.{h,cpp}` — cache targets/world as
   weak-ptrs in `Apply`, undo in `Revert`, override `Tick(float)` only if it ticks.
2. Register it in `UGDPAnomalyInjectorSubsystem::Initialize`: `Register(MakeUnique<FGDPAnomaly_<Name>>())`.
3. Include the header path-relative from `Private/`: `#include "Anomalies/GDPAnomaly_<Name>.h"` (gotcha G10).
4. Add a catalog row above and a smoke line to the runbook.
No interface change is needed for actor-, world-, or cvar-scoped shapes.

## Limitations
- **Cross-anomaly target overlap** = last-writer-wins on the single `bHidden` flag (gotcha G12). Fine
  for one-anomaly-at-a-time use (terminal state after `RevertAll` is always visible). Compound /
  simultaneous anomalies will need a subsystem-level "hidden-by" coordinator — addable **without**
  touching `IGDPAnomaly`. Flagged, not built.

## Game-agnostic invariant
The module depends only on `Core`/`CoreUObject`/`Engine` and never references host (StackOBot) types.
All anomalies use public UE APIs only (`SetActorHiddenInGame`, `UGameplayStatics`).

## Verification model
Non-visual gates are checked in PIE via the `unreal-mcpython` MCP bridge (state/log reads: match
counts, `IsActive`, world time-dilation value, flicker toggle logs); the owner eyeballs the visual
gates (flicker, felt slowdown). See gotcha G8 for the bridge setup.
