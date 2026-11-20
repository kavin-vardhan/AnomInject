<!-- Collected by Claude Code (mailbox brief 094-21) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md; sha256 E0D784E263FE7AF12EAA024E72BB9DDC90B3B8990025E1D1DF7B48C2FA491638, 20915 bytes; thread 01a14334, resume 2026-10-28 01:49:07 - 03:49:29 IST, 7223 s, exit 0; body below is byte-identical -->
094-21 is complete. EV5 closes the remaining exit-purge access, including Stop() root changes. The same probe source compiled in all six targets on the first attempt, including Baseline51 at d7eee97. The real static check passed and all 24 mutants failed. EI4 returned zero actions; the 27/39/44 manifests are rebound and all 110 files rehashed. Entry/exit P9, content, EA11, full EK1 and the canary check passed. There were no real disk pauses or runtime launches. Runtime confirmation of the exit behavior remains for a later authorized leg.

Source/build commit: 40a8eaf983518fe1992e172434aa8caf347654f1. Final documentation commit: 7039df78950196e42a2cd07773d47de56f7b6397. Both are local on feat/europa-port; nothing was pushed or tagged. Production source remains equal to 489c828 / 1c4cce7 for the candidate and d7eee97 for Baseline51; content, descriptors and C# are unchanged. The owner kit was not run or re-armed.

Entry and exit proofs passed against P9 v2, whose manifest SHA-256 is 8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74.

| Proof | Entry | Exit |
|---|---:|---:|
| Inventory files | 472553 | 472553 |
| Admitted bookkeeping/retained marker entries | 1023 | 1023 |
| Full binary/config hashes | 23033 | 23033 |
| Bytes hashed | 104794989456 | 104794989456 |

[Entry receipt](<D:/IA_Europa/Receipts/09421/P9/entry/summary.json>); [exit receipt](<D:/IA_Europa/Receipts/09421/P9/exit/summary.json>). Protected Git and source manifests match. m51 remains 53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67 with index 526aba7fbf38d9936fcca0551362002e6332ac42b2e1ffcb1c5b322d4c9ea35e and its baseline 15 untracked entries. The 72 refs and five prunable worktree registrations are retained.

| Drive | Entry free GiB | Exit free GiB | Start / warn / floor GiB |
|---|---:|---:|---|
| C: | 20.41 | 19.89 | 13 / 11 / 10 |
| D: | 74.14 | 73.74 | 40 / 30 / 25 |
| E: | 33.46 | 33.45 | 25 / 20 / 15 |

ED3 was verified read-only. The 4.25 builds selected MSVC 14.16.27023 (compiler 19.16.27051) and SDK 10.0.17763.0; 5.1 selected MSVC 14.38.33130 (compiler 19.38.33144) and SDK 10.0.22621.0. Other installed MSVC versions remain unchanged. [Toolchain receipt](<D:/IA_Europa/Receipts/09421/toolchains.json>).

The final content check matched all 1,453 Paris files (2,960,507,507 bytes) and five plugin packages (189,599 bytes), including comparison with the original Paris files and plugin Git blobs. The empty EditorRuns/31124 marker remains untouched under ET2. [Content hashes](<D:/IA_Europa/Receipts/09421/content-exit-check.json>); [artifact and marker rehash](<D:/IA_Europa/Receipts/09421/qualification/bound-exit-check.json>).

EV5 guards every probe entry path that can touch UObjects against engine exit, GExitPurge and garbage collection. Root changes go only through ChangeRoot; Stop() clears raw bookkeeping and emits root_release=skipped at purge. ReleaseGC and ScreenshotDone record skipped results without dereferencing UObjects. World receipt lookup/removal now uses a cached UPTRINT address key, avoiding weak-pointer construction after purge. Safe world/PIE cleanup retains guarded per-level actor inspection; object lookups stay confined to guarded initialization.

