<!-- Collected by Claude Code (mailbox brief 094-22) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md; sha256 082AE19FF90D89944257C9424EA8A59F95FD2892DF0994289F0FA07838C30FB3, 18332 bytes; thread 01a14334, resume 2026-10-28 04:53:35 - 07:29:49 IST, 9374 s, exit 0; body below is byte-identical -->
094-22 is complete: the automated EA_Core / EA_Travel kit is armed and both final dry runs passed on throwaway slots. All six real attempt slots and real state folders remain absent. No Unreal process, build, cook, commandlet or UAT was launched. The 110 EW1 artifacts, three manifests, fifteen probe source files, 1,458 existing Content files and spent 09417 kit are unchanged. No persistent setting changed; no push, tags or new binary archives. Real authoring and runtime clean-exit proof remain for the later authorized job.

Final feature commit: `1a9afc129f2174bdf0e738b0b9960ec5d232ddc4`. The kit binds tooling commit `8abb79edde194214c3bf3e620269f89139c7cdf5`; the last commit changes only CLAUDE.md and the session journal. W4 date overrides were absent.

Both EA8 proofs pass against P9 v2: 472,553 inventory files, 1,023 previously admitted metadata entries, and 23,033 selected binary/config hashes covering 104,794,989,456 bytes. Protected git and original source manifests match. P9 v2 remains `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`. The ET2 marker 31124 remains unchanged. Evidence: [entry](D:/IA_Europa/Receipts/09422/P9/entry-corrected/summary.json), [exit](D:/IA_Europa/Receipts/09422/P9/exit/summary.json), [bound-file check](D:/IA_Europa/Receipts/09422/bound-exit-check.json).

The initial entry comparison omitted 09421's accepted metadata history. All 1,022 affected rows matched its accepted exit receipt in size, hash, mtime and file identity. That initial comparison is preserved; the corrected entry includes the accepted history without editing v2. See [carry-forward proof](D:/IA_Europa/Receipts/09422/P9/accepted-carry-forward.json).

| Free space | Entry GiB | Exit GiB | Start / warning / floor GiB |
|---|---:|---:|---|
| C: | 20.13 | 19.99 | 13 / 11 / 10 |
| D: | 73.41 | 71.73 | 40 / 30 / 25 |
| E: | 33.45 | 33.43 | 25 / 20 / 15 |

ED3 confirms VS 2022 17.12 and VS 2017 Build Tools 15.9; MSVC 14.16.27023, 14.36.32532, 14.38.33130 and 14.42.34433; SDK 10.0.17763.0 and 10.0.22621.0. No toolchain change. m51 remains `53bf725`, with its original index and 15 baseline untracked entries; 72 refs and five prunable registrations remain. Baseline51 stays `d7eee97`, ReGate51 stays `40a8eaf`. [Final git/W4 proof](D:/IA_Europa/Receipts/09422/final-state.json).

The [bound design note](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09422-authoring-design.md) gives the full 4.25 source reading. Each recipe uses ordinary UE4Editor.exe with the stock Python and EditorScriptingUtilities plugins enabled only for that process. The generated `<attempt>/script/author.py` calls the committed `tools/europa/author_425_fixture.py` for exactly one recipe. No descriptor or config-file edit is required. CLI enabling is in [PluginManager.cpp](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Projects/Private/PluginManager.cpp:567); execution after world/registry readiness and next-tick QUIT_EDITOR are in [EditorPythonExecuter.cpp](E:/EpicGames/UE_4.25/Engine/Plugins/Editor/EditorScriptingUtilities/Source/EditorScriptingUtilities/Private/EditorPythonExecuter.cpp:139). The script parses under the installed Python 2.7.14.

Only these new files are declared:

| Recipe | Exact output |
|---|---|
| EA_Core | `D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/SM_EA_TwoLod.uasset` |
| EA_Core | `D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/EA_Core.umap` |
| EA_Travel | `D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/EA_Travel.umap` |

Existing destinations and partial outputs from earlier attempts refuse. Core must qualify before Travel; Travel rehashes Core outputs and loads its mesh without saving it. Existing Content and config/descriptor files are checked before and after. Undeclared files, including an unexpected BuiltData package, refuse qualification.

The per-session limit is 1,800 seconds from process creation. The kit omits `-unattended` under EW3's fallback: [MessageDialog.cpp](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Misc/MessageDialog.cpp:64) includes a destructive-capable implicit Yes default, so all three required unattended guarantees were not established. EU4's false override is mandatory. Ensures return before reporting; an interactive crash window can remain, but the kit never sends it and the deadline stops only the owned tree, including its crash client. [WindowsPlatformCrashContext.cpp](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Windows/WindowsPlatformCrashContext.cpp:598).

