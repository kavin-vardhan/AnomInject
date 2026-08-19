# Setup runbook (living document)

Authoritative, replayable recipe to go from a clean StackOBot checkout to a running PIE
session with the `AnomalyInjector` plugin loaded. Update this whenever the steps change.

Paths assume:
- Source engine: `D:\UESource\UnrealEngine` (UE **5.1**, Release-5.1 — canonical; originally UE 5.4.4).
- Host project: `D:\IntrusiveAnomalies\StackOBot` (natively-5.1 StackOBot; the old 5.4 host
  `D:\Unreal Projects\StackOBot` is retired).

> **5.1 specifics (M2.5/M2.6):**
> - Host-target build constants are `BuildSettingsVersion.V2` / `EngineIncludeOrderVersion.Unreal5_1`
>   (the 5.4 `V5` / `Unreal5_4` do not exist on 5.1 — gotcha G17). The plugin pins no `CppStandard`
>   (inherits 5.1's C++17). Generate project files / build against the **5.1 source** engine.
> - If the editor throws **"ShaderCompileWorker output version 8, got 20"**, the SCW program is stale
>   from the engine branch switch — rebuild it (gotcha G18) before launching.
> - The `unreal-mcpython` bridge had its `BehaviorTreeEditor` dependency **severed** to build on 5.1
>   (gotcha G8); its 2 BT-authoring tools are unavailable, everything else works.

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
Remove-Item -Recurse -Force "D:\IntrusiveAnomalies\StackOBot\Binaries"     -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "D:\IntrusiveAnomalies\StackOBot\Intermediate" -ErrorAction SilentlyContinue
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

The plugin is enabled via `"EnabledByDefault": true` in `AnomalyInjector.uplugin`, so the
`.uproject` needs **no** `Plugins[]` entry for it.

## 3. (Optional) Generate IDE project files
Only needed if you want the `.sln` for Visual Studio / Rider:
```powershell
& "D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat" -projectfiles `
  -project="D:\IntrusiveAnomalies\StackOBot\StackOBot.uproject" -game -engine -progress
```
Building from the command line (step 4) does not require this.

## 4. Build the editor target
```powershell
& "D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat" `
  StackOBotEditor Win64 Development `
  -project="D:\IntrusiveAnomalies\StackOBot\StackOBot.uproject" -waitmutex
```
A clean compile of just our modules (the engine is prebuilt) is the stage-gate "Compiles
Development Editor, clean." This produces `Binaries\Win64\UnrealEditor-StackOBot.dll` and
`Binaries\Win64\UnrealEditor-AnomalyInjector.dll`.

## 5. Launch + PIE
- Open `D:\IntrusiveAnomalies\StackOBot\StackOBot.uproject` (double-click uses the associated
  source engine). If prompted to rebuild modules, allow it.
- Confirm the plugin is enabled: **Edit → Plugins → Anomaly Injection → Anomaly Injector** (it is on by
  default).
- Open the level `Content/StackOBot/Maps/MainWorld`, press **Play** (PIE).
- A green **`[IAI] AnomalyInjector ticking (active: N/Total)`** heartbeat appears on-screen — proves
  the subsystem initialized and ticks in PIE.

## 6. Smoke test (stage gate)
Open the console in PIE (press `` ` `` backtick) and run:
1. `IAI.ListAnomalies` — Output Log (category `LogAnomaly`) lists **seven** anomalies as
   `id - description - usage`, sorted: `blinking`, `camera_clipping`, `lighting_mismatch`,
   `lod_corruption`, `lod_popping`, `missing_object`, `time_dilation`.
2. `IAI.ListActors` — prints `Class | Name | Label` for every actor; pick a target substring.
3. `IAI.Apply missing_object <substring>` — pick a **persistent level prop** (a visible
   `StaticMeshActor` in `MainWorld`, e.g. an `SM_*`/`BPP_Struct_*` placement). The matched object
   vanishes. (The Bot is **runtime-spawned** — gotcha G4 — so it only matches after it spawns.)
4. `IAI.Apply blinking <substring>` — the matched object visibly blinks (default 5 Hz). Optional
   rate: `IAI.Apply blinking <substring> 2`.
5. `IAI.Apply time_dilation 0.2` — the game slows to ~20% speed; `IAI.Revert time_dilation`
   restores normal speed (to the captured baseline).
6. `IAI.Apply lighting_mismatch <substring> [off|dim <f>|recolor <r g b>|noshadow]` — mismatch the
   lights on matching actors (default `dim` 0.1). e.g. `IAI.Apply lighting_mismatch Light recolor 1 0 1`
   (magenta), `... Light dim 0.05`, `... Light off`, `... Light noshadow`. **Needs a Movable light
   to be visible** (gotcha G14) — `IAI.Revert lighting_mismatch` restores intensity/color/visibility/shadow.
7. `IAI.Apply lod_corruption <substring> [lod-index]` — force matching **static or skeletal** mesh
   components to a LOD (default worst/highest per component). In stock MainWorld use
   `IAI.Apply lod_corruption SM_Ramp` to validate by **state** (`forced_lod 0→1`; ramps are single-LOD so
   no visual), `IAI.Apply lod_corruption Foliage` for a best-effort visual on the instanced Bush/Tree (the
   only multi-LOD meshes), and — after the Bot spawns (Play) — `IAI.Apply lod_corruption Bot` to hit the
   skeletal Bot (1 static + 2 skinned components in one apply; the Bot is single-LOD → state-only, G20).
   There is no deterministic *visual* LOD target in stock MainWorld — gotchas G15/G20. `IAI.Revert lod_corruption` restores.
