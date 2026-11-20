# Office card — Europa: the four default anomalies (blinking, lod_corruption, missing_texture, corrupted_texture), checked by eye and in one short seeded capture

**What this is:** one office visit. It **replaces card 094-43** (`_reviews\094-43-office-card-europa.md`).
1. Update the plugin copy to `feat/europa-port` at **`c5d0919`** and build it into Europa's project with Europa's fork.
2. Briefly check the four default anomalies by eye.
3. Run one short seeded capture with the **client's default anomalies (blinking, lod_corruption, missing_texture, corrupted_texture)**, and type back the reads in §7. They are numbers and fixed words only.

- 🏠 marks what was **proven at home** on stock UE 4.25.4 (jobs 094-43r and 094-45r, 2026-11-04).
- 🔮 marks a **prediction** for Europa's fork (`G120`: the fork may be modified).
- camera_clipping is **not** on this card. It is opt-in by route only, and its port is the next job.

**What is new since card 094-43:**
- **Two texture anomalies, as on the client's 5.1 build:**
  - `missing_texture` swaps the target's materials for a grey/white checker;
  - `corrupted_texture` swaps them for a solid pink.
  - Both now fire in the default pool. They use two 4.25-authored materials in the plugin's own `Content\Engine425\Materials\`, which the plugin copy brings with it.
- 🏠 On the home Paris level (EA_Core) on D3D11 1280×720, D3D12 1280×720 and D3D12 1279×719:
  - every texture fire changed its object;
  - the label masks matched the changed pixels exactly (IoU 1.000), with 0 changed pixels outside the masks;
  - every revert restored the object exactly.