`-nowrite` inhibits persistent config writes ([ConfigCacheIni.cpp](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Misc/ConfigCacheIni.cpp:1476)). `-SkipCompile`, `-stdout` and bound target/binary preflight prevent automatic rebuild admission ([LaunchEngineLoop.cpp](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Launch/Private/LaunchEngineLoop.cpp:5438)). A clean result requires the skipped exit `world_cleanup` row and exactly one final `probe_stop`, with no PIE. A crash or incomplete authoring receipt fails.

For each real slot, `A` below is exactly `D:/IA_Europa/Receipts/09422/fixture-attempts/<EA_Core|EA_Travel>/<01|02|03>`. The six fully expanded argv arrays are in [exact-argv.json](D:/IA_Europa/Receipts/09422/arming/exact-argv.json).

```text
E:/EpicGames/UE_4.25/Engine/Binaries/Win64/UE4Editor.exe
D:/IA_Europa/StylizedParisStreet/StylizedParisStreet.uproject
/Engine/Maps/Entry
-UserDir=D:/IA_Europa/StylizedParisStreet/
-ShaderWorkingDir=D:/IA_Europa/Caches/425/ShaderWorking/
-DDC=Europa425DDC
-abslog=A/session/Runtime.log
-AbsCrashReportClientLog=A/session/CrashClient.log
-NoLiveCoding -NoHotReload -windowed -ResX=1280 -ResY=720
-IAIBench -EuropaPhaseA -EuropaProbeOut=A/session/probe
-EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities
-ExecutePythonScript=A/script/author.py
-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False
-nowrite -SkipCompile -stdout
```

Expected footprint: DDC Boot.ddc/Local and shader scratch stay under `D:/IA_Europa/Caches/425`; TEMP/TMP and .NET scratch use the corresponding `D:/IA_Europa/Temp/09422/fixture-attempts/...` slot. Logs, probe and authoring receipts use A; stock Saved logs/crashes stay in the owned host. C: is limited to the conservatively charged EA4 user/XML/crash/driver/WER classes. E: permits ET2 runtime markers and the existing EG1 allowance; launcher maintenance remains foreign. Routing citations: [Paths.cpp](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Misc/Paths.cpp:1557) and [HotReload.cpp](E:/EpicGames/UE_4.25/Engine/Source/Developer/HotReload/Private/HotReload.cpp:389). No footprint was generated by Unreal in this job.

The full measured profile keeps 300 seconds of human idle, 6 GiB free physical memory, GPU at most 15%, no foreign Unreal/UBT/UHT process, and three spaced polls. The launcher must already be running; its family is recorded and excluded from the Unreal gate. The kit never starts it. During a session, a fresh collector checks the same profile about every 25 seconds; a lost/unknown condition or 75-second stale sample stops the owned tree. A blocked collector cannot postpone the independent deadline. EW2's spent lock prevents automatic relaunch.

| Check | Result |
|---|---|
| Kit boundaries, scope, EU3/EU4, refusal reasons, aliases, slots, post-creation locks and deadline ownership | 89/89 PASS |
| Measured-gate regressions, including bare UBT/UHT and dotnet UBT | 48/48 PASS |
| Mid-session gate, stale/blocked collector and owned-tree stop | 17/17 PASS |
| Static check on three hosts plus tracked probe template | 33 cases per source, all PASS |
| Collector child environment | Metadata and synthetic canary absent |
| EA11 plugin headers and in-memory mutant | 7 headers PASS; probe not applicable |

Receipts: [kit tests](D:/IA_Europa/Receipts/09422/self-tests/final/result.json), [measured tests](D:/IA_Europa/Receipts/09422/self-tests/final/measured-tests.json), [session tests](D:/IA_Europa/Receipts/09422/self-tests/session-gate-final/result.json), [static mutants](D:/IA_Europa/Receipts/09422/self-tests/static-four-sources.json), [EA11](D:/IA_Europa/Receipts/09422/ea11.json).

The eight added static mutants cover StaticFindFirstObject, FObjectIterator, ForEachObjectOfClass, GetObjectsOfClass, ClearInternalFlags(RootSet), LoadSynchronous, constructing a weak pointer from a raw cleanup world, and a plain Stop dereference. All fail as intended; the real probe source is unchanged. Alias tests reject short-name spellings and a real junction; D: short-name generation was disabled and was not changed. Timeout tests use harmless Python children/grandchildren and preserve the sibling control.

G1 removes every EUROPA_* metadata name from native build/runtime and collector children while preserving other permitted base values. Synthetic 4.25/5.1 makefiles pass EN1's byte comparison; metadata, canary and foreign-prefix mutants fail. The six existing makefiles retain their exact prior bytes and six old metadata names under EW3 until their next EO3 refresh. No UBT ran here.

