# REPORT for 093-12b-j2-tsr-half-res-proof-continue.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 093-12b-j2-tsr-half-res-proof-continue.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-15 20:37:22 | finished: 2026-10-15 23:18:32 | duration: 161 min | exit code: 0
context at last turn: 547150 tokens (55% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\093-12b-j2-tsr-half-res-proof-continue.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner**
- The 50 % proof under TSR passed on both test games, and none of the 16 pairing runs showed a frame slip.
- A camera cut crashed the editor. The fault is in this branch's new code, not in anything already shipped or pushed.
- Nothing was pushed or cooked, your PC is back as it was, and chat needs to rule on a code fix before anything continues.

**My environment and the attempt numbering**
- `GIT_AUTHOR_DATE` and `GIT_COMMITTER_DATE` are not set in my environment. I removed them for the child anyway, and the launch receipt confirms the child saw neither.
- 093-12 is the 7th physical attempt in this run folder. The resume script renamed 093-11's exit file to `exit-code-attempt5.txt` (it was the 6th attempt), and the ledger id is `…-resume5`. The mapping is in `code-receipts-093-12\attempt-numbering.txt`.
- The mailbox briefs 093-12 and 093-12R never started a Codex job, so they are not counted.

**The Codex job**
- Same thread, one try, 20:42:01 → 23:08:09 IST (8,768 s), exit 0, `VERDICT: NEEDS-DECISION`.
- Local commits:
  - `cdef048`, the Z2 freeze, committed 20:47:54; the first fresh run launched at 20:57:18.62.
  - `7049614`, the closing docs.
- **Nothing pushed.** The remote `fix/followup-capture` stays at `1bf7054`.
- The candidate `eff797c` rehashes 16/16 against its manifest in all three banks, before and after the job.
- The run is archived as `followup-capture-J2-tsr-09312-eff797c-57F83797-C6D74FED-UNQUALIFIED`.
- Two harness fixes, one attempt each, both in scratch scripts with no gate change:
  - the Z2 wrapper had wrongly required masks on unlabelled rows; fixed before the first run;
  - a text-encoding error when writing the admission check's log.
- `tbb.pdb` was reinstalled with a hash already in the original install receipts. It is not a third packaging attempt.

**The 50 % proof (fresh StackOBot native 50 % run, my own checks)**
1. **Reduced render:** all 80 mask-pass log lines show a 640×496 internal render inside 1280×992, one per captured frame. The 100 % run's log fails this check (0 of 81 lines), as it should.
2. **One size throughout:** all 80 pictures and 68 masks are 1280×992 and match every label row. Changing one row's width makes the check fail.
3. **Box agreement:** X n = 0, Y n = 0. Both axes are readings only, so there is no gating axis and the 0.96 mutant test cannot apply. All 18 labelled boxes touch the left picture edge, so the frozen estimator excludes them. This is the same on every 50 % run, so box agreement at 50 % under TSR is still unmeasured.
4. **Pairing:** 56/0/0 (CURRENT/PREVIOUS/NEITHER). The shifted-mask copy reads 7/49/0 and is rejected.

The stopped 093-11 run, as a reading only: parts 1 and 2 pass, part 3 is n = 0, part 4 is 56/0/0.

**My review: 29 OK · 0 GAP · 1 OPEN**
- My reader matches Codex exactly on all six new pairing runs, originals and shifted copies.
- The exact J1 delayed control writes 0 of 360, with the J1 archive's DLLs loaded.
- The pre-F1 control writes 360 frames, all from the wrong view.
- The StackOBot admission probe is 360/360 with 4 events, refusal counters 0/1.
- I confirmed the crash myself:
  - I mapped the crash address in the exact candidate DLL to `FoldExposureExclusion`.
  - In the source, an owner change at `AnomalyCaptureSubsystem.cpp:4731-4733` empties the exclusion mask but keeps its width and height. At the same size, `:1486` skips reallocating and `:1524` reads the empty array.
  - That code came in with commit `e13ddc52` (J2) and is absent from J1 (`1bf7054`) and the release.
  - Resize changes the picture size and reallocates, which is why the resize runs passed.
- What the crashed run left:
  - 57 frames, all before the cut;
  - 3 dropped samples (57–59), each with a named reason;
  - the active event ended by the epoch reset, with no normal end.
- OPEN: the remaining camera runs, the cook and every packaged-build check, which the stop left unrun.

**Reader lines:** none. J2 did not qualify, so I ran no office reader.

**Ready for office checkpoint I?** No.

**NEEDS-DECISION**
1. Whether to allow a product fix for the exposure-exclusion reset on an owner change, then re-run the camera runs, the cook and the packaged-build checks. The stale `ExposureExclusionPx` count after the reset belongs with that fix.
2. Whether Z2's box-agreement part should be allowed to pass with zero usable rows. The pairing setup cannot produce usable rows at 1280×992 (observation A in the review).
3. For the owner: several of your apps (PowerToys, WhatsApp) closed during the run. Nothing in the job could have closed them; I'm noting it only for your awareness.

The full review is in `_reviews\093-12-code-review.md`.
api-equivalent cost (not billed on Max): $14.09 | turns: 99 | running total across all briefs: $4,573.23
