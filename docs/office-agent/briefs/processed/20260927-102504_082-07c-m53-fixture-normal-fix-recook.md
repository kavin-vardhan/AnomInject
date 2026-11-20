# ACK-VERDICT: 082-07b RED ruled in _reviews\082-07c-chat-ruling-s1-legs-red.md - fixture defect (normal readout never wired Normal; feature correctly refused); UV route B pixel-exact; fix scene -> harness fixes + G3 strengthened -> resume
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: high
# NAME: 082 m53 fixture normal fix + re-cook (Code, Opus 5.5)

# 082-07c — m53 S1: fix the normal-family fixture materials, re-cook, G-COOK, re-stage

You are Claude Code, running **headless** from the GDP mailbox. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long. Put status notes in the same message as your next tool call and keep going.

End the run only when:
- (a) every step below is done;
- (b) a stop condition below fires; or
- (c) something you need is deliberately protected from you or needs the owner.

Every final message follows the structure at the end and starts with a `VERDICT:` line. Run commandlets and cooks in the background and poll them with bounded PowerShell waits; never end the run while one is running. If the Bash tool stalls on this box, use the PowerShell tool. Keep context lean.

## Spec
- The ruling: `_reviews\082-07c-chat-ruling-s1-legs-red.md`, item 1.
- Your 082-07b report and journal.
- The 082-06/06d procedure: the manifests, the archive, the cook with G-COOK under the amended MainWorld control-pair rule, and staging.

## The work
1. **Fix the fixture tool** (`make_texcorrupt_fixture.py`). Normal-family readout materials must connect **Normal** in a Lit shading model, and they may keep the Emissive readout.
   - Re-author only the affected packages.
   - Read them back from disk: every normal-family target has `MP_Normal` connected. Show the materials and the proof.
   - Commit it in CaptureBench.
2. **M1″.** Only the allowed paths may change.
3. **Re-cook**, then run the map gate and **G-COOK**:
   - `CB_GateLevel` must be exact;
   - `MainWorld` uses the control-pair rule, reusing or extending the variance region from 082-06d;
   - every fixture package must be present;
   - M2″ must be clean.
4. **If G-COOK passes:**
   - re-stage the exe and container, verifying the hashes at the destination;
   - add a provenance entry;
   - run A44 on the staged exe;
   - archive the build being replaced.

   **If G-COOK fails:** do not stage.
5. **An offline admission check**, where possible without the game: using the cooked assets or the editor, confirm every normal Q-row target would pass the admission tree's V1 and V2 steps. The in-engine G0 census follows in 082-07e.
6. **Journal and status.** Commit and push. No tag.

## Hard rules
- No legs and no source change to the plugin.
- In CaptureBench, write only under `/Game/CaptureBenchTexCorrupt/` and the tool's own file.
- Never kill anything that is not yours, and never by name.
- Don't touch `m51`, `master`, tags, `ToCodex\` or `E:\<personal-folder>`.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN = the fixture is fixed and proven, G-COOK passes, and the build is re-staged.
2. **Plain summary for the owner**: 2–3 short lines.
3. The fixture fix and the read-back proofs.
4. M1″ and M2″.
5. G-COOK readings.
6. The new staged hashes and the A44 result.
7. The offline admission check.
8. NEEDS-DECISION items, or "none".
