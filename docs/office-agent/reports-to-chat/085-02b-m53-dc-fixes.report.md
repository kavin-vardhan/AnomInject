# REPORT for 085-02b-m53-dc-fixes.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 085-02b-m53-dc-fixes.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 10:03:51 | finished: 2026-09-29 11:01:43 | duration: 58 min | exit code: 0
context at last turn: 712210 tokens (71% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\085-02b-m53-dc-fixes.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner:** I fixed the four review findings, added the round-robin mode for Targeted captures and the timeout limit, then rebuilt. Both targets built with 0 warnings and every test passes. Codex's independent re-check then found one new serious problem, and it blocks 085-03 until chat rules. It came from the round-robin: a Targeted capture meant as the "identity null" baseline (the reference run that should corrupt nothing) would get a real corruption mode inserted and run one. Nothing was launched, cooked or staged.

**Items** (each tested both ways):
- **F1 + F2** — `AnomalyExclusion.h` (`FTrailFacts`/`ClassifyTrail`, new states `trail_reopenable` and `label_tail`) and the provider `QueryExclusionTrail`, which now scans trails, carried trails, carried tails and unfinalized snapshots.
  - The TAA off-frames case: m52 transition entries on frames 102–109, m53 admitted first at 111.
  - The carried tail-only next run: entries on frames 0–3, m53 admitted at 5.
  - The closed-at-503 trail with receipt 504 outstanding is excluded, both attached and force-detached.
  - Mutant 4 (the 085-02 "closed releases" rule) fails 22 checks; mutant 3 (tail ignored) fails 21.
- **EXCL-A gate** — `AnomalyExclusion::IsCoEntryFrame` is shared by the in-run counter and `tools/exclusion_gate.py`, which the 085-03 harness can import. Its selftest plants co-entry frames and they FAIL; a counter that disagrees with the artifact FAILs; a leg with `texcorrupt_admitted_after_unresolved > 0` reads INVALID.
- **F3** — `RecordAfterApply` in `ApplyAnomaly`; `IsIdFireLive` now reads `IsActive()` only. Mutant 6 (refusal drops the record) leaves an installed effect with no record and fails; mutant 7 (live needs a record) fails.
- **F4** — `AnomalyViewport::FReadOnlyEnumerationScope` covers the whole census body, and its end line becomes `end stats_unchanged=<0|1>`. `tools/census_readonly_check.py` PASSes, and all 17 planted cases FAIL.
- **F5** — documented as a bench-only limitation in readme §8.8, a new checklist box and architecture.md. The 085-03 harness must assert one RestoreDelay run per process.
- **Round-robin** — `TexCorruptPure::TargetedAttempt`/`RoundRobinNext` plus `TryFireSpecific`, logging `mode_source=round_robin`. KAT `GKatTargeted`: every targeted fire takes exactly 1 draw. Mutants 4/5/6 fail 162/46/27 checks.
- **Free-running bound** — `IsRestoringPastTimeout` plus `AllRestoringPastTimeout`. The exclusion releases exactly at timeout 120; mutant 5 fails.
- **Readme** — §8.8 gains the round-robin line, the exclusion "in the frames", the new counter definition, the census `stats_unchanged` meaning and the "all" pool note. The checklist grammar now requires `end stats_unchanged=1`.

**Suites and mutants:**

| suite | clean | mutants |
|---|---|---|
| exclusion | 224/0 | 9, all fail |
| draw | 571/0 | 6, all fail |
| pure | 251/0 | mutant fails with 56 |
| active-source | 47/0 | 3, all fail |
| m52 window | 282/0 | — |
| camera-clipping slab | 154/0 | — |

- Also clean: destructor check PASS (+6), label-rule selftest 24 OK, consistency and change-oracle OK, the KAT regenerates identically, and the comment strip changed 0 files.
- **Lever audit:** PASS on 46 names, selftest 11 OK; the binary audit PASSes on the exe and on the 5 DLLs.

**Build:** Editor took 16 actions in 266 s and Game 7 actions in 124 s; both exit 0 with 0 warnings on the first try. Game exe SHA-256 `76A04998…` (242,449,920 B), archived at `_binary_baselines\m53dcfix-76A04998\` and re-hashed there (6/6 match). The A44 scan finds every new token, and the new overlap text is present with the old text gone.

**Codex re-check** (1,243 s; collected byte-identical to `_reviews\085-02b-codex-m53-dc-recheck.md`):
- **Verdict:** NEEDS-DECISION.
- **F1 PARTIAL:** the m52 side is repaired. The reverse direction misses m53's retained auto-fire entry after a raw early `IAI.Revert` followed by a forced m52 fire. That sequence is not in the EXCL-A/EXCL-B recipes; residual severity high.
- **F2 RESOLVED.**
- **F3 RESOLVED.**
- **F4 PARTIAL:** first-use configuration echoes still print during a census in a cold process; residual severity medium.
- **NEW N1, high:** the round-robin overrides the bench identity, identity-redraw and tile-probe levers, which expect no mode argument. This sits on B-M53's identity-null path.

**Commits pushed:**
- On `feat/m53-uv-normal-corruption`: `b52970a` (source) and `225854e` (docs). No tag, no merge.
- `m51` (`53bf725`), `master` and `fix/m52-label-timing` (`02d15a4`) are untouched. The host is detached at `b52970a`.
- About 0.71M tokens of context used.

**NEEDS-DECISION:**
1. **N1 (high, bench path — blocks 085-03 under the loop rule):** should a targeted no-mode fire leave the mode out when a bench identity/redraw/tile-probe lever is active? That is Codex's suggestion; I did not make it.
2. **F1 reverse residual (high, not on the recipes):** should the provider also read the auto-injector's retained live fires and pending FireWindow snapshots for m53?
3. **F4 residual (medium):** should the first-use configuration echoes be suppressed inside the census scope?

The MCP connectors that need authorization (Gmail, Calendar and others) were not needed for this brief.
api-equivalent cost (not billed on Max): $34.78 | turns: 146 | running total across all briefs: $3,521.95
