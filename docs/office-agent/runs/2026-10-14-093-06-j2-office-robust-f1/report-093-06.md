J2 is implemented but unqualified: Lyra selected-viewport PIE exposed a label/picture disagreement.  
Part 0 and 22 planned engine cases passed; one failed and 61 were not run after the D2 stop.  
Nothing was pushed; both hosts are restored, staging is unchanged, and the candidate is archived as UNQUALIFIED.

**2. Part 0.** Completed and committed before J2 changes in `59b2fa19aa82f6c0ebbf2db1f223ba849b796c9f`.

| Wrong-object copy check | PIE | Staged |
|---|---:|---:|
| Original images / events / measured labelled masks | 1152 / 4 / 192 | 1152 / 4 / 192 |
| Original visibility / integrity exits | 0 / 0 | 0 / 0 |
| Mutant visibility / integrity exits | 1 / 1 | 1 / 1 |
| Claimed-target median RGB difference; threshold | 0.333440; 4 | 0.336060; 4 |
| Minimum true-target RGB difference | 42.920601 | 39.354332 |
| Original/donor files changed | 0 | 0 |

The first 48-frame UV event was consistently relabelled from `StaticMeshActor_0` to `StaticMeshActor_12`, using that other object's measured mask from the matching J1 normal capture. RGB stayed byte-identical; mask overlap was zero on all 48 frames. Identity and mask structure passed, while both unchanged visibility checks rejected the claimed target. The observational label-pixel reader's 0/UNASSESSABLE result was not treated as visibility acceptance. Evidence: [Part 0](E:/IA_BuildCache/_r936/part0-r1/result.json).

The blanket status claim that every J1 can-fail ran was corrected. G479 and the readme limitations now record that event-state silhouette masks omit pixels cut away by corrupted alpha, although edges stay zero frames off. The regular-line future item is the union of pre-event and event silhouettes.

Both staged proxy runs produced three events because attempt ordinal 0 was refused as `texture_uniform:pending` (`F1_S_proxy/game.log:4936`, `Proxy_S_aaoff_marker0/game.log:4933`).

**3. J2 as built.** Used `E:/IA_BuildCache/_r933/AnomalyInjector` on `fix/followup-capture`, starting at pushed `1bf7054`. Applied the saved nine-file patch and untracked family header cleanly. Patch SHA-256: `E5A64F5F85ACB9982CE5833A197C4A8509FDB612AF9556783A17111477278507`.

Activation only controls participation. Game-thread resolution requires one capture-world local player and a real display target. Requests freeze epoch, generation, scene, target and local-player/view identity; family ID and view index bind before dequeue and are checked again on the render thread and at publication. Changes retire pending requests and baselines. Submitted readbacks remain alive until GPU completion, then rejected results are discarded. Unknown targets defer: no fallback or first-wins selection is enabled. Split, stereo and same-world display ambiguity are refused and counted.

Product source is confined to `Source/AnomalyCapture`: `AnomalyCaptureOwnership.h`, new `AnomalyCaptureOwnerState.h` and `AnomalyCaptureFamily.h`; `AnomalyCaptureSubsystem.cpp` and its public header; `AnomalySveCapturer.h/.cpp`; `AnomalySceneViewExtension.cpp`; `AnomalyMaskSceneViewExtension.h/.cpp`; `AnomalyCensus.h/.cpp`; `AnomalyChangeFamilyData.h/.cpp`; `AnomalyChangeStage.h/.cpp`; `AnomalyFrameCapturer.h`; `AnomalyMaskTypes.h`; and `AnomalyLabelWriter.h/.cpp`.

Tools: `capture_ownership_selftest.cpp`, `office_checkpoint_i.py` and its documentation. Repository status, architecture, readme limitations, gotchas, predictions, runbook and [journal](E:/IA_BuildCache/_r933/AnomalyInjector/docs/sessions/2026-10-14-093-06-j2-capture-ownership.md) are updated. No natural-cause code, shader changes, label-schema additions, release/master merge or delivery branch change. All 6,982 written label rows were scanned: zero ownership-diagnostic keys.

