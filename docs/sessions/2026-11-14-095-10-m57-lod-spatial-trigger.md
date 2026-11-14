# 095-10 - m57: `lod_popping` with a spatial trigger (written, Editor-compiled, offline-tested; no engine leg)

**Date:** 2026-11-14 · **Author:** Claude Code (Opus 5.5, xhigh), headless · **Branch:** `feat/m57-lod-spatial-trigger`
cut from `integ/m56-m58` `876c719` (contains the m59 and m59b merges).
**Binding:** `_reviews\095-08-ruling.md` (Q1-Q12, G1-G4, H1-H4), `_reviews\095-08-plan-B-lod-spatial-trigger.md` (the
build specification; the ruling wins where they differ), `095-03b-ruling.md` E2/E4, `095-06b-ruling.md` F1.

## 0. State in one paragraph
Plan section 11 commits 1-7 are on the branch, plus four small fix commits. The plugin Editor target compiled 0 errors /
0 warnings after commits 2, 3, 4, 5 and at the tip; lever audit PASS (source and Editor binaries, 62 levers). 19 of 19
pure C++ selftests pass (new `lod_trigger_selftest` 120 checks, 9 mutants all fail it); every Python suite passes (label
rule 107 cases, kit 103, unit tests 59). **No engine leg ran, the Game target was not built, no clip was made, nothing
was cooked.** Predictions frozen: `code-receipts-095-10\predictions-095-10.txt`, sha256 `269FECF8...7A0B`.

## 1. Commits (plugin)
| # | Hash | Subject |
|---|---|---|
| 1 | `4d3100f` | feat(lod): m57 engine-exact LOD prediction |
| 2 | `86e866d` | feat(lod): m57 lod_popping spatial modes |
| 2f | `ab73625` | fix(lod): m57 remove a stray BOM fragment from AnomalyDefaults.cpp |
| 3 | `a089970` | feat(capture): m57 deferred-onset timeout per anomaly, burst extension, subtypes and keys |
| 4 | `33e4a89` | feat(capture): m57 render-side LOD record |
| 5 | `693e226` | feat(bench): m57 lod_v1 camera schedule and levers |
| 5f | `f2b2f74` | fix(lod): m57 normalize Anomaly_LodPopping.cpp line endings |
| 6 | `636edbe` | chore(tools): m57 readers learn the LOD subtypes, reasons and keys |
| 5g | `adba506` | fix(bench): m57 lod_v1 copies in front of the fixture backdrop |
| 1f | `94024ae` | fix(lod): m57 distance release margin on the later of the two thresholds |
| 7 | this commit | docs: m57 |

CaptureBench `m57-gates` `9eb8ca6` (local; the repo has no remote; checkout left on `master`).

## 2. What was built (ruling choices)
- **Mechanism (Q1):** plugin-timed forced LOD on one plain static mesh component; thresholds from the engine's own
  `ComputeStaticMeshLOD` on the frame's `FSceneView` (`AnomalyLod::PredictEngineLod`).
- **Scope (Q2):** plain static only; skeletal (regardless of `LodAllowSkeletal`), ISM, HISM, Nanite refused by name.
  m59's refusals and switch unchanged.
- **Triggers:** `distance_threshold` (freeze at the engine's LOD, release past the contrast switch and the first
  positive size, both x1.1; down-pop on retreat past 0.85, on by default, `IAI.Anomaly.LodDownPop`; invisible release
  when the engine coarsens back) and `view_entry` (hold the coarsest contrasting LOD off screen on the side the camera
  turns toward; release after `LodPoppingEntryFrames` = 4 positive frames). `auto` picks distance, then view entry.
- **Q3:** a spatial fire with no labelled frame writes no event; its id goes to `run_summary.lod_untriggered_event_ids`.
- **Q4 (the change):** every event with labelled frames is kept and marked (section 4).
- **Q5/Q10:** `lod_subvisible` (1024) and `lod_unconfirmed` (2048); labels from the tick-end prediction, confirmed per
  frame on the render thread.
- **Q7/Q8:** `periodic` bench-gate only (`periodic_bench_only`); the 200 cm and highest-LOD gates stay in the periodic
  path (which the auto pool no longer reaches).
