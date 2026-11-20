# 093-05 — Chat ruling: J1 is QUALIFIED, with three closure items folded into J2's Part 0; J2 released

**Date:** 2026-10-14, about 04:30 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's resumed J1 session, attempt 3: 10,698 s, VERDICT GREEN, pushed `fix/followup-capture` at `1bf7054`.
- Claude Code's review: `_reviews\093-05-code-review.md` (29 OK · 1 GAP; observations A–D).
- Mailbox report 093-05: NEEDS-DECISION, 190 min, $12.24 API-equivalent.

## Verdict
**J1 is QUALIFIED.** Code recounted five legs byte-identically, in PIE and staged:
- every edge 0 frames off;
- every labelled frame with a measured mask;
- all the other can-fails rejecting;
- the cook corrected and archived with container hashes;
- no product change since `72f3eb3`;
- the predictions unchanged;
- staging, host and content restored;
- only `fix/followup-capture` pushed.

The push stands. The closure items below go into J2's Part 0.

## Rulings

### E1. The missing "wrong object" can-fail (predictions line 33)
It must run, but it does not block J2 from starting.
- **J2 Part 0** builds it on **copies** of the existing J1 UV captures: an event relabelled onto a different object, so that the visible change lies outside the claimed mask. The visibility checks must reject it, on a PIE capture and a staged capture.
- If they don't reject it, that is a checker defect: stop.
- The status-block sentence that says every can-fail ran is corrected in the same docs commit.

### E2. The ">12 pixels" rule is C2-only: confirmed
- The written predictions (lines 12 and 32) scope it to C2.
- The post-result discovery was disclosed.
- No AA-off result depends on it; it changes only the TSR diagnostics. Accepted.

### E3. Masked-alpha caveat: recorded as a known limitation
- **What happens:** the mask follows the event-state silhouette, so pixels that the corrupted alpha cuts away change *outside* the mask. In the reader, the ring changes more than the region, while edges stay 0 frames off.
- **Where it's recorded:** a gotcha and the readme limitations (J2 Part 0).
- **Future item, for the regular delivery line and not natural:** mask the union of the pre-event and event silhouettes for silhouette-changing corruptions, and add a gate for mask completeness against the changed area.

### E4. Staged proxy produced 3 events instead of 4
J2 Part 0 reads the refusal reason from the log and states it in one line. Three events meets the minimum.

### E5. J2 is released
- **The job:** the office-robust F1 on `fix/followup-capture`, per 093-02 P2, with the plan's §A3 boundaries 1–5.
- **Diagnostics and reader:**
  - the `capture_ownership` run_summary block;
  - `NotGameView` counted by sub-reason;
  - the compact office checkpoint I reader (P6).
- **Can-fails:**
  - J1 `E41ABF93` (guard present, WIP absent) on the rebuilt delayed-viewport harness;
  - release `218BA4C7` as the pre-F1 control.
- **Coverage:** PIE and staged, StackOBot and the Lyra worktree.
- **How it runs:** a new Codex implementer job (fresh session) under the D2 stop rule; Code reviews.

## Next
- After J2 qualifies at home, the owner runs **office checkpoint I** on the second office host (required) and the first (optional). Chat will send a short checklist; the reader prints one line per case.
- Then the natural branch `research/natural-causes` is created from the J2 head, and J3 starts.
