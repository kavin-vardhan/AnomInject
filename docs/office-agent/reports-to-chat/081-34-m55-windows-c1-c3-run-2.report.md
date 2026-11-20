# REPORT for 081-34-m55-windows-c1-c3-run-2.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-34-m55-windows-c1-c3-run-2.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-26 02:36:06 | finished: 2026-09-26 02:46:42 | duration: 11 min | exit code: 0
context at last turn: 362997 tokens (36% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-34-m55-windows-c1-c3-run-2.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

c1 resumed at `R23_CB_S1` and stopped at the next legacy group, **CB-S**, on a key-presence FAIL. MAIN-N, MAIN-S, c2 and c3 were not run. Docs are pushed.

## Plain summary for the owner
- The PC stayed quiet. The idle check ran 7 times and never had to wait, and nothing was voided.
- The c1 resume worked: only the six CB-S runs were launched, and nothing already decided was re-run.
- One new-build run marked all 90 frames "shader not ready". No other run has ever shown this on the packaged bench. The labels, masks and pixel counts in that run are identical to the old build's.
- The new build's code doesn't touch that marker, and why it fired this once is not known. Chat needs to decide whether this marker belongs in the "old output unchanged" check before c1 can go on.

## Per-window table

| window | legs run | passed | invalid | ENV-INTERRUPTED | dropped / below-floor | person-present waits | wall time | postflight |
|---|---|---|---|---|---|---|---|---|
| c1 (resume) | 6 legacy (CB-S) | 6 READING on attempt 1; group **FAIL** | 0 | 0 | 0 / 0 | 7 checks, 0 s | 21:07:16–21:11:51 UTC (4.6 min) | PASS |
| c2 | not started | — | — | — | — | — | — | — |
| c3 | not started | — | — | — | — | — | — | — |

## Failure
**Group and legs.** Legacy `CB-S`: synth order, `CB_GateLevel`, `blinking` on actor 49.
- S1 and S6: pre-m55 (0844220E).
- S2 and S5: build 3, evidence OFF.
- S3 and S4: build 3, evidence ON.

**Evaluator result.** Verdict FAIL with one problem: `key presence: {"CB_S2": ["labels/0/anomaly_materials_incomplete", "labels/0/render_state", "labels/0/shader_jobs_pending"]}`.
- All 12 cross pairs show 0 extras and 0 gated-unequal fields.
- The harness stopped with `Stop(1)` and chunk exit 1, and recorded `LEGACY_GROUP_CB_S FAIL` in the ledger.
- Files: `_reviews\081-23-evidence\c1\legacy-CB-S-comparison.json`, and the cause read at `_reviews\081-34-evidence\c1-legacy-cb-s-cause-read.md`.

**What `CB_S2` shows.** These are m47's conditional shader-readiness keys:
- on all 90 rows: `render_state="shaders_pending"`, `shader_jobs_pending 0`, `anomaly_materials_incomplete 1`;
- in run_summary: `shader_prewarm_incomplete 1`, `frames_shaders_pending 90`;
- in the log: `PREWARM shaders materials=2 incomplete=1 waited=0.0ms pendingBefore=0 pendingAfter=0`. That same log line states the count is "STRUCTURALLY ZERO in a packaged build". This measurement shows that claim is wrong.

**Everything else matches pre-m55 `CB_S1`:** masks, `mask_value`, `target_pixels`, bboxes, events, onsets and occupancy (`cb-s-pairwise-diff.txt`). `blinking` swaps no material, so the marker cannot describe these frames' pixels.

**Incidence.** This is the first packaged StackOBot case among the 972 banked sessions that carry m47 fields. The only other non-zero is a Lyra smoke run: prewarm incomplete 2, 0 frames marked. The other 11 legacy legs in this requalification read 0.

**Cause read.**
- `git diff 031a103 85c99b3 -- Source` changes no m47 line, and the leg ran with evidence OFF.
- **Mechanism: not established.** The log doesn't say which of the two swap materials was incomplete.

**Is the rule or the build wrong?** It looks like the rule's encoding.
- m47 writes these keys only when a material is incomplete ("a healthy run gains no key"), so the key-presence rule reads a pre-m55 diagnostic value as a schema change.
- A second weakness in the rule (G297): the cross clause learns its allowed variance from control pairs, and `CB_S2` is half of the S2/S5 pair. It therefore taught the clause its own difference, and synth order has no strict clause. Key presence was the only rule that could see this, and it fired correctly. In synth order, a one-leg deviation that only changes values on keys every leg already has would pass.

**person_evidence:** false on all six legs.

## Notable readings
- **Refusal census (`ChangeMaxBytes 1`) and G10:** first runtime proofs already PASSed in 081-31. Not re-run.
- **Legacy vs pre-m55:** CB-S with evidence ON (S3/S4) and OFF (S5) matches pre-m55 on every path except the run-unique ones. S2 matches too, apart from the m47 keys. ON legs carry their change block; OFF legs leak nothing.
- **Lyra G270:** not reached.
- **Person-present gate:** fired 7 times (window start plus 6 attempts), all clear with 0 s waits. Budget `081-34` used 31.1 s of 2700 s, all of it the quiet-gate wait at window start.
- **AMENDMENT 5 live:**
  - A: no coalesced frame, so nothing was excused;
  - B: every tag map was the identity, so no π was applied;
  - C: nothing was needed from the historical set.
- **`census_cycles` (CB-S):**

  | leg | binary | evidence | census_cycles |
  |---|---|---|---|
  | S1 | pre-m55 | — | 27 |
  | S2 | build 3 | OFF | 26 |
  | S3 | build 3 | ON | 27 |
  | S4 | build 3 | ON | 27 |
  | S5 | build 3 | OFF | 27 |
  | S6 | pre-m55 | — | 28 |

  CB-N was 26/26/27/27/26/26. The earlier "ON = 27" association does not separate here: 27 also appears on pre-m55 and OFF legs.

## Deviations
- The console went to `_reviews\081-34-evidence\c1-console.txt` rather than the `_reviews\` root.
- New files in `_reviews\081-34-evidence\`: the cause read, `scan_m47.py`, `diff_cbs.py`, `cb-s-pairwise-diff.txt`, and `_docs_edit.py` (the doc-edit helper).
- No harness file, ledger or boundary was edited by hand.

## Commits pushed
`feat/m55-change-evidence` `50cc235..64bdb64`, docs only, from a scratch worktree that has since been removed:
- the journal `docs/sessions/2026-09-26-081-34-code-m55-c1-legacy-cb-s-stop.md`, with the full c0–c3 ledger: 92 legs. 6 are c0 readings, 19 c1 stack legs passed, 6 legacy legs are in CB-N (PASS-REEVALUATED), 6 are in CB-S (FAIL), and 55 were not run (8 legacy c1, 42 c2, 5 c3);
- the CLAUDE.md status block;
- 🆕 G297.

Unchanged: the main checkout is `m51` `53bf725`, and `master` is `031a103`, same as origin. Staged exe 002805CF. No build, merge or tag. The commit moves the feature head, so the boundary must be re-issued before the next window.

## NEEDS-DECISION
1. Are m47's conditional keys in legacy-identity scope, or a diagnostic class sampled per run and environment (like `exposure_dip`)? The keys are `render_state`, `shader_jobs_pending`, `anomaly_materials_incomplete`, and run_summary `frames_shaders_pending` and `shader_prewarm_incomplete`.
2. If they stay in scope, should an excusal be tied to the leg's own `PREWARM … incomplete=N` line and capped, the way ruling A was?
3. G297: in synth order, a single leg's value-only deviation is self-excused by its own control pair. Does this need a rule change before MAIN-S?

After the ruling: re-evaluate CB-S from the bank, or amend the rules before MAIN-N and MAIN-S. Then re-issue the boundary and resume c1 at `MAINON_N1`.

## Wall-clock
- **c1:** 4.6 min. Preflight and quiet wait 31 s; the six CB-S legs ran 21:07:47–21:09:31 UTC; then comparison and postflight.
- **c2 and c3:** not run.
- **Session:** about 20 min in total.

Several MCP connectors need authorising in their connector settings, and the PDF viewer failed to connect. None were needed here.
api-equivalent cost (not billed on Max): $5.82 | turns: 40 | running total across all briefs: $2,251.27
