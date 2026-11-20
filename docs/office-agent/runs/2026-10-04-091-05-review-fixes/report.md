VERDICT: GREEN

2. Owner summary

Pull `feat/m52-proxy-blur` at `5d6352b0f503f62d980b83ef0eb741af59426f84`.
`fix/m52-label-timing` is `1eaf2056d550110f6178ce3ee9e361d94bdcc248`; `feat/m53-uv-normal-corruption` is `4c93e5fb0375507f7bba8eec54a89b2661ae4a25`.
All three feature branches are pushed fast-forward; the required proof passes. This is the implementer result for independent review.

3. Findings and engine proof

Evidence root **W** = `E:/IA_BuildCache/_r915`. Predictions were written before legs in [predictions.md](E:/IA_BuildCache/_r915/predictions.md). File:line references below use the final proxy tree. Final documentation commits preserve the tested Source, Shaders, Config, descriptor and tools trees; bindings are in [git-final.json](E:/IA_BuildCache/_r915/git-final.json).

**F1.** Owner scene and viewport are checked before colour dequeue and mask service. A mask family without a bound fails closed. Sample-frame and unique-family keys prevent stale or aliased service. Skipped owner renders become counted `capture_unpaired` drops with no picture, labels row or annotation membership. Pending masks are matched by session index, then consumed or drained without counting dropped pictures. Rejection telemetry contains counts only.

Source: [Source/AnomalyCapture/Private/AnomalyCaptureOwnership.h:5](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCaptureOwnership.h:5), [Source/AnomalyCapture/Private/AnomalySceneViewExtension.cpp:65](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyCapture/Private/AnomalySceneViewExtension.cpp:65), [Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp:73](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp:73), [Source/AnomalyCapture/Private/AnomalyMaskServe.h:5](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyMaskServe.h:5), [Source/AnomalyCapture/Private/AnomalySveCapturer.cpp:55](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyCapture/Private/AnomalySveCapturer.cpp:55), [Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:4642](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:4642), [Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp:1359](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp:1359).

UE 5.1.1 source verification: `Engine/Source/Runtime/Engine/Private/GameViewportClient.cpp:1274` supplies `InViewport` and `MyWorld->Scene`; `Engine/Source/Runtime/Engine/Public/SceneViewExtensionContext.h:27` exposes viewport/world; `Engine/Source/Runtime/Renderer/Private/SceneRendering.cpp:4552` assigns per-scene family frame numbers. Thus frame number alone is insufficient.

Pure tests: ownership 19/19 and mask service 16/16. Owner/non-owner orders, same-scene wrong viewport, absent scene/viewport/bound, stale/future samples, expiry and equal scene-frame numbers are covered. The review predicate, fail-open, scene-only, stale-sample, no-expiry and frame-alias mutants all fail. Evidence: [pure results](E:/IA_BuildCache/_r915/pure/results.json).

Engine setup: PIE in a new window, two realtime editor-world viewports, with editor draws after capture sampling and before the game draw. A green plane exists only in the verified distinct PIE world. Every labelled PNG is measured for its marker; erased-marker controls fail. State/scalar read-backs are wiring evidence only.

| New-window PIE | Fires | Labelled | Colour / mask rejects | Realtime editor views | Wrong pictures | Missing masks | Evidence |
|---|---|---|---|---|---|---|---|
| UV TSR | 3 | 144 | 2753 / 1849 | 2 | 0 | 0 | [P_P_uv_owner_final](E:/IA_BuildCache/_r915/P_P_uv_owner_final.owner.json) |
| Shared proxy TSR | 3 | 144 | 2753 / 1465 | 2 | 0 | 0 | [P_P_proxy_owner_final](E:/IA_BuildCache/_r915/P_P_proxy_owner_final.owner.json) |

Both ownership legs have zero unflagged E1 t50/t10 disagreements. Their maximum t10 tails are 4 frames (UV) and 27 frames (shared proxy), inside the unchanged 40-frame window.

| Skip control | Dropped indices | Rows/images | Labelled | Unpaired | Missing masks | Mask summary vs rows | Evidence |
|---|---|---|---|---|---|---|---|
| Clean lead-in | 61, 62, 63 | 909 | 144 | 3 | 0 | exact | [P_P_uv_owner_skip_final2](E:/IA_BuildCache/_r915/P_P_uv_owner_skip_final2.skipped.json) |
| Inside active event | 201, 202, 203 | 909 | 141 | 3 | 0 | exact | [P_P_uv_owner_skip_active_final2](E:/IA_BuildCache/_r915/P_P_uv_owner_skip_active_final2.skipped.json) |

Can-fail: C7FB5212 is the archived `58386be` build with production trees identical to `d660b5d` ([binding and hashes](E:/IA_BuildCache/_r915/old-build-source-binding.json)). Both old-build ownership legs label 144 editor-view pictures across three events; every picture lacks the game-only marker, including normalized-coordinate checks. The old pictures are 942 x 869; the game viewport is 1280 x 992. The UV repeat and proxy control have valid distinct-world metadata. E1 reports no judgeable target event in those wrong views. This reproduces F1; the guard is not supported merely by missing old rejection counters. Evidence: [old_P_uv_owner_final](E:/IA_BuildCache/_r915/old_P_uv_owner_final.owner.json) and [old_P_proxy_owner](E:/IA_BuildCache/_r915/old_P_proxy_owner.owner.json).

**F2.** Every successfully committed slot contributes to an explicit changed-component scope. Deferred/uninstalled slots contribute nothing; exact revert clears the scope. Excluded components also release still-owned census stencil tags, preserving host replacement values and host-disabled custom depth.

Source: [Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:218](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:218), [Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:1249](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:1249), [Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:202](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:202), [Source/AnomalyCapture/Private/AnomalyStencilTag.cpp:171](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyStencilTag.cpp:171), [Source/AnomalyInjector/Public/AnomalyLabelSync.h:469](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyInjector/Public/AnomalyLabelSync.h:469). Six scope-release pure tests pass; old no-restore and unowned-restore mutants each fail two cases.

