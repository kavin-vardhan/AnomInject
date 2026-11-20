# REPORT for 078-10-frozen-pairing-campaign-predictions.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-10-frozen-pairing-campaign-predictions.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 19:50:38 | finished: 2026-09-06 20:00:42 | duration: 10 min | exit code: 0
context at last turn: 463888 tokens (46% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-10-frozen-pairing-campaign-predictions.live.txt

---- FINAL MESSAGE (Claude Code) ----
Documentation and preflight complete. Nothing ran.

## 1. Preflight (read-only, all matched)

| item | value |
|---|---|
| `AnomalyInjector` HEAD | `d572b3dbcace8202f18de33ce9c697ab89f96037` → now `e0d63f11b5f2829894b31fcd0342683ca881f019` |
| `master` == `origin/master` | `ac13700d99fe116056f5aa11611f8931a563a3f2`, unmoved |
| production freeze | `6713f51..HEAD -- Source Shaders` **EMPTY**; tracked tree clean |
| handoffs | **10** preserved |
| CaptureBench | `472a409…`, 0 tracked mods, 7 untracked tools preserved |
| Lyra | detached `4a4bfcc…`, clean, untouched |

A `B8AC65D8…EAD8` (241,400,320 B), candidate `D50DDE78…4796` (241,401,856 B), staged runtime **== candidate** (untouched), and all five container files matched the disposition's full hashes. Runner `9E8D669F…` and checker `918EB4B5…` byte-unchanged since 078-08/09. No executable was staged or launched. **No `*QUARANTINE*` file exists anywhere under `D:\IntrusiveAnomalies` (depth 6)** — recorded, not chased.

## 2. Deliverable

`docs/predictions/2026-09-06-m51-frozen-pairing-recheck.md` — 22,783 B, 358 lines, sha256 `2E63E53DF9B40A3657A5AAE4E0FCDB28CCC4B179BE04B5947C57BB6DE21FE849`. Commit **`e0d63f1`** (402 insertions, 1 deletion — the deletion is the contract's trailing-newline artifact, verified re-added verbatim). Files changed: the new predictions record + a `docs/OPERATING-CONTRACT.md` §9 entry (append-only). `git diff --check` clean.

**Command inventory: 9 full commands, zero ellipses, zero "same as" placeholders.** Five campaign legs (`F1R_A_REF_NAT` → `F1R_B_NAT` / `F1R_B_SYN` / `F1R_B_NAT_SP50` / `F1R_B_SYN_SP50`) plus four conditional controls (`F1R_A_CTL_*`). Mechanically validated: 15 parameters each, **every parameter exists in the unchanged `run_leg.ps1`**, every integer parses, all 9 labels distinct, script/interpreter/checker paths all resolve. Bank prefix `M51R_` verified free across **45 bank paths + 36 attempt directories**, zero clashes.

**The Marker correction.** 078-05's smoke subrecord pinned `-Marker 0` while asserting it reproduced `PB1F_F_PAIR_*` "exactly (G184)". The banked Phase B legs pass **no** `-Marker`, so they ran at the runner default **`Marker 1`** — the claim was false. The campaign issues `-Marker 1` explicitly. Two limits recorded with it: `Write-LegGeometry` (`:167-188`) captures **neither `Marker` nor `AutoExposure` nor `MaxAttempts`**, so the Phase B value rests on the default and **not on a receipt**; and 078-06 measured that axis to move the verdict by **nothing** — so this is provenance hygiene, not an expected number change.

**Stop/retry logic.** Reference failure → stop, no control, no candidate. First *valid* candidate pairing failure → **exactly one** matching A-control → **stop regardless of result**. Refusal / undecidable / `<3` counted events → stop, and it does **not** count as a valid failure or release a control. `MaxAttempts 4` is a **pose-invalidity budget only**; an accepted pose-valid failure is **never** retried; physical attempts counted separately from accepted bank copies. Predicate unchanged; **exit 0 = READ, never PASS**; exits 2/3/4 are refusals; **`decidable == 0` exits 0 with `UNDECIDABLE` and is classified INVALID**. Exit 3 is structurally impossible (`COLLIDING_ANOMALIES = {"corrupted_texture"}`; every leg fires `blinking`).

## 3. Open limitations recorded in the draft

Two-position probe **cannot detect an even-frame slip** · the earlier failing state is **unexplained**, so a reference pass doesn't guarantee later state · no exact `CURRENT`/`no-data` counts predicted (arbitrary timing) · FPS/`speed_ratio` are telemetry, pacing band **not adopted**, no retroactive invalidation · failure without `F1` does not exclude an `F1` contribution · tool hashes are today's, **not historical receipts** · a candidate failure plus control **informs attribution, proves nothing** about environment, FIFO or client cause · identity/lifecycle work, "second arm stale" (narrowed to proven identity, never queue position), cross-domain frame-number equality, 8-bit+validity probe and `WithholdWorldRender` all **held and unproved** · `M-F1b` UNRUN, Lyra arm not waived.

## 4. Judgement call for you to override if wanted

I appended the **contract §9 entry only** and did **not** edit `CLAUDE.md`'s status block. Rationale: §9 is the documented authoritative ruling location that the status block already routes cold-start readers to, and appending bytes there avoids the emoji/encoding hazard of rewriting that large block. Say the word and I'll add a status line.

Two of my own checkers threw false alarms this session and were corrected before anything rested on them: the deletion-accounting regex (078-09) and the runner-parameter extractor here, which reported `-Label` as unknown because it captured `true` from `Mandatory=$true`. Both were checker defects, not document defects.

## 5. Final state

Nothing staged, launched, built, cooked or implemented. `master` unmoved; **nothing pushed** (`origin/m51` absent); no tag; no amend/reset/clean/stash; path-scoped adds only. Staged runtime still the candidate, container byte-unchanged, CaptureBench and Lyra untouched. Zero `StackOBot` processes; **no background tasks were started by this brief and none is running.** Observed but not touched: the owner reopened an editor (PID 29552, started 19:45:14) — recorded as metadata only, and a non-blocker since the campaign explicitly does not require editor absence.

Context: roughly **45–50% of 1M** (078-09 closed at 39%; this brief added preflight, three review docs, the M-F1 manifest record and the drafting) — comfortably below the 70% rollover boundary.

**VERDICT: NEEDS-DECISION**
api-equivalent cost (not billed on Max): $6.57 | turns: 23 | running total across all briefs: $1,227.28
