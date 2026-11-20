094-38 remains NEEDS-DECISION. The LOD parity fix, measured diagnostic and exact 5.1 far plane are committed. Game Development builds successfully. Editor used its three FM1 attempts: two memory stops, then a compile error. That error is fixed and compiled in Game, but the Editor budget is exhausted. No kit was armed, no smoke ran, and FV6 redaction was not executed.

Entry and exit lean checks passed: m51, master, release/2026-10-05, tags, Content, Config, descriptors and the .uproject are unchanged. Foreign workloads were recorded and left alone; drive-root additions/removals were empty. Evidence: [entry.json](D:/IA_Europa/Receipts/09438/entry.json) and [exit.json](D:/IA_Europa/Receipts/09438/exit.json).

The LOD call now passes only its target, allowing shared 5.1 `Anomaly_LodCorruption.cpp:29-32` to choose `WorstLodSentinel`; `AnomalyLod.cpp:144-152` resolves the coarsest model. The parity inventory covers 39 files and identifies all 104 retained 4.25-only lines, each with its reason and source citation: [09438-auto-parity.json](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09438-auto-parity.json).

| Difference found | Disposition |
|---|---|
| Pool replaced texture IDs with lighting/LOD only on 4.25 | Removed; original first nine IDs retained, two shared default-off opt-ins appended. |
| Default-enabled pool differed | Removed; original four defaults retained. |
| Lighting candidate collection existed only on 4.25 | Shared explicit opt-in path using the same visible receivers and AffectsPrimitive logic. This extends the shared opt-in surface; defaults remain unchanged. |
| Extra multi-LOD candidate filter | Removed. |
| Explicit finest-model argument | Removed; shared coarsest default used. |
| Missing-object bench-only admission | Removed; shared registration is AnomalyInjectorSubsystem.cpp:275. |

Retained lines cover unavailable backend/registry APIs, stock 4.25's lack of Nanite, measured component/view/physics API adapters, the missing UTickableWorldSubsystem dispatch, and disabled texture-backend stubs. The inventory cites the corresponding 4.25 declarations and PHASE0 measurements. Existing authorized census ownership and host-custom-depth exclusions remain recorded deviations.

FV3 now reads viewport type through GetViewportType, client through GetClass, actual viewport and captured sizes separately, actual Slate backbuffer format, RHI, render-hook count and census lost/no-pass/polluted counters. Tick fields read FApp fixed-step/delta, GEngine fixed-rate/rate/smoothing, and the callback's actual hook name/ELevelTick. An eight-cell canvas marker supplies a pixel test: post-UI controls must contain it, and every capture must exclude it. The reader independently checks the PNGs. Logging is scheduled at resolved frames 1 and 300. Sources: 4.25 UnrealClient.h:606, GameViewportClient.cpp:1641 and SceneRendering.cpp:3044. Runtime proof is NOT RUN; no self-check lines were emitted, so there are no verbatim lines to report.

FV4 writes 1,000,000 for both finite and infinite annotation far planes, matching shared 5.1 AnomalyCaptureSubsystem.cpp:594,10291. Raw projection values remain measured separately.

Changed-code verification passed: eight FV test groups and their negatives; the native timeout/discard, finite/infinite-far and marker tests; seven launch-admission cases; five keyword-Rotator checks; and the fixture-name scan. No in-editor Python changed, so a new Python 2.7 harness run was inapplicable. Two harness setup errors were corrected (text decoding and reused output directory); their failed receipts remain. [test receipts](D:/IA_Europa/Receipts/09438/tests/fv-summary.json) contains the final results.

All actual UBT starts used BelowNormal priority and -MaxParallelActions=2. Free-commit readings are GiB; duration is the guarded build process duration.

| Actual attempt | Start | Minimum | Seconds | Result |
|---|---:|---:|---:|---|
| Editor 1 (editor-01) | 10.021 | 2.076 | 67.39 | Owned-tree memory stop |
| Editor 2 (editor-05) | 10.439 | 2.063 | 42.00 | Owned-tree memory stop |
| Editor 3 (editor-06) | 10.582 | 7.932 | 81.91 | Exit 6; 5 AnomalyCapture diagnostics |
| Game 1 (game-01) | 10.207 | 8.010 | 98.12 | PASS; exit 0; no errors |

The compile failure was one invalid ToSharedRef call on an existing TSharedRef, followed by four serializer overload diagnostics, all in AnomalyCaptureDiagnostic425.cpp:117. Commit 4dbcf9b passes the reference directly; Game compiled it successfully. The first build and Game reported zero compile errors. The two memory stops killed only recorded owned PIDs. Three additional admission-only directories contain no UBT PID and consumed no build attempt. [build-summary.json](D:/IA_Europa/Receipts/09438/build-summary.json) retains argv, readings, PIDs and errors.

All smoke measurements remain NOT RUN:

