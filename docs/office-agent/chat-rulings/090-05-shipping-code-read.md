# 090-05 — Shipping-only code read (no Shipping build), full table

Read on 2026-09-30 by a read-only sub-agent, reviewed by Code. Tree A = `fix/m52-label-timing` at `8d78bfc`
(`E:\IA_BuildCache\_r84_host\StackOBot\Plugins\AnomalyInjector`), tree B = `feat/m53-uv-normal-corruption` at `fee8c7a`
(read before the 090-05 merge; the merge `2d5a5ea` inherits tree A's layout, so R1–R4 carry over unchanged). Line numbers
are tree A unless marked B. Macros: `ANOMALY_CAPTURE`, `ANOMALY_CONTROL_SERVER`, `ANOMALY_SHADERS` are 0 in Shipping
(`*.Build.cs`); `AnomalyInjector` / `AnomalyBench` define none; `AnomalyBench` is denied in Shipping by the `.uplugin`; the
bench gate is a runtime check (`AnomalyBenchGate::IsEnabled` returns false under `UE_BUILD_SHIPPING`). No `#elif`, `#ifdef`
or compound Shipping condition exists in either tree. Shipping sets `NO_LOGGING=1` (UE_LOG still compiles its arguments),
C4101 and C4150 are errors, C4100/C4189/C4702 disabled.

| # | file:line | condition | Shipping-side body | identifiers → where declared | verdict |
|---|---|---|---|---|---|
| A1 | AnomalyCaptureModule.cpp:13 (#else :16) | `ANOMALY_CAPTURE` → #else | `UE_LOG(LogAnomalyCapture, …compiled out…)` | `UE_LOG` via AnomalyCaptureLog.h; `LogAnomalyCapture` decl/DEFINE; `IMPLEMENT_MODULE` → ModuleManager.h | OK |
| A2 | AnomalyCaptureModule.cpp:23–25 | `ANOMALY_CAPTURE` | empty (the 090-05 probe reset); unguarded `UE_LOG` | as A1; AnomalyViewport.h / AnomalyMeasurability.h included unconditionally, the latter's body is empty in Shipping | OK |
| A3 | 28 whole-file wrappers (AsyncWriter, CaptureLetterbox, Census, ChangeFamilyData, ChangeMeasurement, ChangeStage, FrameCapturer, LabelWriter, MaskMeasure, MaskSceneViewExtension, Measurability, PreviewTee, SceneViewExtension, StencilTag, SveCapturer, SveKeyRing; .h and .cpp) | `ANOMALY_CAPTURE` | only the lines above the `#if` (CoreMinimal / the file's own header) | Core only; AnomalyLabelWriter.h needs only its own includes, all visible in Shipping | OK (but they remove what R1–R4 need) |
| A4 | AnomalyMeasurability.cpp:31–36 | `WITH_EDITORONLY_DATA` | skips `bDisplayNaniteFallbackMesh` | inside the file's `#if` | OK |
| A5 | AnomalyCaptureSubsystem.h:572–575 | `WITH_EDITOR` | two members absent | referenced only inside `WITH_EDITOR` blocks | OK |
| **R1** | AnomalyCaptureSubsystem.cpp:14–88 [B:14–94] | `ANOMALY_CAPTURE` | ~65 includes dropped | unguarded code needs `FAnomalyAsyncWriter`, `AnomalyStencilTag::`, complete `UAnomalyAutoInjectorSubsystem` / `FAutoLiveFireInfo`, `UAnomalyInjectorSubsystem`, `AnomalyStuckMipWindow::`, `FAnomalyMaskRecord`, AnomalyMaskTypes.h types, `AnomalyLabel::EAnomalyMaskState`, `FAnomalyChangeIssuePtr` / `FAnomalyChangeMaskPtr`, complete `FAnomalyPreviewTee` / `FAnomalyRunLog` (destructor :596), `AStaticMeshActor`, `UStaticMesh`, `UStaticMeshComponent`, `UMeshComponent`, `TActorIterator`, `APostProcessVolume`, `APlayerController`, `APlayerCameraManager`, `FMaterialResource`, `FFileHelper` | **RISK** |
| **R2** | AnomalyCaptureSubsystem.cpp:197–439 [B:204–440] | `ANOMALY_CAPTURE` | `FAnomalyCaptureAsyncState` is `{ }` | unguarded `Async->Writer/MaskMeasure/MaskExtension/ChangeStage/Trails/DeferredMasks/MaskGatedTags/RenderResultBySI/InheritedMaskRecords`, nested `FStuckTrail` / `FRenderEventResult` / `FDeferredMask` | **RISK** |
| **R3** | AnomalyCaptureSubsystem.cpp:442–586 [B:443–579] | `ANOMALY_CAPTURE` | anonymous-namespace constants absent | `GMaskPairingProbeTag` / `PosA` / `PosB` used at 1326, 1540, 2165–2207 | **RISK** |
| A6 | :90–191 | `ANOMALY_CAPTURE` | statics absent | none used by Shipping code | OK |
| A7 | :120, :133 (nested) | `ANOMINJECT_FW_TICKPIN`, `FW_BUILD_…` | dead in Shipping | none | OK |
| A8 | :608–633 | `ANOMALY_CAPTURE` | mask-reduce helpers absent | unreferenced | OK |
| A9 | :638 (#else :842) | `ANOMALY_CAPTURE` → #else | `UE_LOG` in Initialize | `LogAnomalyCapture`, `Super` | OK |
| A10 | :85–87, :834–841, :893–902 | `WITH_EDITOR` | dead | none | OK |
| A11 | 19 function bodies (Deinitialize, Tick, OnWorldTickEnd*, RunStencilHygieneCheck, OnEndFrameMaskSample, StepBenchForceTagCollision, GatherAnomalySwapMaterials, BenchForceAnomalyShaderRecompile, PrewarmAnomalyShaders, SetLabelsInDelivery, SetTickPin, Start/EndRunLog, SetRunLog, SetRunLogVerbose, BeginActualRun, StopRun) | `ANOMALY_CAPTURE` | empty | members, `Super` | OK |
| A12 | :2899 (#else :2916) | `WITH_EDITOR` nested | `UE_LOG(…Num())` | `TArray` | OK |
| A13 | #else bodies at :2819, :2843, :3228, :3333, :3787, :4203 | `ANOMALY_CAPTURE` → #else | literals / `UE_LOG` | Core | OK |
| A14 | :2098–2137, :2223–2365, :2651–2712 | `!UE_BUILD_SHIPPING` | 11 `SetBench*` definitions absent | declared unconditionally, no Shipping caller | OK |
| A15 | :3097–3117 | `!UE_BUILD_SHIPPING` | empty | none | OK (enclosing function fails under R1) |
| **R4** | :4227–10473 [B:4240–10719] | `ANOMALY_CAPTURE` | 74 member definitions absent | unguarded callers of `ResolveAuto`, `IsFireLabelledThisFrame`, `IsRenderTruthFire`, `IsRenderOrderPending` → unresolved externals | **RISK** (link) |
| A16 | :5342, :5352, :5381, :7865, :7942, :8738 | nested | dead | none | OK |
| A17 | AnomalyControlServerModule.cpp:11 (#else :13) | `ANOMALY_CONTROL_SERVER` → #else | `UE_LOG` | own log header | OK |
| A18 | AnomalyControlServerSubsystem.h:45–79 | `ANOMALY_CONTROL_SERVER` | private members absent | Shipping code touches only `bListening` | OK |
| A19 | AnomalyControlServerSubsystem.cpp:17–36 | `ANOMALY_CONTROL_SERVER` | includes/statics dropped | unguarded code uses only IConsoleManager / World / Core | OK |
| A20 | cpp:103, :118, :191 (#else) | `ANOMALY_CONTROL_SERVER` → #else | `UE_LOG` / `return false` | own log | OK |
| A21 | cpp:178–185, :217–254 | `ANOMALY_CONTROL_SERVER` | empty | members | OK |
| A22 | cpp:257–908 (+ nested `!UE_BUILD_SHIPPING`) | `ANOMALY_CONTROL_SERVER` | handlers absent | none | OK |
| A23 | Anomaly_ChangeCase.cpp:12–15 | `!UE_BUILD_SHIPPING` | cvar absent | used only in the #else of :18 | OK |
| A24 | Anomaly_ChangeCase.cpp:18 (#else :20) | `UE_BUILD_SHIPPING` | `return false;` | none | OK |
| A25 | Anomaly_StuckLowMip.cpp:36–38 | `!UE_BUILD_SHIPPING` | lever flag absent | used only inside guarded code | OK |
| A26 | Anomaly_StuckLowMip.cpp:355–365 | `!UE_BUILD_SHIPPING` | two definitions absent | declared only under the same guard; callers not compiled in Shipping | OK |
| A27 | Anomaly_StuckLowMip.cpp:461 (#else :463) | `UE_BUILD_SHIPPING` | `const bool bLegacyPurity = false;` | literal, read unguarded | OK |
| A28 | Anomaly_StuckLowMip.cpp:472–479, :635–649, :797–804, :1732–1898 | `!UE_BUILD_SHIPPING` | empty | none | OK |
| A29 | AnomalyBenchGate.cpp:14 (#else :16) | `UE_BUILD_SHIPPING` | `return false;` | none | OK |
| A30 | AnomalyBenchGate.cpp:25–35 | `!UE_BUILD_SHIPPING` | empty; returns empty `FString` | Core | OK |
| A31–A37 | AnomalyInjectorSubsystem.cpp :39–47, :191–197, :240–301, :410–412, :480 (#else), :654–670, :1160–1435 | `!UE_BUILD_SHIPPING` / `WITH_EDITOR` | bench latches, ChangeCase registrations, bench commands absent; `Label = TEXT("(no-label)")` | all uses inside the same guards; `FAnomaly_ChangeCase` still declared | OK |
| A38 | AnomalyStuckMipStats.h:57–61 | `!UE_BUILD_SHIPPING` | two declarations absent | see A26 | OK |
| A39–A40 | AnomalyShaders module + the two shader TUs | `ANOMALY_SHADERS` | Core-only TUs | Core | OK |
| B1 | B AnomalyCaptureSubsystem.cpp, the same four directives | as R1–R4 | as R1–R4 | as R1–R4, plus `IsTexCorruptWarmWanted` (B:2804–2813) needing `AnomalyTexCorrupt::IsTexCorruptId` (B:32) | **RISK** (4) |
| B2 | B :2798–2801 | `ANOMALY_CAPTURE` | empty | none | OK |
| B3 | B AnomalyCaptureModule.cpp:11 (#else :13) | `ANOMALY_CAPTURE` → #else | `UE_LOG` | own log | OK |
| B4 | B Anomaly_TexCorrupt.cpp, 13 `!UE_BUILD_SHIPPING` blocks | `!UE_BUILD_SHIPPING` | empty | everything read around them is unconditional | OK |
| B5–B7 | B TexCorruptBench.cpp :17–44, :48 (#else :50), :96–417 | `!UE_BUILD_SHIPPING` / `UE_BUILD_SHIPPING` | bench state / `(void)Context;` / bench commands absent | `RunCensus` defined unconditionally in TexCorruptTree.cpp | OK |
| B8 | B TexCorruptTree.cpp:158–163 | `WITH_EDITORONLY_DATA` | skips the fallback flag, then `UseNanite(...)` | RenderUtils.h / SceneInterface.h; m53's AnomalyInjector.Build.cs adds RenderCore and RHI in every config | OK |

**Total:** 140 branches (117 in A; 23 in B: 19 B-specific + 4 repeats of R1–R4) → **132 OK, 8 RISK = 4 distinct failures,
each in both trees**, all in the Shipping translation unit of `AnomalyCaptureSubsystem.cpp`.

**The RISK list with the unguarded functions** (tree A lines, [tree B]): destructor :596 [587] (incomplete
`FAnomalyPreviewTee` / `FAnomalyRunLog`, C4150); `EnqueueTargetMaskPng` :1280 [1287]; `FoldExposureExclusion` :1300 [1307];
`ReleaseTargetMaskSelfTags` :1386 [1393]; `EnsureMaskRecordsForCapturedFrame` :1403 [1410]; `UpdateMaskRecordLabelledWindow`
:1452 [1459]; `ArmTargetMaskOwn` :1470 [1477]; `ServiceTargetMask` :1624 [1631]; `SpawnMaskPairingProbe` :2139 [2146];
`StepMaskPairingProbe` :2195 [2202]; `StepBenchObservabilityLevers` :2466 [2454] (090-05's `IAI.Bench.RawRevertAt` body
lives here); `EnqueueCensusMaskDump` :2621 [2576]; `ScanHostPostProcessCustomDepthReaders` :2987 [2983]; B-only
`IsTexCorruptWarmWanted` B:2804. Plus the link-level callers of R4's members.

**Proposed fix (not applied):** wrap those bodies in `#if ANOMALY_CAPTURE` as their siblings (for example
`OnWorldTickEndMask`) already are, and make `FAnomalyPreviewTee` / `FAnomalyRunLog` complete in the Shipping TU (e.g. include
`AnomalyRunLog.h` unconditionally; it is Core-only). Verify with one Shipping compile of the plugin.

**Side observation, outside Shipping:** `AnomalySceneViewExtension.cpp:189` has a side effect inside
`check(!Data->Claim(View, false))`, which would be lost in a Test build (checks off, capture on). Code's reading: it sits
under `Stage->Gate(8, …)` right after a successful first `Claim`, so it asserts a second claim is refused; losing it drops a
self-check, not behaviour. Benign; recorded, not changed.
