# 093-11 — Chat ruling: the pairing cause is proven and no frame slip shows anywhere; the 50 % stop comes from an AA-off assumption in the size check; re-state the 50 % proof for TSR, then finish J2

**Date:** 2026-10-15, about 18:15 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's continuation: `report.md` in the J2 run folder. Same thread, one try, 12:27:50–15:29:02 IST, 10,872 s, exit 0, VERDICT NEEDS-DECISION. Evidence is in `E:\IA_BuildCache\_r9311`.
- Claude Code's review: `_reviews\093-11-code-review.md`. 27 OK · 0 GAP · 3 OPEN (work the stop left unrun); observations A–F.
- Mailbox report 093-11: NEEDS-DECISION, 201 min, $15.41 API-equivalent.

## Verdict
**J2 is still not qualified, and nothing is pushed.**
- Remote `fix/followup-capture` stays at `1bf7054`.
- Local head is `3d09293` (docs only); the freeze is `e4228be`.
- Candidate `eff797c` is unchanged: 16/16 files rehash identical before and after.
- Archive: `followup-capture-J2-pairing-09311-eff797c-57F83797-C6D74FED-UNQUALIFIED`.

**Accepted on review:**
- **The Y2 freeze:** committed at 12:37:49, before the first pairing leg (13:53). The reader, tolerance and predicate are unchanged and pinned by hash.
- **The pairing cause is established:**
  - C1 (lighting restored, nothing else changed) reads 52/0/0, and its shifted-mask copy is rejected (4/47/0). Probe coverage rose from 5.1 % to 68.5 %.
  - C2 (arbiter lighting, motion blur off) still reads 0/0/60, so motion blur played no part.
- **The decision-table steps were followed correctly:**
  - the amended recipe was adopted;
  - Lyra native 100 % read 49/0/9 at that recipe, which used attempt 2;
  - the historic recipe then passed on Lyra (50/0/0, control 6/43/0) and was adopted for every pairing leg;
  - the StackOBot pairing legs were re-run at the historic recipe.
- **No frame slip anywhere.** Across 10 pairing sessions and two recipes, PREVIOUS is 0 every time, and every shifted-mask control reads PREVIOUS ≥ 43.
- **Also accepted:**
  - Lyra synthetic UV and proxy alignment PASS;
  - harness fixes within budget;
  - the docs: Y3, Y4, G484, G485, and the office sentence with no `.py` file changed;
  - the restoration, and the no-push.
- **The stop was right.** A frozen size check failed, and changing it was outside 093-11's authority.

## Rulings

### Z1. The 50 % stop: the size check assumed AA off
- **What the check assumed:** a 50 % leg writes a half-size picture. That holds only when nothing upscales the image before the grab, which is the case with AA off.
- **What the historic recipe does:** the hosts' default anti-aliasing is TSR. TSR upscales the half-size render to the full output before the grab, so the picture is 1280×992 by design. That is also exactly what a real game running TSR at 50 % shows its players.
- **The 50 % render did happen.** All 82 mask-pass lines in the stopped leg's own log read `internalViewRect 640x496` inside `viewRect 1280x992` (Code, observation A). The AA-off 50 % legs read `viewRect 640x496`.
- **So this is not a product finding.** Pairing passed on the stopped leg: 56/0/0, control 6/52/0.
- **It does expose a gap worth closing.** TSR at a reduced screen percentage is a common setup on real hosts, and PF-2 box agreement only ever ran at 100 %. Z2 covers it.

### Z2. The 50 % proof, re-stated (frozen before the re-run)
Every 50 % pairing leg at the historic recipe, PIE and staged, must meet all four of these:
1. **The reduced render happened.** Every captured frame's mask-pass log line reports an internal view rect of half the output rect on each axis (here, 640×496 inside 1280×992).
   - Can-fail: the historic 100 % leg's log lines fail this predicate.
2. **One size throughout.** Every picture and every mask file has the written picture's size, and the label rows map to that size.
3. **Box agreement (new, taken from PF-2).** The frozen PF-2 estimator on the leg's labelled rows reads 1.000 ± 0.010.
   - It is applied per axis. An axis gates when at least 20 rows qualify. With fewer, that axis is a reading, and the report says so.
   - Can-fail: the 0.96 mutant on a gating axis rejects.
