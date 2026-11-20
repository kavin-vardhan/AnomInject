<!-- Collected by Claude Code (mailbox brief 094-15) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md; sha256 2A77886798DA9C7FB96D4F78C10E2E2C7D3156BD6E863F46D0EE538C09D8B202, 33903 bytes; thread 01a14334, resume 23:05:55-01:43:49 IST, 9475 s, exit 0; body below is byte-identical -->
**094-15 — Europa Phase A, A5 complete.** Report assembled 2026-10-26T01:38:53+05:30; work began 2026-10-25 23:05:54 IST.

All six final Editor/Game Development rebuilds passed, including the identical probe against release `d7eee97`. Each has zero C++ errors or warnings, zero engine and engine-plugin actions, and a passing environment proof. The clean-engine query found zero outdated actions. The 27/39/44 binary manifests are rebound, and the owner kit passed its real preflights in dry-run mode with zero launches.

P9 entry and exit passed. No protected-state, secret/canary, disk-floor or engine-write STOP occurred. EO1's corrected reader passed its second validation; EO2's fallback was not used. Feature HEAD is `3d20b5c2a994de539d630d90d1d0e7f4e5aa4dd6`, local only. The compiled candidate/probe source is `550e9b79e4d60b553cceb4f7b7dba6b22c2386a7`.

This completes A5 readiness. Runtime, picture, GC, tick, content-authoring and owner observations remain A6 onward. No editor, game, commandlet, cook, UAT or ShaderCompileWorker was launched. No push, tag, setting change or foreign-process stop occurred.

**Entry, exit and protected state**

| Proof | Result | Inventory files | Full binary/config hashes | Bytes hashed | Admitted EG1 paths |
|---|---|---|---|---|---|
| Entry | PASS | 472553 | 23033 | 104794989456 | 1023 |
| Exit | PASS | 472553 | 23033 | 104794989456 | 1023 |

P9 v2 was re-derived byte-for-byte from v1 and the removal manifest. Its SHA-256 remains `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74` (472,552 lines). The extra inventory entry and inherited bookkeeping are handled by the existing accepted proof; no baseline was changed. The full all-file hash was not repeated. Protected inventories were error-fatal; selected binaries/configs were fully hashed. Both originals, the retained release baseline, remaining protected worktrees, engine protected files and D: overflow data matched. The m51 checkout remains `53bf725`, with the same index hash and 15 untracked entries; master, release and tags are unchanged. The five prunable worktree registrations were left alone. Entry also passed read-only connectivity fsck and the earlier ledger integrity check. [Entry proof](<D:/IA_Europa/Receipts/09415/P9/entry/summary.json>); [exit proof](<D:/IA_Europa/Receipts/09415/P9/exit/summary.json>).

| Reading | C: GiB | D: GiB | E: GiB |
|---|---|---|---|
| Entry | 18.89 | 82.24 | 137.11 |
| Exit | 18.69 | 81.28 | 133.06 |

ED1 start/warning/floor thresholds were C: 13/11/10, D: 70/55/50, E: 25/20/15 GiB on resolved volumes. Every operation stayed above its warning margin. Visual Studio remained 2022 Community 17.12.35707.178 and 2017 Build Tools 15.9.34031.82. Installed MSVC directories were 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023; no 14.44 directory. SDKs 10.0.17763.0 and 10.0.22621.0 remained available. Actual compiler responses below confirm the pinned selections. [Toolchain inventory](<D:/IA_Europa/Receipts/09415/toolchains/installed.json>).

**EO6 and EO1 validation**

| Check | Observed answer |
|---|---|
| EO6 masked receipt | 17 checks pass; the raw-value mutant is rejected. |
| Retained 4.25 exit-6 log | 3 project compiles executed; WorldProbe and ProbeDriver failed. |
| Retained 4.25 exit-0 log | 5 executed actions: 2 compiles, .lib, .dll and .target. |
| 5.1 format coverage | 800 status lines: 29 retained Editor, 765 retained Game, 6 current failed Editor. |
| Current failed 5.1 graph join | 3 executed compiles; 3 cancelled dependents recorded separately. |
| Engine-output mutant | Rejected. |
| No graph match mutant | Rejected. |
| Ambiguous graph match mutant | Rejected. |

EO6 invocation 1 lacked its output parent; invocation 2 passed after that harness repair and the masked-value assertion. EO1 attempt 1 passed the two known answers, 794 retained 5.1 lines and the three mutants before any new build. The first current 5.1 failure exposed cancelled-dependent status lines. Attempt 2 separates them from executed work and passed the complete set above. **EO2 fallback was not used on any build; the reader is not OPEN.** All six retained known-answer input files remain size/mtime/SHA-equal. The accepted 094-14 Editor action evidence was completed offline, before the first new build, without querying UBT. [EO1 validation](<D:/IA_Europa/Receipts/09415/tests/eo1-attempt2.json>); [EO6 validation](<D:/IA_Europa/Receipts/09415/tests/eo6-attempt2/results.json>).

