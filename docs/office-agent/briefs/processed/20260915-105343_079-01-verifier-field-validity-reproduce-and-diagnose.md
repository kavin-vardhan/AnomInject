# SESSION: new-default
# NAME: 079 verifier field validity
# MODEL: claude-opus-5
# EFFORT: xhigh
# ACK-VERDICT: last_verdict RED came from SRD-005 (the SRD video project run through this mailbox, killed session) - unrelated to AnomalyInjector, holds nothing here. 078-17 NEEDS-DECISION acknowledged: the identity/lifecycle design (1f61196, m51) is NOT accepted and stays held; nothing in it is released by this brief. This brief opens a SEPARATE unit (verifier field validity) that touches no m51 source and no gate.

# 079-01 — label-pixel verifier: reproduce the field misfire at home, diagnose, propose the fix. NO FIX IN THIS BRIEF.

## 0. Orchestration change (record it)
From 2026-09-15 chat Claude is the orchestrator of this mailbox again; the Codex pilot (078-01..078-17) is over. Codex is used as an independent reviewer at crucial checkpoints only (chat names them). One orchestrator per mailbox, unchanged. Append a dated entry to `docs/OPERATING-CONTRACT.md` saying exactly this, and update the `_mailbox/README.md` "Orchestrator" line to match. Start a NEW default Code session (the 078 session is docs-only context at 60%; this is a different unit).

## 1. The finding (owner, office box, 2026-09-09 — readings only, office data never comes home)
`tools/verify_capture.py --label-pixel-gate --report-only` was run on M2-build Concorde captures (schema v1, delivery mode, `labels.jsonl` present, NO masks). It marked almost every event **NOT-VISIBLE or NOT-MEASURABLE**. The owner checked those labels by eye against the frames: **they are correct.** So on field data the gate produces FALSE FAILS (NOT-VISIBLE on a correct label) and mass unread events. The selftest passed. That proves the gate CAN fail; nothing ever proved it PASSES a known-good real session — the missing half of G96.

Consequences already ruled (record in the journal, act on nothing else): the verifier does NOT go to the client for the M2 re-review until fixed and proven on a known-good real session; card Section G readings on field sessions are void until then; this unit goes ahead of the identity work.

The owner's transcribed readings (header block, sample NOT-VISIBLE lines with tau/base, NOT-MEASURABLE reasons, camera-moving yes/no) have NOT arrived yet — treat them as pending; do not wait for them, reproduce instead.

## 2. Hypotheses to rank by measurement (do not fix blind)
- **H1 moving camera.** The gate scores a frame as "changed" when the count of pixels in the bbox that differ from the previous frame exceeds `tau`, and `tau` is learned from clean frames. With a gameplay camera every bbox pixel changes every frame ⇒ `base` is enormous, `tau` above anything the anomaly adds ⇒ onset never detected ⇒ NOT-VISIBLE. Bench sessions have a pinned camera (B1 pose gate), which is exactly why this never showed.
- **H2 dense auto-injection.** Overlapping/adjacent events leave no clean baseline window ⇒ guard relaxed / contaminated / "overlaps another event" ⇒ NOT-MEASURABLE, and the measurable range shrinks.
- **H3 oversized v1 bboxes** (G124 class: actor bounds, spline/instanced meshes) dilute the region so the anomaly's change is a small fraction of the box.
- Prior art to cite: the crisis-weekend handoff recorded "delivery-mode captures cannot self-verify offsets (FULLFRAME degrades to wrong answers; verification is always delivery-OFF)". That was `measure_label_offset.py`'s fullframe mode; the m49 gate inherited the same envelope without stating it. Find that note in the journals and link it.

## 3. Deliverables of THIS brief (plan first — file-by-file plan, wait for approval, then execute steps 3.1–3.4)
3.1 **Read** `tools/verify_capture.py` (`label_pixel_gate`, baseline/guard logic, `_gap_guess`, verdict rules) and `tools/measure_label_offset.py` (`read_labels`, `FrameCache`, region diff). Write down, in the journal, the exact formula for `base`, `tau`, "changed", the baseline window selection, and every path that yields NOT-VISIBLE and each NOT-MEASURABLE reason. No paraphrase — cite line numbers.

