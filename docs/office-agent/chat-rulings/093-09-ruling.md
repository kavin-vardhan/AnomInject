# 093-09 — Chat ruling: interrupted by a PC shutdown at about 04:29 IST; the gates run so far stand; continue from PF-3

**Date:** 2026-10-15, about 07:30 IST · **Author:** chat Claude (orchestrator)

**What happened:**
- At about 04:29 IST every shell in the mailbox session started returning exit code 66 with no output.
- Codex's process died without an exit code. Its last event was at 04:29:11.
- Code's session ended with no final message, so the watcher derived RED ("session killed / empty final message").
- The PC went down: a restart, shutdown or power loss; the cause isn't established. The owner switched it back on and restarted the watcher at 07:09.
- **There is no Code review of 093-09.** The evidence below is Codex's own event stream, read by chat. Code reviews it in 093-10.

## Progress before the shutdown (Codex's stream; not yet reviewed)
- **W4:** neither date variable was set. The three commits were re-created with current dates; old and new trees were verified equal, and a git bundle was preserved.
- **W1:** `fovDeg` is back on `GetFOVAngle()` (`3dc0c98`). The predictions were re-frozen before any leg.
- **Candidate `eff797c`:** StackOBot EXE `57F83797`, Lyra EXE `C6D74FED`. All 8 builds pass with 0 errors and 0 warnings, and the binary and string audits pass.
- **PF-1: PASS.** The exact off-screen arrangement gives:
  - 360 written frames;
  - 0 rows claiming the target visible;
  - 0 applied fires;
  - 4 counted `targeted_refused_off_screen`.
  - The retained 093-06 and 093-07 sessions are rejected, including by the new generic apply-receipt check.
- **PF-2: 12 of 12 PIE cases PASS** (both fixtures, X and Y axis constraints, three sizes). Box scale reads 0.999–1.000, with 4 visible events, every labelled frame with a measured mask, and 0 edge error in each case. Every scale mutant is rejected.
  - **Harness repair:** the launcher ignored the spec's `marker: false` and drew the animated barcode over the target. One case failed on that; the failure is retained. After the repair, the clean-frame difference dropped from about 12.8 to 0.33, and all six StackOBot cases were repeated with unchanged thresholds.
- **The 0.813 can-fail is reproduced:** the archived `f608d56` Lyra DLLs read box scale 0.813005 on both axes (`b=813`) and are rejected. The candidate DLLs were restored by hash.
- **The office reader's healthy PASS line:** `I1.11 PASS v=4 c=0 b=1000 #0`, the same on Python 3.10 and 3.13. Eight reader mutations are rejected, and the contact sheet shows 20 game-view frames.
- **PF-3 is unfinished.** The UV capture passed its own checks: 1,152 of 1,152 frames, 4 events, 0 edge error, measured masks. The comparison against the retained baseline then failed on a **harness setup mismatch**: Codex forced the old picture size (1123×870) as the viewport, where the retained run used 1280×992, which gave a 3.16 px box difference. Codex diagnosed it and was about to repeat PF-3 with the original viewport setup when the PC went down.
- **Other harness fixes, disclosed:**
  - an empty GPU-counter sample now reads "not clear";
  - the runner closes its own editor PID after capture, so a finished PIE run no longer resets the input-idle clock.

## Rulings

### X1. The gates already run stand for candidate `eff797c`, under three conditions
1. The candidate binaries rehash identical after the crash.
2. Code's review confirms them.
3. The marker repair and the PF-3 viewport diagnosis are harness issues under D2, with thresholds unchanged.

The PF-3 repeat with the original 1280×992 viewport setup is the first attempt for that issue.

### X2. 093-10: recover the bench, then continue from PF-3
1. **Post-crash check, first:**
   - no stray editor, Codex or build process from the dead run;
   - hosts, staging and content against the receipts;
   - restore anything left mid-leg by receipt;
   - the worktree head and status;
   - rehash the candidate binaries against the `eff797c` manifest. A mismatch means rebuild, and every gate re-runs.
2. **Then, in order:**
   1. the PF-3 repeat;
   2. PF-4 and PF-5;
   3. the full J2 matrix on both fixtures;
   4. the camera switch;
   5. the cook;
   6. every staged gate;
   7. the push, only when everything is green.
3. **The dead attempt has no exit code.** Never fabricate one. If the resume script refuses because of that, Code launches a new implementer job with the full context instead. Nobody edits the relay scripts.

### X3. Review
Code reviews **both** the 093-09 portion (everything above) and the 093-10 work after Codex exits, under W3.

### X4. For the owner
The PC went down at about 04:29. If Windows restarted it by itself (for example, for an update), that will keep killing overnight runs. How to handle it is the owner's call. Nobody here changes his system settings.

## Next
- **093-10 is queued.**
- **Unchanged:**
  - after J2 qualifies at home, office checkpoint I, then `research/natural-causes` and J3;
  - still open from K8: the `AspectRatioAxisConstraint` read on both office hosts.