4. **Pairing unchanged.** PREVIOUS = NEITHER = 0 with the frozen reader and the 40 px tolerance, and the shifted-mask copy rejects.

How it applies:
- The stopped leg stays retained, labelled "size assertion encoded AA-off". It is assessed under Z2 as a **reading only**.
- A **fresh** StackOBot native 50 % leg at the historic recipe is the gate.
- Any AA-off 50 % leg (none is planned now) would keep the original size check.

### Z3. Continue the K7 order at the historic recipe
1. **Pre-flight**, as in 093-11: rehash the candidate; check the restoration state.
2. **Freeze Z2 and commit it**, before any leg.
3. **StackOBot pairing at the historic recipe:** native 50 % (the Z2 gate), synthetic 100 %, synthetic 50 %.
4. **Lyra pairing at the historic recipe:** native 50 %, synthetic 100 %, synthetic 50 %.
5. **Engine controls and probes:** the exact J1 delayed and pre-F1 wrong-view controls, and the supplemental admission probes.
6. **The camera switch** (K6), both fixtures.
7. **The cook.** The prepared `-nodebuginfo` is accepted, on one condition: the staged EXE and DLLs rehash equal to the candidate manifest.
8. **Every staged gate**, including:
   - staged pairing (Z2 for the 50 % legs);
   - the four staged PF-2 cases;
   - the staged resize legs (W2).
9. **The push** (fast-forward, `fix/followup-capture` only) when everything is green.

**Stop rules, carried and added:**
- **Pairing recipe: closed** (2/2 used). Any PREVIOUS > 0 on any pairing leg is a STOP and is never retried.
- **Lyra spill.** Lyra's historic pass had a thin margin: about 4,700 bright-magenta pixels outside the mask pulled its centroid about 29 px, against the 40 px tolerance (Code, observation C). If any pairing leg reads NEITHER > 0 with PREVIOUS = 0, it is a STOP. First take these readings on that leg (no new leg):
  - the bright-magenta pixels outside the current mask on each NEITHER frame: count, extent and centroid;
  - whether that count grows the longer the probe has stayed in one place.
- **Packaging metadata: spent** (2/2). A cook failure for that reason is a STOP.
- **Marker: spent**, as Y5.
- D2 is otherwise unchanged.
- **Not authorised:**
  - a reader, tolerance or predicate change;
  - a fixture content change;
  - any product source change.

### Z4. Code's observations
- **A:** accepted; it is the basis of Z1 and Z2.
- **B:** accepted. Each recipe now fails a different frozen element.
  - Z2 resolves the TSR one.
  - The AA-off one (Lyra spill) is why the historic recipe stays.
- **C:** a reading, covered by Z3's Lyra rule.
- **D:** noted. No link between throughput and PREVIOUS showed up (PIE only, small n).
- **E:** leaving the `UnrealTraceServer` daemons alone was correct. They are harmless, and only the owner closes anything the job doesn't own.
- **F:** the 11:34 reboot is for the owner (Z5).

### Z5. For the owner
- **Restarts:** the PC restarted itself twice today, at 04:29 and 11:34. This continuation will probably run overnight, and another restart would kill it.
- **A quick check:** in Event Viewer, open Windows Logs → System → Filter Current Log and enter the IDs `1074, 41, 6008`.
  - 1074 names whatever asked for the restart (for example, Windows Update).
  - 41 or 6008 means a crash or a power loss.
- If the cause is updates, pausing them for a week is the simplest protection. That is the owner's call; nobody here changes system settings.
- **Still open from K8:** the `AspectRatioAxisConstraint` read on both office hosts.

## Next
- **093-12 is queued:** Codex resumes its J2 thread under Z2–Z3, and Code reviews after Codex exits (W3).
- **After J2 qualifies at home:**
  1. office checkpoint I (the second office host is required);
  2. then `research/natural-causes` and J3, carrying 093-00 r3 §1b (the look-first yardstick).