The new static two-component actor has eligible C1 and C2 with a null material slot, no override, drawing the engine default material. Fixture receipts verify that state; UE `StaticMeshRender.cpp:2225` confirms the null-material fallback. Source hashes/context are in `W/engine-source-proof.json`. Each row below requires C1-only mask, m26 and both bboxes, visible C1 change, no C2 pixels changing by more than 12 levels, and C2 RGB inside the clean-frame noise envelope.

| m53 fixture | Fires | Labelled | C2 mask pixels | m26 C1 pixels, each fire | C2 >12 pixels | Raw C2 max delta | Evidence |
|---|---|---|---|---|---|---|---|
| UV PIE | 3 | 144 | 0 | 35721/35721/35721 | 0 | 2 | [M_P_uv_scope_final_v2](E:/IA_BuildCache/_r915/M_P_uv_scope_final_v2.scope.json) |
| Normal PIE | 3 | 144 | 0 | 35721/35721/35721 | 0 | 2 | [M_P_normal_scope_final_v2](E:/IA_BuildCache/_r915/M_P_normal_scope_final_v2.scope.json) |
| UV staged | 3 | 144 | 0 | 65536/65536/65536 | 0 | 2 | [M_S_uv_scope_final_v2](E:/IA_BuildCache/_r915/M_S_uv_scope_final_v2.scope.json) |
| Normal staged | 3 | 144 | 0 | 65536/65536/65536 | 0 | 2 | [M_S_normal_scope_final_v2](E:/IA_BuildCache/_r915/M_S_normal_scope_final_v2.scope.json) |

All material restores are exact and final stencil hygiene matches. C2 byte equality initially failed (1-2 levels also appeared in clean frames); the explicit gate amendment uses the existing co-user mean-noise envelope plus 0.05, with a failing 20-level pixel control. This is not a byte-identical-image claim. Can-fail: C7FB5212 masks C2 on all 144 labelled frames, 35,721 C2 pixels per frame: [old_P_uv_scope](E:/IA_BuildCache/_r915/old_P_uv_scope.scope.json).

**F3, requested part.** UV uniformity examines RGB only unless the resolved material is masked/translucent or has an opacity/opacity-mask input connected. Normal rules remain unchanged. Source: [Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp:761](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp:761) and [Source/AnomalyInjector/Private/Anomalies/TexCorruptPure.h:1682](E:/IA_BuildCache/_r52p_src/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/TexCorruptPure.h:1682). `Material.cpp:3961` verifies the cached runtime property-connection API. Uniformity tests pass 36/36; always-count-alpha and never-count-alpha mutants fail.

The fixture has constant RGB and checker alpha. Opaque attempts must be refused as `texture_uniform`; masked events must visibly change on every labelled image.

| Material/environment | Events | Labelled | Uniform refusals | Visibly changed labelled frames | Evidence |
|---|---|---|---|---|---|
| Opaque PIE | 0 | 0 | 3 | 0 | [M_P_alpha_opaque_final](E:/IA_BuildCache/_r915/M_P_alpha_opaque_final.alpha.json) |
| Masked PIE | 3 | 144 | 0 | 144 | [M_P_alpha_masked_final](E:/IA_BuildCache/_r915/M_P_alpha_masked_final.alpha.json) |
| Opaque staged | 0 | 0 | 3 | 0 | [M_S_alpha_opaque_final](E:/IA_BuildCache/_r915/M_S_alpha_opaque_final.alpha.json) |
| Masked staged | 3 | 144 | 0 | 144 | [M_S_alpha_masked_final](E:/IA_BuildCache/_r915/M_S_alpha_masked_final.alpha.json) |

Can-fail: C7FB5212 admits three opaque events / 144 labelled frames, with zero pixels changing by more than 12 levels and raw maximum delta 1: [old_P_alpha_opaque](E:/IA_BuildCache/_r915/old_P_alpha_opaque.alpha.json). Readme section 8.8 states plainly that variation in unsampled packed-map channels or zero-strength normal inputs can still label invisible events; a measured visible-change gate is deferred by the ruling.

**F6.** [tools/verify_capture.py:3264](E:/IA_BuildCache/_r52p_src/AnomalyInjector/tools/verify_capture.py:3264) collects every labels-side identity before transition filtering. [tools/test_annotation_transitions.py:70](E:/IA_BuildCache/_r52p_src/AnomalyInjector/tools/test_annotation_transitions.py:70) covers the exact wrong-ID control, missing/duplicate IDs and transition-free unlabelled entries. The standalone `tools/annotation_transition_check.py` calls this same helper; its CLI and the `verify_capture` hook were both exercised. Seven annotation tests and 68 label-rule tests pass. Full Python discovery also passes all 51 tests, including the annotation tests (`W/python-unittest-final.json`).

| Control | Old checker/hook | New CLI | New verify hook |
|---|---|---|---|
| Original transition-free capture | PASS | PASS | PASS |
| ID changed to uv_corruption@999999\|WrongTarget | incorrect PASS | FAIL | FAIL |

Evidence: [exact control and both entry points](E:/IA_BuildCache/_r915/f6-cli-hook-controls.json). All 18 final candidate captures have exact labels/annotation event-ID set equality and matching transition export. Annotation-only unlabelled records remain allowed by the general checker.

**F4/F5.** Documentation only: [docs/client-readme.md:629](E:/IA_BuildCache/_r52p_src/AnomalyInjector/docs/client-readme.md:629), [docs/client-readme.md:1408](E:/IA_BuildCache/_r52p_src/AnomalyInjector/docs/client-readme.md:1408), [docs/client-readme.md:1494](E:/IA_BuildCache/_r52p_src/AnomalyInjector/docs/client-readme.md:1494). Size/ratio refusals are selection heuristics; copy draws are not bound to one coherent retained snapshot under heavy streaming churn, and tripwires count rather than veto.