**4. Gates and can-fails.** [Predictions](E:/IA_BuildCache/_r933/AnomalyInjector/docs/predictions/093-06-j2-capture-ownership.md) preceded the legs. `r/w` means requested/written; `v` visible events; `e` maximum measured edge error. NR means not run after D2. The full [84-case ledger](E:/IA_BuildCache/_r936/qualification.json) records 22 PASS, one FAIL and 61 NR. One supplemental R3 StackOBot healthy case also passed. There were 26 launches including two retained harness failures.

| Gate | StackOBot PIE | Lyra PIE | StackOBot staged | Lyra staged | Can-fail |
|---|---|---|---|---|---|
| Delayed viewport | PASS: 360/360, v4, e0; generation update 1 | PASS: same | NR | NR | Exact J1 DLLs drop 360/360 on StackOBot; Lyra control NR |
| New window + different realtime editor picture | PASS: wrong view 0; 2477 non-owner rejections | PASS: wrong view 0; 2508 rejections | NR | NR | Exact release DLLs capture 360 wrong-view pictures on StackOBot; Lyra control NR |
| Selected viewport | PASS: 360/360, v4, e0 | **FAIL: 360/360, v0, zero masks, 96 positive rows** | Editor-only | Editor-only | Wrong-world/target/family CPU mutants reject; observed failure retained |
| Two-client PIE | PASS: both 360/360, worlds 2/3, wrong view 0 | NR | Editor-only | Editor-only | Actual other-client PNG copy rejected |
| Resize | PASS: 360/358; 2 drops; generation +1; stale count 5; post-change PNGs 960x720 | NR | NR | NR | Generation/binding mutants reject; no stale-size image accepted |
| Two same-world displays | PASS: 360/0; ambiguity 2163; accepted ambiguous 0 | NR | NR | NR | First-wins mutant rejects |
| Split / stereo refusal | PASS: 360/0 each; counts 1414 / 728 | NR | NR | NR | Multi-view/stereo mutants reject; stereo lever is not HMD qualification |
| NotGameView controller / game-thread / show flag | PASS: 360/0 each; counts 1067 / 1069 / 1067 | PASS: 360/0 each; counts 1072 / 1067 / 1067 | NR | NR | Each flag forced independently; healthy paths pass both fixtures |
| UV / proxy native regression | PASS: each 1152/1152, v4, e0, 192 measured labelled masks | NR | NR | NR | Ten copied-capture controls per leg; originals unchanged |
| Synthetic alignment; pairing native/synthetic at 100%/50% | NR | NR | NR | NR | Recipes/control readers prepared, no engine claim |
| Office reader | Home PASS/FAIL and three additional controls pass | No office qualification | N/A | N/A | Wrong-client image, missing telemetry and shifted onset reject |

The regression controls cover onset/offset ±1, missing/blank/wrong-count masks, erased marker, corrupt mask summary and a physical one-pixel mask deletion. Part 0 supplies the wrong-object can-fail. No threshold was relaxed.

Normal and strict Editor/Game builds pass on both fixtures with zero errors/warnings. The standalone header passes compiled 71 TUs per target, unchanged at the final product head. All 69 CPU commands returned expected exits, including 18 deliberate mutant failures; the family header additionally passed 20 adapter checks with eight failing mutants. Python: 51 tests. Source lever audit: 53 registrations and 12 self-tests. Binary lever audits: both fixtures, Editor/Game. R3 string scan: 1622 additions fully attributed, including 12 manually verified source/PE records; zero unresolved. ASCII/UTF-16 scanner can-fails and the old-J1 compiled-marker control reject. Comment strip changed zero files; diff checks pass.

