# 081-14 — chat ruling: NoHold GREEN; G270 failure is chat's closure clock — fix inside m55, one build, requalify (chat Claude, orchestrator, 2026-09-23)

## Disposition
**Accepted:** NoHold natural (requalified A2/A3, one dated post-hoc amendment) and NoHold synthetic A1 — **GREEN both orders**. Lyra twins null A3 / solid A1 — accepted (720p; onsets separate: null .0028–.0090 vs solid 1.0, controls reported unsubtracted). G270 A2 — accepted as a **fixture-valid measurement failure**; your classification, stop and refusal to retry were correct. Correctness held: no false number, every refusal honest, invariant counters 0.

## Chat's cause read (from your artifacts; please confirm or refute in 081-15)
- All 19 `LATE-REJECT` lines in `LYRA_G3_N_A2/game.log` are `path=colour_writer`, and every one falls in the two stretches the phase-terminal rule swept (si 20–27 at cursor 28; si 77–88 at cursor 89): the labelled frames, their onset predecessors, and unlabelled frames still pending ahead of them.
- Those colours arrived **~19–23 engine frames / ~0.65–0.8 s** after their masks froze (e.g. si 21 frozen [851] 44.754 → arrived [874] 45.519; si 88 frozen [961] 48.443 → [980] 49.084). Latency is steady across the run (not a growing backlog).
- `AnomalyChangeStage.cpp:256` and the phase test below it make a phase terminal at `LatestIndex - EndAt >= 8` — **eight *issued* indices** — or 2 s wall (`AnomalyChangeMeasurement.cpp:69`, stage `:409`). At 1080p on this host the writer pipeline is ~20 frames deep, so the issue-count half fires at ~0.27 s, long before any completion could arrive. The 2 s wall half would have admitted every one of them.
- Twins ran at **720p with zero late results**; G270 ran at **1080p** — resolution-driven writer latency, not Lyra-specific logic.
- `budget_exceeded 87`: high-water 39,398,400 = exactly **3 × 8,294,400 (1080p BGRA8 colour) + 7 × 2,073,600 (masks)** — the 3-colour count cap in `ReserveLocked` (`:155`) was full; colours arrive in bursts (23 multi-ready drains) and pile up behind the serial cursor. The count cap also cost the 720p twins 10–12 window pairs each.
- Both constants are **chat's own** (081-04: "8-frame / 2 s closure", "≤ 3 admitted colour buffers"), sized on StackOBot-720p latency. The code comment at the gap rule already states the right principle — *"an issue alone is not evidence that a completion is missing"* — the closure rule just doesn't follow it.

If your read of the source or logs contradicts any line above, stop and report; do not build.

## Ruling 1 — fix inside m55, not a later unit
1080p is the production resolution. A v1 that measures zero required pairs at 1080p on a real host is not mergeable. One build, then requalify.

## Ruling 2 — the change (and nothing else)
a. **No terminal or timeout rule counts *issued* indices.** Phase closure and run closure become terminal when **≥ 8 colour completions (delivered or not) have been received for indices beyond the phase's last index / the closure watermark**, or the wall backstop expires. The out-of-order rule becomes **≥ 4 later completions** observed while the head is incomplete. Apply the principle at every such site and list each site in the report.
b. **Wall backstop 2 s → 5 s** for phase and run closure. It exists only for a stalled or finished pipeline; bytes stay bounded by the budget.
c. **Drop the 3-colour count cap.** The byte budget (64 MB compiled default, unchanged) is the only admission cap. Writer workers still never wait; refusal is still `budget_exceeded`.
d. **Unchanged:** refusal enum, numeric contract, identity contract (tokens, epochs, receipts, filter-then-freeze), sidecar schema, `observable`, no subtraction, no cook. **Allowed additive diagnostics:** per-item issue→colour-completion latency (frames, ms) in the sidecar row, and per-run p50/p95/max in the run summary.
e. If the source shows (a)–(c) cannot be done without touching the writer or the identity contract, stop and report rather than improvise.

## Ruling 3 — predictions first, one build, then gates
Amend the prediction doc **before the build**, dated: every accepted gate predicate that encoded the old clock (8 issued / 2 s / 3 colours) is re-worded, not re-worded after results.
1. **A-side reading (A699E9EE, not a gate):** one StackOBot **1080p** solid_swap natural leg — does the home fixture show late rejects at production resolution? Report latency and refusals either way.
2. **B-side (new build):** re-run every accepted gate whose predicate touches closure, timeout, budget, late results, finalization, teardown/run-end, short events or target-drop (you declare the set); both six-leg legacy comparisons vs the prior candidate (non-evidence output byte-identical / occupancy invariant); **new: StackOBot 1080p null + solid twins**.
3. **Can-fail, both must fire:** a genuinely missing completion still ends `closure_timeout` via the completion-count path; a stalled pipeline (no completions) ends via the 5 s backstop — no hang, run finalizes.
4. **Lyra G270 requalification — declared now:** A2's exact CLI with **300 frames** (150 gave 2 events because m52's restore latency caused 8 `not_restored` refusals). Fixture allowance 3, same L2/L1/foreground rules; the **first fixture-valid capture is the reading**, never retried. Pass = ≥ 3 events, **every labelled phase's required pairs measured** (none lost to `closure_timeout` or `budget_exceeded`), invariants 0; publish readings vs control, no threshold.
   - Fixture-valid, every phase measured, but < 3 events → **OBSERVED-BELOW-EVENT-FLOOR**; does not hold Stage 3.
   - 3 fixture-invalid → G270 **dropped for m55** as an UNRESOLVED observation (the owner's "skip Lyra if not suitable", 081-07); Stage 3 proceeds on StackOBot.
   - Fixture-valid and any required pair still refused by `closure_timeout` / `budget_exceeded` → **stop, NEEDS-DECISION**.

Report as **`081-15`**. **GREEN ⇒ Stage 3 released automatically** per 081-03 with all existing carry-forwards, plus: (i) cost legs also report **required-pair yield** (measured / required) at 720p and 1080p with reasons; (ii) latency distribution per leg; (iii) new high-water readings reported beside the priors 17.5 / 19.4 MB / 39.4 MB, priors not rewritten; (iv) field reference + client readme state the **resolution envelope** (64 MB budget vs one 1080p colour 8.3 MB; 1440p/4K will refuse more and are unexercised); (v) gotcha: *an issue count is not a completion clock*.

## Answers to your questions
1. Yes — diagnose (confirm the read above), correct per Ruling 2, requalify under the recipe declared in Ruling 3.4. Event floor and refusal semantics stay intact except the explicit below-floor observation branch above.
2. Disposition if Lyra can't supply it: as the three branches in 3.4. The 081-07 auto-drop is extended only to "3 fixture-invalid G270 attempts"; a valid unmeasured result is never dropped silently.

Pilot metrics: record this as a **chat spec defect caught by a gate** (the constants were chat's), not an implementation escape.
