**094-39: NEEDS-DECISION.** Both 4.25 targets build and the FW3/FW4 checks pass in the retry, but the office smoke remains incomplete: missing_object never fires. Both authorized slots are spent. FV6 is skipped because it requires a passing smoke.

Entry and exit fast checks pass: m51/index/status, master, release/2026-10-05 and all 30 tags are unchanged; Paris Content, Config, descriptors and project file are unchanged. Both sessions rechecked protected files. The rebound manifest is 27/27 equal. Exit free space: C: 26.23, D: 35.03, E: 173.78 GiB; no drive-root entries changed. [Exit receipt](D:/IA_Europa/Receipts/09439/exit.json).

FW3 uses the shared `HasMultipleLods` predicate for auto candidates and components, retaining `WorstLodSentinel` and coarsest apply. Sources: [candidate guard](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp:453), [component guard](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_LodCorruption.cpp:57). G506 records the deliberate addition and retained pool extension; no new keys.

FW4 records marker request/frame IDs, saves matching post-UI controls, and joins them to exact pre-UI captures. Wrong/missing draw, capture or control pairs fail. `tick` and `tick_type` are explicitly build constants; `tick_calls` is measured. FW5 lists the 0.8 s interval / 0.4 s hold / one concurrent fire in [the parity inventory](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09438-auto-parity.json).

Changed-code tests pass: FW3 single-LOD/wrong-target/finest-LOD negatives; FW4 frame/request/control negatives; FW6 cutoff and admission tests; keyword-Rotator/static checks. The fallback retry test rejects missing, disabled and incorrect presets using kept frame 309. No in-editor Python changed, so the Python 2.7 harness was not applicable. [Test receipts](D:/IA_Europa/Receipts/09439/tests).

Both Development targets built from `d25b8eb`. Every UBT launch used BelowNormal priority and `-MaxParallelActions=2`. All compile-error counts are zero in every module; the two failures were owned-tree memory stops, not compiler errors.

| Attempt | Start / minimum free commit (GiB) | Result |
|---|---:|---|
| editor-01 | 8.570 / 1.216 | Stopped below 3 GiB; partial outputs quarantined by receipt |
| editor-02 | 10.746 / 2.898 | Stopped below 3 GiB; partial outputs quarantined by receipt |
| editor-03 | 9.787 / 3.056 | PASS, exit 0 |
| game-01 | 10.577 / 4.424 | PASS, exit 0 |

[Build attempts, readings, argv and logs](D:/IA_Europa/Receipts/09439/build-summary.json). No further native change followed these builds.

Map: **EA_Core fallback**, because street admission did not pass by build-pair completion plus 45 minutes, at 02:10:10 IST. First admission: 10.18 GiB free commit, 10.15 GiB physical and 359 idle seconds. Both runs used the armed kit; no direct launch.

| Session | Start–exit (IST Nov 2) | Runtime | Frames / drops | Exit | Verdict |
|---|---|---:|---:|---:|---|
| 01 | 02:10:15–02:12:43 | 147.80 s | 2352 / 0 | 0 | FAIL |
| 02 | 02:19:40–02:21:44 | 124.30 s | 2352 / 0 | 0 | FAIL |

Session 01 fired only lighting. Its 6% default bounds prefilter excluded visible targets: LOD 2.80% drawn, hide cube 1.85%. The tested retry sets only the fallback launch prefilter to 0.5%, matching the existing measured-census floor; the 0.5%–25% census band and all reader gates stay unchanged. This process-local setting is echoed and expires at exit. No map, Config, native code or player-camera change. [Retry qualification](D:/IA_Europa/Receipts/09439/C5-retry-qualification.json).

| Anomaly | Session 01 fires / affected / masks | Session 02 fires / affected / masks | Retry result |
|---|---:|---:|---|
| blinking | 0 / 0 / 0 | 30 / 180 / 180 | PASS |
| lighting_mismatch | 12 / 156 / 156 | 6 / 78 / 78 | PASS |
| lod_corruption | 0 / 0 / 0 | 12 / 156 / 156 | PASS |
| missing_object | 0 / 0 / 0 | 0 / 0 / 0 | FAIL |

