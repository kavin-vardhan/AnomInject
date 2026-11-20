<!-- Collected by Claude Code (mailbox brief 094-27c) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md at 2026-10-31T02:04:59 IST; body 51286 B sha256 CC904650E6A4D6279C01C54A04AC44C8024F13FFC60BB488F5F89F4CD480DC51; byte-identical below this line -->
094-27c is **RED**. The two A6 maps and shared mesh were quarantined and authored fresh; both camera checks passed before any save. Keyword-only Rotator checks, the real-signature Python 2.7 stub, callback re-entry/single-shot exit and crash counting passed qualification. The prior Niagara shutdown crash root cause remains unconfirmed. Runtime sessions: 18 of 18; crashes: 1. Gates: A3 PASS, A5 FAIL, A6 FAIL, A7 FAIL, A8 FAIL. Counted-event contact sheets: 1. Initial exit canary scan: GAP, 4 unreadable paths outside the tested exclusions, no matches. Raw files and generated reader verdicts are retained. No native build, push or tags.

The tested tooling is committed as `7073697ba95c2bd4c238426b08d137e910bd9ec0`; the final feature commit is `2e2766f1e3d134e58125ba24c8665d44ff46e8e7`, on a clean `feat/europa-port`. [The receipt index](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09427c-runtime-receipts.json>) binds 2883 files. No native build, push or tag occurred. All real evidence is retained under `D:/IA_Europa/Receipts/09427c`.

The [EA8 entry proof](<D:/IA_Europa/Receipts/09427c/P9/entry/summary.json>) and [EA8 exit proof](<D:/IA_Europa/Receipts/09427c/P9/exit/summary.json>) both passed against unchanged P9 v2. Entry hashed 23033 binary/config files (104794989456 bytes); exit hashed 23033. Protected git/source manifests and m51 remained equal. [The prior-results entry check](<D:/IA_Europa/Receipts/09427c/prior-results-entry.json>) found no protected change or upload in 094-27b; its already-ruled crash was not counted as a new crash.

| Drive | Entry free GiB | Final free GiB | Start / warning / floor GiB |
|---|---|---|---|
| C | 28.07 | 26.72 | 13 / 11 / 10 |
| D | 48.54 | 55.72 | 40 / 30 / 25 |
| E | 285.93 | 278.71 | 15 / 12 / 10 |

ED3 toolchains: VS Community 2022 17.12.35707.178; Build Tools 2017 15.9.34031.82; MSVC 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023; SDK 10.0.17763.0 and 10.0.22621.0. No 14.44, compile or export ran. [Toolchain receipt](<D:/IA_Europa/Receipts/09427c/toolchains.json>).

FI2 changes the camera to `unreal.Rotator(pitch=-12.0, yaw=0.0, roll=0.0)` and asserts normalized read-back within ±0.01° before any package save. The old recipe saved the mesh and implicitly saved a new level before reaching the camera; the corrected recipe creates an unsaved blank world, checks the camera, and then saves only the declared packages. The [dated amendment and prediction](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09427c-runtime-authoring-amendment.md>) and [re-bound A5 manifest](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09427c-a5-fixture-manifest.json>) document this necessary ordering change. The six FE1 post-process values remain unchanged.

Stock 4.25 [NoExportTypes.h](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/CoreUObject/Public/UObject/NoExportTypes.h:454>) names MakeRotator as the native make; [KismetMathLibrary.h](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h:2142>) declares its Roll, Pitch, Yaw order. The Python 2.7 stub models exactly `Rotator(roll=0.0, pitch=0.0, yaw=0.0)`. [FileHelpers.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Private/FileHelpers.cpp:4241>) implements new_blank_map(False); save_map is at line 4225. The static AST rule passes all five executed Python source/generator files and rejects every positional Rotator call with arguments. The separate 5.1 recipe was not edited or run.

FI3 adds a busy guard before map loading and a terminal state before callback removal. It records actual/expected checks, unregisters the callback and requests exit once; later callbacks do nothing. FI4 counts actual runtime crashes: the first fails its leg and drains its owned tree, monitors first; the second ends the run. Authoring crash, broken smoke, protected change, upload/canary still end it immediately. FI5 records locked C:/Windows files by name and size, without treating them as coverage gaps.

**Prior crash cause: UNCONFIRMED.** The nested load requests are confirmed: [09427b Load Runtime.log](<D:/IA_Europa/Receipts/09427b/runtime-attempts/Load/01/session/Runtime.log:711>) has requests at lines 711, 713 and 714, all frame 10 and within 29 ms, before the first completed load at line 731. Completions 731/750/769 precede quit requests 732/751/770. [The preserved old driver](<D:/IA_Europa/Receipts/09427c/before-fi/runtime_editor_425.py>) has no busy/terminal guard at lines 122–166; readback calls load_level at 75 and raises on camera mismatch at 104–105; finish at 32–44 can repeat. These were requests already in flight, not retries after the first failure.

The source route supports callback re-entry: EditorLevelLibrary.cpp:615–632 calls LoadMap; FileHelpers.cpp:4213–4219 enables progress; FeedbackContextEditor.cpp:357–369/476/539–543 can tick Slate; SlateApplication.cpp:1503 broadcasts post-tick; PySlate.cpp:126–137 registers Python there. The harness deliberately reproduces re-entry. The exact old progress stack was not captured. The later shutdown access violation is at Runtime.log:783, with unsymbolized Niagara frames at 785–788 after Preparing to exit at 775. That does not establish the Niagara fault’s root cause. [Full cited analysis](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09427c-runtime-authoring-amendment.md:11>).

| Qualification / can-fail | Result / receipt |
|---|---|
| Old rolled camera; zero saves; one exit; keyword-only static mutants; real Rotator signature | [PASS, Python 2.7 author harness attempt 2](<D:/IA_Europa/Receipts/09427c/scratch/author-python27-attempt2/summary.json>) |
| Normalized angles; bad check exits once; no later load; nested callback; first-raise unregister | [PASS, Python 2.7 runtime harness attempt 2](<D:/IA_Europa/Receipts/09427c/scratch/runtime-python27-attempt2/summary.json>) |
| First crash / pass / second crash; FI5 exclusion boundaries | [PASS, FI cases](<D:/IA_Europa/Receipts/09427c/scratch/fi-tests-attempt1/summary.json>) |
| Real streams and missing/duplicate/reordered/unsafe mutations | [PASS, real-file table](<D:/IA_Europa/Receipts/09427c/scratch/real-files-attempt1/summary.json>) |
| FH, runtime scope/argv, pixel/AA, FC, FD, FE, managed files, fixture, session, ledger/tooling and finish | [All covering suites PASS](<D:/IA_Europa/Receipts/09427c/tooling-qualification.json>) |
| Ten stand-in rehearsal groups, including detached monitor, failing then passing leg, ensure versus crash, Content/build/deadline stops, FI3/FI4 | [PASS, final rehearsal](<D:/IA_Europa/Receipts/09427c/scratch/rehearsal-attempt2/summary.json>) |

