VERDICT: GREEN

**2. Owner summary**

`annotation.json` now lists each event's settling frames and reasons, plus the session's distinct transition-frame count; training drops these frames.  
The second office host's already-flagged two-frame tail would now be listed; the final home matrix has zero unflagged t50/t10 tail frames.  
Pull `feat/m52-proxy-blur` at `d660b5d6e67c7af8b0f264e59cf66ed02f433968`.

Required proof is complete: 16 final-source legs, 80 visible fires, 3865 labelled frames, zero missing measured masks, exact annotation/labels equality, and no changed shared-texture co-users. The deferred 091-02 section 0 and static hold proof pass. All three authorized branches were pushed fast-forward and their remote heads verified. This is the implementer's evidence for independent review.

Full report: [report.md](D:/IntrusiveAnomalies/_relay/runs/2026-10-03-091-03-annotation-transitions/report.md). Evidence: [final proof and pushed heads](E:/IA_BuildCache/_r913/release-success-audit.json), [build/artifact checkpoint](E:/IA_BuildCache/_r913/checkpoint-audit-release.json), [capture/restoration ledger](E:/IA_BuildCache/_r913/close-release-evidence.json), [predictions and amendments](E:/IA_BuildCache/_r913/predeclared.md). Prior RED reports and failed attempts are retained and superseded by this result.

**3. Detection and schema**

The engine inspected is UE **5.1.1**, compatible changelist **23058290**. [Source locations and file hashes](E:/IA_BuildCache/_r913/temporal-source-evidence.json) retain the version-specific evidence. Paths below are relative to `D:/UESource/UnrealEngine/Engine/Source/Runtime`.

| Temporal source | Engine evidence | Implemented detection |
|---|---|---|
| TAA / TSR | `Engine/Private/SceneUtils.cpp:41`; `SceneView.cpp:954` | The same SVE callback that copies color records the actual view method. Methods 2/4 are temporal. |
| Registered third-party temporal upscaler | `Renderer/Private/SceneRendering.cpp:2553`; `PostProcess/TemporalAA.cpp:1015` | Record family upscaler presence after renderer setup; conservatively temporal even if a separate setting disables use. |
| Forward / MSAA | `SceneUtils.cpp:85` | Engine defaults resolve forward/mobile-deferred support and MSAA count; the capture records the resulting view. Unsupported TSR can fall back to TAA. |
| Mobile | `SceneUtils.cpp:45`; `PostProcess/PostProcessing.cpp:1923,2264` | Feature-level defaults include mobile AA/HDR rules; actual view/upscaler evidence is sampled with the capture. |
| Per-view overrides | `SceneView.cpp:954`; `SceneRendering.cpp:2770` | Observe the postprocess view after show flags, realtime/view-state restrictions and family/renderer overrides. |
| Runtime user/scalability settings | `GameUserSettings.cpp:429,466,857`; `Scalability.cpp:466` | Re-read defaults while sampling/completing and inspect every capture receipt. Stock AA quality selects a quality group; game/configuration code can change algorithm CVars. |
| Missing or unknown evidence | Capture receipt and fallback paths | Missing family/view receipt or an unknown method is temporal. Backbuffer/synchronous capture cannot silently imply AA off. |

`bLabelTemporalAa` retains any temporal/default/unknown evidence through the run, so switching AA off cannot remove outstanding exclusions. Confirmed methods 0/1/3 with no other evidence remain non-temporal. Confirmed AA-off runs have zero temporal windows/reasons; independent reasons such as `pie_end_settle` remain exported, as [ruling 1](D:/IntrusiveAnomalies/_relay/runs/2026-10-03-091-03-annotation-transitions/orchestrator-ruling-1.txt) requires. This classifies AA/upscaler history, not unrelated GI/denoiser histories. Mobile, third-party and runtime-switch paths have source/pure-test evidence; no completed device/runtime-switch proof is claimed.

The EFFECTIVE line and `run_summary.label_temporal_source` record observed method bits, upscaler presence, unknown-view evidence, latest default/feature level and retained temporal state. Final matrix and observer runs recorded view bits `0x10` (TSR), `0x4` (TAA), `0x1` (off), upscaler 0 and unknown 0. AA off retained temporal=0.

Schema **2.1** keeps `label_schema: 2`, adds `label_schema_minor: 1`, stable `event_id`, sorted unique event `transition_frames`, and per-emitted-entry `transition_reasons` counts. The root `transition_frame_count` counts the union across events. All existing reasons, onset, partial-onset, detached tails and transition-only events use the same finalized emission predicates as labels.jsonl. Reason counts can overlap; the root does not double-count overlapping events. README section 8.7 says to drop the union: **never use as negatives, never as positives**.

Generic fire-window tracking now covers UV/normal and retains frozen geometry across capture boundaries. The final default is **3/40/1** (on/off/hide), cap 64; confirmed AA off is 0/0/0. The initial judgeable E1 t10 maximum, 36, selected forty with four spare frames. Final TAA PIE reaches 37, one above the prediction; the unchanged window still has three spare frames. The static hold and all required final legs fit the same window. No threshold or window was retuned after those final results.

The [second-office typed readings](D:/IntrusiveAnomalies/_relay/runs/2026-10-03-091-03-annotation-transitions/office-reads.txt) were rechecked: 171 transition entries, off-window 16. The export omission is the direct consumer gap; startup detection had classified that run temporal. No EFFECTIVE line was supplied. No office captures, logs, frames or client files were imported.

**4. Pixel proof, can-fails and deferred section 0**

[Ruling 2](D:/IntrusiveAnomalies/_relay/runs/2026-10-03-091-03-annotation-transitions/orchestrator-ruling-2.txt) supersedes the original strict-null acceptance: unchanged **084-09 E1 t50** is the release gate, with zero unflagged mismatches; the declared window must also cover **t10 plus margin**. Below-t10 residuals above null are diagnostics only. No E1 threshold changed. Predictions preceded the launches.

Final-source results follow. Tails are frames after the final labelled frame. Every row has **zero unflagged t50 and t10 frames**, exact annotation/labels transition equality and zero missing masks. Exported tails contain all measured t10 tail frames in both files. Proxy Source `58386be` uses EXE `C7FB5212` and matching DLLs; m53 Source `e874e28` uses EXE `75A9553C` and matching DLLs. The final pushed documentation heads have these exact Source trees.