4. Final proxy regression, gates, delivery

| Configuration | Fires | Labelled | Unflagged t50 / t10 | Maximum t10 tail | Missing masks | Co-users / failures | Evidence |
|---|---|---|---|---|---|---|---|
| Shared proxy PIE TSR | 3 | 144 | 0 / 0 | 27 | 0 | 20 / 0 | [P_P_proxy_owner_final](E:/IA_BuildCache/_r915/P_P_proxy_owner_final.coverage.json) |
| Shared proxy staged TSR | 3 | 144 | 0 / 0 | 24 | 0 | 20 / 0 | [P_S_proxy_tsr_r2_final](E:/IA_BuildCache/_r915/P_S_proxy_tsr_r2_final.coverage.json) |
| UV PIE TSR | 3 | 144 | 0 / 0 | 5 | 0 | 1 / 0 | [P_P_uv_cousers_tsr_final](E:/IA_BuildCache/_r915/P_P_uv_cousers_tsr_final.coverage.json) |
| UV staged TSR | 3 | 144 | 0 / 0 | 3 | 0 | 1 / 0 | [P_S_uv_cousers_tsr_final](E:/IA_BuildCache/_r915/P_S_uv_cousers_tsr_final.coverage.json) |
| Normal PIE TSR | 3 | 144 | 0 / 0 | 2 | 0 | 1 / 0 | [P_P_normal_cousers_tsr_final](E:/IA_BuildCache/_r915/P_P_normal_cousers_tsr_final.coverage.json) |
| Normal staged TSR | 3 | 144 | 0 / 0 | 0 | 0 | 1 / 0 | [P_S_normal_cousers_tsr_final](E:/IA_BuildCache/_r915/P_S_normal_cousers_tsr_final.coverage.json) |
| Shared proxy PIE AA off | 3 | 144 | 0 / 0 | 0 | 0 | 20 / 0 | [P_P_proxy_off_final](E:/IA_BuildCache/_r915/P_P_proxy_off_final.coverage.json) |

All seven configurations use proxy source `dc42e35` / EXE `3ADC2EA4`. The unchanged temporal defaults are 3 / 40 / 1. E1 checks all unexcused t50/t10 disagreements, including onset; t10 tails fit 40. Strict AA-off edges also pass. TSR strict lower-threshold tails are retained as diagnostics, not substituted for the requested t50/t10 gates. The inherited AA-off UV/normal reader uses the established PIE-settle exclusions and preserves raw 084-09 pixel/edge parity.

Every labelled mask PNG has the picture dimensions and exact recorded target-pixel count. The separate CB_CoUsers fixture supplies real same-texture UV and normal peers; proxy has 20 co-users. Collateral RGB stays within pre-event clean noise plus 0.05. Shifted co-user pixels fail; freezing target pixels makes the shared-proxy reader report zero visible events.

The office kit on this local TSR PIE capture judges three shared-proxy events: start +0 for all three, end +0 for all three, wrong-object 0, failures 0. [Kit output](E:/IA_BuildCache/_r915/kit/P_P_proxy_owner_final.txt). No office data was imported.

Across 18 distinct final candidate captures: 48 positive events, 2301 labelled pictures, zero missing measured masks, and exact event identities/transition export. [Final integrity certificate](E:/IA_BuildCache/_r915/final-integrity.json); [proof summary and exact session paths](E:/IA_BuildCache/_r915/proof-summary.json).

| Gate | Passing result | Can-fail evidence |
|---|---|---|
| Strict + normal builds | Game and Editor 0 errors / 0 warnings on all three | 4c82ee2 fails C4456; failed logs retained |
| Strict includes | 60 / 68 / 69 independent header TUs; no unity, no PCH, IWYU; Build.cs restored | Actual strict compiler rejection at 4c82ee2 |
| Ownership / mask arm | 19 / 16 pure checks; final pixel owner tests pass | Review predicate, wrong viewport, fail-open, stale, expiry and alias mutants |
| Scope / uniformity | 6 / 36 pure checks and both engine cases | Old scope and alpha-only opaque captures; restore/alpha mutants |
| Annotation / labels | 7 / 68 tests; exact final IDs and transitions | Review wrong-ID control fails CLI and hook |
| E1 pixel coverage | 0 unflagged t50/t10; t10 <=40 | Banked 16-window control has 61 unflagged t10 frames |
| Mask PNG integrity | All final labelled PNGs measured and counted | Unmeasured, null/missing/blank PNG and wrong count mutations fail |
| Collateral / visibility | C2 and co-users within measured clean noise | 20-level shifts fail; frozen target has zero visible events |
| Office kit | 3 judged, start/end 0, wrong-object/fail 0 | 93 selftest cases including negatives |
| Idle / foreign editor | Every launch passes; final legs monitor other editor/game PIDs | 10 policy controls reject missing metrics, editor/GPU/game/build and low disk/headroom |
| Bench levers | Source and six binaries pass on each branch; Shipping guards retained | Lever audit 12 selftest cases |
| String scan | ASCII and UTF-16; every addition reviewed; zero unresolved | Unapproved ASCII and UTF-16 additions are detected |
| Archives | Source, copied files and every ZIP member re-hashed | Valid ZIP with one changed member byte fails digest |
| Comment/diff checks | Stripper before each commit; staged diff/check reviewed | No ignored source comments or unresolved conflicts |

Gate evidence: `W/pure/results.json`, `W/pure/scope-results.json`, `W/idle-gate-controls.json`, `W/CONTROL_P_proxy_tsr_long_516CBFDF.coverage.json`, per-capture `*.mask-integrity.json`, `W/shared-pixel-can-fail.json`, `W/artifact-accounting-can-fail.json`, `W/string-accounting-can-fail.json`, and `W/strict-<source>/summary.json`. String review classifies full encoded-byte ranges against PE metadata and relocation-normalized baseline bytes; literal baseline matches are recorded.