Reader source citations: 4.25 `Engine/Source/Programs/UnrealBuildTool/Executors/ParallelExecutor.cs:207–213,262–276`; 5.1 `ParallelExecutor.cs:327–340,348–380,399–400`, including cancelled dependents at 348–352. The retained exit-6 console SHA is `1e181313660dc6e7fc51e9a1a24471938d8b96b36b8a83fafa4735f6c49ca447`; exit-0 is `1d0fa5dada1c61bb6ae50699f888cd30dc044ae3d6c044a93234897b0a05b5de`; both native graphs are `4487a0aaf38f6eaeeb42f4792ef389bb6eb4868acafa315b30e65c7e9e0ee46e`.

**One probe source and the rebuilds**

The first Baseline51 Editor compile hit C1083 at `AnomalyControlServerSubsystem.h:12`: the release header includes private `IWebSocketServer.h`. One probe-only fix avoids that header on 5.1. It finds the already-loaded exact control subsystem class and uses public native world/tickable-subsystem APIs; the 4.25 typed path remains behind the engine-version branch. The exact class, instance count, identity and tick/lifecycle observations remain represented. The same five probe files are byte-identical in all three hosts and match the committed templates, including Baseline51 at d7eee97. No production Source, Content, descriptor, C# rules or .uproject changed. This is 5.1 effect class C (host diagnostics); production files are B for this job. The error did not survive a fix attempt. Both 4.25 legs were rebuilt again after the shared probe edit. [Probe change and engine citations](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09415-probe-compatibility.md>).

| Final leg / receipt | Plugin SHA | Project/plugin/engine-source-in-host/engine/engine-plugin | Exit | Wall / native seconds | C++ errors / warnings |
|---|---|---|---|---|---|
| [425-editor-rebound](<D:/IA_Europa/Receipts/09415/builds/425-editor-rebound/result.json>) | 550e9b7 | 5/0/0/0/0 | 0 | 63.48 / 22.37 | 0 / 0 |
| [425-game-rebound](<D:/IA_Europa/Receipts/09415/builds/425-game-rebound/result.json>) | 550e9b7 | 4/0/0/0/0 | 0 | 73.31 / 32.99 | 0 / 0 |
| [51base-editor-fixed](<D:/IA_Europa/Receipts/09415/builds/51base-editor-fixed/result.json>) | d7eee97 | 4/1/0/0/0 | 0 | 110.70 / 68.99 | 0 / 0 |
| [51base-game](<D:/IA_Europa/Receipts/09415/builds/51base-game/result.json>) | d7eee97 | 5/0/0/0/0 | 0 | 135.86 / 94.90 | 0 / 0 |
| [51port-editor](<D:/IA_Europa/Receipts/09415/builds/51port-editor/result.json>) | 550e9b7 | 5/1/0/0/0 | 0 | 110.86 / 68.96 | 0 / 0 |
| [51port-game](<D:/IA_Europa/Receipts/09415/builds/51port-game/result.json>) | 550e9b7 | 5/0/0/0/0 | 0 | 140.04 / 99.17 | 0 / 0 |

All six use probe/tooling source `550e9b7`; the two Baseline51 plugin SHAs deliberately remain d7eee97. The single plugin action in each 5.1 Editor row is WriteMetadata, not plugin C++ compilation. Feature commits after 550e9b7 change tooling/docs only, so the final binary source binding remains valid.

| Other native result | SHA | Executed counts | Exit | Wall / native seconds | Disposition |
|---|---|---|---|---|---|
| 094-14 Editor, recovered offline | 35eec14 | 5/0/0/0/0 | 0 | 64.37 / retained log | Accepted evidence; final artifacts superseded. |
| 425-game | 131ed32 | 5/0/0/0/0 | 0 | 79.75 / 38.76 | Superseded by rebound after shared probe edit. |
| 51base-editor, first run | d7eee97 | 3/0/0/0/0; 3 cancelled | 6 | 79.83 / see native log | Two C1083 diagnostics; fixed once. |

Each build used the engine UBT executable, `<target> Win64 Development -Project=<owned .uproject> -Log=<build receipt folder>/UBT.log -NoEngineChanges -NoBuildUHT -NoHotReload -NoHotReloadFromIDE -NoXGE -MaxParallelActions=4 -Verbose`. For 4.25 the executable is `E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe`; for 5.1 it is `D:/UESource/UnrealEngine/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe`. Targets are the host Editor target and its Game target. Every linked result contains the complete literal argv, routed child environment, action graph, executed-action mapping, archived compiler responses, output hashes and ledger operation. [All build fields](<D:/IA_Europa/Receipts/09415/summary/builds.json>).

