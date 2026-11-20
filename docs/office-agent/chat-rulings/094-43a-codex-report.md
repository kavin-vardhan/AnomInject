<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, gpt-6-astra, max, danger-full-access resume, exit 0, 5439 s, 23:36:21-01:07:01 IST). Body below is byte-identical to report.md, sha256 F30C2D3B24DB9B37C6D6155F4E8949FE511E5FAEC96B62065361E80E0C484A61. -->
**094-43a: NEEDS-DECISION.** GC6 is committed. The name-lookup route links and draws the fixture materials, including hidden targets, but fails P3 on newly uncovered objects. GC4 stopped further runtime work before P4. The thresholds and reference rule remain unchanged.

GC6 persists UTC, PID and reason before termination. All **7 self-tests pass**. It also correctly classifies the real reference/01 exit125 as `owned-stop`, following intent for PID5600 at `2026-11-03T19:00:30.859332Z`; the genuine shader-startup exit3 remains `crash`. The owned stop followed an unreadable, already-exited UBT child. The guard was retained; no WinError5 recurred. [Tests and actual exits](D:/IA_Europa/Receipts/09443a/gc6-real-exits.json).

The fixture-only route uses:

- `FindShaderTypeByName` for `TDepthOnlyVS<true/false>` and `FDepthOnlyPS<false>`, then the material's rendering-thread map, vertex-factory mesh map and by-type lookup. The direct `FMaterial::GetShader` overload is private, so the public map chain is used. [Shader.h:2195](E:/EpicGames/UE_4.25/Engine/Source/Runtime/RenderCore/Public/Shader.h:2195), [MaterialShared.h:1764](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Public/MaterialShared.h:1764), [map lookup:1193](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Public/MaterialShared.h:1193).
- Engine fallback/default-material selection, base material/VF bindings, `FMeshPassProcessor` and `DrawDynamicMeshPass`. Opaque unmodified meshes use the default material; masked and WPO meshes retain their material maps. [DepthRendering.cpp:974](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Private/DepthRendering.cpp:974), [MeshMaterialShader.h:57](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Public/MeshMaterialShader.h:57), [MeshPassProcessor.inl:135](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Public/MeshPassProcessor.inl:135).
- Explicit component draws, including blink-hidden components, into private depth and ID targets from the existing view-family hook. The new route uses public static-mesh proxy access, no Renderer-private include/call, and no host custom-depth targets or setters. Existing global shaders accumulate IDs; no new material shader type is registered. [StaticMeshResources.h:831](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Public/StaticMeshResources.h:831). Scope is the fixture's plain static-mesh components/local vertex factory.

**P1 passes.** All builds used BelowNormal priority and `-MaxParallelActions=2`; no memory stop occurred.

| Target/attempt | Start free commit, GiB | Seconds | Result |
|---|---:|---:|---|
| Editor 1, link probe | 18.22 | 72.621 | Pass, 0 LNK errors |
| Editor 2, full pass | 17.27 | 33.636 | 6 AnomalyCapture C++ diagnostics, 0 LNK errors |
| Editor 3 | 18.21 | 48.163 | Pass, 0 LNK errors |
| Game 1 | 18.21 | 66.862 | Pass, 0 LNK errors |

The C++ fix used 4.25's public `RenderData`, raw RHI depth allocation and `MakeDepthStencilTargetActions`. One startup shader crash then exposed unsupported uint4 UAV loads. A shader-only fix used scalar R32_UINT scratch; the old/fixed FXC test and real warm-up retry passed. Native Source and all seven bound binaries remained identical during that shader-only retry. Editor's three-build budget is spent.

**P2 passes:** the second successful warm launch logged **0 `Missing cached shader map` lines**, zero pending jobs and normal exit. No experimental material shader type was present.

**P3 fails.** Reference/02 and private-off/01 each produced **2,352 frames**, 69 events and normal exit. The fixed reader reports:

| Object | Nonempty individual pairs | Minimum IoU | Maximum differing pixels |
|---|---:|---:|---:|
| Floor, opaque | 1,844 | 0.9999235 | 27 |
| Hide, opaque | 1,844 | 1.0000000 | 0 |
| LOD_A, opaque | 1,844 | 1.0000000 | 0 |
| LOD_B, opaque | 1,844 | 1.0000000 | 0 |
| Masked | 91 | **0** | **4,700** |
| WPO | 1,688 | **0** | **4,962** |

Masked fails on **7 frames**, WPO on **12**. All **132 compared hidden-blink frames** are nonempty and match. Twenty-six shared lighting-group pairs match; another **26 frames/208 owner records** cannot be paired per object. The empty DefaultPawn result is not counted as nonempty coverage. [Full measurements](E:/IA_Fixtures/Outline425/Receipts/09443a/P3-coverage.json).

The exact observed blocker is visibility timing: at [frame 708](E:/IA_Fixtures/Outline425/Receipts/09443a/private-off/01/plugin-frames/epoch01/frame-000708.png), the private ID contains 4,962 WPO pixels while scene depth is zero there and both colour captures lack the object. It appears at frame709; 4,538 pixels change beyond one level. Frame815 has the corresponding 4,700-pixel masked failure. Engine temporal-occlusion admission is the source-supported explanation, not directly instrumented query telemetry: it consumes earlier query results before admitting base/custom-depth draws. The immediate private pass bypasses that admission. `PrimitiveVisibilityMap` belongs to Renderer-private `FViewInfo`; no private-header workaround was added. [SceneVisibility.cpp:930](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Private/SceneVisibility.cpp:930), [SceneRendering.h:877](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Private/SceneRendering.h:877), [pixel diagnosis](E:/IA_Fixtures/Outline425/Receipts/09443a/P3-pixel-diagnosis.json).

