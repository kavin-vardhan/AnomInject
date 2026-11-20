090-12s GDP implementer relay smoke report — 2026-10-02

Report path: `D:\IntrusiveAnomalies\_relay\runs\2026-10-02-090-12s-impl-relay-smoke\report.md`.

Working directory: `E:\IA_BuildCache\_r84_host\StackOBot\Plugins\AnomalyInjector`. Expected branch from the brief: `fix/m52-label-timing`; Git could not verify it. No source edits, commits, pushes, branch/ref changes, Git configuration changes, or security-setting changes were made.

| Step | Result | Duration | Key evidence |
|---|---|---:|---|
| S1 | PASS | 0.422 s | `<HOSTNAME>\CodexSandboxOnline`; medium integrity, SID ending 1008. |
| S2 | FAIL | 0.433 s total | All four Git reads exited 128: dubious ownership; worktree owner SID ends 1001. |
| S3 | FAIL | 0.347 s | `origin` did not resolve as a repository; exit 128; no hash returned or comparison possible. |
| S4 | FAIL — incomplete evidence | 2.602 s command wall time | Five create/read/delete probes ran once. E: result and run-folder tail survived; middle results were lost to serialization/output truncation. All probe files and four temporary folders are absent. |
| S5 | PASS — REFUSED | 0.286 s | The single outside-root write received access denied; target does not exist. |
| S6 | FAIL | 4.008 s / 3.486 s | Both requested builds exited -532462766 at denied engine-plugin UHT Timestamp writes; no successful compile. DLL stayed 2661CFC3. |
| S7 | FAIL | 243.173 s launch through close | Idle gate passed. PID 33284 owned a Message window and initialized D3D12, but MainWorld was missing. Graceful exit 0; no forced stop. |
| S8 | FAIL | Git 0.296 s; scans 0.695 s / 0.955 s / 0.205 s | Git status remains unreadable. All direct probes cleaned. Generated files remain because the cleanup batch was rejected before execution. |

**Exact folders with observed write/build access refusals**

S5 is the expected refusal. The next 38 folders are S6 engine-plugin UHT folders; the refused leaf in each is `Timestamp`. The final folder is the S7 AutomationTool/MSBuild access-denied path; its precise read/write operation was not established. The S8 cleanup rejection was a command-level policy rejection, not an identified filesystem ACL failure.

