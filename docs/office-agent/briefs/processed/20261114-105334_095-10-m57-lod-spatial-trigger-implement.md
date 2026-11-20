# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 095-10 m57 lod_popping spatial trigger - write the plan's commits on its branch, light Editor compile, pure selftests - no engine legs, no Codex

# 095-10 — implement m57: `lod_popping` with a spatial trigger

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**What this is:** the client's annotation review said a LOD pop must correspond to a distance threshold or to turning toward a newly visible area, not to a timer. An earlier session of yours wrote the plan; chat accepted it with one change. You implement it now.

**The machine is shared** and the owner's work comes first. This brief is **light**: code, throttled Editor compiles, pure selftests, Python tests. **No Game build, no editor or game launch, no capture leg, no cook, no content authoring.**

**Read first, in this order (binding):**
1. `D:\IntrusiveAnomalies\_reviews\095-08-ruling.md`, all of it including the addendum. **Q4 differs from the plan: an event with visibly wrong frames but no spatial swap is KEPT and MARKED, never dropped.** G1 to G4 and H3 also bind you.
2. `D:\IntrusiveAnomalies\_reviews\095-08-plan-B-lod-spatial-trigger.md`: **the build specification.** Where the ruling differs, the ruling wins.
3. `D:\IntrusiveAnomalies\_reviews\095-03b-ruling.md` (machine rules E2, E4) and `095-06b-ruling.md` (F1: disk rule for light briefs).
4. The repo's `CLAUDE.md` status block and the journals on `integ/m56-m58`, including m59's.

**Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-10\`. Python: `C:\Python313\python.exe`.

## Pre-flight: stop with NEEDS-DECISION if any fails
- The primary checkout is on `integ/m56-m58`, clean apart from the known untracked files, equal to origin, and its tip contains the m59 merges (the 095-09b report names it).
- E: and D: each have at least 5 GiB free. No Unreal or build process of ours is running. Never kill a foreign process.

## The work
Create **`feat/m57-lod-spatial-trigger` off the `integ/m56-m58` tip.** Implement plan §11 commits 1 to 7, in order, **committing each one as soon as it is written.**

**Chat's choices (the ruling):**
- **Mechanism:** plugin-timed forced LOD. Plain static meshes only; skeletal, instanced, hierarchical-instanced and Nanite targets refused by name. m59's refusals and its skeletal switch stay as they are; the spatial modes refuse skeletal regardless of the switch.
- **Never-triggered target:** no event; its id in `run_summary`.
- **Q4, the change:** an event that had labelled frames stays an event. Each swap records its cause: `distance`, `view_entry` or `hold_cap`. The event records whether a swap was visible on screen. `run_summary` counts events by kind. Nothing with labelled frames is ever removed from `annotation.json`. Design the exact keys in the plan's style and document them.
- **`lod_subvisible`** and **`lod_unconfirmed`** flags as planned. Labels from the tick-end prediction with the engine's own function, confirmed per frame on the render thread.
- **Down-pop on retreat:** built, on by default, with a setting to turn it off (the owner decides the shipped default from the clips).
- **`periodic`:** bench gate only; the 200 cm proximity gate and the highest-LOD gate apply to `periodic` only.
- **The camera path** lives in the `AnomalyBench` module.
- **Draw weight:** equal.
- **Schema minor:** 3.
- **The two new `IAnomaly` hooks** are additive with defaults; no other anomaly's behaviour changes. State that in the read-back below.

**Bench content scripts (CaptureBench repo, a branch `m57-gates` cut from `m56-gates`; local, the repo has no remote):**
- The fixture script gains two options, **written and syntax-checked, not run** (running it needs the editor, which the heavy brief will do before its cook):
  - a second mesh pair whose coarse LOD switches at a larger on-screen size (ruling G2), so an approach reads like a real approach;
  - an HISM copy and an ISM copy of the LOD fixture mesh (ruling H3), for m59's decisive leg.
- The clip-assembly script and the two m57 check scripts from plan §8.3 and §9, each proven both ways on doctored sessions where that is possible without an engine leg.
- Leave CaptureBench on `master`.

## Compile, gently
- **Editor target only,** Development, Win64, incremental, **2 parallel actions**, started only with at least 4 GiB of physical memory free **checked at launch** (wait up to 20 minutes in total if it is not; then use a syntax-only compile and say so).
- 0 errors and 0 warnings in the plugin. Compile at least after commit 2, after commit 4 and at the end. Lever audit after commit 5.
- **Do not build the Game target.** Do not launch the editor or the game.

## Checks in this brief
- The pure selftest of commit 1 and its mutants; every existing pure selftest; every Python tool test, with new cases for the LOD subtypes, the two new reasons, the swap keys and the kept-and-marked events.
- **A read-back against "untouched":** every other anomaly's membership rule, the m56 continuity accounting, the settle rule (a swap is not a settle edge), the m58 terrain rule, the m59 refusals. Item by item: untouched, or exactly what changed.
- Freeze `predictions-095-10.txt` with the plan's §9 predictions, amended for the Q4 change, before any leg ever runs.

## Context budget
This is the largest unit so far. Read the capture subsystem in targeted ranges. **If your context passes 60 %:** finish and commit the commit you are on, compile it if memory allows, push if the compile was clean, and report exactly which commits remain. That is a valid NEEDS-DECISION ending.

## Finish
- A status-block entry and a session journal, saying plainly: written, Editor-compiled (or not), selftests passed, **no engine leg run, Game target not built, no clip made yet.**
- **Push `feat/m57-lod-spatial-trigger` only if the last Editor compile ran clean** (pre-push scan first). **No merge into `integ/m56-m58` or `master`, no tag.**
- **Leave the primary checkout on `integ/m56-m58`,** clean, with its Editor binaries rebuilt from its tip, because the heavy gate brief runs on that branch next.

## Rules
- **No Codex.** No `--force`, `clean`, `reset --hard`, `stash`, squash or rebase. Delete nothing outside your own outputs.
- **Never touch the owner's processes. Never kill a process by name.** No machine settings.
- **Never touch:** the `m51` ref, `fix/followup-capture`, `feat/europa-port`, `master`, the unit branches m56, m58 and m59, `D:\IA_Europa`, tags, the dashboard repo.
- Never print a token. Codenames only. Office data never comes home.
- **How your turn ends:** do not end while anything you started is still running, and do not end on a progress update or a question. Target: under 6 hours.

## Final message
- **The very first line is exactly `VERDICT: GREEN`, `VERDICT: NEEDS-DECISION` or `VERDICT: RED`.** Bare line, no code fence. GREEN means commits 1 to 7 are on the branch, the tests pass, the last Editor compile was clean and the branch is pushed.
- Then:
  1. **For the owner (3 lines, plain words):** what is built, what is not proven yet, what he will be shown first.
  2. **Commits** (hash and subject), the branch tip, pushed or local. The CaptureBench branch tip.
  3. **Compile:** each run, memory readings, result, warning count, lever audit.
  4. **Tests;** the "untouched" read-back; the frozen predictions.
  5. **The Q4 keys as built.** Deviations from the plan, each with its reason.
  6. **What the heavy brief must do for m57 and m59 before its cook** (the fixture options to run), and the clip legs after it.
  7. **What the host's binaries match now.** Context used.
  8. **Safety,** or "none". **Decisions chat must make,** or "none".