| Build / anomaly | Mode / AA | Fires | t50 tail | t10 tail | Exported tail | Labelled / missing masks |
|---|---|---:|---:|---:|---|---|
| 58386be / proxy | pie / TSR | 8 | 7..8 | 23..25 | 40 | 384 / 0 |
| 58386be / proxy | pie / TAA | 8 | 15 | 37 | 40 | 384 / 0 |
| 58386be / proxy | pie / off | 8 | 0 | 0 | 1 (PIE settle only) | 384 / 0 |
| 58386be / proxy | staged / TSR | 7 | 7..8 | 23..24 | 40 | 336 / 0 |
| 58386be / proxy | staged / TAA | 7 | 11 | 36 | 40 | 336 / 0 |
| 58386be / proxy | staged / off | 7 | 0 | 0 | 0 (empty) | 336 / 0 |
| 58386be / uv | pie / TSR | 3 | 0 | 4..5 | 40 | 144 / 0 |
| 58386be / normal | pie / TSR | 3 | 0 | 1..2 | 40 | 144 / 0 |
| 58386be / uv | staged / TSR | 4 | 0 | 3 | 40 | 192 / 0 |
| 58386be / normal | staged / TSR | 4 | 0 | 0 | 40 | 192 / 0 |
| e874e28 / uv | pie / off | 3 | 0 | 0 | 1 (PIE settle only) | 144 / 0 |
| e874e28 / normal | pie / off | 3 | 0 | 0 | 1 (PIE settle only) | 144 / 0 |
| e874e28 / uv | staged / off | 4 | 0 | 0 | 0 (empty) | 192 / 0 |
| e874e28 / normal | staged / off | 4 | 0 | 0 | 0 (empty) | 192 / 0 |
| 58386be / hold_static | pie / TSR | 3 | 7..8 | 24..25 | 40 | 165 / 0 |
| 58386be / hold_static | staged / TSR | 4 | 7..8 | 23..24 | 40 | 196 / 0 |

The maximum final t10 tail is **37**; static Hold0913 reaches **25**. Both fit 40. Shared proxy: 20 co-users/event, all unchanged, zero mask-outside, m26 mismatch or bad bounding boxes. AA off is raw strict/t10/t50 **0/0** with zero temporal windows/reasons. Independent `pie_end_settle` remains one exported frame/event in PIE, as ruling 1 requires; staged AA-off transition frames are empty.

Deferred **091-02 section 0 on final builds** (all visibly changed, zero labelled frames without a measured mask):

| Branch / type | PIE fires | Staged fires | Result |
|---|---:|---:|---|
| m53 / uv | 3 | 4 | PASS; missing masks 0 |
| m53 / normal | 3 | 4 | PASS; missing masks 0 |
| proxy / uv | 3 | 4 | PASS; missing masks 0 |
| proxy / normal | 3 | 4 | PASS; missing masks 0 |
| proxy / shared stuck_low_mip (all AA) | 24 | 21 | PASS; missing masks 0 |

The actual final-source office kit on TSR PIE judged 8/8, start/end 0, wrong objects 0, censored 0, failures 0. [Kit output](E:/IA_BuildCache/_r913/logs/resume2-office-tsr.txt). Per-session verifier, annotation and mask checks pass across all 16 legs.

The AA-off UV/normal confirmations use the existing 090-10e successor for PIE settle handling under ruling 1. The older 084-09 UV reader rejected the valid flags despite raw 0/0; that formal FAIL is retained. Every affected capture has identical visible-frame sets, depths and edges in both readers and raw strict/t10/t50 0/0. The successor passes 13 tests, including failing wrong-PIE/extra-visible/nondecay controls and 400 no-PIE parity cases. All proxy, temporal UV/normal and hold paths retain 084-09. No pixel threshold changed. [Compatibility proof](E:/IA_BuildCache/_r913/resume2-pie-reader-compatibility.json), [selftests](E:/IA_BuildCache/_r913/logs/resume2-pie-reader-selftest.txt).

Completed **initial 16-window candidate** evidence, proxy Source `8131578`, EXE `516CBFDF`; these are not final-40-source passes:

| Leg | Fires | Measured t50 / t10 tail | Listed offset | Unflagged t50 / t10 | Labelled frames / missing masks |
|---|---:|---|---:|---|---|
| Proxy TSR PIE | 8 | 7..8 / 22..26 | 16 | 0 / 61 | 384 / 0 |
| Proxy TSR staged | 7 | 7..8 / 23..24 | 16 | 0 / 52 | 336 / 0 |
| Proxy TAA PIE | 8 | 15 / 36 | 16 | 0 / 160 | 384 / 0 |
| Proxy TAA staged, 512 post | 7 | 11 / 36 | 16 | 0 / 140 | 336 / 0 |
| UV TSR PIE | 8 | 0 / 5..6 | 16 | 0 / 0 | 384 / 0 |
| Normal TSR PIE | 8 | 0 / 1..2 | 16 | 0 / 0 | 384 / 0 |

All six legs have exact annotation/labels transition equality. The proxy t10 failures establish that the old window is too short. Raw temporal offsets are not raw 0/0; their t50 gate passes because every t50 tail frame is flagged. MainWorld hold is **unjudgeable/censored**, with an unstable clean reference; its raw t10 observations reach +46 and do not establish a valid settled bound. The later static Hold0913 fixture supplies the judgeable final hold results above; the MainWorld result remains unjudgeable.

Passive residency observer, **diagnostic only**, EXE `81ED86AB`, temporary patch SHA-256 `dbfce7d56316aa85307c4273f9ddad07bdbc5d205afec33e67bf1a1b97fccbe4`:

| Proxy PIE leg | Fires | Source resident mips / first mip | Resource ID throughout leg | E1 t50 / t10 tail | AA-off result |
|---|---:|---|---|---|---|
| TSR | 3 | 8 / 1, unchanged | 9200883824256 | 7..8 / 24..26 | — |
| TAA, 512 post | 3 | 8 / 1, unchanged | 8370607942144 | 15 / 36 | — |
| AA off | 3 | 8 / 1, unchanged | 9615080352640 | 0 / 0 | strict/t10/t50 0/0; temporal reasons/windows 0; three PIE-settle entries |

Sampling is passive, in the same render capture callback, covering pre/live/tail frames with no missing/bad-status records. It supports **temporal history rather than source re-streaming for these proxy PIE legs**. UV/normal, staged and hold cause checks are deferred as optional by the resume. The observer is behind `-IAIBench` plus `-IAIResidencyProbe`, excluded from Shipping, never merged, and restored before final production builds. No residency behavior was changed. Its sensitivity to an actual residency change still needs the planned hold control.

Above-null diagnostics, with raw clean reference 192 and maximum pairwise RGB MAE over the first 128 clean post-warmup frames:

| Initial-candidate leg | Last measured excess | Limit / status |
|---|---:|---|
| Proxy TSR PIE / staged | +44 / +46 | Independent clean validation passed |
| UV / normal TSR PIE | +45 / +18 | Independent clean validation passed |
| Proxy TAA PIE, 512 post | +203 | Three fires; returned within null |
| Proxy TAA staged, 512 post | recurring through +507 | Small periodic residual; not a clean finite settling bound |
| MainWorld hold | Unjudged | Clean variation too large; static fixture final proof above |
| Final source | Independent raw-null diagnostic not repeated | Required E1 t50/t10 proof is above |

Null calibration changed openly after failures: median, extra identity null, pristine null, raw reference, then fixed pairwise calibration. All variants remain. The initial staged TAA held-out clean check failed; a 600-deferred independent clean run was added under the same algorithm, yielding bound 0.7566197713. Later periodic peaks reached 0.7569643656. These are diagnostic results, not acceptance or a reason to declare 256. The provisional 256 builds are superseded.

Final E1 own-detrend strict residuals are a separate diagnostic from the independent-null table above; strict failures do not override ruling2. Their per-event values and unexcused-frame lists are retained in the final audit.