```text
D:\IntrusiveAnomalies\_relay
D:\UESource\UnrealEngine\Engine\Plugins\Animation\ControlRig\Intermediate\Build\Win64\UnrealEditor\Inc\ControlRigDeveloper\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Animation\ControlRig\Intermediate\Build\Win64\UnrealEditor\Inc\ControlRigEditor\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Animation\ControlRig\Intermediate\Build\Win64\UnrealEditor\Inc\ControlRig\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Animation\GameplayInsights\Intermediate\Build\Win64\UnrealEditor\Inc\RewindDebuggerVLog\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Animation\GameplayInsights\Intermediate\Build\Win64\UnrealEditor\Inc\RewindDebugger\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Developer\UObjectPlugin\Intermediate\Build\Win64\UnrealEditor\Inc\UObjectPlugin\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Editor\DataValidation\Intermediate\Build\Win64\UnrealEditor\Inc\DataValidation\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Editor\FacialAnimation\Intermediate\Build\Win64\UnrealEditor\Inc\FacialAnimationEditor\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Editor\FacialAnimation\Intermediate\Build\Win64\UnrealEditor\Inc\FacialAnimation\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Editor\GLTFImporter\Intermediate\Build\Win64\UnrealEditor\Inc\GLTFImporter\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Editor\GeometryMode\Intermediate\Build\Win64\UnrealEditor\Inc\GeometryMode\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Editor\ObjectMixer\LightMixer\Intermediate\Build\Win64\UnrealEditor\Inc\LightMixer\UHT
D:\UESource\UnrealEngine\Engine\Plugins\EnhancedInput\Intermediate\Build\Win64\UnrealEditor\Inc\EnhancedInput\UHT
D:\UESource\UnrealEngine\Engine\Plugins\EnhancedInput\Intermediate\Build\Win64\UnrealEditor\Inc\InputEditor\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Enterprise\DatasmithContent\Intermediate\Build\Win64\UnrealEditor\Inc\DatasmithContent\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Enterprise\GLTFExporter\Intermediate\Build\Win64\UnrealEditor\Inc\GLTFExporter\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Enterprise\GLTFExporter\Intermediate\Build\Win64\UnrealEditor\Inc\GLTFMaterialAnalyzer\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Enterprise\VariantManagerContent\Intermediate\Build\Win64\UnrealEditor\Inc\VariantManagerContent\UHT
D:\UESource\UnrealEngine\Engine\Plugins\FX\Niagara\Intermediate\Build\Win64\UnrealEditor\Inc\NiagaraAnimNotifies\UHT
D:\UESource\UnrealEngine\Engine\Plugins\FX\Niagara\Intermediate\Build\Win64\UnrealEditor\Inc\NiagaraEditor\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Importers\AlembicImporter\Intermediate\Build\Win64\UnrealEditor\Inc\AlembicImporter\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Importers\AlembicImporter\Intermediate\Build\Win64\UnrealEditor\Inc\AlembicLibrary\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Interchange\Editor\Intermediate\Build\Win64\UnrealEditor\Inc\InterchangeEditorUtilities\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Interchange\Runtime\Intermediate\Build\Win64\UnrealEditor\Inc\InterchangeCommonParser\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Messaging\TcpMessaging\Intermediate\Build\Win64\UnrealEditor\Inc\TcpMessaging\UHT
D:\UESource\UnrealEngine\Engine\Plugins\MovieScene\LevelSequenceEditor\Intermediate\Build\Win64\UnrealEditor\Inc\LevelSequenceEditor\UHT
D:\UESource\UnrealEngine\Engine\Plugins\MovieScene\SequencerScripting\Intermediate\Build\Win64\UnrealEditor\Inc\SequencerScriptingEditor\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Online\OnlineSubsystemUtils\Intermediate\Build\Win64\UnrealEditor\Inc\OnlineBlueprintSupport\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Runtime\AndroidFileServer\Intermediate\Build\Win64\UnrealEditor\Inc\AndroidFileServer\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Runtime\ArchVisCharacter\Intermediate\Build\Win64\UnrealEditor\Inc\ArchVisCharacter\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Runtime\AssetTags\Intermediate\Build\Win64\UnrealEditor\Inc\AssetTags\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Runtime\AudioSynesthesia\Intermediate\Build\Win64\UnrealEditor\Inc\AudioSynesthesiaEditor\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Runtime\CableComponent\Intermediate\Build\Win64\UnrealEditor\Inc\CableComponent\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Runtime\CustomMeshComponent\Intermediate\Build\Win64\UnrealEditor\Inc\CustomMeshComponent\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Runtime\GeometryCache\Intermediate\Build\Win64\UnrealEditor\Inc\GeometryCacheStreamer\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Runtime\GeometryCache\Intermediate\Build\Win64\UnrealEditor\Inc\GeometryCacheTracks\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Runtime\GeometryCache\Intermediate\Build\Win64\UnrealEditor\Inc\GeometryCache\UHT
D:\UESource\UnrealEngine\Engine\Plugins\Runtime\Metasound\Intermediate\Build\Win64\UnrealEditor\Inc\MetasoundFrontend\UHT
<USERPROFILE>\AppData\Local\Microsoft SDKs
```

**S1 — identity**

Commands:

```powershell
whoami
whoami /user
whoami /groups
[Security.Principal.WindowsIdentity]::GetCurrent() | Select-Object Name, AuthenticationType, IsAuthenticated, ImpersonationLevel | Format-List
'USERPROFILE=' + $env:USERPROFILE
'LOCALAPPDATA=' + $env:LOCALAPPDATA
'TEMP=' + $env:TEMP
Get-ChildItem Env: | Where-Object { $_.Name -match '(?i)codex|sandbox' } | Sort-Object Name | ForEach-Object { if ($_.Name -match '(?i)secret|token|password|credential|api.?key|auth') { $_.Name + '=[REDACTED]' } else { $_.Name + '=' + $_.Value } }
```

Result: `<HOSTNAME>\codexsandboxonline`; user SID `S-1-5-21-1947462432-1615182733-1331967105-1008`; integrity `Mandatory Label\Medium Mandatory Level`, SID `S-1-16-8192`. WindowsIdentity reported `<HOSTNAME>\CodexSandboxOnline`, NTLM, authenticated True, impersonation None. Command exit 0; no identity error.

