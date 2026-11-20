# 090-08 — Chat ruling on the Codex re-check (CHANGES REQUIRED): the Shipping exclusion and constructor are accepted; every HIGH is fixed before any merge, in two source rounds (090-09: R1–R3, R5–R10 and the kit; 090-10: R4, the capture pairing, plus the extended smoke harness); merges move to the weekend; delivery stays Monday

Date: 2026-10-01 ~07:45 IST · Author: chat Claude · Review `_reviews\090-08-codex-source-recheck.md` (Astra max, 1,702 s) · Heads fix `c29e223`, m53 `2015cfa`

## Accepted
- **Question 3:** the Shipping exclusion (the Development, Test and Editor graphs are unchanged; no Anomaly module in Shipping) and the vtable-helper constructor (identical to the engine macro).
- **The original F1 case is repaired,** and the interrupted-m53 exclusion uses the correct live state.
- **Code waited for memory headroom** before launching Codex, so the owner's render wasn't put at risk. That was the right call.

## Chat's own assessment
Codex found 4 HIGH label/picture holes that Code's tests and chat's rulings missed. The common cause is that "installed" was defined from the **records made at apply time**, not from **what renders now**, and that the capture **pairs** label inputs with a picture made at a different moment. None of these is exercised by a static-camera, static-target bench, which is why every pixel gate passed. Real gameplay has moving targets, material swaps and mesh changes, so they must be fixed before delivery. Per the governing rule, sync comes first.

## Rulings
1. **R1 (HIGH), FIX.**
   - "Installed" means **at least one targeted slot**, on a live, registered, visible component, **currently renders our material** (the material the component resolves for that slot now).
     - None left: `effect_interrupted`, unlabelled, unmasked.
     - Some replaced: **still labelled**, because the object still shows the effect (the box/mask identify the object, per F5). These frames are counted as `label_effect_partial_frames`.
2. **R2 (HIGH), FIX.** Revert restores or clears **every override index written at apply**, whatever the current slot count. After revert, assert that no override of ours remains at any index.
3. **R3 (HIGH), FIX.** The installed test reads **current render state**: the component is valid, registered and visible; the slot exists on the current mesh; and the resolved material at that slot is ours. Re-evaluate it every captured frame, at the same sample point.
4. **R4 (HIGH), FIX in 090-10, after 090-09 verifies it.**
   - **Async:** freeze the membership geometry (the actor bounds used for on-screen membership and projection), together with the installed state and the event identity, at the **same sample** that the capture request carries. Consume the frozen values at readback completion.
   - **Sync** (`IAI.Capture.Async 0` and the async-rectangle fallback): pair each picture with the state sampled for **that** picture, using a one-deep state history or equivalent. Failing that, flag those frames (`capture_unpaired`) so they are dropped, and mark the path unsupported for delivery.
   - **090-09 reports first:**
     - when the fallback fires;
     - whether any delivery default uses it;
     - a fix design with a cost estimate.
5. **R5 (HIGH), FIX.**
   - The Nanite check enumerates **every primitive the actor draws, ignoring target-selection exclusions**.
   - With `AllowNanite` 0, **no frame is labelled** while the target draws any Nanite primitive. A Nanite part appearing mid-event gives `nanite_unmaskable` frames and the event is reverted at the next tick.
6. **R6 (MEDIUM), FIX: fail CLOSED.**
   - With `AllowNanite` 0 and no probe registered, every target is refused as `nanite_probe_missing`, with a loud log line and a `run_summary` field.
   - It is the owner's decision ("skip Nanite") that must hold, not the probe's presence.
7. **R7 (HIGH), FIX.**
   - The kit never reads PASS for an event with any labelled run it couldn't judge. That event reads **unjudged**, and READ BACK gains `unjudged N`, with one line on the office card.
   - Codex's offline repro becomes a selftest case.
8. **R8 (MEDIUM), FIX.** An `effect_interrupted` frame never gets the AA excuse, whatever other reasons it carries. Codex's repro becomes a selftest case.
9. **R9 (MEDIUM), FIX (text), after 090-09's source changes.**
   - Every item Codex lists, including:
     - **`effect_interrupted` means "the effect may be fully or partly gone: drop these frames, never use them as negatives"**;
     - the early-revert proof is stated as "unit-tested; capture proof pending" until the smoke runs;
     - "every labelled event has a real mask" is qualified;
     - Shipping wording: "the plugin's modules are not built for Shipping".
   - Also R1's partial semantics and R6's fail-closed rule.
10. **R10 (LOW), FIX.** F1's cost is logged per **captured frame** (the sum of that frame's calls): mean and max against the 0.05 / 0.5 ms threshold.
11. **R11 (MEDIUM, yield only):** fix it if it's a local change (about 20 lines or fewer, with tests). Otherwise document "spline meshes on Nanite-enabled assets are skipped" and defer.
12. **NOTE (manual re-apply):** it stays documented as unsupported. Make sure the readme says so in the operator section.
13. **The kit's one-frame-gap limit** (censored, not failed) is accepted as a known limit and documented on the office card.

## Pixel proof
None of R1, R3 or R4 can be proven by a static bench. 090-10 extends the 090-06 harness with declared legs. Each needs a bench lever where one is missing, and an OLD-exe can-fail where the old behaviour is wrong:
- **R1:** a partial slot replace on a multi-slot target;
- **R3:** the host hides the target mid-event, and the host changes the mesh mid-event;
- **R4 async:** a target moving across the frustum edge during an event, and a target destroyed mid-event;
- **R4 sync:** the same F1 leg under `IAI.Capture.Async 0`.

It is re-pinned to the new exe and dry-run. The run follows when the PC is idle.

## Schedule (a product consequence, told to the owner)
- **Master merges move from Friday to the weekend.** They happen once both rounds, the extended smoke and a Codex re-check pass.
- **Delivery stays Monday 5 Oct.** If the weekend gates don't pass, the owner chooses between shipping with documented limits and slipping. He'll get that decision Sunday with the evidence, not before.
- **The owner's office test today on `2015cfa` stays useful:** compile, census, m52 counts, and kit readings on uninterrupted events. Interrupted events read today are treated as unproven (R7).
- `_held\090-06r2-smoke-run.md` is **superseded**: the smoke runs on the fixed exe.
