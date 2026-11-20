J1's required home PIE and staged gates and their can-fails pass at 0/0/0; native AA-off label/picture edges are zero frames off.
E41ABF93 and product source are unchanged; the corrected bench cook is archived with separate container hashes.
Only fix/followup-capture was pushed, at 1bf7054; the host and staging are restored. This is not a delivery.

**2. Heads, merge and defaults**

Worktree: `E:/IA_BuildCache/_r933/AnomalyInjector`. Pushed only `fix/followup-capture`, without force or tags, to **1bf7054f5b8664f057282ed863a62ab5dbbd60e3**. The remote branch was absent; it was created without rewriting history. Remote verification shows this was the only changed ref. The worktree is clean.

Every newly published J1 commit:

| Commit | Purpose |
|---|---|
| 72f3eb3e2df1dff5587716fcd5bc0a650ae8000c | Approved release/follow-up merge |
| 72edc488d99b018328474426ac96fdd1dd18565e | Original proof-stop and restoration docs |
| 1e6dc652d314f92a885c5a741b2b4039c5df1928 | Corrected F6 explanation and packaging-stop docs |
| ca2605f77fa6efb018169c70f2a57b2c662ba296 | Cook output-path diagnosis |
| 1bf7054f5b8664f057282ed863a62ab5dbbd60e3 | Completed PIE/staged qualification, status, architecture, journal and G478 |

Merge `72f3eb3` has release `d7eee97731812384d4fd00af9032254923363ada` and follow-up `5d6352b0f503f62d980b83ef0eb741af59426f84` as parents. Only the expected CLAUDE.md conflict occurred; both clearly dated release/follow-up histories were retained. Defaults are **0/0/0**; the existing explicit **3/40/1** override test passes. No Source, Shaders, tools, natural mechanism, F1 WIP or label key changed in either continuation. The pushed product tree equals tested `72f3eb3`.

`git diff 5d6352b HEAD -- Source Shaders tools` equals the release's `d660b5d..d7eee97` patch in every hunk and offset. Only two Git blob-index rows differ; normalization removes index metadata (four rows per patch). Both normalized SHA-256 values are **439b778cc84570a7fa02cfa52799ba3777e22758e2e1a436eeb2e7aa695d1e41**. A valid extra-Source-file patch is rejected by that comparison. Predictions remain byte-identical, SHA-256 **16c8e76b3b68cbea52bf707bb78a00ad8824eb559aadd33a076adb580e5922e6**.

G477 remains the accepted F6 correction. Before this commit, all 70 refs were rechecked for every numeric G reference; the maximum was G477, so the new lesson is G478. Comment stripping changed zero of 178 files. The reviewed documentation diff is four files, 195 insertions/27 deletions; diff checks pass. All other local branches/tags and remote refs are unchanged. [Push receipt](E:/IA_BuildCache/_r933/continuation09305/receipts/push.json), [source/diff receipt](E:/IA_BuildCache/_r933/continuation09305/receipts/precommit.json).

**3. Builds, hashes, cook and restoration**

The accepted normal and strict Editor/Game builds remain valid: all four passed with zero errors/warnings; each strict target compiled 69 independent header translation units. Build.cs was restored by hash. Product binaries were not rebuilt during the continuations. Fifteen C++ suites, 51 Python tests and 98 pixel-reader cases pass. Eighteen existing mutants, five integration/default/override/ownership mutants, and four additional alpha/scope mutants reject their planted defects. The explicit-window suite has 316 assertions. Lever verification covers source, EXE and five DLLs, 53 registrations and 12 audit test cases. String review covers 1,462 added printable sequences, two manually resolved boundary artifacts and zero unresolved additions. [Build results](E:/IA_BuildCache/_r933/strict/summary.json), [static results](E:/IA_BuildCache/_r933/static-summary.json).

All hashes below are SHA-256. Candidate binaries, unchanged throughout:

| Binary | Hash |
|---|---|
| StackOBot.exe | e41abf93bd6abc231deb7f93ace6594784ee74348b4db737cbdd465a0d3dd37f |
| UnrealEditor-AnomalyBench.dll | 2d014241ec9f65ced693d6836494ab6e85f173dc22fb24e6153263d99efaf5bc |
| UnrealEditor-AnomalyCapture.dll | e176b8e0b4ccb2efe5089f6929fbb830147bc61abe381df47bf9602640263425 |
| UnrealEditor-AnomalyControlServer.dll | 10c64f857e8c271d0f44c43e8c63beaac4ee67e2cde348f918f5ad0dd4195a70 |
| UnrealEditor-AnomalyInjector.dll | 5171ba2fbfd180ea8a7fe6618334b73655726a93db9b223f4c70f4844e35a35f |
| UnrealEditor-AnomalyShaders.dll | 589602b04bc775caeef7ccf8b1f21eee7cb39a91de95935ed57c5249e1d4f75e |

Cooked containers, recorded separately:

| Container | Hash |
|---|---|
| StackOBot-Windows.pak | d146d10bf649c19252338aca17a351fa8fdbe7cc45eb685aa6002cf786e6eaa6 |
| StackOBot-Windows.utoc | 5a39646c91334e40205dc853cc328b177d833d7f1f267af0d7152411b199dc75 |
| StackOBot-Windows.ucas | 1025998649c81de020351168f61529f0cb3b20a957e041f6588a37b134324bc1 |
| global.utoc | 508e61f0915875e1f2e858e05cd4bc902920af9eec6d012d664d8514553d4980 |
| global.ucas | f80a5da44a41f8f2b754cc8134a208d5c470eb1454539c5a8562b5171d12827b |

New qualified archive: **D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J1-09305-E41ABF93-5A39646C**, 624,819,375 bytes, **11/11 files rehashed**. It is the J2 guard-present/WIP-absent control, not a delivery. Its manifest identifies product source `72f3eb3`; its documentation-head field records `ca2605f`, the head when archived, before the final documentation-only commit. [Archive manifest](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J1-09305-E41ABF93-5A39646C/manifest.json).

Existing archive **D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J1-72f3eb3-E41ABF93** is preserved and rehashed 6/6. Its historical incorrect F6 explanation is superseded by G477, the new archive README and this report. Existing archives/index were not edited.

Shipped-release control archive **D:/IntrusiveAnomalies/_binary_baselines/release-2026-10-05-23098fa-218BA4C7**, rehashed 6/6:

| Binary | Hash |
|---|---|
| StackOBot.exe | 218ba4c79bb2bf6d9e14706772c04b3c53027cc05b84d4ab100a3349b650b7d6 |
| UnrealEditor-AnomalyBench.dll | 9f967665827cd1358b5f914466c36ecc45e47e5e972c749aa0221283bc92dfce |
| UnrealEditor-AnomalyCapture.dll | 470ebe5f6c5d8fa11d9872392035c62c2bab2480aee462a4d4d58ac59e8a5be3 |
| UnrealEditor-AnomalyControlServer.dll | 0c9c329bedf22be975a09042eb242a21cda726fcab63553240a46685b1dd8cf1 |
| UnrealEditor-AnomalyInjector.dll | 45a1512f0c7b59e321d8de97dd03e23b9bee175f503f974fb5a200f99b5a1b90 |
| UnrealEditor-AnomalyShaders.dll | 43d78d878cd1e8cd41b23db1fb747fc5c6bd6a103cb5089bb4fb984b1afc47db |

Engine/helper identities:

| Binary | Hash |
|---|---|
| UnrealEditor.exe | 5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b |
| UnrealEditor-Cmd.exe | 44f4aae265b37e62a7e167fff17cd55f8aa78030822cbd8425dc023737d81a48 |
| UnrealPak.exe | 1f88d51b0ad8c280f20e32d7cdaa584f7e8e000b93ce3eeb8689684272e3d73c |
| UnrealEditor-CaptureBench.dll | a7443041103affbcd06b724a1ee1cd66f30c636d7ba246d2b34e661b0e89fb51 |
| UnrealEditor-Review0916.dll | 297478d189e5a41a78369c06a12641102d2d063bc8aeea20484545d3975bfccb |