| Final leg | Strict tail | Strict unexcused frames |
|---|---:|---:|
| F_P_proxy_tsr_58386be_r2 | 35..42 | 2 |
| F_P_proxy_taa_58386be_r2 | 50 | 72 |
| F_P_proxy_off_58386be_r2 | 0 | 0 |
| F_S_proxy_tsr_58386be_r2_stage1 | 37..39 | 0 |
| F_S_proxy_taa_58386be_r2 | 50 | 70 |
| F_S_proxy_off_58386be_r2 | 0 | 0 |
| F_P_uv_tsr_58386be_r2 | 22..26 | 0 |
| F_P_normal_tsr_58386be_r2 | 8..50 | 2 |
| F_S_uv_tsr_58386be_r2 | 22..23 | 0 |
| F_S_normal_tsr_58386be_r2 | 9..10 | 0 |
| M53_P_uv_off_e874e28_r2 | 0 | 0 |
| M53_P_normal_off_e874e28_r2 | 0 | 0 |
| M53_S_uv_off_e874e28_r2 | 0 | 0 |
| M53_S_normal_off_e874e28_r2 | 0 | 0 |
| F_P_hold_tsr_58386be_r2 | 35..41 | 1 |
| F_S_hold_tsr_58386be_r2 | 37..38 | 0 |

Can-fails and tool checks:

| Check | Evidence |
|---|---|
| Annotation equality | 5 tests; 10 doctored variants fail, including wrong/missing/extra/duplicate/shuffled frames, event identity, reasons, total and schema. A real candidate tail removed while its own counts remain consistent fails checker/verifier and the kit. |
| Old export | The strict checker rejects retained old annotation; e09fa9c source exports no event transitions. No new e09fa9c engine run was performed. |
| Pixel/timing gate | Actual initial 16-window proxy captures fail t10 coverage. A one-frame shift of the observer AA-off labels makes unchanged E1 fail 3/3, six unexcused frames; originals pass 3/3 at 0/0. |
| Masks / wrong object | Retained actual missing-mask cases fail with 2 and 1 missing frames. The retained shared-source lever case visibly changes 20 co-users and fails all 8 events; the initial candidate shared proxy passes its co-user checks. |
| Office kit | 93 selftests pass with both PNG decoders; actual final-source new-schema TSR capture passes 8/8. Missing later flags, an undecayed tail and an invalid hold-route PIE flag fail. |
| Existing readers | Verifier 44 tests; label-rule 65; R4 geometry guard 12; lever selftest 12; tail-reader tests 6 pass. |
| Temporal/window policy | 315 C++ checks, zero failures. Six compiled mutants fail: default16, cap32, unknown-as-off, missing-view-as-off, ignored-upscaler and off-as-temporal. |
| Artifacts | Unexpected appended semantic string is detected; corrupted archive member fails re-hash. |

Pixel proof is supplied separately by the final matrix above. The actual new idle-gate function rejects retained foreign-editor inputs; a mutant dropping that condition is detected. Completed run files and negative controls are in the evidence ledger and `_r913/logs`.

**5. Gates, commits, archives, settings and deviations**

| Built Source | Normal Editor / Game | Strict includes | EXE SHA prefix | Lever audit |
|---|---|---|---|---|
| fix `1413a73` | 0 errors / 0 warnings, both | Shared code covered by proxy | 3D437AB3 | 34/34 |
| m53 `e874e28` | 0/0, both | Shared code covered by proxy | 75A9553C | 52/52 |
| proxy `58386be` | 0/0, both | Editor/Game 0/0; 68 header TUs each | C7FB5212 | 53/53 |

Strict settings were restored and normal binaries rebuilt. Defining headers include `SceneUtils.h` and guarded `TemporalUpscaler.h` (5.7) / `PostProcess/TemporalAA.h` (5.1). Final documentation heads have exactly the built Source trees; all three worktrees are clean, diff checks pass and starting heads remain ancestors. Comment stripping changed no Source files.

Two failed build attempts remain: initial UBT `0xC00000FD` stack overflow and initial m53 Game `C1002` out of compiler heap. Unchanged retries passed. No compiler/security workaround was applied. String scans have zero unresolved additions after PE export/debug/FH4/Rich metadata and relocation review. Semantic changes are the new fields/evidence line **plus two updated existing CVar help strings**, an explicit deviation from the fields/log-only expectation so the help describes capture-time detection and default40 accurately.

**All three branches pushed fast-forward; remote heads verified:**

| Branch | Pushed head | Proved Source |
|---|---|---|
| fix/m52-label-timing | `8badc2339aff9db4c73e8d8371432a1e7b115456` | `1413a735858e463cf4c0989b429e11e1b85a1932` |
| feat/m53-uv-normal-corruption | `783a7689d41d83d65884e73d447d335a44fdafac` | `e874e28559d863648e1aee143c317890a49f6ecc` |
| feat/m52-proxy-blur | `d660b5d6e67c7af8b0f264e59cf66ed02f433968` | `58386be800dd0f3585e27958b15ab5e03c8613e9` |

[Push receipt](E:/IA_BuildCache/_r913/logs/push-release.txt). Every newly pushed commit follows; F=fix, M=m53, P=proxy identifies its reachable pushed branches. No force-push, tag, master/m51 write or history rewrite.