| Engine | Actual toolchain selection | SDK | Effective standard / PCH |
|---|---|---|---|
| 4.25 | 14.16.27051, directory 14.16.27023; cl product 14.16.27051.0 (19.16.27051) | 10.0.17763.0 | /std:c++17; no /Yu, /Yc or /Fp |
| 5.1 | 14.38.33130, cl product 14.38.33144.0 (19.38.33144) | 10.0.22621.0 | /std:c++17; /Yu and /Fp reuse SharedPCH.Engine.ShadowErrors |

The 5.1 C# standard policy remains Default. All final C++ warning/error lists are empty. The only final warnings are the two unchanged, accepted 4.25 UBT/user-XML warnings: `<USERPROFILE>/AppData/Roaming/Unreal Engine/UnrealBuildTool/BuildConfiguration.xml:16` (`bAllowUBAExecutor`) and `:22` (`MemoryPerActionBytes`). Their cause is newer options in the existing user XML that 4.25 does not understand. No XML or other setting was edited. C5043 and C2855 remain zero.

**Memory admission and EO3 refresh receipts**

| Invocation | Physical GiB at start | Commit GiB at start | Admission seconds | Memory stops |
|---|---|---|---|---|
| 425-game | 6.59 | 26.39 | 40.01 | 0 |
| 51base-editor-recovered-evidence | 5.92 | 25.95 | 40.01 | 0 |
| 51base-editor-fixed | 7.06 | 27.78 | 40.01 | 0 |
| 51base-game | 6.41 | 25.82 | 40.01 | 0 |
| 51port-editor | 8.68 | 25.81 | 40.01 | 0 |
| 51port-game | 10.45 | 30.63 | 40.01 | 0 |
| 425-editor-rebound | 10.27 | 29.89 | 40.01 | 0 |
| 425-game-rebound | 10.30 | 29.71 | 40.01 | 0 |
| clean-engine-query | 10.22 | 29.69 | 40.01 | 0 |

Each admission required three passing polls about 20 seconds apart, physical ≥4 GiB and commit ≥16 GiB. Only the mandatory approximately 40-second settling period occurred; no extra low-memory wait or in-run stop occurred. The in-run commit floor stayed 6 GiB, and parallelism stayed four. Full poll histories are linked from the build summary. No foreign process was stopped.

| EO3 receipt | Target Makefile.bin before run |
|---|---|
| [425-game](<D:/IA_Europa/Receipts/09415/builds/425-game/makefile-refresh.json>) | Absent; no removal |
| [51base-editor-recovered-evidence](<D:/IA_Europa/Receipts/09415/builds/51base-editor/makefile-refresh.json>) | Absent; no removal |
| [51base-editor-fixed](<D:/IA_Europa/Receipts/09415/builds/51base-editor-fixed/makefile-refresh.json>) | 29,475,668 B; 33382713d20a8fe0fecea278c726f9fd84027b87d9c0c82f4f603e815ba983f9 |
| [51base-game](<D:/IA_Europa/Receipts/09415/builds/51base-game/makefile-refresh.json>) | Absent; no removal |
| [51port-editor](<D:/IA_Europa/Receipts/09415/builds/51port-editor/makefile-refresh.json>) | Absent; no removal |
| [51port-game](<D:/IA_Europa/Receipts/09415/builds/51port-game/makefile-refresh.json>) | Absent; no removal |
| [425-editor-rebound](<D:/IA_Europa/Receipts/09415/builds/425-editor-rebound/makefile-refresh.json>) | 713,371 B; b6ced0086b704e43f3b560c3e82313da19157da151cb7fbe82ebdd125f28cd4b |
| [425-game-rebound](<D:/IA_Europa/Receipts/09415/builds/425-game-rebound/makefile-refresh.json>) | 550,796 B; e073fd1e42cab00627818b4bd061bbaf839f9201a32b4fd0f907a18ceaa530c0 |
| [clean-engine-query](<D:/IA_Europa/Receipts/09415/builds/clean-engine-query/makefile-refresh.json>) | 29,475,716 B; b625750817b368b22084ea9cff73321ad0d218e651af94b9d1c3260250805147 |

All nine receipts were taken before the native invocation. Four existing makefiles were removed; five were already absent. Every receipt records the exact owned target path, size, mtime and SHA before removal; result receipts confirm each removal. No engine or protected makefile was removed. The recovered 094-14 log was read offline and needed no refresh.

**Environment, module comparison and clean-engine query**

The real base environment contained IA_CANARY_TOKEN and CLAUDECODE, making the exclusion test non-vacuous. The inherited EL1/EM2 denylist removes the secret-name classes and tool/session prefixes. Wrapper receipts are masked. After the required per-operation routing assignments, every non-blocked variable in that prepared base is retained unchanged at the wrapper boundary; EN1 admits only the selected UBT toolchain changes inside native children. All six final makefiles and compiler environments pass with zero blocked names and no missing base variables. Their PATH retains the base as a byte-identical suffix. The only added variables are VC_COMPILER_DIR and VC_COMPILER_PATH inside the selected toolchain. The 4.25 prefix is selected SDK x64, MSVC HostX64/x86, MSVC HostX64/x64; the 5.1 prefix is selected SDK x64 and MSVC HostX64/x64. The accepted exact-prefix, missing-base, foreign-prefix and canary mutants remain enforced. [Final six-makefile proof](<D:/IA_Europa/Receipts/09415/environment-final/summary.json>); [Entry positive control](<D:/IA_Europa/Receipts/09415/entry-environment.json>).

