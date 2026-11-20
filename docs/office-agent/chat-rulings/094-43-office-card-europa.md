# Office card — Europa: check the outline leak is gone (private ID/depth pass), with one short seeded capture

**What this is:** one office visit, replacing the capture half of card 094-40.
1. Update the plugin copy to `feat/europa-port` at **`93613ae`** and build it into Europa's project with Europa's fork.
2. Briefly check two anomalies by eye.
3. Run one short seeded capture with the **client's default anomalies (blinking and lod_corruption)**. Watch whether the game's outline effect shows on objects, and type back the reads in §7.

- 🏠 marks what was **proven at home** on stock UE 4.25.4 (job 094-43r, 2026-11-04).
- 🔮 marks a **prediction** for Europa's fork (`G120`: the fork may be modified).

**Why:** on 11-02 the capture lit up the game's own outline effect on the objects it counted and labelled (`polluted=1`, outlines on objects every frame). The plugin now counts and masks objects in its **own** depth and ID textures and **never turns on custom depth or stencil** on the game's objects. 🏠 On a home test level with an outline effect: 0 new outline pixels in 2,352 frames, labels identical to the old method, `polluted=0`, 0 tagged components, on D3D11 and D3D12.

**HARD RULE:** nothing leaves the office box except the lines you type by hand into the read-back block (§7). No files, logs, screenshots, clipboard or chat. **Never type a project, level, actor, class, folder or studio name into a read-back.** Where a value could be a name, the card asks for `standard` / `custom` instead.

**Names used below:**
- `<ForkRoot>` = the folder that holds the fork's `Engine\` folder.
- `<Project>` = the folder that holds Europa's `.uproject`; `<Name>` = that file's name without `.uproject`.
- `<SourceClone>` = the office clone of **this plugin repo** that you copy the plugin from. It is **not** the game's repository and is **outside** `<Project>`.
- `<Mesh>` = an actor name you read on screen. It stays on the office box.
- `<Out>` = a **new, empty** folder **outside** `<Project>`, on a drive with at least 8 GB free. Use a path with **no spaces**. `<OutFwd>` = the same path written with forward slashes (for example `D:/IA_Office/cap02`).

## 1. Before you go (home) — nothing to do
- **Already pushed:** Code pushed `feat/europa-port` = **`93613ae`** to origin on 2026-11-04 at about 05:10 IST, a fast-forward from `d1cdc7b`. `git ls-remote origin feat/europa-port` returned `93613ae86213…`. No tags, no backup refs, no force. Just fetch and check it out (§2).
- **Pre-push scan:** clean (`_reviews\094-43r-prepush-scan.md`). One carried, non-secret item: your Windows profile name appears in a few paths, as it already did on origin.
- **What `93613ae` is:** the source built and qualified at home in 094-43r: the private ID/depth pass (FZ2), the default routes, any-size capture and D3D12 qualification (FZ7).

