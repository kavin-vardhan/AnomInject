<!-- Collected by Claude Code (mailbox brief 094-18) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md; sha256 FC5F1207D5EBF731347A086623DE175748B5980B1CFAAB3FE7BFDA5F02CAED5A, 8108 bytes; thread 01a14334, resume 10:45:59-11:38:32 IST, 3153 s, exit 0; body below is byte-identical -->
**094-18 stopped before the probe fix and rebuilds.** The owner's editor left `E:\EpicGames\UE_4.25\Engine\Intermediate\EditorRuns\31124` (0 bytes), outside the permitted 4.25 `BuildRules` folder. The owner-leg ledger classifies it as STOP, and this job verified the same file, size and timestamp. EN5 explicitly excludes engine-tree writes outside EG1; EG1 admits 5.1 bookkeeping only. [Engine STOP receipt](D:/IA_Europa/Receipts/09418/engine-stop.json)

P9 v2 entry and exit passed. Paris/plugin content and all 110 bound artifacts remain byte-equal. No source edits, builds, runtime launches, commits, pushes, tags, host/protected-file deletions, setting changes or re-arming occurred. The probe defect remains unfixed, and no A6 gate is newly qualified.

The read-only evidence and protection results are:

| Check | Result |
|---|---|
| P9 v2 re-derivation | Exact byte comparison with v1 and the removal manifest passed; v2 SHA-256 `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`. |
| EA8 entry and exit | Each inventoried 472,553 files, admitted the existing 1,023 EG1 records, and matched 23,033 full binary/config hashes (104,794,989,456 bytes). Protected refs, indices and statuses matched. |
| 4.25 engine audit | STOP: the inherited `EditorRuns/31124` file is outside the allowed class. P9 v2's inventory and this additional engine check are separate results. |
| ED1 | Entry C: 23.44 GiB, D: 75.53 GiB, E: 69.45 GiB; exit-proof reading C: 23.44 GiB, D: 75.08 GiB, E: 68.57 GiB. All start thresholds and floors passed. |
| ED3 | MSVC 14.16.27051 (directory 14.16.27023), SDK 10.0.17763.0; MSVC 14.38.33144 (directory 14.38.33130), SDK 10.0.22621.0 verified. No UBT selection/export or compile ran. |
| Paris content | 1,453 files / 2,960,507,507 bytes match the supplied precheck hashes and the original E: content. |
| Plugin content | Five files / 189,599 bytes match the supplied precheck hashes and tracked Git blobs. |
| EA11 | Seven plugin reflected headers pass, including the in-memory forbidden-guard mutant. Probe headers are outside EA11's scope. |

Evidence: [entry](D:/IA_Europa/Receipts/09418/P9/entry/summary.json), [exit](D:/IA_Europa/Receipts/09418/P9/exit/summary.json), [toolchains](D:/IA_Europa/Receipts/09418/toolchains.json), [content hashes](D:/IA_Europa/Receipts/09418/content-stop-check.json), [EA11](D:/IA_Europa/Receipts/09418/ea11.json).

The owner-leg facts checked before closeout are:

| EP3 / ES3 item | Finding |
|---|---|
| Attempt and exit | Slot 01 used; launched marker present; receipt records one launch, editor PID 31124, exit code 3. Slots 02/03 and `refused-attempts` are absent. |
| Current processes | The closeout snapshot found no UE editor, CrashReportClient, ShaderCompileWorker, UBT or AutomationTool process. No process was stopped. |
| Second session / report upload | Not determined in this stopped job; crash-client history remains to be checked. |
| EA4 owner-leg charges | Existing ledger records C: 25,172,775 bytes, E: 0, engine51: 0. Its one engine STOP is the zero-byte marker. Complete per-file crash-reporter attribution remains pending. |
| Content admission versus prediction | Content bytes pass the checks above. Five-package header rejection, registry skipping, load/resave/thumbnail/cook behavior, prompts, warnings, DDC/shader routing and shader counts remain unqualified. |
| Probe observations / A6 gates | Not parsed or qualified after the hard STOP. No conclusion about lost evidence or need for another owner session is made. ES1 keeps the original session spent. |

Evidence: [owner receipt extract and STOP](D:/IA_Europa/Receipts/09418/engine-stop.json), [process, slot and kit snapshot](D:/IA_Europa/Receipts/09418/read-only-closeout.json). The original owner evidence was left unchanged.

