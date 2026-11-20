# Office card — Europa D-lite: build the plugin on Europa's fork, then a four-anomaly smoke

**What this is:** one office visit. You compile `feat/europa-port` into Europa's project with Europa's fork, then watch four anomalies apply and revert by eye.
- 🏠 marks what was **proven at home** on stock UE 4.25.4.
- 🔮 marks a **prediction** for Europa's fork (`G120`: the fork may be modified).

**HARD RULE:** nothing leaves the office box except the lines you type by hand into the read-back block (§6). No files, logs, screenshots, clipboard or chat. **Never type a project, level, actor, folder or studio name into a read-back.**

**Names used below:**
- `<ForkRoot>` = the folder that holds the fork's `Engine\` folder.
- `<Project>` = the folder that holds Europa's `.uproject`; `<Name>` = that file's name without `.uproject`.
- `<Mesh>` and `<Light>` = actor names you read on screen. They stay on the office box.

## 1. Before you go (home)
**Scan verdict (094-29): PUSH-READY: yes** (`_reviews\094-29-prepush-scan.md`). One owner-call item: the tools scripts contain your Windows profile path (non-secret).

**Code pushes for you (your call, 11-02). You run nothing at home.** Before your office day, chat has Code push the tested commit `4f46a9c` (or a later commit that has passed its checks) onto the branch `feat/europa-port`, with no tags, and confirm that it arrived. Later local commits stay home until chat clears them (one of them carries a list of your drive folder names that is being removed first). These are the exact commands Code uses, for reference only:
```
git -C "D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector" push origin 4f46a9c:refs/heads/feat/europa-port
git -C "D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector" ls-remote origin feat/europa-port
```
The last line must start with `4f46a9c`. The office build pins `4f46a9c` (§2).

## 2. Get the plugin onto the office box (cmd)
1. **Check for an existing copy:**
   ```
   dir /s /b "<Project>\Plugins\AnomalyInjector.uplugin"
   ```
   It must print **File Not Found** (or "cannot find"). If a file is listed, **stop** and read back `R1 existing copy`. (UE scans `Plugins\` recursively, so two copies is a fatal duplicate.)
2. **Clone the branch.** Use the same URL your office clone of this repo uses. `git -C "<that clone>" remote get-url origin` shows it.
   ```
   git clone --branch feat/europa-port --single-branch <that URL> "<Project>\Plugins\AnomalyInjector"
   git -C "<Project>\Plugins\AnomalyInjector" checkout --detach 4f46a9c
   git -C "<Project>\Plugins\AnomalyInjector" log -1 --format=%h
   ```
   The last line must print `4f46a9c`. That is the home-tested source: `abb240b` plus one plan document.
   - 🏠 Home mounted the same tree at `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector`.
   - **Do not add this folder to the project's source control.**
3. **Nothing to switch on.** The plugin's `Build.cs` detects 4.25 itself (`AnomalyInjector.Build.cs`, `Is425`) and compiles out capture, the control server, shaders and bench. 🏠 Those modules still load and log "compiled out" (094-27e session log). **No `.uproject` edit.**
   - 🔮 The plugin is `EnabledByDefault`, so a project-folder plugin loads without a `.uproject` entry. The home host listed it explicitly.

## 3. Build (cmd; close Europa's editor first)
4. **Run:**
   ```
   "<ForkRoot>\Engine\Build\BatchFiles\Build.bat" <Name>Editor Win64 Development -Project="<Project>\<Name>.uproject" -WaitMutex -NoEngineChanges
   echo %ERRORLEVEL%
   ```
   The editor target name is the one in `<Project>\Source\<Name>Editor.Target.cs`. `-NoEngineChanges` makes the build stop rather than rebuild the fork's engine.
   - 🏠 The same `Source` (tree `5b4cbbf1`, unchanged since `d9c91c1`) built Editor Development with **0 C++ errors and 0 warnings**: 094-08 at `489c828`, and 094-21 at `40a8eaf`. That build used VS2017 14.16.27051, SDK 10.0.17763 and C++17.
   - 🔮 The fork reports engine 4.25. Anything else stops the build with "supports exactly Unreal Engine 4.25 and 5.1".
   - 🔮 Its compiler accepts C++17 and its headers match stock 4.25.
   - 🔮 Its `WebSocketNetworking` plugin exists. The plugin descriptor asks for it.
5. **Read** the build's `Anomaly profile: engine=… version=…` line: type the `engine=` value and only the **numbers** of `version=` (for example `4.25.4`). If any letters or words follow the numbers, don't type them; type `+text` instead (**R3**, chat's amendment 11-02). Read the exit code (**R2**). If R2 is not `0`, find the **first** line that says `error` and names an `Anomaly…` module. Type it **starting at `Source\`**, never the folder part before it. If the line has no `Source\`, type only its message text, with any folder path removed (**R4**).

## 4. Injection smoke
6. **Launch** like the home A7 legs: the fork's editor binary in game mode.
   ```
   "<ForkRoot>\Engine\Binaries\Win64\UE4Editor.exe" "<Project>\<Name>.uproject" <level path, e.g. /Game/…/SomeLevel> -game -windowed -ResX=1280 -ResY=720 -IAIBench -log
   ```
   - 🏠 This is 094-27e's launch line minus the probe flags. `-log` only opens a log window.
   - **`-IAIBench` is required:** on 4.25, missing_object exists only with it (`Compat/AnomalyCapabilities.cpp:7`).
   - You may leave out the level path; then use any level with ordinary visible meshes and lights.
   - 🔮 The fork's `-game` reaches a playable level. **Fallback:** open the editor normally, load the level, press **Play**, and type the same lines into the Play window's console. That fallback is unproven at home.
7. **Open the console** (the **`** key). Type each line, then read the answer in the log window (lines starting `LogAnomaly`):
   ```
   t.MaxFPS 30
   IAI.ListAnomalies
   IAI.DumpVisible
   IAI.ListActors
   ```
   - `t.MaxFPS 30` is the home leg's frame cap; it keeps the blink at about 0.4 s per phase.
   - `IAI.ListAnomalies` ends with `--- N anomaly(ies) ---`. Expect **4** (**R6**). 🏠 The home log reads "4 anomaly type(s) registered".
   - `IAI.DumpVisible` lists the meshes on screen as `[i] <ActorName> | asset=… | … dist=…`. Pick a close, plainly visible one as `<Mesh>`.
     - If it lists 0, use `IAI.ListActors` instead: pick a `StaticMeshActor` whose label you recognise and take the **second** column (the Name).
     - 🏠 The visible-set code and exact `=Name` targeting ran on 4.25: 094-27c Admission log, `blinking: matched 1 actor(s) for '=EA_Floor'`.
   - From `IAI.ListActors`, take as `<Light>` the Name of a `PointLight`, `SpotLight`, `DirectionalLight` (or a lamp Blueprint) that lights what you see.