Cook receipt: **exit 0, 41.976 seconds**. BuildCookRun Development Win64 used `-cook -stage -pak -archive -skipbuild -nocompile -nocompileeditor -nop4 -utf8output -unattended`, `-AdditionalCookerOptions=-IAIBench`, and **CookOutputDir ending in `continuation09305/cooked/Windows`**. Twelve existing maps cover GateLevel, MainMenu/MainWorld, TexCorrupt, ProxyDetail/Scope, HoldDetail and the five Review0915 fixtures. Cook, package, stage and archive succeed; all thirteen required map names including Entry are present. An extra absent required-map control exits 1. E41ABF93 remains unchanged. No security block occurred. [Cook receipt](E:/IA_BuildCache/_r933/continuation09305/cook-receipt.json), [map controls](E:/IA_BuildCache/_r933/continuation09305/cook-maps-receipt.json).

Archive-byte can-fail: the earlier scratch EXE hashes **1495e3ce1179515abbb715c8181ab858b74e5dc027e58a167bb4a5152ba037a2**; the copied UTOC mutant hashes **047669aafd121d9e588502c2be2f3f0cc2edd36ebdf11dca365d91c0553d93af**. Both mismatches are detected; neither mutant was executed and original archives remain unchanged.

Restoration returned the host to detached **23098fa7415f148e935acbd4a97e18da04e6c46c**, clean: all **201** original host files match, all **1,327** original staging files match, and all **2,756** content files are unchanged. Exactly 24 job-created host files and 105 job-created stage files were removed by path/hash receipt. Generated cook outputs, caches and evidence remain. No owned engine process remains; no foreign PID or security setting was touched. Across 43 new launches, the longest idle wait was 118.85 seconds and minimum physical headroom 8.81 GiB. Final free space: D: **189,610,102,784 bytes**; E: **224,725,204,992 bytes**, both above 50 GiB. [Restoration](E:/IA_BuildCache/_r933/continuation09305/receipts/restored.json), [final closure](E:/IA_BuildCache/_r933/continuation09305/receipts/closure.json).

Every CPU-test/mutant EXE hash is listed below and in the [42-file manifest](E:/IA_BuildCache/_r933/continuation09305/test-binary-hashes.json). `W` means `E:/IA_BuildCache/_r933`. Original host binary hashes are also enumerated in the restoration receipt.

