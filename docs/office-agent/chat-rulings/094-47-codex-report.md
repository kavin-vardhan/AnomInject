<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, danger-full-access resume, exit 0, 6590 s, 14:36:55-16:26:45 IST, PC in modern standby 15:30:45-16:13:50). Body below is byte-identical to report.md, sha256 57BD89BD2DE74EDB7C9C4FC328548EEF3DB18F4D5BF3FB140BCCE8C5904D0161. -->
094-47 stopped at Stage A / GQ2 / A1. The 4.25 Development Editor and Game builds pass. ReGate51 compiled and linked the changed modules with UBT exit 0, but fourteen first-header error diagnostics failed the unchanged build gate. Protocol smoke, packaging, dashboard work and Stage C remain unrun. LOCAL VALIDATION / NOT OFFICE-READY. The inherited camera qualification and plugin-push hold remain in force under OD4.

Final local `feat/europa-port`: `a2250fda7a9b2f3754fd13c6061fe632f45b9d6e`. Remote, checked with read-only `ls-remote`: `c5d0919e5074404d26da8c16ccf845b360bde383`. No plugin or dashboard commit was pushed; no tags were created. The dashboard remains clean on `master` at `7ea2e51bf55e19c2949641bbff2ec72afb4ae493`; `feat/europa-425` was not created.

| Stage | Result | Evidence and limits |
|---|---|---|
| A: implementation and focused tests | Implemented; helper tests pass | 543 checks; all four framing, fragmentation, queue and teardown mutants fail as predicted. Four redaction tests pass. |
| A: 4.25 Editor / Game | PASS | Final builds at `a231a57`: 22.649 s / 33.115 s, exit 0, zero compiler error lines, zero engine actions. |
| A: 5.1, ReGate51 | FAILED GQ2/A1 | Exit 0 after 213.306 s; 14 first-header errors. No 5.1 launch. |
| A: editor `-game` protocol smoke | UNRUN | Stage A compile gate stopped the sequence. The smoke script parses, but no live protocol claim is made. |
| B: package, packaged console C5, dashboard tests/build | UNRUN | No cook, package, desktop build or desktop connection. |
| C: both-RHI equivalence, early stop, reconnect, second capture | UNRUN | No captured frames or lifecycle results. |

The local implementation adds the loopback 4.25 libwebsockets transport, raw binary messages, fragment reassembly, bounded queues and dispatch after service; the 5.1 wrapper retains the existing transport calls. Console and dashboard use the shared C5 entry, with cancellable warmup, admission errors, keep-alive policy and final requested/written counts. The native PNG preset uses the existing auto pool and schedule, with omitted seed 4242. The office include-order change is included. These are source/compile results; auth, cancellation, teardown and capture equivalence still require the live gates. Shipping remains excluded. No camera algorithm, default route, material or host Content/Config change was made.

The fourteen ReGate51 diagnostics affect AnomalyCapture translation units whose first three source lines are byte-equal to entry `f3fbba7`. Correcting that broader 5.1 include ordering exceeds GQ2's shared-transport scope, so it was not changed. Both changed DLLs linked, but exit 0 is not a passing gate. Details: [stop.json](<D:/IA_Europa/Receipts/09447/stop.json>) and [ReGate51 UBT log](<D:/IA_Europa/Receipts/09447/builds/regate51-01/UBT.log>). ReGate51 was restored to its original clean detached `40a8eaf983518fe1992e172434aa8caf347654f1`; its build artifacts are retained and archived as unqualified.

The repository lever audit also fails: 12 R6 inline-guard findings and 32 B2 source/binary registration mismatches. Eleven R6 source lines are identical at entry; the other is the extracted shared C5 guard. The audit compares the unfiltered source registrations with the tested 4.25 binary union. No exception or passing verdict was substituted. The full strict-include audit remains incomplete. [Lever audit](<D:/IA_Europa/Receipts/09447/lever-audit-result.json>).

