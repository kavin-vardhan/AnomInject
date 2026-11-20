# ACK-VERDICT: 084-07 NEEDS-DECISION ruled in _reviews\084-07-chat-ruling-source-round3.md — partial guard re-based on the null band; warm-up events count; visible_positive change accepted
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 084-07b readme, cook, Codex re-check (Code, Opus 5.5)

# 084-07b — The client readme's exact label semantics, a cook of `fix/m52-label-timing` (so the bench container carries AnomalyBench), and a Codex re-check of the 084-07 fixes through the Relay. NO game launch

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going. End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line. **The cook and the Codex job outlive the tool timeout: run them in the background and poll.** Keep context lean. Write docs with the file tools (G331).

**The PC may be in use. Compiling and cooking are allowed; launching the game and anything that takes focus are not.**

## Bootstrap: read these first, then summarise in three lines
1. `_reviews\084-06-chat-ruling-bench.md` (ruling 3 and the status table) and `_reviews\084-05b-chat-ruling-harness-codex-red.md`, decision 3 (F1 semantics).
2. `_mailbox\to_chat\084-07-sync-fixes-round3.report.md` and **chat's ruling `_reviews\084-07-chat-ruling-source-round3.md`** (decision 3 and the `visible_positive` consequence are this brief's).
3. The journal `docs/sessions/2026-09-29-084-07-sync-fixes-round3.md`, by section.

## The work
1. **Start the Codex re-check first (it runs in the background through the Relay):**
   - scope = **the 084-07 changes only**: the m52 `partial` transition flag (or the render-lag fix), camera_clipping triangle confirmation and F2, F4, F5, F3, and the row families;
   - run folder `runs\2026-09-29-084-07-sync-recheck\`;
   - ask for RESOLVED / PARTIAL / NOT per item, plus anything new, with file:line evidence;
   - collect it to `_reviews\084-07-codex-sync-recheck.md` when it lands. **Do not act on it.**
2. **Cook `fix/m52-label-timing`** at its head (after 084-07) into a complete staged set.
   - The cooked plugin descriptor must list `AnomalyBench` as a bench-only module. Prove it by extracting the descriptor (as 084-06 did with UnrealPak).
   - Hash every file, and archive to `_binary_baselines\m52fix-cook-<hash>\`, with the exe/container pairing recorded.
   - **Do not stage it.** The m53 set stays staged.
   - Record the cook time.
3. **Client readme: the exact per-frame label semantics, per anomaly:**
   - `anomaly_present` = the event is active on this frame (for `stuck_low_mip`, the active window is the render-held window);
   - `visible_positive` / `observable` = visibility per frame;
   - `transition`, with every reason (`taa` with the 3/8/1 values, `partial`, `camera_clipping_unconfirmed`) and what a client should do with those frames;
   - every allowed `anomaly_present`-without-labelling-event family from 084-07;
   - camera_clipping's label definition (slab + triangle confirmation), its limits and cost;
   - the mask-ID recycling;
   - a one-paragraph "how to build a per-frame training label from these fields" recipe.

   Every statement must match the source; list each field with its file:line.
   - Also: 084-07 ruling decision 3 (m52's partial onset frames), and **the `visible_positive` / `labelled` change**: rows that are event-active but not labelled now read `visible_positive: false`, and deliveries before this build read `true` there.
3b. **`verify_capture.py`** reads the new `labelled` / `visible_positive` rule and the `transition` reasons.
   - Its selftest gains cases both ways: an event-active unlabelled row must not count as a visible positive, and an old-build session is read under the old rule and says so.
   - Re-run its existing selftest and contracts; they must be unchanged except for the declared cases.
4. **The delivery packaging checklist** (in the repo, where the m53 DC plan will reference it): add the G354 item. Before packaging, extract and diff the shipped plugin descriptor and module list against the intended set: bench modules and levers absent or inert without `-IAIBench`, nothing silently missing.
5. **Docs:** the journal `docs/sessions/2026-09-29-084-07b-readme-cook-recheck.md`, the status block, and gotchas. Commit and push on `fix/m52-label-timing`. No tag, no merge.

## Hard rules
- No game launch, no staging, nothing that takes focus.
- Never kill anything that is not yours, and never by name.
- Don't touch `m51`, `master`, tags, `ToCodex\`, `E:\<personal-folder>`, or the m53 branch and files.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner**: 2–3 lines.
3. The Codex re-check: the verdict and each item's status, one line each.
4. The cook: hashes, the descriptor proof, the archive path, the minutes.
5. The readme: sections written, and the field-to-source table count. `verify_capture.py`: its selftest before and after.
6. Commits pushed.
7. NEEDS-DECISION items, or "none".