[ChangeRoot and purge guard](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:52>); [Stop](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:158>); [ScreenshotDone](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:267>); [ReleaseGC](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:393>). The source ordering is material: [4.25 Obj.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/CoreUObject/Private/UObject/Obj.cpp:4476>) and [5.1 Obj.cpp](<D:/UESource/UnrealEngine/Engine/Source/Runtime/CoreUObject/Private/UObject/Obj.cpp:4877>) set GExitPurge before later module shutdown. [4.25 shutdown](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Launch/Private/LaunchEngineLoop.cpp:4218>); [5.1 shutdown](<D:/UESource/UnrealEngine/Engine/Source/Runtime/Launch/Private/LaunchEngineLoop.cpp:4592>). ENGINE_MAJOR_VERSION remains the compatibility switch; no production code changed.

The widened static check passed on D2 attempt 1. [Check source](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/probe_teardown_check.py>); [all cases and violations](<D:/IA_Europa/Receipts/09421/probe/static-check.json>).

| Case group | Cases | Expected result observed |
|---|---:|---|
| Real probe | 1 | PASS |
| Existing actor iterator/range, lookup/load and reachability mutants | 9 | FAIL |
| LoadClass; StaticLoadClass | 2 | FAIL |
| TObjectIterator; TObjectRange | 2 | FAIL |
| FindFirstObject | 1 | FAIL |
| AddToRoot / RemoveFromRoot after purge | 2 | FAIL |
| Root change before the helper guard | 1 | FAIL |
| Missing purge guard in ChangeRoot, ReleaseGC, ScreenshotDone, Command, Execute or Identity | 6 | FAIL |
| Weak UObject key reintroduced into cached receipt lookup | 1 | FAIL |

All six native builds exited 0 with -NoEngineChanges, -NoBuildUHT and -MaxParallelActions=4. They executed 33 actions, including 18 probe compilation actions, with zero engine actions and zero compile retries. The accepted EO1 reader joined executed log lines to the native graphs; EO2 fallback was not used.

| Leg | Plugin source | Project / plugin / engine actions | Exit | EO3 receipt |
|---|---|---|---:|---|
| [425-editor](<D:/IA_Europa/Receipts/09421/builds/425-editor/result.json>) | 40a8eaf | 6 / 0 / 0 | 0 | [refresh](<D:/IA_Europa/Receipts/09421/builds/425-editor/makefile-refresh.json>) |
| [425-game](<D:/IA_Europa/Receipts/09421/builds/425-game/result.json>) | 40a8eaf | 5 / 0 / 0 | 0 | [refresh](<D:/IA_Europa/Receipts/09421/builds/425-game/makefile-refresh.json>) |
| [51base-editor](<D:/IA_Europa/Receipts/09421/builds/51base-editor/result.json>) | d7eee97 | 5 / 1 / 0 | 0 | [refresh](<D:/IA_Europa/Receipts/09421/builds/51base-editor/makefile-refresh.json>) |
| [51base-game](<D:/IA_Europa/Receipts/09421/builds/51base-game/result.json>) | d7eee97 | 5 / 0 / 0 | 0 | [refresh](<D:/IA_Europa/Receipts/09421/builds/51base-game/makefile-refresh.json>) |
| [51port-editor](<D:/IA_Europa/Receipts/09421/builds/51port-editor/result.json>) | 40a8eaf | 5 / 1 / 0 | 0 | [refresh](<D:/IA_Europa/Receipts/09421/builds/51port-editor/makefile-refresh.json>) |
| [51port-game](<D:/IA_Europa/Receipts/09421/builds/51port-game/result.json>) | 40a8eaf | 5 / 0 / 0 | 0 | [refresh](<D:/IA_Europa/Receipts/09421/builds/51port-game/makefile-refresh.json>) |
| [clean-engine-query](<D:/IA_Europa/Receipts/09421/builds/clean-engine-query/result.json>) | d7eee97 | 0 / 0 / 0 | 0 | [refresh](<D:/IA_Europa/Receipts/09421/builds/clean-engine-query/makefile-refresh.json>) |