The failed harness receipts remain visible: initial missing accepted-history comparison; first test suite's leftover owned junction; validation-driver import before tests; closeout's one-name versus six-name metadata assertion; and summary's own RUNNING-receipt assertion. Each was corrected without weakening a safety gate. The junction and own incomplete snapshot were removed by receipt, with target data retained. The final kit candidate and regressions pass; no D2 problem exceeded two attempts. Details and resolutions are in [ledger summary](D:/IA_Europa/Receipts/09422/ledger-summary.json).

`authorized_by_chat` cites `D:/IntrusiveAnomalies/_reviews/094-21-ruling.md#EW2`, with EW3 policy. `runtime_authorized` permits only two 4.25 authoring sessions: one EA_Core and one EA_Travel, new declared packages only, 1,800 seconds each. No PIE, cook, packaging, commandlet, production edit, config edit, build or query is authorized. Three prelaunch slots exist in the configuration per recipe; one successful process creation permanently spends that recipe, including later bookkeeping failure.

Real receipt slots are `D:/IA_Europa/Receipts/09422/fixture-attempts/EA_Core/01`, `/02`, `/03` and the corresponding `EA_Travel/01`, `/02`, `/03`. TEMP slots mirror them under `D:/IA_Europa/Temp/09422/fixture-attempts`. Recipe state is `Receipts/09422/fixture-state/EA_Core` and `/EA_Travel`; the global lock is `/global`. Every real slot/state path is absent. The 09417 launched mark and kit remain spent and unchanged.

| Bound item | SHA-256 |
|---|---|
| Europa425-FixtureAuthoring.bat | `4c5ad14eeb8fd22f0ebcd6ec2349ad28a874c21826cfda36df03c99b45982c41` |
| Europa425-FixtureAuthoring.json | `42c89db2f4fa5aa8018c6f95d937e03b74d9f39127a270b7b6e50571b1036989` |
| EW1 4.25 manifest, 27 artifacts | `c501895c9856638dd76e1c2091be8726f6797014bcb0714e306fd9d6ebd98df6` |
| EW1 Baseline51 manifest, 39 artifacts | `b05e6f8c9ce12fa1c5b4dbd142a59383f737bb8a3b7885b7219d1bcdb6c1fc37` |
| EW1 ReGate51 manifest, 44 artifacts | `3865fe0c6ed039d5bad2bc54d1a6a89f11c403e98237b4c6caf74ca661ea12e2` |

The [arming receipt](D:/IA_Europa/Receipts/09422/arming-final/receipt.json) records all 60 tooling hashes, 199 host bindings, seven engine executable/plugin bindings, slots and scope. Every individual EXE/DLL and other artifact hash remains in the unchanged [4.25](D:/IA_Europa/Receipts/09421/qualification/final-425-binaries.json), [Baseline51](D:/IA_Europa/Receipts/09421/qualification/final-baseline51-binaries.json) and [ReGate51](D:/IA_Europa/Receipts/09421/qualification/final-regate51-binaries.json) manifests.

The actual new .bat passed both recipes on fresh `Receipts/09422/dry-run-final` and `Temp/09422/dry-run-final` roots. P9, Content, config, executable/tooling hashes, routing, authorization and argv checks were real; gate samples were explicitly synthetic. Zero Unreal launches. [Final dry-run receipt](D:/IA_Europa/Receipts/09422/dry-run-final-result.json). The earlier dry run is retained separately; the final re-bound kit has its own complete receipt.

The complete [console catalog](D:/IA_Europa/Receipts/09422/arming-final/console-catalog.json) binds 177 authored print/refusal expressions with source lines. Dynamic paths, measured values, OS errors and Unreal output are recorded verbatim in the listed logs. The ordinary visible lines/templates are:

```text
Europa fixture authoring: checking the EW2 arming.
{recipe}: attempt {n} of 3; checking protected files, Content, binaries and routing.
{recipe}: protected files, Content, binaries and routing passed.
Waiting: three passing polls, 25 seconds apart.
Waiting: {each unmet condition joined by ; }.
Measured gate passed: three quiet polls; the launcher is already running.
Stopped: no measured idle window within four hours. No editor was started.
{recipe}: armed dry run passed. No editor was started; real slots remain unused.
{recipe}: starting the one authorized authoring session; limit 1800 seconds. Output is recorded in {session_path}.
{recipe}: clean exit and output checks passed; this recipe is now spent.
{recipe}: previously completed; its launch authorization remains spent.
Stopped: {recipe} did not pass. No later recipe or automatic retry will start.
Stopped: {reason} No further session will be started.
Both recipe dry runs finished. No editor was started.
Both recipe sessions finished. The two launch authorizations are spent.
```

Waiting names each unmet condition: mouse/keyboard idle, physical memory, GPU, readable process list, launcher already running, foreign Unreal PID, or disk floor; unavailable readings are named. Every refusal retains its own reason. Mid-session gate loss records its reasons, stops only the owned tree and leaves the recipe spent.

