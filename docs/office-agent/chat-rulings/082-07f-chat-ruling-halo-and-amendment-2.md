# 082-07f — Chat ruling: the fixture fix is accepted, the probe's (b) and (c) are accepted as a screen-space halo, and AMENDMENT 2 is scoped

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `1395377` · CaptureBench `2c5ec56` · Staged exe `2FCDF059`, utoc `20DA6F98`, ucas `534C5863`

## Accepted
- **Cause, named from the engine source and a decisive read.** `M_TC_Masked` was saved with a stale `bCanMaskedBeAssumedOpaque = true`, so its effective blend was Opaque.
  - As loaded from disk it reads `BLEND_OPAQUE`; after an in-memory recompile it reads `BLEND_MASKED`. It was the only material whose effective blend differed from its authored one.
  - Good diagnosis.
- **Audit.** 42 fault rows plus 46 identity rows, each checked from disk read-back and against the offline minimum.
  - The audit is proven to detect the defect: on the pre-fix read-back it flags exactly MASKED's two rows, and it reads 0 afterwards.
  - The tool's verify now requires effective blend = authored blend, and the negative control fails exactly on MASKED.
- **G-COOK PASS.** The manifests change exactly `M_TC_Masked.uasset`.
  - **Keep the new build staged.** exe `2FCDF059`, utoc `20DA6F98`, ucas `534C5863` are the build under test for the rest of S1. The rollback is archived.
- **Probe (a) PASS.** The masked tile clips on all 100 frames, and its mask drops from 4,096 to 2,048 px.

## NEEDS-DECISION rulings
1. **Probe (b) and (c): ACCEPTED as a screen-space halo around the only changed tile. Not a re-cook side effect.** The evidence:
   - the far field reads ≤ 1;
   - the difference is spatially structured around the one re-authored tile, and identical in all 100 frames;
   - the manifests show exactly one package changed;
   - on the old build, with no cook involved, every 082-07e wrong copy against its null leaks 8–17 levels within 4 px of its tile.

   **Chat spec defect:** the (b) clause as I wrote it demanded ≤ 2 right up to the edge of a tile whose appearance was deliberately changed. That ignores screen-space spread (bloom, AO, TAA). I own it.
   - The mechanism is **not established**. Its cause read goes to S2, alongside the start-up element. Make no probe for it now.
2. **Build staged: CONFIRMED** (see above).
3. **G-BIND (3)'s outside clause: split it in AMENDMENT 2.** Today it is `outside_max ≤ 2` over everything outside the event region.
   - **Ring:** pixels outside the event region but within **R px** of it read max |d| **< 32**. Their halo is published as a reading.
     - R is set **from the bank before any G-BIND leg runs**: the maximum halo extent measured on the 082-07e wrong-copy vs null pairs and the 082-07f probe (currently 48 px). It is not tuned on G-BIND legs.
     - 32 is the fixture's offline wrong-copy floor. The measured halo peaks at 17.
   - **Far field:** pixels beyond R read max |d| **≤ 2**, unchanged.
   - **Per-tile leak reading:** for every other fixture tile, report the count of pixels > 2 and its max.
   - **Proven both ways:**
     - a synthetic leak must FAIL: the tile-probe pattern pasted onto a neighbouring tile, once inside the ring and once beyond it;
     - every banked halo pair must PASS.
   - **No-flip:** a check over every banked comparison where the clause applies.
4. **Apply the same review to every row not yet run.** Scan them all for a whole-frame or outside-target clause that the halo would trip, and give each one the same ring split with the same proofs. Rows whose clause compares applied vs null *after revert* (G3) are unaffected, because the scene is identical there. List every row considered.

## AMENDMENT 2 also carries
- **The build under test** is the one above.
  - Rows already judged on `30FE0FDE` stand as within-build results (the 082-07c precedent).
  - **MASKED re-runs whole** (ID, NULL, WC). Its 082-07e attempts are superseded, with their history kept, as in 082-07d.
- **MASKED PRE row, checked in the NULL_MASKED leg before the wrong copy is judged:**
  - the target mask count equals the probe-measured clipped count (2,048) on every measured frame;
  - every alpha < 0.5 cell reads within the measured sky range.

  Proven both ways: the 082-07e null (unclipped) FAILS and the 082-07f probe PASSES. A PRE failure stops the run as FIXTURE-CANNOT-EXERCISE; it is not a feature FAIL.
- **Evidence cited:**
  - the MASKED wrong-copy offline minimum (138 / 140, floor 32);
  - the new G-BIND (3) offline proof;
  - `082-07f-evidence\audit.json` as the fixture's admission evidence.
- **The locked `082-07-common.py`:** only its staged-hash constants change, and the diff shows nothing else.
- **Findings:**
  - the halo, whose mechanism is open; its cause read goes to S2;
  - **for the S4 client docs:** masks do not include the few-px screen-space halo that any visible anomaly casts on its surroundings.

## Sequencing
- **082-07g (xhigh, fresh session, bench-free):** AMENDMENT 2, the harness changes and their proofs, the supersede record, the boundary re-issue, a dry run, and the 082-07h invocation.
- **082-07h (high):** resume the legs from MASKED.
