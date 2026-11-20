# 081-07 — chat ruling: Stage 1 CLOSED on 36/60 by owner decision; Lyra scoped down; Stage 2 RELEASED (chat Claude, orchestrator, 2026-09-21)

**Owner decision (the owner, 2026-09-21): skip Lyra where it is not suitable.** Applied as follows.

## Stage 1 — closed
- Stage 1 is **ACCEPTED** on the 36 accepted legs: StackOBot G1–G15 both orders (30/30) and Lyra G13–G15 both orders (6/6). The identity/lifetime code is host-agnostic and every failure path was forced on StackOBot; the 24 remaining Lyra legs were the same failure paths on a second host, i.e. regression insurance, not new coverage.
- The 24 Lyra G1–G12 legs are recorded as **DROPPED-BY-OWNER-DECISION** (not UNEXERCISED, not FAIL) in the ledger, plan, journal and the Stage 3 report's limits block, with the reason: the host chooses the spawn per launch and the accepted candidate has no placement lever. The three L1-invalid attempts and the original failed-precondition-undefined leg stay preserved.
- Codex's preflight was correct: the no-rebuild rule and the lever requirement conflicted, and it was right not to weaken either silently. That rule was a Stage-1 artefact (the candidate was already accepted). It is lifted by the Stage 2 rebuild below.

## Lyra going forward — one role only
- Stage 2 rebuilds the binary anyway. Add the **L2 placement lever** in that build as a bench-only lever in its own fixture-side module if feasible (per Codex's preference; otherwise gated exactly like `IAI.Bench.*`), with the properties Codex listed: preserves host camera behaviour, distinguishes camera origin from pawn origin, logs placement, independent L1 afterwards, validates the placement→`capture_start` log interval incl. buffered respawn/possession. Never in the auto pool or a client build; no Lyra source changes.
- Lyra's Stage 2 legs are reduced to exactly: the `null_effect` / `solid_swap` twins (one leg each, natural order only) and the G-3 observation on the G270 recipe. Nothing else runs on Lyra in Stage 2 or Stage 3 (cost A/B/B/A is StackOBot only, as already specified).
- **If L2+L1 cannot produce a valid leg within the 3-attempt allowance on the first Lyra leg attempted, Lyra is dropped entirely for m55** — record it, do not ask again; m55 v1 ships as StackOBot-validated with Lyra listed as an unqualified fixture. That is the owner's "skip if not suitable" applied.

## Stage 2 — released now
Proceed per `081-03` §Stage 2 with the 081-04/081-05 amendments (closure watermark, 14 reasons, sidecar contract, filter-then-freeze, budget rules) and this ruling. Bench twins, measurement fields, phases/onset/window, final records, all finalisation events incl. teardown (the Stage 1 UNEXERCISED teardown line must be exercised here), hide/blink/short-event gates, `StuckMipNoHold` ⇒ `no_labelled_frames`, G-3 on Lyra as scoped above. Predecessor for invariance = `AEBD09EA` (Stage 1 candidate), verified at its archive first. StackOBot remains the qualification fixture for every gate that has both-orders coverage.

Report to `_reviews/081-08-codex-m55-stage-2-report.md`; ~4 h bound per stage; stop and report if exceeded. GREEN there ⇒ Stage 3 released automatically under 081-03.
