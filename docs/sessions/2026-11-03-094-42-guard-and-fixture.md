# 094-42: interim guard, routes and outline fixture

Authority: 094-41 FZ3-FZ6, 094-42 request, FM1/FJ1/FK1. Published baseline d1cdc7b stays in history. No push, tags or production private pass.

## Implementation

The scanner ports the three sources in 5.1's `ScanHostPostProcessCustomDepthReaders` at d7eee97 (the retained port body is `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:3251-3389`): post-process volumes, cached camera blends and camera POV. Stock 4.25 `Engine/Source/Runtime/Engine/Public/MaterialShared.h:1228` supplies `UsesSceneTexture`. Enabled custom-depth components count as writers; unresolved blendables, missing shader maps/resources and missing world/camera count as unknown. Reader or unknown refuses before street setup, stencil mode or ledger changes. Writer-only hosts stay admitted and their prior states remain untouched. This snapshot does not certify native/custom passes or later additions absent; private-pass isolation remains future work.

The original 5.1 scanner is at d7eee97 `AnomalyCaptureSubsystem.cpp:3179-3313`; its settings walk at 3234-3240 inspects every blendable without filtering its weight. The 4.25 port preserves that conservative rule. Both outline-on and blendable-off controls therefore report a reader and use the explicit fixture bypass; turning the effect off is not a way to pass production admission. A zero-weight-skip mutant is refused by the scanner parity test. This correction was made before any build or launch.

The receipt records reader/writer/unknown counts, admission, bypass, current mode and actual tagged-component counts before/after the scan. The refusal logs `C5 refused: host custom-depth <result>` and exits once. The repro bypass requires `-IAIBench -IAIBenchBypassHostDepth` and a recognized fixture package. The regression never uses it.

Planted test states require IAIBench, a recognized fixture package and an authored EA_ actor tag. Arbitrary maps receive no planted states. This narrowly scoped bench exception is required by FZ3.

C5 accepts `output seed [route,route]`; C4 accepts `output count seed [route,route]`. Omission selects blinking,lod_corruption. Lists must be nonempty, distinct and in the supported auto pool. The pool/keys do not change. Routes are logged and saved in routes.json and labels-receipt.json. Stock `UnrealEngine.cpp:1584-1604` splits ExecCmds on commas; the armed launcher uses the stock local-player ExecMacro file route (`LocalPlayer.cpp:1522-1546`), which submits each whole line. Its owned receipt path contains Binaries as stock ExecMacro requires. No Editor-only Python module is needed in -game.

## Predictions and gates, fixed before sessions

- Both existing Development targets build under FM1: 8 GiB commit start, 3 GiB stop, two actions, BelowNormal, three attempts each. All27 bound files are rebound. Runtime: FJ1 D25/15 GiB, physical4, commit8/4, five idle minutes. No calendar deadline is specified by 094-42; each session retains a finite limit.
- EA_Core regression passes the four routes explicitly, seed4242 once, interval0.8/hold0.4/max1. Bench bounds floor0.5%; shipped6%. The unchanged office reader must pass FV5/FX6: at least3 fires each, every LOD event changes pixels, masks exactly on affected frames, exact restore, schema2.1/sets/coverage/far plane, player view and measured pre-UI pairs. No guard bypass.
- Separate C++ fixture at E:/IA_Fixtures/Outline425 on installed4.25. Author only its declared outputs, via kept Python recipe. Several meshes, two multi-LOD targets, movable light, player start, CustomDepth outline material. Separate full-plugin build budgets and manifest; Paris assets/config/binaries never receive fixture output.
- Repro: outline-on versus blendable-off with identical pose/routes/seed and bench guard bypass. Count distinctive outline pixels and changed pixels on corresponding clean census frames and tagged objects. Retain census-off clean holds as another untagged control. Guard-on leaves the outline enabled and must show reader refusal, zero tags before/after, no setup/census/mask/capture output, normal exit.
- Spike(a): cite stock prepass types, shader-map lookup and bindings for opaque/masked/WPO, including export limits. Spike(b): isolated never-merged/pushed branch adds one trivial FMeshMaterialShader. Compare a no-change fixture launch and the first launch after registration, same fixture/DDC/config, reporting actual material recompilation counts and seconds. No full private pass.