The two 5.1 Editor actions classified as plugin actions are target metadata, not production compilation. The only diagnostics were the two existing 4.25 user-XML schema warnings for bAllowUBAExecutor and MemoryPerActionBytes. There were no new port warnings. Each EO3 receipt precedes removal of that target’s own Makefile.bin; the initial 4.25 Editor receipt records that it was already absent. The EI4 query followed the rebuilds, used its own refresh, exited 0 and found zero actions.

EV4 admitted the six incremental builds only after three 20-second polls meeting physical 4 GiB and commit 10 GiB. EI4 conservatively retained the 16 GiB commit gate. Across 55 readings, the minimum admitted physical/commit values were 8.98/21.93 GiB. Running commit stayed at or above 14.34 GiB. Physical memory briefly fell to 79.25 MiB during ReGate51 Game; EI2's running stop is commit-based and was never reached. [Memory and ledger summary](<D:/IA_Europa/Receipts/09421/ledger-summary.json>).

The final six makefiles preserve every non-blocked base variable and contain no blocked name or canary. Only the exact selected toolchain PATH prefix and VC_COMPILER_DIR / VC_COMPILER_PATH additions were admitted. [EN1 proof](<D:/IA_Europa/Receipts/09421/environment-final/summary.json>). C#, descriptors and .uproject files did not change, so the conditional JsonExport/mutant rerun was not triggered. EA11 passed all seven reflected plugin headers and detected its in-memory forbidden-conditional mutant; it is not applicable to the non-reflected probe. [EA11](<D:/IA_Europa/Receipts/09421/ea11.json>).

The new manifests bind probe/tooling source 40a8eaf983518fe1992e172434aa8caf347654f1. Baseline51 retains plugin source d7eee97731812384d4fd00af9032254923363ada; Paris and ReGate51 use 40a8eaf983518fe1992e172434aa8caf347654f1. The final commit changes documentation only, with compiled source/tooling equality verified. The old 09415 manifest bytes and retained Shipping outputs are unchanged.

| Host | Files / changed | New manifest SHA-256 |
|---|---:|---|
| [425](<D:/IA_Europa/Receipts/09421/qualification/final-425-binaries.json>) | 27 / 4 | c501895c9856638dd76e1c2091be8726f6797014bcb0714e306fd9d6ebd98df6 |
| [51base](<D:/IA_Europa/Receipts/09421/qualification/final-baseline51-binaries.json>) | 39 / 5 | b05e6f8c9ce12fa1c5b4dbd142a59383f737bb8a3b7885b7219d1bcdb6c1fc37 |
| [51port](<D:/IA_Europa/Receipts/09421/qualification/final-regate51-binaries.json>) | 44 / 5 | 3865fe0c6ed039d5bad2bc54d1a6a89f11c403e98237b4c6caf74ca661ea12e2 |

Every bound executable/DLL hash is listed below; identical third-party binaries in both 5.1 hosts share a row. 425 means the Paris host; B means Baseline51/StackOBot; R means ReGate51/StackOBot, all under D:/IA_Europa. H is Binaries/Win64; P is Plugins/AnomalyInjector/Binaries/Win64; C is Plugins/CaptureBench/Binaries/Win64. The manifests above include full paths and hashes for all 110 artifacts, including PDBs and receipts.

