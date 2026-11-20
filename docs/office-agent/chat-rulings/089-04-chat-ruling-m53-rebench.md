# 089-04 — Chat ruling on the m53 re-bench (RED, derived: the Code session died on a network error after the harness completed): fixture sync and the arms are proven again; MainWorld uv is unjudgeable because both raised-cap hosts are Nanite (no masks); the G-MODE evaluator has a family mix-up

Date: 2026-09-30 ~08:30 IST · Author: chat Claude · m53 `bf0ac20` + premise lock `a7f542d` (local; the session died before pushing) · Pairing DC2 · Harness exit 0, 47/47 accepted, postflight PASS, restore on attempt 1, label-rule all PASS

## Read from `_reviews\089-04-evidence\run\report.md` (the harness completed; only the Code session's final write-up was lost to `ENOTFOUND`)
- **B-M53 fixture: PASS again, all 4 modes:** 17/17 onsets and offsets at +0/+0; the kit reads PASS-WITH-CENSORED 14/14.
- **The arms:** CD3 +1, CD5 +3 and RD3 +1 are **proven**, and NOAPPLY is proven (holes). **RD5 reads end −4 (not +3): NOT proven,** cause unknown.
- **G-BIND PASS** (per-frame, uniform half 8 < 16), **G-CENSUS PASS** (host and fixture cold reads), **G-LEVER-OFF PASS.**
- **MainWorld uv (SM_Ramp2, RoomBuilderSquare under the declared 256 MiB bench raise):** every event is NOT-MEASURABLE, with "mask rows missing 96". **Both hosts are the Nanite targets 089-01 identified** (UNMEASURABLE TARGET reason=nanite).
  - The aligned null shows **no wrong-object change** (GT-NULL wrong_object PASS on all 5 legs).
  - The timing can't be judged from pixels, because there is no mask.
- **P-ORC:** uv16 / uv64 **PASS** (16/16, 64/64, separation 56, interior 0, 1:1 texel scale). The normal oracle still fails on interior spread and pattern.
- **G-MODE rows are inverted:** uv reads INSUFFICIENT `porc_failed` although uv's P-ORC passed, while normal reads PASS although normal's P-ORC failed. **This is an evaluator family mix-up** (the overall `porc.pass_` is used instead of the per-family one), and it makes G-MODE unreliable as reported.

## Rulings
1. **m53 label sync: PROVEN on the fixture** (two independent runs, all 4 modes, 3 conditions in 085-04 plus the regression now), with **0 co-entry frames** throughout.
   - **Real content: unjudgeable from pixels this week.** The only framable eligible hosts are Nanite, so there is no mask.
   - The mechanism is the same material swap, in the fire tick, as `corrupted_texture`, which is SYNC-PROVEN on real content. The readme states the evidence basis plainly.
   - **The owner's Thursday office look is the real-content check.**
2. **G-MODE: fix the family mix-up** (each family's rows gated on that family's P-ORC) and **re-evaluate offline from the banked legs.** No re-run; declared, and the as-built report kept.
   - Expected: uv tile and scramble judged against their passing oracle; normal INSUFFICIENT (its P-ORC failed).
   - Normal mode-correctness then rests on the offline oracle proofs (39/39) and S1's identity work. That is documented.
3. **RD5 end −4:** investigate offline, from the per-frame labels and the revert log. It is not blocking, because RD3 proves the restore-side can-fail.
4. **The session died on `ENOTFOUND`** (this PC's internet or DNS dropped around 07:40; the bridge dropped at the same time). This is not a harness fault.

## Next
- **089-04b (xhigh; offline):**
  - finish the write-up;
  - push the lock commit;
  - the G-MODE family fix + re-evaluation;
  - RD5;
  - confirm the MW hosts are Nanite;
  - the normal P-ORC cause (one paragraph);
  - the journal.
- **Then 090-01:** the final fix-branch source round (the `TransitionOffFrames` default 16, the LOD log wording, and the yield option once the office counts arrive), merged into m53, both builds.
