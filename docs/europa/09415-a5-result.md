# 094-15 A5 build and readiness results

All six Editor/Game Development rebuilds pass with the same probe sources, including the release d7eee97 control. The final candidate and probe source is 550e9b79e4d60b553cceb4f7b7dba6b22c2386a7. Production Source, Content, descriptor, Build.cs and host project declarations did not change. The final closeout proofs and verdict are in the run report; A6 owns the first editor launch.

| Final leg | Plugin SHA | Project / plugin / engine-source-in-host / engine / engine-plugin actions | Native seconds | C++ diagnostics |
|---|---|---|---:|---|
| 4.25 Editor | 550e9b7 | 5 / 0 / 0 / 0 / 0 | 22.37 | 0 errors, 0 warnings |
| 4.25 Game | 550e9b7 | 4 / 0 / 0 / 0 / 0 | 32.99 | 0 errors, 0 warnings |
| Baseline51 Editor | d7eee97 | 4 / 1 / 0 / 0 / 0 | 68.99 | 0 errors, 0 warnings |
| Baseline51 Game | d7eee97 | 5 / 0 / 0 / 0 / 0 | 94.90 | 0 errors, 0 warnings |
| ReGate51 Editor | 550e9b7 | 5 / 1 / 0 / 0 / 0 | 68.96 | 0 errors, 0 warnings |
| ReGate51 Game | 550e9b7 | 5 / 0 / 0 / 0 / 0 | 99.17 | 0 errors, 0 warnings |

The plugin action in each 5.1 Editor row is WriteMetadata, not a plugin compile. Every native invocation used NoEngineChanges and MaxParallelActions=4, passed EI2, received a fresh EO3 target makefile, and passed EN1. The 4.25 responses select MSVC product 14.16.27051 from directory 14.16.27023 and SDK 10.0.17763.0, /std:c++17 and no PCH. The 5.1 responses select product 14.38.33144 from directory 14.38.33130 and SDK 10.0.22621.0, /std:c++17 and normal Engine shared PCH reuse. The two unchanged 4.25 user XML warnings at BuildConfiguration.xml:16 and :22 remain accepted environment observations. No settings were edited.

EO1 attempt 1 passed the retained 4.25 known answers and 794 retained 5.1 status lines. The first current 5.1 failure then exposed cancelled dependents. Attempt 2 records those separately and passes 800 5.1 lines, both 4.25 known answers and all three fail-closed mutants. It also joins the failed current graph correctly: three compiles executed, three dependents cancelled. EO2 fallback was not used. The accepted 094-14 exit-zero Editor evidence was recovered offline before any new build.

Baseline51's first Editor run exposed C1083 through release's public control header, which includes private IWebSocketServer.h. One probe-only fix avoids that header on 5.1 and uses the already-loaded exact subsystem UClass and public native UTickableWorldSubsystem APIs. All three host source sets remain byte-identical, with the version branch inside the probe. The retry passes; the error did not survive a fix. See 09415-probe-compatibility.md for the precise access path and source citations. Both 4.25 legs were rebound after that shared probe edit.

EO6 now checks masked receipts and rejects a raw-value mutant; 17 checks pass. The first test invocation lacked its output parent and was retained as attempt 1; attempt 2 passed without weakening a predicate. The individual accepted exit tools are used; closeout09414.py remains abandoned.

The EI4 Baseline51 Editor query ran after the rebuild, with its own EO3 refresh: exit zero, zero actions in every class, and no bound output deleted. Six final makefiles preserve all base values except the admitted toolchain PATH prefix, and add only selected VC_COMPILER_PATH/VC_COMPILER_DIR. No blocked name remains. The accepted 094-13 8/8/5 module comparison and three Shipping mutants stand: no C#, descriptor or project declaration changed in this job.

New manifests under D:/IA_Europa/Receipts/09415/qualification bind 27 4.25, 39 Baseline51 and 44 ReGate51 artifacts, with explicit plugin/probe source labels and full hashes. Qualified Shipping products remain unchanged. No archive was created. The owner kit's real dry run passes source, routing, binary, P9 and ledger checks and reports PREPARED_ONLY with zero launches. It stays unarmed until a fresh authorized A6 job. The plain card is D:/IntrusiveAnomalies/_reviews/094-15-owner-card-draft.md.

Accepted earlier A5 work remains preparation: 69 synthetic reader controls, fixture recipes, screenshot driver, real observe-only idle collection and package-header analysis. No runtime, picture, GC, tick, fixture-authoring or owner gate is claimed as passed. The five newer plugin packages remain untouched; A6 verifies the predicted 4.25 registry rejection before authoring. The owner must close heavy jobs and answer No to any rebuild-modules prompt.

Evidence: D:/IA_Europa/Receipts/09415. The run report carries entry/exit proof, every operation's drive readings and attribution, engine bookkeeping, all build receipts, binary hashes, commit times and the final verdict. No push, tag, machine-setting change or runtime launch.
