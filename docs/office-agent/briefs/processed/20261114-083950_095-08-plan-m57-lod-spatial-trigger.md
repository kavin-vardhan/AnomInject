# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 095-08 Plan only - m57 lod_popping with a spatial trigger (distance crossing, view entry) - read-only, no build, no Codex

# 095-08 — file-by-file plan for unit B (m57): `lod_popping` gets a spatial trigger

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator). **Planning only.** Chat rules on the plan before any code is written.

**Why:** the client's annotation review said, in our words [client wording paraphrased], that a LOD pop should happen when the camera crosses a sensible distance or when the player turns toward a part of the scene that was not in view before, and that a mesh change with no change in distance, visibility or view should not serve as a typical LOD example. Today's `lod_popping` is a timer: the target snaps between its current LOD and its worst LOD every 8 frames whatever the camera does. The other two review topics are already written (m56 capture continuity, m58 terrain subtype) and wait for their engine gates on `integ/m56-m58` = `9bf6c7e`.

**Read first, in this order (binding):**
1. `D:\IntrusiveAnomalies\_reviews\095-00-ruling.md` §3 (B1 to B6: the direction chat has already set) and §6.
2. `D:\IntrusiveAnomalies\_reviews\095-02-plan-A1-capture-continuity.md` §1 (how the burst state machine, the label sample and the transition machinery work) and `095-06-ruling.md` (what changed in review). After m56, every engine tick is a captured frame; plan on that base.
3. The repo's `CLAUDE.md` status block and the journals on `integ/m56-m58`.
4. Project history on this anomaly: the m30 calibration (coverage and distance gates), gotchas on forced LOD (single-LOD meshes, instanced meshes that ignore forced LOD, G15, G16, G19, G20), and `_reviews\drafts\093-00-natural-causes-design.DRAFT.md` if it exists (its entry LOD-2).

