<!-- Collected by Claude Code (mailbox brief 094-26r) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md at 2026-10-29T20:26:51 IST; 27,531 B, sha256 286BD1732760576B52A72E9CD7A61CEFC5490140B30F017626321AFF6DAB0891; Codex thread 01a14334, resume25, 13:58:54-20:25:00 IST, 23,166 s, 1 try, exit 0. Body below this line is byte-identical. -->
**RED.** The armed kit ran once. EA_Core’s editor exited cleanly, but its Python authoring call failed before creating any package; EA_Travel never started. A pre-existing bound crash-client config disappeared, and the crash-client log records shutdown analytics sending and an HTTP POST. No retry, fix or re-arm was attempted.

094-26r resumed after the PC restart under FA8. [Full report](D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/report.md); evidence in [Receipts/09426r](D:/IA_Europa/Receipts/09426r) and [EA_Core slot 01](D:/IA_Europa/Receipts/09425/fixture-attempts/EA_Core/01). The kit and source remain unchanged. Local documentation commit: `97aec94a5d481b5ac18bfc0a33056a770f0330f9` on `feat/europa-port`; no push or tags.

**Entry and exit proofs.** Fresh [EA8 entry](D:/IA_Europa/Receipts/09426r/P9/entry/summary.json) and [full EA8 exit](D:/IA_Europa/Receipts/09426r/P9/exit/summary.json) both passed against unchanged P9 v2: 472,553 inventory entries, 23,033 binary/config hashes, 104,794,989,456 bytes hashed; protected git and source manifests equal. These proofs do not erase the separately bound Paris Saved/Config failure below. The v2 inventory SHA-256 remains `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`. `m51` remains `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67` with its baseline index/status.

Entry disk free space was C: 18.91, D: 64.56, E: 317.43 GiB. Applied start/warn/floor lines were C: 13/11/10, D: 40/30/25, E: 15/12/10 GiB. [ED3 toolchains](D:/IA_Europa/Receipts/09426r/toolchains.json) passed: VS 2022 17.12.35707.178; VS 2017 15.9.34031.82; MSVC 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023; SDK 10.0.17763.0 and 10.0.22621.0. No build was initiated.

The post-commit [final-state check](D:/IA_Europa/Receipts/09426r/final-state.json) rechecked 1,854 unique hashes: no additional change beyond the recorded missing config, all declared outputs still absent, kit unchanged and feature worktree clean. Its free-space reading was C: 31.56, D: 41.33, E: 304.33 GiB.

Windows had restarted at 13:16:14 IST. The launcher was absent at entry; it first appeared in the observe-only record at 18:08:37 IST, with its folder-qualified helpers excluded. We did not start it or touch foreign processes. The previous interrupted turn used no real slot; its receipts were preserved. FA1’s junction removal and Code’s earlier dry run were not repeated, per FA8.

**Timeline — 2026-10-29, IST.** [Idle pre-check](D:/IA_Europa/Receipts/09426r/idle-precheck-01.jsonl) and [kit gate readings](D:/IA_Europa/Receipts/09425/fixture-attempts/EA_Core/01/idle.jsonl) retain every reading. The pre-check passed at 18:30:21 with 657.61 seconds idle, 7.77 GiB physical free, 13.17 GiB commit free, GPU 1%, and no blocking reason.

| Stage | Start → end | Outcome |
| --- | --- | --- |
| Retry / observe-only wait | 13:58:52 launch; first pre-check 14:18:45 → 18:30:21 | No entry-point start before pre-check PASS |
| EU2 foreign C: pause | 14:30:43 → 14:52:43 | 1,320.05 seconds; resumed after three spaced recovery polls |
| BAT, once | 18:30:21 → 19:00:54 | Exit 2; no retry |
| EA_Core gate, slot 01 | 18:52:58 → 18:56:34 | Eight polls; final three passing; last: idle 372.906 s, physical 6.925 GiB, commit 12.776 GiB, GPU 0% |
| EA_Core editor PID 30564 | 18:58:33 → 18:59:17 | 43.426 seconds; exit 0; normal QUIT_EDITOR shutdown |
| EX3 quarantine | 18:59:20 → 18:59:23 | Zero partial outputs; spent launch lock retained |
| EA_Travel | Not started | No attempt slot/session; blocked after EA_Core STOP |

