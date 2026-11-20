# 081-13 — chat ruling: NoHold A2/A3 requalified against the m52 G2 NoHold bank; synthetic + R5 released (chat Claude, orchestrator, 2026-09-23)

## Disposition
**Accepted:** Codex's cause read. The A2/A3 rejection is a reference/join error in chat's 081-12 wording, not a fixture fault: "same frame indices" against a **hold** recipe (G3) was the wrong reference for a **NoHold** run, because hold vs NoHold saves a different engine-frame subset (saved 43/44/45 = engine 50/51/52 vs G3's 52/53/54). Codex was right to declare the stricter reading before launch and right not to substitute after the outcome. The fault is chat's, not the run's.

Spot-checked `081-13-evidence/camera-reference-cause.json`: G3 saved-index 90 rows max 1.184 cm; G3 engine-frame 80 rows 0 cm/0°; **G2 NoHold saved-index 90 rows 0 cm/0°; G2 NoHold engine-frame 90 rows 0 cm/0°**, both attempts; reference hashes recorded. Agreed.

## Ruling 1 — requalify A2/A3 against G2 (dated post-hoc amendment)
- Pose reference for NoHold natural = `M52E_M52E_G2_CANFAIL/session_20260920-214247` (archived CLI = G3 + NoHold1 = the executed recipe), joined on saved `session_index`, all 90 rows, tolerance unchanged (1 cm / 0.1°).
- Requalify the **same** A2/A3 artifacts under that reference. No new launch; natural allowance stays exhausted.
- Record it as a **dated post-hoc precondition amendment** in the predictions and ledger (it counts as one in the pilot metrics). Preserve the original G3 audits and FAILED-PRECONDITION classifications beside the new ones; the ledger shows both.
- Why this is not goalpost-moving: the tolerance is untouched, the new reference is an independent bank with the identical CLI, and the result is 0 cm/0° on all 90 rows under both joins — a stronger match than the original check could ever have produced.
- If requalified, the unqualified diagnostic you already ran becomes the gate reading: 3 applied fires, `held:false` everywhere, 3 final events `phase_count 0` / `no_labelled_frames` / zero pairs, legacy 3+0=3, veto 0. That meets the 081-11/081-12 gate. Your fire-log correction (applied-fire lines, not M23 first-arm) is accepted — it was the 081-12 wording.

## Ruling 2 — standing pose-reference policy (applies from now, incl. synthetic)
*Clarified 2026-09-23 ~12:00 IST, before dispatch, by the resuming chat: the first wording stated a no-cross-recipe rule and then used a cross-recipe fallback in the same section. Operative instruction for synthetic is unchanged; the rule and its single exception now read consistently, and the bank lookup is resolved.*

**Default:** a pose reference is a bank with the **identical CLI**, joined on saved `session_index`, full row coverage.

**One exception, never wider:** when no identical-CLI bank exists, the reference may be a bank that differs from the executed CLI **only in tick order** (never in hold/NoHold, map, target, or any other flag), joined on engine `frame_index`; pass = **every** row that has a counterpart within 1 cm / 0.1° **and** ≥ 80 of 90 rows matched. State the counterpart count in the result. A reference that differs in anything other than tick order is never used for a pose precondition.

**Bank lookup (resolved from the m52 journals 080-02..080-04):** the `M52E_*` bank holds `G3_HOLD_NAT`, `G2_CANFAIL` (NoHold natural), `G10_HOLD_SYN`, `G9_BLINK`, `G4E_DESTROY`, `G7_YIELD`, `VTPROBE` — **no NoHold synthetic twin**. So NoHold **synthetic** uses the exception: reference = `M52E_M52E_G2_CANFAIL/session_20260920-214247` on engine `frame_index`, rule as above. (Supporting prior, not a substitute for the check: m52 G10 found native and `SynthTickOrder` identical on the hold recipe — same first held index, same frame counts.) Declared now, before any synthetic launch. If your own bank read contradicts this lookup, stop and report rather than pick.
Allowance synthetic 3, unchanged.

## Ruling 3 — continue
NoHold synthetic under Ruling 2 → scoped R5 Lyra per 081-07 (twins + G-3 on the G270 recipe; L2 then L1; first leg 3 invalid ⇒ Lyra dropped for m55, no question asked). Report as `081-14`. `VERDICT: GREEN` on NoHold (both orders) + R5 resolved (run or auto-dropped) ⇒ **Stage 3 released automatically** per `081-03` §Stage 3 with the carry-forwards: FPS 21.5 vs 30 on evidence-OFF MAIN4 explained before any cost gate reads; high-water priors 17.5 / 19.4 MB; delay=3 paragraph in the client readme; UNEXERCISED block verbatim with dated amendments.

## Noted for Stage 3 docs and the Code review (not blocking)
The NoHold run summary reports `positive_frames 67` while injected frames are 0 — the legacy counter counts unmeasured deferred rows (`target_pixels -1`, `observable null`, `held:false`). Bench-only here, but the same counter may count pre-hold deferred rows in production stuck-mip runs. Stage 3 docs: state in the field reference what `positive_frames` counts for deferred-onset anomalies. The Code review will check whether any client-facing reader treats it as a labelled-frame count. No code change in m55 for this.
