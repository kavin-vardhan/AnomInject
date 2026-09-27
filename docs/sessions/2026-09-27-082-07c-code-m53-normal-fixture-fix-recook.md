# 082-07c — m53 S1: normal-family fixture materials fixed, re-cooked, G-COOK passes, STAGED

Session 082-07c, 2026-09-27, Claude Code (Opus 5.5), headless from the GDP mailbox. Ruling:
`_reviews/082-07c-chat-ruling-s1-legs-red.md` item 1 and its Sequencing line (082-07b's RED at G3 N-N1 is a fixture
defect: the normal readout materials never connected Normal). Branch `feat/m53-uv-normal-corruption`. Evidence:
`D:\IntrusiveAnomalies\_reviews\082-07c-evidence\`.

**Outcome: the four normal-family readout materials are fixed and proven from disk; three identical-input cooks ran
clean; G-COOK passes under the amended rule; the new container is STAGED and A44-green.** No leg, no game launch, no plugin
source change, no tag.

## 0. Where the work ran

- Main checkout untouched on `m51` at `53bf725` (`master` `b5f15a3`). Plugin docs on the branch worktree
  `E:\IA_BuildCache\_r53_src\AnomalyInjector`.
- Commandlets and cooks on the scratch host `E:\IA_BuildCache\_r53_host\StackOBot` (plugin worktree detached at `c9f8089`,
  `Content` junctioned to the real D: content). Commandlets only, with a byte manifest bracketing each run (runbook §8.6a).
- Environment at start: no editor, no game, no cook running; only `UnrealTraceServer` (not ours, untouched).

## 1. The fixture fix (CaptureBench `41ef8b9`, `tools/make_texcorrupt_fixture.py`, SHA-256 `CCB8AD13…`)

- **Cause (082-07b, G314):** `readout(kind="normal")` sent `N.rgb * 0.5 + 0.5` into Emissive and left Normal unconnected, in
  an Unlit material. The feature's V1 A3 rule (`Root->IsPropertyConnected(MP_Normal)`, `TexCorruptTree.cpp:571`) correctly
  refused all seven normal targets `normal_unconnected`.
- **Fix:** `connect_normal()` sets `MSM_DEFAULT_LIT` and connects the `Tex` sampler's `RGB` into `MP_Normal`. The
  `NormalReadout` emissive node is kept. `author` mode uses it for every `kind="normal"` readout.
- **New `normals` mode** (`TEXCORRUPT_FIXTURE_MODE=normals`, no rendering): loads ONLY `M_TC_UN1`, `M_TC_NN1`,
  `M_TC_ChainD`, `M_TC_NormalEmis`. For each one it:
  - checks that `Tex` is that material's own normal map;
  - finds the single `Tex` sampler that feeds `NormalReadout` and connects it into Normal, so nothing else in the graph
    changes;
  - recompiles and saves.

  It fails on any other dirty package.
- **Verify mode** now also runs `normal_proofs()`, in a separate process that saves nothing, so every load is from disk.
  Each of the four materials must pass all of these:
  - the shading model is Default Lit;
  - `MP_Normal` is fed by the `Tex` sampler;
  - Emissive is still fed by `NormalReadout`;
  - `Tex` is the material's only texture parameter;
  - `Tex` is bound to its BC5-class normal map (`TC_NORMALMAP`, sRGB off).

  It also checks that **no other fixture material or instance binds a normal map**, so the four cover "any other
  normal-family target". Any failed check fails the run.
- **Negative control first:** the new verify ran on the unfixed fixture and **FAILED on exactly 8 checks**, "MP_Normal fed by
  None" and "MSM_UNLIT" for each of the four (`verify-negative\texcorrupt_fixture_verify.json`). The 4 slot proofs still passed
  and nothing else was flagged.

### The read-back proofs (`texcorrupt_fixture_verify.json`, `fixture-verify.log`, exit 0, 0 problems)

| material | saved SHA-256 (tool) = read back | shading | `MP_Normal` ← | Emissive ← | params | `Tex` |
|---|---|---|---|---|---|---|
| `M_TC_UN1` | `01ac9c26→5496487b` | Default Lit | `TextureSampleParameter2D` `Tex`.`RGB` | `Custom:NormalReadout` | `Tex` | `T_TC_UN1`, normal map, sRGB off |
| `M_TC_NN1` | `0498a250→ec4a0fe9` | Default Lit | same | same | `Tex` | `T_TC_NN1`, same |
| `M_TC_ChainD` | `9c46a541→95dfe8e5` | Default Lit | same | same | `Tex` | `T_TC_ChainD`, same |
| `M_TC_NormalEmis` | `04f121de→450a185a` | Default Lit | same | same | `Tex` | `T_TC_NormalEmis`, same |

- Normal maps bound outside these four: **none**. All 4 slot proofs are PROVEN. All 46 materials compile, and the 49
  textures match the image manifest.
- Editor shader stats for the four: 100 → **191** pixel instructions, 1 → **2** samplers. The texture parameters are unchanged
  (`Tex` only), and `GetUsedTextures` still returns only the normal map (§5).

## 2. Manifests (CaptureBench `texcorrupt_bytecheck.ps1`, baseline 082-05 M0)

| manifest | when | vs M0 | vs the previous manifest |
|---|---|---|---|
| M1pre | before anything | CLEAN (2,082 unchanged, 104 allowed) | ≡ 082-06d's M2′ (0 differences) |
| M1′a | after the normals run | CLEAN | exactly the 4 material packages changed (hashes above) |
| **M1′ (= M1″ of the brief)** | after the verify run | CLEAN | ≡ M1′a (verify wrote nothing) |
| M2″-prestage | after the offline check and cooks e1–e3 | CLEAN | ≡ M1′ |
| **M2″** | after staging + A44 | CLEAN | ≡ M1′ |

`CB_GateLevel.umap` `1d89de17…`, `MainWorld.umap` `a3849daa…` and all 421 MainWorld externals are byte-identical in every
manifest.

## 3. The cooks

- The G-COOK rule was written first in `gcook-predeclare.md` (SHA-256 `90000A4A…`, 10:06:53, before e1 started):
  - three identical-input cooks, with **e1 the candidate** and e2 and e3 as controls;
  - the `MainWorld` region is 082-06d's 13 cooks plus this session's 3;
  - if Q-MW fails, stop and do not stage.
- Same command as 082-06d (`cook-e1-command.txt`), each cook archived to a fresh `E:\IA_BuildCache\_r53_cookout_e1…e3`.
- All three: BUILD SUCCESSFUL, "Target is up to date", **899 packages**, 0 warnings, 0 errors.

| cook | exe | `StackOBot-Windows.utoc` | `.ucas` | `.pak` | `global.utoc` / `.ucas` |
|---|---|---|---|---|---|
| **e1 (candidate)** | `2FCDF059` | `30FE0FDE` | `11231356` | `FD766B7B` | `462B8AC6` / `BB05CF99` |
| e2 | `2FCDF059` | `061512F8` | `CCCE7F8C` | `FD766B7B` | same |
| e3 | `2FCDF059` | `D11D95FD` | `A0D89E45` | `FD766B7B` | same |

⚠ **The runner hung after e1 (G316).** PowerShell 5.1's `Start-Process -Wait` waits for the whole process tree, and e1 had
spawned six MSBuild node-reuse processes.
- Checks before stopping them: their parent PID was e1's AutomationTool, they were created one second after the runner, and
  their command line was `/nodemode:1 /nodeReuse:true`. They were ours.
- The runner and the six nodes were stopped **by PID**, not by name. e1's IoStore list and `MainWorld` extraction were then
  run by hand.
- e2 and e3 ran under a copy of the runner with `MSBUILDDISABLENODEREUSE=1` and `.WaitForExit()`, and took 52 s and 50 s.

Map gate on e1, run in-process (`mapgate-e1.txt`): **exit 0**. `CB_GateLevel`, `MainMenu`, `MainWorld`, `Entry` and
`CB_TexCorruptLevel` are all PRESENT. The default-set run exits 2 on `CB_TexCorruptLevel`, the same negative control as
082-06d.

## 4. G-COOK (`gcook_082_07c.py`, `gcook-082-07c.json` / `.txt`)

| row | reading | verdict |
|---|---|---|
| M1″ / M2″ vs M0 outside the allowed paths | clean; M2″ ≡ M1′ | ✅ |
| map gate | exit 0 | ✅ |
| `CB_GateLevel` exact | chunk `0x01AD80B5B5F83E…` in A, e1, e2, e3 | ✅ |
| `MainWorld` header | `.uheader` `b50c9352…` identical in **17 containers** (A, d1–d13, e1–e3); `.uexp` 7,728,196 B in all | ✅ |
| `MainWorld` control pair | diff(A, e1): 1,617 bytes, **0 outside R** at each alignment (R = 899 / 855 / 700 / 699 words), 0 bytes; the same for e2 and e3 | ✅ |
| fixture packages present | **101/101** fixture + **3/3** plugin in e1, e2, e3 | ✅ |
| Q-NORM (reading) | d1 → e1 non-shader: exactly the **4 normal materials** changed, plus the two shader archives, `MainWorld` and the container header; 0 removed, 0 added | D |

Readings beside the verdict:
- diff(A, e1) is **0 bytes outside 082-06d's 13-cook region on its own**, so the extension was not needed to pass.
- Against this session's 3-cook region alone, 39 bytes fall outside. That is the same small-sample effect as 082-06d §4.1.
- This session's region is contained in 082-06d's (0 bytes outside).
- e1 → e2/e3 non-shader differences are only the shader archives and `MainWorld`.

## 5. The offline admission check (`offline_admission.py`, `offline_admission.json`, a read-only commandlet)

- **Method.** For each of the seven normal-family targets (`TC_UN1`, `TC_NN1`, the four `TC_ChainD_*` and `TC_NormalEmis`,
  i.e. `StaticMeshActor_11/12/22/32/42/52/73`), the script mirrors the static part of the V1 tree:
  - slot steps S1–S8;
  - binding steps B1–B6, B9 and B10;
  - A2, A3, A5 and A6;
  - V2, with an empty ledger against the compiled 128 MiB cap.

  It reads the assets on disk. A3 is read with `GetMaterialPropertyInputNode(MP_Normal)`. That reads the same
  `GetExpressionInputForProperty` as the engine's cached-expression builder, whose bitmask the cooked `IsPropertyConnected`
  returns.
- **Result: 7 of 7 would be admitted**, and no package was dirtied.
  - Required bytes: 109,224 for each 128² map and 27,304 for the 64² ChainD.
  - Every target has one binding, `Tex`, on its own normal map, in group `WORLD_NORMAL_MAP`, with a full chain, cinematic 0
    and LOD bias 0.
  - `GetUsedTextures` returns only that map, so Lit added no material texture binding.
- **Corroborating in-engine evidence:** 082-07b's G0 read all seven as uv `APPLY`, on the same single `Tex` binding. The
  normal family's requirement for a BC5 normal map is identical (`bSRGB` false in both families). So the binding steps, A5,
  A6 and V2 already held in-engine, and only A2 and A3 are family-specific.
- ⚠ **The first run read A5 FAIL on all seven. That was my reader, not the asset (G317):** `blueprint_get_size_x()` returned
  the 32×32 async-compile placeholder. The re-run reads the saved `Dimensions` tag. The first run is kept
  (`offline_admission-run1-placeholder-size.json`).
- **Deferred to the in-engine G0 census (082-07e):**
  - the runtime host-MID link;
  - the cooked shader map being present and complete;
  - the cooked pixel format and mip sizes;
  - resource-ready and streaming state;
  - full residency;
  - A3 as the cooked bitmask.

## 6. Staging and A44

- The build being replaced (`2FCDF059` + `9A26D497` / `10D7F0C0` / `ABD931A2` + `462B8AC6` / `BB05CF99`) was verified 6/6
  against its existing archive `_binary_baselines\m53-s1-cook-9A26D497\`, which stays as the rollback.
- The candidate was archived at `_binary_baselines\m53-s1-normalfix-cook-30FE0FDE\`, full SHA-256 6/6 at the destination
  (`archive-candidate.json`). README entry written.
- **Staged 10:21 IST** into `Builds\BenchGate\Windows\StackOBot\`: the exe and the container quintet only, each verified at
  the destination (`staging-receipt.json`):
  - `2FCDF059→2FCDF059`
  - `9A26D497→30FE0FDE`
  - `10D7F0C0→11231356`
  - `ABD931A2→FD766B7B`
  - `462B8AC6` and `BB05CF99` unchanged.

  No `StackOBot` process was running at staging.
- **A44 on the staged exe** (`a44-staged.txt`), all in UTF-16 with ASCII 0 throughout:

  | token | count |
  |---|---|
  | `TEXCORRUPT-ASSETS` | 1 |
  | `M_CorruptTex_UV` | 4 |
  | `M_CorruptTex_Normal` | 4 |
  | `T_CorruptTex_NoiseN` | 4 |
  | `IAI.Bench.TexCorruptNoApply` | 5 |
  | `collateral_unresolved` | 1 |
  | `slot_empty` | 1 |
  | `default_material_path` | 1 |
  | `nanite_override` | 1 |
  | `host_mid` | 4 |
  | `normal_unconnected` | 1 |
  | `no_normal_map` | 1 |
  | control `IAI.Capture.ShaderPrewarm` | 7 |
  | control `IAI.Bench.StuckMipNoHold` | 5 |
  | control `IAI.Bench.MaskPairingProbe` | 5 |

## 7. For 082-07d / 082-07e (not blocking)

1. **`M_TC_UN1` changed, and U-N1 is one of the 12 accepted UV rows.** The ruling named it for re-authoring. Its 082-07b
   result was taken on the Unlit material, so the bank re-evaluation (ruling item 4) judges old-fixture frames. Whether U-N1
   also re-runs on this build is for chat. G-ID is unaffected in kind, because it compares applied and null runs in the same
   build.
2. **The four tiles are now lit.** BaseColor is unconnected (default black), with default Specular and Roughness, so their
   pixels include a specular term shaped by the normal map. No comparison against 082-07b-banked frames of those tiles is
   valid.
3. The in-engine G0 census must read `APPLY` on the normal family for all seven targets before any normal Q row. That is the
   ruling's item 3 pre-check.

## 8. State at the end

- CaptureBench (local-only): `41ef8b9`. Plugin branch: this journal, the status block, G316 and G317, one `docs` commit,
  pushed. `m51`, `master` and tags untouched; no tag.
- **Bench: exe `2FCDF059` + `30FE0FDE` / `11231356` / `FD766B7B` + `462B8AC6` / `BB05CF99`.**
- Kept on E: `_r53_cookout_e1…e3`, `_r53_extract\e1…e3`, and everything 082-06d kept.
- ⛔ The 082-07 boundary is stale after this commit (it already was after 082-07b). 082-07d re-issues it.

## 9. NEEDS-DECISION

None blocking. §7 items 1 and 2 are for chat's 082-07d scope.