**Changes to proof or implementation, all retained and disclosed.**

- F2 needed one additional stencil cleanup: an untouched C2 kept an old census tag, leaving later m26 reads NOT_MEASURED. The final implementation restores only still-owned values. Its first compile failed because `Prior` shadowed an existing local; the follow-up rename passed. No failed build was used for acceptance.
- Exact adjacent C2 byte equality failed at two levels; phase-matched references and gamma-only tonemapping still failed at one level. All readings remain. Before final legs, the gate was explicitly amended to the existing clean-noise mean envelope +0.05, plus zero >12-level C2 pixels and a failing 20-level control. Tonemapping was restored; E1 thresholds/windows were unchanged.
- Two skip controls found mask-summary defects: already-counted clean outcomes and later active mask results. The 33adf06 and 23a7da5 failures each have 909 rows versus 912 mask outcomes. The final source fixes both and all three branches were rebuilt; the full final matrix uses the final binaries.
- The first marker metadata queried the editor world during PIE and received null. The final harness caches it before PIE, requires two valid distinct worlds and enumerates the editor world. Final owner captures and the old UV control were repeated.
- The first seven-second skip trigger landed in the clean lead-in. The active control now waits for delivered session index 195, rejects a late trigger beyond 215, and requires dropped indices inside the event span. This reads an index, not the anomaly driving signal.
- The original UV/normal fixture had no in-view source-texture peers. A separate duplicate fixture adds actual co-users, and all four final TSR legs use it.
- The first staged proxy run had only two positive fires because one attempt was texture_uniform:pending. Its failure is retained. The repeat requests four attempts with unchanged 192/48/192 timing and requires at least three positive fires.
- Fixture authoring first used an unavailable root-component API; the owned partial assets were receipt-cleaned and authored with the supported property. The scratch editor helper first used an unavailable console-command type; it was corrected before proof. A logging-cadence correction changed only observer logging.
- During the accounting rebuild, one staged co-user leg was intentionally stopped before engine launch, and two own waiting scripts were terminated by recorded PIDs. A repeated archive/string negative-control filename collision was resolved with distinct paths; previous control bytes/hashes are retained.
- The existing fix branch was already checked out in an untouched old worktree. This Git version required `worktree add -f` to create the requested temporary checkout. The old worktree files/index were not changed. The temporary checkout was removed by receipt after completion.
- The initial F6 prediction was broader than the ruling about annotation-only records; it was narrowed explicitly before engine proof. Every labels-side ID remains required. Final documentation heads have exactly the tested production trees.

- Closure restoration first found an absent original CrashReportClient directory. The script now creates the validated parent before copying the receipted original; all 157 original config files subsequently hash-matched. The failed attempt and correction are recorded in [restore-retry.json](E:/IA_BuildCache/_r915/receipts/restore-retry.json).
- The push preflight initially assumed a tracked Config directory. It stopped before pushing because Config is absent. The binding now records absence explicitly and requires identical entries in the tested and final trees; the atomic push then succeeded.

**Bench settings and restoration.** All commands and per-leg process/binary receipts remain under `W/plans`, `W/args`, `W/ev`, `W/runs` and `W/receipts`. Process-local settings: `-IAIBench`, `-NoLiveCoding`, `-unattended`; 30 fps PNG capture, seed 777; SVE=1, Delivery=0, Pace=1, Mask=1, TargetMask=1, RunLog=1, FocusGate=0; Config `0 192 48 192 3` (staged proxy repeat: 4 attempts); caps 900/1100/1400 as recorded. AA method 4 for TSR or 0 for AA-off; auto exposure, eye adaptation, Lumen diffuse, dynamic GI and reflections disabled; CaptureBench.Marker=0; stuck-mip route auto. UV uses tile; normal uses invert. `ShowFlag.Tonemapper 0` occurred only in the failed gamma trial, and was removed for final legs. No transition cvar was overridden (all remain -1; effective 3/40/1 for TSR).

The ownership helper starts new-window PIE at requested 1280 x 1024 (actual game view 1280 x 992), with two realtime editor viewports and a PIE-only marker. Fixture/staged views use 1280 x 720 where recorded. `IAI.Bench.Review915.Skip 3` temporarily disables only the owned game viewport render and restores it. Project02 LODBias=1 was supplied by an owned temporary DefaultDeviceProfiles.ini for cooking/legs and removed by hash receipt. These are bench settings, not shipping defaults.

On closure: original stage EXE plus five container files are restored and re-hashed; Saved/Config originals are restored and only receipted new files removed; original fixture assets are unchanged; the ten requested new fixture assets remain and are archived. The editor-only Review0915 helper is archived and receipt-removed. The host has final proxy source/binaries. The temporary fix worktree is receipt-removed. No foreign process was killed and no security setting changed. Receipts: [host restoration](E:/IA_BuildCache/_r915/receipts/host-restored-final.json), [stage restoration](E:/IA_BuildCache/_r915/receipts/stage-final.json), [worktree removal](E:/IA_BuildCache/_r915/receipts/fix-worktree-removed.json).

Closure verification found no editor/game processes and free space of D: 79.1 GiB / E: 274.3 GiB, both above 50 GiB.

**Commits pushed.** The push was atomic and fast-forward only. Exact remote heads were read back after push. No master, m51 or tag change was made.

| Branch | Previous remote | Pushed head | Proved source |
|---|---|---|---|
| fix/m52-label-timing | 8badc2339aff9db4c73e8d8371432a1e7b115456 | 1eaf2056d550110f6178ce3ee9e361d94bdcc248 | 97c20f29fdec067652bb485bda6c45a130892176 |
| feat/m53-uv-normal-corruption | 783a7689d41d83d65884e73d447d335a44fdafac | 4c93e5fb0375507f7bba8eec54a89b2661ae4a25 | 839f429c793ff6217bb4ecee3ac9f747ec8b191a |
| feat/m52-proxy-blur | d660b5d6e67c7af8b0f264e59cf66ed02f433968 | 5d6352b0f503f62d980b83ef0eb741af59426f84 | dc42e35fffca3d496b80d90e353c1956bd40e775 |

