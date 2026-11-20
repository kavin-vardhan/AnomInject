<!-- Collected by Claude Code (mailbox brief 094-08) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md, 31679 B, sha256 DA623406D00BA2849AA6A8A2D36020D955495770E1BB4491D49618BCEB997BCF, written 2026-10-24 21:44:55 IST by Codex thread 01a14334 (gpt-6-astra / max), job 16:54:57-21:48:25, exit 0. Verbatim below this line. -->
# 094-08 - Europa A4a report

**A4a is complete.** Gate A1 (4.25 clean builds/toolchain), gate A2 (strict includes), and gate A4's build-level Shipping exclusion pass. The final clean Editor Development, Game Development and Shipping builds, plus the EH5 ReGate51 Editor working compile, all compiled **489c828**. Result HEAD **55764fd** changes documentation/reach only; its production source is byte-identical to the qualified commit.

- Both strict targets compiled **64 ordinary sources + 77 header TUs + 11 UHT TUs**, with plugin unity/PCH off. Both restoration rebuilds passed.
- One production fix: a defining weak-pointer include inside the 4.25-only unsupported-texture implementation; **5.1 effect N**. No permanent C#, descriptor or host project change.
- Final 4.25 builds: **0 errors, 0 C++ warnings, 0 engine/engine-plugin actions**. Shipping: **0 IA/probe module actions or compiled products**.
- EA8 entry/exit against P9 v2 and EA11 pass. C: charge **5,655,034 B**, E: charge **0 B**; 5.1 bookkeeping **1,296,969 B**. No floor breach or STOP.
- No runtime/editor/game/cook/commandlet/UAT launch, push, tag, security/global/per-user change or foreign process stop. A4b and runtime/staged qualification remain pending.

Full evidence: [09408 receipt folder](<D:/IA_Europa/Receipts/09408>). Final binary manifest: [A6-A8 artifacts](<D:/IA_Europa/Receipts/09408/qualification/final-425-binaries.json>).

## Entry, protection and toolchains

P9 v2 was re-derived byte for byte from v1 and the approved removal manifest: 707,775 v1 lines = 472,552 retained + 235,223 removed. Retained manifest SHA-256 is `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`; removed manifest is `dd970a071b603e8a08cd38d69cecdea31e0b3ebc111b7ed29584b91a7764d063`. The owner-delete roots are excluded; nothing there was deleted or moved by this job.

| Proof | Inventory files | Full binary/config hashes | Bytes re-hashed | Protected git/source manifests |
| --- | --- | --- | --- | --- |
| [entry](<D:/IA_Europa/Receipts/09408/P9/entry/summary.json>) | 472553 | 23033 | 104,794,989,456 | equal |
| [exit](<D:/IA_Europa/Receipts/09408/P9/exit/summary.json>) | 472553 | 23033 | 104,794,989,456 | equal |

The entry proof admitted the 1,023 inherited EG1 bookkeeping files already ruled in place. The exit proof admits only that inherited state plus this job's classified EG1 metadata. Retained refs, heads, indices and protected source/config/binaries passed; the Paris original and original host manifests matched. The full 472,552-file baseline was not re-hashed. Recovery connectivity and read-only ledger integrity checks also passed.

Read-only toolchain inventory found VS Community 2022 17.12.35707.178 and VS Build Tools 2017 15.9.34031.82. MSVC directories were 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023; no 14.44 directory. Fresh JsonExports selected 4.25 compiler product **14.16.27051** from directory **14.16.27023**, SDK **10.0.17763.0**, and 5.1 directory **14.38.33130**, compiler product **14.38.33144**, SDK **10.0.22621.0**. Actual build responses independently confirmed those selections. No install, repair or fallback occurred.

## Parser controls and cleaning

Gate A1 synthetic controls passed before scheduling builds: nonzero exit, added error, wrong compiler, absent compile/command, wrong SDK, rules-version mismatch, engine action and engine-plugin action all reject. The rules mismatch rejects before scheduling. Strict `/Yu` and unity mutants reject. Twelve reader cases passed, including the positive control.

The dated A4a prediction addendum was committed in cae8fbf at 17:12:24 IST, before the first UBT invocation. The qualification predicates were not weakened after seeing results.

The EF1/EG1/EG2 suite passed 82 cases initially and 83 after adding the isolated strict-entry import control; routing 31, delta storage 9 and budget 5 cases passed. The native UHT/resource reader adds 27 passing cases, including missing generated actions, missing C++ responses, zero coverage and a C++ action disguised as a resource action. Synthetic can-fails created nothing on C: or E:.

