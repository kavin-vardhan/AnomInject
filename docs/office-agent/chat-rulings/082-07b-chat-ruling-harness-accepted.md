# 082-07b — Chat ruling on 082-07a (GREEN): S1 leg harness accepted, the six pre-leg decisions confirmed, and legs released once the owner confirms the PC is free

Date: 2026-09-27 (IST) · Author: chat Claude · Boundary `_reviews/082-07-evidence/boundary.json` (`d936c032…`), head `7dc666e`, predictions `87930fdd…`

All six pre-leg decisions (predictions §9) are confirmed as written:

1. **Forced GC via the engine purge timer is accepted.** A row where no GC pass lands inside the event reads UNEXERCISED, not PASS. It is carried to S2 with a dedicated GC lever.
2. **Level change and re-created component → swept stay UNEXERCISED in S1.** Both are carried to S2, which will add a lever that re-creates a component and a level-change path on the fixture.
3. **No camera-rotation guard on `CB_TexCorruptLevel` is accepted** (G313), with the settle-at-(0,0,0) rule and a separate 6-attempt pose-only budget. Report the discard rate. If the budget is exhausted on a qualification row, that is a stop.
4. **The G-ID region (union of both masks) and the per-frame wrong-copy ≥ 16 rule are accepted** as stricter than the plan.
5. **Chain (d) rows running through both ids are accepted.**
6. **The MainWorld G-ID row is judged only if its own null pair agrees within 2.** INVALID-ARBITER there is reported, not a stop.

The legs run for about 2 h. They are released with `082-07-window.py 082-07b 540` once the owner confirms the PC is free; daytime use of the PC is his.
