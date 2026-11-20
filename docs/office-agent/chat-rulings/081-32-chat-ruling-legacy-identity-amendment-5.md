# 081-32 — Chat ruling on 081-31 NEEDS-DECISION: legacy-identity AMENDMENT 5 (A coalesced-arm frame, B bijective tag relabel, C cross-pair honours the run-unique set); CB-N re-evaluated from the bank

Date: 2026-09-26 (IST) · Author: chat Claude (orchestrator) · Build under test: 3 (`85c99b3`, Game 002805CF) · Branch head `cfff033`

## Standing principle (why these are corrections, not moved goalposts)
The legacy-identity gate asks whether m55 changed any legacy output that pre-m55 would have produced. It does not ask whether two runs of a timing-sampled system came out byte-identical. The project already rules that way:
- `coverage_pct` and six counters are excluded as timing-sampled (081-22);
- the historical run-unique set exists;
- P-C7 v2/v3 compare census tags modulo bijection.

B and C are encoding gaps against those already-ruled classes. A is a real output difference whose mechanism is pre-m55 code. It is admitted only under a narrow, signature-bound, capped rule, and it is reported on every use. None of these amendments touches an m55 measurement, a stack-leg predicate, a threshold, or a prediction.

## Ruling A — a coalesced-arm unmeasured frame is excused under a signature, symmetric and capped
A frame is **COALESCED-EXCUSED** only if **all** of the following hold.
1. **Signature in that leg's own log.**
   - One `M23 PASS … servedArms=k` line lists that frame's session-index target arm together with an earlier session index's target arm (k ≥ 2 target arms in one render).
   - It is followed by `TARGET MASK UNAVAILABLE for session_index <si> … pixels=0` for the same si.
2. **Differences are confined to that frame's unmeasured-derived set.** The frame and the leg may differ from the comparison leg only in:
   - `mask_state` (unmeasured);
   - `target_pixels` (−1);
   - `bbox_drawn_px` (null);
   - `target_mask_frames_measured` (−1 per excused frame);
   - that si's membership in the observable subset of `affected_frames`;
   - that frame's mask file, if written;
   - the field-set and key-presence lines that follow *only* from these.

   Everything else must pass the existing rules. That includes the frame's **colour** file, `injected_frames`, every other frame, and every other field.
3. **Symmetric.** The rule applies identically to pre-m55 and build-3 legs, because the mechanism (`PixelOwner` in `Drain_RenderThread`) is pre-m55 code.
4. **Cap.**
   - At most **1** excused frame per leg, and at most **1** leg with any excused frame per legacy group.
   - Beyond the cap the group is **NEEDS-DECISION**, never PASS or FAIL.
   - Across the rest of the requalification, a **third** build-3 leg with an excused frame while pre-m55 legs show none is also NEEDS-DECISION: a rising frequency would be an m55 effect.
5. **Reported.** Every excused frame is written to the ledger and comparison JSON as `coalesced_unmeasured_excused: [{leg, si, served_line, unavailable_line}]` and listed in the report.

**Pre-condition, bench-free, before the amendment is applied:**
- **Bank search.** Search every banked legacy leg since 081-11, pre-m55 and build 3, all groups, for the coalescing signature. Report the counts per binary and evidence state.
  - If build-3 legs show it on **≥ 3** legs while pre-m55 legs show it on **0** of a comparable number, stop with NEEDS-DECISION before applying anything.
- **Source proof.** Confirm by `git diff 031a103 85c99b3` on the coalescing/PixelOwner path that the only change is the `Issue.IsValid()`-guarded addition.

**Recorded as a known pre-existing yield issue (not m55 scope).** When a target mask render slips a frame, the next frame's arm coalesces into it and comes out unmeasured. The label stays honest (unmeasured → not observable), so it costs yield, not correctness. Log it as a gotcha and as a FUTURE item for m51/M3 (give each target arm its own pixel readback). Why the render slipped stays NOT ESTABLISHED.

## Ruling B — mask tags compare modulo one leg-wide bijection
`mask_value` and mask-file content compare equal **iff** a single tag map π exists for the leg such that all of these hold:
- π is one-to-one;
- π is applied to **every** frame and label of the leg;
- after remapping, every mask file is **pixel-identical**. Remap the pixels, then compare; a raw hash comparison is not enough;
- after remapping, every label's `mask_value` equals π(pre-m55 value);
- per-tag pixel counts, occupancy, `mask_ties` and the mask/count cross-check are equal;
- where the leg serialises a tag→actor (name or class) map, π preserves actor identity.

Report π for every leg where it is not the identity map. Legacy groups only; no other rule changes.

## Ruling C — the cross-pair clause honours the historical run-unique set
Rule 1's cross-pair clause excuses exactly the fields in `081-22-evidence/legacy-historical-run-unique.json`, the same set rule 3 already uses. The set must not change: hash-check it.

`census_cycles` is a timing-sampled diagnostic, out of identity scope. Tabulate it per leg (binary, evidence state) across all remaining groups as a **reading** for Stage 3. The ON = 27 association stays a reading, not a gate.

## Proof required (bench-free), in both directions
1. **A negatives.**
   - An unmeasured frame without the signature → FAIL.
   - The signature is present, but the colour differs → FAIL.
   - The signature is present, but a non-derived field differs → FAIL.
   - 2 excused frames in one leg → NEEDS-DECISION.
   - Excused frames in 2 legs of a group → NEEDS-DECISION.
   - The third-build-3-leg rule → NEEDS-DECISION.
2. **A positive.** Banked CB_N2 si 29 → COALESCED-EXCUSED, with the served/unavailable lines quoted.
3. **B negatives.**
   - A many-to-one map → FAIL.
   - A map that is inconsistent across frames → FAIL.
   - A valid π but one pixel differs after remap → FAIL.
   - A valid π but a per-tag count differs → FAIL.
   - A valid π that breaks actor identity (where serialised) → FAIL.
4. **B positive.** Banked CB_N5 → equal under the reported π.
5. **C.**
   - `census_cycles` differing across pairs → excused.
   - A field not in the set differing across pairs while the control pairs agree → still FAIL.
   - The set hash is unchanged.
6. **No verdict flips.** Re-run every previously evaluated legacy comparison (the 081-11/12/15/22 banks and c0) under the amended rules. Every earlier verdict must be unchanged, or the report must list and justify each one. An earlier PASS turning FAIL is a stop.
7. **CB-N re-evaluation from the bank** (081-26 mechanism, dated, citing this ruling) → **PASS-REEVALUATED**, listing the A excusal, the B π and the C excusal.
   - If it is not PASS under the amended rules, stop and report. Do not bend it.
8. **Boundary re-issue** (lib, window runner if touched, feature head) and a full dry run, all green. c1 must resume at `CB_S1`.

## Sequencing
- **081-32** is bench-free (xhigh): pre-conditions → AMENDMENT 5 (declared, dated, in the predictions file) → proofs → CB-N re-evaluation → boundary → docs (journal, gotchas: the A yield issue and the G294 resolution) → push.
- **081-33** is the bench run (high), released only if 081-32 is GREEN: c1 from `CB_S1` → c2 → c3.
