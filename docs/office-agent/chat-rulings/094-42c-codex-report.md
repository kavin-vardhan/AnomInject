<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, gpt-6-astra, max, danger-full-access resume, exit 0, 21824 s, 17:14:37-23:18:22 IST). Body below is byte-identical to report.md, sha256 5B75F83F99EDE54DDAC7DEE05893628C705C2140B98326E2C15FD23CC7E0ED67. -->
P9 v3, the outline repro and the refusal guard pass. The shader-cost experiment is complete: 47 distinct logged material names recompiled in a 2,906.412 s first launch, versus 22.612 s for the matching warm launch. Direct linking to five depth-only `StaticType` symbols fails on the installed 4.25 renderer. The experiment remains isolated and unpushed.

P9 v3 retains 382,969 entries: v2 minus 89,583 entries under the owner-deleted overflow root. Prior baselines are unchanged; the receipt preserves the existing stream format and cumulative removed roots. Its SHA-256 is `0e7c3386fedc7b46738053220f41b3fd4fa18f0806a7f7b96caed0a099eab666`. `owner_p9.py` selects the v3 baseline, receipt and hash; `root_attribution.py` reads v3. The production derivation/inventory check found 382,970 current files and zero unexplained differences. A planted missing expected record under a retained Content root was refused; no protected file was created or removed. Lexical exclusion works with either an absent or empty retired parent. Full EA8 rehash/toolchain/fsck checks remain deferred by FJ1. [Both test receipts](D:/IA_Europa/Receipts/09442c/tests/p9-v3/tests.json).

The fixture is `E:\IA_Fixtures\Outline425`. Its seven hashed assets are EA_Outline, two two-LOD meshes, and M_Surface, M_Masked, M_WPO and M_Outline. The level has six mesh actors, a Movable light and a PlayerStart. The kept authoring script SHA-256 is `32928cb9757e252dd8fdac407e5754165011c3b73153b94ab1fe5adb232fda5a`. Every launch carried `-nowrite`; the preserved 1,502-byte Saved Engine.ini is unchanged. [Asset paths, sizes and hashes](E:/IA_Fixtures/Outline425/Receipts/09442c/assets.json).

Both fixture targets passed on attempt 1. The spike used its three-build budget: add the trivial shader; measure the expected link failure; compile out that link probe. All UBT launches used BelowNormal priority and two actions. No build was stopped by memory; the link probe has five LNK2001 errors plus LNK1120. The final runtime retry changes only the shader text and reuses build03 after checking the one-file diff and all 20 native-output hashes; no fourth native build ran.

| Build | Attempt | Start free commit (GiB) | Admission wait (s) | Wrapper elapsed (s) | Result |
|---|---:|---:|---:|---:|---|
| Fixture Editor | 1 | 9.665 | 0.00 | 9.47 | PASS |
| Fixture Game | 1 | 8.557 | 0.01 | 28.93 | PASS |
| Spike Editor | 1 | 8.729 | 0.01 | 23.49 | PASS |
| Spike Editor | 2 | 8.722 | 1120.41 | 1147.94 | FAIL |
| Spike Editor | 3 | 9.473 | 0.01 | 28.07 | PASS |

The paired C5 runs each wrote 2,352 frames with zero drops and normal exit, with identical seed 4242 and 69 events: blinking 37, LOD 32. Both used the scoped bench bypass; only the outline blendable weight changed (0/1). The default two routes were used; lighting and missing-object were not requested. GA1 already accepts the earlier four-route Paris smoke, so it was not repeated.

| Repro route | Evaluated frames | Frames with boundary outlines | Boundary pixels/frame |
|---|---:|---:|---:|
| blinking | 186 | 186 | 6,071–6,321 |
| lod_corruption | 351 | 351 | 6,038–6,321 |

