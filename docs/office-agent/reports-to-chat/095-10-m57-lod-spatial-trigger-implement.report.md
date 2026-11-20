# REPORT for 095-10-m57-lod-spatial-trigger-implement.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-10-m57-lod-spatial-trigger-implement.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-14 09:45:10 | finished: 2026-11-14 10:53:34 | duration: 68 min | exit code: 0
context at last turn: 437420 tokens (44% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-10-m57-lod-spatial-trigger-implement.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**1. For the owner**
- `lod_popping` now pops only when the camera's movement causes it: you walk toward an object, or you turn so it comes into view. The old timer is kept for bench tests only.
- Every event that has labelled frames is kept in the dataset, and it is marked with whether the pop was actually visible on screen.
- It is written, compiles cleanly and passes all offline tests. Nothing has run in the game yet: no game build, no cook, no clip.

**2. Commits** (branch `feat/m57-lod-spatial-trigger`, cut from `integ/m56-m58` `876c719`; **pushed**, tip `f77deed` == origin; not merged, no tag)

| Hash | Subject |
|---|---|
| `4d3100f` | feat(lod): m57 engine-exact LOD prediction (plan commit 1) |
| `86e866d` | feat(lod): m57 lod_popping spatial modes (2) |
| `ab73625` | fix(lod): m57 remove a stray BOM fragment from AnomalyDefaults.cpp |
| `a089970` | feat(capture): m57 deferred-onset timeout per anomaly, burst extension, subtypes and keys (3) |
| `33e4a89` | feat(capture): m57 render-side LOD record (4) |
| `693e226` | feat(bench): m57 lod_v1 camera schedule and levers (5) |
| `f2b2f74` | fix(lod): m57 normalize Anomaly_LodPopping.cpp line endings |
| `636edbe` | chore(tools): m57 readers learn the LOD subtypes, reasons and keys (6) |
| `adba506` | fix(bench): m57 lod_v1 copies in front of the fixture backdrop |
| `94024ae` | fix(lod): m57 distance release margin on the later of the two thresholds |
| `f77deed` | docs: m57 - readme, architecture, runbook, G580, journal, status block (7) |

CaptureBench `m57-gates` is at `9eb8ca6`. It is local only (that repo has no remote) and its checkout is left on `master`.
- `make_lod_fixture.py` gains two options, `IAI_LODFIX_ADD=second_pair,instanced`. They are written and syntax-checked but have not been run.
- New scripts: `m57_null_pair.py` (selftest 5 cases), `m57_lod_readout.py` (11 cases), `m57_make_clip.py` (4 cases). Each is proven both ways on doctored sessions.

**3. Compile runs** (Editor target, Development Win64, incremental, 2 parallel actions)

| Run | When | Free at start | Lowest free | Errors / warnings |
|---|---|---|---|---|
| c2 | after commit 2 | about 10.1–10.5 GiB on every run | — | C3872 from a stray BOM fragment I had introduced (fixed in `ab73625`); re-run c2b: 0/0 |
| c3a / c3b | after commit 3 | | | 0/0 |
| c4 | after commit 4 | | | 0/0 |
| c5 | after commit 5 | | | 0/0 |
| final | source tip | 10.10 GiB | 8.47 GiB | 0/0 |
| integ | `876c719` (finish step) | 10.08 GiB | 7.33 GiB | 0/0, 57 s |

The lowest free memory across the m57 runs stayed between 7.5 and 8.8 GiB. Lever audit: PASS on source and on the Editor binaries, 62 levers; its selftest passes.

**4. Tests**
- Pure C++ selftests: 19 of 19 pass. The new `lod_trigger_selftest` runs 120 checks with 0 failures, and all 9 mutants fail it. The selftests that pinned the reason count at 10 now pin 12.
- Python: label rule 107 cases, pixel gate 101, black frame, change oracle 35, office kit 103, unit tests 59, exclusion gate, measure-offset — all pass.
- Not re-run: the m58 terrain mutants, because the script is tied to 095-09's paths. The terrain header is unchanged and its selftest passes.
- Untouched read-back (journal §5):
  - Other anomalies' membership and burst timing are unchanged. The new hooks default to `false` / `-1`, so the Positives phase still reverts in the same tick as before.
  - `stuck_mip_hold_timeouts` still counts stuck_low_mip only.
  - m56 continuity and the settle rule are unchanged. m58 and m59 are unchanged.
  - The reason table grows from 10 to 12 entries.
  - A non-lod run's `run_summary` gains only the two `label_transition_lod_*` keys, both 0.
- Predictions are frozen before any leg in `code-receipts-095-10\predictions-095-10.txt`, sha256 `269FECF8…7A0B`.
- Pre-push scan: PASS. Codename scrub clean over 39 files; token absent; 0 comment lines in 2,648 added source lines.

**5. Q4 keys as built, and deviations**
- **Per event in `annotation.json`:**
  - `anomaly_subtype` = `distance_threshold` | `view_entry` | `periodic`.
  - `lod_swaps`: one entry per swap with `frame`, `tick_index`, `cause` (`distance` | `view_entry` | `hold_cap`), `direction`, `from_lod`, `to_lod`, `screen_size`, `distance_cm`, `coverage_pct`, `on_screen`.
  - `lod_spatial_swap_visible`, `lod_event_kind` (`spatial_swap` | `hold_cap_only` | `no_swap` | `periodic`), `lod_held_lod`, `lod_swap_frames`, `lod_trigger_screen_size`, `lod_trigger_distance_cm`.
  - Root `label_schema_minor` is 3.
- **In `run_summary`, written only when a lod fire ran:** `lod_events_by_kind`, `lod_swaps_by_cause`, `lod_untriggered`, `lod_untriggered_event_ids`, `lod_arm_timeouts`, `lod_window_extension_frames`, `lod_render_checked_frames`, `lod_render_mismatch_frames`, `lod_trigger_settings`.
- **New reason flags:** `lod_subvisible` (1024) and `lod_unconfirmed` (2048).
- **Deviations** (journal §3 has all 13 with reasons):
  - One component per spatial fire: the largest armable one.
  - The release margin applies to whichever comes later: the contrast switch or the first labelled screen size. This is what plan §3.1 says; my first version got it wrong.
  - Labels are computed when asked, and the frame is committed in the next tick from the same input. The hold-open answer is sampled at tick end. Together these make the result independent of which subsystem ticks first.
  - The capture infers the hold-cap swap from the anomaly's last frame.
  - The bench copies are placed at (1700, ±60), because the fixture's backdrop wall stands at x 1850–1950.
  - The office kit reports spatial events as NOT-JUDGEABLE; the bench null-pair check judges them instead.
  - The m26 zero-pixel veto still applies to kept lod events.

**6. What the heavy brief must do**
- **Before the cook:** run CaptureBench `make_lod_fixture.py` with `IAI_LODFIX_ADD=second_pair,instanced` in the editor. Check its `ok` and read-back in the result JSON, and check that the content diff is only the new assets plus `CB_LodFixture.umap`.
- **After the cook:**
  1. m59 leg L59-5 on `CB_LodFix_ISM` and `CB_LodFix_HISM`.
  2. The m57 look-first clips C1–C6, made with `m57_make_clip.py`.
  3. The owner's verdict on the clips.
  4. Gates L0–L12 as frozen in the predictions file.

**7. Host state**
- The primary checkout is on `integ/m56-m58` `876c719` (== origin), with no tracked changes.
- The plugin's Editor DLLs were rebuilt from that tip, 0/0. `AnomalyShaders.dll` was not rebuilt because its source is identical.
- The Game exe has still not been built.
- No build process of mine is left running.
- Context used: about 60 % at the point it was summarized.

**8. Safety:** none. Two encoding faults of mine (a stray BOM fragment and doubled CRs) were caught and fixed. Because of the second one, commit 5's raw diffstat shows the whole `Anomaly_LodPopping.cpp`; `--ignore-cr-at-eol` shows the real change of +142/−3 (G580).

**Decisions chat must make:** none required. Worth knowing:
- The `no_swap` event kind cannot be produced by any `lod_v1` camera path; it is covered offline only.
- The office kit does not judge spatial lod events.
api-equivalent cost (not billed on Max): $44.14 | turns: 212 | running total across all briefs: $6,309.42