8. `IAI.Apply lod_popping <substring> [hz]` — **ticking**: snap matching static/skeletal components between
   their baseline and worst LOD each half-period (default 2 Hz, clamp ≤ 30). e.g. `IAI.Apply lod_popping
   Foliage` (best-effort visual pop on Bush/Tree) or `IAI.Apply lod_popping Bot 2` (skeletal, state-only).
   `IAI.Revert lod_popping` restores the captured baseline regardless of phase. Same visual caveat (G15/G20).
9. `IAI.Apply camera_clipping [near-plane]` — push the near clip plane out (default 100) so near
   geometry clips away; `IAI.Revert camera_clipping` restores the captured baseline (~10). Most
   reliably-visible gate; no targeting.
10. `IAI.RevertAll` — restores everything still active. Stopping PIE also auto-reverts (teardown).
11. `IAI.SetViewportScoping 1` — opt-in viewport scoping (default OFF). Now `IAI.Apply missing_object <sub>`
    (and `blinking` / `lod_corruption` / `lod_popping`) affects only matches **visible in the player's view** —
    aim away from a matched object and it is left untouched; aim at it and it is affected. `IAI.SetViewportScoping 0`
    restores the unscoped behavior. The heartbeat shows `scoping: ON/OFF`. (Diagnostic: `IAI.TestVisibility <sub>
    <ox oy oz> <pitch yaw roll> [fov] [aspect]` logs per-component `frustum/unoccluded/visible` for a synthetic view.)
12. `IAI.Apply missing_texture <substring>` — swap every renderable **static/skeletal** mesh slot on matching actors to
    the shipped **Lit gray/white UV-checker** material (the "missing texture" look; per-component override = object
    isolation, never mutates the shared mesh/material asset). e.g. `IAI.Apply missing_texture SM_Ramp` recolors the ramps;
    `IAI.Revert missing_texture` restores each slot exactly. **No args** (one look for now; the flat-magenta variant + a
    `mode` arg are deferred — gotcha G50). The material renders correctly on Nanite/skeletal meshes (usage flags — G49).
12. `IAI.SetPollRadius <cm>` — opt-in **distance cull** (default OFF) on the renderable-visible set: only renderable
    actors within `<cm>` of the **player pawn** are offered to the selector / auto-injector / dashboard. `<= 0`
    disables it (byte-identical to no cull); no argument prints the current radius. When set, a yellow debug sphere of
    that radius is drawn around the pawn. Independent of `IAI.SetViewportScoping` (this culls the renderable-visible
    *set*; scoping gates the *console finders*). See gotcha G34.

The green on-screen heartbeat reads `[IAI] AnomalyInjector ticking (active: N/Total, scoping: ON/OFF)`.

### 6a. Object Selector + Inject UI (m5)
The selector lets you **pick a visible on-screen object and inject an anomaly on it**, then revert — an interactive
front-end over the m4 visible set. It is a separate subsystem; activation is opt-in (default OFF).
1. `IAI.SelectorUI 1` — turn the UI on. An overlay appears (top-left): a list of currently-**visible** actor names,
   a list of the four injectable anomalies, plus a yellow box + name label on the selected object.
2. **Keys (real Play):** **Tab** = next object, **Shift+Tab** = previous, **C** = cycle anomaly,
   **G** = inject on the selected object, **H** = revert. Tab cycles only objects the player can actually see —
   in-frustum, unoccluded, **and renderable** (static / skeletal mesh; **VFX excluded — G33**). Non-rendering actors
   (volumes, spawn points, debug/streaming actors, landscape) and particle/VFX actors are excluded, so cycling never
   stops on them. Selection is name-sorted (alphabetical) in v1.
   A **"Last:"** line on the HUD reports the result of your last inject/revert — including "0 matched" when a combo
   doesn't apply (e.g. an LOD anomaly that resolves no mesh component on the selected actor).
3. Pick an object with Tab, choose an anomaly with C, press **G** — the selected object is affected; press **H** to revert.
4. `IAI.SelectorUI 0` — turn it off (dormant; everything else is byte-identical to before).
- **Rebind** any key to escape a collision: `IAI.SelectorBind <next|prev|cycle|inject|revert> <KeyName>`
  (e.g. `IAI.SelectorBind inject F` ; key names are UE `EKeys` names like `Tab`, `C`, `F1`, `RightMouseButton`).
- **Three usability facts to know:**
  - **One object per anomaly type at a time.** Injecting the same anomaly id on a second object reverts-then-reapplies
    (one registry instance per id) — the first object reappears. Use different anomaly types to mark several objects at once.
  - **Cycle order is alphabetical (name-sorted)** in v1 (deterministic for the bridge gate). Spatial left-to-right
    ordering is the intended next UX polish.
  - **Keep `IAI.SetViewportScoping 0` while using the selector.** The selector is already self-scoping (you pick from the
    visible set). Running global scoping ON adds a redundant visibility re-test on inject that can *drop* the target if it
    became occluded in the sub-second between select and inject.
- **Steam-overlay caveat:** in a Steam-launched build the Steam overlay grabs **Shift+Tab**; it is fine in PIE/standalone.
  Rebind `prev` to a dedicated key to escape it (gotcha G26).

### 6b. Automatic Injection (m6)
The auto-injector fires the four object-scoped anomalies **randomly on the renderable objects currently on-screen**,
each auto-reverting after a randomized hold. Separate subsystem; two switches, both default OFF.
1. `IAI.Auto.Enable 1` — show the auto-injector overlay (right side): the four **types** (1-4) with on/off, the seed +
   cadence, and the live fires (`id -> target  (Ns)`).
2. **Pick types** (default: all four on) — keys **1/2/3/4** toggle `missing_object` / `blinking` / `lod_corruption` /
   `lod_popping`; or `IAI.Auto.Pool <id|all> <0|1>`.
