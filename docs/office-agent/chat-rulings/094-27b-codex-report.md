<!-- Collected by Claude Code (mailbox brief 094-27b) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md at 2026-10-30 after Codex exited (thread 01a14334, relay tries 1, exit 0, duration 16,949 s); body below is byte-identical, sha256 BD3976DD906E0EC6FB36949990D1DAE48BDB5625E9EA9936D53E6581C6ABD63A, 37172 bytes -->
094-27b is **RED**. Smoke passed. A3 Load exited 3 after the driver reported "Saved camera rotation differs", followed by an access violation with Niagara at the top of the engine stack. FH3 stopped the remaining 16 legs; no A7 pictures were produced. The crash cause remains unresolved. FH2–FH8 fixes passed real-file mutations, Python 2.7 and eight rehearsal cases; both 18-leg dry runs passed. Real sessions: 2; runtime legs started: 1. Run result STOP. Gates: A3 FAIL, A5 NOT_RUN, A6 NOT_RUN, A7 NOT_RUN, A8 NOT_RUN. Counted-event contact sheets: 0. Stop reason: abnormal editor/game exit. Raw evidence and reader verdicts are retained; no verdict was changed by hand. Content, config, descriptors, bound artifacts and marker 31124 remain equal. No build, push or tags.

The final canary scan also has a coverage gap: three locked Windows Update database/log files could not be read. No canary match was found in scanned files. The earlier scan passed; the final gap remains recorded and is not treated as a pass.

The armed tooling commit is `ecfb3ceee40aed85e256624d898fb741a25eb539`; the final feature commit is `0a02987bd2baa32ad9bebfd854bba2c9e136c452`, with a clean worktree. The [committed receipt index](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09427b-runtime-receipts.json>) binds 2096 immutable files. All evidence is under `D:/IA_Europa/Receipts/09427b`. No real retry, manual verdict change, native build, push or tag occurred.

The [EA8 entry proof](<D:/IA_Europa/Receipts/09427b/P9/entry/summary.json>) and [EA8 exit proof](<D:/IA_Europa/Receipts/09427b/P9/exit/summary.json>) both passed against P9 v2. Each compared 472553 inventory entries, admitted 1023 EG1 entries, and hashed 23033 binary/config files (104794989456 bytes). Protected git and source manifests, including m51, remained equal. The [entry review](<D:/IA_Europa/Receipts/09427b/prior-results-entry.json>) of 094-27 found no protected change, crash or upload. P9 v2 was not edited.

| Drive | Entry free GiB | Final free GiB | Start / warning / floor GiB |
|---|---|---|---|
| C | 28.17 | 28.19 | 13 / 11 / 10 |
| D | 57.92 | 53.95 | 40 / 30 / 25 |
| E | 289.67 | 286.51 | 15 / 12 / 10 |

ED3: VS Community 2022 17.12.35707.178; Build Tools 2017 15.9.34031.82; MSVC 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023; SDK 10.0.17763.0 and 10.0.22621.0. No 14.44, compile or export ran. [Toolchains](<D:/IA_Europa/Receipts/09427b/toolchains.json>).

The [dated FH design amendment and prediction](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09427b-runtime-amendment.md>) records the changes: FH2 reads emitted cleanup rows and checks kept real files; FH3 continues after ordinary leg failures; FH4 hashes raw evidence at session end; FH5 adopts crash monitors by exact owned PID; FH6 retains permitted -unattended; FH7 uses 8 GiB commit / 6 GiB physical admission and a below-4-GiB commit stop; FH8 fixes the close-time collector race and requires a zero effective AA echo for each counted picture. FG2 admits only the UE4CC crash-client config INI class. Other protected paths and the existing pixel thresholds remain bound.

The reader emitter table follows. Native paths are in the bound host probe, except where noted. [Machine-readable emitter table](<D:/IA_Europa/Receipts/09427b/emitter-table.json>).

