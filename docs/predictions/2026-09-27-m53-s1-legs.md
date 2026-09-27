# m53 S1 step 6 — the gate legs: pre-declared legs, rules, classes and predictions

**Brief 082-07a, 2026-09-27. Declared before any leg runs.** Branch `feat/m53-uv-normal-corruption`, parent `aadd7eb`
(plan revision 3.4, `docs/predictions/2026-09-26-m53-uv-normal-corruption.md`). The legs run in brief 082-07b through the
harness `D:\IntrusiveAnomalies\_reviews\082-07-*`, locked by `_reviews/082-07-evidence/boundary.json` at the commit that
carries this file. **Nothing here changes a plan gate, a tolerance or a stop row.** Where the plan leaves a mechanism open,
this file fixes it in advance and lists it in §9 as a named deviation.

> **⚠ READ AMENDMENT 1 (082-07d, 2026-09-27) AT THE END FIRST.** It governs over §§1–10 wherever they differ: new build,
> normal rows, the admission pre-check, G3's pixel rule, REASON A2, and the resume plan for 082-07e.

**Inputs:** plan §R12 (gates), §R12.2 (levers), §R12.4 (the G-ID matrix), §R13.1 step 6 and its 14-row stop list; rulings
`_reviews/082-05-chat-ruling-s1-authorised.md`, `082-06b-…`, `082-06c-…`, `082-06d-…`, and `083-02-chat-ruling-disk-cleanup.md`
policy items 1–2.

**The build under test** (staged by 082-06d, checked by the preflight before every window and by the harness before every
attempt): exe `2FCDF059`, `StackOBot-Windows.utoc` `9A26D497`, `.ucas` `10D7F0C0`, `.pak` `ABD931A2`, `global.utoc`
`462B8AC6`, `global.ucas` `BB05CF99`. Source at this branch head is byte-identical to the source that built the exe
(`git diff 1fc20ea HEAD -- Source` is empty).

---

## 1. Common recipe

- **Launch:** the staged exe by path, `<map> -windowed -ResX=1280 -ResY=720 -ExecCmds="…" -unattended -nosplash
  -abslog=<attempt evidence>\game.log`. The full log goes to the evidence folder, so nothing depends on log rotation.
- **Base ExecCmds, every leg:** `r.DefaultFeature.AutoExposure 0`, `r.EyeAdaptationQuality 0` (plus both echoed bare, A48),
  `CaptureBench.Marker 0`, `CaptureBench.Stall.GameMs 0`, `IAI.Capture.SVE 1`, `IAI.Capture.Delivery 0`,
  `IAI.Capture.Pace 1`, `IAI.Capture.Fps 30`, the **AA-off arbiter** `r.AntiAliasingMethod 0`,
  `r.Lumen.DiffuseIndirect.Allow 0`, `r.DynamicGlobalIlluminationMethod 0`, `r.ReflectionMethod 0` (the banked
  `M49_GEDGE_*` / `B19_M45_AA_*` recipe, G184), `IAI.Bench.SynthTickOrder 0`, `IAI.Capture.Mask 1`,
  `IAI.Capture.TargetMask 1`.
  - The marker is **off**: the F-SYN encoding row sits at y 8–72, inside the strip the marker draws into (G125).
- **Capture:** `IAI.Capture.Config <cfg>`, then the leg's levers, then
  `IAI.Capture.Start <attempt label> png 777 <frames> <id> <actor>`. Targeted fire only.
  - F-SYN: `2 16 8 4 0`, 100 frames (16 lead-in, then 8 labelled + 4 post per burst: **7 events per leg**).
  - F-MW: `2 40 8 4 0`, 100 frames (the lead-in covers MainWorld's ~25-frame camera settle: **5 events**).
  - G-RD: `2 16 100 4 0`, 120 frames (**one event of 100 labelled frames**).
  - G4 FIN: `2 16 200 4 0`, 40 frames (the frame cap lands inside the first event). G4 END and GC: `2 16 200 4 0`,
    240 frames (a 200-frame event, ~6.7 s).
- **Maps:** F-SYN `/Game/CaptureBenchTexCorrupt/CB_TexCorruptLevel`; F-MW `/Game/StackOBot/Maps/MainWorld`; F-GATE
  `/Game/CaptureBenchGate/CB_GateLevel` (one refusal row). F-LYRA is S3 (it needs a Lyra worktree build) and is **not** in
  S1.
- **Targets** are cooked actor names. F-SYN names come from the fixture map `082-06-evidence/texcorrupt_fixture_actors.json`
  (hash-locked), and G0's `IAI.ListActors` checks every one of them in the cooked level before any targeted leg (§3,
  FIXTURE-NAMES). The F-MW floor target is `RoomBuilderSquare_C_UAID_00155DE1D4771FCF00_1080865874` (asset
  `SM_FloorBase`, banked sessions `B22_PRECOOK_RESCUE`). The Bot is spawned at run time, so its name is resolved from G0's
  F-MW `IAI.ListActors` listing (exactly one `BP_Bot_C_*`), falling back to the banked `BP_Bot_C_2147482434`; the choice is
  recorded with the leg.
- **Tick order:** every S1 leg runs the native order. No S1 gate asserts a per-frame alignment: G-ID, G-ID-M, G-RD, G3 and
  G-BIND are pixel arbiters, which run native order at the AA-off configuration (G230), and G4, G6, G-REASON, G-COLL, G0,
  G11, TRIP and the luma gate read logs, labels and summaries. The plan's only alignment gate, `G1` (ONSET, both orders),
  is S2.

## 2. Leg validity, attempts, banking and stops

**Validity** is decided per attempt, before any gate reads it, from how the leg ran and never from what it showed (A63):

