# REPORT for 081-31-m55-windows-c1-c3-run.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-31-m55-windows-c1-c3-run.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-26 00:32:09 | finished: 2026-09-26 00:49:30 | duration: 17 min | exit code: 0
context at last turn: 400410 tokens (40% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-31-m55-windows-c1-c3-run.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

c1 stopped at its first legacy group, `CB-N`, on a gate FAIL. All 14 remaining stack legs passed, including `C1_G2_S`. The failure comes from three unrelated differences between legs. Two look like gaps in how the legacy rule is written; one is a real but pre-existing effect. c2 and c3 were not run.

## 2. Plain summary for the owner
- Nobody used the PC this time. The new idle check ran 21 times, never had to wait, and nothing was voided for a person.
- `C1_G2_S` passed on its fresh try, and so did the other 13 remaining test legs.
- The first "old output is unchanged" group failed. One build-3 run lost one frame's mask, another run numbered its mask colours differently, and a counter was off by one.
- The first by source reading is an old, pre-m55 behaviour. The other two are known kinds of run-to-run noise that the rule doesn't allow for yet.
- Chat needs to rule on how to judge this group before c1 can continue. Nothing was re-run or edited.

## 3. Per-window table

| window | legs run | passed | invalid | ENV-INTERRUPTED | dropped / below-floor | person-present waits | wall time | postflight |
|---|---|---|---|---|---|---|---|---|
| c1 | 14 stack + 6 legacy (one group) | 14 stack; legacy group CB-N **FAIL** | 0 | 0 | 0 | 21 checks, 0 s total | 19:03:26–19:13:30 UTC (10 min) | PASS |
| c2 | not started | — | — | — | — | — | — | — |
| c3 | not started | — | — | — | — | — | — | — |

Remaining c1 legs not run: CB-S (6 legs), MAIN-N (4) and MAIN-S (4).

## 4. Failure
**Group and verdict.** Legacy `CB-N`, native order, on CB_GateLevel with `blinking` on actor 49.
- Legs: N1 and N6 pre-m55 (0844220E), N2 and N5 build 3 with evidence OFF, N3 and N4 build 3 with evidence ON.
- All six ran on attempt 1 and produced readings. The comparison returned FAIL with 22 problems.
- The harness stopped with `Stop(1)` and chunk exit 1; postflight PASS.
- Evaluator output: `_reviews\081-23-evidence\c1\legacy-CB-N-comparison.json`. Cause read: `_reviews\081-31-evidence\c1-legacy-cb-n-cause-read.md`.

**(A) `CB_N2` (build 3, OFF): frame 29 has no mask.**
- It reads `target_pixels -1` and `mask_state unmeasured`, so event 2's observable frames are [28,33,34] instead of [28,29,33,34].
- This one frame accounts for the 20-path strict failures on N2's pairs, every "field set not identical" line, and the key-presence line.
- The log shows one mask render serving frame 28's and frame 29's target arms together (`servedArms=3`), then `TARGET MASK UNAVAILABLE … si 29 … pixels=0`.
- In the source, when arms share a render only the first one that wants pixels gets them (`PixelOwner`). That rule is identical in pre-m55 `master` and build 3, and build 3's only addition there is skipped when evidence is OFF.
- Why frame 28's render slipped a frame is not established. N2's speed ratio was 1.000913, against about 1.000003 on N3–N5.
- m55 is not implicated by the source, but no measured control shows this on the pre-m55 binary.
- **Predicate:** this is real output. What to do with it is a scope question for chat.

**(B) `CB_N5` (build 3, OFF): the census numbered the mask tags differently.**
- 89 strict paths, all `mask_value` or mask-file hashes.
- The mapping is one-to-one on 8 tags (247→227; 248–254 each shifted down by one). Occupancy, the mask/count cross-check and every pixel count are equal to pre-m55.
- N2, the other OFF leg, matches pre-m55 exactly, so this does not follow evidence state.
- **Predicate:** looks like an encoding gap. The rule compares raw tag values, while the tag-independent CB fields pass. Earlier comparisons in this project already treat a one-to-one renumbering as equal.

**(C) `census_cycles`: 26, 26, 27, 27, 26, 26 (N1–N6); both ON legs read 27.**
- This field is already in the historical "varies run to run" set, so the strict rule excuses it.
- The cross-pair rule only learns variance from this group's three control pairs, which happened to agree, and never consults the historical set.
- **Predicate:** looks like an encoding gap. ON = 27 is an association on two legs against four, with no mechanism.

**person_evidence:** false on all six legs.

## 5. Notable readings
- **Refusal census at runtime (`ChangeMaxBytes 1`), first run:** `C1_LOW_N` and `C1_LOW_S` pass. All 90 frames were refused, nothing was measured and peak memory stayed at 0.
- **G10, first runtime proof:** both legs pass, with exactly one budget refusal at frame 8 and the oracle matching 17/17.
- **Other stack legs:**
  - G15 N and S pass 16/16 at speed ratio 2.395.
  - COUNT_FAIL passes 28/28 (peak about 41 MB).
  - WALL_FAIL passes with nothing measured.
  - The 1080 legs pass 20/20 (peak 62–66 MB).
  - `C1_G2_S` attempt 4 passes 19/19 (peak 25.8 MB).
- **Legacy vs pre-m55:** only CB-N ran (ON and OFF). The deterministic content matched pre-m55 except for (A), and (B)'s tag numbers.
- **Lyra G270:** not reached.
- **Person-present gate:** 21 checks, all clear with 0 s waits, each excused by the previous attempt's own ALT tap. 31.1 s of the 45-min budget was used, all of it by the quiet-gate wait at window start.

## 6. Deviations
- New files: `_reviews\081-31-c1-console.txt` (+ `.err.txt`), and `_reviews\081-31-evidence\` holding the cause read and two ledger snapshots.
- A temporary pid file I created in `_reviews\` was deleted.
- No harness file, ledger or boundary was edited.

## 7. Commits pushed
`feat/m55-change-evidence` `66976ea..cfff033`, docs only:
- journal `docs/sessions/2026-09-26-081-31-code-m55-c1-legacy-cb-n-stop.md`, with the full c0–c3 ledger (92 legs: 6 c0 readings, 19 c1 stack PASS, 6 legacy readings in the failed group, 61 not run);
- CLAUDE.md status block;
- **G294**: an identity gate that learns its allowed variance from one control pair per group will flag known run-to-run noise that pair didn't happen to show.

Unchanged: main checkout `m51` `53bf725`, `master` `031a103`, no tag, staged exe 002805CF. The scratch worktree is removed.

## 8. NEEDS-DECISION
1. **(A)** Is one unmeasured frame from the pre-existing mask-coalescing rule disqualifying for the legacy-identity gate, or should it be declared and scoped?
2. **(B)** Should the legacy rules compare `mask_value` and the CB mask hashes modulo a one-to-one renumbering?
3. **(C)** Should the cross-pair rule also accept the historical run-to-run set (`census_cycles`)?

Any predicate change needs either a pre-run amendment for CB-S, MAIN-N and MAIN-S, or a re-evaluation of CB-N from the bank as in 081-26. Either way it needs a boundary re-issue, because `cfff033` moved the feature head. c1 would then resume at `CB_S1`, followed by c2 and c3.

## 9. Wall-clock
- **c1:** 10 min. Preflight and quiet wait 31 s; the 14 stack legs ran 19:04–19:09 UTC and the 6 legacy legs 19:09:40–19:11:21 UTC; comparison and postflight followed.
- **c2 and c3:** not run.
- **Session:** about 40 min including the cause read and docs.

Several MCP connectors need authorising in their settings before they can be used, and the PDF viewer failed to connect. None were needed here.
api-equivalent cost (not billed on Max): $6.66 | turns: 43 | running total across all briefs: $2,210.91
