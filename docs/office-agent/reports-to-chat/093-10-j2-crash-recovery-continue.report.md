# REPORT for 093-10-j2-crash-recovery-continue.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 093-10-j2-crash-recovery-continue.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-15 07:12:37 | finished: 2026-10-15 10:07:48 | duration: 175 min | exit code: 0
context at last turn: 589961 tokens (59% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\093-10-j2-crash-recovery-continue.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner**
- The bench came back cleanly after this morning's shutdown. Codex then passed the next checks: picture framing, letterbox and locked FOV, plus 32 of the 84 matrix cases.
- It stopped at the first mask-versus-picture "pairing" test, which failed. Nothing was pushed, and both test projects are back to how they were.
- My review agrees with Codex's results apart from one small mistake in its report. My own reading is that the failure is a lighting problem in the test scene, not a timing bug, but that is not proven yet. Chat needs to decide how to continue.

**Dead attempt and environment**
- The 093-09 attempt has no exit code, and I wrote none.
  - Its events stop at 04:29:11 and the PC booted again at 04:34:05.
  - No process for that Codex thread was alive.
  - No other run folder had the same problem.
  - The note is `code-receipts-093-10\dead-attempt-093-09.txt`.
- The two date variables (`GIT_AUTHOR_DATE`, `GIT_COMMITTER_DATE`) are not set in my environment. Last session's environment did have them. They were also removed for the Codex process, and its launch receipt confirms it could not see them.

**The Codex job**
- It resumed the same session. It ran 07:17–09:59 (9,746 s), one try, exit 0, and ended with `VERDICT: NEEDS-DECISION`.
- **Candidate:** `eff797c` rehashed byte-identical, so no rebuild was needed. StackOBot exe `57F83797`, Lyra exe `C6D74FED`.
- **Branch:** local `fix/followup-capture` moved `571f15a` → `7d5e5a4`, a docs-only commit. The remote is still `1bf7054`; nothing was pushed.
- **Archive:** `followup-capture-J2-recovery-09310-eff797c-57F83797-C6D74FED-UNQUALIFIED`.
- **Results:**
  - PF-3: both comparisons now differ from the saved run by 0.0 px.
  - PF-4 (letterbox) and PF-5 (locked FOV, label reads 70 where the engine's unlocked value is 90) pass.
  - J2 matrix: 32 pass, 1 fail, 51 not run.
  - The failure is native pairing at 100 % on StackOBot: 56 of 56 decidable frames read `NEITHER`.
- **Not run:** the camera switch, the cook, every staged leg and the push.
- **Harness fixes:**
  - PF-3 viewport fix, first attempt.
  - The marker-barcode repair's second attempt was completed by changing how the barcode reader samples frames when the view keeps its vertical FOV. The originals are preserved and no threshold changed. That problem has now used both of its allowed attempts.
  - Smaller first-attempt fixes for idle checking, closing its own editor, a helper list and string attribution.

**My review** (`_reviews\093-10-code-review.md`)
- Part A (the 093-09 work): 11 OK, 0 GAP.
- Part B (the 093-10 work): 17 OK, 1 GAP, 2 OPEN. The two OPEN items are the staged legs with the rest of the unrun matrix, and the camera switch, both left unrun by the stop.
- **GAP (minor):** the report says the healthy reader line came from the corrected StackOBot session. It actually came from the original session that still had the barcode drawn on it. The line passes on both, so no result changes.
- Two related findings, not counted:
  - Both resize legs account for every missing frame as required (360 requested = 358 written + 2 named drops). But the per-reason rejection counter for that drop reason still reads 0.
  - One edge checker could not judge three events after the resize. A second, independent checker measures 0 frames off on all four.

**Reader lines** (I ran them myself, Python 3.10.11 and 3.13.1 give identical output, and session hashes were unchanged)
- Healthy PASS: `I1.11 PASS v=4 c=0 b=1000 #0`
- Null-mask FAIL: `I1.11 FAIL r=360 w=360 v=0 e=-1 x=0 m=96 c=-1 b=-1 #9 I-fail=360,360,385,0,0,3,0,0,0,0,0,0,0,0,0,0`
- Box reads: `B1 n=96 b=999 #0` (StackOBot) and `B1 n=96 b=967 #4` (Lyra). The 0.813 replay read b=813 and was rejected.

**Ready for office checkpoint I? No.** J2 is not qualified: the pairing gate failed, and the camera switch, cook and staged legs have not run. The owner has nothing to do yet. When J2 qualifies, he will need:
- the build pulled;
- cases 11–14 run on the second office host;
- Python 3.10+ with numpy and Pillow;
- `g=` added after glancing at the contact sheet;
- one box read on an older delivered-build session per host.

**NEEDS-DECISION**
1. **The pairing failure: is it the test scene or a product bug?** My check (images in `code-receipts-093-10\`):
   - On the current frame, the probe cube's front face is black (in shadow) and fills its mask outline exactly.
   - Only the lit side face is bright magenta, and the reader centres on that. So every frame reads `NEITHER`, and none reads one frame late.
   - This is the first pairing leg run with the J2 settings, which turn off Lumen, bounce lighting and reflections. It also ran in the editor with motion blur on. Earlier passing pairing runs used packaged builds at normal lighting.
   - No control leg separates these factors, so the cause is not established.
2. **Whether and how to continue from this stop**, keeping the pairing rule (`PREVIOUS` = `NEITHER` = 0) and the K7 order.
3. **The marker-barcode problem has used both of its allowed repair attempts.** A camera-leg failure from the barcode would need a ruling.
api-equivalent cost (not billed on Max): $18.96 | turns: 136 | running total across all briefs: $4,543.73