| rule | invalid when |
|---|---|
| protocol | the runner did not complete (no world, the game exited, the capture wait expired) |
| A63 focus | foreground not held within 20 s of launch, or the run started ≥ 20 s after launch (`run.json` `start_frame` / `sustained_wall_fps`) |
| whole-leg foreground | any capture-time foreground sample is not the game |
| artifact | label rows ≠ the leg's frames, PNGs ≠ rows, or any zero-byte PNG |
| fixture | a targeted fire matched no actor (`Auto.FireSpecific … -> 0 matched`) before any `IAI.Bench.DestroyTarget` fired |
| pose (all capture legs) | the modal view covers < 50 % of rows, or a labelled row is off the modal view |
| pose (legs whose pixels are compared: roles ID, NULL, CTL, FAULT, TILE, TILENULL) | F-SYN: the modal rotation is not the PlayerStart's `(0,0,0)` (the A47 criterion); every map: the modal view differs from the campaign reference, which the first accepted pixel leg on that map sets |
| census / cancel | fewer `TEXCORRUPT-CENSUS END` lines than census commands; no cancel line |

- **Budgets:** 3 invalid attempts, or 6 pose-only invalid attempts, ⇒ `NOT-RUN` (stop code 2). An attempt whose only error is
  the whole-leg foreground **and** that carries person evidence is `ENV-INTERRUPTED`: it does not count, and 5 of them stop
  the window `NOT-QUIET` (code 4). The person-present gate (60 s of no input, excused only inside a clean harness window),
  the quiet gate (no editor or game of any engine, none of our build tools) and the 45-minute wait budget per tag are the
  081-23/081-30 rules, unchanged.