The retry repeats **six identical eight-draw sequences**: LOD, lighting, blink, blink, blink, LOD, blink, blink. `SetRunning(true)` reinitializes the seed at [AnomalyAutoInjectorSubsystem.cpp:239](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp:239); [the driver restarts it per hold](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/Compat/AnomalyCaptureLabels425.cpp:100). The 240-frame hold ends before another draw. Missing-object is never selected; this is a confirmed driver-coverage failure. [Exact log lines and seed sequences](D:/IA_Europa/Receipts/09439/retry-sequence-diagnosis.json). A longer or continuous scored hold needs qualification and a fresh slot. No third launch or fourth Editor build was made.

| Remaining smoke gate, session 02 | Result |
|---|---|
| Affected/clean masks | PASS: 414 affected frames have masks; 1938 clean frames have no mask pixels or mask PNG |
| FW3 | PASS: 12 LOD fires, each on a two-LOD component, choosing 2 of 2 |
| FW4 | PASS: requests 1–8, sample frames 1712–1719, each has its exact positive UI control and marker-free capture; all 2352 captures are marker-free |
| Annotation | PASS: schema 2.1, affected/injected sets agree; far 1,000,000; 42 measured coverage values and 6 explicit unmeasurable values (`valid=false`, `coverage_pct=-1`), no invalid value |
| Census | PASS: peak 3 drawn; zero exhaustion/lost/no-pass/polluted batches. Over 55 and wall strip waived only by fallback |
| Custom depth | PASS: all 14 component states restore; 2352 mid samples pass; reserved 203 never assigned; host-on components unchanged |
| Player view | PASS: 2352/2352 frame views agree |
| Formats/exit | PASS: D3D11, format 18, 1280×720, zero drops, normal exit |
| Upload/protection | PASS: both session/CrashClient logs clear; protected hashes unchanged |

Each LOD event below has **zero changed target pixels beyond one RGB level**, across 13 affected frames. This is information only, not a smoke gate; the reader does not claim a visible LOD change. [Per-frame measurements](D:/IA_Europa/Receipts/09439/c5-attempts/C5/02/reader.json).

| LOD event | Pre-apply reference index | Maximum changed fraction |
|---|---:|---:|
| lod_corruption@2439 | 694 | 0.000000 |
| lod_corruption@2559 | 814 | 0.000000 |
| lod_corruption@2727 | 982 | 0.000000 |
| lod_corruption@2847 | 1102 | 0.000000 |
| lod_corruption@3015 | 1270 | 0.000000 |
| lod_corruption@3135 | 1390 | 0.000000 |
| lod_corruption@3303 | 1558 | 0.000000 |
| lod_corruption@3423 | 1678 | 0.000000 |
| lod_corruption@3591 | 1846 | 0.000000 |
| lod_corruption@3711 | 1966 | 0.000000 |
| lod_corruption@3879 | 2134 | 0.000000 |
| lod_corruption@3999 | 2254 | 0.000000 |

Session 01 self-check messages, verbatim:

```text
Capture425 SELF_CHECK stage=start frames=1 size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily=5 synth=0 viewport=SceneViewport client=GameViewportClient viewport_size=1280x720 backbuffer_format=18 slate=1 pie=0 pre_ui=1 ui_draws=4 ui_controls=4 ui_positive=4 ui_checked=1 ui_hits=0 ui_paired=1 tick=OnWorldPostActorTick425 tick_source=build_constant tick_type=2 tick_type_source=build_constant tick_calls=1223 fixed_step=1 fixed_dt=0.033333333 fixed_rate=0 fixed_fps=30.000000 smoothing=0 census=bench_off batches=0 lost=0 no_pass=0 polluted=0
Capture425 SELF_CHECK stage=300 frames=300 size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily=315 synth=0 viewport=SceneViewport client=GameViewportClient viewport_size=1280x720 backbuffer_format=18 slate=1 pie=0 pre_ui=1 ui_draws=8 ui_controls=8 ui_positive=8 ui_checked=300 ui_hits=0 ui_paired=8 tick=OnWorldPostActorTick425 tick_source=build_constant tick_type=2 tick_type_source=build_constant tick_calls=1533 fixed_step=1 fixed_dt=0.033333333 fixed_rate=0 fixed_fps=30.000000 smoothing=0 census=on batches=1 lost=0 no_pass=0 polluted=0
```

Session 02 self-check messages, verbatim:

```text
Capture425 SELF_CHECK stage=start frames=1 size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily=5 synth=0 viewport=SceneViewport client=GameViewportClient viewport_size=1280x720 backbuffer_format=18 slate=1 pie=0 pre_ui=1 ui_draws=4 ui_controls=4 ui_positive=4 ui_checked=1 ui_hits=0 ui_paired=1 tick=OnWorldPostActorTick425 tick_source=build_constant tick_type=2 tick_type_source=build_constant tick_calls=1354 fixed_step=1 fixed_dt=0.033333333 fixed_rate=0 fixed_fps=30.000000 smoothing=0 census=bench_off batches=0 lost=0 no_pass=0 polluted=0
Capture425 SELF_CHECK stage=300 frames=300 size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily=315 synth=0 viewport=SceneViewport client=GameViewportClient viewport_size=1280x720 backbuffer_format=18 slate=1 pie=0 pre_ui=1 ui_draws=8 ui_controls=8 ui_positive=8 ui_checked=300 ui_hits=0 ui_paired=8 tick=OnWorldPostActorTick425 tick_source=build_constant tick_type=2 tick_type_source=build_constant tick_calls=1664 fixed_step=1 fixed_dt=0.033333333 fixed_rate=0 fixed_fps=30.000000 smoothing=0 census=on batches=0 lost=0 no_pass=0 polluted=0
```

Crash: **none**. Upload sign: **none** in either session’s runtime/CrashClient logs. Protected change: **none**. FH5 adopted monitor 12968 for editor 22152 and monitor 50908 for editor 44440 by exact `-MONITOR=`; both left normally. Both raw sessions are hashed and retained as FAIL. [Session 01 receipt](D:/IA_Europa/Receipts/09439/c5-attempts/C5/01/result.json); [session 02 receipt](D:/IA_Europa/Receipts/09439/c5-attempts/C5/02/result.json).

Runtime-only settings were AA 0, exposure lock, screen percentage 100, dynamic resolution 0, motion blur 0, maxFPS 30 and capture fixed step 1/30; auto seed 4242 and 0.8/0.4/one schedule. Native receipts confirm pacing and stencil restoration. The retry additionally uses the fallback bounds preset 0.5%. No saved or machine setting changed. No archives were created; partial-build quarantine receipts account for moved outputs, and no other manual deletion occurred.

**FV6 not run:** the passing-smoke condition was not met. Zero commits rewritten; no backup ref or tree-diff proof created. The isolated redaction-transform test passed, but no history rewrite is claimed. Code’s scan/card remains downstream.

Bound EXE/DLL SHA-256 values follow. Host means `D:/IA_Europa/StylizedParisStreet/Binaries/Win64`; Plugin means its `Plugins/AnomalyInjector/Binaries/Win64`. [All 27 manifest bindings](D:/IA_Europa/Receipts/09439/binary-state.json).

| Artifact | SHA-256 |
|---|---|
| Host/StylizedParisStreet-Win64-Shipping.exe | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| Host/StylizedParisStreet.exe | `b3bb9b6dec5f01a1a7d06a49d14c0448f84bd4768ee80e52b68e0163ac49359b` |
| Host/UE4Editor-EuropaPhaseAProbe.dll | `f3b52d42a09992d10633c1198d2382fb53c781d9897b2640aff5c778a3af0653` |
| Host/UE4Editor-StylizedParisStreet.dll | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| Plugin/UE4Editor-AnomalyBench.dll | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| Plugin/UE4Editor-AnomalyCapture.dll | `f840b2b04007ae321c61bc68facd15957e4e07d33fd9150cf42201a2f706764c` |
| Plugin/UE4Editor-AnomalyControlServer.dll | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` |
| Plugin/UE4Editor-AnomalyInjector.dll | `4a75ad87f1570d77cb89961efb617b134948397412070824fcda6538fa3760de` |
| Plugin/UE4Editor-AnomalyShaders.dll | `b5b767d343422d7360a8b78ec29c7ce0da0090f056d13c5f9fd11b574240e6ad` |

Local commits on `feat/europa-port`: `d25b8eb38ac819f42ed87732a5284e1117124aa4` (FW fixes); `d8f6ebf89cc0ce5467a8e4dd60e629f87d8050ef` (qualified fallback preset); `0621027a1be8174a7dd4511d0eaa809f0932eae0` (closeout). W4 applied; no push, tags or protected-ref changes. Final worktree clean. [Committed closeout receipts](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09439-office-capture-receipts.json).

VERDICT: NEEDS-DECISION | Both targets build; missing_object never fired in the two-slot smoke, so FV6 remains blocked.