3. `IAI.Auto.Run 1` (or key **J**) — start firing. Anomalies appear on on-screen objects at a random interval
   (default [4,9]s), each on a **distinct** actor (one anomaly per actor), and auto-revert after a random hold
   (default [3,6]s). `IAI.Auto.Run 0` (J) stops + reverts all live fires.
4. Tune live: `IAI.Auto.Interval <min> <max>`, `IAI.Auto.Hold <min> <max>`, `IAI.Auto.MaxConcurrent <n>`,
   `IAI.Auto.Persist <0|1>` (off = auto-revert), `IAI.Auto.Seed <int>` (or key **K** to reseed), `IAI.Auto.Status`.
5. `IAI.Auto.Enable 0` — dormant (everything else byte-identical).
- **Deterministic drive (no real time / no Enable/Run needed):** `IAI.Auto.Seed <int>` then `IAI.Auto.FireOnce`
  (one attempt) or `IAI.Auto.Step <seconds>` (advance the scheduler). This is the bridge state-gate entry point.
- **Keep `IAI.SetViewportScoping 0`** while running (it is self-scoping; a Run-start warning fires if scoping is ON).
- **Do not run the selector and the auto-injector at once** — unsupported; a warning fires if both are enabled.
- Keys `1-4`/`J`/`K` are rebindable: `IAI.Auto.Bind <pool1|pool2|pool3|pool4|run|reseed> <KeyName>`.

### 6c. Labeled frame-capture (m7) — `IAI.Capture.*`
Produces a labeled image sequence (`frame_<GFrameCounter>.png` + `labels.jsonl` + `run.json`) from a LIVE
auto-injection run. **Must be real Play** (Simulate has no game viewport for `ReadPixels`). The capture
subsystem lives in the `AnomalyControlServer` module (present in Development/Test; compiled out of Shipping).
1. **Narrow the fired types** (StackOBot: LOD anomalies are visually null — G15/G20 — so capture only the visual
   ones): `IAI.Auto.Pool all 0` → `IAI.Auto.Pool missing_object 1` → `IAI.Auto.Pool blinking 1`.
2. Ensure the auto-injector's own loop is OFF: `IAI.Auto.Run 0` (capture owns firing; a warning fires if Run is on).
3. **Single labeled frame:** `IAI.Auto.FireOnce` then `IAI.Capture.Shot` → one PNG + one JSONL record under
   `<ProjectSaved>/AnomalyCaptures/manual/` (Gate 1).
4. **Burst run:** `IAI.Auto.Seed <int>` → `IAI.Capture.Config <K> <pre> <positive> <post> <bursts>` (e.g.
   `2 5 10 5 3`) → `IAI.Capture.Start`. It runs deterministically and auto-stops after `<bursts>` (or
   `IAI.Capture.Stop` for a `0`/until-stop run). Output: `<ProjectSaved>/AnomalyCaptures/session_<stamp>/`
   (seed is in `run.json`; a same-second second run gets `-2/-3/...`).
4b. **Targeted vs auto-pool (m10):** full usage is
   `IAI.Capture.Start [outDir] [png|jpeg] [seed] [maxFrames] [anomaly] [targetActor]`. Pass BOTH trailing
   args for a **targeted** run — every burst fires exactly that anomaly on exactly that actor (visibility-
   independent, `=` exact-match; works for non-pool ids like `lod_popping` too); omit both for **auto-pool**.
   Use `""` placeholders to skip leading args, e.g.
   `IAI.Capture.Start "" png "" 60 blinking SM_Ramp3_UAID_..._2086822138` (G60). Only one of the two set →
   warning + auto-pool fallback. StartRun pauses the auto-injector's Run and resumes it on finish (both
   entry points), and reverts ALL active anomalies first (manual injects included — clean slate, G63).
   `run.json` records `mode`/`target_anomaly`/`target_actor`.
5. **Under camera motion:** walk/turn during a long run (`...Config 2 5 30 5 0` → `Start` → move ~10 s → `Stop`).
   `IAI.Capture.ViewLag` defaults to **0** (validated — gotcha G41); raise only if a moving box trails the object.
6. **Verify:** `python tools/verify_capture.py --dir "<...>/run_<seed>_<stamp>"` (needs Pillow) — overlays the
   boxes onto annotated copies + prints a per-frame `present` table (flips at burst boundaries) + present /
   visible-positive / off-screen tallies. PNG is lossless + opaque (G39).
- **Format:** PNG default (dataset fidelity); `jpeg` arg for bandwidth. **`visible_positive`** (present + a valid
  box) is the detection-relevant positive; `present=true` + no box = anomaly active but off-screen (kept as a hard
  negative — G42).
- **Dashboard (m10):** the Tier-2 dashboard's Capture panel has the same two modes — a **Targeted / Auto-pool**
  toggle (targeted = anomaly dropdown + on-screen target picker; Start disabled until both are chosen). The old
  Auto-injection panel is now the **"Capture pool"** (pool checkboxes only — the free-run controls are gone), and
  the manual **Inject panel is removed** (ActivePanel + preview remain).

## 7. Runtime verification recipe (MCP-driven gate checks)
The non-visual stage gates are closed by driving PIE over the `unreal-mcpython` bridge (host tooling,
gotcha G8) and reading state/logs back; the owner eyeballs the visual gates. This is exactly how M1's
gates 2-7 were verified (2026-06-09) - reuse it for future anomalies.

**Prereqs:** editor open with the plugin built; **a play session in `MainWorld`** (the subsystems are
Game/PIE-only, so there is NO AnomalyInjector/Selector/AutoInjector subsystem until PIE is running); the
bridge listens on `127.0.0.1:12029` (it starts with the editor, survives Stop-PIE). You can start/end the
play session **headlessly over the bridge** instead of pressing Play by hand (used to drive the m6 gates):
```python
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not les.is_in_play_in_editor(): les.editor_play_simulate()   # start Simulate (exposes a view, G23)
# ... drive gates ...
les.editor_request_end_play()                                   # end it cleanly
```
Get the PIE world after starting: `gw = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()`.

