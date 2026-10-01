# 090-10b2 — 090-10b written up; readiness asks the base pass and names both failure directions; stuck_low_mip's PIE "shared" is the fixture's control, not the editor world; the census cannot freeze the game

Date: 2026-10-01 (21:07 →) · Brief `_mailbox` 090-10b2 (Code, Opus 5.5, headless) · Runs FIRST, before the m53 compatibility round
090-10c. Pre-declarations: `E:\IA_BuildCache\_r910b2\predeclared.md` (written before any game leg). Harness `_reviews\090-10b2-*`
(common, go, pie driver, read), reusing 090-10b's gate, staging and pixel reader.

## 0. State at start (Part 0)
- **Heads == origin:** m53 `feat/m53-uv-normal-corruption` `0a69ea3`, fix `fix/m52-label-timing` `cf422d5` (the owner pushed both).
  `m51`, `master` (`b5f15a3` == origin) and tags untouched.
- **`0a69ea3` is the source of `65607703`:** committed 16:14:20; editor and game built 16:14–16:18 on `_r53_host` (plugin a detached
  worktree at `0a69ea3`, clean); the strict pass relinked the exe at 16:28; `_r53_host\Binaries\Win64\StackOBot.exe` still hashed
  `65607703` and matched `_binary_baselines\m53-0910b-65607703\` (`archive-hashes.json`, 6/6 match).
- **No orphan of 090-10b:** its watcher PID 27364 was gone; every PID its launcher recorded (`_r910b\ev\*\launch.json`, 68 records) was
  dead; no process carried a 090-10b path in its command line except this session's own. `UnrealTraceServer` (12:43, engine-spawned
  shared service) is in no 090-10b record and was left alone.
- **Found as left by the interrupt, and recorded:** BenchGate staged `65607703` over DC2 (not the m53 S1 set); `_r53_host`'s editor DLLs
  were the bisect's `97D292F8` copies (mtime 17:35), which UBT would have treated as up to date for unchanged modules (G422's shape) —
  moved aside to `_r910b2\host_dll_aside_97D292F8\` (not deleted) before building, so every module relinked from the host's own objects.
- **Environment:** the owner's own UE 5.7 editor (a project of his, started 21:11) was open most of the evening; game legs waited in the quiet
  gate (no editor of any engine, no foreign GPU-heavy process, 60 s without input, 3 reads) and nothing of his was touched.

## A. 090-10b's write-up and Codex's other direction (Part A)
090-10b ended INTERRUPTED; its journal is now `2026-10-01-090-10b-office-nothing-corrupts.md`, written from its own run records (cause,
fix as built, proof table across nine builds × PIE/staged, edges, can-fail, yield/startup/no-visible lines, the read-out tool, the
bounded census, builds and archives). Its bisect had reached five earlier builds: all `0 events / shader_map_incomplete 6` in PIE, all 6/6
staged.

### A.1 The predicate as 090-10b left it, against Codex's caution
`HostShadersReady` (`0a69ea3`): admitted if `IsGameThreadShaderMapComplete()`; else, in the editor, if the compile is finished and the
material's mesh shader map for the component's vertex factory holds **any** shader. Three gaps against `shader-admission-review.md`:
1. **The whole-map short-circuit trusts a cached flag.** `FMaterial::IsGameThreadShaderMapComplete` returns a value cached when the map
   was set (`MaterialShared.cpp:1102-1114`), computed for the usages the material had at compile time. A usage set afterwards (the editor
   sets missing usages itself, `Material.cpp:1749-1771`) can leave it 1 while the component's vertex factory has no shaders.
2. **"Any shader" is not what the base pass needs.** `FBasePassMeshProcessor` gets its shaders through `Material.TryGetShaders(base-pass
   VS + PS, VF)` (`BasePassRendering.cpp:507`) and, if that fails, draws the fallback (default) material.
3. **Identity was checked after readiness and both refusals shared one name.** A component whose proxy draws the Default Material
   because a usage it needs is missing (`Material.cpp:1795`, the InstancedStaticMesh proxy at `InstancedStaticMesh.cpp:1420`) was named
   `shader_map_incomplete` whenever its VF shaders were also absent, which they are by construction.

### A.2 As built (m53 `5fba7df`, timing line `61d186a`)
Chat's three-check order (superseded brief 090-10c-shader-map-gate): **metadata** (S5 `shader_map_unavailable`, unchanged) → **identity**
(S7 `default_material_path`, the existing usage test `FindUsageRefusal` that mirrors the proxy's `CheckMaterialUsage_Concurrent`, now
evaluated before readiness) → **draw readiness** (S6, `ReadHostDrawReadiness`):
- never decides on the whole-map flag (reported only);
- vertex factory per component: ISM `FInstancedStaticMeshVertexFactory`, skinned `TGPUSkinVertexFactory<Default|Unlimited>` or the
  skin-cache passthrough, spline `FSplineMeshVertexFactory` (looked up by name; it is not exported), else `FLocalVertexFactory`;
- enumerates that VF's mesh shader map (`FShaderMapContent::GetShaders`, each shader's type via the map's pointer table) and counts
  base-pass vertex and pixel shaders by type name (`TBasePassVS*`/`TBasePassPS*`, mobile twins) —
  `TexCorruptPure::ClassifyBasePassShaderTypeName` / `JudgeDrawReadiness`;
- refusals: compile still running → `shader_map_incomplete` (sub `<VF>:compile_pending`, transient, the auto-pool retries); finished but
  no VF map / no base-pass VS / no base-pass PS → **`draw_shaders_missing`** (sub `<VF>:no_vertex_factory_shaders|no_base_pass_vs|
  no_base_pass_ps`), a new final reason (run_summary gains `texcorrupt_refused_draw_shaders_missing`);
- `TEXCORRUPT-SHADERMAP '<material>' game_thread_complete=<0|1> compilation_finished=<0|1> vertex_factory=<VF> shaders=<n>
  base_pass_vs=<n> base_pass_ps=<n> -> admitted | refused <reason>`, once per material and verdict.
- ⚠ Limits, stated: the base-pass check is "at least one VS and one PS for the VF", not the exact light-map-policy permutation the
  renderer will pick (that selection lives in the Renderer module, which AnomalyInjector does not link); skinned components accept any
  of the three skin VFs.

**Tests both ways (pure):** `tools/texcorrupt_readiness_selftest.cpp` base **15/0**; mutants each fail: (1) whole-map only (the office
build's S6) 4 failures, (2) the `0a69ea3` predicate 3, (3) any-shader 2, (4) identity after readiness (source-order check) 1.

### A.3 In PIE: healthy, missing, identity (fixture; a spline mesh summoned into the PIE world)
Pixel rule as 090-10b (median drawn-box difference of the labelled frames vs the last clean frame ≥ max(4.0, 4 × the clean-to-clean
null)). Python cannot build an InstancedStaticMesh in the PIE world (`BeginDeferredActorSpawnFromClass` and `AddComponentByClass` are not
script-exported; the first identity attempt failed on that and produced no reading), so the constructs summon an `ASplineMeshActor`
(`EnableCheats`, `summon /Script/Engine.SplineMeshActor`): `USplineMeshComponent` needs `MATUSAGE_SplineMesh`
(`SplineMeshSceneProxy.cpp:76-78` swaps in the Default Material otherwise) and draws with `FSplineMeshVertexFactory`. The legs named
`P_ISM_*` are these spline constructs (named before the switch). Material flags are edited in memory only; the content manifest
after the last leg reads 2,207 files, 0 added, changed or removed (against 090-10b's last manifest).

| case | build | how | readiness / refusal (log) | events | visibly corrupt |
|---|---|---|---|---|---|
| healthy partial map | `E2BCE76B` | fixture targets `_0` (uv) and `_12` (normal) | `game_thread_complete=0 compilation_finished=1 vertex_factory=FLocalVertexFactory shaders=6 base_pass_vs=1 base_pass_ps=1 -> admitted` | 6 · 6 (and 6 · 6 · 6 on repeats) | **6/6 · 6/6** (39.8–53.6 · 9.3–17.5; null ≤ 0.34) |
| same, staged | `E2BCE76B` | BenchGate over DC2 | (cooked) | 6 · 6 | **6/6 · 6/6** (42.8–58.5 · 11.1–22.3) |
| **identity** | `E2BCE76B` | spline, `M_TC_UC4` `used_with_spline_meshes=0`, editor auto-set off; engine: `missing bUsedWithSplineMeshes=True! Default Material will be used` | identity first: **`default_material_path:spline_mesh`** on every attempt (readiness line: no spline-VF shaders) | **0** | — |
| identity | `65607703` | same | checked the WRONG VF (`FLocalVertexFactory shaders=8 -> admitted`), then S7 `default_material_path:spline_mesh` | 0 | — |
| **needed shaders genuinely absent** | `E2BCE76B` | spline, `M_TC_UC5` (never compiled for splines), usage set without recompile, fired at once | first `compilation_finished=0 vertex_factory=none -> refused compile_pending` → slot **`shader_map_incomplete:none:compile_pending`**, `Auto.Yield`, no event; the editor compiled the permutation on demand (`MaterialShared.cpp:3348-3388`), then `FSplineMeshVertexFactory shaders=4 base_pass_vs=1 base_pass_ps=1 -> admitted` | 13 (all after admission) | **13/13** (56–71) |
| same | `65607703` | `M_TC_UC3` | `compilation_finished=0 vertex_factory=FLocalVertexFactory shaders=8 -> refused` (wrong VF, one name) → `shader_map_incomplete`, then admitted | 13 | 13/13 |
| usage set, permutation already in the job cache | `E2BCE76B` | `M_TC_UC4` | admitted at once (`FSplineMeshVertexFactory shaders=4`) | 6 | 6/6 |
| healthy after a recompile | `E2BCE76B` | `M_TC_UC4`, usage set with notification, fired after 75 s | admitted | 28 | **28/28** |
| stale "complete" flag (attempted) | both | `M_TC_UC2` recompiled (skeletal usage with notification), then spline usage without recompile | **did not form**: the map still read `game_thread_complete=0`, the editor compiled on demand, both builds admitted, 7/7 visibly corrupt | 7 · 7 | 7/7 · 7/7 |

**Reading.** Identity is refused by its own name, before readiness, and never labelled. Needed draw shaders that do not exist are
refused (named `shader_map_incomplete` with sub `compile_pending`, because in the editor the base pass's own request starts their compile
the moment the component draws) and nothing is labelled until they exist; every event that was labelled is visibly corrupt. **What was
not shown in-engine:** a permanent `draw_shaders_missing`. It needs a map whose cached flag reads complete (only then does `TryGetShaders`
skip the on-demand compile, `MaterialShared.cpp:3368`); two fixture materials read complete in PIE (`M_TC_SkelNoUsage` after the editor
set its skeletal usage itself, `WorldGridMaterial`), but my construct could not produce one on a material I can target. The case is
pinned by the pure test (mutant 2, the `0a69ea3` predicate, admits it) and by source. Natural `no_vertex_factory_shaders` verdicts did
occur (`M_TC_NaniteOverride`, a Nanite-only override material whose actor the Nanite gate refuses first), never as an event reason.

## B. stuck_low_mip in PIE (Part B)
### B.1 The premise did not survive the evidence
The brief: *"in PIE stuck_low_mip refuses 6/6 `refused_shared` ... the purity enumeration ... counts the editor world's copy."*
090-10b's own PIE leg (`P_8131479A_SM`, office exe, fixture, targeted `StaticMeshActor_77`) logged, on each of its 6 attempts:

    stuck_low_mip: PURITY ENUMERATION for '=StaticMeshActor_77' - scope ALL LOADED LEVELS (1, active or not): 98 component(s) ...
    stuck_low_mip: REFUSED TEXTURE 'T_TC_M52Shared' shared_world - 2 user component(s) in the whole loaded world (1 not a target
    component) ... Users: [ StaticMeshActor_77.StaticMeshComponent0(StaticMeshComponent) StaticMeshActor_78.StaticMeshComponent0(...) ]

One level, the PIE world's; the second user is `_78`, in that same world. The fixture builds it so on purpose:
`make_texcorrupt_fixture.py:1443-1446` gives `TC_M52Hold` (role "m52 target (EXCL)") and `TC_M52Share` (role "m52 co-sharer") the same
`M_TC_M52Shared`. **It is the m52 purity negative control, refused on every build in every mode.** The scan was already world-scoped
(`GatherLoadedLevels(World)` = `World->GetLevels()` + its loaded streaming levels). Cross-check on MainWorld (090-10b `07A65316` rock
legs): every refused texture has 2 users in PIE **and** 2 staged (`T_detail_N`, `T_black`, `T_grunge_mask`); PIE scans 1008 components
against staged 855 (editor-only components), and the user counts do not move.

**Per world, in PIE, on this round's build (`P_PROBE__2`):** PIE world `/Game/CaptureBenchTexCorrupt/UEDPIE_0_CB_TexCorruptLevel` →
`T_TC_M52Shared` used by `StaticMeshActor_77.StaticMeshComponent0`, `StaticMeshActor_78.StaticMeshComponent0`; editor world
`/Game/CaptureBenchTexCorrupt/CB_TexCorruptLevel` → the same two names. Four copies exist across the two worlds; the product counted **2**
(`P_SM77`: `PURITY ENUMERATION ... world 'CB_TexCorruptLevel' (type=PIE), scope ALL LOADED LEVELS OF THAT WORLD (1 ...): 98
component(s)`, `shared_world - 2 user component(s)`). (`UnrealEditorSubsystem.get_editor_world` returns None during PIE and logs
"The Editor is currently in a play mode"; the probe reads the editor world as the loaded map asset.)

**So no purity code changed.** What changed (fix `c60de04`, merged): the PURITY line names its world and type and says other worlds are
not scanned; the HELD NONE summary no longer calls a world-rule refusal "shared with a visible component"; `m52_log_counts.py` follows the
wording (`a5f0a06`); `m52_window_selftest` carries the two-world case — a component of another world is out of scope (pure), a second
user in the same world is counted (shared) — and an all-worlds `InPurityScope` mutant fails it (2 failures, incl. the new check).
⚠ The brief's "this is the likely reason it fired zero on the first office host" is therefore **not supported**; why stuck_low_mip shows no
blur at the office is not established here (the office has given no stuck_low_mip counts; `Auto.Yield stuck_low_mip` will name it).

### B.2 PIE pixel proof and label sync (MainWorld rock, B9 recipe, with its B5 null)
MainWorld rock `StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803`, `IAI.Targets.AllowNanite 1`, `IAI.Capture.Config 2 4 8 30 0`,
1,200 frames, `r.AntiAliasingMethod 0`, `-IAIBench`; null B5 = the same with `IAI.Bench.StuckMipNoHold 1` (the lever registers only
with `-IAIBench`; the first B5 pair lacked it, read `Command not recognized`, and was discarded as invalid). Gate: the m52 E1 evaluator
`_reviews\084-09-eval.py` (`m52_leg` + `summarize_rows`), release threshold t50.

| leg | where | events | E1 t50 | start / end | unexcused frames | leg verdict |
|---|---|---|---|---|---|---|
| B9 vs B5 | **PIE** | 18 (17 judgeable) | **PASS 17/17** | `{+0:17}` / `{+0:17}` | 0 | **PASS** |
| B9 vs B5 | staged | 17 (16 judgeable) | **PASS 16/16** | `{+3:16}` (flagged partial frames) / `{+0:16}` | 0 | **PASS** (as 084-09's B9) |
| auto-pool three types (AllowNanite 0) | PIE | 5 stuck_low_mip | — | — | — | **5/5 visibly blurred** (4.17–4.93, null ≤ 0.55) |
| targeted `StaticMeshActor_77` | PIE | 0 | — | — | — | refused `shared` ×6, as designed |

(The strict 0-threshold reading of the PIE B9 has 2 end-edge FAILs and 3 unexcused frames; t10 and t50 are clean. 090-10b's pixel-rule
reader is not the m52 gate: its 4.0 floor is set for uv/normal and reads stuck_low_mip's blur, median 3.3–4.3, as 8/18 on the same leg.)

## C. The census cannot freeze the game (Part C)
Bound declared before any leg: no frame over 100 ms while the census runs. New in `61d186a`: an
`IAI-TEXCORRUPT-CENSUS v1 timing frames=<n> work_ms_max=<ms> frame_interval_ms_max=<ms>` line before the unchanged `end` line
(`census_readonly_check.py` allows it: format, three numeric arguments, auxiliary line; source PASS, selftest 38/38).

| scene | where | build | candidates | frames | plugin `work_ms_max` / `frame_interval_ms_max` | independent max frame (median before) | progress lines | verdict |
|---|---|---|---|---|---|---|---|---|
| MainWorld | PIE | `E2BCE76B` | 343 | 4 | 25.6 / 25.7 | **36.7 ms** (16.6) | 0 (done in < 2 s) | PASS |
| MainWorld after a capture | PIE | `E2BCE76B` | 343 | 3 | 4.6 / 12.3 | **17.0 ms** (12.2) | 0 | PASS |
| MainWorld | staged | `E2BCE76B` | 343 | 2 | 4.6 / 13.0 | **18.0 ms** CSV, 0 of 600 frames > 100 ms (12.7) | 0 | PASS |
| office scale: MainWorld + 15,000 summoned actors | PIE | `E2BCE76B` | 15,343 | 103 | 51.8 / 51.9 | **63.9 ms** (16.7) | 0 (done in 1.7 s) | PASS |
| characterisation: + 40,000 actors | PIE | `E2BCE76B` | 40,343 | 671 | **195.3** / 636.0 | 650.0 ms; 2 of 696 frames > 100 ms (33.4) | **11** | ⚠ over the bound |
| MainWorld, no prior capture | PIE | `8131479A` (office) | 343 | 1 | — | 19.9 ms — every candidate refused `corruptor_not_ready` at E2, before the O(N²) resolution | — | (not a reproduction) |
| **MainWorld after a capture (the office's order)** | PIE | `8131479A` | 343 | 1 | — | **192.3 ms** (14.2); `shader_map_incomplete:68` | — | **can-fail fires** |
| **MainWorld** | staged | `8131479A` | 343 | 1 | — | **150.8 ms** CSV (12.5) | — | **can-fail fires** |

Instruments: PIE, the driver's slate post-tick recorder (wall time between editor ticks, over the census `begin`..`end` window); staged,
the engine CSV profiler's per-frame `FrameTime` over 600 frames that contain the census. Both builds read by the same instrument.
Counts on the new build equal 090-10b's staged reading (uv 25/318, normal 9/334 staged; PIE normal 8/335). ⚠ **The bound fails above
office scale:** the `all` enumeration of the first call is not sliced — 25 ms at 343 actors, 52 ms at 15,343, 195 ms at 40,343 — and at
40k a 650 ms frame falls between two census calls whose own work was under 196 ms (engine work in that frame; cause not established,
G120). The office's world is 15,906 candidates. 090-10b's "0.972 s" for the office exe came from log-line gaps; the per-frame CSV
reading here is 150.8 ms; at 15,906 candidates the O(N²) term scales that to minutes, the office's "froze".

## D. Builds, gates, archives, commits

| | m53 `feat/m53-uv-normal-corruption` | fix `fix/m52-label-timing` |
|---|---|---|
| code head (proven) | `61d186a` | `a5f0a06` (Source as `c60de04`) |
| exe | **`E2BCE76B`** | **`5E93A74E`** |
| normal build | game + editor exit 0, 0 warnings (on `5fba7df` first: exe `109A8406`) | same |
| strict include pass | **0/0**, 64/64 header TUs, Build.cs restored, normal 0/0 | **0/0**, Build.cs restored, normal 0/0 |
| string scan vs the previous archive | vs `65607703`: 35 = this round's strings (old/new `TEXCORRUPT-SHADERMAP`, PURITY, HELD NONE, census help; new `timing` line, `draw_shaders_missing`, `refused %s`, the two VF names looked up by name) + one-byte string-pool artefacts | vs `DA903919`: 12 = PURITY and HELD NONE old/new in exe and DLL + 4 one-byte artefacts |
| lever audit | 47/47 PASS, exe and DLLs | 34/34 PASS, exe and DLLs |
| C++ suites | base 7/7 (adds readiness 15/0), mutants 27/27 failing | base 5/5, mutants 20/20 failing + the all-worlds purity mutant |
| Python suites | 27/27 (on the final head) | 19/19 (on the final head) |
| archive | `_binary_baselines\m53-0910b2-E2BCE76B\` (6/6 re-hashed) | `_binary_baselines\m52fix-0910b2-5E93A74E\` (6/6 re-hashed) |

Commits: fix `c60de04` (stuck_low_mip log), `a5f0a06` (m52_log_counts), the docs commit carrying this journal; m53 `45f1367` and `ec3f617` (merges of fix),
`5fba7df` (readiness, identity order), `61d186a` (census timing line), the merge of that docs commit and an m53 docs commit (architecture, client-readme, status). The m53 head pushed is the one the legs ran on
(exe `E2BCE76B` is the strict pass's normal rebuild of `61d186a`; later commits are docs only).

## E. Open
- **PIE end edge +1 on about one uv/normal event per PIE leg** (the first frame after the label still differs): 7 of 7 PIE legs on
  `E2BCE76B` and 3 of 3 on `0a69ea3` in this session (A/B: `P_UV_65607703__ab1` 2 events, `__ab2` 1, `P_UV_E2BCE76B__ab3` 1); 090-10b saw
  it on `2F924FB3` and not on its two `65607703` PIE legs; staged legs show none. Pre-existing and PIE-only; mechanism not chased (G120).
- **Unsliced census enumeration**: over 100 ms in its first frame above ~30k actors (195 ms at 40k). Slicing it is a small change;
  not made mid-campaign because every leg here ran on `61d186a`.
- **`draw_shaders_missing` in-engine**: shown only by the pure test and source (see A.3).
- **The office's `runtime_lod_bias`** (14,454 of 15,906 first refusals on `0a69ea3`): 090-10c's subject (Codex's compatibility review).
- **stuck_low_mip at the office**: no counts exist; the brief's attribution to the editor world is refuted (B.1). `Auto.Yield
  stuck_low_mip` and `tools/anomaly_refusal_counts.py` will name it.
- Spline and skin-cache vertex factories are looked up by name; the base-pass test is "a VS and a PS for the VF", not the exact
  light-map-policy permutation (A.2).
