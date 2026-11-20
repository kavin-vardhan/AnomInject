# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 095-07b The first engine run of the client-feedback units - fixtures, one cook, Game builds, the LOD clips for the owner, then the gates for m56, m58 and m59 - HEAVY, no Codex
# ACK-VERDICT: 095-07's NEEDS-DECISION is resolved by chat ruling _reviews\095-07-ruling.md. It never started: its memory pre-flight failed while the owner's own Blender renders were running. Nothing needs re-doing. The owner has now said the PC is free; the memory floor is corrected to 8 GiB (D2 of that ruling). This brief is the same run, re-sent.

# 095-07b — the heavy run: build, cook, clips for the owner, gates

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**This is 095-07 re-sent.** The first attempt stopped in pre-flight on memory and changed nothing. 095-14 (the prep) is GREEN and accepted by chat, including its note on the G5 allowance. Start from `code-receipts-095-14\tips.txt` and check it against origin yourself. Receipts of the first attempt stay in `code-receipts-095-07\`; write this run's into `code-receipts-095-07b\`.

**Where things stand:** four units answer the client's annotation review. All are written, reviewed by a fresh session, fixed, and pass every offline test. **None has run in the engine.** This brief is the first run.
- `integ/m56-m58`: master + m58 (terrain blink subtype) + m56 (capture continuity) + m59 (forced-LOD refusals). **The build that ships.**
- `feat/m57-lod-spatial-trigger`: the integration tip plus m57 (`lod_popping` spatial trigger). Not merged.
- `feat/m58-terrain-blink-subtype` = `5e1bc03`: m58 alone, for the two "nothing else changed" gates.
- The release exe is archived: `_binary_baselines\release-2026-10-05-23098fa-218BA4C7`.
- The Game exe on the host is from 24 Sep and matches no branch. No cook exists.

**The owner has said the machine is free for this run.** E: had about 115 GB free at 14:47 IST. His work still comes first (see Rules).

**Read first, in this order (binding):**
1. `code-receipts-095-14\tips.txt` and 095-14's journal.
2. `D:\IntrusiveAnomalies\_reviews\095-07-ruling.md`, `095-13-ruling.md`, `095-12-ruling.md`, `095-11-ruling.md` with its addendum, `095-08-ruling.md` (G2, G4), `095-06-ruling.md` ("The build plan for 095-07"), `095-06b-ruling.md`.
3. The run lists and frozen predictions, each with every dated amendment:
   - `code-receipts-095-06b\095-07-run-list.md`
   - `code-receipts-095-05\predictions-095-05.txt` and CaptureBench `tools/m56_gate_predictions.md`
   - `code-receipts-095-09\predictions-095-09.txt` (legs L59-1 to L59-11)
   - `code-receipts-095-10\predictions-095-10.txt` and CaptureBench `tools/m57_gate_predictions.md`
4. The plans, for the gate definitions: `_reviews\095-02-plan-A1-capture-continuity.md`, `095-02-plan-C-terrain-blink-subtype.md`, `095-08-plan-B-lod-spatial-trigger.md`.
5. The journals of 095-06b, 095-12 and 095-13, and the setup runbook's cook and code-only-swap sections (G103).

**Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-07b\`. Python: `C:\Python313\python.exe`.

## Pre-flight: stop with NEEDS-DECISION if any fails
- `tips.txt` exists and every tip in it equals origin (CaptureBench is local: equals the branch). The primary checkout is clean apart from the known untracked files.
- **E: has at least 85 GiB free.** D: has at least 20 GiB free.
- **At least 8 GiB of physical memory is free, and none of the owner's render apps is running** (Blender, including headless render loops; ComfyUI; Resolve). Look, never touch. If either fails, wait up to 20 minutes, reading once a minute, then stop with NEEDS-DECISION and name what was running.
- No Unreal, build or StackOBot process is running. Never kill a foreign process; name it and stop.

## Part 1 — fixtures and the one cook
1. Run the fixture script in the editor for all four options (`second_pair`, `instanced`, `pair_cube`, `minlod1`), with the command lines from `tips.txt`. Check what it wrote against the predictions' descriptions (LOD counts, minimum LOD, Nanite off, placements). Commit the content on the bench branch.
2. **One cook** that serves every exe by code-only swap (G103). Maps: `CB_GateLevel`, `CB_LodFixture`, `CB_TexCorruptLevel`, `MainWorld`, `MainMenu`, `Entry`, plus the m58 landscape fixture level if C-G2 needs one. Read the map set back with `verify_cooked_maps.ps1`. Record which Editor binaries cooked it.

## Part 2 — Game exes
Build and archive each under `_binary_baselines\` with its commit and hash in the folder name, then swap by archive, never by rebuild:
1. **m57** exe from `feat/m57-lod-spatial-trigger` (first, for the clips).
2. **Integration** exe from `integ/m56-m58`.
3. **m58-only** exe from `feat/m58-terrain-blink-subtype`.
4. The **release** exe comes from its archive; do not rebuild it.
Each exe must start on the cook, load `CB_GateLevel`, and quit cleanly before any leg counts on it. Lever audit on each binary.

## Part 3 — the LOD clips for the owner (before the long gates)
The owner judges these by eye. They are the one human step in this unit, so they come first, as soon as the m57 exe and the cook exist.
- On the m57 exe, the six look-first clips **C1 to C6** as the plan, the predictions and their amendments define them, **each beside its no-anomaly twin** (same seed, same camera path). Pinned capture configs, the default (SVE) capture path.
- **C1 and C2 (turn-in, view entry) in three variants each:** the low detail visible for **30** (the default), **15** and **60** captured frames.
- **C3** (walk-up, distance threshold) on the second mesh pair.
- **C4** (walk-up then walk-away) with the down-pop at its new default, off: one pop on the way in, nothing on the way out.
- **C5** (standing still: nothing happens) and **C6** (the old timer, for contrast).
- Output: side-by-side `.mp4` files with the label box overlaid on the anomaly side, under `D:\IntrusiveAnomalies\_owner_clips\095-07\`, plus a one-page `README.md` there that says in plain words, for a reader who does not write code, what each clip shows and what to look for.
- For each clip also record: the event kind, the frames labelled, the swap frame against its prediction, and whether the readers pass on it. A clip with no event in it is a finding, not a clip; report it and go on.
- **No m57 gate campaign in this brief.** That waits for the owner's verdict on the clips.

## Part 4 — the gates
Predictions are frozen; each reading is compared with what was written before the leg. Every can-fail must fail.
1. **m56 (integration exe; release exe for can-fails and comparison):** G1 to G6 as the plan and run list define them, including G4(a) (the release exe must fail the continuity check), G4(d) (the mask-wait defect reproduced on the release exe) and G5 with a same-binary release control pair. Readers on every leg as the run list names them.
2. **The settle-exactness table (required, m56):** for every shipped anomaly, in both tick orders: the first frame on which the change is fully on screen after onset, and the first frame on which it is fully gone after offset, measured against the flagged settle frames. State per anomaly whether K = 0 would have mislabelled any frame. Re-check the m26 veto window under K = 0 from the same legs. **Do not change the default.** Chat rules on K from this table.
3. **m58:** C-G2, C-G3, C-G6 on the integration exe; C-G4 and C-G5 on the m58-only exe against the release exe.
4. **m59 (integration exe; release exe for can-fails):** L59-1 to L59-11 as the predictions and their amendments define them. L59-5 (instanced meshes), L59-8, L59-9 and L59-10 are decisive.
5. **Texture-corruption onset cost (measurement only; owner input of 2026-11-17).** The client saw the frame jump only after the two texture-corruption anomalies (the pink one and the checkered one), not after other anomalies. Their games advance on the wall clock (G64), so one long frame at the moment a corrupted texture is applied would show as a jump even with no frame dropped. Measure it on the integration exe and on the release exe, on `CB_TexCorruptLevel` and on `MainWorld`:
   - For each of the two anomalies, a capture with at least 10 events. Per event: the wall-time length of the onset frame, the two frames after it and the revert frame, against the run's median frame; the game-thread time spent inside Apply and Revert; whether any render flush, texture build, material compile or streaming wait happens on those frames.
   - The first event of a process against later ones (first-use cost), and one other anomaly (blink) on the same path as the control.
   - State plainly: does applying a corrupted texture stall the frame, by how much, on the first use or every time, and what in the code causes it. **Do not fix it in this brief.**

**When a gate fails:**
- A bench-script or fixture-script fault: fix it on the bench branch as its own commit, prove the fix both ways, re-run the leg, and say so in the report.
- A product fault: **do not change plugin Source in this brief.** Record the reading, the leg folder and the smallest reproduction; go on with every gate that does not depend on it.

## Rules
- **No Codex.** No `--force`, `clean`, `reset --hard`, `stash`, squash or rebase. Delete nothing outside your own outputs. No merge to `master`, no tag.
- **The owner's work comes first.** Never touch his processes. **Never kill a process by name;** stop only PIDs you started, after checking start time and image path. No machine settings.
  - If free physical memory stays under 2 GiB for five minutes during a build or cook, stop your own compilers by PID, keep what is archived, and end with NEEDS-DECISION.
  - If one of his render apps starts while you are running, note the time in the receipts and carry on; the memory rule above decides. Never start a new build or the cook while one is running: wait up to 20 minutes for it to end, then stop with NEEDS-DECISION and report exactly what is done and what is left.
- **Disk:** check E: and D: at the moment of each large write (a build, the cook, each capture leg). Stop below 50 GiB on E: or 10 GiB on D:. Capture legs write under the junctioned bench folders, not into the repo.
- **Never touch:** the `m51` ref, `fix/followup-capture`, `feat/europa-port`, `D:\IA_Europa`, tags, the dashboard repo.
- Never print a token. Codenames only. Office data never comes home.
- **Commit each item as soon as it is done. Bank each reading as soon as its leg ends.** If your context passes 60 %, finish the leg you are on and report what remains as NEEDS-DECISION, with the exact list of legs not yet run.
- **If the clips are done and something later stops the run, say so on the first line after the verdict,** so the owner can watch them while the rest is sorted out.
- **How your turn ends:** do not end while anything you started is still running, and do not end on a progress update or a question. End only with the final message below.

## Final message
- **The very first line is exactly `VERDICT: GREEN`, `VERDICT: NEEDS-DECISION` or `VERDICT: RED`.** Bare line, no code fence. GREEN means the clips are delivered and every gate in Part 4 ran and passed with every can-fail failing. RED means a product fault was found.
- Then:
  1. **For the owner (4 lines, plain words):** where the clips are and what to look at; whether frame skipping is gone; whether anything failed.
  2. **The clips:** one row per clip: file, what it shows, event kind, frames labelled, swap frame against prediction, readers.
  3. **Gate table:** one row per gate or leg: exe, prediction, reading, PASS or FAIL, leg folder.
  4. **The settle-exactness table,** and what it says about K = 0.
  5. **Exes and cook:** archive folders, commits, hashes; what the host's binaries match now.
  6. **Faults found,** each with its reproduction; bench-script fixes made.
  7. **Disk and memory** at start and end. Context used.
  8. **Safety,** or "none". **Decisions chat must make,** or "none".