The accepted 094-13 JsonExport module comparison remains applicable: Baseline51 eight pairs, ReGate51 eight pairs, 4.25 five pairs, and all three Shipping mutants. No C#, descriptor or .uproject changed here, so no new export was required. [Accepted matrix](<D:/IA_Europa/Receipts/09413/descriptor-sets/a5-denylist/matrices/step3-summary.json>).

EI4 ran after the Baseline51 rebuild with its own EO3 refresh and `-WriteOutdatedActions=<receipt>/actions.json`, in addition to the build argv above. **Exit 0; zero project, plugin, engine-source-in-host, engine and engine-plugin actions.** Wall 49.46 seconds, native 8.45 seconds. No bound output was deleted; 15 host cache/metadata updates were recorded. [Clean-engine query](<D:/IA_Europa/Receipts/09415/builds/clean-engine-query/result.json>).

**Rebound manifests and hashes**

| Host | Files / changed | Manifest SHA-256 |
|---|---|---|
| [Europa 4.25](<D:/IA_Europa/Receipts/09415/qualification/final-425-binaries.json>) | 27 / 4 | 4beccca6a58a32b643a691d8c1b460bef0cb997db59d8c8bd17a954b9f42931f |
| [Baseline51](<D:/IA_Europa/Receipts/09415/qualification/final-baseline51-binaries.json>) | 39 / 5 | 617a908243fccc3bd1f8fefc82768d79e7052fa2136be4223cfb7450ac079a8c |
| [ReGate51](<D:/IA_Europa/Receipts/09415/qualification/final-regate51-binaries.json>) | 44 / 5 | f6e52a85b078fc44f713a32f649dd70dcf4daa74868e323568abbec09d8fa7b2 |

All 110 bound paths were rehashed. The manifests distinguish each plugin source, shared probe source, build source and retained A4 artifact. Qualified Shipping files remain byte-identical and no Shipping build ran. Full PDB, import-library and target-receipt hashes are in the manifests and EG2 receipts. The final 4.25 probe DLL/PDB/import library are present. No archive was created. The full EXE/DLL hash list follows; names map to their absolute paths in those manifests.

