# 087-01 — Chat ruling on the office check kit (NEEDS-DECISION): accepted; tonight the kit runs as a third, independent reading beside 084-09 (no harness patch); merge the kit branch into the fix branch before the rehearsal

Date: 2026-09-29 ~17:15 IST · Author: chat Claude · Plugin `feat/office-check-kit` `94dd112` (off `fix/m52-label-timing` `02d15a4`) · AnomDash `ab6e63f`

## Accepted
- **`label_sync_check.py`:**
  - selftest 32/32 with and without Pillow, and identical numbers from both decoders;
  - numbers only;
  - transition-aware, and partial-aware per 084-07 decisions 1–2;
  - refuses to pass a doctored case.
- **The proofs:**
  - master stuck_low_mip FAILs in 084-03/06's direction and size;
  - fixed builds read 0 at 50 % transition-aware (and the synthesised-partial copy of B9 reads 0);
  - **every edge both tools judge on the four proven types (871 starts / 744 ends) equals 086-01;**
  - doctored labels FAIL, except stuck_low_mip +1 under TAA, the documented G353 onset blind spot. It FAILs 16/16 with AA off.
- **Runtime:** 180 s with Pillow, 857 s stdlib-only, on the 56-event set.
- **The card `OFFICE-CHECK.md`,** and "what it can't judge".
- **The AnomDash bundler fix** (dev token removed, the delivered token written from the build log, placeholders refused, exit 4 on missing inputs), proven with a synthetic log. The rehearsal repeats it with a real build log.
- **G395 / G396.**

## Decisions
1. **G395 in `084-09-eval.py`: do NOT patch the frozen harness hours before the bench. Run the kit as a third, independent reading** on every judged leg of tonight's parts 1 and 2 (084-09 and 085-04), beside each harness's own evaluator and the 086-01 checker.
   - **Rule for the morning ruling:** any FAIL on any instrument is investigated per event.
   - A FAIL may be called a G395 artefact **only** when shown on that event: the judged frame is inside its own reference, and the kit and 086-01 both read 0.
   - Magnitudes are quoted from the kit, not from 084-09-eval, where they differ.
   - `084-09-eval` is patched for G395 after tonight, in the next harness round.
2. **Merge order for delivery:**
   - `feat/office-check-kit` merges into `fix/m52-label-timing` (no-ff) **before** the delivery rehearsal;
   - the fix branch then merges into m53 again;
   - the rehearsal runs a fresh AnomDash `npm run build` and the token check with the real build's log.

   This goes into 088-01.

## Next
- **084-11** (the m52 yield plan, option A per the owner's go), released now.
- **Then 084-12** (option A source + build) if the plan is clean, so Wednesday by day is harness + Codex review.
- **Tonight ~22:30:** the night bench, with the kit as the third reading.
- **Wednesday:** 088-01 (the rehearsal, with the merge order above).
