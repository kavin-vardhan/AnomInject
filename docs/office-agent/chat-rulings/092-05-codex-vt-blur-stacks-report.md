VERDICT: GREEN

Part A PASS. Part B PASS. Bounded branch proof; no delivery, merge or release change. This is implementer evidence for independent review.

Both parts passed on feat/vt-blur-stacks @ 5dc871702907e12e99663bbc391b612e35a31c3b.  
Office command: IAI.TexCorrupt.Census all vt  
Fixture example: IAI-VT-CENSUS v1 family=2 streaming=3 preallocated=0 stacks=1:0,2:0,3:3,4:0,5:0,6:0,7:0,8:0

3. Design and scope. The default-view form is `IAI.TexCorrupt.Census vt`; `allreasons vt` also works. Family 0=UV, 1=normal, 2=blurry. Output contains counts and histograms, with no asset, material, actor or level names. `only_vt_unassessed` preserves dependency limits; VT-only counts are not a promise of future eligibility.

- [TexCorruptVTDiagnostics.cpp:86](E:/IA_BuildCache/_r921/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/TexCorruptVTDiagnostics.cpp:86): aggregates binding/complete-component formats, compiled stacks, tile/border/address histograms and all-reasons VT-only counts; numeric output starts at line 207. Format bits BC5=1, DXT5=2, BC7=4, DXT1=8, BGRA8=16, other=32.
- [TexCorruptTree.cpp:1989](E:/IA_BuildCache/_r921/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp:1989): passive census reads cached uniformity without measurement or residency requests. Complete-stack eligibility/refusals are at lines 672 and 750.
- [AnomalyPrivateVT.cpp:92](E:/IA_BuildCache/_r921/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/AnomalyPrivateVT.cpp:92): preserves source first-mip bookkeeping and creates private VT resources. Lines 212/270 validate producers, exact mapped levels, layer-to-page-table mapping and root/full readiness. Line 387 reads and retains the actual material allocation through the public uniform cache.
- [Anomaly_TexCorrupt.cpp:1245](E:/IA_BuildCache/_r921/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:1245): verifies every combined allocation before `CommitSlots` at line 1291; all required component replacements commit together. References release before producer cleanup at line 259.
- [AnomalyVTPageCheck.usf:1](E:/IA_BuildCache/_r921/AnomalyInjector/Shaders/Private/AnomalyVTPageCheck.usf:1): integer GPU page-table reads into a buffer avoid the fragile texture-copy path.

The new saved-camera 1920×1080 fixture has one compiled three-layer streaming stack, independent colour/normal/roughness bands, a shared-texture co-user, ordinary control and unnamed-layer refusal. The target covers 19.753% and is 1280 pixels wide, meeting the unchanged 25% ceiling and 8x Auto rule for a 128-pixel output. Sources are 1024 square with authored colour/normal/roughness LOD biases 0/1/2. PIE first mips 0/1/2 plus additional drops 3/2/1 yield 128×128 on all layers. Cooking trims the biases, so staged first mips are 0/0/0; the same additional drops yield the same output sizes. Drop 0 preserves effective sizes 1024/512/256. Blurry remains OFF by default; UV/normal still refuse VT. Hold and ordinary proxy behavior are unchanged.

4. Proof tables. Predictions and every correction were saved before their corresponding reruns. Raw legs, failed attempts, logs, images and readers are under `E:\IA_BuildCache\_r921\09205`; compact gate receipts and harnesses are on the branch in `tools/proof09205`.

Part A expected → printed counts. VT component/binding and stack counts apply to every family; refusal triples are UV/normal/blurry.

| Fixture/scope | Components / VT components / bindings | Stack layers:count | VT-only | Blurry stack-only |
| --- | --- | --- | --- | --- |
| Single/view | 1/1/1 → 1/1/1 | 1:1 → 1:1 | 1/0/0 → 1/0/0 | 0 → 0 |
| Single/all | 6/5/6 → 6/5/6 | 1:4,2:1 → same | 3/0/0 → 3/0/0 | 0 → 0 |
| Stacks/view | 1/1/3 → 1/1/3 | 3:1 → 3:1 | 1/1/1 → 1/1/1 | 1 → 1 |
| Stacks/all | 4/3/9 → 4/3/9 | 3:3 → 3:3 | 2/2/2 → 2/2/2 | 2 → 2 |

