# 082-06d — m53 S1: fixture slot fix, targeted re-author, re-cook, G-COOK (amended), STAGED

Session 082-06d, 2026-09-27, Claude Code (Opus 5.5), headless from the GDP mailbox. Ruling:
`_reviews/082-06d-chat-ruling-gcook-fixture.md` (082-06's NEEDS-DECISION: the `MainWorld` G-COOK row amended to the
control-pair form, `CB_GateLevel` stays exact; the fixture fix, the re-cook and staging authorised). Branch
`feat/m53-uv-normal-corruption`. Evidence: `D:\IntrusiveAnomalies\_reviews\082-06d-evidence\`.

**Outcome: the fixture is fixed and its four refusal-reason producers are proven from disk; 13 identical-input cooks ran
clean; G-COOK passes under the amended rule; the new build is STAGED and A44-green.** No leg, no game launch, no tag.

## 0. Where the work ran

- Main checkout untouched on `m51` at `53bf725`. Plugin docs on the branch worktree `E:\IA_BuildCache\_r53_src\AnomalyInjector`.
- Commandlets and cooks on the scratch host `E:\IA_BuildCache\_r53_host\StackOBot` (plugin worktree detached at `c9f8089`;
  `Content` and `Plugins\RoomGenerator\Content` junctioned to the real D: content, as in 082-06). No plugin source changed;
  every cook's build step was "Target is up to date", so the exe is 082-06c's `2FCDF059`.
- Commandlets only; a byte manifest before and after every commandlet and cook run (§2). No interactive editor was opened.
- Environment: no foreign editor ran. Six idle MSBuild reuse nodes from 082-06's cook (05:52) were present, not ours, left alone.

## 1. The fixture fix (CaptureBench `47d6fff`, `tools/make_texcorrupt_fixture.py`, SHA-256 `584C9848…`)

- **Cause (082-06, G310):** UE's Python returns array struct elements by copy, so the tool's loop edits were dropped and
  the unmodified slot arrays were written back.
- **Static meshes:** slots are set with `UStaticMesh::SetMaterial(i, M)` (engine `StaticMesh.cpp`, accepts null) and
  checked in memory afterwards.
- **Skeletal duplicate:** a new list of `SkeletalMaterial` entries is built (each entry a copy with `material_interface`
  set, so the slot name and UV data are kept) and assigned with `set_editor_property("materials", …)`.
- **New `slots` mode** (`TEXCORRUPT_FIXTURE_MODE=slots`, run without commandlet rendering): re-sets ONLY the four
  duplicated meshes of the existing fixture, saves those four packages, fails on any other dirty package, and requires
  each expected material package name in the saved `.uasset` bytes.
- **Verify mode** (a separate process that never saves, so every load is from disk) now reads each duplicated mesh's
  slots back and proves the four producers; any failure fails the run.
- **Negative control first:** the new verify ran on the unfixed 082-06 fixture and FAILED on all four meshes
  (`verify-negative\texcorrupt_fixture_verify.json`: `WorldGridMaterial` ×3, `None` on the skeletal).

### The four read-back proofs (`texcorrupt_fixture_verify.json`, `fixture-verify.log`)

| producer | mesh (read from disk) | proof |
|---|---|---|
| `slot_empty` | `SM_TC_PlaneNoMat` | 1 slot (`lambert1`), material **None** |
| `host_mid` where=`asset_slot` | `SM_TC_PlaneAssetMid` | slot = `M_TC_AssetMid`, whose `Tex` = `T_TC_AssetMid` |
| `nanite_override` | `SM_TC_CubeNanite` | Nanite **enabled**; slot = `M_TC_NaniteBase`, `used_with_nanite` True, override `OverrideMaterialRef="/Game/CaptureBenchTexCorrupt/Materials/M_TC_NaniteOverride…"`, `bEnableOverride=True` |
| `default_material_path` | `SK_TC_Cube` | slot `MaterialSlot` = `M_TC_SkelNoUsage`, `used_with_skeletal_mesh` **False** |

Each verify read is tied to the saved bytes: the SHA-256 recorded by the slots run after each save equals the one the
verify process read (4/4). `WorldGridMaterial` no longer appears in three of the saved files; in `SM_TC_CubeNanite` it
remains only as that slot's NAME.

## 2. Manifests (CaptureBench `texcorrupt_bytecheck.ps1`, baseline 082-05 M0)

| manifest | when | vs M0 | vs the previous manifest |
|---|---|---|---|
| M1pre | before anything | CLEAN (2,082 unchanged, 104 allowed) | ≡ 082-06's M2 (0 differences) |
| M1′a | after the slots run | CLEAN | exactly the 4 mesh packages changed (`SK_TC_Cube` `aa55d26e→c67fe6e6`, `SM_TC_CubeNanite` `987ea98d→08f3dc44`, `SM_TC_PlaneAssetMid` `2c68a9d3→0cb3efc4`, `SM_TC_PlaneNoMat` `eb2b80ac→d2f85d7c`) |
| **M1′** | after the verify run | CLEAN | ≡ M1′a (verify wrote nothing) |
| M2a | after cooks d1–d3 | CLEAN | ≡ M1′ |
| M2′-prestage | after cooks d4–d13 | CLEAN | ≡ M1′ |
| **M2′** | after staging + A44 (the ruling's order) | CLEAN | ≡ M1′ |

`CB_GateLevel.umap` `1d89de17…`, `MainWorld.umap` `a3849daa…` and all 421 MainWorld externals are byte-identical in every
manifest. One harness slip, no tree effect: a first M1′a call piped the child through `Select -First 1`, which stopped
it before it wrote the manifest; it was re-taken without truncation (runbook §8.6a).

## 3. The cooks

Same command as 082-06 (`cook-d1-command.txt`), each archived to its own fresh E: directory `_r53_cookout_d1…d13`. All 13:
BUILD SUCCESSFUL, **899 packages** (082-06: 894; the 5 formerly missing now cook), 0 iteratively skipped, 0 warnings,
0 errors; ~45 s each warm. **d1 was named the candidate before any G-COOK reading; d2…d13 are controls.**

| cook | exe | `StackOBot-Windows.utoc` | `.ucas` | `.pak` | `global.utoc` / `.ucas` |
|---|---|---|---|---|---|
| **d1 (candidate)** | `2FCDF059` | `9A26D497` | `10D7F0C0` | `ABD931A2` | `462B8AC6` / `BB05CF99` |
| d2 | `2FCDF059` | `35CEE54F` | `88398A56` | `67AFB7BE` | same |
| d3 | `2FCDF059` | `6AF12C60` | `846658B6` | `67AFB7BE` | same |

Map gate on d1 (`mapgate-d1.txt`): **exit 0**, `CB_GateLevel`, `MainMenu`, `MainWorld`, `Entry`, `CB_TexCorruptLevel` all
PRESENT; the default-set run still names `CB_TexCorruptLevel` as unexpected (exit 2, the negative control). A first call
through `powershell -File` passed `-Required` as one string and printed a false "omitted"; re-run in-process (§8.6a).

## 4. G-COOK under the amended rule (`gcook_082_06d.py`, `gcook-082-06d-n13.json`)

The reader compares at CONTAINER level: each container's `MainWorld` is extracted with `UnrealPak -Extract` into its
`.uheader` (the zen header) and `.uexp`, and so is the archived pre-m53 container A (`67EA1FE0`).

| row | reading | verdict |
|---|---|---|
| M1′ / M2′ vs M0 outside the allowed paths | clean, and M2′ ≡ M1′ | ✅ |
| archive verified before the cook | 082-06's `m53-s1-precook-container-67EA1FE0\`, 6/6 (unchanged, ruling: stays valid) | ✅ |
| map gate, new level named | exit 0 | ✅ |
| `CB_GateLevel` exact | `0x01AD80B5B5F83E…` in A and all 13 cooks | ✅ |
| `MainWorld` header | `.uheader` SHA-256 `b50c9352…` in A and all 13; `.uexp` 7,728,196 B in all 14; chunk 7,898,974 B in all | ✅ |
| `MainWorld` control pair | diff(A, d1) outside the 13-cook region: **0 words at each of the 4 alignments, 0 bytes**; the same for diff(A, dN), every N; R′ built without d1: 0 | ✅ |
| fixture packages present | **101/101** fixture + **3/3** plugin assets in d1 (and in every cook) | ✅ |
| D (reported) | A→d1 non-shader: 927 unchanged, 6 changed (the 4 shader archives, `MainWorld`, the container header), 0 removed, 111 added — the fixture, the 3 plugin assets, and `SkeletalCube_Skeleton` as a dependency | D |

### 4.1 How the 13 came about — stated in full

- The rule I pre-declared in the reader (before reading anything): R = the union of the pairwise differences of the
  identical-input cooks; PASS iff diff(A, d1) lies inside R at **every** one of the four 4-byte word alignments. This is
  stricter than the ruling, which names no granularity.
- **With d1–d3 it FAILED:** outside R = 4 / 20 / 25 / 0 words; 49 bytes outside at byte level; the same counts for
  diff(A, d2) and diff(A, d3), i.e. bytes where all three new cooks agreed and A did not (`gcook-082-06d.json`, the
  3-cook record, kept).
- **Read before acting** (`mw_hex_probe.py`): the varying data are 16-byte (X, Y, Z, V) records with V a random value in
  [0, 1]; every differing byte in any cook lies inside a V field; the 49 bytes are all V's most-significant byte
  (`0x3d/0x3e/0x3f`), where the three new cooks had landed in the same binade. The V fields start at different residues
  mod 4 in different arrays, so a fixed-alignment word pairs V's top byte with constant bytes.
- **The ruling's route, fixed in writing first** (`gcook-predeclare-extra-cooks.md`, SHA-256 `A251B5D3…`, 06:23, before
  d4 started): exactly **10 more identical-input cooks** (d4–d13), the candidate unchanged, the predicate unchanged, the
  growth curve reported. A byte that differs systematically between A and the new inputs cannot enter the region however
  many cooks run.
- Byte-region growth by cook count: 1610, 1721, 1772, 1788, 1798, 1798, 1799, **1800**, 1800, 1800, 1800, 1800 — flat
  from the 9th cook on. At 13 cooks every alignment reads 0 outside. **PASS.**
- ⚠ **For review:** the verdict rests on the ruling's "run another identical-input cook" clause applied ten times under a
  count fixed in advance. The 3-cook failure and its hex reading are in the evidence unedited. G312.

### 4.2 A check that was attempted and is NOT obtained

A string scan of the cooked `M_TC_SkelNoUsage` for `bUsedWithSkeletalMesh` read 0 — but the positive control
`M_CorruptedTexture_Pink` (the flag is true) also read 0: zen packages do not carry property names as plain text. The
instrument is blind (G96), so no cooked-flag claim is made. Engine source says only a scene proxy sets the flag
(`SkeletalMesh.cpp:5788`, `FSkeletalMeshSceneProxy`), and M2′ ≡ M1′ shows the cook saved no source change.

## 5. Staging and A44

- Candidate archived first at `_binary_baselines\m53-s1-cook-9A26D497\`, 6/6 SHA-256 verified at the destination
  (`archive-candidate.json`), README entry written.
- **Staged 06:32 IST** into `Builds\BenchGate\Windows\StackOBot\`: the exe and the container quintet only, each verified at
  the destination (`staging-receipt.json`): `E0BE6F0A→2FCDF059`, `67EA1FE0→9A26D497`, `2CEFB8F4→10D7F0C0`,
  `E03C6610→ABD931A2`, `C70ECDAA→462B8AC6`, `A16A18A8→BB05CF99`. The launcher stub, the manifests and the PDB were not
  copied; a tree comparison showed no other build file differing, and only bench session folders exist solely on the
  staged side. Rollback: `m53-s1-precook-container-67EA1FE0\`. No bench-class process ran at staging.
- **A44 on the staged exe** (`a44-staged.txt`), UTF-16: `TEXCORRUPT-ASSETS` 1, `M_CorruptTex_UV` 4, `M_CorruptTex_Normal` 4,
  `T_CorruptTex_NoiseN` 4, `IAI.Bench.TexCorruptNoApply` 5, `collateral_unresolved` 1, `slot_empty` 1,
  `default_material_path` 1, `nanite_override` 1, `host_mid` 4; controls `IAI.Capture.ShaderPrewarm` 7,
  `IAI.Bench.StuckMipNoHold` 5, `IAI.Bench.MaskPairingProbe` 5; ASCII 0 throughout (the `TEXT()` literals are UTF-16).

## 6. Deviations, each with its reason

1. **The tool was re-run in a new targeted `slots` mode**, not full author mode: the ruling says "only the affected
   packages", and a full author run re-imports every texture (new package bytes for all 101). M1′a shows exactly four
   files changed.
2. **13 cooks instead of 2–3** (§4.1), under the ruling's clause and a count written down before they ran.
3. **The cooked usage-flag check** (§4.2) was attempted and discarded as blind.
4. **M2′ was taken twice** (after the cooks and after staging); both are ≡ M1′.

## 7. State at the end

- CaptureBench (local-only): `47d6fff`. Plugin branch: this journal, the plan's revision 3.4 (§R0.00000 and the three
  🔁 082-06d rows), the runbook's §8.6a, G312 and the status block, one `docs` commit, pushed. `m51`, `master`, tags
  untouched; no tag.
- The D: project's `Content\CaptureBenchTexCorrupt\` holds the fixed fixture (101 assets).
- **Bench: exe `2FCDF059` + `9A26D497` / `10D7F0C0` / `ABD931A2` + `462B8AC6` / `BB05CF99`.**
- Kept on E:: `_r53_cookout_d1…d13` (~0.6 GB each), `_r53_extract` (the MainWorld extractions), 082-06's `_r53_cookout`
  and `_r53_cookout_ctl`; the scratch host, its junctions and both worktrees (for 082-07).

## 8. NEEDS-DECISION

None blocking. For review: §4.1's route to the `MainWorld` PASS (3-cook failure, hex reading, ten more cooks fixed in advance).