**Core idioms** (via `mcp__unreal-mcpython__util_execute_python`):
- Get the PIE world (NOT the editor world):
  `gw = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()`
- Fire a command: `unreal.SystemLibrary.execute_console_command(gw, "IAI.Apply missing_object SM_Ramp")`
- Read an actor's hidden flag against the PIE world: iterate
  `unreal.GameplayStatics.get_all_actors_of_class(gw, unreal.Actor)`, filter by name substring, read
  `a.get_editor_property('hidden')`. (Editor-world actors are different instances - always read `gw`'s.)
- Read global time dilation: `unreal.GameplayStatics.get_global_time_dilation(gw)`
- Read logs: `mcp__unreal-mcpython__util_get_output_log` with a keyword, e.g. `anomaly`,
  `blinking toggle`, `Heartbeat`, `deinitializing`.

**Gotchas that bit us (save yourself the rediscovery):**
- **blinking toggles + the heartbeat log at Verbose.** First run
  `execute_console_command(gw, "Log LogAnomaly Verbose")`, then grep `blinking toggle`.
- **Never `time.sleep()` in Python** - it blocks the game thread so nothing ticks. For ticking effects,
  let real wall-clock pass *between* MCP calls, then read the accumulated toggle log.
- **PIE can run at a few FPS;** the blinking `while`-drain replays multiple half-periods per frame, so the
  toggle log still advances. (Several toggles sharing one timestamp is expected, not a bug.)
- **Teardown gate:** leave anomalies active, **Stop PIE**, then read the log (editor stays open, bridge
  still up) for `Subsystem deinitializing; reverted N active anomaly(ies).`
- **Target selection:** pick a small, countable, persistent set - M1 used `SM_Ramp` (2 actors). The Bot
  is runtime-spawned (G4), so it only matches after it spawns.

**Gate -> check:**
| gate | drive | assert |
|------|-------|--------|
| ListAnomalies | `IAI.ListAnomalies` | log: **7** lines, sorted, `id - description - usage` |
| missing_object | `IAI.Apply missing_object SM_Ramp` | both ramps `hidden == True`; log `matched 2 actor(s)` |
| blinking | `Log LogAnomaly Verbose`; `IAI.Apply blinking SM_Ramp` | repeating `blinking toggle -> HIDDEN/VISIBLE`; heartbeat `active: 1/N` |
| time_dilation | `IAI.Apply time_dilation 0.2`; `IAI.Revert time_dilation` | dilation `0.2`, then back to the captured baseline |
| lighting_mismatch | find a Movable `ULightComponent` (`Mobility==Movable`); `IAI.Apply lighting_mismatch <sub> recolor 1 0 1` | matched-count >= 1; read component `Intensity`/`GetLightColor()`/`GetVisibleFlag()`/`CastShadows` changed; `Revert` -> all restored. Owner eyeballs the lit change (movable target). |
| lod_corruption (static, regression) | `IAI.Apply lod_corruption SM_Ramp` | both ramps `forced_lod_model 0→1`, log `forced LOD 1 of 1`; `Revert` -> 0. Must be **identical to M2** (regression gate). |
| lod_corruption (skeletal + heterogeneous) | after Bot spawns: `IAI.Apply lod_corruption Bot` | one apply hits `StaticMeshComponent_0` (static, `forced_lod_model`) + `CharacterMesh0`/`Jetpack` (skinned, `get_forced_lod()`) → all `0→1`, log `forced LOD on 3 component(s)`; `Revert` -> all 0. Bot single-LOD → state-only (G20). |
| lod_popping | `Log LogAnomaly Verbose`; `IAI.Apply lod_popping Bot` | snap log alternates `POPPED ↔ BASELINE (N components)` at 2 Hz; `Revert` -> captured baseline on all; re-apply mid-oscillation re-captures true baseline (no stuck popped value). |
| camera_clipping | `IAI.Apply camera_clipping 100`; `IAI.Revert camera_clipping` | `GNearClippingPlane` == 100, then baseline (~10). Owner eyeballs near geometry vanishing / returning. |
| missing_texture | `IAI.Apply missing_texture SM_Ramp`; `IAI.Revert missing_texture` | every matched SM/SK slot → `M_MissingTexture_Checker`; a sibling sharing the same mesh+material is **untouched** (per-component isolation); `Revert` restores each slot's original exactly (explicit override → ptr; else cleared to asset default). Renders the checker on Nanite/skeletal targets (usage flags — G49). |
| RevertAll | apply >=2 anomalies, then `IAI.RevertAll` | all `IsActive==false`; captured state restored (hidden flags false, dilation baseline, lights/LOD/near-clip restored) |
| teardown | apply, then **Stop PIE** | log `Subsystem deinitializing; reverted N...`; re-check nothing stuck |
| no-leak | `IAI.Apply <id> A` then `IAI.Apply <id> B` (esp. `lighting_mismatch recolor`) | single capture set; only B's targets active; A's restored (no stuck lights/LODs) |
| viewport frustum (synthetic) | `IAI.TestVisibility SM_Ramp <O> <R>` from 3 poses: looking at the ramps; looking away (behind); 53k units away but in cone | log per-comp: in-cone → `frustum=1`; behind camera → `frustum=0` (near-plane); far-but-in-cone → `frustum=1` (far not clipping). Reversed-Z VP validated (gotcha G24). |
| viewport occlusion (synthetic) | place a big blocker between a synthetic camera and SM_Ramp, then `IAI.TestVisibility` from that pose vs. a clear pose | blocked → `frustum=1 unoccluded=0`; clear → `frustum=1 unoccluded=1`. Same targets; occlusion flips on line-of-sight only. |
| viewport regression (OFF) | default `scoping OFF`: re-run the `missing_object` / `lod_corruption` rows above | **byte-identical to M1/M3** (matched/forced/reverted counts unchanged) — the regression gate. |
| viewport scoping (ON) | `IAI.SetViewportScoping 1`; `IAI.Apply missing_object SM_Ramp` | matched count = the ramps **in the resolved live view**; no-view → full set + "treated as unscoped" warning (AMB-V3). Owner eyeballs off-screen-untouched / on-screen-affected in **real Play**. |
| selector renderable filter | `IAI.SelectorUI 1`; `IAI.Selector.Status` | the visible-names list **excludes** non-renderables (RVTVolume / PlayerStart / GameplayDebuggerCategoryReplicator / LandscapeStreamingProxy / RoomBuilderSquare) **and particle/VFX actors (Niagara/Cascade — excluded since G33)**, and **includes** renderables (Bot, ramps, pressure plates, doors, foliage/HISM). Live-enumerate the excluded actors' components to prove *why* (their primitives are UBoxComponent/capsule/UFXSystemComponent/etc., not static/skeletal mesh — gotchas G29/G33). |
| selector model (cycle) | `IAI.SelectorUI 1`; repeat `IAI.Selector.Next` then `IAI.Selector.Status` | `Status` shows `selected` advancing through the **name-sorted** visible set (and `visible (N)` listed); wraps after the last. Deterministic in Simulate (view resolves, G23). |
| selector zero-match HUD (escape hatch) | pure-VFX actors are no longer selectable (G33), so reach the zero-match via the console by-name escape hatch: `IAI.Apply lod_corruption =<VfxActorName>` | `Apply ... -> not applied` / "0 matched" in the log (R4 plumbing intact); the AMB-2 path is surfaced, not silent. The `=name` finder bypasses the renderable-set predicate, so it still reaches the VFX actor. |
| selector model (anomaly) | `IAI.Selector.Cycle` ×N; `IAI.Selector.Status` | chosen anomaly cycles `missing_object → blinking → lod_corruption → lod_popping → …`. |
| selector inject (exact-match) | select an actor, `IAI.Selector.Inject`; read the target's hidden / `forced_lod_model` | the selected actor changed (e.g. `hidden==True` for `missing_object`); **confirm the `=` exact-name hit ONLY that actor**, not a numbered sibling — if stock content has a `<name>`/`<name>2` pair, select `<name>` and assert `<name>2` is untouched. |
| selector revert | `IAI.Selector.Revert` | the last-injected id is reverted; target restored. |
| selector OFF (regression) | `IAI.SelectorUI 0`; re-run the `ListAnomalies` / `missing_object SM_Ramp` rows | **byte-identical** to before the selector existed (subsystem dormant); the **`=` sentinel** leaves substring gates (`SM_Ramp` → 2 ramps, `Bot`, `Foliage`) unchanged. |
| auto fire (deterministic) | `IAI.Auto.Enable 1`; `IAI.Auto.Seed 1234`; `IAI.Auto.Pool all 0`; `IAI.Auto.Pool missing_object 1`; `IAI.Auto.FireOnce`; `IAI.Auto.Status` | exactly **1** live fire, id `missing_object`, target in the renderable-visible set, target `hidden==True`; the `=` hit ONLY that actor (no prefix-sibling). |
| auto collision-free (concurrent) | enable `{missing_object, blinking, lod_corruption}`; repeat `IAI.Auto.Step 5`; `IAI.Auto.Status` | **no two live fires on one actor** (OVERRIDE-1) and **no id double-live** (i); live count ≤ enabled-id count. |
| auto auto-revert (R-LIFE) | `IAI.Auto.Hold 1 1`; `IAI.Auto.FireOnce`; `IAI.Auto.Step 1.5`; `IAI.Auto.Status` | the fire **auto-reverted** (live count drops; target restored; log `Auto.Revert ... hold elapsed`). Then `IAI.Auto.Persist 1`; `FireOnce`; `Step 100` → still live (persists). |
| auto no-blind-fire | aim at nothing (empty renderable-visible set); `IAI.Auto.FireOnce`/`Step` | **zero** fires (`GetVisibleRenderableActors` empty → never inject blind). |
| auto seed reproducibility | same `IAI.Auto.Seed S` + same `FireOnce`/`Step` sequence + same camera | identical fire/target/hold sequence across two runs (R-SEED; choices reproduce given the same visible-set sequence). |
| auto zero-match | pure-VFX actors are no longer in the auto pool's target set (G33), so the auto path no longer produces an LOD-on-VFX zero-match. The zero-match plumbing (stream advances Id/Target/Hold; no slot leak on `ApplyAnomaly==false`) is covered by the console escape-hatch row above + code-identity. | n/a via the set — re-pointed to the escape hatch. |
| auto OFF (regression) | `IAI.Auto.Enable 0` (default); re-run the `ListAnomalies` / `missing_object SM_Ramp` rows | **byte-identical** (subsystem dormant — Tick early-returns, no HUD delegate, no stream churn). |
| auto coexistence-warn | `IAI.SelectorUI 1` then `IAI.Auto.Enable 1`; separately `IAI.SetViewportScoping 1` then `IAI.Auto.Run 1` | each logs a **Warning** (selector+auto both on; scoping ON at run-start). Neither **blocks** (R-COEXIST). |
| poll-radius cull (G34) | note baseline `IAI.DumpVisible` count; `IAI.SetPollRadius <R>` with a small R (e.g. 1500); `IAI.DumpVisible` again (and check the selector/auto target set) | actors beyond R of the **pawn** drop out of the set; near actors remain. The drop is identical in the selector, the auto pool, and the dashboard (one source of truth). |
| poll-radius set-identity | with `R > 0` set: `IAI.DumpVisible` | still `byte-identical(set+order): MATCH` — the cull is applied identically to `GetVisibleRenderableActors` and `GetVisibleRenderableActorInfos`. |
| poll-radius OFF (regression) | `IAI.SetPollRadius 0` (default sentinel); re-run the prior renderable-set / `IAI.DumpVisible` rows | **byte-identical** to no-cull (R ≤ 0 disables entirely). `IAI.SetPollRadius` with no arg logs the current radius + usage and changes nothing. |
| poll-radius debug sphere (eyeball) | `IAI.SetPollRadius <R>` in real Play | a yellow debug sphere of radius R is centered on the **live pawn** and follows it each frame; it disappears at `IAI.SetPollRadius 0`. |

**Synthetic-view gate recipe (viewport core).** The core is a pure function of (explicit view, world), so it is
state-gatable deterministically with `IAI.TestVisibility` — no live player needed. Read a target's world bounds
(`actor.get_actor_bounds(False)`), pick a camera origin/rotation, fire `IAI.TestVisibility`, and read the per-component
`frustum/unoccluded/visible` lines off `LogAnomaly`. For a clean **close-range** occlusion negative, you need a
full-coverage blocker in the **PIE** world — but `EditorActorSubsystem.spawn_actor_from_object` refuses to spawn
during play. The working route: end Simulate, spawn a large `/Engine/BasicShapes/Cube` (default collision blocks
`ECC_Visibility`) between the chosen camera and the target in the **editor** world, restart Simulate (it duplicates
into the PIE world), gate, then **end Simulate and `destroy_actor` the blocker — never save the map**. (Session 008
drove exactly this: behind/far/in-cone frustum cases + a wall-between vs clear-line occlusion pair, all green.)

To read component state via the bridge: get the PIE world, `GameplayStatics.get_all_actors_of_class(gw, unreal.Actor)`,
filter by substring, then iterate `a.get_components_by_class(unreal.LightComponent)` /
`unreal.StaticMeshComponent` / `unreal.SkinnedMeshComponent` and read the property. Forced-LOD reads:
static = `c.get_editor_property('forced_lod_model')`; skinned = `c.get_forced_lod()`. **The skinned
component's `get_num_lods()` is NOT bound in editor Python** — read the runtime LOD count off the C++ log
line `lod_corruption: '<comp>' forced LOD N of M` instead (M = the helper's `GetNumLODs()`; gotcha G19).
The Bot (`BP_Bot_C_0`) carries 1 static + 2 skinned comps and spawns even in a Simulate session (G20).
Near clip: `unreal.SystemLibrary` has no getter — read it back via the `camera_clipping: near clip X -> Y.`
log line (`util_get_output_log` keyword `camera_clipping`).

## 8. Packaged bench leg — the recipe (use this, do not reconstruct one)

The standing test baseline is a **local packaged build**, not PIE (G76). For a **code-only** change the
whole cycle is a build plus one file copy — **no cook** (**G103**).

### 8.1 Build and stage

```powershell
& "D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat" `
  StackOBot Win64 Development -project="D:\IntrusiveAnomalies\StackOBot\StackOBot.uproject" -waitmutex

$staged = "D:\IntrusiveAnomalies\StackOBot\Builds\BenchGate\Windows\StackOBot\Binaries\Win64\StackOBot.exe"
Copy-Item $staged "$staged.bak" -Force        # keep the previous binary if it is a baseline
Copy-Item "D:\IntrusiveAnomalies\StackOBot\Binaries\Win64\StackOBot.exe" $staged -Force
```

⚠ **`Builds\BenchGate\Windows\StackOBot.exe` (217 KB) is the LAUNCHER STUB** — never scan or run it as
the binary under test (**G90**). The real one is under `...\StackOBot\Binaries\Win64\`.

### 8.2 A44 provenance — scan the STAGED artifact, BOTH encodings

The hot-swap **is** the stage step, so *compiled ≠ staged* still applies.

```powershell
$b=[System.IO.File]::ReadAllBytes($staged)
$a=[System.Text.Encoding]::ASCII.GetString($b); $u=[System.Text.Encoding]::Unicode.GetString($b)
foreach($p in @("<a symbol your change adds>","IsHideTypeAnomaly")){
  "{0,-32} ascii={1} utf16={2}" -f $p,
    ([regex]::Matches($a,[regex]::Escape($p))).Count,
    ([regex]::Matches($u,[regex]::Escape($p))).Count }
```

⚠ **UE string literals are UTF-16.** An ASCII-only scan returns **0 for everything** and reads as *"the
change did not reach the package"* — a false negative on A44's load-bearing half. **A scan that reports
zero across ALL symbols is SUSPECT TOOLING until the encoding is confirmed** — a real pre-change binary
still matches *something*.

### 8.3 Run a leg, with focus forced (G104)

```powershell
$cmds = 'IAI.Capture.Config 2 4 8 4 0, IAI.Capture.Start MYLEG png 777 90 blinking StaticMeshActor_49'
Get-Process -Name "StackOBot*" -EA SilentlyContinue | Stop-Process -Force      # A46 idle-box assert
$p = Start-Process -FilePath $staged -PassThru -ArgumentList @(
  "/Game/CaptureBenchGate/CB_GateLevel","-windowed","-ResX=1280","-ResY=720",
  "-ExecCmds=`"$cmds`"","-unattended","-nosplash")
$sh = New-Object -ComObject WScript.Shell
1..12 | ForEach-Object { Start-Sleep -Milliseconds 400; try { $sh.AppActivate($p.Id) } catch {} }
Start-Sleep -Seconds 60
Get-Process -Name "StackOBot*" -EA SilentlyContinue | Stop-Process -Force
```

**Launch geometry is a leg VARIABLE (S4-0), not a constant.** `run_leg.ps1` takes `-ResX -ResY
-Fullscreen -ExtraExecCmds -GeometryNote`; defaults are unchanged, so existing invocations run
identically. Every banked attempt carries `_leg_geometry.json` **beside** the session recording the
launch rect, window mode, OS DPI scale and leg config — a leg that will be graded later must state the
conditions it was produced under (**G106**).

⚠ **B1's pose gate CANNOT RUN off 1280×720** — `CALIB_BBOX` is frozen in **pixels**, so any other
resolution fails it for a reason unrelated to pose. `check_pose.py` prints the per-component ratio;
a **uniform** ratio matching the resolution ratio with a stable `modal_rot` means **resolution scope**,
not bifurcation. Rect evidence is still valid on such a leg; **alignment evidence is not.**

⚠ **A display-scale change does NOT reach a packaged build** (**G114**) — it is DPI-unaware, so Windows
virtualises it. To probe DPI you must force awareness on (`~ HIGHDPIAWARE` in AppCompatFlags) **and
verify it with `GetProcessDpiAwareness` before the leg**, or the clean null you get is an artifact.

- ⚠ **Output lands beside the EXE, not under `Saved`** — `...\Binaries\Win64\MYLEG\session_<ts>\`
  (**G101**). The log prints only the relative path.
- ⚠ **`-ExecCmds` fires at STARTUP ONLY.** There is no way to issue a post-run console command from the
  packaged harness; anything needing that (e.g. a post-run `capture_status`) needs a WS client against
  the control server.

### 8.4 Validity before verdict

```powershell
(Get-Content "<session>\run.json" -Raw | ConvertFrom-Json).start_frame   # must MATCH across compared legs
```

**A63:** legs in a cross-binary comparison are comparable only if `start_frame` matches. A leg that rode
the 30 s focus timeout (`start_frame` in the thousands, `ticks_msec[0]` ≈ 30270) is **INVALID** — re-run
it, and **do not read its diff**. *A leg is discarded for how it ran, never for what it showed.*

**A62:** verify the session **on disk after the process has exited** — directory present, file count
right. A log line saying `FINISHED` is not evidence a file exists.

### 8.5 Run the gate — do not hand-roll a comparator

```powershell
& "C:\Python313\python.exe" `
  "D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\subset_gate.py" `
  <controlA> <controlB> <testA> <testB>
```

`controlA`/`controlB` are two runs of the **same reference binary** (they establish the run-unique
field set empirically); `testA`/`testB` are the reference leg vs the leg under test. It enforces **A63
first** — if `start_frame` differs it prints **no verdict** and exits **2**. Exit `0` pass / `1` fail /
`2` invalid. `compare_sessions.py` is the field-by-field differ underneath it.

⚠ **These scripts produced the accepted S3a verdicts.** Re-deriving a comparator from a description is
how it drifts from the one the banked results were graded with.

**G92:** re-bank before any step that wipes `Saved`. A code-only hot-swap does **not** touch it.

---

## 8.6 FULL COOK — the recipe (use this; §8.1–§8.5 above are the CODE-ONLY hot-swap)

⚠ **Everything above this point is the hot-swap: a build plus one file copy, NO COOK (G103).** It is
the right cycle for a code change and the wrong one for anything that changes **content** — a new or
changed level, a config value the game reads from its cooked ini (secrets, default map, capture
defaults), or a new asset. Those need a full cook, and until 2026-08-19 this document had **no
recipe for one**; it had to be reconstructed from `G91`. Written down here after being executed.

**Run the steps in this order. Steps 0–3 are protective and are not optional.**

### 0. 🚨 CHECK FREE SPACE FIRST — GO / NO-GO (G130)

```powershell
(Get-PSDrive D).Free / 1GB
```

| free | verdict |
|---|---|
| **≥ 15 GB** | ✅ **GO** |
| 10–15 GB | ⚠ **marginal** — free something regenerable first |
| **< 10 GB** | ⛔ **NO-GO. Do not start.** |

⚠ **THE OUTPUT IS NOT THE WORKING SET.** The cooked `.ucas` is **284 MB**, and that number is
useless for planning: the cook additionally writes `Saved\Cooked`, `Saved\StagedBuilds` **and** the
archive copy, so the transient requirement is **multiple GB**. Measured 2026-08-19: the project tree
went from 19.12 GB free to **0.94 GB** across two builds and eight legs — `Intermediate` alone reached
**14.54 GB**.

🚨 **WHY THIS IS A GATE AND NOT A TIP: a cook that runs out of disk mid-way leaves a HALF-WRITTEN
CONTAINER BEHIND A BUILD THAT STILL BOOTS.** The failure does not announce itself; it produces an
artifact that presents as healthy. **That is worse than any amount of lost progress.**

**Regenerable trees, safe to free without any retention decision** (measured sizes, 2026-08-19):

| tree | size | note |
|---|---|---|
| `StackOBot\Intermediate` | 14.54 GB | forces a full rebuild, ~3 min |
| `StackOBot\.vs` | 4.72 GB | Visual Studio cache; the CLI build does not use it |
| `Builds\BenchGate\...\Saved\` | 5.66 GB | ⚠ **only after verifying every session is banked BY SESSION ID + per-file manifest** |

⛔ **NOT free space, and named here so they are never treated as such:** `_binary_baselines` (the two
preserved quartets) · `_bench_sessions_bank` · `Builds\MidRepro` (the `m17` repro harness, a
documented validation asset) · **`Builds\Windows` — the pre-cook 3-map build, which is the PHYSICAL
EVIDENCE behind `S-1` and the `G87` correction; it is the artifact that proves `MainWorld` was never
cooked, and re-deriving that would need a cook.**

### 1. Re-bank first (G92)

The archive step is destructive **under conditions that are not established**. Move anything unbanked
out of the package tree **before** cooking:

- `Builds\BenchGate\Windows\StackOBot\Saved\` — the historical capture dirs.
- `Builds\BenchGate\Windows\StackOBot\Binaries\Win64\<LABEL>\` — ⚠ **leg output lands beside the EXE,
  not under `Saved` (G101)**, so it is easy to miss. **Match by SESSION ID, not by directory name** —
  the harness banks the *accepted* session under a different directory name, so a name-based check
  reports false duplicates. The 2026-08-19 sweep found **9 unbanked items** this way, four of which
  were the raw evidence behind m25's S4-3 and S4-4 claims.

📏 **Measured 2026-08-19: that cook wiped NOTHING** (leg dirs 56→56, baselines 4→4, `Saved` 23→23).
⛔ **This does NOT retire the step.** One cook, one flag set (no `-clean`, archiving into an existing
tree); the 2026-08-16 wipe stands and which factor decides is unknown. One copy versus an
unrecoverable loss.

### 2. Rescue the baseline QUARTET (G121)

⛔ **A baseline is `exe + utoc + ucas + pak`. An exe alone is half an artifact** — a content-only cook
leaves the exe byte-identical while changing maps, secrets and hundreds of MB of content.

Copy the current exe **and the `Content\Paks\` set** to `D:\IntrusiveAnomalies\_binary_baselines\`
(outside `Builds\`, where a stage cannot reach), then **verify by hash AT THE NEW LOCATION** — A62, a
copy that ran is not a copy that landed. Update that directory's `README.md`.

### 3. Declare the map set IN WRITING, before the cook

The cook is **map-restricted by `-map=`**. Anything not listed is **not in the build**, silently.
**That is exactly how `MainWorld` came to be absent from every build for months** (G87's correction,
G120). `CB_GateLevel` is **non-negotiable** — every m25 certification is expressed in it.

### 4. Cook, stage, pak, archive

```powershell
& "D:\UESource\UnrealEngine\Engine\Build\BatchFiles\RunUAT.bat" `
  BuildCookRun `
  -project="D:\IntrusiveAnomalies\StackOBot\StackOBot.uproject" `
  -platform=Win64 -clientconfig=Development `
  -cook -stage -pak -archive `
  -archivedirectory="D:\IntrusiveAnomalies\StackOBot\Builds\BenchGate" `
  -build -utf8output -nocompileeditor `
  -map="/Game/CaptureBenchGate/CB_GateLevel+/Game/StackOBot/UI/MainMenu/MainMenu+/Game/StackOBot/Maps/MainWorld"
```

`Entry.umap` arrives without being listed (engine default). **A World Partition level's external
actors are pulled in by naming the level** — `MainWorld`'s 419 one-file-per-actor externals cooked
from the single `-map=` entry. Measured wall time: **2 m 27 s** for that set on a warm DDC.
⛔ **If the cook requires a production-code change to succeed, HALT** — that is a scope change.

### 5. GATE: read the cooked map set back OUT OF THE ARTIFACT

```powershell
powershell -File "D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\verify_cooked_maps.ps1"
```

**`-map=` is an INPUT; the `.utoc` container index is the ARTIFACT** (G119). Exit `0` pass · `1` a
required map missing, with a distinct HALT for `CB_GateLevel` · `2` unexpected entries **or a scan
that found nothing in either encoding**. ⚠ **The index encoding is NOT stable** — pre-cook containers
read back as UTF-16, the post-cook one as ASCII — so a single-encoding scan can return a clean-looking
*"no maps cooked"*. The tool scans both and says which answered.

### 6. Token read-back (G118 / G112-amended)

A packaged build enforces the **cooked** config. Read the enforced token from the **running build's own
log**, never from the source ini:

```powershell
powershell -File "D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\ws_scoping_echo.ps1"
```

It prints source-vs-enforced side by side and shouts on a placeholder. ⛔ **If the build still enforces
a placeholder after a full cook, HALT** — that means the cook is not consuming the config you think it
is, which is a bigger finding than the token.

### 7. A44 scan of the STAGED artifact, both encodings (§8.2, G103)

Scan for a symbol the change adds **and** a control string that predates it. ⚠ **Zero across ALL
symbols is SUSPECT TOOLING, not a clean result** — `TEXT()` literals are UTF-16.

### 8. Record the new build identity — all four hashes

`exe · StackOBot-Windows.utoc · .ucas · .pak`. ⚠ **The exe hash may be UNCHANGED** if no code changed
(the archived exe keeps its *compile* time), so **the exe hash alone does not tell you the cook
happened.** The `.utoc` size/mtime/hash does.

### 9. SMOKE: one leg in `CB_GateLevel`, on-calibration

1280×720 windowed, 100 % scale, `VideoFps` 30 pinned, SVE default **not forced**, delivery OFF, target
`StaticMeshActor_49` (so **B1 applies**). Assert: `capture_path` `sve` · `content_clock` `wall` ·
`key_ring` `missed 0` and `published == consumed` · **B1 pose-matched** · **A54 ALL-ALIGNED with the
in-leg positive control decisive in BOTH directions** · **≥ 3 counted events**.
⛔ **A cook that changed certified behaviour is a FINDING** — it must not be discovered later, inside
a result.

## Troubleshooting
- **"The following modules are missing or built with a different engine version… rebuild?"** —
  expected if `Binaries/` is stale or absent. Click **Yes**, or run step 4 first.
- **Plugin not listed / not loaded** — confirm `EnabledByDefault: true` in the `.uplugin` and
  that `UnrealEditor-AnomalyInjector.dll` exists under `Binaries\Win64` after the build.
- **Commands print "subsystem not present"** — you ran them outside a Game/PIE world. The
  subsystem only exists in Game + PIE (by design); run the commands inside PIE.
