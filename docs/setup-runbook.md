# Setup runbook (living document)

Authoritative, replayable recipe to go from a clean StackOBot checkout to a running PIE
session with the `GDPAnomalyInjector` plugin loaded. Update this whenever the steps change.

Paths assume:
- Source engine: `D:\UESource\UnrealEngine` (UE 5.4.4).
- Host project: `D:\Unreal Projects\StackOBot` (note the space).

---

## 0. One-time facts about this machine
- The `.uproject`'s `EngineAssociation` GUID `{B34F356C-4AE7-256A-F0E1-318A632BB902}` is
  already registered to `D:/UESource/UnrealEngine` under
  `HKCU\Software\Epic Games\Unreal Engine\Builds`. Verify with PowerShell:
  ```powershell
  Get-ItemProperty "HKCU:\Software\Epic Games\Unreal Engine\Builds"
  ```
  If the GUID is missing, re-create the association by right-clicking `StackOBot.uproject`
  → **Switch Unreal Engine Version…** → select the `D:\UESource\UnrealEngine` source build.
- The source engine must already be compiled (`Engine\Binaries\Win64\UnrealEditor.exe`
  exists). If not, build the engine first — otherwise the project build below would try to
  compile the whole engine.

## 1. Clean stale artifacts
StackOBot ships (or accumulates) stale project-root build outputs that reference a `StackOBot`
game module. After we add our own `Source/StackOBot/` module these become valid again, but a
clean start avoids "missing modules, rebuild?" prompts. Delete **only** the project-root
`Binaries/` and `Intermediate/`. Do **not** delete `Saved/`, and never touch engine or plugin
artifacts.
```powershell
Remove-Item -Recurse -Force "D:\Unreal Projects\StackOBot\Binaries"     -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "D:\Unreal Projects\StackOBot\Intermediate" -ErrorAction SilentlyContinue
```

## 2. Project is a code project (Route A)
StackOBot is otherwise Blueprint-only. We make it a representative **code project** by adding a
minimal primary game module — host scaffolding that lives in the project, **never** in the
plugin repo:
- `Source/StackOBot/StackOBot.Build.cs`  (deps: Core, CoreUObject, Engine, InputCore)
- `Source/StackOBot/StackOBot.h`
- `Source/StackOBot/StackOBot.cpp`  (`IMPLEMENT_PRIMARY_GAME_MODULE(...)`)
- `Source/StackOBot.Target.cs`  (Game)
- `Source/StackOBotEditor.Target.cs`  (Editor)
- `StackOBot.uproject` gains a `"Modules": [{ "Name": "StackOBot", ... }]` entry.

The plugin is enabled via `"EnabledByDefault": true` in `GDPAnomalyInjector.uplugin`, so the
`.uproject` needs **no** `Plugins[]` entry for it.

## 3. (Optional) Generate IDE project files
Only needed if you want the `.sln` for Visual Studio / Rider:
```powershell
& "D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat" -projectfiles `
  -project="D:\Unreal Projects\StackOBot\StackOBot.uproject" -game -engine -progress
```
Building from the command line (step 4) does not require this.

## 4. Build the editor target
```powershell
& "D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat" `
  StackOBotEditor Win64 Development `
  -project="D:\Unreal Projects\StackOBot\StackOBot.uproject" -waitmutex
```
A clean compile of just our modules (the engine is prebuilt) is the stage-gate "Compiles
Development Editor, clean." This produces `Binaries\Win64\UnrealEditor-StackOBot.dll` and
`Binaries\Win64\UnrealEditor-GDPAnomalyInjector.dll`.

## 5. Launch + PIE
- Open `D:\Unreal Projects\StackOBot\StackOBot.uproject` (double-click uses the associated
  source engine). If prompted to rebuild modules, allow it.
- Confirm the plugin is enabled: **Edit → Plugins → GDP → GDP Anomaly Injector** (it is on by
  default).
- Open the level `Content/StackOBot/Maps/MainWorld`, press **Play** (PIE).
- A green **`[GDP] AnomalyInjector ticking (active: N/Total)`** heartbeat appears on-screen — proves
  the subsystem initialized and ticks in PIE.

## 6. Smoke test (stage gate)
Open the console in PIE (press `` ` `` backtick) and run:
1. `GDP.ListAnomalies` — Output Log (category `LogGDPAnomaly`) lists **six** anomalies as
   `id - description - usage`, sorted: `camera_clipping`, `flicker`, `lighting_mismatch`,
   `lod_corruption`, `missing_object`, `time_dilation`.
