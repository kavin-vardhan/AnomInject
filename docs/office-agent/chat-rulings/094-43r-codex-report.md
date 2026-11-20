<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, gpt-6-astra, max, danger-full-access resume, exit 0, 7,634 s, 02:28:30-04:35:44 IST). Body below is byte-identical to report.md, sha256 BC3DCA4AA2927FFBA450E6110BB2FF848566FE11B929E55B899B8D2ACE0158DF. -->
094-43 / 094-43r: Stage A and H1–H5 PASS. FZ2 is implemented on `feat/europa-port`; no spike branch was merged and nothing was pushed. Code owns the scan, push and final office card.

GE1 preserves all 71 baseline refs and admits only the four recorded extras by exact SHA or ancestry. All eight self-tests pass, including the requested moved/unknown-ref negatives. The full P9 `main()` passed in 354.433 s: 382,970 inventory files, 22,628 rehashed files and 100,696,938,006 bytes, with no differences. The baseline was not regenerated.

Stage A uses the saved frame’s scene-depth uniform buffer. Visible IDs require equal depth; logically hidden event targets may lie in front of it. The frozen tolerance is **4/16,777,215 device-Z units** (2.384185933e-7), derived from depth storage/conversion quantization and the common position path; it was never tuned. Sources: 4.25 `D3D11Device.cpp:38–42,140–150`, `RHIDefinitions.h:725`, and [the exported scene-texture uniform-buffer function](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Public/SceneRenderTargetParameters.h:106).

| Stage A gate | Result |
|---|---|
| A-P2 | Warm launch: 0 `Missing cached shader map` lines. |
| A-P3 | IoU 1.000 and 0 differing pixels on every paired object frame; 132 hidden-blink frames nonempty and matching. Masked/WPO nonempty comparisons: 84/1,637. GD4 separately records 78 lighting owner-group mismatch frames. |
| A-P4 | 0 new magenta pixels on all 2,352 paired frames. |
| A-P5 | 0 enabled/tagged components; exact before/after state; `r.CustomDepth=1` at all three reads in each run. |

Stage A used seed 4242 and the same reader and kept legacy reference (`09443a/reference/02`). Both sessions wrote 2,352 frames, dropped 0 and exited normally. [Stage A evidence](E:/IA_Fixtures/Outline425/Receipts/09443r/stagea-result.json), [Stage A example frame 746](E:/IA_Fixtures/Outline425/Receipts/09443r/private-on/01/plugin-frames/epoch01/frame-000746.png).

Stage B ports source from the proven spike. The main files are `AnomalyPrivatePass425.{h,cpp}`, `AnomalyPrivatePolicy425.h`, `AnomalyPrivate425Shader.{h,cpp}` and `AnomalyPrivate425.usf`, with integration in Masks425, Labels425, ViewMetadata425, HostDepth425, StreetBench425, FrameCapturer and the auto-injector. It uses direct component proxies, a plugin-owned dynamic-element collector, batched draws, eight persistent fenced target slots and asynchronous readback. Colour and mask copies account for row pitch and current dimensions. No new material shader type was added.

The public chain is [FindShaderTypeByName](E:/EpicGames/UE_4.25/Engine/Source/Runtime/RenderCore/Public/Shader.h:2195), [the rendering-thread material shader map](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Public/MaterialShared.h:1764), its vertex-factory mesh map, exported [shader bindings](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Public/MeshMaterialShader.h:74) and [DrawDynamicMeshPass](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Public/MeshPassProcessor.inl:140). Static meshes use the public static proxy; ISM/HISM/skeletal meshes use virtual dynamic-element collection (`SceneManagement.h:2167,2221`; `InstancedStaticMesh.cpp:885`; `HierarchicalInstancedStaticMesh.cpp:1433`; `SkeletalMesh.cpp:4844`). No non-exported Renderer symbol is named in this route.

Unsupported component/material/binding cases are `unavailable`, excluded from census and selection, and listed in the run receipt. These static fixtures reported 0 unavailable components: 7 verified in the outline fixture and 5 in EA_Core. EA_Core has no skeletal target; live skeletal/instance coverage is not claimed. Normal C4/C5 makes no host custom-depth, stencil-value, write-mask or `r.CustomDepth` write. Private IDs use a plugin-owned stencil attachment. Legacy rendering requires the explicit fixture-only bench reference. The scanner runs at each request and retains reader/unknown refusal when private rendering is unavailable.