The pause’s minimum C: free space was 6.910 GiB. No command/file edit occurred inside the open pause; the pause receipt was the only pause write. The window remained FA5’s ending 06:00 IST on 10-30. The earlier 10:00–10:30 quiet slot had already ended before this retry.

**Console, verbatim.** [Original console](D:/IA_Europa/Receipts/09426r/entrypoint-01-console.log); SHA-256 `355db647bd863695a129f834560a34aed35a7d902a5232e4808940908393d9c2`.

```text
Europa fixture authoring: checking the EW2 arming.
EA_Core: attempt 1 of 3; checking protected files, Content, binaries and routing.
EA_Core: protected files, Content, binaries and routing passed.
Waiting: mouse and keyboard idle for 300 seconds.
Waiting: mouse and keyboard idle for 300 seconds.
Waiting: mouse and keyboard idle for 300 seconds.
Waiting: three passing polls, 25 seconds apart.
Measured gate passed: three quiet polls; the launcher is already running.
EA_Core: starting the one authorized authoring session; limit 1800 seconds. Output is recorded in D:\IA_Europa\Receipts\09425\fixture-attempts\EA_Core\01\session.
EA_Core: 0 declared partial outputs quarantined by receipt. A retry requires chat re-arm.
Stopped: [Errno 2] No such file or directory: 'D:\\IA_Europa\\Receipts\\09425\\fixture-attempts\\EA_Core\\01\\authoring.json' No further session will be started.
Stopped: EA_Core did not pass. No later recipe or automatic retry will start.
```

**a. Exit and errors.** [Runtime.log](D:/IA_Europa/Receipts/09425/fixture-attempts/EA_Core/01/session/Runtime.log) and [probe.jsonl](D:/IA_Europa/Receipts/09425/fixture-attempts/EA_Core/01/session/probe/probe.jsonl): 25 probe rows; one engine-exit cleanup with `actor_snapshot="skipped"`; exactly one final `probe_stop`, with root release skipped; no PIE. Runtime.log:673–678 records QUIT_EDITOR and normal shutdown. The editor’s exit was 0; the kit’s later missing-receipt check failed. [Assessment](D:/IA_Europa/Receipts/09426r/assessment.json) records the evidence without claiming successful authoring.

| Runtime.log line | Error / classification |
| --- | --- |
| 639 | Map check says 0 errors and 0 warnings; informational engine/host note. |
| 666 | LogPython Error: traceback starts — our authoring tooling. |
| 667 | Generated script/author.py, line 3 — our authoring tooling. |
| 668 | module['author']('EA_Core', receipt_path) — our authoring tooling. |
| 669 | author_425_fixture.py, line 9 — our authoring tooling. |
| 670 | PROJECT = os.path.normcase(os.path.abspath(unreal.Paths.project_dir())) — our authoring tooling. |
| 671 | AttributeError: 'NoneType' object has no attribute 'path' — our authoring tooling. |

Those six error lines are one traceback. No other error, ensure or fatal was found in the retained session logs; no plugin/probe C++ error was recorded. `os` was None at its first `os.path` access. The cause was not reproduced or repaired in this no-fix job. No new crash/ensure report folder was found. Owned PIDs 30564, 14248, 36112, 22148 and 9892 were all independently absent at closeout; nothing was killed by name.

**b. Outputs and protected files.** All 1,458 existing Content files are unchanged, with zero new Content files, BuiltData or extra packages. Descriptors, .uproject, 110 bound artifacts and three bound manifests are unchanged. Of 42 bound config files, 41 are unchanged and this existing file is missing: `Saved/Config/CrashReportClient/UE4CC-Windows-57F07A004884350F454788B699A5B40C/CrashReportClient.ini` (112 bytes; prior SHA-256 `ffd2f515d5b546c4d9f3a65c58af871cfe2c11812ae3cbd7a5b3a15718906b65`). It was not restored. [Exact before/after evidence](D:/IA_Europa/Receipts/09426r/result-evidence.json).