D2: author harness attempt 1 correctly refused the still-existing real output paths; attempt 2 models only those three paths as absent within its recording stub and verifies their actual hashes unchanged. Runtime Python 2.7 passed twice; the final run adds the post-process position read-back. All other covering suites passed their first run. Both ten-group rehearsals passed. No test saved an asset or launched Unreal. The old runpy-dict launcher mutant still fails. Raw real files and their mutation copies remain distinct; no old FAIL receipt was rewritten.

The standing FH2 emitter requirements remain source-bound. [Emitter table](<D:/IA_Europa/Receipts/09427c/emitter-table.json>) and [real-file/mutation table](<D:/IA_Europa/Receipts/09427c/scratch/real-files-attempt1/summary.json>) include the normal files and the prior crashed Load, whose expected reader result remains FAIL. Every original is hash-equal.

| Reader | Required row / fields | Emitter |
|---|---|---|
| all | *: kind sequence process_frame monotonic_seconds world world_pointer world_name world_type epoch | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:88>) |
| clean_exit startup | probe_start: capture_define control_define shader_define | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:143>) |
| clean_exit | probe_stop: root_release outstanding_screenshot | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:158>) |
| clean_exit | world_cleanup: actor_snapshot session_ended; skip_reason when actor_snapshot=skipped | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:211>) |
| clean_exit | gc_restore_skipped: skip_reason | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:393>) |
| lifecycle admission | world_initialized:  | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:203>) |
| lifecycle admission | world_snapshot: label subsystems; catalogue active_count enabled_pool in Game/PIE | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:294>) |
| lifecycle | world_snapshot.subsystems: class registered_count instances cdo_should_create cdo_allowed cdo_tick_type | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:267>) |
| lifecycle | world_snapshot.subsystems.instances: pointer initialized allowed template | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:276>) |
| lifecycle | world_update_begin: tick_type paused eligible previous_update_unclosed | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:234>) |
| lifecycle | world_update_end: tick_type paused | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:254>) |
| lifecycle | subsystem_audit: audit_line | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:183>) |
| lifecycle | dormant_created: objects hold_frames | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:506>) |
| lifecycle | dormant_created.objects: initialized allowed | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:488>) |
| GC | gc_retained: field_alive array_alive gc_cycles | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:440>) |
| GC | gc_released: field_expired array_expired gc_cycles | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:448>) |
| GC | gc_bench_components: constructed fields_present owned_components_alive engine_major gc_cycles | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:459>) |
| pictures admission lifecycle | command_result: command arguments success skip_restore_control; r.PostProcessAAQuality for prepare; live_fires for successful auto-fire | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:149>) |
| pictures | command_result.prepare: r.PostProcessAAQuality | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:217>) |
| pictures | screenshot_request: path request show_ui high_resolution | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:245>) |
| pictures | screenshot_complete: path request request_world request_epoch request_frame width height written viewport_matches | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:267>) |
| pictures admission | component_state: label actors | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:303>) |
| pictures admission | component_state.actors: identity name hidden components | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:326>) |
| pictures admission | component_state.actors.components: identity name; visible hidden_game main_pass depth_pass cast_shadow cast_hidden_shadow dynamic_shadow static_shadow distance_field dynamic_indirect custom_depth receives_decals ray_tracing stencil forced_lod mesh materials intensity color shadow according to component class; whole actors state compared | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:339>) |
| clean_exit monitor | driver_failure: reason | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:69>) |
| monitor | script_complete: steps | [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:106>) |

The [quarantine plan and per-file receipts](<D:/IA_Europa/Receipts/09427c/quarantine/receipt.json>) preserve all three old packages. [The A6 arming](<D:/IA_Europa/Receipts/09427c/authoring-arming/receipt.json>) has two recipe sessions, no Smoke, fifteen initially absent state/attempt paths, and rejects 09426d and older. No authoring dry run was performed. [The authoring live reading](<D:/IA_Europa/Receipts/09427c/authoring-arming/live-reading.json>) and [actual idle precheck](<D:/IA_Europa/Receipts/09427c/authoring-idle-precheck-01.jsonl>) record the admission wait.

| Recipe | Gate wait s | Start IST | End IST | Duration s | Exit | Result |
|---|---|---|---|---|---|---|
| EA_Core | 72.84348970001156 | 10-30 19:11:46 | 10-30 19:12:23 | 36.614 | 0 | AUTHORING_PASS |
| EA_Travel | 536.2048969999887 | 10-30 19:28:26 | 10-30 19:28:39 | 12.351 | 0 | AUTHORING_PASS |

| Recipe camera | Actual | Normalized | Expected | Tolerance / before-save check |
|---|---|---|---|---|
| EA_Core | {"pitch": -12.0, "roll": 0.0, "yaw": 0.0} | {"pitch": -12.0, "roll": 0.0, "yaw": 0.0} | {"pitch": -12.0, "roll": 0.0, "yaw": 0.0} | ±0.01° / True |
| EA_Travel | {"pitch": -12.0, "roll": 0.0, "yaw": 0.0} | {"pitch": -12.0, "roll": 0.0, "yaw": 0.0} | {"pitch": -12.0, "roll": 0.0, "yaw": 0.0} | ±0.01° / True |

| New output path | Bytes | SHA-256 |
|---|---|---|
| D:\IA_Europa\StylizedParisStreet\Content\EuropaPhaseA\SM_EA_TwoLod.uasset | 137553 | c4fb39761a37edfc145be008add2c4e87cba731e32e238395ae0b35ad1ea6723 |
| D:\IA_Europa\StylizedParisStreet\Content\EuropaPhaseA\EA_Core.umap | 23168 | 3c9c34dce6336ef6907ec489467e21f79295d0a024865b799b7490df5951f59b |
| D:\IA_Europa\StylizedParisStreet\Content\EuropaPhaseA\EA_Travel.umap | 21124 | 6b7c9dd632955d4f353a4f8b3b2c7775013f5705a433d67c554b4f4c4f003909 |

