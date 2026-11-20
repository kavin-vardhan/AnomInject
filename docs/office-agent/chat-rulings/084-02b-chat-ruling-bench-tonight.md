# 084-02b — Chat ruling: bench tonight on FD621B27; remaining Codex partials fixed tomorrow by day; the edge-route scope; harness gaps corrected and the bank re-evaluated

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `fix/m52-label-timing` `90dfa6f` · Fix exe `FD621B27` · Mini-delta `_reviews\084-02b-codex-m52-minidelta.md`

## Accepted
- **The corrections:** F1–F7, Shipping (source), resource replacement, and the Apply-time purity scope with the mid-hold monitor.
- **Unit tests:** 93/93, each correction failing on the 084-02 logic.
- **The two-sided gate:** all four can-fail cases FAIL on real banked pixels, and Codex's late-offset counterexample is reproduced on real data.
- **Build:** 0 plugin warnings, archived and pinned.

## Decision 1: 084-03 runs TONIGHT on `FD621B27`, when the owner releases the PC
- Codex now finds only **partials on edge paths.** Its RESOLVED list covers the core; nothing it found invalidates the ordinary hold/revert path the owner saw at the office.
- **Tonight's bench is the evidence the owner's rule demands:** real pixels, both edges, baseline vs fix, with the residual measured.
- **The raw sessions are banked.** Every gate verdict is re-evaluated from the bank once the evaluator gaps (decision 3) are corrected. **Tonight's verdicts are provisional.**
- **Tonight's run must also report:**
  - how often each edge path actually happens: trail reopens, ORDER HOLD, gap-unknowns, carried trails, contaminated events, the unresolved timeout;
  - **the mid-hold monitor's per-tick game-thread cost.**

## Decision 2: the scope of the edge routes
- **Engine changes are out** (the plugin must stay game-agnostic).
- **Delivery limitations, documented in the client readme:**
  - a texture parameter changed on a runtime dynamic material instance mid-hold;
  - a material set on an unregistered component that later registers, unless the existing per-tick diff catches it cheaply at registration.
  - **Tomorrow's pass:** say whether the diff covers the second route at acceptable cost. If yes, include it.
- **Fix tomorrow by day** (084-04, source, with unit tests both ways):
  - **F1:** queued-but-not-started stream-outs are fenced (query the streamer's pending requests for held textures, not only the in-flight work);
  - **F2:** a reopen restores mask and m55 continuity for the reopened frames, and the reservation and refusal are released correctly;
  - **F5:** inherited frames get masks at the next run's start; a non-render-truth run in between keeps the carried ownership;
  - **F6:** the 64-tick fallback defers to the final label, so it never disagrees with a late label.
- **Priority:** anything the bench shows actually happening comes first.

## Decision 3: the harness gaps are corrected before verdicts are final (not before the bench)
- **Gap censoring:** a missing frame or engine-frame jump adjacent to an edge makes that edge **censored**, never passed.
- **Repeated visible runs:** a later visible run is not "isolated". Any visible frame outside the labelled window fails, unless it is proven to be a different cause (the null run shows it too).
- **Missing masks FAIL** on frames in the labelled window.
- **Contamination:** a contaminated event fails gate (iii) unless the contamination was flagged and reverted as ruled. Report every one.
- Re-evaluate tonight's bank with the corrected evaluator, both ways.

## Stopping the review loop (a delivery-week rule)
- After 084-04, Codex does **one** combined check: the source delta plus tonight's evidence.
- Findings on paths that the bench shows **never occur**, and that are not reachable in ordinary capture, become **documented limitations**, not blockers.
- Anything that occurs in the data, or that breaks sync on the ordinary path, blocks.

## Sequence
1. **084-03 (high, fresh session):** tonight on the owner's go.
2. **084-04 (xhigh), tomorrow by day:** the decision-2 fixes, the decision-3 evaluator corrections, and the bank re-evaluation.
3. **Rebuild,** then a short confirmation bench (Tue night), plus the combined Codex check.
4. **Merge to `master`.** Then the audit of all anomalies, and m53.
