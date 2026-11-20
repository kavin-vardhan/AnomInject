# Europa 4.25 — dashboard free-play captures (office card, final)

| Item | Value |
|---|---|
| Plugin `feat/europa-port` | **`2828f6ab85406cacc6b5e4c06840a3a1b4cebbc0`** (pushed; pull this commit only) |
| Dashboard | unchanged: `feat/europa-425` `b8ffaca`; keep the AnomDash you already use |
| Content | unchanged since the 094-47 cook; only native code changed |

`<SourceClone>`, `<Project>`, `<Name>`, `<ForkRoot>`, `<Map>`, `<PackageOut>` and `<GameExe>` mean the same as on the 094-47 card.

## 1. Pull and repackage

```bat
git -C "<SourceClone>" fetch origin feat/europa-port
git -C "<SourceClone>" checkout --detach 2828f6ab85406cacc6b5e4c06840a3a1b4cebbc0
git -C "<SourceClone>" rev-parse HEAD
```

Copy the source into `<Project>\Plugins\AnomalyInjector` as on the 094-47 card (old copy backed up outside `Plugins`; exclude `.git`, `Binaries`, `Intermediate`; no second `.uplugin` under `Plugins`). Close the editor, then:

```bat
"<ForkRoot>\Engine\Build\BatchFiles\Build.bat" <Name>Editor Win64 Development -Project="<Project>\<Name>.uproject" -WaitMutex -NoEngineChanges
echo %ERRORLEVEL%
"<ForkRoot>\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -nop4 -project="<Project>\<Name>.uproject" -targetplatform=Win64 -clientconfig=Development -build -skipcook -stage -pak -package -archive -archivedirectory="<PackageOut>" -utf8output -unattended
echo %ERRORLEVEL%
```

- `-skipcook` is right if the 094-47 cook output is still in the project (only code changed since then).
- **Otherwise (cook output gone, or content changed): full BuildCookRun** instead of the second command:

```bat
"<ForkRoot>\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -nop4 -project="<Project>\<Name>.uproject" -targetplatform=Win64 -clientconfig=Development -build -cook -stage -pak -package -archive -archivedirectory="<PackageOut>" -map="<Map>" -CookDir="<Project>\Plugins\AnomalyInjector\Content\Engine425" -utf8output -unattended -AdditionalCookerOptions="-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False -ini:EditorSettings:[/Script/UnrealEd.AnalyticsPrivacySettings]:bSendUsageData=False"
echo %ERRORLEVEL%
```

- Both exit codes must be 0. Development only (Shipping has no control server).

## 2. Launch

```bat
"<GameExe>" -game -ExecCmds="IAI.Server.Start 8077"
```

- Add `-windowed -ResX=1280 -ResY=720` and `-d3d11` or `-d3d12` if the fork needs them. **No `-IAIBench`.**
- **First launch:** if Windows Firewall asks about the game, choose **Allow**, **Private networks**.
- Double-click AnomDash and wait for **Connected** (it finds the game and its token itself). Never type or copy a token.

## 3. Capture