| Declared path under D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA | Size | SHA-256 |
| --- | --- | --- |
| SM_EA_TwoLod.uasset | Absent | Not applicable |
| EA_Core.umap | Absent | Not applicable |
| EA_Travel.umap | Absent | Not applicable |

A new 96-byte `Saved/Autosaves/PackageRestoreData.json` is recorded in the allowed Saved footprint. It is not a Content package. The missing bound config remains a protected-state STOP independently of the allowed Saved writes.

**c. Authored content against A5.** The [committed recipe](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09411-fixture-recipes.md), [author script](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/author_425_fixture.py) and [A5 script manifest](D:/IA_Europa/Receipts/09411/fixtures/probe-scripts/manifest.json) remain unchanged. `authoring.json` was never written. Every authoring item below is unproved because EA_Core failed before the initial receipt; EA_Travel was not run.

| Recipe item | Observed result |
| --- | --- |
| Exact new packages and refusal to overwrite | Entry confirmed all destinations absent; no authoring output. |
| Sphere LOD0 / Cube LOD1; at least two real render LODs | Not reached. |
| EA_Floor and tagged EA_HideCube, EA_BlinkCube, EA_LOD | Not reached. |
| Movable point light, intensity 5000, radius 1800, shadows | Not reached. |
| EA_Camera fixed transform/FOV 50 | Not reached. |
| Unbound post-process: exposure 1/1, no motion blur, no AA | Not reached. |
| GameModeBase, PlayerStart, saved level and actor/target inventory | Not reached. |
| EA_Travel reuses the two-LOD mesh | No session. |
| AUTHORED_NOT_QUALIFIED receipt; A5 measurement scripts | Receipt missing; scripts not run. No A7/pixel qualification claimed. |

**d. Upload check — FAIL.** The EU4 `-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False` switch is present in the prepared argv, attempt result, runtime-child receipt and Runtime.log:145. Nevertheless, [CrashClient.log](D:/IA_Europa/Receipts/09425/fixture-attempts/EA_Core/01/session/CrashClient.log:86) says `EditorSessionSummary sent report. Type=Shutdown`; line 93 records an HTTP POST to `datarouter.ol.epicgames.com`, `UploadType=eteventstream`, status Processing. This is engine session/shutdown analytics; no crash-payload upload was found. Remote acknowledgement was not independently verified, but the required no-upload proof fails. [Engine summary sender](E:/EpicGames/UE_4.25/Engine/Source/Developer/EditorAnalyticsSession/Private/EditorSessionSummarySender.cpp:279) records and flushes the summary event, then emits that message at line 302. No setting was changed to work around it.