| Host(s) | EXE/DLL | SHA-256 |
|---|---|---|
| Europa 4.25 | StylizedParisStreet-Win64-Shipping.exe | 6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861 |
| Europa 4.25 | StylizedParisStreet.exe | 5b14a10a3383e5234c91a7f400426c0ccff32be56fe98bfa1b2af4fae5e74a24 |
| Europa 4.25 | UE4Editor-EuropaPhaseAProbe.dll | 7a9f5f93de451c2b5eabb7c8abfac49c42569a1db949e70a2bc2c4eecbb4d58e |
| Europa 4.25 | UE4Editor-StylizedParisStreet.dll | 8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c |
| Europa 4.25 | UE4Editor-AnomalyBench.dll | 2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940 |
| Europa 4.25 | UE4Editor-AnomalyCapture.dll | 9627b803fdb4e94c4e80678176c7478435c092b1c7f993b3024cc936bb50c36c |
| Europa 4.25 | UE4Editor-AnomalyControlServer.dll | 0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b |
| Europa 4.25 | UE4Editor-AnomalyInjector.dll | 9c9bb82daf01ed85900b6794d75570ea38d447e911285c989153e5ec01700f0a |
| Europa 4.25 | UE4Editor-AnomalyShaders.dll | c3db764fc6566f01ca1f425d3bde0469ca8d1ae88ceb01bfb8f5a82f5e8cfd96 |
| Baseline51, ReGate51 | boost_atomic-vc142-mt-x64-1_70.dll | ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765 |
| Baseline51, ReGate51 | boost_chrono-vc142-mt-x64-1_70.dll | 17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26 |
| Baseline51, ReGate51 | boost_iostreams-vc142-mt-x64-1_70.dll | 7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441 |
| Baseline51, ReGate51 | boost_program_options-vc142-mt-x64-1_70.dll | dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5 |
| Baseline51, ReGate51 | boost_python39-vc142-mt-x64-1_70.dll | 9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df |
| Baseline51, ReGate51 | boost_regex-vc142-mt-x64-1_70.dll | 387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64 |
| Baseline51, ReGate51 | boost_system-vc142-mt-x64-1_70.dll | 805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4 |
| Baseline51, ReGate51 | boost_thread-vc142-mt-x64-1_70.dll | bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48 |
| Baseline51, ReGate51 | D3D12Core.dll | 40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2 |
| Baseline51, ReGate51 | d3d12SDKLayers.dll | 1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052 |
| Baseline51, ReGate51 | OpenImageDenoise.dll | 11213c268e6c67262d48e7351e1f6c196acbb91a9a1cf059d4d758c34368f08a |
| Baseline51 | StackOBot.exe | 64e7b9997a89e8017e7791351e2f30e388be86b6899675c0f93dc65e310622e8 |
| Baseline51, ReGate51 | tbb.dll | 6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f |
| Baseline51, ReGate51 | tbb12.dll | de25bc26f71c5fa53b585116db11d643dc63373fab094e5d786a2ef7eb50786d |
| Baseline51 | UnrealEditor-EuropaPhaseAProbe.dll | 4c245e0448ecbae5ff32a48b1e14ee5cbc48597e35065feed6b66ef8f6e58b25 |
| Baseline51 | UnrealEditor-StackOBot.dll | 4092eb194804229dd2e4e85dec4c50eb96cefd03491b5f7038c084365e43ec19 |
| Baseline51 | UnrealEditor-AnomalyBench.dll | 6a8a770188e44660e0d0ba790d14515b600df498eb657bd24af07245dee211d8 |
| Baseline51 | UnrealEditor-AnomalyCapture.dll | 9ab3578e0cdab25d9cacf93b226d8d54af2acfda6af0631c5da76f3d5eed9d94 |
| Baseline51 | UnrealEditor-AnomalyControlServer.dll | 9a41d9a8cb649f0dbd962082ea0b535f6162b2f611146b20d202f2fd457a4b57 |
| Baseline51 | UnrealEditor-AnomalyInjector.dll | 91d14a4af7711c3563dccb30589d013648d9a23b5a7f33563e67f0d06443974e |
| Baseline51 | UnrealEditor-AnomalyShaders.dll | 284c64cd7936ea260e780a1a010916a72312a77acb2282a0d413b4196b584b0d |
| Baseline51 | UnrealEditor-CaptureBench.dll | e2ef940d6df3ca2d8075015498da127f32540df20b844d1c23e9ffe1de3ce5d8 |
| ReGate51 | StackOBot-Win64-Shipping.exe | 010ab98e78e1a4a04c4dc1fb3edf3841336b3bc6f4d8a6db5be24841b6845fb4 |
| ReGate51 | StackOBot.exe | b3d15a4ca5c59f855bae30e1a57af5f1b36b32a38eddc2351ca513ae86d72b33 |
| ReGate51 | UnrealEditor-EuropaPhaseAProbe.dll | 39f134ad8b830f5e6ff953a825c6c3567c1454e3d62787c787ba8e1bcd6b4763 |
| ReGate51 | UnrealEditor-StackOBot.dll | 741b766316baa00dac0a63631dd407b7262ce47fb5f3c677f8d135054117cc0f |
| ReGate51 | UnrealEditor-AnomalyBench.dll | 128e48761dfc075662200648667725e426f8e978d8400a8aedbab9c66612c33c |
| ReGate51 | UnrealEditor-AnomalyCapture.dll | 0e14f06a515844ad27174789fe455284c798a64e8fffe3a8bd12f251e4f162e6 |
| ReGate51 | UnrealEditor-AnomalyControlServer.dll | 3e740f3b9bcf76cc060a25e3eb6602e5a34b375c9e336b86bd065dac129a8a80 |
| ReGate51 | UnrealEditor-AnomalyInjector.dll | 88f288eed3763402f91fc5e38fe8612451df8cc3f54db95a6f391f0aefa7e4b5 |
| ReGate51 | UnrealEditor-AnomalyShaders.dll | f8736404b31745068a78638807114d25a4f42a3df093f4f2e8c8e28aa7c93c9c |
| ReGate51 | UnrealEditor-CaptureBench.dll | 89032f62c2ff43f9f55cbce60a3abd8e68378287bf8ec7fad24f3cec0c11d7f6 |

Superseded output hashes are retained for provenance; they are not the A6 bindings:

| Earlier leg | EXE/DLL | SHA-256 |
|---|---|---|
| 425-editor-final | UE4Editor-EuropaPhaseAProbe.dll | 720b4301fd261a2df41aa3d72d26759b699064e78941cd093f91c394554eb477 |
| 425-game | StylizedParisStreet.exe | a35a024716d04f8c205eda23a74428308cf2175b5518ef268014052fd6b46a63 |

**EA11, EK1 and canary closeout**

EA11 passed on the plugin’s seven reflected headers; its forbidden-preprocessor mutant failed as required. The probe has no reflected headers: **not applicable**, rather than an empty-root pass. [EA11 result](<D:/IA_Europa/Receipts/09415/ea11-final.json>).