| Commit | Branches | Change |
|---|---|---|
| `e151208f7b73535cc74a43cb71f2c6c6107ecb58` | FMP | fix(labels): export event transitions and observe captured temporal history |
| `1df4d26b950e78563edc6ae10d7e4714fa742e93` | FMP | fix(labels): pair hide transitions with rendered AA evidence and verify session exports |
| `e34a79b2a1a3a00a55aeff7f6150b1c80dfdd991` | FMP | test(labels): guard deferred transitions against live geometry reads |
| `49ec8e9151effff3a36e33b5ed0e39ea6a6ac694` | FMP | fix(tools): recognize proxy fire-window transition reasons |
| `9397eec7d1457902c183455fc4b491b9155d8587` | FMP | docs(labels): record 091-03 builds and unresolved release proof |
| `41bc7c7ce645a4a74c36a6b29c9b6af4b81e33ed` | FMP | fix(tools): judge temporal tails beyond a validated PIE exclusion |
| `ab210860ac8631f6313753eb97999b94dff99bf8` | FMP | fix(labels): cover measured long temporal tails and carry fire windows |
| `c43f538beb80ac3765be73613300f01a3b83bfab` | FMP | Set temporal exclusions from measured E1 t10 tails |
| `1413a735858e463cf4c0989b429e11e1b85a1932` | FMP | Include captured AA and upscaler definitions directly |
| `fa6d9db7c1adbc4b4dae3748884038a8a10f19a2` | FMP | docs(labels): record final builds and blocked release proof |
| `9c2c770bead506aa370066dcfda0a57dd65659af` | FMP | docs(labels): clarify additive fire-window and unknown-AA exclusions |
| `6e31fd0659400b860305dcae09b5157361523481` | FMP | docs(labels): record repeated final-proof startup failure |
| `8badc2339aff9db4c73e8d8371432a1e7b115456` | FMP | docs(labels): close final release proof and deferred home confirmation |
| `3f4f1c72fcf4b88bb273e3100e8e701e23a5a0de` | MP | Merge annotation transition export and capture-time AA evidence into m53 |
| `7502b8abd4a24aa98182523704976cc23c980503` | MP | Merge proxy-aware transition reader evidence into m53 |
| `8707549ee977ce072cbf0086557ae3e56c836d6a` | MP | docs: merge 091-03 release evidence into m53 |
| `d52046ed7e7377ca81f883bacd98e48c14d4fd36` | MP | Merge 091-03 office reader corrections and resumed proof status |
| `5a8d3f7f32be11faaea97df13ce135803c67e778` | MP | merge: measured temporal tail coverage into m53 |
| `f9fac4efd9c0cff178d4505a24a3e4508658b04c` | MP | Merge measured t10 exclusion window into m53 |
| `e874e28559d863648e1aee143c317890a49f6ecc` | MP | Merge final AA include discipline into m53 |
| `4ed1cb23fc352ce30a8de815c8ef4c61f817a7e4` | MP | docs: merge 091-03 final build and blocked proof evidence into m53 |
| `869801d35bcd93bb5a0b8bb600d4ab87e5fbafcc` | MP | docs: merge clarified transition training rules into m53 |
| `2b68d18e24dc0bab04bb58e4e285a620ec3fac2e` | MP | docs(labels): update m53 return-tail window description |
| `730927804b017c5862205f2f5cca42cf46bdb5a0` | MP | docs: merge 091-03 final-proof startup failure into m53 |
| `783a7689d41d83d65884e73d447d335a44fdafac` | MP | docs: merge completed release proof into m53 |
| `8131578cc40dce6c1f402672b50e766199a20ce6` | P | Merge annotation transitions and temporal detection into proxy branch |
| `c87b6e0fdb8aae2aea5439e199629a713b0104e7` | P | Merge transition reader validation into proxy branch |
| `c873ab34b9b5a6de13af4ab05eff761e8332849a` | P | docs: merge 091-03 release evidence into proxy branch |
| `21fcb01ca5d0407774c054f9283195bbafd4cf65` | P | Merge 091-03 office reader corrections and resumed proof status |
| `fbed8a94f061bb853802c828d47c6070065f1d0b` | P | merge: measured temporal tail coverage into proxy blur |
| `96e7a32a48322c45d45a85dc62421fa556ed0557` | P | Merge measured t10 exclusion window into proxy blur |
| `58386be800dd0f3585e27958b15ab5e03c8613e9` | P | Merge final AA include discipline into proxy blur |
| `c99d0e9236c14fced3034763d0121bd370a66778` | P | docs: merge 091-03 final build and blocked proof evidence into proxy |
| `b01a1ff221a5734d2167a8ce6c62833542efa88e` | P | docs: merge clarified transition training rules into proxy |
| `6a2dd9af8e1123f694c25227017fcd57d729d896` | P | docs: merge m53 return-tail description correction |
| `04affe64f0ebafc1977e9bf71e571bcdca192154` | P | docs: merge 091-03 final-proof startup failure into proxy |
| `d660b5d6e67c7af8b0f264e59cf66ed02f433968` | P | docs: merge completed release proof into proxy blur |

Archive root is `E:/IA_BuildCache/_r913/archives`. Each ZIP has a same-named extracted directory, manifest and six binary entries. Original binaries, copies and ZIP members were re-hashed; all ZIP hashes were rechecked at close. The release sets are 1413a73/e874e28/58386be; residency0913 is diagnostic-only and all other sets are superseded.

| Snapshot | Archive path | ZIP SHA-256 |
|---|---|---|
| 1413a73 | [09103-1413a73-3D437AB3.zip](E:/IA_BuildCache/_r913/archives/09103-1413a73-3D437AB3.zip) | `f089f8d6f1c454c66176bf2f5261c3d68d0267d70ebbde91dd5d292182231068` |
| 1df4d26 | [09103-1df4d26-7B943716.zip](E:/IA_BuildCache/_r913/archives/09103-1df4d26-7B943716.zip) | `afe04c7b7db667907ef1a37db6f871b1f8f5e406bb2d89d433f6d8304d28c8f6` |
| 3f4f1c7 | [09103-3f4f1c7-2468CB8D.zip](E:/IA_BuildCache/_r913/archives/09103-3f4f1c7-2468CB8D.zip) | `26ff269e3618168c3e1466849030f43076375ee05f3e396e282a2108e1570bbb` |
| 58386be | [09103-58386be-C7FB5212.zip](E:/IA_BuildCache/_r913/archives/09103-58386be-C7FB5212.zip) | `15769e1b2c73f53bdb3e508a886a7ca131d30e5d8e379dd1a5887a823235d517` |
| 5a8d3f7 | [09103-5a8d3f7-BA803C1F.zip](E:/IA_BuildCache/_r913/archives/09103-5a8d3f7-BA803C1F.zip) | `a8327e84773c86fc1c915e378d55b53626fa1d2414d59c5e5b67d316466d49bb` |
| 8131578 | [09103-8131578-516CBFDF.zip](E:/IA_BuildCache/_r913/archives/09103-8131578-516CBFDF.zip) | `c8030645ae7df8ab76b8c781c15f1a9653a0e08e3ccbbbdc18b311b4edea3afd` |
| 96e7a32 | [09103-96e7a32-5AE62AB8.zip](E:/IA_BuildCache/_r913/archives/09103-96e7a32-5AE62AB8.zip) | `d63b87a45c014c5daaa8448b2d5319960e9f3e7a7903fb22aea4556001682b63` |
| ab21086 | [09103-ab21086-33F161F7.zip](E:/IA_BuildCache/_r913/archives/09103-ab21086-33F161F7.zip) | `a01d85c54f59b055df210b7aca564644cfda5730ee93cc8e64e10bf3a4e8266b` |
| c43f538 | [09103-c43f538-E6635DF9.zip](E:/IA_BuildCache/_r913/archives/09103-c43f538-E6635DF9.zip) | `3d7b6f161c8c812180d96065677fb49076d9fedb42d513925cb821c9edae5f35` |
| e151208 | [09103-e151208-479BA61E.zip](E:/IA_BuildCache/_r913/archives/09103-e151208-479BA61E.zip) | `6bf7d1db771476b69645a7e27b0b582593477482243c89ecb7fcea8a753d83ad` |
| e874e28 | [09103-e874e28-75A9553C.zip](E:/IA_BuildCache/_r913/archives/09103-e874e28-75A9553C.zip) | `d28e57765240dd937eceac08b019e25bcbecc3f81a4d049697c9a049a8d3a3ec` |
| f9fac4e | [09103-f9fac4e-327C71D9.zip](E:/IA_BuildCache/_r913/archives/09103-f9fac4e-327C71D9.zip) | `1c082e42b80d7597c415096a2262a39dde6d1764ce47453e4d0fa94be2a8f7df` |
| fbed8a9 | [09103-fbed8a9-C474597B.zip](E:/IA_BuildCache/_r913/archives/09103-fbed8a9-C474597B.zip) | `ff74cb2a80bdec16a1a970a604c4e7359617fe1e72c7f37c631cffbf563987f3` |
| residency0913 | [09103-residency0913-81ED86AB.zip](E:/IA_BuildCache/_r913/archives/09103-residency0913-81ED86AB.zip) | `b8831e8f77da29a47c47bd9b3acd28491e7eb29418cc979cbd0026f75c12923b` |

Every archived EXE/DLL hash follows. `EXE` is `StackOBot.exe`; module names mean `editor/UnrealEditor-<module>.dll` within that snapshot. Repeated hashes are intentional.

