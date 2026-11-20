# 090-02 — Chat ruling on Codex's evidence review (CHANGES REQUIRED): F1 is fixed in source (a FireWindow label must end when the effect ends); F2–F5 and LOW readme corrections; F3/F6 evaluator repairs before the evaluator's next release use; post-result changes judged sound

Date: 2026-09-30 ~10:45 IST · Author: chat Claude · Heads `fix/m52-label-timing` `0347e8c`, `feat/m53-uv-normal-corruption` `da902d6` · Review `_reviews\090-02-codex-evidence-review.md` (Astra max, 2,336 s)

## Accepted
- **Question 2, the post-result changes:** the 085-04 WARMUP exclusion, G405, G406, camera_clipping's confirmed-hold reading, the 089-04b G-MODE family gate and `d57246c` are **sound.** Codex re-ran each against one-frame mutations. G395 was the right direction but incomplete (F6).
- **Question 3:** no functional defect in the OffFrames 16 default, the LOD wording, or the merge.

## Rulings
1. **F1 (HIGH), FIX IN SOURCE before the delivery.** A FireWindow label (missing_texture, corrupted_texture, uv/normal corruption) stays positive until the scheduled end even if the effect is gone early: a manual/raw revert, a host that replaces the material mid-event (common in real games: damage flashes, material swaps), or a target teardown.
   - No bench exercises it, but on real content it is a label that disagrees with the picture, which the release rule forbids. It is also cheap to fix.
   - **The rule:** FireWindow membership in `IsAnnotationMember` requires **the effect to be installed on this frame.** Our material or MID is still bound on the targeted slots, checked per captured frame, cheaply, on the same sample point as today.
   - **Frames where it isn't installed are not labelled.** They carry `transition_reason: effect_interrupted` so clients can see why, and an early revert ends the label at the actual revert.
   - m53's foreign-replace re-apply (2 ticks later) is covered the same way: the frames in between are unlabelled and flagged.
   - **Tests both ways,** including Codex's exact case (apply 10, revert 20: labelled 10–19 only). A mutant that ignores the installed check must fail. The existing exclusion selftest expectation that encoded the old behaviour is updated.
   - **A bench smoke tonight** (see below).
2. **F2 (HIGH), F4, F5 and LOW: readme corrections** (no source change):
   - state the **actual release rule**: the half-strength edge, 0 frames off outside flagged transition frames, with the TAA onset blind spot named;
   - correct "fade ended 4–6 frames after" to "fell below half strength 4–6 frames after";
   - remove "unflagged frames agree pixel for pixel";
   - the §8.7a table states **exactly what was run** (which orders, AA on = TSR, synthetic AA-off not run for m53, repeats natural-order only, mode checks on separate fixture objects, one resolution, not yet on the office hosts);
   - soften "every mesh / never an unchanged part";
   - "any negative value = default".
3. **F3 (MEDIUM):**
   - the readme counts become "16 complete events + 1 whose end runs past the capture (censored)";
   - **the evaluator wrapper stops counting an unfinished end as proven** before its next release use.
4. **F6 (MEDIUM):** E1's offset window (RD5) is repaired **before its next release use** (already queued), with no re-run now. RD5's pixel reading stands.
5. **E: below the 50 GB floor:** the owner's call (asked).

## Combining with the owner's Nanite decision
F1 and the Nanite skip (the owner's "we can skip nanite") both touch who gets labelled. They go into **one source round, 090-05**, with one build per branch and one bench smoke tonight. 090-03 is superseded by 090-05.

## Next (one brief at a time)
1. **090-04,** the strict-include compile fix (the second office host is blocked), released now.
2. **090-05:** F1, the Nanite skip (a setting, default skip), and the readme corrections (F2–F5, LOW, F3 counts), on the fix branch; merge into m53; builds.
3. **Tonight's bench smoke (idle-gated):**
   - FireWindow B-REAL legs (missing/corrupted texture) and m53 fixture legs, plus an early-revert leg proving F1 from pixels;
   - a Nanite-skip leg (0 unmeasured-Nanite lines; every labelled frame masked);
   - a quick sync regression.
4. **Thursday:** the owner's office test on the resulting head.
