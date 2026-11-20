# REPORT for 079-06-verifier-per-edge-validity-and-honest-scoring.md
verdict: UNDECLARED (no VERDICT line (2026-09-06 rule: fail closed - holds the next brief like NEEDS-DECISION)) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 079-06-verifier-per-edge-validity-and-honest-scoring.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-15 13:01:19 | finished: 2026-09-15 13:58:49 | duration: 58 min | exit code: 0
context at last turn: 637717 tokens (64% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\079-06-verifier-per-edge-validity-and-honest-scoring.live.txt

---- FINAL MESSAGE (Claude Code) ----
All six predeclared readings came back as written. **VERDICT: GREEN.**

## What was done

Codex's five findings were all accepted and all corrected. Four commits on `fix/verifier-field-validity`, pushed (`57e91cd..bff1b00`); **nothing merged**, `master` == `origin/master` == `ac13700`, checkout restored to `m51` `53bf725`, `Source/`/`Shaders/`/`measure_label_offset.py` byte-unchanged (the latter's blob SHA-256 matches Codex's own table exactly).

**AMENDMENT 4** was committed at `789dd8d` before a line of the correction existed.

## Readings, per P

- **P1 — selftest 24/24, `SELFTEST: OK`.** Not one of the 17 existing verdict tokens moved (`moving_over_cap`'s printed *reason* changed only). Both `NOT-VISIBLE` cases survive, so the verdict was scoped, not disabled. The three 079-05 fixtures now read `NOT-MEASURABLE`; the two burst-free shifted variants still read `ONSET-SHIFT(-1)`.
- **P2 — the pinned bench is unmoved.** All six `M49_GEDGE_*`: `PASS 4 / SHIFT 0 / NOT-VISIBLE 0 / PARTIAL 0 / NOT-MEASURABLE 0`, `tau=0.0040`, exit 0. `m_edge` is exactly `0.000000` on all 56 bench edges.
- **P3 — the fixed cohort, 780 cells.** Guard **off**: 376 recovered / **19 WRONG** / 385 refused. Guard **on** at 0.000140: 52 / **0 wrong** / 728. `REGION_CAP = 0.00014` from N = 736.
- **P4 —** all three aligned fixtures `NOT-MEASURABLE`; the guard-off column reproduces Codex's `ONSET-SHIFT(+1) ×2` / `NOT-VISIBLE ×1`. `codex_small_region` reads **0.0039 whole-frame against 0.1200 regionally** — the old cap was 0.040, so lowering it would have changed nothing.
- **P5 —** zero NOT-VISIBLE, zero SHIFT on all three moving sessions; `VERDICT UNREAD` on each; `A2L_LEGA` `PASS-PARTIAL (2 fully, 0 partly, 5 unread of 7)`.
- **P6 —** 12/12 batch checks OK, including a `--all` CLI run end-to-end.

## Three things worth your attention

**A real gate defect the required change exposed.** Refusing an onset for regional change also removed the lower bound it placed on the end search, which then re-found the onset and returned `-1` where `+1` was correct — a cell right with the guard off and wrong with it on. The search now runs before the refusal: *a refusal must remove an answer, never a constraint.* Nothing in the brief asked for this; only scoring the same cohort at both guard settings surfaced it.

**The cap has the same fragility 079-04 was criticised for.** It is pinned by one edge of one event (`A1L_LEGA` ev0 run[4,5] onset), margin 3.4 %, and that cell **inverted the sign** of an injected shift while its region looked quiet — so a small `m_edge` is necessary, not sufficient. Written into the journal rather than around it.

**The trade runs the unflattering way.** This did not make the tool more useful; it made it stop being wrong. Cohort recovery falls 48.2 % → 6.7 %, wrong falls 2.4 % → 0, and three of four real moving-camera sessions now read nothing at all — where they previously printed `VERDICT PASS` after reading nothing.

Journal `docs/sessions/2026-09-15-079-06-…`; gotcha **G262**; response for re-review at `_reviews/079-06-code-response-to-codex-079-05.md`; evidence `_bench_sessions_bank\M079_VERIFIER_EVIDENCE\079-06\` (17 files). CLAUDE.md was not touched, following this branch's own precedent — flagging it in case you want it updated.
api-equivalent cost (not billed on Max): $57.74 | turns: 186 | running total across all briefs: $1,557.13
