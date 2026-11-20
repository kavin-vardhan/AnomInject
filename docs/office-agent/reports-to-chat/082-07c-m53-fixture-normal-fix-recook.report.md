# REPORT for 082-07c-m53-fixture-normal-fix-recook.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-07c-m53-fixture-normal-fix-recook.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-27 09:56:07 | finished: 2026-09-27 10:25:04 | duration: 29 min | exit code: 0
context at last turn: 470190 tokens (47% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-07c-m53-fixture-normal-fix-recook.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Plain summary for the owner**
- The test scene's four normal-map materials now actually use their normal map. They were shading with it only as a colour readout, which is why normal corruption was refused in 082-07b.
- I proved the fix by reading the saved files back from disk, rebuilt the game package three times, and every package check passed. The new package is installed on the bench.
- No game run happened this session. The next step (082-07d) fixes the test harness before the legs resume.

**Fixture fix and read-back proofs**
- **Change:** CaptureBench `41ef8b9`, `tools/make_texcorrupt_fixture.py`. The normal readouts are now Default Lit, and the `Tex` sampler's `RGB` feeds `MP_Normal` as well as the existing Emissive readout.
- **Targeted re-author:** a new `normals` mode edited only `M_TC_UN1`, `M_TC_NN1`, `M_TC_ChainD` and `M_TC_NormalEmis`. It connected the existing `Tex` sampler and changed nothing else in each graph. Nothing else was left unsaved.
- **Negative control first:** the new verify check, run on the unfixed scene, failed on exactly 8 checks (for each of the four: Normal unconnected, and Unlit). The 4 slot proofs from 082-06d still passed.
- **Verify on the fixed scene** (a separate process that saves nothing, so every read is from disk): exit 0, 0 problems. The saved hashes match the ones the tool recorded after each save:

| material | hash before → after | shading | Normal fed by | Emissive fed by | texture |
|---|---|---|---|---|---|
| `M_TC_UN1` | `01ac9c26 → 5496487b` | Default Lit | `Tex.RGB` | `NormalReadout` | `T_TC_UN1`, normal map, sRGB off |
| `M_TC_NN1` | `0498a250 → ec4a0fe9` | Default Lit | `Tex.RGB` | `NormalReadout` | `T_TC_NN1`, same |
| `M_TC_ChainD` | `9c46a541 → 95dfe8e5` | Default Lit | `Tex.RGB` | `NormalReadout` | `T_TC_ChainD`, same |
| `M_TC_NormalEmis` | `04f121de → 450a185a` | Default Lit | `Tex.RGB` | `NormalReadout` | `T_TC_NormalEmis`, same |

- No other scene material or instance binds a normal map, so these four cover every normal-family target. All 4 slot proofs still pass, and all 46 materials compile.

**M1″ and M2″** (baseline 082-05 M0)
- **Before any edit:** clean, and identical to 082-06d's last manifest.
- **After the re-author:** clean; exactly those 4 packages changed.
- **M1″ (after verify):** clean, identical to the post-edit manifest, so verify wrote nothing.
- **M2″ after the cooks, and again after staging:** clean, identical to M1″.
- `CB_GateLevel.umap`, `MainWorld.umap` and all 421 MainWorld externals are byte-identical in every manifest.

**G-COOK readings** (rule written first in `gcook-predeclare.md`, SHA-256 `90000A4A…`, before any cook; e1 is the candidate, e2 and e3 are controls)
- All three cooks: build step "Target is up to date", 899 packages, 0 warnings, 0 errors.
- **Map gate:** exit 0 with all five maps present. The default-set run exits 2 on `CB_TexCorruptLevel`, the expected negative control.
- **`CB_GateLevel`:** exact, the same chunk hash in the pre-m53 container and all three cooks.
- **`MainWorld`:** the header is identical across 17 containers (pre-m53, 082-06d's 13 cooks, and e1–e3), and the export data is the same size in all of them.
  - diff(pre-m53, e1) is 1,617 bytes, with 0 outside the variance region at each of the four alignments (082-06d's 13 cooks plus this session's 3).
  - It is also 0 bytes outside 082-06d's region on its own, so the extension was not needed to pass.
  - e2 and e3 are also 0 outside.
- **Fixture:** 101/101 scene packages and 3/3 plugin assets present in every cook.
- **What changed from the previous build (082-06d's staged cook) to e1:** the 4 materials, plus the shader archives, `MainWorld`'s known-variable bytes and the container header. Nothing was removed or added.
- **Runner incident:** my cook runner hung after e1. PowerShell 5.1's `Start-Process -Wait` waits for the whole process tree, and the cook had left six idle MSBuild processes behind (G316).
  - I confirmed they were this cook's own by parent PID and creation time, and stopped them by PID, not by name.
  - e2 and e3 then ran with MSBuild node reuse disabled.

**New staged hashes and A44**
- **Staged 10:21 IST**, each file verified at the destination:
  - exe `2FCDF059`, unchanged;
  - `StackOBot-Windows.utoc` `9A26D497 → 30FE0FDE`;
  - `.ucas` `10D7F0C0 → 11231356`;
  - `.pak` `ABD931A2 → FD766B7B`;
  - `global.utoc` / `global.ucas` `462B8AC6` / `BB05CF99`, unchanged.
- **Archives:** the new build is at `_binary_baselines\m53-s1-normalfix-cook-30FE0FDE\`, 6/6 by full SHA-256. The replaced build is already archived and verified 6/6 at `m53-s1-cook-9A26D497\`, which is the rollback. The provenance entry is in the README.
- **A44:** green, UTF-16 throughout. Every m53 token reads as it did in 082-06d, plus `normal_unconnected` 1 and `no_normal_map` 1. Controls: `ShaderPrewarm` 7, `StuckMipNoHold` 5, `MaskPairingProbe` 5.

**Offline admission check**
- All 7 normal targets would be admitted: `StaticMeshActor_11`, `_12`, `_22`, `_32`, `_42`, `_52` and `_73`. The check mirrors every static V1 step (the slot steps, the binding steps, A2, A3, A5 and A6) and V2, reading the assets on disk.
  - The required size is 27–109 KB against the 128 MiB cap.
  - Each target has one binding, `Tex`, on its own normal map, and Lit added no texture binding.
- **Supporting evidence:** in 082-07b's census all seven already read UV `APPLY` on the same binding, so only the normal-specific checks A2 and A3 were in doubt.
- **A first run of my check wrongly refused all seven for being too small.** It read the placeholder size a texture reports while it is still compiling (G317). The re-run reads the size saved with the asset. The first run is kept in the evidence.
- **Left to the in-engine census in 082-07e:** the runtime-only steps (residency, streaming state, the cooked shader map, and A3 as read in the cooked build).

**Commits**
- CaptureBench `41ef8b9`.
- Plugin `9bdc6f6` on `feat/m53-uv-normal-corruption`, pushed: the journal `docs/sessions/2026-09-27-082-07c-code-m53-normal-fixture-fix-recook.md`, the status block, and gotchas G316 and G317.
- `m51` is at `53bf725`, `master` at `b5f15a3`. No tag. Evidence is in `_reviews\082-07c-evidence\`.

**NEEDS-DECISION**
None blocking. Two items for 082-07d:
1. `M_TC_UN1` changed, and U-N1 is one of the 12 accepted UV rows. Its banked result is on the old Unlit material, so the bank re-evaluation judges frames from the old scene. Should U-N1 also re-run on this build?
2. The four normal tiles are now lit (black base colour, default specular), so their pixels include a specular term. None of 082-07b's banked frames of those tiles is valid for comparison against the new build. G-ID is unaffected, because it compares corrupted and uncorrupted runs within the same build.

The 082-07 boundary was already stale after 082-07b and stays stale after this commit; 082-07d re-issues it.
api-equivalent cost (not billed on Max): $11.79 | turns: 91 | running total across all briefs: $2,689.06