## Hard limits
- **Read-only on every repo.** No edit, branch, commit, checkout, push or stash. The primary checkout stays on `integ/m56-m58`. Read other refs through git objects.
- **No build, no compile, no editor or game launch, no capture.** The owner's own apps are using the machine, and E: is far below its floor.
- **No Codex.** Sub-reviewers are allowed for reading; re-check yourself anything a plan step depends on. Mark claims **[V]** (re-checked against source on `integ/m56-m58` or the UE 5.1.1 source) or **[R]**.
- You write exactly one plan file, `D:\IntrusiveAnomalies\_reviews\095-08-plan-B-lod-spatial-trigger.md`, and receipts in `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-08\`.

## What the plan must contain

**1. How `lod_popping` works today,** with file and line references on `integ/m56-m58`: selection gates (multi-LOD, coverage, distance, highest-LOD), the swap, the revert, the label membership rule, the mask, and how the capture burst drives it.

**2. How Unreal itself picks a LOD** (5.1.1 source, static and skeletal): screen size, the thresholds stored on the mesh, the global scale settings, per-component overrides. From that, **the two ways to cause a pop, compared honestly:**
   - **(i) plugin-timed:** hold a forced LOD and release it when the plugin's own trigger fires;
   - **(ii) engine-timed:** shift where the engine itself switches (a per-component or per-mesh lever, if 5.1 has a safe one), so the engine does the swap as the player moves.
   For each: is it reachable through public API without touching host assets, does it revert cleanly, can the label be measured (which frame the swap landed on, read from the render side, not assumed), and does it exist on 4.25. Recommend one, or a split.

**3. The two triggers from the ruling, designed concretely:**
   - **Distance crossing (B1):** which metric (the engine's own screen size, or camera distance), where the threshold is placed so the swap is clearly visible and happens soon given the camera's current motion, hysteresis for the retreat, what counts as "moving".
   - **View entry (B1):** how a target just outside the view is chosen and prepared while unseen, how entry is detected (the existing visibility layer), how many frames the wrong LOD shows before the snap, and how to avoid labelling a target that is prepared but never seen.

**4. The lifecycle, and how it fits the capture schedule.** Today a burst has fixed lengths: lead-in, fire, positives, revert, gap. A spatially triggered event starts when the player moves, not when a counter says so. Design the states (candidate, armed, triggered, labelled, released, timed out) and say exactly how they map onto the burst state machine, the existing deferred-onset pre-roll, the frame cap, and run end. **B2 is binding:** an armed target that never triggers is released with no label and no event, and every labelled event has something visible on screen. State what the capture does with its frames while a target is armed and waiting.

**5. The labels.** Which frames are positive (the target is on screen at a LOD the engine would not have chosen there, with visible contrast), how onset and offset are decided **from measured render state, 0 frames off**, where the frame of the swap itself is recorded, the subtype values (`distance_threshold`, `view_entry`; the old timer as `periodic`), the additive keys (trigger kind, trigger distance, swap frame), and how the `settle` flag from m56 applies around the swap. Masks: unchanged rules; say what the mask covers on the swap frame.

**6. Yield.** The client's rule is that bugs stay frequent and every injection shows something. Estimate, from the code and banked sessions, how often each trigger can fire while a player moves normally, and what happens when the player stands still (B2 says nothing is labelled; say what the pool does instead so a capture is not empty of LOD events for long). Name the host properties that decide yield: how many multi-LOD meshes are in reach, instanced meshes, single-LOD content.

**7. The old timer mode (B4):** out of the default auto pool, reachable from the console and the bench only. List every place that assumes the timer (the half-period setting and its echo, defaults, the dashboard's pool list in the dashboard repo, docs, the m30 gates).

**8. The bench.** A test-only scripted camera path, because the plugin does not move the player: where it lives (CaptureBench, never the shipping plugin), which existing fixture map has multi-LOD meshes at approachable distances, and what must be authored. **B6 is binding: look first.** Say exactly how short clips of both triggers get made for the owner to judge before any gate campaign (which leg, which frames, how they are assembled with the tools that already exist).

**9. Gates and can-fails,** to the project's label-sync standard: onset and offset 0 frames off from pixels at AA-off; no event without movement; no label on an unseen target; the swap frame recorded equals the swap frame measured; revert leaves nothing behind; the timer mode unchanged when asked for by name; non-LOD anomalies untouched. A can-fail for each. Disk and time per leg.

**10. Risks:** label sync around a swap the engine times; temporal AA residue on the swap frame; skeletal meshes; instanced and hierarchical-instanced meshes; streaming; a target destroyed while armed; two tick orders; interaction with `lod_corruption` (which also forces LODs) and with the census.

**11. Commit breakdown, briefs, bench hours.**

**12. What the client will see change,** in plain words, for chat's client note.

**13. Questions for chat:** each a real fork, with your recommendation and the reason. Chat decides these; the owner is not asked technical questions. Separately list **questions only the owner or the client can answer** (for example what the client's reference clip shows), in one line each.

**14. A 4.25 note:** which touched files differ on `feat/europa-port`, and whether each mechanism exists on 4.25.

Write for a cold reader: short sentences, lists over long paragraphs, codenames only.

## Rules
- **Never touch the owner's processes. Never kill a process by name.** No machine settings. Delete nothing. Never print a token. Office data never comes home; nothing here needs it.
- **How your turn ends:** do not end on a progress update or a question, and do not end while a sub-task is running. If your context passes 65 %, write up what you have and say what is unplanned. Target: under 2 hours.

## Final message
- **The very first line is exactly `VERDICT: GREEN`, `VERDICT: NEEDS-DECISION` or `VERDICT: RED`.** Bare line, no code fence. GREEN means the plan file is written and complete.
- Then:
  1. **For the owner (3 lines, plain words):** what a LOD pop will look like and when it will happen; what he will be shown before it is tested.
  2. **The plan in twelve lines:** the mechanism chosen, both triggers, the lifecycle, the labels, the yield estimate, the bench, the estimate.
  3. **Questions for chat,** with your recommendations. **Questions for the owner or client,** one line each.
  4. **Safety,** or "none".