All 168 census-on clean holds were positive; all 168 census-off holds had zero tags and zero magenta pixels. The fixed reader uses novel magenta, channel difference above 32 and a 3-pixel tagged-boundary band; no-change and wrong-region negatives reject. Frame 76 has 9,461 boundary pixels and 0 control magenta pixels. The fixture material also highlights steep floor-depth gradients, so it is not a constant-width outline. [Pixel measurements](E:/IA_Fixtures/Outline425/Receipts/09442c/outline-pixels.json); [example outline frame](E:/IA_Fixtures/Outline425/Receipts/09442c/repro/01/plugin-frames/epoch01/frame-000076.png) and [matching control](E:/IA_Fixtures/Outline425/Receipts/09442c/control/01/plugin-frames/epoch01/frame-000076.png).

The guard-on run logged exactly `C5 refused: host custom-depth reader`. Its receipt has `tagged_before=0`, `tagged_after_scan=0`, bypass=false, 0 captured frames and 0 tag receipts; it exited normally. The host effect was not disabled to pass. [Guard receipt](E:/IA_Fixtures/Outline425/Receipts/09442c/guard/01/reader.json).

FZ6(b) added one trivial SM5/local-vertex-factory `FMeshMaterialShader` on `spike/europa-09442c-mesh-shader`. The completed comparison uses a fresh shader-source hash followed by a no-change warm launch, identical argv apart from receipt paths, synchronous material compilation and two requested workers. The fresh key follows [MaterialShader.cpp:738–745](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/Materials/MaterialShader.cpp:738); [MaterialShared.cpp:1919–1929](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/Materials/MaterialShared.cpp:1919) and [ShaderCompiler.cpp:1798,1876–1887](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/ShaderCompiler/ShaderCompiler.cpp:1798) establish the synchronous setting and worker count. No DDC was deleted.

| Cost reading | Material cache-miss requests | Distinct logged names | Launch to root exit (s) | Final pending jobs |
|---|---:|---:|---:|---:|
| Original baseline, before type | 0 | 0 | 43.657 | 0 |
| Fresh shader, slot03 | 53 | 47 | 2906.412 | 0 |
| Matching warm baseline02 | 0 | 0 | 22.612 | 0 |

The measured additional launch cost is 2,883.800 s. Durations exclude admission and include loading plus the probe's quiet interval; this single pair is not a throughput benchmark. Cache-miss counts are logged requests/names, with completion established by ready=true, zero pending jobs and normal exit. [Cost data and material names](E:/IA_Fixtures/Outline425/Receipts/09442c/shader-cost.json).

The real `StaticType` probe compiled to the linker, then failed on `TDepthOnlyVS<false>`, `TDepthOnlyVS<true>`, `FDepthOnlyPS<false>`, `FDepthOnlyHS` and `FDepthOnlyDS`. Log SHA-256: `8a362ce5dad72abe5b4abeccfa8d3df83edf846e9843cfb8240636707e2e792c`. This confirms a direct-link limitation for these symbols. Other private-pass approaches remain untested; GA3's source feasibility result remains separate. [Link result and exact errors](E:/IA_Fixtures/Outline425/Receipts/09442c/static-type-result.json).

Failed attempts and deviations are retained. Four authoring slots were needed for the cold-cache bootstrap, stock Python pin/WPO limitations and their tested fixes. Authoring01 was deliberately stopped by owned PID, not an engine crash; its original classification and separate correction remain. The first cost launch ran 13 workers because the INI keys were malformed, crossed the 4 GiB commit floor at 660.923 s, and hit WinError5 during stop handling. Exact owned-PID cleanup succeeded without elevation; the denied operation remains unconfirmed. Its Runtime.log grew after premature sealing, so both original and post-cleanup evidence are kept. Slot02 used one worker but reached the native 1,800-second probe limit with 985 jobs pending. Slot03 changed the runtime compilation policy and fresh shader key, with a matching new baseline; no acceptance threshold was lowered. A redundant offline pixel reader was stopped by its owned PID after the original reader passed.

Runtime levers were `-IAIBench`, `-nowrite`, AA=0, exposure=0, motion blur=0, 1280×720, 30 FPS and seed 4242; scoped bypass was on only for the paired repro, and blendable weight was 0/1. Authoring used `-BENCHMARK` to suppress its automatic launcher path. Worker policy changed from the ignored request to one worker, then synchronous/two workers for the final pair. These per-process settings ended with their processes. The owned fixture retains its assets and local DDC graph, which reads the installed engine cache; the preserved Saved ini is unchanged. No manual files or caches were deleted and no overflow folder was recreated.

