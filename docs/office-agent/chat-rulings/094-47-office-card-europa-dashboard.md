# Europa 4.25 Development + AnomDash — handover card

**Published 2026-11-06.** Both feature branches below are published and confirmed with `ls-remote`. The plugin advanced by fast-forward; the dashboard feature branch was created at the reviewed commit. No tags, force push or other branch changes.

Plugin `feat/europa-port`: **bd767ca8ac7cd1ca18abe34d9cc66764fee100a6**. Dashboard `feat/europa-425`: **b8ffaca1d88a2c89d1ed69adaac85501cc2fc0b5**. Dashboard `master`, plugin release branches and tags are unchanged. Shipping excludes the control server.

The packaged Development game connects to the desktop dashboard, lists the four defaults including LOD corruption, and supports start, stop, reconnect and a second capture. The full D3D11 console/dashboard comparison passes under GR1 and its clarification: both runs wrote and paired 2,352 frames with zero drops; labels, event schedule, routes, seed, actor-bound mask pixels and restoration agree. Each run passed 443 strict texture measurements at IoU 1.000 with zero spill. Current-package D3D12 lifecycle tests pass; GR2 drops the redundant D3D12 full comparison pair. **GO3 PASS:** the final package completed the default-only 1279×719 D3D12 capture with 2,352 paired frames and zero drops; its 14-file compatibility comparison found no removed key or changed type. The handover read commands were verified on this output.

GR1 records census-cycle differences as session nondeterminism: console/console **1,339 rows, max 8, mean 2.72**; console/dashboard **1,639 rows, max 26, mean 11.00**. The likely cause is the cumulative readback-timing counter plus dashboard snapshot work on the game thread. Private-verification and in-flight differences match the console baseline. Six schema entries reduce to the numeric timestamp `t=320` serialized as an integer; only its representation is normalized. Future work: serialize time fields as floats.

Home evidence is from stock UE 4.25.4. The office builds this source with Europa's own fork and checks the readings below. It does not use the home game's executable as an office binary.

## 1. Available home artifacts

- Development package: `E:\IA_BuildCache\09447u-package\WindowsNoEditor` (34 files, 2,369,785,987 bytes). Use its inner `<Name>\Binaries\Win64\<Name>.exe`.
- AnomDash: `D:\IA_Europa\Receipts\09447t\dashboard-build-r4\AnomalyDashboard.exe` (11,190,784 bytes; SHA-256 `c8120946ff5bb4daa713286510f091b75760ae41f0895480adea1a14e481be28`). This is the corrected desktop build with the new frontend embedded.
- Game EXE SHA-256: `e8166cda4ac636f10d70ffa28db3e33da8997a284694b1554b9b79612c416110`; pak SHA-256: `d926673c1e26886f69292162a87bdfcfbfaade283ef83b21e71e48c8dc940f56`.
- Package manifest: `D:\IA_Europa\Receipts\09447u\package-manifest.json`. Native source is `d10bbe859e81ef3a0a0e15dfc5224181cc7911b8`; the later plugin tip adds documentation.

## 2. Fetch and package on the office PC

`<SourceClone>` is the plugin clone outside the game project; `<Project>` contains `<Name>.uproject`; `<ForkRoot>` contains the fork's `Engine`; `<Map>` is the ordinary office test map's `/Game/...` path. Names and paths stay on the office PC. `<PackageOut>` is a new output folder outside the project.

In cmd, update the source clone and verify the full commit:

```bat
git -C "<SourceClone>" fetch origin feat/europa-port
git -C "<SourceClone>" checkout --detach bd767ca8ac7cd1ca18abe34d9cc66764fee100a6
git -C "<SourceClone>" rev-parse HEAD
```

Keep a backup of the old plugin outside the project's entire `Plugins` tree, then copy the checked source into `<Project>\Plugins\AnomalyInjector`. Include `Content\Engine425` and `Shaders`; exclude `.git`, `Binaries` and `Intermediate`. Keep the copy out of the game's source-control submission. Do not leave a second `.uplugin` copy anywhere under `Plugins`.

Close the office editor. Build the Editor target, then package Win64 **Development** using the fork's tools:

```bat
"<ForkRoot>\Engine\Build\BatchFiles\Build.bat" <Name>Editor Win64 Development -Project="<Project>\<Name>.uproject" -WaitMutex -NoEngineChanges
echo %ERRORLEVEL%
"<ForkRoot>\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -nop4 -project="<Project>\<Name>.uproject" -targetplatform=Win64 -clientconfig=Development -build -cook -stage -pak -package -archive -archivedirectory="<PackageOut>" -map="<Map>" -CookDir="<Project>\Plugins\AnomalyInjector\Content\Engine425" -utf8output -unattended -AdditionalCookerOptions="-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False -ini:EditorSettings:[/Script/UnrealEd.AnalyticsPrivacySettings]:bSendUsageData=False"
echo %ERRORLEVEL%
```

