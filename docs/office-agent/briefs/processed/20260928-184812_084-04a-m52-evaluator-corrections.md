# ACK-VERDICT: 084-02b NEEDS-DECISION ruled in _reviews\084-02b-chat-ruling-bench-tonight.md (decision 3 = this brief; bench tonight after it)
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 084 m52 evaluator corrections (Code, Opus 5.5)

# 084-04a — m52 part B harness: correct the evaluator gaps (ruling 084-02b decision 3) BEFORE tonight's bench, so tonight's verdicts are final. Python only; no source, no build, no game launch

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long. Put status notes in the same message as your next tool call and keep going.

End the run only when:
- (a) every step below is done;
- (b) a stop condition below fires; or
- (c) something you need is deliberately protected from you or needs the owner.

Every final message follows the structure at the end and starts with a `VERDICT:` line. Keep context lean. Write docs with the file tools (G331).

**Someone may be using this PC. Python and git only: no game, no compile, and nothing that takes focus.**

## Bootstrap: read these first, then summarise in two lines in your first status note
1. `_reviews\084-02b-chat-ruling-bench-tonight.md`, **decision 3**. **This is the spec.**
2. `_reviews\084-02b-codex-m52-minidelta.md`, its gate findings: gap censoring, a repeated visible run treated as isolated, missing masks passing, contamination not in purity.
3. `_reviews\084-03-README.md` and the `_reviews\084-03-*` harness files; `084-03-gateselftest.py`.

## The work
1. **Correct the evaluator:**
   - **(a) Gap censoring:** a missing frame or engine-frame jump adjacent to an edge makes that edge **CENSORED**, never passed.
   - **(b) Visible frames outside the window:** any pixel-visible frame outside the labelled window FAILS, unless the paired null shows the same visible change at that frame (a different cause). This withdraws G345's "isolated runs are reported, not required" for anything the null doesn't also show.
   - **(c) Missing masks** on labelled frames FAIL.
   - **(d) Contamination:** a contaminated event FAILS gate (iii) unless it was flagged `stuck_mip.contaminated = 1` **and** reverted as ruled. Every contaminated event is listed.
   - **(e)** Add the **edge-path frequency** and **monitor-cost** readings the ruling asks tonight's run to report, if they're not already there.
2. **Prove each correction both ways,** on real banked pixels where possible and on synthetic sessions otherwise. The four earlier can-fail cases must still FAIL, and the repaired window must still PASS. Extend `084-03-gateselftest.py` and report the counts.
3. **Dry run** with the launch stubbed; 0 problems.
4. **Docs:** a journal note and a gotcha if new (next free **G347**). Commit and push `fix/m52-label-timing`. No tag, no merge.
5. **Confirm the 084-03 invocation is unchanged,** or give the new one.

## Hard rules
- **No source change, no build, no staging, no game launch.** Harness files only.
- Never kill anything that is not yours, and never by name.
- Don't touch `m51`, `master`, tags, `ToCodex\`, `E:\<personal-folder>`, or the m53 S2 files.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner**: 2 lines.
3. Corrections (a)–(e), one line each.
4. The proofs, both ways, with counts.
5. The dry run.
6. The 084-03 invocation.
7. Commits pushed.
8. NEEDS-DECISION items, or "none".