Focused tests passed for the state contract and its failing mutants, four component kinds and unavailable cases, padded 4/16-byte rows at widths 1/1279/1280/1600, and the actual global-shader bodies. The old tight-row copy fails the padded test. No in-editor Python changed. [Design and frozen predictions](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09443-stageb-design.md).

All builds used FM1: below-normal UBT, two actions, 8 GiB free commit to start and 3 GiB stop. There were no memory stops or LNK errors. The one compile failure was AnomalyCapture C4458 (`Views` hid the collector member); renaming it `ViewList` fixed it on the next attempt.

| Build | Attempt | Start commit / physical GiB | Seconds | Result |
|---|---:|---:|---:|---|
| A fixture Game | 01 | 22.689 / 11.128 | 54.979 | PASS |
| A fixture Editor | 01 | 22.646 / 11.136 | 48.277 | PASS |
| B fixture Game | 01 | 21.973 / 11.225 | 92.475 | PASS |
| B fixture Editor | 01 | 20.934 / 10.175 | 96.721 | FAIL |
| B fixture Editor | 02 | 21.430 / 11.159 | 39.948 | PASS |
| B host Editor | 01 | 19.023 / 10.827 | 193.853 | PASS |
| B host Game | 01 | 20.597 / 10.821 | 119.005 | PASS |

Stage B fixture H1–H3 passed with the default routes (blinking, LOD), seed 4242. All 2,352 outline-on frames had 0 new magenta pixels. Every paired object had IoU 1.000 and 0 differing pixels: 1,896 nonempty comparisons each for floor, hide target and both LOD targets; 168 masked and 1,658 WPO comparisons; all 222 hidden-blink frames matched. The empty pawn reference stayed empty. Census pixel counts matched frame by frame; there were no lighting groups in this default-route run. Enabled/tagged components and polluted batches were 0; state and `r.CustomDepth=1` were unchanged. [Example frame 746](E:/IA_Fixtures/Outline425/Receipts/09443r/stageb/private-on/01/plugin-frames/epoch01/frame-000746.png), [H1–H3 evidence](E:/IA_Fixtures/Outline425/Receipts/09443r/stageb/H1-H3-result.json).

H4 passed on EA_Core in all three configurations. Each run saved 2,352 frames with stock format 18, 0 drops, one seeded injector start, schema 2.1 with matching sets, valid coverage values and the 5.1 far-plane meaning. Player view, all eight exact UI-marker control/capture pairs, both self-check lines, zero census exhaustion/loss/no-pass/pollution, and exact host state passed. The full four-route list was explicit; the bench floor was 0.5%, with shipped 6% unchanged. The street-only wall-strip and over-55 gates were waived by the EA_Core profile.

| H4 run | Blink fires/masks | Lighting fires/masks | LOD fires/masks | Missing fires/masks | Clean frames / mask pixels |
|---|---:|---:|---:|---:|---:|
| D3D11-1280 | 22/132 | 13/157 | 19/247 | 15/195 | 1621 / 0 |
| D3D12-1280 | 22/132 | 13/157 | 19/247 | 15/195 | 1621 / 0 |
| D3D12-1279 | 22/132 | 13/157 | 19/247 | 15/195 | 1621 / 0 |

D3D11-1280: all 19 LOD events changed target pixels; the per-event minimum changed-pixel counts were 14461, 14469, 14458, 14455, 14469, 14455, 14458, 14455, 14461, 14461, 14458, 14458, 14455, 14455, 14455, 14461, 14458, 14458, 14455. [Per-frame data](D:/IA_Europa/Receipts/09443r/H4/D3D11-1280/01/reader.json).

D3D12-1280: all 19 LOD events changed target pixels; the per-event minimum changed-pixel counts were 14458, 14455, 14455, 14458, 14455, 14461, 14455, 14469, 14458, 14458, 14455, 14455, 14458, 14458, 14461, 14458, 14455, 14455, 14458. [Per-frame data](D:/IA_Europa/Receipts/09443r/H4/D3D12-1280/01/reader.json).

