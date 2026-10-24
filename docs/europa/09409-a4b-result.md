# 094-09: A4b 5.1 baseline and candidate qualification

Authority: 094-08 ruling EI1-EI6. Release control: `d7eee97731812384d4fd00af9032254923363ada`. Qualified candidate: `1c4cce7023081c64f5c747688ea7ef8f69143f7b`. Production source and descriptor are unchanged from A4a's 489c828. The result commit changes documentation and the reach manifest only.

R1 build evidence passes: baseline and candidate Editor and monolithic Game Development, candidate Shipping, generated feature definitions, the baseline-equal tick pin, and no new or worse candidate warning. The real 5.1 strict pass and restoration rebuilds pass for both targets. This does not qualify runtime parity, pixels, capture alignment or packaged exclusion.

| Build | SHA | Target/configuration | Project/plugin/engine-source-in-host/engine-tree actions | Exit | Warning occurrences |
| --- | --- | --- | --- | --- | --- |
| baseline_editor | d7eee97 | StackOBotEditor Development | 9/20/0/0 | 0 | 2 |
| baseline_game | d7eee97 | StackOBot Development | 52/7/706/0 | 0 | 2 |
| candidate_editor | 1c4cce7 | StackOBotEditor Development | 9/20/0/0 | 0 | 2 |
| candidate_game | 1c4cce7 | StackOBot Development | 52/7/706/0 | 0 | 2 |
| shipping | 1c4cce7 | StackOBot Shipping | 37/1/668/0 | 0 | 2 |

All actual responses use MSVC directory 14.38.33130, compiler product 14.38.33144 and SDK 10.0.22621.0. Every UBT invocation has NoEngineChanges and MaxParallelActions=4. Engine source in host Intermediate is expected for monolithic builds; no action writes in either engine tree. The final Baseline51 Editor query is action-plan-only and has zero outdated engine and engine-plugin actions.

EI2 admission uses three samples at free commit >=16 GiB and physical >=4 GiB. In-run supervision stops only the owned UBT tree after two readings below 6 GiB commit; low physical memory alone does not stop a run. The hashed job configuration sets the target allow-list, job folder, memory policy and parallel cap. No global or per-user configuration changed.

Capture, ControlServer and Shaders are 1 in generated non-Shipping definitions. The candidate's explicit bench profile is 1; the release predates those bench macros and its compiled AnomalyBench module/dependencies form the control, as frozen before the first build. Tick pin is baseline-equal. The unchanged host fixture's C4996 SetMasterPoseComponent deprecations are compared by code, normalized file:line, category and multiplicity.

Strict covers all 64 ordinary plugin sources, 77 independent header translation units and 11 UHT-generated sources on both targets: 152 C++ compiles each. Plugin responses have unity and PCH disabled. The real missing-defining-include mutant fails at its own source, while its positive control compiles. PCH/unity and missing-coverage reader mutants reject. Exact Build.cs bytes and tracked status restore, generated test sources are removed by receipt, and normal Editor/Game rebuilds pass. The first restored Editor plan retained cached normal unity objects and contained only relinks, so the unchanged zero-C++ guard refused it. One receipted recovery removed twelve cached normal IA objects (six per target) and rebuilt both restored targets with actual C++ actions. Original strict results and the refusal are retained; strict/restoration-retry/summary.json joins them to the successful normal rebuilds. Future strict runs must account for cached normal unity objects before requesting a fresh normal compile; the zero-compile guard must remain unchanged.

Candidate Shipping has no IA or probe module action, BuildProducts receipt or newly produced compiled output. The separate CaptureBench fixture plugin still compiles one translation unit: its original descriptor has no Shipping blacklist, and the ruled host plugin set is unchanged. This is a bench-host Shipping proof, not a delivery package; fixture removal remains a delivery obligation. The IA descriptor's staging entry carries no module code; packaged exclusion remains later work. No Baseline51 Shipping build was run.

Evidence is under D:/IA_Europa/Receipts/09409. qualification/final-baseline51-binaries.json and qualification/final-regate51-binaries.json bind the artifacts for A9-A11 after strict restoration. qualification/A4a-artifacts-unchanged.json verifies A4a's exact 27-file set for A6-A8. No production change required a new 4.25 compile. Permanent C#, descriptor and .uproject bytes were unchanged, so the full JsonExport/mutant rerun was not triggered; fresh compiler selection and all restored rule hashes are recorded.

The first baseline Game plan exposed a tooling path assumption: monolithic generated definitions live in host Intermediate. The failed pre-compile receipt is retained. The reader was corrected, tested on synthetic and retained native evidence, and the build was repeated. Native UHT short-name aliases preserve engine-source action classification. This did not change production behavior or a gate predicate.

EA8 entry/exit against P9 v2 and EA11 pass. All operation ledgers pass within the C/E and EG1 bookkeeping caps. The owner-authorized cache removal predates this job; prunable worktree registrations and protected refs were not changed. No runtime launch, cook, UAT, push or tag occurred. The host probe remains the empty A3 skeleton. A5 owns its behavior/fixtures; A6 uses the unchanged A4a manifest. Runtime/staged/delivery qualification remains pending.
