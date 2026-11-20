# ACK-VERDICT: 082-06 NEEDS-DECISION ruled in _reviews\082-06d-chat-ruling-gcook-fixture.md - MainWorld cook-identity row amended to the control-pair form (CB_GateLevel stays exact); fixture slot fix + re-cook + staging authorised
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: high
# NAME: 082 m53 fixture fix + re-cook + stage (Code, Opus 5.5)

# 082-06d — m53 S1: fix the fixture tool's slot bug, re-author the affected packages, re-cook, G-COOK (amended), and stage if it passes

You are Claude Code, running **headless** from the GDP mailbox. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long. Put status notes in the same message as your next tool call and keep going. End the run only when (a) every step below is done, (b) a stop condition below fires, or (c) something you need is deliberately protected from you or needs the owner. Every final message follows the structure at the end and starts with a `VERDICT:` line.

Run commandlets and cooks in the background and poll them with bounded PowerShell wait loops; never end the run while one is running. If the Bash tool stalls on this box, use the PowerShell tool. Keep context lean: summarise tool output instead of pasting it.

## Spec
- `_reviews\082-06d-chat-ruling-gcook-fixture.md`.
- Your 082-06 report and journal (`docs/sessions/2026-09-27-082-06-code-m53-s1-authoring-cook.md`).
- The evidence in `_reviews\082-06-evidence\`.

## The work
1. **Fix the fixture tool.**
   - Static meshes: set slots with `SetMaterial` on the asset.
   - The skeletal duplicate: build real material entries.
   - The verify pass reads the slots back **from disk** after saving.
   - Commit in CaptureBench, following its convention.
2. **Re-run the tool** for the affected packages only.
   - Prove `slot_empty`, the asset-slot MID, `nanite_override` and `default_material_path` from the on-disk read-back.
   - Take **M1′**. Any change outside the allowed paths means stop.
3. **Re-cook** with the same procedure as 082-06, then run the map gate.
4. **G-COOK under the amended rule.**
   - `CB_GateLevel` must be exact.
   - `MainWorld` uses the control-pair form. Define the variance region from ≥ 2 identical-input cooks of the new inputs, running one more if needed. The differences against the archived pre-m53 container must fall inside that region, and the `.umap` header must be byte-identical.
   - Every fixture package must be present in the container.
   - M2′ must be clean.
5. **If G-COOK passes:**
   - stage the new exe and container, verifying hashes at the destination;
   - record the provenance in the `_binary_baselines` README;
   - run A44 on the staged exe.

   **If it fails:** do not stage, and report.
6. **Docs:**
   - a dated plan amendment for the G-COOK `MainWorld` row;
   - a runbook note on the linked-Content scratch host (commandlets only, manifests bracket every run, no interactive saves);
   - a journal entry and the status line.

   Commit and push. No tag.

## Hard rules
- No legs, and no game launch beyond what the procedure requires.
- In CaptureBench, write only under `/Game/CaptureBenchTexCorrupt/` and the tool's own file.
- Never kill anything that is not yours, and never by name.
- Don't touch `m51`, `master`, tags, `ToCodex\` or `E:\<personal-folder>`.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means the fixture is fixed and proven, G-COOK passes, and the build is staged.
2. **Plain summary for the owner**, 2–3 short lines.
3. The fixture fix and the four read-back proofs.
4. M1′ and M2′ differences.
5. Cook and G-COOK: the `CB_GateLevel` hash, the `MainWorld` control-pair reading, and whether fixture packages are present.
6. Staging: the new staged exe and container hashes, and the A44 result.
7. NEEDS-DECISION items, or "none".