- The outline fix from card 094-43 (the plugin never turns on custom depth or stencil on the game's objects) is unchanged.

**HARD RULE:** nothing leaves the office box except the lines you type by hand into the read-back block (§7). That means no files, logs, screenshots, clipboard or chat.
- **Never type a project, level, actor, class, folder or studio name into a read-back.**
- Where a value could be a name, the card asks for `standard` / `custom` instead.

**Names used below:**
- `<ForkRoot>` = the folder that holds the fork's `Engine\` folder.
- `<Project>` = the folder that holds Europa's `.uproject`; `<Name>` = that file's name without `.uproject`.
- `<SourceClone>` = the office clone of **this plugin repo** that you copy the plugin from. It is **not** the game's repository, and it sits **outside** `<Project>`.
- `<Mesh>` = an actor name you read on screen. It stays on the office box.
- `<Out>` = a **new, empty** folder **outside** `<Project>`, on a drive with at least 8 GB free, with **no spaces** in its path. `<OutFwd>` = the same path written with forward slashes (for example `D:/IA_Office/cap03`).

## 1. Before you go (home) — nothing to do
- **Already pushed:** Code pushed `feat/europa-port` = **`c5d0919`** to origin on 2026-11-04 at 18:22 IST, a fast-forward from `8df75de`.
  - `git ls-remote origin feat/europa-port` returned `c5d0919e5074…`.
  - No tags, no backup refs, no force. Just fetch and check it out (§2).
- **Pre-push scan:** clean (`_reviews\094-45r-stage1-prepush-scan.md`). One carried, non-secret item: your Windows profile name appears in build-receipt paths, as it already did on origin.
- **What `c5d0919` is:** card 094-43's source plus the release-line texture anomalies, with their 4.25 materials, in the default pool.
  - Bench-only test code (`-IAIBench`) is inert at the office.
  - The home test level's lighting and surface helper lives in the home project only, never in the plugin.

## 2. Get the plugin onto the office box (cmd)
1. **Update the source clone.** R1 is read here, never from the game's repo or the copied folder.
   - **If you have no `<SourceClone>` yet:** make one outside `<Project>`, using the URL your office clone of this repo uses:
     ```
     git clone --branch feat/europa-port --single-branch <that URL> "<SourceClone>"
     ```
   - **Then, in every case:**
     ```
     git -C "<SourceClone>" fetch origin feat/europa-port
     git -C "<SourceClone>" checkout --detach c5d0919
     git -C "<SourceClone>" rev-parse HEAD
     ```
   - The last line must print **`c5d0919e5074404d26da8c16ccf845b360bde383`**. For **R1**, type `yes` if it starts with `c5d0919`.
2. **Find the existing copy:**
   ```
   dir /s /b "<Project>\Plugins\AnomalyInjector.uplugin"
   ```
   - **If it finds one file, `<Project>\Plugins\AnomalyInjector\AnomalyInjector.uplugin` (an earlier visit's copy):** move that whole `AnomalyInjector` folder **out of `<Project>`** (for example next to `<Out>`). Do not just move it aside inside `Plugins\`: UE scans `Plugins\` recursively, so a second copy anywhere under it is a fatal duplicate.
   - **If it says File Not Found:** go on.
   - **Anything else:** stop and read back `R1 existing copy`.
3. **Copy the plugin in.** It must be a plain copy, with no git data and no old build output. The copy includes `Content\Engine425\Materials\`, the two texture materials.
   ```
   robocopy "<SourceClone>" "<Project>\Plugins\AnomalyInjector" /E /XD .git Binaries Intermediate
   echo %ERRORLEVEL%
   ```
   - Exit code `1` means files were copied. Anything `8` or above is a failure: stop and read back `R1 copy failed`.
   - **Keep the copied folder out of the game's repository.** Do not add, commit, check in or vendor it. If the game's source control lists it as new, leave it untracked or unchecked-out.
4. **Nothing to switch on.** There is no `.uproject` edit; the plugin detects 4.25 by itself.

## 3. Build (cmd; close Europa's editor first)
5. **Run:**
   ```
   "<ForkRoot>\Engine\Build\BatchFiles\Build.bat" <Name>Editor Win64 Development -Project="<Project>\<Name>.uproject" -WaitMutex -NoEngineChanges
   echo %ERRORLEVEL%
   ```
   - 🏠 This plugin source built Editor and Game Development on stock 4.25 with 0 errors and 0 link errors (VS2017 14.16, SDK 10.0.17763).
6. **Read the results:**
   - From the build's `Anomaly profile: engine=… version=…` line, type only the `engine=` value and the **numbers** of `version=` (**R3**). If letters follow the numbers, type `+text` instead of them.
   - Read the exit code (**R2**).
   - **If R2 is not `0`:** find the first line that says `error` and names an `Anomaly…` module. Type it starting at `Source\`, never the folder part before it. If it has no `Source\`, type only its message text, with any folder path removed (**R4**). Then **stop here** and type the rest as `n/a`.

## 4. Quick eyeball (it also compiles the two new materials' shaders)
7. **Launch:**
   ```
   "<ForkRoot>\Engine\Binaries\Win64\UE4Editor.exe" "<Project>\<Name>.uproject" <level path, e.g. /Game/…/SomeLevel> -game -windowed -ResX=1280 -ResY=720 -IAIBench -log
   ```
   - Use the **same ordinary level as on earlier visits**, and use the same level again in §5.
   - The first launch may pause while it compiles shaders. Wait for the level to appear.
8. **Open the console (the ` key):**
   - type `t.MaxFPS 30`;
   - then `IAI.ListAnomalies` and read `--- N anomaly(ies) ---` (**R6**; expect **6**);
   - then `IAI.DumpVisible`.

   Pick a close, plainly visible, ordinary solid mesh (not glass or foliage) as `<Mesh>`.
9. **Check by eye:**

| # | Apply | See while active | Revert, then see |
|---|---|---|---|
| a | `IAI.Apply blinking =<Mesh> 12` | `<Mesh>` vanishes and returns about every half second | `IAI.Revert blinking`: it stays visible |
| b | `IAI.Apply lod_corruption =<Mesh> 64` | `<Mesh>` turns visibly coarser; the log says `forced LOD a of b`. If `b` is 1, pick another mesh | `IAI.Revert lod_corruption`: original detail |
| c | `IAI.Apply missing_texture =<Mesh>` | `<Mesh>`'s whole surface becomes a **grey/white checker** | `IAI.Revert missing_texture`: its original surface |
| d | `IAI.Apply corrupted_texture =<Mesh>` | `<Mesh>`'s whole surface becomes **solid pink** | `IAI.Revert corrupted_texture`: its original surface |

   - For c and d, type `checker`, `pink`, `grey` (plain flat grey, which means the material fell back to the engine default) or `none` (**R7b**).
10. **Finish:**
    - type `IAI.RevertAll` and read `reverted N` (**R9**; expect 0);
    - close the game window;
    - then, in cmd, count the default-material warnings in that session's log, which is the newest `.log` under `<Project>\Saved\Logs`. The command prints one number:
      ```
      powershell -NoProfile -Command "$f=Get-ChildItem '<Project>\Saved\Logs\*.log' | Sort-Object LastWriteTime -Descending | Select-Object -First 1; @(Select-String -Path $f.FullName -SimpleMatch 'Default Material will be used').Count"
      ```
      Type only the number (**R9b**; 🏠 0).

## 5. The capture (cmd)
11. **Before launching:**
    - Make `<Out>`: `mkdir <Out>`.
    - Keep at least **8 GB free** on `<Out>`'s drive, and have at least **12 GB of memory available**.
12. **Launch, and run it exactly as written:**
    - Paste the line into a plain `cmd` window yourself.
    - Do **not** run it through any other launcher, script or office tool, and do not shorten or reorder anything.
    ```
    "<ForkRoot>\Engine\Binaries\Win64\UE4Editor.exe" "<Project>\<Name>.uproject" <same level path> -game -windowed -ResX=1280 -ResY=720 -d3d12 -IAIBench -IAICaptureExit -IAICaptureFallback -abslog=<OutFwd>/Runtime.log -ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False -ini:EditorSettings:[/Script/UnrealEd.AnalyticsPrivacySettings]:bSendUsageData=False -ExecCmds="IAI.SetMinScreenCoverage,r.EyeAdaptationQuality 0,r.PostProcessAAQuality 0,r.ScreenPercentage 100,r.DynamicRes.OperationMode 0,r.MotionBlurQuality 0,t.MaxFPS 30,IAI.Capture.C5 <OutFwd> 4242"
    ```
    - **What it does:**
      - It uses the **default routes: blinking, lod_corruption, missing_texture and corrupted_texture**. There is no route list on the line.
      - It runs at the shipped 6 % selection floor. `IAI.SetMinScreenCoverage` with no number only prints the floor.
      - It seeds once with 4242, then fires the four anomalies one at a time on visible targets.
      - It writes 2,352 frames with labels and masks under `<Out>\epoch01\`, restores everything it changed, and exits by itself.
    - **Do not touch the mouse or keyboard,** and keep the game window in front until it closes.
    - **Watch the game window the whole time:** do any objects show the game's outline or highlight effect (**R15**)?
      - An object that is solid pink or grey/white-checkered for a moment is the anomaly itself, not an outline.
      - 🔮 There should be no outlines, unless the game itself outlines something on its own (for example a player-interaction highlight). If so, note `some` and carry on.
    - **If the log shows `C5 warmup timeout`:** wait until the window closes, then launch the same line once more. If it times out again, read back `R13 warmup timeout`.
13. **Read the results (cmd).** Each command prints a line ending in a number, or `yes` / `no`, or the words named. Type only those.
    ```
    findstr /c:"Capture425 SELF_CHECK" <Out>\Runtime.log
    findstr /c:"Capture425 routes=" <Out>\Runtime.log
    findstr /c:"Capture425 host custom-depth" <Out>\Runtime.log
    find /c "C5 refused" <Out>\Runtime.log
    find /c "IAI.SetMinScreenCoverage: current = 6.00%" <Out>\Runtime.log
    find /c "IAI.Apply 'blinking' -> applied" <Out>\Runtime.log
    find /c "IAI.Apply 'lod_corruption' -> applied" <Out>\Runtime.log
    find /c "IAI.Apply 'missing_texture' -> applied" <Out>\Runtime.log
    find /c "IAI.Apply 'corrupted_texture' -> applied" <Out>\Runtime.log
    find /c "texture: revert of" <Out>\Runtime.log
    find /c "residual=0" <Out>\Runtime.log
    find /c "Default Material will be used" <Out>\Runtime.log
    dir /b <Out>\epoch01\frame-*.png | find /c /v ""
    dir /b <Out>\epoch01\mask-*.png | find /c /v ""
    findstr /c:"\"written\":2352" <Out>\epoch01\capture.json >nul && echo yes || echo no
    findstr /c:"\"dropped\":0" <Out>\epoch01\capture.json >nul && echo yes || echo no
    findstr /c:"\"result\":\"PASS\"" <Out>\epoch01\labels-receipt.json >nul && echo yes || echo no
    if exist <Out>\epoch01\annotation.json (echo yes) else (echo no)
    find /c "LogExit: Exiting." <Out>\Runtime.log
    ```
    - `find /c` prints `---------- <path>: N`. Type only `N`, never the path.
    - From the `routes=` and `host custom-depth` lines, type only the text **after** `Capture425 `. That text holds only route words, `none`/`reader`/`unknown` and numbers.
14. **Read the private-pass receipts (cmd).** Each command prints one line of words and numbers.
    ```
    powershell -NoProfile -Command "$j=Get-Content '<Out>\epoch01\private-availability.json' -Raw | ConvertFrom-Json; $u=@($j.components | Where-Object {$_.unavailable} | Group-Object reason | ForEach-Object {$_.Name+'='+$_.Count}) -join ','; 'enabled=' + $j.enabled + ' verified=' + $j.verified + ' unavailable=' + $j.unavailable + ' reasons=' + $u + ' host_stencil_writes=' + $j.host_stencil_writes"
    powershell -NoProfile -Command "$r=@(Get-Content '<Out>\epoch01\private-pass.jsonl' | ForEach-Object {$_ | ConvertFrom-Json}); 'rows=' + $r.Count + ' gpu_mean=' + [math]::Round(($r | Measure-Object gpu_ms -Average).Average,3) + ' rt_mean=' + [math]::Round(($r | Measure-Object cpu_submit_ms -Average).Average,3) + ' blocking=' + @($r | Where-Object {$_.blocking_wait}).Count + ' host_depth=' + @($r | Where-Object {$_.host_depth_writes -ne 0}).Count + ' host_stencil=' + @($r | Where-Object {$_.host_stencil_writes -ne 0}).Count + ' max_objects=' + ($r | Measure-Object objects -Maximum).Maximum"
    powershell -NoProfile -Command "$b=Get-Content '<Out>\epoch01\depth-before.json' -Raw; $a=Get-Content '<Out>\epoch01\depth-after.json' -Raw; $j=$b | ConvertFrom-Json; 'candidates=' + $j.candidates + ' on=' + $j.on_count + ' tagged=' + $j.tagged_components + ' unexpected=' + $j.unexpected + ' before_equals_after=' + ($b -eq $a)"
    powershell -NoProfile -Command "$n=0;$p=0;$t=0; Get-Content '<Out>\epoch01\labels.jsonl' | ForEach-Object { $o=$_ | ConvertFrom-Json; $n++; if($o.census.polluted_batches -ne 0){$p++}; if($o.custom_depth.tagged_components -gt 0){$t++} }; 'rows=' + $n + ' polluted_rows=' + $p + ' tagged_rows=' + $t"
    powershell -NoProfile -Command "$a=Get-Content '<Out>\epoch01\annotation.json' -Raw | ConvertFrom-Json; $g=$a.anomalies | Group-Object anomaly_type | ForEach-Object { $_.Name + '=' + $_.Count + '/' + (($_.Group | ForEach-Object { $_.affected_frames.frame_count } | Measure-Object -Sum).Sum) }; 'events=' + @($a.anomalies).Count + ' ' + ($g -join ' ')"
    ```
    - The fourth command reads 2,352 rows and may take a minute.
    - The unavailable `reasons` are fixed plugin words (`component_type`, `no_scene_proxy`, `material_domain`, `translucent`, `tessellation`, `no_bound_proxy`, `shader_or_dynamic_binding`), never names.
    - The last command prints `type=events/frames` per anomaly type, using fixed type words only.

### 5.1 The two self-check lines
The first command in step 13 prints `stage=start` and `stage=300`. Type the readings and compare the pass/fail fields.

| Field | 🏠 home (D3D12 1280×720) | Kind / what to type |
|---|---|---|
| `size` | 1280x720 | **reading**: type it |
| `viewport_size` | = `size` | pass/fail: must equal `size` |
| `format` | 18 (10-bit backbuffer) | **reading** |
| `backbuffer_format` | = `format` | pass/fail: must equal `format` |
| `RHI` | D3D12 | **reading**: type it |
| `client` | GameViewportClient | **reading**: type `standard` if it says GameViewportClient, else `custom` (never the name) |
| `viewport` | SceneViewport | **reading**: `standard` / `custom` |
| `frames` | 1 / 300 | pass/fail |
| `PostRenderViewFamily` | ≥ 300 at stage=300 | pass/fail; type the number |
| `pre_ui` / `ui_hits` | 1 / 0 | pass/fail |
| `ui_paired` | 8 at stage=300 | pass/fail |
| `fixed_step` / `fixed_fps` | 1 / 30.000000 | pass/fail |
| `census` | on at stage=300 | pass/fail |
| `lost` / `no_pass` | 0 / 0 | pass/fail |
| **`polluted`** | **0** | **pass/fail; type the number either way** |
| `tick_calls` | reading | type the stage=300 number |

**If no SELF_CHECK line prints:** type `absent` for R16/R17. If `<Out>\Runtime.log` holds `LogAnomalyCapture: Error`, type its message text only, with any folder path removed.

### 5.2 What the other reads should say
| Read | 🏠 expectation | If different |
|---|---|---|
| routes | `routes=blinking,lod_corruption,missing_texture,corrupted_texture` | a finding; type what it says |
| host custom-depth | 🔮 `reader readers=1 or more … tagged=0 bench_bypass=0` (the game's outline reads custom depth) | `tagged` must be 0 |
| `C5 refused` count | 0 | if 1, the capture refused: type the host line and stop |
| 6.00 % floor line | 1 | 0 means the floor was changed |
| fires per type | each ≥ 1 (🏠 EA_Core: 15 / 19 / 22 / 13) | a 0 is a finding (no eligible target), not a stop |
| texture reverts / `residual=0` | 🏠 equal to the two texture fire counts added together | `residual=0` below the revert count means an object kept a test material |
| `Default Material will be used` | 0 | above 0 means a texture rendered grey: a finding |
| frame PNGs | 2352 | pass/fail |
| mask PNGs | > 0 | reading |
| written / dropped / receipt / annotation | yes / yes / yes / yes | pass/fail |
| availability | `enabled=True`, `unavailable` is a reading (🏠 0), `host_stencil_writes=0` | unavailable objects are left out of counting and labels, never drawn the old way |
| private pass | `blocking=0 host_depth=0 host_stencil=0`; 🏠 D3D12 `gpu_mean` ≈ 0.16, `rt_mean` ≈ 1.6–1.7 ms | readings |
| custom-depth state | `tagged=0 unexpected=0 before_equals_after=True`; `on` is the game's own count (a reading) | `tagged` above 0 is a finding |
| label rows | `rows=2352 polluted_rows=0 tagged_rows=0` | pass/fail |
| events by type | 🏠 EA_Core: `blink=15/90 lod_corruption=19/247 missing_texture=22/286 corrupted_texture=13/157` | readings (they depend on the level) |
| LogExit count | 1 | 0: check for a crash |

## 6. Stop rules
15. **On a crash:** choose **Close Without Sending**, read back `crash at step N`, and stop. Decline any prompt to send data or feedback.
16. **Never copy files, logs, frames or screenshots off the box.** Type only the read-back. At home it is saved under `D:\IntrusiveAnomalies\_bates_reads`.
17. **Leave the plugin folder, the moved old copy and `<Out>` in place** unless told otherwise. None of them goes into the game's repository.

## 7. Read-back (type these lines)
```
R1  source clone at c5d0919: yes/no   old copy moved out: yes/none   (or: existing copy / copy failed)
R2  build exit code: __
R3  profile: engine=__ version=__
R4  first Anomaly error from Source\ : ____ (or: none)
R5  game window up, no crash: yes/no
R6  anomaly types: __
R7  blinking: applied y/n, visible y/n, reverted y/n
R7b missing_texture: shows checker/pink/grey/none, reverted y/n   corrupted_texture: shows checker/pink/grey/none, reverted y/n
R8  lod_corruption: LOD __ of __, visible y/n, reverted y/n
R9  RevertAll reverted: __
R9b eyeball log "Default Material will be used" count: __
R10 crash (sections 3-4): none / at step __
R11 routes: ____
R12 host custom-depth: ____   C5 refused count: __
R13 capture closed by itself: yes/no   warmup timeout: no/yes/twice
R14 selection floor 6.00% line: __
R15 outlines seen on objects during capture: none / some / every frame
R16 SELF_CHECK start: size=__ viewport_size=__ format=__ backbuffer_format=__ RHI=__ client=standard/custom viewport=standard/custom pass/fail all match: yes / no -> ____
R17 SELF_CHECK 300:   size=__ RHI=__ client=standard/custom PRVF=__ tick_calls=__ census=__ lost=__ no_pass=__ polluted=__ pass/fail all match: yes / no -> ____
R18 fires: blinking __ lod_corruption __ missing_texture __ corrupted_texture __
R18b texture reverts __   residual=0 lines __   capture "Default Material will be used" __
R19 frame PNGs __   mask PNGs __
R20 written 2352: yes/no   dropped 0: yes/no   receipt PASS: yes/no   annotation.json: yes/no
R21 availability: enabled=__ verified=__ unavailable=__ reasons=____ host_stencil_writes=__
R22 private pass: rows=__ gpu_mean=__ rt_mean=__ blocking=__ host_depth=__ host_stencil=__ max_objects=__
R23 custom depth: candidates=__ on=__ tagged=__ unexpected=__ before_equals_after=__
R24 label rows: rows=__ polluted_rows=__ tagged_rows=__
R24b events by type: ____
R25 LogExit count: __
R26 crash (section 5): none / at step __
```
