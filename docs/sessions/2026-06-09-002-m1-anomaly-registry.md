# 2026-06-09-002 — M1: Anomaly Abstraction & Registry (PLAN — implementation pending)

> **Status: plan approved by owner; NOT yet implemented.** One decision (AMB-3) is still open and
> blocks coding. This journal carries the full M1 plan so a fresh session can implement it without
> re-deriving. Planning used a 5-lens design-review (validating the locked interface against the M0
> code, ~20 future anomalies, and UE lifecycle pitfalls), synthesized and owner-approved.

## Goal
Factor M0's hardcoded hide into a general anomaly abstraction + a subsystem-owned registry, and
prove it generalizes via three anomalies of deliberately different lifecycle shapes (static,
ticking, world-global). The point is to **lock an `IGDPAnomaly` interface** that survives ~20 future
anomalies without a logic rewrite.

## Locked interface (do NOT redesign — flag mismatches as ambiguities)
```cpp
class IGDPAnomaly {
public:
    virtual ~IGDPAnomaly() = default;
    virtual FName   GetId() const = 0;            // snake_case, e.g. "missing_object"
    virtual FString GetDescription() const = 0;
    virtual FString GetUsage() const = 0;         // e.g. "<name-substring> [hz]"
    virtual bool    Apply(UWorld* World, const TArray<FString>& Args) = 0; // true = applied
    virtual void    Tick(float DeltaSeconds) {}   // optional; no-op for static anomalies
    virtual void    Revert() = 0;                 // undo everything Apply did
    virtual bool    IsActive() const = 0;
};
```
Plain C++ polymorphic interface, NOT a UCLASS. Registry = `TMap<FName, TUniquePtr<IGDPAnomaly>>`
owned by the subsystem; **explicit** registration in `Initialize` (no self-registration macros).
One instance per type; the instance carries its own active state. Anomalies cache target weak-ptrs
+ a world weak-ptr inside `Apply` so `Tick` needs only `DeltaSeconds`.

## Interface-lock verdict
**Holds — ship as-is.** Across ~20 surveyed future anomalies the strain concentrates on two axes
(prior-state capture; richer-than-string args), both fixable by convention, not signature change.
Only `pso_stall` "breaks" it, and that is a Build.cs module-deps decision (RenderCore/RHI), not an
interface flaw. Two cheap conventions to adopt going forward: (1) revert-from-captured-state
(= AMB-3); (2) a shared arg-parse helper (defer to the 2nd parsing anomaly). Keep
`FindActorsMatching` a free function — do NOT push actor-caching into a base class (world/cvar/spawn
anomalies would inherit dead weight).

## Plan — file by file
**NEW (9 source files + this journal):**
- `Public/IGDPAnomaly.h` — the interface (Public; the subsystem header holds
  `TMap<FName, TUniquePtr<IGDPAnomaly>>` and needs the complete type). No `.generated.h`.
- `Public/GDPTargeting.h` + `Private/GDPTargeting.cpp` —
  `GDPTargeting::FindActorsMatching(UWorld*, const FString&) -> TArray<TWeakObjectPtr<AActor>>`;
  M0's match loop lifted **verbatim minus the hide + per-actor side effects** (pure query).
- `Private/Anomalies/GDPAnomaly_MissingObject.{h,cpp}` — `FGDPAnomaly_MissingObject`. No `Tick`.
- `Private/Anomalies/GDPAnomaly_Flicker.{h,cpp}` — `FGDPAnomaly_Flicker`, the only `Tick` override.
- `Private/Anomalies/GDPAnomaly_TimeDilation.{h,cpp}` — `FGDPAnomaly_TimeDilation`. No actors, no `Tick`.

**MODIFIED:**
- `Public/GDPAnomalyInjectorSubsystem.h` — `#include "IGDPAnomaly.h"`; declare
  `virtual ~UGDPAnomalyInjectorSubsystem() override;` (G9); add private
  `TMap<FName, TUniquePtr<IGDPAnomaly>> Anomalies;` + manager API
  (`ApplyAnomaly / RevertAnomaly / RevertAllActive / ListAnomalies / GetActiveAnomalyCount`).
  REMOVE `HideActorsMatching` / `ShowAllHidden` / `HiddenActors` (state moves into `missing_object`).
  KEEP `ListActors`, lifecycle overrides, `Tick`, `GetStatId`, `DoesSupportWorldType`, heartbeat accum.
- `Private/GDPAnomalyInjectorSubsystem.cpp` — include the three anomaly headers; define
  `~…() = default;` here (complete-type TU for the `TUniquePtr` deleter — G9); `Initialize` registers
  the 3 anomalies; `Deinitialize` → `RevertAllActive()` then `Super` (preserves auto-restore-on-
  teardown); `Tick` ticks only `IsActive()` anomalies and heartbeat becomes `(active: N/Total)`;
  define manager methods; `ApplyAnomaly` enforces revert-then-reapply; console rewrite (keep
  `GDP.ListActors`; remove `GDP.HideActor`/`GDP.ShowAllActors`; add `GDP.ListAnomalies`/`GDP.Apply`/
  `GDP.Revert`/`GDP.RevertAll`; reuse `ResolveSubsystem` null-guard; unknown/no-arg → warn + suggest
  `GDP.ListAnomalies`).
