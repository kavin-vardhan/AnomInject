# REPORT for 078-09-editor-closed-capture.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-09-editor-closed-capture.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 19:17:21 | finished: 2026-09-06 19:24:56 | duration: 8 min | exit code: 0
context at last turn: 392974 tokens (39% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-09-editor-closed-capture.live.txt

---- FINAL MESSAGE (Claude Code) ----
The pair is complete. **Both halves pass**, which lands on the predeclared "not reproduced" branch.

## The pair

| leg | accepted | editor | speed_ratio | fps | tpcf | masks m/b/u | decid | CUR | PREV | NEITH | no-data | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| `M51D_HE_RESIDENT_NAT` | 19:07:43 | resident | 1.005284 | 29.842 | 1.375 | 36/4/0 | 35 | 35 | 0 | 0 | 5 | PASS |
| `M51D_HE_CLOSED_NAT` | 19:19:36 | **absent** | 1.000230 | 29.993 | 1.350 | 39/1/0 | 37 | 37 | 0 | 0 | 3 | PASS |

Gap 11 min 53 s. Both accepted on **attempt 1** at the identical pose; the retry budget was otherwise unused and no valid checker failure was ever retried. Predeclared category: **both pass ⇒ the current failure is not reproduced and the pair discriminates nothing.** The predeclared **inversion did not occur** — it was retained and tested, not assumed away.

Controlled across the pair: same A-side `B8AC65D8`, same frozen container (all five hashes identical before and after both legs), byte-identical runner and checker, same recipe, SVE grab point, **natural** tick order, native 1280×720, shipping compiled defaults read from each run's own banner. Probe enablement read back from each run log (spawn tag 255 + 40 STEP lines), so the checker judged both and refused neither. Each leg: 40 rows si 0..39, 40 PNGs, zero zero-byte, canonical gapped cadence `{4,5,9,10} {16,17,21,22} {28,29,33,34}` plus a cap-truncated tail, vetoed 0, ring missed 0 / corrupted 0.

## What it shows, and what it can't

Closing the editor moved the reading only marginally (1.0053 → 1.0002; 29.84 → 29.99 fps). **The box was already at full throughput with the editor resident, so the pair never contained the failing condition it was built to test.** Against the 18:25/18:26 failures at ratio 3.64/3.43 and 8.2/8.8 fps on the same binary, container, recipe and box, the **throughput** association survives and the **editor-presence** one does not. Whether closing an editor restores a *failing* run is **untested**. No root cause asserted.

## Two corrections to my own 078-08 record, applied in place

- **"CLOSED could now only return `(pass, pass)`" was an overstatement.** The inversion was live; that it happened not to fire doesn't make the reasoning sound.
- **"1.32 cores vs 2.63" does not establish the tested workload was half as heavy.** Measured from each leg's own `t_wall`, the true capture span is **1.709 s / 1.700 s** — the 1.32 figure averaged the 76 s between receipts (~45× the capture, mostly launch and shutdown) while 2.63 averaged hours. **Workload equivalence remains unmeasured.**

Scope held: failure without `F1` does not exclude an `F1` contribution; script mtimes are not historical hash receipts. The banked 078-07 payload-loss finding stands, unrepeated.

One process note: my first deletion-accounting check reported "REAL LOSS" on the contract file — that was my regex failing on a line beginning with a markdown `- ` bullet, not a real deletion. Re-checked correctly: both diffs are purely additive (29+55 insertions, 2 trailing-newline artifacts).

## Artifacts

- `d572b3d` — paired section appended to `docs/sessions/2026-09-06-078-08-m51-editor-resident-half.md` + dated `OPERATING-CONTRACT` note (append-only, explicit UTF-8, no BOM)
- Evidence `_bench_sessions_bank\M51_EDITOR_DIAG_EVIDENCE\` (23 files, incl. `078-09-PAIR-CONSOLIDATED-EVIDENCE.txt`, both checker outputs, four receipts); banks `M51D_HE_RESIDENT_NAT`, `M51D_HE_CLOSED_NAT`

Verified: source freeze `6713f51..HEAD -- Source Shaders` **EMPTY**; `master` == `origin/master` == `ac13700` unchanged; **10** handoffs preserved, no other dirty paths; candidate `D50DDE78` restored and hash-verified after **each** leg; all five container files byte-unchanged; nothing pushed (`origin/m51` absent), no tag; zero editor and zero `StackOBot` processes; no background tasks. No owner process was ever closed or inspected beyond process metadata.

## Still open

Root cause of the 078-05/078-06 pairing failure · why the box ran at ~1.00 tonight and 3.4–3.6 an hour earlier · whether closing an editor restores a failing run · `M-F1b` unrun. Pairing release certification and every `m51` implementation item remain blocked.

**VERDICT: NEEDS-DECISION**
api-equivalent cost (not billed on Max): $5.79 | turns: 25 | running total across all briefs: $1,220.71
