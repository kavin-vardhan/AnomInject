# REPORT for 071-03-g-edge-legs-and-cold-start-prep.md
brief: 071-03-g-edge-legs-and-cold-start-prep.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-04 13:45:49 | finished: 2026-09-04 14:00:40 | duration: 15 min | exit code: 0
context at last turn: 642391 tokens (64% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\071-03-g-edge-legs-and-cold-start-prep.live.txt

---- FINAL MESSAGE (Claude Code) ----
All 16 legs plus every attempt are banked (34 dirs including the two discards), no stray processes, tree clean on master.

```
# 071-03 REPORT — m49 step 2: G-EDGE PASSES, 8 legs, both tick orders
Bench legs only. No Source/ or Shaders/ edit, no build, no cook, no tag.

## 1. Plain-language rundown
1. No build was needed and I proved it rather than assumed it: nothing under Source/ or Shaders/ has changed since m48, so the staged exe DE65F84A IS master's binary.
2. Predictions were written and committed (94d461c) before any leg ran, including the pass condition and what would falsify the prediction.
3. The verifier re-proved it can fail (selftest exit 0) before the first leg.
4. All eight gate legs PASS: 32 events, 40 edge pairs, 0 shifts, 0 NOT-VISIBLE, 0 NOT-MEASURABLE, every leg exit 0 at HIGH confidence.
5. This closes all three gaps the survey found: missing_object is now in an edge gate, the END side is asserted for the first time on master, and the picture clause is a verdict rather than a printed reading.
6. The P9 signature reproduced on the bench — blinking labels {4,5,9,10} native vs {4,5,6,10} synth — and BOTH read PASS against the pixels, so the difference is real rendering, correctly labelled in both orders.
7. The eight delivered-configuration legs are report-only and also read PASS; two show the G228 contamination exactly where expected.
8. One honest limit found by measurement: the AA-off arbiter REDUCES signal, leaving missing_texture at 5.7x tau — the thinnest margin in the set. Reported, not tuned.

## 2. Binary proof
git diff --stat 20db6e5..HEAD -- Source Shaders  ->  EMPTY (all four commits since m48 are docs/tools).
Staged exe: DE65F84A, 241,236,992 B. Container quartet UNCHANGED: utoc EF8EB23C / ucas A8BFFF88 / pak 3C026A8D. No build, no cook.
Instrument selftest before leg 1: "SELFTEST: OK", exit 0 (7 cases, both edges, both directions).

## 3. Gate table
GATE LEGS (AA-off arbiter, pass/fail):
| leg | order | events | PASS | SHIFT | NOT-VIS | NOT-MEAS | exit |
| MT_NAT missing_texture | native | 4 | 4 | 0 | 0 | 0 | 0 |
| CT_NAT corrupted_texture | native | 4 | 4 | 0 | 0 | 0 | 0 |
| MO_NAT missing_object | native | 4 | 4 | 0 | 0 | 0 | 0 |
| BL_NAT blinking | native | 4 | 4 | 0 | 0 | 0 | 0 |
| MT_SYN missing_texture | synth | 4 | 4 | 0 | 0 | 0 | 0 |
| CT_SYN corrupted_texture | synth | 4 | 4 | 0 | 0 | 0 | 0 |
| MO_SYN missing_object | synth | 4 | 4 | 0 | 0 | 0 | 0 |
| BL_SYN blinking | synth | 4 | 4 | 0 | 0 | 0 | 0 |
DELIVERED CONFIG (report-only, G228 — no pass/fail):
| MT_NAT_DEL | 4 | 0 | 0 | 0 | HIGH | 0 |   | CT_NAT_DEL | 4 | 0 | 0 | 0 | HIGH | 0 |
| MO_NAT_DEL | 4 | 0 | 0 | 0 | HIGH | 0 |   | BL_NAT_DEL | 4 | 0 | 0 | 0 | HIGH | 0 |
| MT_SYN_DEL | 4 | 0 | 0 | 0 | HIGH | 0 |   | CT_SYN_DEL | 4 | 0 | 0 | 0 | HIGH | 0 |
| MO_SYN_DEL | 4 | 0 | 0 | 0 | LOW CONTAMINATED=1 | 0 |   | BL_SYN_DEL | 4 | 0 | 0 | 0 | LOW CONTAMINATED=1 | 0 |
Every leg: ceiling +/-7 (min clean gap 14; 15 for blinking), baseline 37-41 clean frames, region = the delivered mask.

## 4. Verdict
G-EDGE PASSES. All eight gate legs meet the pre-declared condition (SHIFT 0 and NOT-VISIBLE 0, at least one measurable event per leg).
Two readings differ from the pre-declaration and both are recorded rather than glossed:
- NOT-MEASURABLE came out ZERO, where §6 predicted one per leg (the truncated final event). At post=14 no event is truncated. The prediction was CONSERVATIVE, not wrong — it was written against the shipped post=4 schedule. The gate is stronger than pre-declared.
- The P9 signature: BL_NAT {4,5,9,10} {26,27,31,32} {48,49,53,54} {70,71,75,76} vs BL_SYN {4,5,6,10} {26,27,28,32} {48,49,50,54} {70,71,72,76} — {n,n+1,n+5,n+6} vs {n,n+1,n+2,n+6}, hidden count conserved at 4, in both configurations. BOTH legs PASS 4/4 at every edge, so the set difference is a real difference in which frames rendered hidden, correctly labelled in both orders. This is new evidence: m40 compared hidden SETS across orders; G-EDGE compares the label against pixel ground truth at both edges. NO MECHANISM ASSERTED (G120); nothing is claimed about the host codenamed Bates.
Measured property of the arbiter (journal §12.7): AA-off REDUCES d for every type — missing_texture 0.0229 vs 0.2627 delivered (5.7x tau, thinnest in the set); corrupted_texture 0.6681 vs 0.9968; missing_object 0.5442 vs 0.8186; blinking 0.5445 vs 0.8276. All decisive against tau=0.0040, but missing_texture has an order of magnitude less headroom than the rest and is the one to watch.
Leg hygiene (A63): 14 of 16 accepted attempt 1. CT_NAT try1 POSE GATE FAILED and BL_NAT_DEL try1 A47 ROTATION GATE FAILED (modal_rot (0.0,0.35,0.0)) — both "CAUSE NOT ESTABLISHED", banked, re-ran clean. No leg was re-run for the number it produced.

## 5. Commits / bank / checkout
Plugin repo, branch master, pushed ae54812..9083d95 (master == origin/master == 9083d95, tree clean):
  94d461c  docs(predictions): m49 G-EDGE pre-declared - 8 legs, both tick orders   [committed BEFORE any leg]
  9083d95  docs(071): m49 step 2 - G-EDGE passes on 8 legs, both tick orders
Bank: _bench_sessions_bank\M49_GEDGE_* — 34 dirs (16 accepted legs + every attempt, including the two discards). 0 stray processes.
Checkout: on master at 9083d95 as instructed (switched off the parked europa-e1 1f5e305 in 071-02, tree clean at the switch). Europa remains parked.
COLD START POINTERS: CLAUDE.md status block (rewritten as the "you are here": m49 in progress, step 1 b062832 DONE, step 2 G-EDGE PASS, Phase A NEXT, the Q3-Q6 rulings, the mask_value-0 precondition, checkout on master) -> docs/sessions/2026-09-04-071-concorde-feedback-triage.md, which now OPENS with a "COLD START FOR 072" block naming the reading order (CLAUDE.md status -> §8 the plan / §8.1 Phase A file list -> §11 step 1 -> §12 step 2 -> predictions -> gotchas tail).

## 6. NEEDS-DECISION
None. G-EDGE passed; nothing is blocked and nothing was stopped.
Carried, not mine to close: the Bates delivery window (owner's, pending); three client questions chat has asked (F1's selection route; MP4-vs-PNG and viewer base; mask.provided for the two not-visible events). Phase A precondition recorded in both CLAUDE.md and journal §11.2: a labels row can carry mask_value 0 while its mask PNG carries the event's real tag — target_pixels' join must not inherit it.
```
api-equivalent cost (not billed on Max): $16.67 | turns: 33 | running total across all briefs: $726.48
