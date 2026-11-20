# REPORT for 082-05-m53-s1a-source-builds.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-05-m53-s1a-source-builds.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-27 02:01:56 | finished: 2026-09-27 03:23:38 | duration: 82 min | exit code: 0
context at last turn: 959541 tokens (96% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-05-m53-s1a-source-builds.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary**
- The four plan fixes (A, B, C, P3) are in the plan as revision 3.1, commit `ea1eed1`, pushed.
- All S1 plugin code is committed in 8 per-area commits (`2a2ab17` … `bb048bb`), plus a docs commit, `193bd35`. Everything is pushed; local == origin.
- Both builds pass G10 with 0 warnings, and every plugin module was compiled from the committed code. The new game exe is **`D7BA87AC`**; it is archived and **not staged**. The bench stays on `E0BE6F0A`.
- Until 082-06 authors the two corruptor materials and the noise texture, every m53 fire refuses `assets_unavailable`. Nothing crashes.
- Not done, by design: no leg, no game or editor launch, no cook, no content, no CaptureBench edit, no tag, no merge. `m51`, `master` and CaptureBench were not touched.

**Commits**

| hash | area | lines |
|---|---|---|
| `ea1eed1` | plan revision 3.1: fixes A/B/C/P3 and a new §R0.00 resolution table | +68 −15 |
| `2a2ab17` | private `RenderCore`/`RHI` in `Build.cs`; the invariant wording in `architecture.md` and `CLAUDE.md` | +21 −4 |
| `9e9a1c3` | shared state: refusal reasons, corruptor parameter contract, three knobs, budget ledger with scratch, run stats, `TexCorruptPure.h`, offline harness | +1,352 |
| `782bb5b` | decision tree, active bindings, residency and bias checks (including C's `streaming_budget`), the Δ1 chain predicate, G0 census, the m52 query | +971 |
| `a5c9f50` | render-target allocation, per-mip draw plus RDG copy, render-thread tripwire, warm draw | +304 |
| `885ec41` | `uv_corruption` / `normal_corruption`: the Apply transaction with rollback, §R8 restore, labels, the G-COLL probe, CDO references; registered, not in the auto pool | +1,417 |
| `2705e2d` | S1 bench levers and `IAI.Bench.TexCorruptCensus` | +349 |
| `e36ae66` | capture side: split prewarm list, `TexCorruptWarm` phase, the mode as `anomaly_subtype`, run_summary keys | +183 −8 |
| `bb048bb` | PRE-DELIVERY-CHECKLIST lines | +11 |
| `193bd35` | journal `docs/sessions/2026-09-27-082-05-…`, branch status block, gotcha G309 | +222 |

**Step 1 preconditions.** The branch equalled origin before any code, and there is disk headroom (C 34.7 GB, D 201.7 GB, E 266 GB free). M0 covers 2,082 files; there is no `CaptureBenchTexCorrupt/` folder yet. It was written by a script in the evidence folder, because CaptureBench is off-limits in this brief.

One gap in the records: `_binary_baselines\README.md` had no entry for any m55 exe. I verified the staged `E0BE6F0A` against its archive file instead, and added an entry to the README saying so.

**G10**
- **Editor target:** exit 0 in 79 s, 0 warnings, 0 errors.
- **Game target:** exit 0 in 67 s, 0 warnings, 0 errors. Before any S1 code, a warm-up build of the unchanged code took 3,920 s; its only two warnings are old deprecations in the host's own `MidReproActor.cpp`.
- Both targets were built from a scratch-host worktree clean at `bb048bb`, with the plugin's build products deleted first, so all five plugin modules were compiled from the committed code.
- **Module set:** `AnomalyInjector` links exactly `Core, CoreUObject, Engine, Foliage, InputCore, RenderCore, RHI`. Against the pre-m53 build, `RenderCore` and `RHI` were added and nothing was removed. The other four plugin modules link exactly as before.
- **String scan of the new exe:** every new command and label string is in it, and the pre-existing strings used as controls are too.
- **Comment strip:** 124 files scanned, 0 changed.
- **Archived and hash-verified, not staged:**
  - `_binary_baselines\StackOBot.exe.m53-s1-D7BA87AC` (241,926,144 bytes);
  - `_binary_baselines\m53-s1-editor-bb048bb\` (the plugin's editor DLLs; `UnrealEditor-AnomalyInjector.dll` is `5BC7DBBB`).

**Offline checks: 83 of 83 pass.** A test program in `tools/` compiles the same `TexCorruptPure.h` the plugin uses, with the same MSVC and warnings as errors.

| check | expected | actual |
|---|---|---|
| §R3.2 table, output chain + scratch | 256² 0.42 · 1024² 6.67 · 2048² 26.67 · 4096² 106.67 MiB | same |
| P3: two same-class / two different-class 4096² maps | 192.00 / 213.33 MiB | same |
| §R14 targets: floor · cube · cube with opacity · weapon · rocks · rock flats · character | 6.67 · 60.00 · 81.33 · 96.00 · 325.33 · 133.33 · 298.67 MiB | same |
| budget fit, 8 cases (64 / 128 / 256 MiB, with live and pending bytes, exact fit, one byte over) | as the plan's §R14.3 | same |
| chain-shape predicate (9 cases), 17 pixel-format rows, source mip per level (7 cases), E5 / `streaming_budget` rules (8 cases) | per plan | same |
| decision-tree precedence: lowest failing step wins, first on a tie, all-untouched gives no eligible slot, step ranking order | per plan | same |

To show the test can fail, I ran it against a copy of the header with two deliberate faults (scratch never shared, mip clamp removed): **8 failures, exit 1**, each on a row those faults reach. The test does not cover the parts that need the running engine (reading real materials, residency, the transaction, the restore); those are 082-07's legs.

**Behaviour while the assets are missing**
- **Startup:** the engine logs `CDO Constructor (AnomalyInjectorSubsystem): Failed to find …` three times at error level (non-fatal, confirmed in engine source). The plugin's own startup line then reports `resolved=0` for each asset.
- **Every fire** is refused `assets_unavailable` with sub-reason `missing:M_CorruptTex_UV,M_CorruptTex_Normal,T_CorruptTex_NoiseN`. Nothing is reserved, allocated or touched.
- **If an asset exists but lacks a parameter** the code sets, the fire is refused `assets_unavailable` naming that parameter.
- The warm draw logs `SKIPPED`; the census command still works.
- For 082-06: its authoring commandlet will log those three CDO errors once before it creates the assets.

**Deviations from the plan**
1. The residency sub-reason `optional_unmounted` is emitted as `optional_not_resident`. Whether an optional mip is mounted is private streamer state; the public state only shows the resident count stopping at the non-optional count.
2. `snapshot_px` is written as two keys, `snapshot_px_w` and `snapshot_px_h`, because label array records hold scalars only.
3. The asset check at the first event step also verifies that each corruptor has every parameter the code sets.
4. A texture with no platform data is refused at the format step as `resource_not_ready` / `no_platform_data`. This cannot happen on cooked content.
5. The skeletal/static usage check for lightmapped static meshes mirrors the engine exactly: the flag is required if LOD0's build data has a light map or shadow map.
6. S1 has no product mode, so any mode argument is refused `mode_invalid`, and so is a fire with no bench mode set.
7. Failure step 5 reports `draw_precondition_failed`. A reserve failure at step 1 counts as `rollback_1` plus `over_budget`.
8. Extra keys:
   - labels: `collateral_count`, `collateral_truncated`, `bench_noapply`, `bench_wrong_copy`;
   - run_summary: `texcorrupt_slots_partial_set`.
9. The checklist line checks asset and object references rather than the literal path, because the plan's own `AssetSlotMid` lever contains the fixture path as a string.
10. The no-allocation null (`NoApply 2`) does not register the target watch; the weak-pointer check still reverts a lost target.

**NEEDS-DECISION:** none. The next step is Codex's source review, then 082-06.

**Kept deliberately:** the warm build host `E:\IA_BuildCache\_r53_host` and the edit worktree `E:\IA_BuildCache\_r53_src`, so 082-06 does not pay the ~65-minute warm-up again. The evidence is in `D:\IntrusiveAnomalies\_reviews\082-05-evidence\`.
api-equivalent cost (not billed on Max): $38.13 | turns: 198 | running total across all briefs: $2,562.10
