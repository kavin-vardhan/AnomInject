# 090-10c — the office refused every UV/normal candidate as `runtime_lod_bias`; m53 now copies the mips the texture actually renders, and every gate is audited at home against office-like content

Date: 2026-10-02 (01:48 →) · Brief `_mailbox` 090-10c (Code, Opus 5.5, headless) · Spec `_reviews\090-10x-chat-ruling-compatibility.md`
(Codex A1–A4 accepted) + `_reviews\090-10x-owner-answer-B.md` (**B**). Pre-declaration `E:\IA_BuildCache\_r910c\predeclared.md` (written
before the fix was built). Harness `_reviews\090-10c-{common,go,read,sum}.py` (reuses 090-10b's gate, staging and pixel reader and 090-10b2's
PIE driver). Evidence `E:\IA_BuildCache\_r910c\` (ev, out, inv, tables, receipts).

## 0. State at start
m53 `feat/m53-uv-normal-corruption` `d7e396d` == origin (code `61d186a`); `_r53_host` detached at `61d186a` (exe `E2BCE76B`). Fix branch
`d3c9423`, `m51`, `master`, tags untouched. Owner's answer to ruling 2 present: **B**. PC quiet (no editor, person idle > 30 min).

## 1. Why the office refused again
The office census on `0a69ea3`: uv eligible 0 of 15,906, `runtime_lod_bias` 14,454 (first-failure attribution). In UE 5.1 per-texture
`LODBias`, texture-group bias (device profile), cinematic mips and the streaming budget change **residency only**:
`GetResourcePostInitState` creates the resource from cooked mip `AssetLODBias = CachedLODBias − cinematic` (`Texture.cpp:1400-1436`),
`FTexture2DResource::CreateTexture` sizes the RHI texture from `RequestedFirstLODIdx` (`Texture2DResource.cpp:101-110`), so RHI mip 0 is
cooked mip `AssetLODBias + MaxNumLODs − NumResidentLODs` (`StreamableRenderResourceState.h:71-83`). Only `r.MipMapLODBias` reaches the
sampler (`Texture2DResource.cpp:83`); the streaming bias is 0 in the editor and floored outside (`TextureStreamingHelpers.cpp:299`),
`UseAllMips` zeroes it (`:325-328`). `0a69ea3` refused all of these (T9 `cinematic` / `per_texture` / `streaming_budget` from the RAW
cvar, E4/E5 global) and then refused anything not fully resident (T10) — while the home fixture's 128 px, unbiased, fully resident
textures passed every gate (G445).

## 2. The all-reasons census (A1) — run BEFORE any fix (C1 `d38bcc4`, admission unchanged)
`IAI.TexCorrupt.Census allreasons`: scope `all`, time-sliced like the census, every check for every object (event, slot, binding;
dependent checks marked unassessed), per type: each blocking key with objects failing it and objects failing ONLY it, the notes,
the top 12 combinations, `b_gain_objects`. Every census also prints `subs` (first refusal as `reason/sub`); the `Auto.Yield` line carries
the same keys. Subs that could name content reduce to the reason; keys are `[A-Za-z0-9_./]`. Selftest both ways: `anomaly_refusal_counts.py
--allreasons-check` PASSes the real office-like table (80 objects in multi-key combinations per type) and FAILs the same leg run with the
mutant lever `IAI.Bench.TexCorruptAllReasonsFirstOnly 1` (rc 1); `census_readonly_check.py` admits the new count-only lines (selftest 40/40).

**Office-like home content** (scratch, by receipt): texture-group LODBias 1 for World / WorldNormalMap / WorldSpecular through a scratch
`DefaultDeviceProfiles.ini` on the scratch host, `sg.TextureQuality 1` (r.Streaming.MipBias 1), `r.Streaming.PoolSize 300`, per-texture
LODBias 1 on every 3rd mesh actor and NumCinematicMipLevels 2 on every 3rd+1 (in memory, nothing saved; content manifest 0/0/0 of 2,207),
`-DPCVars=r.ShaderCompiler.JobCache=1,r.ShaderCompiler.JobCacheDDC=1`.

BEFORE (C1, PIE), first-failure line — **the office's shape at home**:

| leg | uv eligible | normal eligible | first refusals (uv) |
|---|---|---|---|
| fixture, plain | 67 / 82 | 8 / 82 | not_fully_resident 3, … |
| fixture, office-like | **0 / 82** | **0 / 82** | **runtime_lod_bias 74**, … |
| MainWorld, plain | 25 / 343 | 8 / 343 | nanite_unmaskable 266, partial_footprint 46, … |
| MainWorld, office-like | **0 / 343** | **0 / 343** | nanite_unmaskable 266, runtime_lod_bias, … |

ALL-REASONS, office-like fixture, uv (objects / only): not_fully_resident/max_below_cooked 79/0 · runtime_lod_bias/per_texture 79/0 ·
runtime_lod_bias/cinematic 60/0 · not_fully_resident 3/0 · runtime_lod_bias/streaming_budget 3/0 · no_eligible_slot/all_untouched 2/2 ·
below_size_policy 1/0 · draw_shaders_missing 1/0 · excluded_group/group_16 1/0 · map_set_over_cap 1/0 · nanite_override 1/0 ·
nanite_unmaskable 1/0 · no_textures 1/0 · texture_not_parameter 1/0 · unsupported_encoding/PF_R16F 1/0 · unsupported_type/cube 1/0 ·
virtual_texture/virtual 1/0. Top combinations: max_below_cooked+cinematic+per_texture 51 · max_below_cooked+per_texture 18 · … .

ALL-REASONS, office-like MainWorld, uv: nanite_unmaskable 266 · max_below_cooked 188 · per_texture 188 · streaming_budget 188 ·
draw_shaders_missing 155 · no_textures 155 · texture_not_parameter 45 · cinematic 42 · not_fully_resident 39 · host_mid 25 · VT 4 ·
no_eligible_slot 2 · excluded_group 1. Plain MainWorld also showed the next gate: **over_budget 134** (fully resident 2K/4K sets at the
128 MB cap; uv RGBA8 copies) and partial_footprint/slot_translucent 26.

## 3. What changed (C2 `1ef8e6d`, `60acb7a`, `8caf413`; C3 `dcd8d29`)
**Four-rule classification.** A check may block only for (1) label/picture disagreement, (2) wrong object, (3) crash / leak / hard cap,
(4) revert residue (table in §6).
- **A3 resident chain.** `GatherTextureFacts` takes the resident chain (first cooked mip F, `NumResidentLODs` levels); the copy is
  allocated, drawn (`SampleLevel(k)` = cooked `F+k`), budgeted and tripwired on it. T9 and T10 and `streaming_pending` become notes;
  only `not_fully_resident:unmappable` (`AssetLODBias + MaxNumLODs != cooked mips`) still refuses. The tripwire holds the source RHI
  texture from before the first level draw to after the last; a replacement in between is counted (`texcorrupt_copy_source_changed`).
  `snapshot_mip` = F (+ the budget drop); `cooked_mip_count` new. Size policy and the non-spatial exemption judge the cooked texture.
- **A2 effective state.** Streaming notes read `EffectiveStreamingMipBias` (0 in the editor, floor, `UseAllMips` 0).
- **A4 sampler.** The copy's RT sampler gets the global `r.MipMapLODBias` (`TextureRenderTarget2D.cpp:648-655` has none) and every copy
  level is drawn at `SrcMip − bias`, because the source's own sampler bias applies to the explicit-LOD copy draw (G447; measured below).
- **B per-part admission** (owner's answer). Untouched slots no longer count; a mesh component is admitted only whole; when components are
  skipped, an event component scope makes the target mask, the m26 measure and `bbox_px` cover the corrupted components only (
  `AnomalyViewport::SetEventComponentScope`, read by `AnomalyStencilTag::TagActor` / `VerifyActorStillTagged` and
  `ProjectActorBoundsToScreenRect`). Telemetry: `texcorrupt.components_corrupted` / `_skipped`, `slots_corrupted_list`, skipped slots in
  `slots_untouched` as `component_skipped`.
- **Copy ceiling (the next gate, C3).** An event over the RT budget halves its largest copy (never below 512 px) until it fits
  (`FitRequirementToBudget`, note `over_budget/copy_reduced`); only then `over_budget`.
- `partial_footprint`'s sub names the earliest blocking slot. Bench levers (gated): `TexCorruptAllReasonsFirstOnly`, `RtSamplerBias`,
  `SrcMipCompensation`, `NoPartScope`, `HostTexBias`.

## 4. AFTER — the same census on the final head (C3 `dcd8d29`, exe `B3103E32`)

| leg | uv eligible | normal eligible |
|---|---|---|
| fixture, plain | **72** / 82 (was 67) | 8 / 82 |
| fixture, office-like | **72** / 82 (was **0**) | 8 / 82 (was 0) |
| MainWorld, plain | **33** / 343 (was 25) | 9 / 343 |
| MainWorld, office-like | **33** / 343 (was **0**) | 9 / 343 (was 0) |

Office-like fixture, uv blocking keys (objects / only): no_eligible_slot/all_untouched 2/2 · below_size_policy 1/1 · excluded_group 1/1 ·
map_set_over_cap 1/1 · texture_not_parameter 1/1 · unsupported_encoding/PF_R16F 1/1 · unsupported_type/cube 1/1 · virtual_texture 1/1 ·
nanite_unmaskable+nanite_override+draw_shaders_missing+no_textures 1 (TC_Nanite) — every one a declared genuine-refusal producer (D4).
Notes: resident_chain 79, per_texture 79, cinematic 60. Office-like MainWorld uv: nanite_unmaskable 266 (**85 objects blocked by Nanite
ALONE**), draw_shaders_missing + no_textures 155 (Nanite-only materials), texture_not_parameter 45, host_mid 25, VT 4,
no_eligible_slot 2, excluded_group 1; notes resident_chain 188, per_texture 188, cinematic 42. Plain MainWorld: `over_budget` is now a
note on 102 objects (fit by a reduced copy), not a refusal. **D3 holds: no object is blocked by a handled key.**

## 5. Proof in-engine (final head, exe `B3103E32`; fixture targets `StaticMeshActor_0` uv `tile`, `StaticMeshActor_12` normal `invert`; 150 frames, `2 4 8 14 0`, AA-off arbiter; pixel rule 090-10b: labelled-frame median ≥ max(4.0, 4 × clean null); edges 090-10b; identity = max labelled diff ≤ clean null + 0.05)

| variant | path | uv: events / corrupt (median) | normal: events / corrupt (median) | identity exact (max vs null ≈ 0.33 PIE / 0.04 staged) | edges in sync (uv, normal) | copy from |
|---|---|---|---|---|---|---|
| V1 per-texture LODBias 1 | PIE | 7 / 7 (40.96) | 7 / 7 (17.48) | 7/7 (0.340) | 7/7, 7/7 | mip 1 of 8 (64 px) |
| V2 texture-group bias (scratch profile) | PIE | 7 / 7 (41.04) | 7 / 7 (17.49) | 7/7 (0.343) | 6/7, 6/7 (end +1, pre-existing PIE) | mip 1 of 8 |
| V3 cinematic 1 | PIE | 7 / 7 (40.97) | 7 / 7 (17.50) | 7/7 (0.340) | 7/7, 7/7 | mip 0 (the cinematic mip was resident) |
| V4 `r.MipMapLODBias 1` | PIE | 7 / 7 (34.20) | 7 / 7 (14.38) | 7/7 (0.354) | 6/7, 6/7 (end +1) | mip 0; RT bias + SrcMip − 1 |
| V5a `r.Streaming.MipBias 1`, per-texture 1 | PIE | 7 / 7 (40.97) | 7 / 7 (17.48) | 7/7 (0.340) | 7/7, 7/7 | mip 0 (effective bias 0 in the editor) |
| V5b same, per-texture 0 | PIE | 7 / 7 (40.97) | 7 / 7 (17.48) | 7/7 (0.340) | 6/7, 6/7 (end +1) | mip 0 |
| V6 far streamed `TC_Stream2k` | PIE | 7 applied, **not judgeable**: 2×2 px on screen | (normal on `_12`) 7 / 7 (17.50) | — | —, 7/7 | **mip 5 of 12 (64 px)** |
| V1 per-texture (lever) | staged | **INVALID** — the lever blanked the cooked source (G449) | INVALID | — | — | — |
| V2 texture-group (saved profile) | staged | 7 / 7 (46.55) | 7 / 7 (22.32) | 7/7 (0.345) | 7/7, 7/7 | mip 0 — **the bias did not reach the cooked build** (no note); a baseline, not V2 |
| V3 cinematic (lever) | staged | **INVALID** (G449) | INVALID | — | — | — |
| V4 `r.MipMapLODBias 1` | staged | 7 / 7 (37.76) | 7 / 7 (17.78) | 7/7 (0.339) | 7/7, 7/7 | mip 0; note global_sampler |
| V5a | staged | 7 / 7 (46.55) | 7 / 7 (22.32) | 7/7 (0.345) | 7/7, 7/7 | mip 0 |
| V5b | staged | 7 / 7 (46.55) | 7 / 7 (22.32) | 7/7 (0.345) | 7/7, 7/7 | mip 0; note global_streaming (effective 1 in a game) |
| V6 far streamed | staged | 7 applied, not judgeable (2×2 px) | 7 / 7 (22.32) | — | —, 7/7 | mip 0 of 12 at 2048 px on one fire, mip 5 (64 px) on others |

A4 measured (C2 build, identity under `r.MipMapLODBias 1`): RT bias + no compensation **17.9** · neither **3.4** · RT bias + SrcMip − bias **exact** (0.35) → product default. V0 (no bias) identity exact 0.337 vs 0.335.
`texcorrupt_rt_mip_mismatch` 0 and `texcorrupt_copy_source_changed` 0 on every proof leg; `texcorrupt_resident_chain_outputs` = fires on V1/V2 PIE and V6.

**Can-fail, `0a69ea3` (archived `m53-0910b-65607703`, exe + editor DLLs) on the same setups** — the per-actor bench census line and a uv capture:

| variant | PIE | staged |
|---|---|---|
| V1 per-texture | refused `runtime_lod_bias:per_texture` (uv, normal), **0 events** (binding: resident 7 / max 7, AssetLODBias 1, cached 1) | not runnable (lever invalid in a cooked build) |
| V2 texture group | refused `runtime_lod_bias:per_texture`, **0 events** (AssetLODBias 1 from the group) | the variant did not reach the cooked build: applied 7 |
| V3 cinematic | refused `runtime_lod_bias:cinematic`, **0 events** | not runnable |
| V4 global sampler | refused `runtime_lod_bias:global_sampler`, **0 events** | refused `global_sampler`, **0 events** |
| V5a streaming budget | applied 7: the fixture's 128 px textures do not stream, so `StreamingBudgetPossible` is false; the refusal on streamed textures is in the BEFORE census (MainWorld office-like: `streaming_budget` 188 objects) | applied 7 (same) |
| V5b global streaming | refused `runtime_lod_bias:global_streaming`, **0 events** | refused `global_streaming`, **0 events** |
| V6 partly resident | refused `not_fully_resident` (resident 7 of 12), **0 events** | 6 events, refusals `not_fully_resident` on the others (residency of a 2-px far texture fluctuates) |

## 6. Gate audit (reason/sub → count on office-like content → handled / kept → why)
Counts are objects failing the key in the AFTER `allreasons` census on office-like home content (fixture 82 objects / MainWorld 343;
uv first, normal second where they differ; "only" = the objects that key alone blocks). The four rules: (1) label/picture disagreement,
(2) wrong object or visible change outside the target, (3) crash / leak / hard memory cap, (4) revert residue. **Removed** = no longer a
check; **non-blocking** = still evaluated, logged as a note or a slot skip, never refuses the event; **keep** = still refuses, with its rule.

| reason / sub | office-like count (fixture · MainWorld) | decision | why |
|---|---|---|---|
| `runtime_lod_bias/per_texture`, `/group` (device profile), `/cinematic`, `/streaming_budget`, `/global_streaming` | before: 74 first-failure · 188; after: notes 79 · 188, 60 · 42 | **non-blocking** | residency only (`Texture.cpp:1400-1436`); the copy starts from the resident first mip, identity exact (§5) |
| `runtime_lod_bias/global_sampler` (`r.MipMapLODBias`) | before: refused every object; after 0 | **non-blocking** (matched) | RT sampler gets the bias, copy drawn at `SrcMip − bias`; identity 0.35 vs null 0.34 (G447) |
| `not_fully_resident` (partly resident / `max_below_cooked`) | before 79 · 188 (+3 · 39 as first failure); after note `resident_chain` 79 · 188 | **non-blocking** | the copy is the resident chain; V6 copied from mip 5 of 12 |
| `not_fully_resident/unmappable` | 0 · 0 | **keep** (1) | `AssetLODBias + MaxNumLODs ≠ cooked mips`: the RHI mip cannot be mapped to a cooked mip, so the copy could be the wrong level |
| `streaming_pending` | 0 · 0 (note) | **non-blocking** | a pending stream changes the resident chain; the tripwire counts a source replaced mid-copy (`copy_source_changed` 0 on every leg) |
| `over_budget` | plain MainWorld: before 134 first-failure, after note on 102 (uv) / 56 (normal); office-like 0 | **non-blocking** below the floor, **keep** (3) above it | the copy is fit to the 128 MB RT budget by halving its largest texture down to 512 px (`over_budget/copy_reduced`); a set that still does not fit is a hard memory cap |
| `partial_footprint` (now per component) | 0 · 38 uv / 46 normal (first failure; subs `texture_not_parameter` 32, `host_mid` 6, `no_normal_map` 8) | **non-blocking** per slot (B), **keep** (1) per component | untouched slots no longer count; a component with some slots refused is skipped whole, and the mask/bbox cover only corrupted components; an object whose every component is partly refused cannot be labelled without a mask that disagrees with the picture |
| `slot_empty`, `slot_translucent`, `no_textures` (slot level) | slot skips | **non-blocking** | the slot is left untouched; only the event key `no_eligible_slot` refuses |
| `no_eligible_slot/all_untouched` | 2 · 2 (only 2 · 2) | **keep** (1) | nothing would change in the picture |
| `nanite_unmaskable`, `nanite_override` | 1 · 266 (85 only) | **keep** (1) | no custom-depth mask for Nanite (G134); the office hosts draw no Nanite |
| `draw_shaders_missing/none.no_vertex_factory_shaders`, `default_material_path` | 1 · 155 (0 only; Nanite-only materials) | **keep** (1) | the material cannot draw the corruption on this vertex factory: label without a picture |
| `shader_map_unavailable`, `shader_map_incomplete` | 0 · 0 | **keep** (1) as narrowed in 090-10b2 | refuses only when the drawing permutation is missing |
| `texture_not_parameter` | 1 · 45 (2 only; 32 as `partial_footprint` sub) | **keep** (2) — design limit | the texture is not a material parameter, so the only override is the shared material: every user of that material would change |
| `host_mid/override.mid` | 0 · 25 (0 only) | **keep** (2) — C, out of scope | a host-owned MID can be re-set by the game and its siblings share state |
| `virtual_texture/virtual` | 1 · 4 | **keep** (1) — C | a virtual texture's pages are not a mip chain the copy can bind |
| `unsupported_type/cube` (and array, volume) | 1 · 0 | **keep** (1) — C | 2D-only copy pipeline |
| `unsupported_encoding/PF_R16F` | 1 · 0 | **keep** (1) | the copy RT cannot reproduce the encoding |
| `excluded_group/group_16` | 1 · 1 | **keep** (2) | lightmap / shadowmap / UI groups are not the object's surface |
| `map_set_over_cap` | 1 · 0 | **keep** (3) | hard cap on maps per coherent set |
| `below_size_policy` | uv 1 · 0; normal 71 · 108 (0 only) | **keep** (1) | too small to be visible; judged on the cooked size |
| `no_normal_map`, `normal_unconnected` (normal) | 71 · 108, 71 · 24 | **keep** (1) | nothing for normal corruption to change |
| `dxt5_normal_host` (event, `Compat.UseDXT5NormalMaps`) | 0 · 0 | **keep** (1) | the host's normal reconstruction would not match the copy |
| `mip_chain_shape`, `held_by_stuck_low_mip`, `resource_not_ready` | 0 · 0 | **keep** (1)/(3) | no valid mip chain or no RHI resource |
| `mode_invalid`, `no_mesh`, `assets_unavailable`, `corruptor_not_ready` | 0 · 0 | **keep** (1)/(3) | configuration or plugin assets; nothing can be drawn |
| `rt_alloc_failed`, `draw_precondition_failed`, `param_readback_mismatch` | 0 · 0 (fire time) | **keep** (3)/(1)/(4) | allocation failure; the draw would not run; the binding did not take, so the revert could leave residue |
| identity / tile-probe / wrong-copy checks | bench only | **keep behind `-IAIBench`** | proofs, never product refusals |

Estimated shares on the office (not measured there; from the home MainWorld office-like census, Nanite excluded because the office hosts
draw none): `texture_not_parameter` ≈ 13 % of objects (45 of 343; 2 alone, 32 more through a partly refused component), `host_mid` ≈ 7 %,
VT ≈ 1 %, `over_budget` 0 % with the copy ceiling, `map_set_over_cap` < 1 %. **No handled key blocks any object (D3).** The office run of
`IAI.TexCorrupt.Census allreasons` replaces these estimates with its own counts.

## 6.1 B (owner's answer) — as built and how far it is proven
- **Pure, both ways:** `JudgeComponent`, `DecideFootprintStep`, `FitRequirementToBudget` in `texcorrupt_pure_test.cpp` (270 checks, 0
  failures); mutants `part_of_a_component_admitted`, `copy_ceiling_floor_ignored`, `editor_streaming_bias_not_zeroed` each fail it.
- **In engine, slot level:** `BP_EnergyOrb` with Nanite disallowed on the summoned instance (the office hosts draw no Nanite): uv
  `final=APPLY slots=4 qualified=3`, the translucent slot left untouched (`slot_translucent` 7, `qualified` 21), 7 events — where `0a69ea3`
  refuses `partial_footprint` 3/4. The orb rotates and pulses every tick, so its clean null reaches 4.5 and the pixel rule judges only
  2 of 7 events corrupt: **inconclusive as a picture proof, not a pass.**
- **In engine, component scope (mask and `bbox_px` cover only the corrupted components): NOT SHOWN.** No home object exercises it:
  `b_gain_objects` is 0 on all eight censuses (plain and office-like, fixture and MainWorld), and the one multi-component candidate tried
  (`BP_SpawnPad`, three attempts) keeps construction-script components Nanite. The office census prints `b_gain_objects`, which says
  whether B matters there.

## 6.2 Automatic application
Office-like MainWorld, auto-pool, 600 frames: **0 events** — the view candidates are `nanite_unmaskable` 16 and `texture_not_parameter`
16 (uv; normal 20 / 20), i.e. only the kept refusals. Office-like fixture with the shipped selection defaults: 0 events, every round
`no_visible_candidate` (the 6 % coverage floor and the 18 m pawn radius remove the fixture's small targets — selection, not this anomaly).
Office-like fixture with `IAI.SetMinScreenCoverage 0` + `IAI.SetPollRadius 0` as LEG conditions (defaults unchanged), auto-pool uv +
normal, seed 777, 600 frames (`P_AUTO_OL_FIX2_B3103E32`): **28 events applied automatically**, every target carrying the office-like
bias (e.g. `asset_lod_bias=2 cinematic=2`). **17 visibly corrupt (median 19.8–72.0 against a null ≤ 0.35), edges 17 of 17 in sync
(start 0, end 0).** The other 11 read no visible change (median 0.34–3.8): all on the fixture's 16×16 `T_TC_Chain*` mip-chain controls
(`_18`, `_19`, `_21`, `_22`, `_26`); not investigated here, no mechanism asserted. `0a69ea3` cannot be compared on this content: it
refuses every one of these objects (§2).

## 6.3 Builds, checks, archive
- Commits on `feat/m53-uv-normal-corruption`: `d38bcc4` (C1 census), `1ef8e6d` + `60acb7a` + `8caf413` (C2; `60acb7a` does not compile —
  `FTexture::GetOrCreateSamplerState` is protected — and `8caf413` fixes it), `dcd8d29` (C3), then this docs commit. Leg build = `dcd8d29`:
  exe **`B3103E32`**, editor DLLs Injector `1E244F0C`, Capture `E7A9676E`, Bench `9396BB40`, ControlServer `B9D552B6`, Shaders `3BC78EFA`.
- C++ suite (`_r53_selftest\Z0910c`): base 7/7 (pure 270 checks 0 failures), mutants 27/27 fail (the pure mutant header carries 22 faults,
  60 failures). Python suites 27/27; `anomaly_refusal_counts.py` selftest 21; census read-only check PASS, selftest 40/40; lever audit
  PASS on source, exe and editor DLLs; string scan vs `E2BCE76B`: 135 new or changed strings (exe 14/51, Injector DLL 15/51, Capture DLL
  0/4; every one this round's census/telemetry/lever text or a one-byte artefact).
- Strict include pass on `_r53_host` at `dcd8d29`, run AFTER the legs: Game 105 actions / 388 s, Editor 114 / 398 s, **0 errors, 0
  warnings**, 64 of 64 header TUs; restore OK (Build.cs identical, git status unchanged); normal rebuild 0/0. The normal rebuild relinked
  the host (same source, different bytes, G201): host now exe `1D4F99C6`, DLLs Injector `089F348A` / Capture `5AD34EA7`. The legs ran on
  `B3103E32`, which is what is archived.
- Archive `_binary_baselines\m53-0910c-B3103E32\` (exe + editor DLLs re-hashed at the destination + evidence).
- Staged BenchGate restored to the S1 set (exe `2FCDF059`); the staged saved `DeviceProfiles.ini` restored after V2; the scratch
  `DefaultDeviceProfiles.ini` on the host removed after each use; D: content manifest unchanged (2,207 files) — all by receipt
  (`receipts.jsonl`).

## 7. Open
- PIE end edge +1 on one event in some PIE legs (V2, V4): pre-existing, PIE-only (090-10b2 §E: 7 of 7 PIE legs on `E2BCE76B`, 3 of 3 on
  `0a69ea3`); staged legs show none. Not chased (G120).
- Census `allreasons` cannot assess a dependent check (budget, footprint) behind an earlier blanket refusal; re-run on the fixed build.
- C (out of scope): `host_mid` sibling instances, constant textures, VT, unsupported types — counts in §4.
- Staged per-texture and cinematic bias are not exercised: `IAI.Bench.TexCorruptHostTexBias` blanks a cooked texture (G449) and its help
  text still says it works in a packaged build — it should refuse there (bench-only, not changed this round so the tested head stays the
  pushed head). Staged texture-group bias needs a route that reaches the cooked build (a cooked device profile).
- B's component scope (mask and `bbox_px` over corrupted components only) has pure tests and mutants but no in-engine proof: no home
  object has a skipped component that renders (`b_gain_objects` 0). A two-component fixture actor would close it.
- Office-like fixture auto-pool: 11 of 28 events on the 16×16 `T_TC_Chain*` controls read no visible change (§6.2); not investigated.

## 8. Hand-off
The office runs `IAI.TexCorrupt.Census allreasons` in PIE on the pushed head; its table replaces §6's estimates and its `b_gain_objects`
says whether B matters there. ⛔ No tag, no merge to master; the fix branch is unchanged this round.