| Snapshot | Binary | SHA-256 |
|---|---|---|
| 1413a73 | EXE | `3d437ab3dc596f4c9b015798000b34112f9cce4a4e40b7f2d311f7dbc2559b36` |
| 1413a73 | AnomalyBench | `d86c5da564fd19930d5e4015cf900cd2b2d6b783679b524138b4a36f481d1ac4` |
| 1413a73 | AnomalyCapture | `c70dbbb29d2d3f8b115f1435218d95fec85a4e2565197cd1aeec74483828c5e1` |
| 1413a73 | AnomalyControlServer | `c1574d1691c33a0b92b95796f4fe5af9f7adf053c9b2b006ed18f0d5fe1c81db` |
| 1413a73 | AnomalyInjector | `dc46edc4f2cb7a327825d608e0bba39fe1706266081e058406f1f3136d15db8f` |
| 1413a73 | AnomalyShaders | `d156887332c3a6a7f3a89dccec003cd7d9b5d322a89baed424429003520cefa2` |
| 1df4d26 | EXE | `7b943716d93bad76d580eb087a3fd79e20b77a479ad2fd896d3ac0c5895e21e2` |
| 1df4d26 | AnomalyBench | `79a871d92e2a2df758f29e09f303ee5935a012769089334726b48e11e8194210` |
| 1df4d26 | AnomalyCapture | `727f30d3c875e0f78b65073152b5361e1d7c36372357d3e2328b63374feb97b4` |
| 1df4d26 | AnomalyControlServer | `bfb8d774f04cc47b253180f125bc60cd328240ff3d2fe6f180a3ca7a5240cb94` |
| 1df4d26 | AnomalyInjector | `1d0172b05baed54795d1fd7a5686bbc5793a805e8dbc1af47134aa3b6aaeb4b0` |
| 1df4d26 | AnomalyShaders | `54afaa766ae033086e84a92230fbbd956f4368bcbff39e530c6516abbeda2f14` |
| 3f4f1c7 | EXE | `2468cb8de802568d414e0d123965efbcf91e3a410c12b648fb5f19ea0734bda9` |
| 3f4f1c7 | AnomalyBench | `a89aa5fb1145f6316f879e248fcf3a00a9846234d055f95be1443c435b4bba78` |
| 3f4f1c7 | AnomalyCapture | `da495fc88dfc8533607e4e7270c6b73e3be37daf6b3f48e37b53ea89b401a221` |
| 3f4f1c7 | AnomalyControlServer | `b2ed68140c301d09bd3eac1e4792dc7ef995d0fdf985907dc6db50351e6e6d33` |
| 3f4f1c7 | AnomalyInjector | `a294cf68665be1170322d76c5a4004f26fad0df85ed82cb8dd71874a26d0ce8c` |
| 3f4f1c7 | AnomalyShaders | `708ad46431cb253f374c77e26b0152032765c3519ac6b94abb09399e4bc821d9` |
| 58386be | EXE | `c7fb52129a7007b4c7473b1d5009b9f49cf1bbc1b16259859fd4ae180704aede` |
| 58386be | AnomalyBench | `121a6c57e82c509092ccbe058f45a8ada0bb556da0c889611c576e40da805d5d` |
| 58386be | AnomalyCapture | `75469c16d402bb4b4ca843bfb7763eaa8283faa73bd1dca1741735d3dd7d3dc9` |
| 58386be | AnomalyControlServer | `7bb459b99d48d2b97b3dfb919987b8de07d850c8bdd662fc56b1039d557922d7` |
| 58386be | AnomalyInjector | `43da801869b2c768c77b50969910224e134f903402bd59819368446fa3ef22b0` |
| 58386be | AnomalyShaders | `07c9e52d4b47ba85c52d6110494b7f5f7d7d3727faaa62ad9675f5ab757e0bdb` |
| 5a8d3f7 | EXE | `ba803c1fa854578cf0bd3054505f3f0cbad154de568dcc791a0d63b6aa8857d9` |
| 5a8d3f7 | AnomalyBench | `67d9076a326bbb9efa2ac73ee2c57483c90806493f557d566f088e4940d51eca` |
| 5a8d3f7 | AnomalyCapture | `e6eda8fb4a593e8b1a6f1c9a060b3cfe35a0d5e5da88eb03dcdcd8fdb188e34f` |
| 5a8d3f7 | AnomalyControlServer | `94c33f03aa918169772a1426727eb8422a1c2d38ff9a50f0b4a8ff8020683a03` |
| 5a8d3f7 | AnomalyInjector | `988b6e5a10304e50dc96c13a8ba9118ebdbf4d90631bddcf4b2fe6798a145fd6` |
| 5a8d3f7 | AnomalyShaders | `031a84caf8772998f1ff727a6edefa5bc52d95a21e174a18c7d77e86bb2ca073` |
| 8131578 | EXE | `516cbfdfcf0492344b5bbe5812ea7d90e51bc205cd0cb6f7f88b3e78b487fb58` |
| 8131578 | AnomalyBench | `1ebd19df23476749132f786bfd759bf48c506b99d8b8a675ea134f0942230a0d` |
| 8131578 | AnomalyCapture | `1098a97bc87195e3a08abee45845ce4b36989180aaee645327f00cddd35368f6` |
| 8131578 | AnomalyControlServer | `e357d01373ed7d627eb6398b7b11fb778e97075bb3dcdf2a6c25c818b6ee3f9a` |
| 8131578 | AnomalyInjector | `131bb3af71881f113877bf02de860a750766858b62e345cae3571ef065591772` |
| 8131578 | AnomalyShaders | `c6098a92b6362b82ca6cd86997120ebe40c459b80b4a715139a86705edbc2168` |
| 96e7a32 | EXE | `5ae62ab86fe34a7b5e1ed90d60635922513a354e2dbeebb420818ee30dbde7e2` |
| 96e7a32 | AnomalyBench | `9dbdb0cb40e328034ae7250e98625b48905e83028f5836f26361a2521ae9c13c` |
| 96e7a32 | AnomalyCapture | `c793b183c19784f69e2b59ebd2f5cebc85aaeb61f3bde94330aad418102c2c68` |
| 96e7a32 | AnomalyControlServer | `bf2fd360adfbb2e1f02989c722eb5041f791e67b7c17cb31b1b5ffcf3ae83286` |
| 96e7a32 | AnomalyInjector | `38e7b6747ae92bb7a9008f3d5b32a9d13986d8159564531220c1a0f7adc96830` |
| 96e7a32 | AnomalyShaders | `0c2e726d533ed1727d9d26c83c053b2096ae6ffacb6d2f76516d487871eabb2b` |
| ab21086 | EXE | `33f161f7621ec62af563a3e38139245e4e1766fa5db575232c9be4af1c45c0a3` |
| ab21086 | AnomalyBench | `6242cdc8e81750b8f3c00beb812d0f5ebe940442e51265a3033de5d632ab96a1` |
| ab21086 | AnomalyCapture | `c47db7085f89ab294b1b3da06e82ddbaed4e18a2112812374906bf50d79bc5f2` |
| ab21086 | AnomalyControlServer | `eba007cc65970bf8c8ced868314064214af24a050175719a3f79e7522522f0d0` |
| ab21086 | AnomalyInjector | `ea97e64aed194207f4ee1968e49a40e8828e324a38f9c80ce9b8f11e92fd622b` |
| ab21086 | AnomalyShaders | `92891c4d0b489efdf9b244521fed444ec275cc6e674feacbf0a7d54d4546265a` |
| c43f538 | EXE | `e6635df9538e01de695321706a7907650d925b759b633ed9735c5a9cf320b99a` |
| c43f538 | AnomalyBench | `9320c30b3857923e705cc9184f0a3546ab424af9d3bd8c3ea4e26e49e008bada` |
| c43f538 | AnomalyCapture | `201eb5afa6c330c12c9026ef4538a8e42749281723cb43c6632bb52364f4f95b` |
| c43f538 | AnomalyControlServer | `bcbe1ce4f0e8370aac8ddc94b9cb8790f981e54b2ef8ab5b3dcb0d86f8d9fdd7` |
| c43f538 | AnomalyInjector | `5155b4cdf197897e1335c37d6feec37083f7b6ebf1a12b738bd9a40be33e8748` |
| c43f538 | AnomalyShaders | `53143e5b6a78edbcda5cf7184ad2ae27349b438148f54e27043e931b8304ade8` |
| e151208 | EXE | `479ba61efadef336e16d732773dbe221251d41a887ba1ae85009abc2505a8a98` |
| e151208 | AnomalyBench | `79a871d92e2a2df758f29e09f303ee5935a012769089334726b48e11e8194210` |
| e151208 | AnomalyCapture | `198f5d85661ded5f15729ff9e262a7fbb8c4df063a44dbaaa36652444fde4cb4` |
| e151208 | AnomalyControlServer | `bfb8d774f04cc47b253180f125bc60cd328240ff3d2fe6f180a3ca7a5240cb94` |
| e151208 | AnomalyInjector | `1d0172b05baed54795d1fd7a5686bbc5793a805e8dbc1af47134aa3b6aaeb4b0` |
| e151208 | AnomalyShaders | `54afaa766ae033086e84a92230fbbd956f4368bcbff39e530c6516abbeda2f14` |
| e874e28 | EXE | `75a9553cfc0994503f6031f34bd35b40618dce821b78fe1b34362d64ed3a1036` |
| e874e28 | AnomalyBench | `f308186ab8ae70ff29747f519456fc821eef1c7e521142ba65121332ef9ca5af` |
| e874e28 | AnomalyCapture | `16352ea9def43f65f10492c01cdcd56681a0f4659338b326b86ae815535d1cd3` |
| e874e28 | AnomalyControlServer | `c4c5c7fb6d5a5a6137b5da04e712051f22b772b8858b9693dc1d9efda4d72db5` |
| e874e28 | AnomalyInjector | `b58a7722e97542c27477aa1bfd0a814c98cc939ab1c25b120cf8d2c3cd784494` |
| e874e28 | AnomalyShaders | `7f2cf3c03291ead847c7050ee82e74335b85e6329d09f24e012f42a3dd8bd37a` |
| f9fac4e | EXE | `327c71d99ecf2224ac62646bb8111155d76b077bcbbbedf66109ffe5b12bb9d1` |
| f9fac4e | AnomalyBench | `d87b1927a77fd79348af2b4b862a38526aa040cc43ed04b4106a60dce279fc0d` |
| f9fac4e | AnomalyCapture | `2298a41a7b4a36ba68fdbd07b8a5cdd125e3eafb03999a124b7ff9f028d5c548` |
| f9fac4e | AnomalyControlServer | `25d921721f62ff269fc9d441d941d72743740c127b8318107bdc2e0b381d1e67` |
| f9fac4e | AnomalyInjector | `02790340c21ff598f70e6ff624b1a280ed2889c8ece9d6b1f9de99a125415f79` |
| f9fac4e | AnomalyShaders | `2ed54793fc5047f5dd630cb2dfed8244d52d0a28ea43ddeb25b3fb2524ef4916` |
| fbed8a9 | EXE | `c474597b86be59f67e8b3a70f5fa525539ff8bec66449d90d2b54a70e5cffe85` |
| fbed8a9 | AnomalyBench | `6228c4c3f6a6e40a0188d3d1b28f18c184724fda9f4af76c2f3b151dfff6e0de` |
| fbed8a9 | AnomalyCapture | `9c8ec402b4125edfcba7a0222797477c9710f949b6fc414076b02230ef3132e6` |
| fbed8a9 | AnomalyControlServer | `29b035004494bbefd2ffe59fba25e7ee861440ccd64b6c6af4181970c8b54a63` |
| fbed8a9 | AnomalyInjector | `5834567e3dc152f8d00137beaec47c9c9bc41a7d87aa490c16b6ef561f5ebe66` |
| fbed8a9 | AnomalyShaders | `58d7a23920d7e8a92b8428a8b437e478f51564f8568714e48ba3c1a405e4c9a0` |
| residency0913 | EXE | `81ed86abdf1be517bb6ac80d4e0aad15ecedf1c9a7a3a17e35012ce273e79e3a` |
| residency0913 | AnomalyBench | `6228c4c3f6a6e40a0188d3d1b28f18c184724fda9f4af76c2f3b151dfff6e0de` |
| residency0913 | AnomalyCapture | `38df6b001f817ef42866713c68d07dc94d9396034b921b112e8b0a4230a4e087` |
| residency0913 | AnomalyControlServer | `29b035004494bbefd2ffe59fba25e7ee861440ccd64b6c6af4181970c8b54a63` |
| residency0913 | AnomalyInjector | `5834567e3dc152f8d00137beaec47c9c9bc41a7d87aa490c16b6ef561f5ebe66` |
| residency0913 | AnomalyShaders | `58d7a23920d7e8a92b8428a8b437e478f51564f8568714e48ba3c1a405e4c9a0` |