The independent EK1 scan read 37,378 files / 16,446,640,035 bytes with zero errors, skipping 10,839 binary-extension files by its accepted design. All six native makefiles decoded and held no secret-named environment values. The 189 name-hit files were reviewed: existing redacted/masked placeholders, synthetic controls, tracked constants/include guards/regexes, and 66 occurrences equal to the engine SpeedTree literal. The apparent extra SpeedTree and Git-date hits in the first classifier were source regex text; the refined check found zero date-shaped values. The only two new name-hit paths were launch receipts with empty W4 fields. No secret exposure or new redaction was required; the three earlier receipt redactions are preserved. [EK1 raw inventory](<D:/IA_Europa/Receipts/09415/ek1-final-raw.json>); [reviewed classifications](<D:/IA_Europa/Receipts/09415/ek1-final-review.json>).

The EK1 scanner child is filtered, so its live-secret set is empty; that is not treated as a live-canary pass. The separate hash-bound scan supplies that proof: **zero matches**, with file and byte totals in the linked receipt, selecting every file created or modified since the launch second, 2026-10-25 17:35:54 UTC, across all extensions under the port and run folder. The entry positive control confirms the canary was actually present. No literal value was printed or stored. [Canary result](<D:/IA_Europa/Receipts/09415/canary-final.json>).

**Owner kit and A6 handoff**

Installed [Europa425-Editor-OwnerLeg.bat](<D:/IA_Europa/Launch/Europa425-Editor-OwnerLeg.bat>) and [owner card draft](<D:/IntrusiveAnomalies/_reviews/094-15-owner-card-draft.md>). The real batch dry run returned PREPARED_ONLY with zero launches after P9, binary/source bindings, routing and ledger checks. Self-tests pass for dry-run admission and reject an unarmed runtime, wrong host and wrong manifest; all five routing cases pass. [Dry-run receipt](<D:/IA_Europa/Receipts/09415/owner-kit/owner-leg-dry-run/prepared.json>); [self-tests](<D:/IA_Europa/Receipts/09415/owner-kit-self-tests.json>).

The kit verifies 27 bound artifacts and 30 host Source/Config/project/mesh files, uses the project-scoped DDC and routes TEMP/TMP, user directory, shader working files, logs and crash logs to D:. The real dry-run P9 preflight passed. Runtime authorization remains false. A6 must supply a fresh authorized job configuration and keep the retained P9 helper files available. The dry run never entered the idle or launch path.

The plain card says to close Blender and other heavy jobs, close other editors/games, provide at least 6 GiB physical headroom and five minutes of user idle, and answer **No** to every rebuild-modules prompt. Once chat authorizes A6, the owner waits for initial shaders, does not press Play, opens `/Game/ParisStreet/Meshes/SM_WindowHouse3.SM_WindowHouse3` for 60 seconds, and exits without saving. Unexpected dialogs require No/Cancel and a screenshot, not a setting change. The hands-on EditorPreview observation is separate from later idle PIE measurements.

Previously accepted A5 preparation remains: 69 synthetic reader controls, 48 idle/12 attribution/6 runtime-ledger checks, and a 920.686-second observe-only collection (29 polls: 24 CLEAR, five WAIT; GPU 0–40%, free physical 7.42–9.38 GiB). Fixture recipes and paired manifests are prepared; no editor authoring occurred. The screenshot driver and probe compile on both engines. Package-header analysis predicts the five newer plugin packages will be rejected/skipped by 4.25; A6 must observe that boundary before authoring and stop on contrary evidence. The 2026-10-26 prediction addendum records the final bindings and owner preparation without changing gate thresholds, before any authoring leg. [Frozen predictions and addenda](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/predictions/094-europa-phase-a.md>).

**Ledger and write attribution**

All 21 completed main operations and the nested owner preflight passed their ledgers. Cumulative charges: C: **1,484,512 B / 250,000,000 B**, E: **0 B / 50,000,000 B**, 5.1 EG1 bookkeeping **6,161,322 B / 100,000,000 B**. Main plus nested ledger evidence totals **454,687,328 B**, below 2 GiB. One initial full inventory plus per-operation deltas and snapshot hashes retain the EB1 standard. Free-space movements are floor monitors, not attribution. D: figures below are inventoried logical-byte deltas, not physical allocation estimates.

