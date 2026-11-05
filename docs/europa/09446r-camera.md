094-46r stops with NEEDS-DECISION: D3D12 D4 exceeds its sealed count bound on six unlabelled frames. The amplitude bound passes. The remaining two launches and GO3 were not run.

GO2 adds IAICameraFarOnly in AnomalyCameraSession425.cpp:19,45,83. It requires a non-Shipping build, IAIBench, EA_Core, a requested camera_clipping route, IAICameraSlabBench and IAICameraRegression. It holds EA_Camera at the far pose for the complete2352-frame default-route section and omits the near-wall tail. The existing2640-frame combined schedule is unchanged without this flag. The start receipt adds far_only; saved poses and zero camera labels independently verify the far-only condition. The anomaly class, slab algorithm, GN4 counter and default pool are unchanged from eea2acd.

Each near-plane pair uses every index0-2351 against the corresponding near10 control-a prefix. D3D11 reuses the accepted09446 control and positive; their binaries differ from the new floor session only by this guarded native change and its additive receipt. D3D12's controls, near-plane session and positive use the same new binaries. The D3D11 floor is sealed before the explicitly authorized re-comparison. D3D12 floors are sealed before the positive starts.

| RHI/floor | Maximum RGB delta | Maximum pixel count at that level |
|---|---:|---:|
| D3D11 session, accepted09446 | 1 | 560155 |
| D3D11 near-plane | 2 | 255 |
| D3D12 session | 1 | 559838 |
| D3D12 near-plane | 2 | 240 |

The reader records every frame's256-bin distribution and counts above every level. GO2 selects the larger maximum delta and the count belonging to that level; equal levels use the larger measured count. It never carries the lower-level count allowance into the higher level. Thresholds were not retuned.

D3D11:2640frames,144labelled,2496unlabelled,0failures; maximum unlabelled delta2/count254 stays within2/255. D3D12:2640frames,144labelled,2496unlabelled; frames1156,1163,1996,2003,2087,2094 each have255pixels at delta2 against240. No pixel exceeds delta2. Both positives label spans2352-2399,2448-2495,2544-2591 exactly, and every labelled frame changes921600pixels above each floor. Both baseline controls and both far-only sessions label0. Near100 restores to10; controls retain10. The shifted-label reader can-fail produces six boundary mismatches on each RHI.

Every new session passes H4, strict texture G1-G3 and G5:780object-mask frames;22checker and13pink fires; whole-frame IoU1.000 on286checker and157pink frames;0spill;140clean edges;35residue-zero reverts;0drops;0unavailable; every LOD event changes target pixels (minimum event peak68061). No explained-pixel fallback is used. EA_Core has no skeletal candidate; accepted fixture proofs remain unchanged. r.CustomDepth reads1/1/1, custom-depth components/tagged/polluted are0 and stencil writes are0. GN4's counter is0 and matches annotation entry counts.

D3D12 1279x719 combined and default-only, the GO3 schema comparison and office-card read commands are unrun. The four defaults remain blinking,lod_corruption,missing_texture,corrupted_texture; camera_clipping is opt-in only. Origin remains c5d0919; no push or tag.

Entry full P9 passes in352.716seconds; entry/exit FJ1 fast checks pass. Editor/Game builds pass attempt1 in52.624/66.890seconds, with no link errors. Focused reader/scope tests, their can-fails, source/comment/lever/string audit and archive rehash pass. The154-check slab self-test is unchanged and was not repeated. The Shipping binary is retained; no Shipping rebuild is claimed.

Raw evidence: D:/IA_Europa/Receipts/09446r and E:/IA_BuildCache/09446r-evidence. Accepted D3D11: the corresponding09446 roots. Per-frame distributions are in floors/*.json and H4/D3D12-1280-positive/01/paired-pixels.json; D3D11's rejudgment is D3D11-rejudged-full.json. stop.json names all six failures. result-summary.json and09446r-receipts.json index the raw files, readers, builds, hash receipts and C: readings.

The launcher is restored to disabled build-only status. No owned runtime/build process remains. No crash, upload sign or protected change occurred; no file was deleted and no machine setting changed. Only completed text receipts created by this job were losslessly compressed, with hashes and mtimes verified unchanged, to retain the3GiB launch reserve. One premature kit invocation occurred before arming completed; it launched no engine process and consumed no runtime attempt. It was then invoked after ARMED. No measured gate was retried.