Under `Receipts/09422`, the log/receipt paths are:

```text
fixture-runs/<uuid>/wrapper.json
fixture-runs/<uuid>/<recipe>-disk.json
fixture-runs/<uuid>/<recipe>-entry-result.json
fixture-attempts/<recipe>/<slot>/claim.json
fixture-attempts/<recipe>/<slot>/prepared.json
fixture-attempts/<recipe>/<slot>/preflight-console.log
fixture-attempts/<recipe>/<slot>/P9/entry-fixture/
fixture-attempts/<recipe>/<slot>/idle.jsonl
fixture-attempts/<recipe>/<slot>/session-gate.jsonl
fixture-attempts/<recipe>/<slot>/runtime-ledger-console.log
fixture-attempts/<recipe>/<slot>/ledger/fixture-runtime/child-001.log
fixture-attempts/<recipe>/<slot>/ledger/fixture-runtime/child-001.json
fixture-attempts/<recipe>/<slot>/session/Runtime.log
fixture-attempts/<recipe>/<slot>/session/CrashClient.log
fixture-attempts/<recipe>/<slot>/session/probe/probe.jsonl
fixture-attempts/<recipe>/<slot>/authoring.json
fixture-attempts/<recipe>/<slot>/attempt-result.json
fixture-state/<recipe>/launch-reserved.json
fixture-state/<recipe>/launched.json
fixture-state/<recipe>/success.json
```

The fresh canary was present and hash-verified before filtering. The launch-second scan found zero matches in 686 created/changed files, 1,946,142,485 bytes; an all-extension scan is repeated after report creation. The full EK1 scan covered 44,083 files and 24,435,699,433 bytes, with zero read errors, zero oversized skips and zero secret values after classification. Eight changed name contexts were individually checked; 289 others match the prelaunch hashes. Accepted binary-extension exclusions remain explicit. The two old locked temp files were read with the accepted sharing mode, unchanged. [EK1 qualification](D:/IA_Europa/Receipts/09422/secret-scan/qualified.json); [launch-second canary scan](D:/IA_Europa/Receipts/09422/canary-exit.json); [post-report canary scan](D:/IA_Europa/Receipts/09422/canary-final.json).

EU1 audit: one direct read-only bootstrap inspection is recorded as a deviation; the obsolete 09421 wrapper refused the fresh canary hash before starting its child. All other shell entries use the filtered wrapper. Zero direct write-capable commands and zero pause-window/lifecycle violations. The audit's 19 can-fails pass. Two concurrent wrapper log-sharing refusals occurred before child creation; calls were then serialized. Final command details are in [report-time audit](D:/IA_Europa/Receipts/09422/command-audit-report.json) and [final audit](D:/IA_Europa/Receipts/09422/command-audit-final.json).

EU2 pauses: none. Charged C:, E: and 5.1-engine growth is zero. The ledger retains 232 listed OS/tooling access denials; protected enumeration and full EK1 have zero errors. No foreign process was stopped. The [ledger summary](D:/IA_Europa/Receipts/09422/ledger-summary.json) preserves failed child receipts and their resolutions, rather than converting them into successful runs. Work finished within the nine-hour limit.

One interpretation is recorded for ratification: no-overwrite forbids every pre-existing package/file and every earlier-attempt partial output. Stock NewLevel initially serializes the new map ([EditorLevelLibrary.cpp](E:/EpicGames/UE_4.25/Engine/Plugins/Editor/EditorScriptingUtilities/Source/EditorScriptingUtilities/Private/EditorLevelLibrary.cpp:529)); SaveCurrentLevel serializes that same newly created map after authoring (:635). Same-session writes are confined to the declared new package. This uses [094-13-ruling EN5](D:/IntrusiveAnomalies/_reviews/094-13-ruling.md) with EW2; it does not permit changing existing Content. No package serialization ran here. No other EN5 exception was applied.

Local commits on `feat/europa-port`:

- `6e46b9fe793fe719d0a4ea48024359e2969d7467` — feat(europa): prepare bounded A6 fixture authoring kit.
- `572f5c06f941c1fb2912e9e5595470498252b642` — fix(europa): keep entry failure messages accurate after a session.
- `8abb79edde194214c3bf3e620269f89139c7cdf5` — fix(europa): enforce measured gate throughout authoring sessions.
- `1a9afc129f2174bdf0e738b0b9960ec5d232ddc4` — docs(europa): record armed A6 fixture kit and dry-run evidence.

No commits were pushed. No tags, binary archives or Unreal bench settings were changed. The next job must obtain the later chat ruling and prove real authoring plus clean exit; this job stops at the armed, dry-run kit.

VERDICT: GREEN | EW2 fixture-authoring kit armed and dry-run; bindings, P9, scans and gates pass; zero Unreal launches