| Test executable, relative to W | SHA-256 |
|---|---|
| pure/active_source_selftest-mutant-1.exe | 84d580bb1bd909529b2854869564528301e4bdc6aabfaa2c8c750e04424e0cf4 |
| pure/active_source_selftest-mutant-2.exe | c5536ff4ef7fbcbc52d3a1d476e50bf3c565b305b5c1016d8502281abd4abc14 |
| pure/active_source_selftest-mutant-3.exe | 029da0f203965333b55e90ca680a01768233cdcd72489d7e5aad3eb1e2c7da53 |
| pure/active_source_selftest.exe | 58ac5f2b5d353905417108887202be7db6982084d1ec18816b011623724c3f31 |
| pure/camera_clipping_slab_selftest.exe | 34adbc238ca0329e35cf4ac87d24a4d9ab1b4ac035d0e77a8e08eac9fa0d70e6 |
| pure/capture_ownership_selftest.exe | af380a1d8723a62b70cb1c48a069c9b2b96877b83f0d179921c5a07dbd4d37d4 |
| pure/exclusion_selftest-mutant-1.exe | 59804e254c43bf83e2ebbe785b785c98e94e59b80025cebe9db75b7bdf46a5f8 |
| pure/exclusion_selftest-mutant-10.exe | ece49d82dc6cfb6511631de09b96ef4f112514151c53652b3487b1b04750c372 |
| pure/exclusion_selftest-mutant-11.exe | 11c326eef9295d6ceeb60bdc26ffcbdceda94b6afaecbe414fcb53964372037e |
| pure/exclusion_selftest-mutant-2.exe | f597db2228c5a33f7da807295cd224692e6052ea80da188ad9423d6ae35b1c69 |
| pure/exclusion_selftest-mutant-3.exe | 02237434d0cda0c51dfc4580864e522c48fc85238f7f4661dfc2b5ed4a684f9d |
| pure/exclusion_selftest-mutant-4.exe | 334529f3bf22b0bc3a671c8446430b088d6730579485932c243a5cf5c4629baa |
| pure/exclusion_selftest-mutant-5.exe | 90d24414a78e2587e492cf8491504a1c8a7a240d9a19f72e90dbb90b25be9d17 |
| pure/exclusion_selftest-mutant-6.exe | 6336979d0e7080918de7f9516388644104209b03fff4653360a0ed58b03670b4 |
| pure/exclusion_selftest-mutant-7.exe | 625e5fff8b0ec9f8c6dec760ac69ce88ab0d0eb03f724087dd004ad1a96aea6d |
| pure/exclusion_selftest-mutant-8.exe | f244820d38837a368bbf50b2f0209070386be9273e67b12ad6d98f01816bfe9b |
| pure/exclusion_selftest-mutant-9.exe | 9a0c2e181fe865fbe82b07b24df45c6e76c5802462c0d40f056842019be11201 |
| pure/exclusion_selftest.exe | 46babaca745cd6a6454a20e7da8d4370cdd7143dcfb8d2972a7372a144a28669 |
| pure/install_state_selftest.exe | 2134720544fa6e8158d81ae8e9fd689b7dd1db29e4984187cb7057016822dc38 |
| pure/m52_window_selftest.exe | cd3d1b0937dc6f52f91d7f870f7726f9f1e1c4b8bfcf45b0ec6607042ecec2c9 |
| pure/mask_serve_selftest.exe | 3b2caaceae0fdc934d7d1d8ec1c343b0ffc500d8e4c1b13f63942d87d4ffa35b |
| pure/pie_end_settle_selftest.exe | 649935417847e05e8658b4ff5000e5bfb884ff2ce909b68f5fedd7abc06252ff |
| pure/proxy_blur_selftest.exe | 0a0d5f10639bc44f5a476f083c54cea92e254397cf678c6e33fdbbffd4009ffa |
| pure/r4_capture_pairing_selftest.exe | deb153e6118ee9238b15955a72f859011cdcf12135408919bf71f4f9f16733b5 |
| pure/stencil_scope_selftest.exe | 851ed65dfa897cfcfc4fa2d23a0af0ac4be8721c20a8805c2ec77590bf2db24f |
| pure/target_policy_selftest.exe | 69e280a3e140b10aa0eb1d1797a36dfcc478885e583aa100e271eb08f9ce0d4d |
| pure/texcorrupt_pure_test.exe | 0fc100ba9c5bdc6a089904a65b7bfce3830846d73119df7ae39bbcbc18361a49 |
| pure/texcorrupt_readiness_selftest-mutant-1.exe | 61aadf3ea2b7c8206be5787b7a2a9aab5ba21f78d12efda5557682ffe5cc2dca |
| pure/texcorrupt_readiness_selftest-mutant-2.exe | 918fcc381d3362062f923b45d1992837cdf9db3e67e9011ba3073cb5902e26a4 |
| pure/texcorrupt_readiness_selftest-mutant-3.exe | f9491a3ddabf0147e9cee4fdb18d78b3a0fe88dd415768706906defbd0f0f96d |
| pure/texcorrupt_readiness_selftest-mutant-4.exe | ab77c99caf98d0f42f059d40d7b3c0b0ce9a1c109f1dfd551998198660479b59 |
| pure/texcorrupt_readiness_selftest.exe | c9a30547494b28a514ac717e0d8efc0dbfb6a2e47a64e08b5d3c49928b85f5e3 |
| pure/texcorrupt_uniform_selftest.exe | bfd44f0d62f9d8ef403dce18a3d183590e30d1b3f15db2ece38e222f573cc7f3 |
| pure-gate-controls/family_frame_alias/test.exe | 61332cb21179a652c4a9ef320463ef37664b2d61a88175dd65bab94a96515173 |
| pure-gate-controls/ignore_explicit_override/test.exe | cb598101fd76f8fcec0f90d52860ca5cf3aab5247fb134ed372b32df8d3a1842 |
| pure-gate-controls/old_defaults/test.exe | a021c0835e8b7a26c9600069153e91d77621c59af53f02307ca70ccaed8e93db |
| pure-gate-controls/owner_scene_only/test.exe | b078c0673c517771d82750577ea2e3a60d486c675b8a19822e82ef75437b2e1b |
| pure-gate-controls/owner_stale_sample/test.exe | 53bbf528d58bb2bcdfc83402332a24691e2d3a47056afb972ccac1aa48f56ada |
| continuation09305/pure-controls/alpha_always/test.exe | 9d600594e0a006037c1b9b06c53b1122b1a787c860c6aef9a01af5adffcc7fc8 |
| continuation09305/pure-controls/alpha_never/test.exe | c5a808f07e0ba1c4e8651b3c24a3943f00fea224812c78070e2371d6ec2b8985 |
| continuation09305/pure-controls/scope_no_retirement/test.exe | 0a9af9408ca0041d37c99f1bfc667fa143c84e59c0febb736321d484f6afd5c2 |
| continuation09305/pure-controls/scope_unowned_restore/test.exe | 28c21109a2c5db470e20a24f5045fc414e57f5dab3465eab4f49c4017682082d |