| Required formats, expected = printed | UV/blurry bindings | UV/blurry complete components | Normal bindings / complete components |
| --- | --- | --- | --- |
| Single/view | DXT1 1 | set 8:1 | 0 / 0 |
| Single/all | DXT1 6 | set 8:5 | 0 / 0 |
| Stacks/view | BC5 1, DXT1 2 | set 9:1 | BC5 1 / set 1:1 |
| Stacks/all | BC5 3, DXT1 6 | set 9:3 | BC5 3 / set 1:3 |

All other requested format bins are zero. All stacks are streaming, preallocated=0; unused layer-count bins 1…8 are zero. Tile 128 / border 4 / wrap on both axes count 1, 6, 3, 9 bindings respectively; clamp/mirror/unread bins are zero. Four PIE and two staged Part A legs each pass all 480 native images, pre-census null timing, unchanged material/texture/resource/producer/residency fingerprints and zero events. Real +1-binding mutants fail all three families on both fixtures; independent state/pixel mutants and 47 reader cases also pass their can-fail checks. Final Part B view/all-scope census checks repeat unchanged-state proof: format/geometry counts are unchanged; named stack blurry `only_vt`/`only_stack` become zero.

Final Drop 0 identity: every native frame, including both bind/restore edges, uses its own run’s null maximum + 0.05 MAD; no fallback criterion.

| Region | Frames | Maximum MAD | Limit | Result |
| --- | --- | --- | --- | --- |
| target | 480 | 0.328041 | 0.362480 | PASS |
| co_user | 480 | 0.297493 | 0.347493 | PASS |
| ordinary | 480 | 0.302347 | 0.352347 | PASS |

Identity verifies 255 combined layer entries in two attempts, compiled-order producer biases 1/0/2 and effective sizes 512/1024/256. Resources/producers and retained references drain to zero.

| Positive leg | Fires | Frames / labelled | Pixel onset/offset | E1 t50 unflagged | Result |
| --- | --- | --- | --- | --- | --- |
| stacks-pie-targeted-final-v2 | 10 | 1561 / 480 | 0 / 0 | 0 | PASS |
| stacks-pie-auto-final-v2 | 3 | 553 / 144 | 0 / 0 | 0 | PASS |
| stacks-staged-targeted-final-v2 | 5 | 841 / 240 | 0 / 0 | 0 | PASS |
| stacks-staged-auto-final-v2 | 3 | 553 / 144 | 0 / 0 | 0 | PASS |

| Independent layer band | Event median MAD range | Maximum gradient ratio | Missing / extra blur frames |
| --- | --- | --- | --- |
| color | 34.558–34.560 | 0.0053 | 0 / 0 |
| normal | 19.824–19.825 | 0.0204 | 0 / 0 |
| roughness | 5.391–5.395 | 0.0382 | 0 / 0 |

Every band exceeds the frozen MAD threshold (at least 4) and falls below the frozen 0.75 gradient ratio. Intended-blur templates independently catch fallback pages at the edges. All 21 events have target-only measured masks, zero labelled frames without a measured mask, annotation equal to labels, unchanged co-user/ordinary controls and clean revert. Total: 3508 images and 1008 labelled frames.

| Isolation / lifetime | Measured result | Result |
| --- | --- | --- |
| Co-user and ordinary controls | 0 threshold failures across all 21 required events | PASS |
| Revert | 0 residual pixel failures; 21 resource/producer drains 0/0; combined refs 0 | PASS |
| Ten consecutive cycles | 221952-byte allocation peak every cycle; no growth | PASS |
| Destroy / displace | 265 frames each; last anomalous frame 149, interruption 150–167; clean labels, controls and drain | PASS |
| World end | 151 frames, 31 labelled; no missing masks/disagreement; final drain 0/0, refs 0 | PASS |

| Can-fail / refusal | Observed evidence | Result |
| --- | --- | --- |
| Release 218BA4C7 | Actual targeted attempt: three virtual textures, held none; 0 events, unchanged pixels | PASS |
| Accepted af79520 / 1EA292E8 | virtual_texture:stack_layout; 0 events, unchanged pixels; Source/Shaders match c80ef9b | PASS |
| Unnamed layer | virtual_texture:stack_unnamed; 0 events, unchanged pixels | PASS |
| Replace only one layer | Colour and roughness fail their independent pixel checks | CAUGHT |
| No bind | No measured target change; positive/E1/layer checks fail | CAUGHT |
| Early bind, controlled staged | All three intended blurs start at 134 versus label 120: 14 onset frames wrong | CAUGHT |
| Same scheduling delay, normal implementation | One additional staged event passes every reader; 0-frame edges | PASS |
| Early bind, uncontrolled PIE | Locked-root production naturally completed before capture | INCONCLUSIVE; not used |
| Reader / infrastructure mutants | Identity truncation/duplicate/changed pixels; annotation/mask/co-user/revert; growth/drain; false refusal banner; lever exposure; semantic string; archive hash; idle-gate blockers | CAUGHT |

