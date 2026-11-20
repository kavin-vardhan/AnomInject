# Night bench 29→30 Sep (084-09 + 085-04) — chat ruling (NEEDS-DECISION): four anomalies proven by all three tools; m53 in sync on the fixture in all modes; stuck_low_mip, lod_popping, MASK55 and m53 MainWorld need an offline adjudication, fixes and a targeted re-bench

Date: 2026-09-29 ~23:35 IST · Author: chat Claude · Fix build `8A6074AA` + `BF06AE61`; m53 build `6B671987` + `F76F1119` · Banks in `D:\IA_BankOverflow\` (the `IAI_BANK_OVERRIDE` path change, declared: accepted)

## Accepted
- **Part 0 (both rehearsal packages):** token, levers DISABLED, no twins, pool line, probe, census counts only: **all PASS.**
  - The firewall prompts the Part 0 launches raised were closed as Cancel, with no rule created (G401). That was acceptable because they were Code's own launches. Next time, part 0 runs **after** the bench legs, not before.
- **SYNC-PROVEN by all three tools (E1, 086-01, the office kit):**
  - missing_object and corrupted_texture, both tick orders;
  - blinking, native order;
  - missing_texture, **with MT85 decided:** 12/12 in both orders with clean references, so the old +1 was a reference confound.
- **camera_clipping:**
  - the designed edges have **label flip = pixel flip, 0 off**;
  - **MainWorld: over-label fixed** (0 labelled-confirmed frames at the floor, where 084-06 had 200/200);
  - trace cost 44 µs mean.

  **SYNC-PROVEN with AA on (the delivered default).** AA off stays UNJUDGEABLE (the null reads zero); it is a reading owed, not a blocker. The 20 unconfirmed frames are flagged by design, and the readme's "drop unconfirmed frames" recipe covers them.
- **m53 on the fixture: 12/12 PASS** (4 modes × native / AA-off / synthetic), 17/17 onsets and offsets at +0/+0, 0 not-measurable.
  - The must-fail arms FAIL.
  - **0 co-entry frames everywhere** (m52 ⟂ m53 holds in the frames).
  - G-SEED, G-ID and G-SYNC-REG PASS.
- **Part 1b yield (m52, MainWorld, home):** about 5–6 holds in about 22 attempts per capture, all on one rock. The refusals are dominated by `shared_world` (77–93 per capture), then not-streamable (40–64), then too-small (3–27). `baseline_pending` = 0.
- **085-04's two evaluator fixes: ACCEPTED.**
  - Fix 1 is a crash fix.
  - Fix 2 **aligns the count with the ND-5 definition declared before the run** ("not a warm-up"). That is not a loosening: it enforces what was declared. The as-built reports are kept in `_reviews\night-0930\`.
- **Good m53 news:** the census shows **26 uv-eligible and 9 normal-eligible objects in MainWorld's whole world at the delivered 128 MiB.** The bench camera's view had none. Yield on a moving camera is not zero.

## Rulings on the NEEDS-DECISION items
1. **The evaluator fixes:** accepted (above).
2. **G403 (the ND-4 host parse bug): FIX, then re-run m53 MainWorld + COST.** Prefer hosts **eligible at 128 MiB** from the whole-world census, reached by placement (the bench L2 lever), over the 256 MiB raise. Use the declared raise only if no 128 MiB host can be framed. This gives uv **and** normal real-content legs.
3. **The black LOD fixture: FIX** (lighting/sky so frames render; a content change with a manifest diff; re-cook), then run the LOD premise + gate legs. **Also explain `worst=2` on a 2-LOD mesh from source:** is the forced LOD out of range, and does the label depend on it?
4. **Instrument disagreements: an offline per-event adjudication, per the 087-01 rule.** A FAIL is an artefact only if shown on that event.
   - **blinking synthetic** (E1 FAIL 10/10; 086-01 and the kit 0): test for G395 on each event.
   - **m52 B9** (086-01 FAILs 5; E1 PASS; kit 0): is 086-01 blind to the `partial` flag? Show it per event.
   - **m52 B3 si 181, visible 41 frames after the label ended: THIS ONE MATTERS MOST.** Show what is on screen in frames 199–239 (other events, the per-texture render record, residency). Is it the blur still resident (a real desync), another anomaly or the scene (a confound), or a measurement artefact?
   - **m52 B0, 12/18 censored:** why can't the offset be judged under TAA? Propose what makes it judgeable (a null with the matched refresh, a longer post-window) for the re-bench.
   - **The arm sizes:** why CommitDelay 3 reads +1, and RestoreDelay three ways.
5. **MASK55 v2 (peak live 1 again; 270 of 442 labelled frames with no measured mask): INVESTIGATE FIRST.** Masks are half the release rule.
   - Break the 270 frames down by anomaly, `mask_state` and reason.
   - Is it the known G295 coalesced mask arm under load, a mask-pool/TagPoolLimit side effect of the bench lever, or a regression? Compare with 084-06's MASK55, which had 586/586 masked.
   - Then design a concurrency route that actually reaches peak live ≥ 2.
6. **camera_clipping AA off:** a reading owed. Re-run with a null that has signal, if cheap; otherwise documented.
7. **P-ORC / G-MODE (the uv16 oracle found 15/16 cells) and G-BIND (invalid fixture): diagnose and fix the fixture or the harness.** The mode oracle (IN 2) is owed on the bench. Offline oracle proofs (39/39) and S1 identity stand meanwhile.
8. **The arm sizes:** folded into item 4.
9. **G-REASON-DC MIXED rows:** the product refusing `no_textures` / `partial_footprint:1/2:no_textures` is **consistent with ND-1 (strict all-or-nothing)**. The fixture's colour-only slot has no texture, so the **prediction and fixture** were wrong, not the product. Fix the fixture (the colour-only slot gets a base-colour texture and no normal map) or re-declare the prediction before any re-run.
10. **G-LEVER-OFF:** the product evidence passes (no lever acted, no twin listed, and Part 0 shows DISABLED). **The gate predicate is wrong,** because packaged logs never print "not recognized". Re-express it as "lever had no effect + DISABLED echo". **G-CENSUS host:** the harness sends one command per frame. Both are harness fixes.
11. The uv MainWorld and COST legs are unblocked by item 2.

## Status per delivered anomaly (morning of 30 Sep)
| anomaly | status |
|---|---|
| blinking, missing_object, missing_texture, corrupted_texture | **SYNC-PROVEN** (blinking's synthetic-order E1 FAIL pending adjudication) |
| camera_clipping | **SYNC-PROVEN (AA on)**; AA off owed |
| stuck_low_mip | AA off offset exact and onset partial-flagged; **B3's 41-frame tail and B0's censored offsets pending**; yield decision pending the office counts |
| lod_popping | UNJUDGEABLE (black fixture), fix and re-bench |
| uv / normal corruption | **SYNC-PROVEN on the fixture, all 4 modes**; MainWorld legs pending the G403 fix |
| masks | recycling proven; **MASK55 v2's 270 unmeasured-mask frames pending investigation** |

## Next (overnight, one brief at a time)
1. **089-01 (xhigh), offline:** the adjudication (item 4), the MASK55 root cause (item 5), the LOD `worst=2` source read, and the arm sizes.
2. **089-02 (xhigh):** the harness fixes (items 2, 7, 9, 10; G395 in E1), the LOD fixture light + cook, the MASK55 concurrency route, the re-bench leg list, and a dry run.
3. **089-03 (high):** the targeted re-bench when the PC is idle.