| Hosts | Location | Executable/DLL | SHA-256 |
|---|---|---|---|
| 425 | H | StylizedParisStreet-Win64-Shipping.exe | 6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861 |
| 425 | H | StylizedParisStreet.exe | a2db7a028ebdbfc9d5d74600a35a2951421f8def703ca90f5e307ed460eeb1ac |
| 425 | H | UE4Editor-EuropaPhaseAProbe.dll | 9458cb952db4c24adc13211118222f1754a7aacaedbc9bd027505a2a382f824f |
| 425 | H | UE4Editor-StylizedParisStreet.dll | 8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c |
| 425 | P | UE4Editor-AnomalyBench.dll | 2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940 |
| 425 | P | UE4Editor-AnomalyCapture.dll | 9627b803fdb4e94c4e80678176c7478435c092b1c7f993b3024cc936bb50c36c |
| 425 | P | UE4Editor-AnomalyControlServer.dll | 0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b |
| 425 | P | UE4Editor-AnomalyInjector.dll | 9c9bb82daf01ed85900b6794d75570ea38d447e911285c989153e5ec01700f0a |
| 425 | P | UE4Editor-AnomalyShaders.dll | c3db764fc6566f01ca1f425d3bde0469ca8d1ae88ceb01bfb8f5a82f5e8cfd96 |
| B,R | H | boost_atomic-vc142-mt-x64-1_70.dll | ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765 |
| B,R | H | boost_chrono-vc142-mt-x64-1_70.dll | 17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26 |
| B,R | H | boost_iostreams-vc142-mt-x64-1_70.dll | 7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441 |
| B,R | H | boost_program_options-vc142-mt-x64-1_70.dll | dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5 |
| B,R | H | boost_python39-vc142-mt-x64-1_70.dll | 9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df |
| B,R | H | boost_regex-vc142-mt-x64-1_70.dll | 387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64 |
| B,R | H | boost_system-vc142-mt-x64-1_70.dll | 805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4 |
| B,R | H | boost_thread-vc142-mt-x64-1_70.dll | bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48 |
| B,R | H | D3D12/D3D12Core.dll | 40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2 |
| B,R | H | D3D12/d3d12SDKLayers.dll | 1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052 |
| B,R | H | OpenImageDenoise.dll | 11213c268e6c67262d48e7351e1f6c196acbb91a9a1cf059d4d758c34368f08a |
| B | H | StackOBot.exe | b6def734b67b3d03636397ec9d43999458ab86e6baa3db9f21382d394fa2d3d0 |
| B,R | H | tbb.dll | 6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f |
| B,R | H | tbb12.dll | de25bc26f71c5fa53b585116db11d643dc63373fab094e5d786a2ef7eb50786d |
| B | H | UnrealEditor-EuropaPhaseAProbe.dll | ac3b1ca44fc45b61cc3a6e4f1fc7271c383d337fa86ab1ba45eddce4e2d78012 |
| B | H | UnrealEditor-StackOBot.dll | 4092eb194804229dd2e4e85dec4c50eb96cefd03491b5f7038c084365e43ec19 |
| B | P | UnrealEditor-AnomalyBench.dll | 6a8a770188e44660e0d0ba790d14515b600df498eb657bd24af07245dee211d8 |
| B | P | UnrealEditor-AnomalyCapture.dll | 9ab3578e0cdab25d9cacf93b226d8d54af2acfda6af0631c5da76f3d5eed9d94 |
| B | P | UnrealEditor-AnomalyControlServer.dll | 9a41d9a8cb649f0dbd962082ea0b535f6162b2f611146b20d202f2fd457a4b57 |
| B | P | UnrealEditor-AnomalyInjector.dll | 91d14a4af7711c3563dccb30589d013648d9a23b5a7f33563e67f0d06443974e |
| B | P | UnrealEditor-AnomalyShaders.dll | 284c64cd7936ea260e780a1a010916a72312a77acb2282a0d413b4196b584b0d |
| B | C | UnrealEditor-CaptureBench.dll | e2ef940d6df3ca2d8075015498da127f32540df20b844d1c23e9ffe1de3ce5d8 |
| R | H | StackOBot-Win64-Shipping.exe | 010ab98e78e1a4a04c4dc1fb3edf3841336b3bc6f4d8a6db5be24841b6845fb4 |
| R | H | StackOBot.exe | 5b0fb171a9c65dced68cfd52b8c62540c0ecb29714f30091717b9d055d5f2298 |
| R | H | UnrealEditor-EuropaPhaseAProbe.dll | f8cab87d9a3e0bb93a8fc92dd20c22d25851c83051f3cd28a0e97da0c6af1b21 |
| R | H | UnrealEditor-StackOBot.dll | 741b766316baa00dac0a63631dd407b7262ce47fb5f3c677f8d135054117cc0f |
| R | P | UnrealEditor-AnomalyBench.dll | 128e48761dfc075662200648667725e426f8e978d8400a8aedbab9c66612c33c |
| R | P | UnrealEditor-AnomalyCapture.dll | 0e14f06a515844ad27174789fe455284c798a64e8fffe3a8bd12f251e4f162e6 |
| R | P | UnrealEditor-AnomalyControlServer.dll | 3e740f3b9bcf76cc060a25e3eb6602e5a34b375c9e336b86bd065dac129a8a80 |
| R | P | UnrealEditor-AnomalyInjector.dll | 88f288eed3763402f91fc5e38fe8612451df8cc3f54db95a6f391f0aefa7e4b5 |
| R | P | UnrealEditor-AnomalyShaders.dll | f8736404b31745068a78638807114d25a4f42a3df093f4f2e8c8e28aa7c93c9c |
| R | C | UnrealEditor-CaptureBench.dll | 89032f62c2ff43f9f55cbce60a3abd8e68378287bf8ec7fad24f3cec0c11d7f6 |