## 2. Get the plugin onto the office box (cmd)
1. **Update the source clone (fix: R1 is read here, never from the game's repo or the copied folder):**
   - **If you have no `<SourceClone>` yet:** make one outside `<Project>`, using the URL your office clone of this repo uses:
     ```
     git clone --branch feat/europa-port --single-branch <that URL> "<SourceClone>"
     ```
   - **Then, in every case:**
     ```
     git -C "<SourceClone>" fetch origin feat/europa-port
     git -C "<SourceClone>" checkout --detach 93613ae
     git -C "<SourceClone>" rev-parse HEAD
     ```
   - The last line must print **`93613ae86213f3326bdaf4a7c837a4a84b0008ac`** (**R1**: type `yes` if it starts with `93613ae`).
2. **Find the existing copy:**
   ```
   dir /s /b "<Project>\Plugins\AnomalyInjector.uplugin"
   ```
   - **One file, `<Project>\Plugins\AnomalyInjector\AnomalyInjector.uplugin`, from the 11-02 visit:** move that whole `AnomalyInjector` folder **out of `<Project>`** (for example next to `<Out>`), not aside inside `Plugins\`. UE scans `Plugins\` recursively, so a second copy anywhere under it is a fatal duplicate.
   - **File Not Found:** go on.
   - **Anything else:** stop and read back `R1 existing copy`.
3. **Copy the plugin in (a plain copy, no git data, no old build output):**
   ```
   robocopy "<SourceClone>" "<Project>\Plugins\AnomalyInjector" /E /XD .git Binaries Intermediate
   echo %ERRORLEVEL%
   ```
   - Exit code `1` means files were copied; anything `8` or above is a failure (stop, read back `R1 copy failed`).
   - **Fix: keep the copied folder out of the game's repository.** Do not add, commit, check in or vendor it. If the game's source control lists it as new, leave it untracked or unchecked-out.
4. **Nothing to switch on.** No `.uproject` edit. The plugin detects 4.25 itself.

## 3. Build (cmd; close Europa's editor first)
5. **Run:**
   ```
   "<ForkRoot>\Engine\Build\BatchFiles\Build.bat" <Name>Editor Win64 Development -Project="<Project>\<Name>.uproject" -WaitMutex -NoEngineChanges
   echo %ERRORLEVEL%
   ```
   - 🏠 This source built Editor and Game Development on stock 4.25 with 0 errors and 0 link errors (VS2017 14.16, SDK 10.0.17763).
   - 🔮 The fork's renderer headers match stock 4.25: the new pass uses only public 4.25 renderer and material headers.
6. **Read the results:**
   - From the build's `Anomaly profile: engine=… version=…` line, type only the `engine=` value and the **numbers** of `version=` (**R3**). If letters follow the numbers, type `+text` instead of them.
   - Read the exit code (**R2**).
   - **If R2 is not `0`:** find the first line that says `error` and names an `Anomaly…` module. Type it starting at `Source\`, never the folder part before it; if it has no `Source\`, type only its message text with any folder path removed (**R4**). Then **stop here** and type the rest as `n/a`.

## 4. Quick eyeball (it also warms the level's shaders)
7. **Launch:**
   ```
   "<ForkRoot>\Engine\Binaries\Win64\UE4Editor.exe" "<Project>\<Name>.uproject" <level path, e.g. /Game/…/SomeLevel> -game -windowed -ResX=1280 -ResY=720 -IAIBench -log
   ```
   - Use the **same ordinary level as on 11-02**, and the same level in §5.
8. **Console (the ` key):** type `t.MaxFPS 30`, then `IAI.ListAnomalies` (read `--- N anomaly(ies) ---`, **R6**, expect 4), then `IAI.DumpVisible`. Pick a close, plainly visible mesh as `<Mesh>`.
9. **Check by eye:**

| # | Apply | See while active | Revert, then see |
|---|---|---|---|
| a | `IAI.Apply blinking =<Mesh> 12` | `<Mesh>` vanishes and returns about every half second | `IAI.Revert blinking` → stays visible |
| b | `IAI.Apply lod_corruption =<Mesh> 64` | `<Mesh>` turns visibly coarser; the log says `forced LOD a of b`. If `b` is 1, pick another mesh | `IAI.Revert lod_corruption` → original detail |

10. **Finish:** type `IAI.RevertAll`, read `reverted N` (**R9**; expect 0), and close the game window.

## 5. The capture (cmd)
11. **Before launching:**
    - Make `<Out>`: `mkdir <Out>`.
    - Keep at least **8 GB free** on `<Out>`'s drive. On 11-02 the game forced 1920×1080, which needs about twice the 1.5 GB a 1280×720 capture takes.
    - Have at least **12 GB of memory available**.
12. **Launch, and run it exactly as written** (fix):
    - Paste the line into a plain `cmd` window yourself. Do **not** run it through any other launcher, script or office tool, and do not shorten or reorder anything.
    - On 11-02 a tool rewrote the `-ExecCmds` text and changed the selection floor. Stock 4.25 runs every command of a quoted `-ExecCmds`.
    ```
    "<ForkRoot>\Engine\Binaries\Win64\UE4Editor.exe" "<Project>\<Name>.uproject" <same level path> -game -windowed -ResX=1280 -ResY=720 -d3d12 -IAIBench -IAICaptureExit -IAICaptureFallback -abslog=<OutFwd>/Runtime.log -ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False -ini:EditorSettings:[/Script/UnrealEd.AnalyticsPrivacySettings]:bSendUsageData=False -ExecCmds="IAI.SetMinScreenCoverage,r.EyeAdaptationQuality 0,r.PostProcessAAQuality 0,r.ScreenPercentage 100,r.DynamicRes.OperationMode 0,r.MotionBlurQuality 0,t.MaxFPS 30,IAI.Capture.C5 <OutFwd> 4242"
    ```
    - **Differences from 094-40:**
      - `-d3d12`, the RHI Europa used on 11-02. 🏠 D3D12 was qualified at 1280×720 and at the odd size 1279×719.
      - No route list, so the capture uses the **default routes, blinking and lod_corruption** (lighting_mismatch and missing_object are test-only now).
    - **What it does:** the capture runs at the shipped 6 % selection floor (`IAI.SetMinScreenCoverage` with no number only prints it). It seeds once with 4242, fires blinking and lod_corruption one at a time on visible targets, writes 2,352 frames with labels and masks under `<Out>\epoch01\`, restores what it changed, and exits by itself.
    - **Do not touch the mouse or keyboard,** and keep the game window in front until it closes.
    - **Watch the game window the whole time:** do any objects show the game's outline or highlight effect (**R15**)? On 11-02 they did, changing every frame. 🔮 Now there should be none, unless the game itself outlines something on its own (for example a player-interaction highlight). Note `some` and carry on if so.
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
    dir /b <Out>\epoch01\frame-*.png | find /c /v ""
    dir /b <Out>\epoch01\mask-*.png | find /c /v ""
    findstr /c:"\"written\":2352" <Out>\epoch01\capture.json >nul && echo yes || echo no
    findstr /c:"\"dropped\":0" <Out>\epoch01\capture.json >nul && echo yes || echo no
    findstr /c:"\"result\":\"PASS\"" <Out>\epoch01\labels-receipt.json >nul && echo yes || echo no
    if exist <Out>\epoch01\annotation.json (echo yes) else (echo no)
    find /c "LogExit: Exiting." <Out>\Runtime.log
    ```
    `find /c` prints `---------- <path>: N`. Type only `N`, never the path. From the `routes=` and `host custom-depth` lines, type only the text **after** `Capture425 `; it holds only route words, `none`/`reader`/`unknown` and numbers.
