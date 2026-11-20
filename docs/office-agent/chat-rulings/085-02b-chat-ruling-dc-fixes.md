# 085-02b — Chat ruling on the m53 DC fixes (NEEDS-DECISION): accepted; three small residuals fixed in 085-02c; the Codex loop is closed for m53 source

Date: 2026-09-29 ~11:10 IST · Author: chat Claude · Branch `feat/m53-uv-normal-corruption` `225854e` (source `b52970a`) · Exe `76A04998` (archived, not cooked or staged)

## Accepted
- **F1 + F2:** trail classification (`trail_reopenable`, `label_tail`) and the provider over trails, carried trails, carried tails and unfinalized snapshots. The TAA off-frame and outstanding-receipt cases are exact; the old "closed releases" mutant fails.
- **The shared `IsCoEntryFrame`** used by both the in-run counter and `tools/exclusion_gate.py`, with planted co-entry frames failing.
- **F3** (`RecordAfterApply`; live = `IsActive()`), **F4** (the read-only scope, `stats_unchanged`, 17 planted cases fail), **F5** documented, **round-robin**, **the free-running bound.**
- **Suites and mutants:** exclusion 224, draw 571, pure 251, active-source 47, all mutants failing. Lever audit PASS. Build 0 warnings.
- **Codex re-check:** F2 and F3 RESOLVED.

## Rulings (085-02c; small, xhigh, no Codex re-check)
1. **N1 (high, on B-M53's identity-null path): YES, fix.** Bench levers that define their own mode semantics (identity, identity-redraw, tile-probe) take precedence: while one is active, a targeted no-mode fire keeps the mode empty, as before, and logs `mode_source=bench_lever`.
   - These levers exist only under `-IAIBench`, so client builds are unchanged.
   - Test both ways: a round-robin mode inserted under the identity lever must fail.
2. **F1 reverse residual (high; not on the recipes, but "never at the same moment" holds on every route): YES, fix.** The provider also reads the auto-injector's retained live fires and pending FireWindow snapshots for m53.
   - Truth-table case: a raw early `IAI.Revert` of m53, then a forced m52 fire, must be excluded until m53's retained entry clears.
3. **F4 residual (medium): YES, suppress** the first-use configuration echoes inside the census scope, so the office output is only the counts block. Extend `census_readonly_check.py` with a cold-process case.

## The loop is closed for m53 source
There is no Codex re-check of 085-02c. Its fixes are narrow and proven by tests both ways and mutants. Codex's next look is the **evidence review** at merge prep.

## Next
- **085-02c** (xhigh, fresh): the three fixes, then rebuild, suites, mutants, lever audit, archive. No cook or staging.
- **Then 085-03** (the fixture cook + B-M53 harness + dry run), **on the 085-02c build.**
- **Then 087-01.**