Receipts: [A1/strict readers](<D:/IA_Europa/Receipts/09408/self-tests/readers.json>), [EF1/EG1/EG2](<D:/IA_Europa/Receipts/09408/self-tests/strict-import-fix/results.json>), [native UHT reader](<D:/IA_Europa/Receipts/09408/self-tests/uht-reader/result.json>), [budget suite](<D:/IA_Europa/Receipts/09408/self-tests/budgets/results.json>).

| Receipt | Files | Folders | Bytes removed | Purpose |
| --- | --- | --- | --- | --- |
| [EH2-initial](<D:/IA_Europa/Receipts/09408/cleaning/EH2-initial.json>) | 195 | 30 | 1,706,310,565 | Initial clean trio |
| [strict-source-retry](<D:/IA_Europa/Receipts/09408/cleaning/strict-source-retry.json>) | 769 | 54 | 5,336,625,705 | After defining-include fix |
| [strict-reader-retry](<D:/IA_Europa/Receipts/09408/cleaning/strict-reader-retry.json>) | 1215 | 50 | 4,655,881,633 | Before corrected strict qualification |
| [EH5-final](<D:/IA_Europa/Receipts/09408/cleaning/EH5-final.json>) | 1215 | 50 | 4,655,881,721 | Before final clean trio |

Each deletion was preceded by a file hash manifest and physical-path/reparse checks. Only the owned Paris host Binaries, Intermediate/Build, and plugin Binaries/Intermediate were removed; probe outputs were included. No engine, original host, cache or bank folder was deleted.

## Builds and commands

All real builds and action-plan queries ran through the process wrapper and compact write ledger, one at a time, with EF1 and one worker. The common 4.25 invocation was:

```text
E:\EpicGames\UE_4.25\Engine\Binaries\DotNET\UnrealBuildTool.exe <target> Win64 <configuration>
  -Project=D:\IA_Europa\StylizedParisStreet\StylizedParisStreet.uproject
  -Log=<receipt-folder>\UBT.log -NoEngineChanges -NoBuildUHT
  -NoHotReload -NoHotReloadFromIDE -NoXGE -MaxParallelActions=1 -Verbose
```

Each preceding query appended `-WriteOutdatedActions=<receipt-folder>\actions-1.json`. The disposable include controls additionally used `-SingleFile=<owned module>\SIC_MissingDefine09408.cpp`. Strict mode temporarily changed the five plugin rules; command supervision stayed the same. Literal commands, process receipts, response files, toolchain lines and **every produced DLL/EXE/PDB SHA-256** are retained for every row below in its linked receipt and in the consolidated build index. No abbreviated command is offered as a replacement for those receipts.

The EH5 5.1 compile used `D:\UESource\UnrealEngine\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe StackOBotEditor Win64 Development`, `-Project=D:\IA_Europa\ReGate51\StackOBot\StackOBot.uproject`, its own receipt-folder `-Log`, and the same NoEngineChanges/NoBuildUHT/NoHotReload/NoHotReloadFromIDE/NoXGE/MaxParallelActions=1/Verbose flags.

