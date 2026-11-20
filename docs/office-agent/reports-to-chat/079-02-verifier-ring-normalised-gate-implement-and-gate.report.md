# REPORT for 079-02-verifier-ring-normalised-gate-implement-and-gate.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 079-02-verifier-ring-normalised-gate-implement-and-gate.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-15 11:08:35 | finished: 2026-09-15 11:32:02 | duration: 23 min | exit code: 0
context at last turn: 515799 tokens (52% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\079-02-verifier-ring-normalised-gate-implement-and-gate.live.txt

---- FINAL MESSAGE (Claude Code) ----
State is clean and verified. Writing the report.

---

# 079-02 — REPORT

## Outcome

**I built F-A..F-H exactly as specified, and the gates refuted F-A.** `P1` and `P5` pass; **`P2`, `P3` and `P4` fail.** The predeclared stop fired. **No constant was tuned. Nothing is merged.**

`tau` still read `0.0040` on every pinned bench leg and `base=` dropped 37→24 exactly as AMENDMENT 1 declared — **so the failures are not threshold drift.**

| | prediction | reading | |
|---|---|---|---|
| **P1** selftest | 16/16 incl. moving | **16/16 OK** | ✅ |
| **P2** pinned bench inert | 6× `PASS 4`, `tau=0.0040` | 4 legs PASS; **`MT_NAT`+`MT_SYN` → NOT-VISIBLE 4** | ❌ |
| **P3** `M50L_LG9` flips | 0 NOT-VISIBLE | **4 NOT-VISIBLE, FAIL** | ❌ |
| **P4** other Lyra legs | no new NOT-VISIBLE | `A1L_LEGA` **+1 SHIFT**; `LYRA_SMOKE_01` **+1 NOT-VISIBLE** | ❌ |
| **P5** batch | counts identical, `CANNOT RUN` ok | **identical; handled** | ✅ |

## Why F-A failed — three measured reasons

**1 (P2) — ring subtraction destroys a real small signal on a *pinned* camera.** `MT_NAT` event 0 at onset: `d_region = 0.02286`, `d_ring = 0.02872` ⇒ **`net = −0.00586`**. The anomaly turns 1,528 target pixels hot **and 812 ring pixels** hot (GI bounce), and the ring is **less than half** the silhouette's area (28,272 vs 66,837 px — the target hugs the bottom-left corner so the band is clamped on two sides). **`m26`'s `A35` already measured that these anomalies change pixels outside their own bbox** (`SM_Ramp2` peak-OUT 0.2955 > peak-IN 0.1785). Journal 071 named this exact leg *"the thinnest margin in the whole gate set… the one to watch"* at **0.0229** — I measured **0.02286**.

**2 (P3/P4) — ring subtraction does not cancel camera motion.** `tau` on `net` still reads **0.18–0.44** on `M50L_LG9`, because motion is parallax- and content-weighted, not a global scalar. That **corrects `measure_label_offset.py:92-93`'s documented claim**; the module survives only because it *also* carries an ambient⇒UNMEASURABLE escape — which the gate didn't copy. (Mirror image of 079-01: it copied the constants and not the ring; I copied the ring and not the escape.)

**3 — 🔻 I must withdraw a claim from my own 079-01 report.** All six `M50L_LG9` events carry **`observability_measured: false`** and **`bbox_source: "projected"`** — the producer declares it has **no pixel evidence**. Calling them "known-good labels" was an over-claim: journal 076 celebrated `LG9` for *shipping honest events instead of `anomalies: []`*, explicitly unmeasured — never pixel accuracy. The pixels agree: event 2's labelled onset has the **lowest** local excess in its neighbourhood, and an amplified difference at the onset is **indistinguishable from a clean→clean control**. **A known-good real session for this gate does not yet exist** — and that is now a precondition for closing the unit.

## What survives

**Seven of eight elements are unimplicated: F-B, F-C, F-D, F-E, F-F, F-G, F-H.** Only **F-A** is refuted. F-B fired correctly in selftest and would convert the field's NOT-VISIBLE into an honest NOT-MEASURABLE; F-C correctly caught the 100%-of-frame event; F-H is now the thing that would catch this class.

## Recommendations for 079-03 (chat's call)

1. **Use the ring as a validity TEST, not a subtraction** — keep raw `d`, add `mlo`'s own `ambient > AMBIENT_FACTOR·tau ⇒ NOT-MEASURABLE`. With F-B that satisfies rule (a) and leaves the pinned bench **byte-inert**. ⚠ **It probably does not satisfy rule (b)** on heavy-motion sessions — my measurements suggest there may be **no recoverable signal there at all**, in which case saying so is the right answer.
2. **`observability_measured: false` + `bbox_source: "projected"` ⇒ NOT-MEASURABLE by construction** — schema-derived, not tuned.
3. **Get a real surrogate**, and the cheapest route is the owner's still-missing transcribed readings — ask specifically for `observability_measured` and `bbox_source`.

## State

`fix/verifier-field-validity` = **`1197d5c`** (pushed; `b4ccd11` predictions → `1197d5c` implementation, subject marked **GATE FAILED, DO NOT MERGE**). `master` == `origin/master` == **`ac13700`**, untouched. `m51` = **`53bf725`** — the one docs-only contract commit; local-only and unpushed, as throughout 078; **releases nothing, m51 stays HELD.** Checkout restored to `m51` where I found it, tree clean. `measure_label_offset.py` **byte-unchanged**; `Source/`/`Shaders/` untouched; no capture, build or cook. Comment stripper: 0 changed / 97 no-change. Evidence: `_bench_sessions_bank\M079_VERIFIER_EVIDENCE\` (20 files).

**VERDICT: NEEDS-DECISION**
api-equivalent cost (not billed on Max): $18.66 | turns: 67 | running total across all briefs: $1,463.16
