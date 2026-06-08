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
- A green **`[GDP] AnomalyInjector ticking (hidden: N)`** heartbeat appears on-screen — proves
  the subsystem initialized and ticks in PIE.

## 6. Smoke test (stage gate)
Open the console in PIE (press `` ` `` backtick) and run:
1. `GDP.ListActors` — Output Log (category `LogGDPAnomaly`) prints `Class | Name | Label` for
   every actor. Sanity-check it lists the level's props.
2. `GDP.HideActor <substring>` — pick a **persistent level prop** from the list as the primary
   smoke target (a visible `StaticMeshActor` in `MainWorld`, e.g. part of one of the
   `SM_*`/`BPP_Struct_*` placements). The matched object vanishes in the viewport.
   - **Note:** the Bot character is **runtime-spawned**, so `GDP.HideActor Bot` only matches
     after the Bot exists in the world (after it spawns). Use it as a secondary target.
3. `GDP.ShowAllActors` — the hidden object reappears.

## Troubleshooting
- **"The following modules are missing or built with a different engine version… rebuild?"** —
  expected if `Binaries/` is stale or absent. Click **Yes**, or run step 4 first.
- **Plugin not listed / not loaded** — confirm `EnabledByDefault: true` in the `.uplugin` and
  that `UnrealEditor-GDPAnomalyInjector.dll` exists under `Binaries\Win64` after the build.
- **Commands print "subsystem not present"** — you ran them outside a Game/PIE world. The
  subsystem only exists in Game + PIE (by design); run the commands inside PIE.