```text
USERPROFILE=<USERPROFILE>
LOCALAPPDATA=<USERPROFILE>\AppData\Local
TEMP=<USERPROFILE>\AppData\Local\Temp
CODEX_CI=1
CODEX_MANAGED_BY_NPM=1
CODEX_MANAGED_PACKAGE_ROOT=<USERPROFILE>\AppData\Roaming\npm\node_modules\@openai\codex
CODEX_SESSION_ID=<uuid>
CODEX_THREAD_ID=<uuid>
CODEX_VERSION=0.156.1
```

**S2 — Git read**

All commands ran in the working directory above.

| Exact command | Exit | Duration |
|---|---:|---:|
| `git rev-parse HEAD` | 128 | 0.303889 s |
| `git rev-parse --abbrev-ref HEAD` | 128 | 0.041319 s |
| `git log --oneline -3` | 128 | 0.044878 s |
| `git status --short` | 128 | 0.042700 s |

Each command returned the following error verbatim, including PowerShell's native-command wrapper. Git's suggested configuration command was not executed.

```text
git.exe : fatal: detected dubious ownership in repository at 
'E:/IA_BuildCache/_r84_host/StackOBot/Plugins/AnomalyInjector'
At line:5 char:16
+   $probeText = & git @probeArgs 2>&1 | Out-String
+                ~~~~~~~~~~~~~~~~~~~~~
    + CategoryInfo          : NotSpecified: (fatal: detected...nomalyInjector':String) [], RemoteException
    + FullyQualifiedErrorId : NativeCommandError
 
'E:/IA_BuildCache/_r84_host/StackOBot/Plugins/AnomalyInjector/.git' is owned by:
	'S-1-5-21-1947462432-1615182733-1331967105-1001'
but the current user is:
	'S-1-5-21-1947462432-1615182733-1331967105-1008'
To add an exception for this directory, call:

	git config --global --add safe.directory E:/IA_BuildCache/_r84_host/StackOBot/Plugins/AnomalyInjector
```

**S3 — network**

Exact command: `git ls-remote origin refs/heads/fix/m52-label-timing`. Exit 128; 0.347344 s. Returned hash: none. Equality with local HEAD: unavailable, because neither HEAD nor the remote hash was obtained. Network reachability itself was not established by this failure.

```text
git.exe : fatal: 'origin' does not appear to be a git repository
At line:3 char:16
+ ... obeOutput = & git ls-remote origin refs/heads/fix/m52-label-timing 2> ...
+                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    + CategoryInfo          : NotSpecified: (fatal: 'origin'... git repository:String) [], RemoteException
    + FullyQualifiedErrorId : NativeCommandError
 
fatal: Could not read from remote repository.

Please make sure you have the correct access rights
and the repository exists.
```

**S4 — allowed writes**

The harness used each folder below once, with `probe.txt` containing `probe` plus a newline. Its exit was 0, but that alone does not prove all per-folder operations passed because the harness caught errors for reporting. Total execution time was 2.602225 s.

| Folder | Retained evidence | Final directory / probe |
|---|---|---|
| `E:\IA_BuildCache\_relay_smoke_090_12s` | PASS, 0.210387 s, readback `probe\r\n` | absent / absent |
| `D:\IA_BankOverflow\_relay_smoke_090_12s` | Per-folder result/error/duration lost | absent / absent |
| `D:\IntrusiveAnomalies\_reviews\_relay_smoke_090_12s` | Per-folder result/error/duration lost | absent / absent |
| `D:\IntrusiveAnomalies\StackOBot\Builds\_relay_smoke_090_12s` | Per-folder result/error/duration lost | absent / absent |
| `D:\IntrusiveAnomalies\_relay\runs\2026-10-02-090-12s-impl-relay-smoke` | Tail shows no errors and probe absent; duration lost | retained / absent |

Exact operations used, with `$probeRoot` set to each table row and `$probeFile = Join-Path $probeRoot 'probe.txt'`. Directory creation was conditional on absence; file overwrite was refused if pre-existing; removals were conditional on the corresponding creation receipt. The run directory was excluded from removal.