| Build attempt | Exit | Seconds | Result |
|---|---:|---:|---|
| [editor-01](<D:/IA_Europa/Receipts/09447/builds/editor-01/result.json>) | 6 | 136.207 | FAIL: missing engine-version macro includes |
| [editor-02](<D:/IA_Europa/Receipts/09447/builds/editor-02/result.json>) | 0 | 101.207 | PASS; TEXT warning corrected later |
| [game-01](<D:/IA_Europa/Receipts/09447/builds/game-01/result.json>) | 0 | 110.435 | PASS; TEXT warning corrected later |
| [editor-03](<D:/IA_Europa/Receipts/09447/builds/editor-03/result.json>) | 0 | 22.649 | PASS |
| [game-02](<D:/IA_Europa/Receipts/09447/builds/game-02/result.json>) | 0 | 33.115 | PASS |
| [regate51-01](<D:/IA_Europa/Receipts/09447/builds/regate51-01/result.json>) | 0 | 213.306 | FAIL gate: 14 first-header errors |

The initial 4.25 macro errors were corrected within the fresh FM1 budget. The new Windows TEXT warning was removed before the final builds. Engine actions remained zero throughout. Compile predictions were sealed before the builds in [stage-a-predictions.json](<D:/IA_Europa/Receipts/09447/stage-a-predictions.json>). The standalone tests reject a length prefix, premature fragment delivery, an unbounded queue and delivery after stop. Compiled string checks find the new server/C5 markers in the 4.25 binaries; the unchanged Shipping executable is a negative reference, not a new Shipping qualification. [Focused tests](<D:/IA_Europa/Receipts/09447/focused-tests.json>); [binary markers](<D:/IA_Europa/Receipts/09447/binary-markers.json>).

Entry FJ1 checks and P9's full `main()` passed; P9 took 340.385 s. Final FJ1/protection checks pass, and the final Source tree equals the tree compiled at `a231a57`. [P9 result](<D:/IA_Europa/Receipts/09447/owner-p9-result.json>); [final exit](<D:/IA_Europa/Receipts/09447/exit.json>).

All commits below are local on `feat/europa-port`. Author and committer timestamps are equal, real times on 2026-11-05 (IST).

| Commit | Time IST | Subject |
|---|---|---|
| `2af16c00932c61dea86ce827d2d88388093cca7f` | 15:02:26 | feat(europa): add control transport and shared C5 lifecycle |
| `dfd11cadc7ca194cf47900e01c9718a791286ad9` | 15:05:50 | fix(europa): include version macros at public boundaries |
| `a231a577905b3ea6163500b5a5982f108c8c2183` | 15:11:47 | fix(europa): isolate transport platform headers and mask control logs |
| `a2250fda7a9b2f3754fd13c6061fe632f45b9d6e` | 16:21:13 | docs(europa): record control port compile gate stop |

Capacity before and after each native build and archive copy is below, in GiB, ordered C:/D:/E:. Exact byte readings and UTC times are retained in the linked receipts. All readings stay above C:10 and D:/E:50 GiB. There was no admission wait and no runtime launch; the 3 GiB launch reserve was not reduced. No session caused a C: drop of 1 GiB or more because no session ran.

| Step | Before C / D / E | After C / D / E |
|---|---|---|
| [editor-01](<D:/IA_Europa/Receipts/09447/editor-01-capacity.json>) | 22.7598 / 53.2855 / 52.2269 | 22.7599 / 53.3562 / 52.2258 |
| [editor-02](<D:/IA_Europa/Receipts/09447/editor-02-capacity.json>) | 22.7599 / 53.3556 / 52.2258 | 22.7596 / 53.2687 / 52.2252 |
| [game-01](<D:/IA_Europa/Receipts/09447/game-01-capacity.json>) | 22.7595 / 53.2687 / 52.2252 | 22.7594 / 53.2607 / 52.2252 |
| [editor-03](<D:/IA_Europa/Receipts/09447/editor-03-capacity.json>) | 22.7565 / 53.2601 / 52.2252 | 22.7552 / 53.2595 / 52.2251 |
| [game-02](<D:/IA_Europa/Receipts/09447/game-02-capacity.json>) | 22.7552 / 53.2596 / 52.2251 | 22.7550 / 53.2583 / 52.2252 |
| [regate51-01](<D:/IA_Europa/Receipts/09447/regate51-01-capacity.json>) | 22.7543 / 53.2510 / 52.2248 | 22.7535 / 53.2038 / 52.2244 |
| [archive](<D:/IA_Europa/Receipts/09447/archive-capacity.json>) | 22.7624 / 53.2038 / 52.2242 | 22.7624 / 53.2038 / 51.3167 |