| Operation (all ledger PASS) | C GiB before→after | D GiB before→after | E GiB before→after | Port D delta B | C/E/EG1 charge B |
|---|---|---|---|---|---|
| 01-tooling | 18.89→18.90 | 82.05→81.84 | 137.11→137.11 | +918 | 0/0/0 |
| 02-tooling | 18.90→18.90 | 81.84→81.84 | 137.11→137.11 | +246,055 | 0/0/0 |
| 03-425-game | 18.87→18.89 | 81.84→81.79 | 137.11→137.01 | +51,864,640 | 0/0/0 |
| 04-51base-editor | 18.89→18.89 | 81.79→81.75 | 136.95→136.84 | +39,246,442 | 0/0/1,167,215 |
| 05-probe-reader-fix | 18.63→18.63 | 81.75→81.75 | 136.18→136.10 | +158,956 | 0/0/0 |
| 06-51base-editor-fixed | 18.63→18.61 | 81.75→81.69 | 136.02→135.88 | +73,768,209 | 20,480/0/1,167,215 |
| 07-51base-game | 18.61→18.61 | 81.68→81.67 | 136.14→135.98 | +19,034,201 | 0/0/0 |
| 08-51port-editor | 18.61→18.60 | 81.67→81.57 | 135.94→135.86 | +99,837,153 | 0/0/1,913,446 |
| 09-51port-game | 18.72→18.72 | 81.57→81.55 | 135.86→135.86 | +19,645,660 | 0/0/0 |
| 10-425-editor-rebound | 18.71→18.71 | 81.55→81.55 | 135.78→135.59 | +340,568 | 2,072/0/0 |
| 11-425-game-rebound | 18.71→18.72 | 81.55→81.55 | 135.48→135.27 | +461,858 | 1,461,960/0/0 |
| 12-clean-engine-query | 18.72→18.72 | 81.55→81.54 | 135.15→134.95 | +13,859,724 | 0/0/1,913,446 |
| 13-bind | 18.72→18.72 | 81.54→81.54 | 133.17→133.16 | +341,564 | 0/0/0 |
| 14-owner-kit | 18.71→18.70 | 81.54→81.21 | 133.15→133.11 | +350,718,266 | 0/0/0 |
| 15-docs | 18.70→18.70 | 81.21→81.21 | 133.09→133.09 | +30,380 | 0/0/0 |
| 16-ea11 | 18.70→18.70 | 81.21→81.21 | 133.09→133.09 | +1,010 | 0/0/0 |
| 17-ek1 | 18.70→18.69 | 81.21→81.21 | 133.09→133.09 | +99,556 | 0/0/0 |
| 18-reach-repair | 18.69→18.69 | 81.21→81.21 | 133.09→133.09 | +7,494 | 0/0/0 |
| 19-document-correction | 18.69→18.69 | 81.21→81.21 | 133.09→133.08 | +52,539 | 0/0/0 |
| 20-p9-exit | 18.69→18.69 | 81.21→81.09 | 133.06→133.06 | +126,661,058 | 0/0/0 |
| 21-report-binding | 18.69→18.69 | 81.09→81.09 | 133.05→133.05 | +93,870 | 0/0/0 |

| Operation | Foreign change rows | .claude delta B | .codex delta B | ClaudeTemp delta B |
|---|---|---|---|---|
| 01-tooling | 13 | +1,335,625 | -5,820 | +0 |
| 02-tooling | 2 | +457,616 | +124,178 | +0 |
| 03-425-game | 1 | +1,526,967 | +327,920 | +0 |
| 04-51base-editor | 1 | +110,319 | +253,861 | +0 |
| 05-probe-reader-fix | 3 | +1,024,209 | +98,962 | +0 |
| 06-51base-editor-fixed | 4 | +535,619 | +300,741 | +0 |
| 07-51base-game | 2 | +2,055,280 | +189,359 | +0 |
| 08-51port-editor | 4 | +130,587 | +254,582 | +0 |
| 09-51port-game | 1 | +26,233 | +311,226 | +0 |
| 10-425-editor-rebound | 1 | +91,172 | -8,144,408 | +0 |
| 11-425-game-rebound | 1 | -11,766,226 | +20,205 | -84 |
| 12-clean-engine-query | 1 | +95,719 | +4,174,283 | +0 |
| 13-bind | 12 | +77,482 | -37,366 | +0 |
| 14-owner-kit | 41 | +821,083 | +560,024 | +8,387 |
| 15-docs | 8 | +1,179,176 | +300,328 | -16,612 |
| 16-ea11 | 8 | +4,952 | +196,171 | +351 |
| 17-ek1 | 16 | +1,923,507 | +2,341,948 | +2,899 |
| 18-reach-repair | 7 | +4,153 | +109,946 | +0 |
| 19-document-correction | 20 | +1,965,939 | +789,545 | +0 |
| 20-p9-exit | 14 | -5,718,310 | +913,446 | -42 |
| 21-report-binding | 10 | +166,145 | +563,541 | +0 |

Tooling overhead is outside the port budget; .codex resolves to E:/CodexData/home. The full before/after exception-path inventories, foreign path classifications and tooling totals are in [ledger summary](<D:/IA_Europa/Receipts/09415/summary/ledger.json>) and each operation’s compressed classified delta. There were 205 explicitly listed access denials in OS/tooling scopes per snapshot; these were not treated as empty. Protected scopes had zero enumeration errors. C: charges conservatively include permitted Windows/WER writes in compile windows; the receipts do not claim every such byte was generated by the compiler. No E: port exception was charged.