14. **Read the private-pass receipts (cmd; each prints one line of words and numbers):**
    ```
    powershell -NoProfile -Command "$j=Get-Content '<Out>\epoch01\private-availability.json' -Raw | ConvertFrom-Json; $u=@($j.components | Where-Object {$_.unavailable} | Group-Object reason | ForEach-Object {$_.Name+'='+$_.Count}) -join ','; 'enabled=' + $j.enabled + ' verified=' + $j.verified + ' unavailable=' + $j.unavailable + ' reasons=' + $u + ' host_stencil_writes=' + $j.host_stencil_writes"
    powershell -NoProfile -Command "$r=@(Get-Content '<Out>\epoch01\private-pass.jsonl' | ForEach-Object {$_ | ConvertFrom-Json}); 'rows=' + $r.Count + ' gpu_mean=' + [math]::Round(($r | Measure-Object gpu_ms -Average).Average,3) + ' rt_mean=' + [math]::Round(($r | Measure-Object cpu_submit_ms -Average).Average,3) + ' blocking=' + @($r | Where-Object {$_.blocking_wait}).Count + ' host_depth=' + @($r | Where-Object {$_.host_depth_writes -ne 0}).Count + ' host_stencil=' + @($r | Where-Object {$_.host_stencil_writes -ne 0}).Count + ' max_objects=' + ($r | Measure-Object objects -Maximum).Maximum"
    powershell -NoProfile -Command "$b=Get-Content '<Out>\epoch01\depth-before.json' -Raw; $a=Get-Content '<Out>\epoch01\depth-after.json' -Raw; $j=$b | ConvertFrom-Json; 'candidates=' + $j.candidates + ' on=' + $j.on_count + ' tagged=' + $j.tagged_components + ' unexpected=' + $j.unexpected + ' before_equals_after=' + ($b -eq $a)"
    powershell -NoProfile -Command "$n=0;$p=0;$t=0; Get-Content '<Out>\epoch01\labels.jsonl' | ForEach-Object { $o=$_ | ConvertFrom-Json; $n++; if($o.census.polluted_batches -ne 0){$p++}; if($o.custom_depth.tagged_components -gt 0){$t++} }; 'rows=' + $n + ' polluted_rows=' + $p + ' tagged_rows=' + $t"
    ```
    The last command reads 2,352 rows and may take a minute. The unavailable `reasons` are fixed plugin words (`component_type`, `no_scene_proxy`, `material_domain`, `translucent`, `tessellation`, `no_bound_proxy`, `shader_or_dynamic_binding`), never names.

### 5.1 The two self-check lines
The first command in step 13 prints `stage=start` and `stage=300`. Type the readings; compare the pass/fail fields.

| Field | 🏠 home (D3D12 1280×720) | Kind / what to type |
|---|---|---|
| `size` | 1280x720 | **reading**: type it (11-02 forced 1920x1080) |
| `viewport_size` | = `size` | pass/fail: must equal `size` |
| `format` | 18 (10-bit backbuffer) | **reading** |
| `backbuffer_format` | = `format` | pass/fail: must equal `format` |
| `RHI` | D3D12 | **reading**: type it |
| `client` | GameViewportClient | **reading**: type `standard` if it says GameViewportClient, else `custom` (never the name; 11-02 read `custom`) |
| `viewport` | SceneViewport | **reading**: `standard` / `custom` |
| `frames` | 1 / 300 | pass/fail |
| `PostRenderViewFamily` | ≥ 300 at stage=300 | pass/fail; type the number |
| `pre_ui` / `ui_hits` | 1 / 0 | pass/fail |
| `ui_paired` | 8 at stage=300 | pass/fail |
| `fixed_step` / `fixed_fps` | 1 / 30.000000 | pass/fail |
| `census` | on at stage=300 | pass/fail |
| `lost` / `no_pass` | 0 / 0 | pass/fail |
| **`polluted`** | **0** | **pass/fail; type the number either way (11-02 read 1 at stage=300)** |
| `tick_calls` | reading | type the stage=300 number |

