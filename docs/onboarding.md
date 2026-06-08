# Onboarding

## What this is
`GDPAnomalyInjector` is a UE5 plugin that injects **labeled visual anomalies** (graphics
bugs) into UE5 games to produce synthetic training data for bug-detection ML. It is
game-agnostic and tested on the Stack O Bot sample. See [../CLAUDE.md](../CLAUDE.md) for
the full project framing and the locked architecture.

## How the work is run
- **Two-Claude split.** An orchestrating "chat Claude" makes design decisions; Kavin
  (project owner) ferries them to the implementing Claude, which writes the code. When the
  implementer hits a real design fork, it stops and surfaces the ambiguity rather than
  guessing.
- **Plan-before-code.** Each milestone starts with a file-by-file plan that must be approved
  before any code is written.
- **Doc discipline.** Each session produces a journal in `docs/sessions/`. The setup runbook
  is kept current as a living document; gotchas are append-only.

## Where things live
```
Plugins/GDPAnomalyInjector/            <- this plugin = its own git repo, single source of truth
├─ GDPAnomalyInjector.uplugin          <- Runtime module, LoadingPhase Default, EnabledByDefault
├─ CLAUDE.md                           <- canonical context (read first)
├─ docs/                               <- onboarding, runbook, gotchas, session journals
└─ Source/GDPAnomalyInjector/
   ├─ GDPAnomalyInjector.Build.cs      <- deps: Core, CoreUObject, Engine
   ├─ Public/
   │  ├─ GDPAnomalyInjectorLog.h       <- LogGDPAnomaly category
   │  └─ GDPAnomalyInjectorSubsystem.h <- UGDPAnomalyInjectorSubsystem (UTickableWorldSubsystem)
   └─ Private/
      ├─ GDPAnomalyInjectorModule.cpp  <- module boilerplate + log category definition
      └─ GDPAnomalyInjectorSubsystem.cpp <- lifecycle, heartbeat, ops, console commands
```
Host-only scaffolding lives **outside** the plugin, in the StackOBot project:
`Source/StackOBot/` (minimal game module) + `Source/*.Target.cs`. This is what makes the
otherwise Blueprint-only StackOBot a buildable code project; it is not part of the plugin.

## The control surface (console commands)
Open the console in PIE (`` ` `` backtick) and run:
- `GDP.ListActors` — log every actor as `Class | Name | Label`.
- `GDP.HideActor <substring>` — `SetActorHiddenInGame(true)` on actors whose Name or Class
  contains `<substring>` (case-insensitive). Tracked for restore.
- `GDP.ShowAllActors` — restore everything we hid.

Output goes to the **Output Log** under the `LogGDPAnomaly` category. The tick heartbeat
shows on-screen (green) and proves the subsystem is alive.

## Build & run in one screen
See [setup-runbook.md](setup-runbook.md) for the authoritative steps. Short version:
1. Clean stale project-root `Binaries/` + `Intermediate/` if present.
2. Build `StackOBotEditor / Development / Win64` against the source engine.
3. Open `StackOBot.uproject`, confirm the plugin is enabled, load `MainWorld`, press Play.
4. Smoke-test with the three `GDP.*` commands above.