8. **The four anomalies, one at a time.** Apply, look for 5–10 s, revert, look again. The arguments are the home legs' (`tools/europa/runtime_scripts.py`, 094-27c/27e). `IAI.Apply` calls the same `ApplyAnomaly(id, args)` the home probe called.

| # | Apply | See while active | Revert, then see |
|---|---|---|---|
| a | `IAI.Apply blinking =<Mesh> 12` | `<Mesh>` vanishes and returns about every half second. 🏠 cube gone/back on 6 of 6 events, 094-27e | `IAI.Revert blinking` → stays visible |
| b | `IAI.Apply lighting_mismatch =<Light> dim 0.05` | What that light reaches turns much darker (5 % brightness). 🏠 scene dark, 5 counted events, 094-27c | `IAI.Revert lighting_mismatch` → normal light |
| c | `IAI.Apply lod_corruption =<Mesh> 64` | `<Mesh>` turns visibly coarser. The log says `forced LOD a of b`; if `b` is 1, pick another mesh and repeat. 🏠 centre cube turned sphere on 6 of 6 events, 094-27e | `IAI.Revert lod_corruption` → original detail |
| d | `IAI.Apply missing_object =<Mesh>` | `<Mesh>` and its shadow are gone. 🏠 cube and shadow gone on 6 of 6 events, 094-27c / 27d | `IAI.Revert missing_object` → back |

   Each apply logs `IAI.Apply '<id>' -> applied` (or `not applied`). lighting_mismatch also logs `on N light component(s)`.
   - **(c):** home used index `1` on a fixture whose LOD0 was the visibly different shape. `64` takes the same branch and clamps to the mesh's coarsest LOD (`AnomalyLod.cpp`, `ResolveTargetLod`).
   - 🔮 (b) shows only on a **non-Static** light; the home light was Movable.
   - 🔮 (c) shows only if `<Mesh>` has 2 or more LODs.
9. **Finish:** type `IAI.RevertAll` and read `reverted N` (**R11**; expect 0). Then close the game window.

## 5. Stop rules
10. **On a crash:** choose **Close Without Sending**, read back `crash at step N`, and stop. Decline any prompt to send data or feedback.
11. **Never copy files, logs or screenshots off the box.** Type only the read-back. At home, save it under `D:\IntrusiveAnomalies\_bates_reads`.
12. **Leave the plugin folder in place** unless told otherwise. It is a plain clone, not a junction, so deleting it later is safe.

## 6. Read-back (type these 12 lines)
```
R1  plugin at 4f46a9c: yes/no   (or: existing copy)
R2  build exit code: __
R3  profile: engine=__ version=__
R4  first Anomaly error from Source\ : ____ (or: none)
R5  game window up, no crash: yes/no   mode: game/PIE
R6  anomaly types: __
R7  blinking: applied y/n, visible y/n, reverted y/n
R8  lighting_mismatch: lights __, visible y/n, reverted y/n
R9  lod_corruption: LOD __ of __, visible y/n, reverted y/n
R10 missing_object: applied y/n, visible y/n, reverted y/n
R11 RevertAll reverted: __
R12 crash: none / at step __
```
