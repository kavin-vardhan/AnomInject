# REPORT for 094-30-europa-capture-port-plan.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-30-europa-capture-port-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-31 08:37:22 | finished: 2026-10-31 09:14:59 | duration: 38 min | exit code: 0
context at last turn: 450369 tokens (45% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-30-europa-capture-port-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

The capture-port plan is written and committed to `feat/europa-port` as 4f46a9c (docs only, local, not pushed). It has concrete gates and a C1 file list. Nothing protected or in the Paris host changed, and nothing was launched.

**Plain summary for the owner**
- Porting capture to 4.25 is about 7–12 days in three stages: get the plugin writing frames, then prove labels match the picture frame-for-frame, then add masks.
- 4.25 has the same building blocks under older interfaces, so the plugin needs small adapters for reading frames back, for the per-frame timing hook, and for the mask shaders.
- The biggest risk is that a label one frame off still passes. The label gate only runs at normal pacing, which is the condition that hid this bug before on 5.1, so I'm proposing two cheap additions to it.

**Stages and gates (Codex's estimates)**
- **C1, frames (2–3 days):** two sessions, one in the editor (PIE) on EA_Core and one standalone `-game` on EA_Travel. Each must write exactly 60 plugin PNGs at 1280×720 with no missing or duplicate indices, no black frames, and a correct check region; the run exits normally. Negative test: a padded or undersized readback must be refused.
- **C2, labels (2–4 days):** one session of up to 180 s, in both tick orders, with at least 3 apply/revert events per anomaly per order. Labels are compared against pixels at every apply, revert and blink edge. Gate: zero mismatched frames, and any ambiguous edge fails. Negative test: shift the exported labels by one frame and the same reader must fail.
- **C3, masks (3–5 days):** one session; the shaders must load, frames and masks must pair up, tags must be right and state must be restored. Proposed thresholds, not measured: IoU ≥0.90, recall ≥0.95, ≤1 % spill. Negative test: a one-frame mask shift must fail.

**Measured compile errors:** none. Codex didn't compile, because the enable switches are hard-coded in the build rules and the only ready target writes into the Paris host's bound outputs. Isolating that was more than the lean profile allows. My own checks agree no build ran: no build or engine process appeared in any poll, and the host and plugin Intermediate folders are unchanged. So the C1 file list comes from reading source, not from errors; the first C1 build will be the real measurement.

**Review results**
- **Spot-check:** I checked 7 of the "different/missing" rows (05, 06, 07, 12, 13, 15, 26) plus 10 and 25 against the 4.25 source. All are confirmed. The file writers and run log the plan keeps unchanged also compile-match on 4.25.
- **Paris host:** hash-equal before and after for Content (1,456 files), plugin Content, Config, host Binaries (16) and plugin Binaries (11), the descriptor and the `.uproject`.
- **m51:** matches P9 at start, prelaunch and after.
- **W4:** held. The child process saw neither date variable, and the commit carries real timestamps.

**Top risks**
- *Codex's:*
  1. Wrong row pitch, format or crop producing plausible-looking corrupted frames.
  2. Correct labels attached to the previous rendered frame.
  3. 5.1-only code still reachable once capture is switched on.
  4. Stencil ownership or internal/output resolution errors in masks.
  5. Treating a stock single-view pass as covering temporal AA or Europa's fork.
- *Mine:*
  1. **A one-frame label offset passes C2.** The negative test shifts every label at once, which the apply edges alone would catch; it never tests revert edges or the blink interior toggles, where P9 lived. And C2 never runs with pacing off, which is where the old arm→present pairing read −1. Fix: a per-edge-class version of the shift test, plus one `IAI.Capture.Pace 0` block in the same session.
  2. **D3D12 readback on Europa's fork.** `MapStagingSurface` can return 0×0 if it maps before the copy lands (an engine comment admits this). The home gate runs on D3D11 and never hits it, so the adapter must drop and count such frames, and the office smoke must record which renderer it used.
  3. **C3's single threshold can't be evaluated for two anomalies.** For lod_corruption the visible change isn't the target's silhouette, and lighting_mismatch's target is a light with no stencil mask. It needs a metric per anomaly before the thresholds are frozen.
- *Housekeeping:* the new gotcha is numbered G475, which is already used on `feat/vt-blur-stacks`; the next free number is G488. The commit is unpushed, so it can be renumbered in a follow-up before D-lite.

**NEEDS-DECISION:** none blocking. For chat to rule on when writing the implementation requests:
- whether to require my C2 additions and per-anomaly C3 metrics;
- that the C1 request must authorize replacing the Paris host's plugin binaries and re-arming the launch kit's binding manifest.

**One deviation:** before launch I rebuilt the resume prompt once, to correct clock times I had misstated in my added context. The first version was never sent.

Files are in `D:\IntrusiveAnomalies\_reviews\`:
- 094-30-codex-report.md
- 094-30-code-review.md

Receipts are in `code-receipts-094-30\` in the run folder.
api-equivalent cost (not billed on Max): $8.56 | turns: 64 | running total across all briefs: $5,307.07
