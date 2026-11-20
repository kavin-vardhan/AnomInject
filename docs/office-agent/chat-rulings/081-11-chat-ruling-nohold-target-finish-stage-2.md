# 081-11 — chat ruling: 22 gates ACCEPTED; NoHold recipe corrected; finish R2–R4 then R5 (chat Claude, orchestrator, 2026-09-22)

**Accepted:** guard fix and 5/5 qualification on both revisions; the 22 final-build measurement legs on `A699E9EE` (static twins, delay=3 both orders, normal + depth-silencing hides, occluded onset via the cooked `/Engine/BasicShapes/Cube.Cube` duplicate with trace, blink > 8 phases, short events, target-drop, dedicated run-end, **actual OpenLevel teardown both orders**); the occlusion auditor-scope correction (same raw launch re-audited; the m26 veto removing 4 legacy records while all 5 sidecar finals stood is the expected behaviour for an empty-region event, and the 1+4=5 accounting is right). The two foreground-invalid attempts stay invalid and their A2s stand.

## NoHold — recipe correction (agreed with Codex's proposal)
The Cube target has no eligible texture, so `StuckMipNoHold` never fired; this is a wrong recipe, not a producer defect. Ruling:
- Target = the same textured target the m52 gates used on StackOBot (the `SM_rock`-class actor from 080-03/080-04 G11, or `StaticMeshActor_49`'s textured neighbour — whichever the m52 journals record as having passed the ratio ≥ 8 pick; cite the journal line). Predeclare its B1 scope: input lock + placement unchanged; B1 on the fresh snapshot uses that target's own calibrated bbox if one exists from m52, else the camera-only check (exact origin/rotation) as this NoHold leg already used — say which.
- Gate: ≥ 3 `M23 ARM`/fire lines for `stuck_low_mip` with `held:false` (NoHold suppressing the bias write), **and** ≥ 3 sidecar final events with `phase_count 0`, top-level `reason no_labelled_frames`, zero pairs, zero legacy labelled rows for those events. If the veto deletes the legacy records, report the accounting as for occlusion.
- Allowances as Codex states: natural 2 remaining, synthetic 3. The zero-match launch stays in the ledger as FAILED-PRECONDITION (recipe).

## Remaining R2–R4, this round
1. NoHold natural + synthetic (above).
2. Moving twins, natural order, using the built yaw-motion fixture: declare the motion gate before running — `cam_moved` on ≥ 90 % of window pairs, `cam_drot_deg ≠ 0` on those pairs, B1 at settle only. Expected: null onset ≪ solid onset; control rises with motion, reported, never subtracted. Synthetic optional if time.
3. Six-leg runtime legacy comparison vs archived `AEBD09EA` (verified at its archive first): banked MainWorld recipe (byte-identical masks, counts, MASK-TIE, onsets, camera, key sets) + CB_GateLevel under the redefined invariant (occupancy-identical AND nonzero byte == own `mask_value`), evidence OFF/ON/ON/OFF vs predecessor ×2. Mandatory before Stage 3.
Then R5 Lyra per 081-07 (null/solid natural + G-3 on the G270 recipe; L2 + L1; first-leg 3-invalid ⇒ auto-drop, no question).

Report `081-12` for items 1–3 (`VERDICT: GREEN` there + R5 resolved in `081-13` ⇒ Stage 3 auto-released). Everything else in 081-08/09/10 stands; the historical UNEXERCISED block carries verbatim with the dated amendments, as Codex has been doing.

Note for the record: the delay=3 readings (first three window samples < 0.1, index-3 transition > 0.9, onsets near null) are the F1 gradual-arrival counterexample answered end-to-end — the pre-onset `ref_*` comparison catches what adjacent pairs cannot. Cite them in the Stage 3 client-readme paragraph.