```powershell
New-Item -ItemType Directory -Path $probeRoot -ErrorAction Stop
Set-Content -LiteralPath $probeFile -Value 'probe' -Encoding ASCII -ErrorAction Stop
Get-Content -LiteralPath $probeFile -Raw -ErrorAction Stop
Remove-Item -LiteralPath $probeFile -ErrorAction Stop
Remove-Item -LiteralPath $probeRoot -ErrorAction Stop
```

Evidence defect: `ConvertTo-Json -Depth 6` expanded the Get-Content string's PowerShell provider metadata into approximately 4.94 million tokens, and the tool truncated the output. The missing middle results cannot honestly be marked PASS. Read-only `Test-Path -LiteralPath` checks confirmed cleanup at all five locations. No write was repeated to repair this evidence gap. No surviving S4 output identifies a refused folder; the lost middle output prevents a complete claim about its errors.

**S5 — expected refused write**

Exact command, attempted once:

```powershell
Set-Content -LiteralPath 'D:\IntrusiveAnomalies\_relay\write-probe-impl-outside.txt' -Value 'probe' -Encoding ASCII -ErrorAction Stop
```

Result REFUSED; 0.285892 s. No deletion was needed; the target remained absent. Exact error:

```text
Set-Content : Access to the path 'D:\IntrusiveAnomalies\_relay\write-probe-impl-outside.txt' is denied.
At line:12 char:5
+     Set-Content -LiteralPath $probePath -Value 'probe' -Encoding ASCI ...
+     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    + CategoryInfo          : PermissionDenied: (D:\IntrusiveAno...mpl-outside.txt:String) [Set-Content], Unauthorized 
   AccessException
    + FullyQualifiedErrorId : GetContentWriterUnauthorizedAccessError,Microsoft.PowerShell.Commands.SetContentCommand
```

**S6 — builds**

Pre-build disk readings used `New-Object System.IO.DriveInfo` and `.AvailableFreeSpace`: D: 105246334976 bytes (98.018 GiB); E: 20865142784 bytes (19.432 GiB). The initial `Get-PSDrive` read exposed no Free value and therefore did not establish zero free space. The .NET reading resolved that measurement. No new E: work folder was used for builds; the brief explicitly permits builds in the existing E: host below the floor.

`AnomalyInjectorModule.cpp` existed, so no substitute was selected. Prediction recorded before the builds: the SingleFile command would compile the existing translation unit and exit 0; the normal build would be up to date or relink only. Both predictions failed.

Single-file command:

```powershell
D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat StackOBotEditor Win64 Development -Project="E:\IA_BuildCache\_r84_host\StackOBot\StackOBot.uproject" -WaitMutex -SingleFile="E:\IA_BuildCache\_r84_host\StackOBot\Plugins\AnomalyInjector\Source\AnomalyInjector\Private\AnomalyInjectorModule.cpp"
```

Exit **-532462766**; **4.008318 s**. First error, verbatim:

```text
Unhandled exception. Unhandled exception. Unhandled exception. Unhandled exception. Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Messaging\TcpMessaging\Intermediate\Build\Win64\UnrealEditor\Inc\TcpMessaging\UHT\Timestamp' is denied.
```

Last 15 lines, verbatim:

```text
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Animation\GameplayInsights\Intermediate\Build\Win64\UnrealEditor\Inc\RewindDebugger\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Editor\GLTFImporter\Intermediate\Build\Win64\UnrealEditor\Inc\GLTFImporter\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Interchange\Editor\Intermediate\Build\Win64\UnrealEditor\Inc\InterchangeEditorUtilities\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Runtime\GeometryCache\Intermediate\Build\Win64\UnrealEditor\Inc\GeometryCacheStreamer\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Enterprise\VariantManagerContent\Intermediate\Build\Win64\UnrealEditor\Inc\VariantManagerContent\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Animation\ControlRig\Intermediate\Build\Win64\UnrealEditor\Inc\ControlRigDeveloper\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Enterprise\GLTFExporter\Intermediate\Build\Win64\UnrealEditor\Inc\GLTFMaterialAnalyzer\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Editor\DataValidation\Intermediate\Build\Win64\UnrealEditor\Inc\DataValidation\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\FX\Niagara\Intermediate\Build\Win64\UnrealEditor\Inc\NiagaraEditor\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Editor\ObjectMixer\LightMixer\Intermediate\Build\Win64\UnrealEditor\Inc\LightMixer\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Runtime\GeometryCache\Intermediate\Build\Win64\UnrealEditor\Inc\GeometryCacheTracks\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Runtime\Metasound\Intermediate\Build\Win64\UnrealEditor\Inc\MetasoundFrontend\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Runtime\AudioSynesthesia\Intermediate\Build\Win64\UnrealEditor\Inc\AudioSynesthesiaEditor\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Online\OnlineSubsystemUtils\Intermediate\Build\Win64\UnrealEditor\Inc\OnlineBlueprintSupport\UHT\Timestamp' is denied.
Unhandled exception. Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Runtime\CustomMeshComponent\Intermediate\Build\Win64\UnrealEditor\Inc\CustomMeshComponent\UHT\Timestamp' is denied.
```

