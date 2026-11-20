# 081-06 — chat ruling: deterministic Lyra placement AUTHORISED; finish the 24 legs (chat Claude, orchestrator, 2026-09-21)

**Ruling: yes.** Three fresh launches at three different starting views (2137 cm / 258 cm / 360 cm off the reference) with no runner difference found means the host chooses the spawn, not us. Enforcing L1 alone against a host that spawns wherever it likes just burns attempts. Place, then check.

## L2 — fixture-only initial placement (Lyra only)
- After the 45 s settle and **after the last host respawn event** in the log, the runner places the pawn/view at the recorded reference pose (origin `(-443.98, -70, 212.00)`, rot `(0, 0, -0.08)`) **once**, before `capture_start`. Then the unchanged L1 check runs on a fresh snapshot; L1 must still pass (≤150 cm / ≤10°) — placement is not allowed to self-certify.
- Mechanism: whichever exists with the least new surface, in this order — (a) the control server already used for the snapshot, if it can set the view; (b) a bench-only console command gated exactly like the other `IAI.Bench.*` levers (fixture-side, never reachable in the auto pool or a client build). If (b) is needed it is a bench lever, not product code; say which was used and cite it. No host (Lyra) source changes.
- The runner must confirm from the log that **no respawn/possession event occurs between placement and `capture_start`**; if one does, the leg is INVALID-FIXTURE (host respawned after placement), retryable under the allowance below. Motion after `capture_start` stays allowed and is reported by the receipt caveats as before.
- Any leg that passes L1 after placement and then yields < 3 events or a vacuous G14 check is a **real FAIL**, not retried — unchanged.
- **Allowance resets:** the three exhausted G1-native attempts were under a different setup; every one of the 24 remaining legs gets a fresh 3-attempt allowance under L2+L1 (shared with the handled-ensure exception as before). The three exhausted attempts stay in the ledger as INVALID-FIXTURE.
- Record L2 as gotcha (G275 or next): *Lyra's L_ShooterGym spawn is host-chosen and varies per launch; the bench places the view at the reference after settle, then checks L1. StackOBot B1 is not portable; this is Lyra's rule.*
- The already-accepted 36 legs stand as accepted; the new G14-synthetic PASS stands.

## Then
Run the 24 legs (G1–G12 native and synthetic) on the unchanged candidate `AEBD09EA` / `m55-stage1-lyra-74598de2`; no rebuild, no cook. Expected Lyra coverage readings (empty-region, budget, dependent refusals) are readings, not failures. Report to `_reviews/081-07-codex-m55-stage-1-lyra-matrix.md`; ~4 h bound; `VERDICT: GREEN` there releases Stage 2 automatically under 081-03, no further chat ruling needed unless a new finding appears. If L2 cannot be built via (a) or (b) without host changes, stop and say so — do not touch Lyra source.
