VERDICT: NEEDS-DECISION

**2. Owner summary**

No office-ready commit is available to pull. Local candidate: `feat/m52-proxy-blur` at `e09fa9c79acfc80caca81c57c38f9f6d4d34d5ec`; it is **not pushed**.  
Production source changes add defining includes and availability guards only. Builds pass; the required home visual proof was blocked by the idle gate, so delivery is held.

**3. Include audit**

Ground truth: `D:\UESource\UnrealEngine` is UE 5.1.1; `E:\EpicGames\UE_5.7` is the read-only UE 5.7.4 header reference. The first office host's exact version is unknown. This job did not build, launch or modify anything under `E:\EpicGames`.

The reported source errors are addressed directly: `Anomaly_TexCorrupt.cpp` and `TexCorruptTree.cpp` include guarded `MaterialDomain.h`, retaining `MaterialShared.h`; the tree also includes `Engine/SkinnedAssetCommon.h`. In 5.7, `MaterialShared.h:90` only forward-declares the material-domain enum. The skeletal structs' definitions remain in `SkinnedAssetCommon.h` in both references. MSB3073 was the consequence of the compiler failures.

Audited all five modules: 126 m53 and 128 proxy `.cpp`/`.h` files, 6,567 UE 5.1 and 8,488 UE 5.7 Runtime Public/Classes headers, and the used Renderer private interfaces. Feature source diffs each contain **45 files, 317 added lines, 183 include directives from 47 headers, zero removals**. The fix branch has the shared class of dependency too: **38 files, 242 added lines, 138 includes**, all a subset of the table below, merged forward.

The table groups identical additions across files and enumerates every added include. Changed defining paths explain compatibility additions; unchanged paths supply complete definitions that previously depended on transitive includes. Existing stable Core aggregates and valid pointer/reference-only forward declarations remain. `HAL/CriticalSection.h` is the platform interface; its old Windows typedef location is shown. Stats locations cover compile-time configurations.

† Added inside `#if __has_include(...)`. ‡ Old `MaterialTypes.h` is selected only by `#else`: the 5.7 file is a deprecated stub, so four users select `Materials/MaterialParameters.h` when present. Other existing legacy includes are retained.

