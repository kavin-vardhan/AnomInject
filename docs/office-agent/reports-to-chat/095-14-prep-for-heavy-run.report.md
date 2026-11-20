# REPORT for 095-14-prep-for-heavy-run.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-14-prep-for-heavy-run.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-14 13:41:36 | finished: 2026-11-14 14:25:16 | duration: 44 min | exit code: 0
context at last turn: 710307 tokens (71% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-14-prep-for-heavy-run.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**For the owner:** I applied both of chat's rulings. The "pop back down when you walk away" behaviour now ships switched off, the readme says why, and the two missing test objects can now be built by the fixture script. Everything is committed, tested and pushed, and the heavy run can start from `tips.txt`.

**Items, with commit and test**
- **Changelog row for ruling 095-12 D2** (`integ/m56-m58` `0c71f6f`): a texture event that changed only some meshes of an object now names one of the changed meshes; `node.bounds` still covers the whole object. Pre-push scan PASS.
  - **G5 comparator** (CaptureBench `490964f`): G5 read `labels.jsonl` only, but the node name lives in `annotation.json`. As written, the allowance would have checked nothing (new gotcha G583). G5 now also compares node identity per event. Only `asset_name` and `component_class` may differ, and only on scoped texture events. Anything else fails; a missing `annotation.json` or `event_id` makes it refuse to run (exit 2).
  - **Proof both ways:** selftest 26 cases; 5 comparator mutants all fail it; a doctored copy of a real banked session reads 8 of 8 cases as predicted.
  - **Predictions:** m56 file `predictions-095-05` gets AMENDMENT 2. The release build writes every field the new check needs.
- **Merge of the new integration tip into m57:** `60ab181`.
- **Down-pop default 1 → 0, ruling 095-13 D1** (`33978b9`): help text, readme and architecture now call it experimental and unproven, and explain why it cannot happen when the swap follows the engine's own switch closely. The switch still works.
  - The selftest now simulates the C4 approach-and-retreat path with the switch off and on. Both give one fire at frame 59, labelled frames 128..132, one up-pop at 133, no re-hold, and nothing labelled on the retreat. 169 checks; a new mutant that puts the default back to on fails it (16/16 mutants fail).
  - **Predictions:** m57 file `predictions-095-10` gets AMENDMENT 2 (`773aef7` is the CaptureBench copy). A run with `LodDownPop 1` is predicted to look identical; only the settings line in the log and `run_summary` tells the two apart.
- **Readme line for ruling 095-13 D2** (`100e287`): `run_summary` has no per-trigger event count, so the line points to what exists: `lod_swaps_by_cause` (counts pops per trigger), each event's `anomaly_subtype`, and the `Auto.Yield` refusal lines.
- **Fixture options `pair_cube` and `minlod1`, ruling 095-12 D3** (CaptureBench `4ca8524`):
  - `CB_LodPair`: MeshLod (the 2-LOD fixture mesh) at (1000, −150, 600) and MeshOne (engine Cube, 1 LOD) at (1000, 150, 600).
  - `CB_LodFix_MinLod1`: the fixture mesh at (1100, 0, 390), scale 1.5, minimum LOD 1. I used 1.5 because a full-size 4.6 copy would block the second pair and stand on the distance paths.
  - **Tests:** syntax check; a stub-`unreal` test that runs the real script, 13 cases; an offline placement check showing no new object blocks any existing target or camera path.
  - **Predictions:** m59 file `predictions-095-09` gets AMENDMENT 3, recording the fixtures as built.
- **Docs** (`d8d3a7e`): journal, G583, status block.
- All amendments were appended before any leg. Each original file's first bytes still hash to their recorded value (`predictions-amendment-hashes.txt`).

**Four tips**
| Branch | Tip | State |
|---|---|---|
| `integ/m56-m58` | `0c71f6f` | pushed |
| `feat/m57-lod-spatial-trigger` | `d8d3a7e` | pushed, contains the integration tip |
| `feat/m58-terrain-blink-subtype` | `5e1bc03` | unchanged |
| CaptureBench `m57-gates` | `4ca8524` | local only; CaptureBench left on `master` |

`tips.txt` has the fixture command lines: one run with all four options, or one run per option. They must run from a `m57-gates` worktree outside `Plugins\`, because `master`'s copy of the script has no options.

**Compile and tests:** two Editor compiles, both exit 0 with 0 errors and 0 warnings (m57 tip 151 s; integration tip 261 s).
- **On the m57 tip:** 19/19 pure selftests; LOD trigger 169 checks; m59 refusal selftest 141 checks, 13/13 mutants fail; terrain mutants 5/5 fail; label rule 119, pixel gate 101, change oracle 35, kit 104, unit tests OK; lever audit PASS on source and binary.
- **On the integration tip:** lever audit PASS on source and binary.

**What the host's binaries match:** the plugin's Editor DLLs were rebuilt from `integ/m56-m58` `0c71f6f`; they read 57 lever names, the integration count (the m57 build read 62). The Game exe is still the 24 Sep build, and no cook exists.

**Safety:** none. No foreign process was touched, and no process of ours is still running.

**Decisions for chat:** none. One note: the G5 allowance also covers `stuck_low_mip` on the proxy route, because the same source line scopes those events and so renames them too. G5 lists it as its own cause (`stuck_mip_proxy_route`), so the heavy run's reading will show it if it happens. Narrowing it to skipped parts only is a one-line change.
api-equivalent cost (not billed on Max): $26.03 | turns: 189 | running total across all briefs: $6,458.55