| Reader | Required row and fields | Emitter |
|---|---|---|
| all | *: kind sequence process_frame monotonic_seconds world world_pointer world_name world_type epoch | [EuropaPhaseAWorldProbe.cpp:88–101](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:88>) |
| clean_exit startup | probe_start: capture_define control_define shader_define | [EuropaPhaseAWorldProbe.cpp:143–155](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:143>) |
| clean_exit | probe_stop: root_release outstanding_screenshot | [EuropaPhaseAWorldProbe.cpp:158–180](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:158>) |
| clean_exit | world_cleanup: actor_snapshot session_ended; skip_reason when actor_snapshot=skipped | [EuropaPhaseAWorldProbe.cpp:211–225](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:211>) |
| clean_exit | gc_restore_skipped: skip_reason | [EuropaPhaseAProbeDriver.cpp:393–403](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:393>) |
| lifecycle admission | world_initialized:  | [EuropaPhaseAWorldProbe.cpp:203–208](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:203>) |
| lifecycle admission | world_snapshot: label subsystems; catalogue active_count enabled_pool in Game/PIE | [EuropaPhaseAWorldProbe.cpp:294–359](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:294>) |
| lifecycle | world_snapshot.subsystems: class registered_count instances cdo_should_create cdo_allowed cdo_tick_type | [EuropaPhaseAWorldProbe.cpp:267–290](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:267>) |
| lifecycle | world_snapshot.subsystems.instances: pointer initialized allowed template | [EuropaPhaseAWorldProbe.cpp:276–283](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:276>) |
| lifecycle | world_update_begin: tick_type paused eligible previous_update_unclosed | [EuropaPhaseAWorldProbe.cpp:234–251](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:234>) |
| lifecycle | world_update_end: tick_type paused | [EuropaPhaseAWorldProbe.cpp:254–261](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:254>) |
| lifecycle | subsystem_audit: audit_line | [EuropaPhaseAWorldProbe.cpp:183–188](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:183>) |
| lifecycle | dormant_created: objects hold_frames | [EuropaPhaseAProbeDriver.cpp:506–528](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:506>) |
| lifecycle | dormant_created.objects: initialized allowed | [EuropaPhaseAProbeDriver.cpp:488–502](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:488>) |
| GC | gc_retained: field_alive array_alive gc_cycles | [EuropaPhaseAProbeDriver.cpp:440–447](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:440>) |
| GC | gc_released: field_expired array_expired gc_cycles | [EuropaPhaseAProbeDriver.cpp:448–456](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:448>) |
| GC | gc_bench_components: constructed fields_present owned_components_alive engine_major gc_cycles | [EuropaPhaseAProbeDriver.cpp:459–482](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:459>) |
| pictures admission lifecycle | command_result: command arguments success skip_restore_control; r.PostProcessAAQuality for prepare; live_fires for successful auto-fire | [EuropaPhaseAProbeDriver.cpp:149–241](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:149>) |
| pictures | command_result.prepare: r.PostProcessAAQuality | [EuropaPhaseAProbeDriver.cpp:217–241](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:217>) |
| pictures | screenshot_request: path request show_ui high_resolution | [EuropaPhaseAProbeDriver.cpp:245–264](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:245>) |
| pictures | screenshot_complete: path request request_world request_epoch request_frame width height written viewport_matches | [EuropaPhaseAProbeDriver.cpp:267–298](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:267>) |
| pictures admission | component_state: label actors | [EuropaPhaseAProbeDriver.cpp:303–390](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:303>) |
| pictures admission | component_state.actors: identity name hidden components | [EuropaPhaseAProbeDriver.cpp:326–389](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:326>) |
| pictures admission | component_state.actors.components: identity name; visible hidden_game main_pass depth_pass cast_shadow cast_hidden_shadow dynamic_shadow static_shadow distance_field dynamic_indirect custom_depth receives_decals ray_tracing stencil forced_lod mesh materials intensity color shadow according to component class; whole actors state compared | [EuropaPhaseAProbeDriver.cpp:339–382](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:339>) |
| clean_exit monitor | driver_failure: reason | [EuropaPhaseAProbeDriver.cpp:69–100](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:69>) |
| monitor | script_complete: steps | [EuropaPhaseAProbeDriver.cpp:106–111](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:106>) |

The audit payload fields op/instance/class/world/frame/initialized/template originate in [AnomalyTickableWorldSubsystem.cpp](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Compat/AnomalyTickableWorldSubsystem.cpp:80>). Startup log strings originate in AnomalyInjectorModule.cpp:15, AnomalyCaptureModule.cpp:17, AnomalyControlServerModule.cpp:14, AnomalyShadersModule.cpp:25 and AnomalyBenchModule.cpp:504. Execution/driver/monitor fields are emitted by the corresponding Python helpers, not claimed as probe fields. PNG pixels and dimensions are checked from actual engine output. FH2(d) leaves previously unseen runtime row kinds dependent on these emitter citations until a real leg writes them.

| Kept real probe | Rows | Observed reader checks | Mutations |
|---|---|---|---|
| D:\IA_Europa\Receipts\09426c\fixture-attempts\EA_Core\01 | 37 | Cleanup, startup and observed schemas PASS; unseen runtime kinds N/A | 23 |
| D:\IA_Europa\Receipts\09426d\fixture-attempts\EA_Core\01 | 37 | Cleanup, startup and observed schemas PASS; unseen runtime kinds N/A | 23 |
| D:\IA_Europa\Receipts\09426d\fixture-attempts\EA_Travel\01 | 30 | Cleanup, startup and observed schemas PASS; unseen runtime kinds N/A | 23 |
| D:\IA_Europa\Receipts\09427\runtime-attempts\Smoke\01 | 41 | Cleanup, startup and observed schemas PASS; unseen runtime kinds N/A | 23 |
| D:\IA_Europa\Receipts\09426c\fixture-attempts\Smoke\01 | 25 | Cleanup, startup and observed schemas PASS; unseen runtime kinds N/A | 23 |