| File(s) under `Source/` | Type / symbol | Header(s) added | Defining location: UE 5.1 → UE 5.7 |
| --- | --- | --- | --- |
| `AnomalyBench/Private/AnomalyBenchModule.cpp`, `AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.cpp` | `FMinimalViewInfo` | `Camera/CameraTypes.h` | `Camera/CameraTypes.h:50` → `Camera/CameraTypes.h:36` |
| `AnomalyBench/Private/AnomalyBenchModule.cpp` | `FCollisionQueryParams` | `CollisionQueryParams.h` | `CollisionQueryParams.h:41` → `CollisionQueryParams.h:42` |
| `AnomalyBench/Private/AnomalyBenchModule.cpp`, `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`, `AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp` | `ELevelTick` | `Engine/EngineBaseTypes.h` | `Engine/EngineBaseTypes.h:68` → `Engine/EngineBaseTypes.h:69` |
| `AnomalyBench/Private/AnomalyBenchModule.cpp` | `FHitResult` | `Engine/HitResult.h` | `Engine/HitResult.h:19` → `Engine/HitResult.h:20` |
| `AnomalyCapture/Private/AnomalyAsyncWriter.cpp`, `AnomalyCapture/Private/AnomalyAsyncWriter.h`, `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`, `AnomalyCapture/Private/AnomalyChangeStage.cpp`, `AnomalyCapture/Private/AnomalyChangeStage.h`, `AnomalyCapture/Private/AnomalyFrameCapturer.cpp`, `AnomalyCapture/Private/AnomalyFrameCapturer.h`, `AnomalyCapture/Private/AnomalyLabelWriter.cpp`, `AnomalyCapture/Private/AnomalyLabelWriter.h`, `AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp`, `AnomalyCapture/Private/AnomalyMaskTypes.h`, `AnomalyCapture/Private/AnomalyPreviewTee.cpp`, `AnomalyCapture/Private/AnomalyPreviewTee.h`, `AnomalyCapture/Private/AnomalySceneViewExtension.cpp`, `AnomalyCapture/Private/AnomalySceneViewExtension.h`, `AnomalyCapture/Private/AnomalySveCapturer.cpp`, `AnomalyCapture/Private/AnomalySveCapturer.h`, `AnomalyCapture/Public/AnomalyCaptureSubsystem.h`, `AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptCore.h`, `AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp` | `ESPMode` | `Templates/SharedPointerFwd.h`†, `Templates/SharedPointerInternals.h` | `Templates/SharedPointerInternals.h:27` → `Templates/SharedPointerFwd.h:11` |
| `AnomalyCapture/Private/AnomalyAsyncWriter.h`, `AnomalyCapture/Private/AnomalyChangeStage.h`, `AnomalyCapture/Private/AnomalyFrameCapturer.cpp`, `AnomalyCapture/Private/AnomalyFrameCapturer.h`, `AnomalyCapture/Private/AnomalyLabelWriter.cpp`, `AnomalyCapture/Private/AnomalyLabelWriter.h`, `AnomalyCapture/Private/AnomalyMaskSceneViewExtension.h`, `AnomalyCapture/Private/AnomalyPreviewTee.h`, `AnomalyCapture/Private/AnomalySveCapturer.h`, `AnomalyCapture/Private/AnomalySveKeyRing.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptUniform.cpp` | `FCriticalSection` | `HAL/CriticalSection.h` | `Windows/WindowsCriticalSection.h:159` → `HAL/CriticalSection.h:53` |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`, `AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp`, `AnomalyInjector/Private/AnomalyHiddenClass.cpp` | `UActorComponent` | `Components/ActorComponent.h` | `Components/ActorComponent.h:116` → `Components/ActorComponent.h:151` |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `UPrimitiveComponent` | `Components/PrimitiveComponent.h` | `Components/PrimitiveComponent.h:213` → `Components/PrimitiveComponent.h:258` |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `USceneComponent` | `Components/SceneComponent.h` | `Components/SceneComponent.h:88` → `Components/SceneComponent.h:88` |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `FPostProcessSettings` | `Engine/Scene.h` | `Engine/Scene.h:688` → `Engine/Scene.h:692` |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`, `AnomalyCapture/Private/AnomalySveCapturer.cpp`, `AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp`, `AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptBench.cpp` | `UTexture` | `Engine/Texture.h` | `Engine/Texture.h:1097` → `Engine/Texture.h:1218` |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `FGeometry` | `Layout/Geometry.h` | `Layout/Geometry.h:38` → `Layout/Geometry.h:39` |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `ERHIFeatureLevel` | `RHIDefinitions.h` | `RHIDefinitions.h:237` → `RHIFeatureLevel.h:17` |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `ERHIFeatureLevel`, `GMaxRHIFeatureLevel` | `RHIFeatureLevel.h`† | `RHI.h:85`; `RHIDefinitions.h:237` → `RHIFeatureLevel.h:17,109` |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`, `AnomalyCapture/Public/AnomalyCaptureSubsystem.h`, `AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp`, `AnomalyControlServer/Public/AnomalyControlServerSubsystem.h`, `AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp`, `AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp`, `AnomalyInjector/Private/AnomalySelectorSubsystem.cpp`, `AnomalyInjector/Public/AnomalyAutoInjectorSubsystem.h`, `AnomalyInjector/Public/AnomalyInjectorSubsystem.h`, `AnomalyInjector/Public/AnomalySelectorSubsystem.h` | `TStatId` | `Stats/LightweightStats.h`†, `Stats/Stats.h`, `Stats/StatsSystemTypes.h`† | `Stats/Stats.h:116,209`; `Stats/Stats2.h:213` → `Stats/LightweightStats.h:19,415`; `Stats/StatsSystemTypes.h:76` |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`, `AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp`, `AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp`, `AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp`, `AnomalyInjector/Private/AnomalySelectorSubsystem.cpp` | `FSubsystemCollectionBase` | `Subsystems/SubsystemCollection.h` | `Subsystems/SubsystemCollection.h:14` → `Subsystems/SubsystemCollection.h:14` |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `TStrongObjectPtr` | `UObject/StrongObjectPtrTemplates.h`† | `UObject/StrongObjectPtr.h:72` → `UObject/StrongObjectPtrTemplates.h:25` |
| `AnomalyCapture/Private/AnomalyCensus.cpp`, `AnomalyCapture/Private/AnomalyMeasurability.cpp` | `EShaderPlatform` | `RHIDefinitions.h`, `RHIShaderPlatform.h`† | `RHIDefinitions.h:80` → `RHIShaderPlatform.h:10` |
| `AnomalyCapture/Private/AnomalyChangeMeasurement.cpp`, `AnomalyCapture/Private/AnomalyChangeStage.cpp` | `FJsonValue` | `Dom/JsonValue.h` | `Dom/JsonValue.h:19` → `Dom/JsonValue.h:21` |
| `AnomalyCapture/Private/AnomalyChangeStage.cpp` | `FSceneInterface` | `SceneInterface.h` | `SceneInterface.h:53` → `SceneInterface.h:105` |
| `AnomalyCapture/Private/AnomalyFrameCapturer.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp` | `FThreadSafeCounter` | `HAL/ThreadSafeCounter.h` | `HAL/ThreadSafeCounter.h:13` → `HAL/ThreadSafeCounter.h:13` |
| `AnomalyCapture/Private/AnomalyFrameCapturer.cpp` | `ERHIAccess`, `FRHICopyTextureInfo`, `FRHITransitionInfo` | `RHI.h` | `RHI.h:1042,1966,2132` → `RHIAccess.h:10`; `RHICommandList.h:182`; `RHITransition.h:118` |
| `AnomalyCapture/Private/AnomalyFrameCapturer.cpp` | `ERHIAccess` | `RHIAccess.h`† | `RHI.h:1042` → `RHIAccess.h:10` |
| `AnomalyCapture/Private/AnomalyFrameCapturer.cpp`, `AnomalyCapture/Private/AnomalySveCapturer.cpp` | `FRHIGPUTextureReadback` | `RHIGPUReadback.h` | `RHIGPUReadback.h:111` → `RHIGPUReadback.h:138` |
| `AnomalyCapture/Private/AnomalyFrameCapturer.cpp` | `FRHITransitionInfo` | `RHITransition.h`† | `RHI.h:2132` → `RHITransition.h:118` |
| `AnomalyCapture/Private/AnomalyFrameCapturer.h` | `ERHIAccess / FRHIResourceInfo (RHIResources prerequisite)` | `RHI.h` | `RHI.h:1042,1654` → `RHI.h:542`; `RHIAccess.h:10` |
| `AnomalyCapture/Private/AnomalyFrameCapturer.h` | `FTextureRHIRef` | `RHIFwd.h`†, `RHIResources.h` | `RHIResources.h:2315` → `RHIFwd.h:131` |
| `AnomalyCapture/Private/AnomalyLabelWriter.cpp`, `AnomalyCapture/Private/AnomalySveCapturer.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptCore.h`, `AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp` | `EPixelFormat` | `PixelFormat.h` | `PixelFormat.h:15` → `PixelFormat.h:15` |
| `AnomalyCapture/Private/AnomalyMaskMeasure.cpp` | `GMaxRHIShaderPlatform` | `RHIShaderPlatform.h`† | `RHI.h:86` → `RHIShaderPlatform.h:86` |
| `AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp`, `AnomalyCapture/Private/AnomalySceneViewExtension.cpp` | `FPostProcessMaterialInputs` | `PostProcess/PostProcessMaterialInputs.h`† | `PostProcess/PostProcessMaterial.h:59` → `PostProcess/PostProcessMaterialInputs.h:44` |
| `AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp` | `FClearValueBinding` | `RHI.h`, `RHIResources.h` | `RHI.h:1697` → `RHIResources.h:245` |
| `AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptUniform.cpp` | `FRHICommandListImmediate` | `RHICommandList.h` | `RHICommandList.h:4071` → `RHICommandList.h:4625` |
| `AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp` | `FRHIGPUBufferReadback`, `FRHIGPUTextureReadback` | `RHIGPUReadback.h` | `RHIGPUReadback.h:88,111` → `RHIGPUReadback.h:115,138` |
| `AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptUniform.cpp` | `FRDGBufferRef`, `FRDGBufferUAVRef`, `FRDGTextureRef` | `RenderGraphDefinitions.h`, `RenderGraphFwd.h`† | `RenderGraphDefinitions.h:743,765,773` → `RenderGraphFwd.h:13,21,39` |
| `AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptUniform.cpp` | `FRDGBufferDesc` | `RenderGraphResources.h` | `RenderGraphResources.h:870` → `RenderGraphResources.h:939` |
| `AnomalyCapture/Private/AnomalyMeasurability.h` | `EShaderPlatform` | `RHIShaderPlatform.h`† | `RHIDefinitions.h:80` → `RHIShaderPlatform.h:10` |
| `AnomalyCapture/Private/AnomalySceneViewExtension.cpp` | `FClearValueBinding`, `FResolveRect` | `RHI.h`, `RHIResources.h` | `RHI.h:1697,1874` → `RHIResources.h:245,5208` |
| `AnomalyCapture/Private/AnomalySceneViewExtension.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp` | `FRDGTextureRef` | `RenderGraphDefinitions.h`, `RenderGraphFwd.h`† | `RenderGraphDefinitions.h:765` → `RenderGraphFwd.h:39` |
| `AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp` | `ERHIFeatureLevel` | `RHIDefinitions.h`, `RHIFeatureLevel.h`† | `RHIDefinitions.h:237` → `RHIFeatureLevel.h:17` |
| `AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp` | `FObjectKey` | `UObject/ObjectKey.h` | `UObject/ObjectKey.h:11` → `UObject/ObjectKey.h:18` |
| `AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp` | `MD_Surface` | `MaterialDomain.h`†, `MaterialShared.h` | `MaterialShared.h:285` → `MaterialDomain.h:12` |
| `AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp` | `FMaterialParameterInfo`, `FHashedMaterialParameterInfo`, `EMaterialParameterAssociation` | `MaterialTypes.h`‡, `Materials/MaterialParameters.h`† | `MaterialTypes.h:20,28,180` → `Materials/MaterialParameters.h:24,32,183` |
| `AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptBench.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp` | `UMaterialInterface` | `Materials/MaterialInterface.h` | `Materials/MaterialInterface.h:216` → `Materials/MaterialInterface.h:295` |
| `AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.h`, `AnomalyInjector/Private/Anomalies/TexCorruptCore.h` | `FMaterialParameterInfo` | `MaterialTypes.h`‡, `Materials/MaterialParameters.h`† | `MaterialTypes.h:28` → `Materials/MaterialParameters.h:32` |
| `AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp` | `UTexture`, `TextureAddress` | `Engine/Texture.h` | `Engine/Texture.h:65,1097` → `Engine/Texture.h:1218`; `Engine/TextureDefines.h:495` |
| `AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp` | `TA_Wrap`, `TA_Clamp` | `Engine/TextureDefines.h` | `Engine/Texture.h:67,68` → `Engine/TextureDefines.h:497,498` |
| `AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp` | `FRHICopyTextureInfo`, `FRHICommandListImmediate` | `RHICommandList.h` | `RHI.h:1966`; `RHICommandList.h:4071` → `RHICommandList.h:182,4625` |
| `AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp` | `FTextureRHIRef` | `RHIFwd.h`† | `RHIResources.h:2315` → `RHIFwd.h:131` |
| `AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp` | `FSkeletalMeshLODInfo`, `FSkeletalMaterial (GetMaterials element)` | `Engine/SkinnedAssetCommon.h` | `Engine/SkinnedAssetCommon.h:92,228` → `Engine/SkinnedAssetCommon.h:128,369` |
| `AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp`, `AnomalyInjector/Private/Anomalies/TexCorruptUniform.cpp` | `UTexture`, `FTexturePlatformData` | `Engine/Texture.h` | `Engine/Texture.h:763,1097` → `Engine/Texture.h:838,1218` |
| `AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp` | `MD_Surface` | `MaterialDomain.h`† | `MaterialShared.h:285` → `MaterialDomain.h:12` |
| `AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp` | `FShader`, `FShaderType` | `Shader.h` | `Shader.h:817,1165` → `Shader.h:828,1237` |
| `AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp` | `FVertexFactoryType` | `VertexFactory.h` | `VertexFactory.h:334` → `VertexFactory.h:313` |
| `AnomalyInjector/Private/Anomalies/TexCorruptUniform.cpp` | `GMaxRHIFeatureLevel` | `RHI.h`, `RHIFeatureLevel.h`† | `RHI.h:85` → `RHIFeatureLevel.h:109` |
| `AnomalyInjector/Private/Anomalies/TexCorruptUniform.cpp` | `FRHITexture` | `RHIResources.h` | `RHIResources.h:1692` → `RHIResources.h:2149` |
| `AnomalyInjector/Private/AnomalyViewport.cpp` | `FCollisionShape` | `CollisionShape.h` | `CollisionShape.h:20` → `CollisionShape.h:20` |

