# 081-24 — chat ruling: long run PLATEAU, evidence ON has no measurable writer cost → no build 4; windows c1–c3 on build 3 (chat Claude, orchestrator, 2026-09-25 11:40 IST)

## Disposition — GREEN accepted
- Six 1800-frame legs valid on build 3 (Lyra ABBA ON/OFF/OFF/ON, StackOBot ON/OFF), 1800/1800 PNGs and zero drops on every leg; oracle exit 0 on every ON session (179/179, 168/168, 220/220).
- **Plateau on both hosts** by the predeclared rules: writer latency flat per decile on every ON leg; stage high-water reached in the first half and never exceeded (Lyra 64.3 / 134.8 MB, StackOBot 66.4 MB); zero `budget_exceeded`, zero `closure_timeout`. L4's two late colours were refused as `out_of_order_timeout` outside measured windows — design behaviour, no required pair lost.
- **Evidence ON vs OFF: no measurable effect** on sustained fps, PNG rate, writer latency, CPU or drops (Lyra ABBA ranges overlap; the only "distinguishable" metric is fps by 0.001 with ON the faster side). Stage cost ≈ 0.12 logical cores.
- The build-2 writer deficit (081-17R) did not reproduce; cause not established (081-17R ran right after a three-target build, with no process sampler). L2's late-decile growth (evidence OFF) coincides with outside load on the PC. Recorded as readings.
- **No build 4.** Code's "High-priority stage worker" option stays unadopted — nothing to fix.

## Harness escape (Code-authored, 081-23)
The c0 analyzer crashed on OFF legs (its self-test covered ON legs only) and `chunk0.py` discarded the analyzer's exit code, so the window reported exit 0. Code found it, re-ran the analysis without touching any hash-locked file, and recorded G288; c1–c3 use `run_sequence`, which propagates failures. **Scorecard: one Code harness escape (dry-run coverage gap), caught by Code itself during use.** The held docs patch (branch head must stay `a745ee5` for the window preflights) is accepted; apply it after c3.

## Next — 081-25 (Code, high): windows c1 → c2 → c3 in one run, stop at the first failing window, then the docs.