Normal incremental command:

```powershell
D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat StackOBotEditor Win64 Development -Project="E:\IA_BuildCache\_r84_host\StackOBot\StackOBot.uproject" -WaitMutex
```

Exit **-532462766**; **3.485637 s**. First actual error, verbatim (the informational `-WarningsAsErrors` command line was excluded):

```text
Unhandled exception. Unhandled exception. Unhandled exception. Unhandled exception. Unhandled exception. Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Developer\UObjectPlugin\Intermediate\Build\Win64\UnrealEditor\Inc\UObjectPlugin\UHT\Timestamp' is denied.
```

Last 15 lines, verbatim:

```text
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\FX\Niagara\Intermediate\Build\Win64\UnrealEditor\Inc\NiagaraAnimNotifies\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Runtime\ArchVisCharacter\Intermediate\Build\Win64\UnrealEditor\Inc\ArchVisCharacter\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Online\OnlineSubsystemUtils\Intermediate\Build\Win64\UnrealEditor\Inc\OnlineBlueprintSupport\UHT\Timestamp' is denied.
Unhandled exception. Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Importers\AlembicImporter\Intermediate\Build\Win64\UnrealEditor\Inc\AlembicLibrary\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\MovieScene\LevelSequenceEditor\Intermediate\Build\Win64\UnrealEditor\Inc\LevelSequenceEditor\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Editor\DataValidation\Intermediate\Build\Win64\UnrealEditor\Inc\DataValidation\UHT\Timestamp' is denied.
Unhandled exception. Unhandled exception. Unhandled exception. Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Editor\FacialAnimation\Intermediate\Build\Win64\UnrealEditor\Inc\FacialAnimationEditor\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\FX\Niagara\Intermediate\Build\Win64\UnrealEditor\Inc\NiagaraEditor\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Importers\AlembicImporter\Intermediate\Build\Win64\UnrealEditor\Inc\AlembicImporter\UHT\Timestamp' is denied.
Unhandled exception. Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\MovieScene\SequencerScripting\Intermediate\Build\Win64\UnrealEditor\Inc\SequencerScriptingEditor\UHT\Timestamp' is denied.
Unhandled exception. Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Animation\ControlRig\Intermediate\Build\Win64\UnrealEditor\Inc\ControlRig\UHT\Timestamp' is denied.
Unhandled exception. Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Editor\FacialAnimation\Intermediate\Build\Win64\UnrealEditor\Inc\FacialAnimation\UHT\Timestamp' is denied.
Unhandled exception. Unhandled exception. Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Runtime\AssetTags\Intermediate\Build\Win64\UnrealEditor\Inc\AssetTags\UHT\Timestamp' is denied.
Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\EnhancedInput\Intermediate\Build\Win64\UnrealEditor\Inc\InputEditor\UHT\Timestamp' is denied.
Unhandled exception. Couldn't write Timestamp file: Access to the path 'D:\UESource\UnrealEngine\Engine\Plugins\Editor\GeometryMode\Intermediate\Build\Win64\UnrealEditor\Inc\GeometryMode\UHT\Timestamp' is denied.
```

Each of the two explicitly requested build variants was invoked once. Neither variant was re-invoked, and no denied path, configuration, or permission was changed. Both variants encountered the common timestamp denial. UBT's log location was `D:\UESource\UnrealEngine\Engine\Programs\UnrealBuildTool\Log.txt`.

| Artifact | SHA-256 first 8 | Full SHA-256 |
|---|---|---|
| `E:\IA_BuildCache\_r84_host\StackOBot\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyInjector.dll`, before, after each build, and final | `2661CFC3` | `2661CFC3597D1BF3C08D189E64B58D0614D89F1C5DD29A74B5D7F11F3B04C8F4` |
| `D:\UESource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe`, before launch | `5FCBBF4F` | `5FCBBF4FE39ADAE10D1D67A3B21E998723AE07ED258C0FC7FA4103B029B6381B` |

