# 094-27c — camera, callback and crash policy amendment

Prediction date: 2026-10-30. Authority: 094-27b-ruling FI1–FI6, carrying FH2–FH8, FF5 and the earlier cited runtime design. The old armings are spent. Only the three exact FI2 package paths may change, first by quarantine and then by new authoring. No native source, binary, engine config or 5.1 work changes.

FI2: the camera constructor names pitch, yaw and roll explicitly. The recording stub models `(roll=0.0, pitch=0.0, yaw=0.0)`, matching stock 4.25 `Engine/Source/Runtime/CoreUObject/Public/UObject/NoExportTypes.h:454` and `Engine/Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h:2142-2146`. Both engine Python 2.7 harnesses and both kit test entry points run the AST rule over every in-editor Python script and launcher used by these kits. No-argument Rotator is allowed; any supplied arguments require all three keywords. The separate 5.1 authoring recipe is frozen and never runs in this job.

The old recipe saved its mesh before the camera and called new_level, which itself saved a map (`Plugins/Editor/EditorScriptingUtilities/Source/EditorScriptingUtilities/Private/EditorLevelLibrary.cpp:535-543`). FI2's no-save-on-failure requirement therefore needs an unsaved world first: `EditorLoadingAndSavingUtils.new_blank_map(False)` (`Source/Editor/UnrealEd/Public/FileHelpers.h:38-44`; implementation `Private/FileHelpers.cpp:4241-4259`) bypasses SaveDirtyPackages, creates a new world and resets its filename. After the camera assertion succeeds, the unchanged mesh/actor values are authored and `save_map(world, exact_package)` persists the map (`FileHelpers.cpp:4225-4238`, public declaration :65-74). The failure branch records raw/normalized/expected angles, invokes QUIT_EDITOR once and returns without a further engine action. The approved ExecutePythonScript engine route remains; its own next-tick close is stock lifecycle (`EditorPythonExecuter.cpp:148-175,185-195`), not a second Python failure callback.

FI3: tick takes a busy guard before any load. Reentrant ticks only increment a receipt counter. Terminal state is set before unregistering the callback; all actual/expected observations are recorded first, followed by one exit request. Subsequent ticks and finish calls are inert. Angle comparisons normalize both wraparound and their difference, with the ruled 0.01-degree tolerance. Load retains every checked saved value: tags/labels, transforms, static-mesh paths/mobility, LOD count, light settings, FOV, raw and normalized camera rotation, all six post-process fields, unbound state, GameModeBase and PlayerStart location/rotation. No runtime save is introduced.

## Prior Load cause: confirmed facts, unconfirmed crash mechanism

The exact previous source is preserved in Receipts/09427c/before-fi with hashes. Its runtime_editor_425.py:122-166 has no busy or terminal guard; :135-136 calls readback from a registered Slate post-tick callback, :75 calls load_level, :104-105 raises on camera mismatch, and :32-44 records and requests exit on every finish. The real 09427b Load Runtime.log:711,713,714 contains three MAP LOAD requests at frame 10 within 29 ms, before the first completion at :731. Completions :731,750,769 each precede QUIT_EDITOR :732,751,770. These were nested requests already in flight, not three retries after the first failed check.

Stock load_level calls LoadMap (`EditorLevelLibrary.cpp:615-632`), which requests progress (`FileHelpers.cpp:4213-4219`). Slow-task UI can tick Slate (`FeedbackContextEditor.cpp:357-369,476,539-543`); Slate broadcasts post-tick (`Source/Runtime/Slate/Private/Framework/Application/SlateApplication.cpp:1503`), and Python registers on that delegate (`Plugins/Experimental/PythonScriptPlugin/Source/PythonScriptPlugin/Private/PySlate.cpp:126-137`). This establishes the source route for callback re-entry. The harness deliberately re-enters during load_level to test the guard. The exact progress stack was not captured in the old real session, so that invocation path is strongly supported, not a captured stack trace.