| Build receipt | SHA | P/I/E/EP actions | Executed | Exit | Compile / wrapper seconds | C++ errors/warnings |
| --- | --- | --- | --- | --- | --- | --- |
| [builds/editor-clean-1](<D:/IA_Europa/Receipts/09408/builds/editor-clean-1/result.json>) | cae8fbf | 11/24/0/0 | 35 | 0 | 100.4 / 947.2 | 0/0 |
| [builds/game-clean-1](<D:/IA_Europa/Receipts/09408/builds/game-clean-1/result.json>) | cae8fbf | 7/9/0/0 | 16 | 0 | 83.3 / 169.4 | 0/0 |
| [builds/shipping-clean-1](<D:/IA_Europa/Receipts/09408/builds/shipping-clean-1/result.json>) | cae8fbf | 5/0/0/0 | 5 | 0 | 36.4 / 222.8 | 0/0 |
| [strict/attempt2/include_positive](<D:/IA_Europa/Receipts/09408/strict/attempt2/include_positive/result.json>) | f463e3e | 0/1/0/0 | 1 | 0 | 13.6 / 726.8 | 0/0 |
| [strict/attempt2/include_negative](<D:/IA_Europa/Receipts/09408/strict/attempt2/include_negative/result.json>) | f463e3e | 0/1/0/0 | 1 | 6 | 2.1 / 83.5 | 1/0 |
| [strict/attempt2/strict_StylizedParisStreetEditor](<D:/IA_Europa/Receipts/09408/strict/attempt2/strict_StylizedParisStreetEditor/result.json>) | f463e3e | 1/162/0/0 | 158 | 6 | 302.9 / 445.9 | 1/0 |
| [strict/attempt3/include_positive](<D:/IA_Europa/Receipts/09408/strict/attempt3/include_positive/result.json>) | d9c91c1 | 0/1/0/0 | 1 | 0 | 16.1 / 1166.0 | 0/0 |
| [strict/attempt3/include_negative](<D:/IA_Europa/Receipts/09408/strict/attempt3/include_negative/result.json>) | d9c91c1 | 0/1/0/0 | 1 | 6 | 2.1 / 123.8 | 1/0 |
| [strict/attempt3/strict_StylizedParisStreetEditor](<D:/IA_Europa/Receipts/09408/strict/attempt3/strict_StylizedParisStreetEditor/result.json>) | d9c91c1 | 11/167/0/0 | 178 | 0 | 338.9 / 422.2 | 0/0 |
| [strict/attempt3/strict_StylizedParisStreet](<D:/IA_Europa/Receipts/09408/strict/attempt3/strict_StylizedParisStreet/result.json>) | d9c91c1 | 7/152/0/0 | 159 | 0 | 409.0 / 575.8 | 0/0 |
| [strict/attempt3/normal_StylizedParisStreetEditor](<D:/IA_Europa/Receipts/09408/strict/attempt3/normal_StylizedParisStreetEditor/result.json>) | d9c91c1 | 1/19/0/0 | 20 | 0 | 69.3 / 3306.3 | 0/0 |
| [strict/attempt3/normal_StylizedParisStreet](<D:/IA_Europa/Receipts/09408/strict/attempt3/normal_StylizedParisStreet/result.json>) | d9c91c1 | 2/9/0/0 | 11 | 0 | 67.1 / 712.4 | 0/0 |
| [strict/attempt4/include_positive](<D:/IA_Europa/Receipts/09408/strict/attempt4/include_positive/result.json>) | 489c828 | 0/1/0/0 | 1 | 0 | 14.5 / 468.3 | 0/0 |
| [strict/attempt4/include_negative](<D:/IA_Europa/Receipts/09408/strict/attempt4/include_negative/result.json>) | 489c828 | 0/1/0/0 | 1 | 6 | 2.3 / 83.7 | 1/0 |
| [strict/attempt4/strict_StylizedParisStreetEditor](<D:/IA_Europa/Receipts/09408/strict/attempt4/strict_StylizedParisStreetEditor/result.json>) | 489c828 | 11/167/0/0 | 178 | 0 | 357.2 / 440.8 | 0/0 |
| [strict/attempt4/strict_StylizedParisStreet](<D:/IA_Europa/Receipts/09408/strict/attempt4/strict_StylizedParisStreet/result.json>) | 489c828 | 7/152/0/0 | 159 | 0 | 295.8 / 382.2 | 0/0 |
| [strict/attempt4/normal_StylizedParisStreetEditor](<D:/IA_Europa/Receipts/09408/strict/attempt4/normal_StylizedParisStreetEditor/result.json>) | 489c828 | 1/19/0/0 | 20 | 0 | 93.5 / 265.1 | 0/0 |
| [strict/attempt4/normal_StylizedParisStreet](<D:/IA_Europa/Receipts/09408/strict/attempt4/normal_StylizedParisStreet/result.json>) | 489c828 | 2/9/0/0 | 11 | 0 | 65.0 / 153.1 | 0/0 |
| [builds/editor-final](<D:/IA_Europa/Receipts/09408/builds/editor-final/result.json>) | 489c828 | 11/24/0/0 | 35 | 0 | 97.2 / 188.2 | 0/0 |
| [builds/game-final](<D:/IA_Europa/Receipts/09408/builds/game-final/result.json>) | 489c828 | 7/9/0/0 | 16 | 0 | 83.4 / 168.3 | 0/0 |
| [builds/shipping-final](<D:/IA_Europa/Receipts/09408/builds/shipping-final/result.json>) | 489c828 | 5/0/0/0 | 5 | 0 | 31.5 / 116.5 | 0/0 |
| [builds/regate-final](<D:/IA_Europa/Receipts/09408/builds/regate-final/result.json>) | 489c828 | 0/4/0/0 | 4 | 0 | 97.6 / 180.4 | 0/0 |

P/I/E/EP means project/plugin/engine/engine-plugin; the action column is the admitted plan and Executed is the observed action count. Wrapper time includes EF1 waits and the query. Nonzero include-negative exits are predicted controls. The failed full strict attempt is retained and superseded, not claimed as a pass. [All build details and product hashes](<D:/IA_Europa/Receipts/09408/qualification/all-builds.json>).

Initial clean artifacts were superseded after the source fix and receipted cleanup. The final trio was built from empty owned build-output roots. The EH5 ReGate51 Editor build was incremental, as authorized, at the same source SHA; its action and write checks cover both engine and engine-plugin outputs. Baseline51 stayed at d7eee97 and was not built.

ReGate51 had zero warnings and errors. Its single actual C++ response uses /std:c++17 and the existing SharedPCH.Engine.ShadowErrors /Yu and /Fp flags, preserving the normal 5.1 PCH policy. Its WriteMetadata action also writes the plugin module manifest, so the output-based classifier includes that mixed host/plugin action in the four plugin actions.