| Final PIE regression | Fires | E1 t50 unflagged | Result |
| --- | --- | --- | --- |
| regression-hold-final-v2 | 3 | 0 | PASS |
| regression-proxy-final-v2 | 3 | 0 | PASS |
| regression-uv-final-v2 | 3 | 0 | PASS |
| regression-normal-final-v2 | 3 | 0 | PASS |
| regression-single-vt-final-v2 | 3 | 0 | PASS |

5. Gates, pushes, archives and restoration. Part A was committed and pushed before any Part B work. The final documentation head retains the exact tested runtime Source, Shaders and plugin descriptor from `502de71ed82f8eed9bdc51070b55361d40133db6`.

| Gate | Part A | Part B |
| --- | --- | --- |
| Editor / game normal builds | 0 errors / 0 warnings each | 0 errors / 0 warnings each |
| Strict include Editor / game | 0/0, 71 isolated headers each | 0/0, 72 isolated headers each |
| Lever audit / Shipping exclusion | PASS; exposure mutant caught | PASS; 61 registered bench levers |
| String scan | 46 candidates resolved; mutant caught | 209 candidates resolved; mutant caught |
| Direct AutomationTool -nocompile cook | PASS, 28.14 s | PASS, 31.247 s |
| Archive rehash | Final six binaries verified | 9 archives / 54 binary instances verified |
| Scene / source / branch preservation | PASS | 44 inherited engine-source entries + 9 added records, 10 single-fixture assets and protected refs match |

Measured readiness: required positive events use 3 combined entries, 2 attempts, 3.574–6.428 ms; identity uses 255 entries, 2 attempts, 6.784 ms. Each positive private layer uses 1 entry / 2 attempts. Existing limits remain 512 entries, 120 attempts and 10 seconds per allocation; none increased.

All commits below were pushed fast-forward to `feat/vt-blur-stacks`, with no tags or protected-ref writes:

| Checkpoint | Commit | Subject |
| --- | --- | --- |
| A | 898f9e70195ace61757bcc1c69a52e750d2fc6a5 | feat(census): add numeric virtual texture diagnostics |
| A | 1d38649cb6a3d01af4359395d955840f9d28da92 | test(census): add state fingerprint and counting mutant |
| A | 7bebf09646b9cb45144847bf282d57626a131b85 | fix(bench): read residency through the public query |
| A | 16b5f5867c9c3e4c8a8ba3d3764adbf5a8c51f93 | fix(census): keep VT diagnostics passive and numbers only |
| A | 94779af5f2d441398854161136ac673a2aca77ac | test(census): prove passive VT inventory on both fixtures |
| B | 6bfca31aca8e108d6ccc2a2b692aaf5db7fd6496 | feat(vt): verify every layer of the combined blur allocation |
| B | 169292981863ae9ca9297e302036e8c384b70625 | fix(vt): resolve page table resources after RHI updates |
| B | fe94c63c0be3659d95f874e6593a768c37289118 | fix(vt): validate the render material cache and stack layout |
| B | 1fda1cb721e6368eac5bd2a3ed7e2029a729e540 | fix(vt): retain verified combined allocations through the event |
| B | 502de71ed82f8eed9bdc51070b55361d40133db6 | fix(census): retain required VT layers after stack refusal |
| B | 5dc871702907e12e99663bbc391b612e35a31c3b | test(vt): prove complete stack blur and census on final build |

Before-state: source worktree clean at `af79520437e1b470904b0638e99c3a15dd0fcc70`; host clean/detached at `23098fa7415f148e935acbd4a97e18da04e6c46c`. Host is restored to that detached commit, all 6 original binary hashes and 156 configuration hashes. 70 created crash-client INIs were removed by receipt. Temporary regression device profiles were removed by receipt. D: 77.32 GiB and E: 98.74 GiB remain free. Staging stayed in the job’s own cook folders; no delivered build folder changed.