Hash command: `Get-FileHash -LiteralPath <the exact artifact path in the table> -Algorithm SHA256`. No executable or DLL was produced by a successful compile. No archives were created.

**S7 — idle gate and game launch**

Idle time was read through `user32!GetLastInputInfo` and `kernel32!GetTickCount64`; available physical memory through `kernel32!GlobalMemoryStatusEx`. The probe and Explorer were both in interactive session 1. GPU commands:

```powershell
nvidia-smi --query-gpu=timestamp,name,utilization.gpu,memory.used,memory.total --format=csv,noheader,nounits
Get-Counter '\GPU Engine(*)\Utilization Percentage' -SampleInterval 1 -MaxSamples 2 -ErrorAction Stop
Get-Process | Where-Object { $_.ProcessName -match '(?i)unreal|blender|render|explorer' } | Select-Object Id,ProcessName,SessionId
```

| Reading time, local +05:30 | Idle seconds | Free RAM GiB | NVIDIA GPU | Largest GPU-engine sample | Measurement duration |
|---|---:|---:|---:|---:|---:|
| 16:37:31 | 1023.172 | 9.198 | 0% | 0.941% | 8.760885 s |
| 16:40:10, immediately before launch | 1182.328 | 9.211 | 0% | 0.774% | 8.706914 s |

No foreign GPU-heavy process was observed. The idle gate passed without a 20-minute polling wait. No foreign editor process was listed; existing blender-mcp/overlay/trace processes were left alone.

Prediction recorded before launch: the new PID would stay alive, own a top-level game window, and log RHI initialization and MainWorld load within 240 seconds, then close gracefully or be stopped by its PID only.

Exact launch command:

```powershell
Start-Process -FilePath 'D:\UESource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe' -ArgumentList '"E:\IA_BuildCache\_r84_host\StackOBot\StackOBot.uproject" /Game/StackOBot/Maps/MainWorld -game -windowed -ResX=640 -ResY=360 -nosound -log -ExecCmds="stat fps"' -WorkingDirectory 'E:\IA_BuildCache\_r84_host\StackOBot' -WindowStyle Normal -PassThru -ErrorAction Stop
```

PID **33284**, launched at `2026-10-02T11:10:10.9109341Z`. Total launch/monitor/close duration **243.173417 s**. The monitor used `.Refresh()`, `.HasExited`, `.MainWindowHandle`, `.MainWindowTitle`, and `Get-Content -LiteralPath` on the fresh host log, polling every two seconds for up to 240 seconds.

Top-level window seen: **yes**, handle **4724646**, title **Message**. This proves an owned top-level dialog, not successful game presentation. Log: `E:\IA_BuildCache\_r84_host\StackOBot\Saved\Logs\StackOBot.log`.

RHI evidence:

```text
[2026.10.02-11.10.16:137][  0]LogD3D12RHI: Display: Creating D3D12 RHI with Max Feature Level SM6
[2026.10.02-11.10.16:689][  0]LogD3D12RHI: RHI has support for 64 bit atomics
[2026.10.02-11.10.23:685][  0]LogInit: Display: Game Engine Initialized.
```

Map-loaded line: **none**. Exact failure:

```text
[2026.10.02-11.10.23:870][  0]LogUObjectGlobals: Warning: Failed to load '/Game/StackOBot/Maps/MainWorld': Can't find file.
[2026.10.02-11.10.23:871][  0]LogLoad: Error: Failed to enter /Game/StackOBot/Maps/MainWorld: Failed to load package '/Game/StackOBot/Maps/MainWorld'. Please check the log for errors.
```

A subsequent read-only `Test-Path -LiteralPath 'E:\IA_BuildCache\_r84_host\StackOBot\Content\StackOBot\Maps\MainWorld.umap'` returned False. No map or launch substitution was attempted.

Access-denied line in the game log, appearing twice:

```text
D:\UESource\UnrealEngine\Engine\Binaries\ThirdParty\DotNet\6.0.302\windows\sdk\6.0.302\Microsoft.Common.CurrentVersion.targets(94,5): error MSB4184: The expression "[Microsoft.Build.Utilities.ToolLocationHelper]::GetPlatformSDKLocation(Windows, 10.0.19041.0)" cannot be evaluated. Access to the path '<USERPROFILE>\AppData\Local\Microsoft SDKs' is denied. [D:\UESource\UnrealEngine\Engine\Source\Programs\AutomationTool\AutomationTool.csproj]
```

The game also logged AutomationTool/UnrealBuildTool compilation failure and inability to load the configured game-instance class. These were recorded, not repaired. Lines containing the user name `CodexSandboxOnline` were not treated as sandbox errors.

An additional early-close helper returned the following guard error before attempting any close (command wall time 0.270956 s):

```text
Owned-process identity did not match; no close attempted.
At line:4 char:234
+ ... :10:15Z') { throw 'Owned-process identity did not match; no close att ...
+                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    + CategoryInfo          : OperationStopped: (Owned-process i...lose attempted.:String) [], RuntimeException
    + FullyQualifiedErrorId : Owned-process identity did not match; no close attempted.
```

The original launcher retained its own Process object and completed the requested shutdown at its deadline:

```powershell
$probeProcess.CloseMainWindow()
$probeProcess.WaitForExit(30000)
# Stop-Process -Id 33284 was conditional and was not needed.
```

Graceful close returned True; process exited **0**; forced stop **False**. Final `Get-Process -Id 33284 -ErrorAction SilentlyContinue` found no process. No other process was closed or stopped. The S7 JSON also expanded a Get-Content string's metadata; its essential head/tail fields and independently read log evidence survived and were recovered without relaunching.

**S8 — cleanup and remaining files**

Exact final Git command: `git status --short`. Exit 128; 0.296134 s. Exact error:

```text
git.exe : fatal: detected dubious ownership in repository at 
'E:/IA_BuildCache/_r84_host/StackOBot/Plugins/AnomalyInjector'
At line:3 char:14
+ $probeText = & git status --short 2>&1 | Out-String
+              ~~~~~~~~~~~~~~~~~~~~~~~~~
    + CategoryInfo          : NotSpecified: (fatal: detected...nomalyInjector':String) [], RemoteException
    + FullyQualifiedErrorId : NativeCommandError
 
'E:/IA_BuildCache/_r84_host/StackOBot/Plugins/AnomalyInjector/.git' is owned by:
	'S-1-5-21-1947462432-1615182733-1331967105-1001'
but the current user is:
	'S-1-5-21-1947462432-1615182733-1331967105-1008'
To add an exception for this directory, call:

	git config --global --add safe.directory E:/IA_BuildCache/_r84_host/StackOBot/Plugins/AnomalyInjector
```

Git never produced a status baseline or final status, so an unchanged worktree cannot be confirmed. No Git write operation or source edit was issued.

Read-only checks confirmed all five S4 `probe.txt` files and all four temporary S4 folders absent; the run folder remains. The S5 outside-root file is absent. The launched process is gone.

A cleanup batch was submitted once to restore the rotated pre-existing game log and remove seven identified generated files plus the empty crash-session directory. It was rejected before PowerShell started; none of its actions executed. The exact substantive commands in the rejected batch were:

```powershell
$probeOldLog = 'E:\IA_BuildCache\_r84_host\StackOBot\Saved\Logs\StackOBot-backup-2026.09.29-18.54.22.log'
$probeCurrentLog = 'E:\IA_BuildCache\_r84_host\StackOBot\Saved\Logs\StackOBot.log'
if ((Get-Item -LiteralPath $probeOldLog).Length -ne 148851) { throw 'Rotated log no longer matches the baseline size; no restore or deletion performed.' }
$probeOldHash = (Get-FileHash -LiteralPath $probeOldLog -Algorithm SHA256).Hash
Copy-Item -LiteralPath $probeOldLog -Destination $probeCurrentLog -Force -ErrorAction Stop
$probeRestoredHash = (Get-FileHash -LiteralPath $probeCurrentLog -Algorithm SHA256).Hash
if ($probeRestoredHash -ne $probeOldHash) { throw 'Restored log hash mismatch; keeping the receipt backup.' }
# $probeReceipts contained the seven paths listed below, excluding the standard UBT Log.txt.
foreach ($probeReceipt in $probeReceipts) { Remove-Item -LiteralPath $probeReceipt -ErrorAction Stop }
$probeCrashDirectory = 'E:\IA_BuildCache\_r84_host\StackOBot\Saved\Config\CrashReportClient\UECC-Windows-FA89508B44AD842A1FDE22BEBE58544A'
if (@(Get-ChildItem -LiteralPath $probeCrashDirectory -Force).Count -eq 0) { Remove-Item -LiteralPath $probeCrashDirectory -ErrorAction Stop }
```