Final free space: C: 22.2245, D: 53.2085, E: 51.3165 GiB. Packaging capacity was not measured because Stage A stopped first; no claim is made that the package would fit. `E:\IA_BuildCache\09447-package` was never created: 0 files, 0 bytes. Cooked-material and private-pass resource verification are unrun, as are packaged H4 and strict texture G3 (IoU 1.000, zero spill).

The archive is `E:\IA_BuildCache\09447-evidence\binary-archive`: 38 files, 974,343,342 bytes (0.907428 GiB), including 14 EXE/DLL files and 24 supporting files. All copies were rehashed and their mtimes preserved; all 38 still match the live inventories at exit. Full paths, sizes, hashes and mtimes are in [binary-archive.json](<D:/IA_Europa/Receipts/09447/binary-archive.json>). Each artifact link below is its absolute archive path. Unchanged files are included for inventory completeness; the Shipping executable was neither rebuilt nor launched.

| Archived EXE/DLL | SHA-256 |
|---|---|
| [425 / Europa unchanged Shipping.exe](<E:/IA_BuildCache/09447-evidence/binary-archive/425/Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe>) | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| [425 / Europa.exe](<E:/IA_BuildCache/09447-evidence/binary-archive/425/Binaries/Win64/StylizedParisStreet.exe>) | `b5c1a94fb79d292af07a1354957b1252be5cc47d599546e5a33381af36a13ea2` |
| [425 / UE4Editor-EuropaPhaseAProbe.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/425/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll>) | `2b19de1d4f1628d5dac9b8968bed9ea62cfef6326d6d4f7edac7e932bb682595` |
| [425 / UE4Editor-Europa.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/425/Binaries/Win64/UE4Editor-StylizedParisStreet.dll>) | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| [425 / UE4Editor-AnomalyBench.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll>) | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| [425 / UE4Editor-AnomalyCapture.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll>) | `8036d3cda8b4a91fab5ef101eac938b892f5284e0478c64ff57d6dee42a3bf4f` |
| [425 / UE4Editor-AnomalyControlServer.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll>) | `ddf6af9816e8a75b22b642438bf90077030392468c3d8a2173656bb79a25aa8e` |
| [425 / UE4Editor-AnomalyInjector.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll>) | `00b2e8f49f0f5394eab3b8c027ec908d7a0af9efbc574e625ffb905b2e6a256d` |
| [425 / UE4Editor-AnomalyShaders.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll>) | `3d0ff3bdd2a721f48573124a0ceaa79870fdae460bd0a01c90df2220cc7cb0fb` |
| [ReGate51 / UnrealEditor-AnomalyBench.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/ReGate51/Binaries/Win64/UnrealEditor-AnomalyBench.dll>) | `128e48761dfc075662200648667725e426f8e978d8400a8aedbab9c66612c33c` |
| [ReGate51 / UnrealEditor-AnomalyCapture.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/ReGate51/Binaries/Win64/UnrealEditor-AnomalyCapture.dll>) | `dc3195fdb4cb212fef0a2e3e28813aaf2b8d0a8ae52fa3e955ccfc441df344d2` |
| [ReGate51 / UnrealEditor-AnomalyControlServer.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/ReGate51/Binaries/Win64/UnrealEditor-AnomalyControlServer.dll>) | `4e83f6cac88f6e99fc4dec598f574f455020d390d16c02a94e2e6cd234726aeb` |
| [ReGate51 / UnrealEditor-AnomalyInjector.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/ReGate51/Binaries/Win64/UnrealEditor-AnomalyInjector.dll>) | `88f288eed3763402f91fc5e38fe8612451df8cc3f54db95a6f391f0aefa7e4b5` |
| [ReGate51 / UnrealEditor-AnomalyShaders.dll](<E:/IA_BuildCache/09447-evidence/binary-archive/ReGate51/Binaries/Win64/UnrealEditor-AnomalyShaders.dll>) | `f8736404b31745068a78638807114d25a4f42a3df093f4f2e8c8e28aa7c93c9c` |