Bench levers/settings used and restored: `IAI.Bench.VTCensusMiscount`, `TexCorruptIdentity`, `ProxyVTPartial`, `ProxyVTNoBind`, `ProxyVTEarlyBind` were 0/1 and reset to 0; `VTCensusState` and `ProxyVTInventory` were read-only queries. Census used `vt`, `all vt` and legacy `allreasons`. `IAI.Anomaly.StuckMipRoute` used auto/hold; Auto pool used all=0 and stuck_low_mip=1 for Auto legs. Capture used SVE=1, Delivery=0, Pace=1, Fps=30, Mask=1, TargetMask=1, RunLog=1, FocusGate=0, native output, seed 777; schedules were 0/121/48/96 with 1/3/5/10 bursts, identity 0/121/180/179/1 (failed earlier cooldown 96), and regressions 0/192/48/192/3. Accepted-baseline retry alone used TexCorruptWarmDraw=0. All process-local overrides expired at owned process exit.

Rendering settings used: r.AntiAliasingMethod=0, r.DefaultFeature.AutoExposure=0, r.EyeAdaptationQuality=0, CaptureBench.Marker=0, r.Editor.Viewport.OverridePIEScreenPercentage=0, r.ScreenPercentage=100, r.SecondaryScreenPercentage.GameViewport=100, r.DynamicRes.OperationMode=0, r.DynamicGlobalIlluminationMethod=0, r.Lumen.DiffuseIndirect.Allow=0, r.ReflectionMethod=0; stack legs additionally r.BloomQuality=0. Staged pacing used t.MaxFPS=60. Final staged census used LogAnomalyCapture Warning to avoid unrelated telemetry interleaving. The controlled staged can-fail changed r.VT.SyncProduceLockedTiles 1→0→1 and r.VT.MaxTilesProducedPerFrame 30→0→30 after warmup, restoring after 129 written frames; the normal control used the identical schedule. These are production-scheduling controls, with no source residency forcing or VT bias. The final host settings were restored by hash.

Fixture-only changes intentionally retained: distinct colour/normal/roughness assets, 1024 sizes, LOD biases 0/1/2 (ordinary controls matched), saved native camera, fixed point light 60 and directional light 0.03, shadows off, point source radius 150. Temporary ordinary regression device-profile LOD bias 1 was removed by receipt. Strict-build settings were restored before normal rebuilds. Launch flags included -IAIBench, native resolution/windowing/ForceRes as applicable, unattended/no-live-coding; one failed diagnostic launch used -d3ddebug. TEMP/TMP were directed to job scratch after the early selftest exception described below. Exact commands/PIDs/settings receipts are in `levers-used-final.json`, per-leg launch plans and `restoration.json`. No foreign process was killed, no security setting changed, and no engine modification, global/pool VT bias or forced source residency was introduced.

Binary SHA-256 ledger follows. Paths are archive directories; within each, DLLs are under `editor/`. Each archive contains StackOBot.exe and the five named DLLs. All 54 new binary instances were rehashed after runtime proof.

Archive `E:\IA_BuildCache\_r921\09205\archives\probe-1692929-35F9A2A7`; runtime source `169292981863ae9ca9297e302036e8c384b70625`.

| Binary | SHA-256 |
| --- | --- |
| StackOBot.exe | `35f9a2a717a4e6b190791692d25413d424c4fb906312dc81bafb3cf125ccbbfa` |
| editor/UnrealEditor-AnomalyBench.dll | `c7924005510d7256f77ef3172327c750afee180781e3011400feeb5f212531e0` |
| editor/UnrealEditor-AnomalyCapture.dll | `6aac004f303d1936dd3b3ece405572f5eadb5896d165cb9ddb24fd5ce09506ba` |
| editor/UnrealEditor-AnomalyControlServer.dll | `649114d43c997b63c78bc0d1006f2c26dbdf80ec0c5070724806196f6eb9349d` |
| editor/UnrealEditor-AnomalyInjector.dll | `37cea03c72352714147f78813c3ec17f836550d3d4ba94c9acdb229a568fdb56` |
| editor/UnrealEditor-AnomalyShaders.dll | `1a8b1589b44f4a98131792533e5954e31dd30c1fca91a0af6b9737ddccb568dd` |

Archive `E:\IA_BuildCache\_r921\09205\archives\probe-16b5f58-381ED90F`; runtime source `16b5f5867c9c3e4c8a8ba3d3764adbf5a8c51f93`.