1. **Settings → Captures folder:** a writable folder on this PC.
2. **Start capture** = free play: move and play normally. Any length preset, Custom, or **Until I stop**. Anomalies start at about frame 26 (under a second at 30 fps).
3. A fixed length ends by itself; **Stop capture** ends an until-stop run normally. The game stays open; the next capture can start right away.
4. **Library:** the capture appears as a normal `session_<timestamp>` session (thumbnail, timeline), holding exactly the 5.1 files: `annotation.json`, `labels.jsonl`, `run_summary.json`, `mask_map.json`, `Actual_Frames\` and `target_mask\` (a mask only for frames that contain a target).

- **camera_clipping is opt-in.** GR3 qualifies it at 1280×720 on D3D11 and D3D12; at other sizes distant geometry can show depth-precision speckle during a held camera-clipping session.
- **No live preview on 4.25** (also no JPEG, resizing or "One object" capture).

## 4. Numbers-only readback

**Office data stays on the office PC.** Type back only the numbers and fixed words this prints. Set `$S` to the `session_…` folder and run in PowerShell:

```powershell
$S = '<local session folder>'
try {
  $ErrorActionPreference = 'Stop'
  $sum = Get-Content -LiteralPath (Join-Path $S 'run_summary.json') -Raw | ConvertFrom-Json
  $a = Get-Content -LiteralPath (Join-Path $S 'annotation.json') -Raw | ConvertFrom-Json
  $rows = @(Get-Content -LiteralPath (Join-Path $S 'labels.jsonl') | ForEach-Object { $_ | ConvertFrom-Json })
  $md = Join-Path $S 'target_mask'; if (-not (Test-Path -LiteralPath $md)) { $md = Join-Path $S '.dashboard\pending\target_mask' }
  $mm = Join-Path $S 'mask_map.json'; if (-not (Test-Path -LiteralPath $mm)) { $mm = Join-Path $S '.dashboard\pending\mask_map.json' }
  $root = @(Get-ChildItem -LiteralPath $S -File | ForEach-Object { $_.Name })
  $extra = @($root | Where-Object { $_ -notin 'annotation.json','labels.jsonl','run_summary.json','mask_map.json' }).Count
  $bin = @(Get-ChildItem -LiteralPath $S -Recurse -File -Force | Where-Object { $_.Extension -in '.u32','.zlib' }).Count
  'flat_complete={0} epoch01_present={1} mask_map_present={2} extra_root_files={3} u32_zlib_files={4}' -f (Test-Path -LiteralPath (Join-Path $S 'run_summary.json')),(Test-Path -LiteralPath (Join-Path $S 'epoch01')),(Test-Path -LiteralPath $mm),$extra,$bin
  'summary_total={0} rows={1} frames={2} dropped={3} result={4} stopped_by_user={5}' -f $sum.total_frames,$rows.Count,@(Get-ChildItem -LiteralPath (Join-Path $S 'Actual_Frames') -Filter '*.png').Count,$sum.dropped_frames,$sum.result,$sum.stopped_by_user
  'mask_pngs={0} masked_rows={1} summary_measured={2} summary_hidden_blank={3}' -f @(Get-ChildItem -LiteralPath $md -Filter '*.png').Count,@($rows | Where-Object { $_.mask_file }).Count,$sum.target_mask_frames_measured,$sum.target_mask_frames_hidden_blank
  $pp = $rows[-1].census.private_pass
  'input_ignored_rows={0} polluted_rows={1} lost_batch_rows={2} host_stencil_writes={3} private_verified={4} private_unavailable={5}' -f @($rows | Where-Object { $_.move_input_ignored -or $_.look_input_ignored }).Count,@($rows | Where-Object { $_.census.polluted_batches -ne 0 }).Count,@($rows | Where-Object { $_.census.batches_lost -ne 0 }).Count,$pp.host_stencil_writes,$pp.verified,$pp.unavailable
  $first = @($a.anomalies | ForEach-Object { $_.affected_frames.frame_indices } | Measure-Object -Minimum).Minimum
  'events={0} first_labelled_frame={1}' -f @($a.anomalies).Count,$first
  foreach ($id in 'blinking','lod_corruption','missing_texture','corrupted_texture','camera_clipping') {
    $annotationId = if ($id -ceq 'blinking') { 'blink' } else { $id }
    $events = @($a.anomalies | Where-Object { $_.anomaly_type -ceq $annotationId })
    $affected = ($events | ForEach-Object { $_.affected_frames.frame_count } | Measure-Object -Sum).Sum
    '{0}: events={1} affected_frames={2}' -f $id,$events.Count,$affected
  }
} catch { 'reads_failed=true' }
```

**Expected** (home 300-frame capture, same block):
- `flat_complete=True epoch01_present=False mask_map_present=True extra_root_files=0 u32_zlib_files=0`.
- `summary_total` = `rows` = `frames` = the run length; `dropped=0`; `result=PASS`; `stopped_by_user=True` only when Stop ended the run.
- `mask_pngs` = `masked_rows` = `summary_measured`, and `summary_measured` + `summary_hidden_blank` = the run length (home: 140 + 160).
- `input_ignored_rows`, `polluted_rows`, `lost_batch_rows`, `host_stencil_writes`, `private_unavailable` all **0**.
- `first_labelled_frame` about 26. Event counts depend on the office scene; report them.

Also type back: the commit prefix, both exit codes, D3D11 or D3D12, the resolution, whether the player could move (yes/no), whether the session appeared in the Library (yes/no), and whether the game stayed open (yes/no).

---
Dev only: `IAI.Capture.Evidence 1` (console, before Start) adds verification data to a dashboard capture. Not for office captures.