All five original probe files remain hash-equal. All 115 mutated copies fail: missing, duplicate, reordered and unsafe-stop rows, outstanding screenshot at stop, and missing emitted fields. The exact previously rejected 09427 Smoke files pass the complete corrected Smoke reader on a copied fixture. [Real-file test table and hashes](<D:/IA_Europa/Receipts/09427b/scratch/real-files-attempt1/summary.json>). No old FAIL receipt was rewritten.

| Can-fail / suite | Result and receipt |
|---|---|
| Real-row and unsafe-stop mutations | [PASS, 115 rejected mutants](<D:/IA_Europa/Receipts/09427b/scratch/real-files-attempt1/summary.json>) |
| FH3 continuation, retained hard-stop causes, crash/ensure, monitor PID, memory and close race | [PASS, 35 checks](<D:/IA_Europa/Receipts/09427b/scratch/fh-tests-attempt2/summary.json>) |
| Runtime scope, old arming, argv, FD2/FE3, deadline and Content | [PASS, 121 checks](<D:/IA_Europa/Receipts/09427b/scratch/runtime-tests-attempt1/summary.json>) |
| Pixel/control/input and missing or nonzero AA echo | [PASS, 22 checks](<D:/IA_Europa/Receipts/09427b/scratch/reader-tests-attempt1/summary.json>) |
| Saved managed classes and protected INI/Config | [PASS, 89 checks](<D:/IA_Europa/Receipts/09427b/scratch/managed-tests-attempt1/summary.json>) |
| Engine Python 2.7.14 recording harness | [PASS: 3 editor paths, 15 native scripts; runpy-dict mutant refused; zero saves](<D:/IA_Europa/Receipts/09427b/scratch/python27-attempt1/summary.json>) |
| Environment read contract | [PASS: 19 checks](<D:/IA_Europa/Receipts/09427b/scratch/environment-reads.json>) |
| Shared session, owner, ledger, admission, readers, tooling and finish | [PASS: 7 regression suites](<D:/IA_Europa/Receipts/09427b/scratch/regressions-attempt1.json>) |

The [eight-case rehearsal](<D:/IA_Europa/Receipts/09427b/scratch/rehearsal-attempt2/summary.json>) passed initially and again after the FH3 follow-up: passing leg; abnormal exit with evidence retained; deadline; Content-change STOP; UBT-build STOP; self-restarting detached -MONITOR stand-in adopted and stopped before its root; ordinary failing leg followed by a passing leg; normal ensure-folder failure versus abnormal exit. Stand-in logs/probes are copied from kept real files with provenance. An unrelated sibling survived. No Unreal process or real slot was used by these tests. All changed editor-script generation was tested under the engine’s own Python 2.7.14; game scripts use the unchanged native JSON driver.

4.25 [WindowsPlatformCrashContext.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Windows/WindowsPlatformCrashContext.cpp:442>) appends -MONITOR using the editor/game PID; lines 493–536 describe the respawn and temporary PID file. [PythonScriptPlugin.cpp](<E:/EpicGames/UE_4.25/Engine/Plugins/Experimental/PythonScriptPlugin/Source/PythonScriptPlugin/Private/PythonScriptPlugin.cpp:560>) uses the PY console path; [EditorPythonExecuter.cpp](<E:/EpicGames/UE_4.25/Engine/Plugins/Editor/EditorScriptingUtilities/Source/EditorScriptingUtilities/Private/EditorPythonExecuter.cpp:145>) sets the unattended-script flag only in the other execution route. Runtime scripts never import or save. [EngineAnalytics.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/EngineAnalytics.cpp:108>) excludes -game from editor analytics initialization. Both log files still receive the same upload check on every session. [SceneView.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/SceneView.cpp:845>) maps AA quality zero to AAM_None; the bound probe driver:219–222 reads back the effective value before each picture.

The [final arming/install receipt](<D:/IA_Europa/Receipts/09427b/arming/install-followup.json>) preserves the initial 09427b generation; [the first install](<D:/IA_Europa/Receipts/09427b/arming/install.json>) preserved spent 09427. Both complete 18-slot dry runs passed; the final [throwaway dry run](<D:/IA_Europa/Receipts/09427b/scratch/dry2/run-result.json>) and [dry console](<D:/IA_Europa/Receipts/09427b/scratch/dry2/entry-console.txt>) record zero Unreal launches. Qualification finished 10-30 12:08:20 IST; the launch window ends 10-31 06:00 IST. A started session may finish. Each session has exactly one slot under `Receipts/09427b/runtime-attempts/<slug>/01`, with TEMP mirrored and single-use state under `runtime-state/<slug>`; older armings refuse.

