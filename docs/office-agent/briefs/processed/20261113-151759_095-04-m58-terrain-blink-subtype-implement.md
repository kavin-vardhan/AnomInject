# ACK-VERDICT: 095-03b's NEEDS-DECISION is resolved by chat ruling _reviews\095-03b-ruling.md. E: is clean (93 GiB free). The Game build and every engine leg wait for a window when the owner's own apps are closed. Until then Code writes and compiles plugin code lightly; this brief is re-scoped to that.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 095-04 m58 terrain blink subtype - write the code on its branch, light Editor compile, pure selftest - no engine legs, no Codex

# 095-04 — implement m58: terrain as a blink subtype (code and light compile only)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**What this is:** the client's annotation review asked that a terrain disappearance stay in the blink family with its own subtype. An earlier session of yours wrote the plan; chat accepted it. You implement it now.

**The machine is shared right now.** The owner's own apps (Blender, ComfyUI, Resolve) are using most of the memory and part of the GPU, and his work comes first. So this brief is **light on the machine by design**: you write the code, compile only the plugin's Editor modules with throttling, and run the pure selftest. **No Game build, no editor or game launch, no capture leg, no cook.** Those run in a later brief when the PC is free.

**Read first, in this order (binding):**
1. `D:\IntrusiveAnomalies\_reviews\095-03b-ruling.md`: why this brief is light, and the machine rules.
2. `D:\IntrusiveAnomalies\_reviews\095-02-ruling.md`: chat's answers. The "Unit C" sections are your scope.
3. `D:\IntrusiveAnomalies\_reviews\095-02-plan-C-terrain-blink-subtype.md`: **the build specification.** Where a ruling differs, the ruling wins.
4. `D:\IntrusiveAnomalies\_reviews\095-00-ruling.md` §4 and §6.
5. The 095-03b report in `D:\IntrusiveAnomalies\_mailbox\to_chat\` (the Editor target is built at `master`; the Game target stopped at step 392 of 764).
6. The repo's `CLAUDE.md` status block.

**Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-04\`.

## Pre-flight: stop with NEEDS-DECISION if any fails
- The primary checkout is on `master` = `ebc34ed`, clean apart from the known untracked files.
- E: is at or above its 50 GiB floor. (D: is not checked in this brief: you write only source files there, and its free space swings with the owner's page file.)
- No Unreal editor, game or build process of ours is still running from an earlier brief. Never kill a foreign process.

## The work
Create `feat/m58-terrain-blink-subtype` off `master`. Implement plan §8 commits 1 to 4, with the ruling's choices:
- **Scope option (b):** the subtype, **plus** the refusal of `blinking` on engine-landscape targets (`terrain_unlabelable`, counted), with the bench lever that disables the refusal for gate C-G2.
- **Mixed actors:** terrain if the actor or any of its components matches a rule; the rule text says which.
- **Default patterns empty.** The ini key and the console override exist; the run banner echoes both rules, their source, and whether the class rule is ACTIVE or INERT.
- **No schema minor bump.**
- **`missing_object` is untouched.** Selection and eligibility are untouched (C3).
- The new gotcha about non-Nanite grass reporting zero instances goes into the gotchas file (next free number G574; re-check at commit).

## Compile, gently
- **Editor target only,** Development, Win64, incremental (the plugin's modules), with UBT limited to **2 parallel actions**.
- **Only start a compile when at least 4 GiB of physical memory is free.** Sample it first. If it is not free, wait up to 20 minutes in total across the brief, then skip compiling and say so.
- If a compile runs: 0 errors and 0 warnings in the plugin. Fix your own compile errors and recompile; that is normal work, not a failed gate. Stop a compile by PID if free memory falls under 1 GiB for more than two minutes, and report.
- Run the lever audit, since you add a bench lever.
- **Do not build the Game target.** Do not launch the editor or the game.

## Gates in this brief
- **C-G1 only:** the pure selftest and its three mutants (a small standalone compile; fine to run).
- **Everything else waits** (C-G2 to C-G6: all engine legs, PIE and staged). Write `predictions-095-04.txt` for those legs now anyway, so they are frozen before any leg ever runs, and list the legs that remain.
- A failed C-G1 is **reported and stops.**

## Finish
- Commits follow the plan's breakdown, Conventional Commits, one concern each.
- Docs commit: readme subtype row and changelog, the m22 pin note, architecture catalog, the status block, a session journal. The status block says plainly: **written, Editor-compiled (or not), selftest passed, no engine leg run yet, Game target not built.**
- **Push `feat/m58-terrain-blink-subtype` only if the Editor compile ran clean** (pre-push scan first). If you could not compile, keep the commits local and say so. **No merge to `master`, no tag.**
- **Leave the primary checkout on `feat/m58-terrain-blink-subtype`,** clean. State what the host's Editor binaries match when you finish.
- Also write a short office card, `D:\IntrusiveAnomalies\_reviews\095-04-office-card-terrain-pattern.md`: how the owner finds a host's terrain pattern from a terrain event's own label fields at the office, where the ini key goes, and how the run banner confirms it. Plain words. The pattern itself is never written at home.

## Rules
- **No Codex.** No `--force`, `clean`, `reset --hard` or `stash`. Delete nothing outside your own build outputs.
- **Never touch the owner's processes** (Blender, ComfyUI, Resolve or anything else that is not yours). **Never kill a process by name.** No machine settings.
- **Never touch:** the `m51` ref, `fix/followup-capture`, `feat/europa-port`, `D:\IA_Europa`, tags, the dashboard repo.
- Never print a token. Codenames only. Office data never comes home.
- **How your turn ends:** do not end while anything you started is still running, and do not end on a progress update or a question. End only with the final message below. Target: under 2.5 hours.

## Final message
- **The very first line is exactly `VERDICT: GREEN`, `VERDICT: NEEDS-DECISION` or `VERDICT: RED`.** Bare line: no number in front, no code fence around the message. GREEN means the code is committed on the branch, C-G1 passed, and either the Editor compile was clean and the branch is pushed, or you state plainly that memory never allowed a compile and the commits are local.
- Then:
  1. **For the owner (3 lines, plain words):** what a terrain blink is labelled now, what is refused and why, what is left before it can ship.
  2. **Commits** (hash and subject), the branch tip, pushed or local.
  3. **Compile:** ran or skipped, memory readings, result, warning count, lever audit.
  4. **C-G1 results;** the frozen predictions; the legs that remain.
  5. **What the host's binaries match now.**
  6. **Safety,** or "none". **Decisions chat must make,** or "none".