**4. Predictions, results and can-fails**

The [predictions file](E:/IA_BuildCache/_r933/AnomalyInjector/docs/predictions/093-natural-foundation-miss.md) is unchanged; ruling 093-03 supplies the accepted F6 correction. Native-order AA-off is the zero-frame arbiter. Positive cases require at least three visibly changed fires; negative alpha/null cases require zero. Every positive labelled image has an actual same-size nonzero mask with matching count/bounds and event membership. The receipt audit has 196 passing assertions; raw evidence is indexed in [qualification.json](E:/IA_BuildCache/_r933/continuation09305/qualification.json) and [outcome-index.json](E:/IA_BuildCache/_r933/continuation09305/outcome-index.json).

| Gate / prediction | PIE result | PIE can-fail | Staged result | Staged can-fail |
|---|---|---|---|---|
| F1: correct game view, UV/shared proxy | Each: 1,152 pictures, four visible fires, 192 measured labelled masks; every game marker present under two forced editor-world views | 218BA4C7 loses all 1,152 markers and 192 labelled masks in both modes; erased-marker and pure wrong-owner/family controls reject | UV four/192; proxy three/144; every game marker present; zero native edges | Erased-marker copy rejects; pure same-scene/wrong-target/family controls reject |
| F2: C1-only mask/boxes, unchanged C2, tag retirement and restore | UV and normal: four fires each; C1 mask 50,176 pixels, C2 zero; raw C2 retirement and exact restore | Release includes 50,176 C2 pixels per labelled frame; 20-level C2 change, stale/unowned CPU mutants and copied after-state stencil reject | UV and normal: four each; C1 65,536, C2 zero; actual timed console probes show C2 retired and exact material/depth/stencil restore | NoPartScope includes 65,536 C2 pixels; 20-level C2 change and copied after-state stencil reject |
| F3: opaque zero effects; masked positive | Opaque zero events/four refusals, maximum mean 0.3363; masked four visible/192 measured | Release labels four invisible opaque events; always/never-alpha mutants reject opposite halves | Opaque zero/four refusals, max mean 0.3351; masked four/192 | Release labels four invisible opaque events; same alpha-policy mutants reject |
| F6: annotation membership, independent of row self-consistency | Intact old/new hooks and CLI pass on existing capture | Consistent wrong event: old hook misses, new hook/CLI reject; event-ID-only rejected by both | Same on new staged capture | Same corrected control and secondary self-consistency control reject as predicted |
| Dropped masks: exactly three clean/active drops, reconciled totals | 1,149 rows each; clean missing 60-62, active 201-203; 192/189 measured labelled masks. Batched engine helper independently drops 47-49 | Summary total +3 and dropped-counter zero reject; stale-mask pure control rejects | 1,149 rows each; clean missing 52-54, active 192-194; 192/189 measured | Both corrupt-summary controls reject; no orphan image/row/annotation/mask outcomes |
| m52 hold: visible blur, zero edges and recovery | Four fires/220 measured masks; strict/t10/t50 edges all 0 | NoHold: 1,100 frames, zero events/held rows; frozen target rejects | Four/196 measured; all three E1 edge readings 0 | Same zero-effect null and frozen-target rejection |
| Shared proxy: blur, unchanged peers, measured masks and recovery | Four/192; all 20 measured peers within frozen mean envelope; exact raw restore | Frozen target and 20-level shifted peer reject | Three/144 in Marker=0 regression; all 20 full peer footprints pass, masks/recovery pass | Frozen target and shifted peer reject; original marker-overlap failure retained separately |
| UV/normal and co-user regression: visible correct object, recovery | Four/192 per configuration; zero edges; named shared-material peer unchanged | Frozen target and shifted peer reject | Same four/192 and zero edges; named peer unchanged | Same controls reject |
| Label pixels/masks: 0 frames off, actual matching masks | All qualifying native positives pass | Onset/end +/-1, missing/blank/wrong-count mask, physical one-pixel count mutation and erased marker reject | Same | Same capture-specific controls reject |
| Synthetic order: identity/membership and image/mask joins | Pass | Wrong-ID and accounting controls test the same readers; synthetic pixels are not timing evidence | Pass | Same reader can-fails; no synthetic pixel certification claimed |

