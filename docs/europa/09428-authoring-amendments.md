# 09428 authoring amendments

Authority: 094-26-ruling FC1-FC9; EW2/EW3 as amended by EX1-EX5, EY1-EY4 and EZ1-EZ2. The port remains 4.25 only. The old 09425 authoring authorization is retired; its Core session stays spent.

The generated launcher imports `author_425_fixture.py` with `imp.load_source` and retains the module in both its globals and `sys.modules`. It calls `author` while that reference is alive. Python 2.7's temporary runpy module is unsuitable: the installed `Lib/runpy.py:40-45,272-276` removes that module and returns a copied dictionary. CPython 2.7.14 `Python/import.c:588-617,654-697,2967-2983` implements the retained module returned by `imp.load_source`: https://github.com/python/cpython/blob/v2.7.14/Python/import.c . The engine interpreter is `E:/EpicGames/UE_4.25/Engine/Binaries/ThirdParty/Python/Win64/python.exe`, version 2.7.14.

`fixture_python27_harness.py` compiles every 4.25 in-editor source and generated launcher under that interpreter. A recording `unreal` module executes the complete Core and Travel recipe sequences (92 and 89 calls), checks ordering and recipe values, and writes only harness evidence. It creates no Unreal asset. The old runpy-returned-dictionary mutant fails with the observed NoneType.path error; a missing-API mutant fails the smoke check.

One separately authorized Smoke session precedes both recipes. It uses the same editor, Entry map, plugin enable switches, routing and privacy argv. Its generated launcher loads the real author module and `fixture_smoke_425.py`. The smoke reads class/function/property descriptors, checks argument counts where Python 2.7 inspect can determine them, records native signature limitations, and writes `smoke.json`. It calls no Unreal function, constructs no UObject, loads no asset and saves nothing. Its limit is 300 seconds; each recipe retains 1800 seconds. Failed smoke consumes only its own slot and stops before either recipe slot. Core and Travel also require the successful Smoke state even when invoked directly.

Every session requires these exact command-line overrides once, without contradictory duplicates:

```
-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False
-ini:EditorSettings:[/Script/UnrealEd.AnalyticsPrivacySettings]:bSendUsageData=False
```

Source proof, relative to `E:/EpicGames/UE_4.25/Engine/Source`:

| Source | Reason |
|---|---|
| Runtime/Core/Private/Misc/ConfigCacheIni.cpp:1151-1202,3300 | Applies each repeated `-ini:` override to the final in-memory config cache. |
| Editor/UnrealEd/Classes/Analytics/AnalyticsPrivacySettings.h:18-25 | Usage privacy is an EditorSettings config property. |
| Editor/UnrealEd/Private/EditorServer.cpp:1235-1238 | AreEditorAnalyticsEnabled reads that property. |
| Runtime/Engine/Private/EngineAnalytics.cpp:105-122,157-181 | False skips provider initialization, session creation and summary writer/sender creation. |
| Runtime/Core/Private/GenericPlatform/GenericPlatformCrashContext.cpp:363-397 | Reads both privacy properties and serializes them into the temporary crash context used by the monitor. |
| Runtime/Core/Private/Windows/WindowsPlatformCrashContext.cpp:585-638,779-842 | Usage privacy controls crash-client analytics; it is copied to shared context and false adds NoAnalytics on direct crash-client launch. |
| Developer/EditorAnalyticsSession/Private/EditorSessionSummarySender.cpp:273-285 | Identifies the previous session-summary send through the analytics provider. |

The installed tree lacks the CrashReportClient program implementation. The above context hand-off and the disabled editor session writer establish the command-line opt-out path; the smoke must additionally prove both Runtime.log and CrashClient.log contain no POST, upload or session-summary send. Both prior 09426 logs are positive controls. Missing either log refuses qualification. No config file, security setting or launcher state is edited.

FC5 excludes only these managed classes from binding: Saved/Config/CrashReportClient/UE4CC-*, Saved/Autosaves/PackageRestoreData.json, host Intermediate asset-registry caches, and Saved/Logs. Their before/after hashes and ledger entries are retained. Project Config, other Saved/Config files, descriptors, .uproject, Content, EW1 manifests and binaries remain bound. Build snapshots additionally protect host/plugin Binaries and Intermediate, except the explicitly managed asset-registry caches.

FC6 reads every owned process's command line through its retained Windows handle on each 0.2-second runtime poll. UBT is allowed only in QueryTargets mode; missing/unknown mode or unreadable UBT argv stops the owned tree. Compiler, linker, UHT, cook/packaging and additional editor children refuse. Stand-in UBT detection exists only inside the explicit scratch rehearsal contract. Process receipts retain argv, PID, parent and creation time; no foreign process is stopped.

The entry point performs the FA5 ten-minute idle pre-check before each session attempt, followed by the existing measured gate. C: 13/11/10, D: 40/30/25 and E: 15/12/10 GiB remain unchanged. The session-start deadline is 2026-10-30 06:30 IST; an already started session may finish. One fresh Smoke slot and three prelaunch slots per recipe are armed; a successful/failed launch permanently spends that session. The only Content outputs remain SM_EA_TwoLod.uasset, EA_Core.umap and EA_Travel.umap under Content/EuropaPhaseA. The smoke has none.

FA1 forbids creating reparse points in this job. The retained alias regression tests therefore inject the reparse attribute into the filesystem reader for plain scratch directories and prove refusal without creating a junction. Their intent and the historical actual-junction proof are retained; the current run records zero reparse points created.

Receipts live under D:/IA_Europa/Receipts/09428. Runtime qualification, real smoke results and the authoring result belong to the final receipt index and run report. No product/probe source, binaries, fixture recipe, A5 manifest, 5.1 state, global setting, push or tag is changed by this tooling patch.
