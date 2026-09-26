# 082-05 — m53 S1 part 1: plan fixes A/B/C/P3, preconditions and M0, all S1 plugin code, builds (G10)

Session 082-05, 2026-09-27, Claude Code (Opus 5.5), headless from the GDP mailbox. Brief:
`082-05 — m53 S1 part 1`. Ruling: `_reviews/082-05-chat-ruling-s1-authorised.md`. Plan: revision 3.1 of
`docs/predictions/2026-09-26-m53-uv-normal-corruption.md` (this session's step 0). Branch
`feat/m53-uv-normal-corruption`. **No bench leg, no cook, no content authoring, no CaptureBench edit, no staging, no tag.**

## 0. Where the work ran

- The main checkout (`D:\...\Plugins\AnomalyInjector`) stayed on `m51` at `53bf725` and was not touched.
- Edits: a worktree of the feature branch at `E:\IA_BuildCache\_r53_src\AnomalyInjector`.
- Builds: a scratch host at `E:\IA_BuildCache\_r53_host\StackOBot` (the 081-44 recipe: copies of the `.uproject`,
  `Source`, `Config`, `unreal-mcp`, `RoomGenerator`'s `.uplugin`, CaptureBench by `git archive HEAD`), with the plugin as a
  **detached** worktree moved to each commit that was built.
- Evidence: `D:\IntrusiveAnomalies\_reviews\082-05-evidence\`.

## 1. Step 0 — the plan fixes (`ea1eed1`, pushed)

Revision 3.1 folds chat's A, B, C and P3 into the plan, with a new `§R0.00` resolution table:

- **A.** `texelshift` shifts along U only, `+(1/W_m, 0)`. The diagonal shift maps a one-texel checker onto itself
  (`C(x+1, y+1) = C(x, y)`), so its ≥ 128 claim is struck through. The U-only prediction (128 on chain (b), 89 on chain (d),
  every texel) is proven offline in 082-06 by `texcorrupt_fixture_images.py`.
- **B.** `G-COLL` pairs collateral textures by (path, frame offset) across the applied and no-allocation legs, reports
  each texture with a positive deficit `max(0, null − applied)`, and never subtracts counts. A truncated set is reported
  incomplete. The per-texture input is a `TEXCORRUPT-COLL` log line per sample, behind `IAI.Bench.TexCorruptCollateralDetail 1`.
- **C.** The streamer's per-texture budget bias has no public game-thread query, so zero is established conservatively:
  a budget bias is impossible when `r.Streaming.MipBias` ≤ 0, when `UsePerTextureBias` is 0 (E5 decides), or on a
  non-streamed texture. Otherwise T9 refuses `runtime_lod_bias` / **`streaming_budget`**, before T10, whether or not the
  texture is currently resident. Fixture row: the F-SYN streamed 2048² texture with and without `r.Streaming.MipBias 1`.
- **P3.** Two same-class 4096² maps need 192 MiB (213.33 is the two-class figure). No admission outcome changes.

## 2. Step 1 — preconditions and M0

Evidence `preflight.json`, `M0-source-manifest.json`, `m0_manifest.py`.

- **Branch:** local `feat/m53-uv-normal-corruption` == origin at `ea1eed1` after step 0. `master` == `origin/master` == `b5f15a3`.
  The main checkout is on `m51` at `53bf725`, untouched.
- **Staged exe:** `E0BE6F0A` (241,728,512 B), byte-identical to `_binary_baselines\StackOBot.exe.m55-merge-strip-E0BE6F0A`.
  **Container quintet** `67EA1FE0` / `2CEFB8F4` / `E03C6610` + `A16A18A8` / `C70ECDAA` (the Phase B container, as the README records).
  ⚠ **README gap, recorded:** `_binary_baselines\README.md` had no entry for any m55 binary; its last "STAGED" marker is m52's
  `5588F6FB`, which is stale. The staged exe was verified against its archive file instead, and the README gained an 082-05 entry that
  says so.
- **Disk:** C 34.7 GB, D 201.7 GB, E 266.1 GB free: above both floors (runbook §8.6 step 0: 15 GB; 083-02: E: 50 GB).
- **M0:** SHA-256 of every file under `StackOBot\Content` (2,073), `StackOBot\Config` (5), the live plugin's `Content` (2) and the feature
  worktree's `Content` (2): 2,082 files in 20.1 s. `CB_GateLevel.umap` `1d89de1724e3596e…` (110,320 B), `MainWorld.umap`
  `a3849daa1c04fbca…` (21,139 B). No `CaptureBenchTexCorrupt\` folder exists yet.
  - M0 was taken by a script in the evidence folder, not by `CaptureBench/tools/texcorrupt_bytecheck.ps1`: CaptureBench is 082-06's, and
    this brief forbids editing it. The manifest is JSON (`root`, `rel`, `size`, `sha256` per file) so 082-06's tool can compare M1 and M2
    against it directly.

## 3. Step 2 — the S1 code (`2a2ab17` … `bb048bb`, pushed)

| commit | area | files | lines |
|---|---|---|---|
| `2a2ab17` | private `RenderCore` + `RHI`; the invariant wording in `architecture.md` and `CLAUDE.md` | 3 | +21 −4 |
| `9e9a1c3` | shared state: reasons, the corruptor parameter contract, the three knobs, the budget ledger, run stats, `TexCorruptPure.h`, the offline harness | 5 | +1,352 |
| `782bb5b` | the decision tree, active bindings, residency and runtime-bias checks, the Δ1 chain predicate, the G0 census; the m52 query | 4 | +971 |
| `a5c9f50` | render-target allocation, the per-mip draw and RDG copy, the tripwire, the warm draw | 1 | +304 |
| `885ec41` | `uv_corruption` / `normal_corruption`: the transaction, the restore, labels, the G-COLL probe; CDO references; registration | 4 | +1,417 |
| `2705e2d` | the S1 bench levers and `IAI.Bench.TexCorruptCensus` | 1 | +349 |
| `e36ae66` | capture: split prewarm list, `TexCorruptWarm` phase, fire-window labels with the mode as subtype, run_summary keys | 4 | +183 −8 |
| `bb048bb` | the PRE-DELIVERY-CHECKLIST lines | 1 | +11 |

Only the tip was built; the intermediate commits are grouped for review and were not built one by one (the 052 precedent).

**What the code does, by plan section.**

- **§R6 decision tree** (`TexCorruptTree.cpp`). E1 assets and the corruptor parameter contract · E2 corruptor shader map complete ·
  E3 `Compat.UseDXT5NormalMaps` · E4 `r.MipMapLODBias` · E5 global streaming bias · E6 mode · E7 mesh components (viewport scoping as
  the other texture anomalies) · S1–S8 per slot · T1–T10 per binding (T7 is the m52 query) · A1–A6 · V1 earliest refused slot ·
  V2 budget. No side effect; one disposition per binding and slot, one final reason per event.
- **§R2 bindings:** `GetMaterialResource(World->FeatureLevel)` → game-thread shader map → uniform expression set; the parameter test is
  `ParameterInfo.Name` not None; the texture is `GetGameThreadTextureValue`'s.
- **§R4 residency and bias:** resource ready, nothing pending, valid state; `cinematic`, `per_texture` and (fix C) `streaming_budget`
  before T10's `MaxNumLODs == M` and `NumResidentLODs == MaxNumLODs`.
- **§R5 chain predicate:** `Resolved` and `Effective` walked up `Parent`; a MID, a transient-outer link or a link without `RF_WasLoaded`
  refuses `host_mid` with `where` and `kind`.
- **§R3.2 budget:** `ΣB + ΣT` with scratch shared per (W, H, sRGB) class; reserved before anything is allocated; released bytes stay
  `pending_release` for two frames; `texcorrupt_rt_bytes_peak`.
- **§R7.4 transaction** (`Anomaly_TexCorrupt.cpp`): reserve → allocate and verify → corruptor MIDs per texture and level, read back →
  re-check every draw precondition → enqueue (tripwire, then per level clear → draw → copy, tripwire; static modes release scratch) →
  host MIDs with `SetTextureParameterValueByInfo` and read-back → commit. Any failure rolls back without touching a slot.
- **§R3.4 per-mip draw and copy** (`TexCorruptDraw.cpp`): mip 0 straight into the output through the canvas Begin/End pair; mip ≥ 1
  into a 1-mip scratch and `AddCopyTexturePass(DestMipIndex = m)`; the tripwire counts any RHI mip-count or extent disagreement.
- **§R8 restore:** only a slot still holding this event's MID is touched; raw object re-set exactly or reset to the asset; sweep;
  originals held on the subsystem until the slot is restored.
- **§R9 prewarm and warm draw:** `GatherAnomalyPrewarmMaterials` feeds only the prewarm; `TexCorruptWarm` is a 2-frame non-capturing
  phase entered only when an m53 id is targeted or enabled, knob `IAI.Capture.TexCorruptWarmDraw` (compiled on).
- **Both ids registered**, object scope, **identity and the tile probe only, not in the auto pool**. A fire with no mode is refused
  `mode_invalid` (`no_mode_in_s1`).
- **Labels:** the §R10 `texcorrupt.*` keys; `anomaly_subtype` = the mode; `condition_held` from live state.
- **run_summary:** every event-final reason and rollback step always present, the two dispositions objects, the peak, the restore
  counts, `texcorrupt_rt_mip_mismatch`, `texcorrupt_collateral_drops`, `texcorrupt_slots_partial_set`.
- **G-COLL probe:** the collateral set (≤ 256 by path) at Apply; per labelled frame and 2 frames after revert; per-texture
  `TEXCORRUPT-COLL` lines behind `IAI.Bench.TexCorruptCollateralDetail 1` (fix B).
- **Bench levers:** `IAI.Bench.TexCorrupt{NoApply 0|1|2, WrongCopy, Identity, IdentityRedraw, TileProbe 0|2|4, ForceMissingAsset,
  FailStep 2..6, ForeignReplace, CollateralDetail, AssetSlotMid, Census}`, compiled out of Shipping.

**The contract 082-06's authoring must match** (the C++ reads these names; a missing one refuses every fire `assets_unavailable`,
sub-reason `uv_corruptor_lacks:<name>` or `normal_corruptor_lacks:<name>`):

- `/AnomalyInjector/Materials/M_CorruptTex_UV` — textures `SrcColor`, `SrcData`, `SrcNormal`; scalars `SrcKind`, `SrcMip`, `UvScale`,
  `UvOffsetU`, `UvOffsetV`, `UvSwap`, `ScrambleOn`, `ScrambleK`, `ScrambleAInv`, `ScrambleB`, `DbgChanSwap`, `DbgSrgbTwice`,
  `DbgTexelShift`, `TexelSizeU`, `TexelSizeV`, `DbgSkipNormalEncode`, `DbgOpaque`.
- `/AnomalyInjector/Materials/M_CorruptTex_Normal` — textures `SrcNormal`, `NoiseNormal`; scalars `SrcMip`, `NoiseMip`, `NormalSignX`,
  `NormalSignY`, `FlatMix`, `NoiseAmp`, `DbgChanSwap`, `DbgTexelShift`, `TexelSizeU`, `TexelSizeV`, `DbgSkipNormalEncode`.
- `/AnomalyInjector/Textures/T_CorruptTex_NoiseN` — the noise normal.
- `SrcKind` is 0 colour (`SrcColor`), 1 data (`SrcData`), 2 normal (`SrcNormal`). `DbgTexelShift` moves the sample by
  `(TexelSizeU, 0)` (fix A). Every source sample is `TMVM_MipLevel` at `SrcMip` with `AutomaticViewMipBias` off (plan §R7.1).

## 4. Offline checks

`tools/texcorrupt_pure_test.cpp` compiles `TexCorruptPure.h` — the same header the plugin compiles — with the UBT toolchain
(MSVC 14.38, `/std:c++17 /W4 /WX`). Output `offline-checks.txt`: **83 checks, 0 failures**, covering the §R3.2 table, P3 and every
§R14 requirement, the V2 fit at 64/128/256 MiB with live and pending bytes, the T6 chain shape, all 17 §R3.1 format rows, A4/V1
precedence and the rank order, the source mip per level (tile ×2/×4, `mipshift`), and the E5 / `streaming_budget` predicates.
**Proven able to fail (G96):** the same harness against a copy of the header with scratch never shared and the mip clamp removed reads
**8 failures, exit 1** (`offline-checks-mutant.txt`), each on a row those two mutations reach.

⚠ What the harness does **not** cover: the engine-facing parts of the tree (shader-map reads, residency state, the chain predicate on
real materials), the transaction and the restore. Those need the game and are 082-07's `G0`, `G-REASON`, `G3`, `G4` and `G6`.

## 5. Builds (G10)

- **Prewarm** (scratch host, unchanged `49dece1` code, before any S1 edit reached it): Game target 763 actions, **3,920 s**, exit 0,
  2 warnings, both pre-existing deprecations in the host's own `MidReproActor.cpp` (not the plugin).
- **G10 editor** (`StackOBotEditor Win64 Development`, host worktree clean at `bb048bb`, plugin build products cleared first): 16 actions,
  all five plugin modules compiled from scratch, **exit 0, 79 s, 0 warnings, 0 errors**.
- **G10 game** (`StackOBot Win64 Development`, same state): 7 actions, all five plugin modules compiled, link, **exit 0, 67 s,
  0 warnings, 0 errors**.
- **Module set:** `UnrealEditor-AnomalyInjector.dll` links exactly `Core, CoreUObject, Engine, Foliage, InputCore, RenderCore, RHI`
  against the pre-m53 response file's `Core, CoreUObject, Engine, Foliage, InputCore`: **added RenderCore, RHI; removed none.** The
  other four plugin modules' link sets are unchanged.
- **A44** on the new exe: every new token present in UTF-16 (`TEXCORRUPT-ASSETS`, `IAI.Bench.TexCorruptNoApply`, `IAI.Capture.TexCorruptWarmDraw`,
  `texcorrupt_rt_mip_mismatch`, `M_CorruptTex_UV`, `T_CorruptTex_NoiseN`, `uv_corruption`, `normal_corruption`, …) beside pre-existing
  controls (`IAI.Capture.ShaderPrewarm` 7, `IAI.Bench.StuckMipNoHold` 5), so the scan is sound (`a44-scan.json`).
- **Archived, hash-verified at the destination, NOT staged:** `_binary_baselines\StackOBot.exe.m53-s1-D7BA87AC` (241,926,144 B) and
  `_binary_baselines\m53-s1-editor-bb048bb\` (the five plugin editor DLLs + PDBs + `UnrealEditor.modules`;
  `UnrealEditor-AnomalyInjector.dll` `5BC7DBBB`). The staged exe is still `E0BE6F0A`.
- **Comment strip:** `_strip_comments.py` over the worktree with the new files intent-added: 124 files, **0 changed** (`strip-run1.txt`).
- ⚠ **A trap caught on the way (G309):** a compile-check copy of the sources kept their old mtimes, so one game build reported exit 0
  on OLD capture code. It was caught by reading the action list; the gate builds cleared the plugin's build products first.

## 6. Fail-closed behaviour while the 082-06 assets do not exist

- The CDO's three `FObjectFinder`s fail; the engine logs `CDO Constructor (AnomalyInjectorSubsystem): Failed to find <path>` at error
  verbosity (`UObjectGlobals.cpp`, `ConstructorHelpers::FailedToFind`) and the pointers stay null. Non-fatal.
- Startup: `TEXCORRUPT-ASSETS M_CorruptTex_UV resolved=0 …; M_CorruptTex_Normal resolved=0 …; T_CorruptTex_NoiseN resolved=0 …`.
- Every fire: E1 refuses `assets_unavailable`, sub-reason `missing:M_CorruptTex_UV,M_CorruptTex_Normal,T_CorruptTex_NoiseN`, counted in
  `texcorrupt_refused_assets_unavailable`; nothing is reserved, allocated or touched. Once the assets exist but lack a parameter the
  C++ sets, E1 refuses `uv_corruptor_lacks:<name>` / `normal_corruptor_lacks:<name>`.
- The warm draw logs `WARM DRAW SKIPPED` and draws nothing; `IAI.Bench.TexCorruptCensus` still runs and reads the E1 refusal.
- ⚠ For 082-06: until the assets are authored, every editor or commandlet launch of this code logs those three CDO errors. The
  authoring commandlet will print them once before it creates the assets; the cook runs after, when they exist.

## 7. Deviations from the plan, each with its reason

1. **`optional_unmounted` is emitted as `optional_not_resident`.** Whether an optional mip is *mounted* is private streamer state
   (`FStreamingRenderAsset::OptionalMipsState`); the public state shows only that the resident count stops at the non-optional count.
   The sub-reason names the observation, not a cause.
2. **`snapshot_px` is two ints, `snapshot_px_w` / `snapshot_px_h`.** Telemetry array records hold scalars only.
3. **E1 also checks the corruptor parameter contract.** A corruptor missing a parameter would otherwise draw with the parent default;
   refusing names the missing parameter.
4. **T5 reports a texture with no platform data as `resource_not_ready` / `no_platform_data`.** Unreachable on cooked content; named
   rather than folded into `unsupported_encoding`.
5. **S7's "lightmapped static" mirrors the engine exactly:** the usage is required iff LOD0's map build data carries a light map or a
   shadow map (`StaticMeshRender.cpp:2204`, `:2225`).
6. **Mode selection in S1:** a mode argument is refused `mode_invalid` (`unknown:<arg>`) because S1 has no product mode; no argument and
   no lever is `mode_invalid` (`no_mode_in_s1`). The tile probe applies to `uv_corruption` on targeted fire only.
7. **FailStep 5 reports `draw_precondition_failed`**, and a reserve failure at step 1 (a race after V2) counts as
   `texcorrupt_rollback_1` + `over_budget`. The plan's table names reasons for steps 2, 3, 4 and 6 only.
8. **Additive keys beyond §R10:** `texcorrupt.collateral_count` / `collateral_truncated` (named by fix B), `texcorrupt.bench_noapply` /
   `bench_wrong_copy` (bench-only), run_summary `texcorrupt_slots_partial_set` (the §R6.2 A4 diagnostic counter).
9. **The checklist line is worded on asset and object references,** because the plan's own `AssetSlotMid` lever needs the fixture path
   as a string prefix; a literal "nothing references the path" check would fail on correct source.
10. **The G-COLL null runs the probe but no target watch** (NoApply 2 returns before the watch). The weak-pointer backstop still reverts.

## 8. Not done here, by design

No bench leg, no editor or game launch, no cook, no content authoring, no CaptureBench edit, no staging, no tag. Next: Codex's source
review of this code (the ruling's first checkpoint), then 082-06 (authoring + cook + M1/M2 + G-COOK), then 082-07 (legs).