Authoring EY7 evidence a–g is in [authoring-evidence.json](<D:/IA_Europa/Receipts/09427c/authoring-evidence.json>) and [item-by-item assessment](<D:/IA_Europa/Receipts/09427c/authoring-assessment.json>): clean exit/skipped cleanup/one probe_stop/no PIE; exact outputs and 1,458 unchanged other Content files; bound recipe/manifest items; privacy argv and log checks; EditorRuns; C:/E:/5.1 ledger classes; any failed-session quarantine. Every Saved/Intermediate change and each error/ensure candidate is listed. Runtime cold read-back is separate evidence below. The [authoring console](<D:/IA_Europa/Receipts/09427c/authoring-entrypoint-01-console.log>) is retained verbatim.

After both maps passed, [the runtime arming](<D:/IA_Europa/Receipts/09427c/arming/install.json>) bound the new hashes and eighteen initially absent single-use slots. [Retired-arm refusals](<D:/IA_Europa/Receipts/09427c/arming/retired-refusals.json>) reject 09427b and older. [All eighteen throwaway dry runs](<D:/IA_Europa/Receipts/09427c/scratch/dry/run-result.json>) passed without a launch; [their console](<D:/IA_Europa/Receipts/09427c/chain-runtime-dry.log>), [live gate reading](<D:/IA_Europa/Receipts/09427c/live-gate.json>) and [final qualification](<D:/IA_Europa/Receipts/09427c/qualification.json>) are retained. The real-session deadline was 10-31 06:00 IST; a started session could finish.

| Runtime leg | Limit s | Idle / gate wait s | Start IST | End IST | Duration s / exit | Reader |
|---|---|---|---|---|---|---|
| Smoke | 300 | 0.0 / 71.90144270000746 | 10-30 19:48:42 | 10-30 19:48:55 | 12.797 / 0 | PASS |
| Load | 300 | 0.0 / 72.78273510000145 | 10-30 19:50:58 | 10-30 19:51:13 | 15.072 / 0 | PASS |
| WorldsPIE | 600 | 0.0 / 72.51302050000231 | 10-30 19:53:22 | 10-30 19:54:36 | 74.614 / 0 | FAIL: Missing clean probe stop |
| WorldsGame | 300 | 609.256167 / 174.44820670000627 | 10-30 20:08:35 | 10-30 20:09:03 | 27.261 / 125 | FAIL: Nonzero or stopped session |
| GC | 300 | 580.324429 / 71.9363980999915 | 10-30 20:20:55 | 10-30 20:21:02 | 6.665 / 3 | FAIL: Nonzero or stopped session |
| A7_:blinking:positive | 900 | 0.0 / 72.37653120000323 | 10-30 20:23:03 | 10-30 20:23:15 | 12.652 / 125 | FAIL: Nonzero or stopped session |
| A7_:lighting_mismatch:positive | 900 | 0.0 / 71.31207739999809 | 10-30 20:25:09 | 10-30 20:25:34 | 25.023 / 0 | PASS |
| A7_:lod_corruption:positive | 900 | 606.496553 / 72.93839500000468 | 10-30 20:37:45 | 10-30 20:38:13 | 27.522 / 0 | FAIL: Fewer than three valid visible restored events |
| A7_:missing_object:positive | 900 | 625.013975 / 73.54745469999034 | 10-30 20:50:55 | 10-30 20:51:25 | 29.722 / 0 | FAIL: Fewer than three valid visible restored events |
| A7_:blinking:NoApply | 300 | 609.251017 / 72.2823664000025 | 10-30 21:03:46 | 10-30 21:04:13 | 26.845 / 0 | FAIL: Control event invalid under FB3 |
| A7_:lighting_mismatch:NoApply | 300 | 959.314922 / 698.2269285999937 | 10-30 21:33:01 | 10-30 21:33:27 | 25.506 / 0 | FAIL: Control event invalid under FB3 |
| A7_:lod_corruption:NoApply | 300 | 605.650016 / 72.0121000000072 | 10-30 21:45:32 | 10-30 21:45:52 | 20.472 / 0 | PASS |
| A7_:missing_object:NoApply | 300 | 605.338994 / 71.51373730000341 | 10-30 21:57:58 | 10-30 21:58:18 | 20.561 / 0 | PASS |
| A7_:blinking:SkipRestore | 300 | 610.634885 / 71.91371849999996 | 10-30 22:10:31 | 10-30 22:10:55 | 24.471 / 0 | FAIL: Control event invalid under FB3 |
| A7_:lighting_mismatch:SkipRestore | 300 | 6138.530655 / 67.82169520000753 | 10-30 23:55:09 | 10-30 23:55:46 | 36.952 / 125 | FAIL: Nonzero or stopped session |
| A7_:lod_corruption:SkipRestore | 300 | 3939.475582 / 691.2054076000059 | 10-31 01:13:34 | 10-31 01:13:49 | 15.323 / 125 | FAIL: Nonzero or stopped session |
| A7_:missing_object:SkipRestore | 300 | 0.0 / 65.76674020000792 | 10-31 01:15:27 | 10-31 01:20:31 | 303.505 / 125 | FAIL: Nonzero or stopped session |
| Admission | 600 | 588.430844 / 66.38282090000575 | 10-31 01:32:00 | 10-31 01:42:00 | 600.371 / 125 | FAIL: Nonzero or stopped session |

Exact gate samples, root-versus-wrapper exit times, and receipt paths are in [timeline.json](<D:/IA_Europa/Receipts/09427c/timeline.json>). [The runtime console](<D:/IA_Europa/Receipts/09427c/runtime-console.txt>) and [chain console](<D:/IA_Europa/Receipts/09427c/chain-runtime-real.log>) are retained verbatim. Ordinary leg failures retain evidence and proceed; the run’s crash count is 1, with crashed legs ['GC']. No real session was retried.