**P4: not run**, following GC4's stop on P3 failure. No zero-magenta claim or frame746-equivalent proof is made.

**P5 passes for private-off:** 14 candidates restored exactly; all 2,352 samples have **0 enabled custom-depth components, 0 tagged, 0 unexpected**. Three runtime `r.CustomDepth` reads remain **1**. Source audit confirms no host setter and `DepthWrite_StencilNop`; the receipt's stencil-write field is declarative, not an independent GPU counter.

| Private-pass cost, ms/frame | Mean | p95 |
|---|---:|---:|
| GPU timestamp interval | 0.485808 | 0.627450 |
| Render-thread submission | 0.213419 | 0.332095 |
| Submission plus query wait | 2.238676 | 2.555955 |

These 2,352 samples exclude game-thread preparation and metrics-file writing; total game-frame overhead remains unmeasured. [State and timings](E:/IA_Fixtures/Outline425/Receipts/09443a/P5-cost-private-off.json).

All six fixture launches used the armed kit, D3D11 and `-nowrite`. Runtime-only settings were AA0, exposure0, motion blur0, screen percentage100, dynamic resolution0, FPS30, seed4242, four routes and the fixed-view bench floor0.5. The legacy reference alone bypassed the host-reader guard; private-off used `-IAIDepthOnlyByName`. Blendable0 was not saved. Capture state restored on normal exit. No machine/security settings or foreign processes were changed; no manual deletion occurred. P4's omission, the public-map substitution, shader-only native reuse, limited fixture scope and timing scope are the reported deviations/limits.

**Crashes: 1**, the corrected shader-startup failure. Separately, one owned stop. **Uploads: none. Protected changes: none.** Paris was never launched or built. Exit checks preserve m51, protected refs, all27 Paris bound files, seven fixture assets and Saved configuration. D:60.843GiB/E:111.977GiB free. [Exit receipt](D:/IA_Europa/Receipts/09443a/exit.json).

Seven final binaries were archived and rehashed. Archive root: `E:/IA_Fixtures/Outline425/Receipts/09443a/binary-archive`; H means `Binaries/Win64`, P means `Plugins/AnomalyInjector/Binaries/Win64` beneath it. [Exact paths and hashes](D:/IA_Europa/Receipts/09443a/binary-archive.json).

| Archived binary | SHA-256 |
|---|---|
| H/Outline425.exe | `c69ebe63feeca7d439b4a01a40ed4284caea258644aaba28401f89b38c311037` |
| H/UE4Editor-Outline425.dll | `86156bfc506d934efe83499621997fbfd48290f74983d48a9cddb35944ec481f` |
| P/UE4Editor-AnomalyBench.dll | `0a74f35993eddb1406901e4b240360d67c5239cfc5a80ac0d6bcd38517b25e12` |
| P/UE4Editor-AnomalyCapture.dll | `f56cc2393a0591632317280675b2404c809a4d9750429bcf2e5c84af52a3dcfc` |
| P/UE4Editor-AnomalyControlServer.dll | `b84088b69fe7b66adcd79086941bd8374cbb9beebdc63f959113fb24fdc13b40` |
| P/UE4Editor-AnomalyInjector.dll | `fede3315dd36fbe22c4c32537fce9d40883e4b08d722827a540775800f17b02c` |
| P/UE4Editor-AnomalyShaders.dll | `10b0f03d2c3830aaee29ef0b9c458593c6316651f809f11ece780bf49baa50fe` |

Intermediate DLLs were superseded by authorized rebuilds, without separate archives. Unchanged engine executables remain installed, not archived:

| Artifact | SHA-256 |
|---|---|
| Editor1 AnomalyCapture.dll | `5da45d424915e6147ebe3435fbb99acb114ae62c189f140e4e5e7c2cae13d3dd` |
| Editor1 AnomalyShaders.dll | `eddc45d93166ce8a3669128c5e4a4557dcf5027557f2b0e09b9194e8dae8d83c` |
| Editor2 AnomalyShaders.dll | `833e0d0fe1917484fa3d67024a911531dc88dcbb2b48e9a63701a152b0c2fb09` |
| UE4Editor.exe | `4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387` |
| CrashReportClient.exe | `f5c00f65e6dc4b2d3f7182f5efb41b481f8f9b2128f23a956f77397f54bd5449` |

Feature commits: `576975d` GC6, `7688ddf` guarded fixture scope, final **`75ca580bedb72f4db0ac7937107b71336fe866d0`** W4/evidence. New local spike: `spike/europa-09443a-depthonly-byname` at `c1193bdc386cc77bccb7c6c756a6f6fe28f195d0`, native build `1cf84a0`. Old spike remains `740220c`; origin remains `d1cdc7b`. Both worktrees are clean. **No merge, push or tags.** [Evidence index](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09443a-depthonly-receipts.json), [final Git receipt](D:/IA_Europa/Receipts/09443a/final-git.json).

VERDICT: NEEDS-DECISION | Name lookup links, but P3 visibility parity fails; P4 stopped under GC4. GC6 is committed.