- **Q9:** `lod_v1` in `AnomalyBench`. **Q11:** equal draw. **Q12:** `label_schema_minor` 3.
- **G1:** `IAnomaly::GetDeferredOnsetTimeoutFrames()` (-1) and `IAnomaly::IsHoldingEventOpen()` (false), additive.

## 3. Deviations from the plan, each with its reason
1. **One component per spatial fire** (the largest armable one of the matched actor), not every matching component:
   the tracker is per object; a multi-component actor would need one tracker per component and one event.
2. **Release margin** (fix `94024ae`): applied to the later of the contrast switch and the screen size at the first
   positive frame. The first version applied it to the contrast switch only; on the scale-1 copy that pops on the first
   positive frame. This is what plan section 3.1 specifies; the first version was mine and wrong.
3. **Frame commit in the next Tick, label at call time.** The plan caches per `GFrameCounter`; the capture also asks
   mid-tick (pre-roll check) when the camera is still the previous frame's. The cache is keyed on camera pose, hold state
   and the last commit instead, and the observation is committed in the next `Tick` with the same input, so a swap and
   the labels always come from one evaluation.
4. **Hold-open answer sampled in `NoteCapturedFrame`** (tick end) so the capture's extension decision does not depend on
   tick order.
5. **The hold cap counts frames since the first positive frame** (not extension frames): the anomaly enforces it itself;
   the capture adds a 3000-frame safety bound.
6. **Hold-cap swap is inferred by the capture** from the last lod frame's `lod.cap_reached` (the anomaly is reverted on
   that tick and has no telemetry on the swap frame).
7. **`lod_unconfirmed` frames can keep a mask PNG** (masks are armed at tick end, before the drain compares).
8. **The view sample ring is fed from `UAnomalyInjectorSubsystem::DispatchAnomalyTicks`** (one call per tick), not from
   the anomaly's `TickAlways`, because the anomaly has no world before its first Apply.
9. **No `lod_refused_by_reason` key:** refusals already reach the per-id yield tally (`lod_not_armable:<reason>`).
10. **Office kit:** spatial events are reported NOT-JUDGEABLE (their onset is a flag boundary, not a pixel edge); the
    bench null-pair check judges them. `measure_label_offset.py` was not changed.
11. **Bench copies:** at (1700, -/+60) with the distance paths starting at x=-600, because the fixture's backdrop wall
    stands at x 1850..1950 (plan said copies at 20-25 m from x=0).
12. **m45 frame reader not imported:** `m45_identity.py` is untracked in CaptureBench; `m57_null_pair.py` carries the
    same three-line Pillow reader and frame-path convention.
13. **m26 veto unchanged:** a kept lod event whose target the mask measures at zero pixels is still vetoed by the
    existing rule. That is the m26 rule, not a Q4 drop.

## 4. The Q4 keys as built
- `annotation.json` per lod_popping event: `anomaly_subtype` = `lod_trigger` (`distance_threshold` | `view_entry` |
  `periodic`), `lod_held_lod`, `lod_swaps` [{`frame`, `tick_index`, `cause` `distance`|`view_entry`|`hold_cap`,
  `direction` `up`|`down`, `from_lod`, `to_lod`, `screen_size`, `distance_cm`, `coverage_pct`, `on_screen`}],
  `lod_swap_frames`, `lod_spatial_swap_visible` (true iff a `distance` or `view_entry` swap was on screen),
  `lod_event_kind` (`spatial_swap` | `hold_cap_only` | `no_swap` | `periodic`), `lod_trigger_screen_size`,
  `lod_trigger_distance_cm` (first swap, -1 if none). Root `label_schema_minor` 3.
- `run_summary.json` (only when a lod fire ran): `lod_events_by_kind`, `lod_swaps_by_cause`, `lod_untriggered`,
  `lod_untriggered_event_ids`, `lod_arm_timeouts`, `lod_window_extension_frames`, `lod_render_checked_frames`,
  `lod_render_mismatch_frames`, `lod_trigger_settings`; always: `label_transition_lod_subvisible_entries`,
  `label_transition_lod_unconfirmed_entries`.