The [live observe-only reading](<D:/IA_Europa/Receipts/09427b/live-gate.json>) at 10-30 12:08:06 IST recorded 45.69 seconds idle, 2.384 GiB physical, 10.101 GiB free commit, GPU 4%, and reasons ['input_idle', 'physical_memory']. This reading launched nothing. The [outer idle precheck](<D:/IA_Europa/Receipts/09427b/entry-idle-precheck-02.jsonl>) and each leg’s retained precheck/gate decided its actual start.

| Leg | Limit s | Recorded idle wait / gate wait s | Start IST | End IST | Duration s / exit | Reader |
|---|---|---|---|---|---|---|
| Smoke | 300 | 1025.307 / 82.15 | 10-30 14:41:07 | 10-30 14:41:40 | 33.027 / 0 | PASS |
| Load | 300 | 0.0 / 70.845 | 10-30 14:43:41 | 10-30 14:43:52 | 10.624 / 3 | FAIL: Nonzero or stopped session |
| WorldsPIE | 600 | — | — | — | not started | NOT_RUN |
| WorldsGame | 300 | — | — | — | not started | NOT_RUN |
| GC | 300 | — | — | — | not started | NOT_RUN |
| A7_:blinking:positive | 900 | — | — | — | not started | NOT_RUN |
| A7_:lighting_mismatch:positive | 900 | — | — | — | not started | NOT_RUN |
| A7_:lod_corruption:positive | 900 | — | — | — | not started | NOT_RUN |
| A7_:missing_object:positive | 900 | — | — | — | not started | NOT_RUN |
| A7_:blinking:NoApply | 300 | — | — | — | not started | NOT_RUN |
| A7_:lighting_mismatch:NoApply | 300 | — | — | — | not started | NOT_RUN |
| A7_:lod_corruption:NoApply | 300 | — | — | — | not started | NOT_RUN |
| A7_:missing_object:NoApply | 300 | — | — | — | not started | NOT_RUN |
| A7_:blinking:SkipRestore | 300 | — | — | — | not started | NOT_RUN |
| A7_:lighting_mismatch:SkipRestore | 300 | — | — | — | not started | NOT_RUN |
| A7_:lod_corruption:SkipRestore | 300 | — | — | — | not started | NOT_RUN |
| A7_:missing_object:SkipRestore | 300 | — | — | — | not started | NOT_RUN |
| Admission | 600 | — | — | — | not started | NOT_RUN |

Exact precheck samples, gate polls, root and wrapper end times, exit codes and reader paths are in [timeline.json](<D:/IA_Europa/Receipts/09427b/timeline.json>). The console is copied verbatim from [entry-console.txt](<D:/IA_Europa/Receipts/09427b/entry-console.txt>):