Additional distinct EXE/DLL hashes, relative to `E:/IA_BuildCache/_r913`, follow. The [complete ledger](E:/IA_BuildCache/_r913/binary-inventory-release.csv) maps all 252 copies to 117 distinct hashes, including duplicate backups/cook outputs; ledger SHA-256 `2e30ed1f6c61d19a6823f0abfd0cfb76b12f5f860ee19161a5a86bdc75b362a2`.

| Representative path | SHA-256 |
|---|---|
| `cook-stage-hold/Windows/Engine/Binaries/ThirdParty/DbgHelp/dbghelp.dll` | `9a02133854ff9f06c3b23a70f8c2a4814b2ed4eef613244b485e3737259ddf9e` |
| `cook-stage-hold/Windows/Engine/Binaries/ThirdParty/NVIDIA/NVaftermath/Win64/GFSDK_Aftermath_Lib.x64.dll` | `516e14af41a91fe960363728965f354aecc06bcdfaef6bcb45dd3537ede09195` |
| `cook-stage-hold/Windows/Engine/Binaries/ThirdParty/Ogg/Win64/VS2015/libogg_64.dll` | `284a6e70a6dce01f8a02d93e1bb78baf0efb6140459c587d8986d59fb6bd675b` |
| `cook-stage-hold/Windows/Engine/Binaries/ThirdParty/Vorbis/Win64/VS2015/libvorbis_64.dll` | `0f66dcb5837e9b59c499277fd272b862043356aa9145802cf0a63e00a1bcca59` |
| `cook-stage-hold/Windows/Engine/Binaries/ThirdParty/Vorbis/Win64/VS2015/libvorbisfile_64.dll` | `a9d6f5538f44011c1f65d7ace64216a237636717cb0c8c59f4f89cc08dd3d24f` |
| `cook-stage-hold/Windows/Engine/Binaries/ThirdParty/Windows/WinPixEventRuntime/x64/WinPixEventRuntime.dll` | `365a63da6a83c91b76bd7f8a9efcc3806b6188a6d29acdbe9d7e43decf3510d8` |
| `cook-stage-hold/Windows/Engine/Binaries/ThirdParty/Windows/XAudio2_9/x64/xaudio2_9redist.dll` | `0e01f400baa09694cacfd2cfbff0d721c17b56785d2d6e29b6fdd117f70ccc1e` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/boost_atomic-vc142-mt-x64-1_70.dll` | `ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/boost_chrono-vc142-mt-x64-1_70.dll` | `17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/boost_iostreams-vc142-mt-x64-1_70.dll` | `7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/boost_program_options-vc142-mt-x64-1_70.dll` | `dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/boost_python39-vc142-mt-x64-1_70.dll` | `9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/boost_regex-vc142-mt-x64-1_70.dll` | `387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/boost_system-vc142-mt-x64-1_70.dll` | `805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/boost_thread-vc142-mt-x64-1_70.dll` | `bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/D3D12/D3D12Core.dll` | `40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/D3D12/d3d12SDKLayers.dll` | `1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/OpenImageDenoise.dll` | `11213c268e6c67262d48e7351e1f6c196acbb91a9a1cf059d4d758c34368f08a` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/tbb.dll` | `6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f` |
| `cook-stage-hold/Windows/StackOBot/Binaries/Win64/tbb12.dll` | `de25bc26f71c5fa53b585116db11d643dc63373fab094e5d786a2ef7eb50786d` |
| `cook-stage-hold/Windows/StackOBot.exe` | `813c9d033e26add68c85bcc9f9b823cc354933fbe2d0d22fb2d72b63ff7e120d` |
| `pure/clean/test.exe` | `3c742046e628205b0227e18e2005330c611f72a1add842ea110e38dea52f469f` |
| `pure/ignore_upscaler/test.exe` | `2b3b7c2ab440f3db94e5f7702737c1b2aee310f69545b1d7bf8be4cfe2ebfe83` |
| `pure/missing_view_off/test.exe` | `7d50e017e8dcd2b1fef3789c7a87cfc539a9a6b179449bfed101938aa8c845d8` |
| `pure/off_temporal/test.exe` | `efada2ef43603ec3484dbdbe962d8800b563b5d647d2dd5cc8cdd5e9660bf847` |
| `pure/unknown_off/test.exe` | `bb393eca9cb25b084f3a225180877d66fc0023a80489e6807e321f70da3579cf` |
| `pure-t10/clean/test.exe` | `c3b690e0ebd5da8fd7d51248a09faa4f18b5a780da9018017c6e08ef11c3cd41` |
| `pure-t10/ignore_upscaler/test.exe` | `873d9ffba4024603b522ffdbb51401dba8ff4a4187965e881192cfd648540d1c` |
| `pure-t10/missing_view_off/test.exe` | `0029ddafb9598aebcb27e1cccda684a572a2f599a664cc559b532e9ee0ae882e` |
| `pure-t10/off_temporal/test.exe` | `461c4907d6d6afbbca6fe56a57604ccd96f5fda90db7e7fd160c80489ee1e745` |
| `pure-t10/short_cap/test.exe` | `aa69fbdf72493e11e466de96a6ba346e2de56012f1feeaffd32d6bf5f56a2d46` |
| `pure-t10/short_tail/test.exe` | `404d4e7c37c78037d5eebe70541076d22e7e04685f911d1e86478abdaa60451f` |
| `pure-t10/unknown_off/test.exe` | `c79299de61a965a0a16b7bf2a76347d5c84e1f30616248165dd12951bdd5166a` |
| `pure-window/clean/test.exe` | `85bdfd4b9eee771cb41e930875d81436687d76918e80e8cd2df2989fdad7387c` |
| `pure-window/ignore_upscaler/test.exe` | `2889436877ee52a1873c866094c4f7151ef1938ebcd11491c8444b9e067b4bdd` |
| `pure-window/missing_view_off/test.exe` | `497e53a241778d8afd4fdcc8147a7d9041edd6e8b4811cdc36663ae1759cdfbd` |
| `pure-window/off_temporal/test.exe` | `89d98b4363b62ceb206584d7b6c6e6fa1e331b004b5df13233ff4d4d19bbb503` |
| `pure-window/short_cap/test.exe` | `c2dea837c7907ebddd7e417bbb560083b4d15fb8458261cc6e1c87e15ab8d3af` |
| `pure-window/short_tail/test.exe` | `c111b530aa3147ddeb76d857b209b637bea4eb7b014b4ad4dfa8391dde636ff8` |
| `pure-window/unknown_off/test.exe` | `e41e54802264adec582e747bcff9b9a167aad12c79d2dd201cb7df4ee9ff094a` |
| `stage-original/StackOBot.exe` | `2fcdf059176da4e65645b4523c1efd73b13ab2489a8ffc3867ac2de26177cb36` |

