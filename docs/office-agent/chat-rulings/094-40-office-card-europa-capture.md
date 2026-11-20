# Office card — Europa capture smoke: build the plugin on Europa's fork, a four-anomaly eyeball smoke, then one short seeded capture

**What this is:** one office visit, extending the D-lite card (`_reviews\D-lite-office-card-europa.md`).
1. Compile `feat/europa-port` at `d1cdc7b` into Europa's project with Europa's fork.
2. Watch four anomalies apply and revert by eye.
3. Run one short seeded auto-injection capture from the player's view, and type back the reads in §7.

- 🏠 marks what was **proven at home** on stock UE 4.25.4 (job 094-40, 2026-11-02).
- 🔮 marks a **prediction** for Europa's fork (`G120`: the fork may be modified).

**HARD RULE:** nothing leaves the office box except the lines you type by hand into the read-back block (§7). No files, logs, screenshots, clipboard or chat. **Never type a project, level, actor, class, folder or studio name into a read-back.** Where a value could be a name, the card asks for `standard` / `custom` instead.

**Names used below:**
- `<ForkRoot>` = the folder that holds the fork's `Engine\` folder.
- `<Project>` = the folder that holds Europa's `.uproject`; `<Name>` = that file's name without `.uproject`.
- `<Mesh>` and `<Light>` = actor names you read on screen. They stay on the office box.
- `<Out>` = a **new, empty** folder **outside** `<Project>`, on a drive with at least 5 GB free. Use a path with **no spaces**. `<OutFwd>` = the same path written with forward slashes (for example `D:/IA_Office/cap01`).

## 1. Before you go (home) — nothing to do
- **Already pushed (owner's call, 11-02):** Code pushed `feat/europa-port` = **`d1cdc7b`** to origin on 2026-11-02 at 04:55 IST. `git ls-remote origin feat/europa-port` returned `d1cdc7b956db…`, equal to the local tip. No tags, no backup refs, no force.
- **Pre-push scan:** clean (`_reviews\094-40-prepush-scan.md`). One carried, non-secret item from 094-29: one tools script contains your Windows profile path.
- **What `d1cdc7b` is:** the source built and smoke-tested at home in 094-40 (`08fe652`). It differs only by FV6's redaction of one receipts JSON plus the 094-40 close-out docs. `Source`, `Shaders` and the descriptor are byte-identical to the built commit.

## 2. Get the plugin onto the office box (cmd)
1. **Check for an existing copy:**
   ```
   dir /s /b "<Project>\Plugins\AnomalyInjector.uplugin"
   ```
   - **File Not Found:** go to step 2a.
   - **One file is listed and it is `<Project>\Plugins\AnomalyInjector\AnomalyInjector.uplugin` from an earlier visit of this card:** go to step 2b.
   - **Anything else:** stop and read back `R1 existing copy`. UE scans `Plugins\` recursively, so two copies is a fatal duplicate.
2. **(2a) New clone.** Use the same URL your office clone of this repo uses; `git -C "<that clone>" remote get-url origin` shows it.
   ```
   git clone --branch feat/europa-port --single-branch <that URL> "<Project>\Plugins\AnomalyInjector"
   git -C "<Project>\Plugins\AnomalyInjector" checkout --detach d1cdc7b
   git -C "<Project>\Plugins\AnomalyInjector" log -1 --format=%h
   ```
   **(2b) Existing clone:**
   ```
   git -C "<Project>\Plugins\AnomalyInjector" fetch origin feat/europa-port
   git -C "<Project>\Plugins\AnomalyInjector" checkout --detach d1cdc7b
   git -C "<Project>\Plugins\AnomalyInjector" log -1 --format=%h
   ```
   The last line must print **`d1cdc7b`** (**R1**). **Do not add this folder to the project's source control.**
3. **Nothing to switch on.** The plugin's `Build.cs` detects 4.25 itself (`Is425`). **No `.uproject` edit.**
   - 🔮 The plugin is `EnabledByDefault`, so a project-folder plugin loads without a `.uproject` entry. The home host listed it explicitly.

## 3. Build (cmd; close Europa's editor first)
4. **Run:**
   ```
   "<ForkRoot>\Engine\Build\BatchFiles\Build.bat" <Name>Editor Win64 Development -Project="<Project>\<Name>.uproject" -WaitMutex -NoEngineChanges
   echo %ERRORLEVEL%
   ```
   The editor target name is the one in `<Project>\Source\<Name>Editor.Target.cs`. `-NoEngineChanges` makes the build stop rather than rebuild the fork's engine.
   - 🏠 The same `Source` built Editor and Game Development with **0 C++ errors** on 094-40's first attempt each (VS2017 14.16.27051, SDK 10.0.17763, C++17).
   - 🔮 The fork reports engine 4.25. Anything else stops the build with "supports exactly Unreal Engine 4.25 and 5.1".
   - 🔮 Its compiler accepts C++17 and its headers match stock 4.25.
   - 🔮 Its `WebSocketNetworking` plugin exists. The plugin descriptor asks for it.
5. **Read** the build's `Anomaly profile: engine=… version=…` line and type only the `engine=` value and the **numbers** of `version=` (for example `4.25.4`). If any letters or words follow the numbers, don't type them; type `+text` instead (**R3**, chat's amendment 11-02: a fork's version string can carry a branch name). Read the exit code (**R2**).
   - If R2 is not `0`, find the **first** line that says `error` and names an `Anomaly…` module.
   - Type it **starting at `Source\`**, never the folder part before it.
   - If the line has no `Source\`, type only its message text, with any folder path removed (**R4**).
   - **If R2 is not `0`, stop here** and type the rest of the read-back as `n/a`.

## 4. Eyeball smoke (unchanged from D-lite, and it warms the level's shaders for §5)
6. **Launch** the fork's editor binary in game mode:
   ```
   "<ForkRoot>\Engine\Binaries\Win64\UE4Editor.exe" "<Project>\<Name>.uproject" <level path, e.g. /Game/…/SomeLevel> -game -windowed -ResX=1280 -ResY=720 -IAIBench -log
   ```
   - **Pick an ordinary level:** the player stands in it and sees several plain meshes and at least one non-Static light. Use **the same level in §5**.
   - **`-IAIBench` is required:** on 4.25, missing_object and the capture command exist only with it.
   - 🔮 The fork's `-game` reaches a playable level. **Fallback:** open the editor normally, load the level, press **Play**, and type the same lines into the Play window's console. That fallback is unproven at home, and §5 then needs the PIE note there.
7. **Open the console** (the **`** key). Type each line and read the answer in the log window (lines starting `LogAnomaly`):
   ```
   t.MaxFPS 30
   IAI.ListAnomalies
   IAI.DumpVisible
   IAI.ListActors
   ```
   - `IAI.ListAnomalies` ends with `--- N anomaly(ies) ---`. Expect **4** (**R6**).
   - `IAI.DumpVisible` lists on-screen meshes as `[i] <ActorName> | …`. Pick a close, plainly visible one as `<Mesh>`.
     - If it lists 0, use `IAI.ListActors` instead and take the **second** column (the Name) of a `StaticMeshActor` you recognise.
   - From `IAI.ListActors`, take as `<Light>` the Name of a `PointLight`, `SpotLight` or `DirectionalLight` (or lamp Blueprint) that lights what you see.
