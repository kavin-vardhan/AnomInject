# REPORT for 093-06-j2-office-robust-f1.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 093-06-j2-office-robust-f1.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-14 04:20:27 | finished: 2026-10-14 10:35:03 | duration: 375 min | exit code: 0
context at last turn: 586125 tokens (59% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\093-06-j2-office-robust-f1.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner**
1. The "capture the right game view" fix (J2) is built and passed 22 of its home tests, but one test on Lyra failed, so nothing was pushed.
2. The failure comes from older code, not this fix: on games set up like Lyra, the box drawn around an anomaly can be placed in the wrong spot, and in a narrow window it can claim an object is on screen when it isn't.
3. Chat needs to decide whether to fix that box placement first. Office checkpoint I waits until then.

**The Codex job**
- **Run:** `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\`, a fresh session on `gpt-6-astra` / `max` with full access.
- **Duration:** 21,241 s (5 h 54 min), exit 0. It ended with its own `VERDICT: NEEDS-DECISION`, so no resume was needed.
- **Heads:** nothing pushed; remote `fix/followup-capture` is still `1bf7054`. Local commits: `59b2fa1` (Part 0), `e13ddc5`, `744b36d`, `33106f5`, `f608d56` (product, frozen there) and `bb9e741` (docs).
- **Archive:** `followup-capture-J2-09306-f608d56-4BB0F074-B0F2B30C-UNQUALIFIED`, 470 files, manifest SHA-256 `507e2aef…`. StackOBot exe `4BB0F074`, Lyra exe `B0F2B30C`.
- **Results:** 22 planned cases passed, 1 failed, 61 were not run after the stop (that includes every staged leg and the Lyra controls).
- **Harness fixes:** 11, all disclosed, each within two attempts, with the failed evidence kept. They covered resize in PIE, the stereo enum, a stale helper object, the second-viewport world context, the Lyra world-stamp placement, and the string-scan and mutant-summary bookkeeping.

**Part 0**
- **Wrong-object can-fail:** I re-ran the unchanged J1 checks myself. In PIE and staged, the original passes (exit 0/0) and the wrong-object copy is rejected (exit 1/1, not visible). The colour frames are unchanged (28 of 28 sampled are identical).
- **Status-block sentence:** corrected in `59b2fa1`.
- **Masked-alpha limitation:** recorded in gotcha G479 and the readme's known limitations, with the future item.
- **3 events instead of 4:** the first attempt in both staged proxy runs was refused as `texture_uniform:pending` (`F1_S_proxy/game.log:4936`, `Proxy_S_aaoff_marker0/game.log:4933`).

**My review: 30 OK, 2 GAP** (`_reviews\093-06-code-review.md`). The unrun matrix is listed as open, and builds and audits were read but not re-run.
- **Recounts match:** delayed viewport, new window with a second editor view, and the J1 UV regression are all byte-identical to Codex's reader output. Each has 4 of 4 events consistent, every edge 0 frames off, and every labelled frame with a measured mask.
- **Can-fails really failed:** the exact J1 `E41ABF93` DLLs wrote 0 of 360 frames on the delayed viewport. The release `218BA4C7` DLLs wrote all 360 at 942×869, which is the editor view, not the 1123×870 game window. Neither control log has J2's new start line, so the old code really ran. Both controls ran in PIE only; their staged runs weren't reached.
- **Other checks OK:** the plan's five ownership boundaries (§A3 a–e) hold in the code. NotGameView is counted by sub-reason on both fixtures. The `capture_ownership` block is in `run_summary.json` only. The resize leg keeps no stale-size picture. Staging, content, hosts and refs match my own baselines (4 host files have identical content but a moved timestamp). No process outside the job was killed.
- **GAP 1:** the "accepted ambiguous" counter can never be anything but 0, so the office line's `a=` field carries no information.
- **GAP 2:** the office reader crashes on a labelled frame whose mask file is null. It still prints FAIL, but with −1 for visible events, edge error and wrong view instead of counting the missing masks.
- **Office reader lines, my runs:**
  - PASS: `I2.13 PASS r=360 w=360 v=4 e=0 x=0 a=0 #5`
  - FAIL: `I2.11 FAIL r=360 w=360 v=-1 e=-1 x=-1 a=0 #3 I-fail=360,360,385,0,0,3,0,0,0,0,0,0,0,0,0,0`
- **The stop:** I confirmed 96 rows that say "visible" with zero target pixels and no mask. The code involved is unchanged since the release. I checked it against UE 5.1's projection code: the box is scaled by (viewport aspect × 0.75). That predicts 0.813 for the failing window and 0.968 for the passing Lyra windows; I measured 0.813 and 0.967. StackOBot reads 1.000. So the passing Lyra legs also have boxes about 3 % off, and no J2 gate checks that.

**Ready for office checkpoint I? No.**
- J2 is not qualified and not pushed: staged legs, the Lyra controls and the rest of the matrix haven't run.
- The office reader also needs Python 3.11 or later, plus a hand-written file listing at least 20 inspected frames with their SHA-256 hashes. No tool produces that file, and neither requirement is in the reader's documentation.

**NEEDS-DECISION**
1. Decide whether to repair the box placement and visibility flag on games set to MaintainYFOV (pre-existing, outside J2's scope), and in which line, before J2 requalifies. It also affects the passing Lyra legs. Nobody has checked which setting the office hosts use.
2. Rule on the two reader and counter gaps: drop or fix the `a=` field, and make the reader count labelled frames with no mask instead of crashing.
3. Rule on the camera-change risk: a mid-run camera switch would drop the frames in flight. That comes from reading the code and hasn't been measured.
api-equivalent cost (not billed on Max): $17.53 | turns: 127 | running total across all briefs: $4,510.92