2. `GDP.ListActors` — prints `Class | Name | Label` for every actor; pick a target substring.
3. `GDP.Apply missing_object <substring>` — pick a **persistent level prop** (a visible
   `StaticMeshActor` in `MainWorld`, e.g. an `SM_*`/`BPP_Struct_*` placement). The matched object
   vanishes. (The Bot is **runtime-spawned** — gotcha G4 — so it only matches after it spawns.)
4. `GDP.Apply flicker <substring>` — the matched object visibly flickers (default 5 Hz). Optional
   rate: `GDP.Apply flicker <substring> 2`.
5. `GDP.Apply time_dilation 0.2` — the game slows to ~20% speed; `GDP.Revert time_dilation`
   restores normal speed (to the captured baseline).
6. `GDP.Apply lighting_mismatch <substring> [off|dim <f>|recolor <r g b>|noshadow]` — mismatch the
   lights on matching actors (default `dim` 0.1). e.g. `GDP.Apply lighting_mismatch Light recolor 1 0 1`
   (magenta), `... Light dim 0.05`, `... Light off`, `... Light noshadow`. **Needs a Movable light
   to be visible** (gotcha G14) — `GDP.Revert lighting_mismatch` restores intensity/color/visibility/shadow.
7. `GDP.Apply lod_corruption <substring> [lod-index]` — force matching **static-mesh** components to a
   LOD (default worst/highest). In stock MainWorld use `GDP.Apply lod_corruption SM_Ramp` to validate
   by **state** (`forced_lod 0→1`; ramps are single-LOD so no visual), and `GDP.Apply lod_corruption
   Foliage` for a best-effort visual on the instanced Bush/Tree (the only multi-LOD meshes). There is no
   deterministic *visual* LOD target in stock MainWorld — gotcha G15. `GDP.Revert lod_corruption` restores.
8. `GDP.Apply camera_clipping [near-plane]` — push the near clip plane out (default 100) so near
   geometry clips away; `GDP.Revert camera_clipping` restores the captured baseline (~10). Most
   reliably-visible gate; no targeting.
9. `GDP.RevertAll` — restores everything still active. Stopping PIE also auto-reverts (teardown).

The green on-screen heartbeat reads `[GDP] AnomalyInjector ticking (active: N/Total)`.

## 7. Runtime verification recipe (MCP-driven gate checks)
The non-visual stage gates are closed by driving PIE over the `unreal-mcpython` bridge (host tooling,
gotcha G8) and reading state/logs back; the owner eyeballs the visual gates. This is exactly how M1's
gates 2-7 were verified (2026-06-09) - reuse it for future anomalies.

**Prereqs:** editor open with the plugin built; **press Play in `MainWorld`** (the subsystem is
Game/PIE-only, so there is NO GDP subsystem until PIE is running); the bridge listens on
`127.0.0.1:12029` (it starts with the editor, survives Stop-PIE).

**Core idioms** (via `mcp__unreal-mcpython__util_execute_python`):
- Get the PIE world (NOT the editor world):
  `gw = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()`
