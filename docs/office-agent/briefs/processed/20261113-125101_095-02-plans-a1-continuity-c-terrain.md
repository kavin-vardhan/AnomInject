# ACK-VERDICT: 095-01b declared GREEN in its final message and chat accepts it (master = d7eee97 + ebc34ed on origin, primary checkout on master, m51 unmoved). The watcher read it as UNDECLARED only because the VERDICT line was numbered and inside a code fence. See the final-message rule below.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 095-02 Plans only - m56 capture continuity (no silent frame skips) and m58 terrain blink subtype - read-only, no build, no Codex

# 095-02 — two file-by-file plans: A-1 capture continuity (m56) and C terrain blink subtype (m58)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Where things stand:** the primary checkout is on `master` = `ebc34ed` (the delivered M3 release `d7eee97` plus one docs commit). The owner moved the priority back to 5.1 to act on the client's annotation-quality review. This brief is **planning only**. Chat rules on the plans before any code is written.

**Read first, in this order (all binding context):**
1. `D:\IntrusiveAnomalies\_reviews\095-00-ruling.md`: chat's rulings. §2 is unit A, §4 is unit C, §6 is process.
2. `D:\IntrusiveAnomalies\_reviews\095-00-code-capture-review-2026-11-11.md`: your own read-only review from an earlier session. Its line numbers refer to `fix/followup-capture`; re-derive every line number on `master`.
3. `D:\IntrusiveAnomalies\_reviews\095-01-ruling.md`: why nothing can be built yet.
4. The repo's `CLAUDE.md` status block and `docs/sessions/2026-11-13-095-01-return-to-main.md`.

## Hard limits for this brief
- **Read-only on every repo.** No source edit, no branch, no commit, no push, no stash.
- **No build, no editor launch, no capture.** E: is below its floor (ruling 095-01 D1, D5).
- **No Codex,** no Relay.
- **You write exactly two plan files and your receipts:**
  - `D:\IntrusiveAnomalies\_reviews\095-02-plan-A1-capture-continuity.md`
  - `D:\IntrusiveAnomalies\_reviews\095-02-plan-C-terrain-blink-subtype.md`
  - receipts in `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-02\`
- Read other refs (`m51`, `fix/followup-capture`, `feat/europa-port`, the dashboard repo) through git objects only. Never check them out.
- Sub-reviewers are allowed for reading. Re-check yourself anything a plan step depends on, and mark what you did not re-check.

## Plan A-1 — capture continuity (m56)
The standard, from ruling §2: during a run every engine tick produces exactly one published frame, in order; a lost frame is always visible in the session's own files. A-1 covers ruling §2 answers (a), (b), (c) and A1, A2, A3. Throughput work is A-2 and is **not** planned here, except where item 5 says so.

The plan must contain:
1. **How it works today on `master`,** with file and line references:
   - the burst state machine, and every place a tick passes without a capture;
   - how a frame's index is assigned and where the fps stamp comes from;
   - how a frame's label row is decided (present, injected, affected, transition), and what a settle frame's row would contain if it were captured with today's code;
   - the schema-2.1 transition machinery, and how a new reason value `settle` fits it;
   - the two tick orders, and which code depends on the order.
2. **The change, file by file:** functions touched, the smallest change that meets the standard, and what stays untouched. State what happens to the console settle setting and its echo.
3. **New output keys, exactly:** names, types, units, and where each is written (label row, annotation, run summary); the schema minor bump. Then **every reader of these files** and what each needs: host tools, the verifier, the annotation export, the office check kit, the encode step, the plugin's WebClient, and the dashboard repo's Library and overlay code.
4. **Every way an armed frame can fail to be published on `master`** (your D1 to D7, plus anything else you find; leave out the follow-up-only paths). For each: the counter it gets, and how the tick gap shows in the data.
5. **The minimum writer change that A-1 needs** so that no frame is lost silently at run end or under load (ruling answer (c)). Say what is deliberately left for A-2.
6. **Carrying `6713f51`:** whether it applies cleanly to `master`; if not, the outline of the same logic re-implemented.
7. **The gate plan,** to ruling §2's gate set: the exact legs (host, map, config, AA mode, PIE or standalone, paced and unpaced, both tick orders), the existing readers you would use by name, thresholds, the can-fails, and the predictions to freeze before the first leg. Give disk and time per leg and in total, and say which gates can be met **without a cook**.
8. **How to measure whether settle frames are pixel-exact,** per delivered anomaly, so chat can later rule on dropping the flag.
9. **Risks to label sync:** where a label could shift by a frame, what the frame-index history (m7, m18, m21, m40) and gotcha G457 established about settle, and what would prove the change safe.
10. **What the client will see change:** frames per event cycle, new keys, flagged frames, video duration. Plain words; chat uses this for the client note.
11. **Commit breakdown, and your estimate** of briefs and bench hours.

## Plan C — terrain blink subtype (m58)
From ruling §4. The plan must contain:
1. **Where the subtype is decided today** and where the event's targets and their classes are known at that moment.
2. **Terrain detection (C2):** the helper, where it lives, how it recognises engine landscape actors and components by class name through the class chain with no new module dependency, and the optional ini list of extra patterns.
3. **Events with several targets.** Recommend a rule for a blink event that hides terrain and non-terrain together, and say how often that can happen with today's picker.
4. **What terrain blinking actually does on `master` today:**
   - whether landscape is eligible under the current census band, and why or why not;
   - whether hiding it works through the same route as other actors;
   - whether its mask is measured (landscape has its own custom-depth flag);
   - what the label box looks like for it.
   If any of these means a terrain event cannot be labelled to the product standard today, say so plainly. Chat needs that before promising the subtype.
5. **Every reader of `anomaly_subtype`** (as in A-1 item 3) and what the new value does to each, including the m22 subtype pin.
6. **The bench fixture:** whether the standard bench map has a landscape; if not, the cheapest honest fixture.
7. **Gates and can-fails** (ruling C5), with disk and time.
8. **Commit breakdown and estimate.**

## For both plans
- Mark every claim **[V]** (you re-checked it against `master` source or the UE 5.1.1 source) or **[R]** (reported, not re-checked).
- End each plan with **"Questions for chat"**: each a real fork, with your recommendation and the reason. Chat decides these; the owner is not asked technical questions.
- Add a short **"4.25 note"** to each: which of the touched files differ on `feat/europa-port`, from a read-only diff, so the later forward-port can be sized.
- Write for a cold reader: short sentences, lists over long paragraphs, codenames only.

## Rules
- Never touch: the `m51` ref, `fix/followup-capture`, the J2 worktrees under `E:\IA_BuildCache\`, `feat/europa-port`, `D:\IA_Europa`, tags, the dashboard repo's branches.
- Delete nothing. Never kill by name. Never print a token. Office data never comes home; nothing in this brief needs it.
- **How your turn ends:** do not end on a progress update or a question, and do not end while a sub-task is still running. End only with the final message below. Target: under 2 hours.

## Final message
- **The very first line of your final message is exactly `VERDICT: GREEN`, `VERDICT: NEEDS-DECISION` or `VERDICT: RED`.** Bare line: no number in front, no code fence around the message. GREEN means both plan files are written and complete.
- Then:
  1. **For the owner (3 lines, plain words):** what the two plans will change for the client's dataset.
  2. **Plan A-1 in ten lines:** the change, the new keys, the gates, the estimate.
  3. **Plan C in six lines,** including the answer to item 4.
  4. **Questions for chat,** with your recommendations.
  5. **Safety,** or "none".
