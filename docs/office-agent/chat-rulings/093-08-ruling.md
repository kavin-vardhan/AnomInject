# 093-08 — Chat ruling: the admission check is built; Codex stopped early on a metadata gap; fix it, settle the resize/switch accounting, and run the gates

**Date:** 2026-10-15, about 00:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's continuation: `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\report.md`. It resumed the same J2 thread, one try, 4,493 s, and ended with VERDICT NEEDS-DECISION.
- Claude Code's review: `_reviews\093-08-code-review.md`.
  - Part R (the 093-07 projection diff): 8 OK · 1 GAP.
  - Main review: 17 OK · 1 GAP · 3 OPEN.
  - Observations A–E.
- Mailbox report 093-08: NEEDS-DECISION, 84 min, $13.85 API-equivalent.
- Background: the 093-07 mailbox session was filed as CRASHED after the owner restarted the watcher (21:56 IST).

## Verdict
**J2 is still not qualified, and nothing is pushed.**
- Remote `fix/followup-capture` stays at `1bf7054`. Local head `1071ca1`; product `41c1a0c`; archive `…J2-admission-09308-41c1a0c-5F75ECA8-AD40DA99-UNQUALIFIED`.
- **The 093-07 projection fix is accepted on review** (Part R: 8 OK). It has:
  - no FOV/aspect maths left in targeting or labels;
  - `GetProjectionData` at world-tick end;
  - the serving view's unjittered geometry frozen with the request and mapped through the copied rect;
  - a sound near-clip projector;
  - `f9b9328` compiling to a 5.1 fallback with nothing UE 5.7-only;
  - 15 files and no new dependency.
- **The admission check is accepted on review** (M2–M7, M15–M18):
  - the design was committed first;
  - every fire path converges on `ApplyAnomaly` and is covered;
  - the check is frustum-only against the engine projection;
  - the counters are at the `run_summary.json` root;
  - 3 files changed, plus a public engine header;
  - all 8 builds pass.
- **No engine gate ran.** Codex stopped before the first leg on Code's minor finding R8. Its reading of the scope rule was defensible; Code did not order the stop.

## Rulings

### W1. R8: restore `fovDeg` to `GetFOVAngle()`
- **The finding:** `fovDeg` now reads the camera cache instead of `PlayerCameraManager->GetFOVAngle()` (`51cb0d6`). On a host that locks the FOV, the label would report the unlocked FOV, not the rendered one. That changes the key's meaning, which K3 forbids.
- **The fix is authorised:** source `fovDeg` from `GetFOVAngle()` again, in `AnomalyViewport.cpp`. That file is already inside the projection fix's scope. Boxes and visibility stay on the engine projection; only the metadata source changes.
- **New gate PF-5, locked FOV:** one StackOBot PIE leg with the FOV locked through the player controller (for example the `fov 70` console command).
  - The labels' `fovDeg` reads the locked value.
  - PF-2 box agreement holds on that leg.
  - The admission counters behave normally.
  - Can-fail: the `f9b9328` metadata route would report the unlocked value. Show it on a CPU or source-level check, or run it, whichever is cheaper. Never invent the number.

### W2. M8: drop accounting for resize and camera-switch legs
- **L5's floor `w = r` applies to every leg except resize and the camera-switch legs.** In those legs frames are dropped by design at the generation change (093-06 observation A; K6: drops per switch are a reading).
- **The accounting for those legs instead:**
  - every missing frame is accounted for by a named, counted drop reason (`r = w + counted drops`);
  - zero stale or mismatched frames are accepted;
  - all other floors apply: edges 0 where events exist, every labelled frame with a measured mask, and PF-2 agreement on the written frames.
- **Codex's later edit in `1071ca1`** made `w = r` binding for those legs. It is reverted for those legs, and the 093-06 resize accounting (`r = w + d`, stale excluded) is restored.

### W3. Process: Code's mid-job findings do not steer Codex
**Code, during a running Codex job:**
- Writes in-progress findings only to `code-receipts-<brief>\` in the run folder.
- Publishes `_reviews\<brief>-code-review.md` only after Codex exits.
- If a finding must change Codex's course, it stops the job by PID (only the relay-started process) and reports. It never steers through files.

**Every implementer request now carries this line:** "Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions."

### W4. Commit dates (Code's observation B)
- **What happened:** the three 093-08 commits are dated 2001-01-01, because the mailbox session's process environment set `GIT_AUTHOR_DATE` and `GIT_COMMITTER_DATE`.
- **Code's launch:** clear both variables in the environment passed to the relay. Report whether its own environment still has them.
- **Codex, first step:** re-create `7f4329c`, `41c1a0c` and `1071ca1` with correct current dates. Keep the same trees, messages and author identity. Record the old → new hashes in the journal.
  - All three are unpushed; nothing is deleted.
  - The UNQUALIFIED archive keeps its historical hash.

### W5. Code's observations C–E
- **C. No unique player view means a refused object fire.** Console `IAI.Apply`, the selector and the dashboard now refuse object anomalies when there is no unique player view, for example in editor Simulate mode or split-screen. Accepted. It matches the locked rule "no view ⇒ offer nothing" and the client's rule that every injection happens on screen. Record it in the readme's dev notes: object anomalies need Play (PIE or a game build), not Simulate.
- **D. An actor with no renderable bounds** is counted as `targeted_refused_off_screen`. Accepted as documented.
- **E. The PF-1 checker counts fires only through `texcorrupt_fires_applied`.** Also count applied fires through the generic apply receipt or log line, so the admission proof doesn't depend on one anomaly type.

### W6. Order: unchanged from K7 and L6, starting with W4 and W1
1. Re-create the three commits (W4).
2. The `fovDeg` fix and its commit (W1). Re-freeze only the predictions W1 and W2 touch, before any leg.
3. Build.
4. PIE: the PF-1 re-run, PF-2 to PF-5, the J2 matrix on both fixtures, then the camera switch.
5. The cook.
6. Every staged gate.
7. The push, only when everything is green.

The harness items still open from L4 run in this job: the 0.813 replay, and the healthy reader PASS from a new session.

## Next
- **093-09 is queued:** Codex resumes its J2 thread, and Code reviews after Codex exits (W3).
- **Unchanged:**
  - after J2 qualifies at home, office checkpoint I, then `research/natural-causes` and J3;
  - still open from K8: the owner's `AspectRatioAxisConstraint` read on both office hosts.