8. **The four anomalies, one at a time.** Apply, look for 5–10 s, revert, look again.

| # | Apply | See while active | Revert, then see |
|---|---|---|---|
| a | `IAI.Apply blinking =<Mesh> 12` | `<Mesh>` vanishes and returns about every half second | `IAI.Revert blinking` → stays visible |
| b | `IAI.Apply lighting_mismatch =<Light> dim 0.05` | what that light reaches turns much darker | `IAI.Revert lighting_mismatch` → normal light |
| c | `IAI.Apply lod_corruption =<Mesh> 64` | `<Mesh>` turns visibly coarser; the log says `forced LOD a of b`. If `b` is 1, pick another mesh and repeat | `IAI.Revert lod_corruption` → original detail |
| d | `IAI.Apply missing_object =<Mesh>` | `<Mesh>` and its shadow are gone | `IAI.Revert missing_object` → back |

   - 🔮 (b) shows only on a **non-Static** light.
   - 🔮 (c) shows only if `<Mesh>` has 2 or more LODs.
9. **Finish:** type `IAI.RevertAll` and read `reverted N` (**R11**; expect 0). Then close the game window.

## 5. The capture smoke (cmd)
10. **Before launching:**
    - Make `<Out>`: `mkdir <Out>`.
    - Close other heavy applications. The capture writes about **1.5 GB** (2,352 frames at 1280×720; home 1,499 MB), so keep **at least 5 GB free** on `<Out>`'s drive.
    - Have **at least 12 GB of memory available** (Task Manager → Performance → Memory → Available). The home session drew about 2.6 GB on a tiny test level (free commit 9.6 → 7.0 GB); a real level draws more.