```text
LEDGER before 2026-10-30T08:49:58.025536+00:00
{"scope": "engine51", "files": 339410, "folders": 0, "errors": 0}
{"scope": "engine425", "files": 179533, "folders": 0, "errors": 0}
{"scope": "paris_original", "files": 1511, "folders": 0, "errors": 0}
{"scope": "unreal_engine", "files": 1271, "folders": 0, "errors": 0}
{"scope": "ubt", "files": 43, "folders": 0, "errors": 0}
{"scope": "uht", "files": 26, "folders": 20, "errors": 0}
{"scope": "crashclient", "files": 81, "folders": 0, "errors": 0}
{"scope": "epic", "files": 4666, "folders": 0, "errors": 0}
{"scope": "c_root", "files": 16, "folders": 44, "errors": 0}
{"scope": "d_root", "files": 35, "folders": 78, "errors": 0}
{"scope": "e_root", "files": 5, "folders": 65, "errors": 0}
{"scope": "windows", "files": 197255, "folders": 0, "errors": 52}
{"scope": "wer_system", "files": 60, "folders": 0, "errors": 132}
{"scope": "gpu_nvidia", "files": 93, "folders": 0, "errors": 0}
{"scope": "gpu_dx", "files": 67, "folders": 0, "errors": 0}
{"scope": "tool_claude", "files": 16752, "folders": 0, "errors": 0}
{"scope": "tool_codex", "files": 12626, "folders": 0, "errors": 0}
{"scope": "tool_temp", "files": 7771, "folders": 0, "errors": 55}
{"scope": "port", "files": 73429, "folders": 0, "errors": 0}
{"scope": "ue_temp_0", "files": 0, "folders": 0, "errors": 0}
{"scope": "ue_temp_1", "files": 0, "folders": 0, "errors": 0}
{"scope": "ue_temp_2", "files": 0, "folders": 0, "errors": 0}
Waiting: ten minutes of human idle; 6 GiB free physical memory.
Waiting: ten minutes of human idle; 6 GiB free physical memory; 8 GiB free commit.
Waiting: ten minutes of human idle; 6 GiB free physical memory.
Idle pre-check passed: ten minutes idle and all measured conditions hold.
Smoke: checking Content, config, binaries, tooling and routing.
Waiting: three passing polls, 25 seconds apart.
Measured gate passed: three quiet polls; the launcher is already running.
Smoke: starting its one session; limit 300 seconds.
Smoke: PASS; raw evidence hashed; proceeding to the next leg.
Idle pre-check passed: ten minutes idle and all measured conditions hold.
Load: checking Content, config, binaries, tooling and routing.
Waiting: three passing polls, 25 seconds apart.
Measured gate passed: three quiet polls; the launcher is already running.
Load: starting its one session; limit 300 seconds.
Stopped: abnormal editor/game exit. No further session will be started.
LEDGER after 2026-10-30T09:14:06.078078+00:00
{"scope": "engine51", "files": 339410, "folders": 0, "errors": 0}
{"scope": "engine425", "files": 179534, "folders": 0, "errors": 0}
{"scope": "paris_original", "files": 1511, "folders": 0, "errors": 0}
{"scope": "unreal_engine", "files": 1271, "folders": 0, "errors": 0}
{"scope": "ubt", "files": 43, "folders": 0, "errors": 0}
{"scope": "uht", "files": 26, "folders": 20, "errors": 0}
{"scope": "crashclient", "files": 81, "folders": 0, "errors": 0}
{"scope": "epic", "files": 4666, "folders": 0, "errors": 0}
{"scope": "c_root", "files": 16, "folders": 44, "errors": 0}
{"scope": "d_root", "files": 35, "folders": 78, "errors": 0}
{"scope": "e_root", "files": 5, "folders": 65, "errors": 0}
{"scope": "windows", "files": 197256, "folders": 0, "errors": 52}
{"scope": "wer_system", "files": 60, "folders": 0, "errors": 132}
{"scope": "gpu_nvidia", "files": 93, "folders": 0, "errors": 0}
{"scope": "gpu_dx", "files": 67, "folders": 0, "errors": 0}
{"scope": "tool_claude", "files": 16760, "folders": 0, "errors": 0}
{"scope": "tool_codex", "files": 12631, "folders": 0, "errors": 0}
{"scope": "tool_temp", "files": 7771, "folders": 0, "errors": 55}
{"scope": "port", "files": 73476, "folders": 0, "errors": 0}
{"scope": "ue_temp_0", "files": 0, "folders": 0, "errors": 0}
{"scope": "ue_temp_1", "files": 0, "folders": 0, "errors": 0}
{"scope": "ue_temp_2", "files": 0, "folders": 0, "errors": 0}
{"result": "PASS", "classification_counts": {"foreign": 3, "port_C_exception": 42, "port_E_exception": 1, "port_D": 81, "tooling": 47}, "exception_growth_charged": {"C": 53248, "E": 0, "engine51": 0}, "cumulative_job_exception_growth": {"C": 53248, "E": 0, "engine51": 0}, "stops": [], "root_attributions": [], "operation": "runtime-run", "kind": "runtime", "time_utc": "2026-10-30T09:15:46.457956+00:00", "disks_before": {"C": {"resolved": "C:\\", "free": 30163533824, "start": 13958643712, "warning": 11811160064, "floor": 10737418240}, "D": {"resolved": "D:\\", "free": 57662701568, "start": 42949672960, "warning": 32212254720, "floor": 26843545600}, "E": {"resolved": "E:\\", "free": 308993359872, "start": 16106127360, "warning": 12884901888, "floor": 10737418240}}, "disks_after": {"C": {"resolved": "C:\\", "free": 30127452160, "start": 13958643712, "warning": 11811160064, "floor": 10737418240}, "D": {"resolved": "D:\\", "free": 57658257408, "start": 42949672960, "warning": 32212254720, "floor": 26843545600}, "E": {"resolved": "E:\\", "free": 308393988096, "start": 16106127360, "warning": 12884901888, "floor": 10737418240}}, "snapshot_before_sha256": "e5b5eac4c7761c057937d4f704ad388a72a59cc99fab5ecb0c818dc9051daa17", "snapshot_after_sha256": "d87e5c277e40f8b0c05e28fb8c2a3515cdaddb385c7376b7b0b285f0c5cb343f", "port_D_bytes_before": 108113364671, "port_D_bytes_after": 108117103412, "tooling_before": {"tool_claude": {"path": "<USERPROFILE>\\.claude", "resolved": "<USERPROFILE>\\.claude", "present": true, "files": 16752, "folders": 0, "bytes": 12009345259}, "tool_codex": {"path": "<USERPROFILE>\\.codex", "resolved": "E:\\CodexData\\home", "present": true, "files": 12626, "folders": 0, "bytes": 32616521040}, "tool_temp": {"path": "C:/ClaudeTemp", "resolved": "C:\\ClaudeTemp", "present": true, "files": 7771, "folders": 0, "bytes": 707087439}}, "tooling_after": {"tool_claude": {"path": "<USERPROFILE>\\.claude", "resolved": "<USERPROFILE>\\.claude", "present": true, "files": 16760, "folders": 0, "bytes": 12018120481}, "tool_codex": {"path": "<USERPROFILE>\\.codex", "resolved": "E:\\CodexData\\home", "present": true, "files": 12631, "folders": 0, "bytes": 32646139122}, "tool_temp": {"path": "C:/ClaudeTemp", "resolved": "C:\\ClaudeTemp", "present": true, "files": 7771, "folders": 0, "bytes": 707087439}}, "scan_access_errors_before": 239, "scan_access_errors_after": 239, "scope_note": "One initial full inventory, canonical SHA-256 per snapshot, before/after deltas per operation. Error-fatal protected enumeration; listed denied OS/tooling paths are not treated as empty.", "ledger_bytes": 226519148}
```

