<!-- collected by Claude Code (mailbox brief 094-30) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md, 9579 B, sha256 E228F9B1120DAE606F8E1019BBD55DA5C20DE22226470477E176AB86FC5C6906, at 2026-10-31T09:07:57.098803+05:30 -->
094-30 is complete: the [Phase C plan](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/PHASEC-capture-port-plan.md) is committed. It proposes asynchronous plugin frames first, zero-frame label mismatch second, and paired masks third. It contains 36 inventory rows and the C1 file-by-file changes. Capture remains disabled on 4.25; this is a planning result, not runtime qualification.

Entry and exit read-only checks agree: m51 HEAD, branch, index and status; all 52 other local branch/tag refs, including master, release/2026-10-05 and 30 tags. The feature worktree is clean. Free space at exit was C: 27.93, D: 60.34, E: 278.58 GiB. Receipts: [entry](D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/source-entry09430.json), [exit comparison](D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/exit-comparison09430.json). No editor, game, compiler or UBT was launched.

The inventory's different/missing rows are below. IDs refer to the plan, which gives the full source paths and current plugin call sites. Existing PHASE0/PHASE1 measurements were reused.

| IDs | Difference on stock 4.25 | Source anchor / consequence |
|---|---|---|
| 01 | Tickable-world subsystem substrate differs | WorldSubsystem.h:16; retain the already implemented Phase A shim. |
| 05 | Texture readback lacks the pitch/height Lock overload | RHIGPUReadback.h:99; its implementation discards map dimensions at RHIGPUReadback.cpp:128-136. An owned staging adapter is required. |
| 06-07 | Texture creation and transition APIs differ | RHICommandList.h:5179,3390: RHICreateTexture2D and TransitionResource replace the 5.1 calls. |
| 08 | Buffer readback accepts FRHIVertexBuffer | RHIGPUReadback.h:75-83; adapt RDG extraction and retain byte bounds. |
| 09-10 | SVE activation differs; after-pass hooks are absent | SceneViewExtension.h:116-146. Use backbuffer colour first; a separate post-render mask hook later. |
| 11-12 | RDG exists, but its handoff differs and AddEnqueueCopyPass is absent | RenderGraphBuilder.h:20,112,167; construct a local graph and explicit readback passes. |
| 13 | View-family extension-data attachment is absent | SceneView.h:1406,1522-1540; use bounded plugin-owned family/request receipts. |
| 14,27 | Private view/ScreenPass types exist without the 5.1 handoff | SceneRendering.h:865; SceneView.h:880,883. Preserve internal/output rect mapping. |
| 15 | OnWorldTickEnd is absent | LevelTick.cpp:1630,1690-1694 places PostActorTick after tickables under its actor-tick guard. Pixel validation remains mandatory. |
| 17 | The private engine-fork tick pin is absent on stock 4.25 | PHASE1 §C.3; standard FApp pacing survives. Report pin compiled out, not passed. |
| 18 | Default-AA lookup and temporal-upscaler interfaces differ | SceneView.cpp:199,784-873; record the actual view method, not an assumed CVar value. |
| 25-26 | Scene-texture parameter binding differs; CalcSceneCustomStencil is absent | SceneRenderTargetParameters.h:19-106,138; DeferredShadingCommon.ush:635,704 supplies the swizzled stencil-load idiom. |
| 31 | Nanite APIs are absent | Preserve Injector's existing stock-4.25 classifier; Capture must not replace or unregister it. |
| 32 | Streaming/resource telemetry differs | UStreamableRenderAsset exists, but the needed 5.1 resource-state methods do not. Texture.h:837 and TextureResource.h:171-219 expose older accessors. Defer this surface. |
| 33 | Skeletal identity accessor differs | SkinnedMeshComponent.h:210; reuse the existing compatibility helper. |
| 36 | Required public WebSocket server interfaces are absent | The installed plugin has private concrete classes. ControlServer is not a Capture dependency and remains disabled. |

**Compile measurement:** not attempted. Enabling the two modules requires changing hard-coded rules in an isolated source/target copy and routing generated files, response files, link outputs and UBT ancillary writes away from the bound host. The available qualified target writes that host; installed-engine UBT also normally writes its user XML cache. Establishing scratch isolation would exceed this optional lean diagnostic. There are **no measured compiler errors to report**; the table contains source incompatibilities. No native tests were run for this documentation-only change. Fourteen precise source anchors, document links and the 36-row inventory check passed; `git diff --check` passed. [Validation receipt](D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/plan-validation09430.json).

