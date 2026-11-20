094-37: the finishing fixes, focused tests, cleanup and both Development builds passed. **The office smoke remains incomplete:** both authorized sessions stopped on the 4 GiB commit floor. The retry preserved 24 PNGs and the start self-check. No further session was launched; both slots are spent.

FU4 removed **3,008 files, 9,011,502,233 bytes (8.39 GiB)**. D: free increased from **23.62 to 32.02 GiB**. [removed.tsv](D:/IA_Europa/Receipts/_cleanup-094-37/removed.tsv) records paths, sizes and modified times; [summary](D:/IA_Europa/Receipts/_cleanup-094-37/summary.json) records space readings. The reference search kept 39,466 files, including referenced inputs, FH2 probes, 09427e contact sheets, 09427b crash analysis and small summaries. P9, 09434 onward and their CodeReview folders were excluded. No directory or out-of-scope file was deleted. No second cleanup was performed during the resource wait.

The builds include previously pending commits **05c55e2** (G500 ordering guard) and **2b9dc19** (frozen coverage provenance). Infinite annotation far planes now export **1,000,000**, matching 5.1's [default and emission](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:594) at lines 594/10291. Census recovery follows [AnomalyCensus.cpp:629](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCensus.cpp:629) and AnomalyCensus.h:87: expire unresolved batches beyond eight ticks; discard/requeue no-pass and polluted results without credit; process resolved results first. Event ownership survives release. G502 records the retained event-adopts-census-value deviation.

The parity read against protected 5.1 commit `53bf725` found matching apply/revert implementations:

| Anomaly | 5.1 citation and result |
|---|---|
| blinking | [Anomaly_Blinking.cpp:74](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_Blinking.cpp:74): matching Hide/Show toggles and Show on revert. |
| lighting_mismatch | [Anomaly_LightingMismatch.cpp:65](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_LightingMismatch.cpp:65): matching immediate setters and restoration; no anomaly-side ramp. |
| lod_corruption | [Anomaly_LodCorruption.cpp:29](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_LodCorruption.cpp:29): matching requested 1-based/worst model and restoration. |
| missing_object | [Anomaly_MissingObject.cpp:35](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingObject.cpp:35): matching Hide/Show. |

The existing unavailable-contact-shadow API adapter remains; no new anomaly behavior fix was needed. This does not establish identical renderer settling. FU2's D1–D5 remain deferred.

Changed-code tests passed: the native timeout policy's eight/nine-tick boundary, resolved-result priority, discard cases and far planes; reader negatives for invalid self-checks, annotation values, wall changes and masks; keyword-Rotator and fixture-source checks. No in-editor Python changed, so the Python 2.7 harness was not applicable. [Test receipts](D:/IA_Europa/Receipts/09437/tests/fu-summary.json).

Both builds used BelowNormal priority and `-MaxParallelActions=2`. First-build compile errors: **0 across all modules**. No build was stopped or retried.

| Target | Attempt | Start/minimum free commit | Duration | Exit |
|---|---:|---:|---:|---:|
| Editor Development | 1 | 8.159 / 4.906 GiB | 62.66 s | 0 |
| Game Development | 1 | 8.351 / 6.053 GiB | 86.68 s | 0 |

[Build receipts](D:/IA_Europa/Receipts/09437/build-summary.json). Native source: `cedb5054701039fd79503b0395c9a05e909744e4`. The [rebound 27-file manifest](D:/IA_Europa/Receipts/09437/c5-425-binaries-02-C5.json) matches disk at exit.

Both street-map sessions were configured for seed 4242, the four-anomaly pool, the player view and 1280×720 through the armed kit. Times below are IST on November 1.

| Slot | Process interval | Admission/minimum commit | Exit | PNGs |
|---|---|---:|---:|---:|
| 01 | 21:14:17–21:14:24 | 9.565 / 1.045 GiB | 125, owned stop | 0 |
| 02 | 21:40:00–21:40:55 | 8.725 / 3.672 GiB | 125, owned stop | 24 |

Slot 01 exposed a 56-second gap between admission and process creation. Commit `0284a0d` moved admission after setup, retaining all thresholds and protected/build checks. Seven focused cases, ten harmless stand-in cases, the throwaway dry run and observe-only reading passed. A four-second admission wait outside a three-second stand-in limit also passed. The retry's gate-to-creation gap was **1.44 seconds**. Its later memory drop still triggered the required stop; the pressure's source is unconfirmed. [Retry qualification](D:/IA_Europa/Receipts/09437/C5-retry-qualification.json).