| Binary | SHA-256 |
| --- | --- |
| StackOBot.exe | `381ed90fb296fb2dc55094543c82c7ac6e48778217b3f7d24b620425a5531a72` |
| editor/UnrealEditor-AnomalyBench.dll | `c7924005510d7256f77ef3172327c750afee180781e3011400feeb5f212531e0` |
| editor/UnrealEditor-AnomalyCapture.dll | `6aac004f303d1936dd3b3ece405572f5eadb5896d165cb9ddb24fd5ce09506ba` |
| editor/UnrealEditor-AnomalyControlServer.dll | `649114d43c997b63c78bc0d1006f2c26dbdf80ec0c5070724806196f6eb9349d` |
| editor/UnrealEditor-AnomalyInjector.dll | `28d3f2bf9e38ebf7951e4c9cd8bc0ff34837c332a6dc86c9227dcfcb5cd054fe` |
| editor/UnrealEditor-AnomalyShaders.dll | `d97788293a9f169dd609e4d08544a2199ac7d368bd68ebfe75f79cdddf77e9fc` |

Archive `E:\IA_BuildCache\_r921\09205\archives\probe-16b5f58-8BD2BD63`; runtime source `16b5f5867c9c3e4c8a8ba3d3764adbf5a8c51f93`.

| Binary | SHA-256 |
| --- | --- |
| StackOBot.exe | `8bd2bd630398d4f5c20144bc5a42da995155548a10bff736e3cc06d1a34be8c8` |
| editor/UnrealEditor-AnomalyBench.dll | `3f47105c8b1be9ffbd0e9762733f00398b0af39e65dd6cb7656c2934571c35bb` |
| editor/UnrealEditor-AnomalyCapture.dll | `9e5ab9451b5e1522a163e98216bba3a91cefbba64955a595ce621f79273fbc1b` |
| editor/UnrealEditor-AnomalyControlServer.dll | `d8cbd85bd723f1e34df39f24c8dcf37b7581058b84c5845694dcda609fb3dd35` |
| editor/UnrealEditor-AnomalyInjector.dll | `e8344f2f30608fdeee967bddb844ed08b332e717b842340124e49b1ec01442d4` |
| editor/UnrealEditor-AnomalyShaders.dll | `790294cb2358b3f4e1b3370ab4ed8d1bc458f9a588f986fc11f93b77a517592f` |

Archive `E:\IA_BuildCache\_r921\09205\archives\probe-1fda1cb-27DBC2C0`; runtime source `1fda1cb721e6368eac5bd2a3ed7e2029a729e540`.

| Binary | SHA-256 |
| --- | --- |
| StackOBot.exe | `27dbc2c0b42c3a19af3f32c27b2c51f5ef1cacb27029a13097afb7faf518059d` |
| editor/UnrealEditor-AnomalyBench.dll | `c7924005510d7256f77ef3172327c750afee180781e3011400feeb5f212531e0` |
| editor/UnrealEditor-AnomalyCapture.dll | `6aac004f303d1936dd3b3ece405572f5eadb5896d165cb9ddb24fd5ce09506ba` |
| editor/UnrealEditor-AnomalyControlServer.dll | `649114d43c997b63c78bc0d1006f2c26dbdf80ec0c5070724806196f6eb9349d` |
| editor/UnrealEditor-AnomalyInjector.dll | `8c16558ccd83d4d4a99f4d184fcc04fb7658932911a5468c4148cf5a1fa46b16` |
| editor/UnrealEditor-AnomalyShaders.dll | `1a8b1589b44f4a98131792533e5954e31dd30c1fca91a0af6b9737ddccb568dd` |

Archive `E:\IA_BuildCache\_r921\09205\archives\probe-1fda1cb-F3A8C68C`; runtime source `1fda1cb721e6368eac5bd2a3ed7e2029a729e540`.

| Binary | SHA-256 |
| --- | --- |
| StackOBot.exe | `f3a8c68cb019b20ad9df9b698148b59854bab8079a381be74232a1beeebdb03c` |
| editor/UnrealEditor-AnomalyBench.dll | `6d50631c822055330a771ec6f42905c47a79f85e168d717090e655ee4694a8a0` |
| editor/UnrealEditor-AnomalyCapture.dll | `9e25db8af6222da5f891e96e8a451c10829bfefa0058fe32895541495dc74a5b` |
| editor/UnrealEditor-AnomalyControlServer.dll | `f54abc5e1ac344515139ec6a58c1f5cdba9552437ab67a8e7198e7bebbeead3d` |
| editor/UnrealEditor-AnomalyInjector.dll | `a69629697d85b838091731f2d4e40b586e359838547fb572e0ed2acc91d5d6a1` |
| editor/UnrealEditor-AnomalyShaders.dll | `1308cc75643585d6cb94962e84f38b791eb7d4c2a5b4024609e04e5379ee7816` |