The exact stated rejection reason was `blocked by policy`, from `exec_command failed: CreateProcess`. The tool's rejected-command wrapper itself omitted 2,599 characters. Duration: rejected before process execution; tool reported 0.0 s. No per-folder denial was identified for this batch, and it was not retried or split into other cleanup attempts.

Remaining generated/regenerated files verified to exist after the rejection:

```text
E:\IA_BuildCache\_r84_host\StackOBot\Saved\Logs\StackOBot-backup-2026.09.29-18.54.22.log
E:\IA_BuildCache\_r84_host\StackOBot\Saved\Config\CrashReportClient\UECC-Windows-FA89508B44AD842A1FDE22BEBE58544A\CrashReportClient.ini
E:\IA_BuildCache\_r84_host\StackOBot\Intermediate\TargetInfo.json
E:\IA_BuildCache\_r84_host\StackOBot\Intermediate\Config\CoalescedSourceConfigs\GameUserSettings.ini
D:\UESource\UnrealEngine\Engine\Programs\UnrealBuildTool\Log-backup-2026.10.02-11.05.13.txt
D:\UESource\UnrealEngine\Engine\Programs\UnrealBuildTool\Log.txt
D:\UESource\UnrealEngine\Engine\Intermediate\UbtRuns\25500_817fa1e247dfd2ed1a7f0502cfbc9c1c
D:\UESource\UnrealEngine\Engine\Intermediate\UbtRuns\42260_817fa1e247dfd2ed1a7f0502cfbc9c1c
```

The UBT log headers identify this run's two builds at 16:35:13 and 16:35:57. The game log identifies the crash-session GUID and the command producing TargetInfo.json. The standard UBT Log.txt was intentionally excluded from the proposed removals because no pre-S6 path inventory established that this standard log path had been absent. The pre-existing host StackOBot.log remains updated by S7; its rotated prior contents remain in the listed backup. The crash-session directory also remains.

The list records observed tool-generated outputs, not a claim that every engine/cache side effect was inventoried before execution. This limits S8 independently of its Git and cleanup failures.

Run-folder files retained:

- `report.md` — the only file authored directly by this job that remains.
- `command.txt`, `events.jsonl`, `prompt.txt`, `role.txt`, `stderr.txt` — pre-existing Relay files, not edited by this job.
- A last-message file was not present at the final pre-report inventory; Relay may create it after this final response.

The first report-save command never started because Windows returned `CreateProcessAsUserW failed: 206 (The filename or extension is too long.)`. It embedded the complete report in a PowerShell command exceeding the process command-line limit. This was a launch-length error, not a filesystem write denial; the report was then saved through the file-edit tool. No rejected cleanup action was retried.

**Changes, deviations, and review disposition**

Commits pushed: **none**. Branches/refs changed: **none**. Archives: **none**. Bench levers or persistent settings changed: **none**. The only requested runtime display command was `stat fps` in the launched process; that process exited. No `-IAIBench` launch was made.

Deviations and limits: S4's middle evidence was lost to PowerShell metadata serialization; it is marked FAIL rather than assumed to pass. S7's early-close guard aborted without acting; the original launcher performed graceful shutdown at its deadline. S8's cleanup was rejected and remaining artifacts are disclosed. `rg` was unavailable during instruction discovery; subsequent reads used PowerShell. Get-PSDrive lacked a free-space value, so a read-only .NET measurement supplied the actual disk readings. No refused direct write was retried, no permission was escalated, no refusal was worked around, and no source-changing milestone workflow was run for this no-source-change smoke test.

Failing steps: **S2, S3, S4 (evidence completeness), S6, S7, S8**. Claude Code must independently review this evidence; this report makes no release-readiness claim.

Automatic approval review rejected the cleanup command that would restore the rotated game log and remove recorded generated files. Its stated reason was **“blocked by policy.”** The command did not execute and was not retried; the listed artifacts remain.

VERDICT: RED
