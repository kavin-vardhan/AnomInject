# 081-43 — Chat disposition of Codex's 081-42 review (CHANGES REQUIRED): all six P2 accepted, three P3 accepted; fixes + full bank replay before merge

Date: 2026-09-26 (IST) · Author: chat Claude (orchestrator) · Review `_reviews/081-42-codex-review.md` · Feature head `eafa654`

## Summary
Codex found **no C++ defect** in build 2/3. Its structural replay of 106 banked build-3 sessions (4,380 pair rows) found 0 violations.

It found six P2 problems. All six are in validation tools and client docs. **Chat accepts all six**, and owns the ones that trace to its own rulings:
- **F1** traces to 081-26: chat accepted `empty_region` as "honest" without requiring the oracle's disagreement check.
- **F2** traces to 081-35: chat chose visibility over gating for G297 singletons. That was too permissive where an independent invariant exists.
- **F6** traces to 081-41: chat wrote "no measurable cost … writer unaffected … workload is full" into the docs spec as a general statement.

No new game build is needed. The build-3 requalification and the cost results stand; F1 and F2 widen what the gates would have caught, and the bank replay below shows whether any real result changes.

## Rulings
**F1 (P2, accepted). A contradicted `empty_region` must fail.**
- Oracle: when the delivered mask proves target pixels exist (positive target and control denominators) but the row is refused as `empty_region`, count it as `empty_region.disagrees` and make it **exit 1** (a comparison mismatch), with its own summary count.
- G270, and any gate that accepts `empty_region`: reject a leg whose oracle reports any disagreement.
- Keep missing or all-zero mask PNGs **unverifiable**, distinct from a disagreement. Do not invent a disagreement just because no PNG was written.
- Negative: Codex's exact in-memory mutation of `M55B3R23_C3_LYRA_G270_N_A1` (si 21, window 3 → `empty_region`) must now FAIL.

**F2 (P2, accepted; this revises the 081-35 G297 ruling).**
1. **Per-leg invariants run before any noise excusal.** At minimum, each label row's serialized `target_pixels` must equal the decoded pixel count of that row's tag in that frame's mask. Code enumerates any other invariant that can be checked independently from the artifacts, adds it, and lists it.
2. **Candidate-only singletons now stop the run instead of passing.** A difference that appears on exactly one **build-3** leg and is excused only through that leg's own control pair, on a path that is **not** in the frozen historical run-unique set, **not** in the 081-22 timing-sampled set (`coverage_pct` + the six counters), and not resolved by an invariant, is **NEEDS-DECISION**. It never PASSes.

   Chat's earlier worry was false stops on noise. The bank shows that worry doesn't apply: every real singleton so far is on the run-unique or timing-sampled sets, or is M47-excused.
3. The frozen sets, the M47 and A/B/C excusals and the recurrence report stay as they are.
4. Negative: Codex's MAINON_S2 mutation (`labels/4/anomalies[0]/target_pixels` 20,949 → 20,950) must now **FAIL** on the invariant.

**F3 (P2, accepted). Oracle input classification.**
- **Exit 3** ("cannot interpret"), with line and row diagnostics, for any of:
  - malformed JSON lines;
  - non-object lines;
  - invalid required container types (e.g. `receipt = [1]` or `receipt = []` where an object is required).
- Catch decode and type errors at the input boundary. The tool must never exit through a traceback.
- **Precedence:** exit 1 if any readable row mismatches (a proven defect outranks incompleteness); otherwise exit 3 if anything was uninterpretable; otherwise 0.
- The summary always reports both counts. A well-formed session with no comparisons stays 0.
- Tests:
  - malformed-only → 3;
  - valid + malformed → 3;
  - mismatching + malformed → 1;
  - `receipt = [1]` → 3;
  - `receipt = []` → 3.

**Oracle P3 scope note (accepted, small).** Add cheap contradictory-field checks, each counted as a mismatch (exit 1):
- a measured row (`chg_measured` true) must not carry a refusal reason;
- a measured row must have `chg_eligible` and `pair_valid` true;
- a measured row must have its histogram and sum fields present.

Also document plainly that the oracle checks arithmetic and transport only. It checks neither renderer pairing, nor phase-reference selection, nor the full schema.

**F4 (P2, accepted).** Client readme: the control **excludes tagged target pixels but can include light, shadow and reflection spill onto the surroundings**. Match the field reference, and keep the warning against automatic subtraction or causal inference.

**F5 (P2, accepted).** Refusal rows: the target and reference measurement statistics are unavailable (sentinels). Valid **control** statistics, the reference index, camera/identity bookkeeping and running phase maxima **can still be present**, as in the `M55B3R23_C2_EMPTY_N_A1` si 3 counterexample. Audit similar prose against the field tables.

**F6 (P2, accepted).** Rewrite the cost and yield text as a **dated result on the tested fixture and machine**:
- **Fixture:** StackOBot static fixture, `solid_swap`, config `2 4 16 4 0`, 600-frame legs, steady window 60–599.
- **Pacing:** configured at 30 fps; the observed capture arm rate was about 25.07/s. Say so.
- **Statistic:** the one-sided 95 % upper bound on the **mean game-thread CPU-cycle-equivalent Δ per engine frame**. Values: −0.0718 / +0.1100 ms. Cycle-equivalent means blocked or waiting time is excluded.
- **Workload:** each ON leg measured 120/120 required pairs, with 0 drops.
- **Writer:** "no detected loss of throughput at this load".
- **Keep:** the pacing-off stress results, and the statement that there is no structural guarantee of full yield or of memory beyond the 256 MiB admission cap. The observed peaks are fixture-specific; another build-3 Lyra bank reached 134.8 MB.

**N1 (P3, accepted).** Split the delay=3 sentence:
- the adjacent windows 0–2 stayed under 0.1 %;
- window 3 reached all target pixels in both comparisons;
- the reference values had already accumulated ordinary scene change (window 2 ≈ 1.2 %).

**N2 (P3, accepted).** `change_teardown_flush` is emitted at zero. Remove it from the "absent when zero" list.

## Replay requirements (the evidence chat reviews before authorising the merge)
1. **Oracle across the whole m55 bank.** Every banked m55 session, run under the old and new oracle. Report every exit-code change, with its reason. A real session flipping 0 → 1 is a stop for chat; 0 → 3 is reported with its diagnostics.
2. **Every legacy comparison and every G270 evaluation**, re-run under the new lib from the raw bank. Report verdict changes. A real PASS → FAIL or NEEDS-DECISION is a stop for chat, with the evidence.
3. **All negatives both ways:**
   - Codex's F1, F2 and F3 reproductions now fail as ruled;
   - the unmutated banks still read as before;
   - each existing AMENDMENT 1–6 proof still behaves as it did.
4. **Client-facing identifier grep** over the changed doc text: 0 hits.

## Then (separate brief, after chat reviews the replay)
Strip comments (65 lines, Codex-authored), then:
1. review the diff (comments only);
2. build Game and Editor;
3. check the staged-exe provenance (A44);
4. merge no-ff onto master, **no tag**;
5. write the handoff.

`AnomalyBench` removal stays a client-packaging step (PRE-DELIVERY-CHECKLIST §1). Then **m53 = UV/normal texture corruption** (owner's order, 2026-09-26).
