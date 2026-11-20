# REPORT for 084-05b-sync-harness.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-05b-sync-harness.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 00:34:16 | finished: 2026-09-29 01:47:52 | duration: 74 min | exit code: 0
context at last turn: 817881 tokens (82% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-05b-sync-harness.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner**
The label-sync test harness for tonight's bench is built. It is proven on saved captures in both directions: it passes correct labels and fails broken ones. On those captures, the fix lines labels up with the picture exactly when anti-aliasing is off. Codex's review of the new source came back RED, and it isn't clear the bench should still run tonight on that build. I haven't acted on the findings; they need your ruling.

**The harness: legs and gates**
- **Files:** `_reviews\084-06-*`, copied from 084-03 rather than edited. Everything runs on build `E9FF019A` with container `67EA1FE0`. The m53 set is restored in a `finally` block, and restore only refuses on our own processes.
- **Required legs (31):**
  - m52: B0, B3, B9 (anti-aliasing off), B0L, P1, plus **B5, the null, which I added** because the new threshold needs it.
  - B-CC: 7 legs per the 086-02 spec.
  - MASK55: one CB_GateLevel auto-pool leg of 1200 frames, aiming for at least 90 events.
  - B-REAL: 10 legs (two rocks, four anomaly types, two AA-off twins).
  - B-LOD: 4 legs.
  - B-CC-MW: an ON/null pair, **plus a second null I added** so there's a noise floor.
- **Optional:** B-LYRA (6 legs) and a Lyra camera-clipping pair. They can't run yet: the Lyra plugin is still at `caa68c6`, so those legs will record NOT-RUN until it's moved and rebuilt. The new Lyra launcher refuses to start if the PC is busy; it never kills anything (the old one killed editors by name).
- **Evaluator:**
  - **Edge-local thresholds:** each m52 leg is divided by its matched null at the same frame, with the lag found by cross-correlation (r = 0.9995 on B0_FIX). That cancels the scene's 12-second sharpness cycle.
  - **Transition-aware gate:** a leg fails on any unlabelled visible frame without the transition flag, any labelled frame that isn't visible outside the flag, or a flag on a leg without temporal AA.
  - **New rule I added:** a flagged tail must start fading on the first unlabelled frame. Without it, a label that ends early passes (G353).
  - Failures that depend on a censored edge are reported as ambiguous, not as failures.
  - The 086-01 checker runs on every leg as a second verdict, with transition rows kept out of its reference frames.
  - Also included: the MASK55 gate, the B-CC oracle, the B-CC-MW reading, and a reported (not gated) count of rows marked `anomaly_present` that no event labels.

**Proofs, both ways**
- **Offline:** 26/26 known-answer checks.
- **Real saved captures:** 55/55.
  - **AA off:** the exact label passes. Onset +2, offset −5, early over-label −2, late over-label +4, and a transition flag on an AA-off leg all fail.
  - **AA on, with flags at 3/8:** the build's label passes. Removed flags, misplaced flags, an off-window of 1, onset +2, an early end of −5 and a late over-label of +4 all fail. The early end is caught only by the new fade rule.
  - **What the gate can't see:** starting the label up to 2 frames early (up to 3 with the proposed K_on) looks identical to anti-aliasing lag. Only the AA-off legs test onset strictly.
  - **Scene-cycle null:** the old whole-leg threshold (0.27–0.29) judged 0 events on every leg. Tested on a null capture, the new one finds 0 of 18 fake events measurable. The flat threshold without a null term finds 7 of 20.
  - **Mask gate:** it fails the real 90-event capture from before the fix. That capture has 560 unmasked labelled frames from event 55 on, plus 5 with the G295 signature. On edited copies, a legitimate recycle passes; aliasing, one value on two events, a missing mask file, and no recycling at all each fail.

**The 084-03 bank, re-evaluated (at the 50 % rule)**

| legs | result |
|---|---|
| B9_FIX (AA off) | **0/0 on 16/16** |
| B9_BASE (AA off) | −1/+4 on 21/21 |
| B0_BASE, B3_BASE, B7_BASE | FAIL |
| AA-on fix legs as saved (no flags) | FAIL: onset +1 to +2, offset +3 to +6 |
| Same, with flags added offline at 2/8 or 3/8 | B0_FIX **16/16**, B7_FIX **16/16**, B3_FIX 3 pass + 5 censored |
| B0L_FIX (timing lever) | FAIL, as it should |

The 086-01 second verdict agrees in direction on every leg.

**Proposed transition values (for your ruling)**
- **`IAI.Label.TransitionOnFrames 3`:** measured onset lag is +2 at both the 95th and 100th percentile (40 events), plus 1 frame of margin.
- **`IAI.Label.TransitionOffFrames 8`:** measured tail is +6 at both percentiles (35 events). Four B3_FIX tails I couldn't certify reach +8, so I matched them.
- **`TransitionHideFrames` stays 1.**

**Dry run, time and invocation**
- **Dry run** (launches stubbed, using saved captures): exit 0, 0 preflight problems, 0 evaluator errors. All 39 launches were accepted, including the 8 Lyra ones, in 294 s.
- **Real preflight** just now: 0 problems.
- **Estimated time:** required legs about 50 minutes, about 80 with retries and waiting for an idle PC. The optional legs add about 30 minutes once Lyra is rebuilt.
- **084-06 invocation:** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-06-window.py live` (add `--with-optional` only after the Lyra rebuild).

**Codex: RED** (`_reviews\084-05b-codex-sync-source-review.md`, 1,465 s, ledger row added)
- **F1:** G350 is still false for live hide anomalies. A blinking burst's visible frames keep `anomaly_present` set.
- **F2:** camera_clipping misses scaled instanced meshes, because the engine's candidate query ignores instance scale and rotation. Tonight's B-CC fixture has no such mesh, so a pass can't rule this out.
- **F3:** the sync capture path writes a different event set from what the annotation counts. This is inherited, not new.
- **F4:** mask-ID retirement skips components with custom depth switched off that still hold the value, so a recycled ID can alias.
- **F5:** transition history is lost across a run boundary.
- The new levers are excluded from Shipping builds.

**Commits pushed**
- `b3ba294` on `fix/m52-label-timing` (`f51f124..b3ba294`, equal to origin): the journal `2026-09-29-084-05b-sync-harness.md`, G353, and the status block.
- No tag, no merge. m51 is still `53bf725`; staged set and master untouched.

**NEEDS-DECISION**
1. Given Codex RED, run 084-06 tonight on `E9FF019A` as is, or fix F2 (and possibly F1/F4) and rebuild first?
2. Adopt the transition values 3/8, or keep 2/8? K_on 3 widens the onset blind spot by one frame.
3. B-LYRA needs the Lyra plugin moved to `f51f124` and a LyraEditor rebuild. Authorise that for 084-06, or skip Lyra.
4. Accept the rules and legs I added: the null leg B5, the second null for B-CC-MW, the fade-at-label-end rule, and "ambiguous on a censored edge".

Several connectors (Gmail, GitHub and others) need authorising in their connector settings, and the PDF viewer failed to connect. None were used here.
api-equivalent cost (not billed on Max): $30.21 | turns: 159 | running total across all briefs: $3,228.94
