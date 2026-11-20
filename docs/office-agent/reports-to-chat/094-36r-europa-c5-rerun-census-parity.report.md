# REPORT for 094-36r-europa-c5-rerun-census-parity.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-36r-europa-c5-rerun-census-parity.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-01 13:11:50 | finished: 2026-11-01 16:15:23 | duration: 184 min | exit code: 0
context at last turn: 625546 tokens (63% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-36r-europa-c5-rerun-census-parity.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:**
- On 4.25 with a moving camera, our tagging no longer changes the picture. The flickering strip at the wall from last time is gone (0 changed pixels).
- Capture handles the full street scene: 141 objects drawn at once, the player's own view, custom depth left exactly as it was.
- But the labels don't yet match what's visible for any of the four anomalies on this real map: blinking leaves a fading trace after the object reappears, the lighting change ramps up and its mask covers 99 % of the frame, LOD swaps are invisible, and no missing_object event could be scored. Nothing is broken and no files were harmed. Codex also found and fixed (in source only) a frame-ordering bug, but its build budget was spent, so there was no rebuild or retry.

**C5** (one session, 15:11–15:23 IST, 6,336 frames, 0 dropped, exit 0; Codex committed `df5dfdc`, no push). My recompute matches Codex's numbers throughout.

- **Ordering bug (G500):** the first census-on frame at every pose ran with the census off. The first scored frame was at yaw 300 instead of pose 0. The fix (`05c55e2`) is unbuilt.
- **Interference, per pose:**

  | Pose | Stable share | Census-on max | Gate (≤ 0.001) |
  |---|---:|---:|---|
  | 0 | 33.6 % | 1,168 px (0.38 %) | fail |
  | 1 | 70.7 % | 1,830 px (0.28 %) | fail |
  | 2 | 85.4 % | 659 px (0.084 %) | pass, kept |
  | 3 | 52.3 % | 55 px | pass (dropped on the self-check) |
  | 4 | 77.5 % | 3 px | pass, kept |
  | 5 | 69.8 % | 780 px (0.12 %) | fail |

  - Poses 0, 1 and 5 also fail the census-off self-check.
  - My switch test points to scene drift, not the census: with one baseline across the census-off to census-on switch, the change count rises smoothly. The first census-on frame, before any actor is tagged, already sits at the census-on level, and the stable pose 4 shows no step.
  - The wall strip (rows 686–719) has 0 changed pixels on every census-on frame. Codex made no z-fighting claim.
  - By the frozen line, the gate still fails at three poses.
- **Per anomaly** (only poses 2 and 4 were scored):
  - **Blinking:** 14 fired, 9 scored. Hidden frames are perfect (IoU 1.000, precision 1.000, spill 0). But after each reveal the target area keeps a residual fading from about 8 % to 4.5 % over 3–4 frames, giving 13 mismatches and 41 ambiguous frames.
  - **Lighting:** 6 fired, 6 scored, 15 mismatches and 41 ambiguous. The change ramps from 0.039 to 0.20 over the labelled frames, and the mask covers whole receivers, so precision is at most 0.20 against the 0.80 line.
  - **LOD:** 12 fired, 10 scored, 130 mismatches. Labelled frames change only 0.004–0.011 % of the target.
  - **missing_object:** 4 fired, 0 scored; the targets are bushes and an awning, mostly unstable pixels.
  - At pose 2, all 192 clean scored frames exceed the clean line (drift since the reference was taken about 2.5 minutes earlier); pose 4 has 15 over.
  - Neither negative qualifies for all four anomalies.
  - FR3 passes: everything was frozen before the first scored frame was written.
- **Census:** 141 / 116 / 92 objects drawn at poses 0 / 1 / 5, so more than 55 at three poses. The ledger was never exhausted, and the reserved value 203 was never handed out. Some scored targets (e.g. ProvaStrada11_63, SM_Tronco_LP3) were in view at neither pose 0 nor the actual first view, but none has a clean C2 result, so the late-target gate isn't met.
- **Custom depth:** passes. All 1,980 components are equal before and after; all 6,336 mid-session samples follow the rule. The game's own custom-depth actors (`CasaEliminare_8`=203, `DefaultPawn_0`=18) were never tagged or changed.
- **View:** the player view equals the rendered view on all 6,336 frames.
- **Annotations:** 56 keys in 5.1's order, schema 2.1, and frame sets that agree with the rows on all 36 events. Two values differ from 5.1: an infinite far plane is written as `far: 0` (5.1 writes 1,000,000), and `coverage_pct` is −1 everywhere (fix `2b9dc19`, unbuilt).

**Safety:** none.
- No crash and no upload sign. Paris Content (including the street map), Config, descriptor and `.uproject` are hash-equal by my own hashes.
- `m51` equals P9 at start, prelaunch, exit and final.
- Binaries changed only during our builds (BelowNormal, 2 actions); the manifest is 27 of 27 equal.
- Foreign activity (a UE 5.7 <other-home-project> cook, ffmpeg, D: dips to 12.6 GiB) made the kit wait 57.6 minutes; recorded only.

**NEEDS-DECISION:**
1. **A build budget** to rebuild with `05c55e2` and `2b9dc19` and re-run C5. FM1's three Editor attempts are spent.
2. **Interference gate:** the frozen ≤ 0.001 line fails at poses 0, 1 and 5. The evidence points to slow scene drift, not the census. Needs a drift-proof design, for example interleaved census-off and census-on blocks.
3. **Scoring definitions for a real scene:**
   - references taken close to the scored hold;
   - how to treat the fade after a reveal and the ramp of a light change;
   - lighting masks as the receiver pixels that actually change, not whole receivers;
   - a visibility gate for LOD;
   - missing_object targets that avoid wind-animated foliage.
4. **Two 4.25 census gaps to note or fix:** it has no lost-batch timeout like 5.1's, so one lost readback would stall it; and an event adopts an existing census tag instead of asserting its own as 5.1 does.

Files are in `D:\IntrusiveAnomalies\_reviews`:
- 094-36-code-review.md
- 094-36-codex-report.md
api-equivalent cost (not billed on Max): $19.64 | turns: 137 | running total across all briefs: $5,416.08