| Stage | Concrete home gate | Estimate |
|---|---|---|
| C1: plugin frames | Editor/PIE and standalone `-game` boot variants on EA_Core/EA_Travel; each requests 60 plugin PNGs at native 1280x720, within 90 s. Exact count, valid fixture pixels, no black frames, duplicates, unknown formats or crash. This is uncooked game-mode coverage, not a packaged-build claim. | 2-3 engineer-days |
| C2: labels | One bounded session tests both tick orders and at least three complete events per anomaly per order. Independently observed pixels must match every label and annotation index at apply, revert and blink-toggle edges: **zero mismatched frames**. A one-frame label-shift mutation must fail. | 2-4 days |
| C3: masks | One bounded session verifies shader load, paired frame identity, target tags, masks and restoration for all four anomalies. Proposed per-frame thresholds: IoU >=0.90, recall >=0.95, <=1% spill outside a one-pixel target boundary. A shifted-mask mutation must fail. | 3-5 days |

C2 preserves m40's pixel-set reasoning and frozen blink separation ratio. GPU completion order never selects a label; immutable run/world/request/session identity does. The stock PostActorTick argument does not qualify paused/TimeOnly ticks or Europa's fork. C3 distinguishes the missing object's counterfactual silhouette from drawn pixels, and target masks from a light's wider illumination footprint. Thresholds are proposed, not measured, and cannot be lowered after seeing results. Throughput, network control, dashboard and texture parity remain later work. Total: **7-12 hands-on days**, excluding idle waits and unexpected driver/fork defects.

The C1 file list is below; paths are relative to `Source/AnomalyCapture` unless stated. All new routing uses exact 4.25 guards; existing 5.1 paths remain intact.

| Files | Planned change |
|---|---|
| AnomalyCapture.Build.cs | Enable non-Shipping 4.25 capture and its RHI/Slate dependencies; retain shader/private-renderer separation. |
| Source/AnomalyInjector/Public/Compat/AnomalyCapabilities.h; new Private/Compat/AnomalyCaptureFeatures.h | Separate colour availability from unqualified labels/masks and deferred capabilities. |
| Public/Compat/AnomalyCaptureBackend.h; new Private/Compat/AnomalyCaptureBackend425.{h,cpp} | Implement the existing backend seam, including explicit request ownership, failure and stop. |
| Private/AnomalyFrameCapturer.{h,cpp}; new Private/Compat/AnomalyReadback425.{h,cpp} | Port allocation, barriers, fencing and bounded mapping with actual row pitch/height. |
| Public/AnomalyCaptureSubsystem.h; Private/AnomalyCaptureSubsystem.cpp | Route 4.25 capture, bind the deferred sampler, refuse unsupported modes and drain safely. |
| New Private/Compat/AnomalyViewMetadata425.{h,cpp} | Record the selected view, rect, AA and family/request association using available hooks. |
| Private/AnomalySceneViewExtension, AnomalyMaskSceneViewExtension, AnomalySveCapturer, AnomalyChangeFamilyData — each .h/.cpp | Isolate unsupported 5.1 implementations and every call site; unavailable facades must not report success. |
| Private/AnomalyMaskMeasure, AnomalyCensus, AnomalyChangeStage — each .h/.cpp; AnomalyChangeMeasurement.cpp | Close the compiled dependency surface while keeping mask/census/change scoring unavailable in C1. |
| Private/AnomalyMeasurability.cpp; AnomalyCaptureModule.cpp | Retain stock non-Nanite classification and single provider ownership. |
| Private/AnomalyPreviewTee.{h,cpp} | Compile safely with the shared adapter; dashboard preview stays inactive. |
| New tools/europa/capture_425_driver.py and capture_425_reader.py; affected harness | Bounded capture gate, pixel/file checks, Python 2.7 and keyword-Rotator checks for changed scripts. |

Existing encoders, writers, stencil ledger, key ring and pure label policies are retained unless the first enabled compile identifies a specific incompatibility. Shaders and ControlServer remain disabled in C1. The plan names C3's shader-binding and USF changes separately.

The top five risks are:

1. Incorrect pitch, format or crop producing plausible corrupted frames.
2. Correct labels attached to the previous rendered frame.
3. Unsupported 5.1 APIs remaining reachable when Capture is enabled.
4. Stencil ownership or internal/output-resolution errors in masks.
5. Treating stock single-view success as temporal-AA or Europa-fork qualification.

Commit: `feat/europa-port` **`4f46a9c12556af900928f240ed260336e30ce6ee`**, four documentation files. W4 author/committer overrides were removed; both dates are **2026-10-31 09:04:15 +05:30**. No push or tags. No native code, Content, Config, descriptor or binary was changed; no engine, protected checkout or machine setting was written. New exe/DLL hashes and archives: none. Bench levers/settings changed: none. Optional compile omitted for the stated lean-scope reason; no other deviation.

VERDICT: GREEN | Phase C planning complete and committed; capture implementation and real gates remain future work.