| Every new commit | Branches receiving it | Subject |
|---|---|---|
| 6415b62b456c3c5a3facc4785e0feb802a9ebe52 | fix, m53, proxy | fix(capture): bind colour and masks to the owning viewport and sample |
| a15b24d0b18ef2156570273f4355cb22ae127530 | m53, proxy | fix(texcorrupt): scope installed components and ignore unused alpha |
| cc74af04ffcf04efcf0890d32a560c3fee97c89e | m53, proxy | fix: merge capture ownership and event identity checks into m53 |
| f54ff87e4e160b458a70803d3b61478265cdb395 | proxy | fix: merge review fixes into proxy blur |
| 4c82ee280b5f7e7c804cc9d537656b0d50138347 | m53, proxy | fix(texcorrupt): restore excluded component stencil ownership |
| 4f0ac50d65f62b9c61e056a430909fdde312850f | proxy | fix: merge changed-component tag cleanup into proxy |
| 45eba38c50f6e3f593522a784f8f956af0477612 | m53, proxy | fix(capture): avoid shadowing the tracked stencil state |
| 33adf06e58b9e98855495c5b20ae397966b1b770 | proxy | fix: merge strict stencil compile correction |
| 03e7eeb217c9f091f563f0728a706fdbefb6e328 | fix, m53, proxy | fix(capture): exclude dropped requests from mask summary totals |
| a8d7b2b951858b32ba6ce77b8f36b0f90a075267 | m53, proxy | fix: merge dropped-capture accounting into m53 |
| 23a7da5cd6c2a1e09bce55ac925f278b9b4a192b | proxy | fix: merge dropped-capture accounting into proxy |
| 97c20f29fdec067652bb485bda6c45a130892176 | fix, m53, proxy | fix(capture): drain dropped masks by session identity |
| 839f429c793ff6217bb4ecee3ac9f747ec8b191a | m53, proxy | Merge capture mask discard accounting into m53 |
| dc42e35fffca3d496b80d90e353c1956bd40e775 | proxy | Merge capture mask discard accounting into proxy |
| 1eaf2056d550110f6178ce3ee9e361d94bdcc248 | fix, m53, proxy | docs: close F1 and F6 review proof with measured controls |
| 0e1647268804819a694a3f7cbf72ba5c413d300f | m53, proxy | Merge fix review proof and documentation into m53 |
| 4c93e5fb0375507f7bba8eec54a89b2661ae4a25 | m53, proxy | docs: record scope alpha and final regression proof |
| 5d6352b0f503f62d980b83ef0eb741af59426f84 | proxy | Merge final review fixes proof and documentation into proxy |

**Binary SHA-256 inventory.** `EXE` = `StackOBot.exe`; `B`, `C`, `S`, `I`, `H` = `UnrealEditor-AnomalyBench.dll`, `UnrealEditor-AnomalyCapture.dll`, `UnrealEditor-AnomalyControlServer.dll`, `UnrealEditor-AnomalyInjector.dll`, `UnrealEditor-AnomalyShaders.dll`. DLLs are under `editor/` in each build archive. The final source builds are 97c20f2 (fix), 839f429 (m53), dc42e35 (proxy); all other candidate builds below are superseded and retained.

[6415b62b456c3c5a3facc4785e0feb802a9ebe52 / BF06A030](E:/IA_BuildCache/_r915/builds/6415b62/manifest.json)

```text
EXE bf06a03030653ee5a0597e0e4696957377cac6684ad87e3d398afb09f2cb4380
B e5a6e0125446dbf823d62981eb2a1a6800453568bf88d866ce15b4de5a9c535e
C 9c5fe72f250e3d8df86c6ed7719254f12a1413861658aefdc072928fbe974001
S ed7ee76c324f4e0bbce5ae86f957db7240a49e397d554a99447cb24e3571547d
I 28e682696757c75918d2baee89b68bc30b72cdf485d9d0742635e0e8e364eabb
H a0d4a24389f5e08288614a3193ec834db6da70edb761ee1c225203250238e92e
```

[cc74af04ffcf04efcf0890d32a560c3fee97c89e / F9D89AAA](E:/IA_BuildCache/_r915/builds/cc74af0/manifest.json)

```text
EXE f9d89aaa8086bc03f8ac76f32d99f4c02b1394f368dc47b4da490179147d693f
B e369ed525167f5bae803e9172c10eb32196014b5b49535b5a606490c9feab6f1
C f95ebd18a40037c5f2aec79d5b17057e35187bb8208b982a211afe90230a1858
S 8dbe771a59afe86dca15ebfacefb0a7e6664cec336ef606457be930ece7b36dc
I 11805daa805b167b58d28ff2e9274ce34ce5cf19df0b756de485cf80e5923b83
H b02c237028814c29c9007b460518e106591ea34396d7d02e3cab2358fc692f42
```

[f54ff87e4e160b458a70803d3b61478265cdb395 / 4BDF1FBF](E:/IA_BuildCache/_r915/builds/f54ff87/manifest.json)

```text
EXE 4bdf1fbf9d01d619f6b6245394124f4afb6baecf2e02c97122f4079c1a8e61d3
B b78c0922c09eae3f9bfc6a694d5ad749f3996b36a894c70549704f57d2aa6510
C d5d820307d844bac0c05e698befcf9ac08ecc88aecfcd884fb870778091db725
S 02488d61d7c4359af304c2192cae74998eec40662405062a8f3c2e51e8f1747e
I 73433d570fa38eff25484d9778136edba5b2570ae2a7e8237d39c57fc89ae4c7
H dad3308f0dbac565dd63d91095a633faadf8772b456222a8c96be8e12182c94f
```

