# 085-02 — Chat ruling on the m53 DC source (NEEDS-DECISION): merge and DC source accepted; Codex F1–F4 fixed now; F5 is a bench-only limitation; a targeted capture with no mode goes round-robin

Date: 2026-09-29 ~10:05 IST · Author: chat Claude · Branch `feat/m53-uv-normal-corruption` `c9a098d` (merge `7cda2f1`, DC `b1cfc8a`) · Exe `27055E55` (archived, not cooked or staged)

## Accepted
- **The merge:** the same 8 files as the dry run, keep-both; the merged commit's own suites are green; the lever-audit failure on the bare merge was predicted and is cleared by DC (G378).
- **The destructor:** one destructor, with a structural check (6 mutants fail, and it fails on the pre-merge parent).
- **The DC day items as built:**
  - the four modes, with the refusals produced by a real Apply;
  - the scramble CRC;
  - the one-draw mode contract (KAT 286, three mutants fail);
  - the V1P all-or-nothing step;
  - the exclusion rule in a pure header (truth table 102, "live until BeginRevert" fails);
  - m53 on FireWindow → `IsAnnotationMember`;
  - the census command, the cost lines, and readme §8.8.
- **Codex's confirmations:** no m52 label-sync behaviour lost in the merge; `DrawAttempt` matches UE 5.1's stream.

## Rulings
1. **F1 + F2 (high, on EXCL-A): FIX NOW.** The owner's rule is "never at the same moment", and that means **in the frames**, not only in the plugin's state.
   - **Generalise "live"** for the exclusion: an id is live until (a) its last labelled frame **and** its last `transition`-flagged tail frame have been emitted, **and** (b) no receipts are outstanding for it, **and** (c) no detached trail can reopen it.
   - Apply this to both directions (m53's own tail, if it ever gets flags).
   - **Truth-table cases:** m53 admitted during m52's TAA off-frames must fail; admitted while a detached trail with outstanding receipts exists must fail.
   - **EXCL-A's gate adds:** 0 frames carrying both an m52 entry (labelled or transition) and an m53 entry.
2. **F3 (high; off the planned recipes, but it can put an unlabelled effect on screen): FIX NOW.**
   - A running effect's active record is never dropped while the effect is installed.
   - A re-Apply whose target is missing either reverts the running effect first, or is refused and leaves the record intact.
   - Test both ways: the mutant must leave an effect with no record.
3. **F4 (medium, on G-CENSUS, and on Thursday's office path): FIX NOW.**
   - The census uses its own **read-only** enumeration that prints **counts only**: no actor names, ever.
   - It never mutates the run's stats or the pool.
   - Test that the output contains no name tokens, and that the stats are byte-identical before and after.
4. **F5 (medium, bench-only RestoreDelay lever): documented bench-only limitation.** The 085-03 harness asserts **one RestoreDelay run per process** (no later run in the same process).
5. **A dashboard targeted capture with no mode: ROUND-ROBIN, not first-enabled.**
   - When a targeted m53 fire has no mode argument, use the next mode of that id's enabled set in round-robin order (a per-id counter), and log `mode_source=round_robin`.
   - **No RNG draw is added**, so targeted fire's stream is unchanged. The owner (Thursday) and the client see every shipped mode from the dashboard without a dashboard change.
   - Unit test both ways; readme §8.8 gets one line.
6. **Smaller calls:**
   - `family:` before `not_in_delivery:`: **accepted.**
   - The old `Auto.Fire` line kept beside `Auto.Draw`: **accepted.**
   - **The free-running m52 restoring set must be bounded.** The same `restore_unresolved` timeout applies outside a capture. Add a truth-table case showing the exclusion releases after the timeout (an exclusion that can last forever is a defect).
   - `IAI.Auto.Pool all 1` enabling the m53 ids: **accepted** (they ship "available"). The readme notes that "all" pools draw a different sequence from earlier builds for the same seed.

## Next
- **085-02b (xhigh, fresh), released now:**
  - F1–F4, the round-robin, and the free-running bound;
  - rebuild, suites, mutants, lever audit;
  - **a Codex re-check of F1–F4 only**, in the background, collected and not acted on.

  After this, the loop rule applies: only a HIGH finding on a path the bench exercises, or one in the data, blocks.
- **Then:** 085-03 (the fixture cook + harness + dry run on the 085-02b build), then 087-01.
