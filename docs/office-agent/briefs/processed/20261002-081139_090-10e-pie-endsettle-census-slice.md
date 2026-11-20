# ACK-VERDICT: 090-10b2 NEEDS-DECISION ruled in _reviews\090-10b2-chat-ruling.md — PIE end-settle flag + census actor-list slicing here; runs after 090-10c. Build on the LATEST pushed heads (090-10c's).
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 090-10e PIE end-settle flag + census actor-list slicing (Code, Opus 5.5)

# 090-10e — Two small, proven changes on top of 090-10c's heads: (A) in PIE only, flag the first captured frame after a FireWindow label ends (`pie_end_settle`), so the observed PIE end+1 can never be an unflagged mislabel; (B) time-slice the census's actor listing, so no census frame exceeds 100 ms even at 40k actors. In-engine proof in PIE and staged

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going. End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line. Builds and game runs outlive the tool timeout: run them in the background and poll. **Keep context lean (finish below 70 %).**

**Launches only when the idle gate passes**, per 090-10c's clarified rule:
- an open but idle foreign editor doesn't block, provided the person has been idle ≥ 15 min, GPU < 10 % and headroom ≥ 6 GB;
- never touch it.

## Bootstrap
1. `_reviews\090-10b2-chat-ruling.md` (rulings 2–3): **THE SPEC.**
2. The 090-10b2 report: the PIE end+1 observation, and the census timing table.
3. The 090-10c report: its heads. **Build on them.**

## The work (fix branch first, then merge into m53)
**A. `pie_end_settle`.**
- In a PIE world only (the capture's world is a PIE world; never in staged or packaged), the **first captured frame after a FireWindow label's last labelled frame** gets `transition_reason: pie_end_settle`: unlabelled, unmasked, never a member, counted in `run_summary`.
- `verify_capture --label-rule`, the kit and E1 know the reason. It is never an AA excuse and never a pass by itself.
- **Tests both ways:**
  - pure logic;
  - mutants (flagging staged frames; flagging nothing) must fail.
- **In PIE:** on ≥ 3 runs with ≥ 6 fires per type (uv, normal, missing and corrupted texture), every end edge reads 0 frames off outside flagged frames, and the earlier end+1 events now fall on a flagged frame.
- **Staged:** no `pie_end_settle` anywhere; edges unchanged.
- Readme: one line in the field semantics.

**B. Census actor-list slicing.**
- The actor enumeration is time-sliced like the rest of the census.
- **Bound:** no frame over 100 ms on MainWorld PIE/staged, at office scale (~15k) and at 40k actors in PIE.
- Report the 650 ms frame if it recurs, with whatever you can attribute; don't chase it beyond one probe.
- The can-fail is the current head at 40k (195 ms first frame).

## Then
- Builds on both branches;
- the string scan;
- strict 0/0;
- the lever audit;
- archive.

Commit and push; **the m53 head you push is the one that passed A and B in-engine.** Docs: the journal `docs/sessions/2026-10-02-090-10e-pie-endsettle-census-slice.md`, status blocks, gotchas.

## Hard rules
- **Never change any security setting.**
- Never kill anything that is not yours, and never by name. Close only what you launched, by PID.
- Delete nothing; bank to `D:\IA_BankOverflow\`.
- Don't touch `m51`, `master`, tags, `ToCodex\`, `E:\<personal-folder>`, or the frozen `_reviews\090-06-*`.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner**: 2 lines, with **the exact commit to pull**.
3. A: the rule, the tests, the PIE edge table (runs × types → edges, flagged), and staged unchanged.
4. B: the timing table (scene × path → longest frame) and the can-fail.
5. Builds, archives, commits pushed.
6. NEEDS-DECISION items, or "none".
