# REPORT for 084-04a-m52-evaluator-corrections.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-04a-m52-evaluator-corrections.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 17:37:59 | finished: 2026-09-28 18:48:12 | duration: 70 min | exit code: 0
context at last turn: 616638 tokens (62% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-04a-m52-evaluator-corrections.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. The evaluator is corrected, proven and pushed, and tonight's bench can run with the same command. The one open item is that this build cannot report the monitor's per-tick cost the ruling asked for; that doesn't block the bench.

**Plain summary for the owner:** The checker that grades tonight's bench now marks a result unmeasurable when frames are missing next to an edge, and fails any blurred frame outside the label. It also fails frames labelled without masks, and contaminations that weren't flagged and reverted. Each fix was tested both ways: it catches cases the old checker waved through, and it still passes a correct label on real recorded frames.

**Corrections (evaluator id `084-04a`, stamped in every report)**
- **(a) Gap censoring:** a missing frame or an engine-frame jump next to an edge, inside the 2-frame onset check, or inside the 3-frame clean suffix makes that edge CENSORED, never passed. A jump between two blurred frames inside the window censors nothing. A censored event still FAILS if it has a definite failure (FAIL > CENSORED > PASS).
- **(b) Visible outside the window:** every pixel-visible frame outside the window FAILS unless the paired null shows a visible drop at the same frame of the same event. G345's "reported, not required" is withdrawn. Where the threshold was calibrated from that same null, the excusal can't fire inside its spans.
- **(c) Masks:** every labelled frame needs a measured mask. No mask value, `target_pixels` −1, a missing PNG, or an empty mask on a blurred frame FAILS gate (ii), which now runs on every event.
- **(d) Contamination:** each `HOLD CONTAMINATED` must pass four checks, or gate (iii) is `FAIL-CONTAMINATED`. Every one is listed.
  - it is matched to the anomaly's own "REVERTING THE HOLD NOW" line;
  - its trail closed back at baseline;
  - `stuck_mip.contaminated = 1` is on every member frame from that frame index;
  - no flag appears anywhere else.
- **(e) Readings:** per-leg counts of trail reopens, ORDER HOLD, gap-unknowns, carried trails, contaminated events and unresolved/timeouts (including the 64-tick fallback), plus totals. Monitor cost is reported as not measured (see below).

**Proofs, both ways**
- **Selftest: 60 checks, 0 failures.**
  - The 084-02b cases still behave: the four can-fail cases fail and the exact window passes.
  - (a) 11 checks, (b) 7, (c) 6, (d) 15, (e) 4.
  - 6 `[before]` checks run a frozen byte copy of the 084-02b checker. It **passes all six**: Codex's four cases, a single spike, and a contaminated census. So each gap was real.
- **Real banked pixels: 15 of 15 checks proven, 0 contradicted,** on M52F event 577:
  - correct label and a jump inside the window PASS;
  - four label shifts FAIL;
  - two missing-mask variants fail gate (ii);
  - three gap variants are CENSORED;
  - a 2-frame recurrence FAILS, and the same recurrence shown by the null PASSES;
  - a single spike FAILS, and the real unedited spike at si 497 FAILS.
- ⚠ The pass half rests on this one event, with two declared edits:
  - the spike frame 497 re-pointed at its clean neighbour 496;
  - a copy of the session standing in as the null, because the real null has no matching event.
  - Under the corrected rules no unedited banked event can pass.
- **Bank re-evaluation** (16 banked m52 legs, 23 measurable static events), old → new: **CENSORED → FAIL on 20, FAIL → FAIL on 3, no PASS under either.** The old checker simply dropped censored events, which hid their unlabelled blurred frames.
- ⚠ **Observation, no cause established:** at event 577 all three M52F legs show a blur excursion starting at si 497, 10 frames before the main blur. In GD/GE it forms a 2-frame pair and becomes the pixel window, so the real blur counts as outside. Tonight's null will show whether the scene causes it.

**Dry run:** exit 0, preflight 0 problems. B3_BASE FAIL (4 judged, 0 censored), B0_BASE no judgeable event (camera moved), null PASS, doctored proof proven on event 577.

**084-03 invocation: unchanged.** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py live` (also `live --with-optional`, `gates`, `dry`). Tonight's verdicts come from the corrected checker.

**Commits pushed:** `b1eb088` on `fix/m52-label-timing` (`90dfa6f..b1eb088`, head equals origin): the journal note and G347 (a check that reads its evidence only when it exists passes on its absence), with a supersede marker on G345. No tag, no merge. The harness files themselves live outside the repo in `_reviews`. The main checkout is still `m51` `53bf725` and `master` is unchanged.

**Decisions for you (neither blocks tonight)**
1. **Monitor cost can't be measured on FD621B27.** The hold-monitor scan has no timer, its scan count isn't written out, and the 30 fps pacer absorbs any cost below budget. Tonight reports proxies: components walked per tick, and the Apply census time. I recommend adding a source timer in 084-04 and reading it on Tuesday's confirmation bench.
2. **A correctly labelled second blur run still fails.** Under the single-window rule, a later blur run that the plugin labels (for example an F2 reopen labelling a real recurrence) counts as over-labelled. Please rule on whether that should pass. It only matters if tonight shows one.

Several MCP connectors (Gmail, Calendar, GitHub and others) need authorising in their connector settings, and the PDF viewer server failed to connect. This task didn't use any of them.
api-equivalent cost (not billed on Max): $16.95 | turns: 96 | running total across all briefs: $3,095.33
