# 089-04 — the m53 re-bench (prepared by 089-02b; the bench runs as 089-04)

Governing: `_reviews\night-0930-chat-ruling.md` items 2, 7, 9, 10; `_reviews\089-01-chat-ruling-adjudication.md` ruling 4;
`_reviews\089-03-chat-ruling-rebench.md`. Built from the 085-04 harness (no `085-04-*` byte changed); journal
`docs/sessions/2026-09-30-089-02b-m53-rebench-prep.md` on `feat/m53-uv-normal-corruption`. Evidence `_reviews\089-04-evidence\`.
⛔ Nothing in 089-02b launched the game.

## 1. The pairing under test: DC2
**exe `6B671987`** (the archived DC exe, the build of `270ed32`) **+ utoc `45E2FA86` / ucas `8CA6FF5C` / pak `B35DBE96` / globals
`462B8AC6` / `BB05CF99`**, archived `_binary_baselines\m53dc2-cook-45E2FA86\` (6/6 re-hashed), pinned by `089-04-dc2pin.json`. The
only input change from DC (`F76F1119`) is the 089-02b fixture fix (section 3.3). **Restore target: m53 S1** (`2FCDF059` +
`20DA6F98`), read from its immutable archive.

## 2. What changed from 085-04
| item | what | where |
|---|---|---|
| **E1** | the frozen 089-03 evaluator `089-03-eval.py` (`8E48E883…`), loaded only after its hash and its six dependencies match `089-03-evidence\freeze.json`; E9.c.CACHE redirected into this run | common `e1_pin_problems`, eval header |
| **G403** | `final` parsed as `head[:detail]`; host tiers view-128 → whole-world-128 framed by L2 → the declared raise; **L2's capability read from the build source** (it cannot frame MainWorld); per-family candidate caps; identity host probes; H1 / H2 and cap per family in the lock | eval `parse_final`, `census_bench`, `l2_capability`, `g403_candidates`, `probe_reading`, `hosts_from_probes`; legs `P_HOST_FIRE_*`; window `write_candidates`, `build_lock` |
| **aligned null (G406)** | MainWorld identity nulls are 400 frames (ON 300) and run right after their ON legs; rows at the same game time: coverage, the declared wrong-object rule, the AA-off arbiter, an adjudication reading on any E1 FAIL | legs `MW_*`, eval `gt_*`, `wrong_object_gt`, `arbiter_gt` |
| **P-ORC / G-MODE** | `ShowFlag.Tonemapper 0` on every oracle leg (+ `ShowFlag.Lighting 0` on the normal ones), read back; the reader `089-04-oracle.py` (geometry placement of at most a quarter of the cells, the null-as-modes crash guard); the fixture's normal readout linearised | legs, lib `readback_errors`, `089-04-oracle.py`, fixture |
| **G-BIND** | the four BIND legs carry the AA-off arbiter (S1's recipe); `bind_gate` returns INVALID-RECIPE on any non-zero arbiter echo | legs, eval `bind_gate` |
| **MIXED** | fixture: TC_Mixed's component override emptied; predictions re-declared (N `partial_footprint:1/2:no_normal_map`, uv APPLIED) | fixture, eval `reason_gate` |
| **G-LEVER-OFF** | DISABLED echo + no ENABLED + no lever effect or echo + ListAnomalies with no twin + the spot commands issued | eval `lever_off_gate` |
| **G-CENSUS** | one census command per engine frame (DEFER 90 + i); grammar accepts the product's `reasons=-`; reason counts sum to `refused` | lib `build_cmds`, eval `GRAMMAR`, `census_consistency` |
| **arms** | CD3 / RD3 = +1, CD5 / RD5 = +3 (captured frames), both AA; proven = FAIL + expected edge modal +k on ≥ 4 judged + other edge modal 0 (085-04's classifier read a histogram string as a dict and could never say proven) | legs, eval `arm_canfail` |
| **validity read-backs** | arbiter echo 0/0/0/0 on arbiter legs; each declared show flag; the declared cap on every m53 decision line | lib `readback_errors` |
| **third reading** | the office kit (`089-03-kit.py office`, blob `165d55f6`) on every judged m53 leg | eval `kit_pool` |
| **evaluation** | per-leg subprocess pool (6 workers) | window `eval_pool`, mode `evalleg` |
| **freeze** | `089-04-window.py freeze` → `089-04-evidence\freeze.json`; live and dry refuse on any changed byte | window |
| **bank** | `D:\IA_BankOverflow`, prefix `M53DC2_`; dry roots under `089-04-evidence\dry-*` (same drive, hardlinked stand-ins from the night's `M53DC_*`) | common |

## 3. Causes and fixes (offline evidence in `089-04-evidence\orc\`, `bind\`, `fixture\`, `cook\`)
### 3.1 G403 and the hosts
The night's rule compared `final == 'over_budget'` against `over_budget:need_…`. Fixed and proven on the night's logs (SM_Ramp2
need 213,949,084 → cap 268,435,456; RoomBuilderSquare 219,541,488; the old rule names nothing). **L2 cannot frame MainWorld**
(`AnomalyBenchModule.cpp:105-114` map_not_allowed, fixed pose :475-476), so the 26 uv / 9 normal whole-world 128 MiB hosts are
unreachable; per the ruling the declared raise applies (uv 256 MiB); **normal has no host at any cap** (its view refusals are
partial_footprint 3 and texture_not_parameter 2).

### 3.2 P-ORC (uv16 15/16, uv64 60/64, normal separation 13.5)
The filmic tonemapper mixes channels (texel red 100 displays 67 / 79 / 108 depending on G and B), so uv16 cell 13 (on screen,
flat) displayed (108,197,196) against a predicted (83,199,192); vignette, dither and bloom make the interior spread 4; the normal
oracle is lit and its linear readout is compressed. Offline: the reader finds 16/16 and 64/64 on the night's frames (still
FAIL on interior, which only the show flag removes); its selftest 39/39.

### 3.3 G-BIND and MIXED
G-BIND: the DC BIND legs lost S1's arbiter (their echo read TSR 4, Lumen GI 1, Lumen reflections 1); the night's four legs read
INVALID-RECIPE, S1's BIND bank reads OK. MIXED: slot 0 carried a component override `BasicShapeMaterial` (len 1) and resolved to
a textureless engine material; slot 1 (colour-only) already had its colour texture. `dcfix` (CaptureBench `e64d6f9`) emptied the
override and linearised M_TC_OracleN's readout: D: content diff = `CB_TexCorruptLevel.umap` + `M_TC_OracleN.uasset`; read back
fresh (`fixture_out\texcorrupt_fixture_dcfixverify.json`).

### 3.4 The cook
912 packages, 0 errors; the cook changed no D: content (M1 == M2). The environment refuses to create any file named
`dbghelp.dll` since about 03:00 (G418), so staging failed after the pak / IoStore step (exit 0); the container was taken from
`E:\IA_BuildCache\_r53_stage_rb3`. Map gate 5/5 with the inverted probe MISSING; container diff vs `F76F1119` = the fixture map,
M_TC_OracleN, G371's set; 3 of 1,084 unnamed chunks replaced; pak AssetRegistry only. The cook's `-build` relinked the host
exe to `ECC3F085` (not used).

## 4. Proofs (`089-04-gateselftest.py` → `089-04-evidence\gateselftest.json`)
**68 / 68** on the frozen bytes: E1 pin both ways; G403 on the night logs both ways + the tier order; L2 from source; host
probes both ways; G-LEVER-OFF on the night logs + 8 doctored FAILs; G-CENSUS one-per-frame, the night host log FAILs the cold
check and the same log split one frame PASSES; grammar and consistency both ways; the arm classifier (8 synthetic rows) and the
frozen E1 on the night's CD3 / RD3 AA-off (+1 proven, +3 not); MIXED both ways; BIND recipe (night INVALID-RECIPE, S1 OK) and
the read-backs; the GT null (exact alignment, coverage both ways, a 0.25-frame shift, a sparse null's GAPs, the arbiter READS-WRONG
and BELOW-16, the wrong-object rule PASS and an 80×80 painted blob FAIL); the oracle selftest and P-ORC on the night frames; the
leg invariants. The unchanged `085-04-m53rows.py` (`99B157A83A58EDA6`) keeps its recorded 98/98.

## 5. Legs and predictions
| # | leg | group / role | map | target | frames | levers beyond the base (cfg) | prediction |
|---|---|---|---|---|---|---|---|
| 1 | P_HOST_CEN | PREMISE / CENSUS | MainWorld | - | census | IAI.TexCorrupt.Census, IAI.TexCorrupt.Census all (no flag) (cfg `2 4 8 14 0`) | census_readonly_check --log --cold PASS (the two blocks land in consecutive engine frames); view: uv 0 eligible (over_budget 2, partial_footprint 1, texture_not_parameter 2), normal 0; all: uv 26, normal 9 (the night counts; MainWorld is not re-cooked) |
| 2 | P_HOST_BENCH | PREMISE / CENSUS | MainWorld | - | census | IAI.TexCorrupt.Census, IAI.Bench.TexCorruptCensus (cfg `2 4 8 14 0`) | uv budget-only view hosts SM_Ramp2 (need 213,949,084) and RoomBuilderSquare (219,541,488); no normal host at any cap; count join exact |
| 3 | P_ORC_UV | PREMISE / NULL | CB_TexCorruptLevel | TC_OracleUV16 | 120 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, ShowFlag.Tonemapper 0, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | P-ORC uv16 16/16 and uv64 64/64 cells located, interior <= 2, separation >= 32, residual <= 0.5 px, 1:1; ShowFlag.Tonemapper read back 0 (089-02b: the filmic curve mixed channels and added vignette / bloom / dither) |
| 4 | P_ORC_N | PREMISE / NULL | CB_TexCorruptLevel | TC_OracleN | 120 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, ShowFlag.Tonemapper 0, ShowFlag.Lighting 0, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | P-ORC n PASS: 16 cells, separation >= 32, interior <= 2, residual <= 0.5 px, pattern; ShowFlag.Tonemapper and ShowFlag.Lighting read back 0 and the readout linearised (089-02b dcfix) |
| 5 | P_HOST_FIRE_1U | PREMISE_FIRE / FIRE | MainWorld | @C1U | 120 | @CCAP, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | uv: SM_Ramp2 APPLIED under 256 MiB (candidate order = required bytes ascending); RoomBuilderSquare may refuse not_fully_resident (the night P_FLOOR read 0/30); normal: NOT-RUN-PREMISE (no candidate) |
| 6 | P_HOST_FIRE_2U | PREMISE_FIRE / FIRE | MainWorld | @C2U | 120 | @CCAP, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | uv: SM_Ramp2 APPLIED under 256 MiB (candidate order = required bytes ascending); RoomBuilderSquare may refuse not_fully_resident (the night P_FLOOR read 0/30); normal: NOT-RUN-PREMISE (no candidate) |
| 7 | P_HOST_FIRE_1N | PREMISE_FIRE / FIRE | MainWorld | @C1N | 120 | @CCAP, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | uv: SM_Ramp2 APPLIED under 256 MiB (candidate order = required bytes ascending); RoomBuilderSquare may refuse not_fully_resident (the night P_FLOOR read 0/30); normal: NOT-RUN-PREMISE (no candidate) |
| 8 | P_HOST_FIRE_2N | PREMISE_FIRE / FIRE | MainWorld | @C2N | 120 | @CCAP, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | uv: SM_Ramp2 APPLIED under 256 MiB (candidate order = required bytes ascending); RoomBuilderSquare may refuse not_fully_resident (the night P_FLOOR read 0/30); normal: NOT-RUN-PREMISE (no candidate) |
| 9 | LEVER_OFF | LEVER / LEVER_OFF | CB_TexCorruptLevel | TC_UC1 tile | 60 | IAI.Bench.SynthTickOrder 1, IAI.Bench.TexCorruptIdentity 1, IAI.Bench.DestroyTarget StaticMeshActor_12, IAI.ListAnomalies (no flag) (cfg `2 4 8 14 0`) | DISABLED echo (n masked, preset 0) and no ENABLED; none of the three levers echoes or acts (no "-> " set line, SYNTH TICK ORDER stays off, no DestroyTarget ARMED/DESTROYING, the fire reads mode_source=argument, StaticMeshActor_12 never destroyed); IAI.ListAnomalies printed and lists no null_effect / solid_swap |
| 10 | CENSUS_FSYN | CENSUS / CENSUS | CB_TexCorruptLevel | - | census | IAI.TexCorrupt.Census (no flag) (cfg `2 4 8 14 0`) | four count lines, grammar PASS, stats_unchanged=1, cold read PASS |
| 11 | CENSUS_FSYN_BENCH | CENSUS / CENSUS | CB_TexCorruptLevel | - | census | IAI.TexCorrupt.Census, IAI.Bench.TexCorruptCensus (cfg `2 4 8 14 0`) | count join exact; TC_Mixed uv APPLY, normal partial_footprint:1/2:no_normal_map |
| 12 | F_TILE_NAT | BM53F / SYNC | CB_TexCorruptLevel | TC_UC1 tile | 400 |  (cfg `2 4 8 14 0`) | PASS: E1 (frozen 089-03) t50 onset 0 / offset 0 on >= 16 judged, 0 NOT-MEASURABLE (bar predicted-invisible scramble pairs), 0 m53 transition flags, masks exact, label-rule exit 0 (the night: 17/17 +0/+0 per mode) |
| 13 | F_SCRAMBLE_NAT | BM53F / SYNC | CB_TexCorruptLevel | TC_UC1 scramble | 400 |  (cfg `2 4 8 14 0`) | PASS: E1 (frozen 089-03) t50 onset 0 / offset 0 on >= 16 judged, 0 NOT-MEASURABLE (bar predicted-invisible scramble pairs), 0 m53 transition flags, masks exact, label-rule exit 0 (the night: 17/17 +0/+0 per mode) |
| 14 | F_INVERT_NAT | BM53F / SYNC | CB_TexCorruptLevel | TC_NN1 invert | 400 |  (cfg `2 4 8 14 0`) | PASS: E1 (frozen 089-03) t50 onset 0 / offset 0 on >= 16 judged, 0 NOT-MEASURABLE (bar predicted-invisible scramble pairs), 0 m53 transition flags, masks exact, label-rule exit 0 (the night: 17/17 +0/+0 per mode) |
| 15 | F_GREEN_FLIP_NAT | BM53F / SYNC | CB_TexCorruptLevel | TC_NN1 green_flip | 400 |  (cfg `2 4 8 14 0`) | PASS: E1 (frozen 089-03) t50 onset 0 / offset 0 on >= 16 judged, 0 NOT-MEASURABLE (bar predicted-invisible scramble pairs), 0 m53 transition flags, masks exact, label-rule exit 0 (the night: 17/17 +0/+0 per mode) |
| 16 | MODE_TILE | GMODE / MODE | CB_TexCorruptLevel | TC_OracleUV16 tile | 200 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, ShowFlag.Tonemapper 0 (cfg `2 4 8 14 0`) | G-MODE-UV tile PASS on >= 4 counted events; wrong-mode negatives < 50 % |
| 17 | MODE_SCRAMBLE | GMODE / MODE | CB_TexCorruptLevel | TC_OracleUV64 scramble | 200 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, ShowFlag.Tonemapper 0 (cfg `2 4 8 14 0`) | G-MODE-UV scramble PASS; the logged pair equals the recomputation |
| 18 | MODE_UVNULL | GMODE / NULL | CB_TexCorruptLevel | TC_OracleUV16 | 200 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, ShowFlag.Tonemapper 0, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | null as each uv mode < 50 % |
| 19 | MODE_INVERT | GMODE / MODE | CB_TexCorruptLevel | TC_OracleN invert | 200 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, ShowFlag.Tonemapper 0, ShowFlag.Lighting 0 (cfg `2 4 8 14 0`) | G-MODE-N invert PASS |
| 20 | MODE_GREENFLIP | GMODE / MODE | CB_TexCorruptLevel | TC_OracleN green_flip | 200 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, ShowFlag.Tonemapper 0, ShowFlag.Lighting 0 (cfg `2 4 8 14 0`) | G-MODE-N green_flip PASS |
| 21 | MODE_NNULL | GMODE / NULL | CB_TexCorruptLevel | TC_OracleN | 200 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, ShowFlag.Tonemapper 0, ShowFlag.Lighting 0, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | null as each normal mode < 50 % |
| 22 | ARM_CD3_AAOFF | BM53ARM / CANFAIL | CB_TexCorruptLevel | TC_UC1 tile | 200 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, IAI.Bench.TexCorruptCommitDelay 3 (cfg `2 4 8 14 0`) | E1 (frozen) FAIL at t50 with the start edge modal +1 on >= 4 judged events and the end edge modal 0; E2 and the kit read the same shift (a reading) |
| 23 | ARM_CD3_AAON | BM53ARM / CANFAIL | CB_TexCorruptLevel | TC_UC1 tile | 200 | IAI.Bench.TexCorruptCommitDelay 3 (cfg `2 4 8 14 0`) | E1 (frozen) FAIL at t50 with the start edge modal +1 on >= 4 judged events and the end edge modal 0; E2 and the kit read the same shift (a reading) |
| 24 | ARM_CD5_AAOFF | BM53ARM / CANFAIL | CB_TexCorruptLevel | TC_UC1 tile | 200 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, IAI.Bench.TexCorruptCommitDelay 5 (cfg `2 4 8 14 0`) | E1 (frozen) FAIL at t50 with the start edge modal +3 on >= 4 judged events and the end edge modal 0; E2 and the kit read the same shift (a reading) |
| 25 | ARM_CD5_AAON | BM53ARM / CANFAIL | CB_TexCorruptLevel | TC_UC1 tile | 200 | IAI.Bench.TexCorruptCommitDelay 5 (cfg `2 4 8 14 0`) | E1 (frozen) FAIL at t50 with the start edge modal +3 on >= 4 judged events and the end edge modal 0; E2 and the kit read the same shift (a reading) |
| 26 | ARM_RD3_AAOFF | BM53ARM / CANFAIL | CB_TexCorruptLevel | TC_UC1 tile | 200 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, IAI.Bench.TexCorruptRestoreDelay 3 (cfg `2 4 8 14 0`) | E1 (frozen) FAIL at t50 with the end edge modal +1 on >= 4 judged events and the start edge modal 0; E2 and the kit read the same shift (a reading) |
| 27 | ARM_RD3_AAON | BM53ARM / CANFAIL | CB_TexCorruptLevel | TC_UC1 tile | 200 | IAI.Bench.TexCorruptRestoreDelay 3 (cfg `2 4 8 14 0`) | E1 (frozen) FAIL at t50 with the end edge modal +1 on >= 4 judged events and the start edge modal 0; E2 and the kit read the same shift (a reading) |
| 28 | ARM_RD5_AAOFF | BM53ARM / CANFAIL | CB_TexCorruptLevel | TC_UC1 tile | 200 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, IAI.Bench.TexCorruptRestoreDelay 5 (cfg `2 4 8 14 0`) | E1 (frozen) FAIL at t50 with the end edge modal +3 on >= 4 judged events and the start edge modal 0; E2 and the kit read the same shift (a reading) |
| 29 | ARM_RD5_AAON | BM53ARM / CANFAIL | CB_TexCorruptLevel | TC_UC1 tile | 200 | IAI.Bench.TexCorruptRestoreDelay 5 (cfg `2 4 8 14 0`) | E1 (frozen) FAIL at t50 with the end edge modal +3 on >= 4 judged events and the start edge modal 0; E2 and the kit read the same shift (a reading) |
| 30 | ARM_NOAPPLY1 | BM53ARM / CANFAIL | CB_TexCorruptLevel | TC_UC1 tile | 200 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, IAI.Bench.TexCorruptNoApply 1 (cfg `2 4 8 14 0`) | FAIL: every event NOT-MEASURABLE (7 of 7 at night) |
| 31 | REASON_MIXED_N | REASON / REASON | CB_TexCorruptLevel | TC_Mixed invert | 120 |  (cfg `2 4 8 14 0`) | RE-DECLARED before any run: every decision final=partial_footprint:1/2:no_normal_map (slot 0 qualified, slot 1 no_normal_map), 0 APPLIED |
| 32 | REASON_MIXED_UV | REASON / REASON | CB_TexCorruptLevel | TC_Mixed tile | 120 |  (cfg `2 4 8 14 0`) | RE-DECLARED before any run: APPLIED (both slots qualified), no partial_footprint line; label-sync PASS on its events (a reading at 120 frames) |
| 33 | BIND_TILE | BIND / TILE | CB_TexCorruptLevel | TC_Layer | 100 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, IAI.Bench.TexCorruptTileProbe 2 (cfg `2 16 8 4 0`) | G-BIND per-frame minimum PASS (every measured tile-probe frame changes the layer half) and far strip <= 2; recipe read-back 0/0/0/0 |
| 34 | BIND_TILENULL | BIND / TILENULL | CB_TexCorruptLevel | TC_Layer | 100 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, IAI.Bench.TexCorruptTileProbe 2, IAI.Bench.TexCorruptNoApply 1 (cfg `2 16 8 4 0`) | the NoApply null the halves are located from |
| 35 | BIND_ID | BIND / ID | CB_TexCorruptLevel | TC_Layer | 100 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, IAI.Bench.TexCorruptIdentity 1 (cfg `2 16 8 4 0`) | (S1 recipe, reading) |
| 36 | BIND_IDNULL | BIND / NULL | CB_TexCorruptLevel | TC_Layer | 100 | r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, IAI.Bench.TexCorruptIdentity 1, IAI.Bench.TexCorruptNoApply 1 (cfg `2 16 8 4 0`) | (S1 recipe, reading) |
| 37 | MW_H1U_TILE_NAT | BM53MW / SYNC | MainWorld | @H1U tile | 300 | @CAP (cfg `2 4 8 14 0`) | PASS on every judged edge (E1 frozen, t50); >= 8 judged per mode on H1 AA-on or the shortfall goes to chat; the GT-aligned null covers t(first label - 12) .. t(last label) + 2 s; no ON outside-blob absent from the null at the same game time; ND-5 NOT-MEASURABLE rate reported (no number predicted) |
| 38 | MW_H1U_SCRAMBLE_NAT | BM53MW / SYNC | MainWorld | @H1U scramble | 300 | @CAP (cfg `2 4 8 14 0`) | PASS on every judged edge (E1 frozen, t50); >= 8 judged per mode on H1 AA-on or the shortfall goes to chat; the GT-aligned null covers t(first label - 12) .. t(last label) + 2 s; no ON outside-blob absent from the null at the same game time; ND-5 NOT-MEASURABLE rate reported (no number predicted) |
| 39 | MW_H1U_NULL_NAT | BM53MW / NULL | MainWorld | @H1U | 400 | @CAP, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | mode_source=bench_lever lever=identity on every fire; label-rule exit 0; covers its ON legs |
| 40 | MW_H1U_TILE_NAT_AAOFF | BM53MW / AA_OFF | MainWorld | @H1U tile | 300 | @CAP, r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0 (cfg `2 4 8 14 0`) | PASS on every judged edge (E1 frozen, t50); >= 8 judged per mode on H1 AA-on or the shortfall goes to chat; the GT-aligned null covers t(first label - 12) .. t(last label) + 2 s; no ON outside-blob absent from the null at the same game time; ND-5 NOT-MEASURABLE rate reported (no number predicted); AA-off arbiter: every labelled frame reads >= 16 inside the mask against the null at the same game time |
| 41 | MW_H1U_NULL_NAT_AAOFF | BM53MW / NULL | MainWorld | @H1U | 400 | @CAP, r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | mode_source=bench_lever lever=identity on every fire; label-rule exit 0; covers its ON legs |
| 42 | MW_H2U_TILE_NAT | BM53MW / SYNC | MainWorld | @H2U tile | 300 | @CAP (cfg `2 4 8 14 0`) | PASS on every judged edge (E1 frozen, t50); >= 8 judged per mode on H1 AA-on or the shortfall goes to chat; the GT-aligned null covers t(first label - 12) .. t(last label) + 2 s; no ON outside-blob absent from the null at the same game time; ND-5 NOT-MEASURABLE rate reported (no number predicted) |
| 43 | MW_H2U_SCRAMBLE_NAT | BM53MW / SYNC | MainWorld | @H2U scramble | 300 | @CAP (cfg `2 4 8 14 0`) | PASS on every judged edge (E1 frozen, t50); >= 8 judged per mode on H1 AA-on or the shortfall goes to chat; the GT-aligned null covers t(first label - 12) .. t(last label) + 2 s; no ON outside-blob absent from the null at the same game time; ND-5 NOT-MEASURABLE rate reported (no number predicted) |
| 44 | MW_H2U_NULL_NAT | BM53MW / NULL | MainWorld | @H2U | 400 | @CAP, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | mode_source=bench_lever lever=identity on every fire; label-rule exit 0; covers its ON legs |
| 45 | MW_H1N_INVERT_NAT | BM53MW / SYNC | MainWorld | @H1N invert | 300 | @CAP (cfg `2 4 8 14 0`) | PASS on every judged edge (E1 frozen, t50); >= 8 judged per mode on H1 AA-on or the shortfall goes to chat; the GT-aligned null covers t(first label - 12) .. t(last label) + 2 s; no ON outside-blob absent from the null at the same game time; ND-5 NOT-MEASURABLE rate reported (no number predicted) |
| 46 | MW_H1N_GREEN_FLIP_NAT | BM53MW / SYNC | MainWorld | @H1N green_flip | 300 | @CAP (cfg `2 4 8 14 0`) | PASS on every judged edge (E1 frozen, t50); >= 8 judged per mode on H1 AA-on or the shortfall goes to chat; the GT-aligned null covers t(first label - 12) .. t(last label) + 2 s; no ON outside-blob absent from the null at the same game time; ND-5 NOT-MEASURABLE rate reported (no number predicted) |
| 47 | MW_H1N_NULL_NAT | BM53MW / NULL | MainWorld | @H1N | 400 | @CAP, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | mode_source=bench_lever lever=identity on every fire; label-rule exit 0; covers its ON legs |
| 48 | MW_H1N_INVERT_NAT_AAOFF | BM53MW / AA_OFF | MainWorld | @H1N invert | 300 | @CAP, r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0 (cfg `2 4 8 14 0`) | PASS on every judged edge (E1 frozen, t50); >= 8 judged per mode on H1 AA-on or the shortfall goes to chat; the GT-aligned null covers t(first label - 12) .. t(last label) + 2 s; no ON outside-blob absent from the null at the same game time; ND-5 NOT-MEASURABLE rate reported (no number predicted); AA-off arbiter: every labelled frame reads >= 16 inside the mask against the null at the same game time |
| 49 | MW_H1N_NULL_NAT_AAOFF | BM53MW / NULL | MainWorld | @H1N | 400 | @CAP, r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0, r.ReflectionMethod 0, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | mode_source=bench_lever lever=identity on every fire; label-rule exit 0; covers its ON legs |
| 50 | MW_H2N_INVERT_NAT | BM53MW / SYNC | MainWorld | @H2N invert | 300 | @CAP (cfg `2 4 8 14 0`) | PASS on every judged edge (E1 frozen, t50); >= 8 judged per mode on H1 AA-on or the shortfall goes to chat; the GT-aligned null covers t(first label - 12) .. t(last label) + 2 s; no ON outside-blob absent from the null at the same game time; ND-5 NOT-MEASURABLE rate reported (no number predicted) |
| 51 | MW_H2N_GREEN_FLIP_NAT | BM53MW / SYNC | MainWorld | @H2N green_flip | 300 | @CAP (cfg `2 4 8 14 0`) | PASS on every judged edge (E1 frozen, t50); >= 8 judged per mode on H1 AA-on or the shortfall goes to chat; the GT-aligned null covers t(first label - 12) .. t(last label) + 2 s; no ON outside-blob absent from the null at the same game time; ND-5 NOT-MEASURABLE rate reported (no number predicted) |
| 52 | MW_H2N_NULL_NAT | BM53MW / NULL | MainWorld | @H2N | 400 | @CAP, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | mode_source=bench_lever lever=identity on every fire; label-rule exit 0; covers its ON legs |
| 53 | COST_DISCARD | COST / COST | MainWorld | @H1U | 300 | @CAP, IAI.Capture.Pace 0, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | declared discard, run first (G186) |
| 54 | COST_A1 | COST / COST | MainWorld | @H1U | 300 | @CAP, IAI.Capture.Pace 0, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) |  |
| 55 | COST_B1 | COST / COST | MainWorld | @H1U tile | 300 | @CAP, IAI.Capture.Pace 0 (cfg `2 4 8 14 0`) |  |
| 56 | COST_B2 | COST / COST | MainWorld | @H1U tile | 300 | @CAP, IAI.Capture.Pace 0 (cfg `2 4 8 14 0`) |  |
| 57 | COST_A2 | COST / COST | MainWorld | @H1U | 300 | @CAP, IAI.Capture.Pace 0, IAI.Bench.TexCorruptIdentity 1 (cfg `2 4 8 14 0`) | a reading: per fire apply_ms / revert_ms, per frame GPU / GT from TEXCORRUPT-COST; "below the resolution of this instrument" wherever the A spread exceeds B - A; no threshold |

Every capture leg additionally: `verify_capture.py --label-rule` exit 0; `labelled_membership` PASS; presence defect 0; its
read-backs valid; the office kit as a third reading on every judged m53 leg.

## 6. Invocation (when the PC is idle)
```
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\089-04-window.py night --commit   # premise -> host probes -> lock (committed) -> gates -> restore
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\089-04-window.py premise           # premise + host probes only (DC2 stays staged)
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\089-04-window.py lock --commit     # commit the lock doc on the feat branch
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\089-04-window.py live              # gates (refuses without a committed lock)
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\089-04-window.py gates             # re-evaluate accepted legs
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\089-04-window.py restore           # manual G355 restore of m53 S1
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\089-04-window.py dry               # stubbed, the night's banks as stand-ins
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\089-04-gateselftest.py [--fast]
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\089-04-window.py freeze            # only after a deliberate harness change (then selftest + dry)
```
`--only GROUP[,GROUP]` restricts. A second run resumes (accepted legs skipped). Dry-only: `IAI_R894_INJECT_LOCK_S=<s>` holds the
first staged file during the restore (G355).

## 7. Sequence and safety
1. Preflight (exit 10 on any problem): every frozen byte equals `freeze.json`; the E1 pin; DC2 and M53 archives hold their six
   pins and DC2's container lists the five maps; DC2's exe is `6B671987`; feat == origin (or origin + the lock commit only) and
   contains `270ed32`; **L2's capability unchanged**; the main checkout is `m51` `53bf725`; the pinned tool blobs; `m53_glue_check`
   PASS; the gate selftest all-pass; a known staged set; E: ≥ 5 GB, D: ≥ 40 GB; no engine / game / UBT process of ours.
2. Stage DC2 (only differing files, receipt `receipt-01-dc2.json`).
3. PREMISE (census, host census, P-ORC), then `host-candidates.json`, then the host probes, then the lock, committed on the
   feat branch (`docs/predictions/2026-09-30-m53-rebench-lock.md`) **before any gate leg**; `live` refuses without it.
4. Gate legs in the order LEVER, CENSUS, BM53F, GMODE, BM53ARM, REASON, BIND, BM53MW (each null right after its ON legs), COST.
   Person gate (60 s idle) and quiet check before each attempt; nothing is killed except the runner's own PID.
5. G355 restore of m53 S1 in a `finally`. Part 0 is not repeated.
6. Evaluate (pool), `report.json` / `report.md`, postflight (m53 S1 staged, aliases hardlinks, m51 untouched, no leg copies).

## 8. Dry run
`IAI_R894_INJECT_LOCK_S=8 089-04-window.py dry`, root `089-04-evidence\dry-final`, on the frozen bytes: **exit 0 (COMPLETE), preflight 0
problems** (freeze, E1 pin, DC2 / M53 pins, the five maps, L2, m51, tool blobs, glue check, selftest), **44 of 44 stubbed launches
ACCEPTED** (the night's `M53DC_*` banks as stand-ins, census legs on the night's own logs), 13 NOT-RUN-PREMISE exactly as predicted
(normal: no candidate; H2U: its probe stand-in, the night `P_FLOOR`, is unusable), DC2 staged from an M53 start with a verified receipt,
**the G355 restore copied the held exe on attempt 3**, `label_rule_all_pass` true, 765 s (premise 20, legs 119,
evaluation 615, of which the 6-worker pool 409). Stand-in readings, each the known answer and NOT a verdict about DC2:

| gate / leg | reading on the stand-in | why |
|---|---|---|
| lock (G403) | H1U SM_Ramp2, uv cap 268,435,456; H2U, H1N, H2N none | the night census logs; probe 1U stand-in = an identity null (clean), 2U = the night P_FLOOR |
| P-ORC | FAIL on interior only; uv16 16/16 (cell 13 by geometry), uv64 64/64 | the night frames have no show flag |
| G-MODE | INSUFFICIENT on all four | P-ORC fails on the stand-ins |
| G-LEVER-OFF | PASS (every check) | the night no-flag log |
| G-CENSUS | host cold FAIL (both blocks in frame 91), fixture cold PASS, grammar PASS, consistency 4/4, joins PASS | the night logs |
| B-M53 fixture | 4 / 4 PASS, 17 / 17 each | the night F_* legs |
| MW H1U tile NAT (stand-in the night ARM_CD3_AAON) | E1 FAIL onset +1 ×7; GT null covered, wrong object PASS, 7 adjudication readings | an arm stand-in reaches the adjudication path |
| MW H1U tile AA-off (stand-in the night MODE_TILE, 200 frames) | PASS 7 / 7; GT covered, wrong object PASS, arbiter READS-WRONG; doctored labels: exact PASS, 4 variants FAIL | the null (400) covers a 200-frame ON |
| MW H1U scramble NAT (stand-in the night F_SCRAMBLE_NAT, 400 frames) | E1 PASS 17 / 17; GT UNJUDGEABLE-NULL-COVERAGE | a 400-frame ON cannot be covered by a 400-frame null |
| arms | CD3 / RD3 proven +1, both AA; CD5 / RD5 not proven (their stand-ins are CD3 / RD3); NOAPPLY1 proven | |
| MIXED | FAIL both | the night slot-0 override |
| G-BIND | INVALID-RECIPE | the night recipe |
| kit | 22 judged legs, exit 0 | |

A first dry run on pre-final bytes (`dry-run1-superseded`) found two harness defects, both fixed before the final freeze: the G-CENSUS
grammar rejected the product's `reasons=-` (TexCorruptTree.cpp:1142-1144), and the MainWorld stand-ins (400-frame ON against a
400-frame null) never reached the wrong-object / arbiter / adjudication branches (G404).

## 9. Expected bench time (target ≤ 90 min)
From 089-03's ledger (15 s per 200-frame attempt, 18 s per 300, plus about 50 s between attempts; census ≈ 0.8 min):
premise 4 legs ≈ 3.6 + host probes 2 ≈ 2.0 + lock and commit ≈ 1.5 · LEVER 0.9 · CENSUS 1.6 · BM53F 4 × 1.2 = 4.8 · GMODE
6 × 1.1 = 6.6 · arms 9 × 1.1 = 9.9 · MIXED 2 × 1.0 = 2.0 · BIND 4 × 0.95 = 3.8 · MainWorld uv 5 × 1.15 + 3 × 1.25 = 9.5 · COST
5 × 1.1 = 5.5 ⇒ **≈ 52 min of legs**; staging + restore ≈ 3; evaluation (6 workers) ≈ 11; postflight 1 ⇒ **≈ 67 min**,
≈ 80 with a retry and person waits. If H2U's probe refuses, 3 legs fewer (≈ 3.5 min); a normal host (none today) would add 8
legs (≈ 9.5 min).

## 10. Frozen bytes (`089-04-evidence\freeze.json`; SHA-256 prefix)
| file | sha256[:16] |
|---|---|
| `089-04-common.py` | `DC562716FB3321A0` |
| `089-04-legs.py` | `9E04CE8B7EF04424` |
| `089-04-lib.py` | `B61E06EBC44DE1A3` |
| `089-04-eval.py` | `586980EFB9CBFBEE` |
| `089-04-make-eval.py` | `E4D901C5F67CCB8E` |
| `089-04-make-lib.py` | `C75CDEC705BDFF97` |
| `089-04-window.py` | `AEDC1B57D6770E1B` |
| `089-04-run.ps1` | `34F7EA9133B12943` |
| `089-04-gateselftest.py` | `AB0BF9B78DE0C3C7` |
| `085-04-eval.py` | `DAD5D56563D9954F` |
| `085-04-m53rows.py` | `99B157A83A58EDA6` |
| `085-04-oracle.py` | `56A6C4665AABDB0E` |
| `089-03-eval.py` | `8E48E88362627E3E` |
| `089-03-common.py` | `AC76D1F693B2C567` |
| `089-03-kit.py` | `8D6A9E1DA988D66E` |
| `084-09-eval.py` | `77C6BD3B83BE47E8` |
| `084-09-common.py` | `AA33E21F814598C6` |
| `084-06-eval.py` | `4EEF56681ACA77CA` |
| `084-03-lib.py` | `05FEAE6E1B93E4D8` |
| `086-01-work/ls086.py` | `D9E69F38711F8A24` |
| `085-04-evidence/tools/verify_capture.py` | `CDDB36F4ADACA663` |
| `085-04-evidence/tools/census_readonly_check.py` | `804033B094E29FB2` |
| `085-04-evidence/tools/exclusion_gate.py` | `868E11B6DEDBA76D` |
| `085-04-evidence/tools/texcorrupt_draw_kat.py` | `DCFF2BB2E8D26DE4` |
| `089-04-oracle.py` | `3E756C0F39C6D2EE` |
| `089-04-dc2pin.json` | `BD818C475C44BABF` |
