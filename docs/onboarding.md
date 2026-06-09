# Onboarding

## What this is
`GDPAnomalyInjector` is a UE5 plugin that injects **labeled visual anomalies** (graphics
bugs) into UE5 games to produce synthetic training data for bug-detection ML. It is
game-agnostic and tested on the Stack O Bot sample. See [../CLAUDE.md](../CLAUDE.md) for
the full project framing, the **Current status**, and the locked architecture. The living
current-state design + anomaly catalog is [architecture.md](architecture.md).

## How the work is run
- **Two-Claude split.** An orchestrating "chat Claude" makes design decisions; Kavin
  (project owner) ferries them to the implementing Claude, which writes the code. When the
  implementer hits a real design fork, it stops and surfaces the ambiguity rather than
  guessing.
- **Plan-before-code.** Each milestone starts with a file-by-file plan that must be approved
  before any code is written.
- **Doc discipline.** Each session produces a journal in `docs/sessions/`. The setup runbook
  and architecture.md are kept current as living documents; gotchas are append-only. The full
  maintenance protocol (what every milestone must update) is in `../CLAUDE.md`.

## Where things live
```
Plugins/GDPAnomalyInjector/            <- this plugin = its own git repo, single source of truth
├─ GDPAnomalyInjector.uplugin          <- Runtime module, LoadingPhase Default, EnabledByDefault
├─ CLAUDE.md                           <- canonical context + Current status (read first)
├─ docs/                               <- onboarding, architecture, runbook, gotchas, session journals
└─ Source/GDPAnomalyInjector/
   ├─ GDPAnomalyInjector.Build.cs      <- deps: Core, CoreUObject, Engine
   ├─ Public/
   │  ├─ GDPAnomalyInjectorLog.h       <- LogGDPAnomaly category
   │  ├─ IGDPAnomaly.h                 <- the locked anomaly interface (plain C++, not a UCLASS)
   │  ├─ GDPTargeting.h                <- FindActorsMatching (shared label-free targeting)
   │  └─ GDPAnomalyInjectorSubsystem.h <- UGDPAnomalyInjectorSubsystem (manager + registry)
   └─ Private/
      ├─ GDPAnomalyInjectorModule.cpp  <- module boilerplate + log category definition
      ├─ GDPTargeting.cpp
      ├─ GDPAnomalyInjectorSubsystem.cpp <- lifecycle, heartbeat, registry, dispatch, console commands
      └─ Anomalies/                    <- one IGDPAnomaly impl per file
         ├─ GDPAnomaly_MissingObject.{h,cpp}   <- static, actor-scoped
         ├─ GDPAnomaly_Flicker.{h,cpp}         <- ticking, actor-scoped
         └─ GDPAnomaly_TimeDilation.{h,cpp}    <- world-global, no tick
```
Host-only scaffolding lives **outside** the plugin, in the StackOBot project:
`Source/StackOBot/` (minimal game module) + `Source/*.Target.cs`. This is what makes the
otherwise Blueprint-only StackOBot a buildable code project; it is not part of the plugin.

## The control surface (console commands)
Open the console in PIE (`` ` `` backtick) and run:
- `GDP.ListActors` — log every actor as `Class | Name | Label` (targeting aid).
- `GDP.ListAnomalies` — list registered anomalies as `id - description - usage`.
- `GDP.Apply <id> <args...>` — apply an anomaly, e.g. `GDP.Apply missing_object SatelliteDish`,
  `GDP.Apply flicker SatelliteDish 3`, `GDP.Apply time_dilation 0.2`.
- `GDP.Revert <id>` — revert one anomaly.
- `GDP.RevertAll` — revert all active anomalies (also runs automatically on world teardown).

Output goes to the **Output Log** under the `LogGDPAnomaly` category. The tick heartbeat shows
on-screen (green) as `[GDP] AnomalyInjector ticking (active: N/Total)` and proves the subsystem
is alive. The full anomaly catalog is in [architecture.md](architecture.md).

## Build & run in one screen
See [setup-runbook.md](setup-runbook.md) for the authoritative steps. Short version:
1. Clean stale project-root `Binaries/` + `Intermediate/` if present.
2. Build `StackOBotEditor / Development / Win64` against the source engine.
3. Open `StackOBot.uproject`, confirm the plugin is enabled, load `MainWorld`, press Play.
4. Smoke-test with the `GDP.*` commands above (start with `GDP.ListAnomalies`).