## Shipping, warnings and strict evidence

Shipping's final action plan, actual actions, target BuildProducts and newly written compiled outputs contain no IA or probe module. The host receipt still lists `AnomalyInjector.uplugin` as a UFS runtime/staging dependency. This is a descriptor entry, not a compiled module product; no packaged/content-exclusion claim is made. Development DLLs retained beside Shipping are separately hashed and are not Shipping products.

All successful 4.25 compiles have zero C++ warnings, including C2855 and C5043. No new warning suppression was added. The only 4.25 warnings are the accepted environment warnings at `<USERPROFILE>/AppData/Roaming/Unreal Engine/UnrealBuildTool/BuildConfiguration.xml:16` (`bAllowUBAExecutor`) and `:22` (`MemoryPerActionBytes`): 4.25's XML schema does not recognize those existing settings. They are recorded per query/compile and the file stayed untouched. The exact occurrences and any 5.1 warnings are listed in the warning ledger.

| Engine | Code | Module/location | Occurrences in real build/query receipts |
| --- | --- | --- | --- |
| 425 | UBT | UBT/user XML: <USERPROFILE>/AppData/Roaming/Unreal Engine/UnrealBuildTool/BuildConfiguration.xml:16 | 42 |
| 425 | UBT | UBT/user XML: <USERPROFILE>/AppData/Roaming/Unreal Engine/UnrealBuildTool/BuildConfiguration.xml:22 | 42 |

[Warnings ledger](<D:/IA_Europa/Receipts/09408/qualification/warnings.json>); [Shipping evidence](<D:/IA_Europa/Receipts/09408/builds/shipping-final/shipping-exclusion.json>).

Strict qualification `attempt4` passed on both targets, with the following per-target source denominator:

| Module | Ordinary C++ | Header TUs | UHT TUs |
| --- | --- | --- | --- |
| AnomalyBench | 2 | 1 | 2 |
| AnomalyCapture | 20 | 22 | 2 |
| AnomalyControlServer | 4 | 5 | 2 |
| AnomalyInjector | 34 | 46 | 5 |
| AnomalyShaders | 4 | 3 | 0 |
| Total | 64 | 77 | 11 |

Actual plugin responses show `/std:c++17`, no `/Yu`/`/Yc`/PCH dependency and no unity source. Host shared PCH actions remain separate from the plugin strict scope. Private and reflected headers are included. The UHT denominator comes from each module's native manifest output directory and header-derived expected generated filenames. Zero TUs/actions, a missing generated action or missing response rejects.

The real positive include-control compiled. Removing its defining `Components/StaticMeshComponent.h` include caused the predicted C2027 at `SIC_MissingDefine09408.cpp:3`; restoration removed the disposable source. All five Build.cs files restored to their exact before hashes, generated test folders were removed, and before/after tracked status was identical. Normal Editor and Game rebuilds then passed (20 and 11 actions). The EH5 final clean trio subsequently passed.

Retained attempts explain the changes: attempt 1 stopped before UBT because an isolated import path was missing; its first fix passed. Attempt 2 found real C2504 at WeakObjectPtrTemplates.h:31, instantiated by AnomalyUnsupportedTexture.cpp:30; the first defining-include fix passed subsequent compiles. Attempt 3 compiled both targets successfully but the coverage reader misclassified resource actions and selected Editor UHT files for Game. Its 26 reader findings were retained. The first reader correction passed 27 controls and immutable-evidence revalidation; the complete strict sequence was then repeated as attempt 4. Coverage was not reduced and D2 was not exceeded.

Receipts: [canonical strict summary](<D:/IA_Europa/Receipts/09408/strict/attempt4/summary.json>), [real missing-include failure](<D:/IA_Europa/Receipts/09408/strict/attempt4/include_negative/result.json>), [rule restoration](<D:/IA_Europa/Receipts/09408/strict/attempt4/summary.json>), [retained reader revalidation](<D:/IA_Europa/Receipts/09408/strict/revalidated/result.json>).

## Source reach, final binaries and commits

The only production change is `Source/AnomalyInjector/Private/Compat/AnomalyUnsupportedTexture.cpp`: add `UObject/WeakObjectPtr.h` inside `ANOMALY_ENGINE_425` at the weak-pointer use site. **N:** enabled 5.1 tokens are unchanged; no reflected declaration, dependency, capability or runtime behavior was changed. The final ReGate51 Editor working compile covers the shared-source reach.

