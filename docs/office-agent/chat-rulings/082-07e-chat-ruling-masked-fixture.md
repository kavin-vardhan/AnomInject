# 082-07e — Chat ruling on RED at G-ID MASKED: fixture cannot exercise alpha. Fix the fixture (after a full fixture audit), then resume

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `570e6f4` · Staged `2FCDF059` / `30FE0FDE`

## What the run established
- **Normal-map corruption is proven in the engine.**
  - G-ID was **exact (max 0)** on U-N1 and N-N1.
  - The normal wrong copies read 172 and 180, and the channel swaps read 72 and 65.
  - G3 read 0 at the same frame index.
  - G0 now shows all 7 normal-map targets reading APPLY.
- **G-ID identity was exact on all 14 rows run.** TRIP and G11 passed on all 54 legs. P2-1 (3,445 readings) and P2-3 (424 events) are confirmed read-only.
- **The stop is a fixture defect, and the stop was correct.** Even in the null leg, `TC_Masked` never clips: texels with alpha < 0.5 draw their colour, not the background. So its alpha wrong copy reads 0.
  - Consequence: **MASKED's identity PASS is vacuous for alpha.** It proves RGB identity only, and must not be cited as evidence that route B preserves cutouts.

## Rulings
1. **Fix the fixture. Do not drop or demote the row.**
   - Route B's advantage over route A is that it accepts **masked** hosts (the Bot face, Lyra's Manny/Quinn; plan §R14).
   - Showing that a masked host's cutout survives a corrupted-copy bind exactly is product-relevant. It is the only S1 row that covers it.
   - Deferring it to S2 would split S1 across builds for no saving: the PC is free tonight.
2. **Diagnose before fixing, from disk read-back, and name the cause.** The fixture tool has now produced three defects that surfaced one leg at a time (mesh slots, the Normal pin, the masked clip).
   - **Candidates to check:**
     - the actor's actual slot material (the slot-bug precedent);
     - blend mode;
     - the OpacityMask pin's connection, and which sampler output feeds it (A vs RGB);
     - the clip value;
     - a MIC blend-mode override;
     - `T_TC_UC2`'s compression, and whether alpha survives the cook.
   - If the cause lies outside CaptureBench content (an engine or project setting), **stop: NEEDS-DECISION**, and make no workaround.
3. **Full fixture audit before the cook: the G322 rule, applied to every row.** Every S1 Q row that depends on a channel being visible must show all of the following, listed per row:
   - from disk read-back, that the named tile's material routes that channel to a visible output;
   - that an offline wrong-copy proof exists with a predicted minimum ≥ 16.

   Anything missing or wrong is fixed in the **same** cook. The goal is no fourth fixture surprise in the remaining ~139 legs.
4. **Owed for MASKED:**
   - an offline proof that names which alpha quarters cross the 0.5 clip under the alpha fault, with the predicted min |d| against the background;
   - a **PRE row, added by AMENDMENT 2:** before the wrong copy is judged, the null leg must show the clip, with background visible where alpha < 0.5.
5. **Across builds.** Following the 082-07c precedent, rows already judged on `30FE0FDE` stand as within-build results. MASKED re-runs whole (ID, NULL, WC) on the new build.
   - A PROBE in 082-07f checks that the re-cook changed nothing but what it meant to: non-re-authored tiles must match the 082-07e null frames at the same frame index, ≤ 2.

## Sequencing
- **082-07f (xhigh, same session):**
  - the diagnosis, the audit, the fixture fix with read-back proofs, and the offline MASKED proof;
  - re-cook, G-COOK under the amended rule, stage, archive, A44;
  - one labelled in-engine PROBE outside the bank.
- **082-07g (xhigh, fresh session, bench-free):** AMENDMENT 2 (the MASKED PRE row, the build under test, any rows the audit changed), the boundary re-issue, and a dry run.
- **082-07h (high):** resume the legs from MASKED.
