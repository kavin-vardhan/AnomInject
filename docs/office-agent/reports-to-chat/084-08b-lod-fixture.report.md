# REPORT for 084-08b-lod-fixture.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-08b-lod-fixture.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 06:52:29 | finished: 2026-09-29 07:58:02 | duration: 66 min | exit code: 0
context at last turn: 681620 tokens (68% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-08b-lod-fixture.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Plain summary for the owner:** The LOD test map now exists. It has two objects that switch from an orange sphere to a blue cube when their detail level is forced, one with the "dithered transition" material setting and one without. It is cooked into a new test build, the night bench is set up to run it, and a full practice run with the game stubbed out passed. Nothing was launched, staged or merged, and no plugin code changed.

**The fixture** (`/Game/AnomalyFixtures/CB_LodFixture`)
- **Camera:** the PlayerStart at (0, 0, 200), looking along +X. As on `CB_GateLevel`, the game mode finds no spawn pad, so the view sits exactly at the PlayerStart.
- **Two targets:** `CB_LodFix_N` at (850, −340, 200) and `CB_LodFix_D` at (850, 340, 200). Both are scaled 4.6, static, and cast no shadow.
- **Meshes:** LOD0 is the engine sphere (960 triangles, orange). LOD1, the forced LOD, is the engine cube (48 triangles, blue). Each LOD has its own material slot. Screen sizes are set to [1.0, 0.05], and neither mesh is Nanite.
- **Materials:** four Lit materials. Variant D has Dithered LOD Transition on for both of its LOD materials; variant N has it off.
- **Selection rule numbers:** the declared rule, unchanged, selects both targets.
  - Screen size is 0.4466, above LOD1's 0.05, so LOD0 is the natural LOD.
  - Bounds coverage is 26.96 % each (the rule allows 7–60 %, the brief asked 10–40 %).
  - Triangle ratio is 20 (the rule needs at least 8).
  - Drawn silhouette is 11.93 % for the sphere and 26.43 % for the cube.
- **Authoring:** done off-project on the scratch host, read back from disk in a separate process, then the 7 files were copied into D:. The copy is backed up at `_binary_baselines\m52fix-cook-BF06AE61\fixture-source\`.

**The cook**
- **Pairing:** exe `8A6074AA` (unchanged) + utoc `BF06AE61` / ucas `B294A1C8` / pak `87FF0240` + globals `462B8AC6` / `BB05CF99` (unchanged).
- **Result:** 801 packages (794 + 7), 0 warnings, exit 0. Archived at `_binary_baselines\m52fix-cook-BF06AE61\`, 6 of 6 files re-hashed there.
- **Content manifest diff:** `E4C7F38D…` (2,182 files) → `7A5D8D8D…` (2,189 files). The diff is exactly 7 added files, all under `Content\AnomalyFixtures\`, with nothing changed or removed. The manifest was also identical before and after the cook.
- **Descriptor:** the cooked `AnomalyInjector.uplugin` is byte-identical to the branch and to 084-08's cook (`9EFB9B49…`). `AnomalyBench` is first and denied in Shipping.
- **Map gate:** 5 of 5 maps present, including `CB_LodFixture`; the check that `CB_TexCorruptLevel` must be absent reads MISSING as intended.
- **Cook prediction 6 was only partly right, and I'm reporting it as missed.** I also compared the cooked containers chunk by chunk against 084-08's cook:
  - The 9 fixture chunks were added, as predicted.
  - `MainWorld.umap` and the two project shader archives also changed. Earlier cooks of identical content change exactly those three every time, so this is existing cook-to-cook variation, not the fixture (G371).
  - Two engine `FlipBook` entries changed only in path capitalisation; their bytes are identical.
  - One shader-code chunk out of 1,039 was replaced. That fits the new materials' shaders being regrouped, but I haven't established why.

**The harness**
- **Pins:** the new pairing is pinned in `084-09-common.py`. Preflight now reads the fixture map out of the archived container.
- **New legs (10, all required):**
  - `LODP_FN` and `LODP_FD` are the premise legs (AA off, native order, 300 frames).
  - `LOD_F{N,D}_{NAT,SYN,NAT_AAOFF,SYN_AAOFF}` are the gate legs: 720 frames each, about 22 judged events per leg. Each runs only after its own variant's premise passes.
- **Rock legs:** the rock premise legs are kept as the realistic check. The rock gate legs still need `--with-lod-gate`.
- **Gate:** at least 16 judged events and 0 off frames at 50 % outside transition frames. Fewer than 16 judged reads INSUFFICIENT, never PASS.
- **Fade report:** per frame effect level at each pop edge; two or more partial frames at one edge count as a multi-frame fade. On variant D that is a finding and is never excused; variant N is the comparison at the same condition.
- **Predictions (declared before any bench run):**
  - Both premises pass, at 100 % of eligible events measurable.
  - All 8 gate legs pass.
  - No multi-frame fade on D or N with AA off. The engine source (`StaticMeshRender.cpp:1068-1070`, `:2359-2361`) switches dithering off whenever a LOD is forced (G372). This is a source reading; the bench decides.
- **Gate selftest:** 42/42 (was 24), including 7 new checks that must be able to fail.
- **Expected time:** about 87 minutes, about 100 with retries and waits.
- **Full dry run:** exit 0 with 47 of 47 launches accepted. The restore beat the injected file lock on attempt 3.
  - A dry-only switch forced the premises so the gate legs would actually launch; a live run refuses that switch.
  - On the stand-in data (old rock sessions) every LOD gate leg reads INSUFFICIENT, which is the correct answer.
  - A focused re-run of the 10 fixture legs on the final code also exited 0, with 10 of 10 accepted.

**Commits**
- `fix/m52-label-timing` `37bb750`, pushed (`17b9d5d..37bb750`): journal `docs/sessions/2026-09-29-084-08b-lod-fixture.md`, status block, gotchas G370–G373.
- CaptureBench `05adeee`: the authoring tool `tools/make_lod_fixture.py`. That repo has no remote, so it is committed locally only.
- Untouched: `m51`, `master`, tags, the m53 branch and files, and the staged build (still the m53 set).

**NEEDS-DECISION:** none.

Separately: the pdf-viewer MCP server failed to connect this session, and several other MCP connectors need authorising via `/mcp` or claude.ai. None of them were needed for this task.
api-equivalent cost (not billed on Max): $22.62 | turns: 138 | running total across all briefs: $3,398.51
