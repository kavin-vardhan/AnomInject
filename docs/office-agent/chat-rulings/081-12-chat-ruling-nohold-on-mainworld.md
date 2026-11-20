# 081-12 — chat ruling: moving twins + legacy ACCEPTED; NoHold runs on MainWorld with the m52 recipe, no lock/placement (chat Claude, orchestrator, 2026-09-23)

**Accepted:** natural moving twins (null onset 0.028–0.044, solid 0.999–1.0, motion 20/20 pairs, control rises with motion and is reported, never subtracted) and both six-leg legacy comparisons (MainWorld byte-identical; CB under the redefined invariant; 30/30 pairwise). The PowerShell idle-check helper fix is fine. NoHold_N_A1 stays FAILED-PRECONDITION (recipe).

## NoHold — third route, no build change
Neither offered route is needed. This round's own evidence settles it: **MainWorld ran 6 legs on the candidate, all first-attempt valid, with no input lock and no placement** — MainWorld does not have CB's focus-driven pose problem (its pawn/camera is not free-look at the reference; the m52 legs ran there for two days the same way). So:

- Run NoHold on **MainWorld** with the **exact banked m52 G11 recipe** (`M52E_M52E_G3_HOLD_NAT` seed/config/CLI from `anomaly_log.txt`, target `SM_rock_02` actor `…2044254803`, `T_grunge_mask` ratio 20.48) plus `IAI.Bench.StuckMipNoHold 1`, both tick orders. No lock, no placement — the map gate stays as it is.
- Pose precondition for MainWorld (declare before launch): the run's camera rows equal the banked m52 G11 session's camera rows for the same frame indices within 1 cm / 0.1°, exactly the "full camera rows compared across runs" check the legacy legs used this round. A launch that fails that is INVALID-FIXTURE (retryable within the allowance); foreground rule unchanged.
- Gate unchanged from 081-11: ≥ 3 `stuck_low_mip` fires with `held:false`; ≥ 3 sidecar final events with `phase_count 0`, top-level `reason no_labelled_frames`, zero pairs, zero labelled legacy rows for those events; veto accounting reported if the m26 veto removes legacy records.
- Allowances: natural 2, synthetic 3, as held.

Then R5 Lyra per 081-07 (`081-13`). `VERDICT: GREEN` on NoHold + R5 resolved ⇒ Stage 3 auto-released; the Stage 3 brief is already specified in `081-03` §Stage 3 with the carry-forwards (FPS 21.5 prior, high-water priors, delay=3 paragraph for the client readme, UNEXERCISED block verbatim).

## For the record
This chat session is closing after this ruling; a fresh chat picks up from `docs/CHAT-HANDOFF-081-m55-stage-2.md` (written today). Codex's reports keep the same `_reviews/081-NN` numbering; nothing about the protocol changes.