- Fire a command: `unreal.SystemLibrary.execute_console_command(gw, "GDP.Apply missing_object SM_Ramp")`
- Read an actor's hidden flag against the PIE world: iterate
  `unreal.GameplayStatics.get_all_actors_of_class(gw, unreal.Actor)`, filter by name substring, read
  `a.get_editor_property('hidden')`. (Editor-world actors are different instances - always read `gw`'s.)
- Read global time dilation: `unreal.GameplayStatics.get_global_time_dilation(gw)`
- Read logs: `mcp__unreal-mcpython__util_get_output_log` with a keyword, e.g. `anomaly`,
  `flicker toggle`, `Heartbeat`, `deinitializing`.

**Gotchas that bit us (save yourself the rediscovery):**
- **flicker toggles + the heartbeat log at Verbose.** First run
  `execute_console_command(gw, "Log LogGDPAnomaly Verbose")`, then grep `flicker toggle`.
- **Never `time.sleep()` in Python** - it blocks the game thread so nothing ticks. For ticking effects,
  let real wall-clock pass *between* MCP calls, then read the accumulated toggle log.
- **PIE can run at a few FPS;** the flicker `while`-drain replays multiple half-periods per frame, so the
  toggle log still advances. (Several toggles sharing one timestamp is expected, not a bug.)
- **Teardown gate:** leave anomalies active, **Stop PIE**, then read the log (editor stays open, bridge
  still up) for `Subsystem deinitializing; reverted N active anomaly(ies).`
- **Target selection:** pick a small, countable, persistent set - M1 used `SM_Ramp` (2 actors). The Bot
  is runtime-spawned (G4), so it only matches after it spawns.

**Gate -> check:**
| gate | drive | assert |
|------|-------|--------|
| ListAnomalies | `GDP.ListAnomalies` | log: **6** lines, sorted, `id - description - usage` |
| missing_object | `GDP.Apply missing_object SM_Ramp` | both ramps `hidden == True`; log `matched 2 actor(s)` |
| flicker | `Log LogGDPAnomaly Verbose`; `GDP.Apply flicker SM_Ramp` | repeating `flicker toggle -> HIDDEN/VISIBLE`; heartbeat `active: 1/N` |
| time_dilation | `GDP.Apply time_dilation 0.2`; `GDP.Revert time_dilation` | dilation `0.2`, then back to the captured baseline |
| lighting_mismatch | find a Movable `ULightComponent` (`Mobility==Movable`); `GDP.Apply lighting_mismatch <sub> recolor 1 0 1` | matched-count >= 1; read component `Intensity`/`GetLightColor()`/`GetVisibleFlag()`/`CastShadows` changed; `Revert` -> all restored. Owner eyeballs the lit change (movable target). |
| lod_corruption | `GDP.Apply lod_corruption SM_Ramp` (state) + `... Foliage` (best-effort visual) | matched-count >= 1; read each `ForcedLodModel` == forced value (log `forced LOD N of M`); `Revert` -> prior (0). No deterministic visual target in stock MainWorld (G15). |
| camera_clipping | `GDP.Apply camera_clipping 100`; `GDP.Revert camera_clipping` | `GNearClippingPlane` == 100, then baseline (~10). Owner eyeballs near geometry vanishing / returning. |
| RevertAll | apply >=2 anomalies, then `GDP.RevertAll` | all `IsActive==false`; captured state restored (hidden flags false, dilation baseline, lights/LOD/near-clip restored) |
| teardown | apply, then **Stop PIE** | log `Subsystem deinitializing; reverted N...`; re-check nothing stuck |
| no-leak | `GDP.Apply <id> A` then `GDP.Apply <id> B` (esp. `lighting_mismatch recolor`) | single capture set; only B's targets active; A's restored (no stuck lights/LODs) |

To read component state via the bridge: get the PIE world, `GameplayStatics.get_all_actors_of_class(gw, unreal.Actor)`,
filter by substring, then iterate `a.get_components_by_class(unreal.LightComponent)` /
`unreal.StaticMeshComponent` and read the property. Near clip: `unreal.SystemLibrary` has no getter — read it
back via the `camera_clipping: near clip X -> Y.` log line (`util_get_output_log` keyword `camera_clipping`).

## Troubleshooting
- **"The following modules are missing or built with a different engine version… rebuild?"** —
  expected if `Binaries/` is stale or absent. Click **Yes**, or run step 4 first.
- **Plugin not listed / not loaded** — confirm `EnabledByDefault: true` in the `.uplugin` and
  that `UnrealEditor-GDPAnomalyInjector.dll` exists under `Binaries\Win64` after the build.
- **Commands print "subsystem not present"** — you ran them outside a Game/PIE world. The
  subsystem only exists in Game + PIE (by design); run the commands inside PIE.