11. **Launch the capture** (one line):
    ```
    "<ForkRoot>\Engine\Binaries\Win64\UE4Editor.exe" "<Project>\<Name>.uproject" <same level path> -game -windowed -ResX=1280 -ResY=720 -IAIBench -IAICaptureExit -IAICaptureFallback -abslog=<OutFwd>/Runtime.log -ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False -ini:EditorSettings:[/Script/UnrealEd.AnalyticsPrivacySettings]:bSendUsageData=False -ExecCmds="IAI.SetMinScreenCoverage,r.EyeAdaptationQuality 0,r.PostProcessAAQuality 0,r.ScreenPercentage 100,r.DynamicRes.OperationMode 0,r.MotionBlurQuality 0,t.MaxFPS 30,IAI.Capture.C5 <OutFwd> 4242"
    ```
    - This is 094-40's home launch with three changes: no home probe flags, your own `<Out>`, and **no `IAI.SetMinScreenCoverage 0.5`**. The capture therefore runs at the **shipped 6 % selection floor** (FX4); the bare command only prints it.
    - `-IAICaptureFallback` keeps the player's own facing direction for the whole capture, exactly as the home EA_Core run did. The bench tilts the player camera 12° down and holds it there.
    - `IAI.Capture.C5 … 4242` starts the capture once the level's shaders and streaming settle (it waits up to 120 s).
      - It seeds the auto-injector **once** with 4242 and fires blinking, lighting_mismatch, lod_corruption and missing_object at random on visible targets, one at a time, each for 0.4 s.
      - It writes 2,352 frames with labels and masks under `<Out>\epoch01\`, restores what it changed, and exits by itself (`-IAICaptureExit`). 🏠 The session took **125 s** from launch to exit.
    - **Do not touch the mouse or keyboard** and keep the game window in front until it closes itself.
    - **If the log shows `C5 warmup timeout`** (shaders still compiling after 120 s): wait until the window closes, then launch the same line once more. If it times out again, read back `R13 warmup timeout`.
    - **PIE fallback only:** if `-game` failed in §4, open the level, press **Play**, and type the `-ExecCmds` lines one at a time into the Play console. The `-IAIBench` and `-IAICaptureFallback` flags must then be on the editor's command line. At the end the capture asks the whole editor to exit, so save nothing beforehand and expect it to close. This route is unproven at home; put `mode: PIE` in R5.
12. **Read the results (cmd).** Each command prints a line ending in a number, or `yes` / `no`. Type only that number or word.
    ```
    findstr /c:"Capture425 SELF_CHECK" <Out>\Runtime.log
    find /c "IAI.SetMinScreenCoverage: current = 6.00%" <Out>\Runtime.log
    find /c "IAI.Apply 'blinking' -> applied" <Out>\Runtime.log
    find /c "IAI.Apply 'lighting_mismatch' -> applied" <Out>\Runtime.log
    find /c "IAI.Apply 'lod_corruption' -> applied" <Out>\Runtime.log
    find /c "IAI.Apply 'missing_object' -> applied" <Out>\Runtime.log
    dir /b <Out>\epoch01\frame-*.png | find /c /v ""
    dir /b <Out>\epoch01\mask-*.png | find /c /v ""
    findstr /c:"\"written\":2352" <Out>\epoch01\capture.json >nul && echo yes || echo no
    findstr /c:"\"dropped\":0" <Out>\epoch01\capture.json >nul && echo yes || echo no
    findstr /c:"\"result\":\"PASS\"" <Out>\epoch01\labels-receipt.json >nul && echo yes || echo no
    if exist <Out>\epoch01\annotation.json (echo yes) else (echo no)
    find /c "LogExit: Exiting." <Out>\Runtime.log
    ```
    `find /c` prints `---------- <path>: N`. Type only `N`, never the path.

### 5.1 The two self-check lines, field by field
The first command prints two lines, `stage=start` and `stage=300`. Compare every field with the table.
- **pass/fail** fields must equal the expected value. If one differs, finish the visit anyway and type it as `field=value` in R15/R16. It is a finding for chat, not a stop.
- **reading** fields are recorded as they are; there is no pass/fail.
- **constant** fields are printed by the build itself; they prove nothing and are only checked for presence.

| Field | stage=start (home) | stage=300 (home) | Kind / what to do if different |
|---|---|---|---|
| `frames` | 1 | 300 | pass/fail |
| `size` | 1280x720 | 1280x720 | pass/fail (the launch asks 1280×720) |
| `format` | 18 | 18 | **reading** (18 = 10-bit backbuffer at home); type it |
| `RHI` | D3D11 | D3D11 | **reading**; type it. Anything but D3D11 is untested at home |
| `PostRenderViewFamily` | 5 | 316 | pass/fail at stage=300: must be **≥ 300**. Type the number either way |
| `synth` | 0 | 0 | pass/fail |
| `viewport` | SceneViewport | SceneViewport | **reading**: type `standard` if it says SceneViewport, else `custom` (never the name) |
| `client` | GameViewportClient | GameViewportClient | **reading**: type `standard` or `custom` (never the name) |
| `viewport_size` | 1280x720 | 1280x720 | pass/fail: must equal `size` |
| `backbuffer_format` | 18 | 18 | pass/fail: must equal `format` |
| `slate` | 1 | 1 | reading |
| `pie` | 0 | 0 | reading (1 only on the PIE fallback) |
| `pre_ui` | 1 | 1 | pass/fail: 1 means frames are taken before the game's UI |
| `ui_draws` | 4 | 8 | reading at start; pass/fail at stage=300 (**8**) |
| `ui_controls` | 4 | 8 | reading at start; pass/fail at stage=300 (**8**) |
| `ui_positive` | 4 | 8 | reading at start; pass/fail at stage=300 (**8**) |
| `ui_checked` | 1 | 300 | pass/fail: equals `frames` |
| `ui_hits` | 0 | 0 | pass/fail: 0 means no UI marker reached a captured frame |
| `ui_paired` | 1 | 8 | reading at start; pass/fail at stage=300 (**8**) |
| `tick` | OnWorldPostActorTick425 | same | constant |
| `tick_source` | build_constant | same | constant |
| `tick_type` | 2 | 2 | constant |
| `tick_type_source` | build_constant | same | constant |
| `tick_calls` | 1319 | 1630 | **reading**; type the stage=300 number |
| `fixed_step` | 1 | 1 | pass/fail |
| `fixed_dt` | 0.033333333 | 0.033333333 | pass/fail |
| `fixed_rate` | 0 | 0 | reading |
| `fixed_fps` | 30.000000 | 30.000000 | pass/fail |
| `smoothing` | 0 | 0 | reading |
| `census` | bench_off | on | pass/fail |
| `batches` | 0 | 0 | reading |
| `lost` | 0 | 0 | pass/fail |
| `no_pass` | 0 | 0 | pass/fail |
| `polluted` | 0 | 0 | pass/fail |

**If no SELF_CHECK line prints:** type `R15 absent` and `R16 absent`. If `<Out>\Runtime.log` holds a line with `LogAnomalyCapture: Error`, type its message text only (after `Error:`), with any folder path removed, in R15.

### 5.2 The other reads
| Read | Home (EA_Core, 0.5 % bench floor) | Office expectation | Kind / what to do if different |
|---|---|---|---|
| Selection floor line count | — (home used 0.5 %) | **1** | pass/fail. If 0, the floor was not 6 %: type `0` and continue |
| Fires per anomaly (blinking / lighting / LOD / missing) | 22 / 13 / 19 / 15 | each **≥ 1**; 3 or more is what home showed | **reading**. A `0` is a finding (no eligible target at the shipped 6 % floor, a Static light, or no mesh with 2+ LODs), not a stop |
| Frame PNGs | 2352 | **2352** | pass/fail |
| Mask PNGs | 731 | **> 0** if any fire is above 0 | reading (masks exist only on affected frames) |
| `written` 2352 / `dropped` 0 | yes / yes | yes / yes | pass/fail |
| Native receipt `result` PASS | yes | **yes** | pass/fail |
| `annotation.json` present | yes | **yes** | pass/fail |
| `LogExit: Exiting.` count | 1 | **1** | pass/fail (0 means it did not exit normally; check for a crash) |

**Accepted limitation (FX5):** lighting masks leave out any object the game itself already draws into custom depth (outlined or highlighted objects, for example). Those objects still darken on screen but are not in the mask. Nothing to read.

## 6. Stop rules
13. **On a crash:** choose **Close Without Sending**, read back `crash at step N`, and stop. Decline any prompt to send data or feedback.
14. **Never copy files, logs, frames or screenshots off the box.** Type only the read-back. At home it is saved under `D:\IntrusiveAnomalies\_bates_reads`.
15. **Leave the plugin folder and `<Out>` in place** unless told otherwise. `<Out>` holds the dataset and is not under source control. The plugin folder is a plain clone, not a junction.

## 7. Read-back (type these lines)
```
R1  plugin at d1cdc7b: yes/no   (or: existing copy)
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
R12 crash (sections 3-4): none / at step __
R13 capture closed by itself: yes/no   warmup timeout: no/yes/twice
R14 selection floor 6.00% line: __
R15 SELF_CHECK start: pass/fail fields all match: yes / no -> ____ ; format=__ RHI=__ PRVF=__ viewport=standard/custom client=standard/custom slate=__ pie=__ smoothing=__ fixed_rate=__ batches=__
R16 SELF_CHECK 300:   pass/fail fields all match: yes / no -> ____ ; format=__ RHI=__ PRVF=__ tick_calls=__ viewport=standard/custom client=standard/custom slate=__ pie=__ smoothing=__ fixed_rate=__ batches=__
R17 fires: blinking __ lighting_mismatch __ lod_corruption __ missing_object __
R18 frame PNGs __   mask PNGs __
R19 written 2352: yes/no   dropped 0: yes/no
R20 receipt PASS: yes/no
R21 annotation.json present: yes/no
R22 LogExit count: __
R23 crash (section 5): none / at step __
```