Only changed-code tests run: native admission/routes negatives, Python2.7 fixture authoring harness and keyword Rotator check, scoped-kit old-arming/low-memory/low-disk negatives. Final evidence goes in the closeout and Receipts/09442; drive-root names stay local.
## 4.25 camera API note

The scanner ports the cached camera POV scan through `APlayerCameraManager::GetCameraCachePOV`, declared by installed 4.25 `Engine/Classes/Camera/PlayerCameraManager.h:312-313`; 5.1 calls `GetCameraCacheView`. This API difference was found from source before the first build. The scene texture IDs are included explicitly from `MaterialSceneTextureId.h`.

## 094-42r closeout, 2026-11-03

The owner reboot interrupted the initial disk-admission wait; no target build or engine session had run. The resume re-ran the native admission/routes suite, the static refusal/fixture/scanner checks and the macro/admission tests at 4fdfe84. Every FZ3/FZ4 item was also checked by code path. Entry and exit m51/protected-ref checks pass; E had134.14GiB at resume. No files were manually deleted during the resume.

Paris Editor and Game Development both pass attempt1 at4fdfe84, with zero compile errors, two actions, BelowNormal and no memory stop. The27-file manifest is rebound. EA_Core slot01 passes:2352 paired1280x720 frames, format18/D3D11, zero drops, normal exit. Fires/affected-mask frames are blinking22/132, lighting13/157, LOD19/247, missing15/195. All731 affected frames have masks; all1621 clean frames have none. Every LOD event changes target pixels. Schema2.1 and frame sets, far plane/coverage values,14-component exact restore and2352 mid samples, reserved203, player view, eight exact UI-marker pairs and all-frame marker absence pass. Census has no lost/exhausted/no-pass/polluted batches; the street-only gates are waived by the EA_Core profile. The explicit four-route receipt is correct. The scanner reads none, with0 tags before/after and no bypass. Upload and protected-file checks pass.

The preserved fixture at E:/IA_Fixtures/Outline425 now has a detached physical plugin worktree at4fdfe84 and both targets built. Editor attempt1 exposed six cascaded diagnostics from one fixture probe error (ToSharedRef on a TSharedRef); serializing R directly passes attempt2. Game passes attempt1. Every fixture build preserves all Paris files/binaries. The authoring Python script hash is6e4174a1886925076c352f32578d0e7cd4c10df7c17aa055c2be9f6fbd44891c; its engine2.7 harness and keyword-Rotator positive/negative checks pass.

Author slot01 starts11:39:46IST and is stopped by FF2 before the recipe: Saved/Config/Windows/Engine.ini is newly written at11:39:51IST. The argv is missing -nowrite. Exit125, no crash, no upload, zero declared outputs; the generated1502-byte file is retained with sha25664d45f80d234e4710ec289eda4ce41af0947f42d61925e903bd9652a5440d7b8. The terminal result is preserved. No repro, guard, baseline or spike launch follows. G516 records the failure and required next-run correction.

FZ6(a)'s source-only result is retained at D:/IA_Europa/Receipts/09442/shader-route-source.md, with14 hashed source files/excerpts in shader-source-citations.json. Existing desktop depth shader permutations can supply opaque, masked and WPO geometry for a private target, subject to public lookup/binding and permutation/export constraints. No private pass is implemented. FZ6(b) is NOT MEASURED; no spike branch was created, merged or pushed.

Local closeout: D:/IA_Europa/Receipts/09442/closeout.json; regression raw reader: c5-attempts/C5/01/reader.json; fixture stop: E:/IA_Fixtures/Outline425/Receipts/09442/author/01/result.json. All27 rebound binaries and the Paris protected sets pass exit. No new archive, push or tag. Overall NEEDS-DECISION: the fixture authoring STOP leaves FZ5 and the measured shader-cost result incomplete.
