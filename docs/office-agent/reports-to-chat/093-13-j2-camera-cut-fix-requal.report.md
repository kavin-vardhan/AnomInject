# REPORT for 093-13-j2-camera-cut-fix-requal.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 093-13-j2-camera-cut-fix-requal.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-15 23:57:15 | finished: 2026-10-16 01:59:25 | duration: 122 min | exit code: 0
context at last turn: 546973 tokens (55% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\093-13-j2-camera-cut-fix-requal.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner**
- The crash is fixed. The new build gets through the camera cut and blend that crashed the old one, and its labels and masks line up.
- Qualification stopped anyway. A check in the test kit was written to need frame 299, but the camera switch always drops that frame by design, so that check can never run.
- Nothing was pushed or delivered, and your PC is back as it was. Chat needs to rule on that check (and on one more, below) before the next run.

**Environment and attempt numbering**
- `GIT_AUTHOR_DATE` and `GIT_COMMITTER_DATE` are not set in my environment. I removed them for the Codex child anyway, and the child saw neither.
- 093-12 was the 7th physical attempt; the resume script renamed its file to `exit-code-attempt6.txt`. 093-13 is the 8th, written as `exit-code.txt`, ledger id `…-resume6`. The mapping is in `code-receipts-093-13\attempt-numbering.txt`; no script was changed.

**The Codex job**
- Same thread `01a13778…`, one try, 00:03:42 → 01:46:22, **6,160 s**, exit 0, `VERDICT: NEEDS-DECISION`.
- Commits:
  - design note and predictions `ef03f36` at 00:11:01;
  - the fix `9781031` at 00:14:14;
  - docs `0bc6e9f` at 01:33:38.
- **Pushed: nothing.** The remote `fix/followup-capture` is still `1bf7054`; the local head `0bc6e9f` is a fast-forward from it.
- New candidate, built from `9781031`:

  | | Stack | Lyra |
  |---|---|---|
  | Game EXE | `d8d30951…` | `dfe6f164…` |
  | AnomalyCapture DLL | `a820c0d6…` | `53e7ec2e…` |

  I rehashed both banks myself: 32/32 match the new manifest.
- Archive: `followup-capture-J2-exposure-09313-9781031-D8D30951-DFE6F164-UNQUALIFIED`. My rehash: 4,019/4,019 match.
- Harness fixes: three, each on its first attempt, scratch only, with receipts. No gate changed and no spent budget was reopened:
  - a CPU test shim operator;
  - a missing `.uplugin` in the scratch bundle for the binary/lever audit;
  - the closing source-hash audit now compares the git blob instead of the CRLF checkout.

**The fix and the camera switch**
- **The diff:** two files, +17/−10.
  - One helper resets all five exposure-exclusion values, including the pixel count.
  - It is used at run start, at the owner change and at a shape change.
  - Allocation now happens whenever the array's real size differs from width × height.
  - Nothing else under `Source` or `Shaders` changed, and there is no new dependency.
- **CPU self-test:** I re-ran Codex's compiled binaries myself.
  - The old logic fails the same-size owner change with an out-of-range index, which is the crash.
  - The new logic passes all five cases.
  - The tested source equals the git blobs.
- **Builds:** all 8 at 0 errors and 0 warnings, and each one really recompiled the capture module.
- **Stack PIE camera reference:** no assertion. 990 requested = 984 written + 6 named drops (57–59 and 297–299). All 714 labelled rows have measured masks.
- **Stack PIE camera active leg:**
  - My independent pixel check finds onset and recovery error 0 on all 4 events.
  - PF-2 is ≈1.000 on both axes (n = 714); on the moving frames X is 1.00073 and Y 1.00013 (n = 44 each). The 0.96 mutants reject.
  - Both active events continued through the cut and the blend.
  - **The gate fails on one issue only:** the frozen reader hard-codes donor frame 299 for its stale-projection control (`camera_assess.py:160`, written in 093-07, before W2). Every switch drops the 3 frames just before it, so that control can never be built.
- Observation D: 16 `extent_mismatch` pairs on the reference, including 4 before the cut. The active leg shows the same 16.
- Lyra camera and every staged leg: not run.

**Identity proof and TSR half-resolution PF-2**
- Neither was run (`NR`, stopped).
- **Known answer bearing on the next ruling:** two runs of the same eff797c binary with the same UV spec already fail the frozen identity gate:
  - all 192 masks differ, because the tag values differ;
  - label clocks and IDs differ, and `exposure_dip` differs on 32 rows;
  - `end_frame` differs, and it is not on the allowed list.

  So that gate would stop even with no product change.

**My review:** 22 OK · 0 GAP · 5 OPEN. The OPEN rows are the work the stop left unrun: Lyra camera, the reach legs, the identity proof, the TSR PF-2 legs, and the cook with all staged gates. The full table is in `_reviews\093-13-code-review.md`.

**Reader lines:** none. J2 didn't qualify, so I ran no office-reader check on a new session.

**Ready for office checkpoint I?** No.

**NEEDS-DECISION**
1. **The camera control:** authorise a drop-aware donor (for example the last written frame before the blend, 296) or an alternative control. Without it, every camera leg fails: measured on Stack PIE, and the same is predicted for Lyra and staged.
2. **The identity comparator:** as frozen, it fails between two runs of the same binary. It needs a ruling before the identity legs run, for example a run-unique field set taken from a same-binary control pair, with `exposure_dip` handled explicitly.
3. **The resize expectation:** `ExposureExclusionPx` is only a log value, not a `run_summary` field. The expectation needs restating as a log reading, or a field needs authorising.
api-equivalent cost (not billed on Max): $14.91 | turns: 106 | running total across all briefs: $4,588.14
