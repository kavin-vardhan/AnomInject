# REPORT for 078-08-editor-resident-capture.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-08-editor-resident-capture.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 19:02:10 | finished: 2026-09-06 19:14:21 | duration: 12 min | exit code: 0
context at last turn: 353391 tokens (35% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-08-editor-resident-capture.live.txt

---- FINAL MESSAGE (Claude Code) ----
Everything verified. The RESIDENT half ran and the result is not what the plan expected.

## Result

**RESIDENT PASSES**, at full throughput, with the owner's editor resident.

```
TOTALS decidable=35  CURRENT=35  PREVIOUS=0  NEITHER=0  (same-pos=0 no-data=5)
VERDICT: MASK IS THE CURRENT RENDER on every decidable frame        checker exit 0
```

Predicate `NEITHER==0 AND PREVIOUS==0` → **satisfied**. Accepted on **attempt 1** (pose `modal_rot (0,0,0)`, `distinct=1`, `modal=100%`, `pose_match=True`); the 4-attempt budget was otherwise unused. Probe enablement was read back **from the run log** — spawn at tag 255 plus 40 alternating STEP lines — so the checker judged rather than refused.

**My primary prediction was wrong.** §6 named "RESIDENT fails, `PREVIOUS > 0`, ratio ≥ ~1.5". The pass branch I held open on 078-06's `F1A` counter-example is the one that fired.

| leg | time | editor | speed_ratio | fps | masks | decid | CUR | PREV | NEITH | verdict |
|---|---|---|---|---|---|---|---|---|---|---|
| `M51D_HI_OLD_NAT` | 18:25 | resident | 3.6412 | 8.24 | 19 | 13 | 4 | 9 | 0 | FAIL |
| `M51D_HI_NEW_NAT` | 18:26 | resident | 3.4256 | 8.76 | 19 | 13 | 4 | 9 | 0 | FAIL |
| `M51D_HE_RESIDENT_NAT` | 19:07 | resident | **1.0053** | **29.84** | 36 | 35 | **35** | **0** | **0** | **PASS** |

Same binary, container, recipe and box, ~1 hour apart. **An editor is resident on all three rows and the verdict still moves** ⇒ residency is *not sufficient* to produce the failure — measured twice now. What strengthens is the **throughput** association (five legs at ≈1.00 pass, five at ≥1.59 fail): still an association, confounded on time and throughput. No mechanism, threshold or editor policy (`G120`).

Other readings: 40 frames / 25 positive / 3 bursts, `ticks_per_captured_frame` 1.375, masks 36 measured / 4 hidden-blank / **0 unavailable**, key ring 54/54 missed 0 corrupted 0, `wanted_matches` 40, vetoed 0, 40 rows si 0..39 with no hole or duplicate, 40 PNGs zero zero-byte, 1280×720, canonical gapped cadence `{4,5,9,10} {16,17,21,22} {28,29,33,34}` plus one cap-truncated tail.

## The limitation that prices this pass

The resident workload is a **restarted** editor: PID 20300 ran at **1.32 cores sustained across the leg window** (100.2 s CPU / 76 s wall) against PID 25324's **2.63**. It does **not** reproduce the original resident condition, so its pass is weak evidence about that condition — predeclared as a limitation, now measured.

## What this does to the CLOSED half

The pair assumed RESIDENT would reproduce the failure so CLOSED could test recovery. It did not. CLOSED could now only return `(pass, pass)` — nothing about the failure, at the cost of an owner save/close cycle. **The pair has lost its discriminating power.** I did not run it, and I'd suggest reconsidering it rather than scheduling it. Not run: the 078-07 batching read (no failure to decompose), any control, load injection or candidate smoke.

## Commits and restoration

- `39afe83` — prediction record, committed **before** the leg
- `bf88f05` — journal `docs/sessions/2026-09-06-078-08-m51-editor-resident-half.md` + dated `OPERATING-CONTRACT` §9 note (appended bytes only; 29 add / 1 del, the deletion being the trailing-newline artifact)
- Evidence: `_bench_sessions_bank\M51_EDITOR_DIAG_EVIDENCE\` (11 files), bank `M51D_HE_RESIDENT_NAT`

Verified: source freeze `6713f51..HEAD -- Source Shaders` **EMPTY**, worktree clean; `master` == `origin/master` == `ac13700` unchanged; **10** handoffs preserved; staged binary **restored to candidate `D50DDE78…4796`** and hash-verified; all five container files **byte-unchanged**; no push (branch `m51` is local-only), no tag; zero `StackOBot` processes and no background task running. The owner's editor is **still open and untouched** (PID 20300, start 18:59:43) — runner cleanup kills `StackOBot*` only and structurally cannot reach `UnrealEditor.exe`.

**VERDICT: NEEDS-DECISION**
api-equivalent cost (not billed on Max): $11.25 | turns: 44 | running total across all briefs: $1,214.92