| Anomaly | Recorded fires | Mask PNGs | Three-event gate |
|---|---:|---:|---|
| blinking | 0 | 0 | Not reached |
| lighting_mismatch | 0 | 0 | Not reached |
| lod_corruption | 0 | 0 | Not reached |
| missing_object | 0 | 0 | Not reached |

The retry stopped during the initial clean turn, before the scored sweep. Its 24 PNGs, indices 0–23, are nonblack 1280×720; all 27 partial raw mask planes contain zero mask pixels. These are partial observations, not a passed capture gate.

| Remaining FU5 gate | Result |
|---|---|
| Affected masks / clean-frame absence | Unproven: paired labels were not flushed. |
| Schema 2.1, frame sets, far plane, coverage_pct | Unproven: annotation and final receipts were not written. |
| Census >55, no exhaustion/lost batch | Not reached; focused native timeout test passed. |
| Exact custom-depth restore / host actors untouched | Before sample: 1,980 components, two on; no after or paired mid-session proof. |
| Wall strip unchanged census-on | Wall pose not reached. |
| View equals player view on every frame | Unproven: camera rows were not flushed. |
| Self-check start and frame 300 | Start present; frame 300 not reached. |
| Normal exit | Failed in both slots: deliberate memory-limit stops. |

The captured self-check line, verbatim:

```text
Capture425 SELF_CHECK stage=start frames=1 viewport=GameViewportClient/FSceneViewport size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily=4 pre_ui=1 tick=OnWorldPostActorTick/LEVELTICK_All synth=0 fixed=1 census=bench_off batches=0 lost=0 no_pass=0 polluted=0
```

The pre-UI route is stock 4.25 SceneRendering.cpp:3044, before GameViewportClient.cpp:1584/1641/1695. The start line reports actual resolved pixels; no frame-300 line is claimed.

**Safety:** no runtime crash, upload sign or protected change. Exact adopted monitor/game PID pairs were **27068/47884** and **5244/11960**; each monitor was stopped first. All owned roots are gone. Slot 02's Runtime.log and CrashClient.log upload checks passed. Slot 01's CrashClient.log has no sign, but its missing Runtime.log leaves that check incomplete. Both reader verdicts remain FAIL; all **9 and 91** sealed raw-file hashes remain equal. [Partial evidence](D:/IA_Europa/Receipts/09437/closeout-evidence.json).

Runtime-only settings were auto-exposure and AA disabled, screen percentage 100, dynamic resolution and motion blur off, 30 fps and the bench driver. No config was saved; normal in-process restoration was not demonstrated after the forced stops. No machine setting or foreign process was changed.

[Lean exit](D:/IA_Europa/Receipts/09437/exit.json) passed: m51/index/status, protected refs, host Content/Config/descriptors and all bound artifacts unchanged as required. No drive-root entry changed; D: free was **37.25 GiB**. Feature worktree is clean. W4 author/committer date overrides were absent. Commits on `feat/europa-port`: `cedb505` (native fixes), `0284a0d` (admission fix), **`07c853e90b396a466e18571200b04a3149156e9d`** (closeout). Nothing pushed; no tags or archives created. Code's pre-push scan and office card remain Code's task.

EXE/DLL SHA-256 values below include carried bound binaries and the focused native test; full paths are in the manifest and partial-evidence receipt.

| File | SHA-256 |
|---|---|
| StylizedParisStreet-Win64-Shipping.exe | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| StylizedParisStreet.exe | `8fae1cf6f6af6a1618cf27b3256fab90a872a7e836fcb12af1ca6df3d9a0420b` |
| UE4Editor-EuropaPhaseAProbe.dll | `76400798ccd02cec57576555a27fc42b4bf270ab29d61c34f8a555b704c39d0c` |
| UE4Editor-StylizedParisStreet.dll | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| UE4Editor-AnomalyBench.dll | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| UE4Editor-AnomalyCapture.dll | `6dfa607a5aee4b65b74f395aa67ba1b1cc0c6e281179e146b3d9c0b8e6b86cf1` |
| UE4Editor-AnomalyControlServer.dll | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` |
| UE4Editor-AnomalyInjector.dll | `eaea32048873be460e9867f3e76fcc47d7c3166dab9f15fb9ad3d2ac6e72a5b5` |
| UE4Editor-AnomalyShaders.dll | `b5b767d343422d7360a8b78ec29c7ce0da0090f056d13c5f9fd11b574240e6ad` |
| test.exe | `54a113c5661ef100a7ab7bc9a6616c18ea1d46cebdb41f7ec309f77e071c5945` |

VERDICT: NEEDS-DECISION | Fixes built; both authorized smoke slots stopped on memory limits, leaving FU5 incomplete.
