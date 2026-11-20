# 088-01 — Chat ruling on the delivery rehearsal (NEEDS-DECISION): accepted; the launch check runs tonight as part 0; §8.8's rate comes from tonight's ND-5; README Step 1 needs the owner

Date: 2026-09-29 ~18:25 IST · Author: chat Claude · Fix branch `5c2bc83` (kit merged `df63a8a`) · m53 `c2d11f5` · Packages A exe `6D1B659F` / B exe `C18D0F41` in `E:\IA_BuildCache\_rehearsal\`

## Accepted
- **Both packages build like a client's,** with 0 cook errors and 0 warnings.
- **Every non-launch packaging check passes on both:**
  - the descriptor is clean without `--bench-cook`;
  - AnomalyBench is absent;
  - the lever audit passes on the staged folder;
  - the cooked ini holds a 64-character token and delivery mode;
  - **the G376 bundle token PASSes on the real builds and STOPs on the dev ini and on the other path's build;**
  - Setup.bat was walked;
  - Steps 5–7 ran.
- **A vs B:** only m53 differs.
- **Three packaging faults found and fixed** (G398–G400). No product faults.

## Rulings
1. **The launch check runs TONIGHT, as part 0 of the night bench,** from `_rehearsal\<A|B>\game` with no staging:
   - the token;
   - `DISABLED` levers;
   - no twins;
   - the pool line;
   - MaskProbe;
   - B's census.

   Thursday's office launch repeats it on the office build.
2. **§8.8's per-mode rate comes from tonight's 085-04 ND-5 reading.** If MainWorld has no eligible host, §8.8 states the fixture reading, "not measured on realistic content", and points to the office census. A docs brief fills it in on Wednesday.
3. **README Step 1 (the launch steps for the client's build) needs the owner.** Chat asks him for the steps, or for approval to reuse the last delivery's wording. The bundle checker keeps STOPping until it is filled.

## Next
- **The night bench is released now** (the owner's standing permission: tests whenever the PC is idle). Its own idle gate is ≥ 10 min before the first launch and ≥ 5 min between parts.
- **Wednesday:**
  - the bench ruling;
  - the yield decision (office counts);
  - §8.8 and Step 1 fill-ins;
  - the Codex evidence review;
  - merges;
  - the final delivery build.
