# REPORT for 094-27e-europa-a7-last-two-lean.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-27e-europa-a7-last-two-lean.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-31 07:32:42 | finished: 2026-10-31 07:58:52 | duration: 26 min | exit code: 0
context at last turn: 440541 tokens (44% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-27e-europa-a7-last-two-lean.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**For the owner:** All four Phase A anomalies now work on Europa's 4.25 engine: blinking, LOD corruption, lighting mismatch and missing object. Each one visibly applies and then cleanly reverts in its contact-sheet pictures. Codex ran the last three sessions (smoke, blinking, LOD) in 18 minutes this morning (07:37–07:55 IST); Phase A is done.

**Counted events per anomaly** (my own count from the probe rows agrees with the reader):

| Anomaly | Counted events |
|---|---|
| blinking | 6 (new session) |
| lod_corruption | 6 (new session; forcing model 1, the sphere, worked) |
| missing_object | 6 (094-27d re-read) |
| lighting_mismatch | 5 (094-27c); 6 complete, one invalidated under the older coverage rule |

**Your eye check:** in each sheet, the left column is before, the middle is during, and the right is after.
- **Index:** `D:\IA_Europa\Receipts\09427e\contact-sheet-index.json`
- **blinking:** `D:\IA_Europa\Receipts\09427e\runtime-attempts\A7_-blinking-positive\01\blinking-contact-sheet.png`. The right cube is gone in the middle column and back on the right.
- **lod_corruption:** `D:\IA_Europa\Receipts\09427e\runtime-attempts\A7_-lod_corruption-positive\01\lod_corruption-contact-sheet.png`. The centre cube becomes a sphere in the middle and is a cube again on the right.
- **lighting_mismatch:** `D:\IA_Europa\Receipts\09427c\runtime-attempts\A7_-lighting_mismatch-positive\01\lighting_mismatch-contact-sheet.png`. The middle goes dark, with grey cubes and hard shadows; the right matches the left.
- **missing_object:** `D:\IA_Europa\Receipts\09427d\rereads\missing_object\missing_object-contact-sheet.png`. The left cube and its shadow are gone in the middle and back on the right.

**Safety:** none.
- **Crashes:** none. All three sessions exited 0 and wrote no crash folders.
- **Upload signs:** none. The only log lines that look like uploads are the crash client printing its configured address at startup (it then exits without sending) and the command line carrying the upload-off switches.
- **Paris host:** Content, Config, the descriptor and the `.uproject` are hash-equal before and after.
- **`m51`:** unchanged (HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the same 15 untracked entries). `master`, `release/2026-10-05` and the 30 tags are untouched.
- **Commits:** Codex made three local commits on `feat/europa-port` (`4276ca2`, `f77d0fc`, `abb240b`); nothing was pushed or tagged.
- **Tools diff:** only the job ids, the window end, the 4 GiB start line and the 2-minute idle changed. One disclosed amendment: Codex's first dry run refused, so `f77d0fc` lets the shared validator accept FK1's exact memory lines for job 09427e only.

**NEEDS-DECISION items:** none.

Codex's report is `_reviews\094-27e-codex-report.md` and my review is `_reviews\094-27e-code-review.md`.
api-equivalent cost (not billed on Max): $7.75 | turns: 55 | running total across all briefs: $5,286.28