| Operation | EG1 changed paths | Bytes charged | Classes |
|---|---|---|---|
| 04-51base-editor | 1021 | 1167215 | LiveCodingInfo.json: 653, Timestamp: 368 |
| 06-51base-editor-fixed | 1021 | 1167215 | LiveCodingInfo.json: 653, Timestamp: 368 |
| 08-51port-editor | 1022 | 1913446 | ActionHistory.bin: 1, LiveCodingInfo.json: 653, Timestamp: 368 |
| 12-clean-engine-query | 1022 | 1913446 | ActionHistory.bin: 1, LiveCodingInfo.json: 653, Timestamp: 368 |

EG1 path receipts include each changed file’s size, mtime and hash. Only admitted Timestamp, LiveCodingInfo.json and ActionHistory.bin bookkeeping changed. Engine/plugin binaries, generated C++, Source, Content, Config, Shaders and Programs remained protected. The 4.25 install had no port write. The nested owner-preflight ledger charged zero C:/E:/engine bytes and is separately linked here: [owner preflight ledger](<D:/IA_Europa/Receipts/09415/owner-kit/ledger/owner-leg-preflight/summary.json>).

**EN5 readings, source reach and commits**

| Applied stock behaviour | Authority and source citation | Narrow treatment |
|---|---|---|
| Selected toolchain PATH/VC variables | EN1; 4.25 VCEnvironment.cs:144–183; 5.1 VCEnvironment.cs:152–180 | Selected prefixes only; base suffix and all other base values preserved. |
| Owned stale-output cleanup | EN2; 4.25 BuildMode.cs:321; 5.1 BuildMode.cs:568 | First failed 5.1 run removed stale probe PDB/lib; retry restored them. Final query deleted no bound output. |
| Cached makefile environment replay | EO3; 4.25 BuildMode.cs:530–534; 5.1 BuildMode.cs:788–792 | Receipt and refresh only the owned target Makefile.bin before each native run. |

These are the complete EN5-related items applied, already covered by EN1/EN2/EO3. No additional exception or hard-STOP waiver was taken.

The current job changes 18 tracked files, all class C tooling/host-diagnostic/documentation reach. The probe templates change only the 5.1 access path described above. `a5_build.py`, `makefile_actions.py`, `action_reader_tests.py` and `resume_self_tests.py` carry EO1/EO3/EO6; the owner batch, `owner_leg.py` and `owner_p9.py` provide the dry-run kit. The remaining nine files are status, predictions, reach and supporting documentation. Production files remain B for this job. The cumulative release reach joins exactly to 148 Git paths. [Actual per-job reach](<D:/IA_Europa/Receipts/09415/reach-09415.json>).

A documentation formatter mixed Git stderr into filename output, adding one diagnostic row to cumulative reach and two to the per-job list. Its repair helper rejected an incorrect expected count before editing either file and was retired without another invocation. The documentation was corrected directly against Git’s saved native output: 148/148 cumulative paths and 18/18 job paths, no missing or extra entries. This did not rerun a build or qualification reader. Operation 18 records the helper failure; operation 19 records the correction. Likewise operations 01 and 04 retain their failed child results; ledger PASS means write constraints passed, not that those children succeeded. [Native-Git comparison](<D:/IA_Europa/Receipts/09415/reach-manual-review.json>).

| Local feat/europa-port commit | Commit time | Subject |
|---|---|---|
| 131ed3205bb395279f1d59402a5afb4ddc7a3968 | 2026-10-25 23:21:45 +05:30 | fix(europa): qualify action evidence and masked receipts |
| 550e9b79e4d60b553cceb4f7b7dba6b22c2386a7 | 2026-10-25 23:41:04 +05:30 | fix(europa): isolate probe networking includes and cancelled actions |
| 9996b50a4eeb6150033419fba54d614139e84631 | 2026-10-26 00:28:04 +05:30 | feat(europa): prepare scoped owner preview launch kit |
| 9519dd596b29e1012aaef84a6d36ba4d33339efc | 2026-10-26 00:43:15 +05:30 | docs(europa): record A5 build and owner-kit readiness |
| 3d20b5c2a994de539d630d90d1d0e7f4e5aa4dd6 | 2026-10-26 01:19:34 +05:30 | docs(europa): correct reach entries against native Git output |

GIT_AUTHOR_DATE and GIT_COMMITTER_DATE were absent for every commit. No commits were pushed. Paris is clean at 3d20b5c; ReGate51 is clean and detached at compiled source 550e9b7; Baseline51 is clean and detached at d7eee97. The combined closeout script remains abandoned: EA11, EK1, canary, P9 and ledger closeout were run individually. No unresolved A5 STOP or NEEDS-DECISION item remains. A6 needs a fresh authorized scope, a quiet owner window, and the first real content/world/picture observations; the unarmed owner kit cannot provide those during A5.

VERDICT: GREEN | A5 readiness complete; six rebuilds and protected-state proofs pass; no runtime launched
