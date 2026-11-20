# ACK-VERDICT: 082-07e RED ruled in _reviews\082-07e-chat-ruling-masked-fixture.md - fixture defect (TC_Masked never clips, so the alpha wrong copy cannot show); normal family identity-exact; fix the fixture after a full fixture audit, then amend and resume
# MODEL: claude-opus-5-5
# EFFORT: xhigh

# 082-07f — m53 S1: diagnose TC_Masked's missing clip, audit every fixture row, fix, re-cook, re-stage, and probe

You are Claude Code, running **headless** from the GDP mailbox, continuing your 082-07e session. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long. Put status notes in the same message as your next tool call and keep going.

End the run only when:
- (a) every step below is done;
- (b) a stop condition below fires; or
- (c) something you need is deliberately protected from you or needs the owner.

Every final message follows the structure at the end and starts with a `VERDICT:` line. Run commandlets, cooks and the probe in the background and poll them with bounded PowerShell waits; never end the run while one is running. If the Bash tool stalls on this box, use the PowerShell tool. Keep context lean.

## Spec
- **The ruling** `_reviews\082-07e-chat-ruling-masked-fixture.md`: all of it, especially rulings 2–5.
- **Your 082-07e report and journal.**
- **The 082-06, 06d and 07c procedure:** the manifests, the archive, the cook with G-COOK under the amended MainWorld control-pair rule, staging, provenance, and A44. The owner has given the PC for the night.

## The work
1. **Diagnose why `TC_Masked` (`StaticMeshActor_13`) never clips.**
   - Work evidence-first, from disk read-back of the saved packages and, where it matters, the cooked ones. Check each of these:
     - the component's actual slot material or materials;
     - the material's blend mode;
     - whether the OpacityMask input is connected, and which sampler output feeds it (A, or RGB/R);
     - the clip value;
     - any MIC override of the blend mode;
     - `T_TC_UC2`'s compression settings, and whether alpha survives in the cooked texture.
   - Name the cause with its evidence.
   - **If the cause is outside CaptureBench content** (an engine or project render setting), stop and report NEEDS-DECISION. Make no workaround.
2. **Full fixture audit (G322, applied to every row), before anything is cooked.**
   - For every S1 Q row that depends on a channel being visible, give one line with its evidence, from disk read-back. Covered rows: every G-ID row and every wrong copy, including the normal rows, channel swaps, alpha rows, MASKED and the F-SYN parts of G-RD, G-BIND and G4. Each line shows:
     - that the named tile's material routes that channel to a visible output;
     - that the actor's slot holds that material;
     - that an offline wrong-copy proof exists, with a predicted minimum ≥ 16 (manifest entry and value).
   - Any row that is missing a proof or is wrong: fix it in this same cook, or, if a fixture change can't fix it, list it for the amendment.
3. **Fix the fixture tool** (`make_texcorrupt_fixture.py`).
   - Re-author only the affected packages and prove each one by disk read-back.
   - Add the **offline MASKED proof**: which alpha quarters cross the 0.5 clip under the alpha fault, and the predicted min |d| against the background.
   - Commit it in CaptureBench.
4. **M1‴, re-cook, map gate, G-COOK** (`CB_GateLevel` exact; MainWorld under the control-pair rule; every fixture package present), then **M2‴**.
   - **If G-COOK passes:**
     - re-stage the exe and container, verifying the hashes at the destination;
     - add a provenance entry;
     - run A44;
     - archive the `30FE0FDE` container as the rollback.
   - **If G-COOK fails:** do not stage; stop and report.
5. **One in-engine PROBE.** It is not a gate: its frames stay outside the bank, the ledger and the live run dir, and it is labelled `PROBE` everywhere.
   - Launch the fixture with the feature **not installed**, behind the same quiet and person-present gates, and run the postflight.
   - If the harness can't launch outside the window without editing locked files, write a small probe launcher under `_reviews\082-07f-evidence\` that uses the lib read-only.
   - Show that:
     - (a) the masked tile now clips, with background (≈16,31,50) where alpha < 0.5;
     - (b) every tile whose packages were not re-authored matches the 082-07e null frames at the same frame index, max |d| ≤ 2;
     - (c) every re-authored tile still shows its intended readout.
6. **Do not touch** the predictions, the boundary, the locked harness files or `gates.json`. AMENDMENT 2 and the boundary re-issue are the next brief (082-07g).
7. **Journal and status.** Add any new gotcha, commit, and push to the branch. No tag.

## Stop conditions
- The cause is outside CaptureBench content.
- An audit row can't be made visible by any fixture change and is not a candidate for the amendment.
- G-COOK fails.
- The probe shows MASKED still not clipping, or a non-re-authored tile changed by more than 2.

For each: stop and report with the evidence.

## Hard rules
- No legs, no plugin source change and no plugin build.
- In CaptureBench, write only under `/Game/CaptureBenchTexCorrupt/` and the tool's own file.
- Never kill anything that is not yours, and never by name.
- Don't touch `m51`, `master`, tags, `ToCodex\` or `E:\<personal-folder>`.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN = the cause is named, the audit is complete, the fixture is fixed and proven, G-COOK passes, the build is re-staged, and the probe passes (a)–(c).
2. **Plain summary for the owner**: 2–3 short lines.
3. The diagnosis, with its evidence.
4. The audit table, one line per row.
5. The fixture fix and the read-back proofs, including the offline MASKED proof.
6. M1‴, M2‴ and the G-COOK readings.
7. The new staged hashes, A44 and the archive.
8. The probe's readings (a)–(c).
9. The rows the amendment must change, or "none beyond MASKED's PRE row".
10. Commits pushed.
11. NEEDS-DECISION items, or "none".
