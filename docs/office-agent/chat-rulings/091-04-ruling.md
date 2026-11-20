# 091-04 — Ruling on Codex's independent review (Job E): F1, F2 and F6 are fixed before merge; F3 is part-fixed now, with a residual limitation for the owner's decision; F4 and F5 are documented follow-ups. The merge waits for 091-05.

Date: 2026-10-04 ~03:45 IST · Author: Claude Code Desktop (orchestrator/reviewer) · Review `_relay\runs\2026-10-04-091-04-independent-review\review.md` (Codex Astra/max, read-only, 52 min; VERDICT: HOLD MERGE) · Range: m53 `0a69ea3..783a768`, proxy `783a768..d660b5d`, fix `cf422d5..8badc23`.

## What the review confirmed (agreeing with my own checks)
- Annotation schema 2.1 export: 80 events, 0 discrepancies; route frozen per event; no `IsRenderTruthFire` / `bProxyBlur` bug; temporal detection is fail-safe; the 091-02 include guards are correct against 5.1.1 and 5.7.4.
- Can-fails exist for every named checker family. It also correctly marks two of them as circular when read as visual truth (the E1 state subcheck, and the scalar read-back): they are wiring checks and must never be advertised as pixel proof.

## Rulings
**F1 (HIGH), FIX BEFORE MERGE: a non-owner view family can consume a pending colour request or mask arm.**
- The source evidence is convincing: activation ignores the context; `ArmBelongsToFamily` fails open when a family has no bound; colour dequeue doesn't check the owner; the key ring is keyed by frame number.
- My bank search found 0 ownership or `view_rejected` lines in 54 banked PIE logs. But those were command-line `-pie` launches with one rendering viewport. The owner's office PIE is an interactive editor that may have extra realtime viewports, and the delivery default (Async/SVE/Pace) reaches this path. Treated as real.
- **Required:** a minimal ownership guard. Reject families whose scene/world is not the capture world's (and, where cleanly identifiable, whose target is not the capture's viewport) **before** dequeue. A family without a bound must not serve an arm. A request whose owner render was skipped must end as `capture_unpaired` (unlabelled, dropped), never be served by a later family with a stale snapshot. No redesign beyond this.
- **Proof:** an interactive-style PIE with a second realtime editor viewport rendering the editor world, and PIE in a new window. Show the guard rejecting non-owner families; 0 label/picture disagreements (E1 t50), 0 missing masks. Can-fail: the same setup on `d660b5d`. If the old build cannot be made to mislabel in any setup tried, report that honestly; the guard then stays as defence, with that evidence.

**F2 (HIGH), FIX BEFORE MERGE: the UV/normal component scope falls back to the whole actor when an unchanged component has nothing touchable.**
- Always install the positively enumerated changed-component scope for UV/normal, as the proxy path already does; keep it in step with installation and revert.
- **Proof:** a fixture with C1 eligible and C2 rendering a null/default material (no override). The mask, m26 and bbox cover C1 only, with C2's pixels unchanged. Can-fail: `d660b5d` masks C2.

**F3 (HIGH), PART-FIX NOW; residual to the owner: `texture_uniform` passes textures whose variation the material never shows.**
- **Fix now (cheap, rule 1):** for the UV-family rule, alpha variation counts only when the slot's material uses alpha (a masked or translucent blend, or an opacity input connected). Otherwise judge RGB only.
- **Proof:** an alpha-only-variation texture on an opaque material is refused `texture_uniform`, while the same texture on a masked material is still admitted and visibly changes. Can-fail: `d660b5d` labels the opaque case with no visible change.
- **Residual, documented in the readme (§8.8 limitations) and taken to the owner:**
  - variation only in a colour channel the material never samples (packed maps);
  - normal maps feeding a zero-strength normal input.
  
  Both can still give labelled events with no visible change. The real fix is a measured visible-change gate at onset (a target-local RGB change against the pre-event reference, failing safe to unlabelled). That is a follow-up item, not something to build the night before delivery. Banked evidence on office-like content: 28/28 visible; the owner's second office host: visibly corrupting.

**F4 (MEDIUM), FOLLOW-UP:** `below_size_policy`, `too_small_for_ratio` and `imperceptible` are selection/yield heuristics, not proven four-rule refusals. Keep them for Monday (they cost yield, not label truth). Readme wording: "selection heuristics", not "impossible". Follow-up: turn them into preferences or replace them with measured visibility.

**F5 (MEDIUM), FOLLOW-UP:** the copy is not bound to one coherent source snapshot; the tripwires count but don't veto. `rt_mip_mismatch` / `copy_source_changed` are 0 in every banked leg. Follow-up: bind the draws to the retained snapshot and make the tripwire a veto (or revert and flag). Readme: a known robustness limit under heavy streaming churn.

**F6 (LOW), FIX NOW:** `annotation_transition_check` must cross-check every labels-side event identity, including transition-free events, against annotation.json. Add the wrong-ID-without-transitions control.

## Process
- Fixes go to Codex in **091-05** (`_relay\runs\2026-10-04-091-05-review-fixes\`).
- Then the short regression matrix, pushes, and the Job F path: the extended smoke, merges to master, the Development package and the rehearsal.
- Since F1 to F3 touch label truth, the owner's Sunday office check of `d660b5d` stays useful for blur, transitions and UV/normal visibility. The final delivery build will be the 091-05 head after its home proof.
