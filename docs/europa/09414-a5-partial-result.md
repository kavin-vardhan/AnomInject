# 094-14 A5 partial result

A5 is incomplete. The new native action-log reader exhausted D2: first it rejected a filename-only compile progress line; after one fix it rejected the filename-only link line `UE4Editor-EuropaPhaseAProbe.lib`. No third attempt was made. This is our evidence-parser defect, not an engine failure. EN5 does not waive this harness limit.

## Completed

- P9 v2 entry, 23,033 selected binary/config full hashes, original source manifests and protected git state pass. Exit results are in the run report and `Receipts/09414/P9/exit`.
- EN1: 26 strict boundary tests, 16 native toolchain tests and a real fresh-child control pass. The actual 4.25 makefile preserves the base PATH suffix, admits only three selected toolchain entries and VC compiler variables, and has no blocked names.
- Probe compile compatibility: condensed JSON policy defining include, TArray Num emptiness checks, explicit TPair construction. Both edited templates and all three hosts match; 5.1 effect C in the isolated host, intended neutral. No production plugin C++ or content changes.
- At `35eec142f91ef3f440c1c1adf6186232a93bc9ec`, 4.25 Editor Development returns 0. Zero C++ errors/warnings; two existing UBT/user-XML warnings remain. Five logged actions: two host probe compiles, two host links, one metadata write. DLL/PDB/lib are restored. The automated action-evidence gate remains incomplete.
- Prior-artifact comparison: 4.25 25/27 equal (only probe DLL/PDB changed), Baseline51 39/39, ReGate51 44/44; no missing files. These are retained comparisons, not A5 final bindings.
- EA11 passes for seven plugin reflected headers and its mutant. All three host probe source sets are identical; their single header contains no reflected declarations.

## EN5 application

Invalidate only the owned regenerable target Makefile.bin by prior size/mtime/hash receipt, before a fresh native operation, to prevent cached environment restoration replacing the filtered current environment. Stock restore sites: UE4 `Engine/Source/Programs/UnrealBuildTool/Modes/BuildMode.cs:530-534`; UE5 same path `:788-792`. This preserves non-blocked base variables and process routing. No other EN5 exception was applied. The earlier prediction cited the save sites; these are the corresponding restore sites.

## Evidence and resume point

Evidence: `D:/IA_Europa/Receipts/09414`. `closeout-final/native-build-observation.json` distinguishes native compiler exit 0 from wrapper exit 1 and labels the five-action classification as a manual source review. `closeout-final/prior-manifest-comparison.json` retains every hash. No A5 final manifest is issued.

The narrow proposed reader correction is to follow UE4 ParallelExecutor.cs:207-213,265-276 for bare status descriptions for both compile and link actions, and test both status flags plus metadata on retained logs. It is proposed only, pending a fresh harness budget. Retained files permit diagnosis without another compile or a pre-build query.

Then finish the 4.25 Editor evidence, 4.25 Game Development and both Development targets in Baseline51/ReGate51; post-rebuild makefile proofs; EI4 after the Baseline rebuild; all three source-bound manifests; and the A6 owner kit dry run/card. The card must say answer No to a rebuild-modules prompt. No runtime launch is authorized by this result.

The new combined closeout script is also abandoned after two failed attempts: an assertion incorrectly rejected existing XML warnings, then a host invocation of the reflected-header checker was vacuous. Neither modified protected state. Existing independent P9 exit and report steps provide the safety closeout. The action reader and combined closeout script were not attempted a third time.