| Failed runtime leg | Recorded reason | Disposition | Receipt |
|---|---|---|---|
| WorldsPIE | reader FAIL: Missing clean probe stop | reader/driver failure | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/WorldsPIE/01/result.json>) |
| WorldsGame | FH7 free commit below 4 GiB; reader FAIL: Nonzero or stopped session | controlled owned stop | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/WorldsGame/01/result.json>) |
| GC | abnormal editor/game exit; FI4 runtime crash counted; Child exit 3; full argv retained in the result receipt; reader FAIL: Nonzero or stopped session | crash | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/GC/01/result.json>) |
| A7_:blinking:positive | [Errno 13] Permission denied: 'D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-blinking-positive\\01\\session\\probe\\probe.jsonl'; reader FAIL: Nonzero or stopped session | controlled owned stop | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-blinking-positive/01/result.json>) |
| A7_:lod_corruption:positive | reader FAIL: Fewer than three valid visible restored events | reader/driver failure | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lod_corruption-positive/01/result.json>) |
| A7_:missing_object:positive | reader FAIL: Fewer than three valid visible restored events | reader/driver failure | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-missing_object-positive/01/result.json>) |
| A7_:blinking:NoApply | reader FAIL: Control event invalid under FB3 | reader/driver failure | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-blinking-NoApply/01/result.json>) |
| A7_:lighting_mismatch:NoApply | reader FAIL: Control event invalid under FB3 | reader/driver failure | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lighting_mismatch-NoApply/01/result.json>) |
| A7_:blinking:SkipRestore | reader FAIL: Control event invalid under FB3 | reader/driver failure | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-blinking-SkipRestore/01/result.json>) |
| A7_:lighting_mismatch:SkipRestore | [Errno 13] Permission denied: 'D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-lighting_mismatch-SkipRestore\\01\\session\\probe\\probe.jsonl'; reader FAIL: Nonzero or stopped session | controlled owned stop | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lighting_mismatch-SkipRestore/01/result.json>) |
| A7_:lod_corruption:SkipRestore | [Errno 13] Permission denied: 'D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-lod_corruption-SkipRestore\\01\\session\\probe\\probe.jsonl'; reader FAIL: Nonzero or stopped session | controlled owned stop | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lod_corruption-SkipRestore/01/result.json>) |
| A7_:missing_object:SkipRestore | Session limit reached; stopped only the owned editor tree, including crash reporter children.; reader FAIL: Nonzero or stopped session | controlled owned stop | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-missing_object-SkipRestore/01/result.json>) |
| Admission | Session limit reached; stopped only the owned editor tree, including crash reporter children.; reader FAIL: Nonzero or stopped session | controlled owned stop | [raw result](<D:/IA_Europa/Receipts/09427c/runtime-attempts/Admission/01/result.json>) |

The **GC crash is confirmed as a probe construction defect**. [GC Runtime.log](<D:/IA_Europa/Receipts/09427c/runtime-attempts/GC/01/session/Runtime.log:715>) records the abstract-class assertion and the stack at line 727 points to [EuropaPhaseAProbeDriver.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:437>): `NewObject<UObject>(GetTransientPackage())`. Stock [Object.h](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/CoreUObject/Public/UObject/Object.h:49>) marks UObject abstract; [UObjectGlobals.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/CoreUObject/Private/UObject/UObjectGlobals.cpp:2282>) rejects constructing an abstract non-CDO when GIsEditor is false. The leg used `-game`, exited 3 and counted as the first FI4 crash. The owned monitor was stopped and the next leg ran. This native probe was not edited or rebuilt during the job.

| Gate | Generated reader verdict |
|---|---|
| A3 | PASS |
| A5 | FAIL |
| A6 | FAIL |
| A7 | FAIL |
| A8 | FAIL |

A3 saved-value read-back follows; “not observed” means the driver did not reach that value. Full raw/normalized values and individual pass flags are in [Load driver.json](<D:/IA_Europa/Receipts/09427c/runtime-attempts/Load/01/driver.json>).

