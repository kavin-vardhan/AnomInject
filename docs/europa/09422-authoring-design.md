094-22 design, 2026-10-28. Authority: 094-21-ruling EW1–EW3. This job prepares and dry-runs the kit; a later job executes it. The spent 09417 owner kit is unchanged.

The mechanism needs no project descriptor or config-file edit. Each recipe gets one ordinary 4.25 Editor session, using E:/EpicGames/UE_4.25/Engine/Binaries/Win64/UE4Editor.exe and the existing D:/IA_Europa/StylizedParisStreet/StylizedParisStreet.uproject. The initial map is the existing /Engine/Maps/Entry. The stock PythonScriptPlugin and EditorScriptingUtilities are enabled only by -EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities. PluginManager.cpp:567–603 parses this list and configures the enabled plugin set; this branch does not save the descriptor. Both stock plugin DLLs exist and are bound by the kit.

The command uses -ExecutePythonScript=<attempt>/script/author.py. EditorPythonExecuter.cpp:139–179 waits for an editor world and the completed asset registry, then executes PY with that file. Lines 183–195 queue QUIT_EDITOR on the following tick. Lines 203–226 parse the switch, reject commandlets and create the executor. This is an Editor script, not a Python commandlet. The installed patchlevel.h:23–30 identifies Python 2.7.14; the author script and generated entry script use compatible syntax.

The generated entry script calls tools/europa/author_425_fixture.py with an explicit recipe and an absolute receipt path. Those arguments live in the script, never in child-environment wrapper metadata. EA_Core creates the shared sphere/cube two-LOD mesh and its map. EA_Travel loads the previously qualified Core mesh without saving it and creates only its map. Both retain A5's actor labels, locations, camera, fixed exposure, disabled AA/motion blur and movable light. No PIE or anomaly-driving command is issued.

Declared packages are exclusively:

| Recipe | Package | Exact file |
| --- | --- | --- |
| EA_Core | /Game/EuropaPhaseA/SM_EA_TwoLod | D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/SM_EA_TwoLod.uasset |
| EA_Core | /Game/EuropaPhaseA/EA_Core | D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/EA_Core.umap |
| EA_Travel | /Game/EuropaPhaseA/EA_Travel | D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/EA_Travel.umap |

All destinations must be absent both at wrapper preflight and inside the script. Existing fixture destinations are never reused or overwritten; saving a package created in the same session is permitted. Unexpected additional files, including a BuiltData package, stop qualification. The Core success record binds its output bytes before Travel is admitted. Every existing Paris and plugin Content file is SHA-256 checked before and after each session, including Core's files during Travel. Existing .uproject, Config and Saved/Config files are also checked, and any new or modified config file refuses qualification.

EditorLevelLibrary.cpp:506–549 refuses an existing destination and saves the new world; FileHelpers.cpp:4225–4238 converts the exact map package to its file name. EditorLevelLibrary.cpp:635–680 saves the current level (and any MapBuildData); the fixture does no static-light build and the postflight rejects undeclared output. EditorAssetLibrary.cpp:1111 provides the explicit mesh save, and EditorStaticMeshLibrary.cpp:242 provides the LOD copy. Only new fixture packages are passed to save APIs.

Unattended decision: omit -unattended, using EW3's pre-approved bounded alternative. WindowsPlatformCrashContext.cpp:598/603/624–633 shows that unattended plus the EU4 false override refuses monitor reporting; :768/782/801–803 disables the fallback crash client. However, MessageDialog.cpp:64–92 includes an implicit Yes default for YesNoYesAllNoAllCancel. I cannot establish that every conversion, checkout and save-on-exit prompt is non-destructive under unattended. This is not a claim that all dialogs are safe. The executor itself uses GIsRunningUnattendedScript (EditorPythonExecuter.cpp:145), so the script explicitly refuses existing assets and avoids save-all/checkout/convert APIs. A blocking startup or crash dialog is bounded by the owned-tree timeout.

Every invocation includes the exact EU4 override:
`-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False`.
It is mandatory; a missing, duplicate or contradictory override refuses. Ensures therefore return before reporting. Without -unattended an interactive crash window may appear; the kit never clicks Send and terminates only its own tree, including the crash client, at the time limit. The runtime result must be a clean exit with the probe's skipped world_cleanup row and probe_stop; a crash never qualifies.

The argv also includes -nowrite, a per-process config-write inhibitor: ConfigCacheIni.cpp:1476–1481 returns before any FConfigFile write. The EU4 value itself is excluded from save at :1531/1543. No persistent setting is changed. -SkipCompile prevents forced startup compilation (:5438–5442), and -stdout makes an incompatible-module check fail rather than prompt for compilation (LaunchEngineLoop.cpp:5473–5477). The preflight binds the existing editor .target and all EW1 binaries, so a missing-target prompt is refused before launch. No automatic rebuilding is authorized.

