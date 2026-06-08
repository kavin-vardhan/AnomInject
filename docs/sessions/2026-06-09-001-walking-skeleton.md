# 2026-06-09-001 — Walking Skeleton (M0)

> Planning was approved 2026-06-08; implementation + build landed 2026-06-09. One session.

## Goal
Milestone 0 — prove the full inject/restore loop end-to-end with ONE hardcoded anomaly:
a project plugin that breathes (auto-ticking world subsystem with a PIE heartbeat) and can
hide/restore an actor via console commands. No anomaly abstraction/registry yet.

## What was done
- **Cleaned stale artifacts.** Deleted project-root `Binaries/` and `Intermediate/` (orphaned
  `StackOBot` module receipts). Preserved `Saved/`. Engine + plugin artifacts untouched.
- **Created the plugin** at `Plugins/GDPAnomalyInjector/` (its own git repo):
  - `GDPAnomalyInjector.uplugin` — Runtime module, `LoadingPhase Default`, `EnabledByDefault: true`.
  - `Source/GDPAnomalyInjector/GDPAnomalyInjector.Build.cs` — deps `Core`, `CoreUObject`, `Engine`.
  - `Public/GDPAnomalyInjectorLog.h` — `LogGDPAnomaly` category.
  - `Public/GDPAnomalyInjectorSubsystem.h` / `Private/GDPAnomalyInjectorSubsystem.cpp` —
    `UGDPAnomalyInjectorSubsystem : UTickableWorldSubsystem`. Lifecycle
    (`Initialize`/`Deinitialize`), `Tick` heartbeat (on-screen every ~2s + Verbose log),
    `GetStatId`, `DoesSupportWorldType` (Game + PIE only), and ops `ListActors` /
    `HideActorsMatching` / `ShowAllHidden`.
  - `Private/GDPAnomalyInjectorModule.cpp` — module boilerplate + `DEFINE_LOG_CATEGORY`.
  - Console commands `GDP.ListActors`, `GDP.HideActor <substring>`, `GDP.ShowAllActors`
    (module-scoped `FAutoConsoleCommandWithWorldAndArgs`, resolve subsystem from the world).
- **Made StackOBot a code project (Route A)** — host scaffolding outside the plugin:
  `Source/StackOBot/{StackOBot.Build.cs,StackOBot.h,StackOBot.cpp}`,
  `Source/StackOBot.Target.cs`, `Source/StackOBotEditor.Target.cs`, and a `Modules[]` entry
  in `StackOBot.uproject`.
- **Docs scaffolded:** plugin `CLAUDE.md`, `docs/onboarding.md`, `docs/setup-runbook.md`,
  `docs/gotchas.md` (G1–G7), this journal, plugin `.gitignore`, and a thin root `CLAUDE.md`
  pointer at the project root.
- **Built** `StackOBotEditor / Win64 / Development` against source UE 5.4.4 — **clean, exit 0**.

## Problem → Resolution
- **P:** StackOBot had stale `UnrealEditor-StackOBot.dll` + `StackOBotEditor.target` with no
  matching source → would trigger a "missing modules, rebuild?" prompt.
  **R:** Deleted project-root `Binaries/` + `Intermediate/`; Route A re-creates valid modules.
  Recorded as gotcha G1 and runbook step 1.
- **P:** `UTickableWorldSubsystem::GetStatId()` is pure-virtual → won't compile if omitted.
  **R:** Implemented via `RETURN_QUICK_DECLARE_CYCLE_STAT`. Gotcha G5.
- **P:** Bot is runtime-spawned, so it isn't a reliable smoke target at PIE start.
  **R:** Runbook step 6 designates a persistent level prop as primary target, Bot secondary.
  Gotcha G4.

## Deviations from plan
- Added per chat-Claude's instruction: `DoesSupportWorldType` restricting the subsystem to
  `EWorldType::Game` + `EWorldType::PIE` (was flagged as optional in the plan). Recorded as
  invariant in CLAUDE.md and gotcha G7. Flagging the exact `EWorldType` set used (Game + PIE)
  per the request.
- Session journal dated 2026-06-09 (implementation day) rather than the 2026-06-08 placeholder
  in the plan.

## State (stage gate)
| Gate | Status |
|---|---|
| Compiles Development Editor, clean | ✅ Verified — `Build.bat` exit 0; both DLLs produced. |
| Plugin loads in StackOBot; heartbeat visible in PIE | ⏳ Pending interactive PIE run. |
| `GDP.ListActors` prints a sane list | ⏳ Pending interactive PIE run. |
| `GDP.HideActor` makes a visible object vanish | ⏳ Pending interactive PIE run. |
| `GDP.ShowAllActors` restores it | ⏳ Pending interactive PIE run. |
| Docs present | ✅ CLAUDE.md + onboarding + runbook + gotchas + this journal. |

Build log saved at `Saved/GDP_M0_build.log`. Outputs:
`Binaries/Win64/UnrealEditor-StackOBot.dll`,
`Plugins/GDPAnomalyInjector/Binaries/Win64/UnrealEditor-GDPAnomalyInjector.dll`.

## Hand-off
- The compile gate and docs gate are objectively met. The four runtime gates need an
  interactive PIE session (the heartbeat renders on-screen; the hide/restore is visual) — they
  can't be closed headlessly. Steps are in `docs/setup-runbook.md` §5–§6.
- **To verify:** open `StackOBot.uproject` (rebuild modules if prompted), open
  `Content/StackOBot/Maps/MainWorld`, press Play, watch for the green
  `[GDP] AnomalyInjector ticking` heartbeat, then run `GDP.ListActors`, pick a persistent prop
  and `GDP.HideActor <substring>`, then `GDP.ShowAllActors`.
- **Next milestone (M1, not started):** once a few concrete anomalies exist, factor the
  hardcoded hide into an anomaly abstraction/registry. Keep the plugin game-agnostic.
