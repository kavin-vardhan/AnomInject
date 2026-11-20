# REPORT for 095-12-m59-review-followups.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-12-m59-review-followups.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-14 11:26:54 | finished: 2026-11-14 12:04:40 | duration: 38 min | exit code: 0
context at last turn: 637494 tokens (64% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-12-m59-review-followups.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**For the owner:** I closed the review's m59 gaps where a level-of-detail (LOD) event could be labelled with nothing changed on screen, or with a box bigger than what changed. Both branches build cleanly and pass every offline test, and both are pushed. Nothing was launched and no game build was made.

**Per item** (branch `fix/m59-forced-lod-refusals`, each commit compiled for the Editor on its own, 0 errors and 0 warnings)
- **S10 `d8077fc`:** the single-LOD refusal now counts only the LODs the engine can actually draw. LODs below the mesh's minimum (or the component's override) and LODs with no geometry don't count. This covers static and skinned meshes and applies to both LOD anomalies. Proof: 25 new selftest checks; the mutants that ignore the minimum LOD or the empty LODs fail 9 and 8 checks.
- **S9 `91a87c2`:** `lod_corruption` now refuses a forced LOD that is not coarser than the LOD currently on screen (`not_coarser_than_current`). It uses the existing current-LOD prediction, adjusted the way the engine adjusts it (minimum LOD, then the finest LOD streamed in).
  - When the current LOD can't be read (no view, no render data), it refuses as `current_lod_undetermined`. I chose that direction because a label nobody can show to be true is the defect we're removing.
  - Proof: 46 new checks; four mutants fail 7, 3, 2 and 3 checks.
- **S3 `c5113e3`:** when only some meshes of an object are forced, both LOD anomalies now limit the label box, the mask, the visibility count and the event's asset/class name to the forced meshes. They reuse the mechanism the texture anomalies already have; it is set at Apply and cleared at Revert. An object whose meshes are all forced is unchanged.
  - Proof: the scope decision has a pure test with 7 checks; the "never scope" and "always scope" mutants fail 3 and 2. The box and mask effect itself can only be proven by a capture leg (L59-8 below).
- **S11 + N14 `2b4661c`:**
  - The help text, readme and log lines for `IAI.Anomaly.LodAllowSkeletal` no longer say `1` restores earlier behaviour. They now say a single-LOD skeletal mesh is still refused (as `single_lod`) at 1, and reads `skeletal_unmeasured` at 0.
  - A change applies at once outside a capture and from the next capture during one.
  - The read-back during a capture now shows the value in use with its own source, plus the value waiting for the next capture.
  - No filter for older datasets is promised.
- **Docs `d23ab16`:** architecture rows, new gotcha G581, journal `docs/sessions/2026-11-14-095-12-m59-review-follow-ups.md`, status block.

**Tips**
- `fix/m59-forced-lod-refusals` = `d23ab16`, pushed (fast-forward from `af1b08f`).
- `integ/m56-m58` = `e15d06d`, pushed: a true merge `85e90e6` with no conflicts, plus a status commit (fast-forward from `876c719`). The merge adds exactly the fix branch's changes, line for line.

**Compile and tests**
- Every Editor build used 2 parallel actions and started with at least 9.8 GiB free. The integration tip built cleanly: 14 actions, 0 errors, 0 warnings.
- LOD selftest: 141 checks, 0 failures; all 13 mutants fail, on both branches.

| | Fix tip | Integration tip |
|---|---|---|
| Pure C++ selftests | 15/15 | 18/18 |
| Label rule | 65 | 94 |
| Pixel gate | 98 | 101 |
| Kit | 93 | 102 |
| Change oracle | 35 | 35 |
| Lever audit (source and binary) | PASS, 53 names | PASS, 57 names |

- Also passing on both: the unit tests, exclusion gate, measure-offset selftest and the lever selftest (12).
- On the integration tip the 5 terrain-rule mutants all fail, as they should.
- The pre-push scan passed on both branches: the codename check found nothing, the token is absent, and no comments were added to source.

**Amended predictions:** `code-receipts-095-09\predictions-095-09.txt`, AMENDMENT 2, dated before any leg (SHA-256 `729D41BB…1E37`).
- It records the new log-line formats for L59-1, L59-4 and L59-7.
- **L59-8 (S3):** an object with two meshes, `CB_LodPair` (the 2-LOD fixture mesh plus a 1-LOD cube); the box and mask must cover the forced mesh only. The fixture isn't built: it needs a new option in the fixture script riding on 095-07's cook, or a new spawn lever. The can-fail is the release exe's `lod_popping`, which refuses the cube but boxes both meshes.
- **L59-9 (S9):** `lod_corruption CB_LodFix_N 1` on the existing fixture. The release exe should label frames where nothing changes (pixel gate NO-TRACE); the new build should refuse `not_coarser_than_current`. Same leg with index 2 as the control.
- **L59-10 (S10):** `CB_LodFix_MinLod1`, a copy of the fixture with its minimum LOD set to 1; not built, same cook route. The release exe should read NO-TRACE for both anomalies; the new build should refuse `single_lod`.
- **L59-11:** log-only check of the switch read-back, riding on L59-7.

**Host binaries:** the plugin's Editor DLLs match `85e90e6`'s Source, which is also `e15d06d`'s Source. The Game exe is still not built.

**Safety:**
- The checkout went `integ/m56-m58` `876c719` → `fix/m59` `af1b08f` → back to `integ/m56-m58` `e15d06d`. It is clean apart from the known untracked files and equals origin.
- No Codex, no force, reset, stash or rebase. No process was killed. Nothing else was touched: not the other branches, the tags, `D:\IA_Europa` or the dashboard repo.
- No Unreal or build process of mine is still running. D: has 63.2 GiB free and E: 16.1 GiB.
- G580 already exists on `feat/m57`, so I used **G581**; the next free gotcha is G582.

**Decisions for chat**
1. `lod_popping`'s targeted timer on this branch still fires (with a warning) on a target already at its coarsest LOD. S9 named `lod_corruption` only, and m57 replaces this timer, so I left it. Extend S9 to it only if m57 might not merge.
2. The node-name fix is in shared code, so texture-corruption events with skipped parts now also name a corrupted component. `node.bounds` still covers the whole object, which the readme says. Keep, or scope the bounds too?
3. L59-8 and L59-10 need new fixture options in the fixture script before 095-07's cook, and that work isn't assigned yet.

Receipts: `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-12\`
api-equivalent cost (not billed on Max): $19.33 | turns: 142 | running total across all briefs: $6,368.01