| Saved value | EA_Core actual | EA_Travel actual | Expected |
|---|---|---|---|
| EA_BlinkCube.label | EA_BlinkCube | EA_BlinkCube | EA_BlinkCube |
| EA_BlinkCube.mesh | /Engine/BasicShapes/Cube.Cube | /Engine/BasicShapes/Cube.Cube | /Engine/BasicShapes/Cube.Cube |
| EA_BlinkCube.mobility | <ComponentMobility.MOVABLE: 2> | <ComponentMobility.MOVABLE: 2> | <ComponentMobility.MOVABLE: 2> |
| EA_BlinkCube.transform | {"location": [600.0, 260.0, 110.0], "scale": [2.0, 2.0, 2.0]} | {"location": [600.0, 260.0, 110.0], "scale": [2.0, 2.0, 2.0]} | {"location": [600, 260, 110], "scale": [2, 2, 2]} |
| EA_Camera.label | EA_Camera | EA_Camera | EA_Camera |
| EA_Camera.transform | {"location": [-550.0, 0.0, 480.0], "scale": [1.0, 1.0, 1.0]} | {"location": [-550.0, 0.0, 480.0], "scale": [1.0, 1.0, 1.0]} | {"location": [-550, 0, 480], "scale": [1, 1, 1]} |
| EA_Floor.label | EA_Floor | EA_Floor | EA_Floor |
| EA_Floor.mesh | /Engine/BasicShapes/Cube.Cube | /Engine/BasicShapes/Cube.Cube | /Engine/BasicShapes/Cube.Cube |
| EA_Floor.mobility | <ComponentMobility.MOVABLE: 2> | <ComponentMobility.MOVABLE: 2> | <ComponentMobility.MOVABLE: 2> |
| EA_Floor.transform | {"location": [700.0, 0.0, -50.0], "scale": [18.0, 18.0, 1.0]} | {"location": [700.0, 0.0, -50.0], "scale": [18.0, 18.0, 1.0]} | {"location": [700, 0, -50], "scale": [18, 18, 1]} |
| EA_HideCube.label | EA_HideCube | EA_HideCube | EA_HideCube |
| EA_HideCube.mesh | /Engine/BasicShapes/Cube.Cube | /Engine/BasicShapes/Cube.Cube | /Engine/BasicShapes/Cube.Cube |
| EA_HideCube.mobility | <ComponentMobility.MOVABLE: 2> | <ComponentMobility.MOVABLE: 2> | <ComponentMobility.MOVABLE: 2> |
| EA_HideCube.transform | {"location": [600.0, -260.0, 110.0], "scale": [2.0, 2.0, 2.0]} | {"location": [600.0, -260.0, 110.0], "scale": [2.0, 2.0, 2.0]} | {"location": [600, -260, 110], "scale": [2, 2, 2]} |
| EA_LOD.label | EA_LOD | EA_LOD | EA_LOD |
| EA_LOD.mesh | /Game/EuropaPhaseA/SM_EA_TwoLod.SM_EA_TwoLod | /Game/EuropaPhaseA/SM_EA_TwoLod.SM_EA_TwoLod | /Game/EuropaPhaseA/SM_EA_TwoLod.SM_EA_TwoLod |
| EA_LOD.mobility | <ComponentMobility.MOVABLE: 2> | <ComponentMobility.MOVABLE: 2> | <ComponentMobility.MOVABLE: 2> |
| EA_LOD.transform | {"location": [800.0, 0.0, 160.0], "scale": [3.0, 3.0, 3.0]} | {"location": [800.0, 0.0, 160.0], "scale": [3.0, 3.0, 3.0]} | {"location": [800, 0, 160], "scale": [3, 3, 3]} |
| EA_Light.label | EA_Light | EA_Light | EA_Light |
| EA_Light.mobility | <ComponentMobility.MOVABLE: 2> | <ComponentMobility.MOVABLE: 2> | <ComponentMobility.MOVABLE: 2> |
| EA_Light.transform | {"location": [450.0, 0.0, 650.0], "scale": [1.0, 1.0, 1.0]} | {"location": [450.0, 0.0, 650.0], "scale": [1.0, 1.0, 1.0]} | {"location": [450, 0, 650], "scale": [1, 1, 1]} |
| attenuation_radius | 1800.0 | 1800.0 | 1800.0 |
| camera_rotation | {"normalized": [-12.0, 0.0, 0.0], "raw": [-12.0, 0.0, 0.0]} | {"normalized": [-12.0, 0.0, 0.0], "raw": [-12.0, 0.0, 0.0]} | {"pitch": -12.0, "roll": 0.0, "tolerance_degrees": 0.01, "yaw": 0.0} |
| cast_shadows | True | True | True |
| default_game_mode | /Script/Engine.GameModeBase | /Script/Engine.GameModeBase | /Script/Engine.GameModeBase |
| field_of_view | 50.0 | 50.0 | 50.0 |
| intensity | 5000.0 | 5000.0 | 5000.0 |
| lod_count | 2 | 2 | >=2 |
| map_loaded | True | True | True |
| player_start.location | [-550.0, 0.0, 500.0] | [-550.0, 0.0, 500.0] | [-550, 0, 500] |
| player_start.rotation | [0.0, 0.0, 0.0] | [0.0, 0.0, 0.0] | [0, 0, 0] |
| player_start_count | 1 | 1 | 1 |
| postprocess.auto_exposure_max_brightness | 1.0 | 1.0 | 1.0 |
| postprocess.auto_exposure_min_brightness | 1.0 | 1.0 | 1.0 |
| postprocess.motion_blur_amount | 0.0 | 0.0 | 0.0 |
| postprocess.override_auto_exposure_max_brightness | True | True | True |
| postprocess.override_auto_exposure_min_brightness | True | True | True |
| postprocess.override_motion_blur_amount | True | True | True |
| postprocess_count | 1 | 1 | 1 |
| postprocess_location | [0.0, 0.0, 0.0] | [0.0, 0.0, 0.0] | [0, 0, 0] |
| tags | ["EA_BlinkCube", "EA_Camera", "EA_Floor", "EA_HideCube", "EA_LOD", "EA_Light"] | ["EA_BlinkCube", "EA_Camera", "EA_Floor", "EA_HideCube", "EA_LOD", "EA_Light"] | ["EA_BlinkCube", "EA_Camera", "EA_Floor", "EA_HideCube", "EA_LOD", "EA_Light"] |
| unbound | True | True | True |

| A7 leg / anomaly | Attempted | Valid windows | Applied | Reverted | Refused | Invalid | Counted | Reader |
|---|---|---|---|---|---|---|---|---|
| A7_:blinking:positive | 0 | None | 0 | 0 | 0 | None | 0 | FAIL |
| A7_:lighting_mismatch:positive | 8 | 5 | 6 | 6 | 2 | 1 | 5 | PASS |
| A7_:lod_corruption:positive | 8 | 6 | 6 | 6 | 2 | 0 | 0 | FAIL |
| A7_:missing_object:positive | 8 | 6 | 6 | 6 | 2 | 0 | 0 | FAIL |
| A7_:blinking:NoApply | 1 | 0 | 0 | 0 | 1 | 1 | 0 | FAIL |
| A7_:lighting_mismatch:NoApply | 1 | 0 | 0 | 0 | 1 | 1 | 0 | FAIL |
| A7_:lod_corruption:NoApply | 1 | 1 | 0 | 0 | 1 | 0 | 0 | PASS |
| A7_:missing_object:NoApply | 1 | 1 | 0 | 0 | 1 | 0 | 0 | PASS |
| A7_:blinking:SkipRestore | 1 | 0 | 1 | 0 | 0 | 1 | 0 | FAIL |
| A7_:lighting_mismatch:SkipRestore | 0 | None | 0 | 0 | 0 | None | 0 | FAIL |
| A7_:lod_corruption:SkipRestore | 0 | None | 0 | 0 | 0 | None | 0 | FAIL |
| A7_:missing_object:SkipRestore | 0 | None | 0 | 0 | 0 | None | 0 | FAIL |

Where the picture reader did not complete, attempted/applied/reverted/refused counts come only from retained command_result rows. Valid/invalid windows are unassessed (None), and zero events are qualified. No failed session is re-labelled by this accounting.