- `GDPAnomalyInjector.Build.cs` — **no new dep** (`Kismet/GameplayStatics.h` is in `Engine`); comment only.
- `GDPAnomalyInjector.uplugin` — bump `VersionName` `0.1.0 → 0.2.0`.
- Docs at implementation time: `architecture.md` (catalog rows → as-built; fill "how to add an
  anomaly"), `CLAUDE.md` Current status, `onboarding.md` control surface, `setup-runbook.md` §6 smoke,
  `gotchas.md` append G9–G11.

**Build details (hard compile gate):**
- **G9** — `TUniquePtr<IGDPAnomaly>` member in a UCLASS: declare the dtor in the header, define
  `= default` in the `.cpp` (which sees the complete interface type).
- **G10** — UBT auto-adds only the `Public/` and `Private/` roots, NOT `Private/` subfolders →
  include anomaly headers path-relative: `#include "Anomalies/GDPAnomaly_Flicker.h"`.

## Three-anomaly spec
**missing_object** (static): `Apply` guards empty args, reverts-if-active, `FindActorsMatching` →
`SetActorHiddenInGame(true)` + cache weak-ptrs + per-actor log + summary "matched N"; `bActive=(N>0)`.
`Revert` un-hides live targets, reset. *Reproduces M0 exactly.*

**flicker** (ticking, default 5 Hz): `Apply` parses Hz (AMB-6), `HalfPeriod = 0.5/Hz`, caches targets.
`Tick`: `Accumulator += Dt; while (Accumulator >= HalfPeriod) { drain; toggle bHiddenPhase;
SetActorHiddenInGame(phase); log toggle (Verbose) }` — **`while`, not `if`**, so a long frame can't
desync the phase. `Revert`: force visible regardless of phase, reset accumulator/phase.

**time_dilation** (world-global, no actors/Tick): `Apply` caches `WorldWeak`, captures baseline,
`SetGlobalTimeDilation(Scale)`, reads back + warns if clamped (G11). `Revert`: restore captured
baseline (AMB-3). Proves the interface does not assume actor-scoping.

## Standalone ambiguities — resolutions
- **AMB-1** (interface contract): "match count" surfaced by the **anomaly self-logging**; the command
  logs the `bool` verdict. Keeps the locked interface actor-agnostic. **ADOPTED.**
- **AMB-2** (state-leak): zero-match `Apply` → **`false` / inactive**. Pins `true` = "applied with an
  observable effect." **ADOPTED.**
- **AMB-3** (⚠️ OPEN — owner ruling needed): `time_dilation` Revert to **captured baseline**
  (recommended) vs literal **`1.0`** (brief-literal). Deviates from the brief's "set back to 1.0";
  all M1 gates pass identically on Stack O Bot (baseline is 1.0). **BLOCKS the time_dilation revert
  implementation until ruled.**
- **AMB-4** (overlap): cross-anomaly same-actor hidden flag = **last-writer-wins**, documented
  limitation (terminal state after RevertAll is always visible). **ADOPTED.**
- **AMB-5** (determinism): `ListAnomalies` **sorts ids lexically**; registry stays a `TMap`. **ADOPTED.**
- **AMB-6** (crash guard): flicker Hz default 5; non-numeric or `Hz<=0` → warn + fall back to 5 (don't
  fail Apply); clamp ceiling ~60 (avoid `/0`). **ADOPTED.**
- **AMB-7** (guard): `if (!World) return false;` at the top of each `Apply`. **ADOPTED.**

## Planned gotchas (record G9–G11 when implemented)
- **G9** — `TUniquePtr<IGDPAnomaly>` in a UCLASS needs a declared dtor + `= default` in the `.cpp`.
- **G10** — UBT does not put `Private/` subfolders on the include path → path-relative includes.
- **G11** — `SetGlobalTimeDilation` is clamped by `AWorldSettings` Min/MaxGlobalTimeDilation.

## Stage gate (M1) + verification split
1. Compiles Development Editor, clean (headless `Build.bat`, exit 0). 2. `GDP.ListAnomalies` lists
the three (sorted, `id — description — usage`). 3. `GDP.Apply missing_object <sub>` reproduces M0
(hidden-flag read + eyeball). 4. `GDP.Apply flicker <sub>` visibly flickers (toggle logs + eyeball).
5. `GDP.Apply time_dilation 0.2` slows; `GDP.Revert time_dilation` restores (read world dilation +
feel). 6. `GDP.RevertAll` restores all + world-teardown auto-revert. 7. Re-applying an active anomaly
doesn't leak state. 8. Docs updated. — Non-visual gates closed via the `unreal-mcpython` MCP bridge
(match counts, IsActive, dilation value, toggle logs); owner eyeballs flicker + slowdown.

## State
Plan approved; **AMB-3 open**; **no plugin code written**. Docs seeded this session: this journal,
`docs/architecture.md` (current = M0 + catalog with M1 rows marked *designed*), and `CLAUDE.md`
(Current-status marker + Documentation-system map + doc-maintenance protocol).

## Hand-off
- **BLOCKING:** owner ruling on **AMB-3** (capture-baseline vs literal 1.0).
- Then implement per the file-by-file plan to the stage gate; update `architecture.md` catalog rows
  to *as-built*, `CLAUDE.md` Current status, `setup-runbook.md`/`onboarding.md` control surface, and
  append gotchas G9–G11. Close this journal (or open `…-003` for the implementation session).
