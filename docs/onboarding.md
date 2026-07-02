# Onboarding

## What this is
`AnomalyInjector` is a UE5 plugin that injects **labeled visual anomalies** (graphics
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
Plugins/AnomalyInjector/            <- this plugin = its own git repo, single source of truth
├─ AnomalyInjector.uplugin          <- Runtime module, LoadingPhase Default, EnabledByDefault
├─ CLAUDE.md                           <- canonical context + Current status (read first)
├─ docs/                               <- onboarding, architecture, runbook, gotchas, session journals
└─ Source/AnomalyInjector/
   ├─ AnomalyInjector.Build.cs      <- deps: Core, CoreUObject, Engine, InputCore (InputCore = m5: FKey/EKeys; no Slate/UMG; m6 added none)
   ├─ Public/
   │  ├─ AnomalyInjectorLog.h       <- LogAnomaly category
   │  ├─ IAnomaly.h                 <- the locked anomaly interface (plain C++, not a UCLASS)
   │  ├─ AnomalyTargeting.h                <- FindActorsMatching (+ "=" exact-match sentinel) + FindComponentsMatching<T> (A1)
   │  ├─ AnomalyArgs.h                     <- GetFloat/GetInt/GetString (A3, shared arg parse/clamp/warn)
   │  ├─ AnomalyLod.h                      <- forced-LOD dispatch over static+skeletal meshes (M3, shared LOD helper)
   │  ├─ AnomalyViewport.h                 <- frustum+occlusion visibility tests over an explicit view (viewport milestone)
   │  ├─ AnomalyInjectorSubsystem.h <- UAnomalyInjectorSubsystem (manager + registry + viewport-scoping toggle)
   │  ├─ AnomalySelectorSubsystem.h <- UAnomalySelectorSubsystem (m5: select visible object + inject UI; separate subsystem)
   │  └─ AnomalyAutoInjectorSubsystem.h <- UAnomalyAutoInjectorSubsystem (m6: auto-fire on-screen + auto-revert; separate subsystem)
   └─ Private/
      ├─ AnomalyInjectorModule.cpp  <- module boilerplate + log category definition
      ├─ AnomalyTargeting.cpp
      ├─ AnomalyArgs.cpp
      ├─ AnomalyLod.cpp                    <- AnomalyLod impl (Cast<>-dispatched static/skinned forced-LOD)
      ├─ AnomalyViewport.cpp               <- AnomalyViewport impl (reversed-Z frustum + line-trace occlusion + live resolver)
      ├─ AnomalyInjectorSubsystem.cpp <- lifecycle, heartbeat, registry, dispatch, console commands, viewport-scoping toggle
      ├─ AnomalySelectorSubsystem.cpp <- m5: selection model + raw input poll + immediate-mode HUD + IAI.Selector.* commands
      ├─ AnomalyAutoInjectorSubsystem.cpp <- m6: seeded scheduler (AdvanceTime/TryFireOnce) + raw input poll + HUD + IAI.Auto.* commands
      └─ Anomalies/                    <- one IAnomaly impl per file (7 anomalies)
         ├─ Anomaly_MissingObject.{h,cpp}   <- static, actor-scoped
         ├─ Anomaly_Blinking.{h,cpp}        <- ticking, actor-scoped
         ├─ Anomaly_TimeDilation.{h,cpp}    <- world-global, no tick
         ├─ Anomaly_LightingMismatch.{h,cpp}<- component-scoped (lights), per-target capture
         ├─ Anomaly_LodCorruption.{h,cpp}   <- component-scoped (static + skeletal mesh), forced-LOD (AnomalyLod)
         ├─ Anomaly_LodPopping.{h,cpp}      <- ticking, component-scoped (static + skeletal), LOD pop (AnomalyLod)
         └─ Anomaly_CameraClipping.{h,cpp}  <- global near-clip, console-command capture/restore
```
Host-only scaffolding lives **outside** the plugin, in the StackOBot project:
`Source/StackOBot/` (minimal game module) + `Source/*.Target.cs`. This is what makes the
otherwise Blueprint-only StackOBot a buildable code project; it is not part of the plugin.

## The control surface (console commands)
Open the console in PIE (`` ` `` backtick) and run:
- `IAI.ListActors` — log every actor as `Class | Name | Label` (targeting aid).
- `IAI.ListAnomalies` — list registered anomalies as `id - description - usage`.
- `IAI.Apply <id> <args...>` — apply an anomaly, e.g. `IAI.Apply missing_object SatelliteDish`,
  `IAI.Apply blinking SatelliteDish 3`, `IAI.Apply time_dilation 0.2`,
  `IAI.Apply lighting_mismatch Light recolor 1 0 1`, `IAI.Apply lod_corruption Bot` (static or skeletal),
  `IAI.Apply lod_popping Foliage 2`, `IAI.Apply camera_clipping 100`.
- `IAI.Revert <id>` — revert one anomaly.
- `IAI.RevertAll` — revert all active anomalies (also runs automatically on world teardown).
- `IAI.SetViewportScoping <0|1>` — opt-in: scope the four object-scoped anomalies (`missing_object`,
  `blinking`, `lod_corruption`, `lod_popping`) to objects visible in the player's viewport (default OFF).
- `IAI.TestVisibility <substring> <ox oy oz> <pitch yaw roll> [fov] [aspect]` — diagnostic for the
  viewport core against a synthetic view (logs per-component frustum/occlusion/visible).
- **Object Selector + Inject UI (m5):** `IAI.SelectorUI <0|1>` (default OFF) turns on an in-game overlay to
  **select a visible object and inject an anomaly on it**. Real-Play keys: Tab/Shift+Tab cycle the visible object,
  C cycles the anomaly, G injects, H reverts (rebindable via `IAI.SelectorBind`). The bridge drives the same model
  via `IAI.Selector.Next/Prev/Cycle/Inject/Revert` + `IAI.Selector.Status`. See the runbook §6a.
- **Automatic Injection (m6):** `IAI.Auto.Enable <0|1>` (default OFF) shows the auto-injector overlay; `IAI.Auto.Run <0|1>`
  (key J) starts/stops **auto-firing the four object-scoped anomalies randomly on on-screen objects**, each auto-reverting
  after a randomized hold (one anomaly per actor; one seeded `FRandomStream`). Real-Play keys 1-4 toggle the types, K reseeds.
  The bridge drives the deterministic core via `IAI.Auto.Seed`/`IAI.Auto.FireOnce`/`IAI.Auto.Step` + `IAI.Auto.Status`
  (no real time needed). Cadence: `IAI.Auto.Interval/Hold/MaxConcurrent/Persist`. See the runbook §6b.

Output goes to the **Output Log** under the `LogAnomaly` category. The tick heartbeat shows
on-screen (green) as `[IAI] AnomalyInjector ticking (active: N/Total)` and proves the subsystem
is alive. The full anomaly catalog is in [architecture.md](architecture.md).

## Build & run in one screen
See [setup-runbook.md](setup-runbook.md) for the authoritative steps. Short version:
1. Clean stale project-root `Binaries/` + `Intermediate/` if present.
2. Build `StackOBotEditor / Development / Win64` against the source engine.
3. Open `StackOBot.uproject`, confirm the plugin is enabled, load `MainWorld`, press Play.
4. Smoke-test with the `IAI.*` commands above (start with `IAI.ListAnomalies`).