Corrected F6 in **both modes** uses unflagged row 191, changing start_frame to 999999, target_name to WrongTarget and event_id consistently to `uv_corruption@999999|WrongTarget`. Old hook: `[]`. New hook: `labels event missing from annotation: uv_corruption@999999|WrongTarget`; new CLI exit 1. Intact CLI exits 0. The event-ID-only mutant gives `labels event_id mismatch at 191` in both versions and new CLI exit 1. Original capture hashes remain unchanged. **No transition-free capture is needed.** [Both F6 results](E:/IA_BuildCache/_r933/continuation09305/f6-summary.json).

Reader wording is unchanged and exact: **verify_capture.py --label-pixel-gate exits 0 and reports four UNASSESSABLE events on each wrong-view release control. Independent marker and measured-mask checks reject it.** Shifted-label controls likewise produce exit 0/offset notes; the strict zero-edge predicate rejects them. Invisible alpha-release events fail the independent picture check. The reader was not modified. Positive labelled masks are measured; the reader's clean-baseline extrapolation is supplemented by fixed-bbox readings. A copied target mask is supplied only on an added boundary row in a synthetic negative shift control, never as positive measured evidence.

TSR at 0/0/0 is separately diagnostic, as predicted. All measured starts are 0. Recovery tails: PIE proxy strict 42-44/t10 27-28/t50 8-9; PIE hold 42-44/27/8-9; staged proxy 38-39/23/7-8; staged hold 37-38/23-24/7 frames. Proxy/co-user mean and mask checks pass under TSR. These tails are the retained release limitation; **no temporal zero-frame claim** is made.

**5. Differences, settings and harness repairs**

Capture-size cause for **1123x870 PNGs versus 1280x992 viewport metadata from a 1280x1024 request: unknown**. This is not a gate.

The 091-05 archives were deleted on 2026-10-10; the mandated shipped **218BA4C7** control and its five DLLs replace C7FB5212. Defaults are 0/0/0 rather than 3/40/1. The retained independent game barcode replaces the deleted green-plane harness. Four predeclared attempts retain the three-visible-fire minimum; staged proxy yields three, PIE four. Hold has 220 PIE/196 staged measured labelled rows with zero actual picture-edge offsets. Staged raw scope restoration uses actual console property readings, not copied PIE state. Broad built-in collateral summaries are not substituted for measured named peers.

The retained Review0916 helper supplies editor-world draws/skip infrastructure only; LateStart was never invoked and product F1 WIP is absent. Some fixture logs contain handled distance-field NaN ensures/material-usage diagnostics; no crash or target gate failure resulted, and cause is not established. Office targeted/auto-pool behavior, shader-readiness directions, office-like stressors and the <100 ms/15,000-actor census requirement are **not qualified by this home job**. No office data or delivery claim is made.

All temporary settings were process-local or restored by receipt: `-IAIBench`, PIE `-NoLiveCoding`, unattended/nosplash; PIE 1280x1024 request, staged 1280x720; PNG, 30 fps, seed777, cap1400 or null1100; Capture SVE=1, Delivery=0, Pace=1, Mask=1, TargetMask=1, RunLog=1, FocusGate=0; Config `0 192 48 192 4`. AA=0 for certification or 4 for TSR diagnostics; auto exposure, eye adaptation, Lumen diffuse, dynamic GI and reflections=0. Transition CVars remain -1, resolving to 0/0/0. Marker=1 only for ownership, otherwise 0. StuckMipRoute auto/hold, StuckMipNoHold=1 for nulls, TexCorruptNoPartScope=1 for its control; Review916 PIE multi/Observe/Report/Skip, SynthTickOrder=1 for alignment. DEFER commands disable/restore world rendering over three ticks; GETALL probes read state at 0/210/1160, with log-frame wrap accounted for. PIE ownership uses the retained editor views at (-1200,2500,1000) and (-1200,3500,1000), rotation (-20,-90,0), with duplicated 1280x1024 play settings. All exact commands, launch PIDs, idle readings and binary hashes are in each leg's launch.json.

