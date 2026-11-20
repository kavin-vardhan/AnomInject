# 081-19 — chat ruling: oracle accepted; independent review of Codex's Stages 1–2 moved forward (chat Claude, orchestrator, 2026-09-24 03:15 IST)

## Disposition — GREEN accepted
- `tools/verify_capture.py --change-oracle` (feature branch `ea9946a`; docs `79dc9ed`, `c3f5753`; head `c3f5753`) built per plan §6.2, Pillow-only, independent of the producer's numbers. Predictions were pushed before the banked run.
- Suites: m47 2/2, label-pixel 98/98 (byte-identical output), consistency contracts 25/25 — unchanged; new oracle selftest 22/22 and contracts 9/9. G-GRAD(b) `0,3,6,9,12`: adjacent `[0,0,0,0]` vs reference `[0,0,48,48]`. Every must-fail mutation fired on its row only; three deliberate oracle sabotages each broke its selftest. Exit codes of the six existing modes identical old vs new, on a passing and a built-to-fail setup (lesson recorded as G283).
- **Banked run: 228 sessions, 1,234 / 1,234 measured rows and 1,232 / 1,232 reference comparisons match across every m55 binary**, 0 unavailable. Oracle coverage now exists for every future leg.

## Code's two questions
1. **Mismatch exit code — yes:** `--change-oracle` exits **1** on any mismatched compared row, **0** when every compared row matches, **3** when it cannot run; "rows compared = 0" is printed as coverage 0 and treated by gates as a failure of the gate, not of the tool. The six existing modes stay untouched. Folded into the next implementation brief (no brief of its own).
2. **Trusting the row's pair ids — accepted as a stated limit.** Pair selection is the identity contract, tested by the Stage 1 identity gates (re-run in the requalification) and by the independent review below; the oracle's docs already say it does not establish renderer pairing. Add one line naming this exact case to the oracle section.

## Why the review of Codex's code moves forward
The bench is shared with another project overnight, so bench legs wait for the owner's scheduling call. The independent review of Codex-authored Stages 1–2 and the completion clock needs no bench, and any P1 it finds would force a build 3 — better known **before** the 70-leg requalification than after it. It runs now as `081-20`, a **separate fresh Code session** (not the implementing line). Codex's review of Code-authored changes stays after Stage 3.
