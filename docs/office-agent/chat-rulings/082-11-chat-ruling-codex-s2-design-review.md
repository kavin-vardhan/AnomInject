# 082-11 — Chat ruling on Codex's S2 design review (CHANGES REQUIRED): all 11 findings accepted; revise the plan, redesign S2-0, run a Codex delta check

Date: 2026-09-28 (IST) · Author: chat Claude · Plan `f6b47d4` · Review `_reviews\082-11-codex-m53-s2-design-review.md` (Astra/max, 27 min)

## Headline
- There is no P1. The eight mode parameterisations fit the authored graphs, and "0 plugin cooks" holds; the corruptor strings are present in the committed `.uasset` bytes.
- **The MainWorld shadow trace is independently confirmed:**
  - install vs control shows ≥ 16 on 43–70 px at **every** si 39–99;
  - controls agree within 2;
  - NoApply-1 allocates and draws without `SetMaterial` and has **no** trace, so installation is the discriminator.
- Which final image is "correct" is not yet known.

## Rulings (every finding ACCEPTED)
1. **P2-1, mode correctness: a mode-specific Q output oracle.**
   - Keep G1 as the alignment gate. Add an independent per-mode oracle on fixture cells built to be asymmetric:
     - the sampled source cell for tile, swap and scramble;
     - the expected signed and encoded normal for invert, green_flip and flat;
     - a reference tied to the authored sample and amplitude for noise.
   - **Include wrong-mode negatives.** An invert implemented as green_flip must FAIL.
   - If the oracle needs fixture cells, they join the S2 cook.
2. **P2-2, drift: qualify the snapshot path on its own.** Required:
   - snapshot vs original identity, on colour, data and normal encodings;
   - proof that the level MID samples the snapshot, not the live source;
   - a real residency change on the source while drift is active;
   - allocation failure **after** the snapshot is acquired, plus one abnormal drift exit, both with balanced accounting;
   - a boundary-budget case that separates outputs + scratch from outputs + snapshots + scratch.
3. **P2-3, S2-0 redesign: a matrix, not a decision tree.**
   - **Arms:**
     - an m53 identity install;
     - an existing material swap (`corrupted_texture`), footprint-matched where possible, with the limitation recorded where it isn't;
     - a **no-material-change render-state refresh control** (for example, re-registering the same components' render state with the original material);

     each crossed with **shadow cache on and off**, and each cell with its own non-installing nulls.
   - **Protocol:**
     - a single event, followed by an equally long clean tail;
     - log the installed component/slot footprint;
     - record the original, raw and resolved material and the post-revert slot state, including override-array length.
   - **Outcome vocabulary:** "cache-dependent", "reproduced by another material swap", "reproduced by a plain render-state refresh", "specific to the tested m53 path".
   - **If the plain refresh reproduces the installed image,** the non-installing null is the stale image. The anomaly then refreshes a stale shadow rather than leaving residue, and that is recorded as a label caveat, not a restore defect.
   - **A causal defect classification** needs the state evidence as well, not the pixels alone.
4. **P2-4, the renamed successor: KEEP the §R8.3 cleanup contract.**
   - The renamed row proves that no successor slot keeps an owned MID, that the fallback is the asset default, and that resources are released. It makes **no exact-restore claim**.
   - The same-name variant is the exact-restore control.
   - Record the limitation: a component re-created under a new name while corruption is active falls back to its asset default.
5. **P2-5, G5 Restoring:** the premise and gate legs must prove a measured residency drop before Revert **and** an actual insertion into Restoring, then read the entry at the m53 decision.
   - Only if the premise window shows the new fixture can't produce this deterministically: the lever pins the insertion too, and that row is labelled state/plumbing.
   - Pin cleanup at run end is required.
   - Correct the plan's ratio claim (targeted m52 already bypasses it).
6. **P2-6, level change: as Codex proposes.**
   - APPLIED before the open;
   - a world-ending target-loss revert, with old-world pending / `not_yet_due`;
   - a tagged new-world reading at or after the due frame, from the **same** ticket, which must survive the old instance;
   - wait for a separate post-GC census-complete token.
   - The object census is proven able to see a deliberately retained named render target, and missing or misordered tokens FAIL.
7. **P2-7, telemetry:**
   - **The serialisation changes are in scope** (`AnomalyTelemetry.h`, `AnomalyLabelWriter.cpp`).
   - **Non-m53 label output stays byte-identical,** proven on banked runs, and m53's existing keys keep their shapes. Arrays can be empty, and that is stated.
   - One canonical full-component-path + slot identity is used everywhere.
   - Committed, refused, and qualified-but-uncommitted slots are kept apart. The partition assertion covers real commits only, and NoApply is validated separately.
   - Binding-to-slot is verified against the event's captured binding inventory.
8. **P2-8, the RNG contract (chat's decision):** every m53 attempt consumes **exactly one** mode draw, even with an empty enabled set, which then refuses `no_mode_enabled`. Non-m53 ids consume none.
   - The stream position therefore doesn't depend on the mode configuration, so changing D5's sets never perturbs other draws.
   - **Test:** the real selection routine against an independent known-answer stream, covering zero, one and several modes, and applied and refused attempts.
   - **Log:** id, target, full-precision hold, mode, attempt ordinal and stream state.
   - The census-off comparisons stay as integration evidence, with both ids and refused m53 attempts covered.
9. **P2-9, cross-anomaly sharing:** first establish from source whether the **product auto-injection path** can run m52 and m53 at the same time on different actors (MaxConcurrent default, auto-pool rules).
   - **If it can by default:** S2 includes a source fix so m52's co-affected enumeration accounts for m53-owned hosts' original bindings. The failure case: m53 reverts on A while m52 holds the shared source, so A shows the held source without being labelled.
   - **If it can't:** narrow the limitation to raw Apply and `Auto.FireOnce` outside the FSM, and record it.
   - Either way, drift's snapshot-isolation proof (ruling 2) stands.
10. **P2-10, three predicates:**
    - E6 is validated through an actual parse or Apply decision, with earlier-precondition controls, not through the census.
    - G4 CAN asserts no Apply/DECIDE, no warm draw or allocation, no BeginActualRun/FINISHED and no surviving session. Configuration echoes are allowed, against a post-reset baseline. TRIP explicitly exempts a proven pre-focus cancellation.
    - G3-S2 declares the expected per-slot restore disposition (exact, default or swept) instead of requiring restored-exact globally.
11. **P3-1, levers in client builds:**
    - New S2 levers live in `AnomalyBench` or require an explicit bench-only enablement. `DestroyTarget` moves behind the same gate.
    - The S2 proof is narrow (Shipping and bench enablement).
    - **The M3 client-artifact proof,** covering the actual delivered configuration (Development builds are supported), goes on the M3 checklist.

## Sequence
1. **082-12 (xhigh, fresh session, bench-free):**
   - plan revision 2 of `docs/predictions/2026-09-28-m53-s2.md`, applying rulings 1–11;
   - the P2-9 source determination;
   - the S2-0 matrix spec, with predictions declared first;
   - then a **Codex delta check through the Relay**. Collect it; do not act on it.
2. **Then S2-0**, the redesigned probe on the current build, with the owner's PC OK.
3. **Then S2-1 source.**