The temporary Windows/WindowsEditor Project02 profile used LODBias=1, MinLODSize=1, MaxLODSize=16384, aniso/point filtering and SimpleAverage mip generation, SHA-256 **aca6bdf45b81b6c7759654304f85c1f1f6154375581aa046656cf8560a47a76e**. It was removed by receipt. Candidate/release binary swaps and the five live-stage containers were restored; process-local settings ended with their owned processes.

Every 093-05 harness problem and fix follows, with original failures retained. No gate, threshold or prediction changed; no problem exceeded two fix attempts. [Repair ledger](E:/IA_BuildCache/_r933/continuation09305/harness-fixes.json).

1. **Shift-control construction — two fixes.** First corrected the unused annotation list and wrong offset field: update both affected/injected frame lists and read `end`. Second supplied a copied fixed-view mask/entry on the added synthetic boundary row so the edge was assessable. Original RGB stayed unchanged; all four +/-1 controls now reject. v0/v2 failures remain.
2. **Cook path — one fix.** The 093-04 path omitted Windows. Adding that suffix produces the complete, verified container; the prior failed cook is retained.
3. **Integrity walker — one fix.** Replaced the wrong dictionary variable when visiting a list, removing UnboundLocalError without changing checks.
4. **Deferred skip scheduling — one fix.** Separate callbacks disabled rendering for five ticks; the three-drop gate correctly failed. Batched scheduling produces exactly three, with both accounting can-fails still rejecting. The five-drop capture remains.
5. **C2/co-user predicate scope — one fix.** The checker incorrectly applied the C2-only zero-pixels-above-12 rule to every co-user. Restored the frozen scope while retaining mean-envelope +0.05 and the 20-level can-fail. The original proxy-TSR failure/code remains; its means passed, and corrected evaluation/control pass. No threshold was retuned.
6. **Extra-source comparator control — one fix.** Replaced an invalid appended diff marker with a valid additional Source-file patch. `git apply --numstat` accepts the mutant; equality rejects it. Both versions remain.
7. **Staged barcode overlap — one fix.** All 1,398 changed pixels above 12 in each worst co-user frame occupy the barcode rectangle. Kept the original ownership proof and failed collateral reading; a separate Marker=0 capture passes all 20 full original footprints and both image can-fails. No ROI exclusion or threshold adjustment.
8. **Component-log parser — two fixes.** First accommodated actual property formatting but still expected command echoes that Development omitted. Second reads the three ordered six-property groups and requires actual offsets 0/210/1160 modulo the log counter. Positive UV/normal and the NoPartScope control pass their predictions; an altered after-state stencil is rejected. Both failed parser receipts remain.

Earlier disclosed 093-03 repairs remain: MSVC quoting/PATH/include fixes; `/wd4702` only for deliberate unreachable-code mutants; replacing the job's own slow scanner PID31828 with equivalent indexed lookup; separating ASCII/UTF-16 negative-control buffers and rejecting the mistaken relocation hypothesis; valid GPU-counter status handling. The corrected F6 diagnosis is the accepted ruling change, not a transition-related explanation.

Report: [report.md](D:/IntrusiveAnomalies/_relay/runs/2026-10-13-093-03-j1-natural-base/report.md). Prior reports are preserved as report-093-03.md and report-093-04.md. [Current journal](E:/IA_BuildCache/_r933/AnomalyInjector/docs/sessions/2026-10-14-093-05-followup-capture-qualification.md).

**6. NEEDS-DECISION items**

None. J1 is qualified for the stated home integration scope. The independent reviewer rules on this evidence; the owner controls delivery. J2 and subsequent natural work are not performed here.

VERDICT: GREEN
