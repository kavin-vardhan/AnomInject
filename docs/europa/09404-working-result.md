# 094-04 working result: A3 incomplete

2026-10-22. The first 4.25 working compile at `9fdc8f0c6ea2027f9f7810b8f9fc244351c4b0a3` failed, and its post-operation write ledger triggered EA4/ED4's STOP. No second build, compiler fix, 5.1 compile, runtime launch, strict run or push followed. These are working results, not gates A1, A2 or R1.

## Write STOP

`D:/IA_Europa/Receipts/09404/ledger/04-build425-01/summary.json` records twelve path deltas under `%LOCALAPPDATA%/UnrealHeaderTool/Saved/Config`: one old 112-byte CrashReportClient.ini disappeared, a new 112-byte CrashReportClient.ini appeared, and ten two-byte Windows ini files were rewritten. The net size change is zero. UHT configuration files are not EA4's UHT-log exception. The writes remain a STOP regardless of size. Neither engine tree changed; the 4.25 BuildRules charge and 5.1 metadata charge were zero.

The source preflight correctly identified installed-engine project cache routing, but incorrectly treated the process environment route as sufficient for UHT's user directory. `UHT.txt` identifies the C: configuration directory; the requested D: UHT user directory was not created. The wrapper sets UE-CmdLineArgs, but the receipt code checks that mixed-case key in an uppercase-key Windows environment copy and consequently omits it from the recorded environment. The copied environment is passed to the child; effective early UserDir parsing was not established. Native UHT argv has no UserDir. No further engine or UHT experiment was made after the STOP.

Before resuming, Chat must decide between a narrow additional UHT Saved/Config exception with inventories and the existing C: budget, or a separately demonstrated native-argument routing design. No engine edit, global environment change or per-user setting repair is proposed here. These configuration files were not restored or deleted by this job.

## Working compile

Target: StylizedParisStreetEditor Win64 Development. The wrapper used -NoEngineChanges, -NoBuildUHT, -NoHotReload, -NoHotReloadFromIDE, -NoXGE and one worker. Action planning took 44.619 seconds, including successful UHT; the compile invocation took 81.587 seconds. The admitted 35 actions comprise 11 project and 24 plugin actions, all outputs on D:. The compiler executed 24 action entries before returning code 6. Both stock host and empty probe DLLs linked; plugin binaries did not complete.

The 4.25 selection is VS2017 14.16.27051, directory 14.16.27023, SDK 10.0.17763.0. Plugin response files request C++17; their shared PCH responses do not, producing C2855. Twenty C5043 warnings at the five IMPLEMENT_MODULE sites accompany this mismatch. Two existing user XML schema warnings were retained without changing those settings.

Remaining compiler work, all unattempted after the STOP:

- Resolve C2855 inside the 4.25-only PCH/standard policy; a candidate is disabling shared PCHs for those modules. Re-run the entire descriptor set if C# changes.
- Complete ANOMALY_CAPTURE=0 guards around target-mask helpers, probe/observability helpers and material inspection. Review public PreviewPump/Arm/Poll disabled definitions, which currently lie inside an outer enabled-only region. Keep 5.1 function bodies unchanged.
- Match FStaticMeshRenderData's class forward declaration.
- Adapt TFunction reset, per-platform LOD value and predicted skeletal LOD access, frustum construction without a far plane, axis constants, scene-capture-only visibility and the small-number constant through the existing compatibility surface. Preserve the 5.1 paths exactly; no engine or renderer dependency is proposed.
- Repeat 4.25 Editor Development first, then 5.1 ReGate Editor Development only after the write issue is resolved. No compile error has received a fix attempt yet.

The exact diagnostics, action list, response hashes, child PID observations, partial binary hashes and unauthorized path hashes are under `Receipts/09404/builds/425-01`, `ledger/04-build425-01` and `stop`. No archive was created, and no installed or staged binary was swapped.

## Completed source and static work

The shim, five parents, nine raw reflected pointers, capability admission, disabled backend declarations, texture facade and strict-tool adaptation are committed. Their reach and intended 5.1 effects are in phase-a-reach.json. This does not certify a compiling or loadable closure.

EC1 engine-plugin restoration and existing binary checks pass. Both pinned toolsets still exist and fresh JsonExports select them. Eight 5.1 pairs match release, five 4.25 pairs match their expected set, and the plugin plus two probe Shipping mutants change and restore the matrices. The empty probe sources are identical in the two 5.1 hosts and have no Compat dependency or audit behavior. EA11 passes seven reflected headers and its mutant; the strict tool has not run. Asset registry ancestry inspection and runtime lifecycle evidence remain for the authorized first launch.

The report and EA8 exit receipts provide the final protection result. A4a/A4b may not treat this source as ready: A3 and the routing decision remain outstanding.