EU2 recorded zero real pauses, zero disk-caused child stops and zero restarts. EV3 is applied as D: 40/30/25 GiB; C:/E: thresholds are unchanged. The accepted EU2 regression suite passed 12/12 cases under the amended thresholds, and EV4 scope/gate tests passed 12/12. EV1 was checked both for real pause windows and, more strictly, for out-of-band edits while a disk supervisor was active: none. [Disk tests](<D:/IA_Europa/Receipts/09421/disk-tests/ev3-result.json>); [EV3/EV4 tests](<D:/IA_Europa/Receipts/09421/policy/ev3-ev4-tests.json>).

The full EK1 scan read 42,685 files (22,381,928,622 bytes), with zero read errors, zero oversized skips and zero secret values. It retained the accepted 15,058 binary-extension exclusions; all six native makefiles were decoded. The two old owner-slot temporary files were read with GENERIC_READ and Read|Write|Delete sharing, with no content/handle changes. The shared-read can-fail passed. All 278 unchanged name-hit contexts match their pre-launch hashes; the three changed contexts are the transcript and W4 launch receipts. [Full EK1 qualification](<D:/IA_Europa/Receipts/09421/secret-scan/qualified.json>).

The launch canary positive control was present and hash-verified before filtering, so the check is non-vacuous. The all-extension scan starts at the launch second, 2026-10-27T20:19:06Z, and checks both creation and modification times. It found zero current-canary values. Build-child and final makefile environments are clean. [Canary scan](<D:/IA_Europa/Receipts/09421/canary-exit.json>); the later report-only refresh is [canary-final.json](<D:/IA_Europa/Receipts/09421/canary-final.json>). No secret value was printed or stored.

EU1 command audit: one direct read-only deviation, zero direct write-capable commands. The deviation was the initial Get-Content inspection of the old wrapper, ruling, entry script and launch timestamp. It inherited the canary but wrote nothing; EU1 permits continuation with this disclosure. An earlier call to the old wrapper lacked its mandatory Program argument and was refused before its child/body ran. Every subsequent external command used filtered09421.ps1. The auditor’s 16 cases passed, including write-command and edit-during-supervisor can-fails. [Command audit](<D:/IA_Europa/Receipts/09421/command-audit.json>); [final audit refresh](<D:/IA_Europa/Receipts/09421/command-audit-final.json>). The event stream has no timestamps, so the EV1 check uses the stronger ordered supervisor intervals; the real pause list is empty. Each receipt states its event-log cutoff.