Archive `E:\IA_BuildCache\_r921\09205\archives\probe-502de71-4458129F`; runtime source `502de71ed82f8eed9bdc51070b55361d40133db6`.

| Binary | SHA-256 |
| --- | --- |
| StackOBot.exe | `4458129faf79c8f9cf6b45bc163df047111b3345d96c9079b13ebfad16ea9320` |
| editor/UnrealEditor-AnomalyBench.dll | `3f4abcadcfe9b098420f27bbb91a5ca29ac4359953045d3a283068db86684053` |
| editor/UnrealEditor-AnomalyCapture.dll | `91baabbab396ab5d45e6df615d9ded386dd6e99a179d40bbf8ae2fc3b5087100` |
| editor/UnrealEditor-AnomalyControlServer.dll | `ab5cd3bf1e1bb3f398ed9ac7ac9fbb87007257e1cb2d964f93828c7025bc8a2f` |
| editor/UnrealEditor-AnomalyInjector.dll | `8966a655a186881dd1c08e52a25232c92e61b7f00f7b604ce6f5796dc42022a4` |
| editor/UnrealEditor-AnomalyShaders.dll | `6cc03bf61875ef376d3797c6d74c012e9323a36791c0af57c167f7f67818e439` |

Archive `E:\IA_BuildCache\_r921\09205\archives\probe-6bfca31-5886C355`; runtime source `6bfca31aca8e108d6ccc2a2b692aaf5db7fd6496`.

| Binary | SHA-256 |
| --- | --- |
| StackOBot.exe | `5886c3553870ddf92cdb7256c9a39004cd0a95defe528cb407a33ba26cf8aa34` |
| editor/UnrealEditor-AnomalyBench.dll | `c7924005510d7256f77ef3172327c750afee180781e3011400feeb5f212531e0` |
| editor/UnrealEditor-AnomalyCapture.dll | `6aac004f303d1936dd3b3ece405572f5eadb5896d165cb9ddb24fd5ce09506ba` |
| editor/UnrealEditor-AnomalyControlServer.dll | `649114d43c997b63c78bc0d1006f2c26dbdf80ec0c5070724806196f6eb9349d` |
| editor/UnrealEditor-AnomalyInjector.dll | `e00f5529470c8a50237e51f9b44b4f19721ed5d17988f412887200e10785687a` |
| editor/UnrealEditor-AnomalyShaders.dll | `1a8b1589b44f4a98131792533e5954e31dd30c1fca91a0af6b9737ddccb568dd` |

Archive `E:\IA_BuildCache\_r921\09205\archives\probe-7bebf09-11B38564`; runtime source `7bebf09646b9cb45144847bf282d57626a131b85`.

| Binary | SHA-256 |
| --- | --- |
| StackOBot.exe | `11b38564193725851481e6134d69c9d6ef0c09905cbef77d3faa93791f804d0c` |
| editor/UnrealEditor-AnomalyBench.dll | `3f47105c8b1be9ffbd0e9762733f00398b0af39e65dd6cb7656c2934571c35bb` |
| editor/UnrealEditor-AnomalyCapture.dll | `9e5ab9451b5e1522a163e98216bba3a91cefbba64955a595ce621f79273fbc1b` |
| editor/UnrealEditor-AnomalyControlServer.dll | `d8cbd85bd723f1e34df39f24c8dcf37b7581058b84c5845694dcda609fb3dd35` |
| editor/UnrealEditor-AnomalyInjector.dll | `bc4eccaf4bc5cf97131c406a1a6ff76d57ae3042697994be19ddc7d508ea4d98` |
| editor/UnrealEditor-AnomalyShaders.dll | `790294cb2358b3f4e1b3370ab4ed8d1bc458f9a588f986fc11f93b77a517592f` |

Archive `E:\IA_BuildCache\_r921\09205\archives\probe-fe94c63-51DCD233`; runtime source `fe94c63c0be3659d95f874e6593a768c37289118`.