| Gate | Reader verdict |
|---|---|
| A3 | FAIL |
| A5 | NOT_RUN |
| A6 | NOT_RUN |
| A7 | NOT_RUN |
| A8 | NOT_RUN |

Load’s [driver receipt](<D:/IA_Europa/Receipts/09427b/runtime-attempts/Load/01/driver.json>) records `DRIVER_FAIL: RuntimeError: Saved camera rotation differs`. The check expects pitch/yaw/roll `[-12, 0, 0]`, but raises before retaining the actual angles. The log shows three EA_Core loads and three QUIT_EDITOR requests, followed by `EXCEPTION_ACCESS_VIOLATION reading address 0xffffffff`, Niagara stack frames and `StaticShutdownAfterError`. This is a crash after exit was requested, not a clean exit. The fatal log lines are engine-emitted; the preceding driver failure is ours. Callback re-entry is a hypothesis from the repeated sequence, not a proved crash cause. No runtime repair or retry followed. [Crash analysis and source lines](<D:/IA_Europa/Receipts/09427b/crash-analysis.json>).

| A7 anomaly | Attempted | Valid | Applied | Reverted | Refused | Invalid | Counted | Status |
|---|---|---|---|---|---|---|---|---|
| blinking | 0 | 0 | 0 | 0 | 0 | 0 | 0 | NOT_RUN |
| lighting_mismatch | 0 | 0 | 0 | 0 | 0 | 0 | 0 | NOT_RUN |
| lod_corruption | 0 | 0 | 0 | 0 | 0 | 0 | 0 | NOT_RUN |
| missing_object | 0 | 0 | 0 | 0 | 0 | 0 | 0 | NOT_RUN |

These zeros describe unstarted legs, not successful reader verdicts. No A7 event-count receipt or real picture was produced. NoApply, SkipRestore, invalid-target and unsupported-ID controls were not run.

The [contact-sheet index](<D:/IA_Europa/Receipts/09427b/contact-sheet-index.json>) contains 0 counted-event contact sheets; synthetic scratch pictures are excluded. The [per-picture AA index](<D:/IA_Europa/Receipts/09427b/all-picture-aa.json>) lists 0 native picture requests, 0 with eligible zero echoes. Nonzero or missing echoes never count. [Gate verdicts and A7 counts](<D:/IA_Europa/Receipts/09427b/gate-verdicts.json>). The owner’s eye review remains separate from numeric readers.

| Session | Exit cleanup / probe_stop | PIE rows | Upload check | New report folders |
|---|---|---|---|---|
| Smoke | 1 / 1 | 0 | PASS | 0 |
| Load | 3 / 0 | 0 | PASS | 0 |

Each session’s raw files were hashed at its end and verified unchanged during closeout. [Session evidence, upload checks and raw-hash references](<D:/IA_Europa/Receipts/09427b/session-evidence.json>). Both executed editor argv contain EU4 bSendUnattendedBugReports=False and FC4 bSendUsageData=False; both log pairs pass the upload check. No -game session ran, so its live opt-out proof remains unproved. Log evidence covers POST/upload/send signs; it is not a packet capture. All 28 Runtime.log error lines belong to the Load crash’s engine-emitted fatal block; no ensure was observed. The caught driver failure is separately reported above. There was no new crash/ensure folder; CrashClient.log records Editor/ExitCode:3 and MTBF/NoTempCrash. The two pre-existing host crash folders remain recorded in [crash-analysis.json](<D:/IA_Europa/Receipts/09427b/crash-analysis.json>).

| Leg | Monitor PID / parent | Start UTC | Exact command line | Canary environment |
|---|---|---|---|---|
| Smoke | 22808 / 36064 | 2026-10-30T09:11:09.271588+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=408 -WRITE=384 -MONITOR=14924 -abslog=D:/IA_Europa/Receipts/09427b/runtime-attempts/Smoke/01/session/CrashClient.log -ConcertServer="8A5F7E7844344DD4945FECB13439D87B" -RespawnedInstance | absent |
| Load | 17244 / 17448 | 2026-10-30T09:13:41.706476+00:00 | "E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe" "../../../Engine/Binaries/Win64/CrashReportClientEditor.exe"  -READ=500 -WRITE=512 -MONITOR=48368 -abslog=D:/IA_Europa/Receipts/09427b/runtime-attempts/Load/01/session/CrashClient.log -ConcertServer="C72F8BB44F3A7BCF017B79988F7280CD" -RespawnedInstance | absent |