The shutdown access violation itself remains UNCONFIRMED as to root cause. Runtime.log:775 records preparing to exit; :783 records reading 0xffffffff; :785-788 lists four unsymbolized Niagara frames; :807-810 is forced error shutdown. The evidence does not prove that the camera mismatch or nested loads caused that Niagara fault. No claim of a confirmed engine fix is made.

FI4: every runtime crash fails its leg. The first drains all surviving owned processes, monitors first, and proceeds. The second ends the run. A broken smoke, authoring crash, protected change, upload sign or canary hit still ends it immediately. Crash cleanup is a separate receipt field so it cannot misclassify the original crash as a deliberate limit stop. Classified results and raw hashes are generated, never relabelled by hand. The eight existing rehearsal groups are retained with amended crash expectations, plus the single-shot callback case and first-crash/pass/second-crash sequence. Stand-ins start from kept real files, including the failed Load.

FI5: FE2 denied event logs remain a class. Sharing/lock violations 32/33 strictly below C:/Windows join it; the scan records each name, byte size, error and class. Readable Windows files are scanned; unrelated access denials, lookalike folders, other drives and parent escapes are not excluded. No OS permission or service change is made.

Both kits carry exact -MONITOR PID adoption (WindowsPlatformCrashContext.cpp:442-447,493-536), privacy switches, 8/6 GiB start and 4 GiB session commit lines, FF2/FG2 managed classes and FA5 idle admission. FF5 uses the fresh, hash-bound entry proof once; all per-session protected-file and binary checks remain. The authoring kit has two recipes and no Smoke; the runtime kit has 18 single-use slots, with Smoke first. Their deadline is 2026-10-31T00:30:00Z. Rehearsal and Python stubs are not real gate evidence. Pictures retain the previous AA echo, event validity, glass mask and pixel thresholds.

The emitter table and previous engine citations stand in docs/europa/09427b-runtime-amendment.md; a fresh copy is recorded at Receipts/09427c/emitter-table.json. All gate results and pictures remain pending until real sessions finish.

## Qualification before quarantine and real launches

All 15 suites and all ten final rehearsal groups passed. Author Python 2.7 attempt 1 refused the still-existing package paths; attempt 2 models their absence only inside the recording harness and verifies all existing package hashes stay equal. Runtime Python 2.7 passed both initial and final read-back runs. The original failed attempt remains in ledger/fi-implementation/child-007.log. No actual package was moved or saved by a test.

| Suite | Result | Receipt |
|---|---|---|
| fi-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/fi-tests-attempt1/summary.json |
| fh-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/fh-tests-attempt1/summary.json |
| runtime-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/runtime-tests-attempt1/summary.json |
| real-files | PASS | D:/IA_Europa/Receipts/09427c/scratch/real-files-attempt1/summary.json |
| reader-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/reader-tests-attempt1/summary.json |
| fc-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/fc-tests-attempt1/summary.json |
| fd-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/fd-tests-attempt1/summary.json |
| managed-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/managed-tests-attempt1/summary.json |
| fixture-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/fixture-tests-attempt1/result.json |
| fe-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/fe-tests-attempt1.json |
| session-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/session-tests-attempt1/result.json |
| tooling-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/tooling-tests-attempt1/tooling-self-tests.json |
| finish-tests | PASS | D:/IA_Europa/Receipts/09427c/scratch/finish-tests-attempt1/results.json |
| author-python27 | PASS | D:/IA_Europa/Receipts/09427c/scratch/author-python27-attempt2/summary.json |
| runtime-python27 | PASS | D:/IA_Europa/Receipts/09427c/scratch/runtime-python27-attempt2/summary.json |
| rehearsal | PASS | D:/IA_Europa/Receipts/09427c/scratch/rehearsal-attempt2/summary.json |

No native source changed; a rebuild would change bound binaries and is not part of this job. Git diff whitespace and changed-Python syntax/comment scans pass. The three comments noted in the prior review were removed; rationale stays in documentation.
