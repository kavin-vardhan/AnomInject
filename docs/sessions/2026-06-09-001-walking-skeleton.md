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
- **MCP bridge wired into StackOBot to drive the functional smoke.** To verify the runtime
  gates over the `unreal-mcpython` MCP, the `UnrealMCPython` editor plugin (from the RatBurglar
  project) was copied into `StackOBot/Plugins/unreal-mcp/` (host tooling — NOT part of the GDP
  plugin repo), enabled + Python enabled in the `.uproject`, and **patched to build on 5.4**
  (its descriptor targets 5.7; one unexported BehaviorTree editor node caused a single LNK2019).
  See gotcha G8. This is host infrastructure; the GDP plugin and its repo are unaffected.

## State (stage gate) — ALL PASS
| Gate | Status |
|---|---|
| Compiles Development Editor, clean | ✅ `Build.bat` exit 0; both DLLs produced. |
| Plugin loads in StackOBot; heartbeat visible in PIE | ✅ Log: `Mounting … GDPAnomalyInjector` → `module started.` → `Subsystem initialized for world 'MainWorld'.`; `Heartbeat; hidden actors: N` every 2 s. Green on-screen text eyeball-confirmed by owner. |
| `GDP.ListActors` prints a sane list | ✅ `--- 434 actor(s) ---` against the PIE world. |
| `GDP.HideActor` makes a visible object vanish | ✅ Hid the satellite dish (`StaticMeshActor_137/138/139`, SM_SatelliteDish Base/Body/Dish @ XY≈4252,6558); `hidden` flag flipped false→true on all 3; owner eyeball-confirmed the vanish. |
| `GDP.ShowAllActors` restores it | ✅ `GDP.ShowAllActors -> restored 3 actor(s)`; flags true→false; owner eyeball-confirmed reappear. Auto-restore-on-teardown also proven (`Subsystem deinitializing; restored 3 hidden actor(s).` after an accidental Stop-PIE). |
| Docs present | ✅ CLAUDE.md + onboarding + runbook + gotchas + this journal. |

### Functional verification (MCP-driven, 2026-06-09, PIE MainWorld)
Commands were executed against the **PIE game world** (`UnrealEditorSubsystem.get_game_world()`),
never the editor world — confirmed by `Subsystem initialized for world 'MainWorld'` and live
`hidden` before/after reads. Diagnostic layer (load, init, tick, list, hide, restore counts)
read back via `LogGDPAnomaly`; the two pixel-level checks (on-screen heartbeat text, dish
vanish/reappear) confirmed by the owner's eyeball. Split worked exactly as designed, including
the subsystem null-guard never firing because every command hit a valid game world.

Build logs: `Saved/GDP_M0_build.log` (GDP plugin), `Saved/GDP_MCP_build.log` (bridge). Outputs:
`Binaries/Win64/UnrealEditor-StackOBot.dll`,
`Plugins/GDPAnomalyInjector/Binaries/Win64/UnrealEditor-GDPAnomalyInjector.dll`.

## Hand-off
- **M0 stage gate fully passed** — compile, load, heartbeat, list, hide, restore, docs.
- MCP-driven PIE verification is now available for StackOBot: launch the editor (the
  `UnrealMCPython` server starts on `127.0.0.1:12029`), press Play, then commands can be driven
  via the bridge. See gotcha G8 for the 5.4 patch and the revert note for engine upgrades.
- **Next milestone (M1, not started):** once a few concrete anomalies exist, factor the
  hardcoded hide into an anomaly abstraction/registry. Keep the plugin game-agnostic.