The shader/free-function/macro audit found no further moved definition needing an include. Existing `GlobalShader.h`, `ShaderParameterStruct.h` and `RenderingThread.h` interfaces remain valid. `ScreenPass.h` and `SceneTexturesConfig.h` still resolve under the same include spelling despite Public/Private moves. Lexical false positives, string literals and plugin-owned types were excluded after inspection. The full 183-row audit, exact paths and scope notes are retained in [full-include-audit.json](E:/IA_BuildCache/_r912b/full-include-audit.json), [full-include-audit.md](E:/IA_BuildCache/_r912b/full-include-audit.md) and [audit-scope.md](E:/IA_BuildCache/_r912b/audit-scope.md).

**4. Verification and §0**

All builds used the detached plugin checkout in `E:\IA_BuildCache\_r53_host`, with UE 5.1.1.

| Candidate / check | Editor errors / warnings | Game errors / warnings | Result |
| --- | --- | --- | --- |
| Fix source `c2d05ad`, normal | 0 / 0 | 0 / 0 | PASS |
| m53 source `869db52`, normal | 0 / 0 | 0 / 0 | PASS |
| Proxy source `0c6a558`, strict | 0 / 0 | 0 / 0 | PASS; 68 independent header TUs per target |
| Proxy source `0c6a558`, restored normal | 0 / 0 | 0 / 0 | PASS; strict edits restored |
| Final proxy head `e09fa9c`, normal | 0 / 0 | 0 / 0 | PASS; source tree and all six binary hashes unchanged |