Exact argv template, with only recipe/slot-derived absolute paths substituted:

```
E:/EpicGames/UE_4.25/Engine/Binaries/Win64/UE4Editor.exe
D:/IA_Europa/StylizedParisStreet/StylizedParisStreet.uproject
/Engine/Maps/Entry
-UserDir=D:/IA_Europa/StylizedParisStreet/
-ShaderWorkingDir=D:/IA_Europa/Caches/425/ShaderWorking/
-DDC=Europa425DDC
-abslog=<attempt>/session/Runtime.log
-AbsCrashReportClientLog=<attempt>/session/CrashClient.log
-NoLiveCoding -NoHotReload -windowed -ResX=1280 -ResY=720
-IAIBench -EuropaPhaseA -EuropaProbeOut=<attempt>/session/probe
-EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities
-ExecutePythonScript=<attempt>/script/author.py
-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False
-nowrite -SkipCompile -stdout
```

There is no EuropaTickAudit or probe action script: only passive world/cleanup receipts are needed. The existing DDC graphs are verified, not edited. Paths.cpp:1557/1585 parses the UserDir and ShaderWorkingDir redirects. DDC writes are D:/IA_Europa/Caches/425/Boot.ddc and Local; shader scratch is D:/IA_Europa/Caches/425/ShaderWorking. TEMP/TMP and .NET routing use the attempt's D:/IA_Europa/Temp/09422/fixture-attempts/<recipe>/<slot> subtree. Authoring, console, idle, child, deadline and verification receipts are under the matching Receipts/09422/fixture-attempts subtree. Saved may receive stock logs/crash files under the owned host; config writes are inhibited and postflight checked.

C: has only EA4's conservatively charged engine-user/XML/UBT/crash-monitor/driver/Windows-WER classes, never DDC or shader scratch. E: has the ET2 zero-byte EditorRuns/<pid> runtime marker and stock stale-marker removal (Source/Developer/HotReload/Private/HotReload.cpp:389–434), receipted by process identity; all other engine mutations refuse except the pre-existing BuildRules allowance. The existing 31124 marker is never removed by this job. Epic launcher maintenance is foreign under ET3/EU3, but disk limits still apply. The launcher must already be running; the kit never starts it. Its named processes are recorded and excluded from the Unreal gate.

The measured gate requires human idle >=300 seconds, free physical memory >=6 GiB, GPU <=15%, no foreign Unreal process (including bare UBT/UHT and dotnet UBT), and three polls >=24 seconds apart. Unknown readings fail closed. Waiting uses the existing measured four-hour ceiling, with one plain message a minute naming every unmet condition. ED1/EV3 and EU2 remain active independently.

Each session has a 1,800-second wall-clock limit from process creation. At the deadline the retained-handle/creation-time owned-tree tracker stops only that editor and its descendants, including a crash reporter; a harmless child and grandchild prove this without Unreal. The launch reservation is permanent once process creation succeeds, including bookkeeping failures. Each recipe has three fresh prelaunch attempt slots and at most one launched session. Travel requires Core's successful postflight. No retry may overwrite partial packages. There is no automatic relaunch after an interrupted runtime session.

The kit binds all three EW1 manifests (27/39/44), all bound files, host sources/routing, its tooling, the two stock scripting plugin DLLs/descriptors, and its generated script. Dry runs use separate throwaway slot/state roots, enforce the same authorization, hash/routing/content and argv checks, and use labelled synthetic idle readings. They never call the runtime process creator. Real slots remain absent and unused.

Source root for citations above: E:/EpicGames/UE_4.25/Engine. PluginManager.cpp is Source/Runtime/Projects/Private; EditorPythonExecuter.cpp, EditorLevelLibrary.cpp, EditorAssetLibrary.cpp and EditorStaticMeshLibrary.cpp are Plugins/Editor/EditorScriptingUtilities/Source/EditorScriptingUtilities/Private; ConfigCacheIni.cpp and MessageDialog.cpp are Source/Runtime/Core/Private/Misc; WindowsPlatformCrashContext.cpp is Source/Runtime/Core/Private/Windows; LaunchEngineLoop.cpp is Source/Runtime/Launch/Private; FileHelpers.cpp is Source/Editor/UnrealEd/Private; Paths.cpp is Source/Runtime/Core/Private/Misc; patchlevel.h is Source/ThirdParty/Python/Win64/include.
