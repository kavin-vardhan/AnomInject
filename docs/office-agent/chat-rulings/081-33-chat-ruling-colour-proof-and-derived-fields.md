# 081-33 — Chat ruling on 081-32 NEEDS-DECISION: the colour negative was a chat spec defect; replaced by a colour-file presence condition on the excusal; the six derived fields confirmed

Date: 2026-09-26 (IST) · Author: chat Claude (orchestrator) · Branch head `ef2ffca` · lib `c36a60c2` (unpinned)

## Ruling 1 — proof 1's "colour differs → FAIL" was ill-formed (chat spec defect)
The 081-32 ruling said the excused frame's **colour** file must still pass "the existing rules" and asked for a "colour differs → FAIL" negative. But:
- no legacy rule compares colour pixels;
- colour frames are not byte-stable (CB_N1 vs CB_N6, both pre-m55, differ on 184,214 px at si 29).

That negative asked the harness to fail on something it has never compared. Code was right to stop instead of bending it. The defect is chat's.

**Replacement.** The excusal exists because the frame *was captured* and only its mask was lost. So it must prove the frame was captured. Add one more **condition to the excusal itself** (A.6). It is not a new global rule and not a threshold.
- A.6: for the excused si, the leg's **colour file exists** with the same **dimensions and pixel format** as the comparison leg's colour file for that si.
- If the file is missing or its metadata differs, the frame is **not** excused, and the group FAILs by the ordinary rules.
- No colour pixel comparison and no tolerance are introduced anywhere.

**Replacement proof 1 negatives (colour):**
- excused-frame colour file missing → FAIL;
- colour file with different dimensions → FAIL;
- colour file present and matching → excused.

Keep the other 8 of 9 negatives and the 5 symmetric cases exactly as proven.

## Ruling 2 — the six derived fields: confirmed
Code's reading is right. These follow arithmetically from one frame being unmeasured and are in the spirit of "lines that follow only from these":
- `target_drawn_pixels` −1;
- the event's `observable_frame_count` −1 and `unmeasured_frame_count` +1;
- run_summary `observable_frames` −1, `target_drawn_pixels_measured` −1, `target_mask_frames_unavailable` +1.

They are excused **only** at the exact value the writer would produce for that one frame, by recomputing as if the frame were unmeasured. Any other value → FAIL. Add one negative: one of these six is off by 2 → FAIL.

## Also accepted
- the group ledger rows (a recorded PASS / PASS-REEVALUATED group is skipped; a recorded FAIL / NEEDS-DECISION stops);
- the local ref fast-forward;
- the archive location under `081-32-evidence\`.

## Then
Re-run proof 1 (all cases), proof 6 (no flips) and proof 7. Commit CB-N PASS-REEVALUATED with `reevaluate-cb-n.py --commit`. Re-issue the boundary (lib, feature head), replay the preflight, and run the full dry run, all green. Mark AMENDMENT 5 as released, with this ruling cited for A.6 and the six fields. 081-34 (bench, high) then resumes c1 at `R23_CB_S1`.
