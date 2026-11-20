# NAME: 079 verifier field validity
# MODEL: claude-opus-5
# EFFORT: xhigh
# ACK-VERDICT: 079-02 NEEDS-DECISION resolved by chat 2026-09-15 — F-A (ring subtraction) is WITHDRAWN as refuted by P2/P3/P4; F-B..F-H stand; M50L_LG9 is not a known-good session (observability unmeasured, bbox projected); the known-good set becomes the masked Lyra sessions with mask-derived onsets as ground truth. Nothing merges yet.

# 079-03 — raw-d gate + camera-motion envelope set by mask ground truth. Resume session f7ac5221 (52%).

## 0. Rulings
1. **F-A withdrawn.** The onset/end/visibility statistic goes back to raw `d(region)` exactly as on master. The ring is no longer subtracted from anything. `mlo:92-93`'s claim is corrected in a dated note in the tool header (ring subtraction does not cancel parallax motion — measured 079-02).
2. **Motion estimate = global, not ring.** `M_med` := median over clean frames of the whole-frame changed-pixel fraction (downsample for speed if needed; state the factor). The ring is dropped from the motion estimate too — it is clamped at frame edges (079-02 reason 1) and adds nothing over a global read. F-C (region covers ≥ 90 % of frame ⇒ NOT-MEASURABLE) stays, now phrased on region size alone.
3. **Motion escape (replaces F-A):** if `M_med > MOTION_CAP` the event reads `NOT-MEASURABLE(camera motion: M_med=… cap=…)`. `MOTION_CAP` is NOT chosen by taste — it is SET BY §2's known-answer cross-check and written into the header with its provenance.
4. **Code's rec 2 REJECTED as a verdict rule:** `observability_measured:false` / `bbox_source:"projected"` do NOT make an event NOT-MEASURABLE — the gate's job is to check labels against pixels regardless of what the producer claims, and the whole M2 field case is projected bboxes. ACCEPTED as an annotation: print `bbox=<source> obs=<measured|unmeasured|n/a>` on each event line when the keys exist (v1 sessions print `n/a`).
5. F-B (unsatisfiable threshold ⇒ NOT-MEASURABLE, assert never clamp), F-C, F-D, F-E, F-F, F-G, F-H all stand as built. F-H's synthetic cases are re-based on raw `d`; add one heavy-motion synthetic case (scroll fast enough that M_med exceeds the cap) whose known answer is `NOT-MEASURABLE(camera motion)`.

## 1. The known-good real sessions — mask-derived ground truth
`A1L_LEGA` (strong motion, masks) and `LYRA_SMOKE_01` (moderate, masks), plus `A2L_LEGA` (light, masks). Their `labels.jsonl` rows carry per-frame mask evidence (`target_pixels` / `observable` — m49 A1/A2 output; confirm the exact keys in the rows). For each event, derive the MASK ONSET = first frame in the event's span with `observable == true` (or `target_pixels > 0` for texture-class — use the class rule the producer uses; cite it), and the MASK END likewise. That is ground truth the gate never sees (the gate reads frames, not masks). If a session's rows lack these keys, say so and drop it from the set — do not substitute.

## 2. Predeclared readings — AMENDMENT 2 in `docs/predictions/2026-09-15-verifier-field-validity.md`, written BEFORE running
P1 Selftest: all existing cases + the heavy-motion case ⇒ `SELFTEST: OK`.
P2 Pinned bench `M49_GEDGE_*` ×6: **byte-identical verdict lines to 079-01's run** apart from the new annotation columns — `tau=0.0040`, PASS 4/4 on all six including `MT_NAT`/`MT_SYN`. (This is the inertness proof F-A failed.)
P3 Cross-check on the masked Lyra set: for every event where the gate returns PASS or a SHIFT, the gate's onset must equal the MASK ONSET (and its end the MASK END) within 0 frames — tabulate per event: `M_med | gate verdict | gate onset | mask onset | agree?`. Events the gate reads NOT-MEASURABLE are listed with the reason. Predeclare: on `A2L_LEGA` all decided events agree; on `LYRA_SMOKE_01` all decided events agree; on `A1L_LEGA` state your prediction from 079-01/079-02's numbers (it read 4/6 PASS then +1 SHIFT — the SHIFT is either a real label defect confirmed by the mask, or a gate error under motion; the mask decides which).
P4 **Setting the cap:** `MOTION_CAP` = the largest `M_med` among sessions where P3 shows FULL agreement on decided events, rounded down to two significant figures, with at least a 2× margin below the first session that shows a disagreement. If every masked session agrees, the cap is set just above the highest `M_med` observed (state it) and flagged `provisional — no disagreeing session observed`. If disagreement appears already at `LYRA_SMOKE_01`'s level, the cap lands between A2L and SMOKE and that is the honest envelope.
P5 `M50L_LG9` (M_med 0.2649): every event `NOT-MEASURABLE(camera motion)` unless the cap lands above 0.26 — zero NOT-VISIBLE, zero SHIFT.
P6 Batch mode over the bank: counts identical to individual runs.
P7 The 079-02 P2 regression (`MT_NAT` NOT-VISIBLE 4) is gone.
If P2 or P7 fails, STOP — NEEDS-DECISION with numbers. No constant tuning outside the P4 procedure.

## 3. Execution
1. AMENDMENT 2 (docs commit).
2. Revert F-A in `verify_capture.py`; implement §0.2–0.5; `measure_label_offset.py` byte-unchanged.
3. Derive the mask ground truth (§1) with a small script under `CaptureBench/tools/` (new file is fine — it is a checker, not a fixture; name it, hash it in the journal).
4. Run P1 → P2 → P3 → P4 (set the cap, rebuild header) → P5 → P6 → P7. Bank everything under `M079_VERIFIER_EVIDENCE\` (new subfolder for 079-03).
5. Docs: tool header (envelope with the cap's provenance; the corrected mlo note; constants); `docs/client-readme.md` verifier section — plain words: under heavy camera motion the tool reports NOT-MEASURABLE rather than a verdict; M3 datasets carry per-frame visibility from the render, which the tool uses when present; journal `2026-09-15-079-03-…`; gotchas: the F-A lesson (a spatial ring does not cancel parallax; anomalies bleed outside their bbox — A35) as a G-number.
6. Commits on `fix/verifier-field-validity`, pushed, subject no longer "DO NOT MERGE" if P1–P7 read as predeclared. NO merge — chat reads, then Codex reviews at the merge gate.
7. Report per-P with exact lines, the P3 table in full, the cap and its provenance, and `VERDICT: GREEN` only if all seven read as predeclared.