The first strict candidate (`297ee97`) **failed**, with 108 errors per target: legacy `RHIResources.h` needs `RHI.h` first. That prerequisite was added to `AnomalyFrameCapturer.h`; the complete strict run was repeated and passed. The rejected logs and its successful restoration/normal builds remain in `strict-297ee97`. An early PowerShell wrapper also reported exit 1 because successful Git checkout wrote to stderr; the underlying normal builds were 0/0, and the corrected wrapper/final checks passed.

Lever audit: **53 names, 53 registrations, 0 failures**, scanning the executable and five editor DLLs. The **12-case selftest** behaved as expected, including rejected mutants. No bench lever was added; Shipping exclusions and `-IAIBench` gates are unchanged.

String scan versus executable baseline `CFFD276F` and its matching five DLLs: printable ASCII and UTF-16 strings of at least six characters. **Two added ASCII sequences, zero removed strings, zero UTF-16 changes** across all six files. `RSDSeu` in the executable and `RSDSA8Hs#` in the Injector DLL start at PE CodeView records: new PDB GUID bytes only. **Zero new semantic strings.** A self-comparison produced zero differences; an actual copied binary with planted ASCII and UTF-16 semantic strings was rejected in both encodings. It was never executed.

Source proof: each diff operation is an insertion containing only includes or balanced `__has_include`/`#else`/`#endif` blocks. No guard can surround an old line. Removing the added blocks reconstructs every original source line. Ordinary statements, macro definitions, comments and guards spanning old code fail the check. The canonical comment stripper leaves every touched source byte-identical. No function body, API, shader, asset, configuration, Build.cs or product string was edited. The requested documentation adds three files' worth of status/gotcha/journal changes; the complete feature diff is 48 files / 399 insertions.

