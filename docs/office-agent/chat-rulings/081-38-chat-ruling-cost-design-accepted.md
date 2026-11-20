# 081-38 — Chat ruling on 081-37 GREEN: cost design accepted; writer rule stays literal; cost run released

Date: 2026-09-26 05:25 IST · Author: chat Claude (orchestrator) · Feature head `ae56e17` · predictions `docs/predictions/2026-09-26-m55-stage3-cost.md` (hash `fe52e4fa…`) · boundary `_reviews\081-37-evidence\cost-boundary.json`

## Ruling 1 — design accepted as declared
Everything in 081-37 is accepted as declared:
- The FPS 21.5 prior is classified PARTIAL: a start-of-run stall on the flush-per-line run log (G300). It is handled by running the counted legs with the run log off, a 60-frame warm-up exclusion, per-leg stall lists, the `HELD-FPS-PRIOR` rule, and the diagnostic pair `S3_DIAG_A/B`, which is outside the gate.
- The instrument choices, backed by evidence: the external cycle sampler for the game thread (G299), and PNG/s for the writer.
- The statistic: a one-sided 95 % bound from the pooled A/A and B/B spread.
- The validity rules, the load limits and the leg plan.
- Code's additions: the 5 % writer resolution guard, the FPS-prior hold, the discard leg running with the feature off, and person evidence voiding any cost leg.

## Ruling 2 — no writer tolerance; the literal rule stands
Code predicts a real 1–3 % writer drop at 1080p with pacing off, where the writer is saturated. **No tolerance will be added.** Adding one now, because a failure is predicted, would be moving the goalposts. The gate stays exactly as declared: an upper bound of the change below 0 → UNRESOLVED.
- UNRESOLVED is not FAIL. It is an honest reading, and it goes into the Stage 3 report as measured.
- Whether a saturated-writer 1080p drop is acceptable for M3 is decided afterwards, with the numbers in hand.
- The readme will then state the conditions: the drop applies to pacing-off saturation; paced capture is expected to keep up. A cost acceptance on a real trade-off goes to the owner as a product call if it is not clear-cut.

## Release
081-38 runs the cost window once: `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\081-37-cost-window.py 081-38`. Then it writes the Stage 3 cost section of the report, the journal and the docs.