| A7 picture reader outcomes | Pixel-check outcomes before window validity | Invalid-window reasons |
|---|---|---|
| A7_:lighting_mismatch:positive | {"Pixel check passed": 6} | {"observation coverage missing": 1} |
| A7_:lod_corruption:positive | {"No visible active picture": 6} | {} |
| A7_:missing_object:positive | {"Unqualified clean-frame noise": 6} | {} |
| A7_:blinking:NoApply | {"No visible active picture": 1} | {"observation coverage missing": 1} |
| A7_:lighting_mismatch:NoApply | {"No visible active picture": 1} | {"observation coverage missing": 1} |
| A7_:lod_corruption:NoApply | {"No visible active picture": 1} | {} |
| A7_:missing_object:NoApply | {"No visible active picture": 1} | {} |
| A7_:blinking:SkipRestore | {"Unqualified clean-frame noise": 1} | {"observation coverage missing": 1} |

Lighting mismatch has six pixel-check passes, but only five counted events: the sixth lacks observation coverage. The derived outcome label Counted is displayed here as Pixel check passed to separate it from the final counted-event field. The source receipts and five-event qualification are unchanged.

The [contact-sheet index](<D:/IA_Europa/Receipts/09427c/contact-sheet-index.json>) lists 1 counted-event sheets and their original 1280×720 screenshots. [Per-picture AA echoes](<D:/IA_Europa/Receipts/09427c/all-picture-aa.json>) retain effective r.PostProcessAAQuality; missing/nonzero echoes never count. Human input/foreign GPU contamination invalidates an event window. NoApply, SkipRestore, invalid-target and unsupported-ID outcomes remain in their generated readers. Synthetic rehearsal pictures are excluded. The owner’s eye check remains separate.

The same index additionally lists 3 diagnostic sheets made only from retained engine screenshots, visibly marked NOT COUNTED. Their failed reader verdicts and original PNGs are unchanged. [Diagnostic-sheet receipt](<D:/IA_Europa/Receipts/09427c/diagnostic-contact-sheets/receipt.json>). Unavailable complete triplets: [{"anomaly": "blinking", "diagnostic_fallback": "SkipRestore control only; restoration was deliberately skipped; no positive event is qualified", "raw_png_count": 2, "reader_result": "FAIL", "reason": "Positive leg has no complete recorded before/during/after triplet"}].

| Session | Monitor PID / parent | Start UTC | Recorded command line | Canary environment |
|---|---|---|---|---|
| EA_Core | 44044 / 37492 | 2026-10-30T13:41:48.288617+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=524 -WRITE=536 -MONITOR=32272 -abslog=D:/IA_Europa/Receipts/09427c/fixture-attempts/EA_Core/01/session/CrashClient.log -ConcertServer="1A433F05488BC3B9C865259F45A27CF7" -RespawnedInstance | absent |
| EA_Travel | 40476 / 808 | 2026-10-30T13:58:27.183296+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=232 -WRITE=512 -MONITOR=33448 -abslog=D:/IA_Europa/Receipts/09427c/fixture-attempts/EA_Travel/01/session/CrashClient.log -ConcertServer="E1012A44488DF43D1EF13AAA983BF95A" -RespawnedInstance | absent |
| Smoke | 43560 / 3360 | 2026-10-30T14:18:43.291583+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=508 -WRITE=520 -MONITOR=40272 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/Smoke/01/session/CrashClient.log -ConcertServer="A0B4AB26440FFA1FA97915BDFBA51A4D" -RespawnedInstance | absent |
| Load | 38160 / 3208 | 2026-10-30T14:20:58.526554+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=500 -WRITE=512 -MONITOR=33956 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/Load/01/session/CrashClient.log -ConcertServer="94EE9BFA43CE094FB9DDFAB4523DA82E" -RespawnedInstance | absent |
| WorldsPIE | 36004 / 38160 | 2026-10-30T14:23:22.583122+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=520 -WRITE=532 -MONITOR=35340 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/WorldsPIE/01/session/CrashClient.log -ConcertServer="C9498AC1417A47ED330C69866D5CD2BE" -RespawnedInstance | absent |
| WorldsGame | 28696 / 23160 | 2026-10-30T14:38:36.301885+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=576 -WRITE=588 -MONITOR=6420 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/WorldsGame/01/session/CrashClient.log -ConcertServer="8D8D99F1408C7275FC0A8495F5D61567" -RespawnedInstance | absent |
| GC | 42360 / 7804 | 2026-10-30T14:50:55.717798+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=504 -WRITE=516 -MONITOR=9556 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/GC/01/session/CrashClient.log -ConcertServer="8C0311BF42E9FAF0E3142DB346C11224" -RespawnedInstance | absent |
| A7_:blinking:positive | 21324 / 38192 | 2026-10-30T14:53:03.320170+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=228 -WRITE=88 -MONITOR=36884 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-blinking-positive/01/session/CrashClient.log -ConcertServer="E5E8E7D74CCF1A1967DB8ABFC7139255" -RespawnedInstance | absent |
| A7_:lighting_mismatch:positive | 36048 / 24272 | 2026-10-30T14:55:09.585852+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=520 -WRITE=532 -MONITOR=12136 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lighting_mismatch-positive/01/session/CrashClient.log -ConcertServer="FDD0E6674D3909A78453FCAB21BBBEDE" -RespawnedInstance | absent |
| A7_:lod_corruption:positive | 33692 / 33188 | 2026-10-30T15:07:46.055954+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=580 -WRITE=592 -MONITOR=43996 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lod_corruption-positive/01/session/CrashClient.log -ConcertServer="B732F0924D5FC18C2A57F9862D43DDEB" -RespawnedInstance | absent |
| A7_:missing_object:positive | 32780 / 22904 | 2026-10-30T15:20:55.632232+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=548 -WRITE=560 -MONITOR=34824 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-missing_object-positive/01/session/CrashClient.log -ConcertServer="7B0B48D04A9694C53E1C7DB8C9F77AD9" -RespawnedInstance | absent |
| A7_:blinking:NoApply | 17916 / 41992 | 2026-10-30T15:33:46.750397+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=520 -WRITE=532 -MONITOR=41236 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-blinking-NoApply/01/session/CrashClient.log -ConcertServer="501969BE4587B7EB6BA3DE83FC82CE1D" -RespawnedInstance | absent |
| A7_:lighting_mismatch:NoApply | 3956 / 35716 | 2026-10-30T16:03:01.954747+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=528 -WRITE=540 -MONITOR=17704 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lighting_mismatch-NoApply/01/session/CrashClient.log -ConcertServer="0D551E114AC165F768F6CEA041F0F9AB" -RespawnedInstance | absent |
| A7_:lod_corruption:NoApply | 26232 / 48096 | 2026-10-30T16:15:32.481683+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=512 -WRITE=524 -MONITOR=36456 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lod_corruption-NoApply/01/session/CrashClient.log -ConcertServer="0947156845C1EB58A7954C8C50A9FDFB" -RespawnedInstance | absent |
| A7_:missing_object:NoApply | 46144 / 48224 | 2026-10-30T16:27:58.535788+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=500 -WRITE=512 -MONITOR=19344 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-missing_object-NoApply/01/session/CrashClient.log -ConcertServer="BC094EBD498C38922F813F8F12094D53" -RespawnedInstance | absent |
| A7_:blinking:SkipRestore | 19968 / 14408 | 2026-10-30T16:40:31.413495+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=88 -WRITE=536 -MONITOR=45272 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-blinking-SkipRestore/01/session/CrashClient.log -ConcertServer="73D1787C477FAE907FF4E2B44F055178" -RespawnedInstance | absent |
| A7_:lighting_mismatch:SkipRestore | 4460 / 17604 | 2026-10-30T18:25:11.552285+00:00 | "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=188 -WRITE=244 -MONITOR=17604 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lighting_mismatch-SkipRestore/01/session/CrashClient.log -ConcertServer="0E0C97724247062DB863368E030C6DFB" | absent |
| A7_:lighting_mismatch:SkipRestore | 30440 / 4460 | 2026-10-30T18:25:11.587230+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=188 -WRITE=244 -MONITOR=17604 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lighting_mismatch-SkipRestore/01/session/CrashClient.log -ConcertServer="0E0C97724247062DB863368E030C6DFB" -RespawnedInstance | absent |
| A7_:lod_corruption:SkipRestore | 20376 / 32348 | 2026-10-30T19:43:34.816449+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=504 -WRITE=516 -MONITOR=27332 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lod_corruption-SkipRestore/01/session/CrashClient.log -ConcertServer="D6904D934F9FE8AAD810ED82EB8CD519" -RespawnedInstance | absent |
| A7_:missing_object:SkipRestore | 31008 / 9172 | 2026-10-30T19:45:28.284942+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=528 -WRITE=540 -MONITOR=5632 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-missing_object-SkipRestore/01/session/CrashClient.log -ConcertServer="78CC8DD54BA43DFAC2FAFF935B8F5191" -RespawnedInstance | absent |
| Admission | 24752 / 23080 | 2026-10-30T20:02:00.673239+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=500 -WRITE=512 -MONITOR=2104 -abslog=D:/IA_Europa/Receipts/09427c/runtime-attempts/Admission/01/session/CrashClient.log -ConcertServer="F1C34E3D418DB8BF710DF3B3F4C3D3A4" -RespawnedInstance | absent |