| Binary | SHA-256 |
| --- | --- |
| StackOBot.exe | `51dcd2338f10a76d302247f7dc1f673f13b0ac179df86358008812378307f162` |
| editor/UnrealEditor-AnomalyBench.dll | `c7924005510d7256f77ef3172327c750afee180781e3011400feeb5f212531e0` |
| editor/UnrealEditor-AnomalyCapture.dll | `6aac004f303d1936dd3b3ece405572f5eadb5896d165cb9ddb24fd5ce09506ba` |
| editor/UnrealEditor-AnomalyControlServer.dll | `649114d43c997b63c78bc0d1006f2c26dbdf80ec0c5070724806196f6eb9349d` |
| editor/UnrealEditor-AnomalyInjector.dll | `3fd86391d28f1f61f2c5e49a3ae8a582f207269e90b7f271dfaf8455bf6de457` |
| editor/UnrealEditor-AnomalyShaders.dll | `1a8b1589b44f4a98131792533e5954e31dd30c1fca91a0af6b9737ddccb568dd` |

Restored host archive: `E:\IA_BuildCache\_binary_baselines\release-2026-10-05-23098fa-218BA4C7`. These six original hashes match the restored live files; its executable also supplied the release refusal leg.

| Restored binary | SHA-256 |
| --- | --- |
| StackOBot.exe | `218ba4c79bb2bf6d9e14706772c04b3c53027cc05b84d4ab100a3349b650b7d6` |
| UnrealEditor-AnomalyBench.dll | `9f967665827cd1358b5f914466c36ecc45e47e5e972c749aa0221283bc92dfce` |
| UnrealEditor-AnomalyCapture.dll | `470ebe5f6c5d8fa11d9872392035c62c2bab2480aee462a4d4d58ac59e8a5be3` |
| UnrealEditor-AnomalyControlServer.dll | `0c9c329bedf22be975a09042eb242a21cda726fcab63553240a46685b1dd8cf1` |
| UnrealEditor-AnomalyInjector.dll | `45a1512f0c7b59e321d8de97dd03e23b9bee175f503f974fb5a200f99b5a1b90` |
| UnrealEditor-AnomalyShaders.dll | `43d78d878cd1e8cd41b23db1fb747fc5c6bd6a103cb5089bb4fb984b1afc47db` |

Accepted-baseline executable archive: `E:\IA_BuildCache\_r921\09203c\archives\probe-c80ef9b-1EA292E8\StackOBot.exe`; SHA-256 `1ea292e815e5acf5c6ab214043b7d255c0f35d7e456c23c0d4e0a67fe489e891`. Every baseline leg restored its then-current staged executable by receipt, including the failed attempt. Final v2 baseline legs restored 4458129F.

| Fixture archive | SHA-256 |
| --- | --- |
| E:\IA_BuildCache\_r921\09205\archives\fixture-part-a.zip | `3048293c52dd76899d323fdb5fbad16c8b8803e69d655618bf9ac92ad80e2187` |
| E:\IA_BuildCache\_r921\09205\archives\fixture-part-b-calibrated.zip | `d9fcecdde43467ce139ccf2e56b9d25a5542c5109780a67a5289a9cb5499cfb4` |

Staged cook archives are `E:/IA_BuildCache/_r921/09205/cooks/part-a-final/archive/Windows/StackOBot`, `E:/IA_BuildCache/_r921/09205/cooks/part-b-final/archive/Windows/StackOBot` and `E:/IA_BuildCache/_r921/09205/cooks/part-b-final-v2/archive/Windows/StackOBot`. Their executable hashes equal 381ED90F, F3A8C68C and 4458129F above. Each cook result.json records the full executable/pak/utoc/ucas hashes; every recorded artifact was rehashed at final verification. No BuildUAT was run.

6. Deviations and retained failures. None changed a production threshold or weakened the frozen pixel criterion. The following are explicit deviations, including corrections to initially wrong expectations or newly introduced implementation gates: The first complete runtime matrix passed on 1fda1cb/F3A8C68C, but the subsequent stack census caught eight wrong fields after the early unnamed-stack refusal. This was a real diagnostic regression. Commit 502de71 changes only one census aggregation line; frozen expected counts are unchanged. The rebuilt head repeated all required proof legs, both scopes on both fixtures and both real counting mutants. The failed earlier census remains in evidence as a can-fail.