The marker's origin is documented in stock 4.25 [HotReload.cpp](E:/EpicGames/UE_4.25/Engine/Source/Developer/HotReload/Private/HotReload.cpp:379): lines 379–383 select `EngineIntermediateDir/EditorRuns`, lines 389–413 create the PID-named empty file, and lines 422–432 delete it; startup/shutdown call sites are lines 516/543. Its modified time is `2026-10-26T18:43:59.900607+00:00` and SHA-256 is `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`. This explains its origin but does not authorize an exception. The controlling limits are [EA4](D:/IntrusiveAnomalies/_reviews/094-01-ruling.md:101), ED4's 4.25 clause and [EN5](D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:55).

ES2's supplied probe call chain remains the accepted defect context: `Cleanup:157 → Snapshot:281 → SnapshotState:282 → EngineUtils.h:248`. Source verification of that chain, the teardown-path audit, probe-only fix, static check/mutant, six rebuilds, EN1/EO3 receipts, EI4 query and ES4 re-binding were not attempted. The new static-check D2 budget is unspent. Clean exit remains a future runtime proof under ES2.

The existing A6 bindings remain unchanged; no executable, DLL, PDB or archive was produced. [All individual binary hashes](D:/IA_Europa/Receipts/09418/exit-bound-artifacts.json)

| Manifest | Files verified | Manifest SHA-256 |
|---|---:|---|
| 425 | 27 | `4beccca6a58a32b643a691d8c1b460bef0cb997db59d8c8bd17a954b9f42931f` |
| baseline51 | 39 | `617a908243fccc3bd1f8fefc82768d79e7052fa2136be4223cfb7450ac079a8c` |
| regate51 | 44 | `f6e52a85b078fc44f713a32f649dd70dcf4daa74868e323568abbec09d8fa7b2` |

Feature head remains `b20e66ea24eff56272b98153a6bb9db9956f7b07`; Baseline51 remains `d7eee97731812384d4fd00af9032254923363ada`; ReGate51 remains `550e9b79e4d60b553cceb4f7b7dba6b22c2386a7`. All three worktrees are clean. The 09417 kit remains spent and unchanged. No bench lever changed. W4 date overrides were absent; no commit was created or pushed.

The positive-control canary was present in the base environment and matched the launch hash; filtered boundaries excluded blocked names. The launch-window canary scan found zero matches in 120 new/changed files from the launch second. The EK1 scan read 40,336 files / 20,772,386,426 bytes, found no new secret values in readable files, and decoded all six retained host makefiles without blocked names. A seventh makefile was a pre-launch synthetic scanner fixture. The scan had 1 read error: one pre-launch 670,796-byte owner-leg temporary file was permission-denied; its contents remain unverified (path in the scan qualification receipt). This is not an unrestricted all-files clearance. [Secret scan qualification](D:/IA_Europa/Receipts/09418/secret-scan/qualified.json), [canary scan](D:/IA_Europa/Receipts/09418/secret-scan/canary-final.json).

One closeout qualification assertion was corrected after failing: Code's masked-name canary fixture was created during this run, not before launch. Its exact masked placeholder contents and the separate real-canary scan qualify it. The real-canary predicate was unchanged; this correction is recorded in the qualification receipt.

The two 09418 read-only ledger operations passed: C: 0 bytes, E: 0 bytes and engine51: 0 bytes charged; zero engine or bound-artifact changes during those operations. Ledger evidence is 229,127,070 bytes, below 2 GiB. This current-job result does not waive the inherited owner-leg engine STOP. [Ledger summary](D:/IA_Europa/Receipts/09418/ledger-summary.json)

**EN5 applications: none.** The stock marker behavior was traced, but EN5's express engine-write exclusion prevents using its intent clause here. The deviation from the requested implementation is stopping before source changes/builds under that hard STOP; no gate was weakened and no protected file was restored or deleted.

Resume requires chat's disposition of the 4.25 `EditorRuns` marker/class. Then complete the remaining EP3/ES3 evidence checks before the probe fix, static can-fail, six rebuilds, EI4 query and manifest re-binding. No kit launch or re-arm is authorized by this report.

VERDICT: NEEDS-DECISION | Owner-leg 4.25 EditorRuns marker is outside the allowed engine-write class; probe fix and rebuilds not started.