3.2 **Predict, then reproduce at home** on StackOBot, delivery mode ON (`bDeliveryModeDefault` / `IAI.Capture.Delivery 1`), masks OFF (M2 conditions), auto-injection with the DEFAULT pool at the client's density, ~600 frames at 30 fps, 1280x720, and the camera MOVING continuously like gameplay. Use an existing harness lever for the motion if one exists (player-input replay, console orbit/fly command, `ExtraExecCmds`); if none exists, add the smallest bench-only lever (a console command that orbits/translates the pawn camera at a fixed rate) and state it. Capture ONE pinned-camera control with the same recipe. Before running, write the predicted reading for each: moving ⇒ mass NOT-VISIBLE/NOT-MEASURABLE; pinned ⇒ mostly PASS. Then run the gate `--report-only` on both. Owner-eye verification substitute at home: for 5 events in the moving session, dump the frames at start-1, start, start+1 with the bbox drawn (existing overlay tool) into the evidence dir and state whether the label is visibly correct — that is the ground truth the reading is judged against.
   Predeclared outcome: if the moving session reproduces the misfire on labels that are visibly correct, the defect is OURS and bench-measurable; if it does NOT reproduce, say so and stop — the next step is then different and chat decides it.

3.3 **Instrument, don't guess.** A `--debug-events` (or similar, name it) output that prints per event: `base`, `tau`, the per-frame changed-pixel count series across the window ±edge, the baseline frames used and why, and a camera-motion estimate per frame (global whole-frame changed-pixel fraction, AND the `view.rot`/`view.origin` delta from `labels.jsonl` when present). Run it on both sessions. Rank H1/H2/H3 from the numbers.

3.4 **Known-answer regression, current gate:** run the current gate on the four historical bench PASS sessions (the m49 gate passes; find them in the bank/journals) and record that they still PASS — that is the baseline the fix must preserve.

3.5 **Write the fix proposal** (design only, in `docs/predictions/2026-09-15-verifier-field-validity.md`), against these ACCEPTANCE RULES (chat's, fixed):
   a. The gate must NEVER return NOT-VISIBLE for a region it cannot judge. When camera motion or baseline contamination makes the read invalid, the verdict is `NOT-MEASURABLE(<reason with numbers>)`, never a FAIL.
   b. It must actually judge moving-camera sessions to a usable degree, or it is not a client tool. Candidate directions to measure, not assume: normalise bbox change against a surrounding ring / whole-frame change (anomaly = LOCAL excess over global motion); use the view deltas to gate or compensate; second-order (change-of-change) onset detection. Propose ONE primary, with the reasoning from 3.3.
   c. Dense schedules: per-event baseline from the nearest clean frames of THAT region with an explicit floor; below the floor ⇒ NOT-MEASURABLE with the count printed.
   d. Selftest gains the complementary half: a synthetic MOVING-camera aligned session must PASS; a moving-camera shifted session must still read ONSET-SHIFT with the right sign; a moving-camera region with NO anomaly must read NOT-MEASURABLE or PASS, never NOT-VISIBLE.
   Plus the validity evidence the fix brief will have to produce: selftest incl. new cases; the home moving session flips from mass NOT-VISIBLE to PASS on the eye-verified events with NO label changes; the four historical passes still PASS; the owner re-runs on the office box and transcribes summary lines (mostly PASS, remaining NOT-MEASURABLE with printed reasons). Predeclare all four.

## 4. Rules
- Branch `fix/verifier-field-validity` off `master` (`ac13700`). Tool + docs only. `m51` untouched; `Source/`, `Shaders/` untouched anywhere. No merge, no push of anything but this branch, no tag.
- Predictions before results, house style. Evidence dir under CaptureBench per the usual layout; frames from the home bench are fine to keep — this is StackOBot, not office data.
- Journal `docs/sessions/2026-09-15-079-01-verifier-field-validity.md`; gotcha: "a gate proven to fail is half-proven — it must also pass a known-good real session".
- Report: plan (wait for approval) → then measurements, ranking, the fix proposal, and the exact commands. End with `VERDICT: GREEN` if 3.2 reproduced and the proposal is written; `VERDICT: NEEDS-DECISION` if it did not reproduce or a lever had to be invented; RED only on breakage.