Executable/DLL hashes follow. Archive paths are relative to `E:\IA_Fixtures\Outline425\Receipts\09442c\artifacts\`; complete source paths and uses are in [the executable index](E:/IA_Fixtures/Outline425/Receipts/09442c/executable-index.json). Initial fixture binaries and installed engine tools were not archived; all listed archive copies were hash-verified.

| File | SHA-256 | Archive relative path |
|---|---|---|
| Outline425.exe (initial) | `cc9320b3d50343b649388439a1a7ff392c634420a9b1234d99b4423dd6ed089c` | Not archived |
| UE4Editor-Outline425.dll (initial) | `95c0652da992cb31cc4a0cfed4246cd3ba43d55705f4b1b8c0e1159cdbb843d3` | Not archived |
| UE4Editor-AnomalyBench.dll | `0a74f35993eddb1406901e4b240360d67c5239cfc5a80ac0d6bcd38517b25e12` | baseline/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll |
| UE4Editor-AnomalyCapture.dll | `b9df453513918ce47843e8284ca7c987e7df5459e4f25d386e856de73734cc45` | baseline/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll |
| UE4Editor-AnomalyControlServer.dll | `b84088b69fe7b66adcd79086941bd8374cbb9beebdc63f959113fb24fdc13b40` | baseline/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll |
| UE4Editor-AnomalyInjector.dll | `fede3315dd36fbe22c4c32537fce9d40883e4b08d722827a540775800f17b02c` | baseline/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll |
| UE4Editor-AnomalyShaders.dll (baseline) | `634f03833549797f8c16152aa4d8eee7374aac23d06daffde2da30c1a7dd3d8c` | baseline/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll |
| CrashReportClient.exe | `f5c00f65e6dc4b2d3f7182f5efb41b481f8f9b2128f23a956f77397f54bd5449` | Not archived |
| UE4Editor.exe | `4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387` | Not archived |
| Outline425.exe | `55f3f4d33fbe37df472d7a1b6dcec95e907b8e8a2f94751dec8c045903257138` | baseline/Binaries/Win64/Outline425.exe |
| UE4Editor-Outline425.dll | `86156bfc506d934efe83499621997fbfd48290f74983d48a9cddb35944ec481f` | baseline/Binaries/Win64/UE4Editor-Outline425.dll |
| UE4Editor-AnomalyShaders.dll (spike 02/03) | `a2eaf542163bd7803609d5127248141e3378980945b07aaa328a115943d8272b` | cost-retry/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll |
| UE4Editor-AnomalyShaders.dll (spike 01) | `0161a98e5bc8dd6381ddf2be346b2f8711ea0c55a9b8625bb401de92e5de0251` | cost-spike/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll |
| UnrealBuildTool.exe | `07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0` | Not archived |
| ShaderCompileWorker.exe | `6805ce2c9cea663cd356cfc463b470390f227b08ef05e80a12d400363cdf60e6` | Not archived |

Exit fast checks pass: Paris protected files and binaries, the 27-file binding, m51/protected refs/tags, fixture assets and the preserved Saved ini are unchanged. No actual engine crash, upload sign or Paris protected change was found. Foreign activity was recorded only. No security, paging, power or machine settings changed.

Feature commits made in this job: `de29514`, `0b8baca`, and final `d840cdbd2c3d8c004fd0a3255e5df16fb03a60a6` on `feat/europa-port`. Isolated spike commits: `66dcf8d`, `26048a6`, `8447eba`, `740220cf736e60b4fc74d564668f48cc42290a76`; the branch is neither merged nor pushed. Origin remains `d1cdc7b`. Commits pushed: none; tags created: none. [Exit receipt](D:/IA_Europa/Receipts/09442c/exit.json).

VERDICT: GREEN | P9 v3, outline repro and guard pass; shader cost and depth-only link risk are measured.