- Part A initially tried a protected residency timestamp; compilation rejected it. The replacement uses the public residency query. The earliest successful implementation build was superseded without a binary archive; all retained later/final archives are listed above. An archive attempt during relinking failed on the open executable handle before creating an archive.
- Two initial Part A legacy-census warmups failed with D3D12 incompatible texture copies before diagnostics. The opt-in diagnostic became passive because source inspection showed warmup/cache misses could allocate private VTs and print names. Other census forms were unchanged. The exact historical copy caller was not isolated; Part B resolves queued RHI texture references and uses integer-buffer readback, and all final active warmups pass.
- Initial fixture planes were edge-on because positional Unreal Python Rotator arguments were wrong; explicit fields repaired them. A camera diagnosis was wrong and did not fix that issue. Lighting was calibrated before accepted pixel gates.
- Part A expectations were corrected twice from unchanged source: existing 6% view filtering excludes the small controls, and a cold mixed ordinary/VT footprint adds texture_uniform, reducing single/all UV VT-only from 4 to 3. Earlier files/results remain; corrected expectations were frozen before reruns. No eligibility setting changed.
- Staged command text exceeded UE’s 16384-character limit; a short exec macro repaired the harness. Two subsequent census attempts overlapped the null window and another count block had unrelated telemetry interleaved. Those legs are invalid and retained. Later command timing, a null-before-census gate and LogAnomalyCapture Warning fixed the instrument; thresholds did not change.
- The first lever-audit selftest used its inherited default user TEMP path before the job-specific TEMP/TMP override. Its helper removed its own scratch. Subsequent runs used job scratch; no owner configuration was edited.

- Part B fixture size changed from 2048 to 1024 so Drop 0 has 255 combined entries instead of 1023, within the unchanged 512-entry bound. Authored LOD biases 0/1/2 exercise first-mip bookkeeping. The first resize importer reused filenames and overwrote VT flags; its first runtime probe is invalid. The second import saved correct assets but exited with transient missing-miplevel errors. A later load explicitly finished async asset compilation and verified all six textures/materials with 0/0. Fixed-light calibration preceded frozen pixel criteria.
- Early Part B probes refused combined readiness. A non-null indirection texture reference can point at the engine’s black fallback even when the public allocation description has no indirection. The corrected predicate uses that description and resolves RHI references after queued updates. Another initial gate mistook uniform-cache bUpToDate for page readiness; source shows it means complete shader map. That newly introduced gate was replaced with actual render-map/cache/compiled-stack/producer identity and retention of the exact allocation; shader completeness is a note. All required page checks remain. These changes are recorded, rather than claiming the initial gates passed.
- The first identity capture ended at 397 frames because it retained the positive cooldown. The completeness checker rejected it. Identity-only cooldown changed from 96 to 179 for exactly 480 frames; no reader tolerance changed.
- The uncontrolled early-bind PIE mutant was naturally ready and is inconclusive. The predeclared controlled staged experiment delays new producer scheduling, exposes 14 wrong onset frames, and passes a normal-implementation control under the same delay. No engine, pool, bias, cache-flush or source-residency workaround was used.
- The first accepted-baseline refusal attempt crashed in inherited capture uniformity warmup before frames/fire. The retry disabled only existing process-local TexCorruptWarmDraw and reached the unchanged stack_layout selector refusal. A supplementary actual-attempt check also closes a startup-banner false-positive in the older generic reason reader; deleting actual refusal lines is caught on all three raw logs.
- Automatic string classification left metadata candidates unresolved. Structural EH4 metadata decoding covers the complete referenced runs; the RHI flush timing identifier is supported by engine source. The earlier F3A8C68C review left 55 candidates before its supplemental review; both reviews remain recorded. All 209 are resolved without a string allowlist; the appended semantic mutant is still caught.
- Ordinary hold/proxy/UV/normal regression legs retain their accepted 942×869 profile; new stack proofs and single-layer VT regression use native 1920×1080. No preallocated/runtime-VT fixture was authored; the permitted unnamed-layer refusal case was authored and proved. The runtime fixture proves a three-layer stack, while the implementation handles complete supported layouts up to the engine’s eight-layer limit; this is not an eight-layer runtime or office-host proof.

7. NEEDS-DECISION items: none. Independent reviewer ruling and any later delivery remain outside this branch proof. Full report: [report.md](D:/IntrusiveAnomalies/_relay/runs/2026-10-06-092-05-vt-blur-stacks/report.md).

VERDICT: GREEN