- `labels.jsonl` lod entries: `lod.trigger`, `lod.state`, `lod.held_lod`, `lod.engine_lod`, `lod.engine_lod_render`,
  `lod.screen_size`, `lod.distance_cm`, `lod.coverage_pct`, `lod.on_screen`, `lod.held`, `lod.subvisible`, `lod.swap`,
  `lod.cap_reached`, `lod.component`, and on the swap frame `lod.swap_cause`, `lod.swap_direction`, `lod.swap_from_lod`,
  `lod.swap_to_lod`.
- Client filter (readme): keep events whose `lod_spatial_swap_visible` is true.

## 5. Untouched read-back
| Item | Reading |
|---|---|
| Other anomalies' membership | Untouched. `SYNC:IsAnnotationMember` unchanged; the on-screen term lives in lod_popping. |
| Other anomalies' burst timing | Untouched: `IsHoldingEventOpen` defaults false, so the Positives phase reverts in the same tick as before; `GetDeferredOnsetTimeoutFrames` defaults -1, so stuck_low_mip's pre-roll timeout is the global one. |
| `stuck_mip_hold_timeouts` | Counts stuck_low_mip timeouts only (as before, since only stuck_low_mip had deferred onset). |
| m56 continuity | Extension frames are captured every tick (no gap); continuity accounting unchanged. |
| m56 settle rule | Unchanged; a swap is neither a burst fire nor a revert, so it is never flagged settle. |
| m58 terrain rule | Unchanged (no file of it touched; `terrain_rule_selftest` passes). |
| m59 refusals | Unchanged; spatial modes add skeletal refusal regardless of the switch. |
| Reason table | Grew 10 -> 12; three selftests that pinned 10 now pin 12. |
| run_summary for non-lod runs | Gains only the two `label_transition_lod_*` keys (0). |

## 6. Tests
- Editor compiles: c2 (after a one-line BOM fix, 0/0), c3 (0/0), c4 (0/0), c5 (0/0), final (0/0). Memory free at
  launch 10.1-10.5 GiB, minimum during 7.5-8.8 GiB. Receipts `compile-*.log`, `compile-*-mem.txt`.
- Pure C++: 19/19 with include paths (`selftests-c6-inc`); lod trigger 120/0, 9/9 mutants fail.
- Python: label rule 107, pixel gate 101, black frame, change oracle 35, kit 103, unit tests 59, exclusion gate,
  measure offset, lever audit + selftest 12 (`pytests-c6\SUMMARY.txt`). Terrain mutants not re-run (script tied to
  095-09 paths; the terrain header is unchanged).
- CaptureBench: `m57_null_pair.py` 5 cases, `m57_lod_readout.py` 11, `m57_make_clip.py` 4; `make_lod_fixture.py`
  options syntax-checked only.

## 7. For the heavy brief (095-07) and after
- **Before the cook:** run CaptureBench `make_lod_fixture.py` with `IAI_LODFIX_ADD=second_pair,instanced` in the
  editor (adds `CB_LodFix_N2`/`D2` and `CB_LodFix_ISM`/`HISM`); check its result JSON (`ok`, read-back) and the content
  diff (new assets + `CB_LodFixture.umap` only). Merge m57 into the integration branch first only if chat says so; the
  options are content, independent of the m57 code.
- **After the cook:** m59's L59-5 on `CB_LodFix_ISM` / `CB_LodFix_HISM`; then the m57 look-first clips C1-C6 (G4) with
  `m57_make_clip.py`, then the owner's verdict, then gates L0-L12 per the frozen predictions.

## 8. Errors of mine, recorded
- A settings block was piped from PowerShell into Python's stdin and arrived with a mis-decoded BOM; the compile caught
  it (fix `ab73625`).
- A lever block was built with doubled CRs; git stored `Anomaly_LodPopping.cpp` as binary with CRLF and commit 5's
  diffstat shows the whole file (fix `f2b2f74`; `--ignore-cr-at-eol` shows the real +142/-3). G580.
- The first fixture copy position sat behind the backdrop (fix `adba506`), and the first release rule ignored the
  coverage threshold's margin (fix `94024ae`).