[45eba38c50f6e3f593522a784f8f956af0477612 / D9AE9F03](E:/IA_BuildCache/_r915/builds/45eba38/manifest.json)

```text
EXE d9ae9f035986c65038d44cd1412e2083190b9e6fd58995916a8c64cf09711820
B 4dc2e66ad8148b8763bea5459226d20779c133aa71126edc6bb0c296cbaf5315
C 213c7cb52fd82d50727b5e24bbae374506d54dab2d498d9ac1cc102f9d3f4222
S d700f08cad3d0f9fba431722f7292f7330955942ff4d218a6aa1bfe72cd054e9
I 0d8dfcbef78d34a352c84bbd5e4901d9d04abff5e183b7473926dfe04e5e0405
H 53d16a2151dda00de5816d0a6206fdc155fd1845ad2cebbc8f78e8ef7c236373
```

[33adf06e58b9e98855495c5b20ae397966b1b770 / 02C04928](E:/IA_BuildCache/_r915/builds/33adf06/manifest.json)

```text
EXE 02c0492832805fb58b6b405975a8baba345387ccb47343949979d16759d02306
B c919dd4b5cd94599ae65d8e07ebb26b7dde1cbedf1d1ae5669937d5e2a977570
C e589424d1d04f361ddc772d8524f21f638256072333f4444ee90d07903377274
S e5dfa77e2d9acd1a18760af4ec1724b477911f53a51adc3a60023ea299b4e8f8
I 1fe23a8027fae7c48cc663775c16d85ac40ee49fbd95a05f3e67a183357b998f
H c133c256b29ee56a7f64448d91194b3f6cd5b04d210d02fbb74c1761aebd2a9e
```

[03e7eeb217c9f091f563f0728a706fdbefb6e328 / E9AE1D06](E:/IA_BuildCache/_r915/builds/03e7eeb/manifest.json)

```text
EXE e9ae1d06417855c2f6adba4dfbbc8d8e4274c521b19ad3db7188156c3cde91b7
B b7f954c678d4da65d7b17ec9926df840098d4fa4a6cc9cee8b0e31ea06f77b1e
C 13fc37d5b569d0bd00ca7489e795e7ffdfcb9d4347631f198479b2e9b5ef1704
S 770910c8ee8294014598eea09dc6c1d71a4c53b03a0dfd6ccd95846c87253021
I c240622e2f0d2ce8c9629dd1458683981c77e69083e3da09d7df617c7a9a2f47
H 761b9e2b93d2f32e46a52f8a07e59d431abd6dbeb41f6dd776ba481b18f4f004
```

[a8d7b2b951858b32ba6ce77b8f36b0f90a075267 / 234D6DDB](E:/IA_BuildCache/_r915/builds/a8d7b2b/manifest.json)

```text
EXE 234d6ddbc7cead2d3c4747888102e50326a15f0b182addf23ad46923c26734f2
B 1320a7721b312c1acbfb6f5405db395efd3a1cb4084fd0aa0201209e0de3f0a9
C 87340693421d97d0a50a35b811088f6f0c47a75537fb442f7be32d57ce439e35
S b7746dca42629833bd470500fce460145d40adf9cdff5121d594b5adb5afd040
I 77acce4320e6019bd8a8b1fe1195e822634e085eb7e61040bf69047c8ad706ec
H a2bf33fa340459567881dbfd9cb32d38c91a769b8cd55bb618b3cae61e4efec8
```

[23a7da5cd6c2a1e09bce55ac925f278b9b4a192b / 1514B1C7](E:/IA_BuildCache/_r915/builds/23a7da5/manifest.json)

```text
EXE 1514b1c7c3b4092d6fb15b74dc4b305853c2b0c51649a21407a636754c5a00d0
B fd3609e59a4febb2cb9307ecb3fc5206137e3755d9482a41efebeca92e875817
C 72a4a2b410df2246ce22259d1c81b5f8309b7b6486457e8596265b5e5105faf3
S 9cd2f7b48a8ac52e7cf594a48903bbd254b42728dbd1881f3eaccb75cc0ad3a0
I fd1063f3464bfb09ba2cba1f4e8b89e767f87710ccb282de1702f998bd8b408b
H 0b81118195e4307f57dd4e131a73d4819c63ea91850df3c1e4de1f465e0d5c87
```

[97c20f29fdec067652bb485bda6c45a130892176 / FEA79D94](E:/IA_BuildCache/_r915/builds/97c20f2/manifest.json)

```text
EXE fea79d944393cfa579e3249f0f8cdaeac148524bdfe3ff9d513b2531a5c56776
B d6bd2a03337dece4c3a0cd27d03f937ee8a5dc1b45d750a12267042551c901a5
C b7163c77bf8f77e1a06216f645459ee2673047cd3e643e98cc8f7ec9c27842ba
S ab1bede082b8ea977ce264595bf939f292e70960f564089ed9901318ab232922
I e9b101253e18df689735200993cf991cbe7c3f2bc10a392a9fb05ce7200b8598
H 1d2012bd2f8684ef89559d63575a2daf317e28092e1faa42672da5776c3ab6c2
```

[839f429c793ff6217bb4ecee3ac9f747ec8b191a / 57B70BF8](E:/IA_BuildCache/_r915/builds/839f429/manifest.json)

```text
EXE 57b70bf8357881d8dbf8e5b570d108877cb9a8d1fc60220792e2802f8171ac74
B 62a68d8fcf56362ea6f2efffde729dac51a8d4868a346dfd849ff7742f44192b
C 535baa1396110b83daa668573a146f535d8476ac8b3ec7d29579bbcbdb0ad838
S fc8d63d6e7866604e7f51e8014ac4ae82d93378f96280c7d5811f1cc5dcd1d7a
I 47e5f8c386ba3a4d5b2f2b709b7d18586656089dc68ab3fe0ebfc26503549863
H 5dcca137cf83af5d2c52842e5887c39aeba30348adba39a83473301080f3933d
```