The [adopted-monitor table](<D:/IA_Europa/Receipts/09427b/adopted-monitors.json>) records identities, parentage, retained handles’ creation identities and names-only environment observations. Monitor stops, normal exit grace and root exit times are retained in the child/session receipts. Ownership is an exact -MONITOR PID match, never a name match.

No UBT child was recorded, so there is no observed UBT argv to list and no claim that the guard was exercised by a live UBT child.

Compiler/linker/UHT observations: 0. [Read-only process closeout](<D:/IA_Europa/Receipts/09427b/process-closeout.json>). All 1461 Content files, 41 protected config/project records, descriptors and bound build artifacts remain unchanged. The kit binds only the 27-file 4.25 manifest. Marker 31124 and the prior two quarantined packages remain unchanged. [Protected result evidence](<D:/IA_Europa/Receipts/09427b/result-evidence.json>).

| Existing fixture | Bytes | SHA-256 |
|---|---|---|
| SM_EA_TwoLod.uasset | 137553 | bbd24a79a2b46efeea8a8e3d23c65ebb1eef73b16ea1af97e04ff8bb7cec714a |
| EA_Core.umap | 21114 | c2986f9aee65dbb5d5b1af0016759f7ef881d3636400de19938206fb05284a14 |
| EA_Travel.umap | 21124 | 51c7fe5c212b2c7996fa5af4beebdf79bf0b01ceaf59d0a041ceb34f39fd70d0 |

Every Saved/Intermediate change is enumerated with before/after values and leg in [managed-changes.json](<D:/IA_Europa/Receipts/09427b/managed-changes.json>). FF2/FG2/FC5 classes are distinct from protected Config/Content. Runtime failures create no authoring package to quarantine; their evidence stays in its original slot. The earlier authoring quarantine remains untouched.

The [failed-session retention receipt](<D:/IA_Europa/Receipts/09427b/failed-session-retention.json>) verifies Load’s sealed files in place: no Content output existed to move, and no file was deleted. EditorRuns contains the unchanged 31124 marker and the recorded marker for failed Load PID 48368; both stay in place.

The [ledger summary](<D:/IA_Europa/Receipts/09427b/ledger-summary.json>) records 6 completed operations, 0 real pauses and 0 root attributions. Charged C:/E:/5.1-engine growth: {'C': 53248, 'E': 0, 'engine51': 0} bytes, against 250,000,000 / 50,000,000 / 100,000,000-byte limits. Operation/admission STOP counts: 0 / 0. Foreign activity is recorded and never terminated; foreign root contents are not opened.

EU1 audit: {'direct_read_only_deviation': 1, 'filtered_entry': 313}; 39 edit events; 0 pause violations. The single direct read-only bootstrap deviation read the prior wrapper to establish the fresh boundary. There were no direct write-capable shell commands. Auxiliary errors are retained: one read used the wrong text encoding; a documentation helper had an unused import; the first commit helper stripped leading porcelain whitespace and refused before staging; an inline final-helper preparation had a quoting warning and created no file. These were corrected without relaxing a test or touching a real slot. Initial and follow-up qualification tests passed; no prerequisite failed twice. [Full command audit](<D:/IA_Europa/Receipts/09427b/command-audit-final.json>).

The canary was present as a positive control, hash-matched without exposing its value, and dropped at child boundaries. The final launch-second scan is GAP: 2348 files, 682649685 bytes, zero matches and 3 read gaps. FE2 excludes only 21 OS-denied event logs under C:\Windows\System32\winevt\Logs; readable logs are scanned. Gzip/ZIP evidence is decoded. No full EK1 scan ran. [Final canary receipt](<D:/IA_Europa/Receipts/09427b/canary-final.json>).

| Unreadable final-scan file | Error |
|---|---|
| C:\Windows\SoftwareDistribution\DataStore\DataStore.edb | PermissionError / WinError 32 |
| C:\Windows\SoftwareDistribution\DataStore\DataStore.jfm | PermissionError / WinError 32 |
| C:\Windows\SoftwareDistribution\DataStore\Logs\edb.log | PermissionError / WinError 32 |

The [earlier scan](<D:/IA_Europa/Receipts/09427b/canary-exit.json>) passed. An ordinary shared-read [recheck](<D:/IA_Europa/Receipts/09427b/canary-read-gap-recheck.json>) still found these three sharing violations. No service, permission or security setting was changed and no exclusion was added. These files are outside FE2; final coverage remains incomplete. The original GAP receipt is retained.