**If no SELF_CHECK line prints:** type `absent` for R16/R17. If `<Out>\Runtime.log` holds `LogAnomalyCapture: Error`, type its message text only, with any folder path removed.

### 5.2 What the other reads should say
| Read | 🏠 expectation | If different |
|---|---|---|
| routes | `routes=blinking,lod_corruption` | a finding; type what it says |
| host custom-depth | 🔮 `reader readers=1 or more … tagged=0 bench_bypass=0` (the game's outline reads custom depth) | `tagged` must be 0 |
| `C5 refused` count | 0 | if 1, the capture refused: type the host line and stop |
| 6.00 % floor line | 1 | 0 means the floor was changed |
| fires (blinking / lod_corruption) | each ≥ 1 | a 0 is a finding (no eligible target), not a stop |
| frame PNGs | 2352 | pass/fail |
| mask PNGs | > 0 | reading |
| written / dropped / receipt / annotation | yes / yes / yes / yes | pass/fail |
| availability | `enabled=True`, `unavailable` = a reading (🏠 0), `host_stencil_writes=0` | unavailable objects are left out of counting and labels, never drawn the old way |
| private pass | `blocking=0 host_depth=0 host_stencil=0`; 🏠 D3D12 `gpu_mean` ≈ 0.16, `rt_mean` ≈ 1.6–1.7 ms | readings |
| custom-depth state | `tagged=0 unexpected=0 before_equals_after=True`; `on` is the game's own count (a reading) | `tagged` above 0 is a finding |
| label rows | `rows=2352 polluted_rows=0 tagged_rows=0` | pass/fail |
| LogExit count | 1 | 0: check for a crash |

## 6. Stop rules
15. **On a crash:** choose **Close Without Sending**, read back `crash at step N`, and stop. Decline any prompt to send data or feedback.
16. **Never copy files, logs, frames or screenshots off the box.** Type only the read-back. At home it is saved under `D:\IntrusiveAnomalies\_bates_reads`.
17. **Leave the plugin folder, the moved old copy and `<Out>` in place** unless told otherwise. None of them goes into the game's repository.

## 7. Read-back (type these lines)
```
R1  source clone at 93613ae: yes/no   old copy moved out: yes/none   (or: existing copy / copy failed)
R2  build exit code: __
R3  profile: engine=__ version=__
R4  first Anomaly error from Source\ : ____ (or: none)
R5  game window up, no crash: yes/no
R6  anomaly types: __
R7  blinking: applied y/n, visible y/n, reverted y/n
R8  lod_corruption: LOD __ of __, visible y/n, reverted y/n
R9  RevertAll reverted: __
R10 crash (sections 3-4): none / at step __
R11 routes: ____
R12 host custom-depth: ____   C5 refused count: __
R13 capture closed by itself: yes/no   warmup timeout: no/yes/twice
R14 selection floor 6.00% line: __
R15 outlines seen on objects during capture: none / some / every frame
R16 SELF_CHECK start: size=__ viewport_size=__ format=__ backbuffer_format=__ RHI=__ client=standard/custom viewport=standard/custom pass/fail all match: yes / no -> ____
R17 SELF_CHECK 300:   size=__ RHI=__ client=standard/custom PRVF=__ tick_calls=__ census=__ lost=__ no_pass=__ polluted=__ pass/fail all match: yes / no -> ____
R18 fires: blinking __ lod_corruption __
R19 frame PNGs __   mask PNGs __
R20 written 2352: yes/no   dropped 0: yes/no   receipt PASS: yes/no   annotation.json: yes/no
R21 availability: enabled=__ verified=__ unavailable=__ reasons=____ host_stencil_writes=__
R22 private pass: rows=__ gpu_mean=__ rt_mean=__ blocking=__ host_depth=__ host_stencil=__ max_objects=__
R23 custom depth: candidates=__ on=__ tagged=__ unexpected=__ before_equals_after=__
R24 label rows: rows=__ polluted_rows=__ tagged_rows=__
R25 LogExit count: __
R26 crash (section 5): none / at step __
```