`git diff --stat 51e2fdb HEAD -- Source` on final proxy head:

```text
 Source/AnomalyBench/Private/AnomalyBenchModule.cpp |  4 ++++
 .../AnomalyCapture/Private/AnomalyAsyncWriter.cpp  |  4 ++++
 Source/AnomalyCapture/Private/AnomalyAsyncWriter.h |  5 +++++
 .../Private/AnomalyCaptureSubsystem.cpp            | 26 ++++++++++++++++++++++
 Source/AnomalyCapture/Private/AnomalyCensus.cpp    |  4 ++++
 .../Private/AnomalyChangeMeasurement.cpp           |  1 +
 .../AnomalyCapture/Private/AnomalyChangeStage.cpp  |  6 +++++
 Source/AnomalyCapture/Private/AnomalyChangeStage.h |  5 +++++
 .../Private/AnomalyFrameCapturer.cpp               | 14 ++++++++++++
 .../AnomalyCapture/Private/AnomalyFrameCapturer.h  | 10 +++++++++
 .../AnomalyCapture/Private/AnomalyLabelWriter.cpp  |  6 +++++
 Source/AnomalyCapture/Private/AnomalyLabelWriter.h |  5 +++++
 .../AnomalyCapture/Private/AnomalyMaskMeasure.cpp  |  3 +++
 .../Private/AnomalyMaskSceneViewExtension.cpp      | 16 +++++++++++++
 .../Private/AnomalyMaskSceneViewExtension.h        |  1 +
 Source/AnomalyCapture/Private/AnomalyMaskTypes.h   |  4 ++++
 .../Private/AnomalyMeasurability.cpp               |  4 ++++
 .../AnomalyCapture/Private/AnomalyMeasurability.h  |  3 +++
 .../AnomalyCapture/Private/AnomalyPreviewTee.cpp   |  4 ++++
 Source/AnomalyCapture/Private/AnomalyPreviewTee.h  |  5 +++++
 .../Private/AnomalySceneViewExtension.cpp          | 13 +++++++++++
 .../Private/AnomalySceneViewExtension.h            |  4 ++++
 .../AnomalyCapture/Private/AnomalySveCapturer.cpp  |  7 ++++++
 Source/AnomalyCapture/Private/AnomalySveCapturer.h |  5 +++++
 .../AnomalyCapture/Private/AnomalySveKeyRing.cpp   |  1 +
 .../Public/AnomalyCaptureSubsystem.h               | 11 +++++++++
 .../Private/AnomalyControlServerSubsystem.cpp      |  8 +++++++
 .../Public/AnomalyControlServerSubsystem.h         |  7 ++++++
 .../Private/Anomalies/Anomaly_CameraClipping.cpp   |  1 +
 .../Private/Anomalies/Anomaly_StuckLowMip.cpp      |  7 ++++++
 .../Private/Anomalies/Anomaly_TexCorrupt.cpp       | 15 +++++++++++++
 .../Private/Anomalies/Anomaly_TexCorrupt.h         |  5 +++++
 .../Private/Anomalies/TexCorruptBench.cpp          |  2 ++
 .../Private/Anomalies/TexCorruptCore.h             | 10 +++++++++
 .../Private/Anomalies/TexCorruptDraw.cpp           | 16 +++++++++++++
 .../Private/Anomalies/TexCorruptTree.cpp           | 14 ++++++++++++
 .../Private/Anomalies/TexCorruptUniform.cpp        | 13 +++++++++++
 .../Private/AnomalyAutoInjectorSubsystem.cpp       |  8 +++++++
 .../AnomalyInjector/Private/AnomalyHiddenClass.cpp |  1 +
 .../Private/AnomalyInjectorSubsystem.cpp           |  9 ++++++++
 .../Private/AnomalySelectorSubsystem.cpp           |  8 +++++++
 Source/AnomalyInjector/Private/AnomalyViewport.cpp |  1 +
 .../Public/AnomalyAutoInjectorSubsystem.h          |  7 ++++++
 .../Public/AnomalyInjectorSubsystem.h              |  7 ++++++
 .../Public/AnomalySelectorSubsystem.h              |  7 ++++++
 45 files changed, 317 insertions(+)
```