[dc42e35fffca3d496b80d90e353c1956bd40e775 / 3ADC2EA4](E:/IA_BuildCache/_r915/builds/dc42e35/manifest.json)

```text
EXE 3adc2ea4aaa9d5812ae0a5d9265980444c6757551714891b568f407b19d5dcdf
B 3ad7150fae0c753475bec4da10be31bf1f914f372a983dcf7b51c26776513fab
C aac3179337ca01adebbc3a10f4e1b6a174a2dc1a121e69fcf866937b50b81847
S f5aab717cf56f0485e26121c386f48d9ea50df242ac7754bcfe1e565dff5c93f
I c4f8cabb7384cdb4accb2535570e5cffa65f4ab8aea22a257cd8ee1eef0c4d3b
H 32708d26b716be7ce8c74cfd19269c806e3cf00c9a4b62724ca007e14b642618
```

Old control: `E:\IA_BuildCache\_r913\builds\58386be` (production-equivalent to d660b5d).

```text
EXE c7fb52129a7007b4c7473b1d5009b9f49cf1bbc1b16259859fd4ae180704aede
B 121a6c57e82c509092ccbe058f45a8ada0bb556da0c889611c576e40da805d5d
C 75469c16d402bb4b4ca843bfb7763eaa8283faa73bd1dca1741735d3dd7d3dc9
S 7bb459b99d48d2b97b3dfb919987b8de07d850c8bdd662fc56b1039d557922d7
I 43da801869b2c768c77b50969910224e134f903402bd59819368446fa3ef22b0
H 07c9e52d4b47ba85c52d6110494b7f5f7d7d3727faaa62ad9675f5ab757e0bdb
```

| Other executed/restored/control binary | SHA-256 |
|---|---|
| D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe | 5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b |
| D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe | 44f4aae265b37e62a7e167fff17cd55f8aa78030822cbd8425dc023737d81a48 |
| E:/IA_BuildCache/_r53_host/StackOBot/Plugins/Review0915/Binaries/Win64/UnrealEditor-Review0915.dll | e052589e226d8979681cd7225302f84ddee8e53cb89a04263fa391f8153f4708 |
| Prior helper DLL (receipt only): E:/IA_BuildCache/_r53_host/StackOBot/Plugins/Review0915/Binaries/Win64/UnrealEditor-Review0915.dll | 46c78b43716a6e0328916a5aadc03cf30c2f5e71e82d855821226c4fe5f8b54d |
| E:/IA_BuildCache/_r915/pure/mask/test.exe | 1f1a81fe8d7f8e6f5165c909e0d26337b6de04f6ae5dd7e5e0b793e1909f4449 |
| E:/IA_BuildCache/_r915/pure/mask_fail_open/test.exe | 5beeba8a45a7be967eaa4e2305febefcfb1f46ae297fb369f5c99d0b7c720ba9 |
| E:/IA_BuildCache/_r915/pure/owner/test.exe | c9bf8968dbaf09a8d9b63be59f04df089dc9e0a97765cec677ebe55c4fbb7155 |
| E:/IA_BuildCache/_r915/pure/owner_frame_alias/test.exe | 8b10ab8256df4a95dc22b49ac6ade90f4c1d611287e4f036e8d72b3b43350694 |
| E:/IA_BuildCache/_r915/pure/owner_no_expiry/test.exe | a7635e2cac6f097332c44a08fa15cc080bda70b5c52cff5275ad308f927404a0 |
| E:/IA_BuildCache/_r915/pure/owner_review_mutant/test.exe | 32dbeb7ce4f5d586908d282d51edf5cc63c0c114936b33232659e6d74c1dc914 |
| E:/IA_BuildCache/_r915/pure/owner_scene_only/test.exe | 61b83c82d3e0fefc84ad896d6083d3772be52289968dd33f3fc428553254805e |
| E:/IA_BuildCache/_r915/pure/owner_stale/test.exe | 41409a1d9ca83c1ce14e8dd7212ecec2ce33553d8fa66cc7c3ae8d4d84a78e13 |
| E:/IA_BuildCache/_r915/pure/scope_exit-r2/test.exe | 7f3116f1e32a3e54d4f3aead0068abe6ed0039633361821dbe4c15ab5b16ba30 |
| E:/IA_BuildCache/_r915/pure/scope_exit_old-r2/test.exe | 61c7ff546cb11422dc5fd9fcbb39ef6895d27179a9f1b3a3577cf49cb54564a0 |
| E:/IA_BuildCache/_r915/pure/scope_exit_unowned-r2/test.exe | 828a130f1a1c68949d7da70cb1c858e3071844df3158987dab3f8c75411f6274 |
| E:/IA_BuildCache/_r915/pure/uniform/test.exe | 7d82eed803ab91ae7328d36c895cf2d060303b3de593331b1ca373568de8149e |
| E:/IA_BuildCache/_r915/pure/uniform_ignore_alpha/test.exe | 48a85f0f4a5d714253301389c4c608ca94a4de19896bbfd1fda78090ea59f69e |
| E:/IA_BuildCache/_r915/pure/uniform_old_alpha/test.exe | 39275ed8d1f3fa604dbe057be7668d03389fc1c9ca6adeee5a7e16251629dcb1 |
| Restored stage original: D:/IntrusiveAnomalies/StackOBot/Builds/BenchGate/Windows/StackOBot/Binaries/Win64/StackOBot.exe | 2fcdf059176da4e65645b4523c1efd73b13ab2489a8ffc3867ac2de26177cb36 |
| Negative ZIP member: E:/IA_BuildCache/_r915/archive-accounting-member-negative-control.zip!StackOBot.exe | 96bebd129854df74af24f67e49a520e72cba8308f8c7b1e2011bbfc66a79ca97 |
| Negative ZIP member: E:/IA_BuildCache/_r915/archive-dc42e35-member-negative-control.zip!StackOBot.exe | 27d1386fa48e82b67cbede0e59f39a408265dc0ef33a9c8d671ad2f1e2f5639b |
| Negative ZIP member: E:/IA_BuildCache/_r915/archive-member-negative-control.zip!StackOBot.exe | 77e3715cb8bb6ce580708b29204e3610cd4399693b1bb4cf619d197bbe662abe |

