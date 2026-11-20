# REPORT for 081-32-m55-legacy-identity-amendment-5.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-32-m55-legacy-identity-amendment-5.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-26 00:56:01 | finished: 2026-09-26 01:53:52 | duration: 58 min | exit code: 0
context at last turn: 714553 tokens (71% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-32-m55-legacy-identity-amendment-5.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

I implemented and proved AMENDMENT 5, and CB-N passes under it from the bank. But one of the ruling's required proofs cannot come out as expected: "signature present, but the colour differs → FAIL". I ran it as written, it came out PASS, and I stopped there rather than bend it. The CB-N ledger row is not written and the boundary is not re-issued, so no bench window can start until you rule.

## Summary for the owner
- The three rule fixes for the old-output check are built and tested. Under them, the group that failed last time (CB-N) passes from the banked data, with nothing re-run.
- One required test assumes the check compares the colour pictures, and it never has. Two runs of the same program already differ in 20–44 % of pixels, so there is nothing stable to compare.
- The harness is deliberately left unable to start a window until chat decides how to treat that test.
- After the ruling, the next step is short: record the CB-N result, re-issue the boundary, then 081-33 resumes at `CB_S1`.

## Pre-conditions — both clear
**Bank search** (`precondition-bank.json`). All 30 legacy legs banked since 081-11: 081-12 ×12, 081-22 D ×8 and C ×4, c1 CB-N ×6. 081-11 and 081-15 banked none. Each leg has one attempt log.

| binary | evidence | legs | with signature |
|---|---|---|---|
| pre-m55 0844220E | none (no m55) | 8 | **0** |
| build 3 002805CF | OFF | 6 | **1** (CB_N2 si 29) |
| build 3 002805CF | ON | 4 | 0 |
| stage 1 AEBD09EA | OFF | 4 | 0 |
| stage 2 A699E9EE | OFF / ON | 4 / 4 | 0 / 0 |

- CB_N2 si 29 is the only `TARGET MASK UNAVAILABLE` line in the whole legacy bank.
- Build 3 shows it on 1 of 10 legs, below the ≥ 3 threshold, so the stop rule does not fire.

**Source diff** (`precondition-source.json`, 14 of 14 checks, `031a103` vs `85c99b3`):
- The `PixelOwner` selection and the `k == PixelOwner` hand-off are byte-identical.
- `Drain_RenderThread` and `AfterTonemap_RenderThread` lose no line. Every added block is either parallel-array bookkeeping or guarded by an issue-validity check.
- In `ServiceTargetMask`, the UNAVAILABLE decision is unchanged and its additions are guarded by `ChangeStage.IsValid()`. The one rewritten call passes the same pixel bytes when there is no frozen mask.
- `ChangeStage` exists only when evidence is ON, so with evidence OFF no issue on this path is ever valid.
- The `Issue.IsValid()` block is the only change inside the PixelOwner loop itself. Around it there are more additions, all of the same guarded kind.

## Proofs
All evidence is under `_reviews\081-32-evidence\`.

| # | Expected | Actual | Evidence |
|---|---|---|---|
| 1 A negatives | FAIL / NEEDS-DECISION | **8 of 9.** No signature (×2) → FAIL. Non-derived field, another frame, or `injected_frames` differs → FAIL. Two frames in one leg, two legs, or a third build-3 leg → NEEDS-DECISION. Two frequency controls PASS. **Colour differs → PASS, not FAIL** | `am5-proof.json` |
| 1 A symmetric | excused frame on a pre-m55 leg and on a build-3 leg → PASS; same frame without signature → FAIL | 5 of 5 | `am5-proof.json` |
| 2 A positive | CB_N2 si 29 excused | as expected; the old lib FAILs the same readings | `proof/cb-n-amended.json` |
| 3 B negatives | FAIL | 6 of 6: many-to-one, inconsistent across frames, one pixel, per-tag count, actor identity, event swap | `am5-proof.json` |
| 4 B positive | CB_N5 equal under π | valid; all 30 masks pixel-identical after remap; 89 paths excused | `am5-proof.json` |
| 5 C | `census_cycles` excused; a field outside the set still FAILs; set hash unchanged | 10 of 10, both orders; a tampered copy of the set is refused | `am5-proof.json` |
| 6 No flips | every earlier verdict unchanged | 16 comparisons and dry-run mutations. The only flip is c1 CB-N FAIL → PASS (intended). No PASS turned FAIL. Readings on all 30 legs are byte-identical to the old lib's on every existing key | `proof6-no-flips.json` |
| 7 CB-N re-evaluation | PASS-REEVALUATED | **PASS-REEVALUATED, computed but not written** | `cb-n-reevaluation\ledger-row.json` |
| 8 Boundary + dry run | re-issued, all green | **Not re-issued.** Dry run 198/198 (081-30's 182 unchanged + 16 new). Resume proof 5/5: with the row, the first launch is `R23_CB_S1` | `dryrun\comparison-vs-081-30.json`, `resume-proof.json` |

Two synthetic cases and three resume scenarios first failed because of mistakes in my own fixture and checker, not the harness. I corrected them and recorded them in the journal.

## CB-N re-evaluation
**A excusal:** CB_N2 si 29.
- Served line: `[2026.09.25-19.10.03:648][ 41]… M23 PASS id=2305843009213693961 servedArms=3 ids=[ 2305843009213693961 4611686018427387937 2305843009213693962 ] …`
- Unavailable line: `[2026.09.25-19.10.03:711][ 42]… Warning: Capture(m43): TARGET MASK UNAVAILABLE for session_index 29 (id=2305843009213693962, 1280x720, pixels=0) …`

**B π** (CB_N5, relative to CB_N1): 247→227, 248→247, 249→248, 250→249, 251→250, 252→251, 253→252, 254→253.

**C excusal:** `run_summary/census_cycles` is excused on 8 cross pairs. The values are 26, 26, 27, 27, 26, 26, and are kept as a reading for Stage 3.

## Boundary
- Not re-issued.
- The lib is now `c36a60c2`; the pinned value is still `bb4fbb9d`. The window runner is unchanged at `56e86f2e`, and `prep-boundary.json` is unchanged at `7204caed`.
- The feature head is now `ef2ffca`; the pinned value is still `66976ea`.
- An offline replay of the unchanged preflight refuses c1, c2 and c3 for exactly those two reasons (`preflight-refusal.json`).
- No 081-33 invocation yet. After the ruling:
  1. `python _reviews\081-32-evidence\reevaluate-cb-n.py --commit`
  2. re-issue the boundary (lib and feature head) and replay the preflight;
  3. re-run the dry run only if the lib changes again;
  4. then 081-33 runs `081-23-window.py c1 540 081-33`, which resumes at `R23_CB_S1`.

## Deviations
- **Six extra excused fields.** Ruling A names 14 of the 20 paths the excused frame moves. The other six follow arithmetically from the frame being unmeasured: `target_drawn_pixels` −1, the event's `observable_frame_count` −1 and `unmeasured_frame_count` +1, and run_summary `observable_frames` −1, `target_drawn_pixels_measured` −1, `target_mask_frames_unavailable` +1. I excuse them only at the exact value the writer would produce, by re-reading the comparator as if that frame were unmeasured.
- **Group ledger rows.** Legacy groups now get their own ledger row. A group already recorded PASS or PASS-REEVALUATED is skipped; a recorded FAIL or NEEDS-DECISION stops again without re-evaluating.
- **Local ref moved.** I fast-forwarded the local feature ref to origin. 081-31 had pushed `cfff033` from a worktree and left the local ref behind.
- **Archive location.** When run with `--commit`, the re-evaluation script archives the 081-31 c1 files under `081-32-evidence\c1-invocation-3-081-31\`, per the evidence-location rule. Earlier sessions put that archive inside `c1\`.
- **Stray file.** A 0-byte `081-23-lib.py` in the repo root predates this session (25-09 12:32). I left it alone.

## Commits pushed
`feat/m55-change-evidence` `cfff033..ef2ffca`, docs only, from a scratch worktree that has since been removed:
- the 081-32 journal;
- predictions AMENDMENT 5, marked declared but not yet released;
- the CLAUDE.md status block;
- gotchas: G295 (the coalesced-arm yield issue, with the FUTURE per-arm readback fix), G296 (the colour-proof lesson), and G294 marked resolved in the harness.

The main checkout is on `m51` `53bf725`, and `master` is `031a103` == origin. No build, merge or tag.

## NEEDS-DECISION
1. **Proof 1's colour negative.** No legacy rule compares colour frames, and they are not byte-stable (CB_N1 vs CB_N6, both pre-m55, differ on 184,214 px at si 29). Options:
   - drop it as out of legacy scope for every frame, since its colour-side label fields are already covered and FAIL;
   - add a colour-file presence and metadata check for the excused frame;
   - define a colour comparison with a tolerance, which is a new threshold.
2. **The six derived fields.** Please confirm the reading in the first deviation above.

The PDF-viewer connector failed to connect, and several other connectors need authorising in their settings. None were needed for this run.
api-equivalent cost (not billed on Max): $20.69 | turns: 132 | running total across all briefs: $2,231.60