The five standalone transport-test executables remain at their original evidence paths, linked below; they were not packaged or copied into the binary archive.

| Test EXE | SHA-256 |
|---|---|
| [wire](<D:/IA_Europa/Receipts/09447/transport-tests/wire/test.exe>) | `c1d8846b8a1555e0ad4bccd91ef465c756e2e02fb9ec58edbe27af3d92d9bc85` |
| [length-prefix](<D:/IA_Europa/Receipts/09447/transport-tests/length-prefix/test.exe>) | `f9b19736f727a04e4d0b2a3510f05210e7d0551063d33c3fcba0fccd4d6062e4` |
| [early-fragment](<D:/IA_Europa/Receipts/09447/transport-tests/early-fragment/test.exe>) | `b7bfcd66fca4a12deac6ffb55464d8868e1df82ed3bfa8291537c3ca22a096ea` |
| [unbounded-queue](<D:/IA_Europa/Receipts/09447/transport-tests/unbounded-queue/test.exe>) | `cdf6063807dc7ea88fde98952fc2f4aa07e47e235e83bd029f06acf99a41d9d1` |
| [after-stop](<D:/IA_Europa/Receipts/09447/transport-tests/after-stop/test.exe>) | `09f7db8a6c84461f73ae79ad3fe6d5b40847bf3a38874f1338bcee1439d4e390` |

Safety and deviations: zero runtime launches, runtime crashes, upload signs, leaked control tokens or protected Content/Config/descriptor changes were observed. No listener started, so no configured or random control token was generated or copied; copied control-log paths mask token values as `<masked>`. No security, paging, power, elevation or owner-app changes were made. Foreign processes were recorded and left alone. The final owned-process audit checks 156 recorded identities and finds 0 live owned build children. [Owned-process closure](<D:/IA_Europa/Receipts/09447/owned-processes-closed.json>).

Build output replacements and makefile refreshes are receipted: 6 makefile deletions across six build receipts, with path/hash inventories; ReGate51 restoration removed 378 files created by the temporary checkout, listed before restoration. No evidence, owner file or old archive was deleted to fit capacity. [Build/restore ledger](<D:/IA_Europa/Receipts/09447/final-state.json>); [restore intent](<D:/IA_Europa/Receipts/09447/regate51-restore-intent.json>). No compression was performed in 094-47. OD5's accepted earlier compression remains a one-time deviation; it was not originally authorized by GP1.

The extra exit fixture check initially used an obsolete 09442c map hash. It was corrected to the newer asset receipt already verified at entry; all entry-protected fixture assets and the saved ini match. The mismatch observation is retained in [fixture comparison](<D:/IA_Europa/Receipts/09447/fixture-old-receipt-audit.json>). The feature worktree is clean; protected branches/tags, fixture worktree and dashboard refs are unchanged. ReGate51's temporary checkout is restored. The runtime launcher remains disabled. No further physical stage followed the failed GQ2 gate; the time checkpoint was not reached.

Runtime bench settings, materials, near plane and route selections were not changed in this job. Defaults remain blinking, lod_corruption, missing_texture and corrupted_texture. camera_clipping remains opt-in and unqualified: inherited odd-width D4 and strict texture G3 failures, plus unrun GO3, remain open. No camera correction, new floor or relaxed threshold was introduced. The current shipped instruction remains the 094-45r office card; no replacement office-ready card was created. No 5.1 runtime regression was run, and it remains required before any merge toward a 5.1 release.

Completion requires a scope decision for the existing 5.1 first-header diagnostics, followed by the outstanding Stage A audit/protocol gates and Stages B/C. This report does not clear the independent OD4 plugin/camera release hold.

VERDICT: NEEDS-DECISION
