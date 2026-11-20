# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 095-04 m58 terrain blink subtype - implement on its branch, build clean, selftest and PIE gates - no Codex

# 095-04 — implement m58: terrain as a blink subtype

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**What this is:** the client's annotation review asked that a terrain disappearance stay in the blink family with its own subtype. An earlier session of yours wrote the plan; chat accepted it. You implement it now.

**Read first, in this order (binding):**
1. `D:\IntrusiveAnomalies\_reviews\095-02-ruling.md`: chat's answers. The "Unit C" sections are your scope.
2. `D:\IntrusiveAnomalies\_reviews\095-02-plan-C-terrain-blink-subtype.md`: **the build specification.** Where the ruling differs, the ruling wins.
3. `D:\IntrusiveAnomalies\_reviews\095-00-ruling.md` §4 and §6.
4. The 095-03b report in `D:\IntrusiveAnomalies\_mailbox\to_chat\` (the rebuild, the host and fixture findings, the baseline readings, disk).
5. The repo's `CLAUDE.md` status block.

**Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-04\`.

## Pre-flight: stop with NEEDS-DECISION if any fails
- The primary checkout is on `master`, clean apart from the known untracked files, and 095-03b rebuilt it (its binaries match `master`).
- Every drive is at or above its floor, and stays there through an incremental build and about 1 GB of sessions.
- No foreign Unreal or build process is running (never kill one).

## The work
Create `feat/m58-terrain-blink-subtype` off `master`. Implement plan §8 commits 1 to 4, with the ruling's choices:
- **Scope option (b):** the subtype, **plus** the refusal of `blinking` on engine-landscape targets (`terrain_unlabelable`, counted), with the bench lever that disables the refusal for gate C-G2.
- **Mixed actors:** terrain if the actor or any of its components matches a rule; the rule text says which.
- **Default patterns empty.** The ini key and the console override exist; the run banner echoes both rules, their source, and whether the class rule is ACTIVE or INERT.
- **No schema minor bump.**
- **`missing_object` is untouched.** Selection and eligibility are untouched (C3).
- The new gotcha about non-Nanite grass reporting zero instances goes into the gotchas file (next free number G574; re-check at commit).

**Build:** Editor and Game, Development, Win64, in the mode the release proof used. 0 errors and 0 warnings. Run the lever audit, since you add a bench lever.

## Gates in this brief
Freeze `predictions-095-04.txt` before the first leg. Then:
- **C-G1** (pure selftest and its three mutants): all of it.
- **C-G2, C-G3, C-G4, C-G5: the PIE legs only.** No cook exists, so every staged leg waits for the shared cook brief. List exactly which legs remain.
  - **C-G4's "equal to the release build" comparison** needs a release-side leg. If the archived release editor binaries can host it by a receipted swap and restore, do so. Otherwise run only the new-build half and list the comparison as remaining.
- **Admission:** the input-idle wait is waived today (`idle_wait_waived=true`); the other checks stay.
- A failed gate is **reported and stops.** No same-turn fix.

## Finish
- Commits follow the plan's breakdown, Conventional Commits, one concern each.
- Docs commit: readme subtype row and changelog, the m22 pin note, architecture catalog, the status block, a session journal.
- Pre-push scan, then push `feat/m58-terrain-blink-subtype`. **No merge to `master`, no tag.**
- **Leave the primary checkout on `feat/m58-terrain-blink-subtype`,** clean, with the host's binaries built from its tip. State that plainly in the report; the next brief switches branches itself.
- Also write a short office card, `D:\IntrusiveAnomalies\_reviews\095-04-office-card-terrain-pattern.md`: how the owner finds a host's terrain pattern from a terrain event's own label fields at the office, where the ini key goes, and how the run banner confirms it. Plain words. The pattern itself is never written at home.

## Rules
- **No Codex.** No `--force`, `clean`, `reset --hard` or `stash`. Delete nothing outside your own build outputs.
- **Never touch:** the `m51` ref, `fix/followup-capture`, the J2 worktrees under `E:\IA_BuildCache\`, `feat/europa-port`, `D:\IA_Europa`, tags, the dashboard repo.
- **Never kill a process by name.** No machine settings. Never print a token. Codenames only. Office data never comes home.
- **How your turn ends:** do not end while anything you started is still running, and do not end on a progress update or a question. End only with the final message below. Target: under 3.5 hours.

## Final message
- **The very first line is exactly `VERDICT: GREEN`, `VERDICT: NEEDS-DECISION` or `VERDICT: RED`.** Bare line: no number in front, no code fence around the message. GREEN means the branch is pushed, both targets built with 0 warnings, C-G1 passed, and every PIE leg you ran passed.
- Then:
  1. **For the owner (3 lines, plain words):** what a terrain blink is labelled now, what is refused and why, what is left before it can ship.
  2. **Commits** (hash and subject), the branch tip, the push confirmation.
  3. **Build:** result, warning count, hashes, lever audit.
  4. **Gates:** predictions against results per leg; the legs that remain for the cook brief.
  5. **What the host's binaries match now.**
  6. **Safety,** or "none". **Decisions chat must make,** or "none".