**Archives.** All positive binary archives were re-hashed from source to copy and again member-by-member after ZIP creation. Unpacked binary archive directories have the same path without `.zip`. Superseded builds remain available.

| Positive archive | SHA-256 |
|---|---|
| [09105-6415b62-BF06A030.zip](E:/IA_BuildCache/_r915/archives/09105-6415b62-BF06A030.zip) | 5f824fb6eb0923dc2aa777f6576b928a6ab0132dd25978b09218412e7e59da5d |
| [09105-cc74af0-F9D89AAA.zip](E:/IA_BuildCache/_r915/archives/09105-cc74af0-F9D89AAA.zip) | c65c887e432f04365ccd924c24fe699e61ad0514df1ab2c2b7ced9b545f1f323 |
| [09105-f54ff87-4BDF1FBF.zip](E:/IA_BuildCache/_r915/archives/09105-f54ff87-4BDF1FBF.zip) | 96546fe3dab822604a8432fb7a5e4ae271fb4bc553c9a5833db607f95e59b1be |
| [09105-45eba38-D9AE9F03.zip](E:/IA_BuildCache/_r915/archives/09105-45eba38-D9AE9F03.zip) | 0f9a844e12955f620ff6cf73e7acdb0ee97eda4a377ae272f2d8a359265e8d74 |
| [09105-33adf06-02C04928.zip](E:/IA_BuildCache/_r915/archives/09105-33adf06-02C04928.zip) | 1ac3ca9c4bb59d3f566263fb3c4ddc84babd4ae458b1d2b6041cbc01af0aecb9 |
| [09105-03e7eeb-E9AE1D06.zip](E:/IA_BuildCache/_r915/archives/09105-03e7eeb-E9AE1D06.zip) | 0ab79154121bc9c56c7812e8006f8bfae8085bb0ba829f7ea2cd33131f3e017d |
| [09105-a8d7b2b-234D6DDB.zip](E:/IA_BuildCache/_r915/archives/09105-a8d7b2b-234D6DDB.zip) | 42d6c9bdac4d70ea1116c47dde8075ab737287675d441ec8e326536d3990b151 |
| [09105-23a7da5-1514B1C7.zip](E:/IA_BuildCache/_r915/archives/09105-23a7da5-1514B1C7.zip) | fe87dfca5bb5eb0ce6c6a730dae3bb7c2dd8f6f181d68bbbd1da181faf8e6cd8 |
| [09105-97c20f2-FEA79D94.zip](E:/IA_BuildCache/_r915/archives/09105-97c20f2-FEA79D94.zip) | 5245f4074d1834b5a907f02d7d8a99c1a4ccb31a6d8faa384245f060420fecee |
| [09105-839f429-57B70BF8.zip](E:/IA_BuildCache/_r915/archives/09105-839f429-57B70BF8.zip) | fcef207679b0ed458170ecdb39580d7f5efed91c6b553b7ea18bbfbc6b5a5815 |
| [09105-dc42e35-3ADC2EA4.zip](E:/IA_BuildCache/_r915/archives/09105-dc42e35-3ADC2EA4.zip) | 032b3247c0834c8ad19e403d65a4c180a562422da3bf8eabc9fb94323c212c05 |
| [09105-fixtures-and-harness.zip](E:/IA_BuildCache/_r915/archives/09105-fixtures-and-harness.zip) | f4407b37141852ce5de012c9b24c70908dbde588848ca0ba9e9a87eecda2ef43 |

Negative-control ZIPs are intentionally rejected artifacts:

| Negative archive | SHA-256 |
|---|---|
| [archive-accounting-member-negative-control.zip](E:/IA_BuildCache/_r915/archive-accounting-member-negative-control.zip) | 95ee5ecff7b4dd9b43f2857ed2bbb6281dd50b70df6271852264cee14e7dfc68 |
| [archive-dc42e35-member-negative-control.zip](E:/IA_BuildCache/_r915/archive-dc42e35-member-negative-control.zip) | b748ed59becfb2a498e956bb122873247edbf23a2913e401a79f793ee23e8e12 |
| [archive-member-negative-control.zip](E:/IA_BuildCache/_r915/archive-member-negative-control.zip) | 691bf878ed1dd676e3d955fffee6dfbdef955d08c0c135eccc3a2fbea967391e |

Archive/hash manifests: [final binaries](E:/IA_BuildCache/_r915/archives-final.json), [fixtures and harness](E:/IA_BuildCache/_r915/fixture-harness-archive.json), [artifact controls](E:/IA_BuildCache/_r915/artifact-accounting-can-fail.json). The final fixture/harness archive contains the ten new assets, final editor helper, authoring/capture/readout scripts, predictions and member hashes.

Documents updated: `CLAUDE.md` status; `docs/architecture.md`; `docs/client-readme.md`; `docs/gotchas.md` G462-G464; `docs/sessions/2026-10-04-091-05-review-fixes.md`. Full report: [report.md](D:/IntrusiveAnomalies/_relay/runs/2026-10-04-091-05-review-fixes/report.md).

5. NEEDS-DECISION items

None for this implementation brief. The ruling retains the measured visible-change gate for unsampled channels/zero-strength normals, replacement of size/ratio selection heuristics, and coherent copy-snapshot handling as follow-ups. The independent reviewer still rules on this delivery.

VERDICT: GREEN