m53 versus `b8cd3cd`: the same 45 files / 317 insertions. Fix versus `12a6db8`: 38 files / 242 insertions. No other source line changed. Separate PE section comparisons show five rebuilt binaries are not machine-code-identical to CFFD276F; those comparisons are retained and are **not** used to claim runtime equivalence.

The final idle gate refused after **194 seconds**: foreign editor PID **45800**, GPU **34–36%**, human idle **67.5 seconds**, memory headroom **19.7 GiB**. Required limits were no foreign heavy GPU use, under 10% GPU, at least 900 seconds human idle for an open foreign editor, and at least 6 GiB headroom. The editor was untouched.

| Final proxy §0 leg | Fires | Visible change | Label/pixel alignment | Labelled frames without measured mask |
| --- | --- | --- | --- | --- |
| PIE `uv_corruption` | 0; NOT RUN | Unmeasured | Unmeasured | Unmeasured |
| PIE `normal_corruption` | 0; NOT RUN | Unmeasured | Unmeasured | Unmeasured |
| PIE shared `stuck_low_mip`, proxy | 0; NOT RUN | Unmeasured | Unmeasured | Unmeasured |
| Staged `uv_corruption` | 0; NOT RUN | Unmeasured | Unmeasured | Unmeasured |
| Staged `normal_corruption` | 0; NOT RUN | Unmeasured | Unmeasured | Unmeasured |
| Staged shared `stuck_low_mip`, proxy | 0; NOT RUN | Unmeasured | Unmeasured | Unmeasured |

The four planned m53 UV/normal PIE/staged legs also did not run. No zero-fire leg is counted as passing. Retained reader controls were rerun: identity rejected **8/8** events; mask negatives detected **2/56** and **1/56** missing masks. These calibrate readers and do not substitute for candidate proof. Candidate E1/FireWindow and shifted-label controls remain unrun.

Predictions preceded attempts. Copies of the 090-12 harness use the existing fixtures/package and unchanged pixel readers, requiring at least three visible fires, zero unexcused edge disagreement and zero unmasked labels. UV/normal schedule: 160-frame cap, `2 4 8 14 3`; shared proxy: 600-frame cap, `0 48 48 96 3`; AA off, seed 777, inherited masks/settings, auto route required to report proxy. Windows per-process GPU counters supplement an initially empty nvidia-smi reading. The copied launcher accepts a foreign editor only with a fresh valid idle receipt. The 180-second predeclared observation budget took 194 seconds including measurement overhead. No acceptance threshold was relaxed.

