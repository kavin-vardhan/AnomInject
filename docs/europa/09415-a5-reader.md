# 094-15 A5 evidence-reader qualification

EO6 expects masked environment receipts. Its negative control rejects a raw synthetic marker; the real child returns only masks and fingerprints.

EO1 attempt 1 accepts bare status names for the 4.25 executor and command-prefixed names for 5.1, with an exact unique native-action join. It rejects unmatched and ambiguous lines, duplicate execution, incomplete denominators, and engine outputs.

Source: E:/EpicGames/UE_4.25/Engine/Source/Programs/UnrealBuildTool/Executors/ParallelExecutor.cs:207-213 and 262-276. The executor prints the first output line, prefilling StatusDescription when enabled. D:/UESource/UnrealEngine/Engine/Source/Programs/UnrealBuildTool/Executors/ParallelExecutor.cs:327-340,348-380,399-400 prefixes CommandDescription (or executable name), handles optional target/timing fields, and prints the remaining output.

Offline validation: the retained failed 4.25 run joins three project compiles, two failing; the retained successful run joins two compiles, the import library, DLL and target receipt. All 794 status lines in retained 5.1 Editor/Game logs parse. Three EI3 can-fails reject engine output, no match and multiple matches. The first real 5.1 native graph join remains to be proved by this job's first 5.1 build.

The accepted 4.25 Editor build remains bound to 35eec14. Its action, response, binary and EN1 evidence is completed offline in Receipts/09415/builds/425-editor-final. EO2 fallback is not used.

EO3 is explicit authority for the target-owned Makefile.bin refresh before every native build/query. This preserves each run's filtered and redirected environment; source: 4.25 BuildMode.cs:530-534 and 5.1 BuildMode.cs:788-792.

No production plugin Source, Content, descriptor, Build.cs or project declaration changes. This reach is tooling only, 5.1 effect class C. No new EN5 interpretation is needed for these changes.
