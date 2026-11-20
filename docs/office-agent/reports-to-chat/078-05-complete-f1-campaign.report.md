# REPORT for 078-05-complete-f1-campaign.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-05-complete-f1-campaign.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 17:10:20 | finished: 2026-09-06 17:38:50 | duration: 28 min | exit code: 0
context at last turn: 423860 tokens (42% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-05-complete-f1-campaign.live.txt

---- FINAL MESSAGE (Claude Code) ----
All verified: `master` unchanged at `ac13700`, production source diff vs `6713f51` is **empty**, staged exe is the candidate, container byte-unchanged, owner's editor untouched.

---

# 078-05 — F1 candidate campaign complete

## Commits (branch `m51`, nothing pushed)
| commit | what |
|---|---|
| `6713f51` | F1 source — **unchanged, production frozen** (diff vs it is empty) |
| `53d42c2` / `09c636f` | 078-04 predictions + the mask-off amendment |
| `0cb7644` | **078-05 smoke-subrecord amendment — committed before any smoke leg** |
| `6bd56ba` | journal + compact CLAUDE status + contract §9 |

Changed source paths (`master..HEAD`, F1 only): `AnomalyCaptureSubsystem.cpp` (+47/−11), `AnomalyLabelWriter.h` (+2), `AnomalyCaptureSubsystem.h` (+3/−1). `master` = `ac13700`; `be16977` on origin.

## Build recovery
078-04's killed link left a **398,694,416 B** exe where the whole artifact is **~241 MB**, and its task output was **10 bytes reading only `[killed]`** — no build log survived. `G164` with the sign flipped: the partial was *larger*, and only size told them apart. Quarantined in place as `…QUARANTINE-078-04-interrupted-link-9D23AC23`, **never executed**; removing it forced the Link action.

Rebuild: **exit 0**, `[1/2] Link StackOBot.exe` executed, 44.94 s, full log banked.
- candidate **`D50DDE78…4796`**, 241,401,856 B (+1,536 over A-side) · archived · staged once · **built == staged**
- A-side **`B8AC65D8…EEAD8`**, 241,400,320 B
- container **`67EA1FE0`/`2CEFB8F4`/`E03C6610`/`A16A18A8`/`C70ECDAA` — hashed before *and* after every stage, byte-unchanged**
- A44 on the **staged** artifact: 3 new F1 symbols present, 3 pre-existing controls also present ⇒ **sound, not blind**

## The certifying result — both tick orders, B1 live, accepted attempt 1

| | A `B8AC65D8` | B `D50DDE78` |
|---|---|---|
| frames written / dropped | **33 of 90 / 57** | **90 of 90 / 0** |
| `NEVER RESOLVED` · events | 33 · 3 | **0 · 8** |
| F1 warning | 0 | **exactly 1**, reason from the banner's own source |
| rows/PNGs · holes/dups/orphans | 0..32 · 0/0/0 | **0..89 · 0/0/0** |
| row / anomaly keys · schema | 13 / 12 · 2 | **13 / 12 · 2 — unchanged** |

`mask_state` unmeasured, `mask_file` null, `target_pixels`/`target_drawn_pixels` −1, `observable` null, `mask_value` 0; `observability_measured` false on all 8 with `affected == injected`. **Every predicted B-side signature met.**

Resized `OutputHeight 360` arm: 3 attempts/side, PNG IHDR **640×360** as predicted; **B1 NOT APPLICABLE (`G117`)** — ratio `(None, 0.5, 0.4998, 0.5)`, `modal_rot (0,0,0)` ⇒ resolution scope. B1 not weakened. Reported as a reading, never a pass. One synth attempt read 7 events (missing the truncated `[88,89]`, the `A50` case).

## ⚠ The failed checker
**ONSET/G1/G2/G3/G3b/G3c/G7: ALL PASS**, both orders, exit 0.

**MASK-PICTURE-PAIRING failed** `NEITHER == 0 AND PREVIOUS == 0`: `PREVIOUS` = 9/12/10/17 vs a banked baseline of 0.

The `G184` payload diff was **empty**, so I ran an interleaved control — and **the pre-fix binary `B8AC65D8` reads `PREVIOUS 10` today on that byte-identical payload**. Second recipe confirms: cap-90 mask yield is **35 banked, 23 on the same binary today, 25 on the candidate** — today's A-side is *lower* than the candidate.

⇒ **Not attributable to F1; the split is by time/environment.** `G235` in full: today's A-side is off-baseline, so this compares two failing cells and *"it moved nothing vs the previous binary"* is **not** claimed as a pass. **Cause not established, not chased** (`G120`); the resident owner editor (`G97`) is a **named lead, not a finding**. No tolerance widened, no re-run-until-pass, and the `si=8` limit is **not** invoked — this is far broader.

The smoke recipe also **never enabled its own probe** (`m44_pairing_probe.py` refuses with exit 4); fixed and committed before any smoke leg, along with the natural-only SP50 arm, abbreviated checker CLIs, and the cap-40 pairing recipe.

## Corrections to the 078-04 record
1. **"10 runs" duplicate-counts** — there are **eight** unique attempts, verified **by session id**: `M51_F1A_MOFF_NAT` and its `_try1` are both `session_20260906-170307`; the `MOFF_SYN` pair both `…-170352`.
2. The **18–30 written-frame band was missed at 33** — mechanism exact, rate arithmetic pessimistic. Preserved, not retro-fitted.
3. **Workflow incident:** 078-04 ended while its build was still linking, that build was killed, and its `GREEN` was **derived by the harness**, contradicted by its own final message. No mailbox change made.
4. Instrument fix: `read_f1_legs.ps1` dropped index **0** (falsy in PowerShell) — a verification-tool defect, not a build defect.

## Open / unrun / refused
`M-F1b` backbuffer **unrun** (`bSveCapture` deliberately excluded; backbuffer × target-mask has never run) · **pairing cannot certify today** · F2, F6, F14 + 13 more **held** · no Lyra, cook, push, merge, tag, office action.

Evidence: `_bench_sessions_bank\M51_F1_EVIDENCE\` (19 files incl. build log, provenance, per-leg checker output, consolidated read) + 39 banked M51 leg dirs.

**VERDICT: NEEDS-DECISION** — F1 itself passes every gate it was released against, in both orders, but the released MASK-PICTURE-PAIRING checker fails its declared predicate on the candidate *and equally on the pre-fix binary*, so that gate cannot certify anything today and the environmental cause is unestablished. Per the brief, no source fix and no further campaign follow; stopping here for your evidence review.
api-equivalent cost (not billed on Max): $17.41 | turns: 75 | running total across all briefs: $1,184.00
