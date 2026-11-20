<!-- Collected by Claude Code from the relay run folder's report.md (Codex 094-40, exit 0, 1,612 s); the body below is byte-identical. -->
094-40 GREEN: the EA_Core capture smoke passes every gate on its first slot, and FV6 redaction is complete. Both targets built on attempt 1. All four anomalies fire at least three times; all 19 LOD events visibly change the target. The retry is unused. Code owns the subsequent scan, push and office card; I did not push.

FX2 starts/seeds the bench injector once after its clean pre-pass, continues across the six holds, and stops before the final clean tail. Shared 5.1 reset-on-start semantics remain intact: `d7eee97`, `AnomalyAutoInjectorSubsystem.cpp:231`. The 0.8 s interval, 0.4 s hold and one-concurrent schedule are unchanged.

FX3 retains the two-LOD guard. The game-thread player view feeds stock `ComputeStaticMeshLOD`; select the coarsest unless already drawn, then select LOD0. Citations: 4.25 `Engine/Private/SceneManagement.cpp:451-461,543-568` and `Engine/Private/StaticMeshRender.cpp:151-174,2081-2150`. Minimum LOD, forced selection, distance scale and available LOD are respected. Manual and 5.1 paths stay intact. G510/G511 and the parity sweep record these deliberate 4.25 additions.

Focused tests pass: 35 compiled LOD choices, invalid/single-LOD refusals, and one start/stop across 2,352 modeled ticks. The old six-start policy and unchanged-coarsest choice fail. All 12 kept no-op LOD events fail the new pixel gate. Admission refuses a spent kit, a street profile and a lowered commit line. Keyword-Rotator check: 5/5. No in-editor Python changed, so its Python 2.7 harness is not applicable.

Both builds used BelowNormal priority and `-MaxParallelActions=2`, with no memory stop. Compile errors were zero in every module, including AnomalyCapture and AnomalyInjector.

| Target | Attempt | Start / minimum free commit, GiB | Duration | Exit | Result |
|---|---:|---:|---:|---:|---|
| Editor | 1 | 10.104 / 6.394 | 55.58 s | 0 | PASS |
| Game | 1 | 10.072 / 7.855 | 74.66 s | 0 | PASS |

The map was EA_Core, as FX6 directly requires. Slot 01 ran 04:20:20-04:22:26 IST on November 2 (125.17 s), exiting normally with code 0. Admission: 8.608 GiB free commit, 7.823 GiB physical, 7112.9 idle seconds. The session used the bench-only **0.5% selection floor**; the shipped default remains **6%**. One seed-4242 start is recorded. Output: 2,352 contiguous 1280x720 frames, D3D11, backbuffer format 18 (`PF_A2B10G10R10`), zero drops.

| Anomaly | Fires | Affected frames | Mask frames | Mask pixels, summed | Verdict |
|---|---:|---:|---:|---:|---|
| blinking | 22 | 132 | 132 | 2,937,054 | PASS |
| lighting_mismatch | 13 | 157 | 157 | 56,005,197 | PASS |
| lod_corruption | 19 | 247 | 247 | 3,710,434 | PASS |
| missing_object | 15 | 195 | 195 | 4,468,815 | PASS |

All 731 affected frames have matching masks. All 1,621 clean frames have zero mask pixels and no mask PNG. Annotation schema 2.1 and all 69 event sets agree with the rows; far plane is 1,000,000. Coverage values pass: 56 measured selections and 13 explicitly unmeasurable lighting selections (`valid=false`, `coverage_pct=-1`). Census: peak 3 drawn actors; zero exhaustion, lost, no-pass or polluted batches. FX6 waives the street-only >55 and wall-strip gates.

All 14 component states restore exactly, with 2,352 passing mid-session samples; host custom-depth actors stay unchanged and reserved value 203 is never allocated. All 2,352 views match the player. Eight UI-positive controls, requests 1-8 / sample frames 1676-1683, are absent from those exact captures; no captured frame contains the marker. The self-check fields pass at the first resolved frame and frame 300.

Each LOD event predicts zero-based drawn LOD 1 and forces model 1 (LOD0). The following ranges cover all 13 affected frames per event. Changed pixels use the frozen rule: any RGB channel differs by more than two levels inside the current target mask, relative to the preceding capture. Every frame changes at least 14,455 pixels; every event passes.

| LOD event start frame | Reference capture | Changed target pixels per frame |
|---:|---:|---:|
| 2404 | 694 | 14,455-14,462 |
| 2524 | 814 | 14,455-14,462 |
| 2596 | 886 | 14,461-14,469 |
| 2620 | 910 | 14,458-14,470 |
| 2692 | 982 | 14,455-14,462 |
| 2716 | 1006 | 14,469-14,475 |
| 2932 | 1222 | 14,461-14,469 |
| 3004 | 1294 | 14,458-14,467 |
| 3076 | 1366 | 14,455-14,462 |
| 3244 | 1534 | 14,455-14,462 |
| 3268 | 1558 | 14,461-14,469 |
| 3316 | 1606 | 14,455-14,465 |
| 3460 | 1750 | 14,458-14,470 |
| 3628 | 1918 | 14,458-14,470 |
| 3724 | 2014 | 14,469-14,475 |
| 3748 | 2038 | 14,455-14,462 |
| 3820 | 2110 | 14,455-14,465 |
| 3940 | 2230 | 14,461-14,469 |
| 3964 | 2254 | 14,458-14,470 |

Self-check lines, verbatim:

```text
Capture425 SELF_CHECK stage=start frames=1 size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily=5 synth=0 viewport=SceneViewport client=GameViewportClient viewport_size=1280x720 backbuffer_format=18 slate=1 pie=0 pre_ui=1 ui_draws=4 ui_controls=4 ui_positive=4 ui_checked=1 ui_hits=0 ui_paired=1 tick=OnWorldPostActorTick425 tick_source=build_constant tick_type=2 tick_type_source=build_constant tick_calls=1319 fixed_step=1 fixed_dt=0.033333333 fixed_rate=0 fixed_fps=30.000000 smoothing=0 census=bench_off batches=0 lost=0 no_pass=0 polluted=0
Capture425 SELF_CHECK stage=300 frames=300 size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily=316 synth=0 viewport=SceneViewport client=GameViewportClient viewport_size=1280x720 backbuffer_format=18 slate=1 pie=0 pre_ui=1 ui_draws=8 ui_controls=8 ui_positive=8 ui_checked=300 ui_hits=0 ui_paired=8 tick=OnWorldPostActorTick425 tick_source=build_constant tick_type=2 tick_type_source=build_constant tick_calls=1630 fixed_step=1 fixed_dt=0.033333333 fixed_rate=0 fixed_fps=30.000000 smoothing=0 census=on batches=0 lost=0 no_pass=0 polluted=0
```

FV6 replaced root-name arrays with counts and canonical SHA-256 hashes in `docs/europa/09434-live-capture-receipts.json`. It rewrote 26 commits (25 inherited plus this implementation commit). All 65 commits after `4f46a9c` were checked; changed trees differ only in that file. Source and shaders remain byte-identical to the build. Local-only backup: `refs/backup/europa-pre-fv6-09440`, pointing to `08fe652734994549e084d190fff57207bc2a5c58`.

Old → new commit IDs (full IDs in the tree proof):

```text
47a6972 -> bb1026c
120493b -> 446154f
18235f3 -> 2712820
54e11c9 -> 8d66916
2143215 -> 8ce5063
b1710b7 -> 8e52c9f
a4a0df6 -> 73a235a
9a785e0 -> d021ef3
05c55e2 -> 7d01354
2b9dc19 -> c9aa268
df5dfdc -> efd6734
cedb505 -> 918374d
0284a0d -> d8bea66
07c853e -> a131a9f
99e69a2 -> 774441a
0d40bcf -> 22c04a9
d589dd3 -> af30abe
3d52e45 -> 0944dc6
50ad76f -> 265a131
5971792 -> 1027c38
4dbcf9b -> 8c5f322
8bdcb4b -> 26b845e
d25b8eb -> 22a06ac
d8f6ebf -> 6e6fa11
0621027 -> f9ffcbc
08fe652 -> 252b335
```

Proofs: `D:/IA_Europa/Receipts/09440/redaction-tree-proof.json` and `redaction-verification.json`. Raw session evidence and its hashes: `D:/IA_Europa/Receipts/09440/c5-attempts/C5/01/`. The frozen pre-run journal remains unchanged. Exit checks pass for m51, protected refs, Content, Config, descriptors and project file; 27/27 bound artifacts match `c5-425-binaries-01.json` (SHA-256 `0d76f03baa78f01962344325e6e4b71de01908dba9ac96bbb70494341673e8e1`).

Bound EXE/DLL SHA-256 values (full paths and all 27 artifacts in `D:/IA_Europa/Receipts/09440/binary-state.json`):

| File | SHA-256 |
|---|---|
| StylizedParisStreet-Win64-Shipping.exe | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| StylizedParisStreet.exe | `6d402db1e3ab9f064ae088851e1487170fd112cc027a0ea85c07400da007cbdd` |
| UE4Editor-EuropaPhaseAProbe.dll | `f3b52d42a09992d10633c1198d2382fb53c781d9897b2640aff5c778a3af0653` |
| UE4Editor-StylizedParisStreet.dll | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| UE4Editor-AnomalyBench.dll | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| UE4Editor-AnomalyCapture.dll | `8beb9e787819ea63c003409a3d19104631d37772340b4da53110160dd24ddf42` |
| UE4Editor-AnomalyControlServer.dll | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` |
| UE4Editor-AnomalyInjector.dll | `46d212e01ebd9c84776c9e257778ef4f798ca18510ebbe844a0d46c29a6cbb84` |
| UE4Editor-AnomalyShaders.dll | `b5b767d343422d7360a8b78ec29c7ce0da0090f056d13c5f9fd11b574240e6ad` |

No crash, upload sign or protected change. Root PID 41924 and adopted crash monitor PID 39296 (`-MONITOR=41924`, parent 37304) exited normally; no forced stop or new report folder. The upload check covers Runtime.log and CrashClient.log. No archives, deletions, push or tags. Runtime-only AA0, eye-adaptation0, screen-percentage100, dynamic-resolution0, motion-blur0, 30 FPS, bench view/input, fixed step, pacing, auto settings and stencil/LOD states are restored or end with the process. No gate was relaxed after results. The repository's comment-stripper file is absent; added source was inspected as comment-free.

Built commit: `08fe652734994549e084d190fff57207bc2a5c58`; equivalent redacted commit: `252b33596802c8569f0f1066fa73b6765fd49075`. Final W4 commit on `feat/europa-port`: `d1cdc7b956db277c8a19946131bfa91f33dce5f3`. Nothing pushed.

VERDICT: GREEN | EA_Core passes every capture smoke gate; all 19 LOD events change pixels; FV6 redaction is complete.