**Stop evidence:** In [Lyra's selected-viewport session](E:/IA_BuildCache/_r936/legs/lyra_P_selected/capture/session_20261014-095057), the actual target projects entirely outside the 942×869 picture at X −79.365 to −21.432. Row 35 claims X 23.550 with width 47.100. Ninety-six rows say `labelled=true`, `visible_positive=true`, `target_pixels=0`, `observable=false`, and `mask_state=empty`; no target masks were written. World/frame barcodes match all 360 pictures. Visibility and mask-integrity gates fail; edge error is unavailable, not zero.

Lyra's original configuration uses `AspectRatio_MaintainYFOV`; the unchanged product `AnomalyViewport.cpp:716` stores camera FOV as horizontal and reconstructs the frustum from viewport aspect. This is the likely cause, not a completed causal fix. No reframing or product repair was used to hide the result. [Failure readings and hashes](E:/IA_BuildCache/_r936/stop-review.json).

**5. Example ownership block.** This actual block is from the failed selected-viewport case. Ownership counters alone do not certify label visibility.

```json
{
  "capture_ownership": {
    "version": 1,
    "counter_scope": "rejections count request/family checks or resolution changes; served counts colour requests and all mask arms",
    "run_epoch": 1,
    "owner_generation": 1,
    "owner_scene_known_start": 1,
    "owner_target_known_start": 1,
    "owner_updates": 0,
    "world_type": 3,
    "net_mode": 0,
    "requested": 360,
    "sampled": 360,
    "written": 360,
    "colour_served": 360,
    "mask_served": 385,
    "context_world_null": 0,
    "target_fallback_accepts": 0,
    "accepted_ambiguous": 0,
    "unpaired": 0,
    "dropped": 0,
    "stale_generation": 0,
    "pending_at_stop": 0,
    "rejections": {
      "owner_scene_unknown": 0,
      "owner_target_unknown": 0,
      "scene_mismatch": 3,
      "target_mismatch": 0,
      "not_game_view": 0,
      "not_game_view_show_flag": 0,
      "not_game_view_game_thread": 0,
      "not_game_view_controller_id": 0,
      "scene_capture_or_reflection": 0,
      "owner_player_unknown": 0,
      "ambiguous_owner": 0,
      "multiple_views": 0,
      "stereo_view": 0,
      "local_player_view_mismatch": 0,
      "stale_generation": 0,
      "missing_family_binding": 0,
      "owner_render_missing": 0
    }
  }
}
```

**6. Office-reader self-test lines.** These are home tests using host code 1; no office data was used. Host 1 means the first office host, host 2 the second.

```text
I1.11 PASS r=360 w=360 v=4 e=0 x=0 a=0 #0
I1.11 FAIL r=360 w=0 v=-1 e=-1 x=-1 a=0 #2 I-fail=360,0,0,0,0,3,0,1067,0,0,360,0,0,0,0,1067
```

Reader SHA-256: `7380e31bd272f7819c0d56d4a172a41d7e16d25a2c429b7495c891f51be26579`. [Self-test receipts](E:/IA_BuildCache/_r936/office-selftest.json).

**7. Heads, hashes, archive and restoration.** **Pushed commits: none.** Remote `fix/followup-capture` remains `1bf7054f5b8664f057282ed863a62ab5dbbd60e3`. Local head is `bb9e7419caec7c2acc8c5334f36abe641000bf94`; the worktree is clean. Source/Shaders/tools are unchanged since `f608d56248e4c2ab11515b1fdd8f969cae4ead74`. Protected branch/tag refs are unchanged.

Local commits, all on `fix/followup-capture`:

- `59b2fa19aa82f6c0ebbf2db1f223ba849b796c9f` — docs(capture): close J1 wrong-object proof and mask caveats
- `e13ddc528175b7ea9266407d79f2b6965be0fac9` — fix(capture): bind requests to resolved owner generations
- `744b36d3b6dab49f75dd1d99c261100f7e3d807b` — fix(capture): drain invalidated GPU readbacks safely
- `33106f5e270c00874a2729df8e4bc6114cb14e93` — fix(capture): preserve owner identity across PIE reopen
- `f608d56248e4c2ab11515b1fdd8f969cae4ead74` — fix(capture): verify colour and mask binding before publication
- `bb9e7419caec7c2acc8c5334f36abe641000bf94` — docs(capture): record J2 selected-viewport qualification stop

Archive: [followup-capture-J2-09306-f608d56-4BB0F074-B0F2B30C-UNQUALIFIED](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-09306-f608d56-4BB0F074-B0F2B30C-UNQUALIFIED). **UNQUALIFIED**, 470/470 files re-hashed; copied one-byte DLL mutant rejected. Manifest SHA-256: `507e2aef1c8f76b746280792365bbeeb26cf062a0633932a8925980d8a3d13e9`. Raw captures remain under `E:/IA_BuildCache/_r936/legs`; the archive includes the failed case's labels, readings and sample PNG. Complete archived EXE/DLL/PDB/container hashes are in its [manifest](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-09306-f608d56-4BB0F074-B0F2B30C-UNQUALIFIED/manifest.json); staged support-file and restored-original hashes are in the [staging receipt](E:/IA_BuildCache/_r936/receipts/stage-stack.json) and [initial host receipt](E:/IA_BuildCache/_r936/receipts/initial.json).

The UE candidate EXEs below were built and banked; none reached staged qualification. R1/R2/R3 change only the external helper between freezes. Product editor DLLs are identical across those versions. All following hashes are SHA-256.

| Candidate EXE | StackOBot.exe | LyraGame.exe |
|---|---|---|
| R1 | `53184e08e420876c964f3c60e455b0445a4f56ad0b444ede2822bdc9c6dd6b31` | `19feae58712ae66fb0c035463037815bd9b63753937743dea0a526b82c0177ba` |
| R2 | `3cebae221d8a8cd9bc9a5f7c4c67858e3f1b25209a0413c74cad5b3761d66066` | `2f958040bf2fa731e62515dd07dab9eddf3e1511a684222cc33ecd48a47a4a4d` |
| R3 | `4bb0f0749ccacc9f4666d5186b03d7a7f5a9b5763445549e41438dc8cc9fcc27` | `b0f2b30c577db6863b3622e81d622b0072ac29fa822171266c15f6933e4f2336` |

| Product editor DLL | StackOBot | Lyra |
|---|---|---|
| UnrealEditor-AnomalyBench.dll | `56cafc40207e4e7b4f1a3e02948df02d8ca8965a403f0bd2326ff9cce30319d5` | `14fbc492b742173fc7da0df6815f765c92a97be236f246a1975c1f0840557132` |
| UnrealEditor-AnomalyCapture.dll | `d45ce046a55747b9ed48340c8052db69973c554c8ca468f56cbe288a7bf5332a` | `9f0155f89528f6d20f070fe591b2019bdc54f7ffaecf17a99a0a6f95210b04aa` |
| UnrealEditor-AnomalyControlServer.dll | `581ade2f1acf76f2f6ad2c5fb6a9a05cb603f6a12ea45f0c4df9f7639d10042f` | `42c93430c786e775f293750eec50ec2ac8f924669a145d79e3e035ad8b335ca2` |
| UnrealEditor-AnomalyInjector.dll | `d380287d1e018276d1c9b07c6beb08c217dd24cb5078d990f52100ce39f075e7` | `9f48c8bb1ce0fd8521e42671066df06f1ea2d5be1e5b88676db782b3307e6751` |
| UnrealEditor-AnomalyShaders.dll | `cb18bb2aca5cded46c05f691bdcca9f058fb63e6933dfe26bf8025ae10f4e126` | `7804202021e708d80234762a4d3cb9acecf83eb449ee32a46291be6ee45e002d` |

| Helper DLL | StackOBot | Lyra |
|---|---|---|
| Review0936, all versions | `de96239b22af546aabf7e579ab0be60bb05efb10ec02a45ffbd3d7c9001e7e0d` | `fb565adab1aa684d984c8a54e1b047cd2d0b2394ddb996d47677c3081661b68b` |
| Review0936Runtime R1 | `9edf8932bc479f2f8fdf43f076c0adb44ea24d3c2720b34ba05623b913952832` | `d3fe657816e6e2c42deedda3dc699d9c1b07d95920d808b830c869f3a2446b1d` |
| Review0936Runtime R2 | `cc1feb214ee97892ae70c16a04de373fa6d82d6e22e8c9ed00585d828df7596e` | `162cfafe8db03d22078228e0149b27d7c142d85f86ed10b473a62c1c195aa4f7` |
| Review0936Runtime R3 | `e75740cce4ebb5b0f077b96d1cb344ddf10f76ad68336e74df0fc089c41c585f` | `e055e304e071d2fb11299d0289e28bad88f75658554f7430e07bb4222ccc908d` |

Shared launched `UnrealEditor.exe`: `5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b`.

| Exact archived control file | J1 | Release |
|---|---|---|
| StackOBot.exe | `e41abf93bd6abc231deb7f93ace6594784ee74348b4db737cbdd465a0d3dd37f` | `218ba4c79bb2bf6d9e14706772c04b3c53027cc05b84d4ab100a3349b650b7d6` |
| editor/UnrealEditor-AnomalyBench.dll | `2d014241ec9f65ced693d6836494ab6e85f173dc22fb24e6153263d99efaf5bc` | `9f967665827cd1358b5f914466c36ecc45e47e5e972c749aa0221283bc92dfce` |
| editor/UnrealEditor-AnomalyCapture.dll | `e176b8e0b4ccb2efe5089f6929fbb830147bc61abe381df47bf9602640263425` | `470ebe5f6c5d8fa11d9872392035c62c2bab2480aee462a4d4d58ac59e8a5be3` |
| editor/UnrealEditor-AnomalyControlServer.dll | `10c64f857e8c271d0f44c43e8c63beaac4ee67e2cde348f918f5ad0dd4195a70` | `0c9c329bedf22be975a09042eb242a21cda726fcab63553240a46685b1dd8cf1` |
| editor/UnrealEditor-AnomalyInjector.dll | `5171ba2fbfd180ea8a7fe6618334b73655726a93db9b223f4c70f4844e35a35f` | `45a1512f0c7b59e321d8de97dd03e23b9bee175f503f974fb5a200f99b5a1b90` |
| editor/UnrealEditor-AnomalyShaders.dll | `589602b04bc775caeef7ccf8b1f21eee7cb39a91de95935ed57c5249e1d4f75e` | `43d78d878cd1e8cd41b23db1fb747fc5c6bd6a103cb5089bb4fb984b1afc47db` |

Control archive paths: [J1](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J1-09305-E41ABF93-5A39646C); [Release](D:/IntrusiveAnomalies/_binary_baselines/release-2026-10-05-23098fa-218BA4C7). Exact DLL controls ran on StackOBot PIE. Archived EXEs identify the controls; no claim is made that a newly linked staged EXE ran. Staged control builds were not started.

Hosts are restored by hash receipt: StackOBot detached `23098fa7415f148e935acbd4a97e18da04e6c46c`, 201 original files; Lyra detached `caa68c6799db023f7a24c0d6cfc56124ce27b542`, 183 files. Original content matches; receipt-owned helpers/fixtures and 19/26 new build files were removed. All 5764 live-stage files and 37 J1 cook-source files remain unchanged. All 38 recorded process identities are no longer alive. Foreign processes and security settings were untouched. [Restoration](E:/IA_BuildCache/_r936/receipts/restored.json), [staging verification](E:/IA_BuildCache/_r936/receipts/staging-at-stop.json).

Settings used and restored by process exit/config receipts: `-IAIBench`; PNG, seed 777, 30 fps; `CaptureBench.Marker=1`; under `IAI.Capture`, SVE/Pace/Mask/TargetMask/RunLog=1 and Delivery/FocusGate=0. Rendering overrides: `r.DefaultFeature.AutoExposure=0`, `r.EyeAdaptationQuality=0`, `r.AntiAliasingMethod=0`, `r.Lumen.DiffuseIndirect.Allow=0`, `r.DynamicGlobalIlluminationMethod=0`, `r.ReflectionMethod=0`. `IAI.Capture.Config` was `0 30 24 60 4`, cap 360; regression `0 192 48 192 4`, cap 1400. Proxy regression used `IAI.Anomaly.StuckMipRoute=auto`.

Helper commands under `IAI.Bench.Review936` were `PIE` (window/multi/selected/two-client modes), `Observe` and `Report`. Under `IAI.Bench.Review936Runtime`: `ExecClient` 0/1, `LateStart`, `Mode` controller/gameview/showflag/stereo, `Ambiguous`, `Split`, `ResizeAt 100 960 720`, and `Report`. These were process-local. Lyra retained its game instance/viewport with a process-only GameModeBase override. Exact control DLL swaps were restored. Synthetic order, pairing probe and 100%/50% resolution overrides were prepared but never run. Per-leg settings, commands and idle-gate readings remain in the [launch receipts](E:/IA_BuildCache/_r936/legs).

**8. Harness fixes.** Full ledger and retained failures: [harness-fixes.json](E:/IA_BuildCache/_r936/harness-fixes.json). No product source or gate threshold changed after qualification began.

| Problem | Fix / attempts |
|---|---|
| Part 0 wrapper compared an empty error list with integer zero | Test emptiness; 1 |
| Helper lacked the directly used AnomalyInjector module dependency | Add dependency; 1 |
| ResizeFrame is game-only in UE 5.1 | Use SetFixedViewportSize in PIE, discovered before launch; 1 |
| Ad hoc CPU summary misclassified mutants, then mutant compile steps | Separate expected execution exits from compile exits; 2 |
| Unqualified scoped stereo enum values | Qualify enum names; 1 |
| Preserved copy timestamps skipped a stale helper object | Fresh timestamps and binary version checks; 1 |
| Scanner control joined ASCII/UTF-16 bytes | Separate payloads with null bytes; 1 |
| Extra viewport draw lacked the engine's required world-context mapping | Scope the mapping during its resize/draw; R2 retry passes; 1 |
| Pairing recipe could ignore screen-percentage settings | Disable editor/dynamic/DPI overrides and check actual dimensions; pre-launch, never run; 1 |
| FOV-based world stamp clipped a reference on Lyra | Use actual constrained projection; unchanged decoder passes both R3 healthy retries; 1 |
| Manual string triage assumed every question mark meant an import | Verify exact IEEE-754 bytes for .025/.95/.9625; 1 |

The ambiguity-helper failure occurred before Capture.Start. The first Lyra delayed attempt had correct frame barcodes and native event edges but an unreadable clipped world stamp; its failed reading remains preserved. The final selected-viewport failure is a separate product label/picture disagreement and was not reclassified as a harness failure. A pre-existing handled singular-matrix ensure appeared during fixture startup; successful legs exited normally. The only engine abort was the disclosed ambiguity-helper assertion.

Deviations: the D2 stop leaves all staged, synthetic-order and pairing-probe work, remaining Lyra cases, and Lyra archived controls unrun; therefore there is no push. Editor-specific selected/two-client cases have no standalone editor-mode equivalent. The stereo lever tests refusal predicates, not a physical HMD. No office or delivery qualification is claimed.

**9. NEEDS-DECISION.** Rule on the observed selected-viewport projection/label mismatch and authorize its repair scope before resuming qualification. The likely faulty frustum path predates J2 and is outside the changes made here. Reframing the positive fixture alone would conceal the observed disagreement. The 61 unrun cases and push remain blocked on that decision and subsequent green gates.

VERDICT: NEEDS-DECISION