D3D12-1279: all 19 LOD events changed target pixels; the per-event minimum changed-pixel counts were 14428, 14429, 14431, 14422, 14429, 14441, 14431, 14427, 14428, 14428, 14431, 14429, 14422, 14422, 14441, 14428, 14429, 14431, 14422. [Per-frame data](D:/IA_Europa/Receipts/09443r/H4/D3D12-1279/01/reader.json).

H5 measured the private pass itself, including collection/submission on the render thread and scene-depth copy/draw/reduce GPU work. Shared capture/readback and PNG writing are excluded; this is not a whole-engine frame-time subtraction. Every run has 2,352 timing samples and 0 blocking waits.

| Run | GPU mean / p95 ms | Render-thread mean / p95 ms |
|---|---:|---:|
| D3D11-1280 | 0.158516 / 0.189000 | 0.161802 / 0.225900 |
| D3D12-1280 | 0.157144 / 0.189000 | 1.687530 / 2.076075 |
| D3D12-1279 | 0.157491 / 0.189000 | 1.597230 / 1.949485 |

The [card draft](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/office-card-094-43-draft.md) contains all three corrections: use the issued launch line exactly, read R1 from the source clone, and keep the copied plugin outside the game repository. It awaits Code’s scan and concrete office launch line.

Levers were process-local: `-IAIBench`, `-nowrite`, `-IAICaptureExit`, `-IAICaptureFallback`, seed 4242, the fixed 30 Hz sweep/0.8 s interval/0.4 s hold/one concurrent event, eight UI markers, AA off, screen percentage 100, dynamic resolution off, motion blur off, exposure adaptation off, `t.MaxFPS 30`, the explicit routes and viewport/RHI selections. Stage A used `-IAIDepthOnlyByName`; the fixture controls used outline blendable 0/1 and explicit legacy-reference/guard-bypass flags. Fixture launch-only shader settings were 14 unused compiler threads in both modes and asynchronous shader compiling false; upload/analytics/autosave switches stayed false. Complete launch flags and capture macros are preserved in each session’s `prepared.json` and `Binaries/capture.txt`. Native pacing, mask state and custom depth restored; all processes exited, and no settings/assets were saved. The final runtime manifest binds 27 files; the installed kit points to its spent final H4 slot. The previous launcher/config copies are receipted. No fresh office launch is armed.

No crash, upload sign, protected change, security/machine-setting change or foreign-process intervention occurred. Exit checks passed for m51, protected refs/assets, fixture assets and preserved Saved Config, native-source equality and every current/archive hash. No cleanup purge occurred; tracked fixture source replaced during checkout remains in the retained spike ref and is listed in `fixture-checkout-receipt.json`. No gate was weakened or reference retuned. Stage B’s stricter hardware equality and private stencil ID storage were declared before its sessions; the old bench plant was removed as GD5 required.

Built artifacts use source `42ea37e3ddf7e9102757eec6ebd357f861a524bc` for Stage A and `59c128218412ab81ee6bfb82be4bf9cff4e944de` for Stage B. Final documentation/source equality is checked in [exit.json](D:/IA_Europa/Receipts/09443r/exit.json). Archive keys below are complete directory prefixes; each filename resolves directly beneath its listed relative directory.

A archive: `E:/IA_Fixtures/Outline425/Receipts/09443r/binary-archive`.

B archive: `E:/IA_Fixtures/Outline425/Receipts/09443r/stageb/binary-archive`.

H archive: `E:/IA_Fixtures/Outline425/Receipts/09443r/stageb/host-binary-archive`.