FH5 exact -MONITOR ownership, parent/start/argv and environment-name observations are in [adopted-monitors.json](<D:/IA_Europa/Receipts/09427c/adopted-monitors.json>) and the authoring assessment. [WindowsPlatformCrashContext.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Windows/WindowsPlatformCrashContext.cpp:442>) passes the owned PID; lines 493–536 explain respawn and the temporary PID file. Owned stops reach monitors first; normal-exit grace is 60 seconds. New crash/ensure report folders are associated with their leg in [session-evidence.json](<D:/IA_Europa/Receipts/09427c/session-evidence.json>).

| Session | Exit | Skipped cleanup / probe_stop | Upload check | New report folders |
|---|---|---|---|---|
| EA_Core | 0 | 2 / 1 | PASS | see authoring evidence |
| EA_Travel | 0 | 1 / 1 | PASS | see authoring evidence |
| Smoke | 0 | 1 / 1 | PASS | [] |
| Load | 0 | 1 / 1 | PASS | [] |
| WorldsPIE | 0 | 2 / 0 | PASS | [] |
| WorldsGame | 125 | 3 / 0 | PASS | [] |
| GC | 3 | 1 / 0 | PASS | [] |
| A7_:blinking:positive | 125 | 1 / 0 | PASS | [] |
| A7_:lighting_mismatch:positive | 0 | 2 / 1 | PASS | [] |
| A7_:lod_corruption:positive | 0 | 2 / 1 | PASS | [] |
| A7_:missing_object:positive | 0 | 2 / 1 | PASS | [] |
| A7_:blinking:NoApply | 0 | 2 / 1 | PASS | [] |
| A7_:lighting_mismatch:NoApply | 0 | 2 / 1 | PASS | [] |
| A7_:lod_corruption:NoApply | 0 | 2 / 1 | PASS | [] |
| A7_:missing_object:NoApply | 0 | 2 / 1 | PASS | [] |
| A7_:blinking:SkipRestore | 0 | 2 / 1 | PASS | [] |
| A7_:lighting_mismatch:SkipRestore | 125 | 1 / 0 | PASS | [] |
| A7_:lod_corruption:SkipRestore | 125 | 1 / 0 | PASS | [] |
| A7_:missing_object:SkipRestore | 125 | 1 / 0 | PASS | [] |
| Admission | 125 | 1 / 0 | PASS | [] |

Every executed editor/game argv is retained with EU4 bSendUnattendedBugReports=False and FC4 bSendUsageData=False. The same upload-sign reader checks Runtime.log and CrashClient.log, including -game sessions; [EngineAnalytics.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/EngineAnalytics.cpp:108>) supports the game/editor analytics distinction. This is argv/log evidence, not packet capture. All errors and ensures, with engine/host/plugin-or-probe classification, are retained in the session evidence and authoring assessment. Raw runtime evidence was hashed at session end and verified unchanged; no FAIL was relabelled.

| Session | UBT PID | Recorded argv |
|---|---|---|
| EA_Core | 43340 | "E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms |
| EA_Travel | 27368 | "E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms |
| A7_:lod_corruption:NoApply | 23168 | "E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms |
| A7_:blinking:SkipRestore | 19172 | "E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms |
| A7_:lod_corruption:SkipRestore | 3804 | "E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms |

Compiler/linker/UHT observations: 0. [Protected closeout](<D:/IA_Europa/Receipts/09427c/final-state.json>) confirms unchanged protected Content/config/descriptors/binaries, the new fixture hashes, both prior quarantine sets, all kept real probe files, and marker 31124. [Saved/Intermediate changes](<D:/IA_Europa/Receipts/09427c/managed-changes.json>) and the authoring evidence enumerate FF2/FG2/FC5 changes. [Executable/DLL hashes](<D:/IA_Europa/Receipts/09427c/binary-hashes.json>) and [archive inventory](<D:/IA_Europa/Receipts/09427c/archive-image-inventory.json>) list all image hashes and receipt archives; no build archive was created. Bench switches were process-local and ended with their sessions.

The [ledger summary](<D:/IA_Europa/Receipts/09427c/ledger-summary.json>) records 7 operations, 0 real pauses and 0 root attributions. Charged C:/E:/5.1 growth is {'C': 40000666, 'E': 0, 'engine51': 0} bytes against 250,000,000 / 50,000,000 / 100,000,000-byte limits. Foreign activity was recorded or waited out; foreign root contents were not opened.

EU1 command audit: {'boundary_receipt_lock_refusal_before_child': 2, 'direct_read_only_deviation': 1, 'filtered_entry': 507}; 58 edit events; 0 pause violations. [The full audit](<D:/IA_Europa/Receipts/09427c/command-audit-final.json>). The single direct read-only bootstrap deviation read the previous wrapper. Wrapper receipt-lock and ambiguous-argument refusals occurred before any child started; the reads were repeated sequentially or with corrected arguments. The audit enumerates each refusal. There was no direct write-capable shell command.

Other retained orchestration deviations: an early setup-ledger begin collided with the still-finishing entry inventory and reserved a refused folder; a fresh phase began only after entry finished. A documentation helper needed explicit UTF-8. A suite-progress writer’s exclusive-create collision stopped orchestration after the author failure/runtime pass; remaining suites then ran once. A patch-context rejection and read-only missing-path errors changed no protected file. One filtered PowerShell process inspection failed to parse because of native argument quoting; the file-based inspection then passed. A report-only helper’s extra parenthesis was caught and corrected before execution. No real retry, third failed harness attempt, native build or post-run reader repair occurred.

The canary positive control was present and hash-matched, then removed at child boundaries. The final scan from the launch second is **GAP**: 3235 files, 1729316311 bytes, zero matches and 4 unreadable paths outside the tested exclusions. 53 FE2/FI5 files are recorded by name, size and class under C:/Windows (event logs or OS sharing/lock errors). Readable files and decoded gzip/ZIP receipts are scanned. No full EK1 scan ran. [Final canary receipt](<D:/IA_Europa/Receipts/09427c/canary-final.json>). The [initial exit scan](<D:/IA_Europa/Receipts/09427c/canary-exit.json>) is retained unchanged.

| Unclassified canary read gap | Bytes | Windows error |
|---|---|---|
| C:\Windows\System32\SMI\Store\Machine\SCHEMA.DAT{<uuid>}.TM.blf.cnpf | 4096 | 5 |
| C:\Windows\System32\SMI\Store\Machine\SCHEMA.DAT{<uuid>}.TMContainer00000000000000000001.regtrans-ms.cnpf | 4096 | 5 |
| C:\Windows\System32\SMI\Store\Machine\SCHEMA.DAT{<uuid>}.TMContainer00000000000000000002.regtrans-ms.cnpf | 4096 | 5 |
| C:\Windows\System32\winevt\Logs\Microsoft-Windows-CodeIntegrity%4Verbose.etl | 2097152 | 5 |

These reads returned access denied (5), including after a shared-read attempt. The tested classifier covers access-denied .evtx files in the event-log folder and Windows sharing/lock errors 32/33. The .etl and SMI transaction-file denials fall outside that implementation; OS-held locks were not established for them. No exclusion was widened after the run and no GAP receipt was relabelled. Reporting helpers were adjusted only to retain and disclose this result.

| EN5 item / source | Application |
|---|---|
| [Documented stock behavior](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:54>) | Narrow source-cited handling only: Rotator native make, unsaved-map/save ordering, Slate re-entry, monitor respawn, analytics and managed files. No new uncited waiver. |
| [No blocked names/secrets](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:61>) | EL1/EL2/EM2 boundary receipts, nonvacuous canary control and scan; readable monitor environments recorded by name. |
| [No loss of base variables](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:62>) | Nonblocked values preserved by hashes; only approved D: routing and per-process overrides. |
| [Allowed writes](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:63>) | Owned D: scope and EA4/EG1/ET3 classes; exactly FI2’s three Content paths changed; FF2/FG2/FC5 managed changes enumerated. |
| [Protected-state hard STOP](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:64>) | EA8 entry/exit and protected hash comparisons pass; binary manifests unchanged. |
| [Engine-tree hard STOP](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:68>) | Only admitted engine-managed classes; marker 31124 unchanged; no 5.1 execution or source change. |
| [Exposure hard STOP](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:69>) | No canary hit or upload sign; only the explicitly ruled FE2/FI5 unreadable classes excluded. |
| [Machine/security/global settings](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:70>) | None changed; foreign processes untouched; no persistent config edit. |
| [Launch restriction](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:71>) | Only both armed kits launch their declared sessions; FD2/FE3 owned-child rules retained; no cook, commandlet, UAT, packaging or native build. |
| [Floor rule](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:72>) | EV3/EY4/ED1 start/warn/floor lines; EU2/EZ2/FA6 foreign waits; FH7 8/6 GiB admission and below-4 GiB session stop. |
| [Explicit FI amendments](<D:/IntrusiveAnomalies/_reviews/094-27b-ruling.md:94>) | First runtime crash continues after owned cleanup; second/authoring crash, broken smoke, protected change or upload/canary ends the run. No manual verdict relabel. |
| [FI6 / D2 / W4](<D:/IntrusiveAnomalies/_reviews/094-27b-ruling.md:117>) | Qualified before launches; fixed window and single-use slots; actual commit dates; no push or tags. |

| Local feat/europa-port commit | Purpose |
|---|---|
| 7073697ba95c2bd4c238426b08d137e910bd9ec0 | Qualified FI tooling, A5 manifest and dated prediction |
| 2e2766f1e3d134e58125ba24c8665d44ff46e8e7 | Receipts, results, journal, gotchas and status; W4 dates in documentation-commit.json |

VERDICT: RED | Two authoring sessions; 18 runtime sessions; A3 PASS, A5 FAIL, A6 FAIL, A7 FAIL, A8 FAIL; evidence retained; no push.