Both exit codes must be 0. `-CookDir` includes the two 4.25 texture materials; a complete cook includes the private-pass shaders. Use the resulting package's inner Win64 game executable as `<GameExe>`. The home package used already-built matching targets; the office command deliberately builds its own fork's targets. Do not substitute Shipping or skip the office build/cook.

Use the supplied AnomDash executable, or build the pushed dashboard branch in a separate dashboard clone with its existing Rust/MSVC/WebView2 prerequisites:

```bat
git fetch origin feat/europa-425
git checkout --detach b8ffaca1d88a2c89d1ed69adaac85501cc2fc0b5
npm ci
npm test
npx tauri build
```

The executable is `src-tauri\target\release\AnomalyDashboard.exe`. Tauri's configured build command embeds the frontend. `npm run dev` is a mock development UI and is not the delivered desktop app. No toolchain installation is part of this card.

## 3. Start the listener and connect

Choose fresh, writable `<UserDirFwd>` and `<RuntimeLogFwd>` paths written with forward slashes; do not reuse sealed evidence. Start the packaged game on the cooked map:

```bat
"<GameExe>" <Map> -game -windowed -ResX=1280 -ResY=720 -d3d11 -IAIBench -IAICaptureFallback -UserDir="<UserDirFwd>/" -abslog="<RuntimeLogFwd>" -ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False -ini:EditorSettings:[/Script/UnrealEd.AnalyticsPrivacySettings]:bSendUsageData=False -ExecCmds="IAI.SetMinScreenCoverage,r.EyeAdaptationQuality 0,r.PostProcessAAQuality 0,r.ScreenPercentage 100,r.DynamicRes.OperationMode 0,r.MotionBlurQuality 0,t.MaxFPS 30,IAI.Server.Start 8077"
```

Use `-d3d12` in place of `-d3d11` when required by the fork. The server is loopback-only: **`ws://127.0.0.1:8077`**. An occupied port is refused; stop the conflicting owned instance before explicitly restarting the listener. The console commands are `IAI.Server.Start 8077` and `IAI.Server.Stop`.

The default token is generated anew each time the listener starts. Double-click AnomDash: it discovers the listening game and reads its token from that game's log. Wait for **Connected**. If discovery cannot find the game, Settings → **Connect manually** accepts the address above and the token printed in this game session's log. Tokens stay local; never include them in readbacks, commands, commits or uploaded logs.

For a deliberately persistent local token, place `[AnomalyControlServer]` and `Token=<local-secret>` in `<UserDirFwd>/Saved/Config/WindowsNoEditor/Game.ini` before starting the game. The runtime reads `GGameIni`; the log's older “DefaultGame.ini” wording does not change that effective packaged path. A listener restart uses the configured token again. Automatic discovery remains available.

## 4. Run the qualified preset

In AnomDash, select **Capture** and set:

| Control | Value |
|---|---|
| What to capture | Random mix |
| Enabled anomalies | blinking, lod_corruption, missing_texture, corrupted_texture — all four; other routes off |
| Length | Custom → **2352** frames |
| Advanced options → Image format | PNG · lossless |
| Saved image size | As rendered (native) |
| Random seed | **4242** |
| Minimum object size | Shipped **6%** for office use; the home EA_Core bench alone uses 0.5% |
| Settings → Captures folder | New, writable absolute directory on this PC |

Press **Start capture**, then focus the game. Allow warmup and the full run to finish. The dashboard remains connected and the game stays open. Locate the resulting `session_...\epoch01` directory locally for the reads below.

**Stop capture** drains accepted frames and restores state. An intentional early stop is honestly **PARTIAL** with fewer than 2352 frames; it is not a failed claim of a full capture. Stop during warmup cancels without a delayed start. Reconnect resumes the same session status, and a second capture can start after the previous one finishes.

On 4.25 there is **no live preview**. Other frame counts, JPEG, resizing and targeted “One object” captures are unsupported. Non-authentication refusals are not fully surfaced in the UI: if capture does not start, check the game log locally for `capture_refused` or `C5 refused` and confirm the preset. A blank preview alone does not mean capture failed. Library video/overlay generation is not qualified by this delivery.

## 5. Camera clipping and retained limits