| Archive / relative exe or DLL | SHA-256 |
|---|---|
| A / `Binaries/Win64/Outline425.exe` | `ef140d2e308c9490cea4480e63c3fa2d70c63982d2976daaf8f4a6207a8e4243` |
| A / `Binaries/Win64/UE4Editor-Outline425.dll` | `86156bfc506d934efe83499621997fbfd48290f74983d48a9cddb35944ec481f` |
| A / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll` | `0a74f35993eddb1406901e4b240360d67c5239cfc5a80ac0d6bcd38517b25e12` |
| A / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll` | `4fdd60742e64acb2d89ce60a05ffff749dafdce6c816e2b3282261e69405f182` |
| A / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll` | `b84088b69fe7b66adcd79086941bd8374cbb9beebdc63f959113fb24fdc13b40` |
| A / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll` | `fede3315dd36fbe22c4c32537fce9d40883e4b08d722827a540775800f17b02c` |
| A / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll` | `10b0f03d2c3830aaee29ef0b9c458593c6316651f809f11ece780bf49baa50fe` |
| B / `Binaries/Win64/Outline425.exe` | `71cbce5c72d7a9747c25ba2ec3bcd6372e9796fab0d6d4f5d08d1bc37a69d4b3` |
| B / `Binaries/Win64/UE4Editor-Outline425.dll` | `86156bfc506d934efe83499621997fbfd48290f74983d48a9cddb35944ec481f` |
| B / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll` | `0a74f35993eddb1406901e4b240360d67c5239cfc5a80ac0d6bcd38517b25e12` |
| B / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll` | `fe4c1351f7ae397a546a99d83debd5283350833374ffed97028e790f23613466` |
| B / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll` | `b84088b69fe7b66adcd79086941bd8374cbb9beebdc63f959113fb24fdc13b40` |
| B / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll` | `54d1cc1e96a7ee2b7c84aad010bdfc346294f15e7d7911c58b8b94a8c9bc965e` |
| B / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll` | `9ae9b60976505af41e9cdb547380954610bc10ef687ee71083f2240265f578c4` |
| H / `Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe` | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| H / `Binaries/Win64/StylizedParisStreet.exe` | `9f60cec50f3827dce33e721e71946e814d4687b6b910bf1abe1e20c2156ca78f` |
| H / `Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll` | `6037924a08166344146f337b2f0b19ba35a5daf3dc99715db15ef6c332864a89` |
| H / `Binaries/Win64/UE4Editor-StylizedParisStreet.dll` | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| H / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll` | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| H / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll` | `62588a37e75e6fe20f74d79fb01a4b0d4d0484af376b12d23a6aaf6f3a2fb456` |
| H / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll` | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` |
| H / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll` | `35e619aab3bf9727d896d4c41b07ac4515b97ca291c0d738a5a40395161e37fe` |
| H / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll` | `3d0ff3bdd2a721f48573124a0ceaa79870fdae460bd0a01c90df2220cc7cb0fb` |

The retained host Shipping exe was not rebuilt. The launched installed `UE4Editor.exe` stayed at SHA-256 `4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387`. Full archive and binary-string receipts are in [the committed evidence](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09443-private-capture-receipts.json).

Feature commits made in this resume (all local, none pushed):

- `feat/europa-port` `d2f481d3b4fc5d9947da5264be2cf37d505ce5b2` — fix(europa): admit recorded P9 refs under GE1
- `feat/europa-port` `8eb66f12391469596a8053c2126737f1bf62f3ee` — chore(europa): scope the GD3 fixture proof under FM1 and GA5
- `feat/europa-port` `88bc3dcf90be7a8ef52e3b5f9b659a31cc5024e9` — docs(europa): record the passing GD3 fixture gates
- `feat/europa-port` `ea082584bceda498b50a8a0d0f15ad955dcb34ff` — feat(europa): port private batched depth and ID capture for Stage B
- `feat/europa-port` `59c128218412ab81ee6bfb82be4bf9cff4e944de` — fix(europa): avoid collector view member shadow on 4.25
- `feat/europa-port` `93613ae86213f3326bdaf4a7c837a4a84b0008ac` — docs(europa): record private capture qualification on D3D11 and D3D12

Local spike commit: `spike/europa-09443a-depthonly-byname` `42ea37e3ddf7e9102757eec6ebd357f861a524bc`. It is neither merged nor pushed; the older 42c spike is unchanged. Final feature commit: `93613ae86213f3326bdaf4a7c837a4a84b0008ac`.

VERDICT: GREEN | Stage A and H1–H5 pass; private capture is committed locally, ready for Code’s scan and push.