Every EN5 item is accounted for below. No new intent-clause waiver was used; the explicit FH/FG rulings authorize the behavior changes.

| Rule / source | Application |
|---|---|
| [EN5 documented behavior](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:54>) | Stock respawn, PY execution, analytics and managed files have the source citations above; no uncited exception. |
| [EN5 no blocked names/secrets](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:61>) | EL1/EL2/EM2 filtered boundaries, preserved base-variable hashes, nonvacuous canary control and scan; monitor environments recorded by name when readable. |
| [EN5 no loss of base variables](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:62>) | Execution contract preserves non-blocked values; only explicit D: routing and runtime overrides are added. |
| [EN5 allowed writes](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:63>) | D: receipts/tooling and EA4/EG1/ET3 ledger classes; FF2/FG2/FC5 managed changes listed. |
| [EN5 protected-state hard STOP](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:64>) | EA8 entry/exit against v2 and independent protected checks; no Content/config/descriptor/binary/manifest difference. |
| [EN5 engine writes](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:68>) | EG1 classes retained; ET2 marker 31124 unchanged; no 5.1 runtime. |
| [EN5 exposure hard STOP](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:69>) | No canary match; FE2 exclusion is only the ruled denied-event-log class. |
| [EN5 machine/security/global settings](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:70>) | None changed; no foreign process stopped, no persistent config edit. |
| [EN5 launch restriction](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:71>) | Only the armed smoke/runtime sessions launched Unreal; no cook, commandlet, UAT or packaging. FD2/FE3 UBT checks retained. |
| [EN5 floor rule](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:72>) | EV3/EY4 disk lines and EU2/EZ2 waits retained; FH3 treats a controlled in-session floor stop as a failed leg, then admission waits. |
| [EZ1 / FA6](<D:/IntrusiveAnomalies/_reviews/094-24-ruling.md>) | Root changes attributed by the ledger; foreign activity never ends the job. |
| [FH3 / FH4](<D:/IntrusiveAnomalies/_reviews/094-27-ruling.md:54>) | Normal leg failures retain hashes and proceed; crash/protected/upload/canary and fatal smoke conditions stop. No manual FAIL relabel. |
| [FH5 / FH6](<D:/IntrusiveAnomalies/_reviews/094-27-ruling.md:76>) | Exact monitor ownership, monitor-first stop, 60-second grace; permitted unattended runtime argv and no import/save. |
| [FH7 / FH8](<D:/IntrusiveAnomalies/_reviews/094-27-ruling.md:99>) | 8/6 GiB admission, below-4 GiB session stop, six-event budget, game upload checks and effective AA=0 requirement. |
| [D2 / FH9 / W4](<D:/IntrusiveAnomalies/_reviews/094-27-ruling.md:115>) | Qualified before real launch; one real session per leg; actual commit dates, no push/tags; report-only closeout after any stop. |

Prelaunch review found a classification gap after the first qualification: an observed protected-write or upload stop could be lost if the final checks passed. [The synthetic reproduction](<D:/IA_Europa/Receipts/09427b/classifier-reproduction.json>) demonstrated all three affected causes before the fix. The first waiting wrapper was stopped by its exact owned PID before any entry-point start, with [a receipt](<D:/IA_Europa/Receipts/09427b/prelaunch-classifier-review.json>). The fix passed its added checks and a repeated rehearsal/dry run. Both arming generations and waiting logs remain preserved. This was not a real-session retry or a relabelled gate verdict.

| Local feat/europa-port commit | Purpose / W4 |
|---|---|
| 0886769aa90a881f243a4ca9255951d820de7bdf | Initial qualified FH tooling and prediction |
| ecfb3ceee40aed85e256624d898fb741a25eb539 | Prelaunch follow-up retaining observed hard-stop causes |
| 0a02987bd2baa32ad9bebfd854bba2c9e136c452 | 0a02987bd2baa32ad9bebfd854bba2c9e136c452 / 2026-10-30T15:16:18+05:30 / 2026-10-30T15:16:18+05:30 / docs(europa): record 09427b runtime evidence and gate verdicts |

All executable/DLL hashes and the 27-file manifest are in [binary-hashes.json](<D:/IA_Europa/Receipts/09427b/binary-hashes.json>) and [binary-hashes.tsv](<D:/IA_Europa/Receipts/09427b/binary-hashes.tsv>). [Archive inventory](<D:/IA_Europa/Receipts/09427b/archive-image-inventory.json>) lists compressed receipt archives and test-named artifacts; no binary/build archive was created. Bench and CVar/INI switches were process-local and ended with their sessions. The spent kit must not be restarted.

VERDICT: RED | 2 real sessions; A3 FAIL, A5 NOT_RUN, A6 NOT_RUN, A7 NOT_RUN, A8 NOT_RUN; evidence retained, no retry or push.