The first final audit returned REVIEW_REQUIRED for item_160, a filtered git -C <owned worktree> diff -- <documentation paths> while the ledger snapshot was finishing. Inspection confirmed a read-only query; the classifier had omitted this Git form. The original receipt is preserved in [the first refresh](<D:/IA_Europa/Receipts/09421/command-audit-first-refresh.json>). The corrected reader admits only this path-only diff form, and four added cases reject an output option, a commit and shell substitution while accepting the read-only diff. All 16 cases pass. [Correction and evidence](<D:/IA_Europa/Receipts/09421/audit-correction.json>); [corrected audit](<D:/IA_Europa/Receipts/09421/command-audit-corrected.json>). No actual write during a supervisor or pause was found; this report-only classification correction changed no compiled tooling or artifact.

The ledger completed 23 operations through 23-final-state with PASS and no STOP classifications. Cumulative conservative charges are C: 177,477 bytes, E: 0 bytes, and admitted 5.1 bookkeeping: 4,994,107 bytes. The C charge includes Windows/WER changes during build windows, not a claim that our processes wrote them. The 232 existing OS/tooling enumeration denials remain explicitly listed; protected enumeration has zero errors, and full EK1 has zero read errors. [Ledger summary](<D:/IA_Europa/Receipts/09421/ledger-summary.json>). Later report-only operations retain their individual ledgers.

No new EN5 exception was needed. The following already-ruled stock behaviors were applied narrowly and recorded:

| Behavior | Applied authority and source |
|---|---|
| Selected UBT PATH prefix and VC variables | EN1; [4.25 VCEnvironment.cs](<E:/EpicGames/UE_4.25/Engine/Source/Programs/UnrealBuildTool/Platform/Windows/VCEnvironment.cs:141>); [5.1 VCEnvironment.cs](<D:/UESource/UnrealEngine/Engine/Source/Programs/UnrealBuildTool/Platform/Windows/VCEnvironment.cs:149>) |
| Target makefile refresh before native runs, avoiding cached environment restoration | EO3; [4.25 BuildMode.cs](<E:/EpicGames/UE_4.25/Engine/Source/Programs/UnrealBuildTool/Modes/BuildMode.cs:530>); [5.1 BuildMode.cs](<D:/UESource/UnrealEngine/Engine/Source/Programs/UnrealBuildTool/Modes/BuildMode.cs:788>) |
| Rebuild before the clean query; outdated-output handling | EN2; [4.25 BuildMode.cs](<E:/EpicGames/UE_4.25/Engine/Source/Programs/UnrealBuildTool/Modes/BuildMode.cs:321>); [5.1 BuildMode.cs](<D:/UESource/UnrealEngine/Engine/Source/Programs/UnrealBuildTool/Modes/BuildMode.cs:568>) |

These did not relax secret filtering, protected state, engine-write boundaries or any launch prohibition. ET2 retains the marker by explicit ruling. No machine/security/global setting, bench lever or content was changed; no binary delivery archive was created, deleted or moved. Compressed ledger evidence is under [the job ledger](<D:/IA_Europa/Receipts/09421/ledger>), as initial.sqlite.gz and each operation’s before-delta.jsonl.gz, after-delta.jsonl.gz and classified-changes.jsonl.gz.

| Local commit | Author time | Committer time |
|---|---|---|
| 7039df78950196e42a2cd07773d47de56f7b6397 | 2026-10-28T03:21:51+05:30 | 2026-10-28T03:21:51+05:30 |
| 40a8eaf983518fe1992e172434aa8caf347654f1 | 2026-10-28T02:06:58+05:30 | 2026-10-28T02:06:58+05:30 |

W4 author/committer date overrides were absent. [Final Git/source proof](<D:/IA_Europa/Receipts/09421/qualification/final-state.json>). The prediction was committed with source before the builds; the final documentation records the observed results. The later EV7 arming work, launcher/upload prerequisites, and runtime exit confirmation remain outside this job. EV6’s glass-normal baseline remains unchanged.

VERDICT: GREEN | EV5 fixed and compiled in six targets; static mutants, EI4, 110-artifact bindings, P9 and scans pass; no runtime launch.
