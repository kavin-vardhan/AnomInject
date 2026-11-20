# ACK-VERDICT: 082-07j NEEDS-DECISION ruled in _reviews\082-07j-chat-ruling-can-premise.md - G4 CAN is a focus-axis premise defect (G332); re-encode as STOP-BEFORE-FIRE (Q) and re-run fresh; cancel-before-focus owed to S2 with a bench lever; runtime-conditions review of all 35 unrun legs first
# MODEL: claude-opus-5-5
# EFFORT: xhigh

# 082-07k — m53 S1: a runtime-conditions review of the unrun legs, the person-evidence read, AMENDMENT 4 (STOP-BEFORE-FIRE), boundary re-issue and dry run. Bench-free

You are Claude Code, running **headless** from the GDP mailbox, continuing your 082-07j session. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long. Put status notes in the same message as your next tool call and keep going.

End the run only when:
- (a) every step below is done;
- (b) a stop condition below fires; or
- (c) something you need is deliberately protected from you or needs the owner.

Every final message follows the structure at the end and starts with a `VERDICT:` line. If the Bash tool stalls on this box, use the PowerShell tool. Keep context lean. Write docs with the file tools, not PowerShell here-strings (G331).

## Spec
- **The ruling** `_reviews\082-07j-chat-ruling-can-premise.md`: rulings 1–3 and its sequencing.
- **Mechanics follow 082-07g and 082-07i:**
  - the amendment is dated and committed before the lock;
  - every changed encoding is proven both ways against the real banked data;
  - no-flip over the bank;
  - the supersede record keeps history;
  - the boundary is re-issued after the docs commit.

## The work
1. **The runtime-conditions review (ruling 2).**
   - For each of the 35 unrun legs and their rows, name the runtime conditions the premise depends on, check them against the real runner (`082-07-run.ps1`, including `Hold-Focus`) and banked real logs, and give a disposition.
   - Put the full table in the amendment.
2. **The person-evidence read (ruling 3).** Use timestamps from the CAN attempts and from `[M53Fg]::Force`. If the runner's own input is the source and could void unrun legs, fix the detector and prove it both ways: self-generated input is excluded, and real input still counts.
3. **AMENDMENT 4** to `docs/predictions/2026-09-27-m53-s1-legs.md`. It covers:
   - **CAN-UV and CAN-NM → STOP-BEFORE-FIRE (class Q):** FINISH with 0 frames, `fires_applied = 0`, no m53 APPLIED / REFUSED / LEDGER line. Proven both ways: the banked attempts fit it; a synthetic APPLIED line FAILS; frames > 0 FAIL.
   - **The S2 debt:** cancel-before-focus, with a bench-only "treat as unfocused" lever.
   - any changes from the review, with their proofs;
   - predictions for 082-07l.
4. **Supersede entry 5:** the three `G4_CAN_UV` attempts and the NOT-RUN record, history kept, aliases `…__SUPERSEDED_082-07k`. Nothing is deleted. Both CAN legs must re-run fresh.
5. **Re-run every proof suite on the final lib**, plus the no-flip check and the post-lock dry run, and report the counts.
6. **Docs, then the boundary.**
   - Commit and push: the amendment, the journal, the status block, and any gotcha.
   - **Then** re-issue the boundary, and run the preflight replay right after the lock.
7. **The 082-07l invocation:** the exact command with a new budget tag, and the expected time. Confirm it runs only the unrun legs plus the two fresh CAN legs, and re-launches no decided leg.

## Stop conditions
- A no-flip check changes an earlier verdict.
- An encoding can't be proven both ways.
- The review finds a row that can only be exercised with a build or fixture change. List it for S2 and continue only if the rest can still run.
- The preflight replay fails.

## Hard rules
- **No bench, no legs, no build, no cook,** and no change to the plugin source or to CaptureBench.
- Never kill anything that is not yours, and never by name.
- Don't touch `m51`, `master`, tags, `ToCodex\` or `E:\<personal-folder>`.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner**: 2–3 short lines.
3. The runtime-conditions review table.
4. The person-evidence finding, and any detector change.
5. AMENDMENT 4: its contents and commit.
6. The proofs, both ways, the no-flip result and the dry run, with counts.
7. The supersede record.
8. The boundary and the preflight replay.
9. The 082-07l invocation and the expected time.
10. Commits pushed.
11. NEEDS-DECISION items, or "none".