**e. EditorRuns.** Marker `E:/EpicGames/UE_4.25/Engine/Intermediate/EditorRuns/31124` remains zero bytes, with unchanged creation/write time `2026-10-26T18:43:59.9006068Z` and SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`. It is the sole final marker. No manual marker deletion occurred. Endpoint observations do not claim a trace of every transient marker. The 09417 owner kit remains spent.

**f. Ledger classes.** [Ledger summary](D:/IA_Europa/Receipts/09426r/ledger-summary.json) retains all operation and admission results. Runtime classifications: 17 `port_C_exception`, 48 `port_D`, 5 tooling and 2 foreign changes. Own charged growth across completed operations is C: 1114112 bytes, E: 0, 5.1 engine: 0; within EA4’s 250/50 MiB and EG1’s 100 MiB limits. The C: rows comprise eleven crash-client configs, two driver-cache files, one UBT XML cache and three Windows event logs. The two 5.1 hosts/manifests remain frozen, with no launch/build/write operation. OS/tooling metadata access errors remain listed; protected enumeration had zero errors.

A bare UBT child PID 22148 and its conhost child were observed under the editor, alongside two crash-monitor children. No compiler, linker, UHT or ShaderCompileWorker child was recorded. [DesktopPlatformBase.cpp](E:/EpicGames/UE_4.25/Engine/Source/Developer/DesktopPlatform/Private/DesktopPlatformBase.cpp:835) documents stock target discovery through `-Mode=QueryTargets`; the actual child argv was not retained, so that attribution is an inference, not a proved command line. No build was initiated by the agent and bound binaries stayed unchanged.

Foreign roots were recorded without opening their contents: removal of `C:/_DELETE-ME-2026-10-29`; removal/rename of `E:/codex-home` to `E:/_DELETE-ME-codex-copy-2026-10-29`; and new `E:/DaVinciProjects` / `E:/DaVinci Media`. `E:/mcp-servers` was already present at retry entry. The codex-copy rename has matching metadata (12,435 files, 3,865 directories, 31,313,492,502 bytes); writer PID is unknown. The raw ledger flagged its generic port-pattern filenames and remains STOP. [FA6 disposition](D:/IA_Europa/Receipts/09426r/foreign-root-closeout.json) records the evidence and permits report-only closeout; it does not turn the original gate into PASS or authorize another session.

**g. Failed-session quarantine.** [EX3 receipt](D:/IA_Europa/Receipts/09425/fixture-attempts/EA_Core/01/quarantine-receipt.json) records zero declared partial files and zero moves, all declared output paths absent, launch lock unchanged, and `retry_authorized=false`. EA_Core slot 01 is spent. Core slots 02–03 and every Travel slot remain unused; no further session is authorized by this failed run.

**Canary and command audit.** The original [D: launch-second scan](D:/IA_Europa/Receipts/09426r/canary-exit.json) passed with zero matches and zero read errors. The expanded [final canary scan](D:/IA_Europa/Receipts/09426r/canary-final.json) is **GAP**: 334 files, 1012697901 bytes, zero value matches and 3 read errors. It covers new/changed D:/IA_Europa and run-folder files plus exact ledger-attributed C:/E:/engine output files; foreign root contents are excluded. Windows returned Access Denied (5) for the AppXDeploymentServer/Operational, Audio/PlaybackManager and Storage-Storport/Operational EVTX files under C:/Windows/System32/winevt/Logs. Those files remain unproved; no permission/security change or workaround was attempted. The fresh launch positive control was non-vacuous. Recorded child environments exclude blocked names and the canary, including the editor’s 92-name environment. Final report bytes receive a separate hash-based canary check. No full EK1 scan was run, as required by FA6.

[EU1 / EV1 command audit](D:/IA_Europa/Receipts/09426r/command-audit-final.json) passed through its recorded event boundary: 115 filtered invocations, 1 direct read-only startup deviation, zero direct write-capable commands, one closed pause, zero pause-window violations, 71 passing controls. Receipt-sharing refusals before child creation: 1. One exact command/output pair proves a PowerShell `-i` parameter-binding refusal before the wrapper body; five controls qualify that narrow adjudication. Failed read/inspection commands remain in the audit. The original pass receipts and foreign-root STOP are preserved. No execution gate was weakened.

The EU1 deviation was the initial direct `Get-Content` read of FA8, the prior bootstrap/entry helpers, launch time and names-only canary hashes to establish the filtered boundary. All later commands used the retry wrapper. The expanded canary scanner’s nonzero return is retained as GAP; finalization accepts it only to finish this RED report, never to authorize execution.

A report-only final-state checker initially iterated a JSON object as a list and failed. Its schema read was corrected; the checker passed on its second attempt. The failed receipt is preserved. This changed only a run-folder evidence helper; it did not retry authoring or alter source, the kit, a protected file or any gate threshold.

**EN5 applications and limits.** No intent-clause override was applied. [EN5](D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:54) explicitly excludes protected-state changes. Every relevant stock-behaviour question is recorded below; none excuses the RED result.

| Item / source citation | Disposition |
| --- | --- |
| Crash config purge — [GenericPlatformCrashContext.cpp](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/GenericPlatform/GenericPlatformCrashContext.cpp:363); purge at line 909 | Default two-day old-config cleanup is consistent with the missing old file. Explicit protected-config STOP retained; no EN5 waiver. |
| UBT discovery — [DesktopPlatformBase.cpp](E:/EpicGames/UE_4.25/Engine/Source/Developer/DesktopPlatform/Private/DesktopPlatformBase.cpp:835) | Source-supported query interpretation only; actual child argv unavailable. No build/fix permission inferred. |
| Shutdown analytics — [EditorSessionSummarySender.cpp](E:/EpicGames/UE_4.25/Engine/Source/Developer/EditorAnalyticsSession/Private/EditorSessionSummarySender.cpp:279) | Upload proof failed; no EN5 privacy exception. |
| Marker lifecycle — [ET2](D:/IntrusiveAnomalies/_reviews/094-18-ruling.md:54) | Explicit ET2 handling, not an EN5 override; 31124 unchanged. |
| Foreign disk pause / roots — [EU2](D:/IntrusiveAnomalies/_reviews/094-19-ruling.md:46) and [FA6](D:/IntrusiveAnomalies/_reviews/094-25-ruling.md:127) | Explicit amended rules; pause closed, foreign activity recorded, reporting continued only. |

**Commit, flags and hashes.** Documentation-only commit `97aec94a5d481b5ac18bfc0a33056a770f0330f9` (`docs(europa): record 09426 authoring stop and runtime evidence`), authored and committed at `2026-10-29T19:48:54+05:30` with W4 date overrides absent. Changed files: CLAUDE.md, docs/europa/09426-authoring-receipts.json, and docs/sessions/2026-10-29-094-26-real-authoring-stop.md. Diff review and whitespace check passed; no source/build/kit changes, re-arm, push, tags or new archive. [Commit receipt](D:/IA_Europa/Receipts/09426r/documentation-commit.json).

No persistent bench lever was changed. The unchanged kit supplied process-only `-IAIBench`, `-EuropaPhaseA`, Python/EditorScriptingUtilities enablement, 1280×720 window, DDC/UserDir/ShaderWorking/log routing, EU4 switch, `-nowrite` and `-SkipCompile`; no `-unattended`. The process exited. The missing config was deliberately left as evidence rather than restored. The exact argv and environment contracts are in [child-001.json](D:/IA_Europa/Receipts/09425/fixture-attempts/EA_Core/01/ledger/fixture-runtime/child-001.json).

| Bound manifest | Files | SHA-256 |
| --- | --- | --- |
| 4.25 | 27 | c501895c9856638dd76e1c2091be8726f6797014bcb0714e306fd9d6ebd98df6 |
| Baseline51 (frozen) | 39 | b05e6f8c9ce12fa1c5b4dbd142a59383f737bb8a3b7885b7219d1bcdb6c1fc37 |
| ReGate51 (frozen) | 44 | 3865fe0c6ed039d5bad2bc54d1a6a89f11c403e98237b4c6caf74ca661ea12e2 |

Kit BAT SHA-256 `4c5ad14eeb8fd22f0ebcd6ec2349ad28a874c21826cfda36df03c99b45982c41`; kit JSON `01b2ed6ad8c196be5089a4bbb9042024b989cc83a2bdf4dced4ba0e390e841d6`; armed tooling `a8d342595b2f9a900fb8d6078d4976ce41c2e681`, probe source `40a8eaf983518fe1992e172434aa8caf347654f1`. Runtime.log SHA-256 `1d6bbbcd13622d4ded623f7895181b2c0480ed0ad81622aa00ff5ed490dcc190`; CrashClient.log `88400650cc2766dcac7f2546a56d053948927776ac36322c9c89e8548d2e4cdd`; probe.jsonl `fdebbad6e167d07eff690894347d3fceac4e43623337526cfeedad2849dc3a39`.

All 110 artifact paths, sizes and hashes are listed in [binary-hashes.tsv](D:/IA_Europa/Receipts/09426r/binary-hashes.tsv). Every bound EXE/DLL hash follows. Paths are relative to `D:/IA_Europa/`: 425 = `StylizedParisStreet/`, B51 = `Baseline51/StackOBot/`, R51 = `ReGate51/StackOBot/`. These are unchanged records, not new builds.

| Bound EXE/DLL | SHA-256 |
| --- | --- |
| 425/Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe | 6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861 |
| 425/Binaries/Win64/StylizedParisStreet.exe | a2db7a028ebdbfc9d5d74600a35a2951421f8def703ca90f5e307ed460eeb1ac |
| 425/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll | 9458cb952db4c24adc13211118222f1754a7aacaedbc9bd027505a2a382f824f |
| 425/Binaries/Win64/UE4Editor-StylizedParisStreet.dll | 8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c |
| 425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll | 2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940 |
| 425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll | 9627b803fdb4e94c4e80678176c7478435c092b1c7f993b3024cc936bb50c36c |
| 425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll | 0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b |
| 425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll | 9c9bb82daf01ed85900b6794d75570ea38d447e911285c989153e5ec01700f0a |
| 425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll | c3db764fc6566f01ca1f425d3bde0469ca8d1ae88ceb01bfb8f5a82f5e8cfd96 |
| B51/Binaries/Win64/boost_atomic-vc142-mt-x64-1_70.dll | ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765 |
| B51/Binaries/Win64/boost_chrono-vc142-mt-x64-1_70.dll | 17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26 |
| B51/Binaries/Win64/boost_iostreams-vc142-mt-x64-1_70.dll | 7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441 |
| B51/Binaries/Win64/boost_program_options-vc142-mt-x64-1_70.dll | dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5 |
| B51/Binaries/Win64/boost_python39-vc142-mt-x64-1_70.dll | 9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df |
| B51/Binaries/Win64/boost_regex-vc142-mt-x64-1_70.dll | 387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64 |
| B51/Binaries/Win64/boost_system-vc142-mt-x64-1_70.dll | 805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4 |
| B51/Binaries/Win64/boost_thread-vc142-mt-x64-1_70.dll | bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48 |
| B51/Binaries/Win64/D3D12/D3D12Core.dll | 40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2 |
| B51/Binaries/Win64/D3D12/d3d12SDKLayers.dll | 1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052 |
| B51/Binaries/Win64/OpenImageDenoise.dll | 11213c268e6c67262d48e7351e1f6c196acbb91a9a1cf059d4d758c34368f08a |
| B51/Binaries/Win64/StackOBot.exe | b6def734b67b3d03636397ec9d43999458ab86e6baa3db9f21382d394fa2d3d0 |
| B51/Binaries/Win64/tbb.dll | 6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f |
| B51/Binaries/Win64/tbb12.dll | de25bc26f71c5fa53b585116db11d643dc63373fab094e5d786a2ef7eb50786d |
| B51/Binaries/Win64/UnrealEditor-EuropaPhaseAProbe.dll | ac3b1ca44fc45b61cc3a6e4f1fc7271c383d337fa86ab1ba45eddce4e2d78012 |
| B51/Binaries/Win64/UnrealEditor-StackOBot.dll | 4092eb194804229dd2e4e85dec4c50eb96cefd03491b5f7038c084365e43ec19 |
| B51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyBench.dll | 6a8a770188e44660e0d0ba790d14515b600df498eb657bd24af07245dee211d8 |
| B51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyCapture.dll | 9ab3578e0cdab25d9cacf93b226d8d54af2acfda6af0631c5da76f3d5eed9d94 |
| B51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyControlServer.dll | 9a41d9a8cb649f0dbd962082ea0b535f6162b2f611146b20d202f2fd457a4b57 |
| B51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyInjector.dll | 91d14a4af7711c3563dccb30589d013648d9a23b5a7f33563e67f0d06443974e |
| B51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyShaders.dll | 284c64cd7936ea260e780a1a010916a72312a77acb2282a0d413b4196b584b0d |
| B51/Plugins/CaptureBench/Binaries/Win64/UnrealEditor-CaptureBench.dll | e2ef940d6df3ca2d8075015498da127f32540df20b844d1c23e9ffe1de3ce5d8 |
| R51/Binaries/Win64/boost_atomic-vc142-mt-x64-1_70.dll | ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765 |
| R51/Binaries/Win64/boost_chrono-vc142-mt-x64-1_70.dll | 17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26 |
| R51/Binaries/Win64/boost_iostreams-vc142-mt-x64-1_70.dll | 7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441 |
| R51/Binaries/Win64/boost_program_options-vc142-mt-x64-1_70.dll | dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5 |
| R51/Binaries/Win64/boost_python39-vc142-mt-x64-1_70.dll | 9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df |
| R51/Binaries/Win64/boost_regex-vc142-mt-x64-1_70.dll | 387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64 |
| R51/Binaries/Win64/boost_system-vc142-mt-x64-1_70.dll | 805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4 |
| R51/Binaries/Win64/boost_thread-vc142-mt-x64-1_70.dll | bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48 |
| R51/Binaries/Win64/D3D12/D3D12Core.dll | 40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2 |
| R51/Binaries/Win64/D3D12/d3d12SDKLayers.dll | 1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052 |
| R51/Binaries/Win64/OpenImageDenoise.dll | 11213c268e6c67262d48e7351e1f6c196acbb91a9a1cf059d4d758c34368f08a |
| R51/Binaries/Win64/StackOBot-Win64-Shipping.exe | 010ab98e78e1a4a04c4dc1fb3edf3841336b3bc6f4d8a6db5be24841b6845fb4 |
| R51/Binaries/Win64/StackOBot.exe | 5b0fb171a9c65dced68cfd52b8c62540c0ecb29714f30091717b9d055d5f2298 |
| R51/Binaries/Win64/tbb.dll | 6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f |
| R51/Binaries/Win64/tbb12.dll | de25bc26f71c5fa53b585116db11d643dc63373fab094e5d786a2ef7eb50786d |
| R51/Binaries/Win64/UnrealEditor-EuropaPhaseAProbe.dll | f8cab87d9a3e0bb93a8fc92dd20c22d25851c83051f3cd28a0e97da0c6af1b21 |
| R51/Binaries/Win64/UnrealEditor-StackOBot.dll | 741b766316baa00dac0a63631dd407b7262ce47fb5f3c677f8d135054117cc0f |
| R51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyBench.dll | 128e48761dfc075662200648667725e426f8e978d8400a8aedbab9c66612c33c |
| R51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyCapture.dll | 0e14f06a515844ad27174789fe455284c798a64e8fffe3a8bd12f251e4f162e6 |
| R51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyControlServer.dll | 3e740f3b9bcf76cc060a25e3eb6602e5a34b375c9e336b86bd065dac129a8a80 |
| R51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyInjector.dll | 88f288eed3763402f91fc5e38fe8612451df8cc3f54db95a6f391f0aefa7e4b5 |
| R51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyShaders.dll | f8736404b31745068a78638807114d25a4f42a3df093f4f2e8c8e28aa7c93c9c |
| R51/Plugins/CaptureBench/Binaries/Win64/UnrealEditor-CaptureBench.dll | 89032f62c2ff43f9f55cbce60a3abd8e68378287bf8ec7fad24f3cec0c11d7f6 |

Recorded runtime-tree executable hashes:

| Executable | SHA-256 |
| --- | --- |
| C:\Windows\System32\conhost.exe | c86146188b78e484f32cc29e2331689659943b47689a85570342044f499938ac |
| E:\EpicGames\UE_4.25\Engine\Binaries\DotNET\UnrealBuildTool.exe | 07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0 |
| E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe | ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7 |
| E:\EpicGames\UE_4.25\Engine\Binaries\Win64\UE4Editor.exe | 4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387 |

Additional kit-bound engine EXE/DLL hashes (unchanged):

| Engine file | SHA-256 |
| --- | --- |
| E:\EpicGames\UE_4.25\Engine\Binaries\Win64\UE4Editor.exe | 4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387 |
| E:\EpicGames\UE_4.25\Engine\Plugins\Experimental\PythonScriptPlugin\Binaries\Win64\UE4Editor-PythonScriptPlugin.dll | b62d70e6c367674cc21ad8d294a63e32788074cd967fa89eba514709b2a91378 |
| E:\EpicGames\UE_4.25\Engine\Plugins\Editor\EditorScriptingUtilities\Binaries\Win64\UE4Editor-EditorScriptingUtilities.dll | 39800186398b4f7de8fc5c7b29e954f0d31c9fa3341244c06b25d9cbf00fe5b1 |
| E:\EpicGames\UE_4.25\Engine\Binaries\ThirdParty\Python\Win64\python.exe | 52b4dead44802e263f54881d434f6dec5eab9ff3bd69a9d0c1408065543c3e55 |

VERDICT: RED | EA_Core authoring failed; a bound config disappeared and shutdown analytics sending was recorded; EA_Travel was not started.
