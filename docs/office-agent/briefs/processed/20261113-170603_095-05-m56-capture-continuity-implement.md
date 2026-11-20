# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 095-05 m56 capture continuity - write plugin commits 1 to 4 on its branch, light Editor compile, pure selftests - no engine legs, no Codex

# 095-05 — implement m56: capture continuity (plugin code, commits 1 to 4)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**What this is:** the client's annotation review found a jump between two frames right after an anomaly appeared. An earlier session of yours found the cause in the delivered code: the two engine ticks after every anomaly onset and after every offset are rendered but never captured, and the file numbering hides it. It wrote the plan; chat accepted it. You implement the plugin side now.

**The machine is shared.** The owner's own apps may be using most of the memory and part of the GPU, and his work comes first. So this brief is **light by design**: write the code, compile only the plugin's Editor modules with throttling, run the pure selftests. **No Game build, no editor or game launch, no capture leg, no cook.**

**Read first, in this order (binding):**
1. `D:\IntrusiveAnomalies\_reviews\095-02-ruling.md`: chat's answers. The "Unit A-1" sections are your scope.
2. `D:\IntrusiveAnomalies\_reviews\095-02-plan-A1-capture-continuity.md`: **the build specification** (§2 to §6, §9, §11). Where a ruling differs, the ruling wins.
3. `D:\IntrusiveAnomalies\_reviews\095-03b-ruling.md`: the machine rules (E2, E4).
4. `D:\IntrusiveAnomalies\_reviews\095-00-ruling.md` §2 and §6.
5. The repo's `CLAUDE.md` status block on `master`.

**Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-05\`.

## Pre-flight: stop with NEEDS-DECISION if any fails
- The primary checkout is on `feat/m58-terrain-blink-subtype` = `a948046`, clean apart from the known untracked files. (You will switch it; see below.)
- `master` = `ebc34ed` locally and on origin.
- E: is at or above its 50 GiB floor.
- No Unreal editor, game or build process of ours is still running. Never kill a foreign process.

## The work
Switch the primary checkout to `master` (plain checkout), then create **`fix/m56-capture-continuity` off `master`**. Not off the m58 branch: the two units merge separately.

Implement plan §11 **commits 1 to 4**, in order, **committing each one as soon as it is written** (a connection cut-off cost an hour today; uncommitted work is the risk):
1. `fix(capture): carry m51 F1 - wait only for a mask outcome that can exist`: `git cherry-pick -x 6713f51`, the one hunk placed after the ORDER HOLD block (plan §6). If the cherry-pick drags anything else, re-implement the same logic on the plan's anchors and say so.
2. `fix(capture): m56 capture every settle tick and flag it settle`: plan §2.1 to §2.4, §2.6, and the `annotation.json` minor bump to 2.
3. `feat(capture): m56 tick index, run wall time and one honest drop count`: plan §2.5, §2.7, §3.1, §4.
4. `fix(capture): m56 writer finishes every job at run end and counts failures`: plan §5, plus the two bench levers and the lever-audit update.

**Chat's choices, from the 095-02 ruling:**
- `ticks_per_captured_frame` is **redefined** as span ticks / frames written; `capture_span_ticks` is added; `capture_game_ticks` keeps its meaning.
- **Every** settle frame carries the row-level flag, with or without an event entry.
- A frame that failed to write is **removed** from `annotation.json`'s frame lists and **counted**. Nothing is renumbered.
- **The writer's in-flight cap is bounded by pending pixel bytes** (proposed 1 GiB) with a small count floor, not by job count alone. Echo the bound in the run banner.
- The settle default K stays 2. K = 0 must still mean "no flagged frames, nothing skipped".
- The bench levers are bench-gated and compiled out of Shipping.
- **Do not change** label or mask meaning on any non-settle frame, `BeginFire`, `BeginRevert`, `FinalizeArmedLabel`, the phase lengths, the m26 arms, pacing, or the temporal defaults.

**Not in this brief** (the next one does them): commit 5 (the Python readers and their tests), commit 6 (docs beyond what is below), CaptureBench changes, anything in the dashboard repo.

## Compile, gently
- **Editor target only,** Development, Win64, incremental, UBT limited to **2 parallel actions**.
- **Only start a compile when at least 4 GiB of physical memory is free.** Sample first. If it is not free, wait up to 20 minutes in total across the brief, then skip compiling and say so.
- 0 errors and 0 warnings in the plugin. Fixing your own compile errors and recompiling is normal work, not a failed gate. Stop a compile by PID if free memory stays under 1 GiB for more than two minutes, and report.
- Compile at least after commit 2 and after commit 4. Run the lever audit after commit 4.
- **Do not build the Game target.** Do not launch the editor or the game.

## Checks in this brief
- **The pure C++ selftests under `tools/`** that include `AnomalyLabelSync.h` still compile and pass, with one new case for the `settle` reason bit (plan §2.6).
- **A read-back of your own change against the plan's "untouched" list** (plan §2.3 last bullet, §9 items 1 to 8): state, item by item, that it is untouched or what changed.
- Freeze `predictions-095-05.txt` now with the plan's §7 predictions, so they exist before any leg ever runs. No leg runs here.

## Context budget
The capture subsystem file is very large. Read it in targeted ranges, not whole. **If your context passes 60 %:** finish and commit the commit you are on, compile it if memory allows, push if the compile was clean, and report which commits remain. That is a valid NEEDS-DECISION ending, not a failure.

## Finish
- A short status-block entry and a session journal `docs/sessions/2026-11-13-095-05-m56-capture-continuity.md`, saying plainly: written, Editor-compiled (or not), selftests passed, **no engine leg run yet, Game target not built, readers not yet updated (the check tools will reject new sessions until the next brief)**. One small docs commit.
- **Push `fix/m56-capture-continuity` only if the last Editor compile ran clean** (pre-push scan first). Otherwise keep the commits local and say so. **No merge to `master`, no tag.**
- **Leave the primary checkout on `fix/m56-capture-continuity`,** clean. State what the host's Editor binaries match.

## Rules
- **No Codex.** No `--force`, `clean`, `reset --hard` or `stash`. Delete nothing outside your own build outputs.
- **Never touch the owner's processes.** **Never kill a process by name.** No machine settings.
- **Never touch:** the `m51` ref (read `6713f51` from it; do not move it), `fix/followup-capture`, `feat/europa-port`, `feat/m58-terrain-blink-subtype`, `D:\IA_Europa`, tags, the dashboard repo.
- Never print a token. Codenames only. Office data never comes home.
- **How your turn ends:** do not end while anything you started is still running, and do not end on a progress update or a question. End only with the final message below. Target: under 4 hours.

## Final message
- **The very first line is exactly `VERDICT: GREEN`, `VERDICT: NEEDS-DECISION` or `VERDICT: RED`.** Bare line: no number in front, no code fence around the message. GREEN means commits 1 to 4 are on the branch, the selftests pass, and either the Editor compile was clean and the branch is pushed, or you state plainly that memory never allowed a compile and the commits are local.
- Then:
  1. **For the owner (3 lines, plain words):** what changes in a capture now, what is not proven yet, what is left before it can ship.
  2. **Commits** (hash and subject), the branch tip, pushed or local; how `6713f51` went in.
  3. **Compile:** each run, memory readings, result, warning count, lever audit.
  4. **Selftests;** the "untouched" read-back; the frozen predictions.
  5. **Deviations from the plan,** each with its reason.
  6. **What the host's binaries match now.** Context used.
  7. **Safety,** or "none". **Decisions chat must make,** or "none".