| FU5/FV gate | Result |
|---|---|
| Blinking, lighting, LOD, missing-object: at least three events each; affected masks; clean masks absent | No session; fired/mask counts N/A for each anomaly. |
| Each LOD event's visible change | No events or pictures. |
| Schema 2.1, affected/injected sets, coverage_pct, far plane | No runtime annotation. |
| Census over 55, exhaustion/lost batches, wall strip | No runtime evidence; focused timeout/discard tests passed. |
| Exact custom-depth restore; host-owned actors untouched; player-view match | No runtime samples. |
| Measured self-check, pre-UI control, normal exit | No runtime samples. |

Map used: none. The build pair failed before admission, so neither street nor EA_Core fallback was launched. FV6 was not executed because it requires a passing smoke: zero rewritten commits, no backup ref and no tree-diff proof. Its isolated redaction transformation test passed; that is not a history rewrite. Code's scan/card remains subsequent work.

The old spent manifest was not rebound. Only 21/27 entries still match: authorized builds changed or removed the Game EXE/PDB, probe PDB, Capture PDB and Injector DLL/PDB. The Editor binaries are mixed and do not qualify for launch. [binary-state.json](D:/IA_Europa/Receipts/09438/binary-state.json) records every bound file's current/expected hash and existence. Current EXE/DLL SHA-256 values follow; Host means `D:/IA_Europa/StylizedParisStreet/Binaries/Win64`, Plugin means `D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Binaries/Win64`. No archives were created.

| Artifact | SHA-256 |
|---|---|
| Host/StylizedParisStreet-Win64-Shipping.exe | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| Host/StylizedParisStreet.exe | `34b301d20d1c8206da401dc8ea3b43f6b501c9c992cb74d2dfd4823b588cfb37` |
| Host/UE4Editor-EuropaPhaseAProbe.dll | `76400798ccd02cec57576555a27fc42b4bf270ab29d61c34f8a555b704c39d0c` |
| Host/UE4Editor-StylizedParisStreet.dll | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| Plugin/UE4Editor-AnomalyBench.dll | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| Plugin/UE4Editor-AnomalyCapture.dll | `6dfa607a5aee4b65b74f395aa67ba1b1cc0c6e281179e146b3d9c0b8e6b86cf1` |
| Plugin/UE4Editor-AnomalyControlServer.dll | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` |
| Plugin/UE4Editor-AnomalyInjector.dll | `68993ba4042b0d7f5495c26cb79ead8a8b26ef4c1601c51f7ef11e23439c24a0` |
| Plugin/UE4Editor-AnomalyShaders.dll | `b5b767d343422d7360a8b78ec29c7ce0da0090f056d13c5f9fd11b574240e6ad` |
| Unit test: Receipts/09438/tests/census-batch/test.exe | `d36d1abd1e114ce31746d70972ea37526546a69a637424afdc42036ee64c8e06` |

Runtime crashes: none; upload signs: none observed; protected changes: none. No editor/game session ran. No runtime bench lever or security, paging, power or machine setting changed. Build priority and parallelism applied only to owned children, which exited or were stopped. Partial-output quarantine receipts were retained; no additional manual deletion occurred.

Deviations and limits: the Editor fix could not be rebuilt within the three-attempt budget, preventing rebind, smoke and conditional redaction. One admission wrapper was stopped by its recorded PID before UBT so the backbuffer measurement could be added. Another ended before UBT; the next D: reading was 24.897 GiB, consistent with the final disk-floor guard, but the original exception was not retained. The wrapper now saves the full exception. No fourth Editor attempt was made.

All commits below are local on feat/europa-port; none was pushed, no tag was created, and the final worktree is clean. Final commit: `8bdcb4bf54098c22c458eaa118ec97ee0101be16`. Full implementation and closeout receipts: [09438-office-capture-receipts.json](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09438-office-capture-receipts.json).

| Commit | Change |
|---|---|
| `99e69a2b48b3ad9da32c952068274ebf5a6940f5` | fix(europa): restore auto parity and measure capture diagnostics |
| `0d40bcf2039da3fdd312ecde2050746dad17e47b` | test(europa): report each smoke LOD event pixel change |
| `d589dd39f07ea348c455c924bfbf01ad7c73a3f2` | test(europa): isolate repeated diagnostic fixtures |
| `3d52e45aa19286fa60fe78b33c5022b97164ef33` | docs(europa): distinguish unsupported-engine compile guard |
| `50ad76f3aba1ad21b830602d463a5d64f32916cb` | fix(europa): measure viewport extent and backbuffer format separately |
| `59717924ff0e7822885a11c5603b4bfc2ccd5a4c` | docs(europa): include auto admission dependencies in parity inventory |
| `4dbcf9bf5fbffbc13bc8e4df55480473d36fd08a` | fix(europa): serialize the diagnostic shared reference directly |
| `8bdcb4bf54098c22c458eaa118ec97ee0101be16` | docs(europa): record spent Editor budget and successful Game build |

VERDICT: NEEDS-DECISION | Game builds; corrected Editor needs a new build budget before the smoke and FV6 redaction.
