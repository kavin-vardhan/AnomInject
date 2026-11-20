# REPORT for 082-12-m53-s2-plan-rev2-delta.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-12-m53-s2-plan-rev2-delta.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 09:02:24 | finished: 2026-09-28 10:00:12 | duration: 58 min | exit code: 0
context at last turn: 701846 tokens (70% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-12-m53-s2-plan-rev2-delta.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. All five steps of 082-12 are done, but Codex's delta check says S2-0 cannot run as specified until chat rules on it.

**Summary for the owner**
- I rewrote the S2 plan to apply all eleven of Codex's findings, settled the m52/m53 sharing question from source, and wrote the shadow-trace probe (S2-0) as a 24-leg matrix. Nothing was built, cooked or run on the bench.
- Codex's check of the rewrite: 7 findings resolved, 4 partly resolved, and 3 problems in the probe spec. They are wording and harness fixes; the current build is still fine.
- Next is chat's ruling on that check, then S2-0 (it needs your OK on the PC).

**Rulings as applied** (the plan's new "Revision 2" section, R2.1–R2.11)
1. **Mode correctness:** new rows G-MODE-UV and G-MODE-N check which transform was drawn, on new fixture cells where every cell is distinguishable. Wrong modes are tested on real frames, e.g. an `invert` checked as if it were `green_flip` must fail. `noise` is compared against an offline reference baked from the shipped noise table.
2. **Drift snapshot:** qualified on its own by five new G-SNAP rows: identity against the original on colour, data and normal textures; proof the snapshot is what's bound; a real residency drop while drift runs; failure and abnormal-exit accounting; and a budget case that only the snapshot bytes decide.
3. **S2-0:** now its own section, §3A (summary below). The old probe design was removed from the premise window.
4. **Renamed successor:** keeps the cleanup contract (no owned MID left, falls back to the asset default, resources released) with no exact-restore claim. The same-name variant is the exact-restore control, and a skip-the-sweep negative must fail.
5. **G5 Restoring:** each leg must prove a measured residency drop at frame 35, an actual insertion (a new log line at the insertion point), and the refusal at frame 42, and must log that the pin cleared. The ratio claim is withdrawn.
6. **Level change:** one ticket carried from the old world to the new, then a separate after-GC census of render targets, which the runner waits for. It must be able to see a deliberately retained render target.
7. **Telemetry:** the serialiser gains empty-array support, with flat arrays and one slot identity used everywhere. The consistency check covers only real commits, and a new row G8-VAL proves non-m53 labels are byte-identical.
8. **RNG:** every m53 attempt uses exactly one mode draw, even with an empty mode set (which then refuses `no_mode_enabled`); other anomalies use none. Every draw is logged, a known-answer test checks it, and the scramble hash inputs are pinned down exactly.
9. **Cross-anomaly sharing:** see the determination below.
10. **Three predicates:** the invalid-mode refusal is proven through a real apply, not the census. The cancel-before-focus row allows the configuration echoes and compares stats against their reset values. The restore row expects each slot's actual outcome (exact, default or swept) instead of "exact" everywhere.
11. **Levers:** all new levers and `DestroyTarget` register only when the game is launched with `-IAIBench`, and are excluded from Shipping builds. A new row G-LEVER checks this. The check on the actual client build is recorded for M3.

**P2-9 determination: yes, the auto-injector can run m52 and m53 at once on different actors by default**
- The concurrency limit defaults to 4 (`AnomalyAutoInjectorSubsystem.h:132`), and firing only excludes an anomaly that is already live (AAI:241-252) and an actor already in use (AAI:285-289).
- In free-running mode each fire reverts when its own hold ends (AAI:187-197, :699-724). So m53 on actor A can revert while m52 still holds a texture A shares.
- m52 only counts real textures as users (SLM:101-116). An m53-corrupted component is bound to a generated render target instead, so m52 can't see it; with m52's default of refusing any shared texture (`AnomalyDefaults.h:82`), that check is silently bypassed.
- **Fix in S2:** m52 counts an m53-corrupted component as a user of its original textures; the new row G5-REV tests it.
- ⚠ This rests on reading "by default" as the concurrency rules. Out of the box, neither m52 nor the m53 anomalies are switched on in the pool (AAI:35-41).

**S2-0, in brief** (current S1 build, no build or cook)
- **Arms:**
  - m53 identity as in the S1 bank (fires at frame 42, 12 or 21 of 30 slots depending on the launch);
  - m53 identity on all 30 slots (fires at frame 62, budget raised);
  - `corrupted_texture` on the same 30 slots;
  - a plain render refresh with no material change (`Reattach.Components`, which reloads every instanced mesh in the level; that it hits more than the floor is recorded as a limitation).
- Each arm runs with the shadow cache on and off, and each cell has its own two non-installing nulls: 24 legs.
- Each leg has one event and a 60-frame clean tail. Slot state is read before and after through the existing census lever, scheduled with nested `DEFER`, and the first leg confirms those frames before anything else.
- The evaluator must first reproduce Codex's own numbers on the banked legs exactly.
- **Predictions:** the trace reproduces, disappears with the cache off, and appears with `corrupted_texture`; the plain refresh reproduces the installed image (the least certain prediction). The predicted verdict is a stale null image, i.e. a label caveat rather than a restore defect.

**Builds, cooks and sequence**
- Still 1 build pair and 1 fixture cook; the cook now also carries the new mode-checking cells. S2-0 needs neither.
- Premise window about 19 legs; gate window about 129.
- Sequence: chat rules on the delta → S2-0 → chat rules on S2-0 → S2-1 source → Codex source review → S2-2 fixture and cook → S2-3 harness → premise window → gate window → Codex evidence review → close-out.

**Commit:** `82c0ebd` on `feat/m53-uv-normal-corruption` (from `f6b47d4`), pushed; origin matches. It holds the plan, journal `2026-09-28-082-12-code-m53-s2-plan-rev2.md` and the status block. `m51`, `master` and tags are untouched.

**Codex delta:** `VERDICT: CHANGES REQUIRED` (652 s, collected to `_reviews\082-12-codex-m53-s2-delta.md`, ledger row added)
- Resolved: P2-4, P2-5, P2-6, P2-8, P2-9, P2-10, P3-1. Partial: P2-1, P2-2, P2-3, P2-7. No P1.
- **P2-D1:** the `corrupted_texture` arm skips m53's two warm-up frames, so it applies at frame 60 and its frames don't line up with its m53 nulls.
- **P2-D2:** NoApply-2 never prints `APPLIED`, so the null and refresh legs fail the one-event check.
- **P2-D3:** the G-MODE-UV drift recipe produces one event, but the row requires four.
- **P2-D4:** retaining a render target after release can't make the ledger read unbalanced, so that negative can't fail as specified.
- **P2-D5:** the flat bindings row drops association and layer index, so swapping two bindings goes unnoticed.
- **P2-D6:** the PLUGIN-WIDE verdict over-claims; it needs `corrupted_texture` compared directly with m53 identity and real evidence of a bad restore.
- S2-0 runnable: **NO**; D1, D2 and D6 block it.

**NEEDS-DECISION**
1. Chat's ruling on the delta (D1–D6) before S2-0.
2. My defaults in the revision, for chat to confirm or change:
   - the reading of "by default" in P2-9;
   - the tail being the same 60 frames in every S2-0 cell;
   - whether to add a refresh lever limited to the floor's components to S2-1, decided after S2-0;
   - the `noise` tolerance limit of 8 (above it, that check becomes an open debt, not a stop);
   - the `-IAIBench` name and registering levers at runtime;
   - where the M3 client-build check goes.
api-equivalent cost (not billed on Max): $27.08 | turns: 118 | running total across all briefs: $2,945.50
