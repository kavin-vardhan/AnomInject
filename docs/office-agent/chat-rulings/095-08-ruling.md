# 095-08 — Chat ruling: the m57 plan (LOD spatial trigger) is ACCEPTED, with one change; a small label fix goes first

**Date:** 2026-11-14, about 09:10 IST · **Author:** chat Claude (orchestrator)
**Input:** mailbox report 095-08 (GREEN, 26 min, $20.62 API-equivalent) and the plan `_reviews\095-08-plan-B-lod-spatial-trigger.md`, read in full by chat. Read-only; nothing built or changed.

## Verdict
**The plan is the build specification for m57,** with the answers below. Where this ruling and the plan differ, the ruling wins.

## Answers to Code's questions
- **Q1 mechanism: plugin-timed.** Hold a forced LOD, release on the trigger. The engine has no safe per-object lever in 5.1; `SetBoundsScale` would also change culling, shadows and every label box. The trigger thresholds are still computed with the engine's own LOD function, so a pop lands where an engine with shifted thresholds would pop.
- **Q2 scope: plain static meshes only in v1.** Skeletal, instanced, hierarchical-instanced and Nanite targets are refused by name.
- **Q3 an armed target that never triggers: no event,** its id listed in `run_summary`. Same pattern as vetoed events: ids, never counts.
- **Q4 an event with visibly wrong frames but no spatial swap: KEPT and MARKED, not dropped. This is the one change to the plan.**
  - **Why:** those frames are already written as labelled, and they do show an object at a visibly wrong detail level. If the event left `annotation.json`, a client reading only that file would train on those frames as clean. A hidden true positive is worse than an event the client can filter.
  - **How:** every swap carries its cause: `distance`, `view_entry`, or `hold_cap`. The event carries whether a swap was visible on screen. `run_summary` counts each kind. The readme tells the client how to keep only events with a visible spatial swap.
  - The hold cap is the only pop the schedule can cause. It stays rare by design and is always marked `hold_cap`.
- **Q5 wrong-but-too-small frames: the new flag `lod_subvisible`.** The safe direction; the client's drop rule already covers it.
- **Q6 down-pop on retreat: built and on by default for the clips.** The shipped default is the owner's call after he sees clip C4.
- **Q7 `periodic`: bench gate only.**
- **Q8 the 200 cm proximity gate and the highest-LOD gate: `periodic` only.**
- **Q9 camera path: in the `AnomalyBench` module.** It already holds the camera-clipping schedule and is stripped from every delivery. Chat's brief named the wrong place.
- **Q10 label source: the tick-end prediction with the engine's function, confirmed per frame on the render thread;** a disagreement flags the frame `lod_unconfirmed`.
- **Q11 draw weight: equal in v1.** Measure, then decide.
- **Q12 schema minor: 3,** unless m57 reaches the client in the same drop as m56; then one bump to 2 covers both. Decided at merge time.

