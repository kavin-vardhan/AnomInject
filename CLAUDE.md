# GDPAnomalyInjector — canonical context

Personal research project **"GDP: Anomaly Injection"** (intrusive UE5 track). This is a
UE5 plugin that injects **labeled visual anomalies** (graphics bugs — missing objects,
lighting mismatch, LOD corruption, flicker, etc.) into UE5 games, to generate synthetic
training data for bug-detection ML. It is **game-agnostic** (public UE APIs only) and is
tested on Stack O Bot. A separate non-intrusive tool exists elsewhere and is out of scope.

This file is the single source of truth. The folder it lives in is its own git repository.

## Docs (read in this order)
- [docs/onboarding.md](docs/onboarding.md) — what this is, how the work is run, where things live.
- [docs/setup-runbook.md](docs/setup-runbook.md) — **living** recipe to replicate the build/run from scratch.
- [docs/gotchas.md](docs/gotchas.md) — **append-only** non-obvious lessons.
- [docs/sessions/](docs/sessions/) — one journal per session: `YYYY-MM-DD-NNN-slug.md`.

## Environment
- Engine: **source-built UE 5.4.4** at `D:\UESource\UnrealEngine` (registered to the
  `.uproject`'s `EngineAssociation` GUID under `HKCU\Software\Epic Games\Unreal Engine\Builds`).
- Host project: **StackOBot** at `D:\Unreal Projects\StackOBot` (note the space in the path).
- Plugin in-tree at `D:\Unreal Projects\StackOBot\Plugins\GDPAnomalyInjector\`.
- Windows, MSVC. Build target: **StackOBotEditor / Development / Win64**.

## Locked architecture (M0)
- One **Runtime** module `GDPAnomalyInjector`, `LoadingPhase = Default`.
- Build.cs deps: `Core`, `CoreUObject`, `Engine` (later: `Renderer`, `RenderCore`, `RHI`,
  `Slate`, `InputCore`).
- Core injector = a `UTickableWorldSubsystem` (`UGDPAnomalyInjectorSubsystem`) — auto-ticks,
  world-scoped, gives `GetWorld()`. Restricted to **Game + PIE** worlds via
  `DoesSupportWorldType` (never the editor preview world).
- Control surface = console commands via `FAutoConsoleCommandWithWorldAndArgs`
  (`GDP.ListActors`, `GDP.HideActor <substring>`, `GDP.ShowAllActors`). Commands are
  module-scoped and resolve the subsystem from the world the console passes in.
- M0 is deliberately tiny: ONE hardcoded anomaly (hide an actor). **No anomaly
  abstraction/registry yet** — that gets factored once we have several concrete anomalies.

## Invariants (do not violate)
- **Plugin stays game-agnostic.** The `GDPAnomalyInjector` module may depend only on
  `Core`/`CoreUObject`/`Engine` (later `Renderer`/`RenderCore`/`RHI`/`Slate`/`InputCore`)
  and must **never `#include` or reference host game-module types** (e.g. anything from the
  `StackOBot` module). Host-specific buildability lives in the project, never in the plugin.
- **Matching is label-free.** `GDP.HideActor` matches by actor Name or Class only.
  `GetActorLabel()` is editor-only and absent in cooked builds — `ListActors` may print the
  label (guarded by `WITH_EDITOR`) but nothing matches on it.

## Workflow rules
- **Two-Claude split.** Design decisions come from an orchestrating "chat Claude" and are
  ferried by Kavin (project owner). The implementing Claude implements. Genuine design forks
  or ambiguities are surfaced back (listed standalone), not improvised.
- **Plan-before-code.** A new milestone's first response is a file-by-file plan only; no
  implementation until approved.
- **Doc discipline.** Every session ends with a journal entry under `docs/sessions/`. The
  setup-runbook is living; gotchas is append-only.