`camera_clipping` is **opt-in only**, outside the four default routes. GR3 qualifies it at **1280×720 on D3D11 and D3D12**. During a held camera-clipping session, distant geometry can show depth-precision speckle at some resolutions. At 1279×719 the measured textured floor's far rows 373–375 reached 117 levels; the plain floor stayed within 2, and neither 1280×720 leg showed the artifact. GR3 accepts the associated one-pixel strict-G3 miss at (375,239) **only inside held camera-clipping sessions**. Default-pool captures retain IoU 1.000 and zero spill. This default dashboard preset does not opt into camera clipping or its different schedule.

Unsupported/translucent/no-proxy components remain explicitly unavailable rather than labelled clean. Lighting masks exclude the game's own custom-depth actors. The private pass must not enable the game's outline/stencil on targets. No 5.1 runtime or release merge is claimed; that separate regression remains deferred.

## 6. Numbers-only office readback

**Office data stays on the office PC.** Do not upload or bring home captures, logs, screenshots, actor names, project paths or tokens. Type only the numbers and fixed words emitted below. Run after the capture stops. Set `$Epoch` to the local `session_...\epoch01` directory, then run this PowerShell block:

```powershell
$Epoch = '<local epoch directory>'
try {
  $ErrorActionPreference = 'Stop'
  $c = Get-Content -LiteralPath (Join-Path $Epoch 'capture.json') -Raw | ConvertFrom-Json
  $r = Get-Content -LiteralPath (Join-Path $Epoch 'labels-receipt.json') -Raw | ConvertFrom-Json
  $a = Get-Content -LiteralPath (Join-Path $Epoch 'annotation.json') -Raw | ConvertFrom-Json
  $p = Get-Content -LiteralPath (Join-Path $Epoch 'private-availability.json') -Raw | ConvertFrom-Json
  $before = Get-Content -LiteralPath (Join-Path $Epoch 'depth-before.json') -Raw
  $after = Get-Content -LiteralPath (Join-Path $Epoch 'depth-after.json') -Raw
  $rows = @(Get-Content -LiteralPath (Join-Path $Epoch 'labels.jsonl') | ForEach-Object { $_ | ConvertFrom-Json })
  $defaults = 'blinking,lod_corruption,missing_texture,corrupted_texture'
  'requested={0} written={1} paired={2} dropped={3} rows={4}' -f $c.requested,$c.written,$r.paired,$c.dropped,$rows.Count
  'receipt_pass={0} restored_mask={1} restored_pacing={2} depth_equal={3}' -f ($r.result -eq 'PASS'),$r.mask_state_restored,$r.pacing_restored,($before -ceq $after)
  'seed={0} routes_match={1} unmatched_dropped={2}' -f $r.seed,(($r.routes -join ',') -ceq $defaults),$r.unmatched_dropped
  'frames={0} masks={1} polluted_rows={2} tagged_rows={3}' -f @(Get-ChildItem -LiteralPath $Epoch -Filter 'frame-*.png').Count,@(Get-ChildItem -LiteralPath $Epoch -Filter 'mask-*.png').Count,@($rows | Where-Object { $_.census.polluted_batches -ne 0 }).Count,@($rows | Where-Object { $_.custom_depth.tagged_components -gt 0 }).Count
  'private_enabled={0} verified={1} unavailable={2} host_stencil_writes={3}' -f $p.enabled,$p.verified,$p.unavailable,$p.host_stencil_writes
  foreach ($id in $defaults.Split(',')) {
    $annotationId = if ($id -ceq 'blinking') { 'blink' } else { $id }
    $events = @($a.anomalies | Where-Object { $_.anomaly_type -ceq $annotationId })
    $affected = ($events | ForEach-Object { $_.affected_frames.frame_count } | Measure-Object -Sum).Sum
    '{0}: events={1} affected_frames={2}' -f $id,$events.Count,$affected
  }
} catch { 'reads_failed=true' }
```

Full-run expectations: requested/written/paired/rows/frame PNGs = **2352**, drops/unmatched/polluted/tagged/host-stencil writes = **0**, seed = **4242**, receipt/restoration/depth/routes booleans = **True**. Event counts and unavailable components depend on the office scene; report the actual numbers. A deliberate early stop instead reports partial counts and a non-PASS full receipt, while restoration and zero drops must still hold.

Also type: plugin commit prefix; dashboard commit prefix; Editor and packaging exit codes; numeric engine version; D3D11/D3D12; resolution; connected yes/no; LOD visible yes/no; start/stop/reconnect/second-capture yes/no; unwanted outline yes/no. Never transcribe the process, project, level or object names shown by the dashboard.

Chat review NOT REQUIRED for the authorized GR4 publication/handover. The next mandatory Chat checkpoint is a new correctness failure or proposed change to capture/label meaning, tolerance or release scope. The office fork's results remain a separate observation, not a claim derived from home tests.