## Further rulings
- **G1. The two new `IAnomaly` hooks are accepted** (per-anomaly deferred-onset timeout; "hold the event open"). Both are additive with defaults, so no existing anomaly changes.
- **G2. The fixture gets a second mesh pair** whose coarse LOD switches at a larger on-screen size (the plan's option), so the approach clip looks like a real approach and not a long flagged stretch. It needs a cook, so it is authored **before 095-07's cook** and rides on it. The m57 implementation brief adds the option to the fixture script; 095-07 runs it.
- **G3. m57 implementation may start now, light,** on `feat/m57-lod-spatial-trigger` cut from the integration tip. It does not wait for the m56 and m58 gates. If those gates change m56, m57 merges the change in.
- **G4. Look first stands (B6):** six clips, each beside its no-anomaly twin, before any m57 gate campaign.

## A label defect in delivered code: fixed first, as m59
The plan's reading of the engine found two things in what the client already has:
- **Hierarchical-instanced meshes ignore a forced LOD.** `lod_popping` accepts such a target outside foliage, changes nothing, and labels "popped" frames. Code's 11 Nov review (O9) reported the same for `lod_corruption`, plus no single-LOD guard there. `lod_corruption` is a client anomaly. A positive label with no visible change is exactly what this project must never ship, and the mask veto cannot catch it because the pixels are still drawn.
- **Skeletal targets:** a forced LOD set by the plugin reaches the picture one frame late, so the timer's labels may lead the pixels by a frame. Derived from source, never measured.

**Ruling:**
- **m59 (`fix/m59-forced-lod-refusals`), small, before the gates:** verify both HISM claims and the single-LOD claim in source first. Then `lod_corruption` and `lod_popping` refuse hierarchical-instanced targets, and `lod_corruption` refuses single-LOD targets, each by a named, counted reason. Merged into the integration branch so 095-07 gates it.
- **The skeletal one-frame question is measured, not fixed blind:** 095-07 gets one leg for it if a multi-LOD skeletal target can be put on the bench cheaply; otherwise skeletal targets are refused for both anomalies until it is measured. Code recommends which in the m59 report.

## For the owner and the client (passed on by chat)
1. What does the client's reference LOD clip show: an approach pop, a turn pop, or both? For how many frames is the low detail visible?
2. Do they want LOD pops on characters, or props only?
3. In their captures, does the player mostly walk and turn, or stand and watch?
4. Do they want the down-pop on retreat in the dataset?
5. Office read, numbers only: from one recent M3 session on each host, how many `lod_popping` events it holds. This tells us how much multi-LOD content the host really offers.

## Order of work (light until the owner frees the PC and E:)
1. **095-09 (xhigh, light):** m59.
2. **095-10 (xhigh, light):** m57 implementation, commits 1 to 7 of the plan, offline tests, Editor compile.
3. **095-07 (high, heavy, on the owner's word):** Game builds, the fixture's second mesh pair, one cook, every gate for m56, m58 and m59, the settle-exactness table.
4. m57: look-first clips, the owner's verdict, gates, review, merge.

---

## Addendum, 2026-11-14 about 09:30 IST — ruling on report 095-09 (m59)

**Input:** mailbox report 095-09 (GREEN, 32 min, $20.46 API-equivalent). `fix/m59-forced-lod-refusals` = `4f40f69` (pushed); merged into `integ/m56-m58` = `075221f` (pushed); Editor compile 0 errors / 0 warnings; 18 pure selftests pass.

**What was confirmed in the delivered code (from source; nothing measured yet):**
- Hierarchical-instanced meshes ignore a forced LOD. `lod_popping` could pick one outside foliage and label its popped frames with nothing changed on screen.
- `lod_corruption` had no guard at all: single-LOD and hierarchical-instanced targets were forced and labelled. On `master` it is reachable only by a targeted capture.
- Skeletal targets show a forced LOD one frame late, so both anomalies' edge frames can be one frame off on characters.

**Rulings:**
- **H1. The fix is accepted,** including its size (58 changed lines plus a 27-line pure header for the selftest). The refusals `hism_ignores_forced_lod` and `single_lod` apply to every fire.
- **H2. Skeletal: refused by default until measured, behind a switch.** Both anomalies refuse skeletal targets with reason `skeletal_unmeasured`, unless `IAI.Anomaly.LodAllowSkeletal 1` (ini and console; default 0; echoed in the run banner).
  - **Why refuse:** the predicted error is a label one frame off on both edges of every event, on release defaults, with no flag to excuse it. That fails the project's standard, and it cannot be measured today because the bench has no multi-LOD skeletal mesh.
  - **Why a switch and not a hard refusal:** `lod_corruption` on characters was delivered. The owner can turn it back on for a host without a code change, knowing the caveat. Removing a delivered capability is his call; chat's default is the safe direction and is reversible.
  - **The real fix** (label a skeletal target from the frame the engine applies the LOD, proven on a leg) is queued as its own small unit once a multi-LOD skeletal fixture exists.
- **H3. The decisive HISM and ISM leg (L59-5) is wanted.** The fixture script gets an option for an HISM copy and an ISM copy of the LOD fixture mesh, authored before 095-07's cook, alongside m57's second mesh pair.
- **H4. Telling the client is the owner's decision.** Chat's recommendation: say it, briefly, in the plan note. Earlier datasets can be cleaned for the instanced case from their own files: a `lod_popping` or `lod_corruption` event whose first node's `component_class` is a hierarchical-instanced class has labels that may show no change. The single-LOD case has no field to filter on.