Engine/launcher executable hashes recorded by capture receipts (staged EXEs already appear above):

- `D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe`: `5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b`.
- Authoring/cook executable, checked at close: `D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe`: `44f4aae265b37e62a7e167fff17cd55f8aa78030822cbd8425dc023737d81a48`.

Settings and restoration:

- All 16 final launches passed three fresh gate samples and the immediate prelaunch check: no foreign UnrealEditor, game/build or GPU-heavy process, person idle at least 60 s, GPU below 10%, commit headroom at least 6 GiB and both disks at least 50 GiB. No device removal recurred. Unknown readings block. The gate waits if the owner resumes work. [Actual-function can-fail](E:/IA_BuildCache/_r913/resume2-final-gate-can-fail.json) was repeated against the final harness after its staging repairs; the earlier result remains retained.
- The exact distinct runtime console settings used are listed below. They ended with the owned process, and no owner session was modified.
- Capture recipes: seed 777, PNG, 192 preframes/48 configured positive frames; post 96/192/512, three/four/eight attempts as recorded. Final proxy legs requested 8 fires, obtaining 8 PIE and 7 staged; UV/normal/hold requested 3 PIE/4 staged and met the minimum 3, with actual counts in the proof table. Final TAA uses 512 postframes, others 192. Frame caps used: 1600/3000/4928. Proxy/detail target 77, UV target 0 tile, normal target 12 invert; static hold uses its unique copied material/texture. Launch extras: `-IAIBench`, `-ResY=1024`, and the observer-only flag below; full commands are retained per launch.
- Diagnostic levers executed earlier: `IAI.Bench.TexCorruptIdentity 1`, `IAI.Bench.StuckMipNoHold 1`, `IAI.Auto.Pool all 0`, `IAI.Label.TransitionOffFrames 16`, `IAI.Targets.AllowNanite 1` for the old MainWorld hold only; `-IAIResidencyProbe` only on the separate passive observer. Final legs use the production default 40, off-window CVar=-1. Runtime AA-switch and capture-boundary controls were not executed. Runtime settings ended with each owned process; owner sessions were untouched.
- Temporary `DefaultDeviceProfiles.ini` used Project02 LODBias 1 and was removed by receipt after each run. It is absent. All six staged originals re-hash to their pre-job values. All five final editor DLLs and the build-host EXE match `58386be`'s manifest; host Source is clean at `58386be`. The resume 2 backup adds five copies and no new binary hash. All original harnesses and original fixture assets re-hash unchanged.
- Hold0913 assets were newly duplicated and retained under `/Game/CaptureBenchTexCorrupt/Hold0913`. Original detail/scope/material/texture assets were not edited. Authoring/cook receipts and hashes are in `_r913/hold-fixture-*.json` and `cooked-container-hold-hashes.json`; cook outputs/staging/logs were isolated under `_r913`.
- Strict builds temporarily disabled unity/PCH, selected latest include order and IWYU, and generated independent header TUs; Build.cs and scratch restoration were verified. Passive observer source was restored by byte receipt and is absent from release commits. Only owned scratch/profile files were removed. All job engine processes exited; the required queue completed and restored its DLL baseline. No foreign PID, security setting, driver, TDR or owner's configuration changed.

