# 2026-06-09-003 — M1: Anomaly Abstraction & Registry (IMPLEMENTATION)

> Implements the plan in `2026-06-09-002-m1-anomaly-registry.md` (approved in full; AMB-3 ruled
> capture-baseline). **All eight M1 stage gates passed** — clean headless compile + gates 2–7 verified
> live in PIE `MainWorld` via the unreal-mcpython bridge + owner eyeball (2026-06-09).

## Goal
Factor M0's hardcoded hide into the locked `IGDPAnomaly` abstraction + a subsystem-owned registry,
and implement three anomalies of different lifecycle shapes (missing_object / flicker / time_dilation)
to the M1 stage gate.

## What was done
- **Interface + helper (new, Public):** `IGDPAnomaly.h` (locked interface, plain C++); `GDPTargeting.h`
  + `GDPTargeting.cpp` (`FindActorsMatching` — M0's match loop lifted as a pure query).
- **Three anomalies (new, Private/Anomalies/):** `GDPAnomaly_MissingObject.{h,cpp}` (static),
  `GDPAnomaly_Flicker.{h,cpp}` (ticking, `while`-drain accumulator), `GDPAnomaly_TimeDilation.{h,cpp}`
  (world-global; captures baseline before `SetGlobalTimeDilation`, restores it on Revert).
- **Subsystem → manager:** added the registry `TMap<FName, TUniquePtr<IGDPAnomaly>>`, out-of-line dtor
  (G9), explicit registration in `Initialize`, `Deinitialize → RevertAllActive`, Tick drives active
  anomalies + heartbeat `(active: N/Total)`, and `ApplyAnomaly/RevertAnomaly/RevertAllActive/
  ListAnomalies/GetActiveAnomalyCount`. Removed `HideActorsMatching/ShowAllHidden/HiddenActors`.
- **Console surface:** removed `GDP.HideActor`/`GDP.ShowAllActors`; added `GDP.ListAnomalies`,
  `GDP.Apply`, `GDP.Revert`, `GDP.RevertAll`; kept `GDP.ListActors`. Module-scoped, null-guarded.
- **Config:** `Build.cs` comment refreshed (no new dep — `UGameplayStatics` is in Engine);
  `.uplugin` `VersionName 0.1.0 → 0.2.0`.
- **Docs:** appended gotchas **G9–G12**; rewrote `architecture.md` to M1 as-built + anomaly catalog +
  "how to add an anomaly"; updated `onboarding.md` (file tree + control surface) and `setup-runbook.md`
  §6 (smoke commands); updated `CLAUDE.md` Current status.
- **Built** `StackOBotEditor / Win64 / Development` against source UE 5.4.4 — **clean, exit 0**;
  `UnrealEditor-GDPAnomalyInjector.dll` relinked (log: `Saved/GDP_M1_build.log`).

## Problem → Resolution
- **P:** `TUniquePtr<IGDPAnomaly>` member in a UCLASS → incomplete-type at the generated destructor.
  **R:** Declared `virtual ~UGDPAnomalyInjectorSubsystem();` (no `override`) in the header, defined
  `= default` in the `.cpp` where the interface is complete. Gotcha G9.
- **P:** Concrete anomalies live in `Private/Anomalies/`, which UBT does not auto-add to the include path.
  **R:** Path-relative includes `"Anomalies/GDPAnomaly_*.h"`. Gotcha G10. (Build confirmed clean.)
- **P:** AMB-3 — what does `time_dilation` Revert restore to?  **R:** Owner ruled **capture-baseline**:
  capture `GetGlobalTimeDilation` before `SetGlobalTimeDilation`, restore it on Revert (fallback 1.0).
  Overrides the brief's literal "set back to 1.0". Gotcha G11.

## Deviations from plan
- **Re-entrancy consolidated into each anomaly's `Apply`** (revert-then-reapply lives in the anomaly,
  not also in the subsystem's `ApplyAnomaly`) — avoids a redundant double-revert; dispatch stays thin.
- **Added gotcha G12** (cross-anomaly overlap = last-writer-wins + the forward note about a future
  subsystem-level "hidden-by" coordinator for compound anomalies) per owner's AMB-4 instruction.
- **flicker toggle logs at Verbose** (matches M0's heartbeat convention). The runtime check raises
  `LogGDPAnomaly` to Verbose to count toggles (see gate 4 below).

## State (stage gate) — ALL PASS
| # | Gate | Status |
|---|------|--------|
| 1 | Compiles Development Editor, clean (headless) | ✅ `Build.bat` exit 0; DLL relinked. |
| 2 | `GDP.ListAnomalies` lists the three | ✅ log: 3, sorted (`flicker`, `missing_object`, `time_dilation`), `id - description - usage`. |
| 3 | `GDP.Apply missing_object` reproduces M0 | ✅ both `SM_Ramp` actors `bHidden=true`; log `matched 2 actor(s)`; revert → visible. |
| 4 | `GDP.Apply flicker` flickers (Tick) | ✅ continuous `flicker toggle -> HIDDEN/VISIBLE (2 actors)`; heartbeat `active: 1/3`; owner eyeball-confirmed the blink. (`while`-drain handled the low PIE FPS.) |
| 5 | `time_dilation 0.2` slows; Revert restores | ✅ `1.0 → 0.2 → 1.0` (captured baseline); owner felt the slow-mo. |
| 6 | `RevertAll` restores all; teardown auto-revert | ✅ RevertAll restored ramps+dilation; Stop-PIE log: `Subsystem deinitializing; reverted 2 active anomaly(ies).` |
| 7 | Re-applying an active anomaly doesn't leak | ✅ re-applied `missing_object` with a new substring → prior ramps un-hid, only new target hidden. |
| 8 | Docs updated; game-agnostic invariant intact | ✅ architecture/onboarding/runbook/gotchas G9–G12/CLAUDE; deps unchanged. |

## Runtime verification procedure (gates 2–7, to run once PIE is live)
Drive over `unreal-mcpython` against the **PIE game world** (`get_game_world()`), executing
`GDP.*` via `SystemLibrary.execute_console_command` (or calling the subsystem directly) and reading
state back:
- **2:** `GDP.ListAnomalies` → assert 3 lines, sorted (`flicker`, `missing_object`, `time_dilation`).
- **3:** `GDP.ListActors` → pick a persistent prop (e.g. an `SM_SatelliteDish*` `StaticMeshActor`, per
  M0); `GDP.Apply missing_object <sub>` → read each target's `bHidden == true`; assert logged match
  count ≥ 1.
- **4:** `Log LogGDPAnomaly Verbose`; `GDP.Apply flicker <sub>`; over ~2 s confirm repeating
  `flicker toggle -> HIDDEN/VISIBLE` lines + `IsActive`; owner eyeballs the blink.
- **5:** `GDP.Apply time_dilation 0.2` → read `WorldSettings->TimeDilation == 0.2`; `GDP.Revert
  time_dilation` → back to the captured baseline; owner feels the slow/restore.
- **6:** `GDP.RevertAll` → all `IsActive` false, targets visible, dilation baseline; Stop PIE →
  re-confirm nothing stuck (Deinitialize auto-revert).
- **7:** `GDP.Apply flicker A` then `GDP.Apply flicker B` (different substrings) → only B's targets
  active, no stranded-hidden actor from A.

## Functional verification (MCP-driven, 2026-06-09, PIE MainWorld)
Executed against the **PIE game world** (`UnrealEditorSubsystem.get_game_world()` → `MainWorld`), never
the editor world. Commands fired via `SystemLibrary.execute_console_command`; state read back by
iterating game-world actors (`bHidden`) and `GameplayStatics.get_global_time_dilation`; logs read via
`util_get_output_log`. Target: the two `SM_Ramp` props (clean, countable). Owner eyeball-confirmed the
flicker blink and the felt slowdown; teardown confirmed from the editor log after Stop-PIE.

## Hand-off
- **M1 stage gate fully passed** — compile, ListAnomalies, missing_object, flicker (tick + eyeball),
  time_dilation (state + feel), RevertAll + teardown auto-revert, no-leak re-apply, docs.
- Build log `Saved/GDP_M1_build.log`; outputs `Plugins/GDPAnomalyInjector/Binaries/Win64/
  UnrealEditor-GDPAnomalyInjector.dll` + `Binaries/Win64/UnrealEditor-StackOBot.dll`.
- **The locked `IGDPAnomaly` interface + registry are validated and ready** for the ~20 future
  anomalies — no interface change was needed. Adding one is the recipe in `architecture.md` / `CLAUDE.md`.
- **Next milestone (not started):** awaiting brief.