- **Every attempt is banked**, valid or not, as `_bench_sessions_bank\M53S1_<leg>_A<n>\` with `_leg\` (full log, launch
  record, command, runner log, bank manifest).
- **Disk policy (083-02 items 1–2):** the bank copy is verified file by file (path, size, SHA-256) against the exe-side
  output, and only then is the exe-side copy deleted; a mismatch keeps the exe-side copy and stops the window (code 6). The
  accepted attempt is aliased as `M53S1_<leg>\` by **NTFS hardlinks**, never copies.
- **Stop at the first failure.** Gates are evaluated as soon as their legs are accepted. A Q failure stops the window
  (code 1). `INVALID-PAIR`, `INVALID-FIXTURE` or a failed precondition stops it as NEEDS-DECISION (code 3). An evaluator
  error stops it (code 7). D readings and PROOF readings never stop. A recorded stop or `NOT-RUN` stays stopped on every
  later call until a ruling; nothing is re-run to get a pass.

## 3. How pixels are compared

- **Pairing.** Two legs of one row are paired by `session_index`. They must carry the same labelled `session_index` lists
  for the id, or the pair is `INVALID-PAIR`. That is fixed before reading and does not depend on pixels.
- **Region.** On each labelled frame, the region is the **union of the two legs' target masks** at that event's
  `mask_value` (m43 masks, 8-bit, numbered by `session_index`). A frame with no mask in either leg is unmeasured. An event
  counts only with ≥ 1 measured frame.
- **d** = the per-pixel maximum over R, G, B of |applied − null| in the written 8-bit PNG.
- **Arbiter validity (precondition).** The F-SYN control pair `CTL_A` / `CTL_B` (two `NoApply 1` legs of one recipe) must
  read region max ≤ 2; the whole-frame reading is reported. If it does not, F-SYN cannot judge identity and the window stops
  as NEEDS-DECISION. On F-MW the same test only decides whether the F-MW rows are judged.

## 4. The gates

Class **Q** stops (stop row in the last column). **D** is a reading. **PRE** is a precondition. **PROOF** is an owed
in-engine proof.

### 4.1 Preconditions and read-backs

| gate | class | legs | rule | minimum | stop row |
|---|---|---|---|---|---|
| FIXTURE-NAMES | PRE | `G0_FSYN` (`IAI.ListActors` + 77 census commands) | every one of the 77 fixture actor names is in the cooked level's listing | all 77 | — (code 3) |
| LUMA (`G151`) | Q | `CTL_A`, frame `session_index` 0 | mean luma > 0 **and** non-zero pixel count > 0 | 1 frame | 13 |
| G11 | Q | every leg's full log | `TEXCORRUPT-ASSETS` names `M_CorruptTex_UV`, `M_CorruptTex_Normal` and `T_CorruptTex_NoiseN`, each `resolved=1`. The plan's second half (every corruptor × target format drew non-default content) is carried by G-ID, as the plan says. | every leg | 10 |
| TRIP | Q | every leg with a `run_summary` | `texcorrupt_rt_mip_mismatch` = 0 | every leg | 5 |
| BAND F-SYN | PRE | `CTL_A`, `CTL_B` (`TC_UC1`, `NoApply 1`) | region max ≤ 2 (predicted 0, whole frame 0: the m45 precedent) | all 100 frames | — (code 3) |
| G-BIND (1) link | Q | none (static) | `AnomalyInjector.Build.cs` at the locked head declares public `Core, CoreUObject, Engine, InputCore` and private `Foliage, RenderCore, RHI`, nothing else; 082-06c's G10 record reads both targets exit 0 with no warning line; the staged exe equals the G10 archive `2FCDF059` | once | 2 (and 1) |

### 4.2 G-ID — identity through each encoding row (plan §R12.4)

Identity leg (`IAI.Bench.TexCorruptIdentity 1`) against its matched null (`Identity 1` + `NoApply 1`), and one leg per
assigned wrong copy (`Identity 1` + `IAI.Bench.TexCorruptWrongCopy <fault>`), all on F-SYN, 7 events per leg.

- **PASS** iff every measured frame of every event reads max d ≤ 2, **and** ≥ 3 events are measured, **and** every assigned
  wrong copy reads d ≥ 16 on **every** measured frame of ≥ 3 events. EXACT is reported when max d = 0. The histogram (0, 1,
  2, 3–7, 8–15, ≥ 16) is reported per row.
- A wrong copy below 16 means the instrument is invalid (G96): FAIL, stop row 3.

| row | target | id | assigned wrong copies (offline minimum, 082-06 manifest) |
|---|---|---|---|
| U-C1 | `TC_UC1` `StaticMeshActor_0` (null = `CTL_A`) | uv | chanswap (120), srgbtwice (73) |
| U-C2 | `TC_UC2` `StaticMeshActor_1` | uv | chanswap (120), srgbtwice (73), alpha (72) |
| U-C3 | `TC_UC3` `StaticMeshActor_2` | uv | chanswap (120), srgbtwice (73), alpha (72) |
| U-C4 | `TC_UC4` `StaticMeshActor_3` | uv | chanswap (120), srgbtwice (73) |
| U-C5 | `TC_UC5` `StaticMeshActor_4` | uv | srgbtwice (37) |
| U-D1 | `TC_UD1` `StaticMeshActor_5` | uv | chanswap (120) |
| U-D2 | `TC_UD2` `StaticMeshActor_6` | uv | chanswap (120), alpha (72) |
| U-D3 | `TC_UD3` `StaticMeshActor_7` | uv | chanswap (120), alpha (72) |
| U-D4 | `TC_UD4` `StaticMeshActor_8` | uv | chanswap (120) |
| U-D5 | `TC_UD5` `StaticMeshActor_9` | uv | chanswap (56) |
| U-D6 | `TC_UD6` `StaticMeshActor_10` | uv | chanswap (56) |
| U-N1 | `TC_UN1` `StaticMeshActor_11` | uv | normal (89), chanswap (69) |
| N-N1 | `TC_NN1` `StaticMeshActor_12` | normal | normal (89), chanswap (74) |
| MASKED (alpha half) | `TC_Masked` `StaticMeshActor_13` | uv | alpha |

**Minimum:** 3 applied, measured events per row. **Legs:** 14 identity + 14 nulls (`CTL_A` is U-C1's) + 1 control
(`CTL_B`) + 24 wrong copies = 53.
**The F-MW row** (`SM_FloorBase`, identity only, no wrong copy: host content is not provable offline): `FMW_ID` against
`FMW_CTL_A`, judged only if the F-MW control pair (`FMW_CTL_A` / `FMW_CTL_B`) reads region max ≤ 2; otherwise
`INVALID-ARBITER`, a reading. If the identity fire is refused, `NOT-ADMITTED` (a G0 finding against §R14.1, not a gate
failure). Stop row 3 applies only to a judged F-MW row.

### 4.3 G-ID-M — identity across geometry and mip chains

Same criterion as G-ID. Rows: chains **(a)** `ChainA`, **(b)** `ChainB`, **(c)** `ChainCSharpen`, `ChainCBlur`,
`ChainCAlphaCov`, **(d)** `ChainD` × geometries magnification (`_Mag`), minification (`_Far`), grazing (`_Grazing`) and mip
transition (`_Transition`), through uv_corruption (24 rows); chain (d) also through normal_corruption at all four geometries
(4 rows); and the shared-sampler row `TC_Shared` `StaticMeshActor_14`.

| fault | rows it must fail (Q) | offline minimum |
|---|---|---|
| texelshift | (b) Mag, (d) Mag (uv), (d) Mag (normal) | 128; 89 |
| mipshift | (a) Far, (a) Grazing, (a) Transition | 80 |
| mipgen | (a) Far, (a) Transition | 80 |
| chanswap | shared sampler | 120 |
| mipgen (**D**, plan I5) | (c) Sharpen, Blur, AlphaCov × Far, Transition | not provable offline — reported, never a stop |

Actor names: `ChainA` Mag/Grazing/Far/Transition = `StaticMeshActor_17/27/37/47`, `ChainB` 18/28/38/48, `ChainCSharpen`
19/29/39/49, `ChainCBlur` 20/30/40/50, `ChainCAlphaCov` 21/31/41/51, `ChainD` 22/32/42/52. **Minimum:** 3 events per
geometry × chain row. **Legs:** 29 identity + 29 nulls + 9 Q faults + 6 D faults = 73. Stop row 4 (and 5 for the tripwire).

### 4.4 G-RD — identity redraw

`IAI.Bench.TexCorruptIdentityRedraw 1` on `TC_RedrawOpaque` `StaticMeshActor_15` and `TC_RedrawAlpha` `StaticMeshActor_16`;
the matched null adds `NoApply 1`; the control adds `WrongCopy noclear`.

- **PASS** iff an event has ≥ 90 labelled frames, every labelled frame is within G-ID's PASS against the null, the applied
  leg's frame N differs from its frame 1 in **0** pixels of the frame-1 mask, **and** noclear reads d ≥ 16 on the event's last
  frame (offline 112 opaque, 51 alpha).
- **Minimum:** 1 leg per input. **Legs:** 6. Stop row 6.

### 4.5 G3 — revert and settle

From every G-ID / G-ID-M identity leg with its matched null (44 rows including F-MW).

- **Pixels:** the first post-revert frame (`end+1`) against the pre-onset reference (`start−1`), whole frame, must differ in
  no more pixels than the matched null's same comparison (predicted 0 on F-SYN). On F-MW the pixel clause is judged only if
  the F-MW control pair reads 0 over the whole frame; otherwise it is `INVALID-ARBITER`.
- **Log:** per event, `restored-exact + restored-default` = the slots committed on the APPLIED line; `left-to-game`,
  `unresolved`, `swept` all 0; on F-SYN every slot `restored-exact` (the tiles carry an override), naming the material the
  tree's DECIDE line read as `raw` for that slot, with the override length unchanged; and a
  `TEXCORRUPT-LEDGER … kind=post_revert … endpoint=on_frame live=0 pending=0 balance=balanced` line per revert.
- **Minimum:** 3 events per id (`G3-COUNT`, evaluated at the end over every PASS row). Stop row 7.

### 4.6 G4 — exits and failures (each once per id: uv on `TC_UC1`, normal on `TC_NN1`)

| exit | leg | recipe | PASS iff |
|---|---|---|---|
| normal revert | every identity leg | — | G3's log clause |
| `FinishRun` | `G4_FIN_*` | frame cap 40 inside a 200-frame event | the last event is live at the cap; its revert is exact; ledger balanced at R+2; `texcorrupt_restored_exact` = committed |
| cancel before focus | `G4_CAN_*` | `IAI.Capture.Start …, IAI.Capture.Stop` in one batch | the cancel line; no m53 APPLIED / REFUSED / LEDGER line; no session folder |
| target destroyed mid-span | `G4_DES_*` | `IAI.Bench.DestroyTarget <actor> 2` | DESTROYING; `TARGET LOST … (destroyed or removed)`; that revert has `left-to-game` 0 and `swept` 0; its post-revert ledger balanced |
| world end | `G4_END_*` | graceful close (WM_CLOSE to our own PID) 1 s after APPLIED | a revert after the close, exact; every teardown / post-revert ledger line after it reads `live=0`; the process exits within 30 s |
| forced GC | `G4_GC_*` | `gc.TimeBetweenPurgingPendingKillObjects 1`, a 200-frame event | ≥ 1 `LogGarbage: Collecting garbage` between the event's APPLIED and REVERT, `condition_held` true on every labelled frame, exact revert, ledger balanced. No GC pass inside the event ⇒ **UNEXERCISED** (not a pass) |
| foreign material | `G4_FR_*` | `IAI.Bench.TexCorruptForeignReplace 1` | the first revert reads `left-to-game` 1 and restores every other slot; ledger balanced |
| rollback at each step | `G4_FS2/FS2O/FS3/FS4/FS5/FS6_*` | `IAI.Bench.TexCorruptFailStep 2`, `2 1`, `3`, `4`, `5`, `6` | one ROLLED BACK line with that step and reason (`rt_alloc_failed`, `param_readback_mismatch`, `draw_precondition_failed`); for `2 1` the detail reads `resource_created=1`; **the F+2 reading** `TEXCORRUPT-LEDGER kind=rollback … offset=2 endpoint=on_frame live=0 pending=0 balance=balanced`; `texcorrupt_rollback_<n>` = 1; labelled events = APPLIED lines (the rolled-back fire left no label) |
| NoApply 2 watches (P2-6) | `G4_DES2_UV` with `G4_DES_UV` | `NoApply 2` + `DestroyTarget` | `TARGET LOST` ends the NoApply-2 event; its ledger balanced; the apply→TARGET LOST frame offset equals the applied leg's |
| both ids live | `G4_TWO` | uv capture on `TC_UC1`, then `IAI.Apply normal_corruption =StaticMeshActor_12` after the Start (after the clean slate) | the normal event is applied before the first uv fire and is not reverted by any uv event; every uv post-revert ledger line reads `live` = the normal event's own live bytes and `pending` 0; at world end the normal event reverts exactly and the last ledger line reads `live=0` |
| level change | — | **UNEXERCISED** | `IAI.Bench.ChangeTeardownAt` refuses off `CB_GateLevel`, and a packaged unattended leg has no mid-run console channel. The m53 callback it would reach (`OnWorldTeardown`) is exercised by the world-end rows |
| re-created component → swept | — | **UNEXERCISED** | no S1 lever re-creates a component mid-event (§R12.2 lists none) |

**Legs:** 26. Minimum: each exercised row once per id. Stop row 8.

### 4.7 G-BIND (2)–(4), G6 (Δ1) and G-REASON (S1 rows)

All on F-SYN with `Identity 1` (so the tree gets past E6), 7 fires per leg, unless noted.

- **G-REASON PASS** iff every REFUSED line for the id carries the expected final key (equal, or it plus `:sub`) at the
  expected event step; no APPLIED line; `texcorrupt_refused_<reason>` equals the refusals and every other
  `texcorrupt_refused_*` is 0; `texcorrupt_fires_applied` 0; and the expected slot/binding disposition is counted.

| row | producer | expected final (step) | disposition |
|---|---|---|---|
| E1 | `TC_UC1` + `IAI.Bench.TexCorruptForceMissingAsset 1` | `assets_unavailable:bench_force_missing_asset` (E1) | — |
| E4 | `TC_UC1` + `r.MipMapLODBias 1` | `runtime_lod_bias:global_sampler` (E4) | — |
| E5 | `TC_UC1` + `r.Streaming.UsePerTextureBias 0`, `r.Streaming.MipBias 1` | `runtime_lod_bias:global_streaming` (E5) | — |
| E7 | `TC_NoMesh` `TargetPoint_0` | `no_mesh` (E7) | — |
| S1 | `TC_SlotEmpty` `StaticMeshActor_72` | `no_eligible_slot:all_untouched` (V1) | `slot_empty` |
| S2 | `TC_Translucent` `StaticMeshActor_61` | `no_eligible_slot:all_untouched` (V1) | `slot_translucent` |
| S3O | F-MW Bot (resolved name) | `host_mid:override` (V1) | — |
| S3A = **G6 Δ1** | `TC_AssetMid` `StaticMeshActor_60` + `IAI.Bench.TexCorruptAssetSlotMid StaticMeshActor_60` | `host_mid:asset_slot:mid` (V1) | `host_mid:asset_slot:mid`; G6 also needs the lever's ARMED line (≥ 1 slot) and its RESTORED line (≥ 1 put back, 0 left alone) |
| T1 | `TC_VT` `StaticMeshActor_62` | `virtual_texture` (V1) | — |
| T2 | `TC_Cube` `StaticMeshActor_63` | `unsupported_type` (V1) | — |
| T3 | `TC_UIGroup` `StaticMeshActor_64` | `excluded_group` (V1) | — |
| A1 | `TC_UI1x1` `StaticMeshActor_66` | **APPLY**, no refusal | binding `non_spatial_exempt` |
| T4 = **G-BIND (2)** | `TC_Alias` `StaticMeshActor_58` | `texture_not_parameter` (V1) | — |
| T5 | `TC_Half` `StaticMeshActor_67` | `unsupported_encoding` (V1) | — |
| T9C | `TC_Cine` `StaticMeshActor_68` | `runtime_lod_bias:cinematic` (V1) | — |
| T9S | `TC_Stream2k` `StaticMeshActor_74` + `r.Streaming.MipBias 1` | `runtime_lod_bias:streaming_budget` (V1) | — |
| T10 | `TC_Stream2k` without it | `not_fully_resident` (V1) | — |
| A2 | F-MW `SM_FloorBase` actor, normal_corruption | `no_normal_map` (V1) | — |
| A5 | `TC_Small` `StaticMeshActor_69` | `below_size_policy` (V1) | — |
| A6 | `TC_Nine` `StaticMeshActor_70` | `map_set_over_cap` (V1) | — |
| V2 | `TC_Big` `StaticMeshActor_71` | `over_budget` (V2) | — |
| S8 | F-GATE `StaticMeshActor_49` | `no_textures` (V1) | — |
| V3 | the G4 FailStep legs (§4.6) | `rt_alloc_failed` / `draw_precondition_failed` / `param_readback_mismatch` | — |

UNEXERCISED in S1, listed and never read as zero: `corruptor_not_ready`, `dxt5_normal_host`, `shader_map_unavailable`,
`shader_map_incomplete`, `mip_chain_shape`, `resource_not_ready`, `streaming_pending`, `runtime_lod_bias:per_texture`
(§R6.4), and the S2 producers `mode_invalid`, `nanite_override`, `default_material_path`, `normal_unconnected`,
`held_by_stuck_low_mip`. **Legs:** 22. Stop row 9 (G6 and G-REASON), row 2 for T4.

- **G-BIND (3), Q, stop row 2.** On `TC_Layer` `StaticMeshActor_57`: `BIND_TILE` (`TileProbe 2`) against `BIND_TILENULL`
  (`TileProbe 2` + `NoApply 1`), and `BIND_ID` against `BIND_IDNULL`. The global half and the layer half of the tile are
  found from the null picture alone: of the four half-tile candidates (a 2-pixel band at the split excluded), exactly one must
  be uniform (the global `Tex` is a solid colour) with a non-uniform opposite half; otherwise `INVALID-FIXTURE` (code 3).
  **PASS** iff, on every measured frame of ≥ 3 tile-probe events, the global half reads max d ≤ 2, the layer half has ≥ 1
  pixel with d ≥ 16, and everything outside the target reads max d ≤ 2; and identity on the layer target passes G-ID.
- **G-BIND (4), D.** `BIND_DEAD` on `TC_Dead` `StaticMeshActor_59`: the dead binding's tree disposition and whether the fire
  applied, reported.

### 4.8 G-COLL (D) and G0 (D)

- **G-COLL**, `IAI.Bench.TexCorruptCollateralDetail 1` on both legs; applied (`Identity 1`) against the no-allocation null
  (`Identity 1` + `NoApply 2`); on F-SYN `TC_UC1` and on F-MW the `SM_FloorBase` actor. The `TEXCORRUPT-COLL` lines are paired
  by (event, sample kind, frame offset) and then by texture path; a texture's deficit is `max(0, null − applied)` resident
  levels. Reported: every positive deficit by name, their sum, unpaired textures, and incomplete or truncated samples (a set
  with any is **never clean**). Counts are never subtracted across legs. F-MW reads `NOT-ADMITTED` if the fire is refused.
- **G0**, `IAI.Bench.TexCorruptCensus`, read-only, issued in the startup ExecCmds, i.e. on the first engine tick after load
  (the only console channel in a packaged unattended leg). F-SYN: each of the 77 fixture actors by name. F-MW: the visible
  set (no argument) plus seven named MainWorld actors. Every target's uv and normal final key is reported; a disagreement
  with the §R6.4 producer table (F-SYN) or with §R14.1 (F-MW: the rocks `not_fully_resident`; `SM_FloorBase` uv `APPLY`,
  normal `no_normal_map`; `SM_GenericPlane` `no_eligible_slot`) is a **finding**, never a pass or a stop.

### 4.9 The owed in-engine proofs (082-06b / 082-06c)

| proof | where it is read | CONFIRMED iff | class |
|---|---|---|---|
| P2-1 NoApply predicate | every m53 capture leg | APPLIED lines read `installed` (noapply 0) or `slot_not_installed` (noapply 1); NoApply 2 lines read `no_expected_set`; every labelled row's `texcorrupt.condition_detail` is the same value and `condition_held` is true exactly when it reads `installed` (ForeignReplace legs excepted) | PROOF |
| P2-2 collateral set | G-COLL F-SYN apply sample | it contains `T_TC_UC2` (a 64-pixel tile's own texture, far below every injection-selection filter), does not contain the target's `T_TC_UC1`, and reads `null_slots ≥ 1` (`TC_SlotEmpty` measured through the engine default material) | PROOF |
| P2-3 subtype | every m53 capture leg | every `annotation.json` event of the id has `anomaly_subtype` = the leg's mode (`identity`, `identity_redraw`, `tile_probe`), including the FinishRun-truncated event and each leg's last event (whose frames complete after its revert), and labels' `texcorrupt.mode` agrees | PROOF |
| P2-5 rollback accounting | G4 FS rows, especially `FS2O` (created-then-rejected) | as §4.6 | Q (G4) |
| P2-6 NoApply 2 watches | G4 DES2 | as §4.6 | Q (G4) |
| F+2 balance (P3-3) | G4 FS rows | the rollback ledger reading at F+2 is `live=0 pending=0 balance=balanced`; the rollback line itself may show pending > 0 | Q (G4) |

A NOT-CONFIRMED proof does not stop the window. It is reported as NEEDS-DECISION.

## 5. Stop-list mapping (§R13.1)

| # | plan row | read here by | state before 082-07b |
|---|---|---|---|
| 1 | G10 | G-BIND (1) re-reads 082-06c's G10 record for the staged exe | G10 passed in 082-06c |
| 2 | G-BIND | G-BIND (1), REASON T4, G-BIND (3) | — |
| 3 | G-ID | G-ID rows (incl. MASKED, the alpha half) and the judged F-MW row | — |
| 4 | G-ID-M | G-ID-M Q rows | — |
| 5 | mip tripwire | TRIP on every leg | — |
| 6 | G-RD | G-RD rows | — |
| 7 | G3 | G3 rows and G3-COUNT | — |
| 8 | G4 | G4 rows | — |
| 9 | G-REASON / G6 (Δ1) | G-REASON rows, G6 | — |
| 10 | G11 | G11 on every leg | — |
| 11 | G-COOK | not re-run | passed in 082-06d |
| 12 | offline wrong-copy proofs | not re-run; the manifest (`all_relied_on` true) is hash-locked | passed in 082-06 |
| 13 | luma | LUMA | — |
| 14 | fixture tool | not re-run | passed in 082-06d |

## 6. Predictions (082-07b)

- FIXTURE-NAMES, LUMA, G11, TRIP (0 everywhere), BAND F-SYN (region 0, whole frame 0): **PASS / VALID**.
- G-ID: **every row PASS**, max d ≤ 2. EXACT is not predicted on the block-compressed rows (the plan rejects exact-only).
  Every wrong copy reads close to its offline minimum (≥ 16 on every measured frame).
- G-ID-M: **PASS** on every Q row; the (c) `mipgen` readings are not predicted.
- G-RD, G3, G6 and every G-REASON row: **PASS**. G3 reads 0 differing pixels on F-SYN.
- G4: every exercised row **PASS**. The GC row reads **PASS or UNEXERCISED** (whether a purge lands inside the event is not
  predicted). Level change and re-created component: **UNEXERCISED**, declared.
- G-COLL F-SYN: P2-2 **CONFIRMED**, no deficit (every fixture texture is never-streamed). F-MW: a reading.
- F-MW `SM_FloorBase`: **admitted** for uv (§R14.1). Whether its G-ID row is judged depends on the F-MW control pair; a
  MainWorld unattended scene with moving platforms may make it `INVALID-ARBITER`. No prediction is made on that.
- P2-1, P2-3: **CONFIRMED**.

## 7. Named risks (what could stop 082-07b for a reason other than the product)

1. **A47 on F-SYN.** The input lock is refused off `CB_GateLevel`/`L_ShooterGym`, so F-SYN has no rotation guard. Pose-only
   invalidity has its own budget of 6. The discard rate is not known and is reported.
2. **The live runner path** (`082-07-run.ps1` launching the game) cannot be exercised bench-free. The stub, the parser and
   the dry run cover the rest; the first leg is its proof.
3. **The target mask for m53 events** is assumed to exist (m43/m44 arm masks for every labelled object fire). If it does not,
   the first G-ID row reads "0 measured events" and stops as a FAIL. The reading would name the cause.
4. **G0 at the first tick** reads load-time residency on MainWorld's streamed textures; the APPLY-time trees in the F-MW legs
   are the settled reading.
5. **The Bot name** is resolved at run time; if the listing does not give exactly one, the banked name is used and a
   no-match makes that leg invalid.

## 8. Bench time

194 legs (185 F-SYN, 8 F-MW, 1 F-GATE; 187 capture, 2 census, 2 cancel, 3 graceful-close) and 548 gate rows. Estimated
**~2 h of legs** (~38 s per standard leg, launch to bank) plus quiet and person-present waits and any re-attempts. 082-07b is
resume-safe and may take more than one window.

## 9. Named deviations from the plan's wording (fixed here, before any leg)

1. **Forced GC** is the engine's own purge timer, not `obj gc`: no mid-event console channel exists packaged. UNEXERCISED if
   no pass lands inside the event.
2. **Level change** is UNEXERCISED. World end (a graceful close) exercises the same m53 callback.
3. **Re-created component → swept** is UNEXERCISED: S1 has no such lever.
4. **G-ID's region** is the union of the two legs' target masks, per frame.
5. **A wrong copy must read ≥ 16 on every measured frame**, not only as a per-leg maximum (stricter). G-RD's noclear is read
   on the event's last frame.
6. **Chain (d) rows run through both ids**; the plan does not name one.
7. **Arbiter precondition:** an F-SYN control pair above 2 stops as NEEDS-DECISION; on F-MW it only decides whether the F-MW
   rows are judged.
8. **G3's pixel band** is the matched null's own post-revert/pre-onset comparison. The plan's value (0 differing) is
   predicted.
9. **G0 runs at the first engine tick.**
10. **Pose rules** (§2): rotation `(0,0,0)` and a campaign reference for F-SYN pixel legs; a separate 6-attempt pose budget.
11. **P2-1, P2-2 and P2-3 are PROOF readings**, not stop rows; P2-5, P2-6 and the F+2 balance are inside G4.
12. **`IAI.Capture.Mask 1` and `IAI.Capture.TargetMask 1`** are set explicitly, so the region does not depend on ini state.

## 10. What this file does not claim

No leg has run. No pixel, residency, cost or yield number exists for m53 on any fixture. The harness proofs
(`_reviews/082-07-evidence/prep/`) show that each evaluator passes a synthetic good case and fails a synthetic bad case built
from the fixture's own images and the offline-proven faults. They say nothing about the product.

---

## AMENDMENT 1 — 082-07d, 2026-09-27, declared before the resume (082-07e)

**Authority:** `_reviews/082-07c-chat-ruling-s1-legs-red.md` (items 1–5) and chat's ruling on the two 082-07c notes (U-N1
re-runs on the new build; 082-07b frames of the four normal tiles are not compared across builds). This amendment governs over
§§1–10 wherever they differ. The harness (`_reviews/082-07-*`) is re-locked by a boundary issued **after** the commit that
carries this text; the 082-07a boundary is kept beside it as `boundary-082-07a.json`. **No tolerance, budget or class changes,
except the one row named in A1.8**, and that change is flagged for chat.

### A1.1 The build under test

082-07c re-cooked and staged: exe `2FCDF059` (unchanged), `StackOBot-Windows.utoc` `30FE0FDE`, `.ucas` `11231356`, `.pak`
`FD766B7B`, `global.utoc` `462B8AC6`, `global.ucas` `BB05CF99` (archive `_binary_baselines\m53-s1-normalfix-cook-30FE0FDE\`).
The only content change is the four normal-readout materials (`M_TC_UN1`, `M_TC_NN1`, `M_TC_ChainD`, `M_TC_NormalEmis`): Default
Lit, with `Tex.RGB` into `MP_Normal` as well as the existing Emissive readout (CaptureBench `41ef8b9`). §1's build line is
superseded.

### A1.2 The normal rows' self-contradiction, resolved

§4.2 (N-N1), §4.3 (chain (d) through normal_corruption at four geometries) and §4.6 (G4 normal on `TC_NN1`, and G4 TWO) need
normal_corruption to **apply**, while §4.7 lists `normal_unconnected` among the S2 producers that S1 does not exercise. On the
082-07b build both could not hold: all 7 normal-map targets read `normal_unconnected` at V1, because the readouts fed only
Emissive. The fixture now connects Normal, so:

- every normal-family Q row needs its target **admitted** (A1.4 enforces it before launch);
- `normal_unconnected` stays **UNEXERCISED as an S1 gate reason**: no S1 row produces it on the fixed fixture;
- its 082-07b readings (7 census targets, and every fire on `ID_NN1` / `NULL_NN1`, at V1 in a cooked build) are history of the
  superseded build, not an S1 result.

### A1.3 G0 covers the normal family on F-SYN

G0 F-SYN now compares both families on all 77 fixture targets. The declared normal-family table:

| targets | expected final | why |
|---|---|---|
| `TC_UN1`, `TC_NN1`, `TC_ChainD_Mag` / `_Grazing` / `_Far` / `_Transition`, `TC_NormalEmis` (the 7 normal-map targets) | `APPLY` | the fixture fix; 082-07c's offline admission check |
| `TC_NoMesh`, `TC_SlotEmpty`, `TC_Translucent`, `TC_Skeletal`, `TC_Nanite` | the same final as uv (`no_mesh` E7, `no_eligible_slot`, `no_eligible_slot`, `default_material_path`, `nanite_override`) | refused at a step both families share, before the family split |
| every other target | `no_normal_map` (V1) | `TexCorruptTree.cpp:566-570`: A2 when a slot has no normal-map binding |

The 70 targets outside the first row did not change between builds, and the 082-07b census read exactly this table for them.
A disagreement is a **finding** (G0 stays class D), never a stop.

### A1.4 The Q-row admission pre-check (new; a harness stop)

- **When:** before the first leg of a Q row is launched.
- **Rule:** the harness reads the accepted `G0_FSYN` census of the build under test. Every (target, family) the row needs must
  read `APPLY`.
- **Rows with a need:** 116 in all, all F-SYN.
  - G-ID, G-ID-M, G3, G-RD and G-BIND (3).
  - G4 for both ids. G4 TWO needs `TC_UC1` uv **and** `TC_NN1` normal.
  - G-REASON A1, whose expectation is `APPLY`.
- **Otherwise:** an `ADMIT` row reading **FIXTURE-CANNOT-EXERCISE**, stop code **12**.
  - It is a **harness stop, not a feature FAIL**. The census final is printed, so the cause (fixture or feature) can be ruled.
  - No leg of the row is launched, and every later call stays stopped until a ruling.
  - With no accepted census, the row reads **NO-CENSUS**, code 3.
- **Not pre-checked:**
  - refusal rows (G-REASON, G6), because their own evaluation is the test;
  - D, PRE and PROOF rows;
  - every **F-MW** row. G0 F-MW is read at the first engine tick (§7 risk 4), so the apply-time tree in the F-MW legs remains
    the reading there (A1.8).

### A1.5 G3 strengthened (replaces the pixel clause of §4.5; §9 item 8 is withdrawn)

- **Rule:** for each event, take the first post-revert frame (`end+1`) of the applied leg and the matched null's frame at the
  **same `session_index`**. The event passes when max |d| ≤ 2 **inside the union of both legs' masks** over the event's labelled
  frames, **and** max |d| ≤ 2 **over the whole frame**.
- **Unjudged events:** an event with no post-revert frame (cut by the frame cap) or with no mask is not judged.
- **Minimum:** a row needs ≥ 1 judged event. G3-COUNT (≥ 3 events per id) is unchanged. The log clause is unchanged.
- **The 082-07a comparison stays as a diagnostic:** `start−1` against `end+1` within each leg, reported and never judged.
- **F-MW:** the pixel clause is judged when the F-MW control pair's whole-frame max is ≤ 2 (was: = 0). That is the same-index
  arbiter this rule needs. Otherwise the row reads `INVALID-ARBITER`, a reading.

### A1.6 Cause read for the 363,001-pixel pre/post change

- **Named: UE 5.1's tonemapper grain quantization.**
  - `r.Tonemapper.GrainQuantization` is on by default, and the bench recipe does not set it.
  - It adds a ±½-LSB dither whose seed is `Halton(ViewState frame index mod 8)`: `PostProcessTonemap.cpp:637-643` and
    `PostProcessTonemap.h`, `GrainRandomFromFrame`.
- **Measured on the 082-07b bank:**
  - Max |d| is **1** on every one of the 363,001 pixels.
  - Every event's `start−1` and `end+1` are **11 engine frames apart** (`frame_index`), and 11 mod 8 = 3. So every event, in
    both legs, compares the same two dither phases, which is why the count is the same everywhere.
  - Every pair of frames whose engine frames differ by a non-multiple of 8 differs over 362k–364k pixels.
  - From `session_index` 23 on, every pair whose engine frames differ by a **multiple of 8 is bit-identical**.
  - Applied against null at the same index reads **0 over the whole frame on all 100 frames** of the legs checked.
- **A second, small element: cause not established.** In the first ~22 frames of every leg, the reason-producer tile row
  (y ≈ 255–316) changes by up to 3–4 levels over up to ~316 pixels, and it settles before the second event.
- **§6's "G3 reads 0 differing pixels on F-SYN" was wrong** for the old comparison. It is replaced by "applied against null at
  the same index reads 0".

### A1.7 The 11 banked UV G3 rows under A1.5

U-C1 … U-C5 and U-D1 … U-D6 were re-evaluated from the bank, read-only:

- **all PASS**, with region max **0** and whole-frame max **0** on 7 judged events each (the 8th event of each leg is cut by the
  frame cap);
- the diagnostic reads 363,001 pixels at max 1 in both legs;
- the old-rule rows are superseded, not deleted, and the amended rows are written to `gates.json` by the locked lib after the
  boundary.

U-N1 is not among them: it re-runs (A1.9). Its banked 082-07b result is history, superseded and not relied on.

### A1.8 The MainWorld yield reading (a finding) and REASON A2

**G0 F-MW (082-07b, first tick):**

- the floor actor (`RoomBuilderSquare_C_…1080865874`) reads **`not_fully_resident` in both families** (§R14.1 predicted uv
  `APPLY`, normal `no_normal_map`);
- the two rocks read **`texture_not_parameter`** (predicted `not_fully_resident`);
- `BP_SplineSpawn` reads `no_eligible_slot`, as predicted;
- the Bot reads `host_mid:override:mid`, consistent with S3O.

This is recorded as a finding for S3 and the owner: route B's strict admission yields zero on StackOBot at the bench camera
(082-07c ruling). The F-MW rows stay diagnostic. G-ID-FMW and G3 FMW read `NOT-ADMITTED` when the apply-time fire is refused
(§4.2); when it applies, they are judged as declared.

**REASON A2's premise is refuted (this is the one class change here, flagged for chat).**

- **Source:** in `TexCorruptTree.cpp:566-574`, `no_normal_map` (A2, rank 31) is decided only for a slot with **no** normal-map
  binding. A per-binding failure such as `not_fully_resident` ranks 20 and wins `PickEarliestSlot`.
- **Reading:** a MainWorld actor with no normal map reads uv `not_fully_resident` but normal `no_normal_map`.
- **So:** the floor's normal-family `not_fully_resident` means it carries a **connected normal map**, and `no_normal_map` cannot
  be produced on it at any residency.
- **Declared now:** G-REASON A2 reads **FIXTURE-CANNOT-EXERCISE, class D**. Its leg still runs and banks the apply-time tree.
- **Consequence:** `no_normal_map` has **no Q producer in S1**; it is read only by the G0 F-SYN census (64 targets, D).

### A1.9 The resume plan (082-07e)

**The supersede record `_reviews/082-07-evidence/run/supersede.json`** is append-only and hash-locked by the boundary.

- **Entry 1 (the G3 amendment):** the 11 old-rule G3 rows.
- **Entry 2 (the new build):**
  - **7 attempts:** `G0_FSYN_A1`, the four U-N1 legs (`ID_UN1`, `NULL_UN1`, `WC_UN1_NORMAL`, `WC_UN1_CHANSWAP`), `ID_NN1_A1` and
    `NULL_NN1_A1`.
  - **Their 17 recorded gate rows**, including the halt `G3 N-N1 FAIL`. TRIP and G11 of `NULL_NN1` were never evaluated in
    082-07b, because the halt stopped evaluation first.
  - **Their 7 bank aliases**, renamed `M53S1_<leg>__SUPERSEDED_082-07d` and still hardlinked to the A1 attempts.

**Nothing is deleted.** The ledger carries a `RULING` row that points at the record. The harness reads only the live views: a
superseded attempt or row is history, attempt numbering continues (a re-run leg is `_A2`), and a superseded halt no longer
stops.

**Order in 082-07e:**

1. `G0_FSYN`, a census of the build under test. The admission pre-check reads it.
2. The U-N1 re-run.
3. N-N1 and every later leg, in the declared order.

Every other accepted leg stands, and the resume uses a new budget tag.

### A1.10 Across builds

- **The four normal tiles are now lit**, so no 082-07b frame of those tiles is comparable with the new build. G-ID and G3 compare
  applied against null **within one build**, so they are unaffected.
- **BAND F-SYN and LUMA were read on the 082-07b build and stand**, on the unchanged `TC_UC1` tile. The new build's own
  null-against-null band is not re-measured. The re-run rows' G-ID is itself a same-build applied-against-null reading, and a
  reading above 2 would stop as FAIL.

### A1.11 Harness changes carried by the re-issued boundary

- **The window:** `082-07-window.py` calls `L.budget_state()['left_s']`. It is byte-identical to the ratified 082-07b driver,
  which is retired (renamed `…py.RETIRED-082-07d`).
- **The alias error:** an alias error while accepting an attempt now stops with code 6 and records the attempt `INVALID-BANK`.
  Before, it escaped as an uncaught exception, which would have skipped the postflight.
- **A stub launch mode** for the end-to-end proof of the real window. It is refused at import when the run root is the live
  evidence dir, and it writes new attempts only into its scratch bank.

### A1.12 Predictions for 082-07e

- **G0 F-SYN:** 0 findings in both families; the 7 normal-map targets read `APPLY`.
- **FIXTURE-NAMES:** PASS.
- **Every ADMIT row:** ADMITTED.
- **U-N1 and N-N1:** G-ID PASS (applied against null max ≤ 2; 0 expected, as on every UV row), with every assigned wrong copy
  ≥ 16 on every measured frame. No numeric wrong-copy value is predicted on the lit tiles.
- **G3 (A1.5):** 0 at the same index on every judged event.
- **Every other §6 prediction stands**, except A2, which is FIXTURE-CANNOT-EXERCISE (A1.8).