Closing free space: D: 79.18 GiB, E: 319.87 GiB. Exact stage/host/original hashes and launch PIDs are in the restoration ledger. The [final process check](E:/IA_BuildCache/_r913/receipts/final-process-check.json) found no job engine/authoring/cook PID still running. Office reads were rechecked at close: unchanged 171 transition entries/off 16, no EFFECTIVE line supplied. No office captures, logs, frames or client files came to this PC.

Distinct console settings, with staged capture configurations shown as equivalent Config commands (full launch commands, paths, targets and PIDs remain in the ledger):

```text
CaptureBench.Marker 0
IAI.Anomaly.StuckMipRoute auto
IAI.Anomaly.StuckMipRoute hold
IAI.Auto.Pool all 0
IAI.Bench.StuckMipNoHold 1
IAI.Bench.TexCorruptIdentity 1
IAI.Capture.Config 0 192 48 192 3
IAI.Capture.Config 0 192 48 192 4
IAI.Capture.Config 0 192 48 192 8
IAI.Capture.Config 0 192 48 512 3
IAI.Capture.Config 0 192 48 512 8
IAI.Capture.Config 0 192 48 96 8
IAI.Capture.Delivery 0
IAI.Capture.FocusGate 0
IAI.Capture.Fps 30
IAI.Capture.Mask 1
IAI.Capture.Pace 1
IAI.Capture.RunLog 1
IAI.Capture.SVE 1
IAI.Capture.TargetMask 1
IAI.Label.TransitionOffFrames 16
IAI.Targets.AllowNanite 1
r.AntiAliasingMethod 0
r.AntiAliasingMethod 2
r.AntiAliasingMethod 4
r.DefaultFeature.AutoExposure 0
r.DynamicGlobalIlluminationMethod 0
r.EyeAdaptationQuality 0
r.Lumen.DiffuseIndirect.Allow 0
r.ReflectionMethod 0
```

Deviations and failed attempts are retained:

- Ruling1 resolved the AA-off/PIE-settle conflict. Ruling2 replaced strict-null acceptance with unchanged E1 t50 plus t10 coverage/margin. The provisional256 candidate was superseded by measured40 before final proof. Final TAA's37 exceeds the initial36 prediction by one but fits40; no final threshold/window change was needed.
- Initial null calibrations changed after clean-validation failures: median, identity null, pristine null, raw reference, then pairwise calibration. All variants remain; post-event pixels never defined the null. Moving-scene hold was unjudgeable and was replaced for the required proof by the authorized static Hold0913 fixture.
- The R4 source gate's old pre-freeze ordering check failed after capture-time AA moved transition decisions to completion. Its adaptation retained frozen-geometry requirements and added live-read mutants. The office kit needed route validation and a narrowly validated first-frame PIE bridge; invalid hold flags, missing later flags and nondecay still fail. No pixel threshold was changed.
- Two earlier post-exit DLL copies returned errno13; later ordinary copy/hash restoration succeeded, without a matching Defender block or security/ACL workaround. Resume2's first staged capture hit the same error restoring its EXE at+2s; no game/editor remained, and the same ordinary copy later restored all six originals. Subsequent staged delay is10s; PIE remains2s. The next and remaining restores succeeded.
- Resume2's shared reader initially had a missing default receipt; the explicit existing hashed `detail-author.json` fixed it with unchanged ROI/thresholds and no capture rerun. A status-only aggregator was corrected to count list-valued unexcused frames by length. The staging helper expects EXE SHA8; passing the source key failed before launch. The helper now receives the selected build's SHA8 alias, retaining full hash checks; the unlaunched attempt was restored and retained, and the actual staged TSR leg has the `_stage1` suffix.
- The m53 AA-off UV queue stopped on the obsolete all-flags-under-AA-off ban in084-09's UV reader. Ruling1 already requires independent PIE flags to remain. After preserving the failure and declaring the repair, the exact existing090-10e successor passed its13 controls and actual-pixel parity. The remaining AA-off UV/normal legs retain both raw084-09 and compatible results; all raw edges are0/0. The global PIE guard is checked as well. This is an explicit reader compatibility correction after a formal failure, not an unreported gate change.
- Earlier scratch readers/reporters had missing-import, encoding, result-shape and absent-matplotlib failures; retries or the absent plot are recorded. A waiter once read intermediate CAPTURED state; waiters now wait through it and receipts publish atomically. Windows GPU-query escaping was corrected in this job's copy. No original harness or pixel reader was edited.
- The earlier disk stop atD:38.9GiB was genuine. The reviewer attributed it to the owner's growing page file and authorized resumption after recovery; no system change or owner-data cleanup was made.
- Two prior final-source startup attempts, PIDs102896/106640, exited3 with DXGI_ERROR_DEVICE_REMOVED / Aftermath Timeout at the first Present after Engine Initialization, before capture/fire. Both had passed the old gate while an idle foreign UE5.7 editor was open. The reviewer ruled this environmental VRAM/device contention on the8GiB GPU, not a plugin defect. That is the reviewer's attribution. Resume2 prohibited every foreign UnrealEditor and the same binaries completed all16 legs without recurrence. No source, renderer, driver or security workaround was applied. Both failures and unchanged retry remain; neither counts as proof. The retry prediction's displayed21:03 timestamp was corrected by appended metadata to the prelaunch preparation time around21:00, before its21:02 launch.
- The required-first queue replaced earlier optional-first dependencies. Remaining UV/normal/staged/hold residency cause checks, runtime AA-switch and capture-boundary engine controls were deferred under resume1's explicit optional scope. Source/pure-test evidence is retained. No extra diagnostic launches followed the required matrix.
- Only three documentation files changed after the proved builds: CLAUDE status, gotchas and journal, merged fix -> m53 -> proxy. README schema/training instructions were already updated. Source identity to every proved build was checked before and after push. Two existing CVar help strings are the disclosed exception to the new-fields/log-line-only string expectation. All earlier build, restoration, calibration and reader failures remain in evidence.

**6. NEEDS-DECISION items**

None. Required items 1-3 and deferred 091-02 section 0 pass. The owner's pull is `feat/m52-proxy-blur` at `d660b5d6e67c7af8b0f264e59cf66ed02f433968`.

VERDICT: GREEN