Settings changed/restored: **no runtime bench lever, fixture, staged file or device-profile change; zero engine launches**. Temporary strict Build.cs/header-TU changes were restored and verified. Nine original copied harness files retain their hashes. An inventory assertion was corrected from absent directories to empty directories because setup had created empty `receipts`/`out`; no launch or pixel criterion changed. No security setting or foreign process was changed. Final free space: D **60.42 GiB**, E **405.86 GiB**.

Evidence: [strict/normal summary](E:/IA_BuildCache/_r912b/strict-0c6a558/summary.json), [final head verification](E:/IA_BuildCache/_r912b/final-head-verification.json), [lever audit](E:/IA_BuildCache/_r912b/logs/lever-final.txt), [string resolution](E:/IA_BuildCache/_r912b/string-review-resolution.json), [source gate](E:/IA_BuildCache/_r912b/proxy-final-head-source-gate.json), [idle gate](E:/IA_BuildCache/_r912b/confirmation-initial-gate.json), [predictions](E:/IA_BuildCache/_r912b/predeclared.md).

**5. Commits, archives and hashes**

**Commits pushed: none, on any branch.** The brief requires the pushed m53 head to have passed in-engine proof, so the idle-gate block prevents pushing. Read-only remote verification confirms all three remotes remain at their starting commits.

| Branch | Local head, including documentation | Built source commit | Remote unchanged |
| --- | --- | --- | --- |
| `fix/m52-label-timing` | `8b86f4350353d700a600e07f8769633d64462887` | `c2d05add78a046050155fcdce3738d5a0cf0fdea` | `12a6db877654d47f5cb2ed3a5506d2468ee505a3` |
| `feat/m53-uv-normal-corruption` | `cf16766971c19ead05afe6eefc0378a1c98a8bb1` | `869db52914fc5e39da55a9ec76a492f60e73e8d1` | `b8cd3cda44b3d2f426ebac3958e66285db0b2842` |
| `feat/m52-proxy-blur` | `e09fa9c79acfc80caca81c57c38f9f6d4d34d5ec` | `0c6a558f90fa7744da880801b188e87de6252696` | `51e2fdb93a85d1b9ececf0efd6469cf29519af37` |

Fix → m53 → proxy ancestry is verified. Include conflicts were resolved by preserving both branches' required includes; documentation conflicts preserve existing status/history and append G460. All three worktrees are clean. The build host is clean and detached at `e09fa9c79acfc80caca81c57c38f9f6d4d34d5ec`. `CLAUDE.md`, G460 and `docs/sessions/2026-10-03-091-02-include-compat.md` are committed. m51, master and tags were not changed. Full local commit ledgers: [fix](E:/IA_BuildCache/_r912b/fix-commits.txt), [m53](E:/IA_BuildCache/_r912b/m53-commits.txt), [proxy](E:/IA_BuildCache/_r912b/proxy-commits.txt).

| Archive directory | SHA-256 of `archive-hashes.json` | Rehash |
| --- | --- | --- |
| `D:\IA_BankOverflow\_binary_baselines\09102-c2d05ad-55DD161B` | `2ba3ce37a74c3ceba83115919378b04ed46e6d9d1f43859db8102872194b54c5` | 6 / 6 |
| `D:\IA_BankOverflow\_binary_baselines\09102-869db52-FDE0A0F8` | `aa4552d23cccd512dfb88ae0a07e6180535e671f3ca81a579bb1ab94801ee495` | 6 / 6 |
| `D:\IA_BankOverflow\_binary_baselines\09102-0c6a558-A8F88ADD` | `d8263f4a80b7ac6e9e1d441cf2edb7fe33599b8e1eded88ed2ed1b0bfa82819d` | 6 / 6 |

Combined archive: [09102-final-binaries.zip](D:/IA_BankOverflow/09102-final-binaries.zip). **739,783,904 bytes**, all **18 binaries** rehashed from ZIP. SHA-256: `ccdd22969d00a47a7f7e7008a24a6da53516c7cd2fc29293ce1b8e41b4401ad8`. These archives are not office-cleared.

Every retained executable/DLL hash is listed below. The preliminary build is retained at `E:\IA_BuildCache\_r912b\builds\7edd24c`; it was not used for a bench leg. Final build folders are the corresponding three archive directories above. DLL names below expand to `UnrealEditor-Anomaly<name>.dll`; `exe` is `StackOBot.exe`.