Tooling changes are **C**: `a4_build.py`/`a4_evidence.py` add supervised A4 pairs and real compiler/product/exclusion readers; `process_wrapper.py` exposes only the approved pairs; `write_ledger.py` scopes this job; `strict_include_build.py`, `finish_self_tests.py` and `uht_reader_self_tests.py` provide actual strict coverage and controls. Predictions, session/status/result docs and the reach manifest are **C**. The five permanent Build.cs files, descriptor and all host .uprojects are **B for this job**. Their exact restoration/unchanged hashes mean the full JsonExport matrix and three Shipping mutants were not re-run; the previously accepted matrix remains the basis, with fresh toolset-selection exports in this job.

[Joined reach manifest](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/phase-a-reach.json>); [Final verification and EA11](<D:/IA_Europa/Receipts/09408/qualification/final-verification.json>). EA11 passed on the final reflected headers and rejected its mutant.

The following final files are the A6-A8 binding set (full SHA-256, including symbols and receipts). Earlier build hashes remain in their build receipts.

| Paris-relative final artifact | Bytes | SHA-256 |
| --- | --- | --- |
| Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe | 67891200 | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| Binaries/Win64/StylizedParisStreet-Win64-Shipping.exp | 44176 | `44cd0afd18b89127162bdc508c9584181a16c5531ff15361cc5a18a8251fe46b` |
| Binaries/Win64/StylizedParisStreet-Win64-Shipping.lib | 81852 | `f6922002f53bcad9ef4ce9f482316e9e4b4a5f0c70825c981ce4704ff60bef5b` |
| Binaries/Win64/StylizedParisStreet-Win64-Shipping.pdb | 100036608 | `3ef83ad22cfb81c37d72441149e074ea9f8349660ff814e5bc60a59452b898b2` |
| Binaries/Win64/StylizedParisStreet-Win64-Shipping.target | 43520 | `43dd426abc1ac9aa08203335d2747da297be340c8639cb0171b1b7ca9d01a3cc` |
| Binaries/Win64/StylizedParisStreet.exe | 138806272 | `b8870b4b385836fe2b9732be1b9d71fe4959a8c64f3ea4da1278cdee2efeb1aa` |
| Binaries/Win64/StylizedParisStreet.exp | 44147 | `308cc163262c4c52eab4b1848ff9430940458a3b86f090f03d2170ba52cdc672` |
| Binaries/Win64/StylizedParisStreet.lib | 77016 | `bf2cb2c1b76b5700e776fb6368d2db7763cd0d73dd42e4a0c7efa7b83565e323` |
| Binaries/Win64/StylizedParisStreet.pdb | 173101056 | `01b732cbb7c038b06180cc52434755874557f3f08f7b7e95fe8a9bfc4d58da92` |
| Binaries/Win64/StylizedParisStreet.target | 43738 | `1771cd6c0641e0bf586b772c44fce7fed5b820a7b4cb278184441cf8ca6f25a0` |
| Binaries/Win64/StylizedParisStreetEditor.target | 163036 | `4af399c8d25968f7b7b35d0e975d87c25fb800e35a223370d09971eeff5ec7d8` |
| Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll | 89088 | `a2494b974e21aa31c32d3d3d84947fa3e7c2e20f0fccf9a87116a2489a720721` |
| Binaries/Win64/UE4Editor-EuropaPhaseAProbe.pdb | 3461120 | `41db342a455e34d637a7c1c411f52245f59ed983c0b3f644df0a3b53a9afd28b` |
| Binaries/Win64/UE4Editor-StylizedParisStreet.dll | 91648 | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| Binaries/Win64/UE4Editor-StylizedParisStreet.pdb | 34131968 | `784af385f5373c93f075ad6d0b4ec49c8a66c4fd0746e50446fa1624ebadcaa3` |
| Binaries/Win64/UE4Editor.modules | 172 | `c50255a7b1498ffadf67359ed0968313a15c5a91dafc548150dcf6ed1b7d1edc` |
| Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll | 124416 | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.pdb | 6082560 | `f38d86d274977b9832c9ea83fb7b0628ff57ccd4ab430e513ddf0450619e7c45` |
| Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll | 205312 | `9627b803fdb4e94c4e80678176c7478435c092b1c7f993b3024cc936bb50c36c` |
| Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.pdb | 14307328 | `b6bd73d01b96f64d5591d6f923a0b3884596b06dc7fee9d5a104cab8b7539c3b` |
| Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll | 126464 | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` |
| Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.pdb | 13881344 | `361add630160d2a5942f25044f4c207198ab5026bc14dbb1e9029cf3e50abdc6` |
| Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll | 918016 | `9c9bb82daf01ed85900b6794d75570ea38d447e911285c989153e5ec01700f0a` |
| Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.pdb | 29577216 | `b1f556c0a25b7f173a94c9fc2e91ef5d5e9d4ca52a4b867e41228385ae0f8051` |
| Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll | 93184 | `c3db764fc6566f01ca1f425d3bde0469ca8d1ae88ceb01bfb8f5a82f5e8cfd96` |
| Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.pdb | 1904640 | `aea44fa77e930f1c9971f039dd1f58cee11be0b5a08339fb91f254bdfcd84869` |
| Plugins/AnomalyInjector/Binaries/Win64/UE4Editor.modules | 325 | `ff06bd3c17450f18d989cee19119b1c561d325bd109aac1c06e9997538da73d0` |

The same-source ReGate51 incremental compile produced these two binary/symbol products; other engine and host binaries remained intact.

| ReGate51 product | Bytes | SHA-256 |
| --- | --- | --- |
| UnrealEditor-AnomalyInjector.dll | 1772544 | `dfc95c83ad0b4f405b5dafe17a0d7b10efaa40dc503749d6e6d859bd8ced837a` |
| UnrealEditor-AnomalyInjector.pdb | 66269184 | `31ddee9afdc19782d1a19d5a380ecd48ce8c7c0aced39b64fdf52d238b81f567` |

The stock 4.25 editor executable was read and hashed, not launched or changed: `4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387` (462,080 B). No binary archive was created, and no staging/install swap occurred.

| Local commit | Author time | Committer time | Subject |
| --- | --- | --- | --- |
| cae8fbf2d52c0062cd023e01687412852fb0eb30 | 2026-10-24T17:12:24+05:30 | 2026-10-24T17:12:24+05:30 | test(europa): qualify clean 4.25 builds and strict include evidence |
| f463e3e8d056973299e3abac9b5324e54125943a | 2026-10-24T17:55:13+05:30 | 2026-10-24T17:55:13+05:30 | fix(europa): initialize strict tool imports before reader checks |
| d9c91c104f33599b9fc639cb4c3b4a099436f09a | 2026-10-24T18:24:22+05:30 | 2026-10-24T18:24:22+05:30 | fix(europa): include weak pointer definition in disabled texture facade |
| 489c82817108ac791171a20e2203df3e89f9027e | 2026-10-24T20:17:09+05:30 | 2026-10-24T20:17:09+05:30 | fix(europa): read strict C++ coverage from native UHT actions |
| 55764fdccc7c5b433e6d33a74b1d0ff1adda6ed4 | 2026-10-24T21:42:40+05:30 | 2026-10-24T21:42:40+05:30 | docs(europa): record A4a clean build and strict qualification |

All commits are on feat/europa-port, with GIT_AUTHOR_DATE and GIT_COMMITTER_DATE absent. Nothing was pushed. Final result HEAD 55764fdccc7c5b433e6d33a74b1d0ff1adda6ed4 differs from compiled 489c82817108ac791171a20e2203df3e89f9027e only in the four documented result/status/reach files; production diff is empty. ReGate51 remains detached at the compiled SHA.

## EF1 and operation ledger

EF1 recorded 44 actual UBT admissions. Every admission required three samples 20 seconds apart at >=6 GiB free physical and >=12 GiB free commit. In-run sampling retained the ruled stop thresholds. There were **no memory-triggered stops, timeouts or foreign process terminations**. Total recorded admission wait was 8561.0 seconds; longest individual wait 3000.3 seconds. All waits and start readings are retained in [the memory index](<D:/IA_Europa/Receipts/09408/qualification/memory-index.json>) and its hashed JSONL logs.

| Build | Invocation memory log | Admission UTC | Physical GiB | Commit GiB | Wait seconds |
| --- | --- | --- | --- | --- | --- |
| Editor final | child-006-memory.jsonl | 2026-10-24T15:31:58.305905+00:00 | 7.04 | 30.22 | 40.0 |
| Editor final | child-007-memory.jsonl | 2026-10-24T15:32:48.782087+00:00 | 7.77 | 31.20 | 40.0 |
| Game final | child-006-memory.jsonl | 2026-10-24T15:40:49.940848+00:00 | 8.10 | 31.95 | 40.0 |
| Game final | child-007-memory.jsonl | 2026-10-24T15:41:34.042743+00:00 | 8.03 | 31.93 | 40.0 |
| Shipping final | child-006-memory.jsonl | 2026-10-24T15:46:40.305981+00:00 | 7.84 | 31.89 | 40.0 |
| Shipping final | child-007-memory.jsonl | 2026-10-24T15:47:25.069133+00:00 | 7.81 | 31.86 | 40.0 |
| ReGate51 final | child-006-memory.jsonl | 2026-10-24T15:54:53.330464+00:00 | 7.68 | 31.70 | 40.0 |
| ReGate51 final | child-007-memory.jsonl | 2026-10-24T15:55:35.973378+00:00 | 7.55 | 31.63 | 40.0 |

All operation ledgers passed. Free-space readings below are resolved-volume GiB, before -> after. Port D is logical inventory delta, not attributed physical free-space consumption. C/E charges are exception growth under the ruled budgets; owner/Windows writes are classified foreign, and orchestration-folder growth is tooling overhead outside those budgets. The per-operation receipts contain each exception path, before/after size/mtime and hashes where required, all redirects and canonical snapshot hashes.

| Operation | C GiB | D GiB | E GiB | Port D delta MiB | C/E/51 bytes | Foreign changes | Tool overhead MiB |
| --- | --- | --- | --- | --- | --- | --- | --- |
| [01-entry](<D:/IA_Europa/Receipts/09408/ledger/01-entry/summary.json>) | 18.21 -> 17.97 | 136.74 -> 136.42 | 52.65 -> 52.63 | 120.79 | 0/0/0 | 11 | 9.22 |
| [02-tooling](<D:/IA_Europa/Receipts/09408/ledger/02-tooling/summary.json>) | 17.96 -> 17.96 | 136.42 -> 136.42 | 52.59 -> 52.57 | 0.41 | 0/0/0 | 6 | 0.46 |
| [03-selection-clean](<D:/IA_Europa/Receipts/09408/ledger/03-selection-clean/summary.json>) | 17.94 -> 17.94 | 136.41 -> 138.00 | 52.52 -> 52.49 | -1621.81 | 0/0/0 | 6 | 0.85 |
| [04-editor-clean](<D:/IA_Europa/Receipts/09408/ledger/04-editor-clean/summary.json>) | 17.94 -> 17.92 | 138.00 -> 136.41 | 52.41 -> 52.05 | 1627.72 | 3375522/0/0 | 4 | 22.62 |
| [05-game-clean](<D:/IA_Europa/Receipts/09408/ledger/05-game-clean/summary.json>) | 17.91 -> 17.91 | 136.41 -> 134.77 | 52.04 -> 52.04 | 1676.91 | 112/0/0 | 1 | 1.34 |
| [06-shipping-clean](<D:/IA_Europa/Receipts/09408/ledger/06-shipping-clean/summary.json>) | 17.91 -> 17.91 | 134.77 -> 133.57 | 52.03 -> 51.96 | 1225.54 | 12400/0/0 | 2 | -1.72 |
| [07-strict](<D:/IA_Europa/Receipts/09408/ledger/07-strict/summary.json>) | 17.90 -> 17.91 | 133.57 -> 133.57 | 51.96 -> 51.96 | 0.04 | 0/0/0 | 2 | 0.58 |
| [08-strict-import-fix](<D:/IA_Europa/Receipts/09408/ledger/08-strict-import-fix/summary.json>) | 17.90 -> 17.46 | 133.57 -> 133.57 | 51.96 -> 51.97 | 0.01 | 0/0/0 | 11 | 1.20 |
| [09-strict](<D:/IA_Europa/Receipts/09408/ledger/09-strict/summary.json>) | 17.46 -> 17.20 | 133.57 -> 133.02 | 51.97 -> 51.87 | 564.58 | 112/0/0 | 5 | 38.05 |
| [10-weak-include-fix](<D:/IA_Europa/Receipts/09408/ledger/10-weak-include-fix/summary.json>) | 17.20 -> 16.97 | 133.01 -> 137.98 | 51.86 -> 51.85 | -5088.89 | 0/0/0 | 5 | 3.33 |
| [11-strict-fixed](<D:/IA_Europa/Receipts/09408/ledger/11-strict-fixed/summary.json>) | 16.97 -> 16.91 | 137.98 -> 133.63 | 51.84 -> 50.09 | 4448.52 | 2010977/0/0 | 5 | 78.33 |
| [12-uht-reader-fix](<D:/IA_Europa/Receipts/09408/ledger/12-uht-reader-fix/summary.json>) | 16.90 -> 16.90 | 133.63 -> 133.63 | 49.96 -> 49.91 | 0.02 | 0/0/0 | 3 | 1.89 |
| [13-revalidate-clean](<D:/IA_Europa/Receipts/09408/ledger/13-revalidate-clean/summary.json>) | 16.90 -> 16.90 | 133.63 -> 137.96 | 49.88 -> 49.85 | -4437.43 | 0/0/0 | 5 | 0.09 |
| [14-strict-final](<D:/IA_Europa/Receipts/09408/ledger/14-strict-final/summary.json>) | 16.90 -> 16.84 | 137.96 -> 133.61 | 49.83 -> 48.71 | 4448.48 | 43480/0/0 | 7 | 15.98 |
| [15-final-clean](<D:/IA_Europa/Receipts/09408/ledger/15-final-clean/summary.json>) | 16.84 -> 16.84 | 133.61 -> 137.95 | 48.70 -> 48.67 | -4439.38 | 0/0/0 | 2 | 4.15 |
| [16-editor-final](<D:/IA_Europa/Receipts/09408/ledger/16-editor-final/summary.json>) | 16.84 -> 16.83 | 137.95 -> 136.36 | 48.62 -> 48.07 | 1627.72 | 112/0/0 | 1 | 0.78 |
| [17-game-final](<D:/IA_Europa/Receipts/09408/ledger/17-game-final/summary.json>) | 16.83 -> 16.86 | 136.36 -> 134.72 | 48.07 -> 48.07 | 1676.91 | 212207/0/0 | 1 | 0.42 |
| [18-shipping-final](<D:/IA_Europa/Receipts/09408/ledger/18-shipping-final/summary.json>) | 16.90 -> 16.90 | 134.72 -> 133.52 | 48.07 -> 48.07 | 1225.54 | 112/0/0 | 1 | 0.62 |
| [19-regate-prepare](<D:/IA_Europa/Receipts/09408/ledger/19-regate-prepare/summary.json>) | 16.90 -> 16.90 | 133.52 -> 133.52 | 48.07 -> 48.07 | 0.01 | 0/0/0 | 1 | 0.11 |
| [20-regate-final](<D:/IA_Europa/Receipts/09408/ledger/20-regate-final/summary.json>) | 16.90 -> 18.71 | 133.52 -> 133.52 | 48.07 -> 48.07 | 0.10 | 0/0/1296969 | 1 | 0.39 |
| [21-final-verification](<D:/IA_Europa/Receipts/09408/ledger/21-final-verification/summary.json>) | 18.71 -> 18.71 | 133.52 -> 133.52 | 48.07 -> 48.07 | 0.74 | 0/0/0 | 1 | 0.03 |
| [22-exit](<D:/IA_Europa/Receipts/09408/ledger/22-exit/summary.json>) | 18.71 -> 18.71 | 133.52 -> 133.40 | 48.07 -> 48.06 | 120.79 | 0/0/0 | 10 | 0.65 |
| [23-results](<D:/IA_Europa/Receipts/09408/ledger/23-results/summary.json>) | 18.71 -> 18.71 | 133.40 -> 133.40 | 48.06 -> 48.06 | 0.00 | 0/0/0 | 8 | -0.04 |

Totals: C **5,655,034 B / 250 MB**, E **0 B / 50 MB**, 5.1 bookkeeping **1,296,969 B / 100 MB**. Ledger evidence is **227,597,619 B**, below 2 GiB. Start/warning/floor remained C 13/11/10, D 70/55/50 and E 25/20/15 GiB. No floor was breached.

The 4.25 install was inventoried before/after each operation; no E: write was charged. 5.1 changed paths are listed with sizes, mtimes and after hashes in [EG1 bookkeeping evidence](<D:/IA_Europa/Receipts/09408/qualification/engine51-bookkeeping.json>); no engine or engine-plugin compile/link action or binary/generated-C++ write occurred. UHT C: changes stayed inside EE1's Windows settings, CrashReportClient and log classes. No new exception class was introduced.

Operation 20's EG1 classes were: one engine ActionHistory.bin (746,231 B), 207 engine UHT Timestamp files (367,363 B), and 161 engine-plugin UHT Timestamp files (183,375 B): 369 paths and 1,296,969 B total. No other operation changed the 5.1 engine tree.

Orchestration folders were measured per operation: .codex resolves to E:/CodexData/home, while .claude and C:/ClaudeTemp are on C:. The OS/tooling scan retained 204 denied-path records (52 Windows, 97 WER, 55 ClaudeTemp); these were explicitly recorded, not treated as empty. Protected enumeration had zero errors. This is EB1's targeted evidence, not a claim to trace every Windows write. No ETW, USN or elevation was attempted.

## A4b handoff and decisions

A4b still needs the 5.1 release/candidate Editor, Game and Shipping qualification, warning comparison and real strict run. The current A4a wrapper deliberately admits only Paris's three pairs and ReGate51 Editor Development; its pair allowlist must follow A4b's new ruling. No 5.1 Game, Shipping or Baseline51 build occurred here. Native 5.1 UHT layout has synthetic reader coverage only.

Use the final 4.25 manifest above for A6-A8. The probe remains the A3 skeleton; no behavior or shim audit was added. A5 and subsequent runtime, pixel, packaged/cooked, capture, labels/masks/dashboard and delivery gates remain unproved. No bench setting was changed or runtime lever exercised. Temporary strict rules and the disposable include mutant were restored exactly.

There is no open STOP or NEEDS-DECISION item from A4a. The source include omission and both tooling defects were resolved and their failed evidence retained. This report proposes A4a acceptance to chat; it does not authorize A4b or delivery.

VERDICT: GREEN | A4a clean 4.25 trio and strict pass complete on 489c828; same-source ReGate51 Editor passes; P9 v2 and ledgers pass; A4b pending; no push
