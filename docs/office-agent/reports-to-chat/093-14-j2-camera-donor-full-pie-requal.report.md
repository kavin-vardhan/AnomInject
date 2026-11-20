# REPORT for 093-14-j2-camera-donor-full-pie-requal.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 093-14-j2-camera-donor-full-pie-requal.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-16 03:03:31 | finished: 2026-10-16 07:25:06 | duration: 262 min | exit code: 0
context at last turn: 598995 tokens (60% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\093-14-j2-camera-donor-full-pie-requal.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner**
- The camera fix now passes on both games, and the whole in-editor test set (42 cases) passes again on the new build.
- The run stopped at the half-resolution box check. The boxes themselves are right; the script that proves the game really rendered at half size looks up the wrong frame numbers, and changing it needs chat's permission.
- Nothing was packaged or pushed. Your PC is back to how it was, except for about 21 GB of half-written archive left on drive E:.

**My environment and the attempt numbering**
- `GIT_AUTHOR_DATE` and `GIT_COMMITTER_DATE` are not set in my environment. I removed them for the child anyway, and the launch receipt shows the child saw neither.
- 093-13 was the 8th physical attempt; the resume script renamed its exit file to `exit-code-attempt7.txt`. 093-14 is the 9th, with `exit-code.txt` and ledger id `…-resume7`. No script was changed. The mapping is in `code-receipts-093-14\attempt-numbering.txt`.

**The Codex job**
- Ran 03:09:50 → 07:14:46 IST: 14,696 s, one try, exit 0, `VERDICT: NEEDS-DECISION`.
- Commits, all docs only (product stays `9781031`; `Source` and `Shaders` trees unchanged):
  - `741e8a1` at 03:14:27, the freeze;
  - `334d903` at 06:34:01, results and gotcha G487;
  - `0e50b56` at 06:48:07, archive routing.
- Timing: freeze 03:14:27 → StackOBot re-assessment 03:15:18 → first engine leg 03:23:50.
- Heads pushed: none. Remote `fix/followup-capture` is still `1bf7054`; local `0e50b56` is a fast-forward from it.
- Hashes: StackOBot EXE `d8d30951`, Lyra EXE `dfe6f164`. The 32 bank files match the manifest before and after (my rehash). Fixture DLLs: StackOBot `a7443041`, Lyra `017ca90b`.
- Harness fixes, all harness-only and one attempt each:
  - the camera donor (attempt 1 of 2);
  - the string audit had picked the wrong baseline;
  - a UTF-8 read in a reporting script;
  - archive routing, after the first archiver wrote through the `_binary_baselines` junction onto E:.
- The pairing recipe, marker and packaging-metadata budgets were not touched.

**The camera switch**
- **StackOBot, re-assessed retained leg:** my own drop-aware rebuild rejects. Codex's numbers equal mine to every digit.
  - Cut donor 56: X 0.92086 / Y 1.07784.
  - Blend donor 296: X 1.04466 / Y 0.96092.
  - n = 44 per axis; the honest windows read about 1.000.
  - The original "control unavailable" result is unchanged on disk.
- **Lyra, PIE (my recount):**
  - 990 requested = 984 written + 6 named drops (57–59 and 297–299, 3 per switch); 0 fatal lines.
  - 714/714 labelled rows have measured masks.
  - Moving frames: X 1.000098 / Y 0.999884, n = 44 each; the 0.96 mutants reject.
  - Onset and recovery edges: 0 on all four events, by my pixel check.
  - Stale controls reject: cut 0.938/1.059, blend 1.033/0.971.
  - Both active events continued through their switch.
  - 16 `extent_mismatch` onset pairs; summary counter 710.
- **Staged:** not run.

**The full PIE matrix and TSR half-resolution PF-2**

All 42 fresh cases ran on the new binary. My checker found all 64 legs loaded exactly the 9781031 DLLs, and 63 of 64 pass their own assessments; the exception is the TSR leg. The four archived-DLL controls stand unchanged. The rows I recounted myself:

| Case | Result |
|---|---|
| PF-1, Lyra | 360/360, 0 visible rows, 0 applications, admission 4/0 |
| PF-2, Stack Y 942×869 | 1.0000 / 1.0000 |
| PF-2, Lyra X 1123×870 | 1.00036 / 0.99877, n = 96, mutants reject |
| PF-3, Stack UV | 1152/1152, 0 `visible_positive` differences, box delta 0.0 px |
| PF-5 | fovDeg = 70 on 360/360 |
| Resize | 360 = 358 + 2 named drops; exposure exclusion 2,304 px is a log value only |
| Refusals | ambiguity 730, controller 1,067, stereo 728, split 366 (equal to the retained counts) |
| Admission probe, Lyra | 360/360, admission 0/1 |
| Pairing, Lyra native 50 % | 59/0/0; Z2 part 1: 81/81 lines half; part 2: uniform sizes; part 4: shifted copy 8/50/0 rejects; coverage 0.693; speed 1.835 |
| Pairing, other legs | 55–62/0/0 (from Codex's assessments) |

- **TSR half-resolution, StackOBot PIE:** box X 0.996109 / Y 0.994475, n = 96 per axis, mutants reject.
  - The setup proof fails because its reader assumes request id = sample + 1. Target masks are numbered by their own serial (serial 1 is sample 29).
  - All 294 mask passes are 640×360, and every one of the 96 labelled rows is served by one of them. The stop is correct.
- **TSR Lyra, and all of TSR staged:** not run.

**My review:** 30 OK · 0 GAP · 4 OPEN. The OPEN rows are Lyra TSR, the cook, the staged gates and the push, all left unrun by the stop. Observation A: my own 093-13 review accepted this setup-proof reader without testing it on a fixture where masks are armed only on labelled frames.

**Reader lines** (my extra runs on new 9781031 sessions; they are not the qualifying line, since J2 did not qualify):
- `I1.11 PASS v=4 c=0 b=1000 #0` (Codex's self-test session, reproduced)
- `I1.11 PASS v=4 c=0 b=999 #9` (StackOBot healthy)
- `I1.11 PASS v=4 c=1 b=999 #9` (Lyra healthy)

**Ready for office checkpoint I?** No. J2 is unqualified and nothing is pushed.

**NEEDS-DECISION**
1. **The TSR setup proof.** Authorize a reader for fixtures that arm masks only on labelled frames, keeping the estimator, n ≥ 20 per axis and the 1 % limits. One option is to map requests to samples through the leg's own `MASK-SERVED` receipts. Then Lyra TSR, the cook, every staged gate and the push.
2. **Disk space on E:.** `_binary_baselines` is a junction to E:, and E: is now at 51.6 GB free against Codex's 50 GiB floor. Codex's partial archive attempt (26,577 files, about 21.4 GB, marked `INCOMPLETE-ATTEMPT.txt`) is still on E:; its deletion was refused by the approval policy. Rule whether to delete it, or route new evidence to D:, before the cook.

The review is in `_reviews\093-14-code-review.md`; receipts are in `code-receipts-093-14\`.
api-equivalent cost (not billed on Max): $18.75 | turns: 134 | running total across all briefs: $4,606.89