| Build | File | SHA-256 |
| --- | --- | --- |
| `7edd24c` | `exe` | `39c5064b6996c5dc67e42d60ac96951759a004e6fed01a95a81474f783c6a035` |
| `7edd24c` | `Bench` | `73d51fd9edc439f817ad2f3447158787547914cfc4569d5ac5e526415076eaf2` |
| `7edd24c` | `Capture` | `e1e5d749f0faa2e6118fa448dfb0a82c577091f393aec311f35caa9deeff2aff` |
| `7edd24c` | `ControlServer` | `26ab30800f186a3c29886251f90aaae66cbd5dfe0b7743bc3a877cd1ced02412` |
| `7edd24c` | `Injector` | `a626e4ce038abd5df2c953d7348a9f5ef9e55420c73eaf752b996a50311bf88d` |
| `7edd24c` | `Shaders` | `62d3de5494e7c9b0d22dd6a6499ce94ed371c37741b995a0e8fc943b445a1f29` |
| `c2d05ad` | `exe` | `55dd161b0dc75f643783be93c4fa7b4f53bb2d40e16ef47258d9aebe86d8205c` |
| `c2d05ad` | `Bench` | `40e24e3b0d350cebc56b11dc843c1a6e36a50eab7b9a155661f3a00a6d8c2ec9` |
| `c2d05ad` | `Capture` | `c0189fec3697f4107ed6db3c83aa66d9f9266bb5d8e8f60085bc851a0556b860` |
| `c2d05ad` | `ControlServer` | `b01f0edca1a545e060ab2ab2ce40557b956b9a11233ccf63dab2300c75d119cd` |
| `c2d05ad` | `Injector` | `af6dce6cb176690a7c139a06b66e4e506ff7d2410059fb691b557e6015a3398f` |
| `c2d05ad` | `Shaders` | `75ceaf7b31a1650cf5fef5daf1e5724081f1d8ac3d313790250a738aae115236` |
| `869db52` | `exe` | `fde0a0f8d14d7bf264169b54bf127b365cf4358944b7777d77d83ee4a534b35b` |
| `869db52` | `Bench` | `aa61b210dc57f7e7f3524d4d3ae50b7edb93f01b46446059f2293ac47b530fe4` |
| `869db52` | `Capture` | `6a84daca6b47ad86d7a57c4a58438a729df35e8d488e28f3167bbfb1b8222a73` |
| `869db52` | `ControlServer` | `5a4e547b0a9f9088dcbec6d12e479907bfd9e936023f5e681823d145dafff2eb` |
| `869db52` | `Injector` | `ba753c875e485d9c1162fbaa55843e1de98fe5a8382aca7c1da5e9247a400b06` |
| `869db52` | `Shaders` | `11dffe3edf233dbacb520e7897c7a29b574896768fa320e97144b71f9e7a164b` |
| `0c6a558` | `exe` | `a8f88adde0f7ef825ff60a4ebf0c697390d9861955a2cd3c97ba039ac7071594` |
| `0c6a558` | `Bench` | `8be7195bb6ed62324b5e5df2ef916ee71d26e74767315c12356ed6af42609b89` |
| `0c6a558` | `Capture` | `818922d491cda25c23f6eea5c3ae68bb640166e3256de156b543db16eced41e8` |
| `0c6a558` | `ControlServer` | `61cde1a3aa8f3e4ffe9fbd17e1944eebd5c70610ad639928270e63324a709045` |
| `0c6a558` | `Injector` | `c2152c6b9c00615c360efa70edb648c1a9fff6447947fdd91dfac8a3d670a2a3` |
| `0c6a558` | `Shaders` | `916fb857d85c940c95745309fe8b70f69b6ed4e714243803ec5791a4e9a5acd9` |

**6. NEEDS-DECISION items and deviations**

- **Required:** a quiet bench window under the existing safety limits. Run m53's four confirmations and final proxy's six confirmations, each at least three fires, with the declared pixel/mask/edge controls; only then fast-forward-push the three prepared branches. The blocked §0 and withheld pushes are the unfinished requirements. No permission to bypass these requirements was assumed.
- The first office host's exact engine version remains unknown. The header audit proves the inspected 5.1/5.7 layouts; it does not claim an office build or a UE 5.7 compile.
- Additional validation comprised normal fix/m53 builds, binary-section comparison, and actual string-negative binaries. Harness changes were confined to copies, path/schedule selection and stricter GPU measurement/fresh receipts. No fixture was authored or cooked. The initial strict failure, wrapper artifact and empty-directory inventory correction are disclosed above; no failed candidate or unrun leg is presented as passing.

Full evidence/work folder: `E:\IA_BuildCache\_r912b`. Report saved to [report.md](D:/IntrusiveAnomalies/_relay/runs/2026-10-03-091-02-include-compat/report.md).

VERDICT: NEEDS-DECISION